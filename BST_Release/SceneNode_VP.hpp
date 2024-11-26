#include "SceneNode_Atom.hpp"
#include "RenderExpr.hpp"

namespace nsScene{
	void RenderAtomList(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount){
		nsRender::VPMatrix *CurVP = static_cast<nsRender::VPMatrix *>(ArgList[0]->Pointer);
		nsScene::Buffer_AtomList *CurBatch = static_cast<nsScene::Buffer_AtomList *>(ArgList[1]->Buffer);

		const nsRender::IBuffer *I = Output->Buffer->GetInterface<nsRender::IBuffer>();
		const IRasterProgram *J = nullptr;
		INTERFACE_CALL(I, SetAsOutput)(Output->Buffer);

		GLfloat MatInv[16];
		nsMath::inv4(MatInv, CurVP->MatLocal);

		nsBasic::IBase *CurMat = nullptr, *TmpMat;
		TableObj<RenderData> *CurData = nullptr, *TmpData;
		RenderList *ObjList;
		while (CurBatch->Pop(&TmpMat, &TmpData, &ObjList)){
			if (CurMat != TmpMat){
				if (nullptr != J)INTERFACE_CALL(J, Finalize)(CurMat, lpDevice);
				CurMat = TmpMat;
				J = CurMat->GetInterface<IRasterProgram>();
				INTERFACE_CALL(J, Initialize)(CurMat, lpDevice);
				INTERFACE_CALL(J, SetSize)(CurMat, lpDevice, Output->BufferWidth, Output->BufferHeight);
				CurData = nullptr;
			}
			if (CurData != TmpData){
				if (nullptr != CurData)INTERFACE_CALL(J, PostRender)(CurMat, lpDevice);
				CurData = TmpData;
				INTERFACE_CALL(J, LoadData)(CurMat, lpDevice, CurData);
				INTERFACE_CALL(J, PreRender)(CurMat, lpDevice);
			}

			GLfloat MatWorldTmp[16];
			nsMath::mul4_mm(MatInv, ObjList->Transform, MatWorldTmp);
			const IRenderAtom *K = ObjList->Ptr->GetInterface<IRenderAtom>();
			INTERFACE_CALL(J, SetTransform)(CurMat, lpDevice, MatWorldTmp, CurVP->MatProj);
			INTERFACE_CALL(J, SetTransform)(CurMat, lpDevice, MatWorldTmp, CurVP->MatProj);

			INTERFACE_CALL(K, Render)(ObjList->Ptr, lpDevice, 0);

		}
		if (nullptr != J){
			if (nullptr != CurData)INTERFACE_CALL(J, PostRender)(CurMat, lpDevice);
			INTERFACE_CALL(J, Finalize)(CurMat, lpDevice);
		}
	}

	//class Camera_P --BEGIN--

	class Camera_P :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Camera_P this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

