/* Description: Priority queue (heap) class modelled after STL.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_PRIORITY_QUEUE
#define LIB_CONTAINER_PRIORITY_QUEUE

#include "lGeneral.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{

	template < class T, class Container = Vector<T>, class Compare = std::less<typename Container::value_type> > class PriorityQueue{
		//without spaces, > > will be considered as an operator instead of two brackets.
	public:
		// custom typedefs
		typedef PriorityQueue<T, Container, Compare>	this_type;

		// container typedefs, mandated by the STL standard
		typedef Container								container_type;
		typedef typename Container::value_type			value_type;
		typedef typename Container::reference			reference;
		typedef typename Container::const_reference		const_reference;
		typedef typename Container::size_type			size_type;

		// this container doesn't have iterators.
	protected:
		Container container;
		Compare comp;
	public:
		//constructors
		explicit PriorityQueue(const Compare& compare = Compare(), Container&& cont = Container()) :comp(compare), container(std::move(cont)){};
		PriorityQueue(const Compare& compare, const Container& cont) :comp(compare), container(cont){};
		PriorityQueue(const this_type& rhs) :comp(rhs.comp), container(rhs.container){};
		PriorityQueue(this_type&& rhs) :comp(std::move(rhs.comp)), container(std::move(rhs.container)){};

		//element access functions
		inline const_reference top() const{ return this->container.front(); }

		//capacity functions
		inline bool empty() const { return this->container.empty(); }
		inline size_type size() const { return this->container.size(); }

		//modifiers
		inline void push(const T& value){ this->container.push_back(value); std::push_heap(this->container.begin(), this->container.end(), this->comp); }
		inline void push(T&& value){ this->container.push_back(std::move(value)); std::push_heap(this->container.begin(), this->container.end(), this->comp); }
		template< class... Args > inline void emplace(Args&& ... args){ this->container.emplace_back(std::forward<Args>(args)...); std::push_heap(this->container.begin(), this->container.end(), this->comp); }
		inline void pop(){ std::pop_heap(this->container.begin(), this->container.end(), this->comp); this->container.pop_back(); }

		void swap(this_type& rhs){
			std::swap(this->container, rhs.container);
			std::swap(this->comp, rhs.comp);
		}

		//operators
		this_type& operator=(const this_type& rhs){
			this_type temp(rhs);
			this->swap(temp);
			return *this;
		}
		this_type& operator=(this_type&& rhs){
			this->container = std::move(rhs.container);
			this->comp = std::move(rhs.comp);
			return *this;
		}
		~PriorityQueue(){};
	};
}

#endif