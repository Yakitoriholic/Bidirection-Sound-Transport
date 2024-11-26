/* Description: OpenGL related functions which encapsulate things like context status switching, resource allocating and so on.
* Language:C++
* Author:***
*/

#ifndef LIB_GUI_GL
#define LIB_GUI_GL

#include "lGeneral.hpp"
#include "lContainer.hpp"
#include "lMath.hpp"

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check the following macros.
//These macros provide platform-related headers and static libraries.
#endif

#ifdef LIBENV_CPLR_VS
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glew32mx.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#endif

#if defined LIBENV_GUI_XWINDOW
#include<gl/glx.h>
#endif

#include <gl/glew.h>

namespace nsGUI{
	bool GLEW_INITIALIZED = false;

	class GLDevice{
	private:
		//internal structures
		struct _ResourceType{
			GLenum Type, SubType;
			UBINT Dim[3];
		};

		struct _ResourceType_Hash{
		public:
			size_t operator()(const _ResourceType &rhs) const{
				//BKDRHash from NOCOW
				size_t seed = 131, hash = 0;
				const unsigned char *CurPtr = (const unsigned char *)&rhs;
				for (UBINT i = 0; i < sizeof(_ResourceType); i++)hash = hash * seed + (*CurPtr++);
				return hash;
			}
		};

		struct _ResourceType_Pred{
		public:
			bool operator()(const _ResourceType& lhs, const _ResourceType& rhs) const{
				return ((lhs.Type == rhs.Type) &&
					(lhs.SubType == rhs.SubType) &&
					(lhs.Dim[0] == rhs.Dim[0]) &&
					(lhs.Dim[1] == rhs.Dim[1]) &&
					(lhs.Dim[2] == rhs.Dim[2])
					);
			}
		};

		class _ResourceBin{
		public:
			BINT BinSize_2FPrev;
			BINT BinSize_1FPrev;
			nsContainer::Vector<GLuint> Content;

			_ResourceBin() :Content(){
				this->BinSize_2FPrev = 0;
				this->BinSize_1FPrev = 0;
			}
		};

		GLDevice() = delete; //using the default constructor is prohibited
		GLDevice(const GLDevice &) = delete; //not copyable
		GLDevice & operator =(const GLDevice &) = delete; //not copyable

		//internal data
		//enum GLLoaderEnum{

		//};
		//GLuint _LoaderStat[1];

		//resource cache
		nsContainer::HashMap<_ResourceType, _ResourceBin, _ResourceType_Hash, _ResourceType_Pred> _ResHashMap;

#if defined LIBENV_GUI_WIN
		HGLRC RenderContext;
#endif

	public:
#if defined LIBENV_GUI_WIN
		GLDevice(HWND Window);
#endif
		GLuint CreateTexture(GLenum Type, UBINT Width, UBINT Height);
		void DestroyTexture(GLuint Handle, GLenum Type, UBINT Width, UBINT Height);

		GLuint CreateArrayBuffer(UBINT Size, GLenum Usage, const void *InitialData = nullptr);
		void DestroyArrayBuffer(GLuint Handle, UBINT Size, GLenum Usage);
		GLuint CreateElemArrayBuffer(UBINT Size, GLenum Usage, const void *InitialData = nullptr);
		void DestroyElemArrayBuffer(GLuint Handle, UBINT Size, GLenum Usage);
		GLuint CreateUniformBuffer(UBINT Size, GLenum Usage, const void *InitialData = nullptr);
		void DestroyUniformBuffer(GLuint Handle, UBINT Size, GLenum Usage);

		GLuint CreateShaderProgram(char *VertexShader, char *FragmentShader);

		void FrameEnd();
		void ReleaseGraphicResource();
		~GLDevice();
	};
}

/*-------------------------------- IMPLEMENTATION --------------------------------*/

namespace nsGUI{
#if defined LIBENV_GUI_WIN
	GLDevice::GLDevice(HWND Window) :
		_ResHashMap()

