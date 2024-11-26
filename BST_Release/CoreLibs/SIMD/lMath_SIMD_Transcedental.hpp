/* Description: Platform-independent mathematical functions for transcedental functions, with SIMD acceleration. DO NOT include this header directly.
 * Language: C++
 * Author: ***
 */

#ifndef LIB_MATH_SIMD_TRANSCEDENTAL
#define LIB_MATH_SIMD_TRANSCEDENTAL

#include "lGeneral.hpp"
#include "lMacros.hpp"

namespace nsMath{
#if defined LIBENV_CPU_ACCEL_SSE
#define _TUPLE_4(x) {x, x, x, x}
	extern inline __m64& _SSE_fetch_1st_pair(__m128i& scl){ return ((__m64 *)&scl)[0]; }
	extern inline const __m64& _SSE_fetch_1st_pair(const __m128i& scl){ return ((const __m64 *)&scl)[0]; }
	extern inline __m64& _SSE_fetch_1st_pair(__m128& scl){ return ((__m64 *)&scl)[0]; }
	extern inline const __m64& _SSE_fetch_1st_pair(const __m128& scl){ return ((const __m64 *)&scl)[0]; }
	extern inline __m64& _SSE_fetch_2nd_pair(__m128i& scl){ return ((__m64 *)&scl)[1]; }
	extern inline const __m64& _SSE_fetch_2nd_pair(const __m128i& scl){ return ((const __m64 *)&scl)[1]; }
	extern inline __m64& _SSE_fetch_2nd_pair(__m128& scl){ return ((__m64 *)&scl)[1]; }
	extern inline const __m64& _SSE_fetch_2nd_pair(const __m128& scl){ return ((const __m64 *)&scl)[1]; }

	extern inline __m128i& _AVX_fetch_1st_pair(__m256i& scl){ return ((__m128i *)&scl)[0]; }
	extern inline const __m128i& _AVX_fetch_1st_pair(const __m256i& scl){ return ((const __m128i *)&scl)[0]; }
	extern inline __m128& _AVX_fetch_1st_pair(__m256& scl){ return ((__m128 *)&scl)[0]; }
	extern inline const __m128& _AVX_fetch_1st_pair(const __m256& scl){ return ((const __m128 *)&scl)[0]; }
	extern inline __m128i& _AVX_fetch_2nd_pair(__m256i& scl){ return ((__m128i *)&scl)[1]; }
	extern inline const __m128i& _AVX_fetch_2nd_pair(const __m256i& scl){ return ((const __m128i *)&scl)[1]; }
	extern inline __m128& _AVX_fetch_2nd_pair(__m256& scl){ return ((__m128 *)&scl)[1]; }
	extern inline const __m128& _AVX_fetch_2nd_pair(const __m256& scl){ return ((const __m128 *)&scl)[1]; }

	// here "lowres" means that the output result doesn't conform to the IEEE standard.
	extern __m128 _log_simd_SSE_lowres(__m128 scl);

	extern __m128 _exp_simd_SSE_lowres(__m128 scl);
	
#endif

#if defined LIBENV_CPU_ACCEL_AVX
#define _TUPLE_8(x) {x, x, x, x, x, x, x, x}

	extern __m256 _log_simd_AVX_lowres(__m256 scl);

	extern __m256 _exp_simd_AVX_lowres(__m256 scl);
#endif

	template<class T> extern void log_simd_lowres_aligned(T *result, const T *vec, UBINT Length);
	//approximately equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = log(vec[i]);
	//the pointer [result], [vec0] and [vec1] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void log_simd_lowres_unaligned(T *result, const T *vec, UBINT Length);
	//approximately equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = log(vec[i]);

