#ifndef LIB_RENDER_RenderScheduler
#define LIB_RENDER_RenderScheduler

#include "lGeneral.hpp"
#include "lContainer.hpp"
#include "RenderExpr.hpp"
#include "ResMgr_Dynamic.hpp"

#include <stdio.h>
#include <conio.h>

namespace nsRender{
	class RenderScheduler{
	private:
		struct ExprInfo{
			UBINT ID;
			UBINT ParamCount; //outgoing edge count
			UBINT RefCount;
			UBINT TotalRefCount; //incident edge count
		};

		UBINT CurExprPtr, NextExprPtr;
		nsContainer::List<ExprInfo> UnRefedExprList;
		nsContainer::List<ExprInfo> RefedExprList;
		nsContainer::Vector<UBINT> Program;
		nsContainer::Vector<UBINT> ExeList;

		enum Instruction{
			CREATE_BUFFER, //ProgID
			DESTROY_BUFFER, //ProgID
			INITIALIZE, //Prog
			FINALIZE, //Prog
			EXECUTE, //Prog Output_ID Params_ID ParamCount
		};

		void DerefParam(UBINT ParamID){
			nsContainer::List<ExprInfo>::iterator ParamExprInfo((ExprInfo *)this->Program[ParamID]);
			RenderExpr *ParamExpr = (RenderExpr *)this->Program[ParamID + 1];

			if (NULL == this->Program[ParamID] || 0 == ParamExprInfo->RefCount)throw std::exception("Cannot compile the current program. It is corrupted."); //a ring exists
			if (-1 == ParamExprInfo->TotalRefCount){
				ParamExprInfo->TotalRefCount = ParamExprInfo->RefCount;
				if (RenderExpr::CONST_VARIABLE != ParamExpr->ExprType){
					this->ExeList.push_back(ParamID);
					this->ExeList.push_back(Instruction::DESTROY_BUFFER);
				}
			}
			ParamExprInfo->RefCount--;
			if (0 == ParamExprInfo->RefCount)this->UnRefedExprList.splice(this->UnRefedExprList.cbegin(), this->RefedExprList, ParamExprInfo);
		}
		void DeleteParam(UBINT ParamID){
			nsContainer::List<ExprInfo>::iterator ParamExprInfo((ExprInfo *)this->Program[ParamID]);
			RenderExpr *ParamExpr = (RenderExpr *)this->Program[ParamID + 1];

			if (NULL == this->Program[ParamID] || 0 == ParamExprInfo->RefCount)throw std::exception("Cannot compile the current program. It is corrupted."); //a ring exists
			if (-1 == ParamExprInfo->TotalRefCount)ParamExprInfo->TotalRefCount = ParamExprInfo->RefCount;
			ParamExprInfo->RefCount--;

			if (0 == ParamExprInfo->RefCount)this->UnRefedExprList.splice(this->UnRefedExprList.cend(), this->RefedExprList, ParamExprInfo);
		}
		void DerefParamInRange(ExprInfo *RangeExprInfo, UBINT *lpParamID){
			RenderExpr *RangeExpr = (RenderExpr *)this->Program[RangeExprInfo->ID + 1];
			nsContainer::List<ExprInfo>::iterator ParamExprInfo((ExprInfo *)this->Program[*lpParamID]);
			RenderExpr *ParamExpr = (RenderExpr *)this->Program[*lpParamID + 1];

			if (NULL == this->Program[*lpParamID] || 0 == ParamExprInfo->RefCount)throw std::exception("Cannot compile the current program. It is corrupted."); //a ring exists
			if (RangeExpr->OutputType != ParamExpr->OutputType && NULL != RangeExpr->OutputType)throw std::exception("Cannot compile the current program. If a range expression can output, the output type must match the output type of all its parameter expression.");

			//decrease reference count
			if (-1 == ParamExprInfo->TotalRefCount)ParamExprInfo->TotalRefCount = ParamExprInfo->RefCount;
			ParamExprInfo->RefCount--;

			if (1 == ParamExprInfo->TotalRefCount){
				//can be optimized
				if (ParamExpr == RangeExpr){
					//recursively dereference parameter expressions
					UBINT *ParamPtr = &this->Program[*lpParamID + 4];
					for (UBINT i = 0; i < ParamExprInfo->ParamCount; i++)DerefParamInRange(RangeExprInfo, &ParamPtr[i]);
				}
				else if (RenderExpr::FUNCTION == ParamExpr->ExprType || RenderExpr::CONST_FUNCTION == ParamExpr->ExprType){
					//dereference all parameter expressions
					UBINT *InlineParamPtr = &this->Program[ParamExprInfo->ID + 4];
					for (UBINT i = 0; i < ParamExprInfo->ParamCount; i++)DerefParam(InlineParamPtr[i]);

					UBINT ExeVal[5] = { ParamExprInfo->ParamCount, ParamExprInfo->ID + 4, RangeExprInfo->ID, (UBINT)ParamExpr, Instruction::EXECUTE };
					if (NULL == RangeExpr->OutputType)ExeVal[2] = (UBINT)-1;
					this->ExeList.insert(this->ExeList.end(), ExeVal, ExeVal + 5);
					this->RefedExprList.erase(ParamExprInfo);
				}
				else{
					if (RenderExpr::CONST_VARIABLE != ParamExpr->ExprType){
						this->ExeList.push_back(*lpParamID);
						this->ExeList.push_back(Instruction::DESTROY_BUFFER);
					}
					this->UnRefedExprList.splice(this->UnRefedExprList.cbegin(), this->RefedExprList, ParamExprInfo);
					UBINT ExeVal[5] = { 1, lpParamID - this->Program.begin(), RangeExprInfo->ID, (UBINT)RangeExpr, Instruction::EXECUTE };
					this->ExeList.insert(this->ExeList.end(), ExeVal, ExeVal + 5);
				}
			}
			else{
				if (ParamExprInfo->TotalRefCount == ParamExprInfo->RefCount + 1){
					if (RenderExpr::CONST_VARIABLE != ParamExpr->ExprType){
						this->ExeList.push_back(*lpParamID);
						this->ExeList.push_back(Instruction::DESTROY_BUFFER);
					}
					this->UnRefedExprList.splice(this->UnRefedExprList.cbegin(), this->RefedExprList, ParamExprInfo);
				}
				UBINT ExeVal[5] = { 1, lpParamID - this->Program.begin(), RangeExprInfo->ID, (UBINT)RangeExpr, Instruction::EXECUTE };
				this->ExeList.insert(this->ExeList.end(), ExeVal, ExeVal + 5);
			}
		}
		void DeleteParamInRange(ExprInfo *RangeExprInfo, UBINT *lpParamID){
			RenderExpr *RangeExpr = (RenderExpr *)this->Program[RangeExprInfo->ID + 1];
			nsContainer::List<ExprInfo>::iterator ParamExprInfo((ExprInfo *)this->Program[*lpParamID]);
			RenderExpr *ParamExpr = (RenderExpr *)this->Program[*lpParamID + 1];

			if (NULL == this->Program[*lpParamID] || 0 == ParamExprInfo->RefCount)throw std::exception("Cannot compile the current program. It is corrupted."); //a ring exists
			if (RangeExpr->OutputType != ParamExpr->OutputType && NULL != RangeExpr->OutputType)throw std::exception("Cannot compile the current program. If a range expression can output, the output type must match the output type of all its parameter expression.");

			//decrease reference count
			if (-1 == ParamExprInfo->TotalRefCount)ParamExprInfo->TotalRefCount = ParamExprInfo->RefCount;
			ParamExprInfo->RefCount--;

			if (0 == ParamExprInfo->RefCount)this->UnRefedExprList.splice(this->UnRefedExprList.cend(), this->RefedExprList, ParamExprInfo);
		}
	public:
		RenderScheduler() :Program(), ExeList(), UnRefedExprList(), RefedExprList(){
			CurExprPtr = (UBINT)-1; //no expr exist
			NextExprPtr = 0;
		}
		UBINT CreateVar(nsBasic::IBase *Value, UBINT OutputType, UBINT OutputWidth, UBINT OutputHeight){
			RenderExpr *NewExpr = new RenderExpr;
			NewExpr->ExprType = RenderExpr::VARIABLE;
			NewExpr->OutputType = OutputType;
			NewExpr->VarPtr = Value;

			UnRefedExprList.emplace_front();
			ExprInfo *NewExprInfo = &UnRefedExprList.front();
			NewExprInfo->ID = this->NextExprPtr;
			NewExprInfo->ParamCount = 0;
			NewExprInfo->RefCount = 0;
			NewExprInfo->TotalRefCount = (UBINT)-1;

			UBINT ExprVal[4] = { (UBINT)NewExprInfo, (UBINT)NewExpr, OutputWidth, OutputHeight };
			this->Program.insert(this->Program.end(), ExprVal, ExprVal + 4);

			this->CurExprPtr = this->NextExprPtr;
			this->NextExprPtr += 4;
			return this->CurExprPtr;
		}
		UBINT CreateConstVar(nsBasic::IBase *Value, UBINT OutputType, UBINT OutputWidth, UBINT OutputHeight){
			RenderExpr *NewExpr = new RenderExpr;
			NewExpr->ExprType = RenderExpr::CONST_VARIABLE;
			NewExpr->OutputType = OutputType;
			NewExpr->VarPtr = Value;

			UnRefedExprList.emplace_front();
			ExprInfo *NewExprInfo = &UnRefedExprList.front();
			NewExprInfo->ID = this->NextExprPtr;
			NewExprInfo->ParamCount = 0;
			NewExprInfo->RefCount = 0;
			NewExprInfo->TotalRefCount = (UBINT)-1;

			UBINT ExprVal[4] = { (UBINT)NewExprInfo, (UBINT)NewExpr, OutputWidth, OutputHeight };
			this->Program.insert(this->Program.end(), ExprVal, ExprVal + 4);

			this->CurExprPtr = this->NextExprPtr;
			this->NextExprPtr += 4;
			return this->CurExprPtr;
		}
		UBINT _CreateVar_Ptr(void **lpPtr, UBINT size){
			*lpPtr = nsBasic::GlobalMemAlloc(size);
			if (nullptr == *lpPtr)return (UBINT)-1;
			else{
				RenderExpr *NewExpr = new RenderExpr;
				NewExpr->ExprType = RenderExpr::VARIABLE;
				NewExpr->OutputType = __typeid(void *);
				NewExpr->Ptr = *lpPtr;

				UnRefedExprList.emplace_front();
				ExprInfo *NewExprInfo = &UnRefedExprList.front();
				NewExprInfo->ID = this->NextExprPtr;
				NewExprInfo->ParamCount = 0;
				NewExprInfo->RefCount = 0;
				NewExprInfo->TotalRefCount = (UBINT)-1;

				UBINT ExprVal[4] = { (UBINT)NewExprInfo, (UBINT)NewExpr, size, 1 };
				this->Program.insert(this->Program.end(), ExprVal, ExprVal + 4);

				this->CurExprPtr = this->NextExprPtr;
				this->NextExprPtr += 4;
				return this->CurExprPtr;
			}
		}
		template <typename T> inline UBINT CreateVar_Ptr(T **lpPtr){ return this->_CreateVar_Ptr((void **)lpPtr, sizeof(T)); }
		UBINT CreateConstVar_Ptr(void *lpData){
			RenderExpr *NewExpr = new RenderExpr;
			NewExpr->ExprType = RenderExpr::CONST_VARIABLE;
			NewExpr->OutputType = __typeid(void *);
			NewExpr->Ptr = lpData;

			UnRefedExprList.emplace_front();
			ExprInfo *NewExprInfo = &UnRefedExprList.front();
			NewExprInfo->ID = this->NextExprPtr;
			NewExprInfo->ParamCount = 0;
			NewExprInfo->RefCount = 0;
			NewExprInfo->TotalRefCount = (UBINT)-1;

			UBINT ExprVal[4] = { (UBINT)NewExprInfo, (UBINT)NewExpr, 0, 0 };
			this->Program.insert(this->Program.end(), ExprVal, ExprVal + 4);

			this->CurExprPtr = this->NextExprPtr;
			this->NextExprPtr += 4;
			return this->CurExprPtr;
		}
		UBINT CreateFunc(const RenderExpr *Func, UBINT OutputWidth, UBINT OutputHeight){
			UnRefedExprList.emplace_front();
			ExprInfo *NewExprInfo = &UnRefedExprList.front();
			NewExprInfo->ID = this->NextExprPtr;
			NewExprInfo->ParamCount = 0;
			NewExprInfo->RefCount = 0;
			NewExprInfo->TotalRefCount = (UBINT)-1;

			UBINT ExprVal[4] = { (UBINT)NewExprInfo, (UBINT)Func, OutputWidth, OutputHeight };
			this->Program.insert(this->Program.end(), ExprVal, ExprVal + 4);

			this->CurExprPtr = this->NextExprPtr;
			this->NextExprPtr += 4;
			return this->CurExprPtr;
		}
		void PushParamExpr(UBINT ExprID){
			//CreateExpr must be called in previous.
			Program.push_back(ExprID);
			this->NextExprPtr++;

			ExprInfo *TmpExprInfo = &this->UnRefedExprList.front(); //the previous function
			TmpExprInfo->ParamCount++;

			//add ref counter to the refed variable
			TmpExprInfo = (ExprInfo *)(this->Program[ExprID]);
			if (0 == TmpExprInfo->RefCount)this->RefedExprList.splice(this->RefedExprList.cbegin(), this->RefedExprList, nsContainer::List<ExprInfo>::iterator(TmpExprInfo));
			TmpExprInfo->RefCount++;
		}
		inline UBINT GetWidth(UBINT ExprID){ return this->Program[ExprID + 2]; }
		inline UBINT GetHeight(UBINT ExprID){ return this->Program[ExprID + 3]; }
		void Compile(){
			//eliminate all the unused variables
			if (!this->UnRefedExprList.empty()){
				auto _It = this->UnRefedExprList.begin();
				do{
					RenderExpr *CurExpr = (RenderExpr *)this->Program[_It->ID + 1];
					auto _It_Prev = _It++;
					if ((0 == _It_Prev->TotalRefCount || (UBINT)-1 == _It_Prev->TotalRefCount) && NULL != CurExpr->OutputType){
						if (RenderExpr::VARIABLE == CurExpr->ExprType || RenderExpr::CONST_VARIABLE == CurExpr->ExprType){
							if (_It_Prev->ParamCount > 0)throw std::exception("Cannot compile the current program. A variable should never refer to other expressions.");
						}
						else if (RenderExpr::FUNCTION == CurExpr->ExprType || RenderExpr::CONST_FUNCTION == CurExpr->ExprType){
							UBINT *ParamPtr = &this->Program[_It_Prev->ID + 4];
							for (UBINT i = 0; i < _It_Prev->ParamCount; i++)DeleteParam(ParamPtr[i]);
						}
						else if (RenderExpr::RANGE == CurExpr->ExprType){
							UBINT *ParamPtr = &this->Program[_It_Prev->ID + 4];
							for (UBINT i = 0; i < _It_Prev->ParamCount; i++)DeleteParamInRange(&(*_It_Prev), &ParamPtr[i]);
						}
						this->UnRefedExprList.erase(_It_Prev);
					}
				} while (_It != this->UnRefedExprList.end());
			}

			if (!this->UnRefedExprList.empty()){
				do{
					ExprInfo *CurExprInfo = &this->UnRefedExprList.front();
					RenderExpr *CurExpr = (RenderExpr *)this->Program[CurExprInfo->ID + 1];

					if (RenderExpr::VARIABLE == CurExpr->ExprType || RenderExpr::CONST_VARIABLE == CurExpr->ExprType){
						if (CurExprInfo->ParamCount > 0)throw std::exception("Cannot compile the current program. A variable should never refer to other expressions.");
						if (NULL != CurExpr->OutputType){
							this->ExeList.push_back(CurExprInfo->ID);
							this->ExeList.push_back(Instruction::CREATE_BUFFER);
						}
					}
					else if (RenderExpr::FUNCTION == CurExpr->ExprType || RenderExpr::CONST_FUNCTION == CurExpr->ExprType){
						UBINT *ParamPtr = &this->Program[CurExprInfo->ID + 4];
						for (UBINT i = 0; i < CurExprInfo->ParamCount; i++)DerefParam(ParamPtr[i]);
						if (NULL == CurExpr->OutputType){
							UBINT ExeVal[5] = { CurExprInfo->ParamCount, CurExprInfo->ID + 4, (UBINT)-1, (UBINT)CurExpr, Instruction::EXECUTE };
							this->ExeList.insert(this->ExeList.end(), ExeVal, ExeVal + 5);
						}
						else{
							UBINT ExeVal[7] = { CurExprInfo->ParamCount, CurExprInfo->ID + 4, CurExprInfo->ID, (UBINT)CurExpr, Instruction::EXECUTE, CurExprInfo->ID, Instruction::CREATE_BUFFER };
							this->ExeList.insert(this->ExeList.end(), ExeVal, ExeVal + 7);
						}
					}
					else if (RenderExpr::RANGE == CurExpr->ExprType){
						this->ExeList.push_back((UBINT)CurExpr);
						this->ExeList.push_back(Instruction::FINALIZE);
						UBINT *ParamPtr = &this->Program[CurExprInfo->ID + 4];
						for (UBINT i = 0; i < CurExprInfo->ParamCount; i++)DerefParamInRange(CurExprInfo, &ParamPtr[i]);
						this->ExeList.push_back((UBINT)CurExpr);
						this->ExeList.push_back(Instruction::INITIALIZE);
						if (NULL != CurExpr->OutputType){
							this->ExeList.push_back(CurExprInfo->ID);
							this->ExeList.push_back(Instruction::CREATE_BUFFER);
						}
					}

					this->Program[CurExprInfo->ID] = NULL;
					
					this->UnRefedExprList.erase(nsContainer::List<ExprInfo>::iterator(CurExprInfo));
				} while (!this->UnRefedExprList.empty());
			}
		}
		bool CanExecute(){ return !this->ExeList.empty(); }
		void Print(){
			UBINT *IP = this->ExeList.end();
			UBINT *IP_End = this->ExeList.begin();

			RenderExpr *CurExpr;
			RenderBufferInfo *CurBufferInfo, **ParamList;
			UBINT TmpInt, ParamCount, *IDList;

			printf("main:\n");
			while (IP != IP_End){
				IP--;
				switch (*IP){
				case Instruction::CREATE_BUFFER:
					IP--;
					CurBufferInfo = (RenderBufferInfo *)&this->Program[*IP];
					CurExpr = (RenderExpr *)this->Program[*IP + 1];
					if (RenderExpr::VARIABLE == CurExpr->ExprType){
						printf("var 0x%X as type:0x%X = %p;\n", *IP, CurExpr->OutputType, CurExpr->VarPtr);
					}
					else if(RenderExpr::CONST_VARIABLE == CurExpr->ExprType){
						printf("const var 0x%X as type:0x%X = %p;\n", *IP, CurExpr->OutputType, CurExpr->VarPtr);
					}
					else{
						printf("var 0x%X as type:0x%X size:%d * %d;\n", *IP, CurExpr->OutputType, CurBufferInfo->BufferWidth, CurBufferInfo->BufferHeight);
					}
					break;
				case Instruction::DESTROY_BUFFER:
					IP--;
					printf("delete 0x%X;\n", *IP);
					break;
				case Instruction::INITIALIZE:
					IP--;
					printf("initialize 0x%X;\n", *IP);
					break;
				case Instruction::FINALIZE:
					IP--;
					printf("finalize 0x%X;\n", *IP);
					break;
				case Instruction::EXECUTE:
					IP--;
					TmpInt = *IP;
					CurExpr = (RenderExpr *)*IP;
					IP--;
					if ((UBINT)-1 == *IP)printf("0x%X(", TmpInt); else printf("0x%X = 0x%X(", *IP, TmpInt);
					CurBufferInfo = (RenderBufferInfo *)*IP;
					IP--;
					IDList = &this->Program[*IP];
					ParamList = (RenderBufferInfo **)IDList;
					IP--;
					ParamCount = *IP;
					for (UBINT i = 0; i < ParamCount; i++){
						if (0 == i)printf("0x%X", IDList[i]); else printf(", 0x%X", IDList[i]);
					};
					printf(");\n", *IP);
					break;
				}
			}
		}
		void Execute(nsGUI::GLDevice *lpDevice){
			UBINT *IP = this->ExeList.end();
			UBINT *IP_End = this->ExeList.begin();

			RenderExpr *CurExpr;
			RenderBufferInfo *CurBufferInfo, **ParamList;
			UBINT ParamCount, *IDList;
			while (IP != IP_End){
				IP--;
				switch (*IP){
				case Instruction::CREATE_BUFFER:
					IP--;
					CurBufferInfo = (RenderBufferInfo *)&this->Program[*IP];
					CurExpr = (RenderExpr *)this->Program[*IP + 1];
					if (RenderExpr::VARIABLE == CurExpr->ExprType || RenderExpr::CONST_VARIABLE == CurExpr->ExprType){
						CurBufferInfo->Buffer = CurExpr->VarPtr;
						CurBufferInfo->BufferType = CurExpr->OutputType;
						delete CurExpr;
					}
					else{
						CurBufferInfo->Buffer = CreateRenderBuffer(lpDevice, CurExpr->OutputType, CurBufferInfo->BufferWidth, CurBufferInfo->BufferHeight);
						CurBufferInfo->BufferType = CurExpr->OutputType;
					}
					break;
				case Instruction::DESTROY_BUFFER:
					IP--;
					CurBufferInfo = (RenderBufferInfo *)&this->Program[*IP];
					if (CurBufferInfo->BufferType != NULL){
						if (__typeid(void *) == CurBufferInfo->BufferType)nsBasic::GlobalMemFree(CurBufferInfo->Pointer, CurBufferInfo->BufferWidth);
						else CurBufferInfo->Buffer->Destroy(CurBufferInfo->Buffer);
					}
					break;
				case Instruction::INITIALIZE:
					IP--;
					CurExpr = (RenderExpr *)*IP;
					CurExpr->Initialize(lpDevice);
					break;
				case Instruction::FINALIZE:
					IP--;
					CurExpr = (RenderExpr *)*IP;
					CurExpr->Finalize(lpDevice);
					break;
				case Instruction::EXECUTE:
					IP--;
					CurExpr = (RenderExpr *)*IP;
					IP--;
					if ((UBINT)-1 == *IP)CurBufferInfo = nullptr;else CurBufferInfo = (RenderBufferInfo *)&this->Program[*IP];
					IP--;
					IDList = &this->Program[*IP];
					ParamList = (RenderBufferInfo **)IDList;
					IP--;
					ParamCount = *IP;
					for (UBINT i = 0; i < ParamCount; i++)ParamList[i] = (RenderBufferInfo *)&this->Program[IDList[i]];
					CurExpr->Execute(lpDevice, CurBufferInfo, ParamList, ParamCount);
					if (RenderExpr::FUNCTION == CurExpr->ExprType)delete CurExpr;
					break;
				}
			}
		}
		void Clear(){
			this->RefedExprList.clear();
			this->UnRefedExprList.clear();
			Program.clear();
			ExeList.clear();
			this->CurExprPtr = (UBINT)-1; //no expr exist
			this->NextExprPtr = 0;
		}
	};
}
#endif