	{
		static const PIXELFORMATDESCRIPTOR GLPixelFmt = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			24,					//8-bit for every color
			0, 0, 0, 0, 0, 0,	//Color bit set to default
			0, 0,				//Windows does not support alpha channel
			0, 0, 0, 0, 0,		//No accumulation buffer
			32,					//32-bit for z-buffer
			0,					//No stencil buffer
			0,					//Windows does not support auxiliary channel
			PFD_MAIN_PLANE,		//Outdated.Reserved for compability.
			0,					//Reserved.
			0, 0, 0				//No mask.
		};

		HDC TmpDC = GetDC(Window);
		UBINT FmtIndex = ChoosePixelFormat(TmpDC, &GLPixelFmt);
		if (FALSE == SetPixelFormat(TmpDC, (int)FmtIndex, &GLPixelFmt))throw new std::exception("Cannot set OpenGL pixel format.");
		this->RenderContext = wglCreateContext(TmpDC);
		if (nullptr == this->RenderContext)throw new std::exception("OpenGL render context creation failed.");
		wglMakeCurrent(TmpDC, this->RenderContext); //must call this before glewInit()
		if (false == GLEW_INITIALIZED){
			GLenum GlError = glewInit();
			if (GLEW_OK != GlError)throw new std::exception("GLEW initialization failed.");
			else GLEW_INITIALIZED = true;
		}
		ReleaseDC(Window, TmpDC);
	}
