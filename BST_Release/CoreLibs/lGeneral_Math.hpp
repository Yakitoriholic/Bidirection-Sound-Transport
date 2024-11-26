/* Description: Core mathematic functions for library kernel. DO NOT include this header directly.
 * Language:C++
 * Author:***
 */

#define _USE_MATH_DEFINES

#include <cmath>
#include <cfloat>
#include <complex>
#include "type_traits"

namespace nsMath{
	template < typename T, UBINT I > struct TinyVector{
		T Data[I];

		inline T& operator[](UBINT n){ return this->Data[n]; }
		inline const T& operator[](UBINT n) const{ return this->Data[n]; }
	};
	template < typename T, UBINT I > bool operator==(const TinyVector<T, I>& lhs, const TinyVector<T, I>& rhs);
	template < typename T, UBINT I > bool operator!=(const TinyVector<T, I>& lhs, const TinyVector<T, I>& rhs);

	template < typename T > struct NumericTrait;

	static const UBINT TypeID_UINT = 0x0;
	static const UBINT TypeID_INT = 0x10;
	static const UBINT TypeID_FLOAT = 0x20;
	static const UBINT TypeID_INVALID = 0x30;
	static const UBINT TypeID_COMPLEX = 0x40;

	static const UBINT TypeID_UINT_UBound = 3;
	static const UBINT TypeID_INT_UBound = 3;
	static const UBINT TypeID_FLOAT_UBound = 3;
	static const UBINT TypeID_FLOAT_LBound = 2;

	template <> struct NumericTrait<UINT1b>{
		static const UBINT Category = TypeID_UINT;
		static const UBINT TypeID = TypeID_UINT + 0x0;
		static const UINT1b HighestBit = 0x80;
		static const UINT1b Min = 0;
		static const UINT1b Max = 0xFF;
		static const UINT4b GroupOrder_Float_ByteForm;
		static const float GroupOrder_Float;
	};

	template <> struct NumericTrait<UINT2b>{
		static const UBINT Category = TypeID_UINT;
		static const UBINT TypeID = TypeID_UINT + 0x1;
		static const UINT2b HighestBit = 0x8000;
		static const UINT2b Min = 0;
		static const UINT2b Max = 0xFFFF;
		static const UINT4b GroupOrder_Float_ByteForm;
		static const float GroupOrder_Float;
	};

	template <> struct NumericTrait<UINT4b>{
		static const UBINT Category = TypeID_UINT;
		static const UBINT TypeID = TypeID_UINT + 0x2;
		static const UINT4b HighestBit = 0x80000000;
		static const UINT4b Min = 0;
		static const UINT4b Max = 0xFFFFFFFF;
		static const UINT4b GroupOrder_Float_ByteForm;
		static const float GroupOrder_Float;
	};

	template <> struct NumericTrait<UINT8b>{
		static const UBINT Category = TypeID_UINT;
		static const UBINT TypeID = TypeID_UINT + 0x3;
		static const UINT8b HighestBit = 0x8000000000000000;
		static const UINT8b Min = 0;
		static const UINT8b Max = 0xFFFFFFFFFFFFFFFF;
		static const UINT4b GroupOrder_Float_ByteForm;
		static const float GroupOrder_Float;
	};

	template <> struct NumericTrait<INT1b>{
		static const UBINT Category = TypeID_INT;
		static const UBINT TypeID = TypeID_INT + 0x0;

		typedef UINT1b uint_type;
		static const INT1b HighestBit = -0x7Fi8 - 1;
		static const INT1b Min = -0x7Fi8 - 1;
		static const INT1b Max = 0x7Fi8;
		static const UINT4b GroupOrder_Float_ByteForm;
		static const float GroupOrder_Float;
	};

	template <> struct NumericTrait<INT2b>{
		static const UBINT Category = TypeID_INT;
		static const UBINT TypeID = TypeID_INT + 0x1;

		typedef UINT2b uint_type;
		static const INT2b HighestBit = -0x7FFFi16 - 1;
		static const INT2b Min = -0x7FFFi16 - 1;
		static const INT2b Max = 0x7FFFi16;
		static const UINT4b GroupOrder_Float_ByteForm;
		static const float GroupOrder_Float;
	};

	template <> struct NumericTrait<INT4b>{
		static const UBINT Category = TypeID_INT;
		static const UBINT TypeID = TypeID_INT + 0x2;

		typedef UINT4b uint_type;
		static const INT4b HighestBit = -0x7FFFFFFFi32 - 1;
		static const INT4b Min = -0x7FFFFFFFi32 - 1;
		static const INT4b Max = 0x7FFFFFFFi32;
		static const UINT4b GroupOrder_Float_ByteForm;
		static const float GroupOrder_Float;
	};

