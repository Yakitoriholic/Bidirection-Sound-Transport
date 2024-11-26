/* Description: Map implementation modelled after STL. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_MAP
#define LIB_CONTAINER_MAP

#include "lGeneral.hpp"
#include "lContainer_RBTree.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	typedef RBTree _treetype_map;

	template < typename Key, typename T >  struct MapNode : public _treetype_map::node_type{
		std::pair<const Key, T> Data;
	};

	template < typename Key, typename T, typename Compare = nsTemplate::compare<Key>, typename A = std::allocator<std::pair<const Key, T>> > class Map;

	template < typename Key, typename T, typename Compare, typename A > struct MapIterator{
		// custom typedefs
		typedef MapIterator<Key, T, Compare, A>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::bidirectional_iterator_tag	iterator_category; //#0

		typedef typename std::pair<const Key, T>			value_type; //#1
		typedef typename ptrdiff_t							difference_type; //#2
		typedef typename value_type*						pointer; //#3
		typedef typename value_type&						reference; //#4

		MapNode<Key, T> *lpNode;
		Map<Key, T, Compare, A> *lpMap;

		MapIterator() {}
		MapIterator(MapNode<Key, T>* Node, Map<Key, T, Compare, A>* const Map) : lpNode(Node), lpMap(Map){}

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
				if (nullptr == this->lpMap->BaseTree.lpRoot)this->lpNode = nullptr;
				else{
					_treetype_map::node_type *CurNode = this->lpMap->BaseTree.lpRoot;
					while (nullptr != CurNode->Child[1])CurNode = CurNode->Child[1];
					this->lpNode = static_cast<MapNode<Key, T> *>(CurNode);
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
	template < typename Key, typename T, typename Compare, typename A > struct MapIterator_Const{
		// custom typedefs
		typedef MapIterator_Const<Key, T, Compare, A>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::bidirectional_iterator_tag	iterator_category; //#0

		typedef typename std::pair<const Key, T>			value_type; //#1
		typedef typename ptrdiff_t							difference_type; //#2
		typedef typename const value_type*					pointer; //#3
		typedef typename const value_type&					reference; //#4

		const MapNode<Key, T> *lpNode;
		const Map<Key, T, Compare, A> *lpMap;

		MapIterator_Const() {}
		MapIterator_Const(const MapNode<Key, T>* Node, const Map<Key, T, Compare, A>* const Map) : lpNode(Node), lpMap(Map){}
		MapIterator_Const(const MapIterator<Key, T, Compare, A>& rhs) : lpNode(rhs.lpNode), lpMap(rhs.lpMap){}

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
				if (nullptr == this->lpMap->BaseTree.lpRoot)this->lpNode = nullptr;
				else{
					_treetype_map::node_type *CurNode = this->lpMap->BaseTree.lpRoot;
					while (nullptr != CurNode->Child[1])CurNode = CurNode->Child[1];
					this->lpNode = static_cast<MapNode<Key, T> *>(CurNode);
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

	template < typename Key, typename T, typename Compare, typename A > class Map{
	public:
		// custom typedefs
		typedef Map<Key, T, Compare, A>		this_type;

		// container typedefs, mandated by the STL standard
		typedef typename A::template rebind< MapNode<Key, T> >::other	allocator_type;
		//diffrent from the C++ STL standard.
		
		typedef typename std::pair<const Key, T>	value_type;
		typedef typename value_type&				reference;
		typedef typename const value_type&			const_reference;
		typedef typename value_type*				pointer;		//diffrent from the C++ STL standard.
		typedef typename const value_type*			const_pointer;	//diffrent from the C++ STL standard.
		typedef typename size_t						size_type;
		typedef typename ptrdiff_t					difference_type;

		typedef typename Key						key_type;

		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		// container typedefs of iterator, mandated by the STL standard
		typedef MapIterator<Key, T, Compare, A>			iterator;
		typedef MapIterator_Const<Key, T, Compare, A>	const_iterator;
		//typedef reverse_iterator			std::reverse_iterator<iterator>;
		//typedef const_reverse_iterator	std::reverse_iterator<const_iterator>;

		friend struct MapIterator<Key, T, Compare, A>;
		friend struct MapIterator_Const<Key, T, Compare, A>;

	protected:
		_treetype_map	BaseTree;
		allocator_type	Allocator;
		Compare			Comparer;

		//Set<T>& operator =(const HashSet<T>&) = delete;

		// custom methods
		void clear_node(MapNode<Key, T> *lpNode){
			if (nullptr != lpNode->Child[0])clear_node(static_cast<MapNode<Key, T> *>(lpNode->Child[0]));
			if (nullptr != lpNode->Child[1])clear_node(static_cast<MapNode<Key, T> *>(lpNode->Child[1]));
			Allocator.destroy(&lpNode->Data); //since C++11
			Allocator.deallocate(lpNode, 1);
		}
	public:
		// constructors
		explicit Map(const Compare& _Comparer = Compare(), const allocator_type& _Allocator = allocator_type())
			: Allocator(_Allocator), Comparer(_Comparer){
			this->BaseTree.Initialize();
		}
		explicit Map(const allocator_type& _Allocator) : Set(Compare(), _Allocator){}

		//observer functions
		inline allocator_type get_allocator() const { return Allocator; }

		// iterator functions, mandated by the STL standard
		iterator begin(){
			if (nullptr == this->BaseTree.lpRoot)return iterator(nullptr, this);
			else{
				_treetype_map::node_type *CurNode = this->BaseTree.lpRoot;
				while (nullptr != CurNode->Child[0])CurNode = CurNode->Child[0];
				return iterator(static_cast<MapNode<Key, T> *>(CurNode), this);
			}
		}
		inline const_iterator cbegin() const {
			if (nullptr == this->BaseTree.lpRoot)return const_iterator(nullptr, this);
			else{
				_treetype_map::node_type *CurNode = this->BaseTree.lpRoot;
				while (nullptr != CurNode->Child[0])CurNode = CurNode->Child[0];
				return const_iterator(static_cast<MapNode<Key, T> *>(CurNode), this);
			}
		}
		inline iterator end(){ return iterator(nullptr, this); }
		inline const_iterator cend() const { return const_iterator(nullptr, this); }

		// capacity functions
		inline bool empty() const { return BaseTree.Size == 0; }
		inline size_type size() const { return BaseTree.Size; }

		// modifiers
		inline void clear(){
			if (nullptr != this->BaseTree.lpRoot)clear_node(static_cast<MapNode<Key, T> *>(this->BaseTree.lpRoot));
			this->BaseTree.Initialize();
		}
		std::pair<iterator, bool> insert(const value_type& value){
			_treetype_map::node_type **lpCurNodePtr = &this->BaseTree.lpRoot, *ParentPtr = nullptr;
			while (nullptr != *lpCurNodePtr){
				ParentPtr = *lpCurNodePtr;
				int CompResult = Comparer(value, static_cast<MapNode<Key, T> *>(*lpCurNodePtr)->Data);
				if (0 == CompResult)return std::pair<iterator, bool>(iterator(static_cast<MapNode<Key, T> *>(*lpCurNodePtr), this), false);
				else if (CompResult < 0)lpCurNodePtr = &((*lpCurNodePtr)->Child[0]);
				else lpCurNodePtr = &((*lpCurNodePtr)->Child[1]);
			}

			MapNode<Key, T> *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, value); //since C++11

			*lpCurNodePtr = NewNode;
			this->BaseTree.insert(NewNode, ParentPtr);

			return std::pair<iterator, bool>(iterator(NewNode, this), true);
		}
		template< typename ... Args > std::pair<iterator, bool> emplace(Args&& ... args){
			MapNode<Key, T> *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, std::forward<Args>(args)...); //since C++11

			_treetype_map::node_type **lpCurNodePtr = &this->BaseTree.lpRoot, *ParentPtr = nullptr;
			while (nullptr != *lpCurNodePtr){
				ParentPtr = *lpCurNodePtr;
				int CompResult = Comparer(NewNode->Data.first, static_cast<MapNode<Key, T> *>(*lpCurNodePtr)->Data.first);
				if (0 == CompResult){
					return std::pair<iterator, bool>(iterator(static_cast<MapNode<Key, T> *>(*lpCurNodePtr), this), false);
				}
				else if (CompResult < 0)lpCurNodePtr = &((*lpCurNodePtr)->Child[0]);
				else lpCurNodePtr = &((*lpCurNodePtr)->Child[1]);
			}

			MapNode<Key, T> *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, std::forward<Args>(args)...); //since C++11

			*lpCurNodePtr = NewNode;
			this->BaseTree.insert(NewNode, ParentPtr);
			return std::pair<iterator, bool>(iterator(NewNode, this), true);
		}
		template< typename ... Args > std::pair<iterator, bool> try_emplace(const key_type& key, Args&& ... args){
			_treetype_map::node_type **lpCurNodePtr = &this->BaseTree.lpRoot, *ParentPtr = nullptr;
			while (nullptr != *lpCurNodePtr){
				ParentPtr = *lpCurNodePtr;
				int CompResult = Comparer(key, static_cast<MapNode<Key, T> *>(*lpCurNodePtr)->Data.first);
				if (0 == CompResult)return std::pair<iterator, bool>(iterator(static_cast<MapNode<Key, T> *>(*lpCurNodePtr), this), false);
				else if (CompResult < 0)lpCurNodePtr = &((*lpCurNodePtr)->Child[0]);
				else lpCurNodePtr = &((*lpCurNodePtr)->Child[1]);
			}

			MapNode<Key, T> *NewNode = Allocator.allocate(1);
			new((void *)&(NewNode->Data.first)) key_type(key);
			new((void *)&(NewNode->Data.second)) T(args...);

			*lpCurNodePtr = NewNode;
			this->BaseTree.insert(NewNode, ParentPtr);
			return std::pair<iterator, bool>(iterator(NewNode, this), true);
		}

		iterator erase(const_iterator pos){
			if (pos == this->cend())return this->cend();
			else{
				_treetype_map::node_type *NextNode = this->BaseTree.erase(const_cast<MapNode<Key, T> *>(pos.lpNode));

				Allocator.destroy(&pos.lpNode->Data); //since C++11
				Allocator.deallocate(const_cast<MapNode<Key, T> *>(pos.lpNode), 1);

				returniterator(static_cast<MapNode<Key, T> *>(NextNode), this);
			}
		}
		size_type erase(const key_type& key){
			_treetype_map::node_type *CurNodePtr = this->BaseTree.lpRoot;
			while (nullptr != CurNodePtr){
				int CompResult = Comparer(key, static_cast<MapNode<Key, T> *>(CurNodePtr)->Data.first);
				if (0 == CompResult)break;
				else if (CompResult < 0)CurNodePtr = CurNodePtr->Child[0];
				else CurNodePtr = CurNodePtr->Child[1];
			}

			if (nullptr == CurNodePtr)return 0;
			else{
				this->BaseTree.erase(CurNodePtr);

				Allocator.destroy(&static_cast<MapNode<Key, T> *>(CurNodePtr)->Data); //since C++11
				Allocator.deallocate(static_cast<MapNode<Key, T> *>(CurNodePtr), 1);

				return 1;
			}
		}

		//query functions
		iterator find(const Key& key){
			_treetype_map::node_type *CurNodePtr = this->BaseTree.lpRoot;
			while (nullptr != CurNodePtr){
				int CompResult = Comparer(key, static_cast<MapNode<Key, T> *>(CurNodePtr)->Data.first);
				if (0 == CompResult)break;
				else if (CompResult < 0)CurNodePtr = CurNodePtr->Child[0];
				else CurNodePtr = CurNodePtr->Child[1];
			}
			return iterator(static_cast<MapNode<Key, T> *>(CurNodePtr), this);
		}
		const_iterator find(const Key& key) const{
			_treetype_map::node_type *CurNodePtr = this->BaseTree.lpRoot;
			while (nullptr != CurNodePtr){
				int CompResult = Comparer(key, static_cast<MapNode<Key, T> *>(CurNodePtr)->Data.first);
				if (0 == CompResult)break;
				else if (CompResult < 0)CurNodePtr = CurNodePtr->Child[0];
				else CurNodePtr = CurNodePtr->Child[1];
			}
			return const_iterator(static_cast<MapNode<Key, T> *>(CurNodePtr), this);
		}

		inline ~Map(){ clear(); }
	};
}

#endif