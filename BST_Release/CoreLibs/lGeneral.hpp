/* Description: Functions for general use.
 * Language:C++
 * Author:***
 *
 * This library assume that your compiler and the runtime environment has the following features:
 * 1.There is a integer type (defined as the Basic INTeger) which should satisfy the following conditions:
 *   a.It is equivalent to a pointer variable, and mutual type-casting is suitable;
 *   b.It can be accessed atomically by the CPU.
 *
 * The following macros should be defined before you use this library:
 * architecture:LIBENV_SYS_INTELX86 / LIBENV_SYS_INTELX64
 *       -- ISA:see lGeneral_CPU_ISA.hpp for details.
 * OS          :LIBENV_OS_WIN       / LIBENV_OS_LINUX
 * Compiler    :LIBENV_CPLR_VS      / LIBENV_CPLR_GCC
 * Use console :LIBENV_WITHCON      / LIBENV_NOCON
 *
 * The following macros can be defined before using this library:
 * LIBDBG_PLATFORM_TEST: Highlight all the platform-related code.
 *
 * Tips for coding:
 * You should not set ThreadLock to 0 when MSG_DESTROY is received in case that there is a thread locking on it.
 */

#ifndef LIB_GENERAL
#define LIB_GENERAL

#define _USE_MATH_DEFINES // this definition must be put into the earliest place possible - at least in front of <functional>. Otherwise the macros won't be available in Visual Studio.

#include "lGeneral_CPU_ISA.hpp"
#include <functional> // std::bind!
#include <exception>
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check the following macros.
//These macros provide platform-related headers,platform-independent data types,atomic CAS operations and system error feedback method.
#endif

#if defined LIBENV_OS_WIN

#if !defined UNICODE || !defined _UNICODE
#error This library must be complied with UNICODE support.
#endif

#define NOMINMAX
#include "windows.h"

#if defined LIBENV_CPLR_VS
typedef wchar_t UBCHAR;
typedef unsigned char UINT1b;
typedef unsigned short UINT2b;
typedef unsigned long UINT4b;
typedef unsigned long long UINT8b;
typedef char INT1b;
typedef short INT2b;
typedef long INT4b;
typedef long long INT8b;
#if defined LIBENV_SYS_INTELX86
typedef unsigned long UBINT;
extern const UBINT UBINT_MAX = 0xFFFFFFFF;
typedef long int BINT;
typedef float BFLOAT;
#elif defined LIBENV_SYS_INTELX64
typedef unsigned long long UBINT;
extern const UBINT UBINT_MAX = 0xFFFFFFFFFFFFFFFF;
typedef long long int BINT;
typedef double BFLOAT;
#endif
#if defined LIBENV_WITHCON
#pragma comment(linker,"/subsystem:\"Console\" /entry:\"wWinMainCRTStartup\"")
#elif defined  LIBENV_NOCON
#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"wWinMainCRTStartup\"")
#endif
#endif

#define LastSysErr GetLastError()
#elif defined LIBENV_OS_LINUX
#include "unistd.h"
#include "errno.h"

#if defined LIBENV_CPLR_GCC
typedef char UBCHAR;
typedef unsigned char UINT1b;
typedef unsigned short UINT2b;
typedef unsigned int UINT4b;
typedef unsigned long long UINT8b;
typedef char INT1b;
typedef short INT2b;
typedef int INT4b;
typedef long long INT8b;
typedef unsigned long UBINT;
typedef long int BINT;
#if defined LIBENV_SYS_INTELX86
extern const UBINT UBINT_MAX = 0xFFFFFFFF;
typedef float BFLOAT;

#elif defined LIBENV_SYS_INTELX64
extern const UBINT UBINT_MAX = 0xFFFFFFFFFFFFFFFF;
typedef double BFLOAT;
#endif
#endif

#define LastSysErr errno
#else
#endif

template<typename T> struct Type_To_LocalID{
	static const unsigned char _IDPtr = 0; //A value is assigned to this variable to prevent the compiler from optimizing it
	static const UBINT Value;
};
template<typename T> const UBINT Type_To_LocalID<T>::Value = (UBINT)&Type_To_LocalID<T>::_IDPtr;
#define __typeid(T) Type_To_LocalID<T>::Value
template< UBINT > struct LocalID_To_Type{};

#include "lGeneral_IBase.hpp"
#include "lGeneral_Stream.hpp"
#include "lGeneral_CharCoding.hpp"

