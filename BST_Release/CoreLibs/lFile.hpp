/* Description:Functions for File I/O,text coding,and strings.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_FILE
#define LIB_FILE

#include "lGeneral.hpp"

namespace nsFile{
	#if defined LIBENV_OS_WIN

	extern inline UBINT _OpenFile_Read(const UBCHAR *lpFileName){ return (UBINT)CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr); }
	extern inline UBINT _OpenFile_Write(const UBCHAR *lpFileName){ return (UBINT)CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr); }
	extern inline UBINT _OpenFile_Write_OpenExist(const UBCHAR *lpFileName){ return (UBINT)CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, 0, nullptr); }
	extern inline bool _GetFileLen(UBINT hFile, UINT8b *FileLen){ if (GetFileSizeEx((HANDLE)hFile, (PLARGE_INTEGER)FileLen))return true; else return false; }
	extern inline bool _SetFilePtr(UBINT hFile, INT8b Offset, UBINT BaseType){ LARGE_INTEGER TmpInt; TmpInt.QuadPart = Offset; if (SetFilePointerEx((HANDLE)hFile, (LARGE_INTEGER)TmpInt, nullptr, (DWORD)BaseType))return true; else return false; }
	extern inline bool _GetFilePtr(UBINT hFile, UINT8b *lpOffset){ LARGE_INTEGER TmpInt = { 0, 0 }; if (SetFilePointerEx((HANDLE)hFile, TmpInt, (PLARGE_INTEGER)lpOffset, 1))return true; else return false; }
	extern inline UINT4b _ReadFile(UBINT hFile, void *Addr, UINT4b Size){ DWORD RetValue; ReadFile((HANDLE)hFile, Addr, Size, &RetValue, nullptr); return (UINT4b)RetValue; }
	extern inline UINT4b _WriteFile(UBINT hFile, const void *Addr, UINT4b Size){ DWORD RetValue; WriteFile((HANDLE)hFile, Addr, Size, &RetValue, nullptr); return (UINT4b)RetValue; }
	extern inline void _CloseFile(UBINT hFile){CloseHandle((HANDLE)hFile);}
	
#elif defined LIBENV_OS_LINUX
	#define _FILE_OFFSET_BITS 64
	#include "fcntl.h"
	#include <sys/stat.h>

	extern inline UBINT _OpenFile_Read(UBCHAR *lpFileName){return(UBINT)open(lpFileName,O_RDONLY);}
	extern inline UBINT _OpenFile_Write(UBCHAR *lpFileName){return(UBINT)open(lpFileName,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);}
	extern inline UBINT _OpenFile_Write_OpenExist(UBCHAR *lpFileName){return(UBINT)open(lpFileName,O_WRONLY|O_CREAT,S_IRWXU);}
	extern bool _GetFileLen(UBINT hFile,UINT8b *FileLen){
		struct stat64 TmpStat;  
		if(0==fstat64(hFile,&TmpStat)){
			*FileLen=TmpStat.st_size;
			return true;
		}
		else return false;
	}
	extern inline bool _SetFilePtr(UBINT hFile, INT8b Offset, UBINT BaseType){ if (-1==lseek64(hFile,Offset,BaseType))return false;else return true;}
	extern inline bool _GetFilePtr(UBINT hFile, UINT8b *lpOffset){off64_t TmpOffset==lseek64(hFile,0,1));if(-1==TmpOffset)return false;else{*lpOffset=(UINT8b)TmpOffset;return true;}}
	extern inline UINT4b _ReadFile(UBINT hFile,void *Addr,UINT4b Size){return read(hFile,Addr,Size);}
	extern inline UINT4b _WriteFile(UBINT hFile,const void *Addr,UINT4b Size){return write(hFile,Addr,Size);}
	extern inline void _CloseFile(UBINT hFile){close(hFile);}
	#endif

	class ReadFileStream :public nsBasic::Stream_RS<unsigned char>{
	private:
		typedef Stream<unsigned char> stream_base_type;
		static const stream_base_type::Interface _I;

		ReadFileStream(const ReadFileStream &) = delete; //not copyable
		ReadFileStream & operator =(const ReadFileStream &) = delete; //not copyable

		static bool _Read(stream_base_type * const ThisBase, unsigned char *Dest);
		static UINT8b _ReadBulk(stream_base_type * const ThisBase, unsigned char *Dest, UINT8b Count);
		static UINT8b _GetPtr(stream_base_type * const ThisBase);
		static bool _SetPtr(stream_base_type * const ThisBase, nsBasic::StreamSeekType SeekType, INT8b Offset);
		static void _Delete(stream_base_type * const ThisBase){ delete static_cast<ReadFileStream *>(ThisBase); }
		static void _Destroy(stream_base_type * const ThisBase);
	public:
		UBINT FileHandle;

		ReadFileStream();
		ReadFileStream(const UBCHAR *lpFileName);
		void Open(const UBCHAR *lpFileName);
		void Close();
		bool isEOF();
	};

	const nsBasic::Stream<unsigned char>::Interface ReadFileStream::_I{
		ReadFileStream::_Read,
		ReadFileStream::_ReadBulk,
		nullptr,
		nullptr,
		ReadFileStream::_GetPtr,
		ReadFileStream::_SetPtr,
		ReadFileStream::_Delete,
		ReadFileStream::_Destroy
	};

	class WriteFileStream :public nsBasic::Stream_WS<unsigned char>{
	private:
		typedef Stream<unsigned char> stream_base_type;
		static const stream_base_type::Interface _I;

		WriteFileStream(const WriteFileStream &) = delete; //not copyable
		WriteFileStream & operator =(const WriteFileStream &) = delete; //not copyable

		static bool _Write(stream_base_type * const ThisBase, const unsigned char *Src);
		static UINT8b _WriteBulk(stream_base_type * const ThisBase, const unsigned char *Src, UINT8b Count);
		static UINT8b _GetPtr(stream_base_type * const ThisBase);
		static bool _SetPtr(stream_base_type * const ThisBase, nsBasic::StreamSeekType SeekType, INT8b Offset);
		static void _Delete(stream_base_type * const ThisBase){ delete static_cast<WriteFileStream *>(ThisBase); }
		static void _Destroy(stream_base_type * const ThisBase);
	public:
		UBINT FileHandle;

		WriteFileStream();
		WriteFileStream(const UBCHAR *lpFileName);
		WriteFileStream(const UBCHAR *lpFileName, bool ClearPrevious);
		void Open(const UBCHAR *lpFileName, bool ClearPrevious);
		void Close();
		bool isEOF();
	};

	const nsBasic::Stream<unsigned char>::Interface WriteFileStream::_I{
		nullptr,
		nullptr,
		WriteFileStream::_Write,
		WriteFileStream::_WriteBulk,
		WriteFileStream::_GetPtr,
		WriteFileStream::_SetPtr,
		WriteFileStream::_Delete,
		WriteFileStream::_Destroy
	};

	class ReadStreamBuffer :public nsBasic::Stream_R<unsigned char>{
	private:
		typedef Stream<unsigned char> stream_base_type;
		static const stream_base_type::Interface _I;

		ReadStreamBuffer(const ReadStreamBuffer &) = delete; //not copyable
		ReadStreamBuffer & operator =(const ReadStreamBuffer &) = delete; //not copyable

		static bool _Read(stream_base_type * const ThisBase, unsigned char *Dest);
		static UINT8b _ReadBulk(stream_base_type * const ThisBase, unsigned char *Dest, UINT8b Count);
		static void _Delete(stream_base_type * const ThisBase){ delete static_cast<ReadStreamBuffer *>(ThisBase); }
		static void _Destroy(stream_base_type * const ThisBase);
	public:
		nsBasic::Stream_R<unsigned char> *Source;
		unsigned char *BufPtr;
		unsigned char *BufEnd;
		unsigned char *CurPtr;

		ReadStreamBuffer();
		ReadStreamBuffer(nsBasic::Stream_R<unsigned char> *Source);
		void clear();
		bool empty() const;
	};
	
	const nsBasic::Stream<unsigned char>::Interface ReadStreamBuffer::_I{
		ReadStreamBuffer::_Read,
		ReadStreamBuffer::_ReadBulk,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		ReadStreamBuffer::_Delete,
		ReadStreamBuffer::_Destroy
	};

	class WriteStreamBuffer :public nsBasic::Stream_W<unsigned char>{
	private:
		typedef Stream<unsigned char> stream_base_type;
		static const stream_base_type::Interface _I;

		WriteStreamBuffer(const WriteStreamBuffer &) = delete; //not copyable
		WriteStreamBuffer & operator =(const WriteStreamBuffer &) = delete; //not copyable

		static bool _Write(stream_base_type * const ThisBase, const unsigned char *Src); //the return value can only indicate whether the write operation is successful or not.
		static UINT8b _WriteBulk(stream_base_type * const ThisBase, const unsigned char *Src, UINT8b Count); //the return value can only indicate whether the write operation is successful or not.
		static void _Delete(stream_base_type * const ThisBase){ delete static_cast<WriteStreamBuffer *>(ThisBase); }
		static void _Destroy(stream_base_type * const ThisBase);
	public:
		nsBasic::Stream_W<unsigned char> *Destination;
		unsigned char *BufPtr;
		unsigned char *BufEnd;

		WriteStreamBuffer();
		WriteStreamBuffer(nsBasic::Stream_W<unsigned char> *Destination);
		void Flush();
	};

	const nsBasic::Stream<unsigned char>::Interface WriteStreamBuffer::_I{
		nullptr,
		nullptr,
		WriteStreamBuffer::_Write,
		WriteStreamBuffer::_WriteBulk,
		nullptr,
		nullptr,
		WriteStreamBuffer::_Delete,
		WriteStreamBuffer::_Destroy
	};

	class DecoderStream :public nsBasic::Stream_R<UINT4b>{
	private:
		typedef Stream<UINT4b> stream_base_type;
		static const stream_base_type::Interface _I_ANSI;
		static const stream_base_type::Interface _I_UTF8;
		static const stream_base_type::Interface _I_UTF16L;
		static const stream_base_type::Interface _I_UTF16B;

		DecoderStream(const DecoderStream &) = delete; //not copyable
		DecoderStream & operator =(const DecoderStream &) = delete; //not copyable

		static bool _Read_ANSI(stream_base_type * const ThisBase, UINT4b *Dest);
		static bool _Read_UTF8(stream_base_type * const ThisBase, UINT4b *Dest);
		static UINT8b _ReadBulk_UTF8(stream_base_type * const ThisBase, UINT4b *Dest, UINT8b Count);
		static bool _Read_UTF16L(stream_base_type * const ThisBase, UINT4b *Dest);
		static bool _Read_UTF16B(stream_base_type * const ThisBase, UINT4b *Dest);
		static void _Delete(stream_base_type * const ThisBase){ delete static_cast<DecoderStream *>(ThisBase); }
		static void _Destroy(stream_base_type * const ThisBase){}
	public:
		nsBasic::Stream_R<unsigned char> *Source;

		DecoderStream(nsBasic::Stream_R<unsigned char> *StreamSrc, UBINT Coding);
	};

	const nsBasic::Stream<UINT4b>::Interface DecoderStream::_I_ANSI{
		_Read_ANSI,
		nsBasic::Stream<UINT4b>::_ReadBulk_Default,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		DecoderStream::_Delete,
		DecoderStream::_Destroy
	};

	const nsBasic::Stream<UINT4b>::Interface DecoderStream::_I_UTF8{
		_Read_UTF8,
		_ReadBulk_UTF8,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		DecoderStream::_Delete,
		DecoderStream::_Destroy
	};

	const nsBasic::Stream<UINT4b>::Interface DecoderStream::_I_UTF16L{
		_Read_UTF16L,
		nsBasic::Stream<UINT4b>::_ReadBulk_Default,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		DecoderStream::_Delete,
		DecoderStream::_Destroy
	};

	const nsBasic::Stream<UINT4b>::Interface DecoderStream::_I_UTF16B{
		_Read_UTF16B,
		nsBasic::Stream<UINT4b>::_ReadBulk_Default,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		DecoderStream::_Delete,
		DecoderStream::_Destroy
	};

	class CoderStream :public nsBasic::Stream_W<UINT4b>{
	private:
		typedef Stream<UINT4b> stream_base_type;
		static const stream_base_type::Interface _I_ANSI;
		static const stream_base_type::Interface _I_UTF8;
		static const stream_base_type::Interface _I_UTF16L;
		static const stream_base_type::Interface _I_UTF16B;

		CoderStream(const CoderStream &) = delete; //not copyable
		CoderStream & operator =(const CoderStream &) = delete; //not copyable

		static bool _Write_ANSI(stream_base_type * const ThisBase, const UINT4b *Src);
		static bool _Write_UTF8(stream_base_type * const ThisBase, const UINT4b *Src);
		static bool _Write_UTF16L(stream_base_type * const ThisBase, const UINT4b *Src);
		static bool _Write_UTF16B(stream_base_type * const ThisBase, const UINT4b *Src);
		static void _Delete(stream_base_type * const ThisBase){ delete static_cast<CoderStream *>(ThisBase); }
		static void _Destroy(stream_base_type * const ThisBase){}
	public:
		nsBasic::Stream_W<unsigned char> *Destination;

		CoderStream(nsBasic::Stream_W<unsigned char> *StreamDest, UBINT Coding);
	};

	const nsBasic::Stream<UINT4b>::Interface CoderStream::_I_ANSI{
		nullptr,
		nullptr,
		_Write_ANSI,
		nsBasic::Stream<UINT4b>::_WriteBulk_Default,
		nullptr,
		nullptr,
		CoderStream::_Delete,
		CoderStream::_Destroy
	};

	const nsBasic::Stream<UINT4b>::Interface CoderStream::_I_UTF8{
		nullptr,
		nullptr,
		_Write_UTF8,
		nsBasic::Stream<UINT4b>::_WriteBulk_Default,
		nullptr,
		nullptr,
		CoderStream::_Delete,
		CoderStream::_Destroy
	};

	const nsBasic::Stream<UINT4b>::Interface CoderStream::_I_UTF16L{
		nullptr,
		nullptr,
		_Write_UTF16L,
		nsBasic::Stream<UINT4b>::_WriteBulk_Default,
		nullptr,
		nullptr,
		CoderStream::_Delete,
		CoderStream::_Destroy
	};

	const nsBasic::Stream<UINT4b>::Interface CoderStream::_I_UTF16B{
		nullptr,
		nullptr,
		_Write_UTF16B,
		nsBasic::Stream<UINT4b>::_WriteBulk_Default,
		nullptr,
		nullptr,
		CoderStream::_Delete,
		CoderStream::_Destroy
	};

	class MemStream :public nsBasic::Stream_RWS<unsigned char>{
	private:
		typedef Stream<unsigned char> stream_base_type;
		static const stream_base_type::Interface _I;

		void *ArrPtr;
		UBINT _Offset;
		UBINT Length;

		static bool _Read(stream_base_type * const ThisBase, unsigned char *Dest);
		static UINT8b _ReadBulk(stream_base_type * const ThisBase, unsigned char *Dest, UINT8b Count);
		static bool _Write(stream_base_type * const ThisBase, const unsigned char *Src);
		static UINT8b _WriteBulk(stream_base_type * const ThisBase, const unsigned char *Src, UINT8b Count);
		static UINT8b _GetPtr(stream_base_type * const ThisBase);
		static bool _SetPtr(stream_base_type * const ThisBase, nsBasic::StreamSeekType SeekType, INT8b Offset);
		static void _Delete(stream_base_type * const ThisBase){ delete static_cast<MemStream *>(ThisBase); }
		static void _Destroy(stream_base_type * const ThisBase){};
	public:
		MemStream(void * const Addr, UBINT Length);
		UBINT Offset(){ return this->_Offset; }
	};
	const nsBasic::Stream<unsigned char>::Interface MemStream::_I{
		MemStream::_Read,
		MemStream::_ReadBulk,
		MemStream::_Write,
		MemStream::_WriteBulk,
		MemStream::_GetPtr,
		MemStream::_SetPtr,
		MemStream::_Delete,
		MemStream::_Destroy
	};

	template<class T> struct MemFile{
	private:
		MemFile(const MemFile &) = delete; //not copyable
		MemFile & operator =(const MemFile &) = delete; //not copyable
		void * operator new(size_t) = delete; //using operator new is prohibited
		void operator delete(void *) = delete; //using operator delete is prohibited

		static UBINT _cdecl _MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	public:
		void *PrevObj;
		void *NextObj;
		nsBasic::BlockPage *CurPage;
		UBINT CurOffset;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		UBINT BlockSize;

		MemFile();
		inline operator nsBasic::ObjGeneral() const{ return *(nsBasic::ObjGeneral *)this; }
		UBINT PageCount();
		UBINT BlockCount();
		inline T *CurAddr(){ return (T *)(this->CurPage->PageAddr) + this->CurOffset; }
		~MemFile();
	};

	extern bool CreateFolder(UBCHAR *lpPath);
	extern void *ReadFileToMem(UBCHAR *lpFileName,UBINT *BufferLength);

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	ReadFileStream::ReadFileStream(){
		this->lpInterface = const_cast<ReadFileStream::stream_base_type::Interface *>(&ReadFileStream::_I);
		this->FileHandle = (UBINT)-1;
	}
	ReadFileStream::ReadFileStream(const UBCHAR *lpFileName){
		UBINT TmpHandle = _OpenFile_Read(lpFileName);
		if ((UBINT)-1 == TmpHandle)throw new std::exception("failed to open the required file.");
		this->lpInterface = const_cast<ReadFileStream::stream_base_type::Interface *>(&ReadFileStream::_I);
		this->FileHandle = TmpHandle;
	}
	bool ReadFileStream::_Read(ReadFileStream::stream_base_type * const ThisBase, unsigned char *Dest){
		ReadFileStream *This = static_cast<ReadFileStream *>(ThisBase);
		if (_ReadFile(This->FileHandle, Dest, 1) > 0)return true; else return false;
	}
	UINT8b ReadFileStream::_ReadBulk(ReadFileStream::stream_base_type * const ThisBase, unsigned char *Dest, UINT8b Count){
		ReadFileStream *This = static_cast<ReadFileStream *>(ThisBase);
		UINT8b RetValue = 0;
		while (Count > 0x80000000){
			UINT4b TmpValue = _ReadFile(This->FileHandle, Dest, 0x80000000);
			if (TmpValue < 0x80000000)return RetValue + TmpValue;
			RetValue += TmpValue;
			Dest += TmpValue;
			Count -= TmpValue;
		}
		return RetValue + _ReadFile(This->FileHandle, Dest, (UINT4b)Count);
	}
	UINT8b ReadFileStream::_GetPtr(ReadFileStream::stream_base_type * const ThisBase){
		ReadFileStream *This = static_cast<ReadFileStream *>(ThisBase);
		UINT8b Offset;
		if (false == _GetFilePtr(This->FileHandle, &Offset))throw new std::exception("Failed to retrieve file pointer.");
		return Offset;
	}
	bool ReadFileStream::_SetPtr(ReadFileStream::stream_base_type * const ThisBase, nsBasic::StreamSeekType SeekType, INT8b Offset){
		ReadFileStream *This = static_cast<ReadFileStream *>(ThisBase);
		return _SetFilePtr(This->FileHandle, Offset, SeekType);
	}
	void ReadFileStream::Open(const UBCHAR *lpFileName){
		UBINT TmpHandle = _OpenFile_Read(lpFileName);
		if ((UBINT)-1 == TmpHandle)throw new std::exception("failed to open the required file.");
		else{
			if ((UBINT)-1 != this->FileHandle)_CloseFile(this->FileHandle);
			this->FileHandle = TmpHandle;
		}
	}
	void ReadFileStream::Close(){
		if ((UBINT)-1 != this->FileHandle){
			_CloseFile(this->FileHandle);
			this->FileHandle = (UBINT)-1;
		}
	}
	bool ReadFileStream::isEOF(){
		if ((UBINT)-1 == this->FileHandle)return true;
		UINT8b FileLen, FilePtr;
		_GetFileLen(this->FileHandle, &FileLen);
		_GetFilePtr(this->FileHandle, &FilePtr);
		if (FileLen == FilePtr)return true; else return false;
	}
	void ReadFileStream::_Destroy(ReadFileStream::stream_base_type * const ThisBase){
		ReadFileStream *This = static_cast<ReadFileStream *>(ThisBase);
		This->Close();
	}
	
	WriteFileStream::WriteFileStream(){
		this->lpInterface = const_cast<WriteFileStream::stream_base_type::Interface *>(&WriteFileStream::_I);
		this->FileHandle = (UBINT)-1;
	}
	WriteFileStream::WriteFileStream(const UBCHAR *lpFileName){
		UBINT TmpHandle = _OpenFile_Write(lpFileName);
		if ((UBINT)-1 == TmpHandle)throw new std::exception("failed to open the required file.");
		this->lpInterface = const_cast<WriteFileStream::stream_base_type::Interface *>(&WriteFileStream::_I);
		this->FileHandle = TmpHandle;
	}
	WriteFileStream::WriteFileStream(const UBCHAR *lpFileName, bool ClearPrevious){
		UBINT TmpHandle;
		if (ClearPrevious)TmpHandle = _OpenFile_Write(lpFileName); else TmpHandle = _OpenFile_Write_OpenExist(lpFileName);
		if ((UBINT)-1 == TmpHandle)throw new std::exception("failed to open the required file.");
		this->lpInterface = const_cast<WriteFileStream::stream_base_type::Interface *>(&WriteFileStream::_I);
		this->FileHandle = TmpHandle;
	}
	bool WriteFileStream::_Write(WriteFileStream::stream_base_type * const ThisBase, const unsigned char *Src){
		WriteFileStream *This = static_cast<WriteFileStream *>(ThisBase);
		if (_WriteFile(This->FileHandle, Src, 1) > 0)return true; else return false;
	}
	UINT8b WriteFileStream::_WriteBulk(WriteFileStream::stream_base_type * const ThisBase, const unsigned char *Src, UINT8b Count){
		WriteFileStream *This = static_cast<WriteFileStream *>(ThisBase);
		UINT8b RetValue = 0;
		while (Count > 0x80000000){
			UINT4b TmpValue = _WriteFile(This->FileHandle, Src, 0x80000000);
			RetValue += TmpValue;
			if (TmpValue < 0x80000000)return RetValue;
			Src += TmpValue;
			Count -= TmpValue;
		}
		return RetValue + _WriteFile(This->FileHandle, Src, (UINT4b)Count);
	}
	UINT8b WriteFileStream::_GetPtr(WriteFileStream::stream_base_type * const ThisBase){
		WriteFileStream *This = static_cast<WriteFileStream *>(ThisBase);
		UINT8b Offset;
		if (false == _GetFilePtr(This->FileHandle, &Offset))throw new std::exception("Failed to retrieve file pointer.");
		return Offset;
	}
	bool WriteFileStream::_SetPtr(WriteFileStream::stream_base_type * const ThisBase, nsBasic::StreamSeekType SeekType, INT8b Offset){
		WriteFileStream *This = static_cast<WriteFileStream *>(ThisBase);
		return _SetFilePtr(This->FileHandle, Offset, SeekType);
	}
	void WriteFileStream::Open(const UBCHAR *lpFileName, bool ClearPrevious){
		UBINT TmpHandle;
		if (ClearPrevious)TmpHandle = _OpenFile_Write(lpFileName); else TmpHandle = _OpenFile_Write_OpenExist(lpFileName);
		if ((UBINT)-1 == TmpHandle)throw new std::exception("failed to open the required file.");
		else{
			if ((UBINT)-1 != this->FileHandle)_CloseFile(this->FileHandle);
			this->FileHandle = TmpHandle;
		}
	}
	void WriteFileStream::Close(){
		if ((UBINT)-1 != this->FileHandle){
			_CloseFile(this->FileHandle);
			this->FileHandle = (UBINT)-1;
		}
	}
	void WriteFileStream::_Destroy(WriteFileStream::stream_base_type * const ThisBase){
		WriteFileStream *This = static_cast<WriteFileStream *>(ThisBase);
		This->Close();
	}

	ReadStreamBuffer::ReadStreamBuffer(){
		this->BufPtr = (unsigned char *)nsBasic::GetPage(nsEnv::SysPageSize);
		if (nullptr == this->BufPtr)throw std::exception("Failed to create stream buffer.");
		else{
			this->lpInterface = const_cast<ReadStreamBuffer::stream_base_type::Interface *>(&ReadStreamBuffer::_I);
			this->Source = nullptr;
			this->BufEnd = this->BufPtr + nsEnv::SysPageSize;
			this->CurPtr = this->BufEnd;
		}
	}
	ReadStreamBuffer::ReadStreamBuffer(nsBasic::Stream_R<unsigned char> *Source){
		this->BufPtr = (unsigned char *)nsBasic::GetPage(nsEnv::SysPageSize);
		if (nullptr == this->BufPtr)throw std::exception("Failed to create stream buffer.");
		else{
			this->lpInterface = const_cast<ReadStreamBuffer::stream_base_type::Interface *>(&ReadStreamBuffer::_I);
			this->Source = Source;
			this->BufEnd = this->BufPtr + nsEnv::SysPageSize;
			this->CurPtr = this->BufEnd;
		}
	}
	bool ReadStreamBuffer::_Read(ReadStreamBuffer::stream_base_type * const ThisBase, unsigned char *Dest){
		ReadStreamBuffer *This = static_cast<ReadStreamBuffer *>(ThisBase);
		if (This->CurPtr == This->BufEnd){
			if (nullptr == This->Source)return false;
			This->BufEnd = This->BufPtr + (size_t)This->Source->ReadBulk(This->BufPtr, nsEnv::SysPageSize);
			This->CurPtr = This->BufPtr;
			if (This->CurPtr == This->BufEnd)return false;
		}
		*Dest = *This->CurPtr;
		This->CurPtr++;
		return true;
	}
	UINT8b ReadStreamBuffer::_ReadBulk(ReadStreamBuffer::stream_base_type * const ThisBase, unsigned char *Dest, UINT8b Count){
		ReadStreamBuffer *This = static_cast<ReadStreamBuffer *>(ThisBase);
		UINT8b RetValue = 0;
		if (This->CurPtr != This->BufEnd){
			RetValue = This->BufEnd - This->CurPtr;
			if (Count < RetValue){
				memcpy(Dest, This->CurPtr, (size_t)Count);
				This->CurPtr += (size_t)Count;
				return Count;
			}
			memcpy(Dest, This->CurPtr, (size_t)RetValue);
			This->CurPtr = This->BufPtr;
			This->BufEnd = This->BufPtr;
			Count -= RetValue;
			Dest += (size_t)RetValue;
		}
		if (Count>0){
			while (Count > 0x80000000){
				UINT4b TmpValue = (UINT4b)This->Source->ReadBulk(Dest, 0x80000000);
				RetValue += TmpValue;
				if (TmpValue < 0x80000000)return RetValue;
				Dest += TmpValue;
				Count -= TmpValue;
			}
			UINT4b WholePageSize = (UINT4b)(Count - (Count&(nsEnv::SysPageSize - 1))); //suppose nsEnv::SysPageSize and sizeof(UBINT) is the power of 2
			UINT4b WholePageRetValue = (UINT4b)This->Source->ReadBulk(Dest, WholePageSize);
			RetValue += WholePageRetValue;
			if (WholePageRetValue < WholePageSize)return RetValue;
			if (Count>WholePageSize){
				Dest += WholePageSize;
				Count -= WholePageSize;
				UINT4b RemainSize = (UINT4b)This->Source->ReadBulk(This->BufPtr, nsEnv::SysPageSize);
				This->BufEnd = This->BufPtr + RemainSize;
				if (RemainSize > Count){
					memcpy(Dest, This->BufPtr, (size_t)Count);
					This->CurPtr = This->BufPtr + (size_t)Count;
					RetValue += Count;
				}
				else{
					memcpy(Dest, This->BufPtr, RemainSize);
					This->CurPtr = This->BufEnd;
					RetValue += RemainSize;
				}
			}
		}
		return RetValue;
	}
	void ReadStreamBuffer::clear(){
		this->CurPtr = this->BufPtr;
		this->BufEnd = this->BufPtr;
	}
	bool ReadStreamBuffer::empty() const{ return this->CurPtr == this->BufEnd; }
	void ReadStreamBuffer::_Destroy(ReadStreamBuffer::stream_base_type * const ThisBase){
		ReadStreamBuffer *This = static_cast<ReadStreamBuffer *>(ThisBase);
		nsBasic::unGetPage((void *)This->BufPtr, nsEnv::SysPageSize);
	}

	WriteStreamBuffer::WriteStreamBuffer(){
		this->BufPtr = (unsigned char *)nsBasic::GetPage(nsEnv::SysPageSize);
		if (nullptr == this->BufPtr)throw std::exception("Failed to create stream buffer.");
		else{
			this->lpInterface = const_cast<WriteStreamBuffer::stream_base_type::Interface *>(&WriteStreamBuffer::_I);
			this->Destination = nullptr;
			this->BufEnd = this->BufPtr + nsEnv::SysPageSize;
		}
	}
	WriteStreamBuffer::WriteStreamBuffer(nsBasic::Stream_W<unsigned char> *Destination){
		this->BufPtr = (unsigned char *)nsBasic::GetPage(nsEnv::SysPageSize);
		if (nullptr == this->BufPtr)throw std::exception("Failed to create stream buffer.");
		else{
			this->lpInterface = const_cast<WriteStreamBuffer::stream_base_type::Interface *>(&WriteStreamBuffer::_I);
			this->Destination = Destination;
			this->BufEnd = this->BufPtr + nsEnv::SysPageSize;
		}
	}
	bool WriteStreamBuffer::_Write(WriteStreamBuffer::stream_base_type * const ThisBase, const unsigned char *Src){
		WriteStreamBuffer *This = static_cast<WriteStreamBuffer *>(ThisBase);
		if (nullptr == This->Destination)return false;
		else{
			*(unsigned char *)This->BufPtr = *Src;
			This->BufPtr++;
			if (This->BufPtr >= This->BufEnd){
				This->BufPtr -= nsEnv::SysPageSize;
				if (This->Destination->WriteBulk(This->BufPtr, nsEnv::SysPageSize)<nsEnv::SysPageSize)return false;
			}
			return true;
		}
	}
	UINT8b WriteStreamBuffer::_WriteBulk(WriteStreamBuffer::stream_base_type * const ThisBase, const unsigned char *Src, UINT8b Count){
		WriteStreamBuffer *This = static_cast<WriteStreamBuffer *>(ThisBase);
		if (nullptr == This->Destination)return 0;
		UINT8b RetValue = 0;
		if ((UBINT)(This->BufEnd - This->BufPtr) < nsEnv::SysPageSize){
			RetValue = This->BufEnd - This->BufPtr;
			if (Count < RetValue){
				memcpy(This->BufPtr, Src, (size_t)Count);
				This->BufPtr += (size_t)Count;
				return Count;
			}
			memcpy(This->BufPtr, Src, (size_t)RetValue);
			This->BufPtr = This->BufEnd - nsEnv::SysPageSize;
			if (This->Destination->WriteBulk(This->BufPtr, nsEnv::SysPageSize) < nsEnv::SysPageSize)return 0;
			Count -= RetValue;
			Src += (size_t)RetValue;
		}
		if (Count>0){
			while (Count > 0x80000000){
				UINT4b TmpValue = (UINT4b)This->Destination->WriteBulk(Src, 0x80000000);
				RetValue += TmpValue;
				if (TmpValue < 0x80000000)return RetValue;
				Src += TmpValue;
				Count -= TmpValue;
			}
			UINT4b WholePageSize = (UINT4b)(Count - (Count&(nsEnv::SysPageSize - 1))); //suppose nsEnv::SysPageSize and sizeof(UBINT) is the power of 2
			UINT4b WholePageRetValue = (UINT4b)This->Destination->WriteBulk(Src, WholePageSize);
			RetValue += WholePageRetValue;
			if (WholePageRetValue < WholePageSize)return RetValue;
			if (Count>WholePageSize){
				Src += WholePageSize;
				Count -= WholePageSize;
				memcpy(This->BufPtr, Src, (size_t)Count);
				This->BufPtr += (size_t)Count;
				RetValue += Count;
			}
		}
		return RetValue;
	}
	void WriteStreamBuffer::Flush(){
		if (nullptr != this->Destination){
			UBINT FlushLength = this->BufPtr - this->BufEnd + nsEnv::SysPageSize;
			if (this->Destination->WriteBulk(this->BufEnd - nsEnv::SysPageSize, FlushLength) < FlushLength)throw std::exception("Stream flush failed.");
			else this->BufPtr = this->BufEnd - nsEnv::SysPageSize;
		}
	}
	void WriteStreamBuffer::_Destroy(WriteStreamBuffer::stream_base_type * const ThisBase){
		WriteStreamBuffer *This = static_cast<WriteStreamBuffer *>(ThisBase);
		try{ This->Flush(); } catch (...){}
		nsBasic::unGetPage((void *)This->BufPtr, nsEnv::SysPageSize);
	}

	MemStream::MemStream(void * const lpData, UBINT Length){
		this->lpInterface = const_cast<MemStream::stream_base_type::Interface *>(&MemStream::_I);
		this->ArrPtr = lpData;
		this->_Offset = 0;
		this->Length = Length;
	}
	bool MemStream::_Read(MemStream::stream_base_type * const ThisBase, unsigned char *Dest){
		MemStream *This = static_cast<MemStream *>(ThisBase);
		if (This->_Offset >= This->Length)return false;
		else{
			*Dest = *(unsigned char *)((UBINT)This->ArrPtr + This->_Offset);
			This->_Offset++;
			return true;
		}
	}
	UINT8b MemStream::_ReadBulk(MemStream::stream_base_type * const ThisBase, unsigned char *Dest, UINT8b Count){
		MemStream *This = static_cast<MemStream *>(ThisBase);
		if (Count + This->_Offset > This->Length)Count = This->Length - This->_Offset;
		memcpy(Dest, (void *)((UBINT)This->ArrPtr + This->_Offset), (UBINT)Count);
		This->_Offset += (UBINT)Count;
		return Count;
	}
	bool MemStream::_Write(MemStream::stream_base_type * const ThisBase, const unsigned char *Src){
		MemStream *This = static_cast<MemStream *>(ThisBase);
		if (This->_Offset >= This->Length)return false;
		else{
			*(unsigned char *)((UBINT)This->ArrPtr + This->_Offset) = *Src;
			This->_Offset++;
			return true;
		}
	}
	UINT8b MemStream::_WriteBulk(MemStream::stream_base_type * const ThisBase, const unsigned char *Src, UINT8b Count){
		MemStream *This = static_cast<MemStream *>(ThisBase);
		if (Count + This->_Offset > This->Length)Count = This->Length - This->_Offset;
		memcpy((void *)((UBINT)This->ArrPtr + This->_Offset), Src, (UBINT)Count);
		This->_Offset += (UBINT)Count;
		return Count;
	}
	UINT8b MemStream::_GetPtr(MemStream::stream_base_type * const ThisBase){
		MemStream *This = static_cast<MemStream *>(ThisBase);
		return This->_Offset;
	}
	bool MemStream::_SetPtr(MemStream::stream_base_type * const ThisBase, nsBasic::StreamSeekType SeekType, INT8b Offset){
		MemStream *This = static_cast<MemStream *>(ThisBase);
		if (nsBasic::StreamSeekType::FROM_BEGIN == SeekType){
			if (Offset < 0)return false;
			else if ((UBINT)Offset > This->Length)return false;
			else This->_Offset = (UBINT)Offset;
		}
		else if (nsBasic::StreamSeekType::FROM_CURRENT_POS == SeekType){
			if (Offset + This->_Offset<0 || Offset + This->_Offset>This->Length)return false;
			else This->_Offset += (UBINT)Offset;
		}
		else if (nsBasic::StreamSeekType::FROM_END == SeekType){
			if (Offset > 0)return false;
			else if (Offset + This->Length < 0)return false;
			else This->_Offset = This->Length + (UBINT)Offset;
		}
		return true;
	}

	DecoderStream::DecoderStream(nsBasic::Stream_R<unsigned char> *StreamSrc, UBINT Coding){
		this->Source = StreamSrc;
		switch(Coding){
			case nsCharCoding::ANSI:
				this->lpInterface = const_cast<DecoderStream::stream_base_type::Interface *>(&DecoderStream::_I_ANSI);
				break;
			case nsCharCoding::UTF8:
				this->lpInterface = const_cast<DecoderStream::stream_base_type::Interface *>(&DecoderStream::_I_UTF8);
				break;
			case nsCharCoding::UTF16L:
				this->lpInterface = const_cast<DecoderStream::stream_base_type::Interface *>(&DecoderStream::_I_UTF16L);
				break;
			case nsCharCoding::UTF16B:
				this->lpInterface = const_cast<DecoderStream::stream_base_type::Interface *>(&DecoderStream::_I_UTF16B);
				break;
			default:
				this->lpInterface = const_cast<DecoderStream::stream_base_type::Interface *>(&DecoderStream::_I_UTF8);
				break;
		}
	}
	bool DecoderStream::_Read_ANSI(DecoderStream::stream_base_type * const ThisBase, UINT4b *Dest){
		DecoderStream *This = static_cast<DecoderStream *>(ThisBase);
		if (nullptr == This->Source)return false;
		else{
			unsigned char ByteRecv;
			if (false == This->Source->Read(&ByteRecv))return false;
			else{ *Dest = (UINT4b)ByteRecv; return true; }
		}
	}
	bool DecoderStream::_Read_UTF8(DecoderStream::stream_base_type * const ThisBase, UINT4b *Dest){
		DecoderStream *This = static_cast<DecoderStream *>(ThisBase);
		if (nullptr == This->Source)return false;
		else{
			unsigned char ByteRecv,DecoderState = nsCharCoding::_utf8_decode_success;
			UINT4b CodePoint = 0;
			while (true){
				if (false == This->Source->Read(&ByteRecv))return false;
				nsCharCoding::_utf8_decode(&DecoderState, &CodePoint, ByteRecv);
				if (nsCharCoding::_utf8_decode_success == DecoderState){ *Dest = CodePoint; return true; }
				else if (nsCharCoding::_utf8_decode_failed == DecoderState)return false;
			}
		}
	}
	UINT8b DecoderStream::_ReadBulk_UTF8(DecoderStream::stream_base_type * const ThisBase, UINT4b *Dest, UINT8b Count){
		DecoderStream *This = static_cast<DecoderStream *>(ThisBase);
		if (nullptr == This->Source)return 0;
		else{
			unsigned char ByteRecv, DecoderState = nsCharCoding::_utf8_decode_success;
			UINT4b CodePoint = 0;
			UINT8b CPDecoded = 0;
			while (CPDecoded < Count){
				if (false == This->Source->Read(&ByteRecv))break;
				nsCharCoding::_utf8_decode(&DecoderState, &CodePoint, ByteRecv);
				if (nsCharCoding::_utf8_decode_success == DecoderState){ Dest[CPDecoded] = CodePoint; CPDecoded++; }
				else if (nsCharCoding::_utf8_decode_failed == DecoderState)break;
			}
			return CPDecoded;
		}
	}
	bool DecoderStream::_Read_UTF16L(DecoderStream::stream_base_type * const ThisBase, UINT4b *Dest){
		DecoderStream *This = static_cast<DecoderStream *>(ThisBase);
		if (nullptr == This->Source)return false;
		else{
			unsigned char ByteRecv1, ByteRecv2;
			UINT4b CodePoint;

			if (false == This->Source->Read(&ByteRecv1) || false == This->Source->Read(&ByteRecv2))return false;
			CodePoint = (((UINT4b)ByteRecv2) << 8) + (UINT4b)ByteRecv1;
			if (0xDC00 == (CodePoint & 0xFC00))return false;
			else if (0xD800 == (CodePoint & 0xFC00)){
				if (false == This->Source->Read(&ByteRecv1) || false == This->Source->Read(&ByteRecv2))return false;
				if (0xDC != (ByteRecv2 & 0xFC))return false;
				CodePoint = ((CodePoint & 0x3FF) << 10) + 0x10000;
				CodePoint += ((((UINT4b)ByteRecv2) << 8) + (UINT4b)ByteRecv1) & 0x3FF;
				if (CodePoint >= 0x110000)return false;  //restricted by RFC 3629
				else{
					*Dest = CodePoint;
					return true;
				}
			}
			else{
				*Dest = CodePoint;
				return true;
			}
		}
	}
	bool DecoderStream::_Read_UTF16B(DecoderStream::stream_base_type * const ThisBase, UINT4b *Dest){
		DecoderStream *This = static_cast<DecoderStream *>(ThisBase);
		if (nullptr == This->Source)return false;
		else{
			unsigned char ByteRecv1, ByteRecv2;
			UINT4b CodePoint;

			if (false == This->Source->Read(&ByteRecv1) || false == This->Source->Read(&ByteRecv2))return false;
			CodePoint = (((UINT4b)ByteRecv1) << 8) + (UINT4b)ByteRecv2;
			if (0xDC00 == (CodePoint & 0xFC00))return false;
			else if (0xD800 == (CodePoint & 0xFC00)){
				if (false == This->Source->Read(&ByteRecv1) || false == This->Source->Read(&ByteRecv2))return false;
				if (0xDC != (ByteRecv2 & 0xFC))return false;
				CodePoint = ((CodePoint & 0x3FF) << 10) + 0x10000;
				CodePoint += ((((UINT4b)ByteRecv1) << 8) + (UINT4b)ByteRecv2) & 0x3FF;
				if (CodePoint >= 0x110000)return false;  //restricted by RFC 3629
				else{
					*Dest = CodePoint;
					return true;
				}
			}
			else{
				*Dest = CodePoint;
				return true;
			}
		}
	}

	CoderStream::CoderStream(nsBasic::Stream_W<unsigned char> *StreamDest, UBINT Coding){
		this->Destination = StreamDest;
		switch (Coding){
		case nsCharCoding::ANSI:
			this->lpInterface = const_cast<CoderStream::stream_base_type::Interface *>(&CoderStream::_I_ANSI);
			break;
		case nsCharCoding::UTF8:
			this->lpInterface = const_cast<CoderStream::stream_base_type::Interface *>(&CoderStream::_I_UTF8);
			break;
		case nsCharCoding::UTF16L:
			this->lpInterface = const_cast<CoderStream::stream_base_type::Interface *>(&CoderStream::_I_UTF16L);
			break;
		case nsCharCoding::UTF16B:
			this->lpInterface = const_cast<CoderStream::stream_base_type::Interface *>(&CoderStream::_I_UTF16B);
			break;
		default:
			this->lpInterface = const_cast<CoderStream::stream_base_type::Interface *>(&CoderStream::_I_UTF8);
			break;
		}
	}
	bool CoderStream::_Write_ANSI(CoderStream::stream_base_type * const ThisBase, const UINT4b *Src){
		CoderStream *This = static_cast<CoderStream *>(ThisBase);
		if (nullptr == This->Destination)return false;
		else{
			if (false == This->Destination->Write((unsigned char *)Src))return false; //little endian assumed
			else return true;
		}
	}
	bool CoderStream::_Write_UTF8(CoderStream::stream_base_type * const ThisBase, const UINT4b *Src){
		CoderStream *This = static_cast<CoderStream *>(ThisBase);
		if (nullptr == This->Destination)return false;
		else{
			UINT4b IntRecv = *Src, LCnt;
			unsigned char ByteSend;

			if (IntRecv >= 0x110000 || 0xD800 == (IntRecv & 0x1FF800))return false; //restricted by RFC 3629
			else if (IntRecv < 0x80){ IntRecv = 0x80000000 + (IntRecv << 24); LCnt = 1; }
			else if (IntRecv < 0x800){ IntRecv = 0x40000000 + (IntRecv << 18); LCnt = 2; }
			else if (IntRecv < 0x10000){ IntRecv = 0x60000000 + (IntRecv << 12); LCnt = 3; }
			else{ IntRecv = 0x70000000 + (IntRecv << 6); LCnt = 4; }
			for (; LCnt>0; LCnt--){
				ByteSend = (unsigned char)(IntRecv >> 24) + 0x80;
				if (false == This->Destination->Write(&ByteSend))return false;
				IntRecv &= 0xFFFFFF;
				IntRecv <<= 6;
			}
			return true;
		}
	}
	bool CoderStream::_Write_UTF16L(CoderStream::stream_base_type * const ThisBase, const UINT4b *Src){
		CoderStream *This = static_cast<CoderStream *>(ThisBase);
		if (nullptr == This->Destination)return false;
		else{
			UINT4b IntRecv = *Src;

			if (IntRecv >= 0x110000 || 0xD800 == (IntRecv & 0x1FF800))return 1; //restricted by RFC 3629
			else if (IntRecv >= 0x10000){
				unsigned char ByteSend[4];
				IntRecv -= 0x10000;
				IntRecv = ((IntRecv & 0x3FF) << 16) + ((IntRecv & 0xFFC00) >> 10) + 0xDC00D800;
				ByteSend[0] = IntRecv & 0xFF;
				IntRecv >>= 8;
				ByteSend[1] = IntRecv & 0xFF;
				IntRecv >>= 8;
				ByteSend[2] = IntRecv & 0xFF;
				IntRecv >>= 8;
				ByteSend[3] = IntRecv & 0xFF;
				if (4 == This->Destination->WriteBulk(ByteSend, 4))return 1; else return 0;
			}
			else{
				unsigned char ByteSend[2];
				ByteSend[0] = IntRecv & 0xFF;
				IntRecv >>= 8;
				ByteSend[1] = IntRecv & 0xFF;
				if (2 == This->Destination->WriteBulk(ByteSend, 2))return 1; else return 0;
			}
		}
	}
	bool CoderStream::_Write_UTF16B(CoderStream::stream_base_type * const ThisBase, const UINT4b *Src){
		CoderStream *This = static_cast<CoderStream *>(ThisBase);
		if (nullptr == This->Destination)return false;
		else{
			UINT4b IntRecv = *Src;

			if (IntRecv >= 0x110000 || 0xD800 == (IntRecv & 0x1FF800))return 1; //restricted by RFC 3629
			else if (IntRecv >= 0x10000){
				unsigned char ByteSend[4];
				IntRecv -= 0x10000;
				IntRecv = ((IntRecv & 0x3FF) << 16) + ((IntRecv & 0xFFC00) >> 10) + 0xDC00D800;
				ByteSend[1] = IntRecv & 0xFF;
				IntRecv >>= 8;
				ByteSend[0] = IntRecv & 0xFF;
				IntRecv >>= 8;
				ByteSend[3] = IntRecv & 0xFF;
				IntRecv >>= 8;
				ByteSend[2] = IntRecv & 0xFF;
				if (4 == This->Destination->WriteBulk(ByteSend, 4))return 1; else return 0;
			}
			else{
				unsigned char ByteSend[2];
				ByteSend[1] = IntRecv & 0xFF;
				IntRecv >>= 8;
				ByteSend[0] = IntRecv & 0xFF;
				if (2 == This->Destination->WriteBulk(ByteSend, 2))return 1; else return 0;
			}
		}
	}

	template <class T> UBINT _cdecl MemFile<T>::_MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...){
		MemFile<T> *MyFile = (MemFile<T> *)lpObj;
		nsBasic::BlockPage *lpPage1 = (nsBasic::BlockPage *)MyFile->CurPage, *lpPage2;
		if (MSG_STREAM_READ == Msg){
			va_list args;
			va_start(args, Msg);
			void *TgtBuf = va_arg(args, void *);

			if (nullptr == lpPage1->PageAddr)return 1;
			else{
				memcpy(TgtBuf, (void *)((UBINT)(lpPage1->PageAddr) + MyFile->CurOffset*sizeof(T)), sizeof(T));
				MyFile->CurOffset++;
				if (MyFile->CurOffset >= lpPage1->BlockCntr){ MyFile->CurPage = (nsBasic::BlockPage *)lpPage1->NextPage; MyFile->CurOffset = 0; }
				return 0;
			}
		}
		else if (MSG_STREAM_READBULK == Msg){
			va_list args;
			va_start(args, Msg);
			UBINT TgtBuf = va_arg(args, UBINT), BlkCnt = va_arg(args, UBINT), BlkCopied = 0;

			if (nullptr == lpPage1->PageAddr)return BlkCopied;
			else if (BlkCnt<lpPage1->BlockCntr - MyFile->CurOffset){
				memcpy((void *)TgtBuf, (void *)((UBINT)(lpPage1->PageAddr) + MyFile->CurOffset*sizeof(T)), BlkCnt*sizeof(T));
				MyFile->CurOffset += BlkCnt;
				BlkCopied += BlkCnt;
				return BlkCopied;
			}
			else{
				memcpy((void *)TgtBuf, (void *)((UBINT)(lpPage1->PageAddr) + MyFile->CurOffset*sizeof(T)), (lpPage1->BlockCntr - MyFile->CurOffset)*sizeof(T));
				BlkCopied = lpPage1->BlockCntr - MyFile->CurOffset;
				BlkCnt -= BlkCopied;
				TgtBuf += BlkCopied*sizeof(T); //Currently BlkCopied=lpPage1->BlockCntr-MyFile->CurOffset;
				MyFile->CurPage = (nsBasic::BlockPage *)lpPage1->NextPage;
				MyFile->CurOffset = 0;
				lpPage1 = (nsBasic::BlockPage *)MyFile->CurPage;
				while (1){
					if (nullptr == lpPage1->PageAddr)return BlkCopied;
					else if (BlkCnt<lpPage1->BlockCntr){
						memcpy((void *)TgtBuf, (void *)(lpPage1->PageAddr), BlkCnt*sizeof(T));
						MyFile->CurOffset += BlkCnt;
						BlkCopied += BlkCnt;
						return BlkCopied;
					}
					else{
						memcpy((void *)TgtBuf, (void *)(lpPage1->PageAddr), lpPage1->BlockCntr*sizeof(T));
						BlkCnt -= lpPage1->BlockCntr;
						BlkCopied += lpPage1->BlockCntr;
						TgtBuf += lpPage1->BlockCntr*sizeof(T);
						MyFile->CurPage = (nsBasic::BlockPage *)lpPage1->NextPage;
						lpPage1 = (nsBasic::BlockPage *)MyFile->CurPage;
					}
				}
			}
		}
		if (MSG_STREAM_WRITE == Msg){
			va_list args;
			va_start(args, Msg);
			void *SrcBuf = va_arg(args, void *);
			if (nullptr != lpPage1->PageAddr){
				memcpy((void *)((UBINT)lpPage1->PageAddr + MyFile->CurOffset*sizeof(T)), SrcBuf, sizeof(T));
				MyFile->CurOffset++;
				if (MyFile->CurOffset >= lpPage1->BlockCntr){ MyFile->CurPage = (nsBasic::BlockPage *)lpPage1->NextPage; MyFile->CurOffset = 0; }
				return 0;
			}
			else{
				lpPage2 = (nsBasic::BlockPage *)lpPage1->PrevPage;
				if (nullptr == (void *)(lpPage2->NextEmpty)){
					if (nullptr == (lpPage2 = (nsBasic::BlockPage *)nsBasic::MemAlloc(sizeof(nsBasic::BlockPage))))return 1;
					lpPage2->PageAddr = nsBasic::GetPage(nsEnv::SysPageSize);
					if ((UBINT)NULL == lpPage2->PageAddr){ nsBasic::MemFree(lpPage2, sizeof(nsBasic::BlockPage)); return 1; }
					lpPage2->NextEmpty = (UBINT)lpPage2->PageAddr;
					lpPage2->Type = PAGE_BLK;
					lpPage2->PageSize = nsEnv::SysPageSize;
					lpPage2->BlockSize = sizeof(T);
					lpPage2->BlockCntr = 0;
					lpPage2->PrevPage = lpPage1->PrevPage;
					lpPage2->NextPage = lpPage1;
					lpPage1->PrevPage = lpPage2;
					lpPage1 = (nsBasic::BlockPage *)lpPage2->PrevPage;
					lpPage1->NextPage = lpPage2;
				}
				memcpy((void *)lpPage2->NextEmpty, SrcBuf, sizeof(T));
				lpPage2->BlockCntr++;
				lpPage2->NextEmpty += sizeof(T);
				if (lpPage2->NextEmpty - (UBINT)lpPage2->PageAddr >= lpPage2->PageSize)lpPage2->NextEmpty = (UBINT)NULL;
				return 0;
			}
		}
		else if (MSG_STREAM_WRITEBULK == Msg){
			va_list args;
			va_start(args, Msg);
			UBINT SrcBuf = va_arg(args, UBINT);
			UBINT BlkCnt = va_arg(args, UBINT), BlkWritten = 0, MaxBlks = nsEnv::SysPageSize / sizeof(T);
			if (nullptr != lpPage1->PageAddr){
				lpPage2 = lpPage1;
				lpPage1 = (nsBasic::BlockPage *)lpPage1->NextPage;
				if (nullptr != lpPage1->PageAddr){
					if (BlkCnt < lpPage2->BlockCntr - MyFile->CurOffset){
						memcpy((void *)((UBINT)lpPage2->PageAddr + MyFile->CurOffset*sizeof(T)), (void *)SrcBuf, BlkCnt*sizeof(T));
						MyFile->CurOffset += BlkCnt;
						BlkWritten += BlkCnt;
						return BlkWritten;
					}
					else{
						memcpy((void *)((UBINT)lpPage2->PageAddr + MyFile->CurOffset*sizeof(T)), (void *)SrcBuf, (lpPage2->BlockCntr - MyFile->CurOffset)*sizeof(T));
						BlkCnt -= lpPage2->BlockCntr - MyFile->CurOffset;
						BlkWritten = lpPage2->BlockCntr - MyFile->CurOffset;
						SrcBuf += BlkWritten*sizeof(T);
						lpPage2 = lpPage1;
						lpPage1 = (nsBasic::BlockPage *)lpPage1->NextPage;
						MyFile->CurPage = lpPage2;
						MyFile->CurOffset = 0;
						while (nullptr != lpPage1->PageAddr){
							if (BlkCnt < lpPage2->BlockCntr){
								memcpy((void *)(lpPage2->PageAddr), (void *)SrcBuf, BlkCnt*sizeof(T));
								MyFile->CurOffset += BlkCnt;
								BlkWritten += BlkCnt;
								return BlkWritten;
							}
							else{
								memcpy((void *)(lpPage2->PageAddr), (void *)SrcBuf, lpPage2->BlockCntr*sizeof(T));
								BlkCnt -= lpPage2->BlockCntr;
								BlkWritten += lpPage2->BlockCntr;
								SrcBuf += lpPage2->BlockCntr*sizeof(T);
								lpPage2 = lpPage1;
								lpPage1 = (nsBasic::BlockPage *)lpPage1->NextPage;
								MyFile->CurPage = lpPage2;
							}
						}
					}
				}
				if (MyFile->CurOffset + BlkCnt < MaxBlks){
					memcpy((void *)((UBINT)lpPage2->PageAddr + MyFile->CurOffset*sizeof(T)), (void *)SrcBuf, BlkCnt*sizeof(T));
					BlkWritten += BlkCnt;
					MyFile->CurOffset += BlkCnt;
					if (MyFile->CurOffset >= lpPage2->BlockCntr){
						lpPage2->BlockCntr = MyFile->CurOffset;
						lpPage2->NextEmpty = (UBINT)lpPage2->PageAddr + lpPage2->BlockCntr*sizeof(T);
						MyFile->CurPage = lpPage1;
						MyFile->CurOffset = 0;
					}
					return BlkWritten;
				}
				else{
					memcpy((void *)((UBINT)lpPage2->PageAddr + MyFile->CurOffset*sizeof(T)), (void *)SrcBuf, (MaxBlks - MyFile->CurOffset)*sizeof(T));
					BlkCnt -= MaxBlks - MyFile->CurOffset;
					BlkWritten += MaxBlks - MyFile->CurOffset;
					SrcBuf += (MaxBlks - MyFile->CurOffset)*sizeof(T);
					lpPage2->BlockCntr = MaxBlks;
					lpPage2->NextEmpty = (UBINT)NULL;
					MyFile->CurPage = lpPage1;
					MyFile->CurOffset = 0;
				}
			}
			else{
				lpPage2 = (nsBasic::BlockPage *)lpPage1->PrevPage;
				if (nullptr != lpPage2->PageAddr){
					if (BlkCnt + lpPage2->BlockCntr < MaxBlks){
						memcpy((void *)((UBINT)lpPage2->PageAddr + lpPage2->BlockCntr*sizeof(T)), (void *)SrcBuf, BlkCnt*sizeof(T));
						lpPage2->BlockCntr += BlkCnt;
						lpPage2->NextEmpty += BlkCnt*sizeof(T);
						BlkWritten += BlkCnt;
						return BlkWritten;
					}
					else{
						memcpy((void *)((UBINT)lpPage2->PageAddr + lpPage2->BlockCntr*sizeof(T)), (void *)SrcBuf, (MaxBlks - lpPage2->BlockCntr)*sizeof(T));
						BlkCnt -= MaxBlks - lpPage2->BlockCntr;
						BlkWritten += MaxBlks - lpPage2->BlockCntr;
						SrcBuf += (MaxBlks - lpPage2->BlockCntr)*sizeof(T);
						lpPage2->BlockCntr = MaxBlks;
						lpPage2->NextEmpty = (UBINT)NULL;
					}
				}
			}
			while (BlkCnt > 0){
				if (nullptr == (lpPage2 = (nsBasic::BlockPage *)nsBasic::MemAlloc(sizeof(nsBasic::BlockPage))))return BlkWritten;
				lpPage2->PageAddr = nsBasic::GetPage(nsEnv::SysPageSize);
				if (nullptr == lpPage2->PageAddr){ nsBasic::MemFree(lpPage2,sizeof(nsBasic::BlockPage)); return BlkWritten; }
				lpPage2->Type = PAGE_BLK;
				lpPage2->PageSize = nsEnv::SysPageSize;
				lpPage2->BlockSize = sizeof(T);
				lpPage2->PrevPage = lpPage1->PrevPage;
				((nsBasic::BlockPage *)lpPage2->PrevPage)->NextPage = lpPage2;
				lpPage1->PrevPage = lpPage2;

				if (BlkCnt >= MaxBlks){
					memcpy(lpPage2->PageAddr, (void *)SrcBuf, MaxBlks*sizeof(T));
					lpPage2->BlockCntr = MaxBlks;
					lpPage2->NextEmpty = (UBINT)NULL;
					BlkCnt -= MaxBlks;
					BlkWritten += MaxBlks;
					SrcBuf += MaxBlks*sizeof(T);
				}
				else{
					memcpy(lpPage2->PageAddr, (void *)SrcBuf, BlkCnt*sizeof(T));
					lpPage2->BlockCntr = BlkCnt;
					lpPage2->NextEmpty = (UBINT)lpPage2->PageAddr + BlkCnt*sizeof(T);
					BlkWritten += BlkCnt;
					BlkCnt = 0;
				}
			}
			lpPage2->NextPage = lpPage1;
			return BlkWritten;
		}
		else if (MSG_EDITOR_INSERT == Msg){
			va_list args;
			va_start(args, Msg);
			void *SrcBuf = va_arg(args, void *);

			if (MyFile->CurOffset>0){
				//Split the current page
				if (nullptr == (lpPage2 = (nsBasic::BlockPage *)nsBasic::MemAlloc(sizeof(nsBasic::BlockPage))))return 1;
				lpPage2->PageAddr = nsBasic::GetPage(nsEnv::SysPageSize);
				if (nullptr == lpPage2->PageAddr){ nsBasic::MemFree(lpPage2, sizeof(nsBasic::BlockPage)); return 1; }
				lpPage2->Type = PAGE_BLK;
				lpPage2->PageSize = nsEnv::SysPageSize;
				lpPage2->BlockSize = sizeof(T);
				lpPage2->BlockCntr = lpPage1->BlockCntr - MyFile->CurOffset;
				lpPage1->BlockCntr = MyFile->CurOffset;
				lpPage1->NextEmpty = (UBINT)lpPage1->PageAddr + MyFile->CurOffset*sizeof(T);
				memcpy(lpPage2->PageAddr, (void *)lpPage1->NextEmpty, lpPage2->BlockCntr*sizeof(T));
				lpPage2->NextEmpty = (UBINT)lpPage2->PageAddr + lpPage2->BlockCntr*sizeof(T);
				lpPage2->PrevPage = MyFile->CurPage;
				lpPage2->NextPage = lpPage1->NextPage;
				lpPage1->NextPage = lpPage2;
				MyFile->CurPage = lpPage2;
				MyFile->CurOffset = 0;
				//Merge pages on the left side and the right side.We suppose all pages are of the same size(i.e. nsEnv::SysPageSize).
				UBINT BlkCnt = nsEnv::SysPageSize / sizeof(T);
				nsBasic::BlockPage *lpPage3;
				lpPage2 = (nsBasic::BlockPage *)lpPage2->PrevPage;
				do{
					BlkCnt -= lpPage2->BlockCntr;
					lpPage2 = (nsBasic::BlockPage *)lpPage2->PrevPage;
				} while (nullptr != lpPage2->PageAddr && BlkCnt >= lpPage2->BlockCntr);
				lpPage2 = (nsBasic::BlockPage *)lpPage2->NextPage;
				lpPage1 = (nsBasic::BlockPage *)lpPage2->NextPage;
				while (lpPage1 != (nsBasic::BlockPage *)MyFile->CurPage){
					lpPage2->BlockCntr += lpPage1->BlockCntr;
					memcpy((void *)lpPage2->NextEmpty, lpPage1->PageAddr, lpPage1->BlockCntr*sizeof(T));
					lpPage2->NextEmpty += lpPage1->BlockCntr*sizeof(T);
					nsBasic::unGetPage((void *)(lpPage1->PageAddr), lpPage1->PageSize);
					lpPage3 = (nsBasic::BlockPage *)lpPage1->NextPage;
					delete lpPage1;
					lpPage1 = lpPage3;
				}
				lpPage2->NextPage = lpPage1;
				lpPage1->PrevPage = lpPage2;
				if (0 == BlkCnt)lpPage2->NextEmpty = (UBINT)NULL;
				lpPage2 = (nsBasic::BlockPage *)MyFile->CurPage;
				lpPage1 = (nsBasic::BlockPage *)lpPage2->NextPage;
				BlkCnt = nsEnv::SysPageSize / sizeof(T);
				while ((UBINT)NULL != lpPage1->PageAddr && lpPage2->BlockCntr + lpPage1->BlockCntr <= BlkCnt){
					lpPage2->BlockCntr += lpPage1->BlockCntr;
					memcpy((void *)lpPage2->NextEmpty, lpPage1->PageAddr, lpPage1->BlockCntr*sizeof(T));
					lpPage2->NextEmpty += lpPage1->BlockCntr*sizeof(T);
					nsBasic::unGetPage(lpPage1->PageAddr, lpPage1->PageSize);
					lpPage3 = (nsBasic::BlockPage *)lpPage1->NextPage;
					delete lpPage1;
					lpPage1 = lpPage3;
				}
				lpPage2->NextPage = lpPage1;
				lpPage1->PrevPage = lpPage2;
				if (lpPage2->BlockCntr == BlkCnt)lpPage2->NextEmpty = (UBINT)NULL;
				lpPage1 = (nsBasic::BlockPage *)MyFile->CurPage;
			}
			lpPage2 = (nsBasic::BlockPage *)lpPage1->PrevPage;
			if (nullptr == (void *)lpPage2->NextEmpty){
				if (nullptr == (lpPage2 = (nsBasic::BlockPage *)nsBasic::MemAlloc(sizeof(nsBasic::BlockPage))))return 1;
				lpPage2->PageAddr = nsBasic::GetPage(nsEnv::SysPageSize);
				if (nullptr == lpPage2->PageAddr){ nsBasic::MemFree(lpPage2, sizeof(nsBasic::BlockPage)); return 1; }
				lpPage2->NextEmpty = (UBINT)lpPage2->PageAddr;
				lpPage2->Type = PAGE_BLK;
				lpPage2->PageSize = nsEnv::SysPageSize;
				lpPage2->BlockSize = sizeof(T);
				lpPage2->BlockCntr = 0;
				lpPage2->PrevPage = lpPage1->PrevPage;
				lpPage2->NextPage = lpPage1;
				lpPage1->PrevPage = lpPage2;
				lpPage1 = (nsBasic::BlockPage *)lpPage2->PrevPage;
				lpPage1->NextPage = lpPage2;
			}
			memcpy((void *)lpPage2->NextEmpty, SrcBuf, sizeof(T));
			lpPage2->BlockCntr++;
			lpPage2->NextEmpty += sizeof(T);
			if (lpPage2->NextEmpty - (UBINT)lpPage2->PageAddr >= lpPage2->PageSize)lpPage2->NextEmpty = (UBINT)NULL;
			return 0;
		}
		else if (MSG_FILE_SETPTR == Msg){
			va_list args;
			va_start(args, Msg);
			INT8b Offset = va_arg(args, INT8b);
			UBINT BaseType = va_arg(args, UBINT);
			if (0 == BaseType){ //FILE_BEGIN
				if (Offset < 0)return 1;
				while (nullptr != lpPage1->PageAddr)lpPage1 = (nsBasic::BlockPage *)lpPage1->PrevPage; //Now lpPage1->PageAddr is NULL
				do{
					Offset -= lpPage1->BlockCntr;
					lpPage1 = (nsBasic::BlockPage *)lpPage1->NextPage;
				} while (Offset >= (INT8b)lpPage1->BlockCntr && nullptr != lpPage1->PageAddr);
				if (Offset>0 && nullptr == lpPage1->PageAddr) return 1;
			}
			else if (1 == BaseType){ //FILE_CURRENT
				if (Offset <= 0){
					Offset += MyFile->CurOffset; //we suppose that MyFile->CurOffset < INT8b_MAX
					while (Offset < 0){
						lpPage1 = (nsBasic::BlockPage *)lpPage1->PrevPage;
						if (nullptr == lpPage1->PageAddr)return 1;
						Offset += lpPage1->BlockCntr;
					}
				}
				else if (Offset<(INT8b)(lpPage1->BlockCntr - MyFile->CurOffset))Offset += MyFile->CurOffset;
				else{
					Offset -= lpPage1->BlockCntr - MyFile->CurOffset;
					lpPage1 = (nsBasic::BlockPage *)lpPage1->NextPage;
					while (Offset >= (INT8b)lpPage1->BlockCntr && nullptr != lpPage1->PageAddr){
						Offset -= lpPage1->BlockCntr;
						lpPage1 = (nsBasic::BlockPage *)lpPage1->NextPage;
					}
					if (Offset>0 && nullptr == lpPage1->PageAddr) return 1;
				}
			}
			else if (2 == BaseType){ //FILE_END
				if (Offset > 0)return 1;
				while (nullptr != lpPage1->PageAddr)lpPage1 = (nsBasic::BlockPage *)lpPage1->NextPage; //Now lpPage1->PageAddr is NULL
				while (Offset < 0){
					lpPage1 = (nsBasic::BlockPage *)lpPage1->PrevPage;
					if (nullptr == lpPage1->PageAddr)return 1;
					Offset += lpPage1->BlockCntr;
				}
			}
			MyFile->CurPage = lpPage1;
			MyFile->CurOffset = (UBINT)Offset;
			return 0;
		}
		else if (MSG_FILE_GETPTR == Msg){
			va_list args;
			va_start(args, Msg);
			UINT8b *Offset = va_arg(args, UINT8b *);
			*Offset = MyFile->CurOffset;
			lpPage1 = (nsBasic::BlockPage *)lpPage1->PrevPage;
			while (nullptr != lpPage1->PageAddr){
				*Offset += lpPage1->BlockCntr;
				lpPage1 = (nsBasic::BlockPage *)lpPage1->PrevPage;
			}
			return 0;
		}
		else if (MSG_DESTROY == Msg){
			throw new std::exception("this object is only allowed to be created locally.", 1);
			return 1;
		}
		else return 0;
	}
	template<class T> MemFile<T>::MemFile(){
		nsBasic::BlockPage *DummyPage = new nsBasic::BlockPage;
		DummyPage->PrevPage = DummyPage;
		DummyPage->NextPage = DummyPage;
		DummyPage->PageAddr = nullptr;
		DummyPage->NextEmpty = (UBINT)nullptr;
		DummyPage->Type = PAGE_BLK;
		DummyPage->PageSize = 0;
		DummyPage->BlockSize = sizeof(T);
		DummyPage->BlockCntr = 0;
		this->CurPage = DummyPage;
		this->CurOffset = 0;
		this->Type = OBJ_EDITOR_MEMFILE;
		this->MsgProc = this->_MsgProc;
		this->ThreadLock = (UBINT)nullptr;
		this->BlockSize = sizeof(T);
	}
	template<class T> MemFile<T>::~MemFile(){
		nsBasic::BlockPage *lpPage1 = this->CurPage, *lpPage2;
		do{
			lpPage2 = (nsBasic::BlockPage *)lpPage1->NextPage;
			nsBasic::unGetPage(lpPage1->PageAddr, lpPage1->PageSize);
			delete lpPage1;
			lpPage1 = lpPage2;
		} while (lpPage1 != this->CurPage);
		this->CurPage = nullptr;
	}
	template<class T> UBINT MemFile<T>::PageCount(){
		nsBasic::BlockPage *lpPage = this->CurPage;
		UBINT RetValue = 0;
		do{
			RetValue++;
			lpPage = (nsBasic::BlockPage *)lpPage->NextPage;
		} while (lpPage != this->CurPage);
		RetValue--;
		return RetValue;
	}
	template<class T> UBINT MemFile<T>::BlockCount(){
		nsBasic::BlockPage *lpPage = this->CurPage;
		UBINT RetValue = 0;
		do{
			RetValue += lpPage->BlockCntr;
			lpPage = (nsBasic::BlockPage *)lpPage->NextPage;
		} while (lpPage != this->CurPage);
		return RetValue;
	}
	extern bool CreateFolder(UBCHAR *lpPath){
#if defined LIBENV_OS_WIN
		if(CreateDirectory(lpPath, NULL))return true;
		if (ERROR_ALREADY_EXISTS == LastSysErr)return false;
		else throw std::exception("Invalid folder path.");
#elif defined LIBENV_OS_LINUX
		if(mkdir(lpPath, S_IRWXO))return true; // let's presume that the user need S_IRWXO
		if (EEXIST == LastSysErr)return false;
		else throw std::exception("Invalid folder path.");
#endif
	}
	extern void *ReadFileToMem(UBCHAR *lpFileName, UBINT *BufferLength){
		void *Addr;
		UBINT hFile = _OpenFile_Read(lpFileName);
		UINT8b FileLen;
		if((UBINT)-1!=hFile){
			if (!_GetFileLen(hFile, &FileLen) && FileLen < UBINT_MAX){
				Addr = nsBasic::MemAlloc((UBINT)(FileLen + 1));
				if (nullptr != Addr){
					if (FileLen == _ReadFile(hFile, Addr, (UINT4b)FileLen)){
						*((char *)Addr + FileLen) = '\0';
						*BufferLength = (UBINT)(FileLen + 1);
						_CloseFile(hFile);
						return (void *)Addr;
					}
					else nsBasic::MemFree(Addr, (UBINT)(FileLen + 1));
				}
			}
			_CloseFile(hFile);
		}
		return nullptr;
	}
}
#endif