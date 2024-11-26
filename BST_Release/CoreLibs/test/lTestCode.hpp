#include "test/TestData.hpp"

#include "lGeneral.hpp"
#include "lGUI.hpp"
#include "lFile.hpp"
#include "lDSAdv.hpp"
#include <cstdio>
#include <cmath>
#include <ctime>

#if defined LIBENV_OS_LINUX
#include <curses.h>
#else
#include <conio.h>
#endif

extern void Test_PageMgr(){
	UBINT TestSize[0x100],TmpInt1,CurTime;
	void *TestAddr[0x100];
	printf("Press any key to start testing.\n");
	_getch();
	CurTime=(UBINT)time(NULL);
	srand(CurTime);
	for(int i=0;i<0x100;i++){
		TestAddr[i]=0;
		TestSize[i]=0;
	}
	for(int i=0;i<2000000;i++){
		TmpInt1=rand()&0xFF;
		if(0==TestAddr[TmpInt1]){
			TestSize[TmpInt1]=((rand()&0xFFF)+1)*nsEnv::SysPageSize;
			TestAddr[TmpInt1]=nsBasic::GetPage(TestSize[TmpInt1]);
			if(TestAddr[TmpInt1]>0){
				((unsigned char *)TestAddr[TmpInt1])[TestSize[TmpInt1]-1]=1;
			}
			else{
				TestSize[TmpInt1]=0;
			}
		}
		else{
			nsBasic::unGetPage(TestAddr[TmpInt1],TestSize[TmpInt1]);
			TestAddr[TmpInt1]=0;
			TestSize[TmpInt1]=0;
		}
		if(i%10000==0){
			printf("%8XH %8XH|%8XH %8XH LastErr:%8XH\n",nsBasic::AllocedPageCnt,nsBasic::AllocedPageMem,nsBasic::EmptyPageCnt,nsBasic::EmptyPageMem,GetLastError());
		}
	}
	printf("%8XH %8XH|%8XH %8XH LastErr:%8XH\n",nsBasic::AllocedPageCnt,nsBasic::AllocedPageMem,nsBasic::EmptyPageCnt,nsBasic::EmptyPageMem,GetLastError());
	for(int i=0;i<0x100;i++){
		if(TestAddr[i]>0)nsBasic::unGetPage(TestAddr[i],TestSize[i]);
		TestAddr[i]=0;
		TestSize[i]=0;
	}
	printf("%8XH %8XH|%8XH %8XH LastErr:%8XH\n",nsBasic::AllocedPageCnt,nsBasic::AllocedPageMem,nsBasic::EmptyPageCnt,nsBasic::EmptyPageMem,GetLastError());
	/*nsBasic::FlushEmptyPageRing();*/
	printf("%8XH %8XH|%8XH %8XH LastErr:%8XH\n",nsBasic::AllocedPageCnt,nsBasic::AllocedPageMem,nsBasic::EmptyPageCnt,nsBasic::EmptyPageMem,GetLastError());
	printf("done in %d seconds.\n",(UBINT)time(NULL)-CurTime);
	_getch();
}

