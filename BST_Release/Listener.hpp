#ifndef SOUND_LISTENER
#define SOUND_LISTENER

#include "lGeneral.hpp"
#include "lMath_SIMD.hpp"
#include "lAudio_Filter.hpp"
#include "AudioRenderer.hpp"
#include "CrossoverFilter.hpp"

class Listener;

struct HRTFInfo{
	float Gain[2];
	float DelayCoeff[2];
};

const UBINT BandCount = 2;

struct IRSect{
public:
	INT4b DopplerShift;

	float *IR[BandCount];

	UBINT BufferSize_IR;
	UBINT Length_IR;

	//multiband tracing
	FloatVec_SIMD *EC[BandCount];
	UBINT BufferSize_EC;
	UBINT Length_EC;

	void _Realloc_IR(Listener *lpListener, UBINT Size);
	void _Realloc_EC(Listener *lpListener, UBINT Size);
	void _Resize_IR(Listener *lpListener, UBINT Size);
	void _Resize_EC(Listener *lpListener, UBINT Size);
};

class IRTrack{
	Listener *lpParent;

	static const UBINT IRSectHeap_MaxSize = 1;

	IRSect IRSectPool[IRSectHeap_MaxSize];
	IRSect *IRSectHeap[IRSectHeap_MaxSize], *CurIRSect;
	UBINT IRSectHeap_Size;
	INT4b CurIRSect_Shift;

public:
	AudioRenderer::Filter *pFilter;
	AudioRenderer::FilterLane *pLane[BandCount];
	nsContainer::Vector<AudioRenderer::InputStream *> StreamList;

	IRTrack(Listener *lpParent);
	void SetRenderParams(AudioRenderer::Filter *pFilter, AudioRenderer::FilterLane **pLane);
	void LoadDopplerShift(float DopplerShift);

	void AddImpulse(float Delay, FloatVec_SIMD& Intensity, HRTFInfo *HRTF);

	void Compile();
	void GetIR(float *IR_L, float *IR_R);
	void GetEnergyCurve(float *Result);
	void GetEnergyCurve_Multiband(float *Result);

	void BeginWrite();
	void EndWrite();
	~IRTrack();
};

class Listener{
private:
	friend class IRTrack;
	friend struct IRSect;

	float *IRBlockAlloc();
	void IRBlockFree(float *lpBlock);

	//multiband tracing
	CrossoverFilter BandFilter;

	IRTrack *CurrentTrack;

	const float HeadDelayRadius = 2.06E-4f;
	HRTFInfo HRTF_Internal;

	AudioRenderer *pRenderer;
	AudioRenderer::OutputStream *pOutput[BandCount];
public:
	inline void *operator new(size_t size){ return nsBasic::GlobalMemAlloc(size); }
	inline void operator delete(void *ptr){ nsBasic::GlobalMemFree(ptr); };
	
	nsContainer::HashMap<UBINT, IRTrack> IRTrackMap;
	float Volume;

	Listener();
	void SetRenderParams(AudioRenderer *pRenderer, AudioRenderer::OutputStream **pOutputs);
	void BeginWrite();
	void EndWrite_CurTrack();

	IRTrack *LoadTrack(UBINT TrackIndex);
	IRTrack *GetTrack(UBINT TrackIndex);
	void SetCurTrack(IRTrack *pTrack);

	void LoadHRTF(const float *Vector_Incident, const float *HeadVec_X, const float *HeadVec_Y, const float *HeadVec_Z);

	void LoadDopplerShift(float DopplerShift);
	void EvaluateImpulse(float Delay, FloatVec_SIMD& Intensity);

	void Compile();
	bool Render(float *Result_L, float *Result_R);
	~Listener();
};

/*-------------------------------- IMPLEMENTATION --------------------------------*/

