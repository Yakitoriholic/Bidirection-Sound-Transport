/* Description: Interval Tree Map implementation. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_INTRVTREE_MAP
#define LIB_INTRVTREE_MAP

#include "lGeneral.hpp"
#include "lContainer_RBTree.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	typedef RBTree _treetype_intrvtreemap;

	template < typename Dim_Type, typename T > struct IntervalMap{
		const nsMath::TinyVector<Dim_Type, 2> Interval; // left close, right open
		T Data;
	};

	template < typename Dim_Type, typename T >  struct IntrvTreeMapNode : public _treetype_intrvtreemap::node_type{
		Dim_Type MaxRBound;
		IntervalMap<Dim_Type, T> Data;
	};

	template < typename Dim_Type, typename T >  struct IntrvTreeMapNode_Reduce{
		inline void operator()(_treetype_intrvtreemap::node_type *Node){
			IntrvTreeMapNode<Dim_Type, T> *Node_Unpacked = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(Node);
			_treetype_intrvtreemap::node_type *Child0 = Node->Child[0], *Child1 = Node->Child[1];

			Node_Unpacked->MaxRBound = Node_Unpacked->Data.Interval[1];
			if (nullptr != Child0){
				Dim_Type RBoundCandidate = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(Child0)->MaxRBound;
				if (RBoundCandidate > Node_Unpacked->MaxRBound)Node_Unpacked->MaxRBound = RBoundCandidate;
			}
			if (nullptr != Child1){
				Dim_Type RBoundCandidate = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(Child1)->MaxRBound;
				if (RBoundCandidate > Node_Unpacked->MaxRBound)Node_Unpacked->MaxRBound = RBoundCandidate;
			}
		}
	};

	template < typename Dim_Type, typename T, typename A = std::allocator<IntervalMap<const Dim_Type, T>> > class IntrvTreeMap;

	template < typename Dim_Type, typename T, typename A > struct IntrvTreeMapIterator{
		// custom typedefs
		typedef IntrvTreeMapIterator<Dim_Type, T, A>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::bidirectional_iterator_tag	iterator_category; //#0

		typedef typename IntervalMap<Dim_Type, T>			value_type; //#1
		typedef typename ptrdiff_t							difference_type; //#2
		typedef typename value_type*						pointer; //#3
		typedef typename value_type&						reference; //#4

		IntrvTreeMapNode<Dim_Type, T> *lpNode;
		IntrvTreeMap<Dim_Type, T, A> *lpMap;

		IntrvTreeMapIterator() {}
		IntrvTreeMapIterator(IntrvTreeMapNode<Dim_Type, T>* Node, IntrvTreeMap<Dim_Type, T, A>* const Map) : lpNode(Node), lpMap(Map){}

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
					_treetype_intrvtreemap::node_type *CurNode = this->lpMap->BaseTree.lpRoot;
					while (nullptr != CurNode->Child[1])CurNode = CurNode->Child[1];
					this->lpNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNode);
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
	template < typename Dim_Type, typename T, typename A > struct IntrvTreeMapIterator_Const{
		// custom typedefs
		typedef IntrvTreeMapIterator_Const<Dim_Type, T, A>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::bidirectional_iterator_tag	iterator_category; //#0

		typedef typename IntervalMap<Dim_Type, T>			value_type; //#1
		typedef typename ptrdiff_t							difference_type; //#2
		typedef typename const value_type*					pointer; //#3
		typedef typename const value_type&					reference; //#4

		const IntrvTreeMapNode<Dim_Type, T> *lpNode;
		const IntrvTreeMap<Dim_Type, T, A> *lpMap;

		IntrvTreeMapIterator_Const() {}
		IntrvTreeMapIterator_Const(const IntrvTreeMapNode<Dim_Type, T>* Node, const IntrvTreeMap<Dim_Type, T, A>* const Map) : lpNode(Node), lpMap(Map){}
		IntrvTreeMapIterator_Const(const IntrvTreeMapIterator<Dim_Type, T, A>& rhs) : lpNode(rhs.lpNode), lpMap(rhs.lpMap){}

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
					_treetype_intrvtreemap::node_type *CurNode = this->lpMap->BaseTree.lpRoot;
					while (nullptr != CurNode->Child[1])CurNode = CurNode->Child[1];
					this->lpNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNode);
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

	template < typename Dim_Type, typename T, typename A > class IntrvTreeMap{
	public:
		// custom typedefs
		typedef IntrvTreeMap<Dim_Type, T, A>		this_type;

		// container typedefs, mandated by the STL standard
		typedef typename A::template rebind< IntrvTreeMapNode<Dim_Type, T> >::other	allocator_type;
		//diffrent from the C++ STL standard.
		
		typedef typename IntervalMap<Dim_Type, T>			value_type;
		typedef typename value_type&						reference;
		typedef typename const value_type&					const_reference;
		typedef typename value_type*						pointer;		//diffrent from the C++ STL standard.
		typedef typename const value_type*					const_pointer;	//diffrent from the C++ STL standard.
		typedef typename size_t								size_type;
		typedef typename ptrdiff_t							difference_type;

		typedef typename nsMath::TinyVector<Dim_Type, 2>	key_type;

		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		// container typedefs of iterator, mandated by the STL standard
		typedef IntrvTreeMapIterator<Dim_Type, T, A>		iterator;
		typedef IntrvTreeMapIterator_Const<Dim_Type, T, A>	const_iterator;
		//typedef reverse_iterator			std::reverse_iterator<iterator>;
		//typedef const_reverse_iterator	std::reverse_iterator<const_iterator>;

		friend struct IntrvTreeMapIterator<Dim_Type, T, A>;
		friend struct IntrvTreeMapIterator_Const<Dim_Type, T, A>;

	protected:
		_treetype_intrvtreemap	BaseTree;
		allocator_type			Allocator;

		//Set<T>& operator =(const HashSet<T>&) = delete;

		// custom methods
		void clear_node(IntrvTreeMapNode<Dim_Type, T> *lpNode){
			if (nullptr != lpNode->Child[0])clear_node(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(lpNode->Child[0]));
			if (nullptr != lpNode->Child[1])clear_node(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(lpNode->Child[1]));
			Allocator.destroy(&lpNode->Data); //since C++11
			Allocator.deallocate(lpNode, 1);
		}
	public:
		//constructors
		explicit IntrvTreeMap(const allocator_type& _Allocator = allocator_type())
			: Allocator(_Allocator){
			this->BaseTree.Initialize();
		}

		//observer functions
		inline allocator_type get_allocator() const { return Allocator; }

		// iterator functions, mandated by the STL standard
		const_iterator begin(){
			if (nullptr == this->BaseTree.lpRoot)return iterator(nullptr, this);
			else{
				_treetype_intrvtreemap::node_type *CurNode = this->BaseTree.lpRoot;
				while (nullptr != CurNode->Child[0])CurNode = CurNode->Child[0];
				return iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNode), this);
			}
		}
		inline const_iterator cbegin() const {
			if (nullptr == this->BaseTree.lpRoot)return const_iterator(nullptr, this);
			else{
				_treetype_intrvtreemap::node_type *CurNode = this->BaseTree.lpRoot;
				while (nullptr != CurNode->Child[0])CurNode = CurNode->Child[0];
				return const_iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNode), this);
			}
		}
		inline const_iterator end(){ return iterator(nullptr, this); }
		inline const_iterator cend() const { return const_iterator(nullptr, this); }

		//capacity functions
		inline bool empty() const { return BaseTree.Size == 0; }
		inline size_type size() const { return BaseTree.Size; }

		//modifiers
		inline void clear(){
			if (nullptr != this->BaseTree.lpRoot)clear_node(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(this->BaseTree.lpRoot));
			this->BaseTree.Initialize();
		}
		template< typename ... Args > std::pair<iterator, bool> try_emplace(const Dim_Type LBound, const Dim_Type RBound, Args&&... args){
			_treetype_intrvtreemap::node_type **lpCurNodePtr = &this->BaseTree.lpRoot, *ParentPtr = nullptr;
			while (nullptr != *lpCurNodePtr){
				ParentPtr = *lpCurNodePtr;
				Dim_Type LBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(*lpCurNodePtr)->Data.Interval[0];
				if (LBound == LBound_CurNode){
					Dim_Type RBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(*lpCurNodePtr)->Data.Interval[1];
					if (RBound == RBound_CurNode)return std::pair<iterator, bool>(iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(*lpCurNodePtr), this), false);
					else if (RBound < RBound_CurNode)lpCurNodePtr = &((*lpCurNodePtr)->Child[0]);
					else lpCurNodePtr = &((*lpCurNodePtr)->Child[1]);
				}
				else if (LBound < LBound_CurNode)lpCurNodePtr = &((*lpCurNodePtr)->Child[0]);
				else lpCurNodePtr = &((*lpCurNodePtr)->Child[1]);
			}

			IntrvTreeMapNode<Dim_Type, T> *NewNode = Allocator.allocate(1);
			new((void *)&NewNode->Data.Interval[0]) Dim_Type(LBound);
			new((void *)&NewNode->Data.Interval[1]) Dim_Type(RBound);
			new((void *)&(NewNode->Data.Data)) T(args...);

			*lpCurNodePtr = NewNode;
			IntrvTreeMapNode_Reduce<Dim_Type, T> MyReduceFunctor;
			this->BaseTree.insert_reduce(NewNode, ParentPtr, MyReduceFunctor);

			return std::pair<iterator, bool>(iterator(NewNode, this), true);
		}

		iterator erase(const_iterator pos){
			if (pos == this->cend())return this->cend();
			else{
				_treetype_intrvtreemap::node_type *NextNode = this->BaseTree.erase(const_cast<IntrvTreeMapNode<Dim_Type, T> *>(pos.lpNode));

				Allocator.destroy(&pos.lpNode->Data); //since C++11
				Allocator.deallocate(const_cast<IntrvTreeMapNode<Dim_Type, T> *>(pos.lpNode), 1);

				returniterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(NextNode), this);
			}
		}
		size_type erase(const Dim_Type LBound, const Dim_Type RBound){
			_treetype_intrvtreemap::node_type *CurNodePtr = this->BaseTree.lpRoot;
			while (nullptr != CurNodePtr){
				Dim_Type LBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
				if (LBound == LBound_CurNode){
					Dim_Type RBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[1];
					if (RBound == RBound_CurNode)break;
					else if (RBound < RBound_CurNode)CurNodePtr = CurNodePtr->Child[0];
					else CurNodePtr = CurNodePtr->Child[1];
				}
				else if (LBound < LBound_CurNode)CurNodePtr = CurNodePtr->Child[0];
				else CurNodePtr = CurNodePtr->Child[1];
			}

			if (nullptr == CurNodePtr)return 0;
			else{
				this->BaseTree.erase(CurNodePtr);

				Allocator.destroy(&static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data); //since C++11
				Allocator.deallocate(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr), 1);

				return 1;
			}
		}

		//query functions
		iterator find(const Dim_Type LBound, const Dim_Type RBound){
			_treetype_intrvtreemap::node_type *CurNodePtr = this->BaseTree.lpRoot;
			while (nullptr != CurNodePtr){
				Dim_Type LBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
				if (LBound == LBound_CurNode){
					Dim_Type RBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[1];
					if (RBound == RBound_CurNode)break;
					else if (RBound < RBound_CurNode)CurNodePtr = CurNodePtr->Child[0];
					else CurNodePtr = CurNodePtr->Child[1];
				}
				else if (LBound < LBound_CurNode)CurNodePtr = CurNodePtr->Child[0];
				else CurNodePtr = CurNodePtr->Child[1];
			}
			return iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr), this);
		}
		const_iterator find(const Dim_Type LBound, const Dim_Type RBound) const{
			_treetype_intrvtreemap::node_type *CurNodePtr = this->BaseTree.lpRoot;
			while (nullptr != CurNodePtr){
				Dim_Type LBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
				if (LBound == LBound_CurNode){
					Dim_Type RBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[1];
					if (RBound == RBound_CurNode)break;
					else if (RBound < RBound_CurNode)CurNodePtr = CurNodePtr->Child[0];
					else CurNodePtr = CurNodePtr->Child[1];
				}
				else if (LBound < LBound_CurNode)CurNodePtr = CurNodePtr->Child[0];
				else CurNodePtr = CurNodePtr->Child[1];
			}
			return const_iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr), this);
		}
		UBINT find_point_intersect(const Dim_Type pos, Vector<iterator> *lpRetValue){
			_treetype_intrvtreemap::node_type *CurNodePtr = this->BaseTree.lpRoot;
			Vector<IntrvTreeMapNode<Dim_Type, T> *> Stack;
			UBINT IntrvCount = 0;

			if (nullptr != CurNodePtr){
				Dim_Type MinLBound_CurNode = nsMath::NumericTrait<Dim_Type>::Min;
				while (true){
					Dim_Type LBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
					Dim_Type RBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[1];
					Dim_Type MaxRBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->MaxRBound;
					if (LBound_CurNode <= pos && RBound_CurNode > pos){
						IntrvCount++;
						lpRetValue->push_back(iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr), this));
					}
					if (MinLBound_CurNode <= pos && MaxRBound_CurNode > pos){
						if (nullptr != CurNodePtr->Child[0]){
							Stack.push_back(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr));
							CurNodePtr = CurNodePtr->Child[0];
							continue;
						}
						else if (nullptr != CurNodePtr->Child[1]){
							MinLBound_CurNode = LBound_CurNode;
							CurNodePtr = CurNodePtr->Child[1];
							continue;
						}
					}
					do{
						if (Stack.empty()){
							CurNodePtr = nullptr;
							break;
						}
						CurNodePtr = Stack.back();
						Stack.pop_back();
						MinLBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
						CurNodePtr = CurNodePtr->Child[1];
					} while (nullptr == CurNodePtr);
					if (nullptr == CurNodePtr)break;
				}
			}
			return IntrvCount;
		}
		Vector<iterator> find_point_intersect(const Dim_Type pos){
			Vector<iterator> RetValue;
			this->find_point_intersect(pos, &RetValue);
			return std::move(RetValue);
		}
		UBINT find_intrv_intersect(const Dim_Type LBound, const Dim_Type RBound, Vector<iterator> *lpRetValue){
			_treetype_intrvtreemap::node_type *CurNodePtr = this->BaseTree.lpRoot;
			Vector<IntrvTreeMapNode<Dim_Type, T> *> Stack;
			UBINT IntrvCount = 0;

			if (nullptr != CurNodePtr){
				Dim_Type MinLBound_CurNode = nsMath::NumericTrait<Dim_Type>::Min;
				while (true){
					Dim_Type LBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
					Dim_Type RBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[1];
					Dim_Type MaxRBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->MaxRBound;
					Dim_Type LBound_Intersection, RBound_Intersection;

					LBound_Intersection = LBound_CurNode > LBound ? LBound_CurNode : LBound;
					RBound_Intersection = RBound_CurNode < LBound ? RBound_CurNode : RBound;
					if (LBound_Intersection < RBound_Intersection){
						IntrvCount++;
						lpRetValue->push_back(iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr), this));
					}

					LBound_Intersection = MinLBound_CurNode > LBound ? MinLBound_CurNode : LBound;
					RBound_Intersection = MaxRBound_CurNode < LBound ? MaxRBound_CurNode : RBound;
					if (LBound_Intersection < RBound_Intersection){
						if (nullptr != CurNodePtr->Child[0]){
							Stack.push_back(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr));
							CurNodePtr = CurNodePtr->Child[0];
							continue;
						}
						else if (nullptr != CurNodePtr->Child[1]){
							MinLBound_CurNode = LBound_CurNode;
							CurNodePtr = CurNodePtr->Child[1];
							continue;
						}
					}
					do{
						if (Stack.empty()){
							CurNodePtr = nullptr;
							break;
						}
						CurNodePtr = Stack.back();
						Stack.pop_back();
						MinLBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
						CurNodePtr = CurNodePtr->Child[1];
					} while (nullptr == CurNodePtr);
					if (nullptr == CurNodePtr)break;
				}
			}
			return IntrvCount;
		}
		Vector<iterator> find_intrv_intersect(const Dim_Type LBound, const Dim_Type RBound){
			Vector<iterator> RetValue;
			this->find_intrv_intersect(LBound, RBound, &RetValue);
			return std::move(RetValue);
		}
		UBINT find_intrv_subset(const Dim_Type LBound, const Dim_Type RBound, Vector<iterator> *lpRetValue){
			_treetype_intrvtreemap::node_type *CurNodePtr = this->BaseTree.lpRoot;
			Vector<IntrvTreeMapNode<Dim_Type, T> *> Stack;
			UBINT IntrvCount = 0;

			if (nullptr != CurNodePtr){
				Dim_Type MinLBound_CurNode = nsMath::NumericTrait<Dim_Type>::Min;
				while (true){
					Dim_Type LBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
					Dim_Type RBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[1];
					Dim_Type MaxRBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->MaxRBound;
					if (LBound <= LBound_CurNode && RBound >= RBound_CurNode){
						IntrvCount++;
						lpRetValue->push_back(iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr), this));
					}

					Dim_Type LBound_Intersection = MinLBound_CurNode > LBound ? MinLBound_CurNode : LBound;
					Dim_Type RBound_Intersection = MaxRBound_CurNode < LBound ? MaxRBound_CurNode : RBound;
					if (LBound_Intersection < RBound_Intersection){
						if (nullptr != CurNodePtr->Child[0]){
							Stack.push_back(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr));
							CurNodePtr = CurNodePtr->Child[0];
							continue;
						}
						else if (nullptr != CurNodePtr->Child[1]){
							MinLBound_CurNode = LBound_CurNode;
							CurNodePtr = CurNodePtr->Child[1];
							continue;
						}
					}
					do{
						if (Stack.empty()){
							CurNodePtr = nullptr;
							break;
						}
						CurNodePtr = Stack.back();
						Stack.pop_back();
						MinLBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
						CurNodePtr = CurNodePtr->Child[1];
					} while (nullptr == CurNodePtr);
					if (nullptr == CurNodePtr)break;
				}
			}
			return IntrvCount;
		}
		Vector<iterator> find_intrv_subset(const Dim_Type LBound, const Dim_Type RBound){
			Vector<iterator> RetValue;
			this->find_intrv_subset(LBound, RBound, &RetValue);
			return std::move(RetValue);
		}
		UBINT find_intrv_superset(const Dim_Type LBound, const Dim_Type RBound, Vector<iterator> *lpRetValue){
			_treetype_intrvtreemap::node_type *CurNodePtr = this->BaseTree.lpRoot;
			Vector<IntrvTreeMapNode<Dim_Type, T> *> Stack;
			UBINT IntrvCount = 0;

			if (nullptr != CurNodePtr){
				Dim_Type MinLBound_CurNode = nsMath::NumericTrait<Dim_Type>::Min;
				while (true){
					Dim_Type LBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
					Dim_Type RBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[1];
					Dim_Type MaxRBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->MaxRBound;
					if (LBound_CurNode <= LBound && RBound_CurNode >= RBound){
						IntrvCount++;
						lpRetValue->push_back(iterator(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr), this));
					}

					if (MinLBound_CurNode <= LBound && MaxRBound_CurNode >= RBound){
						if (nullptr != CurNodePtr->Child[0]){
							Stack.push_back(static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr));
							CurNodePtr = CurNodePtr->Child[0];
							continue;
						}
						else if (nullptr != CurNodePtr->Child[1]){
							MinLBound_CurNode = LBound_CurNode;
							CurNodePtr = CurNodePtr->Child[1];
							continue;
						}
					}
					do{
						if (Stack.empty()){
							CurNodePtr = nullptr;
							break;
						}
						CurNodePtr = Stack.back();
						Stack.pop_back();
						MinLBound_CurNode = static_cast<IntrvTreeMapNode<Dim_Type, T> *>(CurNodePtr)->Data.Interval[0];
						CurNodePtr = CurNodePtr->Child[1];
					} while (nullptr == CurNodePtr);
					if (nullptr == CurNodePtr)break;
				}
			}
			return IntrvCount;
		}
		Vector<iterator> find_intrv_superset(const Dim_Type LBound, const Dim_Type RBound){
			Vector<iterator> RetValue;
			this->find_intrv_superset(LBound, RBound, &RetValue);
			return std::move(RetValue);
		}

		inline ~IntrvTreeMap(){ clear(); }
	};
}

#endif