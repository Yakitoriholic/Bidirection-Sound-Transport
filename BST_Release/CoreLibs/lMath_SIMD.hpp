/* Description: Platform-independent mathematical functions with SIMD acceleration.
 * Language: C++
 * Author: ***
 */

#ifndef LIB_MATH_SIMD
#define LIB_MATH_SIMD

#include "SIMD/lMath_SIMD_Set.hpp"
#include "SIMD/lMath_SIMD_Convert.hpp"
#include "SIMD/lMath_SIMD_Transcedental.hpp"

#include "lGeneral.hpp"

namespace nsMath{
	template<class T> extern void add_simd_aligned(T *result, const T *vec0, const T *vec1, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
	//the pointer [result], [vec0] and [vec1] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void add_simd_unaligned(T *result, const T *vec0, const T *vec1, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];

	template<class T> extern void mul_simd_aligned(T *result, const T *vec, const T scl, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec[i] * scl;
	//the pointer [result] and [vec] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void mul_simd_aligned(T *result, const T *vec0, const T *vec1, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
	//the pointer [result], [vec0] and [vec1] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void mul_simd_unaligned(T *result, const T *vec, const T scl, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec[i] * scl;

	template<class T> extern void mul_simd_unaligned(T *result, const T *vec0, const T *vec1, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];

	template<class T> extern void muladd_simd_aligned(T *result, const T *vec, const T scl, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] += vec[i] * scl;
	//the pointer [result] and [vec] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void muladd_simd_aligned(T *result, const T *vec0, const T *vec1, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
	//the pointer [result], [vec0] and [vec1] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void muladd_simd_unaligned(T *result, const T *vec, const T scl, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] += vec[i] * scl;

	template<class T> extern void muladd_simd_unaligned(T *result, const T *vec0, const T *vec1, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];

	template<class T> extern void lerp_simd_aligned(T *result, const T *vec0, const T *vec1, const T scl, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec0[i] + scl * (vec1[i] - vec0[i]);
	//the pointer [result] and [vec] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void lerp_simd_aligned(T *result, const T *vec0, const T *vec1, const T *vec2, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
	//the pointer [result], [vec0] and [vec1] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void lerp_simd_unaligned(T *result, const T *vec0, const T *vec1, const T scl, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec0[i] + scl * (vec1[i] - vec0[i]);

	template<class T> extern void lerp_simd_unaligned(T *result, const T *vec0, const T *vec1, const T *vec2, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);

	template<class T> extern void sqrt_simd_aligned(T *result, const T *vec, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = sqrt(vec[i]);
	//the pointer [result], [vec0] and [vec1] must be aligned to nsEnv::SIMDAlignment.

	template<class T> extern void sqrt_simd_unaligned(T *result, const T *vec, UBINT Length);
	//equivalent to the following code:
	//for(UBINT i = 0; i < Length; i++)result[i] = sqrt(vec[i]);

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	template <> extern void add_simd_aligned<float>(float *result, const float *vec0, const float *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result, _mm256_add_ps(_mm256_load_ps(vec0), _mm256_load_ps(vec1)));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result, _mm_add_ps(_mm_load_ps(vec0), _mm_load_ps(vec1)));
#endif
			vec0 += nsEnv::SIMDAlignment / sizeof(float);
			vec1 += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] + vec1[i];
#endif
	}

	template <> extern void add_simd_aligned<double>(double *result, const double *vec0, const double *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
#else
		UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_pd(result, _mm256_add_pd(_mm256_load_pd(vec0), _mm256_load_pd(vec1)));
#elif defined LIBENV_CPU_ACCEL_SSE2
			_mm_store_pd(result, _mm_add_pd(_mm_load_pd(vec0), _mm_load_pd(vec1)));
#endif
			vec0 += nsEnv::SIMDAlignment / sizeof(double);
			vec1 += nsEnv::SIMDAlignment / sizeof(double);
			result += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] + vec1[i];
#endif
	}

	template <> extern void add_simd_aligned<std::complex<float>>(std::complex<float> *result, const std::complex<float> *vec0, const std::complex<float> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
#else
		float *result_unpacked = (float *)result;
		float *vec0_unpacked = (float *)vec0;
		float *vec1_unpacked = (float *)vec1;

		UBINT LoopCount = Length * sizeof(std::complex<float>) / nsEnv::SIMDAlignment;
		UBINT Remainder = (Length * sizeof(std::complex<float>) - LoopCount * nsEnv::SIMDAlignment) / sizeof(float);
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result_unpacked, _mm256_add_ps(_mm256_load_ps(vec0_unpacked), _mm256_load_ps(vec1_unpacked)));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result_unpacked, _mm_add_ps(_mm_load_ps(vec0_unpacked), _mm_load_ps(vec1_unpacked)));
#endif
			vec0_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			vec1_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			result_unpacked += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result_unpacked[i] = vec0_unpacked[i] + vec1_unpacked[i];
#endif
	}

	template <> extern void add_simd_aligned<std::complex<double>>(std::complex<double> *result, const std::complex<double> *vec0, const std::complex<double> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
#else
		double *result_unpacked = (double *)result;
		double *vec0_unpacked = (double *)vec0;
		double *vec1_unpacked = (double *)vec1;

		UBINT LoopCount = Length * sizeof(std::complex<double>) / nsEnv::SIMDAlignment;
		UBINT Remainder = (Length * sizeof(std::complex<double>) - LoopCount * nsEnv::SIMDAlignment) / sizeof(double);
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_pd(result_unpacked, _mm256_add_pd(_mm256_load_pd(vec0_unpacked), _mm256_load_pd(vec1_unpacked)));
#elif defined LIBENV_CPU_ACCEL_SSE2
			_mm_store_pd(result_unpacked, _mm_add_pd(_mm_load_pd(vec0_unpacked), _mm_load_pd(vec1_unpacked)));
#endif
			vec0_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			vec1_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			result_unpacked += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)result_unpacked[i] = vec0_unpacked[i] + vec1_unpacked[i];
#endif
	}

	template <> extern void add_simd_unaligned<float>(float *result, const float *vec0, const float *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] + vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			add_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result, _mm256_add_ps(_mm256_loadu_ps(vec0), _mm256_loadu_ps(vec1)));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_store_ps(result, _mm_add_ps(_mm_loadu_ps(vec0), _mm_loadu_ps(vec1)));
#endif
				vec0 += nsEnv::SIMDAlignment / sizeof(float);
				vec1 += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] + vec1[i];
		}
#endif
	}

	template <> extern void add_simd_unaligned<double>(double *result, const double *vec0, const double *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(double) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(double);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] + vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			add_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_pd(result, _mm256_add_pd(_mm256_loadu_pd(vec0), _mm256_loadu_pd(vec1)));
#elif defined LIBENV_CPU_ACCEL_SSE2
				_mm_store_pd(result, _mm_add_pd(_mm_loadu_pd(vec0), _mm_loadu_pd(vec1)));
#endif
				vec0 += nsEnv::SIMDAlignment / sizeof(double);
				vec1 += nsEnv::SIMDAlignment / sizeof(double);
				result += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] + vec1[i];
		}