inline void IRSect::_Realloc_IR(Listener *lpListener, UBINT Size){
	if (this->BufferSize_IR != Size){
		if (this->BufferSize_IR > 0){
			for (UBINT i = 0; i<BandCount; i++)nsBasic::MemFree_Arr_SIMD(this->IR[i], this->BufferSize_IR);
		}
		if (Size > 0){
			for (UBINT i = 0; i < BandCount; i++){
				this->IR[i] = nsBasic::MemAlloc_Arr_SIMD<float>(Size);
				nsMath::setzero_simd_unaligned(this->IR[i], Size);
			}
		}
		this->BufferSize_IR = Size;
	}
	else{
		for (UBINT i = 0; i<BandCount; i++)nsMath::setzero_simd_unaligned(this->IR[i], Size);
	}
}
inline void IRSect::_Realloc_EC(Listener *lpListener, UBINT Size){
	if (this->BufferSize_EC != Size){
		if (this->BufferSize_EC > 0){
			for (UBINT i = 0; i<BandCount; i++)nsBasic::MemFree_Arr_SIMD(this->EC[i], this->BufferSize_EC);
		}
		if (Size > 0){
			for (UBINT i = 0; i < BandCount; i++){
				this->EC[i] = nsBasic::MemAlloc_Arr_SIMD<FloatVec_SIMD>(Size);
				nsMath::setzero_simd_unaligned(this->EC[i], Size);
			}
		}
		this->BufferSize_EC = Size;
	}
	else{
		for (UBINT i = 0; i<BandCount; i++)nsMath::setzero_simd_unaligned(this->EC[i], Size);
	}
}
inline void IRSect::_Resize_IR(Listener *lpListener, UBINT Size){
	if (this->BufferSize_IR != Size){
		if (Size > 0){
			for (UBINT i = 0; i < BandCount; i++){
				float *IR_Prev = this->IR[i];
				this->IR[i] = nsBasic::MemAlloc_Arr_SIMD<float>(Size);
				if (this->BufferSize_IR > 0){
					if (this->BufferSize_IR <= Size){
						nsBasic::MemMove_Arr(this->IR[i], IR_Prev, this->BufferSize_IR);
						nsMath::setzero_simd_unaligned(&this->IR[i][this->BufferSize_IR], Size - this->BufferSize_IR);
					}
					else nsBasic::MemMove_Arr(this->IR[i], IR_Prev, Size);
					nsBasic::MemFree_Arr_SIMD(IR_Prev, this->BufferSize_IR);
				}
				else nsMath::setzero_simd_unaligned(this->IR[i], Size);
			}
		}
		else{
			for (UBINT i = 0; i < BandCount; i++)nsBasic::MemFree_Arr_SIMD(this->IR[i], this->BufferSize_IR);
		}
		this->BufferSize_IR = Size;
	}
}
inline void IRSect::_Resize_EC(Listener *lpListener, UBINT Size){
	if (this->BufferSize_EC != Size){
		if (Size > 0){
			for (UBINT i = 0; i < BandCount; i++){
				FloatVec_SIMD *EC_Prev = this->EC[i];
				this->EC[i] = nsBasic::MemAlloc_Arr_SIMD<FloatVec_SIMD>(Size);
				if (this->BufferSize_EC > 0){
					if (this->BufferSize_EC <= Size){
						nsBasic::MemMove_Arr(this->EC[i], EC_Prev, this->BufferSize_EC);
						nsMath::setzero_simd_unaligned(&this->EC[i][this->BufferSize_EC], Size - this->BufferSize_EC);
					}
					else nsBasic::MemMove_Arr(this->EC[i], EC_Prev, Size);
					nsBasic::MemFree_Arr_SIMD(EC_Prev, this->BufferSize_EC);
				}
				else nsMath::setzero_simd_unaligned(this->EC[i], Size);
			}
		}
		else{
			for (UBINT i = 0; i < BandCount; i++)nsBasic::MemFree_Arr_SIMD(this->EC[i], this->BufferSize_EC);
		}
		this->BufferSize_EC = Size;
	}
}

