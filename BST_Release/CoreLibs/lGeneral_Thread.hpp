/* Description:Implementation of the basic thread structure and synchronization. DO NOT include this header directly.
 * Language:C++11
 * Author:***
 */

#ifndef LIB_GENERAL_THREAD
#define LIB_GENERAL_THREAD

#if defined LIBENV_OS_WIN
#include "process.h"

// keyed event functions, avaliable since Windows XP
#pragma comment(lib,"ntdll.lib") //ntdll.lib can be found in WDK or DDK.

extern "C" NTSTATUS NTAPI NtCreateKeyedEvent(OUT PHANDLE handle, IN ACCESS_MASK access, IN PVOID attr, IN ULONG flags);
// [flags] must be 0.
extern "C" NTSTATUS NTAPI NtWaitForKeyedEvent(IN HANDLE handle, IN PVOID key, IN BOOLEAN alertable, IN PLARGE_INTEGER mstimeout);
extern "C" NTSTATUS NTAPI NtReleaseKeyedEvent(IN HANDLE handle, IN PVOID key, IN BOOLEAN alertable, IN PLARGE_INTEGER mstimeout);

#if defined LIBENV_CPLR_VS
#if defined LIBENV_SYS_INTELX86
static inline UBINT atomic_add(volatile UBINT *lpDest, UBINT Value){ return InterlockedAdd((LONG *)lpDest, Value); }
static inline UBINT atomic_xadd(volatile UBINT *lpDest, UBINT Value){ return InterlockedExchangeAdd((LONG *)lpDest, Value); }
static inline UBINT atomic_and(volatile UBINT *lpDest, UBINT Value){ return InterlockedAnd((LONG *)lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_or(volatile UBINT *lpDest, UBINT Value){ return InterlockedOr((LONG *)lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_xor(volatile UBINT *lpDest, UBINT Value){ return InterlockedXor((LONG *)lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_xchg(volatile UBINT *lpDest, UBINT NewValue){ return InterlockedExchange((LONG *)lpDest, NewValue); }
static inline UBINT atomic_cmpxchg(volatile UBINT *lpDest, UBINT NewValue, UBINT OldValue){ return InterlockedCompareExchange((LONG *)lpDest, NewValue, OldValue); }
static inline bool atomic_bitset_xchg(volatile UBINT *lpDest, UBINT BitPos){ return 0 != InterlockedBitTestAndSet((LONG *)lpDest, BitPos); }
static inline bool atomic_bitclr_xchg(volatile UBINT *lpDest, UBINT BitPos){ return 0 != InterlockedBitTestAndReset((LONG *)lpDest, BitPos); }
#elif defined LIBENV_SYS_INTELX64
static inline UBINT atomic_add(volatile UBINT *lpDest, UBINT Value){ return InterlockedAdd64((LONGLONG *)lpDest, Value); }
static inline UBINT atomic_xadd(volatile UBINT *lpDest, UBINT Value){ return InterlockedExchangeAdd64((LONGLONG *)lpDest, Value); }
static inline UBINT atomic_and(volatile UBINT *lpDest, UBINT Value){ return InterlockedAnd64((LONGLONG *)lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_or(volatile UBINT *lpDest, UBINT Value){ return InterlockedOr64((LONGLONG *)lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_xor(volatile UBINT *lpDest, UBINT Value){ return InterlockedXor64((LONGLONG *)lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_xchg(volatile UBINT *lpDest, UBINT NewValue){ return InterlockedExchange64((LONGLONG *)lpDest, NewValue); }
static inline UBINT atomic_cmpxchg(volatile UBINT *lpDest, UBINT NewValue, UBINT OldValue){ return InterlockedCompareExchange64((LONGLONG *)lpDest, NewValue, OldValue); }
static inline bool atomic_bitset_xchg(volatile UBINT *lpDest, UBINT BitPos){ return 0 != InterlockedBitTestAndSet64((LONGLONG *)lpDest, BitPos); }
static inline bool atomic_bitclr_xchg(volatile UBINT *lpDest, UBINT BitPos){ return 0 != InterlockedBitTestAndReset64((LONGLONG *)lpDest, BitPos); }
#endif
#endif
#elif defined LIBENV_OS_LINUX
#include "signal.h"
#include "pthread.h"
#include "sys/syscall.h" //futex support, avaliable since kernel v2.6

#if defined LIBENV_CPLR_GCC
#if defined LIBENV_SYS_INTELX86
//The following operation requires GCC 4.1.2 and compiler option '-march=i586/pentium(or higher)'. Recent versions of GCC support __atomic functions. But I haven't used them yet.
static inline UBINT atomic_add(volatile UBINT *lpDest, UBINT Value){ return __sync_add_and_fetch(lpDest, Value); }
static inline UBINT atomic_xadd(volatile UBINT *lpDest, UBINT Value){ return __sync_fetch_and_add(lpDest, Value); }
static inline UBINT atomic_and(volatile UBINT *lpDest, UBINT Value){ return __sync_fetch_and_and(lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_or(volatile UBINT *lpDest, UBINT Value){ return __sync_fetch_and_or(lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_xor(volatile UBINT *lpDest, UBINT Value){ return __sync_fetch_and_xor(lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_xchg(volatile UBINT *lpDest, UBINT NewValue){ return __sync_lock_test_and_set(lpDest, NewValue); } //possibly incorrect
static inline UBINT atomic_cmpxchg(volatile UBINT *lpDest, UBINT NewValue, UBINT OldValue){ return __sync_val_compare_and_swap(lpDest, OldValue, NewValue); }
#elif defined LIBENV_SYS_INTELX64
//The following operation requires GCC 4.1.2 and compiler option '-march=i586/pentium(or higher)'. Recent versions of GCC support __atomic functions. But I haven't used them yet.
static inline UBINT atomic_add(volatile UBINT *lpDest, UBINT Value){ return __sync_add_and_fetch(lpDest, Value); }
static inline UBINT atomic_xadd(volatile UBINT *lpDest, UBINT Value){ return __sync_fetch_and_add(lpDest, Value); }
static inline UBINT atomic_and(volatile UBINT *lpDest, UBINT Value){ return __sync_fetch_and_and(lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_or(volatile UBINT *lpDest, UBINT Value){ return __sync_fetch_and_or(lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_xor(volatile UBINT *lpDest, UBINT Value){ return __sync_fetch_and_xor(lpDest, Value); } //this function is slow on x86-based CPUs.
static inline UBINT atomic_xchg(volatile UBINT *lpDest, UBINT NewValue){ return __sync_lock_test_and_set(lpDest, NewValue); } //possibly incorrect
static inline UBINT atomic_cmpxchg(volatile UBINT *lpDest, UBINT NewValue, UBINT OldValue){ return __sync_val_compare_and_swap(lpDest, OldValue, NewValue); }
#endif
#endif
#else
#endif

namespace nsBasic{
	//Every pointer we used in the following structures is stored as UBINT,which is not suitable in certain architectures.

	struct ThreadStub{
		//Every element in this structure is supposed to be aligned to sizeof(UBINT). 
		UBINT hThread;	 //It's impossible to control a thread directly through its tid in Windows. But you can do it in Linux. In Linux we'll store tid here.
		UBINT WaitingThread_Next;    //Reserved by this library.DO NOT MODIFY ITS VALUE.
		UBINT WaitingThread_Sibling; //Reserved by this library.DO NOT MODIFY ITS VALUE.
		UBINT LockWaiting;           //Reserved by this library.DO NOT MODIFY ITS VALUE.
		UBINT FutexAddr;             //Reserved by this library.DO NOT MODIFY ITS VALUE.
		UBINT ObjBinLock;
		MemHeap ThreadHeap;
		void *ExtInfo;
	};

	extern volatile UBINT ThreadMgrLock;
	extern UBINT ThreadCntr;

	// YieldCurThread tells the scheduler that this thread will give all its remaining CPU time in this round to other working threads.
	// (Its behaviour is not EXACTLY the same as the discription above...whatever.)
	// this function is used to prevent other threads from starvation when a loop is used to check signals from other threads (as in a message loop).
	extern inline void YieldCurThread(){
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Use a suitable API provided by the OS.
#endif

#if defined LIBENV_OS_WIN
		SwitchToThread();
#elif defined LIBENV_OS_LINUX
		sched_yield(); //pthread_yield() calls this function.
#endif
	}

	// YieldCPU is simply a nop which tells the CPU that other cores may go first.
	// This function only works on HT CPUs. It is used to prevent other threads from starvation in lock mechanisms.
	extern inline void YieldCPU(){
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Use a suitable API provided by the OS.
#endif

#if defined LIBENV_OS_WIN
		YieldProcessor();
#elif defined LIBENV_OS_LINUX
#if (defined LIBENV_CPLR_GCC) && ((defined LIBENV_SYS_INTELX86) || (defined LIBENV_SYS_INTELX64))
		asm volatile("rep;nop" ::: "memory"); //in newer architectures it will be translated to pause instruction.
#endif
#endif
	}

	extern void _ThreadSupport_Init(ThreadStub *lpMainThreadStub);
	extern void _ThreadSupport_Destroy(ThreadStub *lpMainThreadStub);
	//These two functions should never be called by library users!

	extern ThreadStub *GetCurThreadStub();

	static void _EnterLock(volatile UBINT *lpLock, volatile ThreadStub *CurThreadStub); //DO NOT USE THIS
	static void _LeaveLock(volatile UBINT *lpLock, volatile ThreadStub *CurThreadStub);  //DO NOT USE THIS
	extern inline void EnterLock(volatile UBINT *lpLock){ return _EnterLock(lpLock, nsBasic::GetCurThreadStub()); }
	extern inline void LeaveLock(volatile UBINT *lpLock){ _LeaveLock(lpLock, nsBasic::GetCurThreadStub()); }

	struct Mutex{
		UBINT LockStat;

		void Initialize(); //a POD cannot have a nontrivial constructor.
		void Enter();
		void Leave();
	};

	struct RWLock{
	public:
		static const UBINT WakeUpSpinRound = 0x200;
		static const UBINT BackoffRound = 0x100;
		struct LockStub{
			LockStub* Prev;
			LockStub* Next;
			LockStub* QueueHead;
			UBINT ReaderCnt;
			UBINT Flag;
		};
		UBINT LockStat;

		void Initialize(); //a POD cannot have a nontrivial constructor.
		void Enter_Read();
		void Leave_Read();
		void Enter_Write();
		void Leave_Write();
	private:
		void Optimize(UBINT Stat);
		void WakeUp(UBINT Stat);
	};

	struct Semaphore{
		UBINT Value;

		void Initialize(UBINT ResCount); //a POD cannot have a nontrivial constructor.
		void Enter();
		void Leave();
	};

	struct CyclicBarrier{
		UBINT StackPtr;
		UBINT ThreadCount;

		void Initialize(UBINT ThreadCount); //a POD cannot have a nontrivial constructor.
		//ThreadCount must be a positive integer.
		void Wait();
	};

	struct CountdownBarrier{
		UBINT StackPtr;
		UBINT ThreadCount;

		void Initialize(UBINT ThreadCount); //a POD cannot have a nontrivial constructor.
		//ThreadCount must be a positive integer.
		void Wait();
	};

	struct Event{
		UBINT StackPtr;

		void Initialize(); //a POD cannot have a nontrivial constructor.
		//ThreadCount must be a positive integer.
		void Wait();
		void Wake();
		void Pulse();
	};

	extern UBINT CreateNormalThread(void *(*lpStartAddr)(void *), void *lpArg); //Not thread safe enough
	extern void *WaitThread(UBINT Handle);
}

/*-------------------------------- IMPLEMENTATION --------------------------------*/

namespace nsBasic{
#if defined LIBENV_OS_WIN
	HANDLE GlobalKEHandle;
#endif
	UBINT _ThreadMgrTLS;
	extern volatile UBINT ThreadMgrLock = (UBINT)NULL;
	extern UBINT ThreadCntr = 0;
	//need to implement a thread pool later...

	//Futex examples:
	//Windows: Wait - NtWaitForKeyedEvent(GlobalKEHandle, Addr, 0, nullptr);
	//         Wake - NtReleaseKeyedEvent(GlobalKEHandle, Addr, 0, nullptr);
	//Linux:   Wait - *Addr = 0;syscall(SYS_futex, Addr, FUTEX_WAIT, 0, nullptr, nullptr, 0);
	//         Wake - *Addr = 1;syscall(SYS_futex, Addr, FUTEX_WAKE, 1, nullptr, nullptr, 0);

	extern void _ThreadSupport_Init(ThreadStub *lpMainThreadStub){
#if defined LIBENV_OS_WIN
		nsBasic::_ThreadMgrTLS = TlsAlloc();
		if (TLS_OUT_OF_INDEXES == nsBasic::_ThreadMgrTLS)throw new std::exception("TLS allocation failed.");
		TlsSetValue((DWORD)nsBasic::_ThreadMgrTLS, (LPVOID)lpMainThreadStub);
		if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), (HANDLE *)&(lpMainThreadStub->hThread), 0, false, DUPLICATE_SAME_ACCESS))throw new std::exception("Cannot get the handle of the main thread.");
		//GetCurrentThread() returns a fake handle which can only be used in the calling thread. DuplicateHandle() returns a real one.
		if (NtCreateKeyedEvent(&GlobalKEHandle, (DWORD)-1, nullptr, 0))throw new std::exception("Failed to initialize keyed event support.");
#elif defined LIBENV_OS_LINUX
		sigset_t tmpSigMask;

		sigfillset(&tmpSigMask);
		pthread_sigmask(SIG_BLOCK, &tmpSigMask, NULL);

		//TLS is supported since Linux kernel v2.6.
		if(EAGAIN == pthread_key_create(&nsBasic::_ThreadMgrTLS, 0))throw new std::exception("TLS allocation failed.");

		pthread_setspecific(nsBasic::_ThreadMgrTLS, lpMainThreadStub);
		lpMainThreadStub->hThread = pthread_self(); //since kernel 2.6.19
		//We'll use signal mechanism to synchronize threads in Linux.
#endif
		lpMainThreadStub->WaitingThread_Next = (UBINT)nullptr;
		lpMainThreadStub->WaitingThread_Sibling = (UBINT)nullptr;
		lpMainThreadStub->LockWaiting = (UBINT)nullptr;
		lpMainThreadStub->FutexAddr = 0;
		lpMainThreadStub->ObjBinLock = (UBINT)nullptr;
		lpMainThreadStub->ThreadHeap = { nullptr };
		lpMainThreadStub->ExtInfo = nullptr;
	}
	extern void _ThreadSupport_Destroy(ThreadStub *lpMainThreadStub){
#if defined LIBENV_OS_WIN
		CloseHandle(GlobalKEHandle);
		CloseHandle((HANDLE)lpMainThreadStub->hThread);
		TlsFree((DWORD)nsBasic::_ThreadMgrTLS);
#elif defined LIBENV_OS_LINUX
		pthread_key_delete(nsBasic::_ThreadMgrTLS);
#endif
	}

	extern ThreadStub *GetCurThreadStub(){
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:This function is usually implemented by thread local storage access.
#endif

#if defined LIBENV_OS_WIN
		return (ThreadStub *)TlsGetValue((DWORD)_ThreadMgrTLS);
#elif defined LIBENV_OS_LINUX
		return (ThreadStub *)pthread_getspecific(_ThreadMgrTLS);
#else
		return NULL;
#endif
	}

	//The following functions provide the basic lock synchronization mechanism.
	//The lock implementation here is a queuing lock. Such a lock is slow in high-contention scenes. But it won't keep CPU busy and no thread will suffer from starvation.
	//The queue is implemented as a linked list. The ThreadStub structure itself serves as a node in the list.
	//This lock is not recursive and deadlock check is not implemented.

	void _EnterLock(volatile UBINT *lpLock, volatile ThreadStub *CurThreadStub){
		ThreadStub *CompeteThreadStub;
		if (nullptr != (CompeteThreadStub = (ThreadStub *)atomic_xchg(lpLock, (UBINT)CurThreadStub))){ //lpLock will always store the last competitor for this lock.
			CurThreadStub->LockWaiting = (UBINT)lpLock;
			//STATE_LOCK_ENTER_0
			CurThreadStub->WaitingThread_Sibling = atomic_xchg(&(CompeteThreadStub->WaitingThread_Next), (UBINT)CurThreadStub); 

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check these lines.
				//Here we need to suspend the current thread.
#endif

#if defined LIBENV_OS_WIN
			NtWaitForKeyedEvent(GlobalKEHandle, (PVOID)&CurThreadStub->FutexAddr, 0, nullptr);
#elif defined LIBENV_OS_LINUX
			CurThreadStub->FutexAddr = 0;
			syscall(SYS_futex, (void *)&CurThreadStub->FutexAddr, FUTEX_WAIT, 0, nullptr, nullptr, 0);
			CurThreadStub->FutexAddr = 0;
#endif
		}
	}
	void _LeaveLock(volatile UBINT *lpLock, volatile ThreadStub *CurThreadStub){
		ThreadStub *CompeteThreadStub, *tmpStub1, *tmpStub2;
		if ((UBINT)CurThreadStub != atomic_cmpxchg(lpLock, (UBINT)nullptr, (UBINT)CurThreadStub)){ //do nothing when no one is waiting
			//Then there is a thread waiting for this lock.
			//Spinlock until the correct competing thread has been found.
			//We'll find the correct competing thread by traversing the competing thread list.
			CompeteThreadStub = (ThreadStub *)CurThreadStub->WaitingThread_Next;
			while (1){
				if (nullptr == CompeteThreadStub){ //possible when the waiting thread is at STATE_LOCK_ENTER_0.
					YieldCPU();
					//pointer reset
					CompeteThreadStub = (ThreadStub *)CurThreadStub->WaitingThread_Next;
				}
				else{
					//one thread may hold multiple locks. We need to check which one request for this lock.
					if ((UBINT)lpLock == CompeteThreadStub->LockWaiting)break;
					else CompeteThreadStub = (ThreadStub *)CompeteThreadStub->WaitingThread_Sibling;
				}
			}
			//Detach the thread competing for this lock from the linked list.
			//We first break the linked list at CompeteThreadStub.
			tmpStub1 = (ThreadStub *)atomic_xchg(&(CurThreadStub->WaitingThread_Next), CompeteThreadStub->WaitingThread_Sibling);
			//now CurThreadStub->WaitingThread_Next is the successor of CompeteThreadStub.
			if (tmpStub1 != CompeteThreadStub){
				//find the predecessor of CompeteThreadStub as tmpStub2
				tmpStub2 = tmpStub1;
				while ((ThreadStub *)tmpStub2->WaitingThread_Sibling != CompeteThreadStub)tmpStub2 = (ThreadStub *)tmpStub2->WaitingThread_Sibling;
				tmpStub2->WaitingThread_Sibling = atomic_xchg(&(CurThreadStub->WaitingThread_Next), (UBINT)tmpStub1);
			}
			CompeteThreadStub->WaitingThread_Sibling = (UBINT)nullptr; //must be done!
			//Now wake up the compete thread.


#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check these lines.
#endif

#if defined LIBENV_OS_WIN
			NtReleaseKeyedEvent(GlobalKEHandle, (PVOID)&CompeteThreadStub->FutexAddr, 0, nullptr);
#elif defined LIBENV_OS_LINUX
			CompeteThreadStub->FutexAddr = 1;
			syscall(SYS_futex, (void *)&CompeteThreadStub->FutexAddr, FUTEX_WAKE, 1, nullptr, nullptr, 0);
#endif
		}
	}

	//Mutexes are preemptive locks which is generally faster than the queuing lock above and cost more CPU time.
	//The framework of this lock implementation here comes from http://locklessinc.com/articles/keyed_events/.
	//This lock is not absolutely safe. But it is safe as long as the OS cannot support more than (UBINT_MAX >> 9) threads.
	//This lock is not recursive and deadlock check is not implemented.

	//Linux support is not verified
	void Mutex::Initialize(){
		this->LockStat = 0;
		// Windows: 0x1: occupied
		//          0x100: waking someone up
		//          LockStat >> 9: threads sleeping on this lock
		// Linux: 0x1: Locked
		//        0x100: Contended
	}


	void Mutex::Enter(){
		UBINT OldStat;
		const UBINT BackoffRound = 0x1000; //works well on Intel Core i5 4200M / Windows 7 sp1. Exponential backoff make things even worse.
#if defined LIBENV_OS_WIN
		while (true){
			//OldStat = lpLock->LockStat;
			//if (0 == (this->LockStat & 0x1)){ //Actually this branch makes the lock slower.

				//It seems that atomic spinning doesn't help much.
				//for (UBINT i = 0; i < 0x200; i++){
					if (false == atomic_bitset_xchg(&this->LockStat, 0))return;
					//YieldCPU();
				//}
			//}

			//STATE_MUTEX_ENTER_0

			//give up spinning, go to sleep
			OldStat = this->LockStat | 1; //Must turn the "locked" bit on as someone may have left the lock at STATE_MUTEX_ENTER_0.
			if (atomic_cmpxchg(&this->LockStat, OldStat + 0x200, OldStat) == OldStat){
				NtWaitForKeyedEvent(GlobalKEHandle, (PVOID)&this->LockStat, 0, nullptr);
				//awake. clear the wake bit
				atomic_add(&this->LockStat, (UBINT)-0x100);
				//For unknown reason (more parallel as different CPU execution units are used?), the line above is better than atomic_bitclr_xchg(&lpLock->LockStat, 8). and much better than non-atomic assignment.
			}

			//The backoff strategy here gives a huge speed boost to this lock.
			for (UBINT i = 0; i<BackoffRound; i++)YieldCPU();
			//if(BEB<0x10000)BEB<<=1; //Exponential backoff doesn't work well in this lock.
		}
#elif defined LIBENV_OS_LINUX
		//not tested yet, need further improvement
		//spin for a while
		for (UBINT i = 0; i < 0x200; i++){
			OldStat = atomic_or(&this->LockStat, 0x1);
			if (0 == (OldStat & 0x1))return;
			YieldCPU();
		}

		//give up, go to sleep
		while ((atomic_xchg(&this->LockStat, 0x101) & 0x1) > 0){
			syscall(SYS_futex, (void *)&this->LockStat, FUTEX_WAIT, 0x1FF, nullptr, nullptr, 0);
		}
#endif
	}
	void Mutex::Leave(){
#if defined LIBENV_OS_WIN
		//release the lock
		*(unsigned char *)&this->LockStat = 0; //much faster than lock btr
		while (true){
			UBINT OldStat = this->LockStat;
			//if nobody is waiting || someone has acquired the lock || someone is waking up others
			//then return
			if (OldStat<0x200 || (OldStat & 0x1FF) > 0)break;
			//if the status hasn't changed
			//try to wake up someone
			if (atomic_cmpxchg(&this->LockStat, OldStat - 0x200 + 0x100, OldStat) == OldStat){
				NtReleaseKeyedEvent(GlobalKEHandle, (PVOID)&this->LockStat, 0, nullptr);
				break;
			}
			YieldCPU();
		}
#elif defined LIBENV_OS_LINUX
		//not tested yet, need further improvement
		UBINT OldStat;

		//return when nobody is waiting
		if (0x1 == this->LockStat && 0x1 == atomic_cmpxchg(&this->LockStat, 0, 0x1))return;
		*(unsigned char *)&this->LockStat = 0;
		//wait for someone to acquire the lock
		for (UBINT i = 0; i < 0x200; i++){
			if ((this->LockStat & 0x1) > 0)return;
			YieldCPU();
		}

		//try to wake up someone
		atomic_and(&this->LockStat, ~(UBINT)0x100);
		syscall(SYS_futex, (void *)&this->LockStat, FUTEX_WAKE, 1, nullptr, nullptr, 0);
#endif
	}

	//Read-write locks are used when read requests are much more frequent than write requests.
	//This lock algorithm comes from the slim read-write lock on Windows, whose internal synchronization does not rely on mutexes.
	//The internal structure of the lock status looks like this:
	// [ Addr ... | Mixed | Waking | Contended | Locked ]
	// MSB                                            LSB
	// bool Locked: true when at least one thread has acquired the lock through Enter_Read or Enter_Write.
	// bool Contended: true when at least one thread is waiting for this lock.
	// bool Waking: true when one thread is going to call WakeUp.
	// bool Mixed: true when both pending read and write requests exist.
	// Addr: If a pending write request exists, it's a pointer to the tail of the waiting queue. Otherwise it is the number of reading threads.
	//
	//[ 0 | 0 | 0 | 0 | 1 ] indicates a writing thread has entered the lock.
	//[ 1 | 0 | 0 | 0 | 1 ] indicates a reading thread has entered the lock.
	//
	//The waiting queue is implemented with a linked list, whose nodes are structured as follows:
	//struct LockStub{
	//	LockStub* Prev; // The previous node in the list.
	//	LockStub* Next; // The next node in the list. This pointer is not initialized until the list is traversed.
	//	LockStub* QueueHead; // The head of the queue known by this node. It is not necessary the current head of the queue.
	//	UBINT ReaderCnt; // If multiple reader is sharing the lock and a writer is entering. The member [Addr] of the lock status will be replaced with the address of the tail node. The original [Addr] value will be stored here.
	//	UBINT Flag;
	//   [ Awake | W/R ]
	//   2             1
	//   bool Awake: true when the thread is going to sleep, but still spin-waiting for another thread to wake it up.
	//   bool W/R: true when the thread is a writing thread.
	//};

	//Linux support is not available.

	void RWLock::Initialize(){
		this->LockStat = 0;
	}
	void RWLock::Optimize(UBINT Stat){
		UBINT CurStat;
		UBINT OldStat;

		OldStat = Stat;
		while (true){
			if (OldStat & 1){
				//locked
				LockStub *CurStub = (LockStub *)(OldStat&(~(UBINT)0xF));
				if (nullptr != CurStub){
					LockStub *TmpPtr = CurStub;
					while (nullptr == TmpPtr->QueueHead){
						LockStub *NextPtr = TmpPtr;
						TmpPtr = TmpPtr->Prev;
						TmpPtr->Next = NextPtr;
					}
					CurStub->QueueHead = TmpPtr->QueueHead;
				}

				CurStat = atomic_cmpxchg(&this->LockStat, OldStat - 4, OldStat); //clear the waking bit
				if (CurStat == OldStat)break;
				OldStat = CurStat;
			}
			else{
				//not locked
				//This branch won't be triggered in the first loop iteration.
				WakeUp(OldStat);
				break;
			}
		}
	}
	void RWLock::WakeUp(UBINT Stat){
		UBINT NewStat;
		UBINT CurStat;
		UBINT OldStat;
		LockStub *WakeUpStub, *CurStub;

		OldStat = Stat;
		while (true){
			if (OldStat & 1){
				//Locked, which means that someone has acquired the lock. We'll leave the burden to him.
				//This branch won't be triggered in the first loop iteration.

				//NewStat = OldStat;
				//NewStat.Waking = false;
				NewStat = OldStat - 4;
				CurStat = atomic_cmpxchg(&this->LockStat, NewStat, OldStat);
				if (CurStat == OldStat)return;
				OldStat = CurStat;
			}
			else{
				//not locked
				CurStub = (LockStub *)(OldStat&(~(UBINT)0xF));

				//find the head node of the queue
				LockStub *TmpPtr = CurStub;
				while (nullptr == TmpPtr->QueueHead){
					LockStub *NextPtr = TmpPtr;
					TmpPtr = TmpPtr->Prev;
					TmpPtr->Next = NextPtr;
				}
				WakeUpStub = TmpPtr->QueueHead;

				if (nullptr != WakeUpStub->Next && (WakeUpStub->Flag & 1) > 0){
					//the head node is a writing thread && it isn't the only one waiting
					//wake up the head node only
					CurStub->QueueHead = WakeUpStub->Next;
					WakeUpStub->Next = nullptr;

					atomic_and(&this->LockStat, ~(UBINT)0x4); //clear waking bit. better than lock btr for unknown reason (more parallel as different CPU execution units are used?).
					if (false == atomic_bitclr_xchg(&WakeUpStub->Flag, 1))NtReleaseKeyedEvent(GlobalKEHandle, (PVOID)WakeUpStub, 0, nullptr);
					return;
				}
				else{
					//wake up everyone
					CurStub->QueueHead = WakeUpStub;

					CurStat = atomic_cmpxchg(&this->LockStat, 0, OldStat);
					if (CurStat == OldStat){
						//success
						//traverse the queue
						do{
							LockStub *PrevStub = WakeUpStub;
							WakeUpStub = WakeUpStub->Next;
							if (false == atomic_bitclr_xchg(&PrevStub->Flag, 1))NtReleaseKeyedEvent(GlobalKEHandle, (PVOID)PrevStub, 0, nullptr);
						} while (nullptr != WakeUpStub);
						return;
					}
					else OldStat = CurStat;
				}
			}
		}
	}
	void RWLock::Enter_Write(){
		__declspec(align(16)) LockStub MyStub = {}; //default initializer, aligned to 16 so all 4 flag bits can be packed into the pointer.

		UBINT NewStat;
		UBINT CurStat;
		UBINT OldStat;
		bool NeedOptimize;

		if (true == atomic_bitset_xchg(&this->LockStat, 0)){
			//someone has acquired the lock
			OldStat = this->LockStat;
			while (true){
				if (OldStat & 1){
					//occupied

					MyStub.Next = nullptr;
					//MyStub.Flag = Awake | WriteThread
					MyStub.Flag = 3;
					NeedOptimize = false;
					if (OldStat & 2){
						//contended, a writer is present
						MyStub.Prev = (LockStub *)(OldStat&(~(UBINT)0xF));
						MyStub.QueueHead = nullptr;
						MyStub.ReaderCnt = 0;
						//NewStat.Addr = &MyStub;
						//NewStat.Mixed = OldStat.Mixed;
						//NewStat.Waking = true;
						//NewStat.Contended = true;
						//NewStat.Locked = true;
						NewStat = (UBINT)&MyStub | (OldStat & 0x8) | 0x7;
						if (0 == (OldStat & 4))NeedOptimize = true; //no thread is waking up
					}
					else{
						//not contended, no writer is present
						MyStub.QueueHead = &MyStub;
						MyStub.ReaderCnt = OldStat >> 4;
						//if (OldStat.Addr > 1){
						//	//a reader exists
						//	NewStat.Addr = &MyStub;
						//	NewStat.Mixed = true;
						//	NewStat.Waking = false;
						//	NewStat.Contended = true;
						//	NewStat.Locked = true;
						//}
						if (MyStub.ReaderCnt > 1)NewStat = (UBINT)&MyStub | 0xB;
						//else{
						//	//nobody is waiting
						//	NewStat.Addr = &MyStub;
						//	NewStat.Mixed = false;
						//	NewStat.Waking = false;
						//	NewStat.Contended = true;
						//	NewStat.Locked = true;
						//}
						else NewStat = (UBINT)&MyStub | 0x3;
					}

					CurStat = atomic_cmpxchg(&this->LockStat, NewStat, OldStat);
					if (CurStat == OldStat){
						//success
						if (NeedOptimize)Optimize(NewStat);
						for (UBINT i = 0; i < WakeUpSpinRound; i++){
							if (0 == (MyStub.Flag & 2))break;
							YieldCPU();
						}
						if (true == atomic_bitclr_xchg(&MyStub.Flag, 1))NtWaitForKeyedEvent(GlobalKEHandle, (PVOID)&MyStub, 0, nullptr);
					}
					else{
						//failed
						for (UBINT i = 0; i < BackoffRound; i++)YieldCPU();
						OldStat = this->LockStat;
					}
				}
				else{
					//not occupied
					if (atomic_cmpxchg(&this->LockStat, OldStat | 1, OldStat) == OldStat)return;
					for (UBINT i = 0; i < BackoffRound; i++)YieldCPU();
					OldStat = this->LockStat;
				}
			}
		}
	}
	void RWLock::Leave_Write(){
		UBINT NewStat;
		UBINT CurStat;
		UBINT OldStat;

		OldStat = atomic_xadd(&this->LockStat, (UBINT)-1);
		if (OldStat & 1){
			//locked
			OldStat--;
			if ((OldStat & 2) > 0 && 0 == (OldStat & 4)){
				//contended, no waking up

				//NewStat = OldStat;
				//NewStat.Locked = false;
				//NewStat.Waking = true;
				NewStat = OldStat | 4;
				CurStat = atomic_cmpxchg(&this->LockStat, NewStat, OldStat);
				if (CurStat == OldStat){
					//success
					WakeUp(NewStat);
				}
			}
		}
		else throw new std::exception("Unknown error occured inside the read - write lock.");
	}
	void RWLock::Enter_Read(){
		__declspec(align(16)) LockStub MyStub = {}; //default initializer, aligned to 16 so all 4 flag bits can be packed into the pointer.

		UBINT NewStat;
		UBINT CurStat;
		UBINT OldStat;
		bool NeedOptimize;

		//	//nobody is waiting
		//	NewStat.Addr = 1;
		//	NewStat.Mixed = false;
		//	NewStat.Waking = false;
		//	NewStat.Contended = false;
		//	NewStat.Locked = true;
		//	OldStat = atomic_cmpxchg(&this->LockStat, NewStat, 0);
		OldStat = atomic_cmpxchg(&this->LockStat, 0x11, 0);
		if (OldStat){
			//someone is doing something on this lock
			while (true){
				// if (OldStat.Locked && (OldStat.Contended || 0 == OldStat.Addr)){
				if ((OldStat & 1) && ((OldStat & 2) > 0 || 0 == (OldStat >> 4))){
					//someone has acquired the lock
					MyStub.Next = nullptr;
					MyStub.ReaderCnt = 0;
					//MyStub.Flag = Awake | ReadThread
					MyStub.Flag = 2;
					NeedOptimize = false;

					if (OldStat & 2){
						//contended
						MyStub.Prev = (LockStub *)(OldStat&(~(UBINT)0xF));
						MyStub.QueueHead = nullptr;
						//	NewStat.Addr = &MyStub;
						//	NewStat.Mixed = OldStat.Mixed;
						//	NewStat.Waking = true;
						//	NewStat.Contended = true;
						//	NewStat.Locked = OldStat.Locked;
						NewStat = (UBINT)&MyStub | (OldStat & 0x9) | 0x6;
						if (0 == (OldStat & 4))NeedOptimize = true; //no thread is waking up
					}
					else{
						//not contended
						MyStub.QueueHead = &MyStub;
						//	NewStat.Addr = &MyStub;
						//	NewStat.Mixed = false;
						//	NewStat.Waking = false;
						//	NewStat.Contended = true;
						//	NewStat.Locked = true;
						NewStat = (UBINT)&MyStub | 0x3;
					}

					CurStat = atomic_cmpxchg(&this->LockStat, NewStat, OldStat);
					if (CurStat == OldStat){
						//success
						if (NeedOptimize)Optimize(NewStat);
						for (UBINT i = 0; i < WakeUpSpinRound; i++){
							if (0 == (MyStub.Flag & 2))break;
							YieldCPU();
						}
						if (true == atomic_bitclr_xchg(&MyStub.Flag, 1))NtWaitForKeyedEvent(GlobalKEHandle, (PVOID)&MyStub, 0, nullptr);
					}
					else{
						//failed
						for (UBINT i = 0; i < BackoffRound; i++)YieldCPU();
						OldStat = this->LockStat;
					}
				}
				else{
					//NewStat = OldStat;
					//NewStat.Locked = true;
					//if (false == OldStat.Contended)NewStat.Addr++;
					NewStat = OldStat | 1;
					if (0 == (OldStat & 2))NewStat += 0x10;

					CurStat = atomic_cmpxchg(&this->LockStat, NewStat, OldStat);
					if (CurStat == OldStat)return;

					for (UBINT i = 0; i < BackoffRound; i++)YieldCPU();
					OldStat = this->LockStat;
				}
			}
		}
	}
	void RWLock::Leave_Read(){
		UBINT NewStat;
		UBINT CurStat;
		UBINT OldStat;
		LockStub *PrevStub;

		OldStat = atomic_cmpxchg(&this->LockStat, 0, 0x11);
		if (0x11 != OldStat){
			if (OldStat & 1){
				//locked
				while (true){
					if (OldStat & 2){
						//contended
						if (OldStat & 8){
							//mixed status (both read and write requests exist)
							PrevStub = (LockStub*)(OldStat&(~(UBINT)0xF));
							while (nullptr == PrevStub->QueueHead)PrevStub = PrevStub->Prev;
							if (atomic_add(&(PrevStub->QueueHead->ReaderCnt), (UBINT)-1) > 0)return;
						}

						while (true){
							//NewStat = OldStat;
							//NewStat.Mixed = false;
							//NewStat.Locked = false;
							NewStat = OldStat & ~(UBINT)0x9;
							if (OldStat & 4){
								//someone is waking up
								CurStat = atomic_cmpxchg(&this->LockStat, NewStat, OldStat);
								if (CurStat == OldStat)return;
							}
							else{
								//NewStat.Waking = true;
								NewStat |= 4;
								CurStat = atomic_cmpxchg(&this->LockStat, NewStat, OldStat);
								if (CurStat == OldStat){
									//success
									WakeUp(NewStat);
									return;
								}
							}
							OldStat = CurStat;
						}
					}
					else{
						//if (0 == OldStat.Addr)NewStat = { 0 };
						//else{
						//	NewStat = OldStat;
						//	NewStat.Addr--;
						//}
						if ((OldStat >> 4) <= 1)NewStat = 0; else NewStat = OldStat - 0x10;
						if (1 == NewStat){
							NewStat = 0;
						}
						CurStat = atomic_cmpxchg(&this->LockStat, NewStat, OldStat);
						if (CurStat == OldStat)return;
						OldStat = CurStat;
					}
				}
			}
			else throw new std::exception("Unknown error occured inside the read-write lock.");
		}
	}

// not conditional variable
//	void CondVar::Initialize(){
//		this->FutexStat = 0;
//	}
//	void CondVar::Wait(){
//#if defined LIBENV_OS_WIN
//		NtWaitForKeyedEvent(GlobalKEHandle, &this->FutexStat, 0, nullptr);
//#elif defined LIBENV_OS_LINUX
//		this->FutexStat = 0;
//		syscall(SYS_futex, (void *)&this->FutexStat, FUTEX_WAIT, 0, nullptr, nullptr, 0);
//#endif
//	}
//	void CondVar::Notify(){
//#if defined LIBENV_OS_WIN
//		NtReleaseKeyedEvent(GlobalKEHandle, &this->FutexStat, 0, nullptr);
//#elif defined LIBENV_OS_LINUX
//		this->FutexStat = 1;
//		syscall(SYS_futex, (void *)&this->FutexStat, FUTEX_WAKE, 1, nullptr, nullptr, 0);
//#endif
//	}

	//Semaphores can be used to manage multiple identical resources. The number of resources is initialized when a semaphore is created. Then the user can use the semaphore as multiple locks.
	//The speed of the implementation below is slow and further optimization is difficult. but it doesn't matter much when the initial value of the semaphore is high. 
	//This synchronization mechanism is not recursive and deadlock check is not implemented.

	//Linux support is not available.
	void Semaphore::Initialize(UBINT ResCount){
		this->Value = ResCount;
	}

	void Semaphore::Enter(){
		BINT NewStat = (BINT)atomic_add(&this->Value, (UBINT)-1);
		if (NewStat < 0)NtWaitForKeyedEvent(GlobalKEHandle, &this->Value, 0, nullptr);
	}
	void Semaphore::Leave(){
		BINT NewStat = (BINT)atomic_add(&this->Value, (UBINT)1);
		if (NewStat <= 0)NtReleaseKeyedEvent(GlobalKEHandle, &this->Value, 0, nullptr);
	}

	//Cyclic barriers halt the waiting thread until the required number of threads has reached the barrier point.
	//The implementation below is a pseudo-lockless algorithm which uses a bit-size spin lock to reduce the size of the barrier.
	//Deadlock check is not implemented.

	//Linux support is not available.
	void CyclicBarrier::Initialize(UBINT ThreadCount){
		this->StackPtr = (UBINT)nullptr;
		this->ThreadCount = ThreadCount;
	}

	void CyclicBarrier::Wait(){
		const UBINT BackoffRound = 0x20;
		const UBINT SpinWaitRound = 0x400;
		//This structure should be aligned by 2. But most compiler do this by default.
		struct BarrierStub{
			BarrierStub* Prev;
			UBINT Value;
			UBINT FutexAddr;
		} MyStub;

		//Acquire the lock
		while (true == atomic_bitset_xchg(&this->StackPtr, 0)){
			for (UBINT i = 0; i<BackoffRound; i++)YieldCPU();
		}

		UBINT OldStat = this->StackPtr & (UBINT)-2;
		MyStub.Prev = (BarrierStub *)OldStat;
		if (nullptr == MyStub.Prev)MyStub.Value = this->ThreadCount - 1;
		else MyStub.Value = MyStub.Prev->Value - 1;
		MyStub.FutexAddr = 0;

		if (0 == MyStub.Value){
			this->StackPtr = (UBINT)nullptr;
			BarrierStub *PrevStub = MyStub.Prev;
			while (nullptr != PrevStub){
				BarrierStub *TmpStub = PrevStub->Prev;

				if (1 == atomic_xchg(&PrevStub->FutexAddr,1))NtReleaseKeyedEvent(GlobalKEHandle, &PrevStub->FutexAddr, 0, nullptr);
				PrevStub = TmpStub;
			}
		}
		else{
			this->StackPtr = (UBINT)&MyStub;
			//spin-waiting
			for (UBINT i = 0; i < SpinWaitRound; i++){
				if (1 == MyStub.FutexAddr)return;
				YieldCPU();
			}
			if (1 == atomic_xchg(&MyStub.FutexAddr, 1))return;
			NtWaitForKeyedEvent(GlobalKEHandle, &MyStub.FutexAddr, 0, nullptr);
		}
	}

	//Countdown barriers halt the waiting thread until the required number of threads has reached the barrier point. The barrier can only be used once.
	//A memory access violation may occur when the number of threads waiting on this barrier exceeds the initialized barrier size.
	//Deadlock check is not implemented.

	//Linux support is not available.
	void CountdownBarrier::Initialize(UBINT ThreadCount){
		this->StackPtr = (UBINT)nullptr;
		this->ThreadCount = ThreadCount;
	}

	void CountdownBarrier::Wait(){
		const UBINT BackoffRound = 0x20;
		const UBINT SpinWaitRound = 0x400;

		struct BarrierStub{
			BarrierStub* Prev;
			UBINT Value;
			UBINT FutexAddr;
		} MyStub;

		//Enqueue
		while (true){
			UBINT OldStat = this->StackPtr;
			if (1 == OldStat)throw new std::exception("Wait on a destroyed countdown barrier.");
			MyStub.Prev = (BarrierStub *)OldStat;
			if (nullptr == MyStub.Prev)MyStub.Value = this->ThreadCount - 1;
			else MyStub.Value = MyStub.Prev->Value - 1; //ABA may happen here, the reason why this barrier is not cyclic.
			MyStub.FutexAddr = 0;

			UBINT CurStat;
			if (0 == MyStub.Value)CurStat = atomic_cmpxchg(&this->StackPtr, 1, OldStat);
			else CurStat = atomic_cmpxchg(&this->StackPtr, (UBINT)&MyStub, OldStat);
			if (CurStat == OldStat)break;
			for (UBINT i = 0; i<BackoffRound; i++)YieldCPU();
		}

		if (0 == MyStub.Value){
			BarrierStub *PrevStub = MyStub.Prev;
			while (nullptr != PrevStub){
				BarrierStub *TmpStub = PrevStub->Prev;
				if (1 == atomic_xchg(&PrevStub->FutexAddr, 1))NtReleaseKeyedEvent(GlobalKEHandle, &PrevStub->FutexAddr, 0, nullptr);
				PrevStub = TmpStub;
			}
		}
		else{
			//spin-waiting
			for (UBINT i = 0; i < SpinWaitRound; i++){
				if (1 == MyStub.FutexAddr)return;
				YieldCPU();
			}
			if (1 == atomic_xchg(&MyStub.FutexAddr, 1))return;
			NtWaitForKeyedEvent(GlobalKEHandle, &MyStub.FutexAddr, 0, nullptr);
		}
	}

	//Events halt the waiting thread until Wake() is called by some other thread. The thread will not halt if the Wake() method is called previously.
	//Pulse() method will wake up all threads waiting on the current event without destroying the event(i.e. other thread can still wait on this event in the future).
	//A Event can be used repeatly. The Initialize() method must be called before the reuse of the Event. Initialize() method and Wake() metho should not be called in the same time.
	//Deadlock check is not implemented.

	//Linux support is not available.
	inline void Event::Initialize(){ this->StackPtr = (UBINT)nullptr; }

	void Event::Wait(){
		const UBINT BackoffRound = 0x20;
		const UBINT SpinWaitRound = 0x400;

		struct EventStub{
			EventStub* Prev;
			UBINT FutexAddr;
		} MyStub;

		//Enqueue
		while (true){
			UBINT OldStat = this->StackPtr;
			if (1 == OldStat)return;
			MyStub.Prev = (EventStub *)OldStat;
			MyStub.FutexAddr = 0;

			UBINT CurStat;
			CurStat = atomic_cmpxchg(&this->StackPtr, (UBINT)&MyStub, OldStat);
			if (CurStat == OldStat)break;
			for (UBINT i = 0; i<BackoffRound; i++)YieldCPU();
		}

		//spin-waiting
		for (UBINT i = 0; i < SpinWaitRound; i++){
			if (1 == MyStub.FutexAddr)return;
			YieldCPU();
		}
		if (1 == atomic_xchg(&MyStub.FutexAddr, 1))return;
		NtWaitForKeyedEvent(GlobalKEHandle, &MyStub.FutexAddr, 0, nullptr);
	}

	void Event::Wake(){
		struct EventStub{
			EventStub* Prev;
			UBINT FutexAddr;
		};

		UBINT CurStat;
		CurStat = atomic_xchg(&this->StackPtr, 1);
		if (1 != CurStat){
			//wake up the previous thread
			EventStub *PrevStub = (EventStub *)CurStat;
			while (nullptr != PrevStub){
				EventStub *TmpStub = PrevStub->Prev;
				if (1 == atomic_xchg(&PrevStub->FutexAddr, 1))NtReleaseKeyedEvent(GlobalKEHandle, &PrevStub->FutexAddr, 0, nullptr);
				PrevStub = TmpStub;
			}
		}
	}

	void Event::Pulse(){
		struct EventStub{
			EventStub* Prev;
			UBINT FutexAddr;
		};

		UBINT CurStat;
		CurStat = atomic_xchg(&this->StackPtr, (UBINT)nullptr);
		if (1 != CurStat){
			//wake up the previous thread
			EventStub *PrevStub = (EventStub *)CurStat;
			while (nullptr != PrevStub){
				EventStub *TmpStub = PrevStub->Prev;
				if (1 == atomic_xchg(&PrevStub->FutexAddr, 1))NtReleaseKeyedEvent(GlobalKEHandle, &PrevStub->FutexAddr, 0, nullptr);
				PrevStub = TmpStub;
			}
		}
	}

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check these lines.
	//We need access to thread local storage variables.
#endif

#if defined LIBENV_OS_WIN
	unsigned _stdcall _ThreadPrepareFunc(void *lpArg){
		void *(*RealEntry)(void *) = (void *(*)(void *))((ThreadStub *)lpArg)->WaitingThread_Next;
		void *RealArg = (void *)((ThreadStub *)lpArg)->WaitingThread_Sibling;
		TlsSetValue((DWORD)_ThreadMgrTLS, lpArg);
#elif defined LIBENV_OS_LINUX
	void *ThreadPrepareFunc(void *lpArg){
		void *(*RealEntry)(void *) = (void *(*)(void *))((ThreadStub *)lpArg)->WaitingThread_Next;
		void *RealArg = (void *)((ThreadStub *)lpArg)->WaitingThread_Sibling;
		sigset_t tmpSigMask;

		pthread_setspecific(_ThreadMgrTLS, lpArg);
		((ThreadStub *)lpArg)->hThread = pthread_self(); //since kernel 2.6.19
		sigfillset(&tmpSigMask);
		pthread_sigmask(SIG_BLOCK, &tmpSigMask, NULL); //to make sure that this thread won't block signals posted to other threads.
#endif
		((ThreadStub *)lpArg)->WaitingThread_Next = (UBINT)nullptr;
		((ThreadStub *)lpArg)->WaitingThread_Sibling = (UBINT)nullptr;
		((ThreadStub *)lpArg)->LockWaiting = (UBINT)nullptr;
		((ThreadStub *)lpArg)->FutexAddr = 0;
		((ThreadStub *)lpArg)->ObjBinLock = (UBINT)nullptr;
		((ThreadStub *)lpArg)->ExtInfo = nullptr;

		RealArg = RealEntry(RealArg);
#ifndef LIB_GENERAL_DEF_ALLOC
		MemHeap TmpHeap = ((ThreadStub *)lpArg)->ThreadHeap;
		TmpHeap.Clear();
#else
		GlobalMemFree(lpArg);
#endif

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check these lines.
		//Maybe there is a more suitable way for your platform.
#endif

#if defined LIBENV_OS_WIN
		//_endthreadex((DWORD)RealArg); //No. You don't need this line -- although MSDN wrote this in their sample code.
		return (DWORD)RealArg;
#elif defined LIBENV_OS_LINUX
		return RealArg;
#endif
	}

	extern UBINT CreateNormalThread(void *(*lpStartAddr)(void *), void *lpArg){
		unsigned int TID;

#ifndef LIB_GENERAL_DEF_ALLOC
		MemHeap NewHeap = { nullptr };
		ThreadStub *MyStub = (ThreadStub *)NewHeap.Alloc(sizeof(ThreadStub));
		if (nullptr == MyStub)return 0;
		MyStub->ThreadHeap = NewHeap;
#else
		ThreadStub *MyStub = (ThreadStub *)GlobalMemAlloc(sizeof(ThreadStub));
		if (nullptr == MyStub)return 0;
#endif

		//pass parameters implicitly
		MyStub->WaitingThread_Next = (UBINT)lpStartAddr;
		MyStub->WaitingThread_Sibling = (UBINT)lpArg;
		EnterLock(&nsBasic::ThreadMgrLock);

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check these lines.
		//Here we need to create a thread.
#endif

#if defined LIBENV_OS_WIN
		if (NULL == (MyStub->hThread = (UBINT)_beginthreadex(nullptr, 0, _ThreadPrepareFunc, (void *)MyStub, 0, &TID))){
#elif defined LIBENV_OS_LINUX
		if (!pthread_create((pthread_t *)&(MyStub->hThread), NULL, _ThreadPrepareFunc, (void *)MyStub)){
#endif

#ifndef LIB_GENERAL_DEF_ALLOC
			NewHeap.Clear();
#else
			GlobalMemFree(MyStub);
#endif
			LeaveLock(&nsBasic::ThreadMgrLock);
			return 0;
		}
		else{
			LeaveLock(&nsBasic::ThreadMgrLock);
			return MyStub->hThread;
		}
	}
	extern void *WaitThread(UBINT Handle){
#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check these lines.
		//This function wait for a thread to stop and return its returned value. It's recommended to call this before a multithreaded program exits.
#endif
		void *RetValue;
#if defined LIBENV_OS_WIN
		WaitForSingleObject((HANDLE)Handle, INFINITE);
		GetExitCodeThread((HANDLE)Handle, (LPDWORD)&RetValue);
		CloseHandle((HANDLE)Handle);
#elif defined LIBENV_OS_LINUX
		pthread_join((pthread_t)(MyStub->hThread), &RetValue);
#endif
		return RetValue;
	}
}

#endif