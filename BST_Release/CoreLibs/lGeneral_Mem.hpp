/* Description:Implementation of memory allocator class. DO NOT include this header directly.
 * Language:C++11
 * Author:***
 *
 * The following macros can be defined before using this library:
 * LIB_GENERAL_MEM_SEC: use safer (but slower) heap which verifies the border of memory blocks before releasing them.
 *
 */
#ifndef LIB_GENERAL_MEM
#define LIB_GENERAL_MEM

namespace nsBasic{
	struct RBTreeStub{
		RBTreeStub *Child[2];
		RBTreeStub *Parent;
		UBINT Color; // 0 = black  1 = Red
	};
	template<int D> inline int RBTree_Insert_RotateBranch(RBTreeStub **lpRoot, RBTreeStub *&CurNode, RBTreeStub *&ParentNode, RBTreeStub *&GParentNode){
		RBTreeStub *UncleNode = GParentNode->Child[!D];
		if (nullptr == UncleNode || 0 == UncleNode->Color){
			if (ParentNode->Child[!D] == CurNode){
				//Case 4
				CurNode->Color = 0;

				ParentNode->Child[!D] = CurNode->Child[D];
				ParentNode->Parent = CurNode;
				CurNode->Child[D] = ParentNode;

				GParentNode->Child[D] = CurNode->Child[!D];
				CurNode->Child[!D] = GParentNode;
				CurNode->Parent = GParentNode->Parent;
				GParentNode->Parent = CurNode;

				if (nullptr != ParentNode->Child[!D])ParentNode->Child[!D]->Parent = ParentNode;
				if (nullptr != GParentNode->Child[D])GParentNode->Child[D]->Parent = GParentNode;

				if (nullptr == CurNode->Parent)*lpRoot = CurNode;
				else if (CurNode->Parent->Child[D] == GParentNode)CurNode->Parent->Child[D] = CurNode;
				else CurNode->Parent->Child[!D] = CurNode;
			}
			else{
				//Case 5
				ParentNode->Color = 0;

				GParentNode->Child[D] = ParentNode->Child[!D];
				ParentNode->Child[!D] = GParentNode;
				ParentNode->Parent = GParentNode->Parent;
				GParentNode->Parent = ParentNode;

				if (nullptr != GParentNode->Child[D])GParentNode->Child[D]->Parent = GParentNode;

				if (nullptr == ParentNode->Parent)*lpRoot = ParentNode;
				else if (ParentNode->Parent->Child[D] == GParentNode)ParentNode->Parent->Child[D] = ParentNode;
				else ParentNode->Parent->Child[!D] = ParentNode;
			}
			return 1;
		}
		else{
			//Case 3
			ParentNode->Color = 0;
			UncleNode->Color = 0;
			CurNode = GParentNode;
			if (nullptr == CurNode->Parent){ CurNode->Color = 0; return 1; }
			else{ ParentNode = CurNode->Parent; return 0; }
		}
	}
	template<int D> inline int RBTree_Delete_RotateBranch(RBTreeStub **lpRoot, RBTreeStub *&ParentNode, RBTreeStub *&ChildNode){
		RBTreeStub *SibNode = ParentNode->Child[!D];
		if (1 == SibNode->Color){
			//Case 2:CP Length
			RBTreeStub *NephewNode = SibNode->Child[D];

			ParentNode->Color = 0; //After 5/6 spin,its color will become black.
			ParentNode->Child[!D] = NephewNode;
			NephewNode->Parent = ParentNode;
			SibNode->Color = 0;
			SibNode->Child[D] = ParentNode;
			SibNode->Parent = ParentNode->Parent;
			ParentNode->Parent = SibNode;

			if (nullptr != NephewNode->Child[!D] && 1 == NephewNode->Child[!D]->Color){
				//Case 2-6
				NephewNode->Child[!D]->Color = 0;
				SibNode->Child[D] = NephewNode;
				NephewNode->Parent = SibNode;
				ParentNode->Child[!D] = NephewNode->Child[D];
				NephewNode->Child[D] = ParentNode;
				ParentNode->Parent = NephewNode;
				NephewNode->Color = 1;

				if (nullptr != ParentNode->Child[!D])ParentNode->Child[!D]->Parent = ParentNode;
			}
			else if (nullptr != NephewNode->Child[D] && 1 == NephewNode->Child[D]->Color){
				//Case 2-5
				RBTreeStub *TmpNode = NephewNode->Child[D];

				SibNode->Child[D] = TmpNode;
				TmpNode->Parent = SibNode;

				ParentNode->Child[!D] = TmpNode->Child[D];
				TmpNode->Child[D] = ParentNode;
				ParentNode->Parent = TmpNode;
				NephewNode->Child[D] = TmpNode->Child[!D];
				TmpNode->Child[!D] = NephewNode;
				NephewNode->Parent = TmpNode;
				NephewNode->Color = 0;

				if (nullptr != ParentNode->Child[!D])ParentNode->Child[!D]->Parent = ParentNode;
				if (nullptr != NephewNode->Child[D])NephewNode->Child[D]->Parent = NephewNode;
			}
			else{
				NephewNode->Color = 1;
			}
			if (nullptr == SibNode->Parent)*lpRoot = SibNode;
			else if (SibNode->Parent->Child[!D] == ParentNode)SibNode->Parent->Child[!D] = SibNode;
			else SibNode->Parent->Child[D] = SibNode;

			ChildNode = SibNode;
			return 1;
		}
		else if (nullptr != SibNode->Child[!D] && 1 == SibNode->Child[!D]->Color){
			//Case 6
			SibNode->Child[!D]->Color = 0;

			SibNode->Color = ParentNode->Color;
			ParentNode->Child[!D] = SibNode->Child[D];
			ParentNode->Color = 0;
			SibNode->Child[D] = ParentNode;
			SibNode->Parent = ParentNode->Parent;
			ParentNode->Parent = SibNode;

			if (nullptr != ParentNode->Child[!D])ParentNode->Child[!D]->Parent = ParentNode;

			if (nullptr == SibNode->Parent)*lpRoot = SibNode;
			else if (SibNode->Parent->Child[!D] == ParentNode)SibNode->Parent->Child[!D] = SibNode;
			else SibNode->Parent->Child[D] = SibNode;

			ChildNode = SibNode;
			return 1;
		}
		else if (nullptr != SibNode->Child[D] && 1 == SibNode->Child[D]->Color){
			//Case 5
			RBTreeStub *TmpNode = SibNode->Child[D];

			SibNode->Child[D] = TmpNode->Child[!D];
			ParentNode->Child[!D] = TmpNode->Child[D];
			TmpNode->Child[!D] = SibNode;
			TmpNode->Child[D] = ParentNode;
			TmpNode->Color = ParentNode->Color;
			TmpNode->Parent = ParentNode->Parent;
			ParentNode->Parent = TmpNode;
			SibNode->Parent = TmpNode;
			ParentNode->Color = 0;
			// SibNode->Color=0;

			if (nullptr != ParentNode->Child[!D])ParentNode->Child[!D]->Parent = ParentNode;
			if (nullptr != SibNode->Child[D])SibNode->Child[D]->Parent = SibNode;

			if (nullptr == TmpNode->Parent)*lpRoot = TmpNode;
			else if (TmpNode->Parent->Child[!D] == ParentNode)TmpNode->Parent->Child[!D] = TmpNode;
			else TmpNode->Parent->Child[D] = TmpNode;

			TmpNode = TmpNode;
			return 1;
		}
		else{
			// Case 3,4
			ChildNode = ParentNode;

			SibNode->Color = 1;
			if (1 == ParentNode->Color){ ParentNode->Color = 0; return 1; }
			else if (nullptr == ParentNode->Parent)return 1;
			else{
				ParentNode = ParentNode->Parent;
				return 0;
			}
		}
	}
	extern void RBTree_Insert_Rebalance(RBTreeStub **lpRoot, RBTreeStub *lpStub){
		//This function assumes that lpStub != *lpRoot.
		RBTreeStub *ParentNode = lpStub->Parent, *GParentNode;

		while (1){
			if (0 == ParentNode->Color)break;
			GParentNode = ParentNode->Parent;
			GParentNode->Color = 1;
			if (GParentNode->Child[0] == ParentNode){
				if (1 == RBTree_Insert_RotateBranch<0>(lpRoot, lpStub, ParentNode, GParentNode))break;
			}
			else if (1 == RBTree_Insert_RotateBranch<1>(lpRoot, lpStub, ParentNode, GParentNode))break;
		}
	}
	extern void RBTree_Delete_Rebalance(RBTreeStub **lpRoot, RBTreeStub *lpStub){
		if (nullptr != lpStub->Child[0] && nullptr != lpStub->Child[1]){
			//find the nearest node that is smaller than lpStub. such a node won't have a right child.
			RBTreeStub *TmpNode = lpStub->Child[0];
			if (nullptr == TmpNode->Child[1]){
				//node swap
				RBTreeStub *SwapPtr = lpStub->Parent;
				lpStub->Parent = lpStub->Child[0];
				lpStub->Child[0] = TmpNode->Child[0];
				TmpNode->Child[0] = TmpNode->Parent;
				TmpNode->Parent = SwapPtr;
				TmpNode->Child[1] = lpStub->Child[1];
				lpStub->Child[1] = nullptr;
				UBINT SwapBuffer = lpStub->Color;
				lpStub->Color = TmpNode->Color;
				TmpNode->Color = SwapBuffer;

				RBTreeStub *ParentNode = TmpNode->Parent;
				if (nullptr == ParentNode)*lpRoot = TmpNode;
				else if (ParentNode->Child[0] == lpStub)ParentNode->Child[0] = TmpNode;
				else ParentNode->Child[1] = TmpNode;

				TmpNode->Child[1]->Parent = TmpNode;
				//TmpNode must have two children.
				if (nullptr != lpStub->Child[0])lpStub->Child[0]->Parent = lpStub;

				//Now lpStub won't have a right child.
			}
			else{
				do{ TmpNode = TmpNode->Child[1]; } while (nullptr != TmpNode->Child[1]);
				//Node swap
				RBTreeStub *SwapPtr = lpStub->Parent;
				lpStub->Parent = TmpNode->Parent;
				TmpNode->Parent = SwapPtr;
				SwapPtr = lpStub->Child[0];
				lpStub->Child[0] = TmpNode->Child[0];
				TmpNode->Child[0] = SwapPtr;
				TmpNode->Child[1] = lpStub->Child[1];
				lpStub->Child[1] = nullptr;
				UBINT SwapBuffer = lpStub->Color;
				lpStub->Color = TmpNode->Color;
				TmpNode->Color = SwapBuffer;

				RBTreeStub *ParentNode = TmpNode->Parent;
				if (nullptr == ParentNode)*lpRoot = TmpNode;
				else if (ParentNode->Child[0] == lpStub)ParentNode->Child[0] = TmpNode;
				else ParentNode->Child[1] = TmpNode;
				lpStub->Parent->Child[1] = lpStub; //lpStub must its parent's right child.

				TmpNode->Child[0]->Parent = TmpNode;
				TmpNode->Child[1]->Parent = TmpNode;
				//TmpNode must have two children.
				if (nullptr != lpStub->Child[0])lpStub->Child[0]->Parent = lpStub;

				//Now lpStub won't have a right child.
			}
		}
		if (nullptr == lpStub->Parent){
			RBTreeStub *TmpNode;
			if (nullptr == lpStub->Child[0])TmpNode = lpStub->Child[1]; else TmpNode = lpStub->Child[0];
			if (nullptr != TmpNode){
				TmpNode->Parent = nullptr;
				TmpNode->Color = 0;
			}
			*lpRoot = TmpNode;
		}
		else{
			RBTreeStub *ParentNode = lpStub->Parent, *ChildNode;
			//take out lpStub and link ChildNode to ParentNode
			if (nullptr == lpStub->Child[0])ChildNode = lpStub->Child[1]; else ChildNode = lpStub->Child[0];
			if (ParentNode->Child[0] == lpStub)ParentNode->Child[0] = ChildNode; else ParentNode->Child[1] = ChildNode;
			if (nullptr != ChildNode)ChildNode->Parent = ParentNode;

			if (0 == lpStub->Color){
				//Balancing
				if (nullptr != ChildNode)ChildNode->Color = 0; //if ChildNode exists,it must be red.
				else while (1){
					if (ParentNode->Child[0] == ChildNode){
						if (1 == RBTree_Delete_RotateBranch<0>(lpRoot, ParentNode, ChildNode))break;  //if ChildNode doesn't exist,then the sibling node must exist.
					}
					else if (1 == RBTree_Delete_RotateBranch<1>(lpRoot, ParentNode, ChildNode))break;
				}
			}
		}
	}