	template <> struct NumericTrait<INT8b>{
		static const UBINT Category = TypeID_INT;
		static const UBINT TypeID = TypeID_INT + 0x3;

		typedef UINT8b uint_type;
		static const INT8b HighestBit = -0x7FFFFFFFFFFFFFFFi64 - 1;
		static const INT8b Min = -0x7FFFFFFFFFFFFFFFi64 - 1;
		static const INT8b Max = 0x7FFFFFFFFFFFFFFFi64;
		static const UINT4b GroupOrder_Float_ByteForm;
		static const float GroupOrder_Float;
	};

	template <> struct NumericTrait<float>{
		static const UBINT Category = TypeID_FLOAT;
		static const UBINT TypeID = TypeID_FLOAT + 0x2;

		typedef UINT4b uint_type;
		static const UINT4b HighestBit = 0x80000000;
		static const UINT4b Inf_Positive_ByteForm;
		static const float Inf_Positive;
		static const UINT4b Inf_Negative_ByteForm;
		static const float Inf_Negative;
		static const UINT4b NaN_Positive_ByteForm;
		static const float NaN_Positive;
		static const UINT4b NaN_Negative_ByteForm;
		static const float NaN_Negative;
		static const float Epsilon;
		static const UINT4b Epsilon_Normalized_ByteForm;
		static const float Epsilon_Normalized;
		static const UINT4b Exponent_Mask;
		static const UBINT Exponent_Bias;
		static const UBINT Exponent_Bit_Count;
		static const UINT4b Mantissa_Mask;
		static const UBINT Mantissa_Bit_Count;
		static const float Min;
		static const float Max;
	};

	template <> struct NumericTrait<double>{
		static const UBINT Category = TypeID_FLOAT;
		static const UBINT TypeID = TypeID_FLOAT + 0x3;

		typedef UINT8b uint_type;
		static const UINT8b HighestBit = 0x8000000000000000;
		static const UINT8b Inf_Positive_ByteForm;
		static const double Inf_Positive;
		static const UINT8b Inf_Negative_ByteForm;
		static const double Inf_Negative;
		static const UINT8b NaN_Positive_ByteForm;
		static const double NaN_Positive;
		static const UINT8b NaN_Negative_ByteForm;
		static const double NaN_Negative;
		static const double Epsilon;
		static const UINT8b Epsilon_Normalized_ByteForm;
		static const double Epsilon_Normalized;
		static const UINT8b Exponent_Mask;
		static const UBINT Exponent_Bias;
		static const UBINT Exponent_Bit_Count;
		static const UINT8b Mantissa_Mask;
		static const UBINT Mantissa_Bit_Count;
		static const double Min;
		static const double Max;
	};

	template <UBINT> struct log2intsim_t;
	template <UBINT i> struct log2intsim_t{
		static const UBINT Value = log2intsim_t< i / 2 >::Value + 1;
	};
	template <> struct log2intsim_t<0>{
		static const UBINT Value = 0;
	};

	extern UBINT log2intsim(UINT8b x);
	extern UBINT log2intsim(UINT4b x);
	extern UBINT floor2power(UBINT x); //return 0 when x = 0.
	extern UBINT ceil2power(UBINT x); //return 0 when x = 0.
	extern UBINT is2power(UBINT x); //true for 0
	template <typename T> extern T alignto(T x, T Alignment); //[Alignment] shuold not be 0.
	extern UINT8b reverse(UINT8b x);
	extern UINT4b reverse(UINT4b x);
	extern unsigned char reverse(unsigned char x);
	extern UINT8b change_endian(UINT8b x);
	extern UINT4b change_endian(UINT4b x);
	extern UBINT rand_LCG(UBINT *seed); //generate a pseudo-random number of range [0,UBINT_MAX] and modify the seed.
}

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

namespace nsMath{
	template < typename T, UBINT I > inline bool operator==(const TinyVector<T, I>& lhs, const TinyVector<T, I>& rhs){
		for (UBINT i = 0; i < I; i++) if (lhs.Data[i] != rhs.Data[i])return false;
		return true;
	}
	template < typename T, UBINT I > inline bool operator!=(const TinyVector<T, I>& lhs, const TinyVector<T, I>& rhs){
		for (UBINT i = 0; i < I; i++) if (lhs.Data[i] != rhs.Data[i])return true;
		return false;
	}