#endif
	}

	template <> extern void add_simd_unaligned<std::complex<float>>(std::complex<float> *result, const std::complex<float> *vec0, const std::complex<float> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(std::complex<float>) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(std::complex<float>);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] + vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			add_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			float *result_unpacked = (float *)result;
			float *vec0_unpacked = (float *)vec0;
			float *vec1_unpacked = (float *)vec1;

			UBINT LoopCount = Length * sizeof(std::complex<float>) / nsEnv::SIMDAlignment;
			UBINT Remainder = (Length * sizeof(std::complex<float>) - LoopCount * nsEnv::SIMDAlignment) / sizeof(float);
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result_unpacked, _mm256_add_ps(_mm256_loadu_ps(vec0_unpacked), _mm256_loadu_ps(vec1_unpacked)));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_store_ps(result_unpacked, _mm_add_ps(_mm_loadu_ps(vec0_unpacked), _mm_loadu_ps(vec1_unpacked)));
#endif
				vec0_unpacked += nsEnv::SIMDAlignment / sizeof(float);
				vec1_unpacked += nsEnv::SIMDAlignment / sizeof(float);
				result_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result_unpacked[i] = vec0_unpacked[i] + vec1_unpacked[i];
		}
#endif
	}

	template <> extern void add_simd_unaligned<std::complex<double>>(std::complex<double> *result, const std::complex<double> *vec0, const std::complex<double> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(std::complex<double>) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(std::complex<double>);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] + vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			add_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			double *result_unpacked = (double *)result;
			double *vec0_unpacked = (double *)vec0;
			double *vec1_unpacked = (double *)vec1;

			UBINT LoopCount = Length * sizeof(std::complex<double>) / nsEnv::SIMDAlignment;
			UBINT Remainder = (Length * sizeof(std::complex<double>) - LoopCount * nsEnv::SIMDAlignment) / sizeof(double);
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_pd(result_unpacked, _mm256_add_pd(_mm256_loadu_pd(vec0_unpacked), _mm256_loadu_pd(vec1_unpacked)));
#elif defined LIBENV_CPU_ACCEL_SSE2
				_mm_store_pd(result_unpacked, _mm_add_pd(_mm_loadu_pd(vec0_unpacked), _mm_loadu_pd(vec1_unpacked)));
#endif
				vec0_unpacked += nsEnv::SIMDAlignment / sizeof(double);
				vec1_unpacked += nsEnv::SIMDAlignment / sizeof(double);
				result_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)result_unpacked[i] = vec0_unpacked[i] + vec1_unpacked[i];
		}
#endif
	}

	template <> extern void mul_simd_aligned<float>(float *result, const float *vec, const float scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec[i] * scl;
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
#if defined LIBENV_CPU_ACCEL_AVX
		__m256 scl_simd = _mm256_broadcast_ss(&scl);
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 scl_simd = _mm_set_ps1(scl);
#endif
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result, _mm256_mul_ps(_mm256_load_ps(vec), scl_simd));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result, _mm_mul_ps(_mm_load_ps(vec), scl_simd));
#endif
			vec += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = vec[i] * scl;
#endif
	}

	template <> extern void mul_simd_aligned<float>(float *result, const float *vec0, const float *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result, _mm256_mul_ps(_mm256_load_ps(vec0), _mm256_load_ps(vec1)));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result, _mm_mul_ps(_mm_load_ps(vec0), _mm_load_ps(vec1)));
#endif
			vec0 += nsEnv::SIMDAlignment / sizeof(float);
			vec1 += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] * vec1[i];
#endif
	}

	template <> extern void mul_simd_aligned<double>(double *result, const double *vec, const double scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec[i] * scl;
#else
		UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
#if defined LIBENV_CPU_ACCEL_AVX
		__m256d scl_simd = _mm256_broadcast_sd(&scl);
#elif defined LIBENV_CPU_ACCEL_SSE2
		__m128d scl_simd = _mm_set_pd(scl, scl);
#endif
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_pd(result, _mm256_mul_pd(_mm256_load_pd(vec), scl_simd));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_pd(result, _mm_mul_pd(_mm_load_pd(vec), scl_simd));
#endif
			vec += nsEnv::SIMDAlignment / sizeof(double);
			result += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = vec[i] * scl;
#endif
	}

	template <> extern void mul_simd_aligned<double>(double *result, const double *vec0, const double *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
#else
		UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_pd(result, _mm256_mul_pd(_mm256_load_pd(vec0), _mm256_load_pd(vec1)));
#elif defined LIBENV_CPU_ACCEL_SSE2
			_mm_store_pd(result, _mm_mul_pd(_mm_load_pd(vec0), _mm_load_pd(vec1)));
#endif
			vec0 += nsEnv::SIMDAlignment / sizeof(double);
			vec1 += nsEnv::SIMDAlignment / sizeof(double);
			result += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] * vec1[i];
#endif
	}

	template <> extern void mul_simd_aligned<std::complex<float>>(std::complex<float> *result, const std::complex<float> *vec0, const std::complex<float> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
#else
		float *result_unpacked = (float *)result;
		float *vec0_unpacked = (float *)vec0;
		float *vec1_unpacked = (float *)vec1;

		UBINT LoopCount = Length * sizeof(std::complex<float>) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment) / sizeof(std::complex<float>);
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			__m256 vec0_shuffle0 = _mm256_load_ps(vec0_unpacked);
			__m256 vec1_simd = _mm256_load_ps(vec1_unpacked);

			__m256 vec0_shuffle1 = _mm256_shuffle_ps(vec0_shuffle0, vec0_shuffle0, 0xB1); //0xB1 = 10 11 00 01
			__m256 vec1_shuffle0 = _mm256_shuffle_ps(vec1_simd, vec1_simd, 0xA0); //0xA0 = 10 10 00 00
			__m256 vec1_shuffle1 = _mm256_shuffle_ps(vec1_simd, vec1_simd, 0xF5); //0xF5 = 11 11 01 01

#if defined LIBENV_CPU_ACCEL_FMA
			__m256 res2 = _mm256_mul_ps(vec0_shuffle1, vec1_shuffle1);
			__m256 res1 = _mm256_fmaddsub_ps(vec0_shuffle0, vec1_shuffle0, res2);
			_mm256_store_ps(result_unpacked, res1);
#else
			__m256 res1 = _mm256_mul_ps(vec0_shuffle0, vec1_shuffle0);
			__m256 res2 = _mm256_mul_ps(vec0_shuffle1, vec1_shuffle1);

			_mm256_store_ps(result_unpacked, _mm256_addsub_ps(res1, res2));
#endif
#elif defined LIBENV_CPU_ACCEL_SSE
			__m128 vec0_shuffle0 = _mm_load_ps(vec0_unpacked);
			__m128 vec1_simd = _mm_load_ps(vec1_unpacked);

			__m128 vec0_shuffle1 = _mm_shuffle_ps(vec0_shuffle0, vec0_shuffle0, 0xB1); //0xB1 = 10 11 00 01
			__m128 vec1_shuffle0 = _mm_shuffle_ps(vec1_simd, vec1_simd, 0xA0); //0xA0 = 10 10 00 00
			__m128 vec1_shuffle1 = _mm_shuffle_ps(vec1_simd, vec1_simd, 0xF5); //0xF5 = 11 11 01 01

			__m128 res1 = _mm_mul_ps(vec0_shuffle0, vec1_shuffle0);
			__m128 res2 = _mm_mul_ps(vec0_shuffle1, vec1_shuffle1);
#if defined LIBENV_CPU_ACCEL_SSE3
			_mm_store_ps(result_unpacked, _mm_addsub_ps(res1, res2));
#else
			const __m128 sign = { -1.0f, 1.0f, -1.0f, 1.0f};

			res2 = _mm_mul_ps(res2, sign);
			_mm_store_ps(result_unpacked, _mm_add_ps(res1, res2));
