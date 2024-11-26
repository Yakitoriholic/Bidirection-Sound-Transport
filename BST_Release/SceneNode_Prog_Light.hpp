#include "SceneNode_General.hpp"

namespace nsScene{
	struct VPLightProperty{
		UINT4b Status;
		float Intensity[3];

		enum StatusFlag{
			INTENSITY_MAP_EXIST = 1
		};
	};

	struct AmbientLightProperty{
		UINT4b Status;
		float Intensity[3];

		enum StatusFlag{
			AO_MASK_EXIST = 1 << 0,
			INTENSITY_MAP_EXIST = 1 << 1
		};
	};

	//shadow caster

	//class ESMShadowCaster --BEGIN--

	class ESMShadowCaster{
	private:
		typedef ESMShadowCaster this_type;

		static GLuint ProgramID;
		static GLuint UBO;

		static struct _GL_Shader_UBO0{
			GLfloat ObjMat_Cam[16];
			GLfloat ProjMat_Cam[16];
			GLfloat ObjMat_Light[16];
			GLfloat ProjMat_Light[16];
			GLfloat ObjMat_Cam_Inv[16];
			GLfloat ProjMat_Cam_Inv[16];
			GLfloat ObjMat_Light_Inv[16];
			GLuint GBufferSize[2];
		} _GL_Shader_UBO0_Inst;

		ESMShadowCaster() = delete;
		~ESMShadowCaster() = delete;
	public:
		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Src, void *Data){ return true; }

		//IRenderExpr
		static void InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		
		static void Initialize();
		static void SetTransform(float *Transform_Camera, float *ProjMat_Camera, float *Transform_Light, float *ProjMat_Light);
		static void SetSize(UBINT Width, UBINT Height);
		static void LoadInputPair(nsBasic::IBase *lpGBuffer, nsBasic::IBase *lpShadowMap);
		static void PreRender();
		static void Finalize();