	template<class T> extern void exp_simd_lowres_aligned(T *result, const T *vec, UBINT Length);
	//approximately equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = exp(vec[i]);
	//the pointer [result], [vec0] and [vec1] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void exp_simd_lowres_unaligned(T *result, const T *vec, UBINT Length);
	//approximately equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = exp(vec[i]);

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

#if defined LIBENV_CPU_ACCEL_SSE
	extern __m128 _log_simd_SSE_lowres(__m128 scl){
		// The algorithm here comes from the library of Julien Pommier (http://gruntthepeon.free.fr/ssemath/), which in turn comes from the cephes mathematical library. But the approximation polynomial is actually not the same.

		// constant declaration --BEGIN--
		// floats
		static const __m128 SIMDConst_1 = _TUPLE_4(1.0f); // Valid initialization, see the C++11 standard 8.5.1.16.
		static const __m128 SIMDConst_0_5 = _TUPLE_4(0.5f);
		static const __m128 SIMDConst_M_SQRT1_2 = _TUPLE_4((float)M_SQRT1_2);
		static const __m128 SIMDConst_M_LN2 = _TUPLE_4((float)M_LN2);
		static const __m128 SIMDConst_Normalize = _TUPLE_4(nsMath::NumericTrait<float>::Epsilon_Normalized);
		static const __m128 SIMDConst_Mantissa = _TUPLE_4(*reinterpret_cast<const float *>(&nsMath::NumericTrait<float>::Mantissa_Mask));

		// Polynomial approximation coefficients. Very close to the Taylor expansion of log(x + 1).
		static const __m128 SIMDConst_P11 = _TUPLE_4(7.0376836292E-2f);
		static const __m128 SIMDConst_P10 = _TUPLE_4(-1.1514610310E-1f);
		static const __m128 SIMDConst_P09 = _TUPLE_4(1.1676998740E-1f);
		static const __m128 SIMDConst_P08 = _TUPLE_4(-1.2420140846E-1f);
		static const __m128 SIMDConst_P07 = _TUPLE_4(1.4249322787E-1f);
		static const __m128 SIMDConst_P06 = _TUPLE_4(-1.6668057665E-1f);
		static const __m128 SIMDConst_P05 = _TUPLE_4(2.0000714765E-1f);
		static const __m128 SIMDConst_P04 = _TUPLE_4(-2.4999993993E-1f);
		static const __m128 SIMDConst_P03 = _TUPLE_4(3.3333331174E-1f);

		// integers
		static const union{
			UINT4b _I[4];
			__m128i Value;
		} SIMDConst_Bias = _TUPLE_4(nsMath::NumericTrait<float>::Exponent_Bias - 1);
		// constant declaration --END--

		__m128 NaN_Mask = _mm_cmple_ps(scl, _mm_setzero_ps());

		scl = _mm_max_ps(scl, SIMDConst_Normalize); // ignore the denormalized float numbers

		// extract the exponent --BEGIN--
		__m128 Exponent;

#ifdef LIBENV_CPU_ACCEL_SSE2
		__m128i ExpInt = _mm_srli_epi32(_mm_castps_si128(scl), nsMath::NumericTrait<float>::Mantissa_Bit_Count);
		ExpInt = _mm_sub_epi32(ExpInt, SIMDConst_Bias.Value);
		Exponent = _mm_cvtepi32_ps(ExpInt);
#else
		__m64 ExpInt_1st = _mm_srli_pi32(_SSE_fetch_1st_pair(scl), nsMath::NumericTrait<float>::Mantissa_Bit_Count);
		__m64 ExpInt_2nd = _mm_srli_pi32(_SSE_fetch_2nd_pair(scl), nsMath::NumericTrait<float>::Mantissa_Bit_Count);

		ExpInt_1st = _mm_sub_pi32(ExpInt_1st, _SSE_fetch_1st_pair(SIMDConst_Bias.Value));
		ExpInt_2nd = _mm_sub_pi32(ExpInt_2nd, _SSE_fetch_1st_pair(SIMDConst_Bias.Value));
		Exponent = _mm_cvtpi32x2_ps(ExpInt_1st, ExpInt_2nd);
		_mm_empty(); // after using MMX registers
#endif

		scl = _mm_and_ps(scl, SIMDConst_Mantissa); // now [scl] only contains the mantissa part.
		scl = _mm_or_ps(scl, SIMDConst_0_5); // this converts [scl] into a number inside [0.5, 1.0).
		// extract the exponent --END--
		// now original [scl] is equivalent to [scl] * 2 ^ [Exponent]. And its logarithm is equal to log([scl]) + [Exponent] * M_LN2.

		// Project [scl] into the interval [sqrt(0.5), sqrt(2)]. If [scl] is smaller than sqrt(0.5), it need to be processed:
		__m128 TmpMask = _mm_cmplt_ps(scl, SIMDConst_M_SQRT1_2);
		Exponent = _mm_sub_ps(Exponent, _mm_and_ps(SIMDConst_1, TmpMask));
		scl = _mm_add_ps(scl, _mm_and_ps(scl, TmpMask));
		
		// Minus 1. The polynomial expansion of log(x + 1) doesn't need a constant term.
		scl = _mm_sub_ps(scl, SIMDConst_1);

		// rational approximation to log(x + 1).
		__m128 y = _mm_mul_ps(SIMDConst_P11, scl);
		y = _mm_add_ps(y, SIMDConst_P10);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P09);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P08);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P07);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P06);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P05);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P04);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P03);
		y = _mm_mul_ps(y, scl);
		y = _mm_sub_ps(y, SIMDConst_0_5);
		y = _mm_mul_ps(y, _mm_mul_ps(scl, scl));
		scl = _mm_add_ps(scl, y);

		scl = _mm_add_ps(scl, _mm_mul_ps(Exponent, SIMDConst_M_LN2));

		return _mm_or_ps(scl, NaN_Mask);
	}

	extern __m128 _exp_simd_SSE_lowres(__m128 scl){
		// The algorithm here comes from the library of Julien Pommier (http://gruntthepeon.free.fr/ssemath/), which in turn comes from the cephes mathematical library. But the approximation polynomial is actually not the same.

		// constant declaration --BEGIN--
		// floats
		static const __m128 SIMDConst_Exp_LBound = _TUPLE_4(-88.3762626647949f);
		static const __m128 SIMDConst_Exp_HBound = _TUPLE_4(88.3762626647949f);

		static const __m128 SIMDConst_1 = _TUPLE_4(1.0f); // Valid initialization, see the C++11 standard 8.5.1.16.
		static const __m128 SIMDConst_M_LN2 = _TUPLE_4((float)M_LN2);
		static const __m128 SIMDConst_M_LN2_INV = _TUPLE_4(1.44269504088896341f);

		// Polynomial approximation coefficients. Very close to the Taylor expansion of exp(x).
		static const __m128 SIMDConst_P07 = _TUPLE_4(1.9875691500E-4f);
		static const __m128 SIMDConst_P06 = _TUPLE_4(1.3981999507E-3f);
		static const __m128 SIMDConst_P05 = _TUPLE_4(8.3334519073E-3f);
		static const __m128 SIMDConst_P04 = _TUPLE_4(4.1665795894E-2f);
		static const __m128 SIMDConst_P03 = _TUPLE_4(1.6666665459E-1f);
		static const __m128 SIMDConst_P02 = _TUPLE_4(5.0000001201E-1f);

		// integers
		static const union{
			UINT4b _I[4];
			__m128i Value;
		} SIMDConst_Bias = _TUPLE_4(nsMath::NumericTrait<float>::Exponent_Bias);
		// constant declaration --END--

		__m128 NaN_Mask = _mm_cmple_ps(scl, _mm_setzero_ps());

		scl = _mm_max_ps(scl, SIMDConst_Exp_LBound);
		scl = _mm_min_ps(scl, SIMDConst_Exp_HBound);

		// now we'll factorize [scl] into a + b * log(2).
		__m128 Exponent = _mm_mul_ps(scl, SIMDConst_M_LN2_INV);
		__m128i ExpInt;

		// Exponent = round(Exponent)
#ifdef LIBENV_CPU_ACCEL_SSE2
		ExpInt = _mm_cvtps_epi32(Exponent); // no overflow may occur, since the valid range of exp() is small
		Exponent = _mm_cvtepi32_ps(ExpInt);
#else
		__m64 ExpInt_1st = _mm_cvttps_pi32(Exponent);
		__m64 ExpInt_2nd = _mm_cvttps_pi32(_mm_movehl_ps(Exponent, Exponent));
		Exponent = _mm_cvtpi32x2_ps(ExpInt_1st, ExpInt_2nd);
#endif

		scl = _mm_sub_ps(scl, _mm_mul_ps(Exponent, SIMDConst_M_LN2));
		// now [scl] is inside [-0.5 * log(2), 0.5 * log(2)].

		// rational approximation to exp(x).
		__m128 y = _mm_mul_ps(SIMDConst_P07, scl);
		y = _mm_add_ps(y, SIMDConst_P06);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P05);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P04);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P03);
		y = _mm_mul_ps(y, scl);
		y = _mm_add_ps(y, SIMDConst_P02);
		y = _mm_mul_ps(y, _mm_mul_ps(scl, scl));
		y = _mm_add_ps(y, scl);
		scl = _mm_add_ps(y, SIMDConst_1);