	const UINT4b NumericTrait<UINT1b>::GroupOrder_Float_ByteForm = 0x43800000;
	const float NumericTrait<UINT1b>::GroupOrder_Float = *reinterpret_cast<const float *>(&GroupOrder_Float_ByteForm);
	const UINT4b NumericTrait<UINT2b>::GroupOrder_Float_ByteForm = 0x47800000;
	const float NumericTrait<UINT2b>::GroupOrder_Float = *reinterpret_cast<const float *>(&GroupOrder_Float_ByteForm);
	const UINT4b NumericTrait<UINT4b>::GroupOrder_Float_ByteForm = 0x4F800000;
	const float NumericTrait<UINT4b>::GroupOrder_Float = *reinterpret_cast<const float *>(&GroupOrder_Float_ByteForm);
	const UINT4b NumericTrait<UINT8b>::GroupOrder_Float_ByteForm = 0x5F800000;
	const float NumericTrait<UINT8b>::GroupOrder_Float = *reinterpret_cast<const float *>(&GroupOrder_Float_ByteForm);
	const UINT4b NumericTrait<INT1b>::GroupOrder_Float_ByteForm = 0x43800000;
	const float NumericTrait<INT1b>::GroupOrder_Float = *reinterpret_cast<const float *>(&GroupOrder_Float_ByteForm);
	const UINT4b NumericTrait<INT2b>::GroupOrder_Float_ByteForm = 0x47800000;
	const float NumericTrait<INT2b>::GroupOrder_Float = *reinterpret_cast<const float *>(&GroupOrder_Float_ByteForm);
	const UINT4b NumericTrait<INT4b>::GroupOrder_Float_ByteForm = 0x4F800000;
	const float NumericTrait<INT4b>::GroupOrder_Float = *reinterpret_cast<const float *>(&GroupOrder_Float_ByteForm);
	const UINT4b NumericTrait<INT8b>::GroupOrder_Float_ByteForm = 0x5F800000;
	const float NumericTrait<INT8b>::GroupOrder_Float = *reinterpret_cast<const float *>(&GroupOrder_Float_ByteForm);


	const UINT4b NumericTrait<float>::Inf_Positive_ByteForm = 0x7F800000;
	const float NumericTrait<float>::Inf_Positive = *reinterpret_cast<const float *>(&Inf_Positive_ByteForm);
	const UINT4b NumericTrait<float>::Inf_Negative_ByteForm = 0xFF800000;
	const float NumericTrait<float>::Inf_Negative = *reinterpret_cast<const float *>(&Inf_Negative_ByteForm);
	const UINT4b NumericTrait<float>::NaN_Positive_ByteForm = 0x7FFFFFFF;
	const float NumericTrait<float>::NaN_Positive = *reinterpret_cast<const float *>(&NaN_Positive_ByteForm);
	const UINT4b NumericTrait<float>::NaN_Negative_ByteForm = 0xFFFFFFFF;
	const float NumericTrait<float>::NaN_Negative = *reinterpret_cast<const float *>(&NaN_Negative_ByteForm);
	const float NumericTrait<float>::Epsilon = FLT_EPSILON;
	const UINT4b NumericTrait<float>::Epsilon_Normalized_ByteForm = 0x800000;
	const float NumericTrait<float>::Epsilon_Normalized = *reinterpret_cast<const float *>(&Epsilon_Normalized_ByteForm);
	const UINT4b NumericTrait<float>::Exponent_Mask = 0x7F800000;
	const UBINT NumericTrait<float>::Exponent_Bit_Count = 8;
	const UBINT NumericTrait<float>::Exponent_Bias = 0x7F;
	const UINT4b NumericTrait<float>::Mantissa_Mask = 0x7FFFFF;
	const UBINT NumericTrait<float>::Mantissa_Bit_Count = 23;
	const float NumericTrait<float>::Min = NumericTrait<float>::Inf_Negative;
	const float NumericTrait<float>::Max = NumericTrait<float>::Inf_Positive;

