/* Description: Platform-independent type conversion functions with SIMD acceleration. DO NOT include this header directly.
 * Language: C++
 * Author: ***
 */

#ifndef LIB_MATH_SIMD_CONVERT
#define LIB_MATH_SIMD_CONVERT

#include "lGeneral.hpp"
#include "lMacros.hpp"

namespace nsMath{
	template<class T1, class T2> extern void convert_simd_aligned(T1 *result, const T2 *vec, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = (T1)vec[i];
	//the pointer [result], [vec0] and [vec1] must be aligned to nsEnv::SIMDAlignment.

	template<class T1, class T2> extern void convert_simd_unaligned(T1 *result, const T2 *vec, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = (T1)vec[i];

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	template <> extern void convert_simd_aligned<INT2b, float>(INT2b *result, const float *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = (INT2b)vec[i];
#else
		UBINT LoopCount = Length * sizeof(INT2b) / sizeof(__m128);
		UBINT Remainder = Length - LoopCount * (sizeof(__m128) / sizeof(INT2b));
		const float *vec0 = vec;
		const float *vec1 = vec + sizeof(__m128) / sizeof(float);
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_SSE2
			_mm_store_ps((float *)result, _mm_castsi128_ps(_mm_packs_epi16(_mm_cvtps_epi32(_mm_load_ps(vec0)), _mm_cvtps_epi32(_mm_load_ps(vec1)))));
#elif defined LIBENV_CPU_ACCEL_SSE
			*(__m64 *)result = _mm_cvtps_pi16(_mm_load_ps(vec0));
			*((__m64 *)result + 1) = _mm_cvtps_pi16(_mm_load_ps(vec1));
#endif
			vec0 += sizeof(__m128) / sizeof(INT2b);
			vec1 += sizeof(__m128) / sizeof(INT2b);
			result += sizeof(__m128) / sizeof(INT2b);
		}
#if !defined LIBENV_CPU_ACCEL_SSE2
		_mm_empty();
#endif
		for (UBINT i = 0; i < Remainder; i++)result[i] = (INT2b)vec0[i];
#endif
	}

	template<> extern void convert_simd_unaligned<INT2b, float>(INT2b *result, const float *vec, UBINT Length){
		//incorrect
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = (INT2b)vec[i];
#else
		UBINT LoopCount = Length * sizeof(INT2b) / sizeof(__m128);
		UBINT Remainder = Length - LoopCount * (sizeof(__m128) / sizeof(INT2b));
		const float *vec0 = vec;
		const float *vec1 = vec + sizeof(__m128) / sizeof(float);
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_SSE2
			_mm_store_ps((float *)result, _mm_castsi128_ps(_mm_packs_epi16(_mm_cvtps_epi32(_mm_load_ps(vec0)), _mm_cvtps_epi32(_mm_load_ps(vec1)))));
#elif defined LIBENV_CPU_ACCEL_SSE
			*(__m64 *)result = _mm_cvtps_pi16(_mm_load_ps(vec0));
			*((__m64 *)result + 1) = _mm_cvtps_pi16(_mm_load_ps(vec1));
#endif
			vec0 += sizeof(__m128) / sizeof(INT2b);
			vec1 += sizeof(__m128) / sizeof(INT2b);
			result += sizeof(__m128) / sizeof(INT2b);
		}
#if !defined LIBENV_CPU_ACCEL_SSE2
		_mm_empty();
#endif
		for (UBINT i = 0; i < Remainder; i++)result[i] = (INT2b)vec0[i];
#endif
	}
}
#endif