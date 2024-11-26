#include "lMath.hpp"
#include "lContainer.hpp"
#include "lFile.hpp"
#include "lJSON.hpp"
#include "lText_Sys.hpp"

#include "RenderCore.hpp"
#include "SceneNode.hpp"
#include "lFormat_OBJ_Wavefront.hpp"

#include "SoundClusterMgr.hpp"

#include "SceneNode_RT.hpp"

nsScene::DepthProgram DepthProgram_Inst;
nsScene::DepthProgram_Sphere DepthProgram_Sphere_Inst;
nsScene::OpaquePhongProgram OpaquePhongProgram_Inst;
nsScene::OpaquePhongProgram_Sphere OpaquePhongProgram_Sphere_Inst;
nsScene::WhiteProgram WhiteProgram_Inst;
nsScene::MatrixTransform CamTransform;
nsScene::Camera_P MyCamera;

struct PosEuler{
	float Position[3];
	float EulerAngle[3];
} MyCameraData;

AudioRenderer *MyAudioRenderer;
AudioRenderer::OutputStream *MySoundOutputChannel[BandCount];
SoundClusterMgr *MySoundClusterMgr;

template <class T> struct AnimationCurve{
	nsContainer::Vector<T> Keys;
	nsContainer::Vector<float> TimeSpan;

	inline void addKey(T NextStat){ this->Keys.push_back(NextStat); }
	inline void addSpan(float Span){ this->TimeSpan.push_back(Span); };
};

void PosEulerTween_Linear(PosEuler *Dest, PosEuler *Src1, PosEuler *Src2, float TweenPos);

class AnimationMgr{
public:
	struct AnimationInfo{
		AnimationCurve<PosEuler> Curve;
		float *pDestMatrix;
		bool *pChanged; // We use a pointer to notify changes for convenience. This is not recommended in practical use.
		UBINT CurKeyID;
		float CurTime;
		
		inline void Reset(){ this->CurKeyID = 0; this->CurTime = 0.0f; }
		inline void Clear(){ this->Curve.Keys.clear(); this->Curve.TimeSpan.clear(); this->Reset(); };
		bool Advance(float TimeSpan); // return true when the animation has ended,
		void Evaluate();
	};

	nsContainer::HashMap<nsText::String, AnimationInfo> AnimationMap;
	inline void Clear(){ this->AnimationMap.clear(); }
};

TableObj<RenderData> *ResManager = nullptr;
AnimationMgr *AnimationManager = nullptr;
AnimationMgr::AnimationInfo *pListenerAnimation = nullptr;
nsContainer::HashMap<AudioRenderer::InputStream *, nsText::String> *pInputStreamNameTable = nullptr;
nsScene::SimpleSceneManager SceneManager;
nsRender::RenderCore *RenderManager;

void LoadAnimationFile(nsBasic::Stream_R<unsigned char> *lpStream);
void SaveAnimationFile(nsBasic::Stream_W<unsigned char> *lpStream);
UBINT LoadScene(nsBasic::Stream_R<unsigned char> *lpStream);

void InitDefaultScene();
void ClearScene();

/*-------------------------------- IMPLEMENTATION --------------------------------*/

