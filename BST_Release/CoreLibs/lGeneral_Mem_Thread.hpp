/* Description:Implementation of a page manager and thread-relevant memory allocators. DO NOT include this header directly.
 * Language:C++11
 * Author:***
 *
 * The following macros can be defined before using this library:
 * LIB_GENERAL_DEF_ALLOC: use the default allocator on the operating system.
 *
 */
namespace nsBasic{

	/*--- sealed class PageManager ---*/
	extern UBINT EmptyPageRingPntr = (UBINT)NULL;
	extern volatile UBINT AllocedPageCnt = 0;
	extern volatile UBINT AllocedPageMem = 0;

	void *GetPage(UBINT PageSize){
		//We don't judge whether PageSize is suitable here.
		void *PageAddr;
#if defined LIBENV_OS_WIN
#if defined LIBENV_SYS_INTELX86
		PageAddr = VirtualAlloc(0, PageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#elif defined LIBENV_SYS_INTELX64
		PageAddr = VirtualAllocEx(GetCurrentProcess(), 0, PageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif
#elif defined LIBENV_OS_LINUX
		PageAddr = mmap(nullptr, PageSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		//The use of MAP_ANONYMOUS in conjunction with MAP_SHARED is only supported on Linux since kernel 2.4.
		//Remind that some Unix-based OS doesn't support MAP_ANONYMOUS(like FreeBSD).The following code should be used to circumvent this problem:
		//PageAddr=(UBINT)mmap(nullptr,PageSize,PROT_READ | PROT_WRITE,MAP_SHARED,open("/dev/zero",O_RDWR),0);
#else
		PageAddr = NULL;
#endif
		if ((UBINT)NULL != PageAddr){
			atomic_add(&AllocedPageMem, PageSize);
			atomic_add(&AllocedPageCnt, 1);
		}
		return PageAddr;
	}
	void unGetPage(void *PageAddr, UBINT PageSize){
		//We don't judge whether PageSize is suitable here.
		//If there are still much memory space,we will put this page into the empty page ring.
		if (NULL != PageAddr){
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this function.
			//This function release memory pages.
#endif

#if defined LIBENV_OS_WIN
#if defined LIBENV_SYS_INTELX86
			VirtualFree(PageAddr, 0, MEM_RELEASE);
#elif defined LIBENV_SYS_INTELX64
			VirtualFreeEx(GetCurrentProcess(), PageAddr, 0, MEM_RELEASE);
#endif
#elif defined LIBENV_OS_LINUX
			munmap(PageAddr, ((SimplePageTag *)PageAddr1)->PageSize);
#endif
			atomic_add(&AllocedPageCnt, (UBINT)-1);
			atomic_add(&AllocedPageMem, (UBINT)0 - (UBINT)PageSize);
		}
	}
	/*--- End ---*/

	extern MemHeap GlobalHeap = { nullptr };
	static volatile UBINT GlobalAllocLock = (UBINT)NULL;

#if defined LIB_GENERAL_DEF_ALLOC
	extern inline void *GlobalMemAlloc(UBINT Size){ return malloc(Size); }
	extern inline void *GlobalMemAlloc_Aligned(UBINT Size, UBINT Alignment){
#if defined LIBENV_CPLR_VS
		return _aligned_malloc(Size, Alignment);
#elif defined GCC
		return aligned_alloc(Size, Alignment); //requires glibc 2.16 (gcc 4.9)
#else
		//try this
		return _mm_malloc(Size, Alignment);
#endif
	}
	extern inline void *GlobalMemAlloc_SIMD(UBINT Size){ return GlobalMemAlloc_Aligned(Size, nsEnv::SIMDAlignment); }

	extern inline void GlobalMemFree(void *Addr){ free(Addr); }
	extern inline void GlobalMemFree(void *Addr, UBINT Size){ free(Addr); }
	extern inline void GlobalMemFree_Aligned(void *Addr){
#if defined LIBENV_CPLR_VS
		_aligned_free(Addr);

#elif defined GCC
		free(Addr);
#else
		//try this
		return _mm_free(Addr);
#endif
	}
	extern inline void GlobalMemFree_Aligned(void *Addr, UBINT Size){
#if defined LIBENV_CPLR_VS
		_aligned_free(Addr);
		
#elif defined GCC
		free(Addr);
#else
		//try this
		return _mm_free(Addr);
#endif
	}
	extern inline void GlobalMemFree_SIMD(void *Addr){ GlobalMemFree_Aligned(Addr); }
	extern inline void GlobalMemFree_SIMD(void *Addr, UBINT Size){ GlobalMemFree_Aligned(Addr, Size); }

	extern inline void *MemAlloc(UBINT Size){ return malloc(Size); }
	extern inline void *MemAlloc_Aligned(UBINT Size, UBINT Alignment){
#if defined LIBENV_CPLR_VS
		return _aligned_malloc(Size, Alignment);
#elif defined GCC
		return aligned_alloc(Size, Alignment); //requires glibc 2.16 (gcc 4.9)
#else
		//try this
		return _mm_malloc(Size, Alignment);
#endif
	}
	extern inline void *MemAlloc_SIMD(UBINT Size){ return MemAlloc_Aligned(Size, nsEnv::SIMDAlignment); }

	extern inline void MemFree(void *Addr){ free(Addr); }
	extern inline void MemFree(void *Addr, UBINT Size){ free(Addr); }
	extern inline void MemFree_Aligned(void *Addr){
#if defined LIBENV_CPLR_VS
		_aligned_free(Addr);

#elif defined GCC
		free(Addr);
#else
		//try this
		return _mm_free(Addr);
#endif
	}
	extern inline void MemFree_Aligned(void *Addr, UBINT Size){
#if defined LIBENV_CPLR_VS
		_aligned_free(Addr);

#elif defined GCC
		free(Addr);
#else
		//try this
		return _mm_free(Addr);
#endif
	}
	extern inline void MemFree_SIMD(void *Addr){ MemFree_Aligned(Addr); }
	extern inline void MemFree_SIMD(void *Addr, UBINT Size){ MemFree_Aligned(Addr, Size); }
#else
#if defined LIB_GENERAL_MEM_SEC
	extern void *GlobalMemAlloc(UBINT Size){
		if (0 == Size || Size > MemHeap::FullMark - sizeof(MemHeap::_PageInfo))return nullptr;
		else{
			void *RetValue;
			EnterLock(&GlobalAllocLock);
			if (Size > nsEnv::SysPageSize / 2){
				RetValue = GlobalHeap._alloc_huge((Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize);
			}
			else{
				UBINT RealSize = nsMath::ceil2power(Size);
				if (RealSize > sizeof(MemHeap::_PageInfo))RetValue = GlobalHeap._alloc_large(RealSize);
				else if (RealSize > sizeof(UBINT))RetValue = GlobalHeap._alloc_medium(RealSize);
				else RetValue = GlobalHeap._alloc_medium(sizeof(UBINT));
			}
			LeaveLock(&GlobalAllocLock);
			return RetValue;
		}
	}
	extern void GlobalMemFree(void *Addr){
		if (nullptr != Addr){
			EnterLock(&GlobalAllocLock);
			MemHeap::_PageInfo *TgtPage = GlobalHeap._search_addr(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)));
			if (nullptr != TgtPage){
				UBINT TmpInfo[2];
				Morton2_inv(TgtPage->MortonCode, TmpInfo);
				TmpInfo[1] &= MemHeap::FullMark - 1;
				if (TmpInfo[1] > nsEnv::SysPageSize / 2)GlobalHeap._free_huge(TgtPage, Addr);
				else if (TmpInfo[1] > sizeof(MemHeap::_PageInfo))GlobalHeap._free_large(TgtPage, Addr);
				else GlobalHeap._free_medium(TgtPage, Addr);
			}
			LeaveLock(&GlobalAllocLock);
		}
	}
	extern void GlobalMemFree(void *Addr, UBINT Size){
		if (nullptr != Addr){
			EnterLock(&GlobalAllocLock);
			UBINT RealSize;
			if (Size > nsEnv::SysPageSize / 2){
				RealSize = (Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize;
				MemHeap::_PageInfo *TgtPage = GlobalHeap._search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), (RealSize | MemHeap::FullMark));
				if (nullptr != TgtPage)GlobalHeap._free_huge(TgtPage, Addr);
				else GlobalHeap.Free(Addr);
			}
			else{
				if (Size <= sizeof(UBINT))RealSize = sizeof(UBINT);
				else RealSize = nsMath::ceil2power(Size);
				if (RealSize > sizeof(MemHeap::_PageInfo)){
					MemHeap::_PageInfo *TgtPage = GlobalHeap._search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), (RealSize | MemHeap::FullMark));
					if (nullptr == TgtPage)TgtPage = GlobalHeap._search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), RealSize);
					if (nullptr == TgtPage)GlobalHeap.Free(Addr);
					else GlobalHeap._free_large(TgtPage, Addr);
				}
				else GlobalHeap._free_medium((MemHeap::_PageInfo *)(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1))), Addr);
			}
			LeaveLock(&GlobalAllocLock);
		}
	}