#endif

	GLuint GLDevice::CreateTexture(GLenum Type, UBINT Width, UBINT Height){
		//first we'll try to find a texture in our resource cache.
		GLuint RetValue;
		_ResourceType NewResType = { GL_TEXTURE_2D, Type, { Width, Height, 0 } };

		auto _It = this->_ResHashMap.find(NewResType);
		if (_It == this->_ResHashMap.end() || _It->second.Content.empty()){
			glGenTextures(1, &RetValue);
			glBindTexture(GL_TEXTURE_2D, RetValue);
			switch (Type){
			case GL_DEPTH_COMPONENT32F:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, (GLsizei)Width, (GLsizei)Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
				break;
			case GL_RGBA32F:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)Width, (GLsizei)Height, 0, GL_RGBA, GL_FLOAT, nullptr);
				break;
			case GL_R32F:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (GLsizei)Width, (GLsizei)Height, 0, GL_RED, GL_FLOAT, nullptr);
				break;
			default:
				throw std::exception("The given texture format is not supported.");
			}
			glTextureParameteriEXT(RetValue, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteriEXT(RetValue, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		}
		else{
			RetValue = _It->second.Content.back();
			if (RetValue > 100){
				RetValue = 0;
			}
			if (_It->second.BinSize_1FPrev > 0)_It->second.BinSize_1FPrev--;
			_It->second.Content.pop_back();
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		return RetValue;
	}
	void GLDevice::DestroyTexture(GLuint Handle, GLenum Type, UBINT Width, UBINT Height){
		_ResourceType NewResType = { GL_TEXTURE_2D, Type, { Width, Height, 0 } };
		auto Result = this->_ResHashMap.try_emplace(NewResType);
		try{
			Result.first->second.Content.push_back(Handle);
		}
		catch (...){ glDeleteTextures(1, &Handle); }
	}
	GLuint GLDevice::CreateArrayBuffer(UBINT Size, GLenum Usage, const void *InitialData){
		GLuint RetValue;
		_ResourceType NewResType = { GL_ARRAY_BUFFER, Usage, { Size, 0, 0 } };

		auto _It = this->_ResHashMap.find(NewResType);
		if (_It == this->_ResHashMap.end() || _It->second.Content.empty()){
			glGenBuffers(1, &RetValue);
			glBindBuffer(GL_ARRAY_BUFFER, RetValue);
			glBufferData(GL_ARRAY_BUFFER, Size, InitialData, Usage);
		}
		else{
			RetValue = _It->second.Content.back();
			if (_It->second.BinSize_1FPrev > 0)_It->second.BinSize_1FPrev--;
			else if (_It->second.BinSize_2FPrev > 0)_It->second.BinSize_2FPrev--;
			_It->second.Content.pop_back();
			if (nullptr != InitialData){
				glBindBuffer(GL_ARRAY_BUFFER, RetValue);
				glBufferSubData(GL_ARRAY_BUFFER, 0, Size, InitialData);
			}
		}
		return RetValue;
	}
	void GLDevice::DestroyArrayBuffer(GLuint Handle, UBINT Size, GLenum Usage){
		_ResourceType NewResType = { GL_ARRAY_BUFFER, Usage, { Size, 0, 0 } };
		auto Result = this->_ResHashMap.try_emplace(NewResType);
		try{
			Result.first->second.Content.push_back(Handle);
		}
		catch (...){ glDeleteBuffers(1, &Handle); }
	}
	GLuint GLDevice::CreateElemArrayBuffer(UBINT Size, GLenum Usage, const void *InitialData){
		GLuint RetValue;
		_ResourceType NewResType = { GL_ELEMENT_ARRAY_BUFFER, Usage, { Size, 0, 0 } };

		auto _It = this->_ResHashMap.find(NewResType);
		if (_It == this->_ResHashMap.end() || _It->second.Content.empty()){
			glGenBuffers(1, &RetValue);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RetValue);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, InitialData, Usage);
		}
		else{
			RetValue = _It->second.Content.back();
			if (_It->second.BinSize_1FPrev > 0)_It->second.BinSize_1FPrev--;
			else if (_It->second.BinSize_2FPrev > 0)_It->second.BinSize_2FPrev--;
			_It->second.Content.pop_back();
			if (nullptr != InitialData){
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RetValue);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, Size, InitialData);
			}
		}
		return RetValue;
	}
	void GLDevice::DestroyElemArrayBuffer(GLuint Handle, UBINT Size, GLenum Usage){
		_ResourceType NewResType = { GL_ELEMENT_ARRAY_BUFFER, Usage, { Size, 0, 0 } };
		auto Result = this->_ResHashMap.try_emplace(NewResType);
		try{
			Result.first->second.Content.push_back(Handle);
		}
		catch (...){ glDeleteBuffers(1, &Handle); }
	}
	GLuint GLDevice::CreateUniformBuffer(UBINT Size, GLenum Usage, const void *InitialData){
		GLuint RetValue;
		_ResourceType NewResType = { GL_UNIFORM_BUFFER, Usage, { Size, 0, 0 } };

		auto _It = this->_ResHashMap.find(NewResType);
		if (_It == this->_ResHashMap.end() || _It->second.Content.empty()){
			glGenBuffers(1, &RetValue);
			glBindBuffer(GL_UNIFORM_BUFFER, RetValue);
			glBufferData(GL_UNIFORM_BUFFER, Size, nullptr, Usage);
		}
		else{
			RetValue = _It->second.Content.back();
			if (_It->second.BinSize_1FPrev > 0)_It->second.BinSize_1FPrev--;
			else if (_It->second.BinSize_2FPrev > 0)_It->second.BinSize_2FPrev--;
			_It->second.Content.pop_back();
			if (nullptr != InitialData){
				glBindBuffer(GL_UNIFORM_BUFFER, RetValue);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, Size, InitialData);
			}
		}
		return RetValue;
	}
	void GLDevice::DestroyUniformBuffer(GLuint Handle, UBINT Size, GLenum Usage){
		_ResourceType NewResType = { GL_UNIFORM_BUFFER, Usage, { Size, 0, 0 } };
		auto Result = this->_ResHashMap.try_emplace(NewResType);
		try{
			Result.first->second.Content.push_back(Handle);
		}
		catch (...){ glDeleteBuffers(1, &Handle); }
	}

	GLuint GLDevice::CreateShaderProgram(char *VertexShader, char *FragmentShader){
		GLuint SP, VS, FS, Result;

		//load VS
		VS = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(VS, 1, (const GLchar **)&VertexShader, nullptr);
		glCompileShader(VS);
		glGetShaderiv(VS, GL_COMPILE_STATUS, (int *)&Result);
		if (!Result){
			//Add breakpoint here to debug
			glGetShaderiv(VS, GL_INFO_LOG_LENGTH, (int *)&Result);
			char *ShaderLog = (char *)nsBasic::MemAlloc(Result);
			glGetShaderInfoLog(VS, (GLsizei)Result, (GLsizei *)&Result, ShaderLog);
			nsBasic::MemFree(ShaderLog, Result);
		}

		FS = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(FS, 1, (const GLchar **)&FragmentShader, nullptr);
		glCompileShader(FS);
		glGetShaderiv(FS, GL_COMPILE_STATUS, (int *)&Result);
		if (!Result){
			//Add breakpoint here to debug
			glGetShaderiv(FS, GL_INFO_LOG_LENGTH, (int *)&Result);
			char *ShaderLog = (char *)nsBasic::MemAlloc(Result);
			glGetShaderInfoLog(FS, (GLsizei)Result, (GLsizei *)&Result, ShaderLog);
			//OutputDebugStringA(ShaderLog);
			nsBasic::MemFree(ShaderLog, Result);
		}

		//Link
		SP = glCreateProgram();
		glAttachShader(SP, (GLuint)VS);
		glAttachShader(SP, (GLuint)FS);
		glLinkProgram(SP);

		glGetProgramiv(SP, GL_LINK_STATUS, (int *)&Result);
		if (!Result){
			//Add breakpoint here to debug
			glGetProgramiv(SP, GL_INFO_LOG_LENGTH, (int *)&Result);
			char *ProgramLog = (char *)nsBasic::MemAlloc(Result);
			glGetProgramInfoLog(SP, (GLsizei)Result, (GLsizei *)&Result, ProgramLog);
			nsBasic::MemFree(ProgramLog, Result);
			return 0;
		}
		else{
			glDeleteShader(VS);
			glDeleteShader(FS);
			return SP;
		}
	}

	void GLDevice::FrameEnd(){
		auto _It = this->_ResHashMap.begin();
		while (_It != this->_ResHashMap.end()){
			if (_It->second.BinSize_2FPrev > 0){
				GLuint *lpRes = &_It->second.Content[_It->second.Content.size() - _It->second.BinSize_2FPrev];
				if (GL_TEXTURE_2D == _It->first.Type){
					glDeleteTextures(_It->second.BinSize_2FPrev, lpRes);
				}
				else if (GL_UNIFORM_BUFFER == _It->first.Type || GL_ARRAY_BUFFER == _It->first.Type || GL_ELEMENT_ARRAY_BUFFER == _It->first.Type){
					glDeleteBuffers(_It->second.BinSize_2FPrev, lpRes);
				}
				_It->second.Content.erase(lpRes, _It->second.Content.end());
			}
			auto _It_Prev = _It;
			_It++;
			if (_It_Prev->second.Content.empty())this->_ResHashMap.erase(_It_Prev);
			else{
				_It_Prev->second.BinSize_2FPrev = _It_Prev->second.BinSize_1FPrev;
				_It_Prev->second.BinSize_1FPrev = _It_Prev->second.Content.size() - _It_Prev->second.BinSize_2FPrev;
			}
		}
	}
	void GLDevice::ReleaseGraphicResource(){
		for (auto _It = this->_ResHashMap.begin(); _It != this->_ResHashMap.end(); _It++){
			if (GL_TEXTURE_2D == _It->first.Type){
				if (_It->second.Content.size() > 0)glDeleteTextures(_It->second.Content.size(), _It->second.Content.begin());
			}
			else if (GL_UNIFORM_BUFFER == _It->first.Type || GL_ARRAY_BUFFER == _It->first.Type || GL_ELEMENT_ARRAY_BUFFER == _It->first.Type){
				if (_It->second.Content.size() > 0)glDeleteBuffers(_It->second.Content.size(), _It->second.Content.begin());
			}
		}
		this->_ResHashMap.clear();
	}
	GLDevice::~GLDevice(){
		this->ReleaseGraphicResource();
#if defined LIBENV_GUI_WIN
		wglDeleteContext(this->RenderContext);
#endif
	}
}

#endif