void PosEulerTween_Linear(PosEuler *Dest, PosEuler *Src1, PosEuler *Src2, float TweenPos){ //TweenPos must be between 0 and 1
	auto AngleInterp = [](float Angle1, float Angle2, float TweenPos){
		if (Angle1 - Angle2 < -M_PI){
			float TmpAngle = Angle2 * TweenPos + (Angle1 + 2 * M_PI) * ((float)1.0 - TweenPos);
			if (TmpAngle>2 * M_PI)return TmpAngle - (float)(2 * M_PI); else return TmpAngle;
		}
		else if (Angle1 - Angle2 > M_PI){
			float TmpAngle = (Angle2 + 2 * M_PI) * TweenPos + Angle1 * ((float)1.0 - TweenPos);
			if (TmpAngle>2 * M_PI)return TmpAngle - (float)(2 * M_PI); else return TmpAngle;
		}
		else return Angle2 * TweenPos + Angle1 * ((float)1.0 - TweenPos);
	};
	Dest->Position[0] = Src2->Position[0] * TweenPos + Src1->Position[0] * (1 - TweenPos);
	Dest->Position[1] = Src2->Position[1] * TweenPos + Src1->Position[1] * (1 - TweenPos);
	Dest->Position[2] = Src2->Position[2] * TweenPos + Src1->Position[2] * (1 - TweenPos);
	Dest->EulerAngle[0] = AngleInterp(Src1->EulerAngle[0], Src2->EulerAngle[0], TweenPos);
	Dest->EulerAngle[1] = Src2->EulerAngle[1] * TweenPos + Src1->EulerAngle[1] * (1 - TweenPos);
	Dest->EulerAngle[2] = AngleInterp(Src1->EulerAngle[2], Src2->EulerAngle[2], TweenPos);
}
bool AnimationMgr::AnimationInfo::Advance(float TimeSpan){
	this->CurTime += TimeSpan;
	size_t PathLen = std::min(this->Curve.Keys.size() - 1, this->Curve.TimeSpan.size());
	while (true){
		if (PathLen <= this->CurKeyID){
			this->CurTime = 0;
			return true;
		}
		else if (this->CurTime >= this->Curve.TimeSpan[this->CurKeyID]){
			this->CurTime -= this->Curve.TimeSpan[this->CurKeyID];
			this->CurKeyID++;
			if (this->CurTime < this->Curve.TimeSpan[this->CurKeyID])return false;
		}
		else return false;
	}
}
void AnimationMgr::AnimationInfo::Evaluate(){
	size_t PathLen = std::min(this->Curve.Keys.size() - 1, this->Curve.TimeSpan.size());
	PosEuler CurPosEuler;
	if (this->CurKeyID >= PathLen){
		CurPosEuler = this->Curve.Keys[PathLen];
	}
	else PosEulerTween_Linear(&CurPosEuler, &this->Curve.Keys[this->CurKeyID], &this->Curve.Keys[this->CurKeyID + 1], this->CurTime / this->Curve.TimeSpan[this->CurKeyID]);
	if (nullptr != this->pDestMatrix)nsMath::Transform4_Pos_RotEuler(CurPosEuler.Position, CurPosEuler.EulerAngle, this->pDestMatrix);
	if (nullptr != this->pChanged)*this->pChanged = true;
}

void LoadAnimationFile(AnimationMgr *pMgr, nsBasic::Stream_R<unsigned char> *lpStream){
	nsFile::DecoderStream Decoder(lpStream, nsCharCoding::ANSI);
	nsText::Lexer MyLexer(&Decoder);
	nsText::String Symbol;

	AnimationMgr::AnimationInfo *pAnimation = nullptr;
	PosEuler TmpCamStat;

	do{
		MyLexer.SkipBlankAndRet();
		if ((UBINT)'#' == MyLexer.LastChar)MyLexer.SkipLine();
		else{
			MyLexer.ReadWord(&Symbol);
			if ("newpath" == Symbol){
				MyLexer.ReadWord(&Symbol);
				auto InsertResult = pMgr->AnimationMap.try_emplace(Symbol);
				if (InsertResult.second){
					pAnimation = &InsertResult.first->second;
					pAnimation->pDestMatrix = nullptr;
					pAnimation->pChanged = nullptr;
				}
			}
			else if ("PosEuler" == Symbol){
				if (nullptr != pAnimation){
					PosEuler CurNode = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
					MyLexer.SkipBlank();
					MyLexer.ReadFloat(&CurNode.Position[0]);
					MyLexer.SkipBlank();
					MyLexer.ReadFloat(&CurNode.Position[1]);
					MyLexer.SkipBlank();
					MyLexer.ReadFloat(&CurNode.Position[2]);
					MyLexer.SkipBlank();
					MyLexer.ReadFloat(&CurNode.EulerAngle[0]);
					MyLexer.SkipBlank();
					MyLexer.ReadFloat(&CurNode.EulerAngle[1]);
					MyLexer.SkipBlank();
					MyLexer.ReadFloat(&CurNode.EulerAngle[2]);
					if (pAnimation->Curve.Keys.size() > pAnimation->Curve.TimeSpan.size() + 1)pAnimation->Curve.TimeSpan.push_back(0.0f);
					pAnimation->Curve.Keys.push_back(CurNode);
				}
			}
			else if ("tween" == Symbol){
				if (nullptr != pAnimation){
					float CurTween;
					MyLexer.SkipBlank();
					MyLexer.ReadFloat(&CurTween);
					pAnimation->Curve.TimeSpan.push_back(CurTween);
				}
			}
			MyLexer.SkipLine();
		}
	} while ((UINT4b)-1 != MyLexer.LastChar);
}