	inline UBINT _bitintrv(UBINT Operand){
#if defined LIBENV_SYS_INTELX64
#ifdef LIBENV_CPU_ACCEL_BMI2
		return _pdep_u64(Operand, 0x5555555555555555);
#else
		static const UBINT Mask[] = { 0x0000FFFF0000FFFF, 0x00FF00FF00FF00FF, 0x0F0F0F0F0F0F0F0F, 0x3333333333333333, 0x5555555555555555 };
		Operand = (Operand | (Operand << 16))&Mask[0];
		Operand = (Operand | (Operand << 8))&Mask[1];
		Operand = (Operand | (Operand << 4))&Mask[2];
		Operand = (Operand | (Operand << 2))&Mask[3];
		Operand = (Operand | (Operand << 1))&Mask[4];
#endif
#elif defined LIBENV_SYS_INTELX86
#ifdef LIBENV_CPU_ACCEL_BMI2
		return _pdep_u32(Operand, 0x55555555);
#else
		static const UBINT Mask[] = { 0x00FF00FF, 0x0F0F0F0F, 0x33333333, 0x55555555 };
		Operand = (Operand | (Operand << 8))&Mask[0];
		Operand = (Operand | (Operand << 4))&Mask[1];
		Operand = (Operand | (Operand << 2))&Mask[2];
		Operand = (Operand | (Operand << 1))&Mask[3];
#endif
#endif
		return Operand;
	}
	inline UBINT _bitintrv_inv(UBINT Operand){
#if defined LIBENV_SYS_INTELX64
#ifdef LIBENV_CPU_ACCEL_BMI2
		return _pext_u64(Operand, 0x5555555555555555);
#else
		static const UBINT Mask[] = { 0x0000FFFF0000FFFF, 0x00FF00FF00FF00FF, 0x0F0F0F0F0F0F0F0F, 0x3333333333333333, 0x5555555555555555 };
		Operand = Operand & Mask[4];
		Operand = (Operand | (Operand >> 1))&Mask[3];
		Operand = (Operand | (Operand >> 2))&Mask[2];
		Operand = (Operand | (Operand >> 4))&Mask[1];
		Operand = (Operand | (Operand >> 8))&Mask[0];
		Operand = (Operand | (Operand >> 16))&0xFFFFFFFF;
#endif
#elif defined LIBENV_SYS_INTELX86
#ifdef LIBENV_CPU_ACCEL_BMI2
		return _pext_u32(Operand, 0x55555555);
#else
		static const UBINT Mask[] = { 0x00FF00FF, 0x0F0F0F0F, 0x33333333, 0x55555555 };
		Operand = Operand & Mask[3];
		Operand = (Operand | (Operand >> 1))&Mask[2];
		Operand = (Operand | (Operand >> 2))&Mask[1];
		Operand = (Operand | (Operand >> 4))&Mask[0];
		Operand = (Operand | (Operand >> 8))&0xFFFF;
#endif
#endif
		return Operand;
	}
	inline void Morton2(UBINT Dim0, UBINT Dim1, UBINT *Result){
#if defined LIBENV_SYS_INTELX64
#ifdef LIBENV_CPU_ACCEL_BMI2
		Result[0] = _pdep_u64(Dim0, 0x5555555555555555) + _pdep_u64(Dim1, 0xAAAAAAAAAAAAAAAA);
		Result[1] = _pdep_u64(Dim0 >> 32, 0x5555555555555555) + _pdep_u64(Dim1 >> 32, 0xAAAAAAAAAAAAAAAA);
#else
		Result[0] = _bitintrv(Dim0 & 0xFFFFFFFF);
		Result[0] += _bitintrv(Dim1 & 0xFFFFFFFF) << 1;
		Result[1] = _bitintrv(Dim0 >> 32);
		Result[1] += _bitintrv(Dim1 >> 32) << 1;
#endif
#elif defined LIBENV_SYS_INTELX86
#ifdef LIBENV_CPU_ACCEL_BMI2
		Result[0] = _pdep_u32(Dim0, 0x55555555) + _pdep_u32(Dim1, 0xAAAAAAAA);
		Result[1] = _pdep_u32(Dim0 >> 16, 0x55555555) + _pdep_u32(Dim1 >> 16, 0xAAAAAAAA);
#else
		Result[0] = _bitintrv(Dim0 & 0xFFFF);
		Result[0] += _bitintrv(Dim1 & 0xFFFF) << 1;
		Result[1] = _bitintrv(Dim0 >> 16);
		Result[1] += _bitintrv(Dim1 >> 16) << 1;
#endif
#endif
	}
	inline void Morton2_inv(UBINT *Operand, UBINT *Result){
#if defined LIBENV_SYS_INTELX64
#ifdef LIBENV_CPU_ACCEL_BMI2
		Result[0] = _pext_u64(Operand[0], 0x5555555555555555) + (_pext_u64(Operand[1], 0x5555555555555555) << 32);
		Result[1] = _pext_u64(Operand[0], 0xAAAAAAAAAAAAAAAA) + (_pext_u64(Operand[1], 0xAAAAAAAAAAAAAAAA) << 32);
#else
		Result[0] = _bitintrv_inv(Operand[0]);
		Result[0] += _bitintrv_inv(Operand[1]) << 32;
		Result[1] = _bitintrv_inv(Operand[0] >> 1);
		Result[1] += _bitintrv_inv(Operand[1] >> 1) << 32;
#endif
#elif defined LIBENV_SYS_INTELX86
#ifdef LIBENV_CPU_ACCEL_BMI2
		Result[0] = _pext_u32(Operand[0], 0x55555555) + (_pext_u32(Operand[1], 0x55555555) << 16);
		Result[1] = _pext_u32(Operand[0], 0xAAAAAAAA) + (_pext_u32(Operand[1], 0xAAAAAAAA) << 16);
#else
		Result[0] = _bitintrv_inv(Operand[0]);
		Result[0] += _bitintrv_inv(Operand[1]) << 16;
		Result[1] = _bitintrv_inv(Operand[0] >> 1);
		Result[1] += _bitintrv_inv(Operand[1] >> 1) << 16;
#endif
#endif
	}

