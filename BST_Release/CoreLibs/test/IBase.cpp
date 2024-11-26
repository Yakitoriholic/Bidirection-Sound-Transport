#include "lGeneral.hpp"
#include <cstdio>

#include <stdio.h>
#if defined LIBENV_OS_LINUX
#include <curses.h>
#else
#include <conio.h>
#endif

//This test also serves as an demonstration of the usage of interface IBase.

class TestClassA:public nsBasic::IBase{ //public inherit from IBase
private:
	typedef TestClassA this_type;

	//using new and delete operator directly is prohibited
	inline void *operator new(size_t size){ return ::operator new(size); }
	inline void operator delete(void *ptr){ ::operator delete(ptr); };

public:
	//definition of new interfaces
	struct ITestClassA{
		void(*PrintChild)(nsBasic::IBase * const);
	};

protected:
	//protected data, used by this class
	nsBasic::IBase *Ptr_From[4];

public:
	//implementation of interfaces
	static bool on_Link_From(nsBasic::IBase * const ThisBase, IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		UBINT i;
		for (i = 0; i < 4; i++){
			if (nullptr == This->Ptr_From[i]){
				This->Ptr_From[i] = Dest;
				break;
			}
		}
		if (i < 4)return true; else return false;
	}
	static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }
	static void on_UnLink_From(nsBasic::IBase * const ThisBase, IBase * const Dest, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		for (UBINT i = 0; i < 4; i++){
			if (Dest == This->Ptr_From[i])This->Ptr_From[i] = nullptr;
		}
	}

	static void PrintChild(nsBasic::IBase * const ThisBase){
		this_type *This = static_cast<this_type *>(ThisBase);
		printf("Child of TestClassA %p:", ThisBase);
		for (UBINT i = 0; i < 4; i++){
			if (nullptr != This->Ptr_From[i])printf("%p ", This->Ptr_From[i]);
		}
		printf("\n");
	}

protected:
	//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
	static const ITestClassA ITestClassA_Inst;

	static const void *GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
		if (IID == __typeid(TestClassA::ITestClassA))return &TestClassA::ITestClassA_Inst;
		else return nullptr;
	}
public:
	static const nsBasic::IBase IBase_Inst;
	//constructors
	TestClassA(){
		*static_cast<nsBasic::IBase * const>(this) = TestClassA::IBase_Inst;
		for (UBINT i = 0; i < 4; i++)this->Ptr_From[i] = nullptr;
	}
	static TestClassA * const Create(){ //equivalent to the new operator
		this_type *RetValue = new this_type();
		RetValue->RefCounter = 0; //enable the reference counter
		return RetValue;
	}

	//destructors
	static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
		this_type *This = static_cast<this_type *>(ThisBase);
		delete This;
	}
	~TestClassA(){
		for (UBINT i = 0; i < 4; i++){
			if (nullptr != this->Ptr_From[i] && nullptr != this->Ptr_From[i]->on_UnLink_Twrd){
				this->Ptr_From[i]->on_UnLink_Twrd(this->Ptr_From[i], this, nullptr);
				//manual deletion
				this->Ptr_From[i]->ref_dec();
				this->Ptr_From[i]->Destroy(this->Ptr_From[i]);
			}
		}
	}
};

const nsBasic::IBase TestClassA::IBase_Inst = {
	TestClassA::GetInterface,
	TestClassA::on_Link_From,
	TestClassA::on_Link_Twrd,
	TestClassA::on_UnLink_From,
	nullptr,
	(UBINT)-1, //reference counter is disabled by default
	0,
	TestClassA::Destroy
};
const TestClassA::ITestClassA TestClassA::ITestClassA_Inst = { TestClassA ::PrintChild };

//end of classA

class TestClassB :public TestClassA{ //public inherit
private:
	typedef TestClassB this_type;

	//new and delete operator should always be redefined
	inline void *operator new(size_t size){ return ::operator new(size); }
	inline void operator delete(void *ptr){ ::operator delete(ptr); };
public:
	//definition of new interfaces
	struct ITestClassB{
		void(*PrintParent)(nsBasic::IBase * const);

		static const UBINT IID = 1;
	};
	

protected:
	//protected data, used by this class
	nsBasic::IBase *Ptr_Twrd[4];

public:
	//implementation of interfaces
	static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		UBINT i;
		for (i = 0; i < 4; i++){
			if (nullptr == This->Ptr_Twrd[i]){
				This->Ptr_Twrd[i] = Src;
				break;
			}
		}
		if (i < 4)return true; else return false;
	}
	static void on_UnLink_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		for (UBINT i = 0; i < 4; i++){
			if (Src == This->Ptr_Twrd[i])This->Ptr_Twrd[i] = nullptr;
		}
	}

	static void PrintParent(nsBasic::IBase * const ThisBase){
		this_type *This = static_cast<this_type *>(ThisBase);
		printf("Parent of TestClassA %p:", ThisBase);
		for (UBINT i = 0; i < 4; i++){
			if (nullptr != This->Ptr_Twrd[i])printf("%p ", This->Ptr_From[i]);
		}
		printf("\n");
	}

protected:
	//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
	static const ITestClassB ITestClassB_Inst;

	static const void *GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
		if (IID == TestClassB::ITestClassB::IID)return &TestClassB::ITestClassB_Inst;
		else return TestClassA::GetInterface(ThisBase, IID);
	}
public:
	static const nsBasic::IBase IBase_Inst;
	//constructors
	TestClassB(){
		this->nsBasic::IBase::GetInterface = this_type::GetInterface;
		this->nsBasic::IBase::on_Link_Twrd = this_type::on_Link_Twrd;
		this->nsBasic::IBase::on_UnLink_Twrd = this_type::on_UnLink_Twrd;
		this->nsBasic::IBase::GetInterface = this_type::GetInterface;
		for (UBINT i = 0; i < 4; i++)this->Ptr_Twrd[i] = nullptr;
	}
	static TestClassB * const Create(){ //equivalent to the new operator
		this_type *RetValue = new this_type();
		RetValue->RefCounter = 0; //enable the reference counter
		return RetValue;
	}

	//destructors
	static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
		this_type *This = static_cast<this_type *>(ThisBase);
		delete This;
	}
	~TestClassB(){
		for (UBINT i = 0; i < 4; i++){
			if (nullptr != this->Ptr_Twrd[i] && nullptr != this->Ptr_Twrd[i]->on_UnLink_From){
				this->Ptr_Twrd[i]->on_UnLink_From(this->Ptr_Twrd[i], this, nullptr);
				//manual deletion
				this->Ptr_Twrd[i]->ref_dec();
			}
		}
	}
};

const TestClassB::ITestClassB TestClassB::ITestClassB_Inst = { TestClassB::PrintParent };

extern void Test_IBase(){
	printf("Press any key to start testing.\n");
	_getch();

	/*TestClassA *A1 = TestClassA::Create();
	TestClassA A2, A3, A4;
	TestClassA *A5 = TestClassA::Create(), *A6 = TestClassA::Create();

	TestClassA::PrintChild(A1);

	nsBasic::IBase::Link(A1, &A2, nullptr);
	nsBasic::IBase::Link(A1, &A3, nullptr);
	nsBasic::IBase::Link(A1, A5, nullptr);
	nsBasic::IBase::Link(A1, A6, nullptr);
	nsBasic::IBase::Link(A1, &A4, nullptr);

	TestClassA::PrintChild(A1);

	nsBasic::IBase::UnLink(A1, A6, nullptr);

	TestClassA::PrintChild(A1);
	TestClassB B1;

	TestClassA::Destroy(A1);*/

	_getch();
}