void SaveAnimationFile(AnimationMgr *pMgr, nsBasic::Stream_W<unsigned char> *lpStream){
	nsFile::CoderStream Coder(lpStream, nsCharCoding::ANSI);

	for (auto _It = pMgr->AnimationMap.begin(); _It != pMgr->AnimationMap.end(); ++_It){
		if (_It->second.Curve.Keys.size() > 0){
			lpStream->WriteBulk("newpath ", 8);
			nsText::WriteString(&Coder, &_It->first);
			lpStream->WriteBulk("\r\n", 2);
			size_t i;
			for (i = 0; i + 1 < _It->second.Curve.Keys.size(); i++){
				lpStream->WriteBulk("PosEuler ", 9);
				nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].Position[0]);
				lpStream->Write(" ");
				nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].Position[1]);
				lpStream->Write(" ");
				nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].Position[2]);
				lpStream->Write(" ");
				nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].EulerAngle[0]);
				lpStream->Write(" ");
				nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].EulerAngle[1]);
				lpStream->Write(" ");
				nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].EulerAngle[2]);
				lpStream->WriteBulk(" \r\ntween ", 9);
				if (i >= _It->second.Curve.TimeSpan.size())lpStream->WriteBulk("0\r\n", 3);
				else{
					nsText::WriteFloat(&Coder, _It->second.Curve.TimeSpan[i]);
					lpStream->WriteBulk("\r\n", 2);
				}
			}
			lpStream->WriteBulk("PosEuler ", 9);
			nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].Position[0]);
			lpStream->Write(" ");
			nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].Position[1]);
			lpStream->Write(" ");
			nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].Position[2]);
			lpStream->Write(" ");
			nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].EulerAngle[0]);
			lpStream->Write(" ");
			nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].EulerAngle[1]);
			lpStream->Write(" ");
			nsText::WriteFloat(&Coder, _It->second.Curve.Keys[i].EulerAngle[2]);
			lpStream->WriteBulk("\r\n", 2);
		}
		lpStream->WriteBulk("\r\n", 2);
	}
}

void InitDefaultScene(){
	MyCameraData.Position[0] = 0.0f;
	MyCameraData.Position[1] = 0.0f;
	MyCameraData.Position[2] = 1.7f;
	MyCameraData.EulerAngle[0] = 0.0f;
	MyCameraData.EulerAngle[1] = 0.0f;
	MyCameraData.EulerAngle[2] = 0.0f;

	nsBasic::IBase::Link(&SceneManager, &CamTransform, nullptr);
	nsBasic::IBase::Link(&CamTransform, &MyCamera, nullptr);

	RenderManager->RenderExpr_Initialize((nsBasic::IBase *)&ScrOutputProgram::IBase_Inst);
	RenderManager->RenderExpr_Initialize((nsBasic::IBase *)&DepthBlendProgram::IBase_Inst);
	RenderManager->RenderExpr_Initialize((nsBasic::IBase *)&nsScene::ESMShadowCaster::IBase_Inst);
	RenderManager->RenderExpr_Initialize((nsBasic::IBase *)&nsScene::OpaquePhongProgram_Light_Normal::IBase_Inst);
	RenderManager->RenderExpr_Initialize((nsBasic::IBase *)&nsScene::SSAOShadowCaster::IBase_Inst);
	RenderManager->RenderExpr_Initialize((nsBasic::IBase *)&nsScene::OpaquePhongProgram_Light_Ambient::IBase_Inst);

	AnimationManager = new AnimationMgr;
	MyAudioRenderer = new AudioRenderer(SndWndLen);
	MySoundClusterMgr = new SoundClusterMgr;

	MySoundOutputChannel[0] = MyAudioRenderer->AddOutput(); MySoundOutputChannel[1] = MyAudioRenderer->AddOutput();
}