		static void Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount);

		static const nsBasic::IBase IBase_Inst;
		static const nsRender::RenderExpr Expr;
	protected:
		static const nsRender::IRenderExpr IRenderExpr_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(nsRender::IRenderExpr))return &this_type::IRenderExpr_Inst;
			else return nullptr;
		}
	};

	GLuint ESMShadowCaster::ProgramID = 0;
	GLuint ESMShadowCaster::UBO = 0;
	const nsBasic::IBase ESMShadowCaster::IBase_Inst{
		ESMShadowCaster::_GetInterface,
		nullptr,
		ESMShadowCaster::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		nullptr
	};
	const nsRender::IRenderExpr ESMShadowCaster::IRenderExpr_Inst{
		ESMShadowCaster::InitializeProgram,
		ESMShadowCaster::FinalizeProgram,
		&ESMShadowCaster::Expr
	};
	const nsRender::RenderExpr ESMShadowCaster::Expr = {
		/*ExprType*/	nsRender::RenderExpr::CONST_FUNCTION,
		/*OutputType*/	__typeid(nsRender::Buffer_FloatMask),
		/*Initialize*/	nullptr,
		/*Finalize*/	nullptr,
		/*Execute*/		ESMShadowCaster::Execute,
		/*VarPtr*/		nullptr
	};
	ESMShadowCaster::_GL_Shader_UBO0 ESMShadowCaster::_GL_Shader_UBO0_Inst;
	//class ESMShadowCaster --END--

	//class OpaquePhongProgram_Light_Normal --BEGIN--

	class OpaquePhongProgram_Light_Normal{ //public inherit from IBase
	private:
		typedef OpaquePhongProgram_Light_Normal this_type;

		static GLuint ProgramID;
		static GLuint UBO;

		static struct _GL_Shader_UBO0{
			GLfloat ObjMat_Cam[16];
			GLfloat ProjMat_Cam[16];
			GLfloat ObjMat_Light[16];
			GLfloat ProjMat_Light[16];
			GLfloat ObjMat_Cam_Inv[16];
			GLfloat ProjMat_Cam_Inv[16];
			GLfloat ObjMat_Light_Inv[16];
			GLfloat Intensity[4]; //actually vec3. a float is added for padding 
			GLuint GBufferSize[2];
		} _GL_Shader_UBO0_Inst;

		OpaquePhongProgram_Light_Normal() = delete;
		~OpaquePhongProgram_Light_Normal() = delete;
	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Src, void *Data){ return true; }

		//IRenderExpr
		static void InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);

		static void Initialize();
		static void SetTransform(float *Transform_Camera, float *ProjMat_Camera, float *Transform_Light, float *ProjMat_Light);
		static void SetSize(UBINT Width, UBINT Height);
		static void LoadData(VPLightProperty *Data);
		static void LoadInputPair(nsBasic::IBase * const lpGBuffer, nsBasic::IBase * const lpShadowMap);
		static void PreRender();
		static void PostRender();
		static void Finalize();

		static void Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount);
		static const nsRender::RenderExpr Expr;
	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const nsRender::IRenderExpr IRenderExpr_Inst;
		static const ILightProgram ILightProgram_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(nsRender::IRenderExpr))return &this_type::IRenderExpr_Inst;
			else if (IID == __typeid(ILightProgram))return &this_type::ILightProgram_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
	};

	GLuint OpaquePhongProgram_Light_Normal::ProgramID = 0;
	GLuint OpaquePhongProgram_Light_Normal::UBO = 0;

	const nsBasic::IBase OpaquePhongProgram_Light_Normal::IBase_Inst = {
		OpaquePhongProgram_Light_Normal::_GetInterface,
		nullptr,
		OpaquePhongProgram_Light_Normal::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		nullptr
	};
	const ILightProgram OpaquePhongProgram_Light_Normal::ILightProgram_Inst = {
		__typeid(nsRender::Buffer_OpaquePhong),
	};
	const nsRender::IRenderExpr OpaquePhongProgram_Light_Normal::IRenderExpr_Inst{
		OpaquePhongProgram_Light_Normal::InitializeProgram,
		OpaquePhongProgram_Light_Normal::FinalizeProgram,
		&OpaquePhongProgram_Light_Normal::Expr
	};
	const nsRender::RenderExpr OpaquePhongProgram_Light_Normal::Expr = {
		/*ExprType*/	nsRender::RenderExpr::CONST_FUNCTION,
		/*OutputType*/	__typeid(nsRender::Buffer_OpaquePhong),
		/*Initialize*/	nullptr,
		/*Finalize*/	nullptr,
		/*Execute*/		OpaquePhongProgram_Light_Normal::Execute,
		/*VarPtr*/		nullptr
	};
	OpaquePhongProgram_Light_Normal::_GL_Shader_UBO0 OpaquePhongProgram_Light_Normal::_GL_Shader_UBO0_Inst;
	//class OpaquePhongProgram_Light_Normal --END--

	//class SSAOShadowCaster --BEGIN--

	class SSAOShadowCaster{
	private:
		typedef SSAOShadowCaster this_type;

		static GLuint ProgramID;
		static GLuint UBO;

		static struct _GL_Shader_UBO0{
			GLfloat ObjMat_Cam[16];
			GLfloat ProjMat_Cam[16];
			GLfloat ObjMat_Cam_Inv[16];
			GLfloat ProjMat_Cam_Inv[16];
			GLuint GBufferSize[2];
		} _GL_Shader_UBO0_Inst;

		SSAOShadowCaster() = delete;
		~SSAOShadowCaster() = delete;
	public:
		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Src, void *Data){ return true; }

		//IRenderExpr
		static void InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);

		static void Initialize();
		static void SetTransform(float *Transform_Camera, float *ProjMat_Camera);
		static void SetSize(UBINT Width, UBINT Height);
		static void LoadGBuffer(nsBasic::IBase *lpGBuffer);
		static void PreRender();
		static void Finalize();

		static void Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount);

		static const nsBasic::IBase IBase_Inst;
		static const nsRender::RenderExpr Expr;
	protected:
		static const nsRender::IRenderExpr IRenderExpr_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(nsRender::IRenderExpr))return &this_type::IRenderExpr_Inst;
			else return nullptr;
		}
	};

	GLuint SSAOShadowCaster::ProgramID = 0;
	GLuint SSAOShadowCaster::UBO = 0;
	const nsBasic::IBase SSAOShadowCaster::IBase_Inst{
		SSAOShadowCaster::_GetInterface,
		nullptr,
		SSAOShadowCaster::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		nullptr
	};
	const nsRender::IRenderExpr SSAOShadowCaster::IRenderExpr_Inst{
		SSAOShadowCaster::InitializeProgram,
		SSAOShadowCaster::FinalizeProgram,
		&SSAOShadowCaster::Expr
	};
	const nsRender::RenderExpr SSAOShadowCaster::Expr = {
		/*ExprType*/	nsRender::RenderExpr::CONST_FUNCTION,
		/*OutputType*/	__typeid(nsRender::Buffer_FloatMask),
		/*Initialize*/	nullptr,
		/*Finalize*/	nullptr,
		/*Execute*/		SSAOShadowCaster::Execute,
		/*VarPtr*/		nullptr
	};
	SSAOShadowCaster::_GL_Shader_UBO0 SSAOShadowCaster::_GL_Shader_UBO0_Inst;
	//class SSAOShadowCaster --END--

	//class OpaquePhongProgram_Light_Ambient --BEGIN--

	class OpaquePhongProgram_Light_Ambient{ //public inherit from IBase
	private:
		typedef OpaquePhongProgram_Light_Ambient this_type;

		static GLuint ProgramID;
		static GLuint UBO;

		static struct _GL_Shader_UBO0{
			GLfloat Intensity[4]; //actually vec3. a float is added for padding
			GLuint GBufferSize[2];
		} _GL_Shader_UBO0_Inst;

		OpaquePhongProgram_Light_Ambient() = delete;
		~OpaquePhongProgram_Light_Ambient() = delete;
	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, nsBasic::IBase * const Src, void *Data){ return true; }

		//IRenderExpr
		static void InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);

		static void Initialize();
		static void SetSize(UBINT Width, UBINT Height);
		static void LoadData(AmbientLightProperty *Data);
		static void LoadInputPair(nsBasic::IBase * const lpGBuffer, nsBasic::IBase *lpShadowMap);
		static void PreRender();
		static void Finalize();

		static void Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount);
		static const nsRender::RenderExpr Expr;
	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const nsRender::IRenderExpr IRenderExpr_Inst;
		static const ILightProgram ILightProgram_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(nsRender::IRenderExpr))return &this_type::IRenderExpr_Inst;
			else if (IID == __typeid(ILightProgram))return &this_type::ILightProgram_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
	};

	GLuint OpaquePhongProgram_Light_Ambient::ProgramID = 0;
	GLuint OpaquePhongProgram_Light_Ambient::UBO = 0;

	const nsBasic::IBase OpaquePhongProgram_Light_Ambient::IBase_Inst = {
		OpaquePhongProgram_Light_Ambient::_GetInterface,
		nullptr,
		OpaquePhongProgram_Light_Ambient::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		nullptr
	};
	const ILightProgram OpaquePhongProgram_Light_Ambient::ILightProgram_Inst = {
		__typeid(nsRender::Buffer_OpaquePhong),
	};
	const nsRender::IRenderExpr OpaquePhongProgram_Light_Ambient::IRenderExpr_Inst{
		OpaquePhongProgram_Light_Ambient::InitializeProgram,
		OpaquePhongProgram_Light_Ambient::FinalizeProgram,
		&OpaquePhongProgram_Light_Ambient::Expr
	};
	const nsRender::RenderExpr OpaquePhongProgram_Light_Ambient::Expr = {
		/*ExprType*/	nsRender::RenderExpr::CONST_FUNCTION,
		/*OutputType*/	__typeid(nsRender::Buffer_OpaquePhong),
		/*Initialize*/	nullptr,
		/*Finalize*/	nullptr,
		/*Execute*/		OpaquePhongProgram_Light_Ambient::Execute,
		/*VarPtr*/		nullptr
	};
	OpaquePhongProgram_Light_Ambient::_GL_Shader_UBO0 OpaquePhongProgram_Light_Ambient::_GL_Shader_UBO0_Inst;
	//class OpaquePhongProgram_Light_Ambient --END--

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	//class ESMShadowCaster --BEGIN--

	void ESMShadowCaster::InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		ESMShadowCaster::ProgramID = lpDevice->CreateShaderProgram(ESMShadowMask_VS, ESMShadowMask_FS);

		GLuint UBOPos, TexPos;
		glUseProgram(ESMShadowCaster::ProgramID);
		UBOPos = glGetUniformBlockIndex(ESMShadowCaster::ProgramID, "GlobalData");
		glUniformBlockBinding(ESMShadowCaster::ProgramID, UBOPos, 0);

		ESMShadowCaster::UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);

		char VarName[] = "tex ";
		for (UBINT i = 0; i < 2; i++){
			VarName[3] = i + '0';
			TexPos = glGetUniformLocation(ESMShadowCaster::ProgramID, VarName);
			glUniform1i(TexPos, i);
		}
	}
	void ESMShadowCaster::FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		lpDevice->DestroyUniformBuffer(ESMShadowCaster::UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		glDeleteProgram(ESMShadowCaster::ProgramID);
		ESMShadowCaster::UBO = 0;
		ESMShadowCaster::ProgramID = 0;
	}
	void ESMShadowCaster::Initialize(){
		glUseProgram(ESMShadowCaster::ProgramID);
	}
	void ESMShadowCaster::SetTransform(float *Transform_Camera, float *ProjMat_Camera, float *Transform_Light, float *ProjMat_Light){
		float TmpMat[16];
		memcpy(_GL_Shader_UBO0_Inst.ObjMat_Cam, Transform_Camera, 16 * sizeof(float));
		memcpy(_GL_Shader_UBO0_Inst.ProjMat_Cam, ProjMat_Camera, 16 * sizeof(float));
		memcpy(_GL_Shader_UBO0_Inst.ObjMat_Light, Transform_Light, 16 * sizeof(float));
		memcpy(_GL_Shader_UBO0_Inst.ProjMat_Light, ProjMat_Light, 16 * sizeof(float));

		nsMath::inv4(_GL_Shader_UBO0_Inst.ObjMat_Cam_Inv, _GL_Shader_UBO0_Inst.ObjMat_Cam);
		nsMath::inv4(_GL_Shader_UBO0_Inst.ProjMat_Cam_Inv, _GL_Shader_UBO0_Inst.ProjMat_Cam);
		nsMath::inv4(_GL_Shader_UBO0_Inst.ObjMat_Light_Inv, _GL_Shader_UBO0_Inst.ObjMat_Light);
	}
	void ESMShadowCaster::SetSize(UBINT Width, UBINT Height){
		_GL_Shader_UBO0_Inst.GBufferSize[0] = (UINT4b)Width;
		_GL_Shader_UBO0_Inst.GBufferSize[1] = (UINT4b)Height;
	}
	void ESMShadowCaster::LoadInputPair(nsBasic::IBase *lpGBuffer, nsBasic::IBase *lpShadowMap){
		const nsRender::IBuffer *I;
		GLuint *MapID;

		I = lpGBuffer->GetInterface<nsRender::IBuffer>();
		MapID = (GLuint *)I->GetValue(lpGBuffer, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *MapID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		I = lpShadowMap->GetInterface<nsRender::IBuffer>();
		MapID = (GLuint *)I->GetValue(lpShadowMap, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *MapID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	void ESMShadowCaster::PreRender(){
		glBindBuffer(GL_UNIFORM_BUFFER, ESMShadowCaster::UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(_GL_Shader_UBO0), &_GL_Shader_UBO0_Inst, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ESMShadowCaster::UBO);
	}
	void ESMShadowCaster::Finalize(){
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void ESMShadowCaster::Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount){
		nsRender::VPMatrix *CamVP = static_cast<nsRender::VPMatrix *>(ArgList[0]->Pointer);
		nsRender::VPMatrix *LightVP = static_cast<nsRender::VPMatrix *>(ArgList[1]->Pointer);

		const nsRender::IBuffer *I = Output->Buffer->GetInterface<nsRender::IBuffer>();
		INTERFACE_CALL(I, SetAsOutput)(Output->Buffer);

		ESMShadowCaster::Initialize();
		ESMShadowCaster::SetTransform(CamVP->MatLocal, CamVP->MatProj, LightVP->MatLocal, LightVP->MatProj);
		ESMShadowCaster::SetSize(Output->BufferWidth, Output->BufferHeight);
		ESMShadowCaster::LoadInputPair(ArgList[2]->Buffer, ArgList[3]->Buffer);
		ESMShadowCaster::PreRender();

		const nsScene::IGeometry *J = ArgList[4]->Buffer->GetInterface<nsScene::IGeometry>();
		INTERFACE_CALL(J, Render)(ArgList[4]->Buffer, lpDevice, 0);

		ESMShadowCaster::Finalize();
	}
	//class ESMShadowCaster --END--

	//class OpaquePhongProgram_Light_Normal --BEGIN--

	void OpaquePhongProgram_Light_Normal::InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		OpaquePhongProgram_Light_Normal::ProgramID = lpDevice->CreateShaderProgram(PhongLight_VS, PhongLight_FS);

		GLuint UBOPos, TexPos;
		glUseProgram(OpaquePhongProgram_Light_Normal::ProgramID);
		UBOPos = glGetUniformBlockIndex(OpaquePhongProgram_Light_Normal::ProgramID, "GlobalData");
		glUniformBlockBinding(OpaquePhongProgram_Light_Normal::ProgramID, UBOPos, 0);
		OpaquePhongProgram_Light_Normal::UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		//glBindBufferBase(GL_UNIFORM_BUFFER, 0, OpaquePhongProgram_Light_Normal::UBO);

		char VarName[] = "tex ";
		for (UBINT i = 0; i < 3; i++){
			VarName[3] = i + '0';
			TexPos = glGetUniformLocation(OpaquePhongProgram_Light_Normal::ProgramID, VarName);
			glUniform1i(TexPos, i);
		}
	}
	void OpaquePhongProgram_Light_Normal::FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		lpDevice->DestroyUniformBuffer(OpaquePhongProgram_Light_Normal::UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		glDeleteProgram(OpaquePhongProgram_Light_Normal::ProgramID);
		OpaquePhongProgram_Light_Normal::UBO = 0;
		OpaquePhongProgram_Light_Normal::ProgramID = 0;
	}

	void OpaquePhongProgram_Light_Normal::Initialize(){
		glUseProgram(OpaquePhongProgram_Light_Normal::ProgramID);
	}
	void OpaquePhongProgram_Light_Normal::SetTransform(float *Transform_Camera, float *ProjMat_Camera, float *Transform_Light, float *ProjMat_Light){
		float TmpMat[16];
		glBindBuffer(GL_UNIFORM_BUFFER, OpaquePhongProgram_Light_Normal::UBO);

		memcpy(_GL_Shader_UBO0_Inst.ObjMat_Cam, Transform_Camera, 16 * sizeof(float));
		memcpy(_GL_Shader_UBO0_Inst.ProjMat_Cam, ProjMat_Camera, 16 * sizeof(float));
		memcpy(_GL_Shader_UBO0_Inst.ObjMat_Light, Transform_Light, 16 * sizeof(float));
		memcpy(_GL_Shader_UBO0_Inst.ProjMat_Light, ProjMat_Light, 16 * sizeof(float));

		nsMath::inv4(_GL_Shader_UBO0_Inst.ObjMat_Cam_Inv, _GL_Shader_UBO0_Inst.ObjMat_Cam);
		nsMath::inv4(_GL_Shader_UBO0_Inst.ProjMat_Cam_Inv, _GL_Shader_UBO0_Inst.ProjMat_Cam);
		nsMath::inv4(_GL_Shader_UBO0_Inst.ObjMat_Light_Inv, _GL_Shader_UBO0_Inst.ObjMat_Light);
	}
	void OpaquePhongProgram_Light_Normal::SetSize(UBINT Width, UBINT Height){
		_GL_Shader_UBO0_Inst.GBufferSize[0] = (UINT4b)Width;
		_GL_Shader_UBO0_Inst.GBufferSize[1] = (UINT4b)Height;
	}
	void OpaquePhongProgram_Light_Normal::LoadData(VPLightProperty *Data){
		_GL_Shader_UBO0_Inst.Intensity[0] = Data->Intensity[0];
		_GL_Shader_UBO0_Inst.Intensity[1] = Data->Intensity[1];
		_GL_Shader_UBO0_Inst.Intensity[2] = Data->Intensity[2];
		GLuint UPos = glGetUniformLocation(OpaquePhongProgram_Light_Normal::ProgramID, "Status");
		glUniform1ui(UPos, 0);
	}
	void OpaquePhongProgram_Light_Normal::LoadInputPair(nsBasic::IBase * const lpGBuffer, nsBasic::IBase * const lpShadowMap){
		const nsRender::IBuffer *I;
		GLuint *MapID;

		I = lpShadowMap->GetInterface<nsRender::IBuffer>();
		MapID = (GLuint *)I->GetValue(lpShadowMap, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *MapID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		I = lpGBuffer->GetInterface<nsRender::IBuffer>();
		MapID = (GLuint *)I->GetValue(lpGBuffer, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, MapID[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, MapID[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	void OpaquePhongProgram_Light_Normal::PreRender(){
		glBindBuffer(GL_UNIFORM_BUFFER, OpaquePhongProgram_Light_Normal::UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(_GL_Shader_UBO0), &_GL_Shader_UBO0_Inst);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, OpaquePhongProgram_Light_Normal::UBO);
	}
	void OpaquePhongProgram_Light_Normal::PostRender(){}
	void OpaquePhongProgram_Light_Normal::Finalize(){
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void OpaquePhongProgram_Light_Normal::Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount){
		nsRender::VPMatrix *CamVP = static_cast<nsRender::VPMatrix *>(ArgList[0]->Pointer);
		nsRender::VPMatrix *LightVP = static_cast<nsRender::VPMatrix *>(ArgList[1]->Pointer);

		const nsRender::IBuffer *I = Output->Buffer->GetInterface<nsRender::IBuffer>();
		INTERFACE_CALL(I, SetAsOutput)(Output->Buffer);

		OpaquePhongProgram_Light_Normal::Initialize();
		OpaquePhongProgram_Light_Normal::SetTransform(CamVP->MatLocal, CamVP->MatProj, LightVP->MatLocal, LightVP->MatProj);
		OpaquePhongProgram_Light_Normal::SetSize(Output->BufferWidth, Output->BufferHeight);
		OpaquePhongProgram_Light_Normal::LoadData((VPLightProperty *)ArgList[5]->Pointer);
		OpaquePhongProgram_Light_Normal::LoadInputPair(ArgList[2]->Buffer, ArgList[3]->Buffer);
		OpaquePhongProgram_Light_Normal::PreRender();

		const nsScene::IGeometry *J = ArgList[4]->Buffer->GetInterface<nsScene::IGeometry>();
		INTERFACE_CALL(J, Render)(ArgList[4]->Buffer, lpDevice, 0);

		OpaquePhongProgram_Light_Normal::PostRender();
		OpaquePhongProgram_Light_Normal::Finalize();
	}
	//class OpaquePhongProgram_Light_Normal --END--

	//class SSAOShadowCaster --BEGIN--

	void SSAOShadowCaster::InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		SSAOShadowCaster::ProgramID = lpDevice->CreateShaderProgram(SSAOShadowMask_VS, SSAOShadowMask_FS);

		GLuint UBOPos, TexPos;
		glUseProgram(SSAOShadowCaster::ProgramID);
		UBOPos = glGetUniformBlockIndex(SSAOShadowCaster::ProgramID, "GlobalData");
		glUniformBlockBinding(SSAOShadowCaster::ProgramID, UBOPos, 0);

		SSAOShadowCaster::UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);

		char VarName[] = "tex ";
		for (UBINT i = 0; i < 2; i++){
			VarName[3] = i + '0';
			TexPos = glGetUniformLocation(SSAOShadowCaster::ProgramID, VarName);
			glUniform1i(TexPos, i);
		}
	}
	void SSAOShadowCaster::FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		lpDevice->DestroyUniformBuffer(SSAOShadowCaster::UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		glDeleteProgram(SSAOShadowCaster::ProgramID);
		SSAOShadowCaster::UBO = 0;
		SSAOShadowCaster::ProgramID = 0;
	}
	void SSAOShadowCaster::Initialize(){
		glUseProgram(SSAOShadowCaster::ProgramID);
	}
	void SSAOShadowCaster::SetTransform(float *Transform_Camera, float *ProjMat_Camera){
		float TmpMat[16];
		memcpy(_GL_Shader_UBO0_Inst.ObjMat_Cam, Transform_Camera, 16 * sizeof(float));
		memcpy(_GL_Shader_UBO0_Inst.ProjMat_Cam, ProjMat_Camera, 16 * sizeof(float));

		nsMath::inv4(_GL_Shader_UBO0_Inst.ObjMat_Cam_Inv, _GL_Shader_UBO0_Inst.ObjMat_Cam);
		nsMath::inv4(_GL_Shader_UBO0_Inst.ProjMat_Cam_Inv, _GL_Shader_UBO0_Inst.ProjMat_Cam);
	}
	void SSAOShadowCaster::SetSize(UBINT Width, UBINT Height){
		_GL_Shader_UBO0_Inst.GBufferSize[0] = (UINT4b)Width;
		_GL_Shader_UBO0_Inst.GBufferSize[1] = (UINT4b)Height;
	}
	void SSAOShadowCaster::LoadGBuffer(nsBasic::IBase *lpGBuffer){
		const nsRender::IBuffer *I;
		GLuint *MapID;

		I = lpGBuffer->GetInterface<nsRender::IBuffer>();
		MapID = (GLuint *)I->GetValue(lpGBuffer, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, MapID[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, MapID[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	void SSAOShadowCaster::PreRender(){
		glBindBuffer(GL_UNIFORM_BUFFER, SSAOShadowCaster::UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(_GL_Shader_UBO0), &_GL_Shader_UBO0_Inst, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, SSAOShadowCaster::UBO);
	}
	void SSAOShadowCaster::Finalize(){
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void SSAOShadowCaster::Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount){
		auto DrawSqr = [](){
			const GLfloat Point[8] = {
				-1.0f, -1.0f,
				1.0f, -1.0f,
				1.0f, 1.0f,
				-1.0f, 1.0f
			};

			GLuint VtxHandle;
			glGenBuffers(1, &VtxHandle);
			glBindBuffer(GL_ARRAY_BUFFER, VtxHandle);
			glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), Point, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, 0);

			glDrawArrays(GL_QUADS, 0, 4);
			glDeleteBuffers(1, &VtxHandle);
		};

		nsRender::VPMatrix *CamVP = static_cast<nsRender::VPMatrix *>(ArgList[0]->Pointer);

		const nsRender::IBuffer *I = Output->Buffer->GetInterface<nsRender::IBuffer>();
		INTERFACE_CALL(I, SetAsOutput)(Output->Buffer);

		SSAOShadowCaster::Initialize();
		SSAOShadowCaster::SetTransform(CamVP->MatLocal, CamVP->MatProj);
		SSAOShadowCaster::SetSize(Output->BufferWidth, Output->BufferHeight);
		SSAOShadowCaster::LoadGBuffer(ArgList[1]->Buffer);
		SSAOShadowCaster::PreRender();

		DrawSqr();

		SSAOShadowCaster::Finalize();
	}
	//class SSAOShadowCaster --END--

	//class OpaquePhongProgram_Light_Ambient --BEGIN--

	void OpaquePhongProgram_Light_Ambient::InitializeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		OpaquePhongProgram_Light_Ambient::ProgramID = lpDevice->CreateShaderProgram(AmbientLight_VS, AmbientLight_FS);

		GLuint UBOPos, TexPos;
		glUseProgram(OpaquePhongProgram_Light_Ambient::ProgramID);
		UBOPos = glGetUniformBlockIndex(OpaquePhongProgram_Light_Ambient::ProgramID, "GlobalData");
		glUniformBlockBinding(OpaquePhongProgram_Light_Ambient::ProgramID, UBOPos, 0);
		OpaquePhongProgram_Light_Ambient::UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);

		char VarName[] = "tex ";
		for (UBINT i = 0; i < 3; i++){
			VarName[3] = i + '0';
			TexPos = glGetUniformLocation(OpaquePhongProgram_Light_Ambient::ProgramID, VarName);
			glUniform1i(TexPos, i);
		}
	}
	void OpaquePhongProgram_Light_Ambient::FinalizeProgram(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		lpDevice->DestroyUniformBuffer(OpaquePhongProgram_Light_Ambient::UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		glDeleteProgram(OpaquePhongProgram_Light_Ambient::ProgramID);
		OpaquePhongProgram_Light_Ambient::UBO = 0;
		OpaquePhongProgram_Light_Ambient::ProgramID = 0;
	}

	void OpaquePhongProgram_Light_Ambient::Initialize(){
		glUseProgram(OpaquePhongProgram_Light_Ambient::ProgramID);
	}
	void OpaquePhongProgram_Light_Ambient::SetSize(UBINT Width, UBINT Height){
		_GL_Shader_UBO0_Inst.GBufferSize[0] = (UINT4b)Width;
		_GL_Shader_UBO0_Inst.GBufferSize[1] = (UINT4b)Height;
	}
	void OpaquePhongProgram_Light_Ambient::LoadData(AmbientLightProperty *Data){
		_GL_Shader_UBO0_Inst.Intensity[0] = Data->Intensity[0];
		_GL_Shader_UBO0_Inst.Intensity[1] = Data->Intensity[1];
		_GL_Shader_UBO0_Inst.Intensity[2] = Data->Intensity[2];
		GLuint UPos = glGetUniformLocation(OpaquePhongProgram_Light_Ambient::ProgramID, "Status");
		glUniform1ui(UPos, Data->Status);
	}
	void OpaquePhongProgram_Light_Ambient::LoadInputPair(nsBasic::IBase * const lpGBuffer, nsBasic::IBase *lpShadowMap){
		const nsRender::IBuffer *I;
		GLuint *MapID;

		I = lpShadowMap->GetInterface<nsRender::IBuffer>();
		MapID = (GLuint *)I->GetValue(lpShadowMap, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *MapID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		I = lpGBuffer->GetInterface<nsRender::IBuffer>();
		MapID = (GLuint *)I->GetValue(lpGBuffer, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, MapID[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, MapID[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	void OpaquePhongProgram_Light_Ambient::PreRender(){
		glBindBuffer(GL_UNIFORM_BUFFER, OpaquePhongProgram_Light_Ambient::UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(_GL_Shader_UBO0), &_GL_Shader_UBO0_Inst);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, OpaquePhongProgram_Light_Ambient::UBO);
	}
	void OpaquePhongProgram_Light_Ambient::Finalize(){
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	void OpaquePhongProgram_Light_Ambient::Execute(nsGUI::GLDevice *lpDevice, nsRender::RenderBufferInfo *Output, nsRender::RenderBufferInfo **ArgList, UBINT ArgCount){
		auto DrawSqr = [](){
			const GLfloat Point[8] = {
				-1.0f, -1.0f,
				1.0f, -1.0f,
				1.0f, 1.0f,
				-1.0f, 1.0f
			};

			GLuint VtxHandle;
			glGenBuffers(1, &VtxHandle);
			glBindBuffer(GL_ARRAY_BUFFER, VtxHandle);
			glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), Point, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, 0);

			glDrawArrays(GL_QUADS, 0, 4);
			glDeleteBuffers(1, &VtxHandle);
		};

		const nsRender::IBuffer *I = Output->Buffer->GetInterface<nsRender::IBuffer>();
		INTERFACE_CALL(I, SetAsOutput)(Output->Buffer);

		OpaquePhongProgram_Light_Ambient::Initialize();
		OpaquePhongProgram_Light_Ambient::SetSize(Output->BufferWidth, Output->BufferHeight);
		OpaquePhongProgram_Light_Ambient::LoadInputPair(ArgList[0]->Buffer, ArgList[1]->Buffer);
		OpaquePhongProgram_Light_Ambient::LoadData((AmbientLightProperty *)ArgList[2]->Pointer);
		OpaquePhongProgram_Light_Ambient::PreRender();

		DrawSqr();

		OpaquePhongProgram_Light_Ambient::Finalize();
	}
	//class OpaquePhongProgram_Light_Ambient --END--
}