	const UINT8b NumericTrait<double>::Inf_Positive_ByteForm = 0x7FF0000000000000;
	const double NumericTrait<double>::Inf_Positive = *reinterpret_cast<const double *>(&Inf_Positive_ByteForm);
	const UINT8b NumericTrait<double>::Inf_Negative_ByteForm = 0xFFF0000000000000;
	const double NumericTrait<double>::Inf_Negative = *reinterpret_cast<const double *>(&Inf_Negative_ByteForm);
	const UINT8b NumericTrait<double>::NaN_Positive_ByteForm = 0x7FFFFFFFFFFFFFFF;
	const double NumericTrait<double>::NaN_Positive = *reinterpret_cast<const double *>(&NaN_Positive_ByteForm);
	const UINT8b NumericTrait<double>::NaN_Negative_ByteForm = 0xFFFFFFFFFFFFFFFF;
	const double NumericTrait<double>::NaN_Negative = *reinterpret_cast<const double *>(&NaN_Negative_ByteForm);
	const double NumericTrait<double>::Epsilon = DBL_EPSILON;
	const UINT8b NumericTrait<double>::Epsilon_Normalized_ByteForm = 0x10000000000000;
	const double NumericTrait<double>::Epsilon_Normalized = *reinterpret_cast<const double *>(&Epsilon_Normalized_ByteForm);
	const UINT8b NumericTrait<double>::Exponent_Mask = 0x7FF0000000000000;
	const UBINT NumericTrait<double>::Exponent_Bit_Count = 11;
	const UBINT NumericTrait<double>::Exponent_Bias = 0x3FF;
	const UINT8b NumericTrait<double>::Mantissa_Mask = 0xFFFFFFFFFFFFF;
	const UBINT NumericTrait<double>::Mantissa_Bit_Count = 52;
	const double NumericTrait<double>::Min = NumericTrait<double>::Inf_Negative;
	const double NumericTrait<double>::Max = NumericTrait<double>::Inf_Positive;

	extern inline UBINT log2intsim(UINT8b x){
		if (0 == x)return 0;
		else{
#if defined LIBENV_CPLR_VS
			UINT4b TmpInt;
#if defined LIBENV_SYS_INTELX86
			if ((x >> 32) > 0){
				_BitScanReverse(&TmpInt, x >> 32);
				TmpInt += 32;
			}
			else _BitScanReverse(&TmpInt, (DWORD)x);
#elif defined LIBENV_SYS_INTELX64
			_BitScanReverse64(&TmpInt, x);
#endif
			return TmpInt + 1;
#elif defined LIBENV_CPLR_GCC
#if defined LIBENV_SYS_INTELX86
			if ((x >> 32) > 0)return 64 - __builtin_clz((UINT4b)(x >> 32));
			else return 32 - __builtin_clz((UINT4b)x);
#elif defined LIBENV_SYS_INTELX64
			return 64 - __builtin_clzll(x);
#endif
#endif
		}
	}

	extern inline UBINT log2intsim(UINT4b x){
		if (0 == x)return 0;
		else{
#if defined LIBENV_CPLR_VS
			UINT4b TmpInt;
			_BitScanReverse(&TmpInt, x);
			return TmpInt + 1;
#elif defined LIBENV_CPLR_GCC
			return 32 - __builtin_clz(x);
#endif
		}
	}
	/* This is the best bit-hacking way I've seen for this.

	extern inline UBINT log2intsim(UINT8b x){
	#ifdef LIBDBG_PLATFORM_TEST
	#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
	#endif

	//This function is highly preformance-critical and platform-related.

	if (0 == x)return 0;
	static const int DeBruijnIndex[64] = {
	1, 12, 2, 13, 17, 30, 3, 14, 23, 18, 42, 26, 31, 49, 4, 62,
	15, 21, 24, 19, 35, 37, 43, 27, 39, 32, 54, 45, 50, 57, 5, 63,
	11, 16, 29, 22, 41, 25, 48, 61, 20, 34, 36, 38, 53, 44, 56, 10,
	28, 40, 47, 60, 33, 52, 55, 9, 46, 59, 51, 8, 58, 7, 6, 64,
	};

	const UBINT ShiftAmnt = 64 - 6, DeBrujinPath = 0x03F08A4C6ACB9DBD;

	x |= x >> 32;
	x |= x >> 16;
	x |= x >> 8;
	x |= x >> 4;
	x |= x >> 2;
	x |= x >> 1;
	return DeBruijnIndex[(x*DeBrujinPath) >> ShiftAmnt];
	}

	extern inline UBINT log2intsim(UINT4b x){
	#ifdef LIBDBG_PLATFORM_TEST
	#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
	#endif

	//This function is highly preformance-critical and platform-related.

	if (0 == x)return 0;
	static const UBINT DeBruijnIndex[32] = {
	1, 10, 2, 11, 14, 22, 3, 30, 12, 15, 17, 19, 23, 26, 4, 31,
	9, 13, 21, 29, 16, 18, 25, 8, 20, 28, 24, 7, 27, 6, 5, 32
	};

	const UBINT ShiftAmnt = 32 - 5, DeBrujinPath = 0x07C4ACDD;
	x |= x >> 16;
	x |= x >> 8;
	x |= x >> 4;
	x |= x >> 2;
	x |= x >> 1;
	return DeBruijnIndex[(x*DeBrujinPath) >> ShiftAmnt];
	}
	*/

