/* Description: Basic data structure classes modelled after STL.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER
#define LIB_CONTAINER

#include "lMath.hpp"
#include "lTemplate.hpp"

namespace nsContainer{
	//The standard C++ library hasn't provided a function to destroy all the classes between two iterators. So we have to make our own wheel. 
	//RangeDestroyer is a template class which contains a member function Destroy().
	//Usage: RangeDestroyer<TypeOfAllocator>::Destroy(Allocator, Iterator_First, Iterator_Last);
	template <typename A>class RangeDestroyer{
	private:
		template <bool C> class _TSelector;
		template <> class _TSelector<true>{
		public:
			static inline void Destroy(A Allocator, typename A::pointer First, typename A::pointer Last){}
		};
		template <> class _TSelector<false>{
		public:
			static inline void Destroy(A Allocator, typename A::pointer First, typename A::pointer Last){
				for (; First != Last; ++First)Allocator.destroy(First);
			}
		};
	public:
		static inline void Destroy(A Allocator, typename A::pointer First, typename A::pointer Last){
			_TSelector<std::is_trivially_destructible<A::value_type>::value>::Destroy(Allocator, First, Last);
		}
	};
}

#include "Container/lContainer_Vector.hpp"
#include "Container/lContainer_List.hpp"
#include "Container/lContainer_PriorityQueue.hpp"

#include "Container/lContainer_Set.hpp"
#include "Container/lContainer_Map.hpp"

#include "Container/lContainer_HashSet.hpp"
#include "Container/lContainer_HashMap.hpp"

#include "Container/lContainer_Concurrent_List.hpp"
#include "Container/lContainer_Concurrent_HashSet.hpp"

#include "Container/lContainer_IntrvTreeMap.hpp"
#include "Container/lContainer_PointQuadtreeMap.hpp"
#include "Container/lContainer_PointOctreeMap.hpp"

#include "Container/lContainer_BVH3DMap.hpp"

#endif