	/*--- sealed class PageManager ---*/
	extern UBINT EmptyPageRingPntr;
	extern volatile UBINT AllocedPageCnt;
	extern volatile UBINT AllocedPageMem;

	extern void *GetPage(UBINT PageSize); //multiples of nsEnv::SysPageSize recommended.
	extern void unGetPage(void *PageAddr, UBINT PageSize);
	/*--- End ---*/

	struct MemHeap_Sec{

		friend void *GlobalMemAlloc(UBINT Size);
		friend void GlobalMemFree(void *Addr);
		friend void GlobalMemFree(void *Addr, UBINT Size);

	private:
#if defined LIBENV_SYS_INTELX64
		static const UBINT FullMark = 0x8000000000000000;
		static const UBINT MaxSearchStackDepth = 2 * (64 - 2 * 3) - 3;
#elif defined LIBENV_SYS_INTELX86
		static const UBINT FullMark = 0x80000000;
		static const UBINT MaxSearchStackDepth = 2 * (32 - 2 * 2) - 3;
#endif

		struct _PageInfo{
			RBTreeStub SetStub;
			UBINT MortonCode[2];
			UBINT BlockCount;
			UBINT NextEmpty;
		};

		static BINT MortonCmp(UBINT a[2], UBINT b[2]);
		static UBINT pwr2minus1(UBINT x);
		void _insert(_PageInfo *PageNode);
		void _delete(_PageInfo *PageNode);
		_PageInfo *_search_addr(UBINT Addr);
		_PageInfo *_search_size(UBINT Size);
		_PageInfo *_search_addr_size(UBINT Addr, UBINT Size);
		void *_alloc_medium(UBINT Size);
		void *_alloc_large(UBINT Size);
		void *_alloc_huge(UBINT Size);
		void _free_medium(_PageInfo *TgtPage, void *Addr);
		void _free_large(_PageInfo *TgtPage, void *Addr);
		void _free_huge(_PageInfo *TgtPage, void *Addr);
	public:
		_PageInfo *Root;

		//as a POD, this class has no non-default constructor/destructor.
		void *Alloc(UBINT Size);
		void *Alloc_Bin(UBINT Size);
		void Free(void *Addr);
		void Free(void *Addr, UBINT Size);
		void Free_Bin(void *Addr, UBINT Size);
		void Clear();
		UBINT AllocedBytes();
		UBINT OccupiedBytes();
	};

	struct MemHeap_Fast{

		friend void *GlobalMemAlloc(UBINT Size);
		friend void GlobalMemFree(void *Addr);
		friend void GlobalMemFree(void *Addr, UBINT Size);

	private:
#if defined LIBENV_SYS_INTELX64
		static const UBINT FullMark = 0x8000000000000000;
		static const UBINT MaxSearchStackDepth = 2 * (64 - 2 * 3) - 3;
#elif defined LIBENV_SYS_INTELX86
		static const UBINT FullMark = 0x80000000;
		static const UBINT MaxSearchStackDepth = 2 * (32 - 2 * 2) - 3;
#endif

		struct _PageInfo{
			RBTreeStub SetStub;
			UBINT Addr_Size[2];
			UBINT BlockCount;
			UBINT NextEmpty;
		};

		static BINT PageCmp(UBINT a[2], UBINT b[2]);
		static UBINT pwr2minus1(UBINT x);
		void _insert(_PageInfo *PageNode);
		void _delete(_PageInfo *PageNode);
		_PageInfo *_search_size(UBINT Size);
		_PageInfo *_search_addr_size(UBINT Addr, UBINT Size);
		void *_alloc_medium(UBINT Size);
		void *_alloc_large(UBINT Size);
		void *_alloc_huge(UBINT Size);
		void _free_medium(_PageInfo *TgtPage, void *Addr);
		void _free_large(_PageInfo *TgtPage, void *Addr);
		void _free_huge(_PageInfo *TgtPage, void *Addr);
	public:
		_PageInfo *Root;

		//as a POD, this class has no non-default constructor/destructor.
		void *Alloc(UBINT Size);
		void *Alloc_Bin(UBINT Size);
		void Free(void *Addr);
		void Free(void *Addr, UBINT Size);
		void Free_Bin(void *Addr, UBINT Size);
		void Clear();
		UBINT AllocedBytes();
		UBINT OccupiedBytes();
	};

#if defined LIB_GENERAL_MEM_SEC
	typedef MemHeap_Sec MemHeap;
#else
	typedef MemHeap_Fast MemHeap;
#endif

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	/*--- sealed class PageManager ---*/

	//see lGeneral_Mem_Thread.hpp for implementation.
	void *GetPage(UBINT PageSize);
	void unGetPage(void *PageAddr, UBINT PageSize);

	/*--- End ---*/