#endif
#endif
			vec0_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			vec1_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			result_unpacked += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)((std::complex<float> *)result_unpacked)[i] = ((std::complex<float> *)vec0_unpacked)[i] * ((std::complex<float> *)vec1_unpacked)[i];
#endif
	}

	template <> extern void mul_simd_aligned<std::complex<double>>(std::complex<double> *result, const std::complex<double> *vec0, const std::complex<double> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
#else
		double *result_unpacked = (double *)result;
		double *vec0_unpacked = (double *)vec0;
		double *vec1_unpacked = (double *)vec1;

		UBINT LoopCount = Length * sizeof(std::complex<double>) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment) / sizeof(std::complex<double>);
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			__m256d vec0_shuffle0 = _mm256_load_pd(vec0_unpacked);
			__m256d vec1_simd = _mm256_load_pd(vec1_unpacked);

			__m256d vec0_shuffle1 = _mm256_shuffle_pd(vec0_shuffle0, vec0_shuffle0, 0x5); //0xA = 0 1 0 1
			__m256d vec1_shuffle0 = _mm256_shuffle_pd(vec1_simd, vec1_simd, 0x0); //0x0 = 0 0 0 0
			__m256d vec1_shuffle1 = _mm256_shuffle_pd(vec1_simd, vec1_simd, 0xF); //0xF = 1 1 1 1

#if defined LIBENV_CPU_ACCEL_FMA
			__m256d res2 = _mm256_mul_pd(vec0_shuffle1, vec1_shuffle1);
			__m256d res1 = _mm256_fmaddsub_pd(vec0_shuffle0, vec1_shuffle0, res2);
			_mm256_store_pd(result_unpacked, res1);
#else
			__m256d res1 = _mm256_mul_pd(vec0_shuffle0, vec1_shuffle0);
			__m256d res2 = _mm256_mul_pd(vec0_shuffle1, vec1_shuffle1);

			_mm256_store_pd(result_unpacked, _mm256_addsub_pd(res1, res2));
#endif
#elif defined LIBENV_CPU_ACCEL_SSE2
			__m128d vec0_shuffle0 = _mm_load_pd(vec0_unpacked);
			__m128d vec1_simd = _mm_load_pd(vec1_unpacked);

			__m128d vec0_shuffle1 = _mm_shuffle_pd(vec0_shuffle0, vec0_shuffle0, 0x1); //0x1 = 0 1
			__m128d vec1_shuffle0 = _mm_shuffle_pd(vec1_simd, vec1_simd, 0x0); //0x0 = 0 0
			__m128d vec1_shuffle1 = _mm_shuffle_pd(vec1_simd, vec1_simd, 0x3); //0xB1 = 1 1

			__m128d res1 = _mm_mul_pd(vec0_shuffle0, vec1_shuffle0);
			__m128d res2 = _mm_mul_pd(vec0_shuffle1, vec1_shuffle1);
#if defined LIBENV_CPU_ACCEL_SSE3
			_mm_store_pd(result_unpacked, _mm_addsub_pd(res1, res2));
#else
			const __m128d sign = { -1.0, 1.0 };

			res2 = _mm_mul_pd(res2, sign);
			_mm_store_pd(result_unpacked, _mm_add_pd(res1, res2));
#endif
#endif
			vec0_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			vec1_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			result_unpacked += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)((std::complex<double> *)result_unpacked)[i] = ((std::complex<double> *)vec0_unpacked)[i] * ((std::complex<double> *)vec1_unpacked)[i];
#endif
	}

	template <> extern void muladd_simd_aligned<float>(float *result, const float *vec, const float scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec[i] * scl;
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
#if defined LIBENV_CPU_ACCEL_AVX
		__m256 scl_simd = _mm256_broadcast_ss(&scl);
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 scl_simd = _mm_set_ps1(scl);
#endif
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
			_mm256_store_ps(result, _mm256_fmadd_ps(_mm256_load_ps(vec), scl_simd, _mm256_load_ps(result)));
#elif defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result, _mm256_add_ps(_mm256_load_ps(result), _mm256_mul_ps(_mm256_load_ps(vec), scl_simd)));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result, _mm_add_ps(_mm_load_ps(result), _mm_mul_ps(_mm_load_ps(vec), scl_simd)));
#endif
			vec += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] += vec[i] * scl;
#endif
	}

	template <> extern void muladd_simd_aligned<float>(float *result, const float *vec0, const float *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
			_mm256_store_ps(result, _mm256_fmadd_ps(_mm256_load_ps(vec0), _mm256_load_ps(vec1), _mm256_load_ps(result)));
#elif defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result, _mm256_add_ps(_mm256_load_ps(result), _mm256_mul_ps(_mm256_load_ps(vec0), _mm256_load_ps(vec1))));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result, _mm_add_ps(_mm_load_ps(result), _mm_mul_ps(_mm_load_ps(vec0), _mm_load_ps(vec1))));
#endif
			vec0 += nsEnv::SIMDAlignment / sizeof(float);
			vec1 += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] += vec0[i] * vec1[i];
#endif
	}

	template <> extern void muladd_simd_aligned<double>(double *result, const double *vec, const double scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec[i] * scl;
#else
		UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
#if defined LIBENV_CPU_ACCEL_AVX
		__m256d scl_simd = _mm256_broadcast_sd(&scl);
#elif defined LIBENV_CPU_ACCEL_SSE2
		__m128d scl_simd = _mm_set_pd(scl, scl);
#endif
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
			_mm256_store_pd(result, _mm256_fmadd_pd(_mm256_load_pd(vec), scl_simd, _mm256_load_pd(result)));
#elif defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_pd(result, _mm256_add_pd(_mm256_load_pd(result), _mm256_mul_pd(_mm256_load_pd(vec), scl_simd)));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_pd(result, _mm_add_pd(_mm_load_pd(result), _mm_mul_pd(_mm_load_pd(vec), scl_simd)));
#endif
			vec += nsEnv::SIMDAlignment / sizeof(double);
			result += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] += vec[i] * scl;
#endif
	}

	template <> extern void muladd_simd_aligned<double>(double *result, const double *vec0, const double *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
#else
		UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
			_mm256_store_pd(result, _mm256_fmadd_pd(_mm256_load_pd(vec0), _mm256_load_pd(vec1), _mm256_load_pd(result)));
#elif defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_pd(result, _mm256_add_pd(_mm256_load_pd(result), _mm256_mul_pd(_mm256_load_pd(vec0), _mm256_load_pd(vec1))));
#elif defined LIBENV_CPU_ACCEL_SSE2
			_mm_store_pd(result, _mm_add_pd(_mm_load_pd(result), _mm_mul_pd(_mm_load_pd(vec0), _mm_load_pd(vec1))));
#endif
			vec0 += nsEnv::SIMDAlignment / sizeof(double);
			vec1 += nsEnv::SIMDAlignment / sizeof(double);
			result += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] += vec0[i] * vec1[i];
#endif
	}

	template <> extern void muladd_simd_aligned<std::complex<float>>(std::complex<float> *result, const std::complex<float> *vec0, const std::complex<float> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
#else
		float *result_unpacked = (float *)result;
		float *vec0_unpacked = (float *)vec0;
		float *vec1_unpacked = (float *)vec1;

		UBINT LoopCount = Length * sizeof(std::complex<float>) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment) / sizeof(std::complex<float>);
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			__m256 vec0_shuffle0 = _mm256_load_ps(vec0_unpacked);
			__m256 vec1_simd = _mm256_load_ps(vec1_unpacked);

			__m256 vec0_shuffle1 = _mm256_shuffle_ps(vec0_shuffle0, vec0_shuffle0, 0xB1); //0xB1 = 10 11 00 01
			__m256 vec1_shuffle0 = _mm256_shuffle_ps(vec1_simd, vec1_simd, 0xA0); //0xA0 = 10 10 00 00
			__m256 vec1_shuffle1 = _mm256_shuffle_ps(vec1_simd, vec1_simd, 0xF5); //0xF5 = 11 11 01 01