IRTrack::IRTrack(Listener *lpParent){
	this->lpParent = lpParent;
	for (UBINT i = 0; i < IRSectHeap_MaxSize; i++){
		for (UBINT j = 0; j < BandCount; j++){
			IRSectPool[i].IR[j] = nullptr;
			IRSectPool[i].EC[j] = nullptr;
		}
		IRSectPool[i].Length_IR = 0;
		IRSectPool[i].BufferSize_IR = 0;
		IRSectPool[i].Length_EC = 0;
		IRSectPool[i].BufferSize_EC = 0;
	}
	this->IRSectHeap_Size = 0;
	for (UBINT i = 0; i < IRSectHeap_MaxSize; i++)this->IRSectHeap[i] = &this->IRSectPool[i];
	this->pFilter = nullptr;
	for (UBINT i = 0; i < BandCount; i++)this->pLane[i] = nullptr;
}
inline void IRTrack::SetRenderParams(AudioRenderer::Filter *pFilter, AudioRenderer::FilterLane **pLane){
	this->pFilter = pFilter;
	for (UBINT i = 0; i < BandCount; i++)this->pLane[i] = pLane[i];
}
void IRTrack::LoadDopplerShift(float DopplerShift){
	INT4b DopplerShift_Sample = (INT4b)(DopplerShift * (float)AudioSampleRate);
	if (this->IRSectHeap_Size < 1){
		this->IRSectHeap[0] = &this->IRSectPool[0];
		this->IRSectHeap[0]->DopplerShift = 0;
		this->IRSectHeap_Size = 1;
	}
	this->CurIRSect = this->IRSectHeap[0];

	this->CurIRSect_Shift = DopplerShift - this->CurIRSect->DopplerShift;
}
void IRTrack::AddImpulse(float Delay, FloatVec_SIMD& Intensity, HRTFInfo *HRTF){
	UINT4b Delay_0 = (UINT4b)(sqrt((Delay * (Delay + HRTF->DelayCoeff[1]) + HRTF->DelayCoeff[0])) * (float)AudioSampleRate);
	UINT4b Delay_1 = (UINT4b)(sqrt((Delay * (Delay - HRTF->DelayCoeff[1]) + HRTF->DelayCoeff[0])) * (float)AudioSampleRate);
	if (Delay_0 < IRLength){
		if (this->CurIRSect->BufferSize_EC <= Delay_0)this->CurIRSect->_Resize_EC(this->lpParent, Delay_0 + 1);
		if (this->CurIRSect->Length_EC < (UBINT)(Delay_0 + 1))this->CurIRSect->Length_EC = (UBINT)(Delay_0 + 1);

		FloatVec_SIMD TmpVec = Intensity;
		TmpVec.mul(HRTF->Gain[0]);
		this->CurIRSect->EC[0][Delay_0].add(TmpVec);
	}

	if (Delay_1 < IRLength){
		if (this->CurIRSect->BufferSize_EC <= Delay_1)this->CurIRSect->_Resize_EC(this->lpParent, Delay_1 + 1);
		if (this->CurIRSect->Length_EC < (UBINT)(Delay_1 + 1))this->CurIRSect->Length_EC = (UBINT)(Delay_1 + 1);

		FloatVec_SIMD TmpVec = Intensity;
		TmpVec.mul(HRTF->Gain[1]);
		this->CurIRSect->EC[1][Delay_1].add(TmpVec);
	}
}
void IRTrack::BeginWrite(){
	for (UBINT i = 0; i < IRSectHeap_MaxSize; i++){
		UBINT IRLength_Estimated = this->IRSectPool[i].Length_IR;
		if (IRLength_Estimated > 0)IRLength_Estimated += IRLength_Estimated / 8;
		if (IRLength_Estimated > IRLength)IRLength_Estimated = IRLength;
		this->IRSectPool[i]._Realloc_IR(this->lpParent, IRLength_Estimated);
		this->IRSectPool[i].Length_IR = 0;

		UBINT ECLength_Estimated = this->IRSectPool[i].Length_EC;
		if (ECLength_Estimated > 0)ECLength_Estimated += ECLength_Estimated / 8;
		if (ECLength_Estimated > IRLength)ECLength_Estimated = IRLength;
		this->IRSectPool[i]._Realloc_EC(this->lpParent, ECLength_Estimated);
		this->IRSectPool[i].Length_EC = 0;
	}
	this->IRSectHeap_Size = 0;
}
void IRTrack::EndWrite(){

#if defined LIBENV_CPU_ACCEL_MMX
	unsigned int flushMode = _MM_GET_FLUSH_ZERO_MODE();
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif

	for (UBINT i = 0; i < IRSectHeap_Size; i++){
		for (UBINT j = 0; j < BandCount; j++){
			for (UBINT k = 0; k < this->IRSectHeap[i]->Length_EC; k++){
				this->IRSectHeap[i]->EC[j][k].sqrt();
			}
		}
	}
	UBINT TrialStep = this->lpParent->pRenderer->GetFrameLen();
	for (UBINT i = 0; i < IRSectHeap_Size; i++){
		if (this->IRSectHeap[i]->BufferSize_IR < this->IRSectHeap[i]->Length_EC)this->IRSectHeap[i]->_Resize_IR(this->lpParent, this->IRSectHeap[i]->Length_EC);
		for (UBINT j = 0; j < BandCount; j++){
			this->lpParent->BandFilter.Clear();
			this->lpParent->BandFilter.StreamIn(this->IRSectHeap[i]->IR[j], this->IRSectHeap[i]->EC[j], this->IRSectHeap[i]->Length_EC);
			UBINT CurPos = this->IRSectHeap[i]->Length_EC;

			while (CurPos < IRLength){
				UBINT CurWndLen = TrialStep;
				if (CurPos + CurWndLen >= IRLength)CurWndLen = IRLength - CurPos;
				if (!this->lpParent->BandFilter.isNegligible()){
					if (this->IRSectHeap[i]->BufferSize_IR < CurPos + CurWndLen)this->IRSectHeap[i]->_Resize_IR(this->lpParent, CurPos + CurWndLen);
					if (CurPos >= this->IRSectHeap[i]->Length_EC)this->lpParent->BandFilter.ZeroFill(&this->IRSectHeap[i]->IR[j][CurPos], CurWndLen);
					else this->lpParent->BandFilter.StreamIn(&this->IRSectHeap[i]->IR[j][CurPos], &this->IRSectHeap[i]->EC[j][CurPos], CurWndLen);
				}
				else break;
				CurPos += CurWndLen;
			}
			this->IRSectHeap[i]->Length_IR = CurPos;
		}
	}
#if defined LIBENV_CPU_ACCEL_MMX
	_MM_SET_FLUSH_ZERO_MODE(flushMode);
#endif

}
void IRTrack::Compile(){
	if (nullptr != this->pFilter){
		this->pFilter->ClearInput();
		// we'll just ignore the doppler effect here...
		AudioRenderer::MixCommand NewCmd;
		NewCmd.Volume = 1.0f;
		NewCmd.Start = 0;
		NewCmd.Length = this->lpParent->pRenderer->GetFrameLen();
		for (UBINT i = 0; i < this->StreamList.size(); i++){
			NewCmd.pStream = this->StreamList[i];
			this->pFilter->AddInput(NewCmd);
		}
		for (UBINT i = 0; i < BandCount; i++){
			this->pFilter->UpdateLane(this->pLane[i], this->IRSectHeap[0]->IR[i], this->IRSectHeap[0]->Length_IR);
		}
		this->pFilter->EndUpdate();
	}
}
void IRTrack::GetIR(float *IR_L, float *IR_R){
	nsMath::setzero_simd_unaligned(IR_L, IRLength);
	nsMath::setzero_simd_unaligned(IR_R, IRLength);
	for (UBINT i = 0; i < IRSectHeap_Size; i++){
		nsMath::add_simd_unaligned(IR_L, IR_L, this->IRSectHeap[i]->IR[0], this->IRSectHeap[i]->Length_IR);
		nsMath::add_simd_unaligned(IR_R, IR_R, this->IRSectHeap[i]->IR[1], this->IRSectHeap[i]->Length_IR);
	}
	nsMath::mul_simd_unaligned(IR_L, IR_L, lpParent->Volume, IRLength);
	nsMath::mul_simd_unaligned(IR_R, IR_R, lpParent->Volume, IRLength);
}
void IRTrack::GetEnergyCurve_Multiband(float *Result){
	nsMath::setzero_simd_unaligned(Result, IRLength * FloatVec_SIMD::Width);
	float *SqrBuffer = nsBasic::MemAlloc_Arr_SIMD<float>(IRLength * FloatVec_SIMD::Width);
	for (UBINT i = 0; i < IRSectHeap_Size; i++){
		nsBasic::MemMove_Arr((FloatVec_SIMD *)SqrBuffer, this->IRSectHeap[i]->EC[0], this->IRSectHeap[i]->Length_EC);
		nsMath::mul_simd_aligned(SqrBuffer, SqrBuffer, SqrBuffer, this->IRSectHeap[i]->Length_EC * FloatVec_SIMD::Width);
		nsMath::add_simd_unaligned(Result, Result, SqrBuffer, this->IRSectHeap[i]->Length_EC * FloatVec_SIMD::Width);

		nsBasic::MemMove_Arr((FloatVec_SIMD *)SqrBuffer, this->IRSectHeap[i]->EC[1], this->IRSectHeap[i]->Length_EC);
		nsMath::mul_simd_aligned(SqrBuffer, SqrBuffer, SqrBuffer, this->IRSectHeap[i]->Length_EC * FloatVec_SIMD::Width);
		nsMath::add_simd_unaligned(Result, Result, SqrBuffer, this->IRSectHeap[i]->Length_EC * FloatVec_SIMD::Width);
	}
	nsBasic::MemFree_Arr_SIMD(SqrBuffer, IRLength * FloatVec_SIMD::Width);
}
IRTrack::~IRTrack(){
	for (UBINT i = 0; i < IRSectHeap_MaxSize; i++){
		IRSectPool[i]._Resize_IR(this->lpParent, 0);
		IRSectPool[i]._Resize_EC(this->lpParent, 0);
	}
}
Listener::Listener(){
	this->Volume = 1.0f;
	this->pRenderer = nullptr;
	for (UBINT i = 0; i < BandCount; i++)this->pOutput[i] = nullptr;
}
inline void Listener::SetRenderParams(AudioRenderer *pRenderer, AudioRenderer::OutputStream **pOutputs){
	this->pRenderer = pRenderer;
	for (UBINT i = 0; i < BandCount; i++)this->pOutput[i] = pOutputs[i];
}
void Listener::BeginWrite(){
	for (auto _It = this->IRTrackMap.begin(); _It != this->IRTrackMap.end(); ++_It)_It->second.BeginWrite();
	this->CurrentTrack = nullptr;
}

