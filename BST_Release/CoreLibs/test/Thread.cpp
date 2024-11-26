#include "lGeneral.hpp"
#include <cstdio>
#include <ctime>

#include <stdio.h>
#if defined LIBENV_OS_LINUX
#include <curses.h>
#else
#include <conio.h>
#endif

//Linux support is not implemented.

const UBINT ThreadCount = MAXIMUM_WAIT_OBJECTS;
const UBINT LockCount = 0x8;
const UBINT LockOpsCount = 0x40000;

void *TestThreadMain_Lock_Native_Time(void *lpArg){
	UBINT RandSeed = nsBasic::GetCurThreadStub()->hThread;
	UBINT tmp;
	for (UBINT i = 0; i<LockOpsCount; i++){
		tmp = nsMath::rand_LCG(&RandSeed) % LockCount;
		EnterCriticalSection((LPCRITICAL_SECTION)lpArg + tmp);
		LeaveCriticalSection((LPCRITICAL_SECTION)lpArg + tmp);
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_Lock_Native_Time(){
	UBINT TestThread[ThreadCount];
	CRITICAL_SECTION LockArr_Native[LockCount];
	UINT8b StartTime, EndTime;

	printf("Native lock test -- %d threads on %d locks, %d ops in total:\n", ThreadCount, LockCount, ThreadCount*LockOpsCount);
	for (int i = 0; i < LockCount; i++)InitializeCriticalSection(&LockArr_Native[i]);
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i<ThreadCount; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_Lock_Native_Time, (void *)LockArr_Native);
	WaitForMultipleObjects(ThreadCount, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}
void *TestThreadMain_Lock_Time(void *lpArg){
	UBINT RandSeed = nsBasic::GetCurThreadStub()->hThread;
	UBINT tmp;
	for (UBINT i = 0; i<LockOpsCount; i++){
		tmp = nsMath::rand_LCG(&RandSeed) % LockCount;
		nsBasic::EnterLock((UBINT *)lpArg + tmp);
		nsBasic::LeaveLock((UBINT *)lpArg + tmp);
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_Lock_Time(){
	UBINT TestThread[ThreadCount];
	UBINT LockArr[LockCount];
	UINT8b StartTime, EndTime;

	printf("Normal lock test -- %d threads on %d locks, %d ops in total:\n", ThreadCount, LockCount, ThreadCount*LockOpsCount);
	for (int i = 0; i < LockCount; i++)LockArr[i] = (UBINT)nullptr;
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i<ThreadCount; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_Lock_Time, (void *)LockArr);
	WaitForMultipleObjects(ThreadCount, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}
void *TestThreadMain_Mutex_Time(void *lpArg){
	UBINT RandSeed = nsBasic::GetCurThreadStub()->hThread;
	UBINT tmp;
	for (UBINT i = 0; i<LockOpsCount; i++){
		tmp = nsMath::rand_LCG(&RandSeed) % LockCount;
		((nsBasic::Mutex *)lpArg + tmp)->Enter();
		((nsBasic::Mutex *)lpArg + tmp)->Leave();
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_Mutex_Time(){
	UBINT TestThread[ThreadCount];
	nsBasic::Mutex LockArr_Spin[LockCount];
	UINT8b StartTime, EndTime;

	printf("Mutex test -- %d threads on %d locks, %d ops in total:\n", ThreadCount, LockCount, ThreadCount*LockOpsCount);
	for (int i = 0; i < LockCount; i++)LockArr_Spin[i].Initialize();
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i<ThreadCount; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_Mutex_Time, (void *)LockArr_Spin);
	WaitForMultipleObjects(ThreadCount, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}
void *TestThreadMain_RWLock_Native_Time(void *lpArg){
	UBINT RandSeed = nsBasic::GetCurThreadStub()->hThread;
	UBINT tmp;
	for (UBINT i = 0; i<LockOpsCount; i++){
		tmp = nsMath::rand_LCG(&RandSeed);
		if ((tmp & 0x7) > 2){
			tmp = nsMath::rand_LCG(&RandSeed) % LockCount;
			AcquireSRWLockShared((SRWLOCK *)lpArg + tmp);
			ReleaseSRWLockShared((SRWLOCK *)lpArg + tmp);
		}
		else{
			tmp = nsMath::rand_LCG(&RandSeed) % LockCount;
			AcquireSRWLockExclusive((SRWLOCK *)lpArg + tmp);
			ReleaseSRWLockExclusive((SRWLOCK *)lpArg + tmp);
		}
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_RWLock_Native_Time(){
	UBINT TestThread[ThreadCount];
	SRWLOCK LockArr_RW_Native[LockCount];
	UINT8b StartTime, EndTime;

	printf("Native Read/Write lock test -- %d threads on %d locks, %d ops in total:\n", ThreadCount, LockCount, ThreadCount*LockOpsCount);
	for (int i = 0; i < LockCount; i++)InitializeSRWLock(&LockArr_RW_Native[i]);
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i<ThreadCount; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_RWLock_Native_Time, (void *)LockArr_RW_Native);
	WaitForMultipleObjects(ThreadCount, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}
void *TestThreadMain_RWLock_Time(void *lpArg){
	UBINT RandSeed = nsBasic::GetCurThreadStub()->hThread;
	UBINT tmp;
	for (UBINT i = 0; i<LockOpsCount; i++){
		tmp = nsMath::rand_LCG(&RandSeed);
		if ((tmp & 0x7) > 2){
			tmp = nsMath::rand_LCG(&RandSeed) % LockCount;
			((nsBasic::RWLock *)lpArg + tmp)->Enter_Read();
			((nsBasic::RWLock *)lpArg + tmp)->Leave_Read();
		}
		else{
			tmp = nsMath::rand_LCG(&RandSeed) % LockCount;
			((nsBasic::RWLock *)lpArg + tmp)->Enter_Write();
			((nsBasic::RWLock *)lpArg + tmp)->Leave_Write();
		}
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_RWLock_Time(){
	UBINT TestThread[ThreadCount];
	nsBasic::RWLock LockArr_RW[LockCount];
	UINT8b StartTime, EndTime;

	printf("Read/Write lock test -- %d threads on %d locks, %d ops in total:\n", ThreadCount, LockCount, ThreadCount*LockOpsCount);
	for (int i = 0; i < LockCount; i++)LockArr_RW[i].Initialize();
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i<ThreadCount; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_RWLock_Time, (void *)LockArr_RW);
	WaitForMultipleObjects(ThreadCount, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}

void *TestThreadMain_Semaphore_Time(void *lpArg){
	UBINT RandSeed = nsBasic::GetCurThreadStub()->hThread;
	UBINT tmp;
	for (UBINT i = 0; i<LockOpsCount; i++){
		tmp = nsMath::rand_LCG(&RandSeed) % LockCount;
		((nsBasic::Semaphore *)lpArg + tmp)->Enter();
		((nsBasic::Semaphore *)lpArg + tmp)->Leave();
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_Semaphore_Time(){
	UBINT TestThread[ThreadCount];
	nsBasic::Semaphore SemArr[LockCount];
	UINT8b StartTime, EndTime;

	printf("Semaphore test -- %d threads on %d locks, %d ops in total:\n", ThreadCount, LockCount, ThreadCount*LockOpsCount);
	for (int i = 0; i < LockCount; i++)SemArr[i].Initialize(2);
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i<ThreadCount; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_Semaphore_Time, (void *)SemArr);
	WaitForMultipleObjects(ThreadCount, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}

nsBasic::CyclicBarrier TestCyclicBarrier;
const UBINT CyclicBarrier_Size = 8;
void *TestThreadMain_CyclicBarrier_Time(void *lpArg){
	for (UBINT i = 0; i < LockOpsCount; i++)TestCyclicBarrier.Wait();
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_CyclicBarrier_Time(){
	UBINT TestThread[CyclicBarrier_Size];
	UINT8b StartTime, EndTime;

	printf("Cyclic barrier test -- %d threads, %d ops in total:\n", CyclicBarrier_Size, CyclicBarrier_Size*LockOpsCount);
	//A deadlock may happen when thread count is not equal to CyclicBarrier_Size.
	TestCyclicBarrier.Initialize(CyclicBarrier_Size);
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i < CyclicBarrier_Size; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_CyclicBarrier_Time, nullptr);
	WaitForMultipleObjects(CyclicBarrier_Size, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}

nsBasic::RWLock TestRWLock;
void *TestThreadMain_RWLock_Verify(void *lpArg){
	volatile UBINT *TestData = (UBINT *)lpArg;
	UBINT RandSeed = nsBasic::GetCurThreadStub()->hThread;
	UBINT LastToken = 0;
	UBINT tmp;
	for (UBINT i = 0; i<LockOpsCount; i++){
		tmp = nsMath::rand_LCG(&RandSeed);
		if ((tmp & 0x7) > 2){
			tmp = nsMath::rand_LCG(&RandSeed) % ThreadCount;
			TestRWLock.Enter_Read();

			UBINT CurToken = TestData[tmp];
			if (CurToken < LastToken){
				printf("Lock Read: Impossible token found:0x%X < 0x%X\n", CurToken, LastToken);
			}
			LastToken = CurToken;

			TestRWLock.Leave_Read();
		}
		else{
			tmp = nsMath::rand_LCG(&RandSeed) % ThreadCount;
			TestRWLock.Enter_Write();

			UBINT CurToken = TestData[tmp];
			if (CurToken < LastToken){
				printf("Lock Write:Impossible token found:0x%X < 0x%X\n", CurToken, LastToken);
			}
			CurToken++;
			LastToken = CurToken;
			for (UBINT i = 0; i < ThreadCount; i++)TestData[i] = CurToken;

			TestRWLock.Leave_Write();
		}
	}
	printf("Thread %8XH finished with the last token 0x%X.\n", nsBasic::GetCurThreadStub()->hThread, LastToken);
	return nullptr;
}
void Test_RWLock_Verify(){
	UBINT TestThread[ThreadCount];
	UBINT TestData[ThreadCount];
	UINT8b StartTime, EndTime;
		
	printf("Read/Write lock verification -- %d threads, %d ops in total:\n", ThreadCount, ThreadCount*LockOpsCount);
	TestRWLock.Initialize();
	for (UBINT i = 0; i < ThreadCount; i++)TestData[i] = 0;
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i<ThreadCount; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_RWLock_Verify, (void *)TestData);
	WaitForMultipleObjects(ThreadCount, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}

nsBasic::Semaphore TestSemaphore;
const UBINT Semaphore_InitVal = 8;
void *TestThreadMain_Semaphore_Verify(void *lpArg){
	volatile UBINT *TestData = (UBINT *)lpArg;
	for (UBINT i = 0; i<LockOpsCount; i++){
		TestSemaphore.Enter();
		UBINT TmpVal = atomic_add(TestData, 1);
		if (TmpVal>Semaphore_InitVal)printf("Error:0x%X thread is working on 0x%X resources.\n", TmpVal, Semaphore_InitVal);
		atomic_add(TestData, (UBINT)-1);
		TestSemaphore.Leave();
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_Semaphore_Verify(){
	UBINT TestThread[ThreadCount];
	UBINT TestData;
	UINT8b StartTime, EndTime;

	printf("Semaphore verification -- %d threads, %d resources, %d ops in total:\n", ThreadCount, Semaphore_InitVal, ThreadCount*LockOpsCount);
	TestSemaphore.Initialize(Semaphore_InitVal);
	TestData = 0;
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i<ThreadCount; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_Semaphore_Verify, (void *)&TestData);
	WaitForMultipleObjects(ThreadCount, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}

const UBINT CyclicBarrier_TestRound = 8;
void *TestThreadMain_CyclicBarrier_Example(void *lpArg){
	for (UBINT i = 0; i<CyclicBarrier_TestRound; i++){
		printf("Thread %8XH on round %d.\n", nsBasic::GetCurThreadStub()->hThread, i);
		TestCyclicBarrier.Wait();
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_CyclicBarrier_Example(){
	UBINT TestThread[CyclicBarrier_Size];
	UINT8b StartTime, EndTime;

	printf("Cyclic barrier example -- %d threads, %d ops in total:\n", CyclicBarrier_Size, CyclicBarrier_Size*CyclicBarrier_TestRound);
	TestCyclicBarrier.Initialize(CyclicBarrier_Size);
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i < CyclicBarrier_Size; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_CyclicBarrier_Example, nullptr);
	WaitForMultipleObjects(CyclicBarrier_Size, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}

nsBasic::Event MyEventPair[2];
const UBINT Event_Size = 8;
const UBINT Event_TestRound = 8;
void *TestThreadMain_Event_Example(void *lpArg){
	UBINT ThreadIndex = (UBINT)lpArg;
	for (UBINT i = 0; i<Event_TestRound; i++){
		printf("Thread %8XH on round %d.\n", nsBasic::GetCurThreadStub()->hThread, i);
		if (i % Event_Size == ThreadIndex){
			MyEventPair[1 - (i & 1)].Initialize();
			MyEventPair[i & 1].Wake();
		}
		else MyEventPair[(i & 1)].Wait();
		TestCyclicBarrier.Wait();
	}
	printf("Thread %8XH finished.\n", nsBasic::GetCurThreadStub()->hThread);
	return nullptr;
}
void Test_Event_Example(){
	UBINT TestThread[Event_Size];
	UINT8b StartTime, EndTime;

	printf("Event example -- %d threads, %d ops in total:\n", Event_Size, Event_Size*Event_TestRound);
	MyEventPair[0].Initialize();
	TestCyclicBarrier.Initialize(Event_Size);
	nsBasic::GetAccurateTimeCntr(&StartTime);
	for (int i = 0; i < Event_Size; i++)TestThread[i] = nsBasic::CreateNormalThread(TestThreadMain_Event_Example, (void *)i);
	WaitForMultipleObjects(Event_Size, (HANDLE *)TestThread, TRUE, INFINITE);
	nsBasic::GetAccurateTimeCntr(&EndTime);
	printf("done in %lld ticks.\n", EndTime - StartTime);
}

extern void Test_SyncIdioms(){
	printf("Press any key to start testing.\n");
	_getch();

	//Test_Lock_Native_Time();
	//Test_Lock_Time();
	//Test_Mutex_Time();
	//Test_RWLock_Native_Time();
	//Test_RWLock_Time();
	//Test_CyclicBarrier_Time();
	//Test_Semaphore_Time();
	//Test_RWLock_Verify();
	//Test_Semaphore_Verify();
	Test_Event_Example();
	_getch();
}