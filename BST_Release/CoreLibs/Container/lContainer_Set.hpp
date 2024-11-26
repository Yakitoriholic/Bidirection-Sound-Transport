/* Description: Set implementation modelled after STL. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_SET
#define LIB_CONTAINER_SET

#include "lGeneral.hpp"
#include "lContainer_RBTree.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	typedef RBTree _treetype_set;

	template < typename T > struct SetNode : public _treetype_set::node_type{
		T Data;
	};

	template < typename Key, typename Compare = nsTemplate::compare<Key>, typename A = std::allocator<Key> > class Set;

	template < typename Key, typename Compare, typename A > struct SetIterator_Const{
		// custom typedefs
		typedef SetIterator_Const<Key, Compare, A>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::bidirectional_iterator_tag	iterator_category; //#0

		typedef typename Key								value_type; //#1
		typedef typename ptrdiff_t							difference_type; //#2
		typedef typename const value_type*					pointer; //#3
		typedef typename const value_type&					reference; //#4

		const SetNode<Key> *lpNode;
		const Set<Key, Compare, A> *lpSet;

		SetIterator_Const() {}
		SetIterator_Const(const SetNode<Key>* Node, const Set<Key, Compare, A>* const Set) : lpNode(Node), lpSet(Set){}

		inline bool operator==(const this_type& rhs) const { return this->lpNode == rhs.lpNode; }
		inline bool operator!=(const this_type& rhs) const { return this->lpNode != rhs.lpNode; }

		inline reference operator*() const { return lpNode->Data; }
		inline pointer operator->() const { return &(lpNode->Data); }

		inline this_type& operator++(){
			this->lpNode = binary_tree_next(this->lpNode);
			return *this;
		}
		inline this_type operator++(int){
			this_type TmpIt = *this;
			++*this;
			return TmpIt;
		}

		inline this_type& operator--(){
			if (nullptr == this->lpNode){
				if (nullptr == this->lpSet->BaseTree.lpRoot)this->lpNode = nullptr;
				else{
					_treetype_set::node_type *CurNode = this->lpSet->BaseTree.lpRoot;
					while (nullptr != CurNode->Child[1])CurNode = CurNode->Child[1];
					this->lpNode = static_cast<SetNode<Key> *>(CurNode);
				}
			}
			else this->lpNode = binary_tree_prev(this->lpNode);
			return *this;
		}
		inline this_type operator--(int){
			this_type TmpIt = *this;
			--*this;
			return TmpIt;
		}
	};

	template < typename Key, typename Compare, typename A > class Set{
	public:
		// custom typedefs
		typedef Set<Key, Compare, A>		this_type;

		// container typedefs, mandated by the STL standard
		typedef typename A::template rebind< SetNode<Key> >::other	allocator_type;
		//diffrent from the C++ STL standard.
		
		typedef typename Key					value_type;
		typedef typename value_type&			reference;
		typedef typename const value_type&		const_reference;
		typedef typename value_type*			pointer;		//diffrent from the C++ STL standard.
		typedef typename const value_type*		const_pointer;	//diffrent from the C++ STL standard.
		typedef typename size_t					size_type;
		typedef typename ptrdiff_t				difference_type;

		typedef typename Key					key_type;

		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		// container typedefs of iterator, mandated by the STL standard
		typedef SetIterator_Const<Key, Compare, A>		iterator;
		typedef SetIterator_Const<Key, Compare, A>		const_iterator;
		//typedef reverse_iterator			std::reverse_iterator<iterator>;
		//typedef const_reverse_iterator	std::reverse_iterator<const_iterator>;

		friend struct SetIterator_Const<Key, Compare, A>;

	protected:
		_treetype_set	BaseTree;
		allocator_type	Allocator;
		Compare			Comparer;

		//Set<T>& operator =(const HashSet<T>&) = delete;

		// custom methods
		void clear_node(SetNode<Key> *lpNode){
			if (nullptr != lpNode->Child[0])clear_node(static_cast<SetNode<Key> *>(lpNode->Child[0]));
			if (nullptr != lpNode->Child[1])clear_node(static_cast<SetNode<Key> *>(lpNode->Child[1]));
			Allocator.destroy(&lpNode->Data); //since C++11
			Allocator.deallocate(lpNode, 1);
		}
	public:
		//constructors
		explicit Set(const Compare& _Comparer = Compare(), const allocator_type& _Allocator = allocator_type())
			: Allocator(_Allocator), Comparer(_Comparer){
			this->BaseTree.Initialize();
		}
		explicit Set(const allocator_type& _Allocator) : Set(Compare(), _Allocator){}

		//observer functions
		inline allocator_type get_allocator() const { return Allocator; }

		// iterator functions, mandated by the STL standard
		const_iterator begin() const {
			if (nullptr == this->BaseTree.lpRoot)return const_iterator(nullptr, this);
			else{
				_treetype_set::node_type *CurNode = this->BaseTree.lpRoot;
				while (nullptr != CurNode->Child[0])CurNode = CurNode->Child[0];
				return const_iterator(static_cast<SetNode<Key> *>(CurNode), this);
			}
		}
		inline const_iterator cbegin() const { return begin(); }
		inline const_iterator end() const { return const_iterator(nullptr, this); }
		inline const_iterator cend() const { return end(); }

		//capacity functions
		inline bool empty() const { return BaseTree.Size == 0; }
		inline size_type size() const { return BaseTree.Size; }

		//modifiers
		inline void clear(){
			if (nullptr != this->BaseTree.lpRoot)clear_node(static_cast<SetNode<Key> *>(this->BaseTree.lpRoot));
			this->BaseTree.Initialize();
		}
		std::pair<const_iterator, bool> insert(const value_type& value){
			_treetype_set::node_type **lpCurNodePtr = &this->BaseTree.lpRoot, *ParentPtr = nullptr;
			while (nullptr != *lpCurNodePtr){
				ParentPtr = *lpCurNodePtr;
				int CompResult = Comparer(value, static_cast<SetNode<Key> *>(*lpCurNodePtr)->Data);
				if (0 == CompResult)return std::pair<const_iterator, bool>(const_iterator(static_cast<SetNode<Key> *>(*lpCurNodePtr), this), false);
				else if (CompResult < 0)lpCurNodePtr = &((*lpCurNodePtr)->Child[0]);
				else lpCurNodePtr = &((*lpCurNodePtr)->Child[1]);
			}

			SetNode<Key> *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, value); //since C++11

			*lpCurNodePtr = NewNode;
			this->BaseTree.insert(NewNode, ParentPtr);

			return std::pair<const_iterator, bool>(const_iterator(NewNode, this), true);
		}
		template< typename ... Args > std::pair<const_iterator, bool> emplace(Args&& ... args){
			SetNode<Key> *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, std::forward<Args>(args)...); //since C++11

			_treetype_set::node_type **lpCurNodePtr = &this->BaseTree.lpRoot, *ParentPtr = nullptr;
			while (nullptr != *lpCurNodePtr){
				ParentPtr = *lpCurNodePtr;
				int CompResult = Comparer(NewNode->Data, static_cast<SetNode<Key> *>(*lpCurNodePtr)->Data);
				if (0 == CompResult){
					Allocator.destroy(&NewNode->Data); //since C++11
					Allocator.deallocate(NewNode, 1);
					return std::pair<const_iterator, bool>(const_iterator(static_cast<SetNode<Key> *>(*lpCurNodePtr), this), false);
				}
				else if (CompResult < 0)lpCurNodePtr = &((*lpCurNodePtr)->Child[0]);
				else lpCurNodePtr = &((*lpCurNodePtr)->Child[1]);
			}

			*lpCurNodePtr = NewNode;
			this->BaseTree.insert(NewNode, ParentPtr);
			return std::pair<const_iterator, bool>(const_iterator(NewNode, this), true);
		}

		const_iterator erase(const_iterator pos){
			if (pos == this->cend())return this->cend();
			else{
				_treetype_set::node_type *NextNode = this->BaseTree.erase(const_cast<SetNode<Key> *>(pos.lpNode));

				Allocator.destroy(&pos.lpNode->Data); //since C++11
				Allocator.deallocate(const_cast<SetNode<Key> *>(pos.lpNode), 1);

				return const_iterator(static_cast<SetNode<Key> *>(NextNode), this);
			}
		}
		size_type erase(const key_type& key){
			_treetype_set::node_type *CurNodePtr = this->BaseTree.lpRoot;
			while (nullptr != CurNodePtr){
				int CompResult = Comparer(key, static_cast<SetNode<Key> *>(CurNodePtr)->Data);
				if (0 == CompResult)break;
				else if (CompResult < 0)CurNodePtr = CurNodePtr->Child[0];
				else CurNodePtr = CurNodePtr->Child[1];
			}

			if (nullptr == CurNodePtr)return 0;
			else{
				this->BaseTree.erase(CurNodePtr);

				Allocator.destroy(&static_cast<SetNode<Key> *>(CurNodePtr)->Data); //since C++11
				Allocator.deallocate(static_cast<SetNode<Key> *>(CurNodePtr), 1);

				return 1;
			}
		}

		//query functions
		const_iterator find(const Key& value) const{
			_treetype_set::node_type *CurNodePtr = this->BaseTree.lpRoot;
			while (nullptr != CurNodePtr){
				int CompResult = Comparer(value, static_cast<SetNode<Key> *>(CurNodePtr)->Data);
				if (0 == CompResult)break;
				else if (CompResult < 0)CurNodePtr = CurNodePtr->Child[0];
				else CurNodePtr = CurNodePtr->Child[1];
			}
			return const_iterator(static_cast<SetNode<Key> *>(CurNodePtr), this);
		}

		inline ~Set(){ clear(); }
	};
}

#endif