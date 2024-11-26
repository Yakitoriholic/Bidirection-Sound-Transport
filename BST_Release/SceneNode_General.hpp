#ifndef LIB_SCENENODE_GENERAL
#define LIB_SCENENODE_GENERAL

#include "lGeneral.hpp"
#include "glShaders.hpp"
#include "Resmgr_Static.hpp"
#include "Resmgr_Dynamic.hpp"

namespace nsScene{
	class VPScheduler;

	struct IBaseList{
		IBaseList *Next;
		nsBasic::IBase *Ptr;
	};

	// class RenderBufferList --BEGIN--
	struct RenderBufferList{
		RenderBufferList *Next;
		UBINT TypeID;
		nsBasic::IBase *Ptr;

		static RenderBufferList *Find(RenderBufferList *ListHead, UBINT TypeID){
			while (nullptr != ListHead){
				if (ListHead->TypeID == TypeID)return ListHead;
				ListHead = ListHead->Next;
			}
			return nullptr;
		}
	};
	// class RenderBufferList --END--

	struct RenderList{
		RenderList *Next;
		nsBasic::IBase *Ptr;
		float Transform[16];
	};

	//Queries
	struct QueryInfo{
		float Transform[16];
		UBINT QueryObjType;
		IBaseList *QueryMask;
		void *ExtInfo;

		enum QueryObjTypeEnum{
			OUTPUT = 1 << 0,
			CAMERA = 1 << 1,
			LIGHT = 1 << 2,
			OBJECT_GEOMETRY = 1 << 3,
			OBJECT_SHADOW = 1 << 4
		};
	};
	struct QueryInfoExt_All {};
	struct QueryInfoExt_View{
		float MatProj[16];
	};

	struct VPInfo{
		nsBasic::IBase *Ptr;
		UBINT RetValueID;
		float Transform[16];
	};

	//Interfaces
	struct ISceneNode{
		void(*VPQuery)(nsBasic::IBase * const, VPScheduler * const, UBINT, QueryInfo *);
		void(*ObjectQuery)(nsBasic::IBase * const, RenderList **, UBINT, QueryInfo *);
	};

	struct IGeometry{
		void(*Render)(nsBasic::IBase * const, nsGUI::GLDevice *lpDevice, UBINT);
	};

	struct IViewport{
		void(*VPFetch)(nsBasic::IBase * const, nsBasic::IBase * const, VPScheduler * const, float *);
		void(*Schedule)(nsBasic::IBase * const, nsRender::RenderScheduler * const, VPInfo * const, VPInfo **, UBINT);
		UBINT(*Composite)(nsBasic::IBase * const, nsRender::RenderScheduler * const, UBINT, UBINT, UBINT); //return (UBINT)-1 for nothing is done.
		void(*FrameEnd)(nsBasic::IBase * const);
	};

	struct IRenderLight{
		void(*Schedule_Render)(nsBasic::IBase * const, nsBasic::IBase * const, nsRender::RenderScheduler * const, float *);
		void(*Schedule_Composite)(nsBasic::IBase * const, UBINT, nsBasic::IBase *, float *, float *, float*);
	};

	struct IRasterProgram{
		UBINT OutputType;
		void(*Initialize)(nsBasic::IBase * const, nsGUI::GLDevice *);
		void(*SetSize)(nsBasic::IBase * const, nsGUI::GLDevice *, UBINT, UBINT);
		void(*SetTransform)(nsBasic::IBase * const, nsGUI::GLDevice *, float *, float *);
		void(*LoadData)(nsBasic::IBase * const, nsGUI::GLDevice *, TableObj<RenderData> *);
		void(*PreRender)(nsBasic::IBase * const, nsGUI::GLDevice *);
		void(*PostRender)(nsBasic::IBase * const, nsGUI::GLDevice *);
		void(*Finalize)(nsBasic::IBase * const, nsGUI::GLDevice *);
	};

	struct ILightProgram{
		UBINT InputType;
	};

	class VPScheduler{
	private:
		struct VPInfoListNode :public VPInfo{
			VPInfoListNode *Prev;
			VPInfoListNode *Next;
			UBINT RefVPArrID;
			UBINT RefVPCnt; //ref to others
			UBINT RefedCnt; //ref from others
		};

		VPInfoListNode *CurVP;
		VPInfoListNode *UnRefedVPList, *RefedVPList;
		nsContainer::HashMap<nsBasic::IBase *, VPInfoListNode *> VPMap;
		nsContainer::Vector<VPInfoListNode *>VPRefArr, VP_Sorted;