#if defined LIBENV_CPU_ACCEL_FMA
			__m256 res2 = _mm256_mul_ps(vec0_shuffle1, vec1_shuffle1);
			__m256 res1 = _mm256_fmaddsub_ps(vec0_shuffle0, vec1_shuffle0, res2);
			_mm256_store_ps(result_unpacked, _mm256_add_ps(_mm256_load_ps(result_unpacked), res1));
#else
			__m256 res1 = _mm256_mul_ps(vec0_shuffle0, vec1_shuffle0);
			__m256 res2 = _mm256_mul_ps(vec0_shuffle1, vec1_shuffle1);

			_mm256_store_ps(result_unpacked, _mm256_add_ps(_mm256_load_ps(result_unpacked), _mm256_addsub_ps(res1, res2)));
#endif
#elif defined LIBENV_CPU_ACCEL_SSE
			__m128 vec0_shuffle0 = _mm_load_ps(vec0_unpacked);
			__m128 vec1_simd = _mm_load_ps(vec1_unpacked);

			__m128 vec0_shuffle1 = _mm_shuffle_ps(vec0_shuffle0, vec0_shuffle0, 0xB1); //0xB1 = 10 11 00 01
			__m128 vec1_shuffle0 = _mm_shuffle_ps(vec1_simd, vec1_simd, 0xA0); //0xA0 = 10 10 00 00
			__m128 vec1_shuffle1 = _mm_shuffle_ps(vec1_simd, vec1_simd, 0xF5); //0xF5 = 11 11 01 01

			__m128 res1 = _mm_mul_ps(vec0_shuffle0, vec1_shuffle0);
			__m128 res2 = _mm_mul_ps(vec0_shuffle1, vec1_shuffle1);
#if defined LIBENV_CPU_ACCEL_SSE3
			_mm_store_ps(result_unpacked, _mm_add_ps(_mm_load_ps(result_unpacked), _mm_addsub_ps(res1, res2)));
#else
			const __m128 sign = { -1.0f, 1.0f, -1.0f, 1.0f };

			res2 = _mm_mul_ps(res2, sign);
			_mm_store_ps(result_unpacked, _mm_add_ps(_mm_load_ps(result_unpacked), _mm_add_ps(res1, res2)));
#endif
#endif
			vec0_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			vec1_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			result_unpacked += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)((std::complex<float> *)result_unpacked)[i] += ((std::complex<float> *)vec0_unpacked)[i] * ((std::complex<float> *)vec1_unpacked)[i];
#endif
	}

	template <> extern void muladd_simd_aligned<std::complex<double>>(std::complex<double> *result, const std::complex<double> *vec0, const std::complex<double> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
#else
		double *result_unpacked = (double *)result;
		double *vec0_unpacked = (double *)vec0;
		double *vec1_unpacked = (double *)vec1;

		UBINT LoopCount = Length * sizeof(std::complex<double>) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment) / sizeof(std::complex<double>);
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			__m256d vec0_shuffle0 = _mm256_load_pd(vec0_unpacked);
			__m256d vec1_simd = _mm256_load_pd(vec1_unpacked);

			__m256d vec0_shuffle1 = _mm256_shuffle_pd(vec0_shuffle0, vec0_shuffle0, 0x5); //0xA = 0 1 0 1
			__m256d vec1_shuffle0 = _mm256_shuffle_pd(vec1_simd, vec1_simd, 0x0); //0x0 = 0 0 0 0
			__m256d vec1_shuffle1 = _mm256_shuffle_pd(vec1_simd, vec1_simd, 0xF); //0xF = 1 1 1 1

#if defined LIBENV_CPU_ACCEL_FMA
			__m256d res2 = _mm256_mul_pd(vec0_shuffle1, vec1_shuffle1);
			__m256d res1 = _mm256_fmaddsub_pd(vec0_shuffle0, vec1_shuffle0, res2);
			_mm256_store_pd(result_unpacked, _mm256_add_pd(_mm256_load_pd(result_unpacked), res1));
#else
			__m256d res1 = _mm256_mul_pd(vec0_shuffle0, vec1_shuffle0);
			__m256d res2 = _mm256_mul_pd(vec0_shuffle1, vec1_shuffle1);

			_mm256_store_pd(result_unpacked, _mm256_add_pd(_mm256_load_pd(result_unpacked), _mm256_addsub_pd(res1, res2)));
#endif
#elif defined LIBENV_CPU_ACCEL_SSE2
			__m128d vec0_shuffle0 = _mm_load_pd(vec0_unpacked);
			__m128d vec1_simd = _mm_load_pd(vec1_unpacked);

			__m128d vec0_shuffle1 = _mm_shuffle_pd(vec0_shuffle0, vec0_shuffle0, 0x1); //0x1 = 0 1
			__m128d vec1_shuffle0 = _mm_shuffle_pd(vec1_simd, vec1_simd, 0x0); //0x0 = 0 0
			__m128d vec1_shuffle1 = _mm_shuffle_pd(vec1_simd, vec1_simd, 0x3); //0xB1 = 1 1

			__m128d res1 = _mm_mul_pd(vec0_shuffle0, vec1_shuffle0);
			__m128d res2 = _mm_mul_pd(vec0_shuffle1, vec1_shuffle1);
#if defined LIBENV_CPU_ACCEL_SSE3
			_mm_store_pd(result_unpacked, _mm_add_pd(_mm_load_pd(result_unpacked), _mm_addsub_pd(res1, res2)));
#else
			const __m128d sign = { -1.0, 1.0 };

			res2 = _mm_mul_pd(res2, sign);
			_mm_store_pd(result_unpacked, _mm_add_pd(_mm_load_pd(result_unpacked), _mm_add_pd(res1, res2)));
#endif
#endif
			vec0_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			vec1_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			result_unpacked += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)((std::complex<double> *)result_unpacked)[i] += ((std::complex<double> *)vec0_unpacked)[i] * ((std::complex<double> *)vec1_unpacked)[i];
#endif
	}

	template <> extern void lerp_simd_aligned<float>(float *result, const float *vec0, const float *vec1, const float *vec2, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
			__m256 vec0_simd = _mm256_load_ps(vec0);
			_mm256_store_ps(result, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_load_ps(vec1), vec0_simd), _mm256_load_ps(vec2), vec0_simd));
#elif defined LIBENV_CPU_ACCEL_AVX
			__m256 vec0_simd = _mm256_load_ps(vec0);
			_mm256_store_ps(result, _mm256_add_ps(vec0_simd, _mm256_mul_ps(_mm256_sub_ps(_mm256_load_ps(vec1), vec0_simd),_mm256_load_ps(vec2))));
#elif defined LIBENV_CPU_ACCEL_SSE
			__m128 vec0_simd = _mm_load_ps(vec0);
			_mm_store_ps(result, _mm_add_ps(vec0_simd, _mm_mul_ps(_mm_sub_ps(_mm_load_ps(vec1), vec0_simd), _mm_load_ps(vec2))));
#endif
			vec0 += nsEnv::SIMDAlignment / sizeof(float);
			vec1 += nsEnv::SIMDAlignment / sizeof(float);
			vec2 += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