	/* This function performs better in Debug mode of Visual Studio, but worse in Release mode.
	* This function branches heavily. It may be slower on a high branch-cost machine.

	extern inline UBINT log2intsim(UINT8b x){
	#ifdef LIBDBG_PLATFORM_TEST
	#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
	#endif

	//This function is highly preformance-critical and platform-related.

	UBINT Result=1;
	if(0==x)return 0;
	if(x>=0xFFFFFFFF){x>>=32;Result+=32;}
	if(x>=0x0000FFFF){x>>=16;Result+=16;}
	if(x>=0x000000FF){x>>=8;Result+=8;}
	if(x>=0x0000000F){x>>=4;Result+=4;}
	if(x>=0x00000003){x>>=2;Result+=2;}
	if(x>=0x00000001){x>>=1;Result+=1;}
	return Result;
	}

	extern inline UBINT log2intsim(UINT4b x){
	#ifdef LIBDBG_PLATFORM_TEST
	#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
	#endif

	//This function is highly preformance-critical and platform-related.

	UBINT Result=1;
	if(0==x)return 0;
	if(x>=0x0000FFFF){x>>=16;Result+=16;}
	if(x>=0x000000FF){x>>=8;Result+=8;}
	if(x>=0x0000000F){x>>=4;Result+=4;}
	if(x>=0x00000003){x>>=2;Result+=2;}
	if(x>=0x00000001){x>>=1;Result+=1;}
	return Result;
	}
	*/

	extern inline UBINT floor2power(UBINT x){
		if (x <= 1)return x;
		else{
#if defined LIBENV_CPLR_VS
			UINT4b TmpInt;
#if defined LIBENV_SYS_INTELX64
			_BitScanReverse64(&TmpInt, x);
#elif defined LIBENV_SYS_INTELX86
			_BitScanReverse(&TmpInt, x);
#endif
			return (UBINT)1 << TmpInt;
#elif defined LIBENV_CPLR_GCC
#if defined LIBENV_SYS_INTELX64
			return (UBINT)1 << (63 - __builtin_clzll(x));
#elif defined LIBENV_SYS_INTELX86
			return (UBINT)1 << (31 - __builtin_clz(x));
#endif
#endif
		}
	}

	/* bit-twiddling way:
	extern inline UBINT ceil2power(UBINT x){
	#ifdef LIBDBG_PLATFORM_TEST
	#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
	#endif

	//This function is highly preformance-critical and platform-related.

	#if defined LIBENV_SYS_INTELX64
	x |= x >> 32;
	#else
	#endif
	x |= x >> 16;
	x |= x >> 8;
	x |= x >> 4;
	x |= x >> 2;
	x |= x >> 1;
	return x - (x >> 1);
	}
	*/

	extern inline UBINT ceil2power(UBINT x){
		if (x <= 1)return x;
		else{
#if defined LIBENV_CPLR_VS
			UINT4b TmpInt;
#if defined LIBENV_SYS_INTELX64
			_BitScanReverse64(&TmpInt, x - 1);
#elif defined LIBENV_SYS_INTELX86
			_BitScanReverse(&TmpInt, x - 1);
#endif
			return (UBINT)2 << TmpInt;
#elif defined LIBENV_CPLR_GCC
#if defined LIBENV_SYS_INTELX64
			return (UBINT)1 << (64 - __builtin_clzll(x - 1));
#elif defined LIBENV_SYS_INTELX86
			return (UBINT)1 << (32 - __builtin_clz(x - 1));
#endif
#endif
		}
	}

	/* bit-twiddling way:
	extern inline UBINT ceil2power(UBINT x){
	#ifdef LIBDBG_PLATFORM_TEST
	#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
	#endif

	//This function is highly preformance-critical and platform-related.

	if (0 == x)return 0;
	else{
	x--;
	#if defined LIBENV_SYS_INTELX64
	x |= x >> 32;
	#else
	#endif
	x |= x >> 16;
	x |= x >> 8;
	x |= x >> 4;
	x |= x >> 2;
	x |= x >> 1;
	return x + 1;
	}
	}
	*/

