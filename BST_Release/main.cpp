#include "main.hpp"

nsGUIText::FontRaster *Font1, *Font2;

nsBasic::ObjGeneral *Screen;

nsGUI::Button *BtnScene, *BtnSrc, *BtnPlay, *BtnRecord, *BtnRecordOffline, *BtnFrameTest;
nsGUI::CheckBox *ChkShowIR, *ChkOutputEC;
nsGUI::Rect ScreenRect;
UBINT ScreenStat, LastMouseX, LastMouseY;

nsBasic::ObjGeneral *Scene_3D, *Scene_2D, *MyEventHdlr;
nsBasic::ObjSet *My3DScene, *MyCtrlScene;

nsAudio::OutputStream *MySndOutputStream;

OPENFILENAME DialogTemplate;

UBCHAR FilePath[260] = L"";

nsMath::TinyVector<float, 3> InitialRayPos[PathCnt];
float *HitEnergyByBounce;

volatile UBINT ECFrameCnt = 0;
UBINT ECFrameCnt_Max = 0;

//Synchronization
volatile UBINT RecordFlag = 0, ExeStopFlag = 0, OutputEC = 0;
nsBasic::CyclicBarrier RTBarrier1, RTBarrier2;

bool Playing = false, ShowIR = false;

template <typename T> class DoubleBuffer{
private:
	T Resource1;
	T Resource2;

	T *ReadQueue[2];
	T *WriteQueue[2];
	UBINT ReaderCount[2];

	nsBasic::Mutex SyncMutex;
public:
	template <typename ... Args> DoubleBuffer(Args ... args) :Resource1(args...), Resource2(args...){
		this->ReaderCount[0] = 0;
		this->ReaderCount[1] = 0;
		this->ReadQueue[0] = nullptr;
		this->ReadQueue[1] = nullptr;
		this->WriteQueue[0] = &Resource1;
		this->WriteQueue[1] = &Resource2;
		this->SyncMutex.Initialize();
	}
	T *RequestRead(){
		T *RetValue = nullptr;
		this->SyncMutex.Enter();

		if (nullptr != this->ReadQueue[1]){
			if (0 == this->ReaderCount[0]){
				this->ReaderCount[0]++;
				RetValue = this->ReadQueue[0];
			}
			else{
				this->ReaderCount[1]++;
				RetValue = this->ReadQueue[1];
				if (nullptr == this->ReadQueue[0]){
					this->ReadQueue[0] = this->ReadQueue[1];
					this->ReadQueue[1] = nullptr;
					this->ReaderCount[0] = this->ReaderCount[1];
					this->ReaderCount[1] = 0;
				}
			}
		}
		else if (nullptr != this->ReadQueue[0]){
			this->ReaderCount[0]++;
			RetValue = this->ReadQueue[0];
		}
		this->SyncMutex.Leave();
		return RetValue;
	}
	void SubmitRead(T * const lpRes){
		if (nullptr != lpRes){
			this->SyncMutex.Enter();
			if (lpRes == this->ReadQueue[1]){
				if (this->ReaderCount[1] > 0)this->ReaderCount[1]--;
			}
			else if (lpRes == this->ReadQueue[0]){
				if (this->ReaderCount[0] > 0)this->ReaderCount[0]--;
				if (0 == this->ReaderCount[0] && nullptr != this->ReadQueue[1]){
					this->WriteQueue[0] = this->ReadQueue[0];
					this->ReadQueue[0] = this->ReadQueue[1];
					this->ReadQueue[1] = nullptr;

					this->ReaderCount[0] = this->ReaderCount[1];
					this->ReaderCount[1] = 0;
				}
			}
			this->SyncMutex.Leave();
		}
	}
	T *RequestWrite(){
		T *RetValue = nullptr;
		this->SyncMutex.Enter();
		if (nullptr != this->WriteQueue[0])RetValue = this->WriteQueue[0];
		else if (nullptr != this->WriteQueue[1]){
			this->WriteQueue[0] = this->WriteQueue[1];
			this->WriteQueue[1] = nullptr;
			RetValue = this->WriteQueue[0];
		}
		this->SyncMutex.Leave();
		return RetValue;
	}
	void SubmitWrite(T * const lpRes){
		if (nullptr != lpRes){
			this->SyncMutex.Enter();
			if (lpRes == this->WriteQueue[0]){
				if (nullptr != this->ReadQueue[1]){
					this->ReadQueue[0] = this->ReadQueue[1];
					this->ReaderCount[0] = this->ReaderCount[1];
				}
				this->ReadQueue[1] = this->WriteQueue[0];
				this->WriteQueue[0] = nullptr;
			}
			this->SyncMutex.Leave();
		}
	}
};

DoubleBuffer<Listener> *lpListenerPair = nullptr;

struct HDRFilterStat{
	static const UBINT RMSWndLen = 0x200; // the window length must be the power of 2.

	float RMSWnd[RMSWndLen];
	float RMSValue;
	UBINT RMSWndPtr;

	float Value_Compressor_Stage1;
	float Value_Compressor_Stage2;
	float Value_Compressor_Stage3;
	float Value_Limiter;

