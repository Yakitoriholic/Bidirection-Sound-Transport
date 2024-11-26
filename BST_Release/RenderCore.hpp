#ifndef LIB_RENDER
#define LIB_RENDER

#include "lContainer.hpp"
#include "lGUI.hpp"
#include "RenderScheduler.hpp"

namespace nsRender{
	struct VPMatrix{
		float MatLocal[16];
		float MatProj[16];
	};

	class RenderCore{
	private:
		RenderCore() = delete; //using the default constructor is prohibited
		RenderCore(const RenderCore &) = delete; //not copyable
		RenderCore & operator =(const RenderCore &) = delete; //not copyable

		nsContainer::HashSet<nsBasic::IBase *> ExprSet;
	public:
		nsGUI::GLDevice *CurDevice;
		RenderScheduler Scheduler;

		RenderCore(nsGUI::GLDevice *lpDevice);
		void RenderExpr_Initialize(nsBasic::IBase *lpExpr);
		void RenderExpr_Finalize(nsBasic::IBase *lpExpr);
		~RenderCore();
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	RenderCore::RenderCore(nsGUI::GLDevice *lpDevice) :Scheduler(), ExprSet(){
		this->CurDevice = lpDevice;
	}
	void RenderCore::RenderExpr_Initialize(nsBasic::IBase *lpExpr){
		const IRenderExpr *I = lpExpr->GetInterface<IRenderExpr>();
		if (nullptr != I){
			auto result = this->ExprSet.insert(lpExpr);
			if (result.second)I->Initialize(lpExpr, this->CurDevice);
		}
	}
	void RenderCore::RenderExpr_Finalize(nsBasic::IBase *lpExpr){
		auto Iterator = this->ExprSet.find(lpExpr);
		if (Iterator != this->ExprSet.end()){
			const IRenderExpr *I = lpExpr->GetInterface<IRenderExpr>();
			I->Finalize(lpExpr, this->CurDevice);
		}
	}
	RenderCore::~RenderCore(){
		auto It_End = this->ExprSet.end();
		auto CurIt = this->ExprSet.begin();
		while (CurIt != It_End){
			const IRenderExpr *I = (*CurIt)->GetInterface<IRenderExpr>();
			I->Finalize(*CurIt, this->CurDevice);
			CurIt++;
		}
	};
}

#endif