extern void Test_Allocator(){
	const UBINT MAX_NODE_COUNT = 0x1000; //must be power of 2
	const UBINT MAX_OP_COUNT = 0x100000; //must be power of 2
	UBINT ALLOC_CONST = nsMath::log2intsim(nsEnv::SysPageSize);

	UBINT NodeSize[MAX_NODE_COUNT];
	void *NodeAddr[MAX_NODE_COUNT];
	UBINT RandSeed;
	UINT8b StartTime, EndTime;
	UBINT *OpQueue = (UBINT *)nsBasic::GetPage(MAX_OP_COUNT * sizeof(UBINT));
	UBINT *OpSize = (UBINT *)nsBasic::GetPage(MAX_OP_COUNT * sizeof(UBINT));
	UBINT *OpAddr = (UBINT *)nsBasic::GetPage(MAX_OP_COUNT * sizeof(UBINT));
	nsBasic::MemHeap MyHeap;
	MyHeap.Root = nullptr;

	memset(NodeSize, 0, MAX_NODE_COUNT * sizeof(UBINT));
	RandSeed = 0; //a constant ,for the convenience of error reproduction.
	for (UBINT i = 0; i<MAX_OP_COUNT; i++){
		nsMath::rand_LCG(&RandSeed);
		OpAddr[i] = RandSeed&(MAX_NODE_COUNT - 1);
		if (0 == NodeSize[OpAddr[i]]){
			OpQueue[i] = 0;
			nsMath::rand_LCG(&RandSeed);
			OpSize[i] = RandSeed % (ALLOC_CONST)+3;
			OpSize[i] = 1 << OpSize[i];
			nsMath::rand_LCG(&RandSeed);
			OpSize[i] = RandSeed&(OpSize[i] - 1);
			NodeSize[OpAddr[i]] = OpSize[i];
		}
		else{
			OpQueue[i] = 1;
			OpSize[i] = NodeSize[OpAddr[i]];
			NodeSize[OpAddr[i]] = 0;
		}
	}

	printf("Allocated page memory:0x%X bytes\n", nsBasic::AllocedPageMem);
	printf("Press any key to start testing.\n");
	_getch();
	printf("%d allocate/free with the custom allocator...", MAX_OP_COUNT);
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (UBINT i = 0; i<MAX_OP_COUNT; i++){
		if (i == 0xFF9ED){
			i = 0xFF9ED;
		}
		if (0 == OpQueue[i]){
			NodeAddr[OpAddr[i]] = MyHeap.Alloc(OpSize[i]);
			if (nullptr != NodeAddr[OpAddr[i]])*(unsigned char *)NodeAddr[OpAddr[i]] = '0';
			else if (0 != OpSize[i]){
				MyHeap.Clear();
				printf("failed.\nPress any key to terminate testing.");
				_getch();
				return;
			}
		}
		else{
			MyHeap.Free(NodeAddr[OpAddr[i]]);
			NodeAddr[OpAddr[i]] = nullptr;
		}
	}
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\nAllocated memory/Occupied memory:0x%X/0x%X bytes\nAllocated page memory:0x%X bytes\nHeap cleaning...\n", EndTime - StartTime, MyHeap.AllocedBytes(), MyHeap.OccupiedBytes(), nsBasic::AllocedPageMem);
	MyHeap.Clear();
	printf("Allocated page memory:0x%X bytes\n", nsBasic::AllocedPageMem);

	memset(NodeAddr, 0, MAX_NODE_COUNT * sizeof(void *));
	printf("%d allocate/free with malloc...", MAX_OP_COUNT);
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (UBINT i = 0; i < MAX_OP_COUNT; i++){
		if (0 == OpQueue[i]){
			NodeAddr[OpAddr[i]] = malloc(OpSize[i]);
			if (nullptr != NodeAddr[OpAddr[i]] && OpSize[i]>0)*(unsigned char *)NodeAddr[OpAddr[i]] = '0';
			else if (0 != OpSize[i]){
				for (UBINT j = 0; j < MAX_NODE_COUNT; j++){
					if (NodeAddr[j] != nullptr)free(NodeAddr[j]);
				}
				printf("failed.\nPress any key to terminate testing.");
				_getch();
				return;
			}
		}
		else{
			free(NodeAddr[OpAddr[i]]);
			NodeAddr[OpAddr[i]] = nullptr;
		}
	}
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %ld ticks.\n", EndTime - StartTime);
	_getch();
	for (UBINT i = 0; i < MAX_NODE_COUNT; i++){
		if (NodeAddr[i] != nullptr)free(NodeAddr[i]);
	}
	nsBasic::unGetPage(OpQueue, MAX_OP_COUNT * sizeof(UBINT));
	nsBasic::unGetPage(OpSize, MAX_OP_COUNT * sizeof(UBINT));
	nsBasic::unGetPage(OpAddr, MAX_OP_COUNT * sizeof(UBINT));
}
void *TestThreadMain_MsgQueue(void *lpArg){
	UBINT CurTime = (UBINT)time(NULL);
	srand(CurTime);

	UBINT TmpInt, BytesWritten = 0;
	unsigned char *Buffer = (unsigned char *)nsBasic::GetPage(nsEnv::SysPageSize);
	for (UBINT i = 0; i < nsEnv::SysPageSize; i++)Buffer[i] = (unsigned char)*(UBINT *)lpArg;
	while (BytesWritten<nsEnv::SysPageSize){
		TmpInt = rand() & 3;
		if (0==TmpInt && BytesWritten + 2 * sizeof(UBINT) <= nsEnv::SysPageSize){
			while (1 == nsBasic::MsgQueue_Write((nsBasic::MsgManager *)(((UBINT *)lpArg)[1]), 0));
			BytesWritten += sizeof(UBINT);
		}
		else{
			TmpInt = rand() % 0x40;//(nsEnv::SysPageSize - sizeof(UBINT));
			if (BytesWritten + TmpInt + 2*sizeof(UBINT)>nsEnv::SysPageSize)TmpInt = nsEnv::SysPageSize - BytesWritten - sizeof(UBINT);
			*(UBINT *)Buffer = TmpInt;
			while (1 == nsBasic::MsgQueue_BulkWrite((nsBasic::MsgManager *)(((UBINT *)lpArg)[1]), Buffer, TmpInt + sizeof(UBINT)));
			BytesWritten += TmpInt + sizeof(UBINT);
		}
	}
	nsBasic::unGetPage(Buffer, nsEnv::SysPageSize);
	return NULL;
}
extern void Test_MsgQueue(){
	UBINT TestThread[0x10], ThreadData[0x20], TmpInt, BytesRead = 0;
	unsigned char *Buffer = (unsigned char *)nsBasic::GetPage(nsEnv::SysPageSize);

	nsBasic::CreateThreadExtObj();
	for (int i = 0; i<0x10; i++){
		ThreadData[i * 2] = i;
		ThreadData[i * 2 + 1] = (UBINT)&(nsBasic::GetThreadExtInfo()->MsgManager);
		ThreadData[i * 2 + 2] = 0;
		if (i < 10)ThreadData[i * 2] += '0'; else ThreadData[i * 2] += 'A' - 10;
	}
	printf("Press any key to start testing.\n");
	_getch();
	UBINT CurTime = (UBINT)time(NULL);

	for (int i = 0; i < 0x10; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_MsgQueue, &ThreadData[i * 2]);
	while (BytesRead < 0x10 * nsEnv::SysPageSize){
		if (0 == nsBasic::MsgQueue_Read(&(nsBasic::GetThreadExtInfo()->MsgManager), &TmpInt)){
			for (UBINT i = 0; i<sizeof(UBINT); i++){
				putchar('_');
				BytesRead++;
				if (0 == (BytesRead & 0x3F))putchar('\n');
			}
			if (TmpInt>0){
				nsBasic::MsgQueue_BulkRead(&(nsBasic::GetThreadExtInfo()->MsgManager), Buffer, TmpInt);
				for (UBINT i = 0; i < TmpInt; i++){
					putchar(Buffer[i]);
					BytesRead++;
					if (0 == (BytesRead & 0x3F))putchar('\n');
				}
			}
		}
	}

	printf("\ndone in %d seconds.\n", (UBINT)time(NULL) - CurTime);
	nsBasic::unGetPage(Buffer, nsEnv::SysPageSize);
	_getch();
}
extern void Test_MemFile(){
	UBINT CurTime,TmpValue;
	nsFile::MemFile<UBINT> MyFile;

	printf("Press any key to start testing.\n");
	_getch();
	CurTime=(UBINT)time(NULL);
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)1, 0);
	printf("%d blocks stored in %d page(s).\n",MyFile.BlockCount(),MyFile.PageCount());
	TmpValue=1;
	for (UBINT i = 0; i < nsEnv::SysPageSize / sizeof(UBINT); i++)SendMsg(&MyFile, MSG_EDITOR_INSERT, &TmpValue);
	printf("%d blocks inserted at index 0 with value %d.\n",nsEnv::SysPageSize/sizeof(UBINT),TmpValue);
	printf("%d blocks stored in %d page(s).\n", MyFile.BlockCount(), MyFile.PageCount());
	TmpValue=2;
	SendMsg(&MyFile, MSG_FILE_SETPTR, -(INT8b)(nsEnv::SysPageSize / sizeof(UBINT)), 1);
	for (UBINT i = 0; i<nsEnv::SysPageSize / sizeof(UBINT); i++)SendMsg(&MyFile, MSG_EDITOR_INSERT, &TmpValue);
	printf("%d blocks inserted at index 0 with value %d.\n",nsEnv::SysPageSize/sizeof(UBINT),TmpValue);
	printf("%d blocks stored in %d page(s).\n", MyFile.BlockCount(), MyFile.PageCount());
	TmpValue=3;
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)3 - (INT8b)2 * nsEnv::SysPageSize / sizeof(UBINT), 2);
	for (UBINT i = 0; i<3; i++)SendMsg(&MyFile, MSG_EDITOR_INSERT, &TmpValue);
	printf("3 blocks inserted at index 3 with value %d.\n",TmpValue);
	printf("%d blocks stored in %d page(s).\n", MyFile.BlockCount(), MyFile.PageCount());
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)1, 0);
	printf("Read from index 1:");
	for(UBINT i=0;i<7;i++){
		SendMsg(&MyFile, MSG_STREAM_READ, &TmpValue);
		printf("%d ",TmpValue);
	}
	printf("\n");
	TmpValue=4;
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)(nsEnv::SysPageSize / sizeof(UBINT)+6), 0);
	for (UBINT i = 0; i<3; i++)SendMsg(&MyFile, MSG_EDITOR_INSERT, &TmpValue);
	printf("3 blocks inserted at index %d with value %d.\n",nsEnv::SysPageSize/sizeof(UBINT)+6,TmpValue);
	printf("%d blocks stored in %d page(s).\n", MyFile.BlockCount(), MyFile.PageCount());
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)-5, 1);
	printf("Read from index %d:",nsEnv::SysPageSize/sizeof(UBINT)+4);
	for(UBINT i=0;i<7;i++){
		SendMsg(&MyFile, MSG_STREAM_READ, &TmpValue);
		printf("%d ",TmpValue);
	}
	printf("\n");
	TmpValue=5;
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)-11, 1);
	for (UBINT i = 0; i<3; i++)SendMsg(&MyFile, MSG_EDITOR_INSERT, &TmpValue);
	printf("3 blocks inserted at index %d with value %d.\n",nsEnv::SysPageSize/sizeof(UBINT),TmpValue);
	printf("%d blocks stored in %d page(s).\n", MyFile.BlockCount(), MyFile.PageCount());
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)-6, 1);
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)4, 1);
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)0, 1);
	printf("Read from index %d:",nsEnv::SysPageSize/sizeof(UBINT)+1);
	for(UBINT i=0;i<7;i++){
		SendMsg(&MyFile, MSG_STREAM_READ, &TmpValue);
		printf("%d ",TmpValue);
	}
	printf("\n");
	
	INT8b Pointer;
	SendMsg(&MyFile, MSG_FILE_GETPTR, &Pointer);
	printf("Currently the file pointer is %d.\n", (int)Pointer);
	printf("done in %d seconds.\n",(UBINT)time(NULL)-CurTime);
	_getch();
}
extern void Test_LinearTable(){
	nsDSAdv::Stack<unsigned char> MyStack;
	unsigned char TmpChar,*TmpArr;

	TmpArr=(unsigned char *)nsBasic::GetPage(2*nsEnv::SysPageSize);
	TmpArr[nsEnv::SysPageSize+1]='\0';
	printf("Press any key to start test 1.\n");
	_getch();
	printf("0x%X bytes are pushed into a stack.\n",2*nsEnv::SysPageSize+1);
	for(UBINT i=0;i<2*nsEnv::SysPageSize+1;i++){
		TmpChar = TestData_Str_Pattern(i);
		MyStack.Push(&TmpChar);
	}
	printf("0x%X bytes are popped out:\n",nsEnv::SysPageSize);
	for(UBINT i=0;i<nsEnv::SysPageSize;i++){
		MyStack.Pop(&TmpChar);
		printf("%c",TmpChar);
	}
	printf("\nNow the height of the stack is 0x%X.\n", MyStack.Length());
	printf("Press any key to continue.\n");
	_getch();
	printf("Here is the remaining content of this stack:\n");
	MyStack.ConvertToArray(TmpArr);
	printf((const char *)TmpArr);

	nsBasic::BlockPageRing MyTable;
	printf("\nPress any key to start test 2.\n");
	_getch();
	printf("0x%X bytes are pushed into a queue.\n",2*nsEnv::SysPageSize+1);
	nsDSAdv::CreateBlockQueue(&MyTable,sizeof(unsigned char));
	for(UBINT i=0;i<2*nsEnv::SysPageSize+1;i++){
		TmpChar = TestData_Str_Pattern(i);
		nsDSAdv::BlockQueue_Push(&MyTable,&TmpChar);
	}
	printf("0x%X bytes are popped out:\n",nsEnv::SysPageSize);
	for(UBINT i=0;i<nsEnv::SysPageSize;i++){
		nsDSAdv::BlockQueue_Pop(&MyTable,&TmpChar);
		printf("%c",TmpChar);
	}
	printf("\nNow the length of the queue is 0x%X.\n",nsDSAdv::BlockQueue_GetLength(&MyTable));
	printf("Press any key to continue.\n");
	_getch();
	printf("Here is the remaining content of this queue:\n");
	nsDSAdv::BlockQueue_ConvertToArray(&MyTable,TmpArr);
	printf((const char *)TmpArr);
	nsBasic::DestroyObject((nsBasic::ObjGeneral *)&MyTable);
	printf("\nPress any key to start test 3.\n");
	_getch();
	nsDSAdv::CreateBlockDEQueue(&MyTable,sizeof(unsigned char));
	printf("0x%X bytes are pushed into a double-ended queue at the tail.\n",nsEnv::SysPageSize+1);
	for(UBINT i=0;i<nsEnv::SysPageSize+1;i++){
		TmpChar = TestData_Str_Pattern(i);
		nsDSAdv::BlockDEQueue_Push_Tail(&MyTable,&TmpChar);
	}
	printf("0x%X bytes are pushed into a double-ended queue at the head.\n",nsEnv::SysPageSize);
	for(UBINT i=nsEnv::SysPageSize+1;i<2*nsEnv::SysPageSize+1;i++){
		TmpChar = TestData_Str_Pattern(i);
		nsDSAdv::BlockDEQueue_Push_Head(&MyTable,&TmpChar);
	}
	printf("0x%X bytes are popped out at the tail:\n",nsEnv::SysPageSize/2);
	for(UBINT i=0;i<nsEnv::SysPageSize/2;i++){
		nsDSAdv::BlockDEQueue_Pop_Tail(&MyTable,&TmpChar);
		printf("%c",TmpChar);
	}
	printf("\n0x%X bytes are popped out at the head:\n",nsEnv::SysPageSize/2);
	for(UBINT i=0;i<nsEnv::SysPageSize/2;i++){
		nsDSAdv::BlockDEQueue_Pop_Head(&MyTable,&TmpChar);
		printf("%c",TmpChar);
	}
	printf("\nNow the length of the queue is 0x%X.\n",nsDSAdv::BlockQueue_GetLength(&MyTable));
	printf("Press any key to continue.\n");
	_getch();
	printf("Here is the remaining content of this queue:\n");
	nsDSAdv::BlockDEQueue_ConvertToArray(&MyTable,TmpArr);
	printf((const char *)TmpArr);
	nsBasic::DestroyObject((nsBasic::ObjGeneral *)&MyTable);
	nsBasic::unGetPage(TmpArr,2*nsEnv::SysPageSize);
	printf("\ndone.\n");
	_getch();
}
extern void Test_BulkReadWrite(){
	UBINT CurTime,TmpValue;
	unsigned char TestChar,TestData[0x3000];
	nsFile::WriteFileStream WFS(L"Test");
	nsFile::WriteStreamBuffer WSB;
	nsFile::ReadStreamBuffer RSB;
	nsFile::MemSection WBS(TestData, 0x3000);
	nsFile::MemSection RBS(TestData, 0x3000);
	nsFile::MemFile<unsigned char> MyFile;

	printf("Press any key to start testing.\n");
	_getch();
	CurTime=(UBINT)time(NULL);
	nsFile::CreateWriteStreamBuffer(&WSB, (nsBasic::ObjGeneral *)&WFS);
	for (int i = 0; i<0x3000; i++)TestData[i] = TestData_Str_Pattern(i);
	printf("Write 2048 bytes to file Test.%d Bytes written.\n", TmpValue = SendMsg(&WSB, MSG_STREAM_WRITEBULK, TestData, 0x800));
	_getch();
	printf("Write 8192 bytes to file Test.%d Bytes written.\n", TmpValue = SendMsg(&WSB, MSG_STREAM_WRITEBULK, TestData + 0x800, 0x2000));
	_getch();
	printf("Write 2048 bytes to file Test.%d Bytes written.\n", TmpValue = SendMsg(&WSB, MSG_STREAM_WRITEBULK, TestData + 0x2800, 0x800));
	_getch();
	SendMsg(&WSB, MSG_DESTROY);
	SendMsg(&WFS, MSG_DESTROY);
	printf("File Test created.\n");
	memset(TestData, 0, 0x3000);

	nsFile::ReadFileStream RFS(L"Test");
	nsFile::CreateReadStreamBuffer(&RSB, (nsBasic::ObjGeneral *)&RFS);
	printf("Request 2048 bytes from file Test.%d Bytes received.\n", TmpValue = SendMsg(&RSB, MSG_STREAM_READBULK, TestData, 0x800));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	printf("Request 8192 bytes from file Test.%d Bytes received.\n", TmpValue = SendMsg(&RSB, MSG_STREAM_READBULK, TestData, 0x2000));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	printf("Request 2050 bytes from file Test.%d Bytes received.\n", TmpValue = SendMsg(&RSB, MSG_STREAM_READBULK, TestData, 0x802));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	SendMsg(&RSB, MSG_DESTROY);
	SendMsg(&RFS, MSG_DESTROY);
	printf("Write 2048 bytes to byte stream.%d Bytes written.\n", TmpValue = SendMsg(&WBS, MSG_STREAM_WRITEBULK, TestData + 0x800, 0x800));
	printf("Request 2048 bytes from byte stream.%d Bytes received.\n", TmpValue = SendMsg(&RBS, MSG_STREAM_READBULK, TestData + 0x800, 0x800));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	for (int i = 0; i<0x3000; i++)TestData[i] = TestData_Str_Pattern(i);
	printf("Write 2048 bytes to memory file.%d Bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData, 0x800));
	printf("Write 2047 bytes to memory file.%d Bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x800, 0x7FF));
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)1, 0);
	printf("Write 6143 bytes to memory file.%d Bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 1, 0x17FF));
	printf("Write 2047 bytes to memory file.%d Bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x1800, 0x7FF));
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)1, 0);
	printf("Write 6143 bytes to memory file.%d Bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 1, 0x17FF));
	printf("Write 1 bytes to memory file.%d Bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x1800, 1));
	printf("Write 2047 bytes to memory file.%d Bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x1801, 0x7FF));
	printf("Write 4096 bytes to memory file.%d Bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x2000, 0x1000));
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)0, 0);
	printf("Request 2048 bytes from memory file.%d Bytes received.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_READBULK, TestData, 0x800));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	printf("Request 8192 bytes from memory file.%d Bytes received.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_READBULK, TestData, 0x2000));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	printf("Request 2050 bytes from memory file.%d Bytes received.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_READBULK, TestData, 0x80B));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	printf("done in %d seconds.\n",(UBINT)time(NULL)-CurTime);
	_getch();
}
extern void Test_Lexer(){
	char TestData[20]=".1\0-.1\0001.\0-1.\0003.14\0";
	nsFile::MemSection RBS(TestData, 20);
	nsFile::DecoderStream Decoder((nsBasic::ObjGeneral *)&RBS, CODING_TEXT_UTF8);
	nsFile::MemSection WBS(TestData, 20);
	
	nsStrAdv::Lexer MyLexer((nsBasic::ObjGeneral *)&Decoder);
	double Result=0;
	UBINT TmpChar;

	printf("Press any key to start testing.\n");
	_getch();
	MyLexer.LoadNextChar();
	MyLexer.ReadSimpleFloat(&Result);
	printf("%s -> %lf\n",TestData,Result);
	MyLexer.LoadNextChar();
	MyLexer.ReadSimpleFloat(&Result);
	printf("%s -> %lf\n",&TestData[3],Result);
	MyLexer.LoadNextChar();
	MyLexer.ReadSimpleFloat(&Result);
	printf("%s -> %lf\n",&TestData[7],Result);
	MyLexer.LoadNextChar();
	MyLexer.ReadSimpleFloat(&Result);
	printf("%s -> %lf\n",&TestData[10],Result);
	MyLexer.LoadNextChar();
	MyLexer.ReadSimpleFloat(&Result);
	printf("%s -> %lf\n",&TestData[14],Result);
	_getch();

	nsFile::CoderStream Coder((nsBasic::ObjGeneral *)&WBS, CODING_TEXT_UTF8);

	SendMsg(&WBS, MSG_FILE_SETPTR, (INT8b)0, 0);
	nsStrAdv::Stream_WriteSimpleUInt((nsBasic::ObjGeneral *)&Coder,0);
	TmpChar = '\0'; SendMsg(&Coder, MSG_STREAM_WRITE, &TmpChar);
	printf("\n%u -> %s\n",0,TestData);
	SendMsg(&WBS, MSG_FILE_SETPTR, (INT8b)0, 0);
	nsStrAdv::Stream_WriteSimpleUInt((nsBasic::ObjGeneral *)&Coder, 0xFFFFFFFF);
	TmpChar = '\0'; SendMsg(&Coder, MSG_STREAM_WRITE, &TmpChar);
	printf("%u -> %s\n",0xFFFFFFFF,TestData);
	SendMsg(&WBS, MSG_FILE_SETPTR, (INT8b)0, 0);
	nsStrAdv::Stream_WriteSimpleFloat((nsBasic::ObjGeneral *)&Coder, -0.0);
	TmpChar = '\0'; SendMsg(&Coder, MSG_STREAM_WRITE, &TmpChar);
	printf("%lf -> %s\n",0.0,TestData);
	SendMsg(&WBS, MSG_FILE_SETPTR, (INT8b)0, 0);
	nsStrAdv::Stream_WriteSimpleFloat((nsBasic::ObjGeneral *)&Coder, 5.0);
	TmpChar = '\0'; SendMsg(&Coder, MSG_STREAM_WRITE, &TmpChar);
	printf("%lf -> %s\n",5.0,TestData);
	SendMsg(&WBS, MSG_FILE_SETPTR, (INT8b)0, 0);
	nsStrAdv::Stream_WriteSimpleFloat((nsBasic::ObjGeneral *)&Coder, 0.0123456789);
	TmpChar = '\0'; SendMsg(&Coder, MSG_STREAM_WRITE, &TmpChar);
	printf("%lf -> %s\n",0.0123456789,TestData);
	SendMsg(&WBS, MSG_FILE_SETPTR, (INT8b)0, 0);
	nsStrAdv::Stream_WriteSimpleFloat((nsBasic::ObjGeneral *)&Coder, -6227020800.0);
	TmpChar = '\0'; SendMsg(&Coder, MSG_STREAM_WRITE, &TmpChar);
	printf("%lf -> %s\n", -6227020800.0, TestData, 20);
	_getch();
}

extern BINT TmpCompFunc(void *A,void *B){
	return *((BINT *)A)-*((BINT *)B);
}
extern void Test_PtrSet(){
	nsDSAdv::PtrSet MySet(TmpCompFunc);
	
	BINT TestArr[0x20000],TmpInt,*EmptyPtr=TestArr,*TmpPtr1,*TmpPtr2;
	UBINT InsertIndex[8]={6,4,3,0,2,5,1,7},DeleteIndex[8]={3,5,7,6,2,0,1,4},CurTime,PfmResult[7];

	printf("Press any key to start testing.\n");
	_getch();
	for(UBINT i=0;i<8;i++)TestArr[i]=i;
	printf("Inserting");
	for(UBINT i=0;i<8;i++){
		printf(" %d",InsertIndex[i]);
		if(NULL==MySet.Insert(TestArr+InsertIndex[i])){printf("...failed.");break;}
	}
	printf("\nQuerying");
	for(UBINT i=0;i<8;i++){
		printf(" %d",i);
		if(NULL==MySet.Query(TestArr+i)){printf("...failed.");break;}
	}
	printf("\nDeleting");
	for(UBINT i=0;i<8;i++){
		printf(" %d",DeleteIndex[i]);
		if (NULL == MySet.Delete(TestArr + DeleteIndex[i])){ printf("...failed."); break; }
	}
	printf("\nInserting");
	for(UBINT i=0;i<8;i++){
		printf(" %d",7-InsertIndex[i]);
		if (NULL == MySet.Insert(TestArr + 7 - InsertIndex[i])){ printf("...failed."); break; }
	}
	printf("\nQuerying");
	for(UBINT i=0;i<8;i++){
		printf(" %d",i);
		if (NULL == MySet.Query(TestArr + i)){ printf("...failed."); break; }
	}
	printf("\nDeleting");
	for(UBINT i=0;i<8;i++){
		printf(" %d",7-DeleteIndex[i]);
		if (NULL == MySet.Delete(TestArr + 7 - DeleteIndex[i])){ printf("...failed."); break; }
	}
	printf("\nFinished.");

	for(UBINT i=0;i<0x1FFFF;i++)TestArr[i]=(BINT)(TestArr+i+1);
	TestArr[0x1FFFF]=(BINT)NULL;
	for(UBINT i=0;i<7;i++)PfmResult[i]=0;

	printf("Press any key to start performance testing.\n");
	_getch();
	CurTime=(UBINT)time(NULL);
	srand(CurTime);

	for(UBINT i=0;i<0x1000000;i++){
		switch(rand()%3){
			case 0:
				if(NULL!=EmptyPtr){
					TmpPtr1=(BINT *)*EmptyPtr;
					*EmptyPtr=rand()%0x100000;
					TmpPtr2 = (BINT *)MySet.Insert(EmptyPtr);
					if(NULL==TmpPtr2 || EmptyPtr==TmpPtr2)PfmResult[1]++;else PfmResult[0]++;
					EmptyPtr=TmpPtr1;
					break;
				}
			case 1:
				TmpInt=rand()%0x100000;
				if (NULL != MySet.Query(&TmpInt))PfmResult[3]++; else PfmResult[4]++;
				break;
			case 2:
				TmpInt=rand()%0x100000;
				TmpPtr1 = (BINT *)MySet.Delete(&TmpInt);
				if(NULL!=TmpPtr1){
					*TmpPtr1=(BINT)EmptyPtr;
					EmptyPtr=TmpPtr1;
					PfmResult[5]++;

				}else PfmResult[6]++;
				break;
		}
		TmpInt=0x94C;
		MySet.Query(&TmpInt);
	}
	printf("done in %d seconds.\n",(UBINT)time(NULL)-CurTime);
	printf("Insert hit:0x%X\n",PfmResult[0]);
	printf("Insert fail:0x%X\n",PfmResult[1]);
	printf("Insert miss:0x%X\n",PfmResult[2]);
	printf("Query hit:0x%X\n",PfmResult[3]);
	printf("Query miss:0x%X\n",PfmResult[4]);
	printf("Delete hit:0x%X\n",PfmResult[5]);
	printf("Delete miss:0x%X\n",PfmResult[6]);
	_getch();
}
extern BINT TmpCompFunc2(void *A,void *B){
	return *((char *)A)-*((char *)B);
}
extern void Test_Sort(){
	char TestStr1[53]="QqWwEeRrTtYyUuIiOoPpAaSsDdFfGgHhJjKkLlZzXxCcVvBbNnMm";
	printf("Before:%s\n",TestStr1);
	nsDSAdv::Sort_C(TestStr1,52,1,TmpCompFunc2);
	printf("After :%s\n",TestStr1);
	char TestStr2[53]="zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";
	printf("Before:%s\n",TestStr2);
	nsDSAdv::Sort_C(TestStr2,52,1,TmpCompFunc2);
	printf("After :%s",TestStr2);
	_getch();
}