	void Initialize(){
		nsMath::setzero_simd_unaligned(RMSWnd, RMSWndLen);
		this->RMSValue = 0.0f;
		this->RMSWndPtr = 0;
		this->Value_Compressor_Stage1 = 0.0f;
		this->Value_Compressor_Stage2 = 0.0f;
		this->Value_Compressor_Stage3 = 0.0f;
		this->Value_Limiter = 0.0f;
	}
	float GetFinalGain(float NextSampleValue){
		this->RMSValue = std::max(this->RMSValue + NextSampleValue * NextSampleValue - this->RMSWnd[this->RMSWndPtr], 0.0f);
		this->RMSWnd[this->RMSWndPtr] = NextSampleValue * NextSampleValue;
		this->RMSWndPtr = (this->RMSWndPtr + 1) & (RMSWndLen - 1);

		float InputLevel = 5.0f * log10(this->RMSValue / (float)RMSWndLen);
		float OutputLevel;

		// compressor 1
		// Threshold -6dB, Ratio 3:1, No Knee, Attack 0.005s / 10dB, Release 0.02s / 10dB
		if (InputLevel > this->Value_Compressor_Stage1)this->Value_Compressor_Stage1 = std::min(InputLevel, this->Value_Compressor_Stage1 + (10.0f / 0.005f) / (float)AudioSampleRate);
		else this->Value_Compressor_Stage1 = std::max(-60.0f, std::max(InputLevel, this->Value_Compressor_Stage1 - (10.0f / 0.02f) / (float)AudioSampleRate));
		OutputLevel = InputLevel;
		if (this->Value_Compressor_Stage1 > -6.0f)OutputLevel += ((this->Value_Compressor_Stage1 + 6.0f) / 3.0f - 6.0f) - this->Value_Compressor_Stage1;

		// compressor 2
		// Threshold -6dB, Ratio 4:1, Linear Knee Radius 6dB, Attack 0.2s / 10dB, release 0.2s / 10dB
		if (OutputLevel > this->Value_Compressor_Stage2)this->Value_Compressor_Stage2 = std::min(OutputLevel, this->Value_Compressor_Stage2 + (10.0f / 0.2f) / (float)AudioSampleRate);
		else this->Value_Compressor_Stage2 = std::max(-60.0f, std::max(OutputLevel, this->Value_Compressor_Stage2 - (10.0f / 0.2f) / (float)AudioSampleRate));
		if (this->Value_Compressor_Stage2 > 0.0f)OutputLevel += (this->Value_Compressor_Stage2 / 4.0f - 4.5f) - this->Value_Compressor_Stage2;
		else if (this->Value_Compressor_Stage2 > -12.0f)OutputLevel += ((this->Value_Compressor_Stage2 + 12.0f) / 1.6f - 12.0f) - this->Value_Compressor_Stage2;

		// compressor 3
		// Threshold -3dB, Ratio 6:1, Linear Knee Radius 2dB, Attack 0.002s / 10dB, release 0.1s / 10dB
		if (OutputLevel > this->Value_Compressor_Stage3)this->Value_Compressor_Stage3 = std::min(OutputLevel, this->Value_Compressor_Stage3 + (10.0f / 0.002f) / (float)AudioSampleRate);
		else this->Value_Compressor_Stage3 = std::max(-60.0f, std::max(OutputLevel, this->Value_Compressor_Stage3 - (10.0f / 0.1f) / (float)AudioSampleRate));
		if (this->Value_Compressor_Stage3 > -1.0f)OutputLevel += ((this->Value_Compressor_Stage3 + 3.0f) / 6.0f - 3.0f) - this->Value_Compressor_Stage3;
		else if (this->Value_Compressor_Stage3 > -5.0f)OutputLevel += ((this->Value_Compressor_Stage3 + 5.0f) * (7.0f / 12.0f) - 5.0f) - this->Value_Compressor_Stage3;

		if (isinf(InputLevel))OutputLevel = 0; else OutputLevel = OutputLevel - InputLevel;
		InputLevel = 10.0f * log10(abs(NextSampleValue));
		OutputLevel += InputLevel;

		// limiter
		if (OutputLevel > this->Value_Limiter)this->Value_Limiter = OutputLevel;
		else this->Value_Limiter = std::max(-60.0f, std::max(OutputLevel, this->Value_Limiter - 0.02f / (float)AudioSampleRate));
		if (this->Value_Limiter > 0.0f)OutputLevel -= this->Value_Limiter;
		OutputLevel -= 0.1f;

		if (isinf(InputLevel)) return 0.0f; else return exp(0.1f * (OutputLevel - InputLevel) * (float)M_LN10);
	}
};

class WaveFileWriter{
public:
	nsFile::WriteFileStream WavFileStream;
	UBINT RecordedLength;

	void OpenFile(UBCHAR *lpFileName){
		if ((UBINT)nullptr != WavFileStream.FileHandle){
			WavFileStream.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
			nsFormat::Serialize_WAV_Info(&WavFileStream, 2, 16, 48000, RecordedLength);
			WavFileStream.Close();
		}
		WavFileStream.Open(lpFileName, true);
		nsFormat::Serialize_WAV_Info(&WavFileStream, 2, 16, 48000, 0);
		RecordedLength = 0;
	}
	void Write(short *const Src, UBINT SamplesToWrite){
		if ((UBINT)nullptr != WavFileStream.FileHandle){
			WavFileStream.WriteBulk(Src, 2 * SamplesToWrite*sizeof(short)); //stereo sound
			RecordedLength += SamplesToWrite;
		}
	}
	void CloseFile(){
		WavFileStream.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
		nsFormat::Serialize_WAV_Info(&WavFileStream, 2, 16, 48000, RecordedLength);
		WavFileStream.Close();
		RecordedLength = 0;
	}
};

//Offline recording
bool OfflineRecState = false;
UBINT OfflineRecCounter = 0;
WaveFileWriter OfflineWavWriter;
HDRFilterStat Offline_HDRFilterStat_L, Offline_HDRFilterStat_R;

//Timer
UINT8b LastTimeTick = 0;

#include "GUI_ClusterCtrl.cpp"

ClusterCtrl *SndClusterCtrl;

