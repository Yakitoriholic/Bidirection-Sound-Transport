/* Description:Platform-independent common algorithms.
* Language:C++
* Author:***
*/

#ifndef LIB_ALG
#define LIB_ALG

#include "lTemplate.hpp"

namespace nsAlg{

#if defined LIBENV_SYS_INTELX86
	static const UBINT MaxQSortDepth = 32 - 5 + 1; // log_2^ISortCutoff = 5
	static const UBINT ISortCutoff = 32;
#elif defined LIBENV_SYS_INTELX64
	static const UBINT MaxQSortDepth = 64 - 5 + 1; // log_2^ISortCutoff = 5
	static const UBINT ISortCutoff = 32;
#endif

	extern UBINT Search_Mem_Mem_Fwd(const void *lpMain, UBINT MainSize, const void *lpPattern, UBINT PatternSize);
	//Do a forward search for the pattern [ lpPattern[0 -- PatternSize - 1] ] in [lpMain[0 -- MainSize - 1] ]. The first occurrence position of the pattern is returned.
	//(UBINT)-1 is returned when the pattern is not found.
	//0 is returned for a zero-sized pattern.

	extern UBINT Search_Mem_Str_Fwd(const void *lpMain, UBINT MainSize, const char *lpPattern);
	//Do a forward search for the zero-ended string [lpPattern] in [ lpMain[0 -- MainSize - 1] ]. The first occurrence position of the pattern is returned.
	//(UBINT)-1 is returned when the pattern is not found.
	//0 is returned for a zero-sized pattern.
	//This function may cause memory access violation when [lpPattern] is not terminated with a zero.

	extern void *Search_Mem_Chr_Bwd(const void *lpMain, UBINT MainSize, unsigned char Char);
	//Do a backward search for [Char] in [ lpMain[0 -- MainSize - 1] ]. The first occurrence position of the pattern is returned.
	//return nullptr when the pattern is not found.

	extern UBINT Search_Mem_Mem_Bwd(const void *lpMain, UBINT MainSize, const void *lpPattern, UBINT PatternSize);
	//Do a backward search for the pattern [ lpPattern[0 -- PatternSize - 1] ] in [ lpMain[0 -- MainSize - 1] ]. The first occurrence position of the pattern is returned.
	//(UBINT)-1 is returned when the pattern is not found.
	//0 is returned for a zero-sized pattern.

	template <class RandomAccessIterator, class T, class Compare = nsTemplate::compare<T>> extern UBINT Binary_Search(RandomAccessIterator first, RandomAccessIterator last, const T& value, Compare comp);
	template <class RandomAccessIterator, class T> extern inline UBINT Binary_Search(RandomAccessIterator first, RandomAccessIterator last, const T& value){
		nsTemplate::compare<T> comp;
		return Binary_Search(first, last, value, comp);
	}
	//returns the index of [value] in the sorted range [first, last). (UBINT)-1 is returned when the value is not found.