namespace nsEnv{
	extern UBINT InstanceID;
	extern UBINT SysPageSize;
	extern UBINT ProcessorCnt;
	extern UBINT CmdLineCnt;
	extern void *CmdLineArr;
}

#include "lGeneral_Math.hpp"
#include "lGeneral_Mem.hpp"

extern enum GlobalDataType{
	PAGETAG_EMPTY,
	PAGETAG_THREAD,
	PAGETAG_OBJ,
	PAGE_BLK,
	SECT_GLYPH,
	OBJ_PAGERING_BLK,
	OBJ_STACK_BLK,
	OBJ_QUEUE_BLK,
	OBJ_DEQUEUE_BLK,
	OBJ_SET_INDEX,
	OBJ_INTRVTREE,
	OBJ_OCTREE,
	OBJ_MEM_ALLOCATOR,
	OBJ_MSG_MANAGER,
	OBJ_FILE_READ,
	OBJ_FILE_WRITE,
	OBJ_FILE_MEMSECT,
	OBJ_EDITOR_MEMFILE,
	OBJ_WINDOW,
	OBJ_GRAPHICDEVICE,
	OBJ_FONTENGINE,
	OBJ_FONTRASTER,
	OBJ_CONTROL_MANAGER,
	OBJ_HITMAP_QUADTREE,
	OBJ_HITMAP_BUFFER,
	OBJ_HITRGN_RECT,
	OBJ_BUTTON,
	OBJ_CHECKBOX,
	OBJ_SLIDER,
	OBJ_PROGBAR,
	OBJ_POINT3D,
	OBJ_TRIPODDUMMY,
	OBJ_TRIPOD,
	OBJ_SPAINTBUFFER,
	OBJ_SNDOUTPUTDEV,
	OBJ_SNDOUTPUTSTREAM,
	OBJ_USERDEF
};
extern enum GlobalMsgType{
	MSG_STREAM_READ,
	MSG_STREAM_READBULK,
	MSG_STREAM_WRITE,
	MSG_STREAM_WRITEBULK,
	MSG_FILE_SETPTR,
	MSG_FILE_GETPTR,
	MSG_EDITOR_INSERT,
	MSG_EDITOR_INSERTBULK,
	MSG_EDITOR_DELETE,
	MSG_WNDSIZE,
	MSG_WNDDESTROY,
	MSG_REPOS,
	MSG_MOUSECHECK,
	MSG_MOUSEENTER,
	MSG_MOUSEEVENT,
	MSG_MOUSELEAVE,
	MSG_KEYBDENTER,
	MSG_KEYBDEVENT,
	MSG_KEYBDLEAVE,
	MSG_SPAINT_DWORD,
	MSG_SPAINT_DWORDEX,
	MSG_PAINT,
	MSG_COMMAND,
	MSG_SCROLL,
	MSG_DRAG2D,
	MSG_DRAG3D,
	MSG_DRAGSTOP,
	MSG_IDLE,
	MSG_DESTROY,
	MSG_USERDEF
};
extern enum GlobalIOPort{
	IO_VIDEO_IN,
	IO_VIDEO_OUT,
	IO_SOUND_IN,
	IO_SOUND_OUT,
	IO_KEYBD,
	IO_MOUSE,
	IO_IME,
	IO_USER
};

namespace nsBasic{
	struct BlockPage{
		void *PrevPage;
		void *NextPage;
		void *PageAddr;
		UBINT NextEmpty;
		UBINT Type;
		UBINT PageSize;
		UBINT BlockSize;
		UBINT BlockCntr;

		void operator delete(void *ptr);
	};
}

#include "lGeneral_Thread.hpp"
#include "lGeneral_Mem_Thread.hpp"

namespace nsBasic{
	/*--- General APIs ---*/
		struct ObjGeneral;
		struct ObjSet;
		typedef UBINT(_cdecl *MSGPROC)(ObjGeneral *, UBINT, ...);

		struct ObjGeneral{
			//Three properties (PrevObj, NextObj, Type) and a virtual method (MsgProc) is predefined in this class.
			//Every class derived from this one, whether explicitly or implicitly, should implement these predefined members.
			void *PrevObj;
			void *NextObj;
			UBINT _Reserved1;
			UBINT _Reserved2;
			UBINT Type;
			MSGPROC MsgProc;
			UBINT _Reserved3;
			UBINT _Reserved4;