void *SoundRayTraceThread(void *lpArg){
#if defined PERFORMANCE_TEST
	UINT8b Time_Start, Time_End, Time_Allocate_Total = 0, Time_Trace_Total = 0, Time_Connect_Total = 0, Time_2Cache_Total = 0, Time_2IR_Total = 0;
#endif

	DoubleBuffer<Listener> MyRendererPair;
	Listener *lpCurListener;

	atomic_xchg((UBINT *)&lpListenerPair, (UBINT)&MyRendererPair);

	const UBINT NeighborCnt = 64;
	const UBINT RandBufferSize = std::max(PathCnt, ConnectionCount);
	float *DestPos = MyCameraData.Position;

	nsMath::RandGenerator_XORShiftPlus MyRand;

	PathTracer MyPathTracer((float)IRLength / (float)AudioSampleRate, IterateTimes);

	while (true){
		RTBarrier1.Wait();
		if (ExeStopFlag)break;

		lpCurListener = MyRendererPair.RequestWrite();
		if (nullptr != lpCurListener){
			if (!MySoundClusterMgr->SoundClusterList_Listener.empty() && !MySoundClusterMgr->SoundClusterList_Listener.empty()){
				MySoundClusterMgr->SoundClusterList_Listener.front().lpListener = lpCurListener;
				lpCurListener->SetRenderParams(MyAudioRenderer, MySoundOutputChannel);

#if !defined USE_PATH_CACHE
				for (auto _It = MySoundClusterMgr->SoundClusterList_Source.begin(); _It != MySoundClusterMgr->SoundClusterList_Source.end(); ++_It)_It->Change_Exist = true;
				for (auto _It = MySoundClusterMgr->SoundClusterList_Listener.begin(); _It != MySoundClusterMgr->SoundClusterList_Listener.end(); ++_It)_It->Change_Exist = true;
#endif

#if defined PERFORMANCE_TEST
				nsBasic::GetAccurateTimeCntr(&Time_Start);
#endif

				MyPathTracer.PathAllocate(MySoundClusterMgr, PathCnt, ConnectionCount);

#if defined PERFORMANCE_TEST
				nsBasic::GetAccurateTimeCntr(&Time_End);
				Time_Allocate_Total += Time_End - Time_Start;
				Time_Start = Time_End;
#endif
				MyPathTracer.LoadSoundClusters(MySoundClusterMgr);

#if defined PERFORMANCE_TEST
				nsBasic::GetAccurateTimeCntr(&Time_End);
				Time_Allocate_Total += Time_End - Time_Start;
				Time_Start = Time_End;
#endif
				MyPathTracer.Trace();

#if defined PERFORMANCE_TEST
				nsBasic::GetAccurateTimeCntr(&Time_End);
				Time_Trace_Total += Time_End - Time_Start;
				Time_Start = Time_End;
#endif

				MyPathTracer.Connect();

#if defined PERFORMANCE_TEST
				nsBasic::GetAccurateTimeCntr(&Time_End);
				Time_Connect_Total += Time_End - Time_Start;
				Time_Start = Time_End;
#endif
				MyPathTracer.Output(MySoundClusterMgr);

#if defined PERFORMANCE_TEST
				nsBasic::GetAccurateTimeCntr(&Time_End);
				Time_2Cache_Total += Time_End - Time_Start;
				Time_Start = Time_End;
#endif

				MyPathTracer.PostProcess(MySoundClusterMgr);

#if defined PERFORMANCE_TEST
				nsBasic::GetAccurateTimeCntr(&Time_End);
				Time_2IR_Total += Time_End - Time_Start;
				printf("SecPerFrame:%lf\nAlloc(%lf)->T(%lf)->C(%lf)->Cache(%lf)->IR(%lf) SamplePerFrame:%lf\n",
					(double)(Time_Allocate_Total + Time_Trace_Total + Time_Connect_Total + Time_2Cache_Total) / ((double)nsEnv::TimeTickPerSec * (double)MyPathTracer.CurrentStatInfo.FrameCounter),
					(double)Time_Allocate_Total / ((double)nsEnv::TimeTickPerSec * (double)MyPathTracer.CurrentStatInfo.FrameCounter),
					(double)Time_Trace_Total / ((double)nsEnv::TimeTickPerSec * (double)MyPathTracer.CurrentStatInfo.FrameCounter),
					(double)Time_Connect_Total / ((double)nsEnv::TimeTickPerSec * (double)MyPathTracer.CurrentStatInfo.FrameCounter),
					(double)Time_2Cache_Total / ((double)nsEnv::TimeTickPerSec * (double)MyPathTracer.CurrentStatInfo.FrameCounter),
					(double)Time_2IR_Total / ((double)nsEnv::TimeTickPerSec * (double)MyPathTracer.CurrentStatInfo.FrameCounter),
					(double)MyPathTracer.CurrentStatInfo.SampleCounter_Total / (double)MyPathTracer.CurrentStatInfo.FrameCounter);
#endif

				if (OutputEC > 0){
					auto _It = MySoundClusterMgr->SoundClusterList_Source.begin();
					UINT4b i = 0;

					UBCHAR TmpFolderPath_EC[260], TmpPath_EC[260];
					for (auto _It = MySoundClusterMgr->SoundClusterList_Source.begin(); _It != MySoundClusterMgr->SoundClusterList_Source.end(); ++_It){
						IRTrack *SelectedTrack = lpCurListener->GetTrack(i);
						if (nullptr != SelectedTrack){
							nsText::String_Sys ClusterName_Sys(_It->ClusterName);
							swprintf(TmpFolderPath_EC, 260, L"s_%s", ClusterName_Sys.cbegin());
							nsFile::CreateFolder(TmpFolderPath_EC);

							swprintf(TmpPath_EC, 260, L"s_%s/EnergyCurve_BDPT_%04d", ClusterName_Sys.cbegin(), (UINT4b)ECFrameCnt);
							nsFile::WriteFileStream TmpStream2(TmpPath_EC, true);

							float *EnergyCurve = (float *)nsBasic::MemAlloc_Arr<float>(IRLength * FloatVec_SIMD::Width);
							float *EnergyCurve_Binned = (float *)nsBasic::MemAlloc_Arr<float>(IRLength * FloatVec_SIMD::Width);
							SelectedTrack->GetEnergyCurve_Multiband(EnergyCurve);
							memset(EnergyCurve_Binned, 0, IRLength * sizeof(FloatVec_SIMD));

							//Bin by 3ms
							for (UBINT i = 0; i < IRLength; i++){
								for (UBINT j = 0; j < FloatVec_SIMD::Width; j++){
									EnergyCurve_Binned[(i * 1000 / (AudioSampleRate * 3)) * FloatVec_SIMD::Width + j] += 10.0f * EnergyCurve[i * FloatVec_SIMD::Width + j]; // The multiplier 10.0f comes from ISO 3382-1-2009.
								}
							}

							TmpStream2.WriteBulk((const void *)EnergyCurve_Binned, ((IRLength * 1000) / (AudioSampleRate * 3) + 1) * sizeof(FloatVec_SIMD));
							nsBasic::MemFree_Arr<float>(EnergyCurve_Binned, IRLength * FloatVec_SIMD::Width);
							nsBasic::MemFree_Arr<float>(EnergyCurve, IRLength * FloatVec_SIMD::Width);
						}
						else if (MySoundClusterMgr->SoundClusterList_Source.size() > i)throw std::exception();
						i++;
					}

					ECFrameCnt++;
				}
			}
			MyRendererPair.SubmitWrite(lpCurListener);
		}
		RTBarrier2.Wait();
	}

	atomic_xchg((UBINT *)&lpListenerPair, (UBINT)nullptr);

	return nullptr;
}
void *SoundRenderThread(void *lpArg){
	float Result_L[SndWndLen], Result_R[SndWndLen];
	float LerpFactor[SndWndLen];
	for (UBINT i = 0; i < SndWndLen; i++)LerpFactor[i] = (float)i / (float)SndWndLen;
	short *SndBuffer;
	UBINT NewBufAddr = (UBINT)nullptr;
	nsFile::WriteFileStream RecordStream;
	UBINT RecordLen = 0;

	// HDR filter
	
	HDRFilterStat HDRFilterStat_L, HDRFilterStat_R;

	HDRFilterStat_L.Initialize();
	HDRFilterStat_R.Initialize();

	nsBasic::CreateThreadExtObj();

	Listener *lpPrevListener = nullptr;
	while (nullptr != (SndBuffer = (short *)MySndOutputStream->WaitFrame())){
		Listener *lpCurListener;

		nsMath::setzero_simd_unaligned(Result_L, SndWndLen);
		nsMath::setzero_simd_unaligned(Result_R, SndWndLen);

		if (nullptr != lpListenerPair && nullptr != (lpCurListener = lpListenerPair->RequestRead())){

#if defined PERFORMANCE_TEST
			UINT8b Time_Begin, Time_End;
			nsBasic::GetAccurateTimeCntr(&Time_Begin);
			printf("Compile");
#endif

			if (lpCurListener != lpPrevListener){
				lpCurListener->Compile();
				lpPrevListener = lpCurListener;
			}

#if defined PERFORMANCE_TEST
			nsBasic::GetAccurateTimeCntr(&Time_End);
			printf("(%lf)->", (double)(Time_End - Time_Begin) / (double)nsEnv::TimeTickPerSec);
			Time_Begin = Time_End;
#endif

			MyAudioRenderer->ReadyForInput();
			MyAudioRenderer->LoadInput();
#if defined PERFORMANCE_TEST
			nsBasic::GetAccurateTimeCntr(&Time_End);
			printf("Input(%lf)->", (double)(Time_End - Time_Begin) / (double)nsEnv::TimeTickPerSec);
#endif
			lpCurListener->Render(Result_L, Result_R);
			lpListenerPair->SubmitRead(lpCurListener);

#if defined PERFORMANCE_TEST
			nsBasic::GetAccurateTimeCntr(&Time_End);
			printf("Render(%lf)\n", (double)(Time_End - Time_Begin) / (double)nsEnv::TimeTickPerSec);
			Time_Begin = Time_End;
#endif
		}

		// HDR filtering and output
		for (UBINT i = 0; i < SndWndLen; i++){
			float FinalGain = std::min(HDRFilterStat_L.GetFinalGain(Result_L[i]), HDRFilterStat_R.GetFinalGain(Result_R[i]));
			SndBuffer[2 * i] = (short)(FinalGain * Result_L[i] * 0x7FFF);
			SndBuffer[2 * i + 1] = (short)(FinalGain * Result_R[i] * 0x7FFF);
		}

		if ((UBINT)-1 == RecordStream.FileHandle){
			if (RecordFlag){
				RecordStream.Open(L"Record.wav", true);
				nsFormat::Serialize_WAV_Info(&RecordStream, 2, 16, AudioSampleRate, 0);
				RecordLen = 0;
				atomic_xchg(&RecordFlag, 0);
			}
		}
		else{
			RecordStream.WriteBulk(SndBuffer, 2 * SndWndLen*sizeof(short));
			RecordLen += SndWndLen;
			if (RecordFlag){
				RecordStream.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
				nsFormat::Serialize_WAV_Info(&RecordStream, 2, 16, AudioSampleRate, RecordLen);
				RecordStream.Close();
				atomic_xchg(&RecordFlag, 0);
			}
		}
		MySndOutputStream->SubmitFrame();
	}

	if ((UBINT)nullptr != RecordStream.FileHandle){
		RecordStream.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
		nsFormat::Serialize_WAV_Info(&RecordStream, 2, 16, AudioSampleRate, RecordLen);
		RecordStream.Close();
		atomic_xchg(&RecordFlag, 0);
	}

	nsBasic::DestroyThreadExtObj();
	return nullptr;
}

