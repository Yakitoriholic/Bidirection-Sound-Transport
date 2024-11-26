/* Description: Platform-independent mathematical functions for memory initialization, with SIMD acceleration. DO NOT include this header directly.
 * Language: C++
 * Author: ***
 */

#ifndef LIB_MATH_SIMD_SET
#define LIB_MATH_SIMD_SET

#include "lGeneral.hpp"
#include "lMacros.hpp"

namespace nsMath{
	template<class T> extern void setzero_simd_aligned(T *result, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = 0;
	//the pointer [result] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void setzero_simd_unaligned(T *result, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = 0;

	template<class T> extern void set_simd_aligned(T *result, const T scl, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = scl;
	//the pointer [result] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void set_simd_unaligned(T *result, const T scl, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = scl;

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	template<class T> extern void setzero_simd_aligned(T *result, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		UBINT *result_unpacked = (UBINT *)result;

		UBINT LoopCount = (Length * sizeof(T)) / sizeof(UBINT);
		UBINT Remainder = (Length * sizeof(T)) % sizeof(UBINT);

		for (UBINT i = 0; i < LoopCount; i++)*(result_unpacked++) = 0;

		unsigned char *result_remainder = (unsigned char *)result_unpacked;
		for (UBINT i = 0; i < Remainder; i++)result_remainder[i] = (unsigned char)0;
#else
		float *result_unpacked = (float *)result;

		UBINT LoopCount = Length * sizeof(T) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length * sizeof(T) - LoopCount * nsEnv::SIMDAlignment;

#if defined LIBENV_CPU_ACCEL_AVX
		__m256 SIMD_Vec = _mm256_setzero_ps();
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 SIMD_Vec = _mm_setzero_ps();
#endif

		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result_unpacked, SIMD_Vec);
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result_unpacked, SIMD_Vec);
#endif
			result_unpacked += nsEnv::SIMDAlignment / sizeof(float);
		}

