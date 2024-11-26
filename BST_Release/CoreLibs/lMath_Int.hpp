/* Description: Platform-independent mathematical functions about integrals.
 * Language: C++
 * Author: ***
 */

#ifndef LIB_MATH_INT
#define LIB_MATH_INT

#include "lMath.hpp"

namespace nsMath{
	
	template <typename T> std::complex<T> FresnelInt_Approx(T x);
	//evaluate the approximate value of Fresnel integral (\int_0^x e^(i \frac{\pi}{2} t^2)dt).

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	// the method used here comes from the paper "Rational Approximations for the Fresnel Integrals" by Mark A. Heald.

	template <typename T> std::complex<T> FresnelInt_Approx(T x){
		bool isPositive;
		if (x < 0){ x = -x; isPositive = false; } else isPositive = true;
		T R0 = (((((((((T)0.0433995 * x) + (T)0.1339259) * x) + (T)0.3460509) * x) + (T)0.6460117) * x) + (T)0.7769507) * x + (T)1;
		T R1 = (((((((((((T)0.13634704 * x) + (T)0.4205217) * x) + (T)1.0917325) * x) + (T)1.9840524) * x) + (T)2.7196741) * x) + (T)2.5129806) * x + sqrt((T)2);
		T A0 = (((((((T)0.0241212 * x) + (T)0.068324) * x) + (T)0.2363641) * x) + (T)0.1945161) * x + (T)1;
		T A1 = (((((((((((T)0.118247 * x) + (T)0.356681) * x) + (T)0.978113) * x) + (T)1.875721) * x) + (T)2.7570246) * x) + (T)2.9355041) * x + (T)2;
		T R = R0 / R1;
		T A = (A0 / A1 - x*x) * (T)M_PI_2;
		if (isPositive)return std::complex<T>((T)0.5 - R * sin(A), (T)0.5 - R * cos(A)); else return std::complex<T>((T)R * sin(A) - 0.5, (T)R * cos(A) - 0.5);
	}
}
#endif