#ifdef LIBENV_CPU_ACCEL_SSE2
		ExpInt = _mm_add_epi32(ExpInt, SIMDConst_Bias.Value);
		ExpInt = _mm_slli_epi32(ExpInt, nsMath::NumericTrait<float>::Mantissa_Bit_Count);
#else
		ExpInt_1st = _mm_add_pi32(ExpInt_1st, _SSE_fetch_1st_pair(SIMDConst_Bias.Value));
		ExpInt_2nd = _mm_add_pi32(ExpInt_2nd, _SSE_fetch_1st_pair(SIMDConst_Bias.Value));
		ExpInt_1st = _mm_slli_pi32(ExpInt_1st, nsMath::NumericTrait<float>::Mantissa_Bit_Count);
		ExpInt_2nd = _mm_slli_pi32(ExpInt_2nd, nsMath::NumericTrait<float>::Mantissa_Bit_Count);
		
		((__m64 *)&ExpInt)[0] = ExpInt_1st; ((__m64 *)&ExpInt)[1] = ExpInt_2nd;
		_mm_empty(); // after using MMX registers
#endif

		return _mm_mul_ps(scl, _mm_castsi128_ps(ExpInt));
	}
#endif

#if defined LIBENV_CPU_ACCEL_AVX
	extern __m256 _log_simd_AVX_lowres(__m256 scl){
		// The algorithm here comes from the library of Julien Pommier (http://gruntthepeon.free.fr/ssemath/), which in turn comes from the cephes mathematical library. But the approximation polynomial is actually not the same.

		// constant declaration --BEGIN--
		// floats
		static const __m256 SIMDConst_1 = _TUPLE_8(1.0f); // Valid initialization, see the C++11 standard 8.5.1.16.
		static const __m256 SIMDConst_0_5 = _TUPLE_8(0.5f);
		static const __m256 SIMDConst_M_SQRT1_2 = _TUPLE_8((float)M_SQRT1_2);
		static const __m256 SIMDConst_M_LN2 = _TUPLE_8((float)M_LN2);
		static const __m256 SIMDConst_Normalize = _TUPLE_8(nsMath::NumericTrait<float>::Epsilon_Normalized);
		static const __m256 SIMDConst_Mantissa = _TUPLE_8(*reinterpret_cast<const float *>(&nsMath::NumericTrait<float>::Mantissa_Mask));

		// Polynomial approximation coefficients. Very close to the Taylor expansion of log(x + 1).
		static const __m256 SIMDConst_P11 = _TUPLE_8(7.0376836292E-2f);
		static const __m256 SIMDConst_P10 = _TUPLE_8(-1.1514610310E-1f);
		static const __m256 SIMDConst_P09 = _TUPLE_8(1.1676998740E-1f);
		static const __m256 SIMDConst_P08 = _TUPLE_8(-1.2420140846E-1f);
		static const __m256 SIMDConst_P07 = _TUPLE_8(1.4249322787E-1f);
		static const __m256 SIMDConst_P06 = _TUPLE_8(-1.6668057665E-1f);
		static const __m256 SIMDConst_P05 = _TUPLE_8(2.0000714765E-1f);
		static const __m256 SIMDConst_P04 = _TUPLE_8(-2.4999993993E-1f);
		static const __m256 SIMDConst_P03 = _TUPLE_8(3.3333331174E-1f);

		// integers
		static const union{
			UINT4b _I[8];
			__m256i Value;
		} SIMDConst_Bias = _TUPLE_8(nsMath::NumericTrait<float>::Exponent_Bias - 1);
		// constant declaration --END--

		__m256 NaN_Mask = _mm256_cmp_ps(scl, _mm256_setzero_ps(), _CMP_LE_OS);

		scl = _mm256_max_ps(scl, SIMDConst_Normalize); // ignore the denormalized float numbers

		// extract the exponent --BEGIN--

#ifdef LIBENV_CPU_ACCEL_AVX2
		__m256i ExpInt = _mm256_srli_epi32(_mm256_castps_si256(scl), nsMath::NumericTrait<float>::Mantissa_Bit_Count);
		ExpInt = _mm256_sub_epi32(ExpInt, SIMDConst_Bias.Value);
		__m256 Exponent = _mm256_cvtepi32_ps(ExpInt);
#else
		__m128i ExpInt_1st = _mm_srli_epi32(_AVX_fetch_1st_pair(_mm256_castps_si256(scl)), nsMath::NumericTrait<float>::Mantissa_Bit_Count);
		__m128i ExpInt_2nd = _mm_srli_epi32(_AVX_fetch_2nd_pair(_mm256_castps_si256(scl)), nsMath::NumericTrait<float>::Mantissa_Bit_Count);
		ExpInt_1st = _mm_sub_epi32(ExpInt_1st, _AVX_fetch_1st_pair(SIMDConst_Bias.Value));
		ExpInt_2nd = _mm_sub_epi32(ExpInt_2nd, _AVX_fetch_1st_pair(SIMDConst_Bias.Value));
		__m256 Exponent = _mm256_cvtepi32_ps(_mm256_set_m128i(ExpInt_2nd, ExpInt_1st));
#endif

		scl = _mm256_and_ps(scl, SIMDConst_Mantissa); // now [scl] only contains the mantissa part.
		scl = _mm256_or_ps(scl, SIMDConst_0_5); // this converts [scl] into a number inside [0.5, 1.0).
		// extract the exponent --END--
		// now original [scl] is equivalent to [scl] * 2 ^ [Exponent]. And its logarithm is equal to log([scl]) + [Exponent] * M_LN2.

		// Project [scl] into the interval [sqrt(0.5), sqrt(2)]. If [scl] is smaller than sqrt(0.5), it need to be processed:
		__m256 TmpMask = _mm256_cmp_ps(scl, SIMDConst_M_SQRT1_2, _CMP_LT_OS);
		Exponent = _mm256_sub_ps(Exponent, _mm256_and_ps(SIMDConst_1, TmpMask));
		scl = _mm256_add_ps(scl, _mm256_and_ps(scl, TmpMask));

		// Minus 1. The polynomial expansion of log(x + 1) doesn't need a constant term.
		scl = _mm256_sub_ps(scl, SIMDConst_1);

		// rational approximation to log(x + 1).
		__m256 y = _mm256_mul_ps(SIMDConst_P11, scl);
		y = _mm256_add_ps(y, SIMDConst_P10);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P09);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P08);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P07);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P06);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P05);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P04);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P03);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_sub_ps(y, SIMDConst_0_5);
		y = _mm256_mul_ps(y, _mm256_mul_ps(scl, scl));
		scl = _mm256_add_ps(scl, y);

		scl = _mm256_add_ps(scl, _mm256_mul_ps(Exponent, SIMDConst_M_LN2));

		return _mm256_or_ps(scl, NaN_Mask);
	}

	extern __m256 _exp_simd_AVX_lowres(__m256 scl){
		// The algorithm here comes from the library of Julien Pommier (http://gruntthepeon.free.fr/ssemath/), which in turn comes from the cephes mathematical library. But the approximation polynomial is actually not the same.

		// constant declaration --BEGIN--
		// floats
		static const __m256 SIMDConst_Exp_LBound = _TUPLE_8(-88.3762626647949f);
		static const __m256 SIMDConst_Exp_HBound = _TUPLE_8(88.3762626647949f);

		static const __m256 SIMDConst_1 = _TUPLE_8(1.0f); // Valid initialization, see the C++11 standard 8.5.1.16.
		static const __m256 SIMDConst_M_LN2 = _TUPLE_8((float)M_LN2);
		static const __m256 SIMDConst_M_LN2_INV = _TUPLE_8(1.44269504088896341f);

		// Polynomial approximation coefficients. Very close to the Taylor expansion of exp(x).
		static const __m256 SIMDConst_P07 = _TUPLE_8(1.9875691500E-4f);
		static const __m256 SIMDConst_P06 = _TUPLE_8(1.3981999507E-3f);
		static const __m256 SIMDConst_P05 = _TUPLE_8(8.3334519073E-3f);
		static const __m256 SIMDConst_P04 = _TUPLE_8(4.1665795894E-2f);
		static const __m256 SIMDConst_P03 = _TUPLE_8(1.6666665459E-1f);
		static const __m256 SIMDConst_P02 = _TUPLE_8(5.0000001201E-1f);

		// integers
		static const union{
			UINT4b _I[8];
			__m256i Value;
		} SIMDConst_Bias = _TUPLE_8(nsMath::NumericTrait<float>::Exponent_Bias);
		// constant declaration --END--

		__m256 NaN_Mask = _mm256_cmp_ps(scl, _mm256_setzero_ps(), _CMP_LE_OS);

		scl = _mm256_max_ps(scl, SIMDConst_Exp_LBound);
		scl = _mm256_min_ps(scl, SIMDConst_Exp_HBound);

		// now we'll factorize [scl] into a + b * log(2).
		__m256 Exponent = _mm256_mul_ps(scl, SIMDConst_M_LN2_INV);
		__m256i ExpInt;

		// Exponent = round(Exponent)
		ExpInt = _mm256_cvtps_epi32(Exponent); // no overflow may occur, since the valid range of exp() is small
		Exponent = _mm256_cvtepi32_ps(ExpInt);

		scl = _mm256_sub_ps(scl, _mm256_mul_ps(Exponent, SIMDConst_M_LN2));
		// now [scl] is inside [-0.5 * log(2), 0.5 * log(2)].

		// rational approximation to exp(x).
		__m256 y = _mm256_mul_ps(SIMDConst_P07, scl);
		y = _mm256_add_ps(y, SIMDConst_P06);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P05);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P04);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P03);
		y = _mm256_mul_ps(y, scl);
		y = _mm256_add_ps(y, SIMDConst_P02);
		y = _mm256_mul_ps(y, _mm256_mul_ps(scl, scl));
		y = _mm256_add_ps(y, scl);
		scl = _mm256_add_ps(y, SIMDConst_1);