#endif
	}

	template <> extern void lerp_simd_aligned<double>(double *result, const double *vec0, const double *vec1, const double *vec2, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
#else
		UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
			__m256d vec0_simd = _mm256_load_pd(vec0);
			_mm256_store_pd(result, _mm256_fmadd_pd(_mm256_sub_pd(_mm256_load_pd(vec1), vec0_simd), _mm256_load_pd(vec2), vec0_simd));
#elif defined LIBENV_CPU_ACCEL_AVX
			__m256d vec0_simd = _mm256_load_pd(vec0);
			_mm256_store_pd(result, _mm256_add_pd(vec0_simd, _mm256_mul_pd(_mm256_sub_pd(_mm256_load_pd(vec1), vec0_simd),_mm256_load_pd(vec2))));
#elif defined LIBENV_CPU_ACCEL_SSE2
			__m128d vec0_simd = _mm_load_pd(vec0);
			_mm_store_pd(result, _mm_add_pd(vec0_simd, _mm_mul_pd(_mm_sub_pd(_mm_load_pd(vec1), vec0_simd), _mm_load_pd(vec2))));
#endif
			vec0 += nsEnv::SIMDAlignment / sizeof(double);
			vec1 += nsEnv::SIMDAlignment / sizeof(double);
			vec2 += nsEnv::SIMDAlignment / sizeof(double);
			result += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
#endif
	}

	template <> extern void mul_simd_unaligned<float>(float *result, const float *vec, const float scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec[i] * scl;
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec[i] * scl;
			result += Extra;
			vec += Extra;
			Length -= Extra;

			mul_simd_aligned(result, vec, scl, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
#if defined LIBENV_CPU_ACCEL_AVX
			__m256 scl_simd = _mm256_broadcast_ss(&scl);
#elif defined LIBENV_CPU_ACCEL_SSE
			__m128 scl_simd = _mm_set_ps1(scl);
#endif
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result, _mm256_mul_ps(_mm256_loadu_ps(vec), scl_simd));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_storeu_ps(result, _mm_mul_ps(_mm_loadu_ps(vec), scl_simd));
#endif
				vec += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = vec[i] * scl;
		}
#endif
	}

	template <> extern void mul_simd_unaligned<float>(float *result, const float *vec0, const float *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] * vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			mul_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result, _mm256_mul_ps(_mm256_loadu_ps(vec0), _mm256_loadu_ps(vec1)));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_storeu_ps(result, _mm_mul_ps(_mm_loadu_ps(vec0), _mm_loadu_ps(vec1)));
#endif
				vec0 += nsEnv::SIMDAlignment / sizeof(float);
				vec1 += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] * vec1[i];
		}
#endif
	}

	template <> extern void mul_simd_unaligned<double>(double *result, const double *vec, const double scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec[i] * scl;
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(double) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(double);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec[i] * scl;
			result += Extra;
			vec += Extra;
			Length -= Extra;

			mul_simd_aligned(result, vec, scl, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
#if defined LIBENV_CPU_ACCEL_AVX
			__m256d scl_simd = _mm256_broadcast_sd(&scl);
#elif defined LIBENV_CPU_ACCEL_SSE2
			__m128d scl_simd = _mm_set_pd(scl, scl);
#endif
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_pd(result, _mm256_mul_pd(_mm256_loadu_pd(vec), scl_simd));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_storeu_pd(result, _mm_mul_pd(_mm_loadu_pd(vec), scl_simd));
#endif
				vec += nsEnv::SIMDAlignment / sizeof(double);
				result += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = vec[i] * scl;
		}
#endif
	}

	template <> extern void mul_simd_unaligned<double>(double *result, const double *vec0, const double *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(double) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(double);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] * vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			mul_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_pd(result, _mm256_mul_pd(_mm256_loadu_pd(vec0), _mm256_loadu_pd(vec1)));
#elif defined LIBENV_CPU_ACCEL_SSE2
				_mm_storeu_pd(result, _mm_mul_pd(_mm_loadu_pd(vec0), _mm_loadu_pd(vec1)));
#endif
				vec0 += nsEnv::SIMDAlignment / sizeof(double);
				vec1 += nsEnv::SIMDAlignment / sizeof(double);
				result += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] * vec1[i];
		}
#endif
	}

	template <> extern void mul_simd_unaligned<std::complex<float>>(std::complex<float> *result, const std::complex<float> *vec0, const std::complex<float> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(std::complex<float>) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(std::complex<float>);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] * vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			mul_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			float *result_unpacked = (float *)result;
			float *vec0_unpacked = (float *)vec0;
			float *vec1_unpacked = (float *)vec1;

			UBINT LoopCount = Length * sizeof(std::complex<float>) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment) / sizeof(std::complex<float>);
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				__m256 vec0_shuffle0 = _mm256_loadu_ps(vec0_unpacked);
				__m256 vec1_simd = _mm256_loadu_ps(vec1_unpacked);

				__m256 vec0_shuffle1 = _mm256_shuffle_ps(vec0_shuffle0, vec0_shuffle0, 0xB1); //0xB1 = 10 11 00 01
				__m256 vec1_shuffle0 = _mm256_shuffle_ps(vec1_simd, vec1_simd, 0xA0); //0xA0 = 10 10 00 00
				__m256 vec1_shuffle1 = _mm256_shuffle_ps(vec1_simd, vec1_simd, 0xF5); //0xF5 = 11 11 01 01

#if defined LIBENV_CPU_ACCEL_FMA
				__m256 res2 = _mm256_mul_ps(vec0_shuffle1, vec1_shuffle1);
				__m256 res1 = _mm256_fmaddsub_ps(vec0_shuffle0, vec1_shuffle0, res2);
				_mm256_storeu_ps(result_unpacked, res1);
#else
				__m256 res1 = _mm256_mul_ps(vec0_shuffle0, vec1_shuffle0);
				__m256 res2 = _mm256_mul_ps(vec0_shuffle1, vec1_shuffle1);

				_mm256_storeu_ps(result_unpacked, _mm256_addsub_ps(res1, res2));
#endif
#elif defined LIBENV_CPU_ACCEL_SSE
				__m128 vec0_shuffle0 = _mm_loadu_ps(vec0_unpacked);
				__m128 vec1_simd = _mm_loadu_ps(vec1_unpacked);

				__m128 vec0_shuffle1 = _mm_shuffle_ps(vec0_shuffle0, vec0_shuffle0, 0xB1); //0xB1 = 10 11 00 01
				__m128 vec1_shuffle0 = _mm_shuffle_ps(vec1_simd, vec1_simd, 0xA0); //0xA0 = 10 10 00 00
				__m128 vec1_shuffle1 = _mm_shuffle_ps(vec1_simd, vec1_simd, 0xF5); //0xF5 = 11 11 01 01

				__m128 res1 = _mm_mul_ps(vec0_shuffle0, vec1_shuffle0);
				__m128 res2 = _mm_mul_ps(vec0_shuffle1, vec1_shuffle1);
#if defined LIBENV_CPU_ACCEL_SSE3
				_mm_storeu_ps(result_unpacked, _mm_addsub_ps(res1, res2));
#else
				const __m128 sign = { -1.0f, 1.0f, -1.0f, 1.0f };

				res2 = _mm_mul_ps(res2, sign);
				_mm_storeu_ps(result_unpacked, _mm_add_ps(res1, res2));
#endif
#endif
				vec0_unpacked += nsEnv::SIMDAlignment / sizeof(float);
				vec1_unpacked += nsEnv::SIMDAlignment / sizeof(float);
				result_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)((std::complex<float> *)result_unpacked)[i] = ((std::complex<float> *)vec0_unpacked)[i] * ((std::complex<float> *)vec1_unpacked)[i];
		}
