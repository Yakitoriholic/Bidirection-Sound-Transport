/* Description: Hash map class modelled after STL. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_MULTIDIMINDEX
#define LIB_CONTAINER_MULTIDIMINDEX

namespace nsContainer{
	template < typename Dim_Type, typename T, UBINT Dim_Count > struct PointMap{
		const nsMath::TinyVector<Dim_Type, Dim_Count> Position;
		T Data;
	};

	template <typename T> extern T _roundfloat(T x, T span){

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
#endif
		// This function returns the nearest value to [x] which equals [span] times an odd integer.
		// [span]: a float number which is either power of 2 or positive infinity.
		// This function returns a wrong result when it doesn't exist. It is recommended to use [span] returned by roundfloat2().

		// x should not be -0.0, NaN or -NaN.

		typedef typename nsMath::NumericTrait<T>::uint_type uint_type;
		const uint_type Exponent = nsMath::NumericTrait<T>::Exponent_Mask;

		uint_type a = *(uint_type *)&x, b = *(uint_type *)&span; // float hacking
		if (a > nsMath::NumericTrait<T>::HighestBit)a--; // special case for negative integers
		if (b >= Exponent)a = 0; // return 0 when [span] is infinity
		else if ((a&(nsMath::NumericTrait<T>::HighestBit - 1)) <= b)a = (a&nsMath::NumericTrait<T>::HighestBit) + b;
		else{
			if (b > nsMath::NumericTrait<T>::Mantissa_Mask){ // not a denormalized number
				b = ((a&(nsMath::NumericTrait<T>::HighestBit - 1)) - b) >> nsMath::NumericTrait<T>::Mantissa_Bit_Count;
				b = nsMath::NumericTrait<T>::Mantissa_Mask >> b;
			}
			a = a&(~b);
			if (b < nsMath::NumericTrait<T>::Mantissa_Mask)a |= (b + 1);
		}
		return *(T *)&a;
	}

	template <typename T> extern T _roundint(T x, T span){

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
#endif
		// This function returns the nearest value to [x] which equals [span] times an odd integer.
		// [span]: an non-zero integer which is power of 2.

		return (x & (~((span << 1) - 1))) + span;
	}

	template <typename T> extern T _roundfloat2(T x, T y){
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
#endif
		// This function returns the minimum float number (span) which satisfies the following conditions:
		// 1. It is either 0, power of 2 or positive infinity.
		// 2. roundfloat(x, span) = roundfloat(y, span).
		
		// x and y should not be -0.0, NaN or -NaN.

		typedef typename nsMath::NumericTrait<T>::uint_type uint_type;
		const uint_type Exponent = nsMath::NumericTrait<T>::Exponent_Mask;
		const UBINT Mantissa_Bits = nsMath::NumericTrait<T>::Mantissa_Bit_Count;

		uint_type a = *(uint_type *)&x, b = *(uint_type *)&y, BitCount; // float hacking
		if ((a&b) >= nsMath::NumericTrait<T>::HighestBit){ a--; b--; } // special case for negative integers
		if ((a^b) >= nsMath::NumericTrait<T>::HighestBit)a = nsMath::NumericTrait<T>::Inf_Positive_ByteForm; //return infinity when a and b has different signs
		else if ((a&Exponent)<(b&Exponent))a = b&Exponent;
		else if ((a&Exponent)>(b&Exponent))a = a&Exponent;
		else if (a == b)a = uint_type(0);
		else{
			b = a^b;
			a = a&Exponent;
			BitCount = nsMath::log2intsim(b) - 1;
			if (BitCount + (a >> Mantissa_Bits) < Mantissa_Bits){
				//the result is a denormalized number
				a = (uint_type)1 << (BitCount + (a >> Mantissa_Bits));
			}
			else a -= (Mantissa_Bits - BitCount) << Mantissa_Bits;
		}
		return *(T *)&a;
	}

	template <typename T> extern T _roundint2(T x, T y){
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this function for performance and robustness.
#endif
		// This function returns the minimum float number (span) which satisfies the following conditions:
		// 1. It is either 0 or power of 2.
		// 2. roundint(x, span) = roundint(y, span).

		if (x == y)return 0;
		else{
			T a = x ^ y;
			return (T)1 << (nsMath::log2intsim(a) - 1);
		}
	}
}

#endif