	template <class RandomAccessIterator, class Compare> extern void Sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp);
	template <class RandomAccessIterator> extern inline void Sort(RandomAccessIterator first, RandomAccessIterator last){
		nsTemplate::compare<std::iterator_traits<RandomAccessIterator>::value_type> comp;
		Sort(first, last, comp);
	}
	//sort the value in range [first, last) by ascending order.

	extern void Sort_C(void *lpArray, UBINT ElementCnt, UBINT ElementSize, BINT(*CmpFunc)(void *, void *));

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	extern UBINT Search_Mem_Mem_Fwd(const void *lpMain, UBINT MainSize, const void *lpPattern, UBINT PatternSize){
		if (0 == PatternSize)return 0;
		else if(PatternSize > MainSize)return (UBINT)-1;
		else if (1 == PatternSize)return (UBINT)memchr(lpMain, *(const char *)lpPattern, MainSize) - (UBINT)lpMain;
		else if (PatternSize <= sizeof(UBINT)){
			UBINT Mask = (((UBINT)1 << (PatternSize * 8 - 1)) << 1) - 1;
			UBINT Main = 0, Pattern = 0;
			const unsigned char *lpMain_Next = ((const unsigned char *)lpMain);
			const unsigned char *lpPattern_Next = ((const unsigned char *)lpPattern);
			for (UBINT i = 0; i < PatternSize; i++){
				Main = (Main << 8) + (*lpMain_Next);
				lpMain_Next++;
			}
			for (UBINT i = 0; i < PatternSize; i++){
				Pattern = (Pattern << 8) + (*lpPattern_Next);
				lpPattern_Next++;
			}

			UBINT Result;
			for (Result = 0; Result < MainSize - PatternSize; Result++){
				if (0 == ((Main ^ Pattern) & Mask))break;
				Main = (Main << 8) + (*lpMain_Next);
				lpMain_Next++;
			}
			if (Result < MainSize - PatternSize)return Result; else return (UBINT)-1;
		}
		else{
			//preprocessing
			const unsigned char *CriticalPtr;
			UBINT SuffixPeriod;

			//compute the critical factorization
			UBINT LengthScanned;
			const unsigned char *CandidatePtr;

			//find the maximal suffix
			const unsigned char *CurMaxSuffixPtr = (const unsigned char *)lpPattern;
			UBINT SuffixPeriod0 = 1;
			LengthScanned = 0;
			CandidatePtr = CurMaxSuffixPtr + 1;
			while ((UBINT)(CandidatePtr - (const unsigned char *)lpPattern) < PatternSize){
				unsigned char C1 = *CurMaxSuffixPtr;
				unsigned char C2 = *CandidatePtr;
				if (C1 > C2){ CandidatePtr++; LengthScanned = 0; SuffixPeriod0 = CandidatePtr - CurMaxSuffixPtr; }
				else if (C1 < C2){ CurMaxSuffixPtr = CandidatePtr - LengthScanned; CandidatePtr = CurMaxSuffixPtr + 1; LengthScanned = 0; SuffixPeriod0 = 1; }
				else if (LengthScanned == SuffixPeriod0){ CandidatePtr++; LengthScanned = 0; }
				else { CandidatePtr++; LengthScanned++; }
			}

			//find the inverse maximal suffix
			//inverse maximal suffix != minimal suffix
			const unsigned char *CurInvMaxSuffixPtr = (const unsigned char *)lpPattern;
			UBINT SuffixPeriod1 = 1;
			LengthScanned = 0;
			CandidatePtr = CurInvMaxSuffixPtr + 1;
			while ((UBINT)(CandidatePtr - (const unsigned char *)lpPattern) < PatternSize){
				unsigned char C1 = *CurInvMaxSuffixPtr;
				unsigned char C2 = *CandidatePtr;
				if (C1 < C2){ CandidatePtr++; LengthScanned = 0; SuffixPeriod1 = CandidatePtr - CurInvMaxSuffixPtr; }
				else if (C1 > C2){ CurInvMaxSuffixPtr = CandidatePtr - LengthScanned; CandidatePtr = CurInvMaxSuffixPtr + 1; LengthScanned = 0; SuffixPeriod1 = 1; }
				else if (LengthScanned == SuffixPeriod1){ CandidatePtr++; LengthScanned = 0; }
				else { CandidatePtr++; LengthScanned++; }
			}

			if (CurMaxSuffixPtr <= CurInvMaxSuffixPtr){
				CriticalPtr = CurMaxSuffixPtr;
				SuffixPeriod = SuffixPeriod0;
			}
			else{
				CriticalPtr = CurInvMaxSuffixPtr;
				SuffixPeriod = SuffixPeriod1;
			}

			UBINT CriticalPos = CriticalPtr - (const unsigned char *)lpPattern;
			const unsigned char *lpMain_Next = ((const unsigned char *)lpMain);
			const unsigned char *RightSentry = lpMain_Next + MainSize - PatternSize;

			//calculate the period
			UBINT Period;
			if (CriticalPos < PatternSize / 2 && 0 == memcmp(lpPattern, CriticalPtr, CriticalPos))Period = SuffixPeriod;
			else{
				Period = PatternSize - CriticalPos;
				if (CriticalPos>Period)Period = CriticalPos;
				Period++;
			}

			if (PatternSize <= 32){
				//two-way string searching
				while (lpMain_Next <= RightSentry){
					const unsigned char *lpC1 = lpMain_Next + CriticalPos;
					const unsigned char *lpC2 = CriticalPtr;
					//forward search
					UBINT i;
					for (i = CriticalPos; i < PatternSize; i++){
						if (*lpC1 != *lpC2)break;
						lpC1++;
						lpC2++;
					}
					if (i < PatternSize)lpMain_Next += i - CriticalPos + 1;
					else{
						//backward search
						lpC1 = lpMain_Next + CriticalPos - 1;
						lpC2 = CriticalPtr - 1;
						for (i = CriticalPos; i > 0; i--){
							if (*lpC1 != *lpC2)break;
							lpC1--;
							lpC2--;
						}
						if (i > 0)lpMain_Next += Period;
						else break;
					}
				}
				if (lpMain_Next == RightSentry + 1)return (UBINT)-1;
				else return lpMain_Next - (const unsigned char *)lpMain;
			}
			else{
				//two-way string searching + Boyer-Moore bad character rule

				UBINT BadCharTable[0x100];
				for (UBINT i = 0; i < 0x100; i++)BadCharTable[i] = PatternSize;
				for (UBINT i = 0; i < PatternSize - 1; i++){
					BadCharTable[((const unsigned char *)lpPattern)[i]] = PatternSize - 1 - i;
				}

				while (lpMain_Next <= RightSentry){
					const unsigned char *lpC1 = lpMain_Next + PatternSize - 1;
					const unsigned char *lpC2 = (const unsigned char *)lpPattern + PatternSize - 1;
					if (*lpC1 != *lpC2)lpMain_Next += BadCharTable[*lpC1]; //bad charater rule
					else{
						//two-way search
						const unsigned char *lpC1 = lpMain_Next + CriticalPos;
						const unsigned char *lpC2 = CriticalPtr;
						//forward search
						UBINT i;
						for (i = CriticalPos; i < PatternSize - 1; i++){
							if (*lpC1 != *lpC2)break;
							lpC1++;
							lpC2++;
						}
						if (i < PatternSize - 1)lpMain_Next += i - CriticalPos + 1;
						else{
							//backward search
							lpC1 = lpMain_Next + CriticalPos - 1;
							lpC2 = CriticalPtr - 1;
							for (i = CriticalPos; i > 0; i--){
								if (*lpC1 != *lpC2)break;
								lpC1--;
								lpC2--;
							}
							if (i > 0)lpMain_Next += Period;
							else break;
						}
					}
				}
				if (lpMain_Next == RightSentry + 1)return (UBINT)-1;
				else return lpMain_Next - (const unsigned char *)lpMain;
			}
		}
	}

	extern UBINT Search_Mem_Str_Fwd(const void *lpMain, UBINT MainSize, const char *lpPattern){
		if ('\0' == *lpPattern)return 0;
		else if ('\0' == lpPattern[1])return (UBINT)memchr(lpMain, *lpPattern, MainSize) - (UBINT)lpMain;
		else{
			UBINT Mask = 1, Main = 0, Pattern = 0, PatternSize = 0;
			const unsigned char *lpMain_Next = ((const unsigned char *)lpMain);
			const unsigned char *lpPattern_Next = ((const unsigned char *)lpPattern);

			while ('\0' != *lpPattern_Next){
				PatternSize++;
				if (PatternSize > MainSize)return (UBINT)-1;
				if (PatternSize > sizeof(UBINT))break;
				Main = (Main << 8) + (*lpMain_Next);
				lpMain_Next++;
				Pattern = (Pattern << 8) + (*lpPattern_Next);
				lpPattern_Next++;
				Mask <<= 8;
			}

			if (PatternSize <= sizeof(UBINT)){
				Mask--;

				UBINT Result;
				for (Result = 0; Result < MainSize - PatternSize; Result++){
					if (0 == ((Main ^ Pattern) & Mask))break;
					Main = (Main << 8) + (*lpMain_Next);
					lpMain_Next++;
				}
				if (Result < MainSize - PatternSize)return Result; else return (UBINT)-1;
			}
			else{
				const unsigned char *CriticalPtr;
				UBINT SuffixPeriod;

				//compute the critical factorization
				UBINT LengthScanned;
				const unsigned char *CandidatePtr;

				//find the maximal suffix
				const unsigned char *CurMaxSuffixPtr = (const unsigned char *)lpPattern;
				UBINT SuffixPeriod0 = 1;
				LengthScanned = 0;
				CandidatePtr = CurMaxSuffixPtr + 1;
				while ('\0' != *CandidatePtr){
					unsigned char C1 = *CurMaxSuffixPtr;
					unsigned char C2 = *CandidatePtr;
					if (C1 > C2){ CandidatePtr++; LengthScanned = 0; SuffixPeriod0 = CandidatePtr - CurMaxSuffixPtr; }
					else if (C1 < C2){ CurMaxSuffixPtr = CandidatePtr - LengthScanned; CandidatePtr = CurMaxSuffixPtr + 1; LengthScanned = 0; SuffixPeriod0 = 1; }
					else if (LengthScanned == SuffixPeriod0){ CandidatePtr++; LengthScanned = 0; }
					else { CandidatePtr++; LengthScanned++; }
				}
				PatternSize = CandidatePtr - (const unsigned char *)lpPattern;
				if (PatternSize > MainSize)return (UBINT)-1;

				//find the inverse maximal suffix
				//inverse maximal suffix != minimal suffix
				const unsigned char *CurInvMaxSuffixPtr = (const unsigned char *)lpPattern;
				UBINT SuffixPeriod1 = 1;
				LengthScanned = 0;
				CandidatePtr = CurInvMaxSuffixPtr + 1;
				while ('\0' != *CandidatePtr){
					unsigned char C1 = *CurInvMaxSuffixPtr;
					unsigned char C2 = *CandidatePtr;
					if (C1 < C2){ CandidatePtr++; LengthScanned = 0; SuffixPeriod1 = CandidatePtr - CurInvMaxSuffixPtr; }
					else if (C1 > C2){ CurInvMaxSuffixPtr = CandidatePtr - LengthScanned; CandidatePtr = CurInvMaxSuffixPtr + 1; LengthScanned = 0; SuffixPeriod1 = 1; }
					else if (LengthScanned == SuffixPeriod1){ CandidatePtr++; LengthScanned = 0; }
					else { CandidatePtr++; LengthScanned++; }
				}

				if (CurMaxSuffixPtr <= CurInvMaxSuffixPtr){
					CriticalPtr = CurMaxSuffixPtr;
					SuffixPeriod = SuffixPeriod0;
				}
				else{
					CriticalPtr = CurInvMaxSuffixPtr;
					SuffixPeriod = SuffixPeriod1;
				}

				UBINT CriticalPos = CriticalPtr - (const unsigned char *)lpPattern;
				const unsigned char *lpMain_Next = ((const unsigned char *)lpMain);
				const unsigned char *RightSentry = lpMain_Next + MainSize - PatternSize;

				UBINT Period;
				if (CriticalPos < PatternSize / 2 && 0 == memcmp(lpPattern, CriticalPtr, CriticalPos))Period = SuffixPeriod;
				else{
					Period = PatternSize - CriticalPos;
					if (CriticalPos>Period)Period = CriticalPos;
					Period++;
				}
				
				if (PatternSize <= 32){
					//two-way string searching
					while (lpMain_Next <= RightSentry){
						const unsigned char *lpC1 = lpMain_Next + CriticalPos;
						const unsigned char *lpC2 = CriticalPtr;
						//forward search
						UBINT i;
						for (i = CriticalPos; i < PatternSize; i++){
							if (*lpC1 != *lpC2)break;
							lpC1++;
							lpC2++;
						}
						if (i < PatternSize)lpMain_Next += i - CriticalPos + 1;
						else{
							//backward search
							lpC1 = lpMain_Next + CriticalPos - 1;
							lpC2 = CriticalPtr - 1;
							for (i = CriticalPos; i > 0; i--){
								if (*lpC1 != *lpC2)break;
								lpC1--;
								lpC2--;
							}
							if (i > 0)lpMain_Next += Period;
							else break;
						}
					}
					if (lpMain_Next == RightSentry + 1)return (UBINT)-1;
					else return lpMain_Next - (const unsigned char *)lpMain;
				}
				else{
					//two-way string searching + Boyer-Moore bad character rule

					UBINT BadCharTable[0x100];
					for (UBINT i = 0; i < 0x100; i++)BadCharTable[i] = PatternSize;
					for (UBINT i = 0; i < PatternSize - 1; i++){
						BadCharTable[((const unsigned char *)lpPattern)[i]] = PatternSize - 1 - i;
					}

					while (lpMain_Next <= RightSentry){
						const unsigned char *lpC1 = lpMain_Next + PatternSize - 1;
						const unsigned char *lpC2 = (const unsigned char *)lpPattern + PatternSize - 1;
						if (*lpC1 != *lpC2)lpMain_Next += BadCharTable[*lpC1]; //bad charater rule
						else{
							//two-way search
							const unsigned char *lpC1 = lpMain_Next + CriticalPos;
							const unsigned char *lpC2 = CriticalPtr;
							//forward search
							UBINT i;
							for (i = CriticalPos; i < PatternSize - 1; i++){
								if (*lpC1 != *lpC2)break;
								lpC1++;
								lpC2++;
							}
							if (i < PatternSize - 1)lpMain_Next += i - CriticalPos + 1;
							else{
								//backward search
								lpC1 = lpMain_Next + CriticalPos - 1;
								lpC2 = CriticalPtr - 1;
								for (i = CriticalPos; i > 0; i--){
									if (*lpC1 != *lpC2)break;
									lpC1--;
									lpC2--;
								}
								if (i > 0)lpMain_Next += Period;
								else break;
							}
						}
					}
					if (lpMain_Next == RightSentry + 1)return (UBINT)-1;
					else return lpMain_Next - (const unsigned char *)lpMain;
				}
			}
		}
	}

	extern void *Search_Mem_Chr_Bwd(const void *lpBuf, unsigned char Char, UBINT Size){
		lpBuf = (const void *)((const unsigned char *)lpBuf + Size - 1);
		while (Size && (*(unsigned char *)lpBuf != (unsigned char)Char)) {
			lpBuf = (unsigned char *)lpBuf - 1;
			Size--;
		}
		return(Size ? (void *)lpBuf : nullptr);
	}
	extern UBINT Search_Mem_Mem_Bwd(const void *lpMain, UBINT MainSize, const void *lpPattern, UBINT PatternSize){
		if (0 == PatternSize)return 0;
		else if(PatternSize > MainSize)return (UBINT)-1;
		else if (1 == PatternSize)return (UBINT)Search_Mem_Chr_Bwd(lpMain, *(const char *)lpPattern, MainSize) - (UBINT)lpMain;
		else{
			lpMain = (const void *)((const unsigned char *)lpMain + MainSize - 1);
			lpPattern = (const void *)((const unsigned char *)lpPattern + PatternSize - 1);

			if (PatternSize <= sizeof(UBINT)){
				UBINT Mask = (((UBINT)1 << (PatternSize * 8 - 1)) << 1) - 1;
				UBINT Main = 0, Pattern = 0;
				const unsigned char *lpMain_Next = ((const unsigned char *)lpMain);
				const unsigned char *lpPattern_Next = ((const unsigned char *)lpPattern);
				for (UBINT i = 0; i < PatternSize; i++){
					Main = (Main << 8) + (*lpMain_Next);
					lpMain_Next--;
				}
				for (UBINT i = 0; i < PatternSize; i++){
					Pattern = (Pattern << 8) + (*lpPattern_Next);
					lpPattern_Next--;
				}

				UBINT Result;
				for (Result = MainSize - PatternSize + 1; Result > 0; Result--){
					if (0 == ((Main ^ Pattern) & Mask))break;
					Main = (Main << 8) + (*lpMain_Next);
					lpMain_Next--;
				}
				if (Result < MainSize - PatternSize)return Result - 1; else return (UBINT)-1;
			}
			else{
				//preprocessing
				const unsigned char *CriticalPtr;
				UBINT PrefixPeriod;

				//compute the critical factorization
				UBINT LengthScanned;
				const unsigned char *CandidatePtr;

				//find the maximal prefix
				const unsigned char *CurMaxPrefixPtr = (const unsigned char *)lpPattern;
				UBINT PrefixPeriod0 = 1;
				LengthScanned = 0;
				CandidatePtr = CurMaxPrefixPtr - 1;
				while ((UBINT)((const unsigned char *)lpPattern - CandidatePtr) < PatternSize){
					unsigned char C1 = *CurMaxPrefixPtr;
					unsigned char C2 = *CandidatePtr;
					if (C1 > C2){ CandidatePtr--; LengthScanned = 0; PrefixPeriod0 = CurMaxPrefixPtr - CandidatePtr; }
					else if (C1 < C2){ CurMaxPrefixPtr = CandidatePtr + LengthScanned; CandidatePtr = CurMaxPrefixPtr - 1; LengthScanned = 0; PrefixPeriod0 = 1; }
					else if (LengthScanned == PrefixPeriod0){ CandidatePtr--; LengthScanned = 0; }
					else { CandidatePtr--; LengthScanned++; }
				}

				//find the inverse maximal prefix
				//inverse maximal prefix != minimal prefix
				const unsigned char *CurInvMaxPrefixPtr = (const unsigned char *)lpPattern;
				UBINT PrefixPeriod1 = 1;
				LengthScanned = 0;
				CandidatePtr = CurInvMaxPrefixPtr - 1;
				while ((UBINT)((const unsigned char *)lpPattern - CandidatePtr) < PatternSize){
					unsigned char C1 = *CurInvMaxPrefixPtr;
					unsigned char C2 = *CandidatePtr;
					if (C1 < C2){ CandidatePtr--; LengthScanned = 0; PrefixPeriod1 = CurInvMaxPrefixPtr - CandidatePtr; }
					else if (C1 > C2){ CurInvMaxPrefixPtr = CandidatePtr + LengthScanned; CandidatePtr = CurInvMaxPrefixPtr - 1; LengthScanned = 0; PrefixPeriod1 = 1; }
					else if (LengthScanned == PrefixPeriod1){ CandidatePtr--; LengthScanned = 0; }
					else { CandidatePtr--; LengthScanned++; }
				}

				if (CurMaxPrefixPtr >= CurInvMaxPrefixPtr){
					CriticalPtr = CurMaxPrefixPtr;
					PrefixPeriod = PrefixPeriod0;
				}
				else{
					CriticalPtr = CurInvMaxPrefixPtr;
					PrefixPeriod = PrefixPeriod1;
				}

				UBINT CriticalPos = (const unsigned char *)lpPattern - CriticalPtr;
				const unsigned char *lpMain_Next = ((const unsigned char *)lpMain);
				const unsigned char *LeftSentry = lpMain_Next - MainSize + PatternSize;

				//calculate the period
				UBINT Period;
				if (CriticalPos < PatternSize / 2 && 0 == memcmp(CriticalPtr - CriticalPos + 1, CriticalPtr + 1, CriticalPos))Period = PrefixPeriod;
				else{
					Period = PatternSize - CriticalPos;
					if (CriticalPos>Period)Period = CriticalPos;
					Period++;
				}

				if (PatternSize <= 32){
					//two-way string searching
					while (lpMain_Next >= LeftSentry){
						const unsigned char *lpC1 = lpMain_Next - CriticalPos;
						const unsigned char *lpC2 = CriticalPtr;
						//forward search
						UBINT i;
						for (i = CriticalPos; i < PatternSize; i++){
							if (*lpC1 != *lpC2)break;
							lpC1--;
							lpC2--;
						}
						if (i < PatternSize)lpMain_Next -= i - CriticalPos + 1;
						else{
							//backward search
							lpC1 = lpMain_Next - CriticalPos + 1;
							lpC2 = CriticalPtr + 1;
							for (i = CriticalPos; i > 0; i--){
								if (*lpC1 != *lpC2)break;
								lpC1++;
								lpC2++;
							}
							if (i > 0)lpMain_Next -= Period;
							else break;
						}
					}
					if (lpMain_Next == LeftSentry - 1)return (UBINT)-1;
					else return MainSize - PatternSize - ((const unsigned char *)lpMain - lpMain_Next);
				}
				else{
					//two-way string searching + Boyer-Moore bad character rule

					UBINT BadCharTable[0x100];
					for (UBINT i = 0; i < 0x100; i++)BadCharTable[i] = PatternSize;
					for (UBINT i = 0; i < PatternSize - 1; i++){
						BadCharTable[*((const unsigned char *)lpPattern - i)] = PatternSize - 1 - i;
					}

					while (lpMain_Next >= LeftSentry){
						const unsigned char *lpC1 = lpMain_Next - PatternSize + 1;
						const unsigned char *lpC2 = (const unsigned char *)lpPattern - PatternSize + 1;
						if (*lpC1 != *lpC2)lpMain_Next -= BadCharTable[*lpC1]; //bad charater rule
						else{
							//two-way search
							const unsigned char *lpC1 = lpMain_Next - CriticalPos;
							const unsigned char *lpC2 = CriticalPtr;
							//forward search
							UBINT i;
							for (i = CriticalPos; i < PatternSize - 1; i++){
								if (*lpC1 != *lpC2)break;
								lpC1--;
								lpC2--;
							}
							if (i < PatternSize - 1)lpMain_Next -= i - CriticalPos + 1;
							else{
								//backward search
								lpC1 = lpMain_Next - CriticalPos + 1;
								lpC2 = CriticalPtr + 1;
								for (i = CriticalPos; i > 0; i--){
									if (*lpC1 != *lpC2)break;
									lpC1++;
									lpC2++;
								}
								if (i > 0)lpMain_Next -= Period;
								else break;
							}
						}
					}
					if (lpMain_Next == LeftSentry - 1)return (UBINT)-1;
					else return MainSize - PatternSize - ((const unsigned char *)lpMain - lpMain_Next);
				}
			}
		}
	}
	template <class RandomAccessIterator, class T, class Compare> extern UBINT Binary_Search(RandomAccessIterator first, RandomAccessIterator last, const T& value, Compare comp){
		UBINT Lo_Ptr = (UBINT)-1, Hi_Ptr = (UBINT)(last - first) - 1;

		while (Lo_Ptr + 1 != Hi_Ptr){
			UBINT Mid_Ptr = (Lo_Ptr + Hi_Ptr) / 2;
			int CompResult = comp(*(first + Mid_Ptr), value);
			if (CompResult < 0)Lo_Ptr = Mid_Ptr;
			else if (CompResult > 0)Hi_Ptr = Mid_Ptr;
			else return Mid_Ptr;
		}
		if (0 == comp(*(first + Hi_Ptr), value)) return Hi_Ptr; else return (UBINT)-1;
	}

	template <class RandomAccessIterator, class Compare> extern void Sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp){
		if (first != last){
			UBINT ElementCnt = last - first;
			RandomAccessIterator TmpStack[MaxQSortDepth], *TmpStackPtr = TmpStack, LBound = first, RBound = --last;

			// calculate the maximum depth for heap sorting
			UBINT HSortDepth = nsMath::log2intsim((UBINT)(last - first));
			HSortDepth += HSortDepth / 2;
			if (HSortDepth > MaxQSortDepth)HSortDepth = MaxQSortDepth;

			while (true){
				if (LBound >= RBound){
					if (TmpStackPtr == TmpStack)return; //finished
					TmpStackPtr--;
					LBound = RBound + 2;
					RBound = *TmpStackPtr;
				}
				else if (RBound - LBound <= ISortCutoff){
					//Insert sort
					while (RBound > LBound){
						RandomAccessIterator i = LBound;
						RandomAccessIterator j = LBound + 1;
						do{
							if (comp(*j, *i) < 0)i = j;
							++j;
						} while (RBound >= j);
						std::swap(*i, *LBound);
						++LBound;
					}
					if (TmpStackPtr == TmpStack)return; //finished
					TmpStackPtr--;
					LBound = RBound + 2;
					RBound = *TmpStackPtr;
				}
				else if (TmpStackPtr >= &TmpStack[HSortDepth]){
					//Heap sort
					UBINT HeapSize, TmpIndex;

					HeapSize = ((RBound - LBound) + 1);

					//Heapify with a bottom-up algorithm whose complexity is O(n).
					for (UBINT Index = HeapSize / 2; Index > 0; Index--){
						RandomAccessIterator k = LBound + (Index - 1);

						TmpIndex = Index * 2;
						do{
							RandomAccessIterator Child = LBound + (TmpIndex - 1);
							//find the larger child in LChild and RChild
							if (TmpIndex < HeapSize && comp(*Child, *(Child + 1)) < 0){
								++Child;
								TmpIndex++;
							}
							//swap the parent and the child if required
							if (comp(*k, *Child) < 0){
								std::swap(*k, *Child);
								k = Child;
								TmpIndex *= 2;
							}
							else break;
						} while (TmpIndex <= HeapSize);
					}

					//Heap sorting
					for (; HeapSize > 1;){
						RandomAccessIterator k = LBound;
						RandomAccessIterator Child = LBound + (HeapSize - 1);
						std::swap(*k, *Child);

						HeapSize--;
						TmpIndex = 2;
						do{
							Child = LBound + (TmpIndex - 1);
							if (TmpIndex < HeapSize && comp(*Child, *(Child + 1)) < 0){
								++Child;
								TmpIndex++;
							}
							if (comp(*k, *Child) < 0){
								std::swap(*k, *Child);
								k = Child;
								TmpIndex *= 2;
							}
							else break;
						} while (TmpIndex <= HeapSize);
					}

					if (TmpStackPtr == TmpStack)return;
					TmpStackPtr--;
					LBound = RBound + 2;
					RBound = *TmpStackPtr;
				}
				else{ //Quick sort
					*TmpStackPtr = RBound;
					TmpStackPtr++;

					RandomAccessIterator TmpLBound = LBound;
					while (RBound > TmpLBound){
						while (comp(*TmpLBound, *RBound) < 0 && TmpLBound < RBound)--RBound;
						if (TmpLBound < RBound){ std::swap(*TmpLBound, *RBound); ++TmpLBound; }
						while (comp(*TmpLBound, *RBound) < 0 && TmpLBound < RBound)++TmpLBound;
						if (TmpLBound < RBound){ std::swap(*TmpLBound, *RBound); --RBound; }
					}
					if (TmpLBound == LBound){
						TmpStackPtr--;
						++LBound;
						RBound = *TmpStackPtr;
					}
					else RBound = --TmpLBound;
				}
			}
		}
	}

	extern void Sort_C(void *lpArray, UBINT ElementCnt, UBINT ElementSize, BINT(*CmpFunc)(void *, void *)){
		auto memswap = [](void *_Src1, void *_Src2, size_t _Size){
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check the following macros.
			//This function is highly preformance-critical and platform-related,Write in assembly language if possible.
#endif

			register UBINT TmpInt;
			switch (_Size%sizeof(UBINT)){
#if defined LIBENV_SYS_INTELX64
			case 7:
				TmpInt = *(unsigned char *)_Src1; *(unsigned char *)_Src1 = *(unsigned char *)_Src2; *(unsigned char *)_Src2 = (unsigned char)TmpInt;
				_Src1 = ((unsigned char *)_Src1) + 1; _Src2 = ((unsigned char *)_Src2) + 1;
			case 6:
				TmpInt = *(unsigned char *)_Src1; *(unsigned char *)_Src1 = *(unsigned char *)_Src2; *(unsigned char *)_Src2 = (unsigned char)TmpInt;
				_Src1 = ((unsigned char *)_Src1) + 1; _Src2 = ((unsigned char *)_Src2) + 1;
			case 5:
				TmpInt = *(unsigned char *)_Src1; *(unsigned char *)_Src1 = *(unsigned char *)_Src2; *(unsigned char *)_Src2 = (unsigned char)TmpInt;
				_Src1 = ((unsigned char *)_Src1) + 1; _Src2 = ((unsigned char *)_Src2) + 1;
			case 4:
				TmpInt = *(unsigned char *)_Src1; *(unsigned char *)_Src1 = *(unsigned char *)_Src2; *(unsigned char *)_Src2 = (unsigned char)TmpInt;
				_Src1 = ((unsigned char *)_Src1) + 1; _Src2 = ((unsigned char *)_Src2) + 1;
#endif
			case 3:
				TmpInt = *(unsigned char *)_Src1; *(unsigned char *)_Src1 = *(unsigned char *)_Src2; *(unsigned char *)_Src2 = (unsigned char)TmpInt;
				_Src1 = ((unsigned char *)_Src1) + 1; _Src2 = ((unsigned char *)_Src2) + 1;
			case 2:
				TmpInt = *(unsigned char *)_Src1; *(unsigned char *)_Src1 = *(unsigned char *)_Src2; *(unsigned char *)_Src2 = (unsigned char)TmpInt;
				_Src1 = ((unsigned char *)_Src1) + 1; _Src2 = ((unsigned char *)_Src2) + 1;
			case 1:
				TmpInt = *(unsigned char *)_Src1; *(unsigned char *)_Src1 = *(unsigned char *)_Src2; *(unsigned char *)_Src2 = (unsigned char)TmpInt;
				_Src1 = ((unsigned char *)_Src1) + 1; _Src2 = ((unsigned char *)_Src2) + 1;
			}
			for (UBINT i = 0; i<_Size / sizeof(UBINT); i++){
				TmpInt = *(UBINT *)_Src1; *(UBINT *)_Src1 = *(UBINT *)_Src2; *(UBINT *)_Src2 = (UBINT)TmpInt;
				_Src1 = ((UBINT *)_Src1) + 1; _Src2 = ((UBINT *)_Src2) + 1;
			}
		};

		if (ElementCnt > 0){
			void *TmpStack[MaxQSortDepth], **TmpStackPtr = TmpStack, *i, *j = ((unsigned char *)lpArray + (ElementCnt - 1)*ElementSize);
			UBINT HSortDepth = nsMath::log2intsim(ElementCnt);

			HSortDepth += HSortDepth / 2;
			if (HSortDepth > MaxQSortDepth)HSortDepth = MaxQSortDepth;
			while (1){
				if (j <= lpArray){
					if (TmpStackPtr == TmpStack)return;
					TmpStackPtr--;
					lpArray = ((unsigned char *)j + 2 * ElementSize);
					j = *TmpStackPtr;
				}
				else if ((UBINT)j - (UBINT)lpArray <= ISortCutoff*ElementSize){ //Insert sort
					void *k;

					while (lpArray < j){
						i = lpArray;
						k = ((unsigned char *)lpArray + ElementSize);
						do{
							if (CmpFunc(k, i) < 0)i = k;
							k = ((unsigned char *)k + ElementSize);
						} while (k <= j);
						memswap(lpArray, i, ElementSize);
						lpArray = ((unsigned char *)lpArray + ElementSize);
					}
					if (TmpStackPtr == TmpStack)return;
					TmpStackPtr--;
					lpArray = ((unsigned char *)j + 2 * ElementSize);
					j = *TmpStackPtr;
				}
				else if (TmpStackPtr >= &TmpStack[HSortDepth]){ //Heap sort
					void *k, *Child;
					UBINT HeapSize, Index, TmpIndex;

					i = ((unsigned char *)lpArray - ElementSize);
					HeapSize = ((UBINT)j - (UBINT)i) / ElementSize;

					//Heapify with a bottom-up algorithm whose complexity is O(n).
					for (Index = HeapSize / 2; Index > 0; Index--){
						k = ((unsigned char *)i + Index*ElementSize);

						TmpIndex = Index * 2;
						do{
							Child = ((unsigned char *)i + TmpIndex*ElementSize);
							if (TmpIndex < HeapSize && CmpFunc(Child, (unsigned char *)Child + ElementSize) < 0){
								Child = (unsigned char *)Child + ElementSize;
								TmpIndex++;
							}
							if (CmpFunc(k, Child)<0){
								memswap(k, Child, ElementSize);
								k = Child;
								TmpIndex *= 2;
							}
							else break;
						} while (TmpIndex <= HeapSize);
					}

					//Heap sorting
					for (; HeapSize>1;){
						k = lpArray;
						Child = ((unsigned char *)i + HeapSize*ElementSize);
						memswap(k, Child, ElementSize);

						HeapSize--;
						TmpIndex = 2;
						do{
							Child = ((unsigned char *)i + TmpIndex*ElementSize);
							if (TmpIndex < HeapSize && CmpFunc(Child, (unsigned char *)Child + ElementSize) < 0){
								Child = (unsigned char *)Child + ElementSize;
								TmpIndex++;
							}
							if (CmpFunc(k, Child) < 0){
								memswap(k, Child, ElementSize);
								k = Child;
								TmpIndex *= 2;
							}
							else break;
						} while (TmpIndex <= HeapSize);
					}

					if (TmpStackPtr == TmpStack)return;
					TmpStackPtr--;
					lpArray = ((unsigned char *)j + 2 * ElementSize);
					j = *TmpStackPtr;
				}
				else{ //Quick sort
					*TmpStackPtr = j;
					TmpStackPtr++;

					i = lpArray;
					while (i < j){
						while (CmpFunc(i, j) < 0 && i < j)j = ((unsigned char *)j - ElementSize);
						if (i < j){ memswap(i, j, ElementSize); i = ((unsigned char *)i + ElementSize); }
						while (CmpFunc(i, j) < 0 && i < j)i = ((unsigned char *)i + ElementSize);
						if (i < j){ memswap(i, j, ElementSize); j = ((unsigned char *)j - ElementSize); }
					}
					if (i == lpArray){
						TmpStackPtr--;
						lpArray = ((unsigned char *)lpArray + ElementSize);
						j = *TmpStackPtr;
					}
					else j = ((unsigned char *)i - ElementSize);
				}
			}
		}
	}
}
#endif