			ObjGeneral(){};
			ObjGeneral(nsBasic::ObjSet *ParentObj, MSGPROC MsgProc);
			~ObjGeneral(){};
			void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(ObjGeneral)); }
		};
		struct ObjSet{
		public:
			void *PrevObj;
			void *NextObj;
			void *ChildObjRing;
			UBINT _Reserved2;
			UBINT Type;
			MSGPROC MsgProc;
			UBINT ThreadLock;
			UBINT _Reserved4;

			ObjSet(){};
			ObjSet(nsBasic::ObjSet *ParentObj, MSGPROC MsgProc);
			inline operator ObjGeneral() const{ return *(ObjGeneral *)this; }
			~ObjSet(){};
			void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(ObjSet)); }
		};
		struct ObjSetExt{
		public:
			void *PrevObj;
			void *NextObj;
			void *ChildObjRing;
			UBINT _Reserved2;
			UBINT Type;
			MSGPROC MsgProc;
			UBINT ThreadLock;
			UBINT ExtInfo;

			inline operator ObjSet() const{ return *(ObjSet *)this; }
			void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(ObjSetExt)); }
		};
		struct BlockPageRing{
			void *PrevObj;
			void *NextObj;
			void *FirstPage;
			UBINT PageCount;
			UBINT Type;
			MSGPROC MsgProc;
			UBINT ThreadLock;
			UBINT BlockSize;

			void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(BlockPageRing)); }
		};
		struct MsgManager{
			//assume that the size of a memory page is smaller than 0x10000 in 32-bit machine and 0x100000000 in 64-bit machine, which is true in most cases.
			void *PrevObj;
			void *NextObj;
			UBINT PageAddr;
			UBINT ReadCntr; //The lower part of this counter is the writable length of the buffer, the higher part is the address offset of the write pointer.
			UBINT Type;
			MSGPROC MsgProc;
			UBINT ThreadLock;
			UBINT WriteCntr; //The lower part of this counter is the readable length of the buffer, the higher part is the address offset of the read pointer. 
		};
		struct ThreadExtInfo{
			MsgManager MsgManager;
		};

		class MemPoolBase{
		private:
			MemPoolBase(const MemPoolBase &) = delete; //not copyable
			MemPoolBase& operator=(const MemPoolBase &) = delete; //not copyable
		protected:
			struct MemPageListNode{
				MemPageListNode *NextPage;
				void *PageAddr;
				UBINT NextEmptyBlockOffset;
				UBINT AllocatedBlockCount;
			} *FirstPage, *LastPage;
			UBINT PageSize;
			UBINT BlockSize;
			void *(*CallBack_Alloc)(UBINT);
			void(*CallBack_Free)(void *, UBINT);
		public:
			MemPoolBase(UBINT BlockSize);
			MemPoolBase(UBINT BlockSize, UBINT PageSize, void *(*CallBack_Alloc)(UBINT) = GlobalMemAlloc, void(*CallBack_Free)(void *, UBINT) = GlobalMemFree);
			MemPoolBase(MemPoolBase &&rhs);

			void *Alloc();
			void Free(void *Addr);
			void Shrink();

			//operators
			MemPoolBase& operator=(MemPoolBase&& rhs);

			~MemPoolBase();
		};

		template<class T> extern inline void MemMove_Arr(T *lpDest, T *lpSrc, UBINT Count){ memmove(lpDest, lpSrc, Count * sizeof(T)); }

		extern inline void SetThreadExtInfo(ThreadExtInfo *lpInfo){ GetCurThreadStub()->ExtInfo = lpInfo; }
		extern inline ThreadExtInfo *GetThreadExtInfo(){ return (ThreadExtInfo *)GetCurThreadStub()->ExtInfo; }
		extern void ObjRing_Attach(ObjGeneral *Obj, void **lpFirstObjAddr);
		extern void ObjRing_Detach(ObjGeneral *Obj, void **lpFirstObjAddr);

		extern UBINT _cdecl MsgProc_Set(ObjGeneral *lpObj,UBINT Msg,...);
		#define SendMsg(lpObj,Msg,...) ((nsBasic::MSGPROC)((nsBasic::ObjGeneral *)(lpObj))->MsgProc)((nsBasic::ObjGeneral *)(lpObj),Msg,__VA_ARGS__)
		extern inline void DestroyObject(ObjGeneral *lpObj){if((UBINT)NULL!=lpObj->MsgProc)((MSGPROC)lpObj->MsgProc)(lpObj,MSG_DESTROY);}
		
		extern UBINT _cdecl MsgProc_MsgManager(ObjGeneral *lpObj,UBINT Msg,...); //Thread safe
		extern UBINT CreateMsgManager(MsgManager *lpMsgManager); //Thread safe
		extern inline void SetMsgPort(MsgManager *lpMsgManager,ObjGeneral *lpObj,UBINT Port){
			ObjGeneral **Dest=(ObjGeneral **)(lpMsgManager->PageAddr+nsEnv::SysPageSize)+Port;
			*Dest=lpObj;
		}
		extern inline ObjGeneral *GetMsgPort(MsgManager *lpMsgManager,UBINT Port){
			return *((ObjGeneral **)(lpMsgManager->PageAddr+nsEnv::SysPageSize)+Port);
		}
		extern UBINT MsgQueue_Read(MsgManager *lpMsgManager,UBINT *lpValue);                //Thread Safe
		extern UBINT MsgQueue_BulkRead(MsgManager *lpMsgManager,void * lpData,UBINT Length); //Thread Safe
		extern UBINT MsgQueue_Write(MsgManager *lpMsgManager,UBINT Value);                    //Thread Safe
		extern UBINT MsgQueue_BulkWrite(MsgManager *lpMsgManager, void * lpData, UBINT Length);  //Thread Safe

		extern UBINT CreateThreadExtObj();
		extern void DestroyThreadExtObj(); //Fatal error (usually memory access violation) may appear if CreateThreadExtObj() hasn't been called.

		extern void GetAccurateTimeCntr(UINT8b *RecvTime);
	/*--- End ---*/
}
extern int UserMain();

