/* Description:Functions for advanced data struture.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_DSADV
#define LIB_DSADV

#include "lGeneral.hpp"
#include "lMath.hpp"

namespace nsDSAdv{

#if defined LIBENV_SYS_INTELX86
	static const UBINT MaxRBTreeDepth = 2 * (32 - 2 * 2) - 3; //32-bit machine,2^2 byte for UBINT
	static const UBINT MaxOctreeDepth = 32 - 5 * 2; //32-bit machine,2^2 byte for UBINT
#elif defined LIBENV_SYS_INTELX64
	static const UBINT MaxRBTreeDepth=2*(64-2*3)-3; //64-bit machine,2^3 byte for UBINT
	static const UBINT MaxOctreeDepth=64-5*3; //64-bit machine,2^3 byte for UBINT
#endif
	static const BFLOAT M_SQRT3=sqrt((BFLOAT)3.0);
	
	struct OctreeNode;
	struct IntrvTreeNode;

	typedef void(*GeneralCopyFunc)(const void *, UBINT); //Address + Length
	typedef BINT(*CompareFunc)(void *, void *);  // result > 0 means former > latter
	typedef void(*OctreeDistCallback)(OctreeNode *, BFLOAT);
	// The first parameter is a neighbor point of the given point.
	// The second parameter is the squared distance of these two points.
	typedef void(*IntrvTreeCallback)(IntrvTreeNode *);
	// The first parameter is a neighbor point of the given point.
	// The second parameter is the squared distance of these two points.

	struct ByteArray{
		//ByteArray is a class for storing pure data, but it is also suitable for storing short strings (like identifiers).
		//ByteArray is not completely thread safe (especially when it is doing memory allocation). But you can carry it all around in your program and free it whenever you like.
	private:union _InternalStruct{
				unsigned char ShortArr[8 * sizeof(UBINT)]; //This works when 8 * sizeof(UBINT) < 0xFF.
				struct{
					UBINT _Reserved;
					unsigned char *ArrPos;
					UBINT ArrLen;
					UBINT MemLen;
				};
	} _Internal;

			ByteArray() = delete; //using the default constructor is prohibited
			operator nsBasic::ObjGeneral() = delete; //not a general object
	public:ByteArray(UBINT Size);
		   ByteArray(void *Data, UBINT Size);
		   ByteArray(const ByteArray &);
		   ByteArray & operator =(const ByteArray &);
		   bool operator ==(const ByteArray &);
		   UBINT Length();
		   unsigned char * const Content();
		   UBINT Hash();
		   BINT compare(const ByteArray &);
		   ~ByteArray();

		   friend void swap(ByteArray &a, ByteArray &b){
			   _InternalStruct TmpStruct = a._Internal;
			   a._Internal = b._Internal;
			   b._Internal = TmpStruct;
		   }
	};

	template<class T> struct Stack{ //NOT thread safe
		void *PrevObj;
		void *NextObj;
		nsBasic::BlockPage *FirstPage;
		UBINT PageCount;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		UBINT BlockSize;

		Stack();
		void * Push(T *SrcAddr);
		UBINT Pop(T *DestAddr);
		UBINT Length();
		void ConvertToArray(void *DestAddr);
		void GeneralCopy(GeneralCopyFunc CopyFunc);
		void Clear();
		~Stack();
		void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(Stack<T>)); }
	};

	struct RBTreeNode{
		RBTreeNode *LChild;
		RBTreeNode *RChild;
		UBINT Color; // 0 = black  1 = Red
		void *Ptr;
	};
	template <CompareFunc CmpFunc> struct PtrSet{
	public:
		void *PrevObj;
		void *NextObj;
		nsBasic::MemHeap Allocator;
		RBTreeNode *RootNode;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		UBINT _Reserved4;

		PtrSet();
		void * Insert(void *Target);
		void * Delete(void *Target);
		void * Query(void *Target);
		void ConvertToArray(void *DestAddr);
		void Clear();
		~PtrSet();
	};
	struct IntrvTreeNode{
		IntrvTreeNode *LChild;
		IntrvTreeNode *RChild;
		IntrvTreeNode *Parent;
		UBINT Color; // 0 = black  1 = Red
		BFLOAT LBound;
		BFLOAT RBound;
		BFLOAT RefRBound; 
		void *Ptr;
	};
	struct IntrvTree{
		void *PrevObj;
		void *NextObj;
		nsBasic::MemHeap Allocator;
		IntrvTreeNode *RootNode;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		UBINT Reserved4;

		IntrvTree();
		void Clear();
		~IntrvTree();
	};
	struct OctreeNode{
		BFLOAT Pos[3];
		BFLOAT Span;
		OctreeNode *lpParent;
		OctreeNode *(*lpChildren)[8];
		UBINT ChildInfo;
		UBINT Reserved;
	};
	struct Octree{
		void *PrevObj;
		void *NextObj;
		nsBasic::MemHeap Allocator;
		OctreeNode *RootNode;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		UBINT Reserved4;

		Octree();
		OctreeNode *Insert(BFLOAT x, BFLOAT y, BFLOAT z);
		void SphericalQuery(BFLOAT x, BFLOAT y, BFLOAT z, BFLOAT DistLimit, OctreeDistCallback cbFunc);
		UBINT GetNearest(BFLOAT x, BFLOAT y, BFLOAT z, UBINT NearestCnt, OctreeNode **lpResult);
		void Clear();
		~Octree();
	};

	extern inline BFLOAT roundfloat(BFLOAT x,BFLOAT span);
	extern inline BFLOAT roundfloat2(BFLOAT x,BFLOAT y);

	extern void * BlockQueue_Push(nsBasic::BlockPageRing *lpQueue,void *SrcAddr);
	extern UBINT BlockQueue_Pop(nsBasic::BlockPageRing *lpQueue,void *DestAddr);
	extern UBINT BlockQueue_GetLength(nsBasic::BlockPageRing *lpQueue);
	extern void BlockQueue_ConvertToArray(nsBasic::BlockPageRing *lpQueue,void *DestAddr);
	extern void BlockQueue_GeneralCopy(nsBasic::BlockPageRing *lpQueue, GeneralCopyFunc CopyFunc);
	extern void CreateBlockQueue(nsBasic::BlockPageRing *lpQueue,UBINT BlockSize);

	extern void * BlockDEQueue_Push_Head(nsBasic::BlockPageRing *lpQueue,void *SrcAddr);
	extern UBINT BlockDEQueue_Pop_Head(nsBasic::BlockPageRing *lpQueue,void *DestAddr);
	extern void * BlockDEQueue_Push_Tail(nsBasic::BlockPageRing *lpQueue,void *SrcAddr);
	extern UBINT BlockDEQueue_Pop_Tail(nsBasic::BlockPageRing *lpQueue,void *DestAddr);
	extern UBINT BlockDEQueue_GetLength(nsBasic::BlockPageRing *lpQueue);
	extern void BlockDEQueue_ConvertToArray(nsBasic::BlockPageRing *lpQueue,void *DestAddr);
	extern void BlockDEQueue_GeneralCopy(nsBasic::BlockPageRing *lpQueue, GeneralCopyFunc CopyFunc);
	extern void CreateBlockDEQueue(nsBasic::BlockPageRing *lpQueue,UBINT BlockSize);

	extern IntrvTreeNode * IntrvTree_Insert(IntrvTree *lpTree,BFLOAT LBound,BFLOAT RBound,void *Target);
	extern void IntrvTree_Delete(IntrvTree *lpTree,IntrvTreeNode *Target);
	extern void IntrvTree_PointQuery(IntrvTree *lpSet,BFLOAT Point,IntrvTreeCallback cbFunc);
	extern void IntrvTree_SegQuery(IntrvTree *lpSet,BFLOAT LBound,BFLOAT RBound,IntrvTreeCallback cbFunc);

	extern void Octree_Delete(Octree *lpTree,OctreeNode *Target);
	extern UBINT Octree_GetNearest(Octree *lpTree,BFLOAT x,BFLOAT y,BFLOAT z,UBINT NearestCnt,OctreeNode **lpResult);
	/*-------------------------------- IMPLEMENTATION --------------------------------*/
	extern inline BFLOAT roundfloat(BFLOAT x,BFLOAT span){

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
#endif
		//This function returns the nearest value to (x) which equals (span) * (an odd integer).
		//(span):a float number which is either power of 2 or positive infinity.
		//This function returns a wrong answer if (x) doesn't exist.It is recommended to use (span) returned by roundfloat2(BFLOAT,BFLOAT).
		//This function is highly preformance-critical and platform-related.

		//Significand=2^Significand_bits;
#if defined LIBENV_SYS_INTELX64
		const UBINT SigBit=0x8000000000000000,Exponent=0x7FF0000000000000,Significand=0x10000000000000,Significand_bits=52;
#else
		const UBINT SigBit=0x80000000,Exponent=0x7F800000,Significand=0x800000,Significand_bits=23;
#endif
		UBINT a=*(UBINT *)&x,b=*(UBINT *)&span; //float hacking
		if(a==SigBit)a=0;else if(a>SigBit)a--;
		if(b>=Exponent)a=0;
		else if((a&(SigBit-1))<=b)a=(a&SigBit)+b;
		else{
			if(b>=Significand){
				b=((a&(SigBit-1))-b)>>Significand_bits;
				b=Significand>>b;
			}
			a=a&(~(b-1));
			if(b<Significand)a|=b;
		}
		return *(BFLOAT *)&a;
	}

	extern inline BFLOAT roundfloat2(BFLOAT x, BFLOAT y){

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
#endif
		//This function returns the minimum float number (span) which satisfies the following conditions:
		// 1. It is either 0,power of 2 or positive infinity.
		// 2. roundfloat(x,span)=roundfloat(y,span).
		//This function is highly preformance-critical and platform-related.
#if defined LIBENV_SYS_INTELX64
		const UBINT SigBit=0x8000000000000000,Exponent=0x7FF0000000000000,Significand_bits=52;
#else
		const UBINT SigBit = 0x80000000, Exponent = 0x7F800000, Significand_bits = 23;
#endif
		UBINT a = *(UBINT *)&x, b = *(UBINT *)&y, BitCount; //float hacking
		if (a == SigBit)a = 0;
		if (b == SigBit)b = 0;
		if ((a&b) >= SigBit){ a--; b--; }
		if ((a^b) >= SigBit)a = Exponent;
		else if ((a&Exponent)<(b&Exponent))a = b&Exponent;
		else if ((a&Exponent)>(b&Exponent))a = a&Exponent;
		else if (a == b)a = 0;
		else{
			b = a^b;
			a = a&Exponent;
			BitCount = nsMath::log2intsim(b) - 1;
			if (BitCount + (a >> Significand_bits) < Significand_bits)a = (UBINT)1 << (BitCount + (a >> Significand_bits));
			else a -= (Significand_bits - BitCount) << Significand_bits;
		}
		return *(BFLOAT *)&a;
	}

	template<class T> Stack<T>::Stack(){
		this->FirstPage = nullptr;
		this->PageCount = 0;
		this->Type = OBJ_STACK_BLK;
		this->MsgProc = nullptr;
		this->ThreadLock = (UBINT)NULL;
		this->BlockSize = sizeof(T);
	}
	template<class T> void *Stack<T>::Push(T *SrcAddr){
		void *PageAddr;
		if(nullptr==this->FirstPage || 0==this->FirstPage->BlockCntr){
			nsBasic::BlockPage *NewPage=new nsBasic::BlockPage;
			if (nullptr == NewPage)return nullptr;
			PageAddr=nsBasic::GetPage(nsEnv::SysPageSize);
			if (nullptr == PageAddr){ delete NewPage; return nullptr; }
			nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)NewPage,(void **)&this->FirstPage);

			NewPage->PageAddr=PageAddr;
			NewPage->NextEmpty=(UBINT)PageAddr+this->BlockSize;
			NewPage->Type=PAGE_BLK;
			NewPage->PageSize=nsEnv::SysPageSize;
			NewPage->BlockSize=this->BlockSize;
			NewPage->BlockCntr=nsEnv::SysPageSize/this->BlockSize-1;
			this->PageCount++;
		}
		else{
			PageAddr=(void *)((nsBasic::BlockPage *)this->FirstPage)->NextEmpty;
			((nsBasic::BlockPage *)this->FirstPage)->NextEmpty=(UBINT)PageAddr+this->BlockSize;
			((nsBasic::BlockPage *)this->FirstPage)->BlockCntr--;
		}
		memcpy((void *)PageAddr,SrcAddr,this->BlockSize);
		return (void *)PageAddr;
	}
	template<class T> UBINT Stack<T>::Pop(T *DestAddr){
		nsBasic::BlockPage *FirstPage=this->FirstPage;
		if(NULL==FirstPage)return 1;
		else{
			FirstPage->NextEmpty-=this->BlockSize;
			FirstPage->BlockCntr++;
			memcpy(DestAddr,(void *)FirstPage->NextEmpty,this->BlockSize);
			if(FirstPage->NextEmpty==(UBINT)FirstPage->PageAddr){
				nsBasic::unGetPage((void *)(FirstPage->PageAddr),nsEnv::SysPageSize);
				nsBasic::ObjRing_Detach((nsBasic::ObjGeneral *)FirstPage,(void **)&this->FirstPage);
				delete FirstPage;
				this->PageCount--;
			}
			return 0;
		}
	}
	template<class T> UBINT Stack<T>::Length(){
		UBINT MaxBlksPerPage=nsEnv::SysPageSize/this->BlockSize,RetValue=0;
		nsBasic::BlockPage *CurPage=(nsBasic::BlockPage *)this->FirstPage;
		if(NULL!=CurPage){
			CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
			while(CurPage!=(nsBasic::BlockPage *)this->FirstPage){
				RetValue+=MaxBlksPerPage;
				CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
			}
			RetValue+=(CurPage->NextEmpty-(UBINT)CurPage->PageAddr)/this->BlockSize;
		}
		return RetValue;
	}
	template<class T> void Stack<T>::ConvertToArray(void *DestAddr){
		nsBasic::BlockPage *CurPage=this->FirstPage;
		if(NULL!=CurPage){
			do{
				CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
				memcpy(DestAddr,(void *)CurPage->PageAddr,CurPage->NextEmpty-(UBINT)CurPage->PageAddr);
				DestAddr=(void *)(CurPage->NextEmpty-(UBINT)CurPage->PageAddr+(UBINT)DestAddr);
			}while(CurPage!=(nsBasic::BlockPage *)this->FirstPage);
		}
	}
	template<class T> void Stack<T>::GeneralCopy(GeneralCopyFunc CopyFunc){
		nsBasic::BlockPage *CurPage = this->FirstPage;
		if (NULL != CurPage){
			do{
				CurPage = (nsBasic::BlockPage *)CurPage->PrevPage;
				CopyFunc((void *)CurPage->PageAddr, CurPage->NextEmpty - (UBINT)CurPage->PageAddr);
			} while (CurPage != (nsBasic::BlockPage *)this->FirstPage);
		}
	}
	template<class T> void Stack<T>::Clear(){
		nsBasic::BlockPage *lpPage1 = this->FirstPage, *lpPage2;
		if (nullptr != lpPage1)do{
			lpPage2 = (nsBasic::BlockPage *)lpPage1->NextPage;
			nsBasic::unGetPage((void *)lpPage1->PageAddr, lpPage1->PageSize);
			delete lpPage1;
			lpPage1 = lpPage2;
		} while (lpPage1 != this->FirstPage);
		this->FirstPage = nullptr;
	}
	template<class T> Stack<T>::~Stack(){
		this->Clear();
	}

	extern void * BlockQueue_Push(nsBasic::BlockPageRing *lpQueue,void *SrcAddr){
		void *PageAddr;
		if(nullptr==lpQueue->FirstPage ||
			((nsBasic::BlockPage *)lpQueue->FirstPage)->NextEmpty-(UBINT)((nsBasic::BlockPage *)lpQueue->FirstPage)->PageAddr>((nsBasic::BlockPage *)lpQueue->FirstPage)->PageSize-lpQueue->BlockSize){
			nsBasic::BlockPage *NewPage = new nsBasic::BlockPage;
			if(NULL==NewPage)return NULL;
			PageAddr=nsBasic::GetPage(nsEnv::SysPageSize);
			if(nullptr==PageAddr){delete NewPage;return NULL;}
			nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)NewPage,&lpQueue->FirstPage);

			NewPage->PageAddr=PageAddr;
			NewPage->NextEmpty=(UBINT)PageAddr+lpQueue->BlockSize;
			NewPage->Type=PAGE_BLK;
			NewPage->PageSize=nsEnv::SysPageSize;
			NewPage->BlockSize=lpQueue->BlockSize;
			NewPage->BlockCntr=(UBINT)NewPage->PageAddr;
			lpQueue->PageCount++;
		}
		else{
			PageAddr=(void *)((nsBasic::BlockPage *)lpQueue->FirstPage)->NextEmpty;
			((nsBasic::BlockPage *)lpQueue->FirstPage)->NextEmpty=(UBINT)PageAddr+lpQueue->BlockSize;
		}
		memcpy((void *)PageAddr,SrcAddr,lpQueue->BlockSize);
		return (void *)PageAddr;
	}
	extern UBINT BlockQueue_Pop(nsBasic::BlockPageRing *lpQueue,void *DestAddr){
		if(nullptr==lpQueue->FirstPage)return 1;
		else{
			nsBasic::BlockPage *LastPage=(nsBasic::BlockPage *)((nsBasic::BlockPage *)lpQueue->FirstPage)->PrevPage;
			memcpy(DestAddr,(void *)LastPage->BlockCntr,lpQueue->BlockSize);
			LastPage->BlockCntr+=lpQueue->BlockSize;
			if(LastPage->NextEmpty==LastPage->BlockCntr){
				nsBasic::unGetPage((void *)(LastPage->PageAddr),nsEnv::SysPageSize);
				nsBasic::ObjRing_Detach((nsBasic::ObjGeneral *)LastPage,&lpQueue->FirstPage);
				delete LastPage;
				lpQueue->PageCount--;
			}
			return 0;
		}
	}
	extern UBINT BlockQueue_GetLength(nsBasic::BlockPageRing *lpQueue){
		UBINT MaxBlksPerPage=nsEnv::SysPageSize/lpQueue->BlockSize,RetValue=0;
		nsBasic::BlockPage *CurPage=(nsBasic::BlockPage *)lpQueue->FirstPage;
		if(NULL!=CurPage){
			CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
			if(CurPage!=(nsBasic::BlockPage *)lpQueue->FirstPage){
				RetValue+=(CurPage->NextEmpty-CurPage->BlockCntr)/lpQueue->BlockSize;
				CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
				while(CurPage!=(nsBasic::BlockPage *)lpQueue->FirstPage){
					RetValue+=MaxBlksPerPage;
					CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
				}
			}
			RetValue+=(CurPage->NextEmpty-CurPage->BlockCntr)/lpQueue->BlockSize;
		}
		return RetValue;
	}
	extern void BlockQueue_ConvertToArray(nsBasic::BlockPageRing *lpQueue,void *DestAddr){
		nsBasic::BlockPage *CurPage=(nsBasic::BlockPage *)lpQueue->FirstPage;
		if(NULL!=CurPage){
			do{
				CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
				memcpy(DestAddr,(void *)CurPage->BlockCntr,CurPage->NextEmpty-CurPage->BlockCntr);
				DestAddr=(void *)(CurPage->NextEmpty-CurPage->BlockCntr+(UBINT)DestAddr);
			}while(CurPage!=(nsBasic::BlockPage *)lpQueue->FirstPage);
		}
	}
	extern void BlockQueue_GeneralCopy(nsBasic::BlockPageRing *lpQueue, GeneralCopyFunc CopyFunc){
		nsBasic::BlockPage *CurPage = (nsBasic::BlockPage *)lpQueue->FirstPage;
		if (NULL != CurPage){
			do{
				CurPage = (nsBasic::BlockPage *)CurPage->PrevPage;
				CopyFunc((void *)CurPage->BlockCntr, CurPage->NextEmpty - CurPage->BlockCntr);
			} while (CurPage != (nsBasic::BlockPage *)lpQueue->FirstPage);
		}
	}
	extern void CreateBlockQueue(nsBasic::BlockPageRing *lpQueue,UBINT BlockSize){
		lpQueue->FirstPage = (UBINT)NULL;
		lpQueue->PageCount=0;
		lpQueue->Type=OBJ_QUEUE_BLK;
		lpQueue->MsgProc=nullptr;
		lpQueue->ThreadLock = (UBINT)NULL;
		lpQueue->BlockSize=BlockSize;
	}

	extern void * BlockDEQueue_Push_Head(nsBasic::BlockPageRing *lpQueue,void *SrcAddr){
		void * PageAddr;
		if((UBINT)NULL==lpQueue->FirstPage ||
			((nsBasic::BlockPage *)(((nsBasic::BlockPage *)lpQueue->FirstPage)->PrevPage))->BlockCntr==(UBINT)((nsBasic::BlockPage *)(((nsBasic::BlockPage *)lpQueue->FirstPage)->PrevPage))->PageAddr){
			nsBasic::BlockPage *NewPage=new nsBasic::BlockPage;
			if(NULL==NewPage)return NULL;
			PageAddr = nsBasic::GetPage(nsEnv::SysPageSize);
			if ((UBINT)NULL == PageAddr){ delete NewPage; return NULL; }
			nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)NewPage,&lpQueue->FirstPage);
			lpQueue->FirstPage=((nsBasic::BlockPage *)lpQueue->FirstPage)->NextPage;
			NewPage->PageAddr=PageAddr;
			NewPage->NextEmpty=(UBINT)PageAddr+nsEnv::SysPageSize-(nsEnv::SysPageSize%lpQueue->BlockSize);
			NewPage->Type=PAGE_BLK;
			NewPage->PageSize=nsEnv::SysPageSize;
			NewPage->BlockSize=lpQueue->BlockSize;
			NewPage->BlockCntr=NewPage->NextEmpty-lpQueue->BlockSize;
			lpQueue->PageCount++;
			memcpy((void *)NewPage->BlockCntr,SrcAddr,lpQueue->BlockSize);
			return (void *)NewPage->BlockCntr;
		}
		else{
			((nsBasic::BlockPage *)(((nsBasic::BlockPage *)lpQueue->FirstPage)->PrevPage))->BlockCntr-=lpQueue->BlockSize;
			memcpy((void *)(((nsBasic::BlockPage *)(((nsBasic::BlockPage *)lpQueue->FirstPage)->PrevPage))->BlockCntr),SrcAddr,lpQueue->BlockSize);
			return (void *)(((nsBasic::BlockPage *)(((nsBasic::BlockPage *)lpQueue->FirstPage)->PrevPage))->BlockCntr);
		}
	}
	extern UBINT BlockDEQueue_Pop_Head(nsBasic::BlockPageRing *lpQueue,void *DestAddr){
		if((UBINT)NULL==lpQueue->FirstPage)return 1;
		else{
			nsBasic::BlockPage *LastPage=(nsBasic::BlockPage *)((nsBasic::BlockPage *)lpQueue->FirstPage)->PrevPage;
			memcpy(DestAddr,(void *)LastPage->BlockCntr,lpQueue->BlockSize);
			LastPage->BlockCntr+=lpQueue->BlockSize;
			if(LastPage->NextEmpty==LastPage->BlockCntr){
				nsBasic::unGetPage((void *)(LastPage->PageAddr),nsEnv::SysPageSize);
				nsBasic::ObjRing_Detach((nsBasic::ObjGeneral *)LastPage,&lpQueue->FirstPage);
				delete LastPage;
				lpQueue->PageCount--;
			}
			return 0;
		}
	}
	extern void * BlockDEQueue_Push_Tail(nsBasic::BlockPageRing *lpQueue,void *SrcAddr){
		void *PageAddr;
		if((UBINT)NULL==lpQueue->FirstPage ||
			((nsBasic::BlockPage *)lpQueue->FirstPage)->NextEmpty-(UBINT)((nsBasic::BlockPage *)lpQueue->FirstPage)->PageAddr>((nsBasic::BlockPage *)lpQueue->FirstPage)->PageSize-lpQueue->BlockSize){
			nsBasic::BlockPage *NewPage = new nsBasic::BlockPage;
			if(NULL==NewPage)return NULL;
			PageAddr=nsBasic::GetPage(nsEnv::SysPageSize);
			if ((UBINT)NULL == PageAddr){ delete NewPage; return NULL; }
			nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)NewPage,&lpQueue->FirstPage);

			NewPage->PageAddr=PageAddr;
			NewPage->NextEmpty=(UBINT)PageAddr+lpQueue->BlockSize;
			NewPage->Type=PAGE_BLK;
			NewPage->PageSize=nsEnv::SysPageSize;
			NewPage->BlockSize=lpQueue->BlockSize;
			NewPage->BlockCntr=(UBINT)NewPage->PageAddr;
			lpQueue->PageCount++;
		}
		else{
			PageAddr=(void *)((nsBasic::BlockPage *)lpQueue->FirstPage)->NextEmpty;
			((nsBasic::BlockPage *)lpQueue->FirstPage)->NextEmpty=(UBINT)PageAddr+lpQueue->BlockSize;
		}
		memcpy((void *)PageAddr,SrcAddr,lpQueue->BlockSize);
		return (void *)PageAddr;
	}
	extern UBINT BlockDEQueue_Pop_Tail(nsBasic::BlockPageRing *lpQueue,void *DestAddr){
		nsBasic::BlockPage *FirstPage=(nsBasic::BlockPage *)lpQueue->FirstPage;
		if(NULL==FirstPage)return 1;
		else{
			FirstPage->NextEmpty-=lpQueue->BlockSize;
			memcpy(DestAddr,(void *)FirstPage->NextEmpty,lpQueue->BlockSize);
			if(FirstPage->NextEmpty<=FirstPage->BlockCntr){
				nsBasic::unGetPage((void *)(FirstPage->PageAddr),nsEnv::SysPageSize);
				nsBasic::ObjRing_Detach((nsBasic::ObjGeneral *)FirstPage,&lpQueue->FirstPage);
				delete FirstPage;
				lpQueue->PageCount--;
			}
			return 0;
		}
	}
	extern UBINT BlockDEQueue_GetLength(nsBasic::BlockPageRing *lpQueue){
		UBINT MaxBlksPerPage=nsEnv::SysPageSize/lpQueue->BlockSize,RetValue=0;
		nsBasic::BlockPage *CurPage=(nsBasic::BlockPage *)lpQueue->FirstPage;
		if(NULL!=CurPage){
			CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
			if(CurPage!=(nsBasic::BlockPage *)lpQueue->FirstPage){
				RetValue+=(CurPage->NextEmpty-CurPage->BlockCntr)/lpQueue->BlockSize;
				CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
				while(CurPage!=(nsBasic::BlockPage *)lpQueue->FirstPage){
					RetValue+=MaxBlksPerPage;
					CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
				}
			}
			RetValue+=(CurPage->NextEmpty-CurPage->BlockCntr)/lpQueue->BlockSize;
		}
		return RetValue;
	}
	extern void BlockDEQueue_ConvertToArray(nsBasic::BlockPageRing *lpQueue,void *DestAddr){
		nsBasic::BlockPage *CurPage=(nsBasic::BlockPage *)lpQueue->FirstPage;
		if(NULL!=CurPage){
			do{
				CurPage=(nsBasic::BlockPage *)CurPage->PrevPage;
				memcpy(DestAddr,(void *)CurPage->BlockCntr,CurPage->NextEmpty-CurPage->BlockCntr);
				DestAddr=(void *)(CurPage->NextEmpty-CurPage->BlockCntr+(UBINT)DestAddr);
			}while(CurPage!=(nsBasic::BlockPage *)lpQueue->FirstPage);
		}
	}
	extern void BlockDEQueue_GeneralCopy(nsBasic::BlockPageRing *lpQueue, GeneralCopyFunc CopyFunc){
		nsBasic::BlockPage *CurPage = (nsBasic::BlockPage *)lpQueue->FirstPage;
		if (NULL != CurPage){
			do{
				CurPage = (nsBasic::BlockPage *)CurPage->PrevPage;
				CopyFunc((void *)CurPage->BlockCntr, CurPage->NextEmpty - CurPage->BlockCntr);
			} while (CurPage != (nsBasic::BlockPage *)lpQueue->FirstPage);
		}
	}
	extern void CreateBlockDEQueue(nsBasic::BlockPageRing *lpQueue,UBINT BlockSize){
		lpQueue->FirstPage=(UBINT)NULL;
		lpQueue->PageCount=0;
		lpQueue->Type=OBJ_DEQUEUE_BLK;
		lpQueue->MsgProc=nullptr;
		lpQueue->ThreadLock=(UBINT)NULL;
		lpQueue->BlockSize=BlockSize;
	}

	template <CompareFunc CmpFunc> void * PtrSet<CmpFunc>::Insert(void *Target){
		if (nullptr == this->RootNode){
			if (nullptr == (this->RootNode = (RBTreeNode *)this->Allocator.Alloc_Bin(sizeof(RBTreeNode))))return nullptr;
			this->RootNode->LChild=(UBINT)NULL;
			this->RootNode->RChild=(UBINT)NULL;
			this->RootNode->Color=0;
			this->RootNode->Ptr=Target;
		}
		else{
			RBTreeNode *TmpStack[MaxRBTreeDepth],*CurNode,*UncleNode;
			BINT CmpResult,TmpStackPtr=0;

			TmpStack[0]=this->RootNode;
			while(1){
				CmpResult=CmpFunc(Target,(void *)TmpStack[TmpStackPtr]->Ptr);
				if(0==CmpResult)return (void *)TmpStack[TmpStackPtr]->Ptr;
				else if(0<CmpResult)CurNode=TmpStack[TmpStackPtr]->RChild;
				else CurNode=TmpStack[TmpStackPtr]->LChild;
				if (nullptr == CurNode){
					if (nullptr == (CurNode = (RBTreeNode *)this->Allocator.Alloc_Bin(sizeof(RBTreeNode))))return nullptr;
					CurNode->LChild=NULL;
					CurNode->RChild=NULL;
					CurNode->Color=1; //red
					CurNode->Ptr=Target;
					if (0 < CmpResult)TmpStack[TmpStackPtr]->RChild = CurNode; else TmpStack[TmpStackPtr]->LChild = CurNode;
					break;
				}
				else TmpStack[++TmpStackPtr]=CurNode;
			}

			//Balancing
			while(1){
				if(1==TmpStack[TmpStackPtr]->Color){
					//Get uncle node
					if((RBTreeNode *)TmpStack[TmpStackPtr-1]->LChild==TmpStack[TmpStackPtr]){
						UncleNode=TmpStack[TmpStackPtr-1]->RChild;
						if(NULL==UncleNode || 0==UncleNode->Color){
							if(TmpStack[TmpStackPtr]->RChild==CurNode){
								//Case L4
								CurNode->Color=0;
								TmpStack[TmpStackPtr-1]->Color=1;

								TmpStack[TmpStackPtr]->RChild=CurNode->LChild;
								CurNode->LChild=TmpStack[TmpStackPtr];

								TmpStack[TmpStackPtr-1]->LChild=CurNode->RChild;
								CurNode->RChild=TmpStack[TmpStackPtr-1];

								if(TmpStackPtr<2)this->RootNode=CurNode;
								else if(TmpStack[TmpStackPtr-2]->LChild==TmpStack[TmpStackPtr-1])TmpStack[TmpStackPtr-2]->LChild=CurNode;
								else TmpStack[TmpStackPtr-2]->RChild=CurNode;
							}
							else{
								// Case L5
								TmpStack[TmpStackPtr]->Color=0;
								TmpStack[TmpStackPtr-1]->Color=1;

								TmpStack[TmpStackPtr-1]->LChild=TmpStack[TmpStackPtr]->RChild;
								TmpStack[TmpStackPtr]->RChild=TmpStack[TmpStackPtr-1];

								if(TmpStackPtr<2)this->RootNode=TmpStack[TmpStackPtr];
								else if(TmpStack[TmpStackPtr-2]->LChild==TmpStack[TmpStackPtr-1])TmpStack[TmpStackPtr-2]->LChild=TmpStack[TmpStackPtr];
								else TmpStack[TmpStackPtr-2]->RChild=TmpStack[TmpStackPtr];
							}
							break;
						}
						else{
							//Case L3
							TmpStack[TmpStackPtr]->Color=0;
							UncleNode->Color=0;
							TmpStack[TmpStackPtr-1]->Color=1;
							CurNode=TmpStack[TmpStackPtr-1];
							if(TmpStackPtr<2){CurNode->Color=0;break;}else TmpStackPtr-=2;
						}
					}
					else{
						UncleNode=TmpStack[TmpStackPtr-1]->LChild;
						if(NULL==UncleNode || 0==UncleNode->Color){
							if(TmpStack[TmpStackPtr]->LChild==CurNode){
								//Case R4
								CurNode->Color=0;
								TmpStack[TmpStackPtr-1]->Color=1;

								TmpStack[TmpStackPtr]->LChild=CurNode->RChild;
								CurNode->RChild=TmpStack[TmpStackPtr];

								TmpStack[TmpStackPtr-1]->RChild=CurNode->LChild;
								CurNode->LChild=TmpStack[TmpStackPtr-1];

								if (TmpStackPtr < 2)this->RootNode = CurNode;
								else if (TmpStack[TmpStackPtr - 2]->RChild == TmpStack[TmpStackPtr - 1])TmpStack[TmpStackPtr - 2]->RChild = CurNode;
								else TmpStack[TmpStackPtr - 2]->LChild = CurNode;
							}
							else{
								// Case R5
								TmpStack[TmpStackPtr]->Color=0;
								TmpStack[TmpStackPtr-1]->Color=1;

								TmpStack[TmpStackPtr-1]->RChild=TmpStack[TmpStackPtr]->LChild;
								TmpStack[TmpStackPtr]->LChild=TmpStack[TmpStackPtr-1];

								if(TmpStackPtr<2)this->RootNode=TmpStack[TmpStackPtr];
								else if(TmpStack[TmpStackPtr-2]->RChild==TmpStack[TmpStackPtr-1])TmpStack[TmpStackPtr-2]->RChild=TmpStack[TmpStackPtr];
								else TmpStack[TmpStackPtr-2]->LChild=TmpStack[TmpStackPtr];
							}
							break;
						}
						else{
							//Case R3
							TmpStack[TmpStackPtr]->Color=0;
							UncleNode->Color=0;
							TmpStack[TmpStackPtr-1]->Color=1;
							CurNode=TmpStack[TmpStackPtr-1];
							if(TmpStackPtr<2){CurNode->Color=0;break;}else TmpStackPtr-=2;
						}
					}
				}
				else break;
			}
		}
		return Target;
	}
	template <CompareFunc CmpFunc> void * PtrSet<CmpFunc>::Delete(void *Target){
		if (nullptr == this->RootNode)return nullptr;
		else{
			RBTreeNode *TmpStack[MaxRBTreeDepth],*CurNode,*TmpNode;
			BINT CmpResult,TmpStackPtr=0;
			void *RetValue;

			TmpStack[0]=this->RootNode;
			while(1){
				CmpResult=CmpFunc(Target,(void *)TmpStack[TmpStackPtr]->Ptr);
				if(0==CmpResult)break;
				else if(0<CmpResult)CurNode=(RBTreeNode *)TmpStack[TmpStackPtr]->RChild;
				else CurNode=TmpStack[TmpStackPtr]->LChild;
				if (nullptr == CurNode)return nullptr; else TmpStack[++TmpStackPtr] = CurNode;
			}

			CurNode=TmpStack[TmpStackPtr];
			TmpStackPtr--;
			RetValue=(void *)CurNode->Ptr;
			if (nullptr != CurNode->LChild && nullptr != CurNode->RChild){
				TmpStackPtr++;
				TmpNode=CurNode->LChild;
				while (nullptr != TmpNode->RChild){
					TmpStack[++TmpStackPtr]=TmpNode;
					TmpNode=TmpNode->RChild;
				}
				CurNode->Ptr=TmpNode->Ptr;
				CurNode=TmpNode;
			}
			if(-1==TmpStackPtr){
				if (nullptr == CurNode->LChild)TmpNode = CurNode->RChild; else TmpNode = CurNode->LChild;
				if(NULL!=TmpNode)TmpNode->Color=0;
				this->RootNode = TmpNode;
			}
			else{
				if (nullptr == CurNode->LChild)TmpNode = CurNode->RChild; else TmpNode = CurNode->LChild;
				if (TmpStack[TmpStackPtr]->LChild == CurNode)TmpStack[TmpStackPtr]->LChild = TmpNode; else TmpStack[TmpStackPtr]->RChild = TmpNode;
				if(0==CurNode->Color){
					//Balancing
					if(NULL!=TmpNode)TmpNode->Color=0;
					else while(1){
						//Get sibling node as TmpNode
						if(TmpStack[TmpStackPtr]->LChild==TmpNode){
							TmpNode=(RBTreeNode *)TmpStack[TmpStackPtr]->RChild;
							if(1==TmpNode->Color){
								//Case L2
								RBTreeNode *TmpNode2=TmpNode->LChild;

								TmpStack[TmpStackPtr]->Color=1;
								TmpStack[TmpStackPtr]->RChild=TmpNode2;
								TmpNode->Color=0;
								TmpNode->LChild=TmpStack[TmpStackPtr];

								if (0 == TmpStackPtr)this->RootNode = TmpNode;
								else if (TmpStack[TmpStackPtr - 1]->RChild == TmpStack[TmpStackPtr])TmpStack[TmpStackPtr - 1]->RChild = TmpNode;
								else TmpStack[TmpStackPtr - 1]->LChild = TmpNode;

								if (nullptr != TmpNode2->RChild && 1 == TmpNode2->RChild->Color){
									//Case L2-L6
									TmpNode2->RChild->Color = 0;
									TmpNode->LChild = TmpNode2;
									TmpStack[TmpStackPtr]->RChild = TmpNode2->LChild;
									TmpNode2->LChild = TmpStack[TmpStackPtr];
								}
								else if ((UBINT)NULL != TmpNode2->LChild && 1 == TmpNode2->LChild->Color){
									//Case L2-L5
									TmpNode->LChild = TmpNode2->LChild;
									TmpNode = TmpNode2;
									TmpNode2 = TmpNode2->LChild;

									TmpNode->LChild = TmpNode2->RChild;
									TmpNode->Color = 0;
									TmpStack[TmpStackPtr]->RChild = TmpNode2->LChild;
									TmpNode2->RChild = TmpNode;
									TmpNode2->LChild = TmpStack[TmpStackPtr];
								}
								TmpNode2->Color=1;
								TmpStack[TmpStackPtr]->Color=0;

								break;
							}
							else if((UBINT)NULL!=TmpNode->RChild && 1==TmpNode->RChild->Color){
								//Case L6
								((RBTreeNode *)TmpNode->RChild)->Color=0;

								TmpNode->Color=TmpStack[TmpStackPtr]->Color;
								TmpStack[TmpStackPtr]->RChild=TmpNode->LChild;
								TmpStack[TmpStackPtr]->Color=0;
								TmpNode->LChild=TmpStack[TmpStackPtr];

								if(0==TmpStackPtr)this->RootNode=TmpNode;
								else if(TmpStack[TmpStackPtr-1]->RChild==TmpStack[TmpStackPtr])TmpStack[TmpStackPtr-1]->RChild=TmpNode;
								else TmpStack[TmpStackPtr-1]->LChild=TmpNode;

								break;
							}
							else if((UBINT)NULL!=TmpNode->LChild && 1==TmpNode->LChild->Color){
								//Case L5
								RBTreeNode *TmpNode2=TmpNode->LChild;

								TmpNode->LChild=TmpNode2->RChild;
								// TmpNode->Color=0;
								TmpStack[TmpStackPtr]->RChild=TmpNode2->LChild;
								TmpNode2->RChild=TmpNode;
								TmpNode2->LChild=TmpStack[TmpStackPtr];
								TmpNode2->Color=TmpStack[TmpStackPtr]->Color;
								TmpStack[TmpStackPtr]->Color=0;

								if (0 == TmpStackPtr)this->RootNode = TmpNode2;
								else if (TmpStack[TmpStackPtr - 1]->RChild == TmpStack[TmpStackPtr])TmpStack[TmpStackPtr - 1]->RChild = TmpNode2;
								else TmpStack[TmpStackPtr - 1]->LChild = TmpNode2;

								break;
							}
							else{
								// Case L3,L4
								TmpNode->Color=1;
								if(1==TmpStack[TmpStackPtr]->Color){TmpStack[TmpStackPtr]->Color=0;break;}
								if(0==TmpStackPtr)break;
								TmpNode=TmpStack[TmpStackPtr];
								TmpStackPtr--;
							}
						}
						else{
							TmpNode=TmpStack[TmpStackPtr]->LChild;
							if(1==TmpNode->Color){
								//Case R2
								RBTreeNode *TmpNode2=TmpNode->RChild;

								TmpStack[TmpStackPtr]->Color=1;
								TmpStack[TmpStackPtr]->LChild=TmpNode2;
								TmpNode->Color=0;
								TmpNode->RChild=TmpStack[TmpStackPtr];

								if(0==TmpStackPtr)this->RootNode=TmpNode;
								else if(TmpStack[TmpStackPtr-1]->LChild==TmpStack[TmpStackPtr])TmpStack[TmpStackPtr-1]->LChild=TmpNode;
								else TmpStack[TmpStackPtr-1]->RChild=TmpNode;

								if (nullptr != TmpNode2->LChild && 1 == TmpNode2->LChild->Color){
									//Case R2-R6
									TmpNode2->LChild->Color=0;
									TmpNode->RChild=TmpNode2;
									TmpStack[TmpStackPtr]->LChild=TmpNode2->RChild;
									TmpNode2->RChild=TmpStack[TmpStackPtr];
								}
								else if (nullptr != TmpNode2->RChild && 1 == TmpNode2->RChild->Color){
									//Case R2-R5
									TmpNode->RChild=TmpNode2->RChild;
									TmpNode=TmpNode2;
									TmpNode2 = TmpNode2->RChild;

									TmpNode->RChild=TmpNode2->LChild;
									TmpNode->Color=0;
									TmpStack[TmpStackPtr]->LChild=TmpNode2->RChild;
									TmpNode2->LChild = TmpNode;
									TmpNode2->RChild = TmpStack[TmpStackPtr];
								}
								TmpNode2->Color=1;
								TmpStack[TmpStackPtr]->Color=0;

								break;
							}
							else if(nullptr!=TmpNode->LChild && 1==TmpNode->LChild->Color){
								//Case R6
								TmpNode->LChild->Color=0;

								TmpNode->Color=TmpStack[TmpStackPtr]->Color;
								TmpStack[TmpStackPtr]->LChild=TmpNode->RChild;
								TmpStack[TmpStackPtr]->Color=0;
								TmpNode->RChild=TmpStack[TmpStackPtr];

								if(0==TmpStackPtr)this->RootNode=TmpNode;
								else if(TmpStack[TmpStackPtr-1]->LChild==TmpStack[TmpStackPtr])TmpStack[TmpStackPtr-1]->LChild=TmpNode;
								else TmpStack[TmpStackPtr-1]->RChild=TmpNode;

								break;
							}
							else if (nullptr != TmpNode->RChild && 1 == TmpNode->RChild->Color){
								//Case R5
								RBTreeNode *TmpNode2=TmpNode->RChild;

								TmpNode->RChild=TmpNode2->LChild;
								// TmpNode->Color=0; //No use
								TmpStack[TmpStackPtr]->LChild=TmpNode2->RChild;
								TmpNode2->LChild=TmpNode;
								TmpNode2->RChild=TmpStack[TmpStackPtr];
								TmpNode2->Color=TmpStack[TmpStackPtr]->Color;
								TmpStack[TmpStackPtr]->Color=0;

								if(0==TmpStackPtr)this->RootNode=TmpNode2;
								else if(TmpStack[TmpStackPtr-1]->LChild==TmpStack[TmpStackPtr])TmpStack[TmpStackPtr-1]->LChild=TmpNode2;
								else TmpStack[TmpStackPtr-1]->RChild=TmpNode2;

								break;
							}
							else{
								// Case R3,R4
								TmpNode->Color=1;
								if(1==TmpStack[TmpStackPtr]->Color){TmpStack[TmpStackPtr]->Color=0;break;}
								if(0==TmpStackPtr)break;
								TmpNode=TmpStack[TmpStackPtr];
								TmpStackPtr--;
							}
						}
					}
				}
			}
			this->Allocator.Free_Bin(CurNode, sizeof(RBTreeNode));
			return RetValue;
		}
	}
	template <CompareFunc CmpFunc> void * PtrSet<CmpFunc>::Query(void *Target){
		if (nullptr == this->RootNode)return NULL;
		else{
			RBTreeNode *CurNode;
			BINT CmpResult;
			CompareFunc CmpFunc=(CompareFunc)this->CmpFunc;

			CurNode=(RBTreeNode *)this->RootNode;
			while(1){
				CmpResult=CmpFunc(Target,(void *)CurNode->Ptr);
				if(0==CmpResult)return (void *)CurNode->Ptr;
				else if(0<CmpResult)CurNode=(RBTreeNode *)CurNode->RChild;
				else CurNode=(RBTreeNode *)CurNode->LChild;
				if (nullptr == CurNode)return nullptr;
			}
		}
	}
	template <CompareFunc CmpFunc> void PtrSet<CmpFunc>::ConvertToArray(void *DestAddr){
		if (nullptr != this->RootNode){
			RBTreeNode *TmpStack[MaxRBTreeDepth], *CurNode, *TmpNode;
			BINT TmpStackPtr = 0;
			void **TgtAddr = (void **)DestAddr;

			TmpStack[0] = this->RootNode;
			CurNode = TmpStack[0];
			while (1){
				if (CurNode == TmpStack[TmpStackPtr]->RChild){
					CurNode = TmpStack[TmpStackPtr];
					*TgtAddr = CurNode->Ptr;
					if (0 == TmpStackPtr)break;
					else{
						TgtAddr++;
						TmpStackPtr--;
					}
				}
				else if (CurNode == TmpStack[TmpStackPtr]->LChild){
					CurNode = TmpStack[TmpStackPtr]->RChild;
					if (nullptr != CurNode)TmpStack[++TmpStackPtr] = CurNode;
				}
				else{ // if (CurNode == TmpStack[TmpStackPtr])
					CurNode = CurNode->LChild;
					if (nullptr != CurNode)TmpStack[++TmpStackPtr] = CurNode;
				}
			}
		}
	}
	template <CompareFunc CmpFunc> PtrSet<CmpFunc>::PtrSet(){
		this->Allocator = { nullptr };
		this->RootNode = nullptr;
		this->Type = OBJ_SET_INDEX;
		this->MsgProc = nullptr;
		this->ThreadLock = 0;
	}
	template <CompareFunc CmpFunc> void PtrSet<CmpFunc>::Clear(){
		this->Allocator.Clear();
		this->RootNode = nullptr;
	}
	template <CompareFunc CmpFunc> PtrSet<CmpFunc>::~PtrSet(){
		this->Allocator.Clear();
	}

	extern IntrvTreeNode * IntrvTree_Insert(IntrvTree *lpTree,BFLOAT LBound,BFLOAT RBound,void *Target){
		if (nullptr == lpTree->RootNode){
			if (nullptr == (lpTree->RootNode = (IntrvTreeNode *)lpTree->Allocator.Alloc_Bin(sizeof(IntrvTreeNode))))return NULL;
			lpTree->RootNode->LChild=nullptr;
			lpTree->RootNode->RChild=nullptr;
			lpTree->RootNode->Parent=nullptr;
			lpTree->RootNode->Color=0;
			lpTree->RootNode->LBound=LBound;
			lpTree->RootNode->RBound=RBound;
			lpTree->RootNode->RefRBound=RBound;
			lpTree->RootNode->Ptr=Target;
			return lpTree->RootNode;
		}
		else{
			IntrvTreeNode *ParentNode = lpTree->RootNode, *CurNode = (IntrvTreeNode *)lpTree->Allocator.Alloc_Bin(sizeof(IntrvTreeNode)), *TmpNode, *UncleNode, *RetValue;
			UBINT CmpResult;
			BFLOAT TmpRBound;

			if(NULL==CurNode)return NULL;
			CurNode->LChild=NULL;
			CurNode->RChild=NULL;
			CurNode->Color=1; //red
			CurNode->LBound=LBound;
			CurNode->RBound=RBound;
			CurNode->RefRBound=RBound;
			CurNode->Ptr=Target;
			while(1){
				if(LBound>ParentNode->LBound || (LBound==ParentNode->LBound && RBound>ParentNode->RBound))CmpResult=1;else CmpResult=0;
				if(CmpResult)TmpNode=ParentNode->RChild;else TmpNode=ParentNode->LChild;
				if(NULL==TmpNode){
					if(CmpResult)ParentNode->RChild=CurNode;else ParentNode->LChild=CurNode;
					CurNode->Parent=ParentNode;
					RetValue=CurNode;
					break;
				}
				else ParentNode=TmpNode;
			}
			RetValue=CurNode;

			//Balancing
			while(1){
				if(1==ParentNode->Color){
					//Get uncle node
					TmpNode=ParentNode->Parent;
					if(TmpNode->LChild==ParentNode){
						UncleNode=TmpNode->RChild;
						if(NULL==UncleNode || 0==UncleNode->Color){
							if(ParentNode->RChild==CurNode){
								//Case L4
								CurNode->Color=0;
								TmpNode->Color=1;

								ParentNode->RChild=CurNode->LChild;
								ParentNode->Parent=CurNode;
								CurNode->LChild=ParentNode;

								TmpNode->LChild=CurNode->RChild;
								CurNode->RChild=TmpNode;
								CurNode->Parent=TmpNode->Parent;
								TmpNode->Parent=CurNode;

								if(nullptr!=ParentNode->RChild)ParentNode->RChild->Parent=ParentNode;
								if(nullptr!=TmpNode->LChild)TmpNode->LChild->Parent=TmpNode;

								ParentNode->RefRBound=ParentNode->RBound;
								if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;
								if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;

								TmpNode->RefRBound=TmpNode->RBound;
								if(nullptr!=TmpNode->LChild && TmpNode->LChild->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=TmpNode->LChild->RefRBound;
								if (nullptr != UncleNode && UncleNode->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound = UncleNode->RefRBound;

								CurNode->RefRBound=CurNode->RBound;
								if(ParentNode->RefRBound>CurNode->RefRBound)CurNode->RefRBound=ParentNode->RefRBound;
								if(TmpNode->RefRBound>CurNode->RefRBound)CurNode->RefRBound=TmpNode->RefRBound;

								if(nullptr==CurNode->Parent)lpTree->RootNode=CurNode;
								else if(CurNode->Parent->LChild==TmpNode)CurNode->Parent->LChild=CurNode;
								else CurNode->Parent->RChild=CurNode;
							}
							else{
								// Case L5
								ParentNode->Color=0;
								TmpNode->Color=1;

								TmpNode->LChild=ParentNode->RChild;
								ParentNode->RChild=TmpNode;
								ParentNode->Parent=TmpNode->Parent;
								TmpNode->Parent=ParentNode;

								if(nullptr!=TmpNode->LChild)TmpNode->LChild->Parent=TmpNode;

								TmpNode->RefRBound=TmpNode->RBound;
								if(nullptr!=TmpNode->LChild && TmpNode->LChild->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=TmpNode->LChild->RefRBound;
								if (nullptr != UncleNode && UncleNode->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound = UncleNode->RefRBound;

								ParentNode->RefRBound=ParentNode->RBound;
								if(CurNode->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=CurNode->RefRBound;
								if(TmpNode->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=TmpNode->RefRBound;

								if(nullptr==ParentNode->Parent)lpTree->RootNode=ParentNode;
								else if(ParentNode->Parent->LChild==TmpNode)ParentNode->Parent->LChild=ParentNode;
								else ParentNode->Parent->RChild=ParentNode;
							}
							break;
						}
						else{
							//Case L3
							ParentNode->Color=0;
							UncleNode->Color=0;
							TmpNode->Color=1;
							if(ParentNode->RefRBound<CurNode->RefRBound)ParentNode->RefRBound=CurNode->RefRBound;
							if(TmpNode->RefRBound<CurNode->RefRBound)TmpNode->RefRBound=CurNode->RefRBound;
							CurNode=TmpNode;
							if (nullptr == CurNode->Parent){ CurNode->Color = 0; break; }
							else ParentNode = CurNode->Parent;
						}
					}
					else{
						UncleNode=TmpNode->LChild;
						if(NULL==UncleNode || 0==UncleNode->Color){
							if(ParentNode->LChild==CurNode){
								//Case R4
								CurNode->Color=0;
								TmpNode->Color=1;

								ParentNode->LChild=CurNode->RChild;
								ParentNode->Parent=CurNode;
								CurNode->RChild=ParentNode;

								TmpNode->RChild=CurNode->LChild;
								CurNode->LChild=TmpNode;
								CurNode->Parent=TmpNode->Parent;
								TmpNode->Parent=CurNode;

								if(nullptr!=ParentNode->LChild)ParentNode->LChild->Parent=ParentNode;
								if(nullptr!=TmpNode->RChild)(TmpNode->RChild)->Parent=TmpNode;

								ParentNode->RefRBound=ParentNode->RBound;
								if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;
								if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;

								TmpNode->RefRBound=TmpNode->RBound;
								if(nullptr!=TmpNode->RChild && (TmpNode->RChild)->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=(TmpNode->RChild)->RefRBound;
								if (nullptr != UncleNode && UncleNode->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound = UncleNode->RefRBound;

								CurNode->RefRBound=CurNode->RBound;
								if(ParentNode->RefRBound>CurNode->RefRBound)CurNode->RefRBound=ParentNode->RefRBound;
								if(TmpNode->RefRBound>CurNode->RefRBound)CurNode->RefRBound=TmpNode->RefRBound;

								if (nullptr == CurNode->Parent)lpTree->RootNode = CurNode;
								else if(CurNode->Parent->RChild==TmpNode)CurNode->Parent->RChild=CurNode;
								else CurNode->Parent->LChild=CurNode;
							}
							else{
								// Case R5
								ParentNode->Color=0;
								TmpNode->Color=1;

								TmpNode->RChild=ParentNode->LChild;
								ParentNode->LChild=TmpNode;
								ParentNode->Parent=TmpNode->Parent;
								TmpNode->Parent=ParentNode;

								if(nullptr!=TmpNode->RChild)(TmpNode->RChild)->Parent=TmpNode;

								TmpNode->RefRBound=TmpNode->RBound;
								if(nullptr!=TmpNode->RChild && (TmpNode->RChild)->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=(TmpNode->RChild)->RefRBound;
								if(NULL!=UncleNode && UncleNode->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=UncleNode->RefRBound;

								ParentNode->RefRBound=ParentNode->RBound;
								if(CurNode->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=CurNode->RefRBound;
								if(TmpNode->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=TmpNode->RefRBound;

								if(NULL==ParentNode->Parent)lpTree->RootNode=ParentNode;
								else if(ParentNode->Parent->RChild==TmpNode)ParentNode->Parent->RChild=ParentNode;
								else ParentNode->Parent->LChild=ParentNode;
								CurNode=ParentNode;
							}
							break;
						}
						else{
							//Case R3
							ParentNode->Color=0;
							UncleNode->Color=0;
							TmpNode->Color=1;
							if(ParentNode->RefRBound<CurNode->RefRBound)ParentNode->RefRBound=CurNode->RefRBound;
							if(TmpNode->RefRBound<CurNode->RefRBound)TmpNode->RefRBound=CurNode->RefRBound;
							CurNode=TmpNode;
							if(NULL==CurNode->Parent){CurNode->Color=0;break;}else ParentNode=CurNode->Parent;
						}
					}
				}
				else break;
			}
			TmpRBound=CurNode->RefRBound;
			while(nullptr!=CurNode->Parent){
				CurNode=CurNode->Parent;
				if(CurNode->RefRBound<TmpRBound)CurNode->RefRBound=TmpRBound;
			}
			return RetValue;
		}
		
	}
	extern void IntrvTree_Delete(IntrvTree *lpTree,IntrvTreeNode *Target){
		if(nullptr!=lpTree->RootNode){
			IntrvTreeNode *CurNode = Target, *ParentNode, *TmpNode, *SibNode, *NephewNode, *SwapPtr;
			UBINT SwapBuffer;

			if(nullptr!=CurNode->LChild && nullptr!=CurNode->RChild){
				TmpNode=CurNode->LChild;
				if(nullptr==TmpNode->RChild){
					//Node swap
					SwapPtr = CurNode->Parent;
					CurNode->Parent=CurNode->LChild;
					CurNode->LChild=TmpNode->LChild;
					TmpNode->LChild=TmpNode->Parent;
					TmpNode->Parent = SwapPtr;
					SwapPtr = CurNode->RChild;
					CurNode->RChild=TmpNode->RChild;
					TmpNode->RChild = SwapPtr;
					SwapBuffer=CurNode->Color;
					CurNode->Color=TmpNode->Color;
					TmpNode->Color=SwapBuffer;

					ParentNode=TmpNode->Parent;
					if(NULL==ParentNode)lpTree->RootNode=TmpNode;
					else if(ParentNode->LChild==CurNode)ParentNode->LChild=TmpNode;
					else ParentNode->RChild=TmpNode;

					ParentNode=CurNode->LChild;
					if(NULL!=ParentNode)ParentNode->Parent=CurNode;

					ParentNode=TmpNode->RChild;
					ParentNode->Parent=TmpNode; //TmpNode must have two children.
					//Now CurNode couldn't have an RChild.
				}
				else{
					while(nullptr!=TmpNode->RChild)TmpNode=TmpNode->RChild;
					//Node swap
					SwapPtr = CurNode->Parent;
					CurNode->Parent=TmpNode->Parent;
					TmpNode->Parent = SwapPtr;
					SwapPtr = CurNode->LChild;
					CurNode->LChild=TmpNode->LChild;
					TmpNode->LChild = SwapPtr;
					SwapPtr = CurNode->RChild;
					CurNode->RChild=TmpNode->RChild;
					TmpNode->RChild = SwapPtr;
					SwapBuffer=CurNode->Color;
					CurNode->Color=TmpNode->Color;
					TmpNode->Color=SwapBuffer;

					ParentNode=TmpNode->Parent;
					if(NULL==ParentNode)lpTree->RootNode=TmpNode;
					else if(ParentNode->LChild==CurNode)ParentNode->LChild=TmpNode;
					else ParentNode->RChild=TmpNode;
					ParentNode=CurNode->Parent;
					if(ParentNode->LChild==TmpNode)ParentNode->LChild=CurNode;
					else ParentNode->RChild=CurNode;

					ParentNode=TmpNode->LChild;
					ParentNode->Parent=TmpNode;  //TmpNode must have two children.
					ParentNode=CurNode->LChild;
					if(NULL!=ParentNode)ParentNode->Parent=CurNode;

					ParentNode=TmpNode->RChild;
					ParentNode->Parent=TmpNode;  //TmpNode must have two children.
					//Now CurNode couldn't have an RChild.
				}
			}
			if(nullptr==CurNode->Parent){
				if(nullptr==CurNode->LChild)TmpNode=CurNode->RChild;else TmpNode=CurNode->LChild;
				if(NULL!=TmpNode){
					TmpNode->Parent=NULL;
					TmpNode->Color=0;
				}
				lpTree->RootNode=TmpNode;
			}
			else{
				ParentNode=CurNode->Parent;
				if(nullptr==CurNode->LChild)TmpNode=CurNode->RChild;else TmpNode=CurNode->LChild;
				if(ParentNode->LChild==CurNode)ParentNode->LChild=TmpNode;else ParentNode->RChild=TmpNode;
				if (nullptr != TmpNode)TmpNode->Parent = ParentNode;
				if(0==CurNode->Color){
					//Balancing
					if(nullptr!=TmpNode)TmpNode->Color=0; //if TmpNode exists,it must be red.
					else while(1){
						//Get sibling node as SibNode
						if(ParentNode->LChild==TmpNode){ //if TmpNode doesn't exist,then the sibling node must exist.
							SibNode=ParentNode->RChild;
							if(1==SibNode->Color){
								//Case L2:CP Length
								NephewNode=SibNode->LChild;

								ParentNode->Color=0; //After L5/L6 spin,its color will become black.
								ParentNode->RChild=NephewNode;
								NephewNode->Parent=ParentNode;
								SibNode->Color=0;
								SibNode->LChild=ParentNode;
								SibNode->Parent=ParentNode->Parent;
								ParentNode->Parent=SibNode;

								if(nullptr!=NephewNode->RChild && 1==NephewNode->RChild->Color){
									//Case L2-L6
									NephewNode->RChild->Color=0;
									SibNode->LChild=NephewNode;
									NephewNode->Parent=SibNode;
									ParentNode->RChild = NephewNode->LChild;
									NephewNode->LChild=ParentNode;
									ParentNode->Parent=NephewNode;
									NephewNode->Color=1;

									if(nullptr!=ParentNode->RChild)ParentNode->RChild->Parent=ParentNode;

									ParentNode->RefRBound=ParentNode->RBound;
									if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;
									if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;

									NephewNode->RefRBound=NephewNode->RBound;
									if(ParentNode->RefRBound>NephewNode->RefRBound)NephewNode->RefRBound=ParentNode->RefRBound;
									if(NephewNode->RChild->RefRBound>NephewNode->RefRBound)NephewNode->RefRBound=NephewNode->RChild->RefRBound;

									SibNode->RefRBound=SibNode->RBound;
									if(NephewNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=NephewNode->RefRBound;
									if(SibNode->RChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->RChild->RefRBound;
								}
								else if(nullptr!=NephewNode->LChild && 1==NephewNode->LChild->Color){
									//Case L2-L5
									IntrvTreeNode *TmpNode2=NephewNode->LChild;

									SibNode->LChild=TmpNode2;
									TmpNode2->Parent=SibNode;

									ParentNode->RChild=TmpNode2->LChild;
									TmpNode2->LChild=ParentNode;
									ParentNode->Parent=TmpNode2;
									NephewNode->LChild=TmpNode2->RChild;
									TmpNode2->RChild=NephewNode;
									NephewNode->Parent=TmpNode2;
									NephewNode->Color=0;

									if(nullptr!=ParentNode->RChild)ParentNode->RChild->Parent=ParentNode;
									if(nullptr!=NephewNode->LChild)NephewNode->LChild->Parent=NephewNode;

									ParentNode->RefRBound=ParentNode->RBound;
									if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;
									if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;

									NephewNode->RefRBound=NephewNode->RBound;
									if(nullptr!=NephewNode->LChild && NephewNode->LChild->RefRBound>NephewNode->RefRBound)NephewNode->RefRBound=NephewNode->LChild->RefRBound;
									if(nullptr!=NephewNode->RChild && NephewNode->RChild->RefRBound>NephewNode->RefRBound)NephewNode->RefRBound=NephewNode->RChild->RefRBound;

									TmpNode2->RefRBound=TmpNode2->RBound;
									if(ParentNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=ParentNode->RefRBound;
									if(NephewNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=NephewNode->RefRBound;

									SibNode->RefRBound=SibNode->RBound;
									if(TmpNode2->RefRBound>SibNode->RefRBound)SibNode->RefRBound=TmpNode2->RefRBound;
									if(SibNode->RChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->RChild->RefRBound;

								}
								else{
									NephewNode->Color=1;

									ParentNode->RefRBound=ParentNode->RBound;
									if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;
									if(NephewNode->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=NephewNode->RefRBound;

									SibNode->RefRBound=SibNode->RBound;
									if(ParentNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=ParentNode->RefRBound;
									if(SibNode->RChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->RChild->RefRBound;
								}
								if(nullptr==SibNode->Parent)lpTree->RootNode=SibNode;
								else if(SibNode->Parent->RChild==ParentNode)SibNode->Parent->RChild=SibNode;
								else SibNode->Parent->LChild=SibNode;

								TmpNode=SibNode;
								break;
							}
							else if(nullptr!=SibNode->RChild && 1==SibNode->RChild->Color){
								//Case L6
								SibNode->RChild->Color=0;

								SibNode->Color=ParentNode->Color;
								ParentNode->RChild=SibNode->LChild;
								ParentNode->Color=0;
								SibNode->LChild=ParentNode;
								SibNode->Parent=ParentNode->Parent;
								ParentNode->Parent=SibNode;

								if(nullptr!=ParentNode->RChild)ParentNode->RChild->Parent=ParentNode;

								ParentNode->RefRBound=ParentNode->RBound;
								if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;
								if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;

								SibNode->RefRBound=SibNode->RBound;
								if(ParentNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=ParentNode->RefRBound;
								if(SibNode->RChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->RChild->RefRBound;

								if(nullptr==SibNode->Parent)lpTree->RootNode=SibNode;
								else if(SibNode->Parent->RChild==ParentNode)SibNode->Parent->RChild=SibNode;
								else SibNode->Parent->LChild=SibNode;

								TmpNode=SibNode;
								break;
							}
							else if(nullptr!=SibNode->LChild && 1==SibNode->LChild->Color){
								//Case L5
								IntrvTreeNode *TmpNode2=SibNode->LChild;

								SibNode->LChild=TmpNode2->RChild;
								ParentNode->RChild=TmpNode2->LChild;
								TmpNode2->RChild=SibNode;
								TmpNode2->LChild=ParentNode;
								TmpNode2->Color=ParentNode->Color;
								TmpNode2->Parent=ParentNode->Parent;
								ParentNode->Parent=TmpNode2;
								SibNode->Parent=TmpNode2;
								ParentNode->Color=0;
								// SibNode->Color=0;

								if(nullptr!=ParentNode->RChild)ParentNode->RChild->Parent=ParentNode;
								if(nullptr!=SibNode->LChild)SibNode->LChild->Parent=SibNode;

								ParentNode->RefRBound=ParentNode->RBound;
								if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;
								if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;

								SibNode->RefRBound=SibNode->RBound;
								if(nullptr!=SibNode->LChild && SibNode->LChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->LChild->RefRBound;
								if(nullptr!=SibNode->RChild && SibNode->RChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->RChild->RefRBound;

								TmpNode2->RefRBound=TmpNode2->RBound;
								if(ParentNode->RefRBound>TmpNode2->RefRBound)TmpNode2->RefRBound=ParentNode->RefRBound;
								if(SibNode->RefRBound>TmpNode2->RefRBound)TmpNode2->RefRBound=SibNode->RefRBound;

								if(nullptr==TmpNode2->Parent)lpTree->RootNode=TmpNode2;
								else if(TmpNode2->Parent->RChild==ParentNode)TmpNode2->Parent->RChild=TmpNode2;
								else TmpNode2->Parent->LChild=TmpNode2;

								TmpNode=TmpNode2;
								break;
							}
							else{
								// Case L3,L4
								ParentNode->RefRBound=ParentNode->RBound;
								if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;
								if(SibNode->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=SibNode->RefRBound;

								TmpNode=ParentNode;

								SibNode->Color=1;
								if(1==ParentNode->Color){ParentNode->Color=0;break;}
								if(nullptr==ParentNode->Parent)break;

								ParentNode=ParentNode->Parent;
							}
						}
						else{
							SibNode=ParentNode->LChild;
							if(1==SibNode->Color){
								//Case R2
								NephewNode=SibNode->RChild;

								ParentNode->Color=0; //After R5/R6 spin,its color will become black.
								ParentNode->LChild=NephewNode;
								NephewNode->Parent=ParentNode;
								SibNode->Color=0;
								SibNode->RChild=ParentNode;
								SibNode->Parent=ParentNode->Parent;
								ParentNode->Parent=SibNode;

								if(nullptr!=NephewNode->LChild && 1==NephewNode->LChild->Color){
									//Case R2-R6
									NephewNode->LChild->Color=0;
									SibNode->RChild=NephewNode;
									NephewNode->Parent=SibNode;
									ParentNode->LChild = NephewNode->RChild;
									NephewNode->RChild=ParentNode;
									ParentNode->Parent=NephewNode;
									NephewNode->Color=1;

									if(nullptr!=ParentNode->LChild)ParentNode->LChild->Parent=ParentNode;

									ParentNode->RefRBound=ParentNode->RBound;
									if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;
									if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;

									NephewNode->RefRBound=NephewNode->RBound;
									if(ParentNode->RefRBound>NephewNode->RefRBound)NephewNode->RefRBound=ParentNode->RefRBound;
									if(NephewNode->LChild->RefRBound>NephewNode->RefRBound)NephewNode->RefRBound=NephewNode->LChild->RefRBound;

									SibNode->RefRBound=SibNode->RBound;
									if(NephewNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=NephewNode->RefRBound;
									if(SibNode->LChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->LChild->RefRBound;
								}
								else if(nullptr!=NephewNode->RChild && 1==NephewNode->RChild->Color){
									//Case R2-R5
									IntrvTreeNode *TmpNode2=NephewNode->RChild;

									SibNode->RChild=TmpNode2;
									TmpNode2->Parent=SibNode;

									ParentNode->LChild=TmpNode2->RChild;
									TmpNode2->RChild=ParentNode;
									ParentNode->Parent=TmpNode2;
									NephewNode->RChild=TmpNode2->LChild;
									TmpNode2->LChild=NephewNode;
									NephewNode->Parent=TmpNode2;
									NephewNode->Color=0;

									if(nullptr!=ParentNode->LChild)ParentNode->LChild->Parent=ParentNode;
									if(nullptr!=NephewNode->RChild)NephewNode->RChild->Parent=NephewNode;

									ParentNode->RefRBound=ParentNode->RBound;
									if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;
									if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;

									NephewNode->RefRBound=NephewNode->RBound;
									if(nullptr!=NephewNode->RChild && NephewNode->RChild->RefRBound>NephewNode->RefRBound)NephewNode->RefRBound=NephewNode->RChild->RefRBound;
									if(nullptr!=NephewNode->LChild && NephewNode->LChild->RefRBound>NephewNode->RefRBound)NephewNode->RefRBound=NephewNode->LChild->RefRBound;

									TmpNode2->RefRBound=TmpNode2->RBound;
									if(ParentNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=ParentNode->RefRBound;
									if(NephewNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=NephewNode->RefRBound;

									SibNode->RefRBound=SibNode->RBound;
									if(TmpNode2->RefRBound>SibNode->RefRBound)SibNode->RefRBound=TmpNode2->RefRBound;
									if(SibNode->LChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->LChild->RefRBound;
								}
								else{
									NephewNode->Color=1;

									ParentNode->RefRBound=ParentNode->RBound;
									if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;
									if(NephewNode->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=NephewNode->RefRBound;

									SibNode->RefRBound=SibNode->RBound;
									if(ParentNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=ParentNode->RefRBound;
									if(SibNode->LChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->LChild->RefRBound;
								}
								if(nullptr==SibNode->Parent)lpTree->RootNode=SibNode;
								else if(SibNode->Parent->LChild==ParentNode)SibNode->Parent->LChild=SibNode;
								else SibNode->Parent->RChild=SibNode;

								TmpNode=SibNode;
								break;
							}
							else if(nullptr!=SibNode->LChild && 1==SibNode->LChild->Color){
								//Case R6
								SibNode->LChild->Color=0;

								SibNode->Color=ParentNode->Color;
								ParentNode->LChild=SibNode->RChild;
								ParentNode->Color=0;
								SibNode->RChild=ParentNode;
								SibNode->Parent=ParentNode->Parent;
								ParentNode->Parent=SibNode;

								if(nullptr!=ParentNode->LChild)ParentNode->LChild->Parent=ParentNode;

								ParentNode->RefRBound=ParentNode->RBound;
								if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;
								if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;

								SibNode->RefRBound=SibNode->RBound;
								if(ParentNode->RefRBound>SibNode->RefRBound)SibNode->RefRBound=ParentNode->RefRBound;
								if(SibNode->LChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->LChild->RefRBound;

								if(nullptr==SibNode->Parent)lpTree->RootNode=SibNode;
								else if(SibNode->Parent->LChild==ParentNode)SibNode->Parent->LChild=SibNode;
								else SibNode->Parent->RChild=SibNode;

								TmpNode=SibNode;
								break;
							}
							else if(nullptr!=SibNode->RChild && 1==SibNode->RChild->Color){
								//Case R5
								IntrvTreeNode *TmpNode2=SibNode->RChild;

								SibNode->RChild=TmpNode2->LChild;
								ParentNode->LChild=TmpNode2->RChild;
								TmpNode2->LChild=SibNode;
								TmpNode2->RChild=ParentNode;
								TmpNode2->Color=ParentNode->Color;
								TmpNode2->Parent=ParentNode->Parent;
								ParentNode->Parent=TmpNode2;
								SibNode->Parent=TmpNode2;
								ParentNode->Color=0;
								// SibNode->Color=0;

								if(nullptr!=ParentNode->LChild)ParentNode->LChild->Parent=ParentNode;
								if(nullptr!=SibNode->RChild)SibNode->RChild->Parent=SibNode;

								ParentNode->RefRBound=ParentNode->RBound;
								if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;
								if(nullptr!=ParentNode->LChild && ParentNode->LChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->LChild->RefRBound;

								SibNode->RefRBound=SibNode->RBound;
								if(nullptr!=SibNode->RChild && SibNode->RChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->RChild->RefRBound;
								if(nullptr!=SibNode->LChild && SibNode->LChild->RefRBound>SibNode->RefRBound)SibNode->RefRBound=SibNode->LChild->RefRBound;

								TmpNode2->RefRBound=TmpNode2->RBound;
								if(ParentNode->RefRBound>TmpNode2->RefRBound)TmpNode2->RefRBound=ParentNode->RefRBound;
								if(SibNode->RefRBound>TmpNode2->RefRBound)TmpNode2->RefRBound=SibNode->RefRBound;

								if(nullptr==TmpNode2->Parent)lpTree->RootNode=TmpNode2;
								else if(TmpNode2->Parent->LChild==ParentNode)TmpNode2->Parent->LChild=TmpNode2;
								else TmpNode2->Parent->RChild=TmpNode2;

								TmpNode=TmpNode2;
								break;
							}
							else{
								// Case R3,R4
								ParentNode->RefRBound=ParentNode->RBound;
								if(nullptr!=ParentNode->RChild && ParentNode->RChild->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=ParentNode->RChild->RefRBound;
								if(SibNode->RefRBound>ParentNode->RefRBound)ParentNode->RefRBound=SibNode->RefRBound;

								TmpNode=ParentNode;

								SibNode->Color=1;
								if(1==ParentNode->Color){ParentNode->Color=0;break;}
								if(nullptr==ParentNode->Parent)break;

								ParentNode=ParentNode->Parent;
							}
						}
					}
				}
				else{
					TmpNode=ParentNode;
					TmpNode->RefRBound=TmpNode->RBound;
					if(nullptr!=TmpNode->LChild && TmpNode->LChild->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=TmpNode->LChild->RefRBound;
					if(nullptr!=TmpNode->RChild && TmpNode->RChild->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=TmpNode->RChild->RefRBound;
				}
				while(nullptr!=TmpNode->Parent){
					TmpNode=TmpNode->Parent;
					TmpNode->RefRBound=TmpNode->RBound;
					if(nullptr!=TmpNode->LChild && TmpNode->LChild->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=TmpNode->LChild->RefRBound;
					if(nullptr!=TmpNode->RChild && TmpNode->RChild->RefRBound>TmpNode->RefRBound)TmpNode->RefRBound=TmpNode->RChild->RefRBound;
				}
			}
			lpTree->Allocator.Free_Bin(CurNode, sizeof(IntrvTreeNode));
		}
	}
	extern void IntrvTree_PointQuery(IntrvTree *lpSet,BFLOAT Point,IntrvTreeCallback cbFunc){
		if(NULL!=(IntrvTreeNode *)lpSet->RootNode){
			IntrvTreeNode *CurNode;
			UBINT TmpStack[MaxRBTreeDepth],TmpStackPtr=0;

			CurNode=(IntrvTreeNode *)lpSet->RootNode;
			if(Point>=CurNode->LBound && Point<CurNode->RBound)cbFunc(CurNode);
			TmpStack[0]=0;
			while(1){
				if(0==TmpStack[TmpStackPtr]){
					TmpStack[TmpStackPtr]++;
					if(nullptr!=CurNode->LChild && (CurNode->LChild)->RefRBound>Point){
						CurNode=CurNode->LChild;
						if(Point>=CurNode->LBound && Point<CurNode->RBound)cbFunc(CurNode);
						TmpStackPtr++;
						TmpStack[TmpStackPtr]=0;
						continue;
					}
				}
				if(1==TmpStack[TmpStackPtr]){
					TmpStack[TmpStackPtr]++;
					if(nullptr!=CurNode->RChild && CurNode->RChild->RefRBound>Point){
						CurNode=CurNode->RChild;
						if(Point>=CurNode->LBound && Point<CurNode->RBound)cbFunc(CurNode);
						TmpStackPtr++;
						TmpStack[TmpStackPtr]=0;
						continue;
					}
				}
				if(TmpStack[TmpStackPtr]>=2){
					if(0==TmpStackPtr)return;
					CurNode=CurNode->Parent;
					TmpStackPtr--;
				}
			}
		}
	}
	extern void IntrvTree_SegQuery(IntrvTree *lpSet,BFLOAT LBound,BFLOAT RBound,IntrvTreeCallback cbFunc){
		if(NULL!=(IntrvTreeNode *)lpSet->RootNode){
			IntrvTreeNode *CurNode;
			UBINT TmpStack[MaxRBTreeDepth],TmpStackPtr=0;

			CurNode=(IntrvTreeNode *)lpSet->RootNode;
			if(RBound>CurNode->LBound && LBound<CurNode->RBound)cbFunc(CurNode);
			TmpStack[0]=0;
			while(1){
				if(0==TmpStack[TmpStackPtr]){
					TmpStack[TmpStackPtr]++;
					if(nullptr!=CurNode->LChild && (CurNode->LChild)->RefRBound>LBound){
						CurNode=CurNode->LChild;
						if(RBound>CurNode->LBound && LBound<CurNode->RBound)cbFunc(CurNode);
						TmpStackPtr++;
						TmpStack[TmpStackPtr]=0;
						continue;
					}
				}
				if(1==TmpStack[TmpStackPtr]){
					TmpStack[TmpStackPtr]++;
					if(nullptr!=CurNode->RChild && CurNode->RChild->RefRBound>LBound){
						CurNode=CurNode->RChild;
						if(RBound>CurNode->LBound && LBound<CurNode->RBound)cbFunc(CurNode);
						TmpStackPtr++;
						TmpStack[TmpStackPtr]=0;
						continue;
					}
				}
				if(TmpStack[TmpStackPtr]>=2){
					if(0==TmpStackPtr)return;
					CurNode=CurNode->Parent;
					TmpStackPtr--;
				}
			}
		}
	}
	IntrvTree::IntrvTree(){
		this->Allocator = { nullptr };
		this->RootNode = nullptr;
		this->Type = OBJ_INTRVTREE;
		this->MsgProc = nullptr;
		this->ThreadLock = (UBINT)nullptr;
	}
	void IntrvTree::Clear(){
		this->Allocator.Clear();
		this->RootNode = nullptr;
	}
	IntrvTree::~IntrvTree(){
		this->Allocator.Clear();
	}

	extern UBINT _cdecl MsgProc_Octree(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		if(MSG_DESTROY==Msg){
			((Octree *)lpObj)->Allocator.Clear();
		}
		return 0;
	}
	OctreeNode * Octree::Insert(BFLOAT x,BFLOAT y,BFLOAT z){
		OctreeNode *ParentNode=NULL,**ChildPtr=(OctreeNode **)&(this->RootNode);
		BFLOAT NewSpan,TmpSpan;
		UBINT ChildIndex;
		while(1){
			if(nullptr==*ChildPtr){
				if (nullptr == (*ChildPtr = (OctreeNode *)this->Allocator.Alloc(sizeof(OctreeNode))))return nullptr;
				(*ChildPtr)->Pos[0]=x;
				(*ChildPtr)->Pos[1]=y;
				(*ChildPtr)->Pos[2]=z;
				(*ChildPtr)->Span=0.0;
				(*ChildPtr)->lpParent=ParentNode;
				(*ChildPtr)->lpChildren=nullptr;
				(*ChildPtr)->ChildInfo=0;
				return *ChildPtr;
			}
			else{
				if(0.0==(*ChildPtr)->Span){
					if (x == (*ChildPtr)->Pos[0] && y == (*ChildPtr)->Pos[1] && z == (*ChildPtr)->Pos[2])return nullptr;
					ChildIndex=8;
				}
				else{
					ChildIndex=0;
					if(x-(*ChildPtr)->Pos[0]<0){
						if(x-(*ChildPtr)->Pos[0]<-(*ChildPtr)->Span)ChildIndex+=8;
					}
					else{
						ChildIndex+=1;
						if(x-(*ChildPtr)->Pos[0]>=(*ChildPtr)->Span)ChildIndex+=8;
					}
					if(y-(*ChildPtr)->Pos[1]<0){
						if(y-(*ChildPtr)->Pos[1]<-(*ChildPtr)->Span)ChildIndex+=8;
					}
					else{
						ChildIndex+=2;
						if(y-(*ChildPtr)->Pos[1]>=(*ChildPtr)->Span)ChildIndex+=8;
					}
					if(z-(*ChildPtr)->Pos[2]<0){
						if(z-(*ChildPtr)->Pos[2]<-(*ChildPtr)->Span)ChildIndex+=8;
					}
					else{
						ChildIndex+=4;
						if(z-(*ChildPtr)->Pos[2]>=(*ChildPtr)->Span)ChildIndex+=8;
					}
				}
				if(ChildIndex>=8){
					OctreeNode *NewNode1,*NewNode2;
					OctreeNode *(*NodeIndex)[8];
					NewNode1 = (OctreeNode *)this->Allocator.Alloc(sizeof(OctreeNode));
					if(NULL!=NewNode1){
						NewNode2 = (OctreeNode *)this->Allocator.Alloc(sizeof(OctreeNode));
						if (NULL != NewNode2){
							NodeIndex = (OctreeNode *(*)[8])this->Allocator.Alloc(8 * sizeof(OctreeNode *));
							if (NULL != NodeIndex){
								NewSpan = 0;
								TmpSpan = roundfloat2(x, (*ChildPtr)->Pos[0]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
								TmpSpan = roundfloat2(y, (*ChildPtr)->Pos[1]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
								TmpSpan = roundfloat2(z, (*ChildPtr)->Pos[2]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
								NewNode2->Pos[0] = roundfloat(x, NewSpan);
								NewNode2->Pos[1] = roundfloat(y, NewSpan);
								NewNode2->Pos[2] = roundfloat(z, NewSpan);
								NewNode2->Span = NewSpan;
								NewNode2->lpParent = ParentNode;
								NewNode2->lpChildren = NodeIndex;
								NewNode2->ChildInfo = 0;

								for (UBINT i = 0; i < 8; i++)(*NodeIndex)[i] = 0;
								ChildIndex = 0;
								if ((*ChildPtr)->Pos[0] >= NewNode2->Pos[0])ChildIndex += 1;
								if ((*ChildPtr)->Pos[1] >= NewNode2->Pos[1])ChildIndex += 2;
								if ((*ChildPtr)->Pos[2] >= NewNode2->Pos[2])ChildIndex += 4;
								(*NodeIndex)[ChildIndex] = (*ChildPtr);
								(*ChildPtr)->lpParent = NewNode2;

								ChildIndex = 0;
								if (x >= NewNode2->Pos[0])ChildIndex += 1;
								if (y >= NewNode2->Pos[1])ChildIndex += 2;
								if (z >= NewNode2->Pos[2])ChildIndex += 4;
								(*NodeIndex)[ChildIndex] = NewNode1;
								NewNode1->Pos[0] = x;
								NewNode1->Pos[1] = y;
								NewNode1->Pos[2] = z;
								NewNode1->Span = 0.0;
								NewNode1->lpParent = NewNode2;
								NewNode1->lpChildren = nullptr;
								NewNode1->ChildInfo = 0;

								*ChildPtr = NewNode2;
								return NewNode1;
							}
							this->Allocator.Free(NewNode2, sizeof(OctreeNode));
						}
						this->Allocator.Free(NewNode1, sizeof(OctreeNode));
					}
					return NULL;
				}
				else{
					ParentNode=*ChildPtr;
					ChildPtr=((OctreeNode **)ParentNode->lpChildren)+ChildIndex;
				}
			}
		}
	}
	void Octree::SphericalQuery(BFLOAT x,BFLOAT y,BFLOAT z,BFLOAT DistLimit,OctreeDistCallback cbFunc){
		if(nullptr!=this->RootNode){
			UBINT TmpStack[MaxOctreeDepth],*TmpStackPtr=TmpStack;
			BFLOAT DistLimitSqr=DistLimit*DistLimit,MaxDist,CurDist;
			OctreeNode *TmpNode=this->RootNode,**ChildPtr;

			while(1){
				if(0.0==TmpNode->Span){
					CurDist=(TmpNode->Pos[0]-x)*(TmpNode->Pos[0]-x)+(TmpNode->Pos[1]-y)*(TmpNode->Pos[1]-y)+(TmpNode->Pos[2]-z)*(TmpNode->Pos[2]-z);
					if(CurDist<=DistLimitSqr)cbFunc(TmpNode,CurDist);
					*TmpStackPtr=8;
				}
				else{
					MaxDist=DistLimitSqr+2*M_SQRT3*DistLimit*TmpNode->Span+(BFLOAT)3.0*TmpNode->Span*TmpNode->Span;
					CurDist=(TmpNode->Pos[0]-x)*(TmpNode->Pos[0]-x)+(TmpNode->Pos[1]-y)*(TmpNode->Pos[1]-y)+(TmpNode->Pos[2]-z)*(TmpNode->Pos[2]-z);
					if(CurDist<=MaxDist){
						ChildPtr=(OctreeNode **)TmpNode->lpChildren;
						*TmpStackPtr=0;
						while(*TmpStackPtr<8 && NULL==ChildPtr[*TmpStackPtr])(*TmpStackPtr)++;
					}
					else *TmpStackPtr=8;
				}
				while(*TmpStackPtr>=8){
					TmpNode=(OctreeNode *)TmpNode->lpParent;
					if(NULL==TmpNode)return;
					else{
						TmpStackPtr--;
						ChildPtr=(OctreeNode **)TmpNode->lpChildren;
						while(*TmpStackPtr<8 && NULL==ChildPtr[*TmpStackPtr])(*TmpStackPtr)++;
					}
				}
				TmpNode=ChildPtr[*TmpStackPtr];
				(*TmpStackPtr)++;
				TmpStackPtr++;
			}
		}
	}
	UBINT Octree::GetNearest(BFLOAT x,BFLOAT y,BFLOAT z,UBINT NearestCnt,OctreeNode **lpResult){

#if defined LIBENV_SYS_INTELX86
		const UBINT BFLOAT_INF_AS_INT=0x7F800000;
#elif defined LIBENV_SYS_INTELX64
		const UBINT BFLOAT_INF_AS_INT=0x7FF0000000000000;
#endif

		if(nullptr!=this->RootNode){
			UBINT TmpStack[2*MaxOctreeDepth],*TmpStackPtr=TmpStack;
			UBINT Quadrant,RetValue=0,TmpInt1,TmpInt2;
			BFLOAT DistLimit=*(BFLOAT *)&BFLOAT_INF_AS_INT,DistLimitSqr=DistLimit*DistLimit,MaxDist,CurDist;
			OctreeNode *TmpNode=this->RootNode,**ChildPtr;

			while(1){
				if(0.0==TmpNode->Span){
					CurDist=(TmpNode->Pos[0]-x)*(TmpNode->Pos[0]-x)+(TmpNode->Pos[1]-y)*(TmpNode->Pos[1]-y)+(TmpNode->Pos[2]-z)*(TmpNode->Pos[2]-z);
					if(CurDist<DistLimitSqr){
						if(RetValue<NearestCnt){
							//Add TmpNode to the bottom of the heap
							lpResult[RetValue]=TmpNode;
							RetValue++;
							TmpInt1=RetValue;
							TmpInt2=TmpInt1>>1;
							while(TmpInt2>0){
								MaxDist=(lpResult[TmpInt2-1]->Pos[0]-x)*(lpResult[TmpInt2-1]->Pos[0]-x)+(lpResult[TmpInt2-1]->Pos[1]-y)*(lpResult[TmpInt2-1]->Pos[1]-y)+(lpResult[TmpInt2-1]->Pos[2]-z)*(lpResult[TmpInt2-1]->Pos[2]-z);
								if(MaxDist<CurDist){
									lpResult[TmpInt1-1]=lpResult[TmpInt2-1];
									lpResult[TmpInt2-1]=TmpNode;
								}
								else break;
								TmpInt1=TmpInt2;
								TmpInt2=TmpInt1>>1;
							}
							if(RetValue>=NearestCnt){
								DistLimitSqr=(lpResult[0]->Pos[0]-x)*(lpResult[0]->Pos[0]-x)+(lpResult[0]->Pos[1]-y)*(lpResult[0]->Pos[1]-y)+(lpResult[0]->Pos[2]-z)*(lpResult[0]->Pos[2]-z);
								DistLimit=sqrt(DistLimitSqr);
							}
						}
						else{
							//Replace the top node of the heap
							DistLimitSqr=CurDist;
							lpResult[0]=TmpNode;
							TmpInt1=1;
							while(1){
								TmpInt2=TmpInt1<<1;
								if(TmpInt2<=NearestCnt){
									CurDist=(lpResult[TmpInt2-1]->Pos[0]-x)*(lpResult[TmpInt2-1]->Pos[0]-x)+(lpResult[TmpInt2-1]->Pos[1]-y)*(lpResult[TmpInt2-1]->Pos[1]-y)+(lpResult[TmpInt2-1]->Pos[2]-z)*(lpResult[TmpInt2-1]->Pos[2]-z);
									if(TmpInt2<NearestCnt){
										MaxDist=(lpResult[TmpInt2]->Pos[0]-x)*(lpResult[TmpInt2]->Pos[0]-x)+(lpResult[TmpInt2]->Pos[1]-y)*(lpResult[TmpInt2]->Pos[1]-y)+(lpResult[TmpInt2]->Pos[2]-z)*(lpResult[TmpInt2]->Pos[2]-z);
										if(MaxDist>CurDist){CurDist=MaxDist;TmpInt2++;}
									}
									if(DistLimitSqr<CurDist){
										lpResult[TmpInt1-1]=lpResult[TmpInt2-1];
										lpResult[TmpInt2-1]=TmpNode;
									}
									else break;
								}
								else break;
								TmpInt1=TmpInt2;
							}
							DistLimitSqr=(lpResult[0]->Pos[0]-x)*(lpResult[0]->Pos[0]-x)+(lpResult[0]->Pos[1]-y)*(lpResult[0]->Pos[1]-y)+(lpResult[0]->Pos[2]-z)*(lpResult[0]->Pos[2]-z);
							DistLimit=sqrt(DistLimitSqr);
						}
					}
					TmpStackPtr[0]=8;
				}
				else{
					MaxDist=DistLimitSqr+2*M_SQRT3*DistLimit*TmpNode->Span+3.0*TmpNode->Span*TmpNode->Span;
					CurDist=(TmpNode->Pos[0]-x)*(TmpNode->Pos[0]-x)+(TmpNode->Pos[1]-y)*(TmpNode->Pos[1]-y)+(TmpNode->Pos[2]-z)*(TmpNode->Pos[2]-z);
					if(CurDist<MaxDist){
						Quadrant=0;
						if(x>=TmpNode->Pos[0])Quadrant+=1;
						if(y>=TmpNode->Pos[1])Quadrant+=2;
						if(z>=TmpNode->Pos[2])Quadrant+=4;
						ChildPtr=(OctreeNode **)TmpNode->lpChildren;
						TmpStackPtr[0]=0;
						TmpStackPtr[1]=Quadrant;
						while(TmpStackPtr[0]<8 && NULL==ChildPtr[TmpStackPtr[0]^Quadrant])TmpStackPtr[0]++;
					}
					else TmpStackPtr[0]=8;
				}
				while(TmpStackPtr[0]>=8){
					TmpNode=(OctreeNode *)TmpNode->lpParent;
					if(NULL==TmpNode)return RetValue;
					else{
						TmpStackPtr-=2;
						Quadrant=TmpStackPtr[1];
						ChildPtr=(OctreeNode **)TmpNode->lpChildren;
						while(TmpStackPtr[0]<8 && NULL==ChildPtr[TmpStackPtr[0]^Quadrant])TmpStackPtr[0]++;
					}
				}
				TmpNode=ChildPtr[TmpStackPtr[0]^Quadrant];
				TmpStackPtr[0]++;
				TmpStackPtr+=2;
			}
		}
		else return 0;
	}
	Octree::Octree(){
		this->Allocator = { nullptr };
		this->RootNode = nullptr;
		this->Type = OBJ_OCTREE;
		this->MsgProc = nullptr;
		this->ThreadLock = (UBINT)NULL;
	}
	void Octree::Clear(){
		this->Allocator.Clear();
		this->RootNode = nullptr;
	}
	Octree::~Octree(){
		this->Allocator.Clear();
	}
}

#endif