#ifdef LIBENV_CPU_ACCEL_AVX2
		ExpInt = _mm256_add_epi32(ExpInt, SIMDConst_Bias.Value);
		ExpInt = _mm256_slli_epi32(ExpInt, nsMath::NumericTrait<float>::Mantissa_Bit_Count);
#else
		__m128i ExpInt_1st = _mm_add_epi32(_AVX_fetch_1st_pair(ExpInt), _AVX_fetch_1st_pair(SIMDConst_Bias.Value));
		__m128i ExpInt_2nd = _mm_add_epi32(_AVX_fetch_2nd_pair(ExpInt), _AVX_fetch_1st_pair(SIMDConst_Bias.Value));
		ExpInt_1st = _mm_slli_epi32(ExpInt_1st, nsMath::NumericTrait<float>::Mantissa_Bit_Count);
		ExpInt_2nd = _mm_slli_epi32(ExpInt_2nd, nsMath::NumericTrait<float>::Mantissa_Bit_Count);

		ExpInt = _mm256_set_m128i(ExpInt_2nd, ExpInt_1st);
#endif

		return _mm256_mul_ps(scl, _mm256_castsi256_ps(ExpInt));
	}
#endif

	template <> extern void log_simd_lowres_aligned<float>(float *result, const float *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = log(vec[i]);
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result, _log_simd_AVX_lowres(_mm256_load_ps(vec)));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result, _log_simd_SSE_lowres(_mm_load_ps(vec)));
#endif
			vec += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = log(vec[i]);
