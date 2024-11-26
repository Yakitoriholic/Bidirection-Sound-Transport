/* Description: Headers for special ISAs on certain platforms (like Intel). DO NOT include this header directly.
 * Language:C++
 * Author:***
 *
 * LIBENV_CPU_ACCEL_MMX: use Intel's MMX instruction sets.
 *                     when using GCC, the compiler version should be at least 3.11 and option "-march=pentium-mmx/k6(or higher) -mmmx" is required.
 * LIBENV_CPU_ACCEL_SSE: use Intel's SSE instruction sets.
 *                     when using GCC, the compiler version should be at least 3.11 and option "-march=pentium3/athlon-4(or higher) -msse" is required.
 * LIBENV_CPU_ACCEL_SSE2: use Intel's SSE2 instruction sets.
 *                     when using GCC, the compiler version should be at least 3.11 and option "-march=pentium-m/pentium4/k8(or higher) -msse2" is required.
 * LIB_CPU_ACCEL_AVX: use Intel's AVX instruction sets.
 *                     when using GCC, the compiler version should be at least 4.6 and option "-march=corei7-avx(or higher) -mavx" is required.
 * LIB_CPU_ACCEL_BMI2: use Intel's BMI2 instruction sets.
 *                     when using GCC, the compiler version should be at least 4.7 and option "-march=haswell/bdver2(or higher) -mbmi2" is required.
 *
 */

#ifndef LIB_GENERAL_CPU_ISA
#define LIB_GENERAL_CPU_ISA

#if defined LIBENV_SYS_INTELX64
#define LIBENV_CPU_ACCEL_SSE2
#endif

#ifdef LIBENV_CPU_ACCEL_BMI2
#define LIBENV_CPU_ACCEL_AVX
#endif

#ifdef LIBENV_CPU_ACCEL_FMA
#define LIBENV_CPU_ACCEL_AVX
#endif

#ifdef LIBENV_CPU_ACCEL_AVX
#include "immintrin.h"
#define LIBENV_CPU_ACCEL_SSE41
#endif

#ifdef LIBENV_CPU_ACCEL_SSE41
#include "smmintrin.h"
#define LIBENV_CPU_ACCEL_SSSE3
#endif

#ifdef LIBENV_CPU_ACCEL_SSSE3
#include "tmmintrin.h"
#define LIBENV_CPU_ACCEL_SSE3
#endif

#ifdef LIBENV_CPU_ACCEL_SSE3
#include "pmmintrin.h"
#define LIBENV_CPU_ACCEL_SSE2
#endif

#ifdef LIBENV_CPU_ACCEL_SSE2
#include "emmintrin.h"
#define LIBENV_CPU_ACCEL_SSE
#endif

#ifdef LIBENV_CPU_ACCEL_SSE
#include "xmmintrin.h"
#define LIBENV_CPU_ACCEL_MMX
#endif

#ifdef LIBENV_CPU_ACCEL_MMX
#include "mmintrin.h"
#else
#define LIBENV_CPU_ACCEL_NONE
#endif

namespace nsEnv{
#if defined LIBENV_CPU_ACCEL_BMI2
	const unsigned char SIMDAlignment = 32;
#elif defined LIBENV_CPU_ACCEL_FMA
	const unsigned char SIMDAlignment = 32;
#elif defined LIBENV_CPU_ACCEL_AVX
	const unsigned char SIMDAlignment = 32;
#elif defined LIBENV_CPU_ACCEL_SSE41
	const unsigned char SIMDAlignment = 16;
#elif defined LIBENV_CPU_ACCEL_SSSE3
	const unsigned char SIMDAlignment = 16;
#elif defined LIBENV_CPU_ACCEL_SSE3
	const unsigned char SIMDAlignment = 16;
#elif defined LIBENV_CPU_ACCEL_SSE2
	const unsigned char SIMDAlignment = 16;
#elif defined LIBENV_CPU_ACCEL_SSE
	const unsigned char SIMDAlignment = 16;
#elif defined LIBENV_CPU_ACCEL_MMX
	const unsigned char SIMDAlignment = 8;
#elif defined LIBENV_SYS_INTELX86
	const unsigned char SIMDAlignment = 4;
#endif
}

#endif