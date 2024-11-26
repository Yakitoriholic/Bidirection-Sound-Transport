/* Description: AVL Tree Implementation. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_AVLTREE
#define LIB_CONTAINER_AVLTREE

#include "lGeneral.hpp"
#include "lContainer_BinaryTree.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	struct AVLTreeNode{
		AVLTreeNode* Parent;
		AVLTreeNode* Child[2];
		BINT Balance;
	};

	class AVLTree{
		// Rotation for AVL:
		//     A
		//    / \
		//   B   x
		//  / \
		// y   z
		//
		// when the balance factor of A becomes -2, only three cases are possible:
		// depth(y) = depth(x)     depth(z) = depth(x) + 1
		// depth(y) = depth(x) + 1 depth(z) = depth(x)
		// depth(y) = depth(x) + 1 depth(z) = depth(x) + 1 (delete only)

		// all possibilities of balance factor changes after rotation (left cases):
		//   -2      0       -2      1
		//   /        \      /        \
		// -1    ->    0    0    ->   -1
		//                 (delete only)
		//
		//   -2      0       -2      0       -2      0
		//   /      / \      /      / \      /      / \
		//  1   -> 0   1    1   -> 0   0    1  -> -1   0
		//   \               \               \
		//   -1               0               1
		//                 (delete only)

	protected:
		template <int Direction> inline void Rotate(AVLTreeNode **lpRootNode){ // 0 -> right rotate, 1 -> left rotate
			AVLTreeNode *ChildA = (*lpRootNode)->Child[Direction];
			(*lpRootNode)->Child[Direction] = ChildA->Child[1 - Direction];
			ChildA->Child[1 - Direction] = *lpRootNode;

			ChildA->Parent = (*lpRootNode)->Parent;
			(*lpRootNode)->Parent = ChildA;
			if (nullptr != (*lpRootNode)->Child[Direction])((*lpRootNode)->Child[Direction])->Parent = *lpRootNode;

			*lpRootNode = ChildA;
		}
	public:
		typedef AVLTreeNode node_type;

		AVLTreeNode *lpRoot;
		UBINT Size;

		//constructors
		void Initialize(){
			this->lpRoot = nullptr;
			this->Size = 0;
		}

		void insert(AVLTreeNode *CurNodePtr, AVLTreeNode *ParentPtr){
			// Insert a new node to the tree
			CurNodePtr->Parent = ParentPtr;
			CurNodePtr->Child[0] = nullptr;
			CurNodePtr->Child[1] = nullptr;
			CurNodePtr->Balance = 0;

			if (nullptr != ParentPtr){
				ParentPtr->Balance += (CurNodePtr == ParentPtr->Child[0]) ? -1 : 1;
				if (0 != ParentPtr->Balance){
					// the height of parent is increased, we need to retrace
					while (nullptr != ParentPtr->Parent){
						AVLTreeNode *GrandParentPtr = ParentPtr->Parent;
						AVLTreeNode **lpParentPtr;
						if (ParentPtr == GrandParentPtr->Child[0]){
							GrandParentPtr->Balance--;
							lpParentPtr = &GrandParentPtr->Child[0];
						}
						else{
							GrandParentPtr->Balance++;
							lpParentPtr = &GrandParentPtr->Child[1];
						}

						//the balance factor part is a bit hard to understand. See the comment at the beginning of this file for reference.
						if (GrandParentPtr->Balance > 1){
							// a rotation is required
							AVLTreeNode **lpGrandParentPtr;
							if (nullptr == GrandParentPtr->Parent)lpGrandParentPtr = &this->lpRoot;
							else if (GrandParentPtr == GrandParentPtr->Parent->Child[0])lpGrandParentPtr = &GrandParentPtr->Parent->Child[0];
							else lpGrandParentPtr = &GrandParentPtr->Parent->Child[1];

							if (ParentPtr->Balance < 0){
								if (ParentPtr->Child[0]->Balance < 0)ParentPtr->Balance = 1; else ParentPtr->Balance = 0;
								if (ParentPtr->Child[0]->Balance > 0)GrandParentPtr->Balance = -1; else GrandParentPtr->Balance = 0;
								Rotate<0>(lpParentPtr);
								//the result may be temporarily unbalanced
							}
							else GrandParentPtr->Balance = 0;

							Rotate<1>(lpGrandParentPtr);
							(*lpGrandParentPtr)->Balance = 0;
							break;
						}
						else if (GrandParentPtr->Balance < -1){
							// a rotation is required
							AVLTreeNode **lpGrandParentPtr;
							if (nullptr == GrandParentPtr->Parent)lpGrandParentPtr = &this->lpRoot;
							else if (GrandParentPtr == GrandParentPtr->Parent->Child[0])lpGrandParentPtr = &GrandParentPtr->Parent->Child[0];
							else lpGrandParentPtr = &GrandParentPtr->Parent->Child[1];

							if (ParentPtr->Balance > 0){
								if (ParentPtr->Child[1]->Balance > 0)ParentPtr->Balance = -1; else ParentPtr->Balance = 0;
								if (ParentPtr->Child[1]->Balance < 0)GrandParentPtr->Balance = 1; else GrandParentPtr->Balance = 0;
								Rotate<1>(lpParentPtr);
								//the result may be temporarily unbalanced
							}
							else GrandParentPtr->Balance = 0;

							Rotate<0>(lpGrandParentPtr);
							(*lpGrandParentPtr)->Balance = 0;
							break;
						}
						else if (0 != GrandParentPtr->Balance)ParentPtr = GrandParentPtr;
						else break;
					}
				}
			}
			this->Size++;
		}
		template < typename Reduce > void insert_reduce(AVLTreeNode *CurNodePtr, AVLTreeNode *ParentPtr, Reduce reduce_functor){
			// Insert a new node to the tree
			CurNodePtr->Parent = ParentPtr;
			CurNodePtr->Child[0] = nullptr;
			CurNodePtr->Child[1] = nullptr;
			CurNodePtr->Balance = 0;
			reduce_functor(CurNodePtr);

			if (nullptr != ParentPtr){
				ParentPtr->Balance += (CurNodePtr == ParentPtr->Child[0]) ? -1 : 1;
				if (0 != ParentPtr->Balance){
					// the height of parent is increased, we need to retrace
					while (nullptr != ParentPtr->Parent){
						AVLTreeNode *GrandParentPtr = ParentPtr->Parent;
						AVLTreeNode **lpParentPtr;
						if (ParentPtr == GrandParentPtr->Child[0]){
							GrandParentPtr->Balance--;
							lpParentPtr = &GrandParentPtr->Child[0];
						}
						else{
							GrandParentPtr->Balance++;
							lpParentPtr = &GrandParentPtr->Child[1];
						}

						//the balance factor part is a bit hard to understand. See the comment at the beginning of this file for reference.
						if (GrandParentPtr->Balance > 1){
							// a rotation is required
							AVLTreeNode **lpGrandParentPtr;
							if (nullptr == GrandParentPtr->Parent)lpGrandParentPtr = &this->lpRoot;
							else if (GrandParentPtr == GrandParentPtr->Parent->Child[0])lpGrandParentPtr = &GrandParentPtr->Parent->Child[0];
							else lpGrandParentPtr = &GrandParentPtr->Parent->Child[1];

							if (ParentPtr->Balance < 0){
								if (ParentPtr->Child[0]->Balance < 0)ParentPtr->Balance = 1; else ParentPtr->Balance = 0;
								if (ParentPtr->Child[0]->Balance > 0)GrandParentPtr->Balance = -1; else GrandParentPtr->Balance = 0;
								Rotate<0>(lpParentPtr);
								//the result may be temporarily unbalanced

								reduce_functor(ParentPtr);
							}
							else GrandParentPtr->Balance = 0;

							Rotate<1>(lpGrandParentPtr);
							(*lpGrandParentPtr)->Balance = 0;

							reduce_functor(GrandParentPtr);
							ParentPtr = *lpGrandParentPtr;
							break;
						}
						else if (GrandParentPtr->Balance < -1){
							// a rotation is required
							AVLTreeNode **lpGrandParentPtr;
							if (nullptr == GrandParentPtr->Parent)lpGrandParentPtr = &this->lpRoot;
							else if (GrandParentPtr == GrandParentPtr->Parent->Child[0])lpGrandParentPtr = &GrandParentPtr->Parent->Child[0];
							else lpGrandParentPtr = &GrandParentPtr->Parent->Child[1];

							if (ParentPtr->Balance > 0){
								if (ParentPtr->Child[1]->Balance > 0)ParentPtr->Balance = -1; else ParentPtr->Balance = 0;
								if (ParentPtr->Child[1]->Balance < 0)GrandParentPtr->Balance = 1; else GrandParentPtr->Balance = 0;
								Rotate<1>(lpParentPtr);
								//the result may be temporarily unbalanced

								reduce_functor(ParentPtr);
							}
							else GrandParentPtr->Balance = 0;

							Rotate<0>(lpGrandParentPtr);
							(*lpGrandParentPtr)->Balance = 0;

							reduce_functor(GrandParentPtr);
							ParentPtr = *lpGrandParentPtr;
							break;
						}
						else if (0 != GrandParentPtr->Balance){
							reduce_functor(ParentPtr);
							ParentPtr = GrandParentPtr;
						}
						else break;
					}
				}
				while (nullptr != ParentPtr){
					reduce_functor(ParentPtr);
					ParentPtr = ParentPtr->Parent;
				}
			}
			this->Size++;
		}
		AVLTreeNode *erase(AVLTreeNode *pos){
			// Unlink the pointer [pos] from the tree without actually destroying the content.
			// Whether the pointer is valid is not checked!
			AVLTreeNode **lpCurNodePtr, *ParentPtr, *RetValue;
			if (pos->Child[0] != nullptr && pos->Child[1] != nullptr){
				// get [RetValue]
				RetValue = pos->Child[1];
				while (nullptr != RetValue->Child[0])RetValue = RetValue->Child[0];

				// need to do an exchange
				ParentPtr = pos;

				lpCurNodePtr = &pos->Child[0];
				while (nullptr != (*lpCurNodePtr)->Child[1]){
					ParentPtr = *lpCurNodePtr;
					lpCurNodePtr = &(*lpCurNodePtr)->Child[1];
				}
				// now it's impossible for lpCurNodePtr to have a right child.
				AVLTreeNode *CurNodePtr = *lpCurNodePtr;
				if (ParentPtr == pos){
					CurNodePtr->Parent = pos->Parent;
					pos->Parent = CurNodePtr;
					pos->Child[0] = CurNodePtr->Child[0];
					CurNodePtr->Child[0] = pos;
					CurNodePtr->Child[1] = pos->Child[1]; pos->Child[1] = nullptr; //std::swap(pos->Child[1], (*lpCurNodePtr)->Child[1]);
					std::swap(pos->Balance, CurNodePtr->Balance);

					//children
					CurNodePtr->Child[1]->Parent = CurNodePtr;
					if (nullptr != pos->Child[0])pos->Child[0]->Parent = pos;

					//parent
					if (nullptr == CurNodePtr->Parent)this->lpRoot = CurNodePtr;
					else if (pos == CurNodePtr->Parent->Child[0])CurNodePtr->Parent->Child[0] = CurNodePtr;
					else CurNodePtr->Parent->Child[1] = CurNodePtr;

					ParentPtr = CurNodePtr;
					lpCurNodePtr = &CurNodePtr->Child[0];
				}
				else{
					std::swap(pos->Parent, CurNodePtr->Parent);
					std::swap(pos->Child[0], CurNodePtr->Child[0]);
					CurNodePtr->Child[1] = pos->Child[1]; pos->Child[1] = nullptr; //std::swap(pos->Child[1], (*lpCurNodePtr)->Child[1]);
					std::swap(pos->Balance, CurNodePtr->Balance);

					//children
					CurNodePtr->Child[0]->Parent = CurNodePtr;
					CurNodePtr->Child[1]->Parent = CurNodePtr;
					if (nullptr != pos->Child[0])pos->Child[0]->Parent = pos;

					//parent
					if (nullptr == CurNodePtr->Parent)this->lpRoot = CurNodePtr;
					else if (pos == CurNodePtr->Parent->Child[0])CurNodePtr->Parent->Child[0] = CurNodePtr;
					else CurNodePtr->Parent->Child[1] = CurNodePtr;

					*lpCurNodePtr = pos;
				}
			}
			else if (this->lpRoot != pos){
				// get [RetValue]
				if (nullptr != pos->Child[1]){
					RetValue = pos->Child[1];
					while (nullptr != RetValue->Child[0])RetValue = RetValue->Child[0];
				}
				else{
					RetValue = pos;
					while (nullptr != RetValue->Parent && RetValue->Parent->Child[1] == RetValue)RetValue = RetValue->Parent;
					RetValue = RetValue->Parent;
				}

				ParentPtr = pos->Parent;
				if (nullptr == ParentPtr)lpCurNodePtr = &this->lpRoot;
				else if (pos == ParentPtr->Child[0])lpCurNodePtr = &ParentPtr->Child[0];
				else lpCurNodePtr = &ParentPtr->Child[1];
			}
			else{
				//delete the node directly
				if (nullptr != pos->Child[0]){
					this->lpRoot = pos->Child[0];
					this->lpRoot->Parent = nullptr;
					RetValue = nullptr;
				}
				else{
					this->lpRoot = pos->Child[1];
					if (nullptr != this->lpRoot)this->lpRoot->Parent = nullptr;
					RetValue = this->lpRoot;
				}
				this->Size--;
				return RetValue;
			}

			if (pos == ParentPtr->Child[0])ParentPtr->Balance++;
			else ParentPtr->Balance--;
			if (nullptr != pos->Child[0]){
				*lpCurNodePtr = pos->Child[0];
				pos->Child[0]->Parent = ParentPtr;
			}
			else{
				*lpCurNodePtr = pos->Child[1];
				if (nullptr != pos->Child[1])pos->Child[1]->Parent = ParentPtr;
			}

			// retracing
			AVLTreeNode *PrevPtr = *lpCurNodePtr;
			while (true){
				if (abs(ParentPtr->Balance) == 1)break;
				else{
					//the balance factor part is a bit hard to understand. See the comment at the beginning of this file for reference.
					if (ParentPtr->Balance > 1){
						// a rotation is required
						AVLTreeNode **lpParentPtr;
						if (nullptr == ParentPtr->Parent)lpParentPtr = &this->lpRoot;
						else if (ParentPtr == ParentPtr->Parent->Child[0])lpParentPtr = &ParentPtr->Parent->Child[0];
						else lpParentPtr = &ParentPtr->Parent->Child[1];

						AVLTreeNode **lpChildPtr = &ParentPtr->Child[1], *ChildPtr = *lpChildPtr;
						if (0 == ChildPtr->Balance){
							ParentPtr->Balance = 1;
							Rotate<1>(lpParentPtr);
							(*lpParentPtr)->Balance = -1;
							break;
						}
						else{
							if (ChildPtr->Balance < 0){
								if (ChildPtr->Child[0]->Balance < 0)ChildPtr->Balance = 1; else ChildPtr->Balance = 0;
								if (ChildPtr->Child[0]->Balance > 0)ParentPtr->Balance = -1; else ParentPtr->Balance = 0;
								Rotate<0>(lpChildPtr);
								//the result may be temporarily unbalanced
							}
							else ParentPtr->Balance = 0;

							Rotate<1>(lpParentPtr);
							(*lpParentPtr)->Balance = 0;

							PrevPtr = *lpParentPtr;
						}
					}
					else if (ParentPtr->Balance < -1){
						// a rotation is required
						AVLTreeNode **lpParentPtr;
						if (nullptr == ParentPtr->Parent)lpParentPtr = &this->lpRoot;
						else if (ParentPtr == ParentPtr->Parent->Child[0])lpParentPtr = &ParentPtr->Parent->Child[0];
						else lpParentPtr = &ParentPtr->Parent->Child[1];

						AVLTreeNode **lpChildPtr = &ParentPtr->Child[0], *ChildPtr = *lpChildPtr;
						if (0 == ChildPtr->Balance){
							ParentPtr->Balance = -1;
							Rotate<0>(lpParentPtr);
							(*lpParentPtr)->Balance = 1;
							break;
						}
						else{
							if (ChildPtr->Balance > 0){
								if (ChildPtr->Child[1]->Balance > 0)ChildPtr->Balance = -1; else ChildPtr->Balance = 0;
								if (ChildPtr->Child[1]->Balance < 0)ParentPtr->Balance = 1; else ParentPtr->Balance = 0;
								Rotate<1>(lpChildPtr);
								//the result may be temporarily unbalanced
							}
							else ParentPtr->Balance = 0;

							Rotate<0>(lpParentPtr);
							(*lpParentPtr)->Balance = 0;

							PrevPtr = *lpParentPtr;
						}
					}
					else PrevPtr = ParentPtr;
				}
				ParentPtr = PrevPtr->Parent;
				if (nullptr != ParentPtr){
					if (PrevPtr == ParentPtr->Child[0])ParentPtr->Balance++;
					else ParentPtr->Balance--;
				}
				else break;
			}
			this->Size--;
			return RetValue;
		}

		template < typename Reduce > AVLTreeNode *erase_reduce(AVLTreeNode *pos, Reduce reduce_functor){
			// Unlink the pointer [pos] from the tree without actually destroying the content.
			// Whether the pointer is valid is not checked!
			AVLTreeNode **lpCurNodePtr, *ParentPtr, *RetValue;
			if (pos->Child[0] != nullptr && pos->Child[1] != nullptr){
				// get [RetValue]
				RetValue = pos->Child[1];
				while (nullptr != RetValue->Child[0])RetValue = RetValue->Child[0];

				// need to do an exchange
				ParentPtr = pos;

				lpCurNodePtr = &pos->Child[0];
				while (nullptr != (*lpCurNodePtr)->Child[1]){
					ParentPtr = *lpCurNodePtr;
					lpCurNodePtr = &(*lpCurNodePtr)->Child[1];
				}
				// now it's impossible for lpCurNodePtr to have a right child.
				AVLTreeNode *CurNodePtr = *lpCurNodePtr;
				if (ParentPtr == pos){
					CurNodePtr->Parent = pos->Parent;
					pos->Parent = CurNodePtr;
					pos->Child[0] = CurNodePtr->Child[0];
					CurNodePtr->Child[0] = pos;
					CurNodePtr->Child[1] = pos->Child[1]; pos->Child[1] = nullptr; //std::swap(pos->Child[1], (*lpCurNodePtr)->Child[1]);
					std::swap(pos->Balance, CurNodePtr->Balance);

					//children
					CurNodePtr->Child[1]->Parent = CurNodePtr;
					if (nullptr != pos->Child[0])pos->Child[0]->Parent = pos;

					//parent
					if (nullptr == CurNodePtr->Parent)this->lpRoot = CurNodePtr;
					else if (pos == CurNodePtr->Parent->Child[0])CurNodePtr->Parent->Child[0] = CurNodePtr;
					else CurNodePtr->Parent->Child[1] = CurNodePtr;

					ParentPtr = CurNodePtr;
					lpCurNodePtr = &CurNodePtr->Child[0];
				}
				else{
					std::swap(pos->Parent, CurNodePtr->Parent);
					std::swap(pos->Child[0], CurNodePtr->Child[0]);
					CurNodePtr->Child[1] = pos->Child[1]; pos->Child[1] = nullptr; //std::swap(pos->Child[1], (*lpCurNodePtr)->Child[1]);
					std::swap(pos->Balance, CurNodePtr->Balance);

					//children
					CurNodePtr->Child[0]->Parent = CurNodePtr;
					CurNodePtr->Child[1]->Parent = CurNodePtr;
					if (nullptr != pos->Child[0])pos->Child[0]->Parent = pos;

					//parent
					if (nullptr == CurNodePtr->Parent)this->lpRoot = CurNodePtr;
					else if (pos == CurNodePtr->Parent->Child[0])CurNodePtr->Parent->Child[0] = CurNodePtr;
					else CurNodePtr->Parent->Child[1] = CurNodePtr;

					*lpCurNodePtr = pos;
				}
			}
			else if (this->lpRoot != pos){
				// get [RetValue]
				if (nullptr != pos->Child[1]){
					RetValue = pos->Child[1];
					while (nullptr != RetValue->Child[0])RetValue = RetValue->Child[0];
				}
				else{
					RetValue = pos;
					while (nullptr != RetValue->Parent && RetValue->Parent->Child[1] == RetValue)RetValue = RetValue->Parent;
					RetValue = RetValue->Parent;
				}

				ParentPtr = pos->Parent;
				if (nullptr == ParentPtr)lpCurNodePtr = &this->lpRoot;
				else if (pos == ParentPtr->Child[0])lpCurNodePtr = &ParentPtr->Child[0];
				else lpCurNodePtr = &ParentPtr->Child[1];
			}
			else{
				//delete the node directly
				if (nullptr != pos->Child[0]){
					this->lpRoot = pos->Child[0];
					this->lpRoot->Parent = nullptr;
					RetValue = nullptr;
				}
				else{
					this->lpRoot = pos->Child[1];
					if (nullptr != this->lpRoot)this->lpRoot->Parent = nullptr;
					RetValue = this->lpRoot;
				}
				this->Size--;
				return RetValue;
			}

			if (pos == ParentPtr->Child[0])ParentPtr->Balance++;
			else ParentPtr->Balance--;
			if (nullptr != pos->Child[0]){
				*lpCurNodePtr = pos->Child[0];
				pos->Child[0]->Parent = ParentPtr;
			}
			else{
				*lpCurNodePtr = pos->Child[1];
				if (nullptr != pos->Child[1])pos->Child[1]->Parent = ParentPtr;
			}

			// retracing
			AVLTreeNode *PrevPtr = *lpCurNodePtr;
			while (true){
				if (abs(ParentPtr->Balance) == 1)break;
				else{
					//the balance factor part is a bit hard to understand. See the comment at the beginning of this file for reference.
					if (ParentPtr->Balance > 1){
						// a rotation is required
						AVLTreeNode **lpParentPtr;
						if (nullptr == ParentPtr->Parent)lpParentPtr = &this->lpRoot;
						else if (ParentPtr == ParentPtr->Parent->Child[0])lpParentPtr = &ParentPtr->Parent->Child[0];
						else lpParentPtr = &ParentPtr->Parent->Child[1];

						AVLTreeNode **lpChildPtr = &ParentPtr->Child[1], *ChildPtr = *lpChildPtr;
						if (0 == ChildPtr->Balance){
							ParentPtr->Balance = 1;
							Rotate<1>(lpParentPtr);
							(*lpParentPtr)->Balance = -1;
							break;
						}
						else{
							if (ChildPtr->Balance < 0){
								if (ChildPtr->Child[0]->Balance < 0)ChildPtr->Balance = 1; else ChildPtr->Balance = 0;
								if (ChildPtr->Child[0]->Balance > 0)ParentPtr->Balance = -1; else ParentPtr->Balance = 0;
								Rotate<0>(lpChildPtr);
								//the result may be temporarily unbalanced

								reduce_functor(ChildPtr);
							}
							else ParentPtr->Balance = 0;

							Rotate<1>(lpParentPtr);
							(*lpParentPtr)->Balance = 0;

							reduce_functor(ParentPtr);
							PrevPtr = *lpParentPtr;
						}
					}
					else if (ParentPtr->Balance < -1){
						// a rotation is required
						AVLTreeNode **lpParentPtr;
						if (nullptr == ParentPtr->Parent)lpParentPtr = &this->lpRoot;
						else if (ParentPtr == ParentPtr->Parent->Child[0])lpParentPtr = &ParentPtr->Parent->Child[0];
						else lpParentPtr = &ParentPtr->Parent->Child[1];

						AVLTreeNode **lpChildPtr = &ParentPtr->Child[0], *ChildPtr = *lpChildPtr;
						if (0 == ChildPtr->Balance){
							ParentPtr->Balance = -1;
							Rotate<0>(lpParentPtr);
							(*lpParentPtr)->Balance = 1;
							break;
						}
						else{
							if (ChildPtr->Balance > 0){
								if (ChildPtr->Child[1]->Balance > 0)ChildPtr->Balance = -1; else ChildPtr->Balance = 0;
								if (ChildPtr->Child[1]->Balance < 0)ParentPtr->Balance = 1; else ParentPtr->Balance = 0;
								Rotate<1>(lpChildPtr);
								//the result may be temporarily unbalanced

								reduce_functor(ChildPtr);
							}
							else ParentPtr->Balance = 0;

							Rotate<0>(lpParentPtr);
							(*lpParentPtr)->Balance = 0;

							reduce_functor(ParentPtr);
							PrevPtr = *lpParentPtr;
						}
					}
					else PrevPtr = ParentPtr;
					reduce_functor(PrevPtr);
				}
				ParentPtr = PrevPtr->Parent;
				if (nullptr != ParentPtr){
					if (PrevPtr == ParentPtr->Child[0])ParentPtr->Balance++;
					else ParentPtr->Balance--;
				}
				else break;
			}
			while (nullptr != ParentPtr){
				reduce_functor(ParentPtr);
				ParentPtr = ParentPtr->Parent;
			}
			this->Size--;
			return RetValue;
		}

		static BINT Height(const AVLTreeNode *pos){
			if (nullptr == pos)return 0;
			else{
				BINT LH = Height(pos->Child[0]);
				BINT RH = Height(pos->Child[1]);
				if (0xFFFFFFFF == LH || 0xFFFFFFFF == RH)return 0xFFFFFFFF;
				if (RH - LH != pos->Balance)return 0xFFFFFFFF;
				if (nullptr != pos->Child[0] && pos->Child[0]->Parent != pos)return 0xFFFFFFFF;
				if (nullptr != pos->Child[1] && pos->Child[1]->Parent != pos)return 0xFFFFFFFF;
				return std::max(LH, RH) + 1;
			}
		}
		bool IsCorrect() const{
			return Height(this->lpRoot) != 0xFFFFFFFF;
		}
	};
}

#endif