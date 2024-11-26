/* Description: Basic stream interface for large-scale object-oriented programming. DO NOT include this header directly.
* Language:C++
* Author:***
*
* This header provides an interface standard which encapsulates the basic stream operation.
*
*/

namespace nsBasic{

	enum StreamSeekType{
		FROM_BEGIN = 0,
		FROM_CURRENT_POS = 1,
		FROM_END = 2
	};

	template < typename T > struct Stream{
		typedef Stream<T> this_type;
		struct Interface{
			bool(*_Read)(this_type * const, T*);
			UINT8b(*_ReadBulk)(this_type * const, T*, UINT8b);
			bool(*_Write)(this_type * const, const T*);
			UINT8b(*_WriteBulk)(this_type * const, const T*, UINT8b);
			UINT8b(*_GetPtr)(this_type * const);
			bool(*_SetPtr)(this_type * const, StreamSeekType, INT8b);
			void(*_Delete)(this_type * const);
			void(*_Destroy)(this_type * const);
		};

		static UINT8b _ReadBulk_Default(this_type * const ThisBase, T* Dest, UINT8b Count){
			T *Tmp = Dest;
			for (UINT8b i = 0; i < Count; i++){
				if (false == ThisBase->lpInterface->_Read(ThisBase, Tmp))return i;
				Tmp++;
			}
			return Count;
		}
		static UINT8b _WriteBulk_Default(this_type * const ThisBase, const T* Src, UINT8b Count){
			const T *Tmp = Src;
			for (UINT8b i = 0; i < Count; i++){
				if (false == ThisBase->lpInterface->_Write(ThisBase, Tmp))return i;
				Tmp++;
			}
			return Count;
		}

		//private:
		Interface *lpInterface;
	public:
		inline void Delete(){ this->lpInterface->_Delete(this); }
		inline Stream::~Stream(){ this->lpInterface->_Destroy(this); }
	};

	template < typename T > struct Stream_R :Stream<T>{
		inline bool Read(T *Dest){ return this->lpInterface->_Read(this, Dest); }
		inline UINT8b ReadBulk(T *Dest, UINT8b Count){ return this->lpInterface->_ReadBulk(this, Dest, Count); }
	};
	template <> struct Stream_R<unsigned char> :Stream<unsigned char>{
		inline bool Read(void *Dest){ return this->lpInterface->_Read(this, (unsigned char *)Dest); }
		inline UINT8b ReadBulk(void *Dest, UINT8b Count){ return this->lpInterface->_ReadBulk(this, (unsigned char *)Dest, Count); }
	};
	template < typename T > Stream_R<T>& operator>>(Stream_R<T>& lhs, T& rhs){
		if (false == lhs.Read(&rhs))throw std::exception("Stream read failed.");
		return lhs;
	}
	template < typename T > Stream_R<unsigned char>& operator>>(Stream_R<unsigned char>& lhs, T& rhs){
		if (lhs.ReadBulk((unsigned char *)&rhs, sizeof(T))<sizeof(T))throw std::exception("Stream read failed.");
		return lhs;
	}
	Stream_R<unsigned char>& operator>>(Stream_R<unsigned char>& lhs, unsigned char& rhs){
		if (false == lhs.Read(&rhs))throw std::exception("Stream read failed.");
		return lhs;
	}

	template < typename T > struct Stream_W :Stream<T>{
		inline bool Write(const T *Src){ return this->lpInterface->_Write(this, Src); }
		inline UINT8b WriteBulk(const T *Src, UINT8b Count){ return this->lpInterface->_WriteBulk(this, Src, Count); }
	};
	template <> struct Stream_W<unsigned char> :Stream<unsigned char>{
		inline bool Write(const void *Src){ return this->lpInterface->_Write(this, (const unsigned char *)Src); }
		inline UINT8b WriteBulk(const void *Src, UINT8b Count){ return this->lpInterface->_WriteBulk(this, (const unsigned char *)Src, Count); }
	};
	template < typename T > Stream_W<T>& operator<<(Stream_W<T>& lhs, const T& rhs){
		if (false == lhs.Write(&rhs))throw std::exception("Stream write failed.");;
		return lhs;
	}
	template < typename T > Stream_W<unsigned char>& operator<<(Stream_W<unsigned char>& lhs, const T& rhs){
		if (lhs.WriteBulk((const unsigned char *)&rhs, sizeof(T))<sizeof(T))throw std::exception("Stream write failed.");
		return lhs;
	}
	Stream_W<unsigned char>& operator<<(Stream_W<unsigned char>& lhs, const unsigned char& rhs){
		if (false == lhs.Write(&rhs))throw std::exception("Stream write failed.");
		return lhs;
	}