	extern inline UBINT is2power(UBINT x){return !(x&(x - 1)); }

	template <typename T> extern inline T alignto(T x, T Alignment){
		T TmpInt = x + Alignment - (T)1; return TmpInt - TmpInt % Alignment;
	}

	extern inline UINT8b reverse(UINT8b x){
		x = ((x >> 1) & 0x5555555555555555) | ((x & 0x5555555555555555) << 1);
		x = ((x >> 2) & 0x3333333333333333) | ((x & 0x3333333333333333) << 2);
		x = ((x >> 4) & 0x0F0F0F0F0F0F0F0F) | ((x & 0x0F0F0F0F0F0F0F0F) << 4);
		x = ((x >> 8) & 0x00FF00FF00FF00FF) | ((x & 0x00FF00FF00FF00FF) << 8);
		x = ((x >> 16) & 0x0000FFFF0000FFFF) | ((x & 0x0000FFFF0000FFFF) << 16);
		return (x >> 32) | (x << 32);
	}
	extern inline UINT4b reverse(UINT4b x){
		x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
		x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
		x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
		x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
		return (x >> 16) | (x << 16);
	}
	extern inline unsigned char reverse(unsigned char x){
		x = ((x >> 1) & 0x55) | ((x & 0x55) << 1);
		x = ((x >> 2) & 0x33) | ((x & 0x33) << 2);
		return (x >> 4) | (x << 4);
	}
	extern UINT8b change_endian(UINT8b x){
		x = (x >> 32) | ((x & 0xFFFFFFFF) << 32);
		x = ((x & 0xFFFF0000FFFF0000) >> 16) | ((x & 0x0000FFFF0000FFFF) << 16);
		x = ((x & 0xFF00FF00FF00FF00) >> 8) | ((x & 0x00FF00FF00FF00FF) << 8);
		return x;
	}
	extern UINT4b change_endian(UINT4b x){
		x = (x >> 16) | ((x & 0xFFFF) << 16);
		x = ((x & 0xFF00FF00) >> 8) | ((x & 0xFF00FF) << 8);
		return x;
	}
	extern inline UBINT rand_LCG(UBINT *seed){
#if defined LIBENV_SYS_INTELX86
		return *seed = *seed * 0x43FD43FD + 0xC39EC3; //from visual basic
#elif defined LIBENV_SYS_INTELX64
		return *seed = *seed * 0x5851F42D4C957F2D + 0x14057B7EF767814F; //from Donald Knuth's MMIX
#endif
	}
}

template <> const UBINT Type_To_LocalID<UINT1b>::Value = nsMath::NumericTrait<UINT1b>::TypeID;
template <> const UBINT Type_To_LocalID<UINT2b>::Value = nsMath::NumericTrait<UINT2b>::TypeID;
template <> const UBINT Type_To_LocalID<UINT4b>::Value = nsMath::NumericTrait<UINT4b>::TypeID;
template <> const UBINT Type_To_LocalID<UINT8b>::Value = nsMath::NumericTrait<UINT8b>::TypeID;
template <> const UBINT Type_To_LocalID<INT1b>::Value = nsMath::NumericTrait<INT1b>::TypeID;
template <> const UBINT Type_To_LocalID<INT2b>::Value = nsMath::NumericTrait<INT2b>::TypeID;
template <> const UBINT Type_To_LocalID<INT4b>::Value = nsMath::NumericTrait<INT4b>::TypeID;
template <> const UBINT Type_To_LocalID<INT8b>::Value = nsMath::NumericTrait<INT8b>::TypeID;
template <> const UBINT Type_To_LocalID<float>::Value = nsMath::NumericTrait<float>::TypeID;
template <> const UBINT Type_To_LocalID<double>::Value = nsMath::NumericTrait<double>::TypeID;

template <> struct LocalID_To_Type<nsMath::TypeID_UINT + 0x0>{ typedef UINT1b Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_UINT + 0x1>{ typedef UINT2b Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_UINT + 0x2>{ typedef UINT4b Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_UINT + 0x3>{ typedef UINT8b Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_INT + 0x0>{ typedef INT1b Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_INT + 0x1>{ typedef INT2b Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_INT + 0x2>{ typedef INT4b Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_INT + 0x3>{ typedef INT8b Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_FLOAT + 0x2>{ typedef float Type; };
template <> struct LocalID_To_Type<nsMath::TypeID_FLOAT + 0x3>{ typedef double Type; };