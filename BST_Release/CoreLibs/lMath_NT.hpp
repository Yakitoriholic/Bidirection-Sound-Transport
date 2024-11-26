/* Description:Platform-independent mathematical functions relative to number theory.
* Language:C++
* Author:***
*/

#ifndef LIB_MATH_NT
#define LIB_MATH_NT

#include "lMath.hpp"

#if defined LIBENV_SYS_INTELX64 && defined LIBENV_CPLR_VS
#pragma comment(lib, "lMath_NT.lib")
extern "C" UBINT _fastcall _mul64_mod(const UBINT scl1, const UBINT scl2, const UBINT modulus);
#endif

namespace nsMath{
	extern UBINT powint(const UBINT scl1, const UBINT scl2);
	//returns pow(scl1, scl2) % (UBINT_MAX + 1).
	extern UBINT sqrtint(const UBINT scl);
	//returns floor(sqrt([scl])).
	extern bool issqr(const UBINT scl);
	//return true when [scl] is a square number.
	template <typename T> extern T gcd(const T scl1, const T scl2);
	//returns the greatest common divisor of [scl1] and [scl2]. 0 is returned if an error occured.
	template <typename T> extern inline T lcm(const T scl1, const T scl2){ T TmpVal = gcd(scl1, scl2); if ((T)0 == TmpVal) return (T)0; else return scl1 * scl2 / TmpVal; }
	//returns the least common multiplier of [scl1] and [scl2]. 0 is returned if an error occured.
	extern UINT4b addmod(const UINT4b scl1, const UINT4b scl2, const UINT4b modulus);
	extern UINT8b addmod(const UINT8b scl1, const UINT8b scl2, const UINT8b modulus);
	//returns the modular addition result of ([scl1] + [scl2]) % [modulus].
	extern UINT4b submod(const UINT4b scl1, const UINT4b scl2, const UINT4b modulus);
	extern UINT8b submod(const UINT8b scl1, const UINT8b scl2, const UINT8b modulus);
	//returns the modular subtraction result of ([scl1] - [scl2]) % [modulus].
	extern UINT4b mulmod(const UINT4b scl1, const UINT4b scl2, const UINT4b modulus);
	extern UINT8b mulmod(const UINT8b scl1, const UINT8b scl2, const UINT8b modulus);
	//returns the modular multiplication result of ([scl1] * [scl2]) % [modulus]. a divide-by-zero error will occur when [modulus] is 0.
	extern UINT4b invmod(const UINT4b scl, const UINT4b modulus);
	extern UINT8b invmod(const UINT8b scl, const UINT8b modulus);
	//returns the modular inverse of [scl] mod [modulus]. 0 is returned if an error occured.
	extern UBINT powmod(const UBINT scl1, const UBINT scl2, const UBINT modulus);
	//returns the modular exponentiation result of pow([scl1], [scl2]) % [modulus]. a divide-by-zero error will occur when [modulus] is 0.
	//[scl1] is expected to be smaller than [modulus], [scl2] is expected to be smaller than [modulus] - 1.
	extern BINT jacobi_sym(const UBINT scl1, const UBINT scl2);
	//returns the Jacobi symbol ([scl1] | [scl2]).
	//[scl2] is expected to be a positive odd integer which is greater than 1.
	//(0 | 1) = 0, (x > 0 | 1) = 1.
	extern bool isprime(UBINT scl);
	//return true when [scl] is a prime number.
	extern UBINT getprime_leq(UBINT scl);
	//return the greatest prime that is less or equal than scl. 0 is returned if such a prime number doesn't exist.
	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	extern UBINT powint(const UBINT scl1, const UBINT scl2){
		UBINT RetValue = 1;
		UBINT scl1_power = scl1;
		UBINT BitMask = scl2;
		while (BitMask > 0){
			if (BitMask & 1)RetValue *= scl1_power;
			scl1_power *= scl1_power;
			BitMask >>= 1;
		}
		return RetValue;
	}
	extern UBINT sqrtint(const UBINT scl){
		if (scl < 2)return scl;
#if defined LIBENV_SYS_INTELX86
		if (scl < 0xFFFFFF){
			BFLOAT scl_float = (BFLOAT)scl;
			return (UBINT)sqrt(scl_float); //+0.5 doesn't seem to be necessary.
		}
		UBINT Val = 0xFFFF;
#elif defined LIBENV_SYS_INTELX64
		if (scl < 0x1FFFFFFFFFFFFF){
			BFLOAT scl_float = (BFLOAT)scl;
			return (UBINT)sqrt(scl_float); //+0.5 doesn't seem to be necessary.
		}
		UBINT Val = 0xFFFFFFFF;
#endif

		//now Val is no less than any possible result.
		//Newton method
		while (Val * Val > scl){
			Val += scl / Val;
			Val >>= 1;
		}
		return Val;
	}
	extern bool issqr(const UBINT scl){
		UBINT Criteria = scl & 0xF;
		if (0 != Criteria && 1 != Criteria && 4 != Criteria && 9 != Criteria)return false;
		UBINT SqrRoot = sqrtint(scl);
		if (SqrRoot*SqrRoot == scl)return true; else return false;
	}
	template <typename T> extern T gcd(const T scl1, const T scl2){
		if (0 == scl1 || 0 == scl2)return 0; //error
		else{
			//Euclidean algorithm
			T VA, VB, VRem;
			if (scl1 > scl2){ VA = scl1; VB = scl2; }
			else{ VA = scl2; VB = scl1; }

			while (true){
				VRem = VA%VB;
				if (0 == VRem)return VB;
				else{ VA = VB; VB = VRem; }
			}
		}
	}
	extern UINT4b invmod(const UINT4b scl, const UINT4b modulus){
		if (modulus < 2 || 0 == scl || scl >= modulus)return 0; //error
		else if (1 == scl)return 1;
		else{
			// Euclidean algorithm
			INT4b MA = 1, MB = 0;
			INT4b NA = 0, NB = 1;
			INT4b KA, KB;

			INT4b QFirst = modulus / scl;
			UINT4b MVal = scl;
			UINT4b NVal = modulus - QFirst*MVal;
			UINT4b KVal;

			if (0 == NVal)return 0; //error, inverse not found
			else if (1 == NVal)return modulus - modulus / scl;
			else{
				while (true){
					UINT4b Q = MVal / NVal;
					KVal = MVal - Q*NVal;
					if (0 == KVal)return 0; //error, inverse not found
					KA = MA - Q*NA;
					KB = MB - Q*NB;
					if (1 == KVal){
						INT8b RetVal = (INT8b)KA - (INT8b)QFirst*(INT8b)KB;
						if(RetVal < 0)RetVal += modulus;
						return (UINT4b)RetVal;
					}
					else{
						MA = NA;
						MB = NB;
						MVal = NVal;

						NA = KA;
						NB = KB;
						NVal = KVal;
					}
				}
			}
		}
	}
	extern UINT8b invmod(const UINT8b scl, const UINT8b modulus){
		if (modulus < 2 || 0 == scl || scl >= modulus)return 0; //error
		else if (1 == scl)return 1;
		else{
			// Euclidean algorithm
			INT8b MA = 1, MB = 0;
			INT8b NA = 0, NB = 1;
			INT8b KA, KB;

			INT8b QFirst = modulus / scl;
			UINT8b MVal = scl;
			UINT8b NVal = modulus - QFirst*MVal;
			UINT8b KVal;

			if (0 == NVal)return 0; //error, inverse not found
			else if (1 == NVal)return modulus - modulus / scl;
			else{
				while (true){
					UINT8b Q = MVal / NVal;
					KVal = MVal - Q*NVal;
					if (0 == KVal)return 0; //error, inverse not found
					KA = MA - Q*NA;
					KB = MB - Q*NB;
					if (1 == KVal){
#if defined LIBENV_SYS_INTELX86
						UINT8b KA_U, QFirst_U, KB_U;
						if (KA >= 0)KA_U = (UINT8b)KA; else KA_U = modulus - ((UINT8b)-KA) % modulus;
						if (QFirst >= 0)QFirst_U = (UINT8b)QFirst; else QFirst_U = modulus - ((UINT8b)-QFirst) % modulus;
						if (KB >= 0)KB_U = (UINT8b)KB; else KB_U = modulus - ((UINT8b)-KB) % modulus;
						return submod(KA_U, mulmod(QFirst_U, KB_U, modulus), modulus);
#elif defined LIBENV_SYS_INTELX64
#if defined LIBENV_CPLR_GCC
						__int128 RetVal = (__int128)KA - (__int128)QFirst*(__int128)KB;
						if (RetVal < 0)RetVal += modulus;
						return (UINT8b)RetVal;
#elif defined LIBENV_CPLR_VS
						INT8b RetVal[2];
						RetVal[1] = _mul128(QFirst, -KB, RetVal); //_mul128 use instruction imul. test invmod(13, 0xFFFFFFFFFFFFFFFF);
						RetVal[1] += KA;
						if ((UINT8b)RetVal[1] < (UINT8b)KA)RetVal[0]++;
						if (RetVal[0] < 0)return modulus + (UINT8b)RetVal[1]; else return (UINT8b)RetVal[1];
#endif
#endif
					}
					else{
						MA = NA;
						MB = NB;
						MVal = NVal;

						NA = KA;
						NB = KB;
						NVal = KVal;
					}
				}
			}
		}
	}
	extern inline UINT4b addmod(const UINT4b scl1, const UINT4b scl2, const UINT4b modulus){
		UINT4b RetValue = scl1 - (modulus - scl2);
		if (RetValue > scl1)RetValue += modulus;
		return RetValue;
	}
	extern inline UINT8b addmod(const UINT8b scl1, const UINT8b scl2, const UINT8b modulus){
		UINT8b RetValue = scl1 - (modulus - scl2);
		if (RetValue > scl1)RetValue += modulus;
		return RetValue;
	}
	extern inline UINT4b submod(const UINT4b scl1, const UINT4b scl2, const UINT4b modulus){
		UINT4b RetValue = scl1 - scl2;
		if (RetValue > scl1)RetValue += modulus;
		return RetValue;
	}
	extern inline UINT8b submod(const UINT8b scl1, const UINT8b scl2, const UINT8b modulus){
		UINT8b RetValue = scl1 - scl2;
		if (RetValue > scl1)RetValue += modulus;
		return RetValue;
	}
	extern UINT4b mulmod(const UINT4b scl1, const UINT4b scl2, const UINT4b modulus){
#if defined LIBENV_SYS_INTELX86
		UINT4b RetValue;
#if defined LIBENV_CPLR_GCC
		asm(
			"movl %1, %%eax\n\t"
			"mull %2\n\t"
			"divl %3\n\t"
			: "=d" (RetValue)
			: "m" (scl1), "m" (scl2), "m" (modulus)
			);

#elif defined LIBENV_CPLR_VS
		_asm{
			mov eax, scl1
				mul scl2
				div modulus
				mov RetValue, edx
		}
#endif
		return RetValue;
#elif defined LIBENV_SYS_INTELX64
		return (UINT4b)(((UINT8b)scl1 * (UINT8b)scl2) % modulus);
#endif
	}
	extern UINT8b mulmod(const UINT8b scl1, const UINT8b scl2, const UINT8b modulus){
#if defined LIBENV_SYS_INTELX86
		UINT8b Result_High = (scl1 >> 32) * (scl2 >> 32);
		UINT8b Result_Mid = (scl1 & 0xFFFFFFFF) * (scl2 >> 32) + (scl1 >> 32) * (scl2 & 0xFFFFFFFF);
		UINT8b Result_Low = (scl1 & 0xFFFFFFFF) * (scl2 & 0xFFFFFFFF);
		Result_High += Result_Mid >> 32;
		Result_Low += Result_Mid << 32;
		Result_High %= modulus;
		Result_Low %= modulus;

		UBINT BitRemain = log2intsim(modulus);
		UINT8b tmp_modulus = modulus << (64 - BitRemain);
		Result_High <<= (64 - BitRemain);
		for (UBINT i = 0; i < 64; i++){
			if (Result_High > tmp_modulus){
				Result_High -= tmp_modulus;
				Result_High <<= 1;
			}
			else if ((Result_High & NumericTrait<UINT8b>::HighestBit) > 0){
				Result_High <<= 1;
				Result_High -= tmp_modulus;
			}
			else Result_High <<= 1;
		}
		Result_High >>= (64 - BitRemain);

		UINT8b RetValue = Result_Low - (modulus - Result_High);
		if(RetValue > Result_Low)RetValue += modulus;
		return RetValue;
#elif defined LIBENV_SYS_INTELX64
#if defined LIBENV_CPLR_GCC
		UINT8b RetValue;
		asm(
			"movq %1, %%rax\n\t"
			"mulq %2\n\t"
			"divq %3\n\t"
			: "=d" (RetValue)
			: "m" (scl1), "m" (scl2), "m" (modulus)
		);
		return RetValue;
#elif defined LIBENV_CPLR_VS
		return _mul64_mod(scl1, scl2, modulus);
#endif
#endif
	}
	extern UBINT powmod(const UBINT scl1, const UBINT scl2, const UBINT modulus){
		UBINT RetValue = 1;
		UBINT scl1_power = scl1;
		UBINT BitMask = scl2;
		while (BitMask > 0){
			if (BitMask & 1)RetValue = mulmod(RetValue, scl1_power, modulus);
			scl1_power = mulmod(scl1_power, scl1_power, modulus);
			BitMask >>= 1;
		}
		return RetValue;
	}
	extern BINT jacobi_sym(const UBINT scl1, const UBINT scl2){
		UBINT a = scl1, b = scl2;
		BINT TmpVal = 1;
		while (true){
			a = a%b;

			if (1 != gcd(a, b))return 0;
			else{
				UBINT LeastBits = log2intsim(a ^ (a - 1)) - 1;
				a >>= LeastBits;

				TmpVal = (TmpVal ^ (1 - (((LeastBits & 1) << 1)&((b >> 1) + 1)))) + 1;
				//TmpVal *= (powint(2, LeastBits) | b);

				//1.k = (2 | b);
				// (2 | b) = 1 if b mod 8 = 1/7
				// (2 | b) = -1 if b mod 8 = 3/5

				//2.k = powint(k, LeastBits);
				//suppose that |m| = 1,
				//powint(-1, n) = 1 - ((n & 1) << 1);
				//powint(m, n) = (1 - ((n & 1) << 1)) & a;

				//3.TmpVal *= k;
				// -1 ^ -1 = 0, 1 ^ 1 = 0;
				// -1 ^ 1 = -2;

				if (1 == a)return TmpVal;
				else{
					//swap
					UBINT c = a;
					a = b; b = c;
					TmpVal = (TmpVal ^ (1 - (a & b & 2))) + 1;
					//TmpVal *= powint(-1, (a - 1) * (b - 1) >> 4);
				}
			}
		}
	}
	extern bool isprime(UBINT scl){
		if (5 == scl || 2 == (scl & (UBINT)-2))return true; //for 2, 3 and 5
		else if (scl < 2 || 0 == (scl & 1))return false;
		else{
			//Baillie - PSW Primality Test

			//UBINT TestBase[7] = { 2, 325, 9375, 28178, 450775, 9780504, 1795265022 }; //from http://miller-rabin.appspot.com/
			//not used

			//Miller¨CRabin test on 2 --BEGIN--
			UBINT d = scl - 1; // d > 0
			UBINT LeastBits = log2intsim(d ^ (d - 1)) - 1;
			d >>= LeastBits;

			UBINT a = 1;
			UBINT apower = 2;
			UBINT bitmask = d;
			UBINT Halfscl = (scl >> 1) + 1;
			while (bitmask > 0){
				if (bitmask & 1)a = mulmod(a, apower, scl);
				if (apower >= Halfscl)apower = (apower << 1) - scl; else apower <<= 1;
				bitmask >>= 1;
			}
			if (a != 1 && a != scl - 1){
				for (UBINT i = 1; i < LeastBits; i++){
					a = mulmod(a, a, scl);
					if (1 == a)return false;
					else if (scl - 1 == a)break;
				}
			}
			//Miller¨CRabin test on 2 --END--

			//remove all the square numbers
			if (issqr(scl))return false;

			//find a d which satisfy (d | scl) = -1;
			UBINT d1 = 5, d2 = scl - 7;
			BINT q = 1;
			while (true){
				UBINT TmpSym = jacobi_sym(d1, scl);
				if (0 == TmpSym && d1 > 0 && d1 < scl)return false;
				else if (-1 == TmpSym){
					d = d1;
					q = scl - q;
					break;
				}
				d1 += 4;
				if (d1>scl)d1 -= scl;

				TmpSym = jacobi_sym(d2, scl);
				if (0 == TmpSym && d2 > 0 && d2 < scl)return false;
				else if (-1 == TmpSym){
					d = d2;
					q = -(~q);
					break;
				}
				if (d2 < 4)d2 += scl;
				d2 -= 4;

				q++;
			}
			//2^32-1 and 2^64-1 won't pass the test above.

			//strong Lucas test --BEGIN--

			UBINT u = 0, v = 2;
			UBINT delta = scl + 1;
			UBINT k = 0;

			LeastBits = log2intsim(delta ^ (delta - 1)) - 1;
			delta >>= LeastBits;

#if defined LIBENV_SYS_INTELX86
			delta = delta << (32 - log2intsim(delta));
#elif defined LIBENV_SYS_INTELX64
			delta = delta << (64 - log2intsim(delta));
#endif
			while (true){
				if ((BINT)delta < 0){
					//u_{k+1} = ((u_k + v_k) / 2) % scl;
					//v_{k+1} = ((d * u_k + v_k) / 2) % scl;
					UBINT u_tmp = u + v;
					if (u_tmp >= scl || u_tmp < u)u_tmp -= scl; //overflow
					if (u_tmp & 1){
						u_tmp >>= 1;
						u_tmp += Halfscl;
						if (u_tmp >= scl || u_tmp < Halfscl)u_tmp -= scl; //overflow
					}
					else u_tmp >>= 1;

					UBINT v_tmp = mulmod(d, u, scl);
					v_tmp = v_tmp + v;
					if (v_tmp >= scl || v_tmp < v)v_tmp -= scl; //overflow
					if (v_tmp & 1){
						v_tmp >>= 1;
						v_tmp += Halfscl;
						if (v_tmp >= scl || v_tmp < Halfscl)v_tmp -= scl; //overflow
					}
					else v_tmp >>= 1;

					u = u_tmp;
					v = v_tmp;

					k += 1;
				}

				delta <<= 1;
				if (0 == delta)break;

				//u_{2k} = u_k * v_k % scl;
				//v_{2k} = (v_k * v_k - 2 * powint(q, k)) % scl;
				u = mulmod(u, v, scl);
				v = mulmod(v, v, scl);
				UBINT TmpVal = powmod(q, k, scl);
				if (TmpVal > Halfscl)TmpVal = (TmpVal << 1) - scl; else TmpVal = TmpVal << 1;
				if (v >= TmpVal)v -= TmpVal; else v = v + scl - TmpVal;
				k <<= 1;
			}
			if (0 == u || 0 == v)return true;
			//11 will pass this test above.

			for (UBINT i = 1; i < LeastBits; i++){
				v = mulmod(v, v, scl);
				UBINT TmpVal = powmod(q, k, scl);
				if (TmpVal > Halfscl)TmpVal = (TmpVal << 1) - scl; else TmpVal = TmpVal << 1;
				if (v >= TmpVal)v -= TmpVal; else v = v + scl - TmpVal;
				if (0 == v)return true;
				k <<= 1;
			}

			//strong Lucas test --END--
			
			return false;
		}
	}
	extern UBINT getprime_leq(UBINT scl){
		if (scl < 2)return 0; //error
		else if (scl < 31){
			UBINT PTable[10] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };
			UBINT Ptr = 0;
			if (scl >= PTable[5])Ptr = 5;
			if (scl >= PTable[Ptr + 2])Ptr += 2;
			if (scl >= PTable[Ptr + 1])Ptr += 1;
			if (scl >= PTable[Ptr + 1])Ptr += 1;
			return PTable[Ptr];
		}
		else{
			UBINT PTable[8] = { 1, 7, 11, 13, 17, 19, 23, 29 };
			UBINT Ptr = 0;
			UBINT Candidate = scl % 30;
			if (Candidate >= PTable[4])Ptr = 4;
			if (Candidate >= PTable[Ptr + 2])Ptr += 2;
			if (Candidate >= PTable[Ptr + 1])Ptr += 1;
			Candidate = scl - Candidate;
			while (true){
				if (isprime(Candidate + PTable[Ptr]))return Candidate + PTable[Ptr];
				if (0 == Ptr){ Candidate -= 30; Ptr = 7; }
				else Ptr--;
			}
			return 0;
		}
	}
}
#endif