/* Description: Platform-independent mathematical functions relative to Monte-Carlo algorithm.
 * Language: C++
 * Author: ***
 */

#ifndef LIB_MATH_MONTE_CARLO
#define LIB_MATH_MONTE_CARLO

#include "lMath.hpp"

namespace nsMath{
	template<class T> extern float VanDerCorputSequence2(T n);
	// returns the [n]-th number in a base-2 van der Corput sequence.
	
	template<class T> extern float VanDerCorputSequence(T n, T Base);
	// returns the [n]-th number in a van der Corput sequence with base [Base].

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	template<class T> extern inline float VanDerCorputSequence2(T n){
		return (float)reverse(n) / NumericTrait<T>::GroupOrder_Float;
	}
	template<class T> extern float VanDerCorputSequence(T n, T Base){
		float InvBase = 1.0f / (float)Base, RetValue = 0.0f;
		float Position = InvBase;
		while (n > 0){
			RetValue += (float)(n % Base) * Position;
			Position *= InvBase;
			n /= Base;
		}
		return RetValue;
	}
}
#endif