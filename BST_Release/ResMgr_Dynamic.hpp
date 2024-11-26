#ifndef LIB_RENDER_RESMGR_DYNAMIC
#define LIB_RENDER_RESMGR_DYNAMIC

#include "lGeneral.hpp"
#include "lContainer.hpp"
#include "lGUI.hpp"

namespace nsRender{
	struct RenderBufferInfo{
		union{
			nsBasic::IBase *Buffer;
			void *Pointer;
		};
		UBINT BufferType;
		UBINT BufferWidth;
		UBINT BufferHeight;
	};

	void *CurOutputBuffer = nullptr;

	struct IBuffer{
		void(*GetSize)(nsBasic::IBase * const, UBINT *);
		void *(*GetValue)(nsBasic::IBase * const, UBINT);
		void(*SetAsOutput)(nsBasic::IBase * const);
		void(*Clear)(nsBasic::IBase * const);
	};

	//class Buffer_Depth --BEGIN--

	class Buffer_Depth :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Buffer_Depth this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		nsGUI::GLDevice *lpDevice;
		GLuint FBO, Texture;
		UBINT Size[2];
	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//IBuffer
		static void GetSize(nsBasic::IBase * const ThisBase, UBINT *lpSize);
		static void *GetValue(nsBasic::IBase * const ThisBase, UBINT Index);
		static void SetAsOutput(nsBasic::IBase * const ThisBase);
		static void Clear(nsBasic::IBase * const ThisBase);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IBuffer IBuffer_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IBuffer))return &this_type::IBuffer_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;

		//constructors
		Buffer_Depth(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height);
		static this_type * const Create(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height){ //equivalent to the new operator
			this_type *RetValue = new this_type(lpDevice, Width, Height);
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~Buffer_Depth();
	};

	const nsBasic::IBase Buffer_Depth::IBase_Inst = {
		Buffer_Depth::_GetInterface,
		nullptr,
		Buffer_Depth::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Buffer_Depth::Destroy
	};
	const IBuffer Buffer_Depth::IBuffer_Inst = {
		Buffer_Depth::GetSize,
		Buffer_Depth::GetValue,
		Buffer_Depth::SetAsOutput,
		Buffer_Depth::Clear
	};

	//class Buffer_Depth --END--

	//class Buffer_FloatMask --BEGIN--

	class Buffer_FloatMask :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Buffer_FloatMask this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		nsGUI::GLDevice *lpDevice;
		GLuint FBO, Texture;
		UBINT Size[2];
	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//IBuffer
		static void GetSize(nsBasic::IBase * const ThisBase, UBINT *lpSize);
		static void *GetValue(nsBasic::IBase * const ThisBase, UBINT Index);
		static void SetAsOutput(nsBasic::IBase * const ThisBase);
		static void Clear(nsBasic::IBase * const ThisBase);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IBuffer IBuffer_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IBuffer))return &this_type::IBuffer_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;

		//constructors
		Buffer_FloatMask(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height);
		static this_type * const Create(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height){ //equivalent to the new operator
			this_type *RetValue = new this_type(lpDevice, Width, Height);
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~Buffer_FloatMask();
	};

	const nsBasic::IBase Buffer_FloatMask::IBase_Inst = {
		Buffer_FloatMask::_GetInterface,
		nullptr,
		Buffer_FloatMask::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Buffer_FloatMask::Destroy
	};
	const IBuffer Buffer_FloatMask::IBuffer_Inst = {
		Buffer_FloatMask::GetSize,
		Buffer_FloatMask::GetValue,
		Buffer_FloatMask::SetAsOutput,
		Buffer_FloatMask::Clear
	};

	//class Buffer_FloatMask --END--

	//class Buffer_OpaquePhong --BEGIN--

	class Buffer_OpaquePhong :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Buffer_OpaquePhong this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		nsGUI::GLDevice *lpDevice;
		GLuint FBO;
		GLuint Texture[2];
		UBINT Size[2];
	public:

		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//IBuffer
		static void GetSize(nsBasic::IBase * const ThisBase, UBINT *lpSize);
		static void *GetValue(nsBasic::IBase * const ThisBase, UBINT Index);
		static void SetAsOutput(nsBasic::IBase * const ThisBase);
		static void Clear(nsBasic::IBase * const ThisBase);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IBuffer IBuffer_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IBuffer))return &this_type::IBuffer_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;

		//constructors
		Buffer_OpaquePhong(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height);
		static this_type * const Create(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height){ //equivalent to the new operator
			this_type *RetValue = new this_type(lpDevice, Width, Height);
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~Buffer_OpaquePhong();
	};

	const nsBasic::IBase Buffer_OpaquePhong::IBase_Inst = {
		Buffer_OpaquePhong::_GetInterface,
		nullptr,
		Buffer_OpaquePhong::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Buffer_OpaquePhong::Destroy
	};
	const IBuffer Buffer_OpaquePhong::IBuffer_Inst = {
		Buffer_OpaquePhong::GetSize,
		Buffer_OpaquePhong::GetValue,
		Buffer_OpaquePhong::SetAsOutput,
		Buffer_OpaquePhong::Clear
	};

	//class Buffer_OpaquePhong --END--

	//class RenderBufferFactory --BEGIN--

	nsBasic::IBase *CreateRenderBuffer(nsGUI::GLDevice *lpDevice, UBINT TypeID, UBINT Width, UBINT Height){
		if (TypeID == __typeid(Buffer_Depth))return Buffer_Depth::Create(lpDevice, Width, Height);
		else if (TypeID == __typeid(Buffer_FloatMask))return Buffer_FloatMask::Create(lpDevice, Width, Height);
		else if (TypeID == __typeid(Buffer_OpaquePhong))return Buffer_OpaquePhong::Create(lpDevice, Width, Height);
		return nullptr;
	}

	//class RenderBufferFactory --END--

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	//class Buffer_Depth --BEGIN--

	void Buffer_Depth::GetSize(nsBasic::IBase * const ThisBase, UBINT *lpSize){
		this_type *This = static_cast<this_type *>(ThisBase);
		lpSize[0] = This->Size[0];
		lpSize[1] = This->Size[1];
	}
	void *Buffer_Depth::GetValue(nsBasic::IBase * const ThisBase, UBINT Index){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (0 == Index)return &This->Texture; else return nullptr;
	}
	void Buffer_Depth::SetAsOutput(nsBasic::IBase * const ThisBase){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (CurOutputBuffer != This){
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, This->FBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, This->Texture, 0);
			glViewport(0, 0, This->Size[0], This->Size[1]);
			Buffer_Depth::Clear(This);
			CurOutputBuffer = This;
		}
	}
	void Buffer_Depth::Clear(nsBasic::IBase * const ThisBase){
		this_type *This = static_cast<this_type *>(ThisBase);
		glClearDepth(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	Buffer_Depth::Buffer_Depth(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height){
		*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
		this->Size[0] = Width;
		this->Size[1] = Height;

		this->lpDevice = lpDevice;
		glGenFramebuffers(1, &this->FBO);
		this->Texture = this->lpDevice->CreateTexture(GL_DEPTH_COMPONENT32F, Width, Height);
	}
	Buffer_Depth::~Buffer_Depth(){
		this->lpDevice->DestroyTexture(this->Texture, GL_DEPTH_COMPONENT32F, this->Size[0], this->Size[1]);
		glDeleteFramebuffers(1, &this->FBO);
	}

	//class Buffer_Depth --END--

	//class Buffer_FloatMask --BEGIN--

	void Buffer_FloatMask::GetSize(nsBasic::IBase * const ThisBase, UBINT *lpSize){
		this_type *This = static_cast<this_type *>(ThisBase);
		lpSize[0] = This->Size[0];
		lpSize[1] = This->Size[1];
	}
	void *Buffer_FloatMask::GetValue(nsBasic::IBase * const ThisBase, UBINT Index){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (0 == Index)return &This->Texture; else return nullptr;
	}
	void Buffer_FloatMask::SetAsOutput(nsBasic::IBase * const ThisBase){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (CurOutputBuffer != This){
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, This->FBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, This->Texture, 0);
			glViewport(0, 0, This->Size[0], This->Size[1]);
			Buffer_FloatMask::Clear(This);
			CurOutputBuffer = This;
		}
	}
	void Buffer_FloatMask::Clear(nsBasic::IBase * const ThisBase){
		this_type *This = static_cast<this_type *>(ThisBase);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	Buffer_FloatMask::Buffer_FloatMask(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height){
		*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
		this->Size[0] = Width;
		this->Size[1] = Height;

		this->lpDevice = lpDevice;
		glGenFramebuffers(1, &this->FBO);
		this->Texture = this->lpDevice->CreateTexture(GL_R32F, Width, Height);
	}
	Buffer_FloatMask::~Buffer_FloatMask(){
		this->lpDevice->DestroyTexture(this->Texture, GL_R32F, this->Size[0], this->Size[1]);
		glDeleteFramebuffers(1, &this->FBO);
	}

	//class Buffer_FloatMask --END--

	//class Buffer_OpaquePhong --BEGIN--

	void Buffer_OpaquePhong::GetSize(nsBasic::IBase * const ThisBase, UBINT *lpSize){
		this_type *This = static_cast<this_type *>(ThisBase);
		lpSize[0] = This->Size[0];
		lpSize[1] = This->Size[1];
	}
	void *Buffer_OpaquePhong::GetValue(nsBasic::IBase * const ThisBase, UBINT Index){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (0 == Index)return This->Texture; else return nullptr;
	}
	void Buffer_OpaquePhong::SetAsOutput(nsBasic::IBase * const ThisBase){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (CurOutputBuffer != This){
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, This->FBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, This->Texture[0], 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, This->Texture[1], 0);
			glViewport(0, 0, This->Size[0], This->Size[1]);
			Buffer_OpaquePhong::Clear(This);
			CurOutputBuffer = This;
		}
	}
	void Buffer_OpaquePhong::Clear(nsBasic::IBase * const ThisBase){
		glClearDepth(1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	Buffer_OpaquePhong::Buffer_OpaquePhong(nsGUI::GLDevice *lpDevice, UBINT Width, UBINT Height){
		*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;

		this->Size[0] = Width;
		this->Size[1] = Height;

		this->lpDevice = lpDevice;
		glGenFramebuffers(1, &this->FBO);
		this->Texture[0] = this->lpDevice->CreateTexture(GL_DEPTH_COMPONENT32F, Width, Height);
		this->Texture[1] = this->lpDevice->CreateTexture(GL_RGBA32F, Width, Height);
	}
	Buffer_OpaquePhong::~Buffer_OpaquePhong(){
		this->lpDevice->DestroyTexture(this->Texture[0], GL_DEPTH_COMPONENT32F, this->Size[0], this->Size[1]);
		this->lpDevice->DestroyTexture(this->Texture[1], GL_RGBA32F, this->Size[0], this->Size[1]);
		glDeleteFramebuffers(1, &this->FBO);
	}

	//class Buffer_OpaquePhong --END--
}

#endif