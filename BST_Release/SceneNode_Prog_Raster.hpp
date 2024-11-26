#include "SceneNode_General.hpp"

namespace nsScene{
	struct MaterialProperty{
		UINT4b Status;
		float Diffuse[3];
		float Specular[3];
		float Opacity;
		float Glossiness;

		enum StatusFlag{
			DIFFUSE_MAP_EXIST = 1 << 1,
			SPECULAR_MAP_EXIST = 1 << 2,
			SPECULAR_EXPONENT_MAP_EXIST = 1 << 3,
			OPACITY_MAP_EXIST = 1 << 4,
			NORMAL_MAP_EXIST = 1 << 5,
			NORMAL_MAP_DEPTH = 1 << 6,
		};
	};

	//class DepthProgram --BEGIN--

	class DepthProgram :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef DepthProgram this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		GLuint ProgramID;
		GLuint UBO;
		GLuint ProgramStat;

		struct _GL_Shader_UBO0{
			GLfloat ObjMat[16];
			GLfloat ProjMat[16];
		} _GL_Shader_UBO0_Inst;

	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//IRasterProgram
		static void Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat);
		static void LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data);
		static void PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IRasterProgram IRasterProgram_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IRasterProgram))return &this_type::IRasterProgram_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		DepthProgram(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->ProgramID = 0;
			this->UBO = 0;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~DepthProgram();
	};

	const nsBasic::IBase DepthProgram::IBase_Inst = {
		DepthProgram::_GetInterface,
		nullptr,
		DepthProgram::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		DepthProgram::Destroy
	};
	const IRasterProgram DepthProgram::IRasterProgram_Inst = {
		__typeid(nsRender::Buffer_Depth),
		DepthProgram::Initialize,
		nullptr,
		DepthProgram::SetTransform,
		DepthProgram::LoadData,
		DepthProgram::PreRender,
		DepthProgram::PostRender,
		DepthProgram::Finalize
	};
	//class DepthProgram --END--

	//class DepthProgram_Sphere --BEGIN--

	class DepthProgram_Sphere :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef DepthProgram_Sphere this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		GLuint ProgramID;
		GLuint UBO;
		GLuint ProgramStat;

		struct _GL_Shader_UBO0{
			GLfloat ObjMat[16];
			GLfloat ProjMat[16];
			GLfloat ObjMat_Inv[16];
			GLfloat ProjMat_Inv[16];
			GLuint GBufferSize[2];
		} _GL_Shader_UBO0_Inst;

	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//IRasterProgram
		static void Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void SetSize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height);
		static void SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat);
		static void LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data);
		static void PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IRasterProgram IRasterProgram_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IRasterProgram))return &this_type::IRasterProgram_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		DepthProgram_Sphere(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->ProgramID = 0;
			this->UBO = 0;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~DepthProgram_Sphere();
	};

	const nsBasic::IBase DepthProgram_Sphere::IBase_Inst = {
		DepthProgram_Sphere::_GetInterface,
		nullptr,
		DepthProgram_Sphere::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		DepthProgram_Sphere::Destroy
	};
	const IRasterProgram DepthProgram_Sphere::IRasterProgram_Inst = {
		__typeid(nsRender::Buffer_Depth),
		DepthProgram_Sphere::Initialize,
		DepthProgram_Sphere::SetSize,
		DepthProgram_Sphere::SetTransform,
		DepthProgram_Sphere::LoadData,
		DepthProgram_Sphere::PreRender,
		DepthProgram_Sphere::PostRender,
		DepthProgram_Sphere::Finalize
	};
	//class DepthProgram_Sphere --END--

	//class OpaquePhongProgram --BEGIN--

	class OpaquePhongProgram :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef OpaquePhongProgram this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		GLuint ProgramID;
		GLuint UBO;
		GLuint ProgramStat;

		struct _GL_Shader_UBO0{
			GLfloat ObjMat[16];
			GLfloat ProjMat[16];
			GLfloat ObjMat_Inv[16];
			GLfloat ProjMat_Inv[16];
			GLfloat Diffuse_Info[4];
			GLfloat Specular_Info[4];
		} _GL_Shader_UBO0_Inst;
	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//IRasterProgram
		static void Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat);
		static void LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data);
		static void PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IRasterProgram IRasterProgram_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IRasterProgram))return &this_type::IRasterProgram_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		OpaquePhongProgram(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->ProgramID = 0;
			this->UBO = 0;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~OpaquePhongProgram();
	};

	const nsBasic::IBase OpaquePhongProgram::IBase_Inst = {
		OpaquePhongProgram::_GetInterface,
		nullptr,
		OpaquePhongProgram::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		OpaquePhongProgram::Destroy
	};
	const IRasterProgram OpaquePhongProgram::IRasterProgram_Inst = {
		__typeid(nsRender::Buffer_OpaquePhong),
		OpaquePhongProgram::Initialize,
		nullptr,
		OpaquePhongProgram::SetTransform,
		OpaquePhongProgram::LoadData,
		OpaquePhongProgram::PreRender,
		OpaquePhongProgram::PostRender,
		OpaquePhongProgram::Finalize
	};
	//class OpaquePhongProgram --END--

	//class OpaquePhongProgram_Sphere --BEGIN--

	class OpaquePhongProgram_Sphere :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef OpaquePhongProgram_Sphere this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		GLuint ProgramID;
		GLuint UBO;
		GLuint ProgramStat;

		struct _GL_Shader_UBO0{
			GLfloat ObjMat[16];
			GLfloat ProjMat[16];
			GLfloat ObjMat_Inv[16];
			GLfloat ProjMat_Inv[16];
			GLfloat Diffuse_Info[4];
			GLfloat Specular_Info[4];
			GLuint GBufferSize[2];
		} _GL_Shader_UBO0_Inst;
	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//IRasterProgram
		static void Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void SetSize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height);
		static void SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat);
		static void LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data);
		static void PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IRasterProgram IRasterProgram_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IRasterProgram))return &this_type::IRasterProgram_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		OpaquePhongProgram_Sphere(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->ProgramID = 0;
			this->UBO = 0;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~OpaquePhongProgram_Sphere();
	};

	const nsBasic::IBase OpaquePhongProgram_Sphere::IBase_Inst = {
		OpaquePhongProgram_Sphere::_GetInterface,
		nullptr,
		OpaquePhongProgram_Sphere::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		OpaquePhongProgram_Sphere::Destroy
	};
	const IRasterProgram OpaquePhongProgram_Sphere::IRasterProgram_Inst = {
		__typeid(nsRender::Buffer_OpaquePhong),
		OpaquePhongProgram_Sphere::Initialize,
		OpaquePhongProgram_Sphere::SetSize,
		OpaquePhongProgram_Sphere::SetTransform,
		OpaquePhongProgram_Sphere::LoadData,
		OpaquePhongProgram_Sphere::PreRender,
		OpaquePhongProgram_Sphere::PostRender,
		OpaquePhongProgram_Sphere::Finalize
	};
	//class OpaquePhongProgram_Sphere --END--

	//class WhiteProgram --BEGIN--
	class WhiteProgram :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef WhiteProgram this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		GLuint ProgramID;
		GLuint UBO;

		struct _GL_Shader_UBO0{
			GLfloat ObjMat[16];
			GLfloat ProjMat[16];
		} _GL_Shader_UBO0_Inst;
	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//IRasterProgram
		static void Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat);
		static void LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data);
		static void PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
		static void Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice);
	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IRasterProgram IRasterProgram_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IRasterProgram))return &this_type::IRasterProgram_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;
		//constructors
		WhiteProgram(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->ProgramID = 0;
			this->UBO = 0;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~WhiteProgram();
	};

	const nsBasic::IBase WhiteProgram::IBase_Inst = {
		WhiteProgram::_GetInterface,
		nullptr,
		WhiteProgram::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		WhiteProgram::Destroy
	};
	const IRasterProgram WhiteProgram::IRasterProgram_Inst = {
		__typeid(nsRender::Buffer_OpaquePhong),
		WhiteProgram::Initialize,
		nullptr,
		WhiteProgram::SetTransform,
		WhiteProgram::LoadData,
		WhiteProgram::PreRender,
		WhiteProgram::PostRender,
		WhiteProgram::Finalize
	};
	//class WhiteProgram --END--

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	//class DepthProgram --BEGIN--

	void DepthProgram::Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (0 == This->ProgramID)This->ProgramID = lpDevice->CreateShaderProgram(DepthShader_VS, DepthShader_FS);

		GLuint UBOPos, TexPos;
		glUseProgram(This->ProgramID);
		UBOPos = glGetUniformBlockIndex(This->ProgramID, "GlobalData");
		glUniformBlockBinding(This->ProgramID, UBOPos, 0);
		This->UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		TexPos = glGetUniformLocation(This->ProgramID, "tex");
		glUniform1i(TexPos, 0);
	}
	void DepthProgram::SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat){
		this_type *This = static_cast<this_type *>(ThisBase);
		memcpy(This->_GL_Shader_UBO0_Inst.ObjMat, WorldMat, 16 * sizeof(float));
		memcpy(This->_GL_Shader_UBO0_Inst.ProjMat, ProjMat, 16 * sizeof(float));
		glBindBuffer(GL_UNIFORM_BUFFER, This->UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(_GL_Shader_UBO0), &This->_GL_Shader_UBO0_Inst, GL_STATIC_DRAW);
	}
	void DepthProgram::LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		TableObj<RenderData> *MapNode;
		if (nullptr != Data){
			MapNode = Data->find("Prop");
			if (nullptr != MapNode){
				nsScene::MaterialProperty *CurMatProp = (nsScene::MaterialProperty *)MapNode->Value.CPUData.data();
				This->ProgramStat = CurMatProp->Status;
			}
			MapNode = Data->find("Map_Opacity");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(0);
				This->ProgramStat |= nsScene::MaterialProperty::OPACITY_MAP_EXIST;
			}
		}
	}
	void DepthProgram::PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		GLuint UPos = glGetUniformLocation(This->ProgramID, "Status");
		glUniform1ui(UPos, This->ProgramStat);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, This->UBO);
	}
	void DepthProgram::PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		This->ProgramStat = 0;
	}
	void DepthProgram::Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		lpDevice->DestroyUniformBuffer(This->UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		This->UBO = 0;
	}
	DepthProgram::~DepthProgram(){
		if (0 != this->ProgramID)glDeleteProgram(this->ProgramID);
		if (0 != this->UBO)glDeleteBuffers(1, &this->UBO);
	}

	//class DepthProgram --END--

	//class DepthProgram_Sphere --BEGIN--

	void DepthProgram_Sphere::Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (0 == This->ProgramID)This->ProgramID = lpDevice->CreateShaderProgram(DepthShader_Sphere_VS, DepthShader_Sphere_FS);

		GLuint UBOPos, TexPos;
		glUseProgram(This->ProgramID);
		UBOPos = glGetUniformBlockIndex(This->ProgramID, "GlobalData");
		glUniformBlockBinding(This->ProgramID, UBOPos, 0);
		This->UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		TexPos = glGetUniformLocation(This->ProgramID, "tex");
		glUniform1i(TexPos, 0);
	}
	void DepthProgram_Sphere::SetSize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height){
		this_type *This = static_cast<this_type *>(ThisBase);
		This->_GL_Shader_UBO0_Inst.GBufferSize[0] = (GLuint)Width;
		This->_GL_Shader_UBO0_Inst.GBufferSize[1] = (GLuint)Height;
	}
	void DepthProgram_Sphere::SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat){
		this_type *This = static_cast<this_type *>(ThisBase);
		memcpy(This->_GL_Shader_UBO0_Inst.ObjMat, WorldMat, 16 * sizeof(float));
		memcpy(This->_GL_Shader_UBO0_Inst.ProjMat, ProjMat, 16 * sizeof(float));
		nsMath::inv4(This->_GL_Shader_UBO0_Inst.ObjMat_Inv, This->_GL_Shader_UBO0_Inst.ObjMat);
		nsMath::inv4(This->_GL_Shader_UBO0_Inst.ProjMat_Inv, This->_GL_Shader_UBO0_Inst.ProjMat);
		glBindBuffer(GL_UNIFORM_BUFFER, This->UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(_GL_Shader_UBO0), &This->_GL_Shader_UBO0_Inst, GL_STATIC_DRAW);
	}
	void DepthProgram_Sphere::LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		TableObj<RenderData> *MapNode;
		if (nullptr != Data){
			MapNode = Data->find("Prop");
			if (nullptr != MapNode){
				nsScene::MaterialProperty *CurMatProp = (nsScene::MaterialProperty *)MapNode->Value.CPUData.data();
				This->ProgramStat = CurMatProp->Status;
			}
			MapNode = Data->find("Map_Opacity");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(0);
				This->ProgramStat |= nsScene::MaterialProperty::OPACITY_MAP_EXIST;
			}
		}
	}
	void DepthProgram_Sphere::PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		GLuint UPos = glGetUniformLocation(This->ProgramID, "Status");
		glUniform1ui(UPos, This->ProgramStat);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, This->UBO);
	}
	void DepthProgram_Sphere::PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		This->ProgramStat = 0;
	}
	void DepthProgram_Sphere::Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		lpDevice->DestroyUniformBuffer(This->UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		This->UBO = 0;
	}
	DepthProgram_Sphere::~DepthProgram_Sphere(){
		if (0 != this->ProgramID)glDeleteProgram(this->ProgramID);
		if (0 != this->UBO)glDeleteBuffers(1, &this->UBO);
	}

	//class DepthProgram_Sphere --END--

	//class OpaquePhongProgram --BEGIN--

	void OpaquePhongProgram::Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (0 == This->ProgramID)This->ProgramID = lpDevice->CreateShaderProgram(PhongShader_VS, PhongShader_FS);

		GLuint UBOPos, TexPos;
		glUseProgram(This->ProgramID);
		UBOPos = glGetUniformBlockIndex(This->ProgramID, "GlobalData");
		glUniformBlockBinding(This->ProgramID, UBOPos, 0);
		This->UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		char VarName[] = "tex ";
		for (UBINT i = 0; i < 4; i++){
			VarName[3] = i + '0';
			TexPos = glGetUniformLocation(This->ProgramID, VarName);
			glUniform1i(TexPos, i);
		}

		This->ProgramStat = 0;
	}
	void OpaquePhongProgram::SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat){
		this_type *This = static_cast<this_type *>(ThisBase);
		memcpy(This->_GL_Shader_UBO0_Inst.ObjMat, WorldMat, 16 * sizeof(float));
		memcpy(This->_GL_Shader_UBO0_Inst.ProjMat, ProjMat, 16 * sizeof(float));
		nsMath::inv4(This->_GL_Shader_UBO0_Inst.ObjMat_Inv, This->_GL_Shader_UBO0_Inst.ObjMat);
		nsMath::inv4(This->_GL_Shader_UBO0_Inst.ProjMat_Inv, This->_GL_Shader_UBO0_Inst.ProjMat);
		glBindBuffer(GL_UNIFORM_BUFFER, This->UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(_GL_Shader_UBO0), &This->_GL_Shader_UBO0_Inst);
	}
	void OpaquePhongProgram::LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		TableObj<RenderData> *MapNode;
		if (nullptr != Data){
			MapNode = Data->find("Prop");
			if (nullptr != MapNode){
				nsScene::MaterialProperty *CurMatProp = (nsScene::MaterialProperty *)MapNode->Value.CPUData.data();
				This->ProgramStat = CurMatProp->Status;
				This->_GL_Shader_UBO0_Inst.Diffuse_Info[0] = CurMatProp->Diffuse[0];
				This->_GL_Shader_UBO0_Inst.Diffuse_Info[1] = CurMatProp->Diffuse[1];
				This->_GL_Shader_UBO0_Inst.Diffuse_Info[2] = CurMatProp->Diffuse[2];
				This->_GL_Shader_UBO0_Inst.Diffuse_Info[3] = 0.0f;
				This->_GL_Shader_UBO0_Inst.Specular_Info[0] = CurMatProp->Specular[0];
				This->_GL_Shader_UBO0_Inst.Specular_Info[1] = CurMatProp->Specular[1];
				This->_GL_Shader_UBO0_Inst.Specular_Info[2] = CurMatProp->Specular[2];
				This->_GL_Shader_UBO0_Inst.Specular_Info[3] = CurMatProp->Glossiness;
			}

			MapNode = Data->find("Map_Diff");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(0);
				This->ProgramStat |= nsScene::MaterialProperty::DIFFUSE_MAP_EXIST;
			}
			MapNode = Data->find("Map_Spec_Exp");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(1);
				This->ProgramStat |= nsScene::MaterialProperty::SPECULAR_EXPONENT_MAP_EXIST;
			}
			MapNode = Data->find("Map_Opacity");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(2);
				This->ProgramStat |= nsScene::MaterialProperty::OPACITY_MAP_EXIST;
			}
			MapNode = Data->find("Map_Bump");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(3);
				This->ProgramStat |= nsScene::MaterialProperty::NORMAL_MAP_EXIST;
				if (1 == MapNode->Value.CPUData.dim_size(2)){
					This->ProgramStat |= nsScene::MaterialProperty::NORMAL_MAP_DEPTH;
				}
			}
		}
	}
	void OpaquePhongProgram::PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		GLuint UPos = glGetUniformLocation(This->ProgramID, "Status");
		glUniform1ui(UPos, This->ProgramStat);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, This->UBO);
	}
	void OpaquePhongProgram::PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		This->ProgramStat = 0;
	}
	void OpaquePhongProgram::Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		lpDevice->DestroyUniformBuffer(This->UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		This->UBO = 0;
	}
	OpaquePhongProgram::~OpaquePhongProgram(){
		if (0 != this->ProgramID)glDeleteProgram(this->ProgramID);
		if (0 != this->UBO)glDeleteBuffers(1, &this->UBO);
	}

	//class OpaquePhongProgram --END--

	//class OpaquePhongProgram_Sphere --BEGIN--

	void OpaquePhongProgram_Sphere::Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (0 == This->ProgramID)This->ProgramID = lpDevice->CreateShaderProgram(PhongShader_Sphere_VS, PhongShader_Sphere_FS);

		GLuint UBOPos, TexPos;
		glUseProgram(This->ProgramID);
		UBOPos = glGetUniformBlockIndex(This->ProgramID, "GlobalData");
		glUniformBlockBinding(This->ProgramID, UBOPos, 0);
		This->UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		char VarName[] = "tex ";
		for (UBINT i = 0; i < 4; i++){
			VarName[3] = i + '0';
			TexPos = glGetUniformLocation(This->ProgramID, VarName);
			glUniform1i(TexPos, i);
		}

		This->ProgramStat = 0;
	}
	void OpaquePhongProgram_Sphere::SetSize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height){
		this_type *This = static_cast<this_type *>(ThisBase);
		This->_GL_Shader_UBO0_Inst.GBufferSize[0] = (GLuint)Width;
		This->_GL_Shader_UBO0_Inst.GBufferSize[1] = (GLuint)Height;
	}
	void OpaquePhongProgram_Sphere::SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat){
		this_type *This = static_cast<this_type *>(ThisBase);
		memcpy(This->_GL_Shader_UBO0_Inst.ObjMat, WorldMat, 16 * sizeof(float));
		memcpy(This->_GL_Shader_UBO0_Inst.ProjMat, ProjMat, 16 * sizeof(float));
		nsMath::inv4(This->_GL_Shader_UBO0_Inst.ObjMat_Inv, This->_GL_Shader_UBO0_Inst.ObjMat);
		nsMath::inv4(This->_GL_Shader_UBO0_Inst.ProjMat_Inv, This->_GL_Shader_UBO0_Inst.ProjMat);
		glBindBuffer(GL_UNIFORM_BUFFER, This->UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(_GL_Shader_UBO0), &This->_GL_Shader_UBO0_Inst);
	}
	void OpaquePhongProgram_Sphere::LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data){
		this_type *This = static_cast<this_type *>(ThisBase);
		TableObj<RenderData> *MapNode;
		if (nullptr != Data){
			MapNode = Data->find("Prop");
			if (nullptr != MapNode){
				nsScene::MaterialProperty *CurMatProp = (nsScene::MaterialProperty *)MapNode->Value.CPUData.data();
				This->ProgramStat = CurMatProp->Status;
				This->_GL_Shader_UBO0_Inst.Diffuse_Info[0] = CurMatProp->Diffuse[0];
				This->_GL_Shader_UBO0_Inst.Diffuse_Info[1] = CurMatProp->Diffuse[1];
				This->_GL_Shader_UBO0_Inst.Diffuse_Info[2] = CurMatProp->Diffuse[2];
				This->_GL_Shader_UBO0_Inst.Diffuse_Info[3] = 0.0f;
				This->_GL_Shader_UBO0_Inst.Specular_Info[0] = CurMatProp->Specular[0];
				This->_GL_Shader_UBO0_Inst.Specular_Info[1] = CurMatProp->Specular[1];
				This->_GL_Shader_UBO0_Inst.Specular_Info[2] = CurMatProp->Specular[2];
				This->_GL_Shader_UBO0_Inst.Specular_Info[3] = CurMatProp->Glossiness;
			}

			MapNode = Data->find("Map_Diff");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(0);
				This->ProgramStat |= nsScene::MaterialProperty::DIFFUSE_MAP_EXIST;
			}
			MapNode = Data->find("Map_Spec_Exp");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(1);
				This->ProgramStat |= nsScene::MaterialProperty::SPECULAR_EXPONENT_MAP_EXIST;
			}
			MapNode = Data->find("Map_Opacity");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(2);
				This->ProgramStat |= nsScene::MaterialProperty::OPACITY_MAP_EXIST;
			}
			MapNode = Data->find("Map_Bump");
			if (nullptr != MapNode){
				MapNode->Value.GPUBind_Tex2D(3);
				This->ProgramStat |= nsScene::MaterialProperty::NORMAL_MAP_EXIST;
				if (1 == MapNode->Value.CPUData.dim_size(2)){
					This->ProgramStat |= nsScene::MaterialProperty::NORMAL_MAP_DEPTH;
				}
			}
		}
	}
	void OpaquePhongProgram_Sphere::PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		GLuint UPos = glGetUniformLocation(This->ProgramID, "Status");
		glUniform1ui(UPos, This->ProgramStat);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, This->UBO);
	}
	void OpaquePhongProgram_Sphere::PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		This->ProgramStat = 0;
	}
	void OpaquePhongProgram_Sphere::Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		lpDevice->DestroyUniformBuffer(This->UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		This->UBO = 0;
	}
	OpaquePhongProgram_Sphere::~OpaquePhongProgram_Sphere(){
		if (0 != this->ProgramID)glDeleteProgram(this->ProgramID);
		if (0 != this->UBO)glDeleteBuffers(1, &this->UBO);
	}

	//class OpaquePhongProgram_Sphere --END--

	//class WhiteProgram --BEGIN--

	void WhiteProgram::Initialize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (0 == This->ProgramID)This->ProgramID = lpDevice->CreateShaderProgram(WhiteShader_VS, WhiteShader_FS);

		GLuint UBOPos;
		glUseProgram(This->ProgramID);
		UBOPos = glGetUniformBlockIndex(This->ProgramID, "GlobalData");
		glUniformBlockBinding(This->ProgramID, UBOPos, 0);
		This->UBO = lpDevice->CreateUniformBuffer(sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
	}
	void WhiteProgram::SetTransform(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, float *WorldMat, float *ProjMat){
		this_type *This = static_cast<this_type *>(ThisBase);
		memcpy(This->_GL_Shader_UBO0_Inst.ObjMat, WorldMat, 16 * sizeof(float));
		memcpy(This->_GL_Shader_UBO0_Inst.ProjMat, ProjMat, 16 * sizeof(float));
		glBindBuffer(GL_UNIFORM_BUFFER, This->UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(_GL_Shader_UBO0), &This->_GL_Shader_UBO0_Inst);
	}
	void WhiteProgram::LoadData(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, TableObj<RenderData> *Data){
	}
	void WhiteProgram::PreRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, This->UBO);
	}
	void WhiteProgram::PostRender(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){}
	void WhiteProgram::Finalize(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice){
		this_type *This = static_cast<this_type *>(ThisBase);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		lpDevice->DestroyUniformBuffer(This->UBO, sizeof(_GL_Shader_UBO0), GL_STATIC_DRAW);
		This->UBO = 0;
	}
	WhiteProgram::~WhiteProgram(){
		if (0 != this->ProgramID)glDeleteProgram(this->ProgramID);
		if (0 != this->UBO)glDeleteBuffers(1, &this->UBO);
	}

	//class WhiteProgram --END--
}