	template < typename T > struct Stream_RW :Stream<T>{
		inline bool Read(T *Dest){ return this->lpInterface->_Read(this, Dest); }
		inline UINT8b ReadBulk(T *Dest, UINT8b Count){ return this->lpInterface->_ReadBulk(this, Dest, Count); }
		inline bool Write(const T *Src){ return this->lpInterface->_Write(this, Src); }
		inline UINT8b WriteBulk(const T *Src, UINT8b Count){ return this->lpInterface->_WriteBulk(this, Src, Count); }
		inline Stream_R<T>& AsReadStream(){ return reinterpret_cast<Stream_R<T>&>(*this); }
		inline Stream_W<T>& AsWriteStream(){ return reinterpret_cast<Stream_W<T>&>(*this); }
	};
	template <> struct Stream_RW<unsigned char> :Stream<unsigned char>{
		inline bool Read(void *Dest){ return this->lpInterface->_Read(this, (unsigned char *)Dest); }
		inline UINT8b ReadBulk(void *Dest, UINT8b Count){ return this->lpInterface->_ReadBulk(this, (unsigned char *)Dest, Count); }
		inline bool Write(const void *Src){ return this->lpInterface->_Write(this, (const unsigned char *)Src); }
		inline UINT8b WriteBulk(const void *Src, UINT8b Count){ return this->lpInterface->_WriteBulk(this, (const unsigned char *)Src, Count); }
		inline Stream_R<unsigned char>& AsReadStream(){ return reinterpret_cast<Stream_R<unsigned char>&>(*this); }
		inline Stream_W<unsigned char>& AsWriteStream(){ return reinterpret_cast<Stream_W<unsigned char>&>(*this); }
	};
	template < typename T > Stream_RW<T>& operator>>(Stream_RW<T>& lhs, T& rhs){
		if (false == lhs.Read(&rhs))throw std::exception("Stream read failed.");
		return lhs;
	}
	template < typename T > Stream_RW<unsigned char>& operator>>(Stream_RW<unsigned char>& lhs, T& rhs){
		if (lhs.ReadBulk((unsigned char *)&rhs, sizeof(T))<sizeof(T))throw std::exception("Stream read failed.");
		return lhs;
	}
	Stream_RW<unsigned char>& operator>>(Stream_RW<unsigned char>& lhs, unsigned char& rhs){
		if (false == lhs.Read(&rhs))throw std::exception("Stream read failed.");
		return lhs;
	}
	template < typename T > Stream_RW<T>& operator<<(Stream_RW<T>& lhs, const T& rhs){
		if (false == lhs.Write(&rhs))throw std::exception("Stream write failed.");;
		return lhs;
	}
	template < typename T > Stream_RW<unsigned char>& operator<<(Stream_RW<unsigned char>& lhs, const T& rhs){
		if (lhs.WriteBulk((const unsigned char *)&rhs, sizeof(T))<sizeof(T))throw std::exception("Stream write failed.");
		return lhs;
	}
	Stream_RW<unsigned char>& operator<<(Stream_RW<unsigned char>& lhs, const unsigned char& rhs){
		if (false == lhs.Write(&rhs))throw std::exception("Stream write failed.");
		return lhs;
	}

	template < typename T > struct Stream_RS :Stream_R<T>{
		inline UINT8b GetPtr(){ return this->lpInterface->_GetPtr(this); }
		inline bool Seek(StreamSeekType SeekType, INT8b Offset){ return this->lpInterface->_SetPtr(this, SeekType, Offset); }
	};

	template < typename T > struct Stream_WS :Stream_W<T>{
		inline UINT8b GetPtr(){ return this->lpInterface->_GetPtr(this); }
		inline bool Seek(StreamSeekType SeekType, INT8b Offset){ return this->lpInterface->_SetPtr(this, SeekType, Offset); }
	};

	template < typename T > struct Stream_RWS :Stream_RW<T>{
		inline UINT8b GetPtr(){ return this->lpInterface->_GetPtr(this); }
		inline bool Seek(StreamSeekType SeekType, INT8b Offset){ return this->lpInterface->_SetPtr(this, SeekType, Offset); }
		inline Stream_R<T>& AsReadSeekStream(){ return reinterpret_cast<Stream_RS<T>&>(*this); }
		inline Stream_W<T>& AsWriteSeekStream(){ return reinterpret_cast<Stream_WS<T>&>(*this); }
	};
}