		unsigned char *result_remainder = (unsigned char *)result_unpacked;
		for (UBINT i = 0; i < Remainder / sizeof(UBINT); i++){
			*(UBINT *)result_remainder = (UBINT)0;
			result_remainder += sizeof(UBINT);
		}
		Remainder &= sizeof(UBINT)-1;
		for (UBINT i = 0; i < Remainder; i++)result_remainder[i] = (unsigned char)0;
#endif
	}

	template<class T> extern void setzero_simd_unaligned(T *result, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback

		//check for alignment
		//suppose that sizeof(UBINT) is a power of 2
		UBINT TotalBytes = Length * sizeof(T);
		UBINT ExtraBytes = sizeof(UBINT) - ((sizeof(UBINT) - 1) & (UBINT)result);
		if (ExtraBytes < TotalBytes)ExtraBytes = TotalBytes;

		unsigned char *result_unpacked = (unsigned char *)result;
		for (UBINT i = 0; i < ExtraBytes; i++)result_unpacked[i] = (unsigned char)0;
		result_unpacked += ExtraBytes;

		TotalBytes -= ExtraBytes;
		if (TotalBytes > 0)setzero_simd_aligned(result_unpacked, TotalBytes);
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT TotalBytes = Length * sizeof(T);
		UBINT ExtraBytes = nsEnv::SIMDAlignment - ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (ExtraBytes > TotalBytes)ExtraBytes = TotalBytes;

		unsigned char *result_unpacked = (unsigned char *)result;
		for (UBINT i = 0; i < (ExtraBytes & (sizeof(UBINT) - 1)); i++)result_unpacked[i] = (unsigned char)0;
		result_unpacked += ExtraBytes & (sizeof(UBINT)-1);
		for (UBINT i = 0; i < ExtraBytes / sizeof(UBINT); i++){
			*(UBINT *)result_unpacked = (UBINT)0;
			result_unpacked += sizeof(UBINT);
		}

		TotalBytes -= ExtraBytes;
		setzero_simd_aligned(result_unpacked, TotalBytes);
#endif
	}

	template<UBINT I> void _set_simd_aligned_bytes(unsigned char *result, const unsigned char *src, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		UBINT SIMD_Vec;
		if (I == sizeof(UBINT))SIMD_Vec = *(UBINT *)src;
		else{
			UBINT SIMD_Vec_Tmp = 0;
			for (UBINT i = 0; i < I; i++)SIMD_Vec_Tmp = (SIMD_Vec_Tmp << 8) + src[I - i - 1];
			SIMD_Vec = 0;
			for (UBINT i = 0; i < sizeof(UBINT) / I; i++)SIMD_Vec = (SIMD_Vec << (8 * I)) + SIMD_Vec_Tmp;
		}

		UBINT LoopCount = Length / sizeof(UBINT);
		UBINT Remainder = Length % sizeof(UBINT);

		for (UBINT i = 0; i < LoopCount; i++){
			*(UBINT *)result = SIMD_Vec;
			result += sizeof(UBINT);
		}

		for (UBINT i = 0; i < Remainder; i++)result[i] = src[i % I];
#else
#if defined LIBENV_CPU_ACCEL_AVX
		__m256 SIMD_Vec = _mm256_setzero_ps();
		if (I == nsEnv::SIMDAlignment)SIMD_Vec = _mm256_loadu_ps((const float *)src);
		else{
			__m256i TmpVec;
			for (UBINT i = 0; i < nsEnv::SIMDAlignment; i++)TmpVec.m256i_u8[i] = src[i % I];
			SIMD_Vec = _mm256_castsi256_ps(TmpVec);
		}
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 SIMD_Vec;
		if (I == nsEnv::SIMDAlignment)SIMD_Vec = _mm_loadu_ps((const float *)src);
		else for (UBINT i = 0; i < nsEnv::SIMDAlignment; i++)SIMD_Vec.m128_u8[i] = src[i % I];
#endif

		UBINT LoopCount = Length / nsEnv::SIMDAlignment;
		UBINT Remainder = Length % nsEnv::SIMDAlignment;

		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps((float *)result, SIMD_Vec);
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps((float *)result, SIMD_Vec);
#endif
			result += nsEnv::SIMDAlignment;
		}

		for (UBINT i = 0; i < Remainder; i++)result[i] = src[i % I];
#endif
	}

	template<UBINT I> void _set_simd_unaligned_bytes(unsigned char *result, const unsigned char *src, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback

		//check for alignment
		//suppose that sizeof(UBINT) is a power of 2
		UBINT ExtraBytes = sizeof(UBINT)-((sizeof(UBINT)-1) & (UBINT)result);
		if (0 == ExtraBytes)_set_simd_aligned_bytes<I>(result, src, Length);
		else if (ExtraBytes > Length)for (UBINT i = 0; i < Length; i++)result[i] = src[i % I];
		else{
			//unaligned
			for (UBINT i = 0; i < ExtraBytes; i++)result[i] = src[i % I];
			Length -= ExtraBytes;
			result += ExtraBytes;

			UBINT SIMD_Vec;
			if (I == sizeof(UBINT))SIMD_Vec = *(UBINT *)src;
			else{
				UBINT SIMD_Vec_Tmp = 0;
				for (UBINT i = 0; i < I; i++)SIMD_Vec_Tmp = (SIMD_Vec_Tmp << 8) + src[I - i - 1];
				SIMD_Vec = 0;
				for (UBINT i = 0; i < sizeof(UBINT) / I; i++)SIMD_Vec = (SIMD_Vec << (8 * I)) + SIMD_Vec_Tmp;
			}
			SIMD_Vec = (SIMD_Vec >> (ExtraBytes * 8)) + (SIMD_Vec << ((sizeof(UBINT) - ExtraBytes) * 8));

			UBINT LoopCount = Length / sizeof(UBINT);
			UBINT Remainder = Length % sizeof(UBINT);

			for (UBINT i = 0; i < LoopCount; i++){
				*(UBINT *)result = SIMD_Vec;
				result += sizeof(UBINT);
			}

			for (UBINT i = 0; i < Remainder; i++)result[i] = src[(i + nsEnv::SIMDAlignment - ExtraBytes) % I];
		}
#else
		UBINT ExtraBytes = nsEnv::SIMDAlignment - ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes)_set_simd_aligned_bytes<I>(result, src, Length);
		else if (ExtraBytes > Length)for (UBINT i = 0; i < Length; i++)result[i] = src[i % I];
		else{
			//unaligned
			for (UBINT i = 0; i < ExtraBytes; i++)result[i] = src[i % I];
			Length -= ExtraBytes;
			result += ExtraBytes;

#if defined LIBENV_CPU_ACCEL_AVX
			__m256 SIMD_Vec = _mm256_setzero_ps();
			__m256i TmpVec;
			for (UBINT i = 0; i < nsEnv::SIMDAlignment; i++)TmpVec.m256i_u8[i] = src[(i + ExtraBytes) % I];
			SIMD_Vec = _mm256_castsi256_ps(TmpVec);
#elif defined LIBENV_CPU_ACCEL_SSE
			__m128 SIMD_Vec;
			for (UBINT i = 0; i < nsEnv::SIMDAlignment; i++)SIMD_Vec.m128_u8[i] = src[(i + ExtraBytes) % I];
#endif

			UBINT LoopCount = Length / nsEnv::SIMDAlignment;
			UBINT Remainder = Length % nsEnv::SIMDAlignment;

			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_store_ps((float *)result, SIMD_Vec);
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_store_ps((float *)result, SIMD_Vec);
#endif
				result += nsEnv::SIMDAlignment;
			}

			for (UBINT i = 0; i < Remainder; i++)result[i] = src[(i + nsEnv::SIMDAlignment - ExtraBytes) % I];
		}
