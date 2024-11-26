/* Description: A STL-style point octree map class. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_POINTMAP3D_OCTREE
#define LIB_CONTAINER_POINTMAP3D_OCTREE

#include "lGeneral.hpp"
#include "lContainer_MultiDimIndex.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{

	template < typename Dim_Type, typename T > struct PointOctreeMapSubTree{
		Dim_Type Span;

		nsMath::TinyVector<Dim_Type, 3> Position_Center;
		PointOctreeMapSubTree<Dim_Type, T> *lpParent;
		Dim_Type *lpChild[8];
	};

	template < typename Dim_Type, typename T > struct PointOctreeMapLeaf{
		Dim_Type Span;

		PointOctreeMapSubTree<Dim_Type, T> *lpParent;
		PointMap<Dim_Type, T, 3> Content;
	};

	template < typename Dim_Type > struct _PointOctreeMap_GetValidPos{
		static void Func(Dim_Type *lpPosition);
	};

	template <typename Dim_Type> void _PointOctreeMap_GetValidPos<Dim_Type>::Func(Dim_Type *lpPosition){}
	template <> void _PointOctreeMap_GetValidPos<float>::Func(float *lpPosition){
		if (-0.0f == lpPosition[0])lpPosition[0] = 0.0f;
		if (-0.0f == lpPosition[1])lpPosition[1] = 0.0f;
		if (-0.0f == lpPosition[2])lpPosition[2] = 0.0f;
		if ((((UINT4b *)lpPosition)[0] & (nsMath::NumericTrait<float>::HighestBit - 1)) >= nsMath::NumericTrait<float>::Inf_Positive_ByteForm)((UINT4b *)lpPosition)[0] &= nsMath::NumericTrait<float>::HighestBit + nsMath::NumericTrait<float>::Exponent_Mask - 1;
		if ((((UINT4b *)lpPosition)[1] & (nsMath::NumericTrait<float>::HighestBit - 1)) >= nsMath::NumericTrait<float>::Inf_Positive_ByteForm)((UINT4b *)lpPosition)[1] &= nsMath::NumericTrait<float>::HighestBit + nsMath::NumericTrait<float>::Exponent_Mask - 1;
		if ((((UINT4b *)lpPosition)[2] & (nsMath::NumericTrait<float>::HighestBit - 1)) >= nsMath::NumericTrait<float>::Inf_Positive_ByteForm)((UINT4b *)lpPosition)[2] &= nsMath::NumericTrait<float>::HighestBit + nsMath::NumericTrait<float>::Exponent_Mask - 1;
	}
	template <> void _PointOctreeMap_GetValidPos<double>::Func(double *lpPosition){
		if (-0.0 == lpPosition[0])lpPosition[0] = 0.0;
		if (-0.0 == lpPosition[1])lpPosition[1] = 0.0;
		if (-0.0 == lpPosition[2])lpPosition[2] = 0.0;
		if ((((UINT8b *)lpPosition)[0] & (nsMath::NumericTrait<double>::HighestBit - 1)) >= nsMath::NumericTrait<double>::Inf_Positive_ByteForm)((UINT8b *)lpPosition)[0] &= nsMath::NumericTrait<double>::HighestBit + nsMath::NumericTrait<double>::Exponent_Mask - 1;
		if ((((UINT8b *)lpPosition)[1] & (nsMath::NumericTrait<double>::HighestBit - 1)) >= nsMath::NumericTrait<double>::Inf_Positive_ByteForm)((UINT8b *)lpPosition)[1] &= nsMath::NumericTrait<double>::HighestBit + nsMath::NumericTrait<double>::Exponent_Mask - 1;
		if ((((UINT8b *)lpPosition)[2] & (nsMath::NumericTrait<double>::HighestBit - 1)) >= nsMath::NumericTrait<double>::Inf_Positive_ByteForm)((UINT8b *)lpPosition)[2] &= nsMath::NumericTrait<double>::HighestBit + nsMath::NumericTrait<double>::Exponent_Mask - 1;
	}

	template < typename Dim_Type, typename T, UBINT Category_Type > struct _PointOctreeMap_Quadrant{
		static UBINT Func(PointOctreeMapSubTree<Dim_Type, T> *lpSubTree, const Dim_Type *lpPosition){
			UBINT RetValue = 0;
			if (lpPosition[0] < lpSubTree->Position_Center[0]){
				if (lpSubTree->Position_Center[0] - lpSubTree->Span > lpPosition[0])RetValue |= 8;
			}
			else{
				RetValue += 1;
				if (lpSubTree->Position_Center[0] + lpSubTree->Span <= lpPosition[0])RetValue |= 8;
			}
			if (lpPosition[1] < lpSubTree->Position_Center[1]){
				if (lpSubTree->Position_Center[1] - lpSubTree->Span > lpPosition[1])RetValue |= 8;
			}
			else{
				RetValue += 2;
				if (lpSubTree->Position_Center[1] + lpSubTree->Span <= lpPosition[1])RetValue |= 8;
			}
			if (lpPosition[2] < lpSubTree->Position_Center[2]){
				if (lpSubTree->Position_Center[2] - lpSubTree->Span > lpPosition[2])RetValue |= 8;
			}
			else{
				RetValue += 4;
				if (lpSubTree->Position_Center[2] + lpSubTree->Span <= lpPosition[2])RetValue |= 8;
			}
			return RetValue;
		}
	};

	template < typename Dim_Type, typename T > struct _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::TypeID_INT>{
		static UBINT Func(PointOctreeMapSubTree<Dim_Type, T> *lpSubTree, const Dim_Type *lpPosition){
			typedef typename nsMath::NumericTrait<Dim_Type>::uint_type uint_type;
			UBINT RetValue = 0;
			if (lpPosition[0] < lpSubTree->Position_Center[0]){
				if ((uint_type)(lpSubTree->Position_Center[0] - lpPosition[0]) > (uint_type)(lpSubTree->Span))RetValue |= 8;
			}
			else{
				RetValue += 1;
				if ((uint_type)(lpPosition[0] - lpSubTree->Position_Center[0]) >= (uint_type)(lpSubTree->Span))RetValue |= 8;
			}
			if (lpPosition[1] < lpSubTree->Position_Center[1]){
				if ((uint_type)(lpSubTree->Position_Center[1] - lpPosition[1]) > (uint_type)(lpSubTree->Span))RetValue |= 8;
			}
			else{
				RetValue += 2;
				if ((uint_type)(lpPosition[1] - lpSubTree->Position_Center[1]) >= (uint_type)(lpSubTree->Span))RetValue |= 8;
			}
			if (lpPosition[2] < lpSubTree->Position_Center[2]){
				if ((uint_type)(lpSubTree->Position_Center[2] - lpPosition[2]) > (uint_type)(lpSubTree->Span))RetValue |= 8;
			}
			else{
				RetValue += 4;
				if ((uint_type)(lpPosition[2] - lpSubTree->Position_Center[2]) >= (uint_type)(lpSubTree->Span))RetValue |= 8;
			}
			return RetValue;
		}
	};

	template < typename Dim_Type, typename T, UBINT Category_Type > struct _PointOctreeMap_NewSubTree{
		static void Func(PointOctreeMapSubTree<Dim_Type, T> *lpSubTree, const Dim_Type *lpPosition1, const Dim_Type *lpPosition2){
			Dim_Type NewSpan = (Dim_Type)0, TmpSpan;
			TmpSpan = _roundint2(lpPosition1[0], lpPosition2[0]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
			TmpSpan = _roundint2(lpPosition1[1], lpPosition2[1]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
			TmpSpan = _roundint2(lpPosition1[2], lpPosition2[2]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
			lpSubTree->Position_Center[0] = _roundint(lpPosition1[0], NewSpan);
			lpSubTree->Position_Center[1] = _roundint(lpPosition1[1], NewSpan);
			lpSubTree->Position_Center[2] = _roundint(lpPosition1[2], NewSpan);
			lpSubTree->Span = NewSpan;

			for (UBINT i = 0; i < 8; i++)lpSubTree->lpChild[i] = 0;
		}
	};

	template < typename Dim_Type, typename T > struct _PointOctreeMap_NewSubTree<Dim_Type, T, nsMath::TypeID_FLOAT>{
		static void Func(PointOctreeMapSubTree<Dim_Type, T> *lpSubTree, const Dim_Type *lpPosition1, const Dim_Type *lpPosition2){
			Dim_Type NewSpan = (Dim_Type)0, TmpSpan;
			TmpSpan = _roundfloat2(lpPosition1[0], lpPosition2[0]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
			TmpSpan = _roundfloat2(lpPosition1[1], lpPosition2[1]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
			TmpSpan = _roundfloat2(lpPosition1[2], lpPosition2[2]); if (NewSpan < TmpSpan)NewSpan = TmpSpan;
			lpSubTree->Position_Center[0] = _roundfloat(lpPosition1[0], NewSpan);
			lpSubTree->Position_Center[1] = _roundfloat(lpPosition1[1], NewSpan);
			lpSubTree->Position_Center[2] = _roundfloat(lpPosition1[2], NewSpan);
			lpSubTree->Span = NewSpan;

			for (UBINT i = 0; i < 8; i++)lpSubTree->lpChild[i] = 0;
		}
	};

	template < typename Dim_Type, typename T, typename A = std::allocator<PointMap<Dim_Type, T, 3> > > class PointOctreeMap;
	template < typename Dim_Type, typename T > struct PointOctreeMapIterator_Const;

	template < typename Dim_Type, typename T > struct PointOctreeMapIterator{
	protected:
		typedef PointOctreeMapSubTree<Dim_Type, T>	subtree_type;
		typedef PointOctreeMapLeaf<Dim_Type, T>		leaf_type;

		static inline subtree_type * _GetSubTree(const Dim_Type *lpSpan){ return (subtree_type *)((UBINT)lpSpan - offsetof(subtree_type, Span)); };
		static inline leaf_type *_GetLeaf(const Dim_Type *lpSpan){ return (leaf_type *)((UBINT)lpSpan - offsetof(leaf_type, Span)); }
	
		leaf_type *lpNode;
	public:
		// custom typedefs
		typedef PointOctreeMapIterator<Dim_Type, T>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::forward_iterator_tag	iterator_category; //#0

		typedef typename PointMap<Dim_Type, T, 3>	value_type; //#1
		typedef typename ptrdiff_t					difference_type; //#2
		typedef typename value_type*				pointer; //#3
		typedef typename value_type&				reference; //#4
		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		friend struct PointOctreeMapIterator_Const<Dim_Type, T>;

		PointOctreeMapIterator(leaf_type *Node) : lpNode(Node){}
		PointOctreeMapIterator() :lpNode(nullptr) {}

		inline leaf_type *_GetNode(){ return this->lpNode; }

		inline bool operator==(const this_type& rhs) const { return this->lpNode == rhs.lpNode; }
		inline bool operator!=(const this_type& rhs) const { return this->lpNode != rhs.lpNode; }

		inline reference operator*() const { return lpNode->Content; }
		inline pointer operator->() const { return &(lpNode->Content); }

		this_type& operator++(){
			if (nullptr != lpNode){
				subtree_type *lpParent = lpNode->lpParent;
				Dim_Type *lpHandle = &lpNode->Span;
				while (true){
					if (nullptr == lpParent){
						lpNode = nullptr;
						return *this;
					}
					else{
						//find the node index
						UBINT i;
						for (i = 0; i < 8; i++){
							if (lpHandle == lpParent->lpChild[i])break;
						}
						for (i++; i < 8; i++){
							if (lpParent->lpChild[i] != nullptr)break;
						}
						if (i < 8){
							if ((Dim_Type)0 == *lpParent->lpChild[i]){
								lpNode = _GetLeaf(lpParent->lpChild[i]);
								break;
							}
							else{
								lpParent = _GetSubTree(lpParent->lpChild[i]);
								while (true){
									for (UBINT i = 0; i < 8; i++){
										if (nullptr != lpParent->lpChild[i]){
											if ((Dim_Type)0 == *lpParent->lpChild[i]){
												lpNode = _GetLeaf(lpParent->lpChild[i]);
												return *this;
											}
											else{
												lpParent = _GetSubTree(lpParent->lpChild[i]);
												break;
											}
										}
									}
								}
							}
						}
						else{
							lpHandle = &lpParent->Span;
							lpParent = lpParent->lpParent;
						}
					}
				}
			}
			return *this;
		}
		this_type operator++(int){
			this_type TmpIt = *this;
			++*this;
			return TmpIt;
		}
	};

	template < typename Dim_Type, typename T > struct PointOctreeMapIterator_Const{
	protected:
		typedef PointOctreeMapSubTree<Dim_Type, T>	subtree_type;
		typedef PointOctreeMapLeaf<Dim_Type, T>		leaf_type;

		static inline subtree_type * _GetSubTree(const Dim_Type *lpSpan){ return (subtree_type *)((UBINT)lpSpan - offsetof(subtree_type, Span)); };
		static inline leaf_type *_GetLeaf(const Dim_Type *lpSpan){ return (leaf_type *)((UBINT)lpSpan - offsetof(leaf_type, Span)); }

		const leaf_type *lpNode;
	public:
		// custom typedefs
		typedef PointOctreeMapIterator_Const<Dim_Type, T>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::forward_iterator_tag		iterator_category; //#0

		typedef typename const PointMap<Dim_Type, T, 3>	value_type; //#1
		typedef typename ptrdiff_t						difference_type; //#2
		typedef typename const value_type*				pointer; //#3
		typedef typename const value_type&				reference; //#4
		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		PointOctreeMapIterator_Const(const leaf_type *Node) : lpNode(Node){}
		PointOctreeMapIterator_Const() :lpNode(nullptr) {}
		PointOctreeMapIterator_Const(const PointOctreeMapIterator<Dim_Type, T>& rhs) : lpNode(rhs.lpNode){}

		inline const leaf_type *_GetNode(){ return this->lpNode; }

		inline bool operator==(const this_type& rhs) const { return this->lpNode == rhs.lpNode; }
		inline bool operator!=(const this_type& rhs) const { return this->lpNode != rhs.lpNode; }

		inline reference operator*() const { return lpNode->Content; }
		inline pointer operator->() const { return &(lpNode->Content); }

		this_type& operator++(){
			if (nullptr != lpNode){
				const subtree_type *lpParent = lpNode->lpParent;
				const Dim_Type *lpHandle = &lpNode->Span;
				while (true){
					if (nullptr == lpParent){
						lpNode = nullptr;
						return *this;
					}
					else{
						//find the node index
						UBINT i;
						for (i = 0; i < 8; i++){
							if (lpHandle == lpParent->lpChild[i])break;
						}
						for (i++; i < 8; i++){
							if (lpParent->lpChild[i] != nullptr)break;
						}
						if (i < 8){
							if ((Dim_Type)0 == *lpParent->lpChild[i]){
								lpNode = _GetLeaf(lpParent->lpChild[i]);
								break;
							}
							else{
								lpParent = _GetSubTree(lpParent->lpChild[i]);
								while (true){
									for (UBINT i = 0; i < 8; i++){
										if (nullptr != lpParent->lpChild[i]){
											if ((Dim_Type)0 == *lpParent->lpChild[i]){
												lpNode = _GetLeaf(lpParent->lpChild[i]);
												return *this;
											}
											else{
												lpParent = _GetSubTree(lpParent->lpChild[i]);
												break;
											}
										}
									}
								}
							}
						}
						else{
							lpHandle = &lpParent->Span;
							lpParent = lpParent->lpParent;
						}
					}
				}
			}
			return *this;
		}
		this_type operator++(int){
			this_type TmpIt = *this;
			++*this;
			return TmpIt;
		}
	};

	template < typename Dim_Type, typename T, typename A > class PointOctreeMap{
	public:
		// custom typedefs

		typedef PointOctreeMap<Dim_Type, T, A>		this_type;
		typedef PointOctreeMapSubTree<Dim_Type, T>	subtree_type;
		typedef PointOctreeMapLeaf<Dim_Type, T>		leaf_type;

		typedef typename A::template rebind<subtree_type>::other	allocator_type_subtree;
		typedef typename A::template rebind<leaf_type>::other		allocator_type_leaf;

		// container typedefs, mandated by the STL standard

		typedef typename PointMap<Dim_Type, T, 3>	value_type;
		typedef typename value_type&				reference;
		typedef typename const value_type&			const_reference;
		typedef typename value_type*				pointer;
		typedef typename const value_type*			const_pointer;
		typedef typename size_t						size_type;
		typedef typename ptrdiff_t					difference_type;

		typedef typename nsMath::TinyVector<Dim_Type, 3>	key_type;
		typedef typename T									mapped_type;

		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		// container typedefs of iterator, mandated by the STL standard
		typedef PointOctreeMapIterator<Dim_Type, T>			iterator;
		typedef PointOctreeMapIterator_Const<Dim_Type, T>	const_iterator;

		friend struct PointOctreeMapIterator<Dim_Type, T>;
		friend struct PointOctreeMapIterator_Const<Dim_Type, T>;

	protected:
		allocator_type_subtree	Allocator_SubTree;
		allocator_type_leaf		Allocator_Leaf;
		
		size_type		NodeCount;
		Dim_Type *		lpRoot;

		static inline subtree_type * _GetSubTree(const Dim_Type *lpSpan){ return (subtree_type *)((UBINT)lpSpan - offsetof(subtree_type, Span)); };
		static inline leaf_type *_GetLeaf(const Dim_Type *lpSpan){ return (leaf_type *)((UBINT)lpSpan - offsetof(leaf_type, Span)); }

		//PointOctreeMap<Dim_Type, T, A>& operator =(const PointOctreeMap<Dim_Type, T, A>&) = delete;

		//custom methods
		void clear_leaf(leaf_type *lpLeaf){
			Allocator_Leaf.destroy(&lpLeaf->Content.Data); //since C++11
			Allocator_Leaf.deallocate(lpLeaf, 1);
		}
		void clear_subtree(subtree_type *lpSubTree){
			for (UBINT i = 0; i < 8; i++){
				if (nullptr != lpSubTree->lpChild[i]){
					if ((Dim_Type)0 == *lpSubTree->lpChild[i])clear_leaf(_GetLeaf(lpSubTree->lpChild[i]));
					else clear_subtree(_GetSubTree(lpSubTree->lpChild[i]));
				}
			}
			Allocator_SubTree.deallocate(lpSubTree, 1);
		}
	public:
		//constructors
		explicit PointOctreeMap(const allocator_type_subtree& _SubTree_Allocator = allocator_type_subtree(), const allocator_type_leaf& _Leaf_Allocator = allocator_type_leaf())
			:Allocator_SubTree(_SubTree_Allocator), Allocator_Leaf(_Leaf_Allocator), lpRoot(nullptr){
			this->NodeCount = 0;
		}

		// iterator functions, mandated by the STL standard
		iterator begin(){
			if (nullptr == this->lpRoot)return iterator(nullptr);
			else{
				Dim_Type *lpChild = this->lpRoot;
				while ((Dim_Type)0 != *lpChild){
					subtree_type *lpSubTree = this->_GetSubTree(lpChild);
					for (UBINT i = 0; i < 8; i++){
						if (nullptr != lpSubTree->lpChild[i]){
							lpChild = lpSubTree->lpChild[i];
							break;
						}
					}
				}
				return iterator(this->_GetLeaf(lpChild));
			}
		}
		const_iterator cbegin() const{
			if (nullptr == this->lpRoot)return const_iterator(nullptr);
			else{
				Dim_Type *lpChild = this->lpRoot;
				while ((Dim_Type)0 != *lpChild){
					subtree_type *lpSubTree = this->_GetSubTree(lpChild);
					for (UBINT i = 0; i < 8; i++){
						if (nullptr != lpSubTree->lpChild[i]){
							lpChild = lpSubTree->lpChild[i];
							break;
						}
					}
				}
				return const_iterator(this->_GetLeaf(lpChild));
			}
		}
		inline iterator end(){ return iterator(); }
		inline const_iterator cend() const{ return const_iterator(); }

		//capacity functions
		inline bool empty() const { return NodeCount == 0; }
		inline size_type size() const { return NodeCount; }

		//modifiers
		void clear(){
			if (nullptr != this->lpRoot){
				if ((Dim_Type)0 == *this->lpRoot)clear_leaf(_GetLeaf(this->lpRoot));
				else clear_subtree(_GetSubTree(this->lpRoot));
			}
			this->lpRoot = nullptr;
			this->NodeCount = 0;
		}

		template< typename ... Args > std::pair<iterator, bool> try_emplace(const Dim_Type x, const Dim_Type y, const Dim_Type z, Args&&... args){
			//first we need to construct the leaf node.
			leaf_type *NewNode = Allocator_Leaf.allocate(1);
			NewNode->Span = (Dim_Type)0;
			Dim_Type *lpPosition = (Dim_Type *)NewNode->Content.Position.Data;
			lpPosition[0] = x; lpPosition[1] = y; lpPosition[2] = z;
			_PointOctreeMap_GetValidPos<Dim_Type>::Func(lpPosition);

			Dim_Type *lpParent = nullptr, **lpChildPtr = &this->lpRoot;
			while (nullptr != *lpChildPtr && **lpChildPtr != Dim_Type(0)){
				PointOctreeMapSubTree<Dim_Type, T> *lpSubTree = this->_GetSubTree(*lpChildPtr);
				UBINT Quadrant = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpSubTree, lpPosition);
				if (Quadrant >= 8)break;
				else{
					lpParent = *lpChildPtr;
					lpChildPtr = &lpSubTree->lpChild[Quadrant];
				}
			}

			if (nullptr == *lpChildPtr){
				//the new node will be placed here
				try{
					new((void *)&NewNode->Content.Data) T(args...);
				}
				catch (...){
					Allocator_Leaf.deallocate(NewNode, 1);
					return std::pair<iterator, bool>(iterator(nullptr), false);
				}

				if (&this->lpRoot == lpChildPtr)NewNode->lpParent = nullptr;
				else NewNode->lpParent = this->_GetSubTree(lpParent);
				*lpChildPtr = &NewNode->Span;
			}
			else if (**lpChildPtr != Dim_Type(0)){
				//create another subtree to contain the new node and the subtree [**lpChildPtr]
				subtree_type *lpNewSubTree = Allocator_SubTree.allocate(1);
				if (nullptr == lpNewSubTree){
					Allocator_Leaf.deallocate(NewNode, 1);
					return std::pair<iterator, bool>(iterator(nullptr), false);
				}

				try{
					new((void *)&NewNode->Content.Data) T(args...);
				}
				catch (...){
					Allocator_SubTree.deallocate(lpNewSubTree, 1);
					Allocator_Leaf.deallocate(NewNode, 1);
					return std::pair<iterator, bool>(iterator(nullptr), false);
				}

				subtree_type *lpOldSubTree = this->_GetSubTree(*lpChildPtr);

				_PointOctreeMap_NewSubTree<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpNewSubTree, lpOldSubTree->Position_Center.Data, lpPosition);
				UBINT Quadrant;
				Quadrant = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpNewSubTree, lpOldSubTree->Position_Center.Data);
				if (Quadrant >= 8)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
				lpNewSubTree->lpChild[Quadrant] = &lpOldSubTree->Span;
				lpOldSubTree->lpParent = lpNewSubTree;

				Quadrant = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpNewSubTree, lpPosition);
				if (Quadrant >= 8)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
				lpNewSubTree->lpChild[Quadrant] = &NewNode->Span;
				NewNode->lpParent = lpNewSubTree;

				if (&this->lpRoot == lpChildPtr)lpNewSubTree->lpParent = nullptr;
				else lpNewSubTree->lpParent = this->_GetSubTree(lpParent);
				*lpChildPtr = &lpNewSubTree->Span;
			}
			else{ // **lpChildPtr == Dim_Type(0)
				leaf_type *lpOldLeaf = this->_GetLeaf(*lpChildPtr);
				if (lpOldLeaf->Content.Position[0] == lpPosition[0] && lpOldLeaf->Content.Position[1] == lpPosition[1] && lpOldLeaf->Content.Position[2] == lpPosition[2]){
					//overlap
					Allocator_Leaf.deallocate(NewNode, 1);
					return std::pair<iterator, bool>(iterator(lpOldLeaf), false);
				}

				//create another subtree to contain the new node and the leaf [**lpChildPtr]
				subtree_type *lpNewSubTree = Allocator_SubTree.allocate(1);
				if (nullptr == lpNewSubTree){
					Allocator_Leaf.deallocate(NewNode, 1);
					return std::pair<iterator, bool>(iterator(nullptr), false);
				}

				try{
					new((void *)&NewNode->Content.Data) T(args...);
				}
				catch (...){
					Allocator_SubTree.deallocate(lpNewSubTree, 1);
					Allocator_Leaf.deallocate(NewNode, 1);
					return std::pair<iterator, bool>(iterator(nullptr), false);
				}

				_PointOctreeMap_NewSubTree<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpNewSubTree, lpOldLeaf->Content.Position.Data, lpPosition);
				UBINT Quadrant1 = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpNewSubTree, lpOldLeaf->Content.Position.Data);
				if (Quadrant1 >= 8)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
				lpNewSubTree->lpChild[Quadrant1] = &lpOldLeaf->Span;
				lpOldLeaf->lpParent = lpNewSubTree;

				UBINT Quadrant2 = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpNewSubTree, lpPosition);
				if (Quadrant2 >= 8)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
				if (Quadrant2 == Quadrant1)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
				lpNewSubTree->lpChild[Quadrant2] = &NewNode->Span;
				NewNode->lpParent = lpNewSubTree;

				if (&this->lpRoot == lpChildPtr)lpNewSubTree->lpParent = nullptr;
				else lpNewSubTree->lpParent = this->_GetSubTree(lpParent);
				*lpChildPtr = &lpNewSubTree->Span;
			}

			this->NodeCount++;
			return std::pair<iterator, bool>(iterator(NewNode), true);
		}

		bool move(const_iterator Pos, const Dim_Type x_new, const Dim_Type y_new, const Dim_Type z_new){
			leaf_type *TgtPtr = (leaf_type *)Pos._GetNode();
			if (nullptr != TgtPtr){
				nsMath::TinyVector<Dim_Type, 3> NewPos;
				NewPos[0] = x_new; NewPos[1] = y_new; NewPos[2] = z_new;
				_PointOctreeMap_GetValidPos<Dim_Type>::Func(NewPos.Data);

				subtree_type *lpParent = TgtPtr->lpParent;
				Dim_Type *lpHandle = &TgtPtr->Span, *lpReplacement = nullptr, **RollBackPtr, **lpChildPtr;

				bool ChildDestroyed = true;
				subtree_type *lpBufferedSubTree = nullptr; //buffered to reduce memory allocation operation.
				while (nullptr != lpParent){
					UBINT i;
					UBINT ChildCount = 0;
					Dim_Type *lpValidChild = nullptr;
					for (i = 0; i < 8; i++){
						if (lpHandle == lpParent->lpChild[i])break;
						if (nullptr != lpParent->lpChild[i]){
							lpValidChild = lpParent->lpChild[i];
							ChildCount++;
						}
					}
					if (ChildDestroyed){
						if (lpParent->lpChild[i] == &TgtPtr->Span)RollBackPtr = &lpParent->lpChild[i]; //if we fail to insert the node later, we can put the leaf back here.
						lpParent->lpChild[i] = lpReplacement;
						if (lpReplacement != nullptr){
							if (*lpReplacement != Dim_Type(0)){
								subtree_type *lpReplacementSubTree = this->_GetSubTree(lpReplacement);
								lpReplacementSubTree->lpParent = lpParent;
							}
							else{
								leaf_type *lpReplacementLeaf = this->_GetLeaf(lpReplacement);
								lpReplacementLeaf->lpParent = lpParent;
							}
							lpValidChild = lpReplacement;
							ChildCount++;
						}
					}
					else{
						lpValidChild = lpParent->lpChild[i];
						ChildCount++;
					}

					UBINT Quadrant = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpParent, NewPos.Data);
					if (Quadrant >= 8){
						//not in this subtree. maybe we need to delete it
						for (i++; i < 8; i++){
							if (nullptr != lpParent->lpChild[i]){
								lpValidChild = lpParent->lpChild[i];
								ChildCount++;
							}
						}

						subtree_type *TmpSubTree = lpParent;
						lpHandle = &lpParent->Span;
						lpParent = lpParent->lpParent;

						if (ChildCount > 1)ChildDestroyed = false;
						else{
							if (ChildCount > 0)lpReplacement = lpValidChild;
							else lpReplacement = nullptr;
							if (nullptr == lpBufferedSubTree)lpBufferedSubTree = TmpSubTree;
							else Allocator_SubTree.deallocate(TmpSubTree, 1);
						}
					}
					else{
						//need an insert
						lpChildPtr = &lpParent->lpChild[Quadrant];
						break;
					}
				}

				//do an insert
				if (nullptr == lpParent){
					if (ChildDestroyed){
						this->lpRoot = lpReplacement;
						if (lpReplacement != nullptr){
							if (*lpReplacement != Dim_Type(0)){
								subtree_type *lpReplacementSubTree = this->_GetSubTree(lpReplacement);
								lpReplacementSubTree->lpParent = nullptr;
							}
							else{
								leaf_type *lpReplacementLeaf = this->_GetLeaf(lpReplacement);
								lpReplacementLeaf->lpParent = nullptr;
							}
						}
					}
					lpChildPtr = &this->lpRoot;
				}
				while (nullptr != *lpChildPtr && **lpChildPtr != Dim_Type(0)){
					PointOctreeMapSubTree<Dim_Type, T> *lpSubTree = this->_GetSubTree(*lpChildPtr);
					UBINT Quadrant = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpSubTree, NewPos.Data);
					if (Quadrant >= 8)break;
					else{
						lpParent = lpSubTree;
						lpChildPtr = &lpSubTree->lpChild[Quadrant];
					}
				}
				if (nullptr == *lpChildPtr){
					//the node will be placed here
					TgtPtr->lpParent = lpParent;
					*lpChildPtr = &TgtPtr->Span;
				}
				else if (**lpChildPtr != Dim_Type(0)){
					if (nullptr == lpBufferedSubTree){
						//create another subtree to contain the new node and the subtree [**lpChildPtr]
						lpBufferedSubTree = Allocator_SubTree.allocate(1);
						if (nullptr == lpBufferedSubTree){
							//allocation failed, performing rollback
							//Notice that [lpBufferedSubTree] will never be nullptr when the tree structure is modified. So we don't need to worry about the structure of the tree.
							//TgtPtr->lpParent hasn't been modified yet, so we don't need to worry about it either.
							*RollBackPtr = &TgtPtr->Span;
							return false;
						}
					}

					subtree_type *lpOldSubTree = this->_GetSubTree(*lpChildPtr);

					_PointOctreeMap_NewSubTree<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpBufferedSubTree, lpOldSubTree->Position_Center.Data, NewPos.Data);
					UBINT Quadrant1 = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpBufferedSubTree, lpOldSubTree->Position_Center.Data);
					if (Quadrant1 >= 8)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
					lpBufferedSubTree->lpChild[Quadrant1] = &lpOldSubTree->Span;
					lpOldSubTree->lpParent = lpBufferedSubTree;

					UBINT Quadrant2 = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpBufferedSubTree, NewPos.Data);
					if (Quadrant2 >= 8)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
					lpBufferedSubTree->lpChild[Quadrant2] = &TgtPtr->Span;
					TgtPtr->lpParent = lpBufferedSubTree;

					lpBufferedSubTree->lpParent = lpParent;
					*lpChildPtr = &lpBufferedSubTree->Span;
				}
				else{ // **lpChildPtr == Dim_Type(0)
					leaf_type *lpOldLeaf = this->_GetLeaf(*lpChildPtr);
					if (lpOldLeaf->Content.Position[0] == NewPos[0] && lpOldLeaf->Content.Position[1] == NewPos[1] && lpOldLeaf->Content.Position[2] == NewPos[2]){
						//overlap, performing rollback
						*RollBackPtr = &TgtPtr->Span;
						return false;
					}

					if (nullptr == lpBufferedSubTree){
						//create another subtree to contain the new node and the subtree [**lpChildPtr]
						lpBufferedSubTree = Allocator_SubTree.allocate(1);
						if (nullptr == lpBufferedSubTree){
							//allocation failed, performing rollback
							*RollBackPtr = &TgtPtr->Span;
							return false;
						}
					}

					_PointOctreeMap_NewSubTree<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpBufferedSubTree, lpOldLeaf->Content.Position.Data, NewPos.Data);
					UBINT Quadrant1 = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpBufferedSubTree, lpOldLeaf->Content.Position.Data);
					if (Quadrant1 >= 8)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
					lpBufferedSubTree->lpChild[Quadrant1] = &lpOldLeaf->Span;
					lpOldLeaf->lpParent = lpBufferedSubTree;

					UBINT Quadrant2 = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpBufferedSubTree, NewPos.Data);
					if (Quadrant2 >= 8)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
					if (Quadrant2 == Quadrant1)throw std::exception(); //check the robustness of the octree algorithm when this exception is thrown
					lpBufferedSubTree->lpChild[Quadrant2] = &TgtPtr->Span;
					TgtPtr->lpParent = lpBufferedSubTree;

					lpBufferedSubTree->lpParent = lpParent;
					*lpChildPtr = &lpBufferedSubTree->Span;
				}

				*(const_cast<nsMath::TinyVector<Dim_Type, 3> *>(&TgtPtr->Content.Position)) = NewPos;
				return true;
			}
			return false;
		}

		iterator erase(const_iterator Pos){
			leaf_type *TgtPtr = (leaf_type *)Pos._GetNode();
			if (nullptr != TgtPtr){
				subtree_type *lpParent = TgtPtr->lpParent;
				Dim_Type *lpHandle = &TgtPtr->Span, *lpReplacement = nullptr;

				Allocator_Leaf.destroy(&TgtPtr->Content.Data); //since C++11
				Allocator_Leaf.deallocate(TgtPtr, 1);
				this->NodeCount--;

				bool ChildDestroyed = true;
				while (nullptr != lpParent){
					UBINT i;
					UBINT ChildCount = 0;
					Dim_Type *lpValidChild = nullptr;

					for (i = 0; i < 8; i++){
						if (lpHandle == lpParent->lpChild[i])break;
						if (nullptr != lpParent->lpChild[i]){
							lpValidChild = lpParent->lpChild[i];
							ChildCount++;
						}
					}
					if (ChildDestroyed){
						lpParent->lpChild[i] = lpReplacement;
						if (lpReplacement != nullptr){
							if (*lpReplacement != Dim_Type(0)){
								subtree_type *lpReplacementSubTree = this->_GetSubTree(lpReplacement);
								lpReplacementSubTree->lpParent = lpParent;
							}
							else{
								leaf_type *lpReplacementLeaf = this->_GetLeaf(lpReplacement);
								lpReplacementLeaf->lpParent = lpParent;
							}
							lpValidChild = lpReplacement;
							ChildCount++;
						}
					}
					else{
						lpValidChild = lpParent->lpChild[i];
						ChildCount++;
					}
					for (i++; i < 8; i++){
						if (nullptr != lpParent->lpChild[i]){
							lpValidChild = lpParent->lpChild[i];
							ChildCount++;
						}
						if (lpParent->lpChild[i] != nullptr)break;
					}
					if (i < 8){
						//the next leaf is found
						if ((Dim_Type)0 == *lpParent->lpChild[i])return iterator(_GetLeaf(lpParent->lpChild[i]));
						else{
							lpParent = _GetSubTree(lpParent->lpChild[i]);
							while (true){
								for (UBINT i = 0; i < 8; i++){
									if (nullptr != lpParent->lpChild[i]){
										if ((Dim_Type)0 == *lpParent->lpChild[i])return iterator(_GetLeaf(lpParent->lpChild[i]));
										else{
											lpParent = _GetSubTree(lpParent->lpChild[i]);
											break;
										}
									}
								}
							}
						}
					}
					else{
						subtree_type *TmpSubTree = lpParent;
						lpHandle = &lpParent->Span;
						lpParent = lpParent->lpParent;

						if (ChildCount > 1)ChildDestroyed = false;
						else{
							if (ChildCount > 0)lpReplacement = lpValidChild; else lpReplacement = nullptr;
							Allocator_SubTree.deallocate(TmpSubTree, 1);
						}
					}
				}
				if (ChildDestroyed){
					this->lpRoot = lpReplacement;
					if (lpReplacement != nullptr){
						if (*lpReplacement != Dim_Type(0)){
							subtree_type *lpReplacementSubTree = this->_GetSubTree(lpReplacement);
							lpReplacementSubTree->lpParent = nullptr;
						}
						else{
							leaf_type *lpReplacementLeaf = this->_GetLeaf(lpReplacement);
							lpReplacementLeaf->lpParent = nullptr;
						}
					}
				}
				return iterator(nullptr);
			}
			else return iterator(nullptr);
		}

		//query functions
		template <typename R> UBINT find_spherical(const R x, const R y, const R z, const R radius, Vector<iterator> *lpRetValue){
			static_assert(nsMath::TypeID_FLOAT == nsMath::NumericTrait<R>::Category, "The distance should be represented with a float number.");
			UBINT NumOfPoints = 0;
			if (!this->empty()){
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot;
				const R radius_sqr = radius * radius, SQRT_3_TIMES_2 = (R)2 * sqrt((R)3);
				do{
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);
						R DistSqr = ((R)lpSubTree->Position_Center[0] - x) * ((R)lpSubTree->Position_Center[0] - x);
						DistSqr += ((R)lpSubTree->Position_Center[1] - y) * ((R)lpSubTree->Position_Center[1] - y);
						DistSqr += ((R)lpSubTree->Position_Center[2] - z) * ((R)lpSubTree->Position_Center[2] - z);
						if (DistSqr < radius_sqr + SQRT_3_TIMES_2 * radius * (R)lpSubTree->Span + (R)3 * (R)lpSubTree->Span * (R)lpSubTree->Span){
							UBINT i;
							for (i = 0; i < 8; i++){
								if (lpSubTree->lpChild[i] != nullptr){
									lpHandle = lpSubTree->lpChild[i];
									std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
									Stack.push_back(StackEntry);
									break;
								}
							}
							if (i >= 8){
								while (!Stack.empty()){
									lpSubTree = Stack.back().first;
									UBINT j;
									for (j = Stack.back().second + 1; j < 8; j++){
										if (lpSubTree->lpChild[j] != nullptr){
											lpHandle = lpSubTree->lpChild[j];
											Stack.back().second = j;
											break;
										}
									}
									if (j >= 8)Stack.pop_back(); else break;
								}
							}
						}
						else{
							while (!Stack.empty()){
								lpSubTree = Stack.back().first;
								UBINT j;
								for (j = Stack.back().second + 1; j < 8; j++){
									if (lpSubTree->lpChild[j] != nullptr){
										lpHandle = lpSubTree->lpChild[j];
										Stack.back().second = j;
										break;
									}
								}
								if (j >= 8)Stack.pop_back(); else break;
							}
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						R DistSqr = ((R)lpLeaf->Content.Position[0] - x) * ((R)lpLeaf->Content.Position[0] - x);
						DistSqr += ((R)lpLeaf->Content.Position[1] - y) * ((R)lpLeaf->Content.Position[1] - y);
						DistSqr += ((R)lpLeaf->Content.Position[2] - z) * ((R)lpLeaf->Content.Position[2] - z);
						if (DistSqr < radius_sqr){
							NumOfPoints++;
							lpRetValue->push_back(iterator(lpLeaf));
						}
						while (!Stack.empty()){
							subtree_type *lpSubTree = Stack.back().first;
							UBINT j;
							for (j = Stack.back().second + 1; j < 8; j++){
								if (lpSubTree->lpChild[j] != nullptr){
									lpHandle = lpSubTree->lpChild[j];
									Stack.back().second = j;
									break;
								}
							}
							if (j >= 8)Stack.pop_back(); else break;
						}
					}
				} while (!Stack.empty());
			}
		}
		template <typename R> Vector<iterator> find_spherical(const R x, const R y, const R z, const R radius_sqr){
			Vector<iterator> RetValue;
			this->find_spherical(x, y, z, radius_sqr, &RetValue);
			return std::move(RetValue);
		}
		template <typename R> UBINT find_spherical(const_iterator pos, const R radius, Vector<iterator> *lpRetValue){
			// the return value will not include [pos].
			static_assert(nsMath::TypeID_FLOAT == nsMath::NumericTrait<R>::Category, "The distance should be represented with a float number.");
			UBINT NumOfPoints = 0;
			if (!this->empty() && pos != this->cend()){
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				const R radius_sqr = radius * radius, SQRT_3_TIMES_2 = (R)2 * sqrt((R)3);

				subtree_type *lpRoot = (subtree_type *)pos._GetNode()->lpParent;
				const Dim_Type *ExcludedPtr = &pos._GetNode()->Span;
				while (nullptr != lpRoot){
					const Dim_Type *lpHandle;
					for (UBINT k = 0; k < 8; k++){
						if (lpRoot->lpChild[k] != nullptr && lpRoot->lpChild[k] != ExcludedPtr){
							lpHandle = lpRoot->lpChild[k];
							do{
								if ((Dim_Type)0 != *lpHandle){
									subtree_type *lpSubTree = _GetSubTree(lpHandle);
									R DistSqr = ((R)lpSubTree->Position_Center[0] - (R)pos->Position[0]) * ((R)lpSubTree->Position_Center[0] - (R)pos->Position[0]);
									DistSqr += ((R)lpSubTree->Position_Center[1] - (R)pos->Position[1]) * ((R)lpSubTree->Position_Center[1] - (R)pos->Position[1]);
									DistSqr += ((R)lpSubTree->Position_Center[2] - (R)pos->Position[2]) * ((R)lpSubTree->Position_Center[2] - (R)pos->Position[2]);
									if (DistSqr < radius_sqr + SQRT_3_TIMES_2 * radius * (R)lpSubTree->Span + (R)3 * (R)lpSubTree->Span * (R)lpSubTree->Span){
										UBINT i;
										for (i = 0; i < 8; i++){
											if (lpSubTree->lpChild[i] != nullptr){
												lpHandle = lpSubTree->lpChild[i];
												std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
												Stack.push_back(StackEntry);
												break;
											}
										}
										if (i >= 8){
											while (!Stack.empty()){
												lpSubTree = Stack.back().first;
												UBINT j;
												for (j = Stack.back().second + 1; j < 8; j++){
													if (lpSubTree->lpChild[j] != nullptr){
														lpHandle = lpSubTree->lpChild[j];
														Stack.back().second = j;
														break;
													}
												}
												if (j >= 8)Stack.pop_back(); else break;
											}
										}
									}
									else{
										while (!Stack.empty()){
											lpSubTree = Stack.back().first;
											UBINT j;
											for (j = Stack.back().second + 1; j < 8; j++){
												if (lpSubTree->lpChild[j] != nullptr){
													lpHandle = lpSubTree->lpChild[j];
													Stack.back().second = j;
													break;
												}
											}
											if (j >= 8)Stack.pop_back(); else break;
										}
									}
								}
								else{
									leaf_type *lpLeaf = _GetLeaf(lpHandle);
									R DistSqr = ((R)lpLeaf->Content.Position[0] - (R)pos->Position[0]) * ((R)lpLeaf->Content.Position[0] - (R)pos->Position[0]);
									DistSqr += ((R)lpLeaf->Content.Position[1] - (R)pos->Position[1]) * ((R)lpLeaf->Content.Position[1] - (R)pos->Position[1]);
									DistSqr += ((R)lpLeaf->Content.Position[2] - (R)pos->Position[2]) * ((R)lpLeaf->Content.Position[2] - (R)pos->Position[2]);
									if (DistSqr < radius_sqr){
										NumOfPoints++;
										lpRetValue->push_back(iterator(lpLeaf));
									}
									while (!Stack.empty()){
										subtree_type *lpSubTree = Stack.back().first;
										UBINT j;
										for (j = Stack.back().second + 1; j < 8; j++){
											if (lpSubTree->lpChild[j] != nullptr){
												lpHandle = lpSubTree->lpChild[j];
												Stack.back().second = j;
												break;
											}
										}
										if (j >= 8)Stack.pop_back(); else break;
									}
								}
							} while (!Stack.empty());
						}
					}
					ExcludedPtr = &lpRoot->Span;
					lpRoot = lpRoot->lpParent;
				}
			}
		}
		template <typename R> Vector<iterator> find_spherical(const_iterator pos, const R radius_sqr){
			Vector<iterator> RetValue;
			this->find_spherical(pos, radius_sqr, &RetValue);
			return std::move(RetValue);
		}
		template <typename R> UBINT find_nearest(const R x, const R y, const R z, UBINT Count, iterator *lpRetValue){
			static_assert(nsMath::TypeID_FLOAT == nsMath::NumericTrait<R>::Category, "The position should be represented with a float number.");
			if (this->empty())return 0;
			else{
				R Max_Radius = nsMath::NumericTrait<R>::Inf_Positive, Max_Radius_Sqr = Max_Radius * Max_Radius;
				const R SQRT_3_TIMES_2 = (R)2 * sqrt((R)3);
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot;

				struct StdPairLess{
					inline bool operator()(const std::pair<R, iterator>& lhs, const std::pair<R, iterator>& rhs){
						return (lhs.first < rhs.first);
					}
				} MyComp;
				Vector<std::pair<R, iterator>> MyQueueVec;
				MyQueueVec.reserve(Count);
				PriorityQueue<std::pair<R, iterator>, Vector<std::pair<R, iterator>>, StdPairLess> MyPriorityQueue(MyComp, std::move(MyQueueVec));

				do{
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);
						R DistSqr = ((R)lpSubTree->Position_Center[0] - x) * ((R)lpSubTree->Position_Center[0] - x);
						DistSqr += ((R)lpSubTree->Position_Center[1] - y) * ((R)lpSubTree->Position_Center[1] - y);
						DistSqr += ((R)lpSubTree->Position_Center[2] - z) * ((R)lpSubTree->Position_Center[2] - z);
						if (DistSqr < Max_Radius_Sqr + SQRT_3_TIMES_2 * Max_Radius * (R)lpSubTree->Span + (R)3 * (R)lpSubTree->Span * (R)lpSubTree->Span){
							UBINT i;
							for (i = 0; i < 8; i++){
								if (lpSubTree->lpChild[i] != nullptr){
									lpHandle = lpSubTree->lpChild[i];
									std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
									Stack.push_back(StackEntry);
									break;
								}
							}
							if (i >= 8){
								while (!Stack.empty()){
									lpSubTree = Stack.back().first;
									UBINT j;
									for (j = Stack.back().second + 1; j < 8; j++){
										if (lpSubTree->lpChild[j] != nullptr){
											lpHandle = lpSubTree->lpChild[j];
											Stack.back().second = j;
											break;
										}
									}
									if (j >= 8)Stack.pop_back(); else break;
								}
							}
						}
						else{
							while (!Stack.empty()){
								lpSubTree = Stack.back().first;
								UBINT j;
								for (j = Stack.back().second + 1; j < 8; j++){
									if (lpSubTree->lpChild[j] != nullptr){
										lpHandle = lpSubTree->lpChild[j];
										Stack.back().second = j;
										break;
									}
								}
								if (j >= 8)Stack.pop_back(); else break;
							}
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						R DistSqr = ((R)lpLeaf->Content.Position[0] - x) * ((R)lpLeaf->Content.Position[0] - x);
						DistSqr += ((R)lpLeaf->Content.Position[1] - y) * ((R)lpLeaf->Content.Position[1] - y);
						DistSqr += ((R)lpLeaf->Content.Position[2] - z) * ((R)lpLeaf->Content.Position[2] - z);
						if (DistSqr < Max_Radius_Sqr){
							if (MyPriorityQueue.size() >= Count){
								MyPriorityQueue.pop();
								MyPriorityQueue.push(std::pair<R, iterator>(DistSqr, iterator(lpLeaf)));
							}
							else MyPriorityQueue.push(std::pair<R, iterator>(DistSqr, iterator(lpLeaf)));
							if (MyPriorityQueue.size() >= Count){
								Max_Radius_Sqr = MyPriorityQueue.top().first;
								Max_Radius = sqrt(Max_Radius_Sqr);
							}
						}

						while (!Stack.empty()){
							subtree_type *lpSubTree = Stack.back().first;
							UBINT j;
							for (j = Stack.back().second + 1; j < 8; j++){
								if (lpSubTree->lpChild[j] != nullptr){
									lpHandle = lpSubTree->lpChild[j];
									Stack.back().second = j;
									break;
								}
							}
							if (j >= 8)Stack.pop_back(); else break;
						}
					}
				} while (!Stack.empty());

				//post-process
				UBINT NumOfPoints = MyPriorityQueue.size();
				for (UBINT i = MyPriorityQueue.size(); i > 0; i--){
					lpRetValue[i - 1] = MyPriorityQueue.top().second;
					MyPriorityQueue.pop();
				}
				return NumOfPoints;
			}
		}
		UBINT find_nearest(const_iterator pos, UBINT Count, iterator *lpRetValue){
			static_assert(nsMath::TypeID_FLOAT == nsMath::NumericTrait<double>::Category, "The position should be represented with a float number.");
			if (this->empty())return 0;
			else{
				double Max_Radius = nsMath::NumericTrait<double>::Inf_Positive, Max_Radius_Sqr = Max_Radius * Max_Radius;
				const double SQRT_3_TIMES_2 = (double)2 * sqrt((double)3);
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot;

				struct StdPairLess{
					inline bool operator()(const std::pair<double, iterator>& lhs, const std::pair<double, iterator>& rhs){
						return (lhs.first < rhs.first);
					}
				} MyComp;
				Vector<std::pair<double, iterator>> MyQueueVec;
				MyQueueVec.reserve(Count);
				PriorityQueue<std::pair<double, iterator>, Vector<std::pair<double, iterator>>, StdPairLess> MyPriorityQueue(MyComp, std::move(MyQueueVec));

				subtree_type *lpRoot = (subtree_type *)pos._GetNode()->lpParent;
				const Dim_Type *ExcludedPtr = &pos._GetNode()->Span;
				while (nullptr != lpRoot){
					const Dim_Type *lpHandle;
					for (UBINT k = 0; k < 8; k++){
						if (lpRoot->lpChild[k] != nullptr && lpRoot->lpChild[k] != ExcludedPtr){
							lpHandle = lpRoot->lpChild[k];
							do{
								if ((Dim_Type)0 != *lpHandle){
									subtree_type *lpSubTree = _GetSubTree(lpHandle);
									double DistSqr = ((double)lpSubTree->Position_Center[0] - pos->Position[0]) * ((double)lpSubTree->Position_Center[0] - pos->Position[0]);
									DistSqr += ((double)lpSubTree->Position_Center[1] - pos->Position[1]) * ((double)lpSubTree->Position_Center[1] - pos->Position[1]);
									DistSqr += ((double)lpSubTree->Position_Center[2] - pos->Position[2]) * ((double)lpSubTree->Position_Center[2] - pos->Position[2]);
									if (DistSqr < Max_Radius_Sqr + SQRT_3_TIMES_2 * Max_Radius * (double)lpSubTree->Span + 3.0 * (double)lpSubTree->Span * (double)lpSubTree->Span){
										UBINT i;
										for (i = 0; i < 8; i++){
											if (lpSubTree->lpChild[i] != nullptr){
												lpHandle = lpSubTree->lpChild[i];
												std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
												Stack.push_back(StackEntry);
												break;
											}
										}
										if (i >= 8){
											while (!Stack.empty()){
												lpSubTree = Stack.back().first;
												UBINT j;
												for (j = Stack.back().second + 1; j < 8; j++){
													if (lpSubTree->lpChild[j] != nullptr){
														lpHandle = lpSubTree->lpChild[j];
														Stack.back().second = j;
														break;
													}
												}
												if (j >= 8)Stack.pop_back(); else break;
											}
										}
									}
									else{
										while (!Stack.empty()){
											lpSubTree = Stack.back().first;
											UBINT j;
											for (j = Stack.back().second + 1; j < 8; j++){
												if (lpSubTree->lpChild[j] != nullptr){
													lpHandle = lpSubTree->lpChild[j];
													Stack.back().second = j;
													break;
												}
											}
											if (j >= 8)Stack.pop_back(); else break;
										}
									}
								}
								else{
									leaf_type *lpLeaf = _GetLeaf(lpHandle);
									double DistSqr = ((double)lpLeaf->Content.Position[0] - pos->Position[0]) * ((double)lpLeaf->Content.Position[0] - pos->Position[0]);
									DistSqr += ((double)lpLeaf->Content.Position[1] - pos->Position[1]) * ((double)lpLeaf->Content.Position[1] - pos->Position[1]);
									DistSqr += ((double)lpLeaf->Content.Position[2] - pos->Position[2]) * ((double)lpLeaf->Content.Position[2] - pos->Position[2]);
									if (DistSqr < Max_Radius_Sqr){
										if (MyPriorityQueue.size() >= Count){
											MyPriorityQueue.pop();
											MyPriorityQueue.push(std::pair<double, iterator>(DistSqr, iterator(lpLeaf)));
										}
										else MyPriorityQueue.push(std::pair<double, iterator>(DistSqr, iterator(lpLeaf)));
										if (MyPriorityQueue.size() >= Count){
											Max_Radius_Sqr = MyPriorityQueue.top().first;
											Max_Radius = sqrt(Max_Radius_Sqr);
										}
									}

									while (!Stack.empty()){
										subtree_type *lpSubTree = Stack.back().first;
										UBINT j;
										for (j = Stack.back().second + 1; j < 8; j++){
											if (lpSubTree->lpChild[j] != nullptr){
												lpHandle = lpSubTree->lpChild[j];
												Stack.back().second = j;
												break;
											}
										}
										if (j >= 8)Stack.pop_back(); else break;
									}
								}
							} while (!Stack.empty());
						}
					}
					ExcludedPtr = &lpRoot->Span;
					lpRoot = lpRoot->lpParent;
				}
				//post-process
				UBINT NumOfPoints = MyPriorityQueue.size();
				for (UBINT i = MyPriorityQueue.size(); i > 0; i--){
					lpRetValue[i - 1] = MyPriorityQueue.top().second;
					MyPriorityQueue.pop();
				}
				return NumOfPoints;
			}
		}
		iterator find_min_x(){
			if (this->empty())return this->end();
			else{
				UBINT Query_Order[8] = { 0, 2, 4, 6, 1, 3, 5, 7 };
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot, min_x = nsMath::NumericTrait<Dim_Type>::Max;
				iterator RetValue;
				do{
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);
						if (lpSubTree->Position_Center[0] - lpSubTree->Span <= min_x){ // [Center - span, Center + span)
							UBINT i;
							for (i = 0; i < 8; i++){
								if (lpSubTree->lpChild[Query_Order[i]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[i]];
									std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
									Stack.push_back(StackEntry);
									break;
								}
							}
							if (i >= 8){
								while (!Stack.empty()){
									lpSubTree = Stack.back().first;
									UBINT j;
									for (j = Stack.back().second + 1; j < 8; j++){
										if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
											lpHandle = lpSubTree->lpChild[Query_Order[j]];
											Stack.back().second = j;
											break;
										}
									}
									if (j >= 8)Stack.pop_back(); else break;
								}
							}
						}
						else{
							while (!Stack.empty()){
								lpSubTree = Stack.back().first;
								UBINT j;
								for (j = Stack.back().second + 1; j < 8; j++){
									if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
										lpHandle = lpSubTree->lpChild[Query_Order[j]];
										Stack.back().second = j;
										break;
									}
								}
								if (j >= 8)Stack.pop_back(); else break;
							}
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						if (lpLeaf->Content.Position[0] <= min_x){
							min_x = lpLeaf->Content.Position[0];
							RetValue = iterator(lpLeaf);
						}
						while (!Stack.empty()){
							subtree_type *lpSubTree = Stack.back().first;
							UBINT j;
							for (j = Stack.back().second + 1; j < 8; j++){
								if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[j]];
									Stack.back().second = j;
									break;
								}
							}
							if (j >= 8)Stack.pop_back(); else break;
						}
					}
				} while (!Stack.empty());
				return RetValue;
			}
		}
		iterator find_max_x(){
			if (this->empty())return this->end();
			else{
				UBINT Query_Order[8] = { 7, 5, 3, 1, 6, 4, 2, 0 };
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot, max_x = nsMath::NumericTrait<Dim_Type>::Min;
				iterator RetValue;
				do{
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);
						if (lpSubTree->Position_Center[0] + lpSubTree->Span > max_x){ // [Center - span, Center + span)
							UBINT i;
							for (i = 0; i < 8; i++){
								if (lpSubTree->lpChild[Query_Order[i]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[i]];
									std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
									Stack.push_back(StackEntry);
									break;
								}
							}
							if (i >= 8){
								while (!Stack.empty()){
									lpSubTree = Stack.back().first;
									UBINT j;
									for (j = Stack.back().second + 1; j < 8; j++){
										if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
											lpHandle = lpSubTree->lpChild[Query_Order[j]];
											Stack.back().second = j;
											break;
										}
									}
									if (j >= 8)Stack.pop_back(); else break;
								}
							}
						}
						else{
							while (!Stack.empty()){
								lpSubTree = Stack.back().first;
								UBINT j;
								for (j = Stack.back().second + 1; j < 8; j++){
									if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
										lpHandle = lpSubTree->lpChild[Query_Order[j]];
										Stack.back().second = j;
										break;
									}
								}
								if (j >= 8)Stack.pop_back(); else break;
							}
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						if (lpLeaf->Content.Position[0] >= max_x){
							max_x = lpLeaf->Content.Position[0];
							RetValue = iterator(lpLeaf);
						}
						while (!Stack.empty()){
							subtree_type *lpSubTree = Stack.back().first;
							UBINT j;
							for (j = Stack.back().second + 1; j < 8; j++){
								if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[j]];
									Stack.back().second = j;
									break;
								}
							}
							if (j >= 8)Stack.pop_back(); else break;
						}
					}
				} while (!Stack.empty());
				return RetValue;
			}
		}
		iterator find_min_y(){
			if (this->empty())return this->end();
			else{
				UBINT Query_Order[8] = { 0, 1, 4, 5, 2, 3, 6, 7 };
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot, min_y = nsMath::NumericTrait<Dim_Type>::Max;
				iterator RetValue;
				do{
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);
						if (lpSubTree->Position_Center[1] - lpSubTree->Span <= min_y){ // [Center - span, Center + span)
							UBINT i;
							for (i = 0; i < 8; i++){
								if (lpSubTree->lpChild[Query_Order[i]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[i]];
									std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
									Stack.push_back(StackEntry);
									break;
								}
							}
							if (i >= 8){
								while (!Stack.empty()){
									lpSubTree = Stack.back().first;
									UBINT j;
									for (j = Stack.back().second + 1; j < 8; j++){
										if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
											lpHandle = lpSubTree->lpChild[Query_Order[j]];
											Stack.back().second = j;
											break;
										}
									}
									if (j >= 8)Stack.pop_back(); else break;
								}
							}
						}
						else{
							while (!Stack.empty()){
								lpSubTree = Stack.back().first;
								UBINT j;
								for (j = Stack.back().second + 1; j < 8; j++){
									if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
										lpHandle = lpSubTree->lpChild[Query_Order[j]];
										Stack.back().second = j;
										break;
									}
								}
								if (j >= 8)Stack.pop_back(); else break;
							}
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						if (lpLeaf->Content.Position[1] <= min_y){
							min_y = lpLeaf->Content.Position[1];
							RetValue = iterator(lpLeaf);
						}
						while (!Stack.empty()){
							subtree_type *lpSubTree = Stack.back().first;
							UBINT j;
							for (j = Stack.back().second + 1; j < 8; j++){
								if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[j]];
									Stack.back().second = j;
									break;
								}
							}
							if (j >= 8)Stack.pop_back(); else break;
						}
					}
				} while (!Stack.empty());
				return RetValue;
			}
		}
		iterator find_max_y(){
			if (this->empty())return this->end();
			else{
				UBINT Query_Order[8] = { 7, 6, 3, 2, 5, 4, 1, 0 };
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot, max_y = nsMath::NumericTrait<Dim_Type>::Min;
				iterator RetValue;
				do{
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);
						if (lpSubTree->Position_Center[1] + lpSubTree->Span > max_y){ // [Center - span, Center + span)
							UBINT i;
							for (i = 0; i < 8; i++){
								if (lpSubTree->lpChild[Query_Order[i]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[i]];
									std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
									Stack.push_back(StackEntry);
									break;
								}
							}
							if (i >= 8){
								while (!Stack.empty()){
									lpSubTree = Stack.back().first;
									UBINT j;
									for (j = Stack.back().second + 1; j < 8; j++){
										if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
											lpHandle = lpSubTree->lpChild[Query_Order[j]];
											Stack.back().second = j;
											break;
										}
									}
									if (j >= 8)Stack.pop_back(); else break;
								}
							}
						}
						else{
							while (!Stack.empty()){
								lpSubTree = Stack.back().first;
								UBINT j;
								for (j = Stack.back().second + 1; j < 8; j++){
									if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
										lpHandle = lpSubTree->lpChild[Query_Order[j]];
										Stack.back().second = j;
										break;
									}
								}
								if (j >= 8)Stack.pop_back(); else break;
							}
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						if (lpLeaf->Content.Position[1] >= max_y){
							max_y = lpLeaf->Content.Position[1];
							RetValue = iterator(lpLeaf);
						}
						while (!Stack.empty()){
							subtree_type *lpSubTree = Stack.back().first;
							UBINT j;
							for (j = Stack.back().second + 1; j < 8; j++){
								if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[j]];
									Stack.back().second = j;
									break;
								}
							}
							if (j >= 8)Stack.pop_back(); else break;
						}
					}
				} while (!Stack.empty());
				return RetValue;
			}
		}
		iterator find_min_z(){
			if (this->empty())return this->end();
			else{
				UBINT Query_Order[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot, min_z = nsMath::NumericTrait<Dim_Type>::Max;
				iterator RetValue;
				do{
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);
						if (lpSubTree->Position_Center[2] - lpSubTree->Span <= min_z){ // [Center - span, Center + span)
							UBINT i;
							for (i = 0; i < 8; i++){
								if (lpSubTree->lpChild[Query_Order[i]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[i]];
									std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
									Stack.push_back(StackEntry);
									break;
								}
							}
							if (i >= 8){
								while (!Stack.empty()){
									lpSubTree = Stack.back().first;
									UBINT j;
									for (j = Stack.back().second + 1; j < 8; j++){
										if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
											lpHandle = lpSubTree->lpChild[Query_Order[j]];
											Stack.back().second = j;
											break;
										}
									}
									if (j >= 8)Stack.pop_back(); else break;
								}
							}
						}
						else{
							while (!Stack.empty()){
								lpSubTree = Stack.back().first;
								UBINT j;
								for (j = Stack.back().second + 1; j < 8; j++){
									if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
										lpHandle = lpSubTree->lpChild[Query_Order[j]];
										Stack.back().second = j;
										break;
									}
								}
								if (j >= 8)Stack.pop_back(); else break;
							}
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						if (lpLeaf->Content.Position[2] <= min_z){
							min_z = lpLeaf->Content.Position[2];
							RetValue = iterator(lpLeaf);
						}
						while (!Stack.empty()){
							subtree_type *lpSubTree = Stack.back().first;
							UBINT j;
							for (j = Stack.back().second + 1; j < 8; j++){
								if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[j]];
									Stack.back().second = j;
									break;
								}
							}
							if (j >= 8)Stack.pop_back(); else break;
						}
					}
				} while (!Stack.empty());
				return RetValue;
			}
		}
		iterator find_max_z(){
			if (this->empty())return this->end();
			else{
				UBINT Query_Order[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };
				Vector<std::pair<subtree_type *, UBINT>> Stack;
				Dim_Type *lpHandle = this->lpRoot, max_z = nsMath::NumericTrait<Dim_Type>::Min;
				iterator RetValue;
				do{
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);
						if (lpSubTree->Position_Center[2] + lpSubTree->Span > max_z){ // [Center - span, Center + span)
							UBINT i;
							for (i = 0; i < 8; i++){
								if (lpSubTree->lpChild[Query_Order[i]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[i]];
									std::pair<subtree_type *, UBINT> StackEntry(lpSubTree, i);
									Stack.push_back(StackEntry);
									break;
								}
							}
							if (i >= 8){
								while (!Stack.empty()){
									lpSubTree = Stack.back().first;
									UBINT j;
									for (j = Stack.back().second + 1; j < 8; j++){
										if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
											lpHandle = lpSubTree->lpChild[Query_Order[j]];
											Stack.back().second = j;
											break;
										}
									}
									if (j >= 8)Stack.pop_back(); else break;
								}
							}
						}
						else{
							while (!Stack.empty()){
								lpSubTree = Stack.back().first;
								UBINT j;
								for (j = Stack.back().second + 1; j < 8; j++){
									if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
										lpHandle = lpSubTree->lpChild[Query_Order[j]];
										Stack.back().second = j;
										break;
									}
								}
								if (j >= 8)Stack.pop_back(); else break;
							}
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						if (lpLeaf->Content.Position[2] >= max_z){
							max_z = lpLeaf->Content.Position[2];
							RetValue = iterator(lpLeaf);
						}
						while (!Stack.empty()){
							subtree_type *lpSubTree = Stack.back().first;
							UBINT j;
							for (j = Stack.back().second + 1; j < 8; j++){
								if (lpSubTree->lpChild[Query_Order[j]] != nullptr){
									lpHandle = lpSubTree->lpChild[Query_Order[j]];
									Stack.back().second = j;
									break;
								}
							}
							if (j >= 8)Stack.pop_back(); else break;
						}
					}
				} while (!Stack.empty());
				return RetValue;
			}
		}
		iterator find(const Dim_Type x, const Dim_Type y, const Dim_Type z){
			if (!this->empty()){
				Dim_Type *lpHandle = this->lpRoot;
				Dim_Type lpPosition[3] = { x, y, z };

				while (true){
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);

						UBINT Quadrant = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpSubTree, lpPosition);
						if (Quadrant >= 8)return this->end();
						else{
							lpHandle = lpSubTree->lpChild[Quadrant];
							if (nullptr == lpHandle)return this->end();
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						if (lpLeaf->Content.Position[0] == x && lpLeaf->Content.Position[1] == y && lpLeaf->Content.Position[2] == z)return iterator(lpLeaf);
						else return this->end();
					}
				}
			}
			else return this->end();
		}
		const_iterator find(const Dim_Type x, const Dim_Type y, const Dim_Type z) const{
			if (!this->empty()){
				Dim_Type *lpHandle = this->lpRoot;
				Dim_Type lpPosition[3] = { x, y, z };

				while (true){
					if ((Dim_Type)0 != *lpHandle){
						subtree_type *lpSubTree = _GetSubTree(lpHandle);

						UBINT Quadrant = _PointOctreeMap_Quadrant<Dim_Type, T, nsMath::NumericTrait<Dim_Type>::Category>::Func(lpSubTree, lpPosition);
						if (Quadrant >= 8)return this->cend();
						else{
							lpHandle = lpSubTree->lpChild[Quadrant];
							if (nullptr == lpHandle)return this->cend();
						}
					}
					else{
						leaf_type *lpLeaf = _GetLeaf(lpHandle);
						if (lpLeaf->Content.Position[0] == x && lpLeaf->Content.Position[1] == y && lpLeaf->Content.Position[2] == z)return const_iterator(lpLeaf);
						else return this->cend();
					}
				}
			}
			else return this->cend();
		}

		//element access functions
		inline T& operator()(const Dim_Type x, const Dim_Type y, const Dim_Type z){ return this->find(x, y, z)->Data; }
		inline const T& operator()(const Dim_Type x, const Dim_Type y, const Dim_Type z) const{ return this->find(x, y, z)->Data; }

		inline ~PointOctreeMap(){ this->clear(); }
	};
}

#endif