extern UBINT _cdecl MsgProc_Scene_3D(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...){
	va_list args;
	va_start(args, Msg);

	if (MSG_SPAINT_DWORD == Msg || MSG_SPAINT_DWORDEX == Msg){
		nsGUI::Rect *tmpRect;
		nsGUI::GLDevice *MyDevice = va_arg(args, nsGUI::GLDevice *);
		tmpRect = va_arg(args, nsGUI::Rect *);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else if (MSG_PAINT == Msg){
		nsGUI::Rect *tmpRect;
		nsGUI::GLDevice *MyDevice = va_arg(args, nsGUI::GLDevice *);
		tmpRect = va_arg(args, nsGUI::Rect *);

		if (false == OfflineRecState)nsMath::Transform4_Pos_RotEuler(MyCameraData.Position, MyCameraData.EulerAngle, CamTransform.Matrix);

		float ZoomFactor = 0.5f*sqrt(2.0f / ((ScreenRect.XMax - ScreenRect.XMin) * (ScreenRect.YMax - ScreenRect.YMin)));
		MyCamera.Size[0] = ScreenRect.XMax - ScreenRect.XMin;
		MyCamera.Size[1] = ScreenRect.YMax - ScreenRect.YMin;
		MyCamera.Frustum[0] = (BINT)(ScreenRect.XMin - ScreenRect.XMax)*ZoomFactor;
		MyCamera.Frustum[1] = (BINT)(ScreenRect.YMin - ScreenRect.YMax)*ZoomFactor;
		MyCamera.Frustum[2] = -MyCamera.Frustum[0];
		MyCamera.Frustum[3] = -MyCamera.Frustum[1];
		MyCamera.Frustum[4] = 0.1f;
		MyCamera.Frustum[5] = nsMath::NumericTrait<float>::Inf_Positive;

		RTBarrier1.Wait();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
			
		nsScene::RenderOnce(RenderManager, &SceneManager);

		float WorldMat[16], ProjMat[16];
		nsMath::Transform4_Perspective_Inf_Frustum(MyCamera.Frustum, ProjMat);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(ProjMat);
		glMatrixMode(GL_MODELVIEW);
		nsMath::inv4(WorldMat, CamTransform.Matrix);
		glLoadMatrixf(WorldMat);

		glClearDepth(1.0);
		glClear(GL_DEPTH);

		glPointSize(4.0f);
		glColor3f(1.0f, 1.0f, 0.0f);
		glBegin(GL_POINTS);
		{
			for (auto _It = AnimationManager->AnimationMap.begin(); _It != AnimationManager->AnimationMap.end(); ++_It){
				for (size_t i = 0; i < _It->second.Curve.Keys.size(); i++){
					glVertex3fv(_It->second.Curve.Keys[i].Position);
				}
			}
		}
		glEnd();
		glLineWidth(2.0f);
		glColor3f(0.5f, 0.5f, 0.0f);
		for (auto _It = AnimationManager->AnimationMap.begin(); _It != AnimationManager->AnimationMap.end(); ++_It){
			glBegin(GL_LINE_STRIP);
			{
				for (size_t i = 0; i + 1 < _It->second.Curve.Keys.size(); i++){
					glVertex3fv(_It->second.Curve.Keys[i].Position);
					glVertex3fv(_It->second.Curve.Keys[i + 1].Position);
				}
			}
			glEnd();
		}

		glDisable(GL_DEPTH_TEST);

		RTBarrier2.Wait();
	}
	va_end(args);
	return 0;
}

extern UBINT _cdecl MsgProc_Scene_2D(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...){
	va_list args;
	va_start(args, Msg);
	if (MSG_PAINT == Msg){
		nsGUI::GLDevice *MyDevice = va_arg(args, nsGUI::GLDevice *);
		
		glViewport(ScreenRect.XMin, ScreenRect.YMin, ScreenRect.XMax - ScreenRect.XMin, ScreenRect.YMax - ScreenRect.YMin);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(ScreenRect.XMin, ScreenRect.XMax, ScreenRect.YMax, ScreenRect.YMin, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if (OfflineRecState){
			OfflineRecCounter++;

			short ProcessedWavData[2 * SndWndLen];
			float Result_L[SndWndLen], Result_R[SndWndLen];

			Listener *lpCurListener = lpListenerPair->RequestRead();

			lpCurListener->Compile();
			MyAudioRenderer->ReadyForInput();
			MyAudioRenderer->LoadInput();
			if (false == lpCurListener->Render(Result_L, Result_R)){
				nsMath::setzero_simd_unaligned(Result_L, SndWndLen);
				nsMath::setzero_simd_unaligned(Result_R, SndWndLen);
			}
			lpListenerPair->SubmitRead(lpCurListener);

			// HDR filtering
			for (UBINT i = 0; i < SndWndLen; i++){
				float FinalGain = std::min(Offline_HDRFilterStat_L.GetFinalGain(Result_L[i]), Offline_HDRFilterStat_R.GetFinalGain(Result_R[i]));
				ProcessedWavData[2 * i] = (short)(FinalGain * Result_L[i] * 0x7FFF);
				ProcessedWavData[2 * i + 1] = (short)(FinalGain * Result_R[i] * 0x7FFF);
			}

			OfflineWavWriter.Write(ProcessedWavData, SndWndLen);

			lpListenerPair->SubmitRead(lpCurListener);

			bool Stopped = false;
			for (auto _It = AnimationManager->AnimationMap.begin(); _It != AnimationManager->AnimationMap.end(); ++_It){
				Stopped = Stopped || _It->second.Advance((float)SndWndLen / (float)AudioSampleRate);
				_It->second.Evaluate();
			}
			if (Stopped){
				OfflineWavWriter.CloseFile();
				OfflineRecState = false;
			}
		}

		UINT8b CurTimeTick;
		double FPS;
		nsBasic::GetAccurateTimeCntr(&CurTimeTick);
		if (0 == LastTimeTick)FPS = 0; else FPS = (double)nsEnv::TimeTickPerSec / (double)(CurTimeTick - LastTimeTick);
		LastTimeTick = CurTimeTick;

		glColor3f(1.0f, 1.0f, 1.0f);

		UBCHAR TmpStr[260];
		ScreenRect.XMin += 3;
		ScreenRect.YMax -= 3;
		swprintf(TmpStr, 260, L"FPS:%lf, Camera Position:(%f, %f, %f)", FPS, MyCameraData.Position[0], MyCameraData.Position[1], MyCameraData.Position[2]);
		nsGUI::GLDrawText(Font1, &ScreenRect, (char *)TmpStr, nsCharCoding::CHARCODING_SYS, nsGUI::TEXTLAYOUT_HORI_LEFT | nsGUI::TEXTLAYOUT_VERT_BOTTOM);
		ScreenRect.XMin -= 3;
		ScreenRect.YMax += 3;

		if (ShowIR && (UBINT)-1 != SndClusterCtrl->ClusterIndex_Selected){
			Listener *lpCurListener = lpListenerPair->RequestRead();
			IRTrack *SelectedTrack = lpCurListener->GetTrack(SndClusterCtrl->ClusterIndex_Selected);
			if (nullptr != SelectedTrack){
				float *IR_L = (float *)nsBasic::MemAlloc(IRLength*sizeof(float));
				float *IR_R = (float *)nsBasic::MemAlloc(IRLength*sizeof(float));
				float IR_Min, IR_Max;
				UBINT CtrlHeight = (ScreenRect.YMax - ScreenRect.YMin - 24.0) / 2, LinePos, Remainder, IRSamples;

				SelectedTrack->GetIR(IR_L, IR_R);

				glColor3f(0.0f, 1.0f, 0.0f);
				glBegin(GL_LINES);
				LinePos = 5; Remainder = 0; IR_Min = 0.0f, IR_Max = 0.0f;
				for (UBINT i = 0; i < IRLength; i++){
					Remainder += ScreenRect.XMax - ScreenRect.XMin - 10;
					if (IR_L[i]>IR_Max)IR_Max = IR_L[i]; if (IR_L[i]<IR_Min)IR_Min = IR_L[i];
					if (Remainder > IRLength){
						IR_Min *= 30.0f, IR_Max *= 30.0f;
						glVertex2f(LinePos, ScreenRect.YMax - 17.0f - 0.5 * (float)CtrlHeight * (1.0f + IR_Min));
						glVertex2f(LinePos, ScreenRect.YMax - 18.0f - 0.5 * (float)CtrlHeight * (1.0f + IR_Max));
						Remainder -= IRLength;
						IR_Min = 0.0f, IR_Max = 0.0f;
						LinePos++;
					}
				}
				LinePos = 5; Remainder = 0; IR_Min = 0.0f, IR_Max = 0.0f;
				for (UBINT i = 0; i < IRLength; i++){
					Remainder += ScreenRect.XMax - ScreenRect.XMin - 10;
					if (IR_R[i]>IR_Max)IR_Max = IR_R[i]; if (IR_R[i]<IR_Min)IR_Min = IR_R[i];
					if (Remainder > IRLength){
						IR_Min *= 30.0f, IR_Max *= 30.0f;
						glVertex2f(LinePos, ScreenRect.YMax - 19.0f - 0.5 * (float)CtrlHeight * (3.0f + IR_Min));
						glVertex2f(LinePos, ScreenRect.YMax - 20.0f - 0.5 * (float)CtrlHeight * (3.0f + IR_Max));
						Remainder -= IRLength;
						IR_Min = 0.0f, IR_Max = 0.0f;
						LinePos++;
					}
				}
				glEnd();

				glBegin(GL_LINES);
				glColor3f(1.0f, 0.0f, 0.0f);
				for (UBINT i = 0; i < IterateTimes - 1; i++){
					glVertex2f((float)(ScreenRect.XMax * (2 * i + 1)) / (float)(2 * (IterateTimes - 1)), 0.0f);
					glVertex2f((float)(ScreenRect.XMax * (2 * i + 1)) / (float)(2 * (IterateTimes - 1)), (float)ScreenRect.YMax * MySoundClusterMgr->SoundClusterList_Listener.begin()->lpRelation_Row_Next->SampleProbability[i]);
				}
				glEnd();

				nsBasic::MemFree(IR_L, IRLength*sizeof(float));
				nsBasic::MemFree(IR_R, IRLength*sizeof(float));
			}
			lpListenerPair->SubmitRead(lpCurListener);
		}
		else if (ECFrameCnt_Max > 0){
			Listener *lpCurListener = lpListenerPair->RequestRead();
			lpListenerPair->SubmitRead(lpCurListener);
		}

		glColor3f(1.0f, 1.0f, 1.0f);

		nsGUI::Rect Label1 = { 25, 175, 300, 190 }, Label2 = { 25, 195, 300, 210 };
		nsGUI::GLDrawText(Font1, &Label1, "Show Impulse Response", nsCharCoding::ANSI, nsGUI::TEXTLAYOUT_HORI_LEFT | nsGUI::TEXTLAYOUT_VERT_MEDIUM);
		nsGUI::GLDrawText(Font1, &Label2, "Record Energy Response", nsCharCoding::ANSI, nsGUI::TEXTLAYOUT_HORI_LEFT | nsGUI::TEXTLAYOUT_VERT_MEDIUM);
	}
	va_end(args);
	return 0;
}
extern UBINT _cdecl MyEventHdlrProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...){
	va_list args;
	va_start(args, Msg);
	nsBasic::ObjGeneral *Ctrl = va_arg(args, nsBasic::ObjGeneral *);
	if (MSG_COMMAND == Msg){
		if ((nsBasic::ObjGeneral *)BtnScene == Ctrl){
			FilePath[0] = '\0';
			OPENFILENAME DlgParam = DialogTemplate;
			DlgParam.lpstrFilter = L"All files\0*.*\0\0";
			DlgParam.lpstrFile = FilePath;
			DlgParam.lpstrTitle = L"Open File";

			if (GetOpenFileName(&DlgParam)){
				Playing = false;
				MySndOutputStream->Pause();

				nsFile::ReadFileStream RFS(FilePath);
				nsFile::ReadStreamBuffer RSB(&RFS);
				LoadScene(&RSB);
			}
		}
		else if ((nsBasic::ObjGeneral *)BtnPlay == Ctrl){
			Playing = !Playing;
			if (Playing)MySndOutputStream->Play();
			else MySndOutputStream->Pause();
		}
		else if ((nsBasic::ObjGeneral *)BtnRecord == Ctrl){
			atomic_add(&RecordFlag, 1);
		}
		else if ((nsBasic::ObjGeneral *)BtnRecordOffline == Ctrl){
			if (OfflineRecState){
				OfflineWavWriter.CloseFile();
				OfflineRecState = false;
				OfflineRecCounter = 0;
			}
			else{
				FilePath[0] = '\0';
				OPENFILENAME DlgParam = DialogTemplate;
				DlgParam.lpstrFilter = L"WAV files(*.wav)\0*.wav\0\0";
				DlgParam.lpstrFile = FilePath;
				DlgParam.lpstrTitle = L"Save Waveform File";
				DlgParam.lpstrDefExt = L"wav";

				if (GetSaveFileName(&DlgParam)){
					OfflineWavWriter.OpenFile(FilePath);
					Offline_HDRFilterStat_L.Initialize();
					Offline_HDRFilterStat_R.Initialize();
					OfflineRecState = true;

					for (auto _It = AnimationManager->AnimationMap.begin(); _It != AnimationManager->AnimationMap.end(); ++_It){
						_It->second.Reset();
						_It->second.Evaluate();
					}
				}

				atomic_xchg(&ECFrameCnt, 0);
			}
		}
		else if ((nsBasic::ObjGeneral *)BtnFrameTest == Ctrl){
			// output energy curve
			atomic_xchg(&ECFrameCnt, 0);
			ECFrameCnt_Max = TestFrameCount;
		}
		else if ((nsBasic::ObjGeneral *)ChkShowIR == Ctrl){
			ShowIR = (1 == ChkShowIR->Data.Status);
		}
		else if ((nsBasic::ObjGeneral *)ChkOutputEC == Ctrl){
			atomic_xchg(&OutputEC, 1 == ChkOutputEC->Data.Status ? 1 : 0);
		}
	}
	else if (MSG_IDLE == Msg){
		if (ECFrameCnt < ECFrameCnt_Max)nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
		else ECFrameCnt_Max = 0;
		if (OfflineRecState)nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
	}
	va_end(args);
	return 0;
}
extern UBINT _cdecl MsgProc_Screen(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...){
	va_list args;
	va_start(args, Msg);

	switch (Msg){
	case MSG_REPOS:
		ScreenRect.XMin = va_arg(args, UBINT);
		ScreenRect.YMin = va_arg(args, UBINT);
		ScreenRect.XMax = ScreenRect.XMin + va_arg(args, UBINT);
		ScreenRect.YMax = ScreenRect.YMin + va_arg(args, UBINT);
		return 0;
	case MSG_MOUSEENTER:
		ScreenStat = 1;
		nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
		return 0;
	case MSG_MOUSEEVENT:
	{
		nsGUI::MouseStat *MyMouseStat = va_arg(args, nsGUI::MouseStat *);
		UBINT NewStat;

		if (MyMouseStat->BtnStat & nsGUI::MBTN_L){
			NewStat = 2;
			if (2 == ScreenStat){
				if (false == OfflineRecState || AnimationManager->AnimationMap.empty()){
					MyCameraData.EulerAngle[0] -= M_PI*((float)MyMouseStat->XPos - (float)LastMouseX) / (float)(ScreenRect.XMax - ScreenRect.XMin);
					if (MyCameraData.EulerAngle[0] < 0)MyCameraData.EulerAngle[0] += 2 * M_PI;
					MyCameraData.EulerAngle[1] -= M_PI*((float)MyMouseStat->YPos - (float)LastMouseY) / (float)(ScreenRect.YMax - ScreenRect.YMin);
					if (MyCameraData.EulerAngle[1] > M_PI_2)MyCameraData.EulerAngle[1] = M_PI_2;
					if (MyCameraData.EulerAngle[1] < -M_PI_2)MyCameraData.EulerAngle[1] = -M_PI_2;
					MySoundClusterMgr->SoundClusterList_Listener.front().Change_Exist = true;
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
				}
			}
			LastMouseX = MyMouseStat->XPos;
			LastMouseY = MyMouseStat->YPos;
		}
		else NewStat = 1;

		if (0 != MyMouseStat->ZPos){
			if (false == OfflineRecState || AnimationManager->AnimationMap.empty()){
				if (MyMouseStat->ZPos > 0x8000)MyCameraData.Position[2] += 0.001*(float)(0x10000 - MyMouseStat->ZPos); else MyCameraData.Position[2] -= 0.001*(float)MyMouseStat->ZPos;
				MySoundClusterMgr->SoundClusterList_Listener.front().Change_Exist = true;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
		}
		ScreenStat = NewStat;
		return 0;
	}
	case MSG_MOUSELEAVE:
		ScreenStat = 0;
		nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
		return 0;
	case MSG_KEYBDENTER:
		return 1;
	case MSG_KEYBDEVENT:
		if(GetKeyState(VK_UP) & 0xFF80){
			float Direction[3] = { cos(MyCameraData.EulerAngle[0]), -sin(MyCameraData.EulerAngle[0]), 0.0f }; // , Norm[3], Result[3];

			if (false == OfflineRecState || AnimationManager->AnimationMap.empty()){
				MyCameraData.Position[0] += 0.05f*Direction[0];
				MyCameraData.Position[1] += 0.05f*Direction[1];
				MySoundClusterMgr->SoundClusterList_Listener.front().Change_Exist = true;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
		}
		else if (GetKeyState(VK_DOWN) & 0xFF80){
			float Direction[3] = { -cos(MyCameraData.EulerAngle[0]), sin(MyCameraData.EulerAngle[0]), 0.0f }; // , Norm[3], Result[3];
			if (false == OfflineRecState || AnimationManager->AnimationMap.empty()){
				MyCameraData.Position[0] += 0.05f*Direction[0];
				MyCameraData.Position[1] += 0.05f*Direction[1];
				MySoundClusterMgr->SoundClusterList_Listener.front().Change_Exist = true;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
		}
		else if (GetKeyState(VK_LEFT) & 0xFF80){
			float Direction[3] = { sin(MyCameraData.EulerAngle[0]), cos(MyCameraData.EulerAngle[0]), 0.0f }; // , Norm[3], Result[3];
			if (false == OfflineRecState || AnimationManager->AnimationMap.empty()){
				MyCameraData.Position[0] += 0.05f*Direction[0];
				MyCameraData.Position[1] += 0.05f*Direction[1];
				MySoundClusterMgr->SoundClusterList_Listener.front().Change_Exist = true;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
		}
		else if (GetKeyState(VK_RIGHT) & 0xFF80){
			float Direction[3] = { -sin(MyCameraData.EulerAngle[0]), -cos(MyCameraData.EulerAngle[0]), 0.0f }; // , Norm[3], Result[3];
			if (false == OfflineRecState || AnimationManager->AnimationMap.empty()){
				MyCameraData.Position[0] += 0.05f*Direction[0];
				MyCameraData.Position[1] += 0.05f*Direction[1];
				MySoundClusterMgr->SoundClusterList_Listener.front().Change_Exist = true;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
		}
		return 0;
	case MSG_KEYBDLEAVE:
		return 0;
	case MSG_PAINT:
		if (ScreenStat>0){
			if (1 == ScreenStat)glColor3d(0.5, 0.5, 0.6); else glColor3d(0.2, 0.2, 0.5);
			glLineWidth(5.0);
			glBegin(GL_LINE_LOOP);
			glVertex2d(ScreenRect.XMin, ScreenRect.YMin);
			glVertex2d(ScreenRect.XMin, ScreenRect.YMax);
			glVertex2d(ScreenRect.XMax, ScreenRect.YMax);
			glVertex2d(ScreenRect.XMax, ScreenRect.YMin);
			glEnd();
		}
		return 0;
	case MSG_DESTROY:
		delete lpObj;
		return 0;
	default:
		return 0;
	}
}

extern int UserMain(){
	nsBasic::CreateThreadExtObj();

	DialogTemplate.lStructSize = sizeof(OPENFILENAME);
	DialogTemplate.hwndOwner = NULL;
	DialogTemplate.hInstance = (HINSTANCE)nsEnv::InstanceID;
	DialogTemplate.lpstrCustomFilter = NULL;
	DialogTemplate.nMaxCustFilter = 0;
	DialogTemplate.nFilterIndex = 0;
	DialogTemplate.nMaxFile = MAX_PATH;
	DialogTemplate.lpstrFileTitle = NULL;
	DialogTemplate.nMaxFileTitle = 0;
	DialogTemplate.lpstrInitialDir = NULL;
	DialogTemplate.Flags = OFN_FILEMUSTEXIST;
	DialogTemplate.lpstrDefExt = NULL;
	DialogTemplate.pvReserved = NULL;
	DialogTemplate.dwReserved = 0;
	DialogTemplate.FlagsEx = 0;

	MyEventHdlr = new nsBasic::ObjGeneral(nullptr, MyEventHdlrProc);
	nsBasic::SetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager), MyEventHdlr, IO_USER);

	ScreenRect.XMin = 20;
	ScreenRect.YMin = 20;
	ScreenRect.XMax = 820;
	ScreenRect.YMax = 620;
	nsGUI::LoadWindowMgr();

	nsGUI::GLWindow MyWnd(&ScreenRect);
	nsBasic::SetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager), (nsBasic::ObjGeneral *)&MyWnd, IO_VIDEO_OUT);

	nsGUI::ControlManager MyCtrlMgr;
	nsBasic::SetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager), (nsBasic::ObjGeneral *)&MyCtrlMgr, IO_KEYBD);
	nsBasic::SetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager), (nsBasic::ObjGeneral *)&MyCtrlMgr, IO_MOUSE);
	nsBasic::SetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager), (nsBasic::ObjGeneral *)&MyCtrlMgr, IO_IME);

	nsGUIText::FontEngine MyTextEngine;

	wchar_t FontFolderPath[260];
	SHGetSpecialFolderPath(nullptr, FontFolderPath, CSIDL_FONTS, FALSE);
	nsText::String_Sys FontFolderPath_StrSys(FontFolderPath);
	nsText::String FontFolderPath_Str1(FontFolderPath_StrSys), FontFolderPath_Str2(FontFolderPath_Str1);
	FontFolderPath_Str1 += "\\cour.ttf";
	FontFolderPath_Str2 += "\\courbd.ttf";
	Font1 = MyTextEngine.CreateFontRaster((const char *)FontFolderPath_Str1.cbegin(), 12);
	Font2 = MyTextEngine.CreateFontRaster((const char *)FontFolderPath_Str2.cbegin(), 24);

	My3DScene = new nsBasic::ObjSet((nsBasic::ObjSet *)&MyWnd, nsGUI::MsgProc_GL3DScene);
	Scene_3D = new nsBasic::ObjGeneral(My3DScene, MsgProc_Scene_3D);

	Scene_2D = new nsBasic::ObjGeneral((nsBasic::ObjSet *)&MyWnd, MsgProc_Scene_2D);

	MyCtrlScene = new nsBasic::ObjSet((nsBasic::ObjSet *)&MyWnd, nsGUI::MsgProc_GLCtrlScene);

	Screen = new nsBasic::ObjGeneral(MyCtrlScene, MsgProc_Screen);
	nsGUI::RectHitRgn *ScreenRgn = new nsGUI::RectHitRgn((nsBasic::ObjSet *)&MyCtrlMgr, Screen, &ScreenRect);

	SndClusterCtrl = new ClusterCtrl(MyCtrlScene);
	SndClusterCtrl->RegisterHitRgn((nsBasic::ObjSet *)&MyCtrlMgr);

	BtnScene = new nsGUI::Button((nsBasic::ObjSet *)MyCtrlScene);
	BtnScene->Data.CtrlPos = { 5, 5, 50, 30 };
	BtnScene->Data.Dock = 0;
	BtnScene->Data.Status = 1;
	BtnScene->Data.InnerStatus = 0;
	BtnScene->Data.CtrlText = "Open";
	BtnScene->Data.CtrlFont = Font1;
	BtnScene->RegisterHitRgn((nsBasic::ObjSet *)&MyCtrlMgr);

	BtnPlay = new nsGUI::Button((nsBasic::ObjSet *)MyCtrlScene);
	BtnPlay->Data.CtrlPos = { 55, 5, 100, 30 };
	BtnPlay->Data.Dock = 0;
	BtnPlay->Data.Status = 1;
	BtnPlay->Data.InnerStatus = 0;
	BtnPlay->Data.CtrlText = "Play";
	BtnPlay->Data.CtrlFont = Font1;
	BtnPlay->RegisterHitRgn((nsBasic::ObjSet *)&MyCtrlMgr);

	BtnRecord = new nsGUI::Button((nsBasic::ObjSet *)MyCtrlScene);
	BtnRecord->Data.CtrlPos = { 5, 35, 100, 60 };
	BtnRecord->Data.Dock = 0;
	BtnRecord->Data.Status = 1;
	BtnRecord->Data.InnerStatus = 0;
	BtnRecord->Data.CtrlText = "Record(RT)";
	BtnRecord->Data.CtrlFont = Font1;
	BtnRecord->RegisterHitRgn((nsBasic::ObjSet *)&MyCtrlMgr);

	BtnRecordOffline = new nsGUI::Button((nsBasic::ObjSet *)MyCtrlScene);
	BtnRecordOffline->Data.CtrlPos = { 5, 65, 100, 90 };
	BtnRecordOffline->Data.Dock = 0;
	BtnRecordOffline->Data.Status = 1;
	BtnRecordOffline->Data.InnerStatus = 0;
	BtnRecordOffline->Data.CtrlText = "Record(Path)";
	BtnRecordOffline->Data.CtrlFont = Font1;
	BtnRecordOffline->RegisterHitRgn((nsBasic::ObjSet *)&MyCtrlMgr);

	BtnFrameTest = new nsGUI::Button((nsBasic::ObjSet *)MyCtrlScene);
	BtnFrameTest->Data.CtrlPos = { 5, 95, 100, 120 };
	BtnFrameTest->Data.Dock = 0;
	BtnFrameTest->Data.Status = 1;
	BtnFrameTest->Data.InnerStatus = 0;
	BtnFrameTest->Data.CtrlText = "Frame Test";
	BtnFrameTest->Data.CtrlFont = Font1;
	BtnFrameTest->RegisterHitRgn((nsBasic::ObjSet *)&MyCtrlMgr);

	ChkShowIR = new nsGUI::CheckBox((nsBasic::ObjSet *)MyCtrlScene);
	ChkShowIR->Data.CtrlPos = { 5, 175, 20, 190 };
	ChkShowIR->Data.Dock = 0;
	ChkShowIR->Data.Status = 0;
	ChkShowIR->Data.InnerStatus = 0;
	ChkShowIR->RegisterHitRgn((nsBasic::ObjSet *)&MyCtrlMgr);

	ChkOutputEC = new nsGUI::CheckBox((nsBasic::ObjSet *)MyCtrlScene);
	ChkOutputEC->Data.CtrlPos = { 5, 195, 20, 210 };
	ChkOutputEC->Data.Dock = 0;
	ChkOutputEC->Data.Status = 0;
	ChkOutputEC->Data.InnerStatus = 0;
	ChkOutputEC->RegisterHitRgn((nsBasic::ObjSet *)&MyCtrlMgr);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	Embree_Init();

	nsAudio::OutputDevice_WASAPI MySndOutputDevice;
	MySndOutputStream = MySndOutputDevice.CreateStream_Output<INT2b>(2, SndWndLen, AudioSampleRate);
	UBINT hSoundRenderThread = nsBasic::CreateNormalThread(SoundRenderThread, nullptr);
	RTBarrier1.Initialize(2);
	RTBarrier2.Initialize(2);
	UBINT hSoundRayTraceThread = nsBasic::CreateNormalThread(SoundRayTraceThread, nullptr);

	RenderManager = new nsRender::RenderCore(MyWnd.GetGraphicDevice());
	InitDefaultScene();

	MyWnd.Show();
	nsGUI::SimpleMsgPump();

	atomic_xchg(&ExeStopFlag, 1);
	MySndOutputStream->Stop();
	nsBasic::WaitThread(hSoundRenderThread);
	RTBarrier1.Wait();
	nsBasic::WaitThread(hSoundRayTraceThread);

	ClearScene();
	delete RenderManager;

	Embree_Destroy();
	return 0;
}