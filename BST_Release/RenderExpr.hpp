#ifndef LIB_RENDER_RenderExpr
#define LIB_RENDER_RenderExpr

#include "glShaders.hpp"
#include "ResMgr_Static.hpp"
#include "ResMgr_Dynamic.hpp"

namespace nsRender{
	struct RenderExpr{
		UBINT ExprType;
		UBINT OutputType;
		void(*Initialize)(nsGUI::GLDevice *lpDevice); //initialize render status
		void(*Finalize)(nsGUI::GLDevice *lpDevice); //finalize render status
		void(*Execute)(nsGUI::GLDevice *lpDevice, RenderBufferInfo *Output, RenderBufferInfo **ArgList, UBINT ArgCount);
		union{
			nsBasic::IBase *VarPtr;
			void *Ptr;
		};

		enum FunctionTypeEnum{
			CONST_VARIABLE,
			VARIABLE,
			CONST_FUNCTION,
			FUNCTION,
			RANGE
		};
	};

	struct IRenderExpr{
		void(*Initialize)(nsBasic::IBase * const, nsGUI::GLDevice *); //initialize the render expression itself (compile shader programs, etc)
		void(*Finalize)(nsBasic::IBase * const, nsGUI::GLDevice *);
		const nsRender::RenderExpr *lpExpr;
	};
}

class DepthBlendProgram{
private:
	typedef DepthBlendProgram this_type;

	DepthBlendProgram() = delete;
	~DepthBlendProgram() = delete;

	static GLuint ProgramID;
public:

	//implementation of interfaces
	//IBase
	static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Src, void *Data){ return true; }

	//IRenderExpr
	static void InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		if (0 == DepthBlendProgram::ProgramID)DepthBlendProgram::ProgramID = lpDevice->CreateShaderProgram(DepthBlendShader_VS, DepthBlendShader_FS);

		GLuint TexPos;
		glUseProgram(DepthBlendProgram::ProgramID);

		char VarName[] = "tex ";
		for (UBINT i = 0; i < 2; i++){
			VarName[3] = i + '0';
			TexPos = glGetUniformLocation(DepthBlendProgram::ProgramID, VarName);
			glUniform1i(TexPos, i);
		}
	}
	static void FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		glDeleteProgram(DepthBlendProgram::ProgramID);
		DepthBlendProgram::ProgramID = 0;
	}

	static void Initialize(nsGUI::GLDevice *lpDevice){
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		
		//the lines below may cover the information of bounded g-buffer.
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClearDepth(1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	static void Finalize(nsGUI::GLDevice *lpDevice){
		glDisable(GL_BLEND);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
	}

	static const nsBasic::IBase IBase_Inst;
	static const nsRender::RenderExpr Expr;

protected:
	static const nsRender::IRenderExpr IRenderExpr_Inst;

	static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
		if (IID == __typeid(nsRender::IRenderExpr))return &this_type::IRenderExpr_Inst;
		else return nullptr;
	}
};
GLuint DepthBlendProgram::ProgramID = 0;

const nsBasic::IBase DepthBlendProgram::IBase_Inst{
	DepthBlendProgram::_GetInterface,
	nullptr,
	DepthBlendProgram::on_Link_Twrd,
	nullptr,
	nullptr,
	(UBINT)-1, //reference counter is disabled by default
	0,
	nullptr
};
const nsRender::IRenderExpr DepthBlendProgram::IRenderExpr_Inst{
	DepthBlendProgram::InitializeProgram,
	DepthBlendProgram::FinalizeProgram,
	&DepthBlendProgram::Expr
};
const nsRender::RenderExpr DepthBlendProgram::Expr = {
	/*ExprType*/	nsRender::RenderExpr::RANGE,
	/*OutputType*/	__typeid(nsRender::Buffer_OpaquePhong),
	/*Initialize*/	DepthBlendProgram::Initialize,
	/*Finalize*/	DepthBlendProgram::Finalize,
	/*Execute*/		nullptr,
	/*VarPtr*/		nullptr
};