#endif
	}

	template <> extern void mul_simd_unaligned<std::complex<double>>(std::complex<double> *result, const std::complex<double> *vec0, const std::complex<double> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] * vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(std::complex<double>) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(std::complex<double>);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] * vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			mul_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			double *result_unpacked = (double *)result;
			double *vec0_unpacked = (double *)vec0;
			double *vec1_unpacked = (double *)vec1;

			UBINT LoopCount = Length * sizeof(std::complex<double>) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment) / sizeof(std::complex<double>);
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				__m256d vec0_shuffle0 = _mm256_loadu_pd(vec0_unpacked);
				__m256d vec1_simd = _mm256_loadu_pd(vec1_unpacked);

				__m256d vec0_shuffle1 = _mm256_shuffle_pd(vec0_shuffle0, vec0_shuffle0, 0x5); //0xA = 0 1 0 1
				__m256d vec1_shuffle0 = _mm256_shuffle_pd(vec1_simd, vec1_simd, 0x0); //0x0 = 0 0 0 0
				__m256d vec1_shuffle1 = _mm256_shuffle_pd(vec1_simd, vec1_simd, 0xF); //0xF = 1 1 1 1

#if defined LIBENV_CPU_ACCEL_FMA
				__m256d res2 = _mm256_mul_pd(vec0_shuffle1, vec1_shuffle1);
				__m256d res1 = _mm256_fmaddsub_pd(vec0_shuffle0, vec1_shuffle0, res2);
				_mm256_storeu_pd(result_unpacked, res1);
#else
				__m256d res1 = _mm256_mul_pd(vec0_shuffle0, vec1_shuffle0);
				__m256d res2 = _mm256_mul_pd(vec0_shuffle1, vec1_shuffle1);

				_mm256_storeu_pd(result_unpacked, _mm256_addsub_pd(res1, res2));
#endif
#elif defined LIBENV_CPU_ACCEL_SSE2
				__m128d vec0_shuffle0 = _mm_loadu_pd(vec0_unpacked);
				__m128d vec1_simd = _mm_loadu_pd(vec1_unpacked);

				__m128d vec0_shuffle1 = _mm_shuffle_pd(vec0_shuffle0, vec0_shuffle0, 0x1); //0x2 = 0 1
				__m128d vec1_shuffle0 = _mm_shuffle_pd(vec1_simd, vec1_simd, 0x0); //0x0 = 0 0
				__m128d vec1_shuffle1 = _mm_shuffle_pd(vec1_simd, vec1_simd, 0x3); //0x3 = 1 1

				__m128d res1 = _mm_mul_pd(vec0_shuffle0, vec1_shuffle0);
				__m128d res2 = _mm_mul_pd(vec0_shuffle1, vec1_shuffle1);
#if defined LIBENV_CPU_ACCEL_SSE3
				_mm_storeu_pd(result_unpacked, _mm_addsub_pd(res1, res2));
#else
				const __m128d sign = { -1.0, 1.0 };

				res2 = _mm_mul_pd(res2, sign);
				_mm_storeu_pd(result_unpacked, _mm_add_pd(res1, res2));
#endif
#endif
				vec0_unpacked += nsEnv::SIMDAlignment / sizeof(double);
				vec1_unpacked += nsEnv::SIMDAlignment / sizeof(double);
				result_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)((std::complex<double> *)result_unpacked)[i] = ((std::complex<double> *)vec0_unpacked)[i] * ((std::complex<double> *)vec1_unpacked)[i];
		}
#endif
	}

	template <> extern void muladd_simd_unaligned<float>(float *result, const float *vec, const float scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec[i] * scl;
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] += vec[i] * scl;
			result += Extra;
			vec += Extra;
			Length -= Extra;

			muladd_simd_aligned(result, vec, scl, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
#if defined LIBENV_CPU_ACCEL_AVX
			__m256 scl_simd = _mm256_broadcast_ss(&scl);
#elif defined LIBENV_CPU_ACCEL_SSE
			__m128 scl_simd = _mm_set_ps1(scl);
#endif
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
				_mm256_storeu_ps(result, _mm256_fmadd_ps(_mm256_loadu_ps(vec), scl_simd, _mm256_loadu_ps(result)));
#elif defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result, _mm256_add_ps(_mm256_loadu_ps(result), _mm256_mul_ps(_mm256_loadu_ps(vec), scl_simd)));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_storeu_ps(result, _mm_add_ps(_mm_loadu_ps(result), _mm_mul_ps(_mm_loadu_ps(vec), scl_simd)));
#endif
				vec += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] += vec[i] * scl;
		}
#endif
	}

	template <> extern void muladd_simd_unaligned<float>(float *result, const float *vec0, const float *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] += vec0[i] * vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			muladd_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
				_mm256_storeu_ps(result, _mm256_fmadd_ps(_mm256_loadu_ps(vec0), _mm256_load_ps(vec1), _mm256_loadu_ps(result)));
#elif defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result, _mm256_add_ps(_mm256_loadu_ps(result), _mm256_mul_ps(_mm256_loadu_ps(vec0), _mm256_loadu_ps(vec1))));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_storeu_ps(result, _mm_add_ps(_mm_loadu_ps(result), _mm_mul_ps(_mm_loadu_ps(vec0), _mm_loadu_ps(vec1))));
#endif
				vec0 += nsEnv::SIMDAlignment / sizeof(float);
				vec1 += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] += vec0[i] * vec1[i];
		}
#endif
	}

	template <> extern void muladd_simd_unaligned<double>(double *result, const double *vec, const double scl, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec[i] * scl;
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(double) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(double);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] += vec[i] * scl;
			result += Extra;
			vec += Extra;
			Length -= Extra;

			muladd_simd_aligned(result, vec, scl, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
#if defined LIBENV_CPU_ACCEL_AVX
			__m256d scl_simd = _mm256_broadcast_sd(&scl);
#elif defined LIBENV_CPU_ACCEL_SSE2
			__m128d scl_simd = _mm_set_pd(scl, scl);
#endif
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
				_mm256_storeu_pd(result, _mm256_fmadd_pd(_mm256_loadu_pd(vec), scl_simd, _mm256_loadu_pd(result)));
#elif defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_pd(result, _mm256_add_pd(_mm256_loadu_pd(result), _mm256_mul_pd(_mm256_loadu_pd(vec), scl_simd)));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_storeu_pd(result, _mm_add_pd(_mm_loadu_pd(result), _mm_mul_pd(_mm_loadu_pd(vec), scl_simd)));
#endif
				vec += nsEnv::SIMDAlignment / sizeof(double);
				result += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] += vec[i] * scl;
		}
#endif
	}

	template <> extern void muladd_simd_unaligned<double>(double *result, const double *vec0, const double *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(double) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(double);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] += vec0[i] * vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			muladd_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
				_mm256_storeu_pd(result, _mm256_fmadd_pd(_mm256_loadu_pd(vec0), _mm256_load_pd(vec1), _mm256_loadu_pd(result)));
#elif defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_pd(result, _mm256_add_pd(_mm256_loadu_pd(result), _mm256_mul_pd(_mm256_loadu_pd(vec0), _mm256_loadu_pd(vec1))));
#elif defined LIBENV_CPU_ACCEL_SSE2
				_mm_storeu_pd(result, _mm_add_pd(_mm_loadu_pd(result), _mm_mul_pd(_mm_loadu_pd(vec0), _mm_loadu_pd(vec1))));