#endif
	}

	template<class T, bool _fallback> struct _set_simd_aligned;

	template<class T, bool _fallback> struct _set_simd_unaligned;

	template<class T> struct _set_simd_aligned<T, true>{
		static inline void Func(T *result, const T scl, UBINT Length){
			for (UBINT i = 0; i < Length; i++)result[i] = scl;
		}
	};

	template<class T> struct _set_simd_aligned<T, false>{
		static inline void Func(T *result, const T scl, UBINT Length){
			_set_simd_aligned_bytes<sizeof(T)>((unsigned char *)result, (const unsigned char *)&scl, Length * sizeof(T));
		}
	};

	template<class T> struct _set_simd_unaligned<T, true>{
		static inline void Func(T *result, const T scl, UBINT Length){
			for (UBINT i = 0; i < Length; i++)result[i] = scl;
		}
	};

	template<class T> struct _set_simd_unaligned<T, false>{
		static inline void Func(T *result, const T scl, UBINT Length){
			_set_simd_unaligned_bytes<sizeof(T)>((unsigned char *)result, (const unsigned char *)&scl, Length * sizeof(T));
		}
	};

	template<class T> extern inline void set_simd_aligned(T *result, const T scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		const bool _NeedFallback = (sizeof(T) > sizeof(UBINT)) || (sizeof(UBINT) % sizeof(T) > 0);
		_set_simd_aligned<T, _NeedFallback>::Func(result, scl, Length);
#else
		const bool _NeedFallback = sizeof(T) > nsEnv::SIMDAlignment || (sizeof(UBINT) % sizeof(T) > 0);
		_set_simd_aligned<T, _NeedFallback>::Func(result, scl, Length);
#endif
	}

	template<class T> extern inline void set_simd_unaligned(T *result, const T scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		const bool _NeedFallback = sizeof(T) > sizeof(UBINT) || (sizeof(UBINT) % sizeof(T) > 0);
		_set_simd_unaligned<T, _NeedFallback>::Func(result, scl, Length);
#else
		const bool _NeedFallback = sizeof(T) > nsEnv::SIMDAlignment || (sizeof(UBINT) % sizeof(T) > 0);
		_set_simd_unaligned<T, _NeedFallback>::Func(result, scl, Length);
#endif
	}
}
#endif