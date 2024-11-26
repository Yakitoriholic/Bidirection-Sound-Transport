/* Description:Platform-independent mathematical functions for random number generation.
* Language:C++
* Author:***
*/

#ifndef LIB_MATH_RAND
#define LIB_MATH_RAND

#include "lMath.hpp"

namespace nsMath{
	
	class RandGenerator{
	protected:
		typedef RandGenerator this_type;

		struct Interface{
			UBINT _Min;
			UBINT _Max;
			UBINT(*_Rand)(this_type * const);
			void(*_Destroy)(this_type * const);
		};

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); }

		const Interface *lpInterface;
	public:
		inline UBINT operator()(){ return lpInterface->_Rand(this); }
		inline void discard(){ lpInterface->_Rand(this); }
		inline UBINT min() const{ return lpInterface->_Min; }
		inline UBINT max() const{ return lpInterface->_Max; }
		inline ~RandGenerator(){ lpInterface->_Destroy(this); }
	};

	class RandGenerator_Fake : public RandGenerator{
	private:
		static const RandGenerator::Interface _I;

		static UBINT _Rand(RandGenerator * const ThisBase);
		static void _Destroy(RandGenerator * const ThisBase){};
	public:
		UBINT Seed;
		inline RandGenerator_Fake(){ this->Seed = 0; this->lpInterface = &_I; }
		inline RandGenerator_Fake(UBINT Seed){ this->Seed = Seed; this->lpInterface = &_I; }
	};

	const RandGenerator::Interface RandGenerator_Fake::_I{
		nsMath::NumericTrait<UBINT>::Min,
		nsMath::NumericTrait<UBINT>::Max,
		RandGenerator_Fake::_Rand,
		RandGenerator_Fake::_Destroy
	};

	class RandGenerator_LCG : public RandGenerator{
		//Notice that using LCG algorithm to generate high dimension vectors with consequentially generated numbers will degenerate terribly (see Marsaglia's Theorem).
	private:
		static const RandGenerator::Interface _I;

		static UBINT _Rand(RandGenerator * const ThisBase);
		static void _Destroy(RandGenerator * const ThisBase){};
	public:
		UBINT Seed;
		inline RandGenerator_LCG(){ this->Seed = 0; this->lpInterface = &_I; }
		inline RandGenerator_LCG(UBINT Seed){ this->Seed = Seed; this->lpInterface = &_I; }
	};

	const RandGenerator::Interface RandGenerator_LCG::_I{
		nsMath::NumericTrait<UBINT>::Min,
		nsMath::NumericTrait<UBINT>::Max,
		RandGenerator_LCG::_Rand,
		RandGenerator_LCG::_Destroy
	};

	class RandGenerator_XORShiftPlus : public RandGenerator{
	private:
		static const RandGenerator::Interface _I;

		static UBINT _Rand(RandGenerator * const ThisBase);
		static void _Destroy(RandGenerator * const ThisBase){};
	public:
		UBINT Seed[2];
#if defined LIBENV_SYS_INTELX64
		inline RandGenerator_XORShiftPlus(){ this->Seed[0] = 0; this->Seed[1] = 0x14057B7EF767814F; this->lpInterface = &_I; }
#elif defined LIBENV_SYS_INTELX86
		inline RandGenerator_XORShiftPlus(){ this->Seed[0] = 0; this->Seed[1] = 0xC39EC3; this->lpInterface = &_I; }
#endif
		inline RandGenerator_XORShiftPlus(UBINT Seed0, UBINT Seed1){ this->Seed[0] = Seed0; this->Seed[1] = Seed1; this->lpInterface = &_I; }
	};

	const RandGenerator::Interface RandGenerator_XORShiftPlus::_I{
		1,
		nsMath::NumericTrait<UBINT>::Max,
		RandGenerator_XORShiftPlus::_Rand,
		RandGenerator_XORShiftPlus::_Destroy
	};

	template <typename T> T RandFloat(UBINT *Seed);
	template <typename T> T RandFloat(RandGenerator *lpRNG);
	//generating float point numbers uniformly distributed in [0 1].

	template <typename T> void Distribution_Uniform_Spherical(T *result, UBINT *Seed);
	template <typename T> void Distribution_Uniform_Spherical(T *result, RandGenerator *lpRNG);
	//generating 3D points uniformly distributed on a sphere of radius 1.

	template <typename T> void Distribution_Uniform_Hemispherical(T *result, UBINT *Seed);
	template <typename T> void Uniform_Hemispherical_Distribution(T *result, RandGenerator *lpRNG);
	//generating 3D points uniformly distributed on a hemisphere (with [1 0 0] at the top) of radius 1.

	template <typename T> void Distribution_CosWeighted_Hemispherical(T *result, UBINT *Seed);
	template <typename T> void Distribution_CosWeighted_Hemispherical(T *result, RandGenerator *lpRNG);
	//generating 3D points distributed on a hemisphere (with [1 0 0] at the top) of radius 1. The PDF of the distribution is cos(result[0]) / M_PI.

	template <typename T> void Distribution_Power_CosWeighted_Hemispherical(T *result, T power, UBINT *Seed);
	template <typename T> void Distribution_Power_CosWeighted_Hemispherical(T *result, T power, RandGenerator *lpRNG);
	//generating 3D points distributed on a hemisphere (with [1 0 0] at the top) of radius 1. The PDF of the distribution is ([power] + 1) * cos(result[0]) ^ [power] / (2 * M_PI).

	template <typename T> void Distribution_Uniform_Disk(T *result, UBINT *Seed);
	template <typename T> void Distribution_Uniform_Disk(T *result, RandGenerator *lpRNG);
	//generating 2D points uniformly distributed inside a disk of radius 1.

	template <typename T> void Random_Vertical_Vector(T *result, const T *vec, UBINT *Seed);
	template <typename T> void Random_Vertical_Vector(T *result, const T *vec, RandGenerator *lpRNG);
	//generating normalized random 3D vectors perpendicular to [vec].

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	UBINT RandGenerator_Fake::_Rand(RandGenerator * const ThisBase){
		RandGenerator_Fake *This = static_cast<RandGenerator_Fake *>(ThisBase);
		return This->Seed;
	}

	UBINT RandGenerator_LCG::_Rand(RandGenerator * const ThisBase){
		RandGenerator_LCG *This = static_cast<RandGenerator_LCG *>(ThisBase);
		return nsMath::rand_LCG(&This->Seed);
	}

	UBINT RandGenerator_XORShiftPlus::_Rand(RandGenerator * const ThisBase){
		RandGenerator_XORShiftPlus *This = static_cast<RandGenerator_XORShiftPlus *>(ThisBase);

		UBINT s1 = This->Seed[0];
		const UBINT s0 = This->Seed[1];
		This->Seed[0] = s0;
#if defined LIBENV_SYS_INTELX64
		s1 ^= s1 << 23;
		s1 ^= s1 >> 17;
		s1 ^= s0 ^ (s0 >> 26);
#elif defined LIBENV_SYS_INTELX86
		s1 ^= s1 << 16;
		s1 ^= s1 >> 11;
		s1 ^= s0 ^ (s0 >> 19);
#endif
		This->Seed[1] = s1;
		return s0 + s1;
	}
	template <typename T> inline T RandFloat(UBINT *Seed){ return (T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX; }
	template <typename T> inline T RandFloat(RandGenerator *lpRNG){ return (T)((*lpRNG)() - lpRNG->min()) / (T)(lpRNG->max() - lpRNG->min()); }

	template <typename T> void Distribution_Uniform_Spherical(T *result, UBINT *Seed){
		float theta = (T)2.0 * (T)M_PI * (T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX;
		result[0] = (T)2.0 * ((T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX) - (T)1.0;
		result[1] = sqrt(1 - result[0] * result[0])*cos(theta);
		result[2] = sqrt(1 - result[0] * result[0])*sin(theta);
	}
	template <typename T> void Distribution_Uniform_Spherical(T *result, RandGenerator *lpRNG){
		float theta = (T)2.0 * (T)M_PI * RandFloat<T>(lpRNG);
		result[0] = (T)2.0 * RandFloat<T>(lpRNG) -(T)1.0;
		result[1] = sqrt(1 - result[0] * result[0])*cos(theta);
		result[2] = sqrt(1 - result[0] * result[0])*sin(theta);
	}
	template <typename T> void Distribution_Uniform_Hemispherical(T *result, UBINT *Seed){
		float theta = (T)2.0 * (T)M_PI *(T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX;
		result[0] = ((T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX);
		result[1] = sqrt(1 - result[0] * result[0])*cos(theta);
		result[2] = sqrt(1 - result[0] * result[0])*sin(theta);
	}
	template <typename T> void Distribution_Uniform_Hemispherical(T *result, RandGenerator *lpRNG){
		float theta = (T)2.0 * (T)M_PI * RandFloat<T>(lpRNG);
		result[0] = RandFloat<T>(lpRNG);
		result[1] = sqrt(1 - result[0] * result[0])*cos(theta);
		result[2] = sqrt(1 - result[0] * result[0])*sin(theta);
	}
	template <typename T> void Distribution_CosWeighted_Hemispherical(T *result, UBINT *Seed){
		float theta = (T)2.0 * (T)M_PI *(T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX;
		float xsqr = (T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX;
		result[0] = sqrt(xsqr);

		float r = sqrt(1 - xsqr);
		result[1] = r * cos(theta);
		result[2] = r * sin(theta);
	}
	template <typename T> void Distribution_CosWeighted_Hemispherical(T *result, RandGenerator *lpRNG){
		float theta = (T)2.0 * (T)M_PI * RandFloat<T>(lpRNG);
		float xsqr = RandFloat<T>(lpRNG);
		result[0] = sqrt(xsqr);

		float r = sqrt(1 - xsqr);
		result[1] = r * cos(theta);
		result[2] = r * sin(theta);
	}
	template <typename T> void Distribution_Power_CosWeighted_Hemispherical(T *result, T power, UBINT *Seed){
		float theta = (T)2.0 * (T)M_PI *(T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX;
		float xsqr = exp(log(RandFloat<T>(lpRNG)) - (power + (T)1.0));
		result[0] = sqrt(xsqr);

		float r = sqrt(1 - xsqr);
		result[1] = r * cos(theta);
		result[2] = r * sin(theta);
	}
	template <typename T> void Distribution_Power_CosWeighted_Hemispherical(T *result, T power, RandGenerator *lpRNG){
		float theta = (T)2.0 * (T)M_PI * RandFloat<T>(lpRNG);
		float xsqr = exp(log(RandFloat<T>(lpRNG)) - (power + (T)1.0));
		result[0] = sqrt(xsqr);

		float r = sqrt(1 - xsqr);
		result[1] = r * cos(theta);
		result[2] = r * sin(theta);
	}
	template <typename T> void Distribution_Uniform_Disk(T *result, UBINT *Seed){
		float theta = (T)2.0 * (T)M_PI *(T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX;
		float r = sqrt((T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX);
		result[0] = r * cos(theta);
		result[1] = r * sin(theta);
	}
	template <typename T> void Distribution_Uniform_Disk(T *result, RandGenerator *lpRNG){
		float theta = (T)2.0 * (T)M_PI * RandFloat<T>(lpRNG);
		float r = sqrt(RandFloat<T>(lpRNG));
		result[0] = r * cos(theta);
		result[1] = r * sin(theta);
	}
	template <typename T> void Random_Vertical_Vector(T *result, const T *vec, UBINT *Seed){
		T XDir[3], YDir[3];
		nsMath::vert3(XDir, vec);
		nsMath::cross3(YDir, vec, XDir);

		T Dist = sqrt(nsMath::dot<3>(XDir, XDir));
		XDir[0] /= Dist; XDir[1] /= Dist; XDir[2] /= Dist;
		Dist = sqrt(nsMath::dot<3>(YDir, YDir));
		YDir[0] /= Dist; YDir[1] /= Dist; YDir[2] /= Dist;

		T theta = (T)2.0 * (T)M_PI *(T)nsMath::rand_LCG(Seed) / (T)UBINT_MAX;
		XDir[0] *= cos(theta); XDir[1] *= cos(theta); XDir[2] *= cos(theta);
		YDir[0] *= sin(theta); YDir[1] *= sin(theta); YDir[2] *= sin(theta);
		result[0] = XDir[0] + YDir[0]; result[1] = XDir[1] + YDir[1]; result[2] = XDir[2] + YDir[2];
	}
	template <typename T> void Random_Vertical_Vector(T *result, const T *vec, RandGenerator *lpRNG){
		T XDir[3], YDir[3];
		nsMath::vert3(XDir, vec);
		nsMath::cross3(YDir, vec, XDir);

		T Dist = sqrt(nsMath::dot<3>(XDir, XDir));
		XDir[0] /= Dist; XDir[1] /= Dist; XDir[2] /= Dist;
		Dist = sqrt(nsMath::dot<3>(YDir, YDir));
		YDir[0] /= Dist; YDir[1] /= Dist; YDir[2] /= Dist;

		T theta = (T)2.0 * (T)M_PI * RandFloat<T>(lpRNG);
		XDir[0] *= cos(theta); XDir[1] *= cos(theta); XDir[2] *= cos(theta);
		YDir[0] *= sin(theta); YDir[1] *= sin(theta); YDir[2] *= sin(theta);
		result[0] = XDir[0] + YDir[0]; result[1] = XDir[1] + YDir[1]; result[2] = XDir[2] + YDir[2];
	}
}
#endif