#endif
				vec0 += nsEnv::SIMDAlignment / sizeof(double);
				vec1 += nsEnv::SIMDAlignment / sizeof(double);
				result += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] += vec0[i] * vec1[i];
		}
#endif
	}

	template <> extern void muladd_simd_unaligned<std::complex<float>>(std::complex<float> *result, const std::complex<float> *vec0, const std::complex<float> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(std::complex<float>) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(std::complex<float>);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] += vec0[i] * vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			muladd_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			float *result_unpacked = (float *)result;
			float *vec0_unpacked = (float *)vec0;
			float *vec1_unpacked = (float *)vec1;

			UBINT LoopCount = Length * sizeof(std::complex<float>) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment) / sizeof(std::complex<float>);
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				__m256 vec0_shuffle0 = _mm256_loadu_ps(vec0_unpacked);
				__m256 vec1_simd = _mm256_loadu_ps(vec1_unpacked);

				__m256 vec0_shuffle1 = _mm256_shuffle_ps(vec0_shuffle0, vec0_shuffle0, 0xB1); //0xB1 = 10 11 00 01
				__m256 vec1_shuffle0 = _mm256_shuffle_ps(vec1_simd, vec1_simd, 0xA0); //0xA0 = 10 10 00 00
				__m256 vec1_shuffle1 = _mm256_shuffle_ps(vec1_simd, vec1_simd, 0xF5); //0xF5 = 11 11 01 01

#if defined LIBENV_CPU_ACCEL_FMA
				__m256 res2 = _mm256_mul_ps(vec0_shuffle1, vec1_shuffle1);
				__m256 res1 = _mm256_fmaddsub_ps(vec0_shuffle0, vec1_shuffle0, res2);
				_mm256_storeu_ps(result_unpacked, _mm256_add_ps(_mm256_loadu_ps(result_unpacked), res1));
#else
				__m256 res1 = _mm256_mul_ps(vec0_shuffle0, vec1_shuffle0);
				__m256 res2 = _mm256_mul_ps(vec0_shuffle1, vec1_shuffle1);

				_mm256_storeu_ps(result_unpacked, _mm256_add_ps(_mm256_loadu_ps(result_unpacked), _mm256_addsub_ps(res1, res2)));
#endif
#elif defined LIBENV_CPU_ACCEL_SSE
				__m128 vec0_shuffle0 = _mm_loadu_ps(vec0_unpacked);
				__m128 vec1_simd = _mm_loadu_ps(vec1_unpacked);

				__m128 vec0_shuffle1 = _mm_shuffle_ps(vec0_shuffle0, vec0_shuffle0, 0xB1); //0xB1 = 10 11 00 01
				__m128 vec1_shuffle0 = _mm_shuffle_ps(vec1_simd, vec1_simd, 0xA0); //0xA0 = 10 10 00 00
				__m128 vec1_shuffle1 = _mm_shuffle_ps(vec1_simd, vec1_simd, 0xF5); //0xF5 = 11 11 01 01

				__m128 res1 = _mm_mul_ps(vec0_shuffle0, vec1_shuffle0);
				__m128 res2 = _mm_mul_ps(vec0_shuffle1, vec1_shuffle1);
#if defined LIBENV_CPU_ACCEL_SSE3
				_mm_storeu_ps(result_unpacked, _mm_add_ps(_mm_loadu_ps(result_unpacked), _mm_addsub_ps(res1, res2)));
#else
				const __m128 sign = { -1.0f, 1.0f, -1.0f, 1.0f };

				res2 = _mm_mul_ps(res2, sign);
				_mm_storeu_ps(result_unpacked, _mm_add_ps(_mm_loadu_ps(result_unpacked), _mm_add_ps(res1, res2)));
#endif
#endif
				vec0_unpacked += nsEnv::SIMDAlignment / sizeof(float);
				vec1_unpacked += nsEnv::SIMDAlignment / sizeof(float);
				result_unpacked += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)((std::complex<float> *)result_unpacked)[i] += ((std::complex<float> *)vec0_unpacked)[i] * ((std::complex<float> *)vec1_unpacked)[i];
		}
#endif
	}

	template <> extern void muladd_simd_unaligned<std::complex<double>>(std::complex<double> *result, const std::complex<double> *vec0, const std::complex<double> *vec1, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] += vec0[i] * vec1[i];
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(std::complex<double>) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(std::complex<double>);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] += vec0[i] * vec1[i];
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			Length -= Extra;

			muladd_simd_aligned(result, vec0, vec1, Length);
		}
		else{
			//unaligned
			double *result_unpacked = (double *)result;
			double *vec0_unpacked = (double *)vec0;
			double *vec1_unpacked = (double *)vec1;

			UBINT LoopCount = Length * sizeof(std::complex<double>) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment) / sizeof(std::complex<double>);
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				__m256d vec0_shuffle0 = _mm256_loadu_pd(vec0_unpacked);
				__m256d vec1_simd = _mm256_loadu_pd(vec1_unpacked);

				__m256d vec0_shuffle1 = _mm256_shuffle_pd(vec0_shuffle0, vec0_shuffle0, 0x5); //0xA = 0 1 0 1
				__m256d vec1_shuffle0 = _mm256_shuffle_pd(vec1_simd, vec1_simd, 0x0); //0x0 = 0 0 0 0
				__m256d vec1_shuffle1 = _mm256_shuffle_pd(vec1_simd, vec1_simd, 0xF); //0xF = 1 1 1 1

#if defined LIBENV_CPU_ACCEL_FMA
				__m256d res2 = _mm256_mul_pd(vec0_shuffle1, vec1_shuffle1);
				__m256d res1 = _mm256_fmaddsub_pd(vec0_shuffle0, vec1_shuffle0, res2);
				_mm256_storeu_pd(result_unpacked, _mm256_add_pd(_mm256_loadu_pd(result_unpacked), res1));
#else
				__m256d res1 = _mm256_mul_pd(vec0_shuffle0, vec1_shuffle0);
				__m256d res2 = _mm256_mul_pd(vec0_shuffle1, vec1_shuffle1);

				_mm256_storeu_pd(result_unpacked, _mm256_add_pd(_mm256_loadu_pd(result_unpacked), _mm256_addsub_pd(res1, res2)));
#endif
#elif defined LIBENV_CPU_ACCEL_SSE2
				__m128d vec0_shuffle0 = _mm_loadu_pd(vec0_unpacked);
				__m128d vec1_simd = _mm_loadu_pd(vec1_unpacked);

				__m128d vec0_shuffle1 = _mm_shuffle_pd(vec0_shuffle0, vec0_shuffle0, 0x1); //0x2 = 0 1
				__m128d vec1_shuffle0 = _mm_shuffle_pd(vec1_simd, vec1_simd, 0x0); //0x0 = 0 0
				__m128d vec1_shuffle1 = _mm_shuffle_pd(vec1_simd, vec1_simd, 0x3); //0x3 = 1 1

				__m128d res1 = _mm_mul_pd(vec0_shuffle0, vec1_shuffle0);
				__m128d res2 = _mm_mul_pd(vec0_shuffle1, vec1_shuffle1);
#if defined LIBENV_CPU_ACCEL_SSE3
				_mm_storeu_pd(result_unpacked, _mm_add_pd(_mm_loadu_pd(result_unpacked), _mm_addsub_pd(res1, res2)));
#else
				const __m128d sign = { -1.0, 1.0 };

				res2 = _mm_mul_pd(res2, sign);
				_mm_storeu_pd(result_unpacked, _mm_add_pd(_mm_loadu_pd(result_unpacked), _mm_add_pd(res1, res2)));
