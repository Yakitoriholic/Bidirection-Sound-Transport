#include "lGeneral.hpp"
#include "lMath_Rand.hpp"
#include <cstdio>
#include <cmath>
#include <ctime>

// include the tree structure for testing.

#include "Container/lContainer_RBTree.hpp"
typedef nsContainer::RBTree _treetype_test;

#if defined LIBENV_OS_LINUX
#include <curses.h>
#else
#include <conio.h>
#endif

struct TestMapNode : public _treetype_test::node_type{
	UBINT NodeCount;
	UBINT Data;
};

struct TestMapNode_Reduce{
	inline void operator()(_treetype_test::node_type *Node){
		TestMapNode *Node_Unpacked = static_cast<TestMapNode *>(Node);
		_treetype_test::node_type *Child0 = Node->Child[0], *Child1 = Node->Child[1];

		Node_Unpacked->NodeCount = 1;
		if (nullptr != Child0)Node_Unpacked->NodeCount += static_cast<TestMapNode *>(Child0)->NodeCount;
		if (nullptr != Child1)Node_Unpacked->NodeCount += static_cast<TestMapNode *>(Child1)->NodeCount;
	}
};

bool IsCorrect(_treetype_test::node_type *Node){
	if (nullptr == Node)return true;
	if (!IsCorrect(Node->Child[0]))return false;
	if (!IsCorrect(Node->Child[1]))return false;

	TestMapNode *Node_Unpacked = static_cast<TestMapNode *>(Node);
	_treetype_test::node_type *Child0 = Node->Child[0], *Child1 = Node->Child[1];

	UBINT TmpNodeCount = 1;
	if (nullptr != Child0)TmpNodeCount += static_cast<TestMapNode *>(Child0)->NodeCount;
	if (nullptr != Child1)TmpNodeCount += static_cast<TestMapNode *>(Child1)->NodeCount;
	return Node_Unpacked->NodeCount == TmpNodeCount;
}

extern void Test_TreeReduce(){
	const int ValCnt = 1000, Ops = 1000000;
	int ValList[ValCnt];
	TestMapNode *Exist[ValCnt];

	nsMath::RandGenerator_XORShiftPlus MyRand;
	_treetype_test MyTree;
	TestMapNode_Reduce MyReduceFunctor;

	MyTree.Initialize();
	for (UBINT i = 0; i < ValCnt; i++)Exist[i] = nullptr;
	for (UBINT i = 0; i < Ops; i++){
		UBINT a = MyRand() % ValCnt;
		if (Exist[a] != nullptr){
			MyTree.erase_reduce(Exist[a], MyReduceFunctor);
			Exist[a] = nullptr;
		}
		else{
			UBINT b = MyRand() % ValCnt;

			_treetype_test::node_type **lpCurNodePtr = &MyTree.lpRoot, *ParentPtr = nullptr;
			while (nullptr != *lpCurNodePtr){
				ParentPtr = *lpCurNodePtr;

				if (b == static_cast<TestMapNode *>(*lpCurNodePtr)->Data)break;
				else if (b < static_cast<TestMapNode *>(*lpCurNodePtr)->Data)lpCurNodePtr = &((*lpCurNodePtr)->Child[0]);
				else lpCurNodePtr = &((*lpCurNodePtr)->Child[1]);
			}
			if (nullptr == *lpCurNodePtr){
				TestMapNode *NewNode = (TestMapNode *)nsBasic::MemAlloc(sizeof(TestMapNode));
				NewNode->Data = b;

				*lpCurNodePtr = NewNode;
				MyTree.insert_reduce(NewNode, ParentPtr, MyReduceFunctor);

				ValList[a] = b;
				Exist[a] = NewNode;
			}
		}

		//check for consistency
		if (!MyTree.IsCorrect()){
			printf("Structure error at %d!", i);
			throw std::exception();
		}
		if (!IsCorrect(MyTree.lpRoot)){
			printf("Reduce error at %d!", i);
			throw std::exception();
		}
	}
	for (UBINT i = 0; i < ValCnt; i++){
		if (nullptr != Exist[i])nsBasic::MemFree(Exist[i], sizeof(TestMapNode));
	}
}