	public:
		void Initialize(nsBasic::IBase * const SceneRoot){
			nsScene::QueryInfo NewQuery;
			nsMath::identity4(NewQuery.Transform);
			NewQuery.QueryObjType = nsScene::QueryInfo::OUTPUT;
			NewQuery.QueryMask = nullptr;
			NewQuery.ExtInfo = nullptr;

			const nsScene::ISceneNode *I = SceneRoot->GetInterface<nsScene::ISceneNode>();
			INTERFACE_CALL(I, VPQuery)(SceneRoot, this, __typeid(nsScene::QueryInfoExt_All), &NewQuery);
		}
		bool NextVPQuery(nsBasic::IBase * const SceneRoot){
			if (nullptr == CurVP){
				if (nullptr == UnRefedVPList)return false;
				else CurVP = UnRefedVPList->Prev;
			}
			else if (CurVP == UnRefedVPList){
				if (nullptr == RefedVPList)return false;
				else CurVP = RefedVPList->Prev;
			}
			else if (CurVP == RefedVPList)return false;
			const nsScene::IViewport *I = CurVP->Ptr->GetInterface<nsScene::IViewport>();
			INTERFACE_CALL(I, VPFetch)(CurVP->Ptr, SceneRoot, this, CurVP->Transform);
			CurVP = CurVP->Prev;
			return true;
		}
		float *AddReference(nsBasic::IBase * const RefVP){
			//returns a pointer to the transform matrix waiting to be filled. a nullptr is returned for failure.
			auto _It = VPMap.find(RefVP);

			VPInfoListNode *RefNode;
			float *RetValue;
			if (VPMap.end() == _It){
				//create a new one
				RefNode = new VPInfoListNode;

				//attach to cyclic linked list
				if (nullptr == CurVP){
					if (nullptr == this->UnRefedVPList){
						RefNode->Prev = RefNode;
						RefNode->Next = RefNode;
					}
					else{
						RefNode->Prev = this->UnRefedVPList->Prev;
						RefNode->Next = this->UnRefedVPList;
						RefNode->Prev->Next = RefNode;
						RefNode->Next->Prev = RefNode;
					}
					this->UnRefedVPList = RefNode;
					RefNode->RefedCnt = 0;
				}
				else{
					if (nullptr == this->RefedVPList){
						RefNode->Prev = RefNode;
						RefNode->Next = RefNode;
					}
					else{
						RefNode->Prev = this->RefedVPList->Prev;
						RefNode->Next = this->RefedVPList;
						RefNode->Prev->Next = RefNode;
						RefNode->Next->Prev = RefNode;
					}
					this->RefedVPList = RefNode;
					RefNode->RefedCnt = 1;
				}

				RefNode->RefVPCnt = 0;
				RefNode->RefVPArrID = 0;
				RefNode->Ptr = RefVP;
				RefNode->RetValueID = (UBINT)-1;

				VPMap.try_emplace(RefVP, RefNode);
				RetValue = RefNode->Transform;
			}
			else{
				RefNode = _It->second;
				RefNode->RefedCnt++;
				RetValue = nullptr;
			}
			if (nullptr != CurVP){
				if (0 == CurVP->RefVPCnt)CurVP->RefVPArrID = VPRefArr.size();
				CurVP->RefVPCnt++;
				VPRefArr.push_back(RefNode);
			}
			return RetValue;
		}
		void Sort(){
			if (nullptr != this->UnRefedVPList){
				VPInfoListNode *ProcessedVPList = nullptr;

				//break the link between the last node and the first node
				this->UnRefedVPList->Prev->Next = nullptr;
				this->UnRefedVPList->Prev = nullptr;

				do{
					VPInfoListNode *CurNode = this->UnRefedVPList;
					this->UnRefedVPList = CurNode->Next;
					
					//attach to cyclic linked list
					if (nullptr == ProcessedVPList){
						CurNode->Prev = CurNode;
						CurNode->Next = CurNode;
					}
					else{
						CurNode->Prev = ProcessedVPList->Prev;
						CurNode->Next = ProcessedVPList;
						CurNode->Prev->Next = CurNode;
						CurNode->Next->Prev = CurNode;
					}
					ProcessedVPList = CurNode;

					this->VP_Sorted.push_back(CurNode);
					for (UBINT i = CurNode->RefVPArrID; i < CurNode->RefVPArrID + CurNode->RefVPCnt; i++){
						VPInfoListNode *TgtNode = this->VPRefArr[i];
						if (0 == --TgtNode->RefedCnt){
							//attach to un-refed list
							TgtNode->Prev = nullptr;
							TgtNode->Next = this->UnRefedVPList;
							if (nullptr != this->UnRefedVPList)this->UnRefedVPList->Prev = TgtNode;
							this->UnRefedVPList = TgtNode;
						}
					}
				} while (this->UnRefedVPList != nullptr);

				this->UnRefedVPList = ProcessedVPList;
				this->RefedVPList = nullptr;
			}
		}
		void Execute(nsBasic::IBase * const SceneRoot, nsRender::RenderScheduler *lpScheduler){
			auto i = this->VP_Sorted.end();
			while (i != this->VP_Sorted.begin()){
				i--;
				const nsScene::IViewport *I = (*i)->Ptr->GetInterface<nsScene::IViewport>();
				INTERFACE_CALL(I, Schedule)(SceneRoot, lpScheduler, *i, (VPInfo **)&this->VPRefArr[(*i)->RefVPArrID]/* usually a correct cast*/, (*i)->RefVPCnt);
			}
		}
		void FrameEnd(){
			auto i = this->VP_Sorted.end();
			while (i != this->VP_Sorted.begin()){
				i--;
				const nsScene::IViewport *I = (*i)->Ptr->GetInterface<nsScene::IViewport>();
				INTERFACE_CALL(I, FrameEnd)((*i)->Ptr);
			}
		}
		void Clear(){
			this->CurVP = nullptr;

			VPInfoListNode *CurNode = this->UnRefedVPList;
			if (nullptr != CurNode){
				do{
					VPInfoListNode *TmpNode = CurNode->Next;
					delete CurNode;
					CurNode = TmpNode;
				} while (CurNode != this->UnRefedVPList);
			}
			this->UnRefedVPList = nullptr;

			CurNode = this->RefedVPList;
			if (nullptr != CurNode){
				do{
					VPInfoListNode *TmpNode = CurNode->Next;
					delete CurNode;
					CurNode = TmpNode;
				} while (CurNode != this->RefedVPList);
			}
			this->RefedVPList = nullptr;

			this->VPMap.clear();
			this->VPRefArr.clear();
			this->VP_Sorted.clear();
		}
		VPScheduler() :VPMap(), VPRefArr(), VP_Sorted(){
			this->CurVP = nullptr;
			this->UnRefedVPList = nullptr;
			this->RefedVPList = nullptr;
		}
		~VPScheduler(){ this->Clear(); }
	};
}
#endif