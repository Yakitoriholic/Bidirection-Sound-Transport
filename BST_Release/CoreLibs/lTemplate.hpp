/* Description: Basic data structure classes modelled after STL.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_TEMPLATE
#define LIB_TEMPLATE

#include "lGeneral.hpp"

#include <iterator>
#include <algorithm>

namespace nsTemplate{
	//cond<> is a template function which returns A when C is true and B when otherwise.
	template < bool C, typename A, typename B > struct cond;
	template < typename A, typename B > struct cond < true, A, B >{ typedef A value; };
	template < typename A, typename B > struct cond < false, A, B >{ typedef B value; };

	//compare<> is a three way comparison functor which returns -1 when the left hand side is lesser than the righht hand side.
	//This functor is used in the implementation of ordered containers.
	//I don't know what C++ standard commitee members think. But the three-way functor is evidently more efficient than the std::less<> functor. 
	template < typename T = void > struct compare : public std::binary_function<T, T, int>{
		int operator()(const T& lhs, const T& rhs) const{
			return lhs < rhs ? -1 : (lhs == rhs ? 0 : 1);
		}
	};
}
#endif