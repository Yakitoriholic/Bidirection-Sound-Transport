/* Description: Red-Black Tree Implementation. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_RBTREE
#define LIB_CONTAINER_RBTREE

#include "lGeneral.hpp"
#include "lContainer_BinaryTree.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	struct RBTreeNode{
		RBTreeNode*	Parent;
		RBTreeNode* Child[2];
		UBINT Color; // 0 = black  1 = Red
	};

	class RBTree{
	protected:
		template<int Direction> inline RBTreeNode *RBTree_Insert_RotateBranch(RBTreeNode **lpRoot, RBTreeNode *CurNodePtr, RBTreeNode *ParentPtr, RBTreeNode *GrandParentPtr){
			RBTreeNode *UncleNode = GrandParentPtr->Child[1 - Direction];
			if (nullptr == UncleNode || 0 == UncleNode->Color){
				if (ParentPtr->Child[1 - Direction] == CurNodePtr){
					//Case 4
					CurNodePtr->Color = 0;

					ParentPtr->Child[1 - Direction] = CurNodePtr->Child[Direction];
					ParentPtr->Parent = CurNodePtr;
					CurNodePtr->Child[Direction] = ParentPtr;

					GrandParentPtr->Child[Direction] = CurNodePtr->Child[1 - Direction];
					CurNodePtr->Child[1 - Direction] = GrandParentPtr;
					CurNodePtr->Parent = GrandParentPtr->Parent;
					GrandParentPtr->Parent = CurNodePtr;

					if (nullptr != ParentPtr->Child[1 - Direction])ParentPtr->Child[1 - Direction]->Parent = ParentPtr;
					if (nullptr != GrandParentPtr->Child[Direction])GrandParentPtr->Child[Direction]->Parent = GrandParentPtr;

					if (nullptr == CurNodePtr->Parent)*lpRoot = CurNodePtr;
					else if (CurNodePtr->Parent->Child[Direction] == GrandParentPtr)CurNodePtr->Parent->Child[Direction] = CurNodePtr;
					else CurNodePtr->Parent->Child[1 - Direction] = CurNodePtr;
				}
				else{
					//Case 5
					ParentPtr->Color = 0;

					GrandParentPtr->Child[Direction] = ParentPtr->Child[1 - Direction];
					ParentPtr->Child[1 - Direction] = GrandParentPtr;
					ParentPtr->Parent = GrandParentPtr->Parent;
					GrandParentPtr->Parent = ParentPtr;

					if (nullptr != GrandParentPtr->Child[Direction])GrandParentPtr->Child[Direction]->Parent = GrandParentPtr;

					if (nullptr == ParentPtr->Parent)*lpRoot = ParentPtr;
					else if (ParentPtr->Parent->Child[Direction] == GrandParentPtr)ParentPtr->Parent->Child[Direction] = ParentPtr;
					else ParentPtr->Parent->Child[1 - Direction] = ParentPtr;
				}
				return nullptr;
			}
			else{
				//Case 3
				ParentPtr->Color = 0;
				UncleNode->Color = 0;
				if (nullptr == GrandParentPtr->Parent){ GrandParentPtr->Color = 0; return nullptr; }
				else return GrandParentPtr;
			}
		}
		template<int Direction, typename Reduce> inline bool RBTree_Insert_RotateBranch_Reduce(RBTreeNode **lpRoot, RBTreeNode **lpCurNodePtr, RBTreeNode *ParentPtr, RBTreeNode *GrandParentPtr, Reduce reduce_functor){
			RBTreeNode *CurNodePtr = *lpCurNodePtr;
			RBTreeNode *UncleNode = GrandParentPtr->Child[1 - Direction];
			if (nullptr == UncleNode || 0 == UncleNode->Color){
				if (ParentPtr->Child[1 - Direction] == CurNodePtr){
					//Case 4
					CurNodePtr->Color = 0;

					ParentPtr->Child[1 - Direction] = CurNodePtr->Child[Direction];
					ParentPtr->Parent = CurNodePtr;
					CurNodePtr->Child[Direction] = ParentPtr;

					GrandParentPtr->Child[Direction] = CurNodePtr->Child[1 - Direction];
					CurNodePtr->Child[1 - Direction] = GrandParentPtr;
					CurNodePtr->Parent = GrandParentPtr->Parent;
					GrandParentPtr->Parent = CurNodePtr;

					if (nullptr != ParentPtr->Child[1 - Direction])ParentPtr->Child[1 - Direction]->Parent = ParentPtr;
					if (nullptr != GrandParentPtr->Child[Direction])GrandParentPtr->Child[Direction]->Parent = GrandParentPtr;

					if (nullptr == CurNodePtr->Parent)*lpRoot = CurNodePtr;
					else if (CurNodePtr->Parent->Child[Direction] == GrandParentPtr)CurNodePtr->Parent->Child[Direction] = CurNodePtr;
					else CurNodePtr->Parent->Child[1 - Direction] = CurNodePtr;

					reduce_functor(ParentPtr);
					reduce_functor(GrandParentPtr);

					*lpCurNodePtr = CurNodePtr;
				}
				else{
					//Case 5
					ParentPtr->Color = 0;

					GrandParentPtr->Child[Direction] = ParentPtr->Child[1 - Direction];
					ParentPtr->Child[1 - Direction] = GrandParentPtr;
					ParentPtr->Parent = GrandParentPtr->Parent;
					GrandParentPtr->Parent = ParentPtr;

					if (nullptr != GrandParentPtr->Child[Direction])GrandParentPtr->Child[Direction]->Parent = GrandParentPtr;

					if (nullptr == ParentPtr->Parent)*lpRoot = ParentPtr;
					else if (ParentPtr->Parent->Child[Direction] == GrandParentPtr)ParentPtr->Parent->Child[Direction] = ParentPtr;
					else ParentPtr->Parent->Child[1 - Direction] = ParentPtr;

					reduce_functor(GrandParentPtr);

					*lpCurNodePtr = ParentPtr;
				}
				return true;
			}
			else{
				//Case 3
				ParentPtr->Color = 0;
				UncleNode->Color = 0;
				reduce_functor(ParentPtr);
				*lpCurNodePtr = GrandParentPtr;
				if (nullptr == GrandParentPtr->Parent){ GrandParentPtr->Color = 0; return true; }
				else{
					reduce_functor(GrandParentPtr);
					return false;
				}
			}
		}
		template<int Direction> inline RBTreeNode *RBTree_Delete_RotateBranch(RBTreeNode **lpRoot, RBTreeNode *ParentNode){
			RBTreeNode *SibNode = ParentNode->Child[1 - Direction];
			if (1 == SibNode->Color){
				//Case 2:CP Length
				RBTreeNode *NephewNode = SibNode->Child[Direction];

				ParentNode->Color = 0; // After 5/6 spin, its color will become black.
				ParentNode->Child[1 - Direction] = NephewNode;
				NephewNode->Parent = ParentNode;
				SibNode->Color = 0;
				SibNode->Child[Direction] = ParentNode;
				SibNode->Parent = ParentNode->Parent;
				ParentNode->Parent = SibNode;

				if (nullptr != NephewNode->Child[1 - Direction] && 1 == NephewNode->Child[1 - Direction]->Color){
					//Case 2-6
					NephewNode->Child[1 - Direction]->Color = 0;
					SibNode->Child[Direction] = NephewNode;
					NephewNode->Parent = SibNode;
					ParentNode->Child[1 - Direction] = NephewNode->Child[Direction];
					NephewNode->Child[Direction] = ParentNode;
					ParentNode->Parent = NephewNode;
					NephewNode->Color = 1;

					if (nullptr != ParentNode->Child[1 - Direction])ParentNode->Child[1 - Direction]->Parent = ParentNode;
				}
				else if (nullptr != NephewNode->Child[Direction] && 1 == NephewNode->Child[Direction]->Color){
					//Case 2-5
					RBTreeNode *TmpNode = NephewNode->Child[Direction];

					SibNode->Child[Direction] = TmpNode;
					TmpNode->Parent = SibNode;

					ParentNode->Child[1 - Direction] = TmpNode->Child[Direction];
					TmpNode->Child[Direction] = ParentNode;
					ParentNode->Parent = TmpNode;
					NephewNode->Child[Direction] = TmpNode->Child[1 - Direction];
					TmpNode->Child[1 - Direction] = NephewNode;
					NephewNode->Parent = TmpNode;
					NephewNode->Color = 0;

					if (nullptr != ParentNode->Child[1 - Direction])ParentNode->Child[1 - Direction]->Parent = ParentNode;
					if (nullptr != NephewNode->Child[Direction])NephewNode->Child[Direction]->Parent = NephewNode;
				}
				else{
					NephewNode->Color = 1;
				}
				if (nullptr == SibNode->Parent)*lpRoot = SibNode;
				else if (SibNode->Parent->Child[1 - Direction] == ParentNode)SibNode->Parent->Child[1 - Direction] = SibNode;
				else SibNode->Parent->Child[Direction] = SibNode;

				return nullptr;
			}
			else if (nullptr != SibNode->Child[1 - Direction] && 1 == SibNode->Child[1 - Direction]->Color){
				//Case 6
				SibNode->Child[1 - Direction]->Color = 0;

				SibNode->Color = ParentNode->Color;
				ParentNode->Child[1 - Direction] = SibNode->Child[Direction];
				ParentNode->Color = 0;
				SibNode->Child[Direction] = ParentNode;
				SibNode->Parent = ParentNode->Parent;
				ParentNode->Parent = SibNode;

				if (nullptr != ParentNode->Child[1 - Direction])ParentNode->Child[1 - Direction]->Parent = ParentNode;

				if (nullptr == SibNode->Parent)*lpRoot = SibNode;
				else if (SibNode->Parent->Child[1 - Direction] == ParentNode)SibNode->Parent->Child[1 - Direction] = SibNode;
				else SibNode->Parent->Child[Direction] = SibNode;

				return nullptr;
			}
			else if (nullptr != SibNode->Child[Direction] && 1 == SibNode->Child[Direction]->Color){
				//Case 5
				RBTreeNode *TmpNode = SibNode->Child[Direction];

				SibNode->Child[Direction] = TmpNode->Child[1 - Direction];
				ParentNode->Child[1 - Direction] = TmpNode->Child[Direction];
				TmpNode->Child[1 - Direction] = SibNode;
				TmpNode->Child[Direction] = ParentNode;
				TmpNode->Color = ParentNode->Color;
				TmpNode->Parent = ParentNode->Parent;
				ParentNode->Parent = TmpNode;
				SibNode->Parent = TmpNode;
				ParentNode->Color = 0;

				if (nullptr != ParentNode->Child[1 - Direction])ParentNode->Child[1 - Direction]->Parent = ParentNode;
				if (nullptr != SibNode->Child[Direction])SibNode->Child[Direction]->Parent = SibNode;

				if (nullptr == TmpNode->Parent)*lpRoot = TmpNode;
				else if (TmpNode->Parent->Child[1 - Direction] == ParentNode)TmpNode->Parent->Child[1 - Direction] = TmpNode;
				else TmpNode->Parent->Child[Direction] = TmpNode;

				return nullptr;
			}
			else{
				// Case 3,4
				SibNode->Color = 1;
				if (1 == ParentNode->Color){ ParentNode->Color = 0; return nullptr; }
				else if (nullptr == ParentNode->Parent)return nullptr;
				else return ParentNode;
			}
		}
		template<int Direction, typename Reduce> inline bool RBTree_Delete_RotateBranch_Reduce(RBTreeNode **lpRoot, RBTreeNode **lpParentNode, Reduce reduce_functor){
			RBTreeNode *ParentNode = *lpParentNode;
			RBTreeNode *SibNode = ParentNode->Child[1 - Direction];
			if (1 == SibNode->Color){
				//Case 2:CP Length
				RBTreeNode *NephewNode = SibNode->Child[Direction];

				ParentNode->Color = 0; // After 5/6 spin, its color will become black.
				ParentNode->Child[1 - Direction] = NephewNode;
				NephewNode->Parent = ParentNode;
				SibNode->Color = 0;
				SibNode->Child[Direction] = ParentNode;
				SibNode->Parent = ParentNode->Parent;
				ParentNode->Parent = SibNode;

				if (nullptr != NephewNode->Child[1 - Direction] && 1 == NephewNode->Child[1 - Direction]->Color){
					//Case 2-6
					NephewNode->Child[1 - Direction]->Color = 0;
					SibNode->Child[Direction] = NephewNode;
					NephewNode->Parent = SibNode;
					ParentNode->Child[1 - Direction] = NephewNode->Child[Direction];
					NephewNode->Child[Direction] = ParentNode;
					ParentNode->Parent = NephewNode;
					NephewNode->Color = 1;

					if (nullptr != ParentNode->Child[1 - Direction])ParentNode->Child[1 - Direction]->Parent = ParentNode;

					reduce_functor(ParentNode);
					reduce_functor(NephewNode);
				}
				else if (nullptr != NephewNode->Child[Direction] && 1 == NephewNode->Child[Direction]->Color){
					//Case 2-5
					RBTreeNode *TmpNode = NephewNode->Child[Direction];

					SibNode->Child[Direction] = TmpNode;
					TmpNode->Parent = SibNode;

					ParentNode->Child[1 - Direction] = TmpNode->Child[Direction];
					TmpNode->Child[Direction] = ParentNode;
					ParentNode->Parent = TmpNode;
					NephewNode->Child[Direction] = TmpNode->Child[1 - Direction];
					TmpNode->Child[1 - Direction] = NephewNode;
					NephewNode->Parent = TmpNode;
					NephewNode->Color = 0;

					if (nullptr != ParentNode->Child[1 - Direction])ParentNode->Child[1 - Direction]->Parent = ParentNode;
					if (nullptr != NephewNode->Child[Direction])NephewNode->Child[Direction]->Parent = NephewNode;

					reduce_functor(NephewNode);
					reduce_functor(ParentNode);
					reduce_functor(TmpNode);
				}
				else{
					NephewNode->Color = 1;
					reduce_functor(ParentNode);
				}
				if (nullptr == SibNode->Parent)*lpRoot = SibNode;
				else if (SibNode->Parent->Child[1 - Direction] == ParentNode)SibNode->Parent->Child[1 - Direction] = SibNode;
				else SibNode->Parent->Child[Direction] = SibNode;

				*lpParentNode = SibNode;
				return true;
			}
			else if (nullptr != SibNode->Child[1 - Direction] && 1 == SibNode->Child[1 - Direction]->Color){
				//Case 6
				SibNode->Child[1 - Direction]->Color = 0;

				SibNode->Color = ParentNode->Color;
				ParentNode->Child[1 - Direction] = SibNode->Child[Direction];
				ParentNode->Color = 0;
				SibNode->Child[Direction] = ParentNode;
				SibNode->Parent = ParentNode->Parent;
				ParentNode->Parent = SibNode;

				if (nullptr != ParentNode->Child[1 - Direction])ParentNode->Child[1 - Direction]->Parent = ParentNode;

				if (nullptr == SibNode->Parent)*lpRoot = SibNode;
				else if (SibNode->Parent->Child[1 - Direction] == ParentNode)SibNode->Parent->Child[1 - Direction] = SibNode;
				else SibNode->Parent->Child[Direction] = SibNode;

				reduce_functor(ParentNode);
				*lpParentNode = SibNode;
				return true;
			}
			else if (nullptr != SibNode->Child[Direction] && 1 == SibNode->Child[Direction]->Color){
				//Case 5
				RBTreeNode *TmpNode = SibNode->Child[Direction];

				SibNode->Child[Direction] = TmpNode->Child[1 - Direction];
				ParentNode->Child[1 - Direction] = TmpNode->Child[Direction];
				TmpNode->Child[1 - Direction] = SibNode;
				TmpNode->Child[Direction] = ParentNode;
				TmpNode->Color = ParentNode->Color;
				TmpNode->Parent = ParentNode->Parent;
				ParentNode->Parent = TmpNode;
				SibNode->Parent = TmpNode;
				ParentNode->Color = 0;

				if (nullptr != ParentNode->Child[1 - Direction])ParentNode->Child[1 - Direction]->Parent = ParentNode;
				if (nullptr != SibNode->Child[Direction])SibNode->Child[Direction]->Parent = SibNode;

				if (nullptr == TmpNode->Parent)*lpRoot = TmpNode;
				else if (TmpNode->Parent->Child[1 - Direction] == ParentNode)TmpNode->Parent->Child[1 - Direction] = TmpNode;
				else TmpNode->Parent->Child[Direction] = TmpNode;

				reduce_functor(ParentNode);
				reduce_functor(SibNode);
				*lpParentNode = TmpNode;
				return true;
			}
			else{
				// Case 3,4
				SibNode->Color = 1;
				if (1 == ParentNode->Color){ ParentNode->Color = 0; return true; }
				else if (nullptr == ParentNode->Parent)return true;
				else{
					reduce_functor(ParentNode);
					return false;
				}
			}
		}
	public:
		typedef RBTreeNode node_type;

		RBTreeNode *lpRoot;
		UBINT Size;

		//constructors
		void Initialize(){
			this->lpRoot = nullptr;
			this->Size = 0;
		}

		void insert(RBTreeNode *CurNodePtr, RBTreeNode *ParentPtr){
			// Insert a new node to the tree
			CurNodePtr->Parent = ParentPtr;
			CurNodePtr->Child[0] = nullptr;
			CurNodePtr->Child[1] = nullptr;

			if (nullptr == ParentPtr)CurNodePtr->Color = 0;
			else{
				CurNodePtr->Color = 1;
				while (true){
					if (0 == ParentPtr->Color)break;
					//otherwise the grandparent must exist, for the root is black
					RBTreeNode *GrandParentPtr = ParentPtr->Parent;
					GrandParentPtr->Color = 1;

					if (GrandParentPtr->Child[0] == ParentPtr)CurNodePtr = RBTree_Insert_RotateBranch<0>(&this->lpRoot, CurNodePtr, ParentPtr, GrandParentPtr);
					else CurNodePtr = RBTree_Insert_RotateBranch<1>(&this->lpRoot, CurNodePtr, ParentPtr, GrandParentPtr);
					if (nullptr == CurNodePtr)break;
					ParentPtr = CurNodePtr->Parent;
				}
			}
			this->Size++;
		}
		template < typename Reduce > void insert_reduce(RBTreeNode *CurNodePtr, RBTreeNode *ParentPtr, Reduce reduce_functor){
			// Insert a new node to the tree
			CurNodePtr->Parent = ParentPtr;
			CurNodePtr->Child[0] = nullptr;
			CurNodePtr->Child[1] = nullptr;
			reduce_functor(CurNodePtr);

			if (nullptr == ParentPtr)CurNodePtr->Color = 0;
			else{
				CurNodePtr->Color = 1;
				while (true){
					if (0 == ParentPtr->Color)break;
					//otherwise the grandparent must exist, for the root is black
					RBTreeNode *GrandParentPtr = ParentPtr->Parent;
					GrandParentPtr->Color = 1;

					bool Finished;
					if (GrandParentPtr->Child[0] == ParentPtr)Finished = RBTree_Insert_RotateBranch_Reduce<0>(&this->lpRoot, &CurNodePtr, ParentPtr, GrandParentPtr, reduce_functor);
					else Finished = RBTree_Insert_RotateBranch_Reduce<1>(&this->lpRoot, &CurNodePtr, ParentPtr, GrandParentPtr, reduce_functor);
					if (Finished){ ParentPtr = CurNodePtr; break; }
					else ParentPtr = CurNodePtr->Parent;
				}
			}
			while (nullptr != ParentPtr){
				reduce_functor(ParentPtr);
				ParentPtr = ParentPtr->Parent;
			}
			this->Size++;
		}
		RBTreeNode *erase(RBTreeNode *pos){
			// Unlink the pointer [pos] from the tree without actually destroying the content.
			// Whether the pointer is valid is not checked!
			RBTreeNode **lpCurNodePtr, *ParentPtr, *RetValue;
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
				RBTreeNode *CurNodePtr = *lpCurNodePtr;
				if (ParentPtr == pos){
					CurNodePtr->Parent = pos->Parent;
					pos->Parent = CurNodePtr;
					pos->Child[0] = CurNodePtr->Child[0];
					CurNodePtr->Child[0] = pos;
					CurNodePtr->Child[1] = pos->Child[1]; pos->Child[1] = nullptr; //std::swap(pos->Child[1], (*lpCurNodePtr)->Child[1]);
					std::swap(pos->Color, CurNodePtr->Color);

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
					std::swap(pos->Color, CurNodePtr->Color);

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
					this->lpRoot->Color = 0;
					RetValue = nullptr;
				}
				else{
					this->lpRoot = pos->Child[1];
					if (nullptr != this->lpRoot){
						this->lpRoot->Parent = nullptr;
						this->lpRoot->Color = 0;
					}
					RetValue = this->lpRoot;
				}
				this->Size--;
				return RetValue;
			}

			if (nullptr != pos->Child[0]){
				*lpCurNodePtr = pos->Child[0];
				pos->Child[0]->Parent = ParentPtr;
			}
			else{
				*lpCurNodePtr = pos->Child[1];
				if (nullptr != pos->Child[1])pos->Child[1]->Parent = ParentPtr;
			}

			if (0 == pos->Color){
				//Balancing
				if (nullptr != *lpCurNodePtr)(*lpCurNodePtr)->Color = 0; //if [*lpCurNodePtr] is not null, it must be red.
				else{
					RBTreeNode *CurNodePtr = *lpCurNodePtr;
					while (true){
						if (ParentPtr->Child[0] == CurNodePtr)CurNodePtr = RBTree_Delete_RotateBranch<0>(&this->lpRoot, ParentPtr);  // if [CurNodePtr] doesn't exist,then the sibling node must exist.
						else CurNodePtr = RBTree_Delete_RotateBranch<1>(&this->lpRoot, ParentPtr);
						if (nullptr == CurNodePtr)break;
						ParentPtr = ParentPtr->Parent;
					}
				}
			}
			this->Size--;
			return RetValue;
		}
		template < typename Reduce > RBTreeNode *erase_reduce(RBTreeNode *pos, Reduce reduce_functor){
			// Unlink the pointer [pos] from the tree without actually destroying the content.
			// Whether the pointer is valid is not checked!
			RBTreeNode **lpCurNodePtr, *ParentPtr, *RetValue;
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
				RBTreeNode *CurNodePtr = *lpCurNodePtr;
				if (ParentPtr == pos){
					CurNodePtr->Parent = pos->Parent;
					pos->Parent = CurNodePtr;
					pos->Child[0] = CurNodePtr->Child[0];
					CurNodePtr->Child[0] = pos;
					CurNodePtr->Child[1] = pos->Child[1]; pos->Child[1] = nullptr; //std::swap(pos->Child[1], (*lpCurNodePtr)->Child[1]);
					std::swap(pos->Color, CurNodePtr->Color);

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
					std::swap(pos->Color, CurNodePtr->Color);

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
					this->lpRoot->Color = 0;
					RetValue = nullptr;
				}
				else{
					this->lpRoot = pos->Child[1];
					if (nullptr != this->lpRoot){
						this->lpRoot->Parent = nullptr;
						this->lpRoot->Color = 0;
					}
					RetValue = this->lpRoot;
				}
				this->Size--;
				return RetValue;
			}

			if (nullptr != pos->Child[0]){
				*lpCurNodePtr = pos->Child[0];
				pos->Child[0]->Parent = ParentPtr;
			}
			else{
				*lpCurNodePtr = pos->Child[1];
				if (nullptr != pos->Child[1])pos->Child[1]->Parent = ParentPtr;
			}

			if (0 == pos->Color){
				//Balancing
				if (nullptr != *lpCurNodePtr)(*lpCurNodePtr)->Color = 0; //if [*lpCurNodePtr] is not null, it must be red.
				else{
					RBTreeNode *CurNodePtr = *lpCurNodePtr;
					while (true){
						bool Finished;
						if (ParentPtr->Child[0] == CurNodePtr)Finished = RBTree_Delete_RotateBranch_Reduce<0>(&this->lpRoot, &ParentPtr, reduce_functor);  // if [CurNodePtr] doesn't exist,then the sibling node must exist.
						else Finished = RBTree_Delete_RotateBranch_Reduce<1>(&this->lpRoot, &ParentPtr, reduce_functor);
						if (Finished)break;
						CurNodePtr = ParentPtr;
						ParentPtr = ParentPtr->Parent;
					}
				}
			}
			while (nullptr != ParentPtr){
				reduce_functor(ParentPtr);
				ParentPtr = ParentPtr->Parent;
			}
			this->Size--;
			return RetValue;
		}

		static BINT BlackHeight(const RBTreeNode *pos){
			if (nullptr == pos)return 0;
			else{
				BINT LH = BlackHeight(pos->Child[0]);
				BINT RH = BlackHeight(pos->Child[1]);
				if (LH != RH)return 0xFFFFFFFF;
				if (nullptr != pos->Child[0] && pos->Child[0]->Parent != pos)return 0xFFFFFFFF;
				if (nullptr != pos->Child[1] && pos->Child[1]->Parent != pos)return 0xFFFFFFFF;
				if (1 == pos->Color){
					if (nullptr != pos->Child[0] && 1 == pos->Child[0]->Color)return 0xFFFFFFFF;
					if (nullptr != pos->Child[1] && 1 == pos->Child[1]->Color)return 0xFFFFFFFF;
					return LH;
				}
				else return LH + 1;
			}
		}
		bool IsCorrect() const{
			return BlackHeight(this->lpRoot) != 0xFFFFFFFF;
		}
	};
}

#endif