inline void Listener::EndWrite_CurTrack(){
	if (nullptr != CurrentTrack)CurrentTrack->EndWrite();
}
inline IRTrack *Listener::LoadTrack(UBINT TrackIndex){
	auto InsertResult = this->IRTrackMap.try_emplace(TrackIndex, this);
	return &InsertResult.first->second;
}
inline IRTrack *Listener::GetTrack(UBINT TrackIndex){
	auto _It = this->IRTrackMap.find(TrackIndex);
	if (_It == this->IRTrackMap.end())return nullptr;
	else return &_It->second;
}
inline void Listener::SetCurTrack(IRTrack *pTrack){
	this->CurrentTrack = pTrack;
}
void Listener::LoadHRTF(const float *Vector_Incident, const float *HeadVec_X, const float *HeadVec_Y, const float *HeadVec_Z){
	// LHS coordinate, the direction of listener is (1, 0, 0).
	float Length = sqrt(nsMath::dot<3>(Vector_Incident, Vector_Incident));

	HRTF_Internal.Gain[0] = -nsMath::dot<3>(Vector_Incident, HeadVec_Y) / Length;
	HRTF_Internal.DelayCoeff[0] = HeadDelayRadius * HeadDelayRadius;
	HRTF_Internal.DelayCoeff[1] = -2.0f * HeadDelayRadius * HRTF_Internal.Gain[0];
	HRTF_Internal.Gain[0] = (1.0f + HRTF_Internal.Gain[0]) / 2.0f;
	HRTF_Internal.Gain[1] = 1.0f - HRTF_Internal.Gain[0];
}
inline void Listener::LoadDopplerShift(float DopplerShift){
	this->CurrentTrack->LoadDopplerShift(DopplerShift);
}
inline void Listener::EvaluateImpulse(float Delay, FloatVec_SIMD& Intensity){
	this->CurrentTrack->AddImpulse(Delay, Intensity, &this->HRTF_Internal);
}
inline void Listener::Compile(){
	for (auto _It = this->IRTrackMap.begin(); _It != this->IRTrackMap.end(); ++_It)_It->second.Compile();
}
inline bool Listener::Render(float *Result_L, float *Result_R){
	if (nullptr != this->pRenderer){
		this->pRenderer->RenderBegin();
		this->pRenderer->ExecuteFilter();
		this->pRenderer->ExecuteOutput();
		if (1.0f == this->Volume){
			nsBasic::MemMove_Arr(Result_L, this->pOutput[0]->pData, this->pRenderer->GetFrameLen());
			nsBasic::MemMove_Arr(Result_R, this->pOutput[1]->pData, this->pRenderer->GetFrameLen());
		}
		else{
			nsMath::mul_simd_unaligned(Result_L, this->pOutput[0]->pData, this->Volume, this->pRenderer->GetFrameLen());
			nsMath::mul_simd_unaligned(Result_R, this->pOutput[1]->pData, this->Volume, this->pRenderer->GetFrameLen());
		}
	}
	return nullptr != this->pRenderer;
}
inline Listener::~Listener(){ this->IRTrackMap.clear(); }
#endif