#endif
#endif
				vec0_unpacked += nsEnv::SIMDAlignment / sizeof(double);
				vec1_unpacked += nsEnv::SIMDAlignment / sizeof(double);
				result_unpacked += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)((std::complex<double> *)result_unpacked)[i] += ((std::complex<double> *)vec0_unpacked)[i] * ((std::complex<double> *)vec1_unpacked)[i];
		}
#endif
	}

	template <> extern void lerp_simd_unaligned<float>(float *result, const float *vec0, const float *vec1, const float *vec2, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec2)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			vec2 += Extra;
			Length -= Extra;

			lerp_simd_aligned(result, vec0, vec1, vec2, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
				__m256 vec0_simd = _mm256_load_ps(vec0);
				_mm256_storeu_ps(result, _mm256_fmadd_ps(_mm256_sub_ps(_mm256_loadu_ps(vec1), vec0_simd), _mm256_loadu_ps(vec2), vec0_simd));
#elif defined LIBENV_CPU_ACCEL_AVX
				__m256 vec0_simd = _mm256_loadu_ps(vec0);
				_mm256_storeu_ps(result, _mm256_add_ps(vec0_simd, _mm256_mul_ps(_mm256_sub_ps(_mm256_loadu_ps(vec1), vec0_simd),_mm256_loadu_ps(vec2))));
#elif defined LIBENV_CPU_ACCEL_SSE
				__m128 vec0_simd = _mm_loadu_ps(vec0);
				_mm_storeu_ps(result, _mm_add_ps(vec0_simd, _mm_mul_ps(_mm_sub_ps(_mm_loadu_ps(vec1), vec0_simd), _mm_loadu_ps(vec2))));
#endif
				vec0 += nsEnv::SIMDAlignment / sizeof(float);
				vec1 += nsEnv::SIMDAlignment / sizeof(float);
				vec2 += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
		}
#endif
	}

	template <> extern void lerp_simd_unaligned<double>(double *result, const double *vec0, const double *vec1, const double *vec2, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(double) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec0) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec1) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec2)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(double);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
			result += Extra;
			vec0 += Extra;
			vec1 += Extra;
			vec2 += Extra;
			Length -= Extra;

			lerp_simd_aligned(result, vec0, vec1, vec2, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_FMA
				__m256d vec0_simd = _mm256_load_pd(vec0);
				_mm256_storeu_pd(result, _mm256_fmadd_pd(_mm256_sub_pd(_mm256_loadu_pd(vec1), vec0_simd), _mm256_loadu_pd(vec2), vec0_simd));
#elif defined LIBENV_CPU_ACCEL_AVX
				__m256d vec0_simd = _mm256_loadu_pd(vec0);
				_mm256_storeu_pd(result, _mm256_add_pd(vec0_simd, _mm256_mul_pd(_mm256_sub_pd(_mm256_loadu_pd(vec1), vec0_simd),_mm256_loadu_pd(vec2))));
#elif defined LIBENV_CPU_ACCEL_SSE2
				__m128d vec0_simd = _mm_loadu_pd(vec0);
				_mm_storeu_pd(result, _mm_add_pd(vec0_simd, _mm_mul_pd(_mm_sub_pd(_mm_loadu_pd(vec1), vec0_simd), _mm_loadu_pd(vec2))));
#endif
				vec0 += nsEnv::SIMDAlignment / sizeof(double);
				vec1 += nsEnv::SIMDAlignment / sizeof(double);
				vec2 += nsEnv::SIMDAlignment / sizeof(double);
				result += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = vec0[i] + vec2[i] * (vec1[i] - vec0[i]);
		}
#endif
	}

	template <> extern void sqrt_simd_aligned<float>(float *result, const float *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = sqrt(vec[i]);
#else
		UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_ps(result, _mm256_sqrt_ps(_mm256_load_ps(vec)));
#elif defined LIBENV_CPU_ACCEL_SSE
			_mm_store_ps(result, _mm_sqrt_ps(_mm_load_ps(vec)));
#endif
			vec += nsEnv::SIMDAlignment / sizeof(float);
			result += nsEnv::SIMDAlignment / sizeof(float);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = sqrt(vec[i]);
#endif
	}

	template <> extern void sqrt_simd_aligned<double>(double *result, const double *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = sqrt(vec[i]);
#else
		UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
		UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
		for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
			_mm256_store_pd(result, _mm256_sqrt_pd(_mm256_load_pd(vec)));
#elif defined LIBENV_CPU_ACCEL_SSE2
			_mm_store_pd(result, _mm_sqrt_pd(_mm_load_pd(vec)));
#endif
			vec += nsEnv::SIMDAlignment / sizeof(double);
			result += nsEnv::SIMDAlignment / sizeof(double);
		}
		for (UBINT i = 0; i < Remainder; i++)result[i] = sqrt(vec[i]);
#endif
	}

	template <> extern void sqrt_simd_unaligned<float>(float *result, const float *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = sqrt(vec[i]);
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(float) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(float);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = sqrt(vec[i]);
			result += Extra;
			vec += Extra;
			Length -= Extra;

			sqrt_simd_aligned(result, vec, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(float) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(float));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_ps(result, _mm256_sqrt_ps(_mm256_loadu_ps(vec)));
#elif defined LIBENV_CPU_ACCEL_SSE
				_mm_storeu_ps(result, _mm_sqrt_ps(_mm_loadu_ps(vec)));
#endif
				vec += nsEnv::SIMDAlignment / sizeof(float);
				result += nsEnv::SIMDAlignment / sizeof(float);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = sqrt(vec[i]);
		}
#endif
	}

	template <> extern void sqrt_simd_unaligned<double>(double *result, const double *vec, UBINT Length){
#if defined LIBENV_CPU_ACCEL_NONE || (!defined LIBENV_CPU_ACCEL_SSE2)
		//fallback
		for (UBINT i = 0; i < Length; i++)result[i] = sqrt(vec[i]);
#else
		//check for alignment
		//suppose that nsEnv::SIMDAlignment is a power of 2
		UBINT ExtraBytes = ((nsEnv::SIMDAlignment - 1) & (UBINT)result);
		if (0 == ExtraBytes % sizeof(double) && ExtraBytes == ((nsEnv::SIMDAlignment - 1) & (UBINT)vec)){
			//aligned
			UBINT Extra = (nsEnv::SIMDAlignment - ExtraBytes) / sizeof(double);
			if (Extra > Length)Extra = Length;
			for (UBINT i = 0; i < Extra; i++)result[i] = sqrt(vec[i]);
			result += Extra;
			vec += Extra;
			Length -= Extra;

			sqrt_simd_aligned(result, vec, Length);
		}
		else{
			//unaligned
			UBINT LoopCount = Length * sizeof(double) / nsEnv::SIMDAlignment;
			UBINT Remainder = Length - LoopCount * (nsEnv::SIMDAlignment / sizeof(double));
			for (UBINT i = 0; i < LoopCount; i++){
#if defined LIBENV_CPU_ACCEL_AVX
				_mm256_storeu_pd(result, _mm256_sqrt_pd(_mm256_loadu_pd(vec)));
#elif defined LIBENV_CPU_ACCEL_SSE2
				_mm_storeu_pd(result, _mm_sqrt_pd(_mm_loadu_pd(vec)));
#endif
				vec += nsEnv::SIMDAlignment / sizeof(double);
				result += nsEnv::SIMDAlignment / sizeof(double);
			}
			for (UBINT i = 0; i < Remainder; i++)result[i] = sqrt(vec[i]);
		}
#endif
	}
}
#endif