/*-------------------------------- IMPLEMENTATION --------------------------------*/

namespace nsEnv{
	extern UBINT InstanceID = 0;
	extern UBINT SysPageSize = 0;
	extern UBINT ProcessorCnt = 0;
	extern UBINT CmdLineCnt = 0;
	extern void *CmdLineArr = 0;

	extern UINT8b TimeTickPerSec = 0;
}
namespace nsBasic{
	//Not guaranteed bug free.

	void BlockPage::operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(BlockPage)); }

	//--- General APIs ---//

	ObjGeneral::ObjGeneral(nsBasic::ObjSet *ParentObj, MSGPROC MsgProc){
		this->MsgProc = MsgProc;
		if (NULL != ParentObj)nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)this, (void **)&(ParentObj->ChildObjRing));
	}

	ObjSet::ObjSet(nsBasic::ObjSet *ParentObj, MSGPROC MsgProc){
		this->ChildObjRing = nullptr;
		this->MsgProc = MsgProc;
		this->ThreadLock = (UBINT)nullptr;
		if (NULL != ParentObj)nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)this, (void **)&(ParentObj->ChildObjRing));
	}

	extern void ObjRing_Attach(ObjGeneral *Obj,void **lpFirstObjAddr){
		if(0==*lpFirstObjAddr){
			((ObjGeneral *)Obj)->PrevObj=Obj;
			((ObjGeneral *)Obj)->NextObj=Obj;
		}
		else{
			Obj->NextObj=*lpFirstObjAddr;
			Obj->PrevObj=((ObjGeneral *)*lpFirstObjAddr)->PrevObj;
			((ObjGeneral *)*lpFirstObjAddr)->PrevObj=Obj;
			((ObjGeneral *)Obj->PrevObj)->NextObj=Obj;
		}
		*lpFirstObjAddr=Obj;
	}
	extern void ObjRing_Detach(ObjGeneral *Obj, void **lpFirstObjAddr){
		if(*lpFirstObjAddr==Obj){
			if(Obj->NextObj==Obj)*lpFirstObjAddr=(UBINT)NULL;
			else{
				*lpFirstObjAddr=Obj->NextObj;
				((ObjGeneral *)Obj->NextObj)->PrevObj=Obj->PrevObj;
				((ObjGeneral *)Obj->PrevObj)->NextObj=Obj->NextObj;
			}
		}
		else{
			((ObjGeneral *)Obj->NextObj)->PrevObj=Obj->PrevObj;
			((ObjGeneral *)Obj->PrevObj)->NextObj=Obj->NextObj;
		}
	}
	
	// class MemPoolBase --BEGIN--

	MemPoolBase::MemPoolBase(UBINT BlockSize){
		if (BlockSize < sizeof(void *))BlockSize = sizeof(void *);
		this->FirstPage = nullptr;
		this->LastPage = nullptr;
		this->PageSize = nsEnv::SysPageSize * sizeof(BlockSize);
		this->BlockSize = BlockSize;
		this->CallBack_Alloc = GlobalMemAlloc;
		this->CallBack_Free = GlobalMemFree;
	}
	MemPoolBase::MemPoolBase(UBINT BlockSize, UBINT PageSize, void *(*CallBack_Alloc)(UBINT), void(*CallBack_Free)(void *, UBINT)){
		if (BlockSize < sizeof(void *))BlockSize = sizeof(void *);
		this->FirstPage = nullptr;
		this->LastPage = nullptr;
		if (this->PageSize < BlockSize)this->PageSize = BlockSize; else this->PageSize = PageSize;
		this->BlockSize = BlockSize;
		this->CallBack_Alloc = CallBack_Alloc;
		this->CallBack_Free = CallBack_Free;
	}
	MemPoolBase::MemPoolBase(MemPoolBase &&rhs){
		this->FirstPage = rhs.FirstPage;
		this->LastPage = rhs.LastPage;
		this->PageSize = rhs.PageSize;
		this->BlockSize = rhs.BlockSize;
		this->CallBack_Alloc = rhs.CallBack_Alloc;
		this->CallBack_Free = rhs.CallBack_Free;
		rhs.FirstPage = nullptr;
		rhs.LastPage = nullptr;
	}
	MemPoolBase& MemPoolBase::operator=(MemPoolBase&& rhs){
		this->FirstPage = rhs.FirstPage;
		this->LastPage = rhs.LastPage;
		this->PageSize = rhs.PageSize;
		this->BlockSize = rhs.BlockSize;
		this->CallBack_Alloc = rhs.CallBack_Alloc;
		this->CallBack_Free = rhs.CallBack_Free;
		rhs.FirstPage = nullptr;
		rhs.LastPage = nullptr;
		return *this;
	}
	void *MemPoolBase::Alloc(){
		MemPageListNode *TargetPage = this->FirstPage;
		if (nullptr == TargetPage || (UBINT)-1 == TargetPage->NextEmptyBlockOffset){
			//Allocate a new page
			TargetPage = (MemPageListNode *)GlobalMemAlloc(sizeof(MemPageListNode));
			if (nullptr == TargetPage)return nullptr;
			void *NewPageAddr = this->CallBack_Alloc(this->PageSize);
			if (nullptr == NewPageAddr){
				nsBasic::GlobalMemFree(TargetPage, sizeof(MemPageListNode));
				return nullptr;
			}

			//initialize the page
			TargetPage->NextPage = this->FirstPage;
			TargetPage->PageAddr = NewPageAddr;
			TargetPage->AllocatedBlockCount = 0;
			TargetPage->NextEmptyBlockOffset = 0;

			this->FirstPage = TargetPage;
			if (nullptr == this->LastPage)this->LastPage = TargetPage;

			//initialize the block linked-list inside the page
			UBINT PageOffset = 0;
			for (UBINT i = 0; i < this->PageSize / this->BlockSize - 1; i++){
				*(UBINT *)((UBINT)TargetPage->PageAddr + PageOffset) = PageOffset + BlockSize;
				PageOffset += BlockSize;
			}
			*(UBINT *)((UBINT)TargetPage->PageAddr + PageOffset) = (UBINT)-1;
		}

		void *RetValue = (void *)((UBINT)TargetPage->PageAddr + TargetPage->NextEmptyBlockOffset);
		TargetPage->AllocatedBlockCount++;
		TargetPage->NextEmptyBlockOffset = *(UBINT *)RetValue;
		
		if ((UBINT)-1 == TargetPage->NextEmptyBlockOffset){
			//sink the page to the bottom
			this->LastPage->NextPage = TargetPage;
			this->LastPage = TargetPage;
			this->FirstPage = TargetPage->NextPage;
			TargetPage->NextPage = nullptr;
		}

		return RetValue;
	}
	void MemPoolBase::Free(void *Addr){
		//find the page where the returned block resides
		MemPageListNode *PrevPage = nullptr, *TargetPage = this->FirstPage;
		while (nullptr != TargetPage){
			if ((UBINT)Addr - (UBINT)TargetPage->PageAddr < this->PageSize)break;
			PrevPage = TargetPage;
			TargetPage = TargetPage->NextPage;
		}
		if (nullptr != TargetPage){
			*(UBINT *)Addr = TargetPage->NextEmptyBlockOffset;
			TargetPage->NextEmptyBlockOffset = (UBINT)Addr - (UBINT)TargetPage->PageAddr;
			TargetPage->AllocatedBlockCount--;
			if (nullptr != PrevPage){
				PrevPage->NextPage = TargetPage->NextPage;
				TargetPage->NextPage = this->FirstPage;
				this->FirstPage = TargetPage;
				if (this->LastPage == TargetPage)this->LastPage = PrevPage;
			}
		}
	}
	void MemPoolBase::Shrink(){
		MemPageListNode *PrevPage = nullptr, *TargetPage = this->FirstPage;
		while (nullptr != TargetPage){
			MemPageListNode *NextPage = TargetPage->NextPage;
			if (0 == TargetPage->AllocatedBlockCount){
				if (nullptr == PrevPage)this->FirstPage = NextPage;
				else PrevPage->NextPage = NextPage;
				if (this->LastPage = TargetPage)this->LastPage = PrevPage;

				this->CallBack_Free(TargetPage->PageAddr, this->PageSize);
				GlobalMemFree(TargetPage, sizeof(MemPageListNode));
			}
			else PrevPage = TargetPage;
			TargetPage = NextPage;
		}
	}
	MemPoolBase::~MemPoolBase(){
		MemPageListNode *TargetPage = this->FirstPage;
		while (nullptr != TargetPage){
			MemPageListNode *TmpPage = TargetPage;
			TargetPage = TargetPage->NextPage;
			this->CallBack_Free(TmpPage->PageAddr, this->PageSize);
			GlobalMemFree(TmpPage, sizeof(MemPageListNode));
		}
	}

	// class MemPoolBase --END--

	extern UBINT _cdecl MsgProc_Set(ObjGeneral *lpObj,UBINT Msg,...){
		ObjGeneral *Obj1=(ObjGeneral *)((ObjSet *)lpObj)->ChildObjRing,*Obj2;
		if(NULL!=Obj1){
			if(MSG_DESTROY==Msg){
				do{
					Obj2=(ObjGeneral *)Obj1->NextObj;
					DestroyObject(Obj1);
					Obj1=Obj2;
				}while(Obj1!=(ObjGeneral *)((ObjSet *)lpObj)->ChildObjRing);
				((ObjSet *)lpObj)->ChildObjRing=(UBINT)NULL;
			}
		}
		return 0;
	}
	
	extern UBINT _cdecl MsgProc_MsgManager(ObjGeneral *lpObj,UBINT Msg,...){
		if(MSG_DESTROY==Msg){
			EnterLock(&((MsgManager *)lpObj)->ThreadLock);
			unGetPage((void *)(((MsgManager *)lpObj)->PageAddr),2*nsEnv::SysPageSize);
			LeaveLock(&((MsgManager *)lpObj)->ThreadLock);
		}
		return 0;
	}
	extern UBINT CreateMsgManager(MsgManager *lpMsgManager){
		lpMsgManager->PageAddr=(UBINT)GetPage(2*nsEnv::SysPageSize); //Maybe it's better to use two pages of smaller size.
		if(0!=lpMsgManager->PageAddr){
			UBINT *TmpPtr = (UBINT *)(lpMsgManager->PageAddr + nsEnv::SysPageSize);
			for (UBINT i = 0; i < nsEnv::SysPageSize / sizeof(UBINT); i++)TmpPtr[i] = 0;
			lpMsgManager->Type = OBJ_MSG_MANAGER;
			lpMsgManager->MsgProc = MsgProc_MsgManager;
			lpMsgManager->ThreadLock = (UBINT)NULL;
			lpMsgManager->ReadCntr = 0;
			lpMsgManager->WriteCntr = nsEnv::SysPageSize;
			return 0;
		}
		else return 1;
	}
	extern UBINT MsgQueue_Read(MsgManager *lpMsgManager,UBINT *lpValue){
#if defined LIBENV_SYS_INTELX86
		static UBINT HalfInt = 0xFFFF, HalfIntBits = 16;
#else if
		static const unsigned int HalfInt = 0xFFFFFFFF, HalfIntBits = 32;
#endif
		UBINT RCntrSnapShot = lpMsgManager->ReadCntr;
		if ((RCntrSnapShot&HalfInt)>0){
			const UBINT BitMask = nsEnv::SysPageSize - 1;
			atomic_add(&(lpMsgManager->ReadCntr), sizeof(UBINT)* HalfInt);
			*lpValue = *(UBINT *)(lpMsgManager->PageAddr + ((RCntrSnapShot >> HalfIntBits) & BitMask)); //Correct when nsEnv::SysPageSize and sizeof(UBINT) is the power of 
			atomic_add(&(lpMsgManager->WriteCntr), sizeof(UBINT));
			return 0;
		}
		else return 1;
	}
	extern UBINT MsgQueue_BulkRead(MsgManager *lpMsgManager,void *lpData,UBINT Length){
#if defined LIBENV_SYS_INTELX86
		static UBINT HalfInt = 0xFFFF, HalfIntBits = 16;
#else if
		static const unsigned int HalfInt = 0xFFFFFFFF, HalfIntBits = 32;
#endif
		UBINT RCntrSnapShot = lpMsgManager->ReadCntr;
		if ((RCntrSnapShot&HalfInt) >= Length){
			const UBINT BitMask = nsEnv::SysPageSize - 1;
			UBINT Remainder = Length%sizeof(UBINT);
			Length /= sizeof(UBINT);
			if (Remainder > 0)atomic_add(&lpMsgManager->ReadCntr, (Length + 1)*sizeof(UBINT)*HalfInt);
			else atomic_add(&lpMsgManager->ReadCntr, Length*sizeof(UBINT)*HalfInt);
			RCntrSnapShot >>= HalfIntBits;
			for (UBINT i = 0; i<Length; i++){
				*(UBINT *)lpData = *(UBINT *)(lpMsgManager->PageAddr + (RCntrSnapShot & BitMask)); //Correct when nsEnv::SysPageSize and sizeof(UBINT) is the power of 2
				lpData = ((UBINT *)lpData)+1;
				RCntrSnapShot += sizeof(UBINT);
			}
			if (Remainder>0){
				UBINT LastBytes = *(UBINT *)(lpMsgManager->PageAddr + (RCntrSnapShot & BitMask));  //Correct when nsEnv::SysPageSize and sizeof(UBINT) is the power of 2
				Length++;
				for (; Remainder > 0; Remainder--){
					*(unsigned char *)lpData = (unsigned char)LastBytes & 0xFF;
					lpData = ((unsigned char *)lpData) + 1;
					LastBytes >>= 8;
				}
			}
			atomic_add(&lpMsgManager->WriteCntr, Length*sizeof(UBINT));
			return 0;
		}
		else return 1;
	}
	extern UBINT MsgQueue_Write(MsgManager *lpMsgManager,UBINT Value){
#if defined LIBENV_SYS_INTELX86
		static const unsigned int HalfInt = 0xFFFF, HalfIntBits = 16;
#else if
		static const unsigned int HalfInt = 0xFFFFFFFF, HalfIntBits = 32;
#endif
		EnterLock(&(lpMsgManager->ThreadLock));
		UBINT WCntrSnapShot = lpMsgManager->WriteCntr;
		if ((WCntrSnapShot&HalfInt) > 0){
			const UBINT BitMask = nsEnv::SysPageSize - 1;
			atomic_add(&lpMsgManager->WriteCntr, HalfInt*sizeof(UBINT));
			*(UBINT *)(lpMsgManager->PageAddr + ((WCntrSnapShot >> HalfIntBits) & BitMask)) = Value; //Correct when nsEnv::SysPageSize and sizeof(UBINT) is the power of 2
			atomic_add(&lpMsgManager->ReadCntr, sizeof(UBINT));
			LeaveLock(&(lpMsgManager->ThreadLock));
			return 0;
		}
		else { LeaveLock(&(lpMsgManager->ThreadLock)); return 1; }
	}
	extern UBINT MsgQueue_BulkWrite(MsgManager *lpMsgManager,void *lpData,UBINT Length){
#if defined LIBENV_SYS_INTELX86
		static const unsigned int HalfInt = 0xFFFF, HalfIntBits = 16;
#else if
		static const unsigned int HalfInt = 0xFFFFFFFF, HalfIntBits = 32;
#endif
		EnterLock(&(lpMsgManager->ThreadLock));
		UBINT WCntrSnapShot = lpMsgManager->WriteCntr;
		if ((WCntrSnapShot&HalfInt) >=Length){
			const UBINT BitMask = nsEnv::SysPageSize - 1;
			UBINT Remainder = Length%sizeof(UBINT);
			Length /= sizeof(UBINT);
			if (Remainder > 0)atomic_add(&lpMsgManager->WriteCntr, (Length + 1)*sizeof(UBINT)*HalfInt);
			else atomic_add(&lpMsgManager->WriteCntr, Length*sizeof(UBINT)*HalfInt);
			WCntrSnapShot >>= HalfIntBits;
			for (UBINT i = 0; i<Length; i++){
				*(UBINT *)(lpMsgManager->PageAddr + (WCntrSnapShot & BitMask)) = *(UBINT *)lpData; //Correct when nsEnv::SysPageSize and sizeof(UBINT) is the power of 2
				lpData = ((UBINT *)lpData) + 1;
				WCntrSnapShot += sizeof(UBINT);
			}
			if (Remainder > 0){
				UBINT LastBytes = 0;
				Length++;
				for (; Remainder > 0; Remainder--){
					LastBytes <<= 8;
					LastBytes += *(unsigned char *)lpData;
				}
				*(UBINT *)(lpMsgManager->PageAddr + (WCntrSnapShot & BitMask)) = LastBytes;
			}
			atomic_add(&lpMsgManager->ReadCntr, Length*sizeof(UBINT));
			LeaveLock(&(lpMsgManager->ThreadLock));
			return 0;
		}
		else { LeaveLock(&(lpMsgManager->ThreadLock)); return 1; }
	}
	extern UBINT CreateThreadExtObj(){
		ThreadExtInfo *MyThreadExtInfo;

		MyThreadExtInfo=(ThreadExtInfo *)MemAlloc(sizeof(ThreadExtInfo));
		if (NULL != MyThreadExtInfo){
			if (0 == CreateMsgManager(&(MyThreadExtInfo->MsgManager))){
				GetCurThreadStub()->ExtInfo = MyThreadExtInfo;
				return 0;
			}
		}
		MemFree(MyThreadExtInfo, sizeof(ThreadExtInfo));
		return 1;
	}
	extern void DestroyThreadExtObj(){
		ThreadExtInfo *MyThreadExtInfo=(ThreadExtInfo *)GetCurThreadStub()->ExtInfo;
		DestroyObject((nsBasic::ObjGeneral *)&(MyThreadExtInfo->MsgManager));
		MemFree(MyThreadExtInfo, sizeof(ThreadExtInfo));
		GetCurThreadStub()->ExtInfo = nullptr;
	}
	extern void GetAccurateTimeCntr(UINT8b *RecvTime){
#if defined LIBENV_OS_WIN
		QueryPerformanceCounter((LARGE_INTEGER *)RecvTime);
#elif defined LIBENV_OS_LINUX
		timespec tp;
		clock_gettime(CLOCK_MONOTONIC,&tp);
		*RecvTime=timespec_to_ns(&tp);
#endif
	}

	//--- End ---//
}

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check the beginning and the end of this function.
//Here is the entry point of the whole program.
//TODO:Initialize every variables in nsEnv.
//	   Get a page of memory for the thread manager.
//	   Get a slot for thread local storage variable.
//     Call the UserMain.
//	   Collect the garbage.
#endif