	public:
		UBINT Size[2];
		float Frustum[6];

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//ISceneNode
		static void VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *QueryData);

		//IRenderVP
		static void VPFetch(nsBasic::IBase * const ThisBase, nsBasic::IBase * const SceneManager, VPScheduler * const lpScheduler, float *Transform);
		static void Schedule(nsBasic::IBase * const SceneManager, nsRender::RenderScheduler * const lpScheduler, VPInfo * const ThisInfo, VPInfo ** RefVPInfo, UBINT RefVPCount);

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
		Camera_P(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~Camera_P(){}
	};

	const nsBasic::IBase Camera_P::IBase_Inst = {
		Camera_P::_GetInterface,
		nullptr,
		Camera_P::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Camera_P::Destroy
	};
	const ISceneNode Camera_P::ISceneNode_Inst = {
		Camera_P::VPQuery,
		nullptr
	};
	const IViewport Camera_P::IViewport_Inst = {
		Camera_P::VPFetch,
		Camera_P::Schedule,
		nullptr,
		nullptr
	};

	//class Camera_P --END--

	//class Camera_O --BEGIN--

	class Camera_O :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Camera_O this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

	public:
		UBINT Size[2];
		float ViewBox[6];

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//ISceneNode
		static void VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *QueryData);

		//IRenderVP
		static void VPFetch(nsBasic::IBase * const ThisBase, nsBasic::IBase * const SceneManager, VPScheduler * const lpScheduler, float *Transform);
		static void Schedule(nsBasic::IBase * const SceneManager, nsRender::RenderScheduler * const lpScheduler, VPInfo * const ThisInfo, VPInfo ** RefVPInfo, UBINT RefVPCount);

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
		Camera_O(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~Camera_O(){}
	};

	const nsBasic::IBase Camera_O::IBase_Inst = {
		Camera_O::_GetInterface,
		nullptr,
		Camera_O::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Camera_O::Destroy
	};
	const ISceneNode Camera_O::ISceneNode_Inst = {
		Camera_O::VPQuery,
		nullptr
	};
	const IViewport Camera_O::IViewport_Inst = {
		Camera_O::VPFetch,
		Camera_O::Schedule,
		nullptr,
		nullptr
	};

	//class Camera_O --END--

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	//class Camera_P --BEGIN--

	void Camera_P::VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *QueryData){
		if (QueryData->QueryObjType & QueryInfo::OUTPUT){
			float *CamTransform = lpScheduler->AddReference(ThisBase);
			if (nullptr != CamTransform)memcpy(CamTransform, QueryData->Transform, 16 * sizeof(float));
		}
	}
	void Camera_P::VPFetch(nsBasic::IBase * const ThisBase, nsBasic::IBase * const SceneManager, VPScheduler * const lpScheduler, float *Transform){
		this_type *This = static_cast<this_type *>(ThisBase);

		QueryInfo NewQuery;
		QueryInfoExt_View NewQueryExt;
		nsMath::identity4(NewQuery.Transform);
		NewQuery.QueryMask = nullptr;
		NewQuery.QueryObjType = QueryInfo::CAMERA | QueryInfo::LIGHT;
		NewQuery.ExtInfo = &NewQueryExt;

		float TmpMat1[16], TmpMat2[16];
		nsMath::inv4(TmpMat1, Transform);
		nsMath::Transform4_Perspective_Frustum(This->Frustum, TmpMat2);
		nsMath::mul4_mm(TmpMat2, TmpMat1, NewQueryExt.MatProj);

		const ISceneNode *I = SceneManager->GetInterface<ISceneNode>();
		INTERFACE_CALL(I, VPQuery)(SceneManager, lpScheduler, __typeid(QueryInfoExt_View), &NewQuery);
	}
	void Camera_P::Schedule(nsBasic::IBase * const SceneManager, nsRender::RenderScheduler * const lpScheduler, VPInfo * const ThisInfo, VPInfo ** RefVPInfo, UBINT RefVPCount){
		this_type *This = static_cast<this_type *>(ThisInfo->Ptr);

		RenderList *ObjList = nullptr;

		QueryInfo NewQuery;
		QueryInfoExt_View NewQueryExt;
		nsMath::identity4(NewQuery.Transform);
		NewQuery.QueryObjType = QueryInfo::OBJECT_GEOMETRY;
		NewQuery.QueryMask = nullptr;
		NewQuery.ExtInfo = &NewQueryExt;

		float TmpMat1[16], TmpMat2[16];
		nsMath::inv4(TmpMat1, ThisInfo->Transform);
		nsMath::Transform4_Perspective_Frustum(This->Frustum, TmpMat2);
		nsMath::mul4_mm(TmpMat2, TmpMat1, NewQueryExt.MatProj);

		const ISceneNode *I = SceneManager->GetInterface<ISceneNode>();
		INTERFACE_CALL(I, ObjectQuery)(SceneManager, &ObjList, __typeid(QueryInfoExt_View), &NewQuery);

		nsContainer::HashMap<UBINT, nsScene::Buffer_AtomList *> MatHashMap;

		if (nullptr != ObjList){
			nsRender::VPMatrix *CurVP;
			UBINT VPID = lpScheduler->CreateVar_Ptr(&CurVP);
			memcpy(CurVP->MatLocal, ThisInfo->Transform, 16 * sizeof(GLfloat));
			memcpy(CurVP->MatProj, TmpMat2, 16 * sizeof(GLfloat));

			do{
				const IRenderAtom *J = ObjList->Ptr->GetInterface<IRenderAtom>();

				nsBasic::IBase *Material = J->GetRenderProgram(ObjList->Ptr, IRenderAtom::GBUFFER);
				TableObj<RenderData> *MatData = J->GetRenderProgramData(ObjList->Ptr, IRenderAtom::GBUFFER);
				const IRasterProgram *K = Material->GetInterface<IRasterProgram>();

				nsScene::Buffer_AtomList *CurBatch;
				auto _It = MatHashMap.find(K->OutputType);
				if (MatHashMap.end() == _It){
					CurBatch = nsScene::Buffer_AtomList::Create();
					MatHashMap.try_emplace(K->OutputType, CurBatch);
				}
				else CurBatch = _It->second;
				CurBatch->Push(Material, MatData, ObjList);

				ObjList = ObjList->Next;
			}while (nullptr != ObjList);

			nsContainer::Vector<UBINT> LightRetValStack;

			//for all materials
			//render
			auto MyIterator = MatHashMap.begin();
			while (MyIterator != MatHashMap.end()){
				nsScene::Buffer_AtomList* CurBatch = MyIterator->second;
				CurBatch->Sort();
				UBINT LID = lpScheduler->CreateVar(CurBatch, __typeid(nsScene::Buffer_AtomList), 0, 0);

				nsRender::RenderExpr *NewExpr = new nsRender::RenderExpr;
				NewExpr->ExprType = nsRender::RenderExpr::FUNCTION;
				NewExpr->OutputType = MyIterator->first;
				NewExpr->Execute = RenderAtomList;

				//render
				UBINT RBID = lpScheduler->CreateFunc(NewExpr, This->Size[0], This->Size[1]);

				lpScheduler->PushParamExpr(VPID);
				lpScheduler->PushParamExpr(LID);
				for (UBINT i = 0; i < RefVPCount; i++){
					if ((UBINT)-1 != RefVPInfo[i]->RetValueID)lpScheduler->PushParamExpr(RefVPInfo[i]->RetValueID);
				}

				//composite with light
				for (UBINT i = 0; i < RefVPCount; i++){
					const IViewport *J = RefVPInfo[i]->Ptr->GetInterface<IViewport>();
					if (J->Composite){
						UBINT ResultID = J->Composite(RefVPInfo[i]->Ptr, lpScheduler, RBID, VPID, MyIterator->first);
						if ((UBINT)-1 != ResultID)LightRetValStack.push_back(ResultID);
					}
				}
				++MyIterator;
			}

			UBINT BlendResult = lpScheduler->CreateFunc(&DepthBlendProgram::Expr, This->Size[0], This->Size[1]);
			for (auto StackIt = LightRetValStack.begin(); StackIt != LightRetValStack.end(); StackIt++)lpScheduler->PushParamExpr(*StackIt);
			lpScheduler->CreateFunc(&ScrOutputProgram::Expr, This->Size[0], This->Size[1]);
			lpScheduler->PushParamExpr(BlendResult);
		}
	}

	//class Camera_P --END--

	//class Camera_O --BEGIN--

	void Camera_O::VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *QueryData){
		if (QueryData->QueryObjType & QueryInfo::OUTPUT){
			float *CamTransform = lpScheduler->AddReference(ThisBase);
			if (nullptr != CamTransform)memcpy(CamTransform, QueryData->Transform, 16 * sizeof(float));
		}
	}
	void Camera_O::VPFetch(nsBasic::IBase * const ThisBase, nsBasic::IBase * const SceneManager, VPScheduler * const lpScheduler, float *Transform){
		this_type *This = static_cast<this_type *>(ThisBase);

		QueryInfo NewQuery;
		QueryInfoExt_View NewQueryExt;
		nsMath::identity4(NewQuery.Transform);
		NewQuery.QueryMask = nullptr;
		NewQuery.QueryObjType = QueryInfo::CAMERA | QueryInfo::LIGHT;
		NewQuery.ExtInfo = &NewQueryExt;

		float TmpMat1[16], TmpMat2[16];
		nsMath::inv4(TmpMat1, Transform);
		nsMath::Transform4_Ortho_ViewBox(This->ViewBox, TmpMat2);
		nsMath::mul4_mm(TmpMat2, TmpMat1, NewQueryExt.MatProj);

		const ISceneNode *I = SceneManager->GetInterface<ISceneNode>();
		INTERFACE_CALL(I, VPQuery)(SceneManager, lpScheduler, __typeid(QueryInfoExt_View), &NewQuery);
	}
	void Camera_O::Schedule(nsBasic::IBase * const SceneManager, nsRender::RenderScheduler * const lpScheduler, VPInfo * const ThisInfo, VPInfo ** RefVPInfo, UBINT RefVPCount){
		this_type *This = static_cast<this_type *>(ThisInfo->Ptr);

		RenderList *ObjList = nullptr;

		QueryInfo NewQuery;
		QueryInfoExt_View NewQueryExt;
		nsMath::identity4(NewQuery.Transform);
		NewQuery.QueryObjType = QueryInfo::OBJECT_GEOMETRY;
		NewQuery.QueryMask = nullptr;
		NewQuery.ExtInfo = &NewQueryExt;

		float TmpMat1[16], TmpMat2[16];
		nsMath::inv4(TmpMat1, ThisInfo->Transform);
		nsMath::Transform4_Ortho_ViewBox(This->ViewBox, TmpMat2);
		nsMath::mul4_mm(TmpMat2, TmpMat1, NewQueryExt.MatProj);

		const ISceneNode *I = SceneManager->GetInterface<ISceneNode>();
		INTERFACE_CALL(I, ObjectQuery)(SceneManager, &ObjList, __typeid(QueryInfoExt_View), &NewQuery);

		nsContainer::HashMap<UBINT, nsScene::Buffer_AtomList *> MatHashMap;

		if (nullptr != ObjList){
			nsRender::VPMatrix *CurVP;
			UBINT VPID = lpScheduler->CreateVar_Ptr(&CurVP);
			memcpy(CurVP->MatLocal, ThisInfo->Transform, 16 * sizeof(GLfloat));
			memcpy(CurVP->MatProj, TmpMat2, 16 * sizeof(GLfloat));

			do{
				const IRenderAtom *J = ObjList->Ptr->GetInterface<IRenderAtom>();

				nsBasic::IBase *Material = J->GetRenderProgram(ObjList->Ptr, IRenderAtom::GBUFFER);
				TableObj<RenderData> *MatData = J->GetRenderProgramData(ObjList->Ptr, IRenderAtom::GBUFFER);
				const IRasterProgram *K = Material->GetInterface<IRasterProgram>();

				nsScene::Buffer_AtomList *CurBatch;
				auto _It = MatHashMap.find(K->OutputType);
				if (MatHashMap.end() == _It){
					CurBatch = nsScene::Buffer_AtomList::Create();
					MatHashMap.try_emplace(K->OutputType, CurBatch);
				}
				else CurBatch = _It->second;
				CurBatch->Push(Material, MatData, ObjList);

				ObjList = ObjList->Next;
			} while (nullptr != ObjList);

			nsContainer::Vector<UBINT> LightRetValStack;

			//for all materials
			//render
			auto MyIterator = MatHashMap.begin();
			while (MyIterator != MatHashMap.end()){
				nsScene::Buffer_AtomList* CurBatch = MyIterator->second;
				CurBatch->Sort();
				UBINT LID = lpScheduler->CreateVar(CurBatch, __typeid(nsScene::Buffer_AtomList), 0, 0);

				nsRender::RenderExpr *NewExpr = new nsRender::RenderExpr;
				NewExpr->ExprType = nsRender::RenderExpr::FUNCTION;
				NewExpr->OutputType = MyIterator->first;
				NewExpr->Execute = RenderAtomList;

				//render
				UBINT RBID = lpScheduler->CreateFunc(NewExpr, This->Size[0], This->Size[1]);

				lpScheduler->PushParamExpr(VPID);
				lpScheduler->PushParamExpr(LID);
				for (UBINT i = 0; i < RefVPCount; i++){
					if ((UBINT)-1 != RefVPInfo[i]->RetValueID)lpScheduler->PushParamExpr(RefVPInfo[i]->RetValueID);
				}

				//composite with light
				for (UBINT i = 0; i < RefVPCount; i++){
					const IViewport *J = RefVPInfo[i]->Ptr->GetInterface<IViewport>();
					if (J->Composite){
						UBINT ResultID = J->Composite(RefVPInfo[i]->Ptr, lpScheduler, RBID, VPID, MyIterator->first);
						if ((UBINT)-1 != ResultID)LightRetValStack.push_back(ResultID);
					}
				}
				++MyIterator;
			}

			UBINT BlendResult = lpScheduler->CreateFunc(&DepthBlendProgram::Expr, This->Size[0], This->Size[1]);
			for (auto StackIt = LightRetValStack.begin(); StackIt != LightRetValStack.end(); StackIt++)lpScheduler->PushParamExpr(*StackIt);
			lpScheduler->CreateFunc(&ScrOutputProgram::Expr, This->Size[0], This->Size[1]);
			lpScheduler->PushParamExpr(BlendResult);
		}
	}

	//class Camera_O --END--
}