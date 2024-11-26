/* Description: List class modelled after STL. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_LIST
#define LIB_CONTAINER_LIST

#include "lGeneral.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{

	template < typename T > struct ListNode;

	template < typename T > struct ListNodeStub{
		ListNode<T>*	Prev;
		ListNode<T>*	Next;
	};

	template < typename T > struct ListNode : public ListNodeStub<T>{
		T Data;
	};

	template < typename T > struct ListIterator{
		// iterator typedefs, mandated by the STL standard
		typedef typename std::bidirectional_iterator_tag iterator_category; //#0

		typedef typename T			value_type; //#1
		typedef typename ptrdiff_t	difference_type; //#2
		typedef typename T*			pointer; //#3
		typedef typename T&			reference; //#4
		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		//types used inside the class
		typedef ListIterator<T>			this_type;
		typedef typename ListNode<T>*	node_pointer;

		node_pointer lpNode;

		ListIterator(node_pointer x) : lpNode(x) {}
		ListIterator(pointer x) : lpNode((node_pointer)((UBINT)x - offsetof(ListNode<T>, Data))) {}
		ListIterator() {}
		//ListIterator(const this_type& rhs) : lpNode(rhs.lpNode) {}

		bool operator==(const this_type& rhs) const { return lpNode == rhs.lpNode; }
		bool operator!=(const this_type& rhs) const { return lpNode != rhs.lpNode; }

		reference operator*() const { return lpNode->Data; }
		pointer operator->() const { return &(lpNode->Data); }

		this_type& operator++(){
			lpNode = lpNode->Next;
			return *this;
		}
		this_type operator++(int){
			this_type TmpIt = *this;
			++*this;
			return TmpIt;
		}
		this_type& operator--(){
			lpNode = lpNode->Prev;
			return *this;
		}
		this_type operator--(int){
			this_type TmpIt = *this;
			--*this;
			return TmpIt;
		}
	};

	template < typename T > struct ListIterator_Const{
		// iterator typedefs, mandated by the STL standard
		typedef typename std::bidirectional_iterator_tag iterator_category; //#0

		typedef typename T			value_type; //#1
		typedef typename ptrdiff_t	difference_type; //#2
		typedef typename const T*	pointer; //#3
		typedef typename const T&	reference; //#4
		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		//types used inside the class
		typedef ListIterator_Const<T>		this_type;
		typedef typename const ListNode<T>*	node_pointer;

		node_pointer lpNode;

		ListIterator_Const(node_pointer x) : lpNode(x) {}
		ListIterator_Const(pointer x) : lpNode((node_pointer)((UBINT)x - offsetof(ListNode<T>, Data))) {}
		ListIterator_Const() {}
		ListIterator_Const(const ListIterator<T>& rhs) : lpNode(rhs.lpNode) {}

		bool operator==(const this_type& rhs) const { return lpNode == rhs.lpNode; }
		bool operator!=(const this_type& rhs) const { return lpNode != rhs.lpNode; }

		reference operator*() const { return lpNode->Data; }
		pointer operator->() const { return &(lpNode->Data); }

		this_type& operator++(){
			lpNode = lpNode->Next;
			return *this;
		}
		this_type operator++(int){
			this_type TmpIt = *this;
			++*this;
			return TmpIt;
		}
		this_type& operator--(){
			lpNode = lpNode->Prev;
			return *this;
		}
		this_type operator--(int){
			this_type TmpIt = *this;
			--*this;
			return TmpIt;
		}
	};

	template < typename T, typename A = std::allocator<T> > class List{
		//without spaces, > > will be considered as an operator instead of two brackets.
	public:
		// custom typedefs
		typedef List<T, A>				this_type;
		typedef ListNode<T>				node_type;

		// container typedefs, mandated by the STL standard
		typedef typename A::template rebind<node_type>::other	allocator_type; //diffrent from the C++ STL standard.
		typedef typename T										value_type;
		typedef typename T&										reference;
		typedef typename const T&								const_reference;
		typedef typename T*										pointer;
		typedef typename const T*								const_pointer;
		typedef typename size_t									size_type;
		typedef typename ptrdiff_t								difference_type;

		// container typedefs of iterator, mandated by the STL standard
		typedef ListIterator<value_type>		iterator;
		typedef ListIterator_Const<value_type>	const_iterator;

	protected:
		ListNodeStub<T> EndPtr;
		allocator_type Allocator;

	public:
		explicit List(const allocator_type& alloc) : EndPtr(){
			Allocator = alloc;
			EndPtr.Prev = static_cast<node_type *>(&EndPtr);
			EndPtr.Next = static_cast<node_type *>(&EndPtr);
		};
		List() :List(allocator_type()){}

		//observer functions
		inline allocator_type get_allocator() const { return Allocator; }

		// iterator functions, mandated by the STL standard
		inline iterator begin(){ return EndPtr.Next; }
		inline const_iterator cbegin() const { return EndPtr.Next; }
		inline iterator end(){ return static_cast<node_type *>(&EndPtr); }
		inline const_iterator cend() const { return static_cast<const node_type *>(&EndPtr); }

		//element access functions
		inline reference front(){ return EndPtr.Next->Data; }
		inline const_reference front() const { return EndPtr.Next->Data; }
		inline reference back(){ return EndPtr.Prev->Data; }
		inline const_reference back() const { return EndPtr.Prev->Data; }

		//capacity functions
		inline bool empty() const { return EndPtr.Next == static_cast<const node_type *>(&EndPtr); }
		size_type size() const {
			size_type Counter = 0;
			node_type *CurNode = EndPtr.Next;
			while (CurNode != static_cast<const node_type *>(&EndPtr)){
				Counter++;
				CurNode = CurNode->Next;
			}
			return Counter;
		}

		//modifiers
		void clear(){
			node_type *CurNode = EndPtr.Next;
			while (CurNode != static_cast<node_type *>(&EndPtr)){
				node_type *TmpNode = CurNode;
				CurNode = CurNode->Next;
				Allocator.destroy(&TmpNode->Data);
				Allocator.deallocate(TmpNode, 1);
			}
			EndPtr.Prev = static_cast<node_type *>(&EndPtr);
			EndPtr.Next = static_cast<node_type *>(&EndPtr);
		}
		iterator insert(const_iterator Pos, const_reference Value){
			node_type *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, Value); //since C++11
			NewNode->Next = Pos.lpNode;
			NewNode->Prev = Pos.lpNode->Prev;
			NewNode->Prev->Next = NewNode;
			Pos.lpNode->Prev = NewNode;
			return NewNode;
		}
		iterator erase(const_iterator Pos){
			node_type *CurNode = const_cast<node_type *>(Pos.lpNode);
			node_type *PrevNode = CurNode->Prev;
			node_type *NextNode = CurNode->Next;
			NextNode->Prev = PrevNode;
			PrevNode->Next = NextNode;
			Allocator.destroy(&CurNode->Data);
			Allocator.deallocate(CurNode, 1);
			return NextNode;
		}
		void push_back(const_reference Value){
			node_type *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, Value); //since C++11
			NewNode->Next = static_cast<node_type *>(&EndPtr);
			NewNode->Prev = EndPtr.Prev;
			EndPtr.Prev->Next = NewNode;
			EndPtr.Prev = NewNode;
		}
		template <typename ... Args> void emplace_back(Args&& ... args){
			node_type *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, std::forward<Args>(args)...); //since C++11
			NewNode->Next = static_cast<node_type *>(&EndPtr);
			NewNode->Prev = EndPtr.Prev;
			EndPtr.Prev->Next = NewNode;
			EndPtr.Prev = NewNode;
		}
		void pop_back(){ //may cause fatal exception when the list is empty
			node_type *CurNode = EndPtr.Prev;
			node_type *PrevNode = CurNode->Prev;
			EndPtr.Prev = PrevNode;
			PrevNode->Next = static_cast<node_type *>(&EndPtr);
			Allocator.destroy(&CurNode->Data);
			Allocator.deallocate(CurNode, 1);
		}
		void push_front(const_reference Value){
			node_type *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, Value); //since C++11
			NewNode->Next = EndPtr.Next;
			NewNode->Prev = static_cast<node_type *>(&EndPtr);
			EndPtr.Next->Prev = NewNode;
			EndPtr.Next = NewNode;
		}
		template <typename ... Args> void emplace_front(Args&& ... args){
			node_type *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, std::forward<Args>(args)...); //since C++11
			NewNode->Next = EndPtr.Next;
			NewNode->Prev = static_cast<node_type *>(&EndPtr);
			EndPtr.Next->Prev = NewNode;
			EndPtr.Next = NewNode;
		}
		void pop_front(){ //may cause fatal exception when the list is empty
			node_type *CurNode = EndPtr.Next;
			node_type *NextNode = CurNode->Next;
			EndPtr.Next = NextNode;
			NextNode->Prev = static_cast<node_type *>(&EndPtr);
			Allocator.destroy(&CurNode->Data);
			Allocator.deallocate(CurNode, 1);
		}

		//operations
		void splice(const_iterator pos, this_type& other, const_iterator it){
			if (it != other.cend()){
				iterator _it(const_cast<ListNode<T> *>(it.lpNode));
				iterator _pos(const_cast<ListNode<T> *>(pos.lpNode));

				//detach from [other]
				_it.lpNode->Prev->Next = _it.lpNode->Next;
				_it.lpNode->Next->Prev = _it.lpNode->Prev;

				//attach to this
				_it.lpNode->Next = _pos.lpNode;
				_it.lpNode->Prev = _pos.lpNode->Prev;
				_pos.lpNode->Prev->Next = _it.lpNode;
				_pos.lpNode->Prev = _it.lpNode;
			}
		}

		inline ~List(){ clear(); }
	};
}

#endif