#if defined LIBENV_OS_WIN
int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR lpCmdLine,int nShowCmd){
	int retv;
	nsBasic::ThreadStub MainThreadStub;

	SYSTEM_INFO mySysInfo;
	GetSystemInfo(&mySysInfo);

	nsEnv::InstanceID=(UBINT)hInstance;
	nsEnv::SysPageSize=mySysInfo.dwPageSize;
	nsEnv::ProcessorCnt=mySysInfo.dwNumberOfProcessors;
	if (nsEnv::SysPageSize < sizeof(nsBasic::ThreadStub))return 0; //The current memory page size is too small for this library.
	nsEnv::CmdLineCnt = 1;
	nsEnv::CmdLineArr = lpCmdLine;
	QueryPerformanceFrequency((LARGE_INTEGER *)&nsEnv::TimeTickPerSec);
#elif defined LIBENV_OS_LINUX
int main(int argc, char* argv[]){
	int retv;
	nsBasic::ThreadStub MainThreadStub;

	nsEnv::InstanceID=0;
	nsEnv::SysPageSize=sysconf(_SC_PAGESIZE);
	nsEnv::ProcessorCnt=sysconf(_SC_NPROCESSORS_CONF);
	if(nsEnv::SysPageSize<sizeof(nsBasic::ObjBinPageTag)+sizeof(nsBasic::ThreadStub))return 0; //The current memory page size is too small for this library.
	nsEnv::CmdLineCnt = argc;
	nsEnv::CmdLineArr = argv;
	nsEnv::TimeTickPerSec=1000000000;
#else	
#error This library does not support this operating system.
#endif
	_ThreadSupport_Init(&MainThreadStub);
	retv = UserMain();

	/*nsBasic::FlushEmptyPageRing();*/
	MainThreadStub.ThreadHeap.Clear();
	nsBasic::GlobalHeap.Clear();
	_ThreadSupport_Destroy(&MainThreadStub);
	return retv;
}

#endif