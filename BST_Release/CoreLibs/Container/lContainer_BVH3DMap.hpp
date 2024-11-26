/* Description: STL-style bounding volume hierarchy implementation. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_BVH_3D_MAP
#define LIB_CONTAINER_BVH_3D_MAP

#include "lGeneral.hpp"
#include "lContainer_BinaryTree.hpp"
#include "lMath_CG.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	template < typename Dim_Type, typename T > struct BVHNode_3D{
		nsMath::AABB_3D<Dim_Type> Box;
		BVHNode_3D<Dim_Type, T> *Parent;
		BVHNode_3D<Dim_Type, T> *Child[2];
	};

	template < typename Dim_Type, typename T > struct BVHLeaf_3D: public BVHNode_3D<Dim_Type, T>{
		T Data;
	};

	template < typename Dim_Type, typename T > struct BVH3DMapIterator{
		// custom typedefs
		typedef BVH3DMapIterator<Dim_Type, T>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::forward_iterator_tag	iterator_category; //#0

		typedef typename T				value_type; //#1
		typedef typename ptrdiff_t		difference_type; //#2
		typedef typename value_type*	pointer; //#3
		typedef typename value_type&	reference; //#4

		BVHLeaf_3D<Dim_Type, T> *lpNode;

		BVH3DMapIterator() {}
		BVH3DMapIterator(BVHLeaf_3D<Dim_Type, T> * Node) : lpNode(Node){}

		inline bool operator==(const this_type& rhs) const { return this->lpNode == rhs.lpNode; }
		inline bool operator!=(const this_type& rhs) const { return this->lpNode != rhs.lpNode; }

		inline reference operator*() const { return lpNode->Data; }
		inline pointer operator->() const { return &(lpNode->Data); }

		inline nsMath::AABB_3D<Dim_Type> &Box() const { return lpNode->Box; }

		inline this_type& operator++(){
			this->lpNode = binary_tree_next(this->lpNode);
			return *this;
		}
		inline this_type operator++(int){
			this_type TmpIt = *this;
			++*this;
			return TmpIt;
		}
	};

	template < typename Dim_Type, typename T > struct BVH3DMapIterator_Const{
		// custom typedefs
		typedef BVH3DMapIterator<Dim_Type, T>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::forward_iterator_tag	iterator_category; //#0

		typedef typename T					value_type; //#1
		typedef typename ptrdiff_t			difference_type; //#2
		typedef typename const value_type*	pointer; //#3
		typedef typename const value_type&	reference; //#4

		const BVHLeaf_3D<Dim_Type, T> *lpNode;

		BVH3DMapIterator_Const() {}
		BVH3DMapIterator_Const(const BVHLeaf_3D<Dim_Type, T> * Node) : lpNode(Node){}
		BVH3DMapIterator_Const(const BVH3DMapIterator<Dim_Type, T>& rhs) : lpNode(rhs.lpNode){}

		inline bool operator==(const this_type& rhs) const { return this->lpNode == rhs.lpNode; }
		inline bool operator!=(const this_type& rhs) const { return this->lpNode != rhs.lpNode; }

		inline reference operator*() const { return lpNode->Data; }
		inline pointer operator->() const { return &(lpNode->Data); }

		inline const nsMath::AABB_3D<Dim_Type> &Box() const { return lpNode->Box; }

		inline this_type& operator++(){
			this->lpNode = binary_tree_next(this->lpNode);
			return *this;
		}
		inline this_type operator++(int){
			this_type TmpIt = *this;
			++*this;
			return TmpIt;
		}
	};

	template < typename Dim_Type, typename T, typename A = std::allocator<T> > class BVH3DMap;

	template < typename Dim_Type, typename T, typename A > class BVH3DMap{
	public:
		// custom typedefs
		typedef BVH3DMap<Dim_Type, T, A>		this_type;
		typedef BVHNode_3D<Dim_Type, T>			subtree_type;
		typedef BVHLeaf_3D<Dim_Type, T>			leaf_type;

		typedef typename A::template rebind<BVHNode_3D<Dim_Type, T>>::other	allocator_type_subtree;
		typedef typename A::template rebind<BVHLeaf_3D<Dim_Type, T>>::other	allocator_type_leaf;

		// container typedefs, mandated by the STL standard
		typedef typename A					allocator_type;

		typedef typename T					value_type;
		typedef typename value_type&		reference;
		typedef typename const value_type&	const_reference;
		typedef typename value_type*		pointer;		//diffrent from the C++ STL standard.
		typedef typename const value_type*	const_pointer;	//diffrent from the C++ STL standard.
		typedef typename size_t				size_type;
		typedef typename ptrdiff_t			difference_type;

		typedef typename nsMath::AABB_3D<Dim_Type>	key_type;
		typedef typename T							mapped_type;

		// container typedefs of iterator, mandated by the STL standard
		typedef BVH3DMapIterator<Dim_Type, T>		iterator;
		typedef BVH3DMapIterator_Const<Dim_Type, T>	const_iterator;

		friend struct BVH3DMapIterator<Dim_Type, T>;
		friend struct BVH3DMapIterator_Const<Dim_Type, T>;

	protected:
		allocator_type_subtree	Allocator_SubTree;
		allocator_type_leaf		Allocator_Leaf;

		size_type					NodeCount;
		BVHNode_3D<Dim_Type, T> *	lpRoot;

		// custom methods
		void clear_node(subtree_type *lpNode){
			if (nullptr == lpNode->Child[0] /* && nullptr == lpNode->Child[1] */ ){ // BVH is always a full binary tree.
				// it's a leaf.
				leaf_type *lpLeaf = static_cast<leaf_type *>(lpNode);
				Allocator_Leaf.destroy(&lpLeaf->Data); //since C++11
				Allocator_Leaf.deallocate(lpLeaf, 1);
			}
			else{
				clear_node(static_cast<MapNode<Key, T> *>(lpNode->Child[0]));
				clear_node(static_cast<MapNode<Key, T> *>(lpNode->Child[1]));
				Allocator_SubTree.deallocate(lpNode, 1);
			}
		}
		void box_reduce(subtree_type *lpNode){
			while (nullptr != lpNode->Parent){
				lpNode = lpNode->Parent;
				nsMath::AABB_3D_Merge(&lpNode->Parent->Box, &lpNode->Parent->Child[0]->Box, &lpNode->Parent->Child[1]->Box);
			}
		}

	public:
		// constructors
		explicit BVH3DMap(const allocator_type_subtree& _SubTree_Allocator = allocator_type_subtree(), const allocator_type_leaf& _Leaf_Allocator = allocator_type_leaf())
			:Allocator_SubTree(_SubTree_Allocator), Allocator_Leaf(_Leaf_Allocator), lpRoot(nullptr){
			this->NodeCount = 0;
		}

		// iterator functions, mandated by the STL standard
		iterator begin(){
			if (nullptr == this->lpRoot)return iterator(nullptr);
			else{
				subtree_type *CurNode = this->lpRoot;
				while (nullptr != CurNode->Child[0])CurNode = CurNode->Child[0];
				return iterator(static_cast<leaf_type *>(CurNode));
			}
		}
		inline const_iterator cbegin() const {
			if (nullptr == this->lpRoot)return const_iterator(nullptr);
			else{
				subtree_type *CurNode = this->lpRoot;
				while (nullptr != CurNode->Child[0])CurNode = CurNode->Child[0];
				return const_iterator(static_cast<leaf_type *>(CurNode));
			}
		}
		inline iterator end(){ return iterator(nullptr); }
		inline const_iterator cend() const { return const_iterator(nullptr); }
		inline const subtree_type * root() const { return this->lpRoot; }

		// capacity functions
		inline bool empty() const { return this->NodeCount == 0; }
		inline size_type size() const { return this->NodeCount; }

		// modifiers
		inline void clear(){
			if (nullptr != this->lpRoot)clear_node(this->lpRoot);
			this->lpRoot = nullptr;
			this->NodeCount = 0;
		}

		template< typename ... Args > std::pair<iterator, bool> try_emplace(
			const Dim_Type x_min, const Dim_Type y_min, const Dim_Type z_min,
			const Dim_Type x_max, const Dim_Type y_max, const Dim_Type z_max,
			Args&&... args){

			//first we need to construct the leaf node.
			leaf_type *NewLeaf;
			try{ NewLeaf = Allocator_Leaf.allocate(1); }
			catch (...){
				return std::pair<iterator, bool>(iterator(nullptr), false);
			}

			if (nullptr == this->lpRoot){
				try{ Allocator_Leaf.construct(&NewLeaf->Data, std::forward<Args>(args)...); } //since C++11
				catch (...){
					Allocator_Leaf.deallocate(NewLeaf, 1);
					return std::pair<iterator, bool>(iterator(nullptr), false);
				}

				NewLeaf->Box.Min[0] = x_min; NewLeaf->Box.Min[1] = y_min; NewLeaf->Box.Min[2] = z_min;
				NewLeaf->Box.Max[0] = x_max; NewLeaf->Box.Max[1] = y_max; NewLeaf->Box.Max[2] = z_max;

				this->lpRoot = NewLeaf;

				NewLeaf->Parent = NewSubTree;
			}
			else{
				subtree_type *NewSubTree, *lpParent, **lpCurNodePtr;

				try{ NewSubTree = Allocator_SubTree.allocate(1); }
				catch (...){
					Allocator_Leaf.deallocate(NewLeaf, 1);
					return std::pair<iterator, bool>(iterator(nullptr), false);
				}

				try{ Allocator_Leaf.construct(&NewLeaf->Data, std::forward<Args>(args)...); } //since C++11
				catch (...){
					Allocator_SubTree.deallocate(NewSubTree, 1);
					Allocator_Leaf.deallocate(NewLeaf, 1);
					return std::pair<iterator, bool>(iterator(nullptr), false);
				}

				NewLeaf->Box.Min[0] = x_min; NewLeaf->Box.Min[1] = y_min; NewLeaf->Box.Min[2] = z_min;
				NewLeaf->Box.Max[0] = x_max; NewLeaf->Box.Max[1] = y_max; NewLeaf->Box.Max[2] = z_max;

				Dim_Type Half_Surface_Area_N = nsMath::AABB_3D_Half_Surface_Area(&NewLeaf->Box);
				Dim_Type Half_Surface_Area_LR = nsMath::AABB_3D_Half_Surface_Area(&this->lpRoot->Box);
				lpCurNodePtr = &this->lpRoot;
				lpParent = nullptr;

				while (nullptr != (*lpCurNodePtr)->Child[0]){ // while (*lpCurNodePtr) is not a leaf
					subtree_type *lpCurNode = *lpCurNodePtr;

					//compute the cost of node merging
					Dim_Type Half_Surface_Area_L = nsMath::AABB_3D_Half_Surface_Area(&lpCurNode->Child[0]->Box);
					Dim_Type Half_Surface_Area_R = nsMath::AABB_3D_Half_Surface_Area(&lpCurNode->Child[1]->Box);

					nsMath::AABB_3D AABB_Tmp;

					nsMath::AABB_3D_Merge(&AABB_Tmp, &NewLeaf->Box, &lpCurNode->Child[0]->Box);
					Dim_Type Half_Surface_Area_LN = nsMath::AABB_3D_Half_Surface_Area(&AABB_Tmp);
					nsMath::AABB_3D_Merge(&AABB_Tmp, &NewLeaf->Box, &lpCurNode->Child[1]->Box);
					Dim_Type Half_Surface_Area_RN = nsMath::AABB_3D_Half_Surface_Area(&AABB_Tmp);

					if (Half_Surface_Area_R + Half_Surface_Area_LN < Half_Surface_Area_L + Half_Surface_Area_RN){
						if (Half_Surface_Area_N + Half_Surface_Area_LR < Half_Surface_Area_R + Half_Surface_Area_LN)break;
						else{
							lpCurNodePtr = &lpCurNode->Child[0];
							lpParent = lpCurNode;
							Half_Surface_Area_LR = nsMath::AABB_3D_Half_Surface_Area(&lpCurNode->Box);
						}
					}
					else{ // Half_Surface_Area_L + Half_Surface_Area_RN < Half_Surface_Area_R + Half_Surface_Area_LN
						if (Half_Surface_Area_N + Half_Surface_Area_LR < Half_Surface_Area_L + Half_Surface_Area_RN)break;
						else{
							lpCurNodePtr = &lpCurNode->Child[1];
							lpParent = lpCurNode;
							Half_Surface_Area_LR = nsMath::AABB_3D_Half_Surface_Area(&lpCurNode->Box);
						}
					}
				}

				NewSubTree->Parent = lpParent;
				NewSubTree->Child[0] = *lpCurNodePtr;
				NewSubTree->Child[1] = NewLeaf;
				*lpCurNodePtr = NewSubTree;
				nsMath::AABB_3D_Merge(&AABB_Tmp, &NewSubTree->Child[0]->Box, &NewSubTree->Child[1]->Box);

				NewLeaf->Parent = NewSubTree;	
			}
			NewLeaf->Child[0] = nullptr;
			NewLeaf->Child[1] = nullptr;

			this->NodeCount++;
			return std::pair<iterator, bool>(iterator(NewLeaf), true);
		}
		size_t erase(const_iterator pos){
			if (pos == this->cend())return 0;
			else if(this->lpRoot == pos->lpNode){
				Allocator_Leaf.destroy(&pos->lpNode->Data); //since C++11
				Allocator_Leaf.deallocate(pos->lpNode, 1);

				this->lpRoot = nullptr;
				this->NodeCount = 0;
				return 1;
			}
			else{
				subtree_type *lpSibling;

				subtree_type *lpParent = pos->lpNode->Parent;
				if (pos->lpNode == lpParent->Child[0])lpSibling = lpParent->Child[1]; else lpSibling = lpParent->Child[0];

				subtree_type *lpParentPtr;
				if (nullptr == lpParent->Parent)lpParentPtr = &this->lpRoot;
				else if (lpParent->Parent->Child[0] == lpParent)lpParentPtr = &lpParent->Parent->Child[0];
				else lpParentPtr = &lpParent->Parent->Child[1];

				*lpParentPtr = lpSibling;

				Allocator_Leaf.destroy(&pos->lpNode->Data); //since C++11
				Allocator_Leaf.deallocate(pos->lpNode, 1);

				Allocator_SubTree.deallocate(lpParent, 1);

				box_reduce(lpSibling);
				return 1;
			}
		}

		//query functions
		void find_traverse(bool (*callback_box_intersect)(const nsMath::AABB_3D<Dim_Type> *, void *), bool (*callback_leaf_intersect)(const T *, void *), void *user_data) const {
			const subtree_type *CurNode = this->lpRoot;
			while (nullptr != CurNode){
				if (nullptr == CurNode->Child[0]){
					if (false == callback_leaf_intersect(&static_cast<const leaf_type *>(CurNode)->Data, user_data))break;
					else{
						while (true){
							if (nullptr == CurNode->Parent)return;
							else if (static_cast<const subtree_type *>(CurNode->Parent->Child[0]) == CurNode)CurNode = CurNode->Parent->Child[1];
							else CurNode = CurNode->Parent;
						}
					}
				}
				else if (callback_box_intersect(&CurNode->Box, user_data))CurNode = CurNode->Child[0];
				else{
					while (true){
						if (nullptr == CurNode->Parent)return;
						else if (static_cast<const subtree_type *>(CurNode->Parent->Child[0]) == CurNode)CurNode = CurNode->Parent->Child[1];
						else CurNode = CurNode->Parent;
					}
				}
			}
		}

		inline ~BVH3DMap(){ clear(); }
	};
}

#endif