#endif
	}

	template <> extern void log_simd_lowres_unaligned<float>(float *result, const float *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = log(vec[i]);
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = log(vec[i]);
			result += Extra;
			vec += Extra;
			Length -= Extra;

			log_simd_lowres_aligned(result, vec, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result, _log_simd_AVX_lowres(_mm256_load_ps(vec)));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_store_ps(result, _log_simd_SSE_lowres(_mm_load_ps(vec)));
#endif
				vec += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = log(vec[i]);
		}
#endif
	}

	template <> extern void exp_simd_lowres_aligned<float>(float *result, const float *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = exp(vec[i]);
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result, _exp_simd_AVX_lowres(_mm256_load_ps(vec)));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result, _exp_simd_SSE_lowres(_mm_load_ps(vec)));
#endif
			vec += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = exp(vec[i]);
#endif
	}

	template <> extern void exp_simd_lowres_unaligned<float>(float *result, const float *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = exp(vec[i]);
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = exp(vec[i]);
			result += Extra;
			vec += Extra;
			Length -= Extra;

			exp_simd_lowres_aligned(result, vec, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result, _exp_simd_AVX_lowres(_mm256_load_ps(vec)));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_store_ps(result, _exp_simd_SSE_lowres(_mm_load_ps(vec)));
#endif
				vec += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = exp(vec[i]);
		}
#endif
	}

#if defined LIBENV_CPU_ACCEL_AVX
#undef _TUPLE_8
#endif

#if defined LIBENV_CPU_ACCEL_SSE
#undef _TUPLE_4
#endif
}
#endif