#include "SceneNode_General.hpp"
#include "SceneNode_Atom.hpp"

namespace nsScene{
	//class PointLight --BEGIN--

	class PointLight :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef PointLight this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		struct LightProgList{
			LightProgList *Next;
			UBINT BufferType;
			nsBasic::IBase *Ptr;
		} *ProgList;
		bool FrustumMask[6];
		UBINT VPTransformID[6], ShadowMapID[6], ResID[6], FrustumID;
		float Frustum[6];
	public:
		float Intensity[3];

		//implementation of interfaces
		//IBase
		static bool on_Link_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data);
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }
		static void on_UnLink_From(nsBasic::IBase * const ThisBase, IBase * const Dest, void *Data);

		//ISceneNode
		static void VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *QueryData);

		//IViewport
		static void Schedule(nsBasic::IBase * const SceneManager, nsRender::RenderScheduler * const lpScheduler, VPInfo * const ThisInfo, VPInfo ** RefVPInfo, UBINT RefVPCount);
		static UBINT Composite(nsBasic::IBase * const ThisBase, nsRender::RenderScheduler * const lpScheduler, UBINT ID_GBuffer, UBINT ID_VP, UBINT MatType);
		static void FrameEnd(nsBasic::IBase * const ThisBase);
	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const ISceneNode ISceneNode_Inst;
		static const IViewport IViewport_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(ISceneNode))return &this_type::ISceneNode_Inst;
			else if (IID == __typeid(IViewport))return &this_type::IViewport_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		PointLight(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->ProgList = nullptr;
			for (UBINT i = 0; i < 6; i++)this->ShadowMapID[i] = (UBINT)-1;
			for (UBINT i = 0; i < 6; i++)this->VPTransformID[i] = (UBINT)-1;
			for (UBINT i = 0; i < 6; i++)this->ResID[i] = (UBINT)-1;
			for (UBINT i = 0; i < 6; i++)this->FrustumMask[i] = false;
			this->FrustumID = (UBINT)-1;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			RetValue->Frustum[0] = -1.0f;
			RetValue->Frustum[1] = -1.0f;
			RetValue->Frustum[2] = 1.0f;
			RetValue->Frustum[3] = 1.0f;
			RetValue->Frustum[4] = 0.01f;
			RetValue->Frustum[5] = 100.0f;
			return RetValue;
		}

		//public methods
		void AdjustFrustum_Default(){
			this->Frustum[5] = sqrt(nsMath::dot<3, float>(this->Intensity, this->Intensity)) * 16.0f;
			if (this->Frustum[5] < 0.02f)this->Frustum[5] = 0.02f;
			this->Frustum[4] = this->Frustum[5] / 1000.0f;
			if (this->Frustum[4] > 0.01f)this->Frustum[4] = 0.01f;
		}
		void clear();

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		inline ~PointLight(){ this->clear(); }
	};

	const nsBasic::IBase PointLight::IBase_Inst = {
		PointLight::_GetInterface,
		PointLight::on_Link_From,
		PointLight::on_Link_Twrd,
		PointLight::on_UnLink_From,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		PointLight::Destroy
	};
	const ISceneNode PointLight::ISceneNode_Inst = {
		PointLight::VPQuery,
		nullptr
	};
	const IViewport PointLight::IViewport_Inst = {
		nullptr,
		PointLight::Schedule,
		PointLight::Composite,
		PointLight::FrameEnd
	};

	class AmbientLight :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef AmbientLight this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		struct LightProgList{
			LightProgList *Next;
			UBINT BufferType;
			nsBasic::IBase *Ptr;
		} *ProgList;
	public:
		float Intensity[3];
		bool UseAO;
		TableObj<RenderData> *Data;

		//implementation of interfaces
		//IBase
		static bool on_Link_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data);
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }
		static void on_UnLink_From(nsBasic::IBase * const ThisBase, IBase * const Dest, void *Data);

		//ISceneNode
		static void VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *QueryData);

		//IViewport
		static UBINT Composite(nsBasic::IBase * const ThisBase, nsRender::RenderScheduler * const lpScheduler, UBINT ID_GBuffer, UBINT ID_VP, UBINT MatType);
	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const ISceneNode ISceneNode_Inst;
		static const IViewport IViewport_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(ISceneNode))return &this_type::ISceneNode_Inst;
			else if (IID == __typeid(IViewport))return &this_type::IViewport_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		AmbientLight(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->ProgList = nullptr;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods
		void clear();

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		inline ~AmbientLight(){ this->clear(); }
	};

	const nsBasic::IBase AmbientLight::IBase_Inst = {
		AmbientLight::_GetInterface,
		AmbientLight::on_Link_From,
		AmbientLight::on_Link_Twrd,
		AmbientLight::on_UnLink_From,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		AmbientLight::Destroy
	};
	const ISceneNode AmbientLight::ISceneNode_Inst = {
		AmbientLight::VPQuery,
		nullptr
	};
	const IViewport AmbientLight::IViewport_Inst = {
		nullptr,
		nullptr,
		AmbientLight::Composite,
		nullptr
	};
	//class PointLight --END--

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	//class PointLight --BEGIN--

	bool PointLight::on_Link_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);

		const ILightProgram *I = Dest->GetInterface<ILightProgram>();
		if (nullptr != I){
			UBINT TypeID = I->InputType;
			LightProgList *TmpNode = This->ProgList;
			while (nullptr != TmpNode){
				if (TmpNode->BufferType == TypeID)return false;
			}

			LightProgList *NewNode = new LightProgList;
			if (nullptr == NewNode)return false;
			else{
				NewNode->Ptr = Dest;
				NewNode->BufferType = TypeID;
				NewNode->Next = This->ProgList;
				This->ProgList = NewNode;
				return true;
			}
		}
		else return false;
	}

	void PointLight::on_UnLink_From(nsBasic::IBase * const ThisBase, IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		LightProgList **TmpPtr = &This->ProgList;
		while (*TmpPtr != nullptr){
			if (Dest == (*TmpPtr)->Ptr){
				LightProgList *PtrToDelete = *TmpPtr;
				*TmpPtr = PtrToDelete->Next;
				delete PtrToDelete;
				return;
			}
			else TmpPtr = &((*TmpPtr)->Next);
		}
	}
	void PointLight::VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *QueryData){
		this_type *This = static_cast<this_type *>(ThisBase);

		if (QueryData->QueryObjType & QueryInfo::LIGHT){
			if (QueryType == __typeid(QueryInfoExt_View)){
				QueryInfoExt_View *QueryData_View = static_cast<QueryInfoExt_View *>(QueryData->ExtInfo);
				UBINT VisibleFrustum = 0;
				for (UBINT i = 0; i < 6; i++){
					float TmpTransform[16];
					nsMath::Transform4_CubeMap(QueryData->Transform, TmpTransform, i);
					if (true == nsMath::Intersect_View_Frustum_Coarse(QueryData_View->MatProj, This->Frustum, TmpTransform)){
						This->FrustumMask[i] = true;
						VisibleFrustum++;
					}
				}
				if (0 == VisibleFrustum)return;
			}
			else if (QueryType == __typeid(QueryInfoExt_All)){
				for (UBINT i = 0; i < 6; i++)This->FrustumMask[i] = true;
			}

			float *CamTransform = lpScheduler->AddReference(ThisBase);
			if (nullptr != CamTransform)memcpy(CamTransform, QueryData->Transform, 16 * sizeof(float));
		}
	}
	void PointLight::Schedule(nsBasic::IBase * const SceneManager, nsRender::RenderScheduler * const lpScheduler, VPInfo * const ThisInfo, VPInfo ** RefVPInfo, UBINT RefVPCount){
		this_type *This = static_cast<this_type *>(ThisInfo->Ptr);

		RenderList *ObjList = nullptr;

		for (UBINT i = 0; i < 6; i++){
			if (true == This->FrustumMask[i]){
				QueryInfo NewQuery;
				QueryInfoExt_View NewQueryExt;

				nsMath::identity4(NewQuery.Transform);
				NewQuery.QueryObjType = QueryInfo::OBJECT_SHADOW;
				NewQuery.QueryMask = nullptr;
				NewQuery.ExtInfo = &NewQueryExt;

				float TmpMat1[16], TmpMat2[16];
				nsMath::inv4(TmpMat1, ThisInfo->Transform);
				nsMath::Transform4_CubeMap_Inv(TmpMat1, TmpMat2, i);
				nsMath::Transform4_Perspective_Frustum(This->Frustum, TmpMat1);
				nsMath::mul4_mm(TmpMat1, TmpMat2, NewQueryExt.MatProj);

				const ISceneNode *I = SceneManager->GetInterface<ISceneNode>();
				INTERFACE_CALL(I, ObjectQuery)(SceneManager, &ObjList, __typeid(QueryInfoExt_View), &NewQuery);

				nsScene::Buffer_AtomList* CurBatch = nullptr;

				if (nullptr != ObjList){
					do{
						const IRenderAtom *J = ObjList->Ptr->GetInterface<IRenderAtom>();

						nsBasic::IBase *Material = J->GetRenderProgram(ObjList->Ptr, IRenderAtom::DEPTH_ONLY);
						TableObj<RenderData> *MatData = J->GetRenderProgramData(ObjList->Ptr, IRenderAtom::DEPTH_ONLY);
						const IRasterProgram *K = Material->GetInterface<IRasterProgram>();

						if (nullptr == CurBatch)CurBatch = nsScene::Buffer_AtomList::Create();
						CurBatch->Push(Material, MatData, ObjList);

						ObjList = ObjList->Next;
					} while (nullptr != ObjList);

					if (nullptr != CurBatch){
						CurBatch->Sort();
						UBINT LID = lpScheduler->CreateVar(CurBatch, __typeid(Buffer_AtomList), 0, 0);

						nsRender::VPMatrix *CurVP;
						UBINT VPID = lpScheduler->CreateVar_Ptr(&CurVP);
						nsMath::Transform4_CubeMap(ThisInfo->Transform, CurVP->MatLocal, i);
						nsMath::Transform4_Perspective_Frustum(This->Frustum, CurVP->MatProj);
						This->VPTransformID[i] = VPID;

						nsRender::RenderExpr *NewExpr = new nsRender::RenderExpr;
						NewExpr->ExprType = nsRender::RenderExpr::FUNCTION;
						NewExpr->OutputType = __typeid(nsRender::Buffer_Depth);
						NewExpr->Execute = RenderAtomList;

						This->ShadowMapID[i] = lpScheduler->CreateFunc(NewExpr, 0x200, 0x200); //512 * 512
						lpScheduler->PushParamExpr(VPID);
						lpScheduler->PushParamExpr(LID);
					}
				}
			}
		}
	}
	UBINT PointLight::Composite(nsBasic::IBase * const ThisBase, nsRender::RenderScheduler * const lpScheduler, UBINT ID_GBuffer, UBINT ID_VP, UBINT MatType){
		this_type *This = static_cast<this_type *>(ThisBase);

		//find the corresponding program
		LightProgList *TmpNode = This->ProgList;
		while (nullptr != TmpNode)if (TmpNode->BufferType == MatType)break;
		if (nullptr == TmpNode)return (UBINT)-1;

		UBINT PropID = (UBINT)-1;

		UBINT CompResult[6] = { (UBINT)-1, (UBINT)-1, (UBINT)-1, (UBINT)-1, (UBINT)-1, (UBINT)-1 };
		for (UBINT i = 0; i < 6; i++){
			if ((UBINT)-1 != This->ShadowMapID[i] && (UBINT)-1 != This->VPTransformID[i]){
				if ((UBINT)-1 == This->FrustumID){
					nsScene::Frustum *RenderFrustum = nsScene::Frustum::Create();
					memcpy(RenderFrustum->Data, This->Frustum, 6 * sizeof(float));
					This->FrustumID = lpScheduler->CreateVar(RenderFrustum, __typeid(nsScene::Frustum), 0, 0);
				}
				if (-1 == PropID){
					nsScene::VPLightProperty *CurLightProperty;
					PropID = lpScheduler->CreateVar_Ptr(&CurLightProperty);
					CurLightProperty->Intensity[0] = This->Intensity[0];
					CurLightProperty->Intensity[1] = This->Intensity[1];
					CurLightProperty->Intensity[2] = This->Intensity[2];
					CurLightProperty->Status = 0;
				}

				//compute the shadow mask
				UBINT MID = lpScheduler->CreateFunc(&ESMShadowCaster::Expr, lpScheduler->GetWidth(ID_GBuffer), lpScheduler->GetHeight(ID_GBuffer));
				lpScheduler->PushParamExpr(ID_VP);
				lpScheduler->PushParamExpr(This->VPTransformID[i]);
				lpScheduler->PushParamExpr(ID_GBuffer);
				lpScheduler->PushParamExpr(This->ShadowMapID[i]);
				lpScheduler->PushParamExpr(This->FrustumID);

				//compute the final result
				const nsRender::IRenderExpr *I = TmpNode->Ptr->GetInterface<nsRender::IRenderExpr>();
				CompResult[i] = lpScheduler->CreateFunc(I->lpExpr, lpScheduler->GetWidth(ID_GBuffer), lpScheduler->GetHeight(ID_GBuffer));
				lpScheduler->PushParamExpr(ID_VP);
				lpScheduler->PushParamExpr(This->VPTransformID[i]);
				lpScheduler->PushParamExpr(ID_GBuffer);
				lpScheduler->PushParamExpr(MID);
				lpScheduler->PushParamExpr(This->FrustumID);
				lpScheduler->PushParamExpr(PropID);
			}
		}
		UBINT RetValue = lpScheduler->CreateFunc(&DepthBlendProgram::Expr, lpScheduler->GetWidth(ID_GBuffer), lpScheduler->GetHeight(ID_GBuffer));
		for (UBINT i = 0; i < 6; i++){
			if ((UBINT)-1 != CompResult[i])lpScheduler->PushParamExpr(CompResult[i]);
		}
		return RetValue;
	}
	void PointLight::FrameEnd(nsBasic::IBase * const ThisBase){
		this_type *This = static_cast<this_type *>(ThisBase);
		for (UBINT i = 0; i < 6; i++){
			This->VPTransformID[i] = (UBINT)-1;
			This->ShadowMapID[i] = (UBINT)-1;
			This->ResID[i] = (UBINT)-1;
			This->FrustumMask[i] = false;
		}
		This->FrustumID = (UBINT)-1;
	}
	void PointLight::clear(){
		LightProgList *TmpPtr = this->ProgList;
		while (nullptr != TmpPtr){
			nsBasic::IBase::DerefAndDestroy(this, TmpPtr->Ptr, nullptr);

			LightProgList *NextPtr = TmpPtr->Next;
			delete TmpPtr;
			TmpPtr = NextPtr;
		}
		this->ProgList = nullptr;
	}

	//class PointLight --END--

	//class AmbientLight --BEGIN--

	bool AmbientLight::on_Link_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);

		const ILightProgram *I = Dest->GetInterface<ILightProgram>();
		if (nullptr != I){
			UBINT TypeID = I->InputType;
			LightProgList *TmpNode = This->ProgList;
			while (nullptr != TmpNode){
				if (TmpNode->BufferType == TypeID)return false;
			}

			LightProgList *NewNode = new LightProgList;
			if (nullptr == NewNode)return false;
			else{
				NewNode->Ptr = Dest;
				NewNode->BufferType = TypeID;
				NewNode->Next = This->ProgList;
				This->ProgList = NewNode;
				return true;
			}
		}
		else return false;
	}

	void AmbientLight::on_UnLink_From(nsBasic::IBase * const ThisBase, IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		LightProgList **TmpPtr = &This->ProgList;
		while (*TmpPtr != nullptr){
			if (Dest == (*TmpPtr)->Ptr){
				LightProgList *PtrToDelete = *TmpPtr;
				*TmpPtr = PtrToDelete->Next;
				delete PtrToDelete;
				return;
			}
			else TmpPtr = &((*TmpPtr)->Next);
		}
	}
	void AmbientLight::VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *QueryData){
		if (QueryData->QueryObjType & QueryInfo::LIGHT){
			float *CamTransform = lpScheduler->AddReference(ThisBase);
			if (nullptr != CamTransform)memcpy(CamTransform, QueryData->Transform, 16 * sizeof(float));
		}
	}
	UBINT AmbientLight::Composite(nsBasic::IBase * const ThisBase, nsRender::RenderScheduler * const lpScheduler, UBINT ID_GBuffer, UBINT ID_VP, UBINT MatType){
		this_type *This = static_cast<this_type *>(ThisBase);

		//find the corresponding program
		LightProgList *TmpNode = This->ProgList;
		while (nullptr != TmpNode)if (TmpNode->BufferType == MatType)break;
		if (nullptr == TmpNode)return (UBINT)-1;

		nsScene::AmbientLightProperty *CurLightProperty;
		UBINT PropID = lpScheduler->CreateVar_Ptr(&CurLightProperty);
		CurLightProperty->Intensity[0] = This->Intensity[0];
		CurLightProperty->Intensity[1] = This->Intensity[1];
		CurLightProperty->Intensity[2] = This->Intensity[2];
		CurLightProperty->Status = 0;

		UBINT MID;
		if (This->UseAO){
			CurLightProperty->Status += AmbientLightProperty::AO_MASK_EXIST;

			//compute the ambient occlusion
			MID = lpScheduler->CreateFunc(&SSAOShadowCaster::Expr, lpScheduler->GetWidth(ID_GBuffer), lpScheduler->GetHeight(ID_GBuffer));
			lpScheduler->PushParamExpr(ID_VP);
			lpScheduler->PushParamExpr(ID_GBuffer);
		}
		else MID = ID_GBuffer;

		//compute the final result
		const nsRender::IRenderExpr *I = TmpNode->Ptr->GetInterface<nsRender::IRenderExpr>();
		UBINT CompResult = lpScheduler->CreateFunc(I->lpExpr, lpScheduler->GetWidth(ID_GBuffer), lpScheduler->GetHeight(ID_GBuffer));
		lpScheduler->PushParamExpr(ID_GBuffer);
		lpScheduler->PushParamExpr(MID);
		lpScheduler->PushParamExpr(PropID);

		return CompResult;
	}
	void AmbientLight::clear(){
		LightProgList *TmpPtr = this->ProgList;
		while (nullptr != TmpPtr){
			nsBasic::IBase::DerefAndDestroy(this, TmpPtr->Ptr, nullptr);

			LightProgList *NextPtr = TmpPtr->Next;
			delete TmpPtr;
			TmpPtr = NextPtr;
		}
		this->ProgList = nullptr;
	}

	//class AmbientLight --END--
}