	BINT MemHeap_Sec::MortonCmp(UBINT a[2], UBINT b[2]){
#if defined LIBENV_SYS_INTELX64
		if (a[1] == b[1]){
			if (a[0] == b[0])return 0;
			else if (a[0] > b[0])return 1;
			else return -1;
		}
		else if (a[1] > b[1])return 1;
		else return -1;
#elif defined LIBENV_SYS_INTELX86
		if (*(UINT8b *)a == *(UINT8b *)b)return 0;
		else if (*(UINT8b *)a > *(UINT8b *)b)return 1;
		else return -1;
#endif
	}
	UBINT MemHeap_Sec::pwr2minus1(UBINT x){ // won't work when x is 0
#if defined LIBENV_CPLR_VS
		UINT4b TmpInt;
#if defined LIBENV_SYS_INTELX64
		_BitScanReverse64(&TmpInt, x);
#elif defined LIBENV_SYS_INTELX86
		_BitScanReverse(&TmpInt, x);
#endif
#elif defined LIBENV_CPLR_GCC
#if defined LIBENV_SYS_INTELX64
		UINT4b TmpInt = 63 - __builtin_clzll(x);
#elif defined LIBENV_SYS_INTELX86
		UINT4b TmpInt = 31 - __builtin_clz(x);
#endif
#endif
		return x | ((1 << TmpInt) - 1);
		//#if defined LIBENV_SYS_INTELX64
		//	x |= x >> 32;
		//#endif
		//	x |= x >> 16;
		//	x |= x >> 8;
		//	x |= x >> 4;
		//	x |= x >> 2;
		//	x |= x >> 1;
		//	return x;
	}
	void MemHeap_Sec::_insert(_PageInfo *PageNode){
		PageNode->SetStub.Child[0] = nullptr;
		PageNode->SetStub.Child[1] = nullptr;
		if (nullptr == this->Root){
			this->Root = PageNode;
			PageNode->SetStub.Parent = nullptr;
			PageNode->SetStub.Color = 0;
		}
		else{
			_PageInfo *ParentNode = this->Root;
			PageNode->SetStub.Color = 1; //red
			while (1){
				RBTreeStub **TmpStubPtr;
				BINT CmpResult = MortonCmp(ParentNode->MortonCode, PageNode->MortonCode);
				if (0 == CmpResult)throw new std::exception("fatal error: one address is allocated twice by the system.");
				else if (CmpResult < 0)TmpStubPtr = &(ParentNode->SetStub.Child[1]);
				else TmpStubPtr = &(ParentNode->SetStub.Child[0]);
				if (nullptr == *TmpStubPtr){
					*TmpStubPtr = &(PageNode->SetStub);
					PageNode->SetStub.Parent = &(ParentNode->SetStub);
					break;
				}
				else ParentNode = (_PageInfo *)(*TmpStubPtr);
			}
			RBTree_Insert_Rebalance((RBTreeStub **)&(this->Root), &(PageNode->SetStub));
		}
	}
	void MemHeap_Sec::_delete(_PageInfo *PageNode){
		if (nullptr != this->Root){
			RBTree_Delete_Rebalance((RBTreeStub **)&(this->Root), &(PageNode->SetStub));
		}
	}
	MemHeap_Sec::_PageInfo *MemHeap_Sec::_search_addr(UBINT Addr){
		if (0 == Addr || nullptr == this->Root)return nullptr;
		else{
			UBINT SizeBound[2] = { 0, (UBINT)-1 }, StackDepth = 0;
			_PageInfo *CurNode = this->Root, *RetValue = nullptr;
			struct SearchStackNode{
				UBINT Bound;
				_PageInfo *LChild;
			}SearchStack[MaxSearchStackDepth];

			while (true){
				if (nullptr == CurNode){
					if (0 == StackDepth)break;
					else{
						StackDepth--;
						SizeBound[1] = SizeBound[0];
						SizeBound[0] = SearchStack[StackDepth].Bound;
						if (SizeBound[0] == SizeBound[1])CurNode = nullptr;
						else CurNode = SearchStack[StackDepth].LChild;
					}
				}
				else{
					UBINT TmpPageInfo[2];
					Morton2_inv(CurNode->MortonCode, TmpPageInfo);
					SearchStack[StackDepth].Bound = SizeBound[0];
					if (TmpPageInfo[0] > Addr){
						SizeBound[0] = (TmpPageInfo[1] | (pwr2minus1(TmpPageInfo[0] ^ Addr) >> 1));
						if (SizeBound[0] < (UBINT)-1)SizeBound[0]++;
					}
					else if (TmpPageInfo[0] < Addr){
						SizeBound[0] = (TmpPageInfo[1] & (~(pwr2minus1(TmpPageInfo[0] ^ Addr) >> 1)));
					}
					else{
						RetValue = CurNode;
						break;
					}
					if (SizeBound[0] != SizeBound[1]){
						SearchStack[StackDepth].LChild = (_PageInfo *)CurNode->SetStub.Child[0];
						CurNode = (_PageInfo *)CurNode->SetStub.Child[1];
						StackDepth++;
					}
					else{
						SizeBound[1] = SizeBound[0];
						SizeBound[0] = SearchStack[StackDepth].Bound;
						CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
					}
				}
			}
			return RetValue;
		}
	}
	MemHeap_Sec::_PageInfo *MemHeap_Sec::_search_size(UBINT Size){
		if (0 == Size || nullptr == this->Root)return nullptr;
		else{
			UBINT AddrBound[2] = { 0, (UBINT)-1 }, StackDepth = 0;
			_PageInfo *CurNode = this->Root, *RetValue = nullptr;
			struct SearchStackNode{
				UBINT Bound;
				_PageInfo *LChild;
			}SearchStack[MaxSearchStackDepth];

			while (true){
				if (nullptr == CurNode){
					if (0 == StackDepth)break;
					else{
						StackDepth--;
						AddrBound[1] = AddrBound[0];
						AddrBound[0] = SearchStack[StackDepth].Bound;
						if (AddrBound[0] == AddrBound[1])CurNode = nullptr;
						else CurNode = SearchStack[StackDepth].LChild;
					}
				}
				else{
					UBINT TmpPageInfo[2];
					Morton2_inv(CurNode->MortonCode, TmpPageInfo);
					SearchStack[StackDepth].Bound = AddrBound[0];
					if (TmpPageInfo[1] > Size){
						AddrBound[0] = (TmpPageInfo[0] | pwr2minus1(TmpPageInfo[1] ^ Size));
						if (AddrBound[0] < (UBINT)-1)AddrBound[0] += 1;
					}
					else if (TmpPageInfo[1] < Size){
						AddrBound[0] = (TmpPageInfo[0] & (~pwr2minus1(TmpPageInfo[1] ^ Size)));
					}
					else{
						RetValue = CurNode;
						break;
					}
					if (AddrBound[0] != AddrBound[1]){
						SearchStack[StackDepth].LChild = (_PageInfo *)CurNode->SetStub.Child[0];
						CurNode = (_PageInfo *)CurNode->SetStub.Child[1];
						StackDepth++;
					}
					else{
						AddrBound[1] = AddrBound[0];
						AddrBound[0] = SearchStack[StackDepth].Bound;
						CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
					}
				}
			}
			return RetValue;
		}
	}
	MemHeap_Sec::_PageInfo *MemHeap_Sec::_search_addr_size(UBINT Addr, UBINT Size){
		if (nullptr == this->Root)return nullptr;
		else{
			UBINT TmpMortonCode[2];
			Morton2(Addr, Size, TmpMortonCode);
			_PageInfo *CurNode = this->Root;
			while (1){
				BINT CmpResult = MortonCmp(CurNode->MortonCode, TmpMortonCode);
				if (0 == CmpResult)return CurNode;
				else if (CmpResult > 0)CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				else CurNode = (_PageInfo *)CurNode->SetStub.Child[1];
				if (nullptr == CurNode)return nullptr;
			}
		}
	}
	void *MemHeap_Sec::_alloc_medium(UBINT Size){
		_PageInfo *TgtPage = this->_search_size(Size);
		if (!TgtPage){
			TgtPage = (_PageInfo *)nsBasic::GetPage(nsEnv::SysPageSize);
			if (nullptr == TgtPage)return nullptr;
			Morton2((UBINT)TgtPage, Size, TgtPage->MortonCode);
			TgtPage->BlockCount = 0;
			UBINT i = (UBINT)TgtPage + sizeof(_PageInfo);
			*(UBINT *)i = (UBINT)nullptr;
			for (i += Size; i <= (UBINT)TgtPage + nsEnv::SysPageSize - Size; i += Size)*(UBINT *)i = i - Size;
			TgtPage->NextEmpty = i - Size;
			_insert(TgtPage);
		}
		void *RetValue = (void *)(TgtPage->NextEmpty);
		TgtPage->NextEmpty = *(UBINT *)(TgtPage->NextEmpty);
		TgtPage->BlockCount++;
		if ((UBINT)nullptr == TgtPage->NextEmpty){
			_delete(TgtPage);
			TgtPage->MortonCode[1] |= FullMark;
			_insert(TgtPage);
		}
		return RetValue;
	}
	void *MemHeap_Sec::_alloc_large(UBINT Size){
		_PageInfo *TgtPage = this->_search_size(Size);
		if (!TgtPage){
			void *TgtPageAddr = (_PageInfo *)nsBasic::GetPage(nsEnv::SysPageSize);
			if (nullptr == TgtPageAddr)return nullptr;
			TgtPage = (_PageInfo *)_alloc_medium(sizeof(_PageInfo));
			if (nullptr == TgtPage){
				nsBasic::unGetPage(TgtPageAddr, nsEnv::SysPageSize);
				return nullptr;
			}
			Morton2((UBINT)TgtPageAddr, Size, TgtPage->MortonCode);
			TgtPage->BlockCount = 0;
			UBINT i = (UBINT)TgtPageAddr;
			*(UBINT *)i = (UBINT)nullptr;
			for (i += Size; i <= (UBINT)TgtPageAddr + nsEnv::SysPageSize - Size; i += Size)*(UBINT *)i = i - Size;
			TgtPage->NextEmpty = i - Size;
			_insert(TgtPage);
		}
		void *RetValue = (void *)(TgtPage->NextEmpty);
		TgtPage->NextEmpty = *(UBINT *)(TgtPage->NextEmpty);
		TgtPage->BlockCount++;
		if ((UBINT)nullptr == TgtPage->NextEmpty){
			_delete(TgtPage);
			TgtPage->MortonCode[1] |= FullMark;
			_insert(TgtPage);
		}
		return RetValue;
	}
	void *MemHeap_Sec::_alloc_huge(UBINT Size){
		void *TgtPageAddr = (_PageInfo *)nsBasic::GetPage(Size);
		if (nullptr == TgtPageAddr)return nullptr;
		_PageInfo *TgtPage = (_PageInfo *)_alloc_medium(sizeof(_PageInfo));
		if (nullptr == TgtPage){
			nsBasic::unGetPage(TgtPageAddr, Size);
			return nullptr;
		}
		Morton2((UBINT)TgtPageAddr, Size | FullMark, TgtPage->MortonCode);
		TgtPage->BlockCount = 1;
		TgtPage->NextEmpty = 0;
		_insert(TgtPage);
		return TgtPageAddr;
	}
	void MemHeap_Sec::_free_medium(_PageInfo *TgtPage, void *Addr){
		//memory check
		UBINT TmpInfo[2];
		Morton2_inv(TgtPage->MortonCode, TmpInfo);
		if (0 != (UBINT(Addr) - (UBINT)TgtPage - sizeof(_PageInfo)) % (TmpInfo[1] & (FullMark - 1))){
			throw new std::exception();
		}
		//end memory check
		TgtPage->BlockCount--;
		if (FullMark&TgtPage->MortonCode[1]){
			_delete(TgtPage);
			TgtPage->MortonCode[1] ^= FullMark;
			if (0 == TgtPage->BlockCount){
				nsBasic::unGetPage(TgtPage, nsEnv::SysPageSize);
				return;
			}
			else _insert(TgtPage);
		}
		if (0 == TgtPage->BlockCount){
			_delete(TgtPage);
			nsBasic::unGetPage(TgtPage, nsEnv::SysPageSize);
		}
		else{
			*(UBINT *)Addr = TgtPage->NextEmpty;
			TgtPage->NextEmpty = (UBINT)Addr;
		}
	}
	void MemHeap_Sec::_free_large(_PageInfo *TgtPage, void *Addr){
		UBINT PageInfo[2];
		Morton2_inv(TgtPage->MortonCode, PageInfo);

		//memory check
		if (0 != (UBINT(Addr) - PageInfo[0]) % (PageInfo[1] & (FullMark - 1))){
			throw new std::exception();
		}
		//end memory check

		TgtPage->BlockCount--;
		if (FullMark&PageInfo[1]){
			_delete(TgtPage);
			TgtPage->MortonCode[1] ^= FullMark;
			if (0 == TgtPage->BlockCount){
				//impossible
				throw new std::exception();
			}
			else _insert(TgtPage);
		}
		if (0 == TgtPage->BlockCount){
			_delete(TgtPage);
			nsBasic::unGetPage((void *)PageInfo[0], nsEnv::SysPageSize);
			_PageInfo *TmpPage = _search_addr_size(((UBINT)TgtPage)&(~(nsEnv::SysPageSize - 1)), (sizeof(_PageInfo) | FullMark));
			if (nullptr == TmpPage)TmpPage = _search_addr_size(((UBINT)TgtPage)&(~(nsEnv::SysPageSize - 1)), sizeof(_PageInfo));
			_free_medium(TmpPage, TgtPage);
		}
		else{
			*(UBINT *)Addr = TgtPage->NextEmpty;
			TgtPage->NextEmpty = (UBINT)Addr;
		}
	}
	void MemHeap_Sec::_free_huge(_PageInfo *TgtPage, void *Addr){
		UBINT PageInfo[2];
		Morton2_inv(TgtPage->MortonCode, PageInfo);
		_delete(TgtPage);
		nsBasic::unGetPage((void *)PageInfo[0], PageInfo[1] ^ FullMark);
		_PageInfo *TmpPage = _search_addr_size(((UBINT)TgtPage)&(~(nsEnv::SysPageSize - 1)), (sizeof(_PageInfo) | FullMark));
		if (nullptr == TmpPage)TmpPage = _search_addr_size(((UBINT)TgtPage)&(~(nsEnv::SysPageSize - 1)), sizeof(_PageInfo));
		_free_medium(TmpPage, TgtPage);
	}
	void *MemHeap_Sec::Alloc(UBINT Size){
		if (0 == Size || Size > FullMark - sizeof(_PageInfo))return nullptr;
		else if (Size > nsEnv::SysPageSize / 2)return _alloc_huge((Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize);
		else{
			UBINT RealSize = nsMath::ceil2power(Size);
			if (RealSize > sizeof(_PageInfo))return _alloc_large(RealSize);
			else if (RealSize > sizeof(UBINT))return _alloc_medium(RealSize);
			else return _alloc_medium(sizeof(UBINT));
		}
	}
	void *MemHeap_Sec::Alloc_Bin(UBINT Size){
		if (0 == Size || Size > FullMark - sizeof(_PageInfo))return nullptr;
		else if (Size > nsEnv::SysPageSize / 2)return _alloc_huge((Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize);
		else{
			if (Size > sizeof(_PageInfo))return _alloc_large(Size);
			else if (Size > sizeof(UBINT))return _alloc_medium(Size);
			else return _alloc_medium(sizeof(UBINT));
		}
	}
	void MemHeap_Sec::Free(void *Addr){
		if (nullptr != Addr){
			_PageInfo *TgtPage = _search_addr(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)));
			if (nullptr != TgtPage){
				UBINT TmpInfo[2];
				Morton2_inv(TgtPage->MortonCode, TmpInfo);
				TmpInfo[1] &= FullMark - 1;
				if (TmpInfo[1] > nsEnv::SysPageSize / 2)_free_huge(TgtPage, Addr);
				else if (TmpInfo[1] > sizeof(_PageInfo))_free_large(TgtPage, Addr);
				else _free_medium(TgtPage, Addr);
			}
		}
	}
	void MemHeap_Sec::Free(void *Addr, UBINT Size){
		if (nullptr != Addr){
			UBINT RealSize;
			if (Size > nsEnv::SysPageSize / 2){
				RealSize = (Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize;
				_PageInfo *TgtPage = _search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), (RealSize | FullMark));
				if (nullptr != TgtPage)_free_huge(TgtPage, Addr);
				else Free(Addr);
			}
			else{
				if (Size <= sizeof(UBINT))RealSize = sizeof(UBINT);
				else RealSize = nsMath::ceil2power(Size);

				_PageInfo *TgtPage = _search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), (RealSize | FullMark));
				if (nullptr == TgtPage)TgtPage = _search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), RealSize);
				if (nullptr == TgtPage)Free(Addr);
				else if (RealSize > sizeof(_PageInfo))_free_large(TgtPage, Addr);
				else _free_medium(TgtPage, Addr);
			}
		}
	}
	void MemHeap_Sec::Free_Bin(void *Addr, UBINT Size){
		if (nullptr != Addr){
			UBINT RealSize;
			if (Size > nsEnv::SysPageSize / 2){
				RealSize = (Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize;
				_PageInfo *TgtPage = _search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), (RealSize | FullMark));
				if (nullptr != TgtPage)_free_huge(TgtPage, Addr);
				else Free(Addr);
			}
			else{
				if (Size <= sizeof(UBINT))RealSize = sizeof(UBINT);
				else RealSize = Size;

				_PageInfo *TgtPage = _search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), (RealSize | FullMark));
				if (nullptr == TgtPage)TgtPage = _search_addr_size(((UBINT)Addr)&(~(nsEnv::SysPageSize - 1)), RealSize);
				if (nullptr == TgtPage)Free(Addr);
				else if(RealSize > sizeof(_PageInfo))_free_large(TgtPage, Addr);
				else _free_medium(TgtPage, Addr);
			}
		}
	}
	void MemHeap_Sec::Clear(){
		if (nullptr != this->Root){
			_PageInfo *TmpList = nullptr, *CurNode = this->Root;
			while (1){
				if (CurNode->SetStub.Child[0] != nullptr)CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				else if (CurNode->SetStub.Child[1] != nullptr)CurNode = (_PageInfo *)CurNode->SetStub.Child[1];
				else{
					CurNode->MortonCode[1] &= ~FullMark;
					UBINT TmpPageInfo[2];
					Morton2_inv(CurNode->MortonCode, TmpPageInfo);
					if (TmpPageInfo[1] <= sizeof(_PageInfo)){
						CurNode->MortonCode[0] = TmpPageInfo[0];
						CurNode->MortonCode[1] = TmpPageInfo[1];
						CurNode->SetStub.Child[0] = &(TmpList->SetStub);
						TmpList = CurNode;
					}
					else{
						if (TmpPageInfo[1] > nsEnv::SysPageSize / 2)nsBasic::unGetPage((void *)TmpPageInfo[0], TmpPageInfo[1]);
						else nsBasic::unGetPage((void *)TmpPageInfo[0], nsEnv::SysPageSize);
					}
					_PageInfo *TmpNode = (_PageInfo *)CurNode->SetStub.Parent;
					if (nullptr == TmpNode)break;
					else if (TmpNode->SetStub.Child[0] == &(CurNode->SetStub))TmpNode->SetStub.Child[0] = nullptr;
					else TmpNode->SetStub.Child[1] = nullptr;
					CurNode = TmpNode;
				}
			}
			while (nullptr != TmpList){
				_PageInfo *NextNode = (_PageInfo *)TmpList->SetStub.Child[0];
				nsBasic::unGetPage((void *)TmpList->MortonCode[0], nsEnv::SysPageSize);
				TmpList = NextNode;
			}
			this->Root = nullptr;
		}
	}
	UBINT MemHeap_Sec::AllocedBytes(){
		if (nullptr != this->Root){
			_PageInfo *CurNode = this->Root;
			_PageInfo *SearchStack[MaxSearchStackDepth];
			UBINT SearchStackDepth = 0, Result = 0;
			while (1){
				if (nullptr == CurNode){
					if (0 == SearchStackDepth)break;
					SearchStackDepth--;
					CurNode = SearchStack[SearchStackDepth];
				}
				else{
					if (nullptr != CurNode->SetStub.Child[1]){
						SearchStack[SearchStackDepth] = (_PageInfo *)CurNode->SetStub.Child[1];
						SearchStackDepth++;
					}

					UBINT TmpPageInfo[2];
					Morton2_inv(CurNode->MortonCode, TmpPageInfo);
					TmpPageInfo[1] &= ~FullMark;
					Result += TmpPageInfo[1] * CurNode->BlockCount;
					CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				}
			}
			return Result;
		}
		else return 0;
	}
	UBINT MemHeap_Sec::OccupiedBytes(){
		if (nullptr != this->Root){
			_PageInfo *CurNode = this->Root;
			_PageInfo *SearchStack[MaxSearchStackDepth];
			UBINT SearchStackDepth = 0, Result = 0;
			while (1){
				if (nullptr == CurNode){
					if (0 == SearchStackDepth)break;
					SearchStackDepth--;
					CurNode = SearchStack[SearchStackDepth];
				}
				else{
					if (nullptr != CurNode->SetStub.Child[1]){
						SearchStack[SearchStackDepth] = (_PageInfo *)CurNode->SetStub.Child[1];
						SearchStackDepth++;
					}
					UBINT TmpPageInfo[2];
					Morton2_inv(CurNode->MortonCode, TmpPageInfo);
					TmpPageInfo[1] &= ~FullMark;
					if (TmpPageInfo[1] <= nsEnv::SysPageSize / 2)Result += nsEnv::SysPageSize;
					else Result += TmpPageInfo[1];
					CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				}
			}
			return Result;
		}
		else return 0;
	}

	BINT MemHeap_Fast::PageCmp(UBINT a[2], UBINT b[2]){
#if defined LIBENV_SYS_INTELX64
		if (a[1] == b[1]){
			if (a[0] == b[0])return 0;
			else if (a[0] > b[0])return 1;
			else return -1;
		}
		else if (a[1] > b[1])return 1;
		else return -1;
#elif defined LIBENV_SYS_INTELX86
		if (*(UINT8b *)a == *(UINT8b *)b)return 0;
		else if (*(UINT8b *)a > *(UINT8b *)b)return 1;
		else return -1;
#endif
	}
	UBINT MemHeap_Fast::pwr2minus1(UBINT x){ // won't work when x is 0
#if defined LIBENV_CPLR_VS
		UINT4b TmpInt;
#if defined LIBENV_SYS_INTELX64
		_BitScanReverse64(&TmpInt, x);
#elif defined LIBENV_SYS_INTELX86
		_BitScanReverse(&TmpInt, x);
#endif
#elif defined LIBENV_CPLR_GCC
#if defined LIBENV_SYS_INTELX64
		UINT4b TmpInt = 63 - __builtin_clzll(x);
#elif defined LIBENV_SYS_INTELX86
		UINT4b TmpInt = 31 - __builtin_clz(x);
#endif
#endif
		return x | ((1 << TmpInt) - 1);
		//#if defined LIBENV_SYS_INTELX64
		//	x |= x >> 32;
		//#endif
		//	x |= x >> 16;
		//	x |= x >> 8;
		//	x |= x >> 4;
		//	x |= x >> 2;
		//	x |= x >> 1;
		//	return x;
	}
	void MemHeap_Fast::_insert(_PageInfo *PageNode){
		PageNode->SetStub.Child[0] = nullptr;
		PageNode->SetStub.Child[1] = nullptr;
		if (nullptr == this->Root){
			this->Root = PageNode;
			PageNode->SetStub.Parent = nullptr;
			PageNode->SetStub.Color = 0;
		}
		else{
			_PageInfo *ParentNode = this->Root;
			PageNode->SetStub.Color = 1; //red
			while (1){
				RBTreeStub **TmpStubPtr;
				BINT CmpResult = PageCmp(ParentNode->Addr_Size, PageNode->Addr_Size);
				if (0 == CmpResult)throw new std::exception("fatal error: one address is allocated twice by the system.");
				else if (CmpResult < 0)TmpStubPtr = &(ParentNode->SetStub.Child[1]);
				else TmpStubPtr = &(ParentNode->SetStub.Child[0]);
				if (nullptr == *TmpStubPtr){
					*TmpStubPtr = &(PageNode->SetStub);
					PageNode->SetStub.Parent = &(ParentNode->SetStub);
					break;
				}
				else ParentNode = (_PageInfo *)(*TmpStubPtr);
			}
			RBTree_Insert_Rebalance((RBTreeStub **)&(this->Root), &(PageNode->SetStub));
		}
	}
	void MemHeap_Fast::_delete(_PageInfo *PageNode){
		if (nullptr != this->Root){
			RBTree_Delete_Rebalance((RBTreeStub **)&(this->Root), &(PageNode->SetStub));
		}
	}
	MemHeap_Fast::_PageInfo *MemHeap_Fast::_search_size(UBINT Size){
		if (nullptr == this->Root)return nullptr;
		else{
			_PageInfo *CurNode = this->Root;
			while (1){
				if (CurNode->Addr_Size[1] == Size)return CurNode;
				else if (CurNode->Addr_Size[1] > Size)CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				else CurNode = (_PageInfo *)CurNode->SetStub.Child[1];
				if (nullptr == CurNode)return nullptr;
			}
		}
	}
	MemHeap_Fast::_PageInfo *MemHeap_Fast::_search_addr_size(UBINT Addr, UBINT Size){
		if (nullptr == this->Root)return nullptr;
		else{
			UBINT Addr_Size_Temp[2] = { Addr, Size };
			_PageInfo *CurNode = this->Root;
			while (1){
				BINT CmpResult = PageCmp(CurNode->Addr_Size, Addr_Size_Temp);
				if (0 == CmpResult)return CurNode;
				else if (CmpResult > 0)CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				else CurNode = (_PageInfo *)CurNode->SetStub.Child[1];
				if (nullptr == CurNode)return nullptr;
			}
		}
	}
	void *MemHeap_Fast::_alloc_medium(UBINT Size){
		_PageInfo *TgtPage = this->_search_size(Size);
		if (!TgtPage){
			TgtPage = (_PageInfo *)nsBasic::GetPage(nsEnv::SysPageSize);
			if (nullptr == TgtPage)return nullptr;
			TgtPage->Addr_Size[0] = (UBINT)TgtPage;
			TgtPage->Addr_Size[1] = Size;
			TgtPage->BlockCount = 0;
			UBINT i = (UBINT)TgtPage + sizeof(_PageInfo);
			*(UBINT *)i = (UBINT)nullptr;
			for (i += Size; i <= (UBINT)TgtPage + nsEnv::SysPageSize - Size; i += Size)*(UBINT *)i = i - Size;
			TgtPage->NextEmpty = i - Size;
			_insert(TgtPage);
		}
		void *RetValue = (void *)(TgtPage->NextEmpty);
		TgtPage->NextEmpty = *(UBINT *)(TgtPage->NextEmpty);
		TgtPage->BlockCount++;
		if ((UBINT)nullptr == TgtPage->NextEmpty){
			_delete(TgtPage);
			TgtPage->Addr_Size[1] |= FullMark;
			_insert(TgtPage);
		}
		return RetValue;
	}
	void *MemHeap_Fast::_alloc_large(UBINT Size){
		_PageInfo *TgtPage = this->_search_size(Size);
		if (!TgtPage){
			void *TgtPageAddr = (_PageInfo *)nsBasic::GetPage(nsEnv::SysPageSize);
			if (nullptr == TgtPageAddr)return nullptr;
			TgtPage = (_PageInfo *)_alloc_medium(sizeof(_PageInfo));
			if (nullptr == TgtPage){
				nsBasic::unGetPage(TgtPageAddr, nsEnv::SysPageSize);
				return nullptr;
			}
			TgtPage->Addr_Size[0] = (UBINT)TgtPageAddr;
			TgtPage->Addr_Size[1] = Size;
			TgtPage->BlockCount = 0;
			UBINT i = (UBINT)TgtPageAddr;
			*(UBINT *)i = (UBINT)nullptr;
			for (i += Size; i <= (UBINT)TgtPageAddr + nsEnv::SysPageSize - Size; i += Size)*(UBINT *)i = i - Size;
			TgtPage->NextEmpty = i - Size;
			_insert(TgtPage);
		}
		void *RetValue = (void *)(TgtPage->NextEmpty);
		TgtPage->NextEmpty = *(UBINT *)(TgtPage->NextEmpty);
		TgtPage->BlockCount++;
		if ((UBINT)nullptr == TgtPage->NextEmpty){
			_delete(TgtPage);
			TgtPage->Addr_Size[1] |= FullMark;
			_insert(TgtPage);
		}
		return RetValue;
	}
	void *MemHeap_Fast::_alloc_huge(UBINT Size){
		void *TgtPageAddr = (_PageInfo *)nsBasic::GetPage(Size);
		if (nullptr == TgtPageAddr)return nullptr;
		_PageInfo *TgtPage = (_PageInfo *)_alloc_medium(sizeof(_PageInfo));
		if (nullptr == TgtPage){
			nsBasic::unGetPage(TgtPageAddr, Size);
			return nullptr;
		}
		TgtPage->Addr_Size[0] = (UBINT)TgtPageAddr;
		TgtPage->Addr_Size[1] = FullMark;
		TgtPage->BlockCount = Size;
		TgtPage->NextEmpty = 0;
		_insert(TgtPage);
		return TgtPageAddr;
	}
	void MemHeap_Fast::_free_medium(_PageInfo *TgtPage, void *Addr){
		TgtPage->BlockCount--;
		if (FullMark&TgtPage->Addr_Size[1]){
			_delete(TgtPage);
			TgtPage->Addr_Size[1] ^= FullMark;
			if (0 == TgtPage->BlockCount){
				nsBasic::unGetPage(TgtPage, nsEnv::SysPageSize);
				return;
			}
			else _insert(TgtPage);
		}
		if (0 == TgtPage->BlockCount){
			_delete(TgtPage);
			nsBasic::unGetPage(TgtPage, nsEnv::SysPageSize);
		}
		else{
			*(UBINT *)Addr = TgtPage->NextEmpty;
			TgtPage->NextEmpty = (UBINT)Addr;
		}
	}
	void MemHeap_Fast::_free_large(_PageInfo *TgtPage, void *Addr){
		TgtPage->BlockCount--;
		if (FullMark&TgtPage->Addr_Size[1]){
			_delete(TgtPage);
			TgtPage->Addr_Size[1] ^= FullMark;
			_insert(TgtPage);
		}
		if (0 == TgtPage->BlockCount){
			_delete(TgtPage);
			nsBasic::unGetPage((void *)TgtPage->Addr_Size[0], nsEnv::SysPageSize);
			_free_medium((_PageInfo *)(((UBINT)TgtPage)&(~(nsEnv::SysPageSize - 1))), TgtPage);
		}
		else{
			*(UBINT *)Addr = TgtPage->NextEmpty;
			TgtPage->NextEmpty = (UBINT)Addr;
		}
	}
	void MemHeap_Fast::_free_huge(_PageInfo *TgtPage, void *Addr){
		_delete(TgtPage);
		nsBasic::unGetPage((void *)TgtPage->Addr_Size[0], TgtPage->BlockCount);
		_free_medium((_PageInfo *)(((UBINT)TgtPage)&(~(nsEnv::SysPageSize - 1))), TgtPage);
	}
	void *MemHeap_Fast::Alloc(UBINT Size){
		if (0 == Size || Size >= FullMark)return nullptr;
		else if (Size > nsEnv::SysPageSize / 2)return _alloc_huge((Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize);
		else if (Size > (nsEnv::SysPageSize - sizeof(_PageInfo)) / 2)return _alloc_large(nsEnv::SysPageSize / 2);
		else{
			UBINT RealSize;
			if (Size > nsEnv::SysPageSize / 4)RealSize = Size;
			else if (Size <= sizeof(UBINT))RealSize = sizeof(UBINT);
			else RealSize = nsMath::ceil2power(Size);
			return _alloc_medium(RealSize);
		}
	}
	void *MemHeap_Fast::Alloc_Bin(UBINT Size){
		if (0 == Size || Size >= FullMark)return nullptr;
		else if (Size > nsEnv::SysPageSize / 2)return _alloc_huge((Size - 1) - ((Size - 1) % nsEnv::SysPageSize) + nsEnv::SysPageSize);
		else if (Size > (nsEnv::SysPageSize - sizeof(_PageInfo)) / 2)return _alloc_large(nsEnv::SysPageSize / 2);
		else{
			if (Size > sizeof(UBINT))return _alloc_medium(Size);
			else return _alloc_medium(sizeof(UBINT));
		}
	}
	void MemHeap_Fast::Free(void *Addr){
		if (nullptr != Addr){
			UBINT AddrVal = (UBINT)Addr;
			UBINT AddrVal_Index = AddrVal&(nsEnv::SysPageSize - 1);
			UBINT AddrVal_Page = AddrVal - AddrVal_Index;
			if (0 == AddrVal_Index){
				_PageInfo *TgtPage = _search_addr_size(AddrVal_Page, FullMark);
				if (nullptr == TgtPage){
					TgtPage = _search_addr_size(AddrVal_Page, (nsEnv::SysPageSize / 2) | FullMark);
					if (nullptr == TgtPage)TgtPage = _search_addr_size(AddrVal_Page, nsEnv::SysPageSize / 2); 
					if (nullptr != TgtPage)_free_large(TgtPage, Addr);
				}
				else _free_huge(TgtPage, Addr);
			}
			else if (nsEnv::SysPageSize / 2 == AddrVal_Index){
				_PageInfo *TgtPage = _search_addr_size(AddrVal_Page, (nsEnv::SysPageSize / 2) | FullMark);
				if (nullptr == TgtPage)TgtPage = _search_addr_size(AddrVal_Page, nsEnv::SysPageSize / 2);
				if (nullptr == TgtPage)_free_medium((_PageInfo *)AddrVal_Page, Addr);
				else _free_large(TgtPage, Addr);
			}
			else _free_medium((_PageInfo *)AddrVal_Page, Addr);
		}
	}
	void MemHeap_Fast::Free(void *Addr, UBINT Size){
		//If the input is incorrect, this function may crash your program.
		if (nullptr != Addr){
			UBINT AddrVal = (UBINT)Addr;
			UBINT AddrVal_Index = AddrVal&(nsEnv::SysPageSize - 1);
			UBINT AddrVal_Page = AddrVal - AddrVal_Index;
			if (Size > nsEnv::SysPageSize / 2){
				_PageInfo *TgtPage = _search_addr_size(AddrVal_Page, FullMark);
				if (nullptr != TgtPage)_free_huge(TgtPage, Addr);
			}
			else if (Size > (nsEnv::SysPageSize - sizeof(_PageInfo)) / 2){
				_PageInfo *TgtPage = _search_addr_size(AddrVal_Page, (nsEnv::SysPageSize / 2) | FullMark);
				if (nullptr == TgtPage)TgtPage = _search_addr_size(AddrVal_Page, nsEnv::SysPageSize / 2);
				if (nullptr != TgtPage)_free_large(TgtPage, Addr);
			}
			else _free_medium((_PageInfo *)AddrVal_Page, Addr);
		}
	}
	void MemHeap_Fast::Free_Bin(void *Addr, UBINT Size){ Free(Addr, Size); }
	void MemHeap_Fast::Clear(){
		if (nullptr != this->Root){
			_PageInfo *TmpList = nullptr, *CurNode = this->Root;
			while (1){
				if (CurNode->SetStub.Child[0] != nullptr)CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				else if (CurNode->SetStub.Child[1] != nullptr)CurNode = (_PageInfo *)CurNode->SetStub.Child[1];
				else{
					CurNode->Addr_Size[1] &= ~FullMark;
					if (0 == CurNode->Addr_Size[1])nsBasic::unGetPage((void *)CurNode->Addr_Size[0], CurNode->BlockCount);
					else if (nsEnv::SysPageSize / 2 <= CurNode->Addr_Size[1])nsBasic::unGetPage((void *)CurNode->Addr_Size[0], nsEnv::SysPageSize);
					else{
						CurNode->SetStub.Child[0] = &(TmpList->SetStub);
						TmpList = CurNode;
					}
					_PageInfo *TmpNode = (_PageInfo *)CurNode->SetStub.Parent;
					if (nullptr == TmpNode)break;
					else if (TmpNode->SetStub.Child[0] == &(CurNode->SetStub))TmpNode->SetStub.Child[0] = nullptr;
					else TmpNode->SetStub.Child[1] = nullptr;
					CurNode = TmpNode;
				}
			}
			while (nullptr != TmpList){
				_PageInfo *NextNode = (_PageInfo *)TmpList->SetStub.Child[0];
				nsBasic::unGetPage((void *)TmpList->Addr_Size[0], nsEnv::SysPageSize);
				TmpList = NextNode;
			}
			this->Root = nullptr;
		}
	}
	UBINT MemHeap_Fast::AllocedBytes(){
		if (nullptr != this->Root){
			_PageInfo *CurNode = this->Root;
			_PageInfo *SearchStack[MaxSearchStackDepth];
			UBINT SearchStackDepth = 0, Result = 0;
			while (1){
				if (nullptr == CurNode){
					if (0 == SearchStackDepth)break;
					SearchStackDepth--;
					CurNode = SearchStack[SearchStackDepth];
				}
				else{
					if (nullptr != CurNode->SetStub.Child[1]){
						SearchStack[SearchStackDepth] = (_PageInfo *)CurNode->SetStub.Child[1];
						SearchStackDepth++;
					}

					UBINT TmpPageSize = CurNode->Addr_Size[1] & ~FullMark;
					if (0 == TmpPageSize)Result += CurNode->BlockCount;
					else Result += TmpPageSize * CurNode->BlockCount;
					CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				}
			}
			return Result;
		}
		else return 0;
	}
	UBINT MemHeap_Fast::OccupiedBytes(){
		if (nullptr != this->Root){
			_PageInfo *CurNode = this->Root;
			_PageInfo *SearchStack[MaxSearchStackDepth];
			UBINT SearchStackDepth = 0, Result = 0;
			while (1){
				if (nullptr == CurNode){
					if (0 == SearchStackDepth)break;
					SearchStackDepth--;
					CurNode = SearchStack[SearchStackDepth];
				}
				else{
					if (nullptr != CurNode->SetStub.Child[1]){
						SearchStack[SearchStackDepth] = (_PageInfo *)CurNode->SetStub.Child[1];
						SearchStackDepth++;
					}
					if (FullMark == CurNode->Addr_Size[1])Result += CurNode->BlockCount;
					else Result += nsEnv::SysPageSize;
					CurNode = (_PageInfo *)CurNode->SetStub.Child[0];
				}
			}
			return Result;
		}
		else return 0;
	}
}
#endif