class ScrOutputProgram{
private:
	typedef ScrOutputProgram this_type;

	ScrOutputProgram() = delete;
	~ScrOutputProgram() = delete;

	static GLuint ProgramID;
public:

	//implementation of interfaces
	//IBase
	static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Src, void *Data){ return true; }

	//IRenderExpr
	static void InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		if (0 == ScrOutputProgram::ProgramID)ScrOutputProgram::ProgramID = lpDevice->CreateShaderProgram(DepthBlendShader_VS, DepthBlendShader_FS);

		GLuint TexPos;
		glUseProgram(ScrOutputProgram::ProgramID);

		char VarName[] = "tex ";
		for (UBINT i = 0; i < 2; i++){
			VarName[3] = i + '0';
			TexPos = glGetUniformLocation(ScrOutputProgram::ProgramID, VarName);
			glUniform1i(TexPos, i);
		}
	}
	static void FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		glDeleteProgram(ScrOutputProgram::ProgramID);
		ScrOutputProgram::ProgramID = 0;
	}

	static void Initialize(nsGUI::GLDevice *lpDevice){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearDepth(1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		nsRender::CurOutputBuffer = nullptr;
	}
	static void Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount){
		auto DrawSqr = [&](){
			const GLfloat Point[8] = {
				-1.0f, -1.0f,
				1.0f, -1.0f,
				1.0f, 1.0f,
				-1.0f, 1.0f
			};

			GLuint VtxHandle = lpDevice->CreateArrayBuffer(8 * sizeof(GLfloat), GL_STATIC_DRAW, Point);
			glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, 0);

			glDrawArrays(GL_QUADS, 0, 4);

			lpDevice->DestroyArrayBuffer(VtxHandle, 8 * sizeof(GLfloat), GL_STATIC_DRAW);
		};

		//buggy
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glUseProgram(ScrOutputProgram::ProgramID);

		const nsRender::IBuffer *I;
		GLuint *MapID;

		I = ArgList[0]->Buffer->GetInterface<nsRender::IBuffer>();
		MapID = (GLuint *)I->GetValue(ArgList[0]->Buffer, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, MapID[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, MapID[1]);

		DrawSqr();
	}
	static void Finalize(nsGUI::GLDevice *lpDevice){
	}

	static const nsBasic::IBase IBase_Inst;
	static const nsRender::RenderExpr Expr;

protected:
	static const nsRender::IRenderExpr IRenderExpr_Inst;

	static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
		if (IID == __typeid(nsRender::IRenderExpr))return &this_type::IRenderExpr_Inst;
		else return nullptr;
	}
};
GLuint ScrOutputProgram::ProgramID = 0;

const nsBasic::IBase ScrOutputProgram::IBase_Inst{
	ScrOutputProgram::_GetInterface,
	nullptr,
	ScrOutputProgram::on_Link_Twrd,
	nullptr,
	nullptr,
	(UBINT)-1, //reference counter is disabled by default
	0,
	nullptr
};
const nsRender::IRenderExpr ScrOutputProgram::IRenderExpr_Inst{
	ScrOutputProgram::InitializeProgram,
	ScrOutputProgram::FinalizeProgram,
	&ScrOutputProgram::Expr
};
const nsRender::RenderExpr ScrOutputProgram::Expr = {
	/*ExprType*/	nsRender::RenderExpr::RANGE,
	/*OutputType*/	NULL,
	/*Initialize*/	ScrOutputProgram::Initialize,
	/*Finalize*/	ScrOutputProgram::Finalize,
	/*Execute*/		ScrOutputProgram::Execute,
	/*VarPtr*/		nullptr
};

nsRender::RenderExpr FloatMask_Filter = {
	/*ExprType*/	nsRender::RenderExpr::CONST_FUNCTION,
	/*OutputType*/	__typeid(nsRender::Buffer_FloatMask),
	/*Initialize*/	nullptr,
	/*Finalize*/	nullptr,
	/*Execute*/		nullptr,
	/*VarPtr*/		nullptr
};

#endif