UBINT LoadScene(nsBasic::Stream_R<unsigned char> *lpStream){
	nsFile::DecoderStream Decoder(lpStream, nsCharCoding::UTF8);
	nsText::Lexer MyLexer(&Decoder);
	nsText::String_Sys TmpPath;
	nsText::String Symbol;

	nsContainer::HashMap<nsText::String, AudioRenderer::InputStream *> InputStreamNameTable_Inverse;

	SceneManager.clear();
	if (nullptr == ResManager)ResManager = TableObj<RenderData>::Create();
	else ResManager->clear();
	if (nullptr == AnimationManager)AnimationManager = new AnimationMgr;
	else AnimationManager->Clear();
	if (nullptr == MyAudioRenderer)new AudioRenderer(SndWndLen);
	else MyAudioRenderer->Clear();
	if (nullptr == MySoundClusterMgr)MySoundClusterMgr = new SoundClusterMgr;
	else MySoundClusterMgr->Clear();
	if (nullptr == pInputStreamNameTable)pInputStreamNameTable = new nsContainer::HashMap<AudioRenderer::InputStream *, nsText::String>;
	else pInputStreamNameTable->clear();
	pListenerAnimation = nullptr;

	MySoundOutputChannel[0] = MyAudioRenderer->AddOutput(); MySoundOutputChannel[1] = MyAudioRenderer->AddOutput();

#ifdef USE_OPTIX
	ClearGGroup();
#elif defined USE_EMBREE
	Embree_ClearScene();
#endif
	
	//create root nodes for objects and materials
	TableObj<RenderData> *ObjRoot = TableObj<RenderData>::Create();
	TableObj<RenderData> *MtlRoot = TableObj<RenderData>::Create();
	ResManager->insert("Obj", ObjRoot);
	ResManager->insert("Mtl", MtlRoot);

	//add default material
	TableObj<RenderData> *DefMtl = TableObj<RenderData>::Create();
	if (DefMtl != MtlRoot->insert("", DefMtl))TableObj<RenderData>::Destroy(DefMtl);

	nsScene::MaterialProperty DefMtl_PropStruct;
	//set the default material property
	DefMtl_PropStruct.Status = 0;
	DefMtl_PropStruct.Diffuse[0] = 1.0f; DefMtl_PropStruct.Diffuse[1] = 1.0f; DefMtl_PropStruct.Diffuse[2] = 1.0f;
	DefMtl_PropStruct.Specular[0] = 0.0f; DefMtl_PropStruct.Specular[1] = 0.0f; DefMtl_PropStruct.Specular[2] = 0.0f;
	DefMtl_PropStruct.Opacity = 1.0f; DefMtl_PropStruct.Glossiness = 0.0f;

	nsMath::Matrix<nsScene::MaterialProperty> DefMtl_PropMat(1);
	DefMtl_PropMat(0) = DefMtl_PropStruct;
	TableObj<RenderData> *NewMtl_Prop = TableObj<RenderData>::Create();
	NewMtl_Prop->Value.CPUData = std::move(DefMtl_PropMat);
	NewMtl_Prop->Value.DataStat = RenderData::CPU_Avail;
	DefMtl->insert("Prop", NewMtl_Prop);

	do{
		while (nsText::IsBlankChar(MyLexer.LastChar) || nsText::IsRetChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
		if ((UBINT)'#' == MyLexer.LastChar)
		while ((!nsText::IsRetChar(MyLexer.LastChar)) && (UINT4b)-1 != MyLexer.LastChar)MyLexer.LoadNextChar(); //line skip
		else{
			MyLexer.ReadWord(&Symbol);
			if ("lobj" == Symbol){
				MyLexer.ReadFilePath(&TmpPath);
				nsFile::ReadFileStream RFS(TmpPath.cbegin());
				nsFile::ReadStreamBuffer RSB(&RFS);
				LoadObjFile(ObjRoot, MtlRoot, &RSB);
			}
			if ("lanim" == Symbol){
				MyLexer.ReadFilePath(&TmpPath);
				nsFile::ReadFileStream RFS(TmpPath.cbegin());
				nsFile::ReadStreamBuffer RSB(&RFS);
				LoadAnimationFile(AnimationManager, &RSB);
			}
			else if ("cobj" == Symbol){
				MyLexer.ReadWord(&Symbol);

				TableObj<RenderData> *NewObj_Res = ObjRoot->find(Symbol);
				if (nullptr != NewObj_Res){
					nsScene::SimpleSceneManager *NewObj = nsScene::SimpleSceneManager::Create();
					for (auto _It = NewObj_Res->begin(); _It.valid(); _It.inc()){
						nsScene::RTAtom *NewAtom = nsScene::RTAtom::Create();
						NewAtom->Data = _It.CurNode->Value;
						NewAtom->NormalProgram = &OpaquePhongProgram_Inst;
						NewAtom->DepthProgram = &DepthProgram_Inst;
						NewAtom->EmbreeLoad();
						NewAtom->RefreshGeometry();

						nsBasic::IBase::Link(NewObj, NewAtom, nullptr);
					}
					nsBasic::IBase::Link(&SceneManager, NewObj, nullptr);
				}
			}
			else if ("cobj_all" == Symbol){
				MyLexer.ReadWord(&Symbol);

				for (auto _It_Obj = ObjRoot->begin(); _It_Obj.valid(); _It_Obj.inc()){
					TableObj<RenderData> *NewObj_Res = _It_Obj.ref();
					nsScene::SimpleSceneManager *NewObj = nsScene::SimpleSceneManager::Create();
					for (auto _It = NewObj_Res->begin(); _It.valid(); _It.inc()){
						nsScene::RTAtom *NewAtom = nsScene::RTAtom::Create();
						NewAtom->Data = _It.CurNode->Value;
						NewAtom->NormalProgram = &OpaquePhongProgram_Inst;
						NewAtom->DepthProgram = &DepthProgram_Inst;
						NewAtom->EmbreeLoad();
						NewAtom->RefreshGeometry();

						nsBasic::IBase::Link(NewObj, NewAtom, nullptr);
					}
					nsBasic::IBase::Link(&SceneManager, NewObj, nullptr);
				}
			}
			else if ("clight" == Symbol){
				nsJSON::JSONReader MyJSONReader;

				MyJSONReader.Generate_String();
				MyJSONReader.BeginRead(&MyLexer);
				auto RootNode = MyJSONReader.ParseNode().second;
				MyJSONReader.ParseAllChild();
				MyJSONReader.EndRead();
				if (RootNode->IsObject()){
					auto Node_Type = (*RootNode)["type"];
					if (nullptr != Node_Type && Node_Type->IsString()){
						if ("point" == Node_Type->GetString()){
							nsScene::PointLight *NewLight = nsScene::PointLight::Create();
							nsScene::MatrixTransform *LightTransform = nsScene::MatrixTransform::Create();

							float Pos[3] = { 0.0f, 0.0f, 0.0f };

							auto Node_Pos = (*RootNode)["position"];
							if (nullptr != Node_Pos && Node_Pos->IsArray()){
								if (nullptr != (*Node_Pos)[(UBINT)0])Pos[0] = (float)(*Node_Pos)[(UBINT)0]->GetFloat();
								if (nullptr != (*Node_Pos)[(UBINT)1])Pos[1] = (float)(*Node_Pos)[(UBINT)1]->GetFloat();
								if (nullptr != (*Node_Pos)[(UBINT)2])Pos[2] = (float)(*Node_Pos)[(UBINT)2]->GetFloat();
							}
							nsMath::Transform4_Pos(Pos, LightTransform->Matrix);

							auto Node_Intensity = (*RootNode)["intensity"];
							if (nullptr != Node_Intensity && Node_Intensity->IsArray()){
								if (nullptr != (*Node_Intensity)[(UBINT)0])NewLight->Intensity[0] = (float)(*Node_Intensity)[(UBINT)0]->GetFloat(); else NewLight->Intensity[0] = 0.0f;
								if (nullptr != (*Node_Intensity)[(UBINT)1])NewLight->Intensity[1] = (float)(*Node_Intensity)[(UBINT)1]->GetFloat(); else NewLight->Intensity[1] = 0.0f;
								if (nullptr != (*Node_Intensity)[(UBINT)2])NewLight->Intensity[2] = (float)(*Node_Intensity)[(UBINT)2]->GetFloat(); else NewLight->Intensity[2] = 0.0f;
							}
							else{
								NewLight->Intensity[0] = 1.0f; NewLight->Intensity[1] = 1.0f; NewLight->Intensity[2] = 1.0f;
							}
							NewLight->AdjustFrustum_Default();

							nsBasic::IBase::Link(&SceneManager, LightTransform, nullptr);
							nsBasic::IBase::Link(LightTransform, NewLight, nullptr);
							nsBasic::IBase::Link(NewLight, (nsBasic::IBase *)&nsScene::OpaquePhongProgram_Light_Normal::IBase_Inst, nullptr);
						}
						else if ("ambient" == Node_Type->GetString()){
							nsScene::AmbientLight *NewLight = nsScene::AmbientLight::Create();

							auto Node_Intensity = (*RootNode)["intensity"];
							if (nullptr != Node_Intensity && Node_Intensity->IsArray()){
								if (nullptr != (*Node_Intensity)[(UBINT)0])NewLight->Intensity[0] = (float)(*Node_Intensity)[(UBINT)0]->GetFloat(); else NewLight->Intensity[0] = 0.0f;
								if (nullptr != (*Node_Intensity)[(UBINT)1])NewLight->Intensity[1] = (float)(*Node_Intensity)[(UBINT)1]->GetFloat(); else NewLight->Intensity[1] = 0.0f;
								if (nullptr != (*Node_Intensity)[(UBINT)2])NewLight->Intensity[2] = (float)(*Node_Intensity)[(UBINT)2]->GetFloat(); else NewLight->Intensity[2] = 0.0f;
							}
							else{
								NewLight->Intensity[0] = 0.1f; NewLight->Intensity[1] = 0.1f; NewLight->Intensity[2] = 0.1f;
							}

							auto Node_UseAO = (*RootNode)["UseAO"];
							if (nullptr != Node_UseAO && Node_UseAO->IsBool())NewLight->UseAO = Node_UseAO->GetBool();
							else NewLight->UseAO = true;

							nsBasic::IBase::Link(&SceneManager, NewLight, nullptr);
							nsBasic::IBase::Link(NewLight, (nsBasic::IBase *)&nsScene::OpaquePhongProgram_Light_Ambient::IBase_Inst, nullptr);
						}
					}
				}
				RootNode->Destroy();
			}
			else if ("snd_stream" == Symbol){
				nsJSON::JSONReader MyJSONReader;

				MyJSONReader.Generate_String();
				MyJSONReader.BeginRead(&MyLexer);
				auto RootNode = MyJSONReader.ParseNode().second;
				MyJSONReader.ParseAllChild();
				MyJSONReader.EndRead();
				if (RootNode->IsObject()){
					auto Node_Name = (*RootNode)["name"];
					auto Node_Path = (*RootNode)["path"];
					if (nullptr != Node_Name && nullptr != Node_Path && Node_Name->IsString() && Node_Path->IsString() && InputStreamNameTable_Inverse.end() == InputStreamNameTable_Inverse.find(Node_Name->GetString())){
						nsText::String_Sys ActualPath = Node_Path->GetString();
						nsFile::ReadFileStream *lpNewFileStream = new nsFile::ReadFileStream(ActualPath.cbegin());
						UBINT ExtPos = nsText::GetNameExtPos(&ActualPath);
						nsFormat::AudioReader *lpNewAudioDecoder = nsFormat::GetAudioReader(&ActualPath[ExtPos], ActualPath.size() - ExtPos);
						
						auto Node_Loop = (*RootNode)["loop"];
						if (nullptr != Node_Loop && Node_Loop->IsBool()){
							lpNewAudioDecoder->Loop = Node_Loop->GetBool();
						}
						else lpNewAudioDecoder->Loop = true;

						float Volume;
						auto Node_Volume = (*RootNode)["volume"];
						if (nullptr != Node_Volume && Node_Volume->IsFloat()){
							Volume = (float)Node_Volume->GetFloat();
						}
						else Volume = 1.0f;

						AudioRenderer::InputStream *pInputStream = MyAudioRenderer->AddInput(lpNewFileStream, lpNewAudioDecoder, Volume);
						if (nullptr == pInputStream){
							lpNewAudioDecoder->Delete();
							lpNewFileStream->Delete();
						}
						else{
							InputStreamNameTable_Inverse.try_emplace(Node_Name->GetString(), pInputStream);
							pInputStreamNameTable->try_emplace(pInputStream, Node_Name->GetString());
						}
					}
				}
				RootNode->Destroy();
			}
			else if ("snd_src" == Symbol){
				nsJSON::JSONReader MyJSONReader;

				MyJSONReader.Generate_String();
				MyJSONReader.BeginRead(&MyLexer);
				auto RootNode = MyJSONReader.ParseNode().second;
				MyJSONReader.ParseAllChild();
				MyJSONReader.EndRead();
				if (RootNode->IsObject()){
					auto Node_Stream = (*RootNode)["stream"];
					if (nullptr != Node_Stream && Node_Stream->IsString()){
						AudioRenderer::InputStream *lpAudioStream = InputStreamNameTable_Inverse.find(Node_Stream->GetString())->second;
						if (nullptr != lpAudioStream){
							float TmpFloat[3];

							auto Node_Position = (*RootNode)["position"];
							if (nullptr != Node_Position && Node_Position->IsArray()){
								if (nullptr != (*Node_Position)[(UBINT)0])TmpFloat[0] = (float)(*Node_Position)[(UBINT)0]->GetFloat(); else TmpFloat[0] = 0.0f;
								if (nullptr != (*Node_Position)[(UBINT)1])TmpFloat[1] = (float)(*Node_Position)[(UBINT)1]->GetFloat(); else TmpFloat[1] = 0.0f;
								if (nullptr != (*Node_Position)[(UBINT)2])TmpFloat[2] = (float)(*Node_Position)[(UBINT)2]->GetFloat(); else TmpFloat[2] = 0.0f;
							}
							else{
								TmpFloat[0] = 0.0f; TmpFloat[1] = 0.0f; TmpFloat[2] = 0.0f;
							}

							SoundCluster_Source *NewSoundCluster = MySoundClusterMgr->AddSource(1.0f);
							nsMath::Transform4_Pos(TmpFloat, NewSoundCluster->TransformMatrix);
							NewSoundCluster->StreamList.push_back(lpAudioStream);
							NewSoundCluster->pFilter = MyAudioRenderer->AddFilter();
							for (UBINT i = 0; i < BandCount; i++){
								NewSoundCluster->pLane[i] = NewSoundCluster->pFilter->AddLane(MySoundOutputChannel[i]);
							}
							NewSoundCluster->ClusterName = Node_Stream->GetString();

							//add an indicator
							nsScene::Sphere *NewSphere = nsScene::Sphere::Create();
							nsScene::MatrixTransform *SphereTransform = nsScene::MatrixTransform::Create();
							NewSphere->Radius = 0.1f;
							NewSphere->CastShadow = false;
							NewSphere->NormalProgram = &OpaquePhongProgram_Sphere_Inst;
							NewSphere->DepthProgram = &DepthProgram_Sphere_Inst;
							NewSphere->Data = MtlRoot->find("");

							nsMath::Transform4_Pos(TmpFloat, NewSoundCluster->TransformMatrix);
							SphereTransform->Matrix = NewSoundCluster->TransformMatrix;

							auto Node_Path = (*RootNode)["path"];
							if (nullptr != Node_Path && Node_Path->IsString()){
								auto _It = AnimationManager->AnimationMap.find(Node_Path->GetString());
								if (_It != AnimationManager->AnimationMap.end()){
									_It->second.pDestMatrix = NewSoundCluster->TransformMatrix;
									_It->second.pChanged = &NewSoundCluster->Change_Exist;
								}
							}

							nsBasic::IBase::Link(&SceneManager, SphereTransform, nullptr);
							nsBasic::IBase::Link(SphereTransform, NewSphere, nullptr);
						}
					}
				}
				RootNode->Destroy();
			}
			else if ("listener" == Symbol){
				nsJSON::JSONReader MyJSONReader;

				MyJSONReader.Generate_String();
				MyJSONReader.BeginRead(&MyLexer);
				auto RootNode = MyJSONReader.ParseNode().second;
				MyJSONReader.ParseAllChild();
				MyJSONReader.EndRead();
				if (RootNode->IsObject()){
					float TmpFloat[3], Volume;

					auto Node_Position = (*RootNode)["position"];
					if (nullptr != Node_Position && Node_Position->IsArray()){
						if (nullptr != (*Node_Position)[(UBINT)0])TmpFloat[0] = (float)(*Node_Position)[(UBINT)0]->GetFloat(); else TmpFloat[0] = 0.0f;
						if (nullptr != (*Node_Position)[(UBINT)1])TmpFloat[1] = (float)(*Node_Position)[(UBINT)1]->GetFloat(); else TmpFloat[1] = 0.0f;
						if (nullptr != (*Node_Position)[(UBINT)2])TmpFloat[2] = (float)(*Node_Position)[(UBINT)2]->GetFloat(); else TmpFloat[2] = 0.0f;
					}
					else{
						TmpFloat[0] = 0.0f; TmpFloat[1] = 0.0f; TmpFloat[2] = 0.0f;
					}
					MyCameraData.Position[0] = TmpFloat[0];
					MyCameraData.Position[1] = TmpFloat[1];
					MyCameraData.Position[2] = TmpFloat[2];

					auto Node_Volume = (*RootNode)["volume"];
					if (nullptr != Node_Volume && Node_Volume->IsFloat()){
						Volume = (float)Node_Volume->GetFloat();
					}
					else{
						Volume = 1.0f;
					}

					SoundCluster_Listener *pListener = MySoundClusterMgr->AddListener(nullptr, Volume);

					nsMath::Transform4_Pos(TmpFloat, pListener->TransformMatrix);
					CamTransform.Matrix = pListener->TransformMatrix;

					auto Node_Path = (*RootNode)["path"];
					if (nullptr != Node_Path && Node_Path->IsString()){
						auto _It = AnimationManager->AnimationMap.find(Node_Path->GetString());
						if (_It != AnimationManager->AnimationMap.end()){
							_It->second.pDestMatrix = pListener->TransformMatrix;
							_It->second.pChanged = &pListener->Change_Exist;
							pListenerAnimation = &_It->second;
						}
					}
				}
				RootNode->Destroy();
			}
			while ((!nsText::IsRetChar(MyLexer.LastChar)) && (UINT4b)-1 != MyLexer.LastChar)MyLexer.LoadNextChar(); //line skip
		}
	} while ((UINT4b)-1 != MyLexer.LastChar);

	nsBasic::IBase::Link(&SceneManager, &CamTransform, nullptr);
	nsBasic::IBase::Link(&CamTransform, &MyCamera, nullptr);

	return 0;
}
void ClearScene(){
	SceneManager.clear();
	if (nullptr != ResManager)ResManager->clear();
	if (nullptr != AnimationManager)AnimationManager->Clear();
	if (nullptr != MySoundClusterMgr)delete MySoundClusterMgr;
	if (nullptr != MyAudioRenderer)delete MyAudioRenderer;
	if (nullptr != pInputStreamNameTable)delete pInputStreamNameTable;
}