#else
	extern void *GlobalMemAlloc(UBINT Size){
		if (0 == Size || Size >= MemHeap::FullMark)return nullptr;
		else{
			void *RetValue;
			EnterLock(&GlobalAllocLock);
			if (Size > nsEnv::SysPageSize / 2)RetValue = GlobalHeap._alloc_huge((Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize);
			else if (Size > (nsEnv::SysPageSize - sizeof(MemHeap::_PageInfo)) / 2)RetValue = GlobalHeap._alloc_large(nsEnv::SysPageSize / 2);
			else{
				UBINT RealSize;
				if (Size > nsEnv::SysPageSize / 4)RealSize = Size;
				else if (Size <= sizeof(UBINT))RealSize = sizeof(UBINT);
				else RealSize = nsMath::ceil2power(Size);
				RetValue = GlobalHeap._alloc_medium(RealSize);
			}
			LeaveLock(&GlobalAllocLock);
			return RetValue;
		}
	}
	void GlobalMemFree(void *Addr){
		if (nullptr != Addr){
			EnterLock(&GlobalAllocLock);
			UBINT AddrVal = (UBINT)Addr;
			UBINT AddrVal_Index = AddrVal&(nsEnv::SysPageSize - 1);
			UBINT AddrVal_Page = AddrVal - AddrVal_Index;
			if (0 == AddrVal_Index){
				MemHeap::_PageInfo *TgtPage = GlobalHeap._search_addr_size(AddrVal_Page, MemHeap::FullMark);
				if (nullptr == TgtPage){
					TgtPage = GlobalHeap._search_addr_size(AddrVal_Page, (nsEnv::SysPageSize / 2 | MemHeap::FullMark));
					if (nullptr == TgtPage)TgtPage = GlobalHeap._search_addr_size(AddrVal_Page, nsEnv::SysPageSize / 2);
					if (nullptr != TgtPage)GlobalHeap._free_large(TgtPage, Addr);
				}
				else GlobalHeap._free_huge(TgtPage, Addr);
			}
			else if (nsEnv::SysPageSize / 2 == AddrVal_Index){
				MemHeap::_PageInfo *TgtPage = GlobalHeap._search_addr_size(AddrVal_Page, nsEnv::SysPageSize / 2);
				if (nullptr == TgtPage)TgtPage = GlobalHeap._search_addr_size(AddrVal_Page, (nsEnv::SysPageSize / 2 | MemHeap::FullMark));
				if (nullptr == TgtPage)GlobalHeap._free_medium((MemHeap::_PageInfo *)AddrVal_Page, Addr);
				else GlobalHeap._free_large(TgtPage, Addr);
			}
			else GlobalHeap._free_medium((MemHeap::_PageInfo *)AddrVal_Page, Addr);
			LeaveLock(&GlobalAllocLock);
		}
	}
	void GlobalMemFree(void *Addr, UBINT Size){
		//If the input is incorrect, this function may crash your program.
		if (nullptr != Addr){
			EnterLock(&GlobalAllocLock);
			UBINT AddrVal = (UBINT)Addr;
			UBINT AddrVal_Index = AddrVal&(nsEnv::SysPageSize - 1);
			UBINT AddrVal_Page = AddrVal - AddrVal_Index;
			if (Size > nsEnv::SysPageSize / 2){
				MemHeap::_PageInfo *TgtPage = GlobalHeap._search_addr_size(AddrVal_Page, MemHeap::FullMark);
				if (nullptr != TgtPage)GlobalHeap._free_huge(TgtPage, Addr);
			}
			else if (Size > (nsEnv::SysPageSize - sizeof(MemHeap::_PageInfo)) / 2){
				MemHeap::_PageInfo *TgtPage = GlobalHeap._search_addr_size(AddrVal_Page, (nsEnv::SysPageSize / 2) | MemHeap::FullMark);
				if (nullptr == TgtPage)TgtPage = GlobalHeap._search_addr_size(AddrVal_Page, nsEnv::SysPageSize / 2);
				if (nullptr != TgtPage)GlobalHeap._free_large(TgtPage, Addr);
			}
			else GlobalHeap._free_medium((MemHeap::_PageInfo *)AddrVal_Page, Addr);
			LeaveLock(&GlobalAllocLock);
		}
	}
#endif

	extern inline void *MemAlloc(UBINT Size){ return GetCurThreadStub()->ThreadHeap.Alloc(Size); }
	extern inline void MemFree(void *Addr){ GetCurThreadStub()->ThreadHeap.Free(Addr); }
	extern inline void MemFree(void *Addr, UBINT Size){ GetCurThreadStub()->ThreadHeap.Free(Addr, Size); }
#endif
	template<class T> extern inline T *GlobalMemAlloc_Arr(UBINT Count){ return (T *)GlobalMemAlloc(Count * sizeof(T)); }
	template<class T> extern inline T *GlobalMemAlloc_Arr_Aligned(UBINT Count, UBINT Alignment){ return (T *)GlobalMemAlloc_Aligned(Count * sizeof(T), Alignment); }
	template<class T> extern inline T *GlobalMemAlloc_Arr_SIMD(UBINT Count){ return (T *)GlobalMemAlloc_Aligned(Count * sizeof(T), nsEnv::SIMDAlignment); }
	template<class T> extern inline void GlobalMemFree_Arr(T *Addr, UBINT Count){ GlobalMemFree(Addr, Count * sizeof(T)); }
	template<class T> extern inline void GlobalMemFree_Arr_Aligned(T *Addr, UBINT Count){ GlobalMemFree_Aligned(Addr, Count * sizeof(T)); }
	template<class T> extern inline void GlobalMemFree_Arr_SIMD(T *Addr, UBINT Count){ GlobalMemFree_Aligned(Addr, Count * sizeof(T)); }

	template<class T> extern inline T *MemAlloc_Arr(UBINT Count){ return (T *)MemAlloc(Count * sizeof(T)); }
	template<class T> extern inline T *MemAlloc_Arr_Aligned(UBINT Count, UBINT Alignment){ return (T *)MemAlloc_Aligned(Count * sizeof(T), Alignment); }
	template<class T> extern inline T *MemAlloc_Arr_SIMD(UBINT Count){ return (T *)MemAlloc_Aligned(Count * sizeof(T), nsEnv::SIMDAlignment); }
	template<class T> extern inline void MemFree_Arr(T *Addr, UBINT Count){ MemFree(Addr, Count * sizeof(T)); }
	template<class T> extern inline void MemFree_Arr_Aligned(T *Addr, UBINT Count){ MemFree_Aligned(Addr, Count * sizeof(T)); }
	template<class T> extern inline void MemFree_Arr_SIMD(T *Addr, UBINT Count){ MemFree_Aligned(Addr, Count * sizeof(T)); }
}

void * operator new(size_t size){
	void *ptr = nsBasic::MemAlloc(size);
	if (nullptr == ptr)throw new std::bad_alloc;
	return ptr;
}
void operator delete(void *ptr){
	nsBasic::MemFree(ptr);
}

#if __cplusplus > 201402L
void operator delete(void *ptr, size_t size){
	nsBasic::MemFree(ptr, size);
}
#endif