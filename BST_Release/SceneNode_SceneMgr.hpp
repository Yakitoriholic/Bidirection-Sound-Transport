#include "SceneNode_General.hpp"

namespace nsScene{

	//class SimpleSceneManager --BEGIN--

	class SimpleSceneManager :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef SimpleSceneManager this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		IBaseList *ObjList;
	public:
		//implementation of interfaces
		//IBase
		static bool on_Link_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data);
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Src, void *Data){ return true; }
		static void on_UnLink_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data);

		//ISceneNode
		static void VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *Info);
		static void ObjectQuery(nsBasic::IBase * const ThisBase, nsScene::RenderList **lpRList, UBINT QueryType, QueryInfo *Info);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const ISceneNode ISceneNode_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(ISceneNode))return &this_type::ISceneNode_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		SimpleSceneManager(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->ObjList = nullptr;
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
		inline ~SimpleSceneManager(){ this->clear(); }
	};

	const nsBasic::IBase SimpleSceneManager::IBase_Inst = {
		SimpleSceneManager::_GetInterface,
		SimpleSceneManager::on_Link_From,
		SimpleSceneManager::on_Link_Twrd,
		SimpleSceneManager::on_UnLink_From,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		SimpleSceneManager::Destroy
	};
	const ISceneNode SimpleSceneManager::ISceneNode_Inst = {
		SimpleSceneManager::VPQuery,
		SimpleSceneManager::ObjectQuery
	};

	//class SimpleSceneManager --END--

	//class MatrixTransform --BEGIN--

	class MatrixTransform :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef MatrixTransform this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		nsBasic::IBase *Child;
	public:
		float _MatrixData[16];
		float *Matrix; // By default this pointer points to _MatrixData, but it can be modified.

		//implementation of interfaces
		//IBase
		static bool on_Link_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data);
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }
		static void on_UnLink_From(nsBasic::IBase * const ThisBase, IBase * const Dest, void *Data);

		//ISceneNode
		static void VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *Info);
		static void ObjectQuery(nsBasic::IBase * const ThisBase, nsScene::RenderList **lpRList, UBINT QueryType, QueryInfo *Info);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const ISceneNode ISceneNode_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(ISceneNode))return &this_type::ISceneNode_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		MatrixTransform(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->Matrix = this->_MatrixData;
			this->Child = nullptr;
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
		inline ~MatrixTransform(){ this->clear(); }
	};

	const nsBasic::IBase MatrixTransform::IBase_Inst = {
		MatrixTransform::_GetInterface,
		MatrixTransform::on_Link_From,
		MatrixTransform::on_Link_Twrd,
		MatrixTransform::on_UnLink_From,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		MatrixTransform::Destroy
	};
	const ISceneNode MatrixTransform::ISceneNode_Inst = {
		MatrixTransform::VPQuery,
		MatrixTransform::ObjectQuery,
	};

	//class MatrixTransform --END--

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	//class SimpleSceneManager --BEGIN--

	bool SimpleSceneManager::on_Link_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		IBaseList *NewNode = new IBaseList;
		if (nullptr == NewNode)return false;
		else{
			NewNode->Ptr = Dest;
			NewNode->Next = This->ObjList;
			This->ObjList = NewNode;
			return true;
		}
	}
	void SimpleSceneManager::on_UnLink_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		IBaseList **TmpPtr = &This->ObjList;
		while (*TmpPtr != nullptr){
			if (Dest == (*TmpPtr)->Ptr){
				IBaseList *PtrToDelete = *TmpPtr;
				*TmpPtr = PtrToDelete->Next;
				delete PtrToDelete;
				return;
			}
			else TmpPtr = &((*TmpPtr)->Next);
		}
	}
	void SimpleSceneManager::VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *Info){
		this_type *This = static_cast<this_type *>(ThisBase);
		IBaseList *TmpPtr = This->ObjList;
		while (TmpPtr != nullptr){
			const ISceneNode *I = TmpPtr->Ptr->GetInterface<ISceneNode>();
			INTERFACE_CALL(I, VPQuery)(TmpPtr->Ptr, lpScheduler, QueryType, Info);
			TmpPtr = TmpPtr->Next;
		}
	}
	void SimpleSceneManager::ObjectQuery(nsBasic::IBase * const ThisBase, nsScene::RenderList **lpRList, UBINT QueryType, QueryInfo *Info){
		this_type *This = static_cast<this_type *>(ThisBase);
		IBaseList *TmpPtr = This->ObjList;
		while (TmpPtr != nullptr){
			const ISceneNode *I = (const ISceneNode *)TmpPtr->Ptr->_GetInterface(TmpPtr->Ptr, __typeid(ISceneNode));
			INTERFACE_CALL(I, ObjectQuery)(TmpPtr->Ptr, lpRList, QueryType, Info);
			TmpPtr = TmpPtr->Next;
		}
	}
	void SimpleSceneManager::clear(){
		IBaseList *TmpPtr = this->ObjList;
		while (nullptr != TmpPtr){
			nsBasic::IBase::DerefAndDestroy(this, TmpPtr->Ptr, nullptr);

			IBaseList *NextPtr = TmpPtr->Next;
			delete TmpPtr;
			TmpPtr = NextPtr;
		}
		this->ObjList = nullptr;
	}

	//class SimpleSceneManager --END--

	//class MatrixTransform --BEGIN--

	bool MatrixTransform::on_Link_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (nullptr == This->Child && nullptr != Dest){
			This->Child = Dest;
			return true;
		}
		else return false;
	}
	void MatrixTransform::on_UnLink_From(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (This->Child == Dest)This->Child = nullptr;
	}
	void MatrixTransform::VPQuery(nsBasic::IBase * const ThisBase, VPScheduler * const lpScheduler, UBINT QueryType, QueryInfo *Info){
		this_type *This = static_cast<this_type *>(ThisBase);

		if (nullptr != This->Child){
			QueryInfo NewInfo;
			nsMath::mul4_mm(This->Matrix, Info->Transform, NewInfo.Transform);
			NewInfo.QueryMask = Info->QueryMask;
			NewInfo.QueryObjType = Info->QueryObjType;
			NewInfo.ExtInfo = Info->ExtInfo;

			const ISceneNode *I = This->Child->GetInterface<ISceneNode>();
			INTERFACE_CALL(I, VPQuery)(This->Child, lpScheduler, QueryType, &NewInfo);
		}
	}
	void MatrixTransform::ObjectQuery(nsBasic::IBase * const ThisBase, nsScene::RenderList **lpRList, UBINT QueryType, QueryInfo *Info){
		this_type *This = static_cast<this_type *>(ThisBase);

		if (nullptr != This->Child){
			QueryInfo NewInfo;
			nsMath::mul4_mm(This->Matrix, Info->Transform, NewInfo.Transform);
			NewInfo.QueryMask = Info->QueryMask;
			NewInfo.QueryObjType = Info->QueryObjType;
			NewInfo.ExtInfo = Info->ExtInfo;

			const ISceneNode *I = This->Child->GetInterface<ISceneNode>();
			INTERFACE_CALL(I, ObjectQuery)(This->Child, lpRList, QueryType, &NewInfo);
		}
	}
	inline void MatrixTransform::clear(){
		if (nullptr != this->Child){
			nsBasic::IBase::DerefAndDestroy(this, this->Child, nullptr);
			this->Child = nullptr;
		}
	}

	//class MatrixTransform --END--
}