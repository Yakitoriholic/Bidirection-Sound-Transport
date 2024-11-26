/* Description:UTF-8 String class. Don't include this header directly.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_TEXT_STRING
#define LIB_TEXT_STRING

#include "lGeneral.hpp"
#include "lAlg.hpp"

namespace nsText{

	class String{
	public:
		// custom typedefs

		// container typedefs, mandated by the C++ standard
		typedef unsigned char			value_type;
		typedef unsigned char&			reference;
		typedef const unsigned char&	const_reference;
		typedef unsigned char*			pointer;
		typedef const unsigned char*	const_pointer;
		typedef UBINT					size_type;
		typedef UBINT					difference_type;

		// container typedefs of iterator, mandated by the C++ standard
		typedef pointer			iterator;
		typedef const_pointer	const_iterator;

		// constants
		static const size_type npos = (size_type)-1;

	protected:
		union _SSOStruct{
			unsigned char Buffer[3 * sizeof(UBINT)]; //This works when 3 * sizeof(UBINT) < 0xFF.
			struct{
				unsigned char *StrPos;
				UBINT StrLen;
				UBINT MemLen;
			};
		} _SSO;

		static const size_type _SSOTail = 3 * sizeof(UBINT)-1;
	public:
		//constructors
		String();
		String(size_type Count);
		String(size_type Count, value_type Char);
		String(const String &rhs);
		String(String &&rhs);
		String(const String_W &rhs);
		String(const char *rhs, size_type Count);
		String(const char *rhs);
		String(const wchar_t *rhs, size_type Count);
		String(const wchar_t *rhs);

		// iterator functions, mandated by the C++ standard
		iterator begin();
		const_iterator cbegin() const;
		iterator end();
		const_iterator cend() const;

		//element access functions
		inline reference operator[](size_type n){ return this->begin()[n]; }

		//capacity functions
		inline bool empty() const { return 0x80 == this->_SSO.Buffer[0]; }
		size_type size() const;
		size_type max_size() const;
		void reserve(size_type NewCapacity);
		size_type capacity() const;
		void shrink_to_fit();

		//modifiers
		void clear();
		String& append(UINT4b CodePoint); //Here [CodePoint] is the Unicode code point of this character.
		String& append(value_type Char);
		String& append(value_type Char, size_type Count);
		String& append(const char *rhs, size_type Count);
		String& append(const char *rhs);
		String& append(const String &rhs);
		String& insert(size_type Index, value_type Char);
		String& insert(size_type Index, value_type Char, size_type Count);
		String& insert(size_type Index, const char *rhs, size_type Count);
		String& insert(size_type Index, const char *rhs);
		String& insert(size_type Index, const String &rhs);
		String& replace(size_type Index, size_type ReplaceLen, value_type Char);
		String& replace(size_type Index, size_type ReplaceLen, value_type Char, size_type Count);
		String& replace(size_type Index, size_type ReplaceLen, const char *rhs, size_type Count);
		String& replace(size_type Index, size_type ReplaceLen, const char *rhs);
		String& replace(size_type Index, size_type ReplaceLen, const String &rhs);
		String& erase(size_type Index = 0, size_type Count = 1);
		void resize(size_type NewSize);
		void resize(size_type NewSize, value_type Char);
		void swap(String &rhs);

		//query functions
		size_type find(value_type Char, size_type InitPos = 0) const;
		size_type find(const char *SubStr, size_type Count, size_type InitPos = 0) const;
		size_type find(const char *SubStr, size_type InitPos = 0) const;
		size_type find(const String& SubStr, size_type InitPos = 0) const;
		size_type rfind(value_type Char, size_type InitPos = npos) const;
		size_type rfind(const char *SubStr, size_type Count, size_type InitPos = npos) const;
		size_type rfind(const char *SubStr, size_type InitPos = npos) const;
		size_type rfind(const String& SubStr, size_type InitPos = npos) const;

		//other operations
		String substr(size_type Index=0, size_type Count = npos);
		int compare(const String& rhs) const;
		inline int compare(const char* rhs) const;

		//operators
		String& operator=(const String& rhs);
		String& operator=(String&& rhs);
		String& operator=(const char *rhs);
		String& operator+=(value_type rhs);
		String& operator+=(const String& rhs);
		String& operator+=(const char *rhs);

		//destructor
		~String();
	};
	
	inline String operator+(const String& lhs, const String& rhs);
	inline String operator+(const String& lhs, const char *rhs);
	inline String operator+(const String& lhs, const String::value_type rhs);
	inline String operator+(const char *lhs, const String& rhs);
	inline String operator+(const String::value_type lhs, const String& rhs);

	inline bool operator==(const String& lhs, const String& rhs){ return 0 == lhs.compare(rhs); }
	inline bool operator!=(const String& lhs, const String& rhs){ return 0 != lhs.compare(rhs); }
	inline bool operator<(const String& lhs, const String& rhs){ return 0 > lhs.compare(rhs); }
	inline bool operator<=(const String& lhs, const String& rhs){ return 0 >= lhs.compare(rhs); }
	inline bool operator>(const String& lhs, const String& rhs){ return 0 < lhs.compare(rhs); }
	inline bool operator>=(const String& lhs, const String& rhs){ return 0 <= lhs.compare(rhs); }

	inline bool operator==(const String& lhs, const char* rhs){ return 0 == lhs.compare(rhs); }
	inline bool operator!=(const String& lhs, const char* rhs){ return 0 != lhs.compare(rhs); }
	inline bool operator<(const String& lhs, const char* rhs){ return 0 > lhs.compare(rhs); }
	inline bool operator<=(const String& lhs, const char* rhs){ return 0 >= lhs.compare(rhs); }
	inline bool operator>(const String& lhs, const char* rhs){ return 0 < lhs.compare(rhs); }
	inline bool operator>=(const String& lhs, const char* rhs){ return 0 <= lhs.compare(rhs); }

	inline bool operator==(const char* lhs, const String& rhs){ return 0 == rhs.compare(lhs); }
	inline bool operator!=(const char* lhs, const String& rhs){ return 0 != rhs.compare(lhs); }
	inline bool operator<(const char* lhs, const String& rhs){ return 0 < rhs.compare(lhs); }
	inline bool operator<=(const char* lhs, const String& rhs){ return 0 <= rhs.compare(lhs); }
	inline bool operator>(const char* lhs, const String& rhs){ return 0 > rhs.compare(lhs); }
	inline bool operator>=(const char* lhs, const String& rhs){ return 0 >= rhs.compare(lhs); }
	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	String::String(){
		this->_SSO.Buffer[_SSOTail] = 0x80;
		this->_SSO.Buffer[0] = '\0';
	}
	String::String(size_type Count){
		if (Count > this->max_size())Count = this->max_size();
		if (Count < _SSOTail){
			this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)Count;
			this->_SSO.Buffer[Count] = '\0';
		}
		else{
			this->_SSO.StrPos = (unsigned char *)nsBasic::GlobalMemAlloc(Count + 1);
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.MemLen = Count + 1;
				this->_SSO.StrLen = Count;
				this->_SSO.StrPos[Count] = '\0';
			}
		}
	}
	String::String(size_type Count, value_type Char){
		if (Count > this->max_size())Count = this->max_size();
		if (Count < _SSOTail){
			this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)Count;
			memset(this->_SSO.Buffer, Char, Count);
			this->_SSO.Buffer[Count] = '\0';
		}
		else{
			this->_SSO.StrPos = (unsigned char *)nsBasic::GlobalMemAlloc(Count + 1);
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.MemLen = Count + 1;
				this->_SSO.StrLen = Count;
				memset(this->_SSO.StrPos, Char, Count);
				this->_SSO.StrPos[Count] = '\0';
			}
		}
	}
	String::String(const String &rhs){
		if (0x80 & rhs._SSO.Buffer[_SSOTail])this->_SSO = rhs._SSO;
		else{
			this->_SSO.StrPos = (unsigned char *)nsBasic::GlobalMemAlloc(rhs._SSO.MemLen);
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.MemLen = rhs._SSO.MemLen;
				this->_SSO.StrLen = rhs._SSO.StrLen;
				memcpy(this->_SSO.StrPos, rhs._SSO.StrPos, this->_SSO.StrLen + 1);
			}
		}
	}
	String::String(String &&rhs){
		this->_SSO = rhs._SSO;
		//get rhs ready for destruct
		rhs._SSO.Buffer[_SSOTail] = 0x80;
	}
	String::String(const char *rhs, size_type Count){
		if (Count >= _SSOTail){
			this->_SSO.StrPos = (unsigned char *)nsBasic::GlobalMemAlloc(Count + 1);
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.StrLen = Count;
				this->_SSO.MemLen = Count + 1;
				memcpy(this->_SSO.StrPos, rhs, Count);
				this->_SSO.StrPos[Count] = '\0';
			}
		}
		else{
			this->_SSO.Buffer[_SSOTail] = (unsigned char)(0x80 + Count);
			memcpy(this->_SSO.Buffer, rhs, Count);
			this->_SSO.Buffer[Count] = '\0';
		}
	}
	String::String(const char *rhs){
		size_t rhsLen = strnlen(rhs, this->max_size());
		if (rhsLen >= _SSOTail){
			this->_SSO.StrPos = (unsigned char *)nsBasic::GlobalMemAlloc(rhsLen + 1);
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.StrLen = rhsLen;
				this->_SSO.MemLen = rhsLen + 1;
				memcpy(this->_SSO.StrPos, rhs, rhsLen);
				this->_SSO.StrPos[rhsLen] = '\0';
			}
		}
		else{
			this->_SSO.Buffer[_SSOTail] = (unsigned char)(0x80 + rhsLen);
			memcpy(this->_SSO.Buffer, rhs, rhsLen + 1);
		}
	}
	inline String::iterator String::begin(){
		if (0x80 & this->_SSO.Buffer[_SSOTail])return this->_SSO.Buffer;
		else return this->_SSO.StrPos;
	}
	inline String::const_iterator String::cbegin() const{
		if (0x80 & this->_SSO.Buffer[_SSOTail])return this->_SSO.Buffer;
		else return this->_SSO.StrPos;
	}
	inline String::iterator String::end(){
		if (0x80 & this->_SSO.Buffer[_SSOTail])return &this->_SSO.Buffer[(0x7F & this->_SSO.Buffer[_SSOTail])];
		else return this->_SSO.StrPos + this->_SSO.StrLen;
	}
	inline String::const_iterator String::cend() const{
		if (0x80 & this->_SSO.Buffer[_SSOTail])return &this->_SSO.Buffer[(0x7F & this->_SSO.Buffer[_SSOTail])];
		else return this->_SSO.StrPos + this->_SSO.StrLen;
	}
	inline String::size_type String::size() const{
		if (0x80 & this->_SSO.Buffer[_SSOTail])return 0x7F & this->_SSO.Buffer[_SSOTail];
		else return this->_SSO.StrLen;
	}
	inline String::size_type String::max_size() const{
#if defined LIBENV_SYS_INTELX64
		return 0x7FFFFFFFFFFFFFFF;
#elif defined LIBENV_SYS_INTELX86
		return 0x7FFFFFFF;
#endif
	}
	void String::reserve(size_type NewCapacity){
		if (NewCapacity > this->max_size())throw std::length_error("String length exceeds its max size.");
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			if (NewCapacity >= _SSOTail){
				size_type CurSize = this->_SSO.Buffer[_SSOTail] & 0x7F;
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewCapacity + 1);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize + 1);

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewCapacity + 1;
				this->_SSO.StrLen = CurSize;
			}
		}
		else{
			if (NewCapacity < this->_SSO.StrLen)NewCapacity = this->_SSO.StrLen;
			size_type CurMemLen = this->_SSO.MemLen;
			if (NewCapacity < _SSOTail){
				size_type CurSize = this->_SSO.StrLen;
				unsigned char *CurPos = this->_SSO.StrPos;
				this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)CurSize;
				memcpy(this->_SSO.Buffer, CurPos, CurSize + 1);
				nsBasic::GlobalMemFree(CurPos, CurMemLen);
			}
			else{
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewCapacity + 1);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, this->_SSO.StrLen + 1);
				this->_SSO.MemLen = NewCapacity + 1;
				nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen);
				this->_SSO.StrPos = NewPos;
			}
		}
	}
	String::size_type String::capacity() const{
		if (0x80 & this->_SSO.Buffer[_SSOTail])return _SSOTail - 1;
		else return this->_SSO.MemLen - 1;
	}
	void String::shrink_to_fit(){
		if (0 == (0x80 & this->_SSO.Buffer[_SSOTail]) && this->_SSO.StrLen + 1<this->_SSO.MemLen){
			size_type CurMemLen = this->_SSO.MemLen;
			if (this->_SSO.StrLen < _SSOTail){
				size_type CurSize = this->_SSO.StrLen;
				this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)CurSize;
				memcpy(this->_SSO.Buffer, this->_SSO.StrPos, CurSize + 1);
				nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen);
			}
			else{
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(this->_SSO.StrLen + 1);
				if (nullptr != NewPos){
					memcpy(NewPos, this->_SSO.StrPos, this->_SSO.StrLen + 1);
					this->_SSO.MemLen = this->_SSO.StrLen + 1;
					nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen);
					this->_SSO.StrPos = NewPos;
				}
			}
		}
	}
	inline void String::clear(){
		if (0 == (0x80 & this->_SSO.Buffer[_SSOTail]))nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
		this->_SSO.Buffer[_SSOTail] = 0x80;
		this->_SSO.Buffer[0] = '\0';
	}
	String& String::append(UINT4b CodePoint){
		//Encode the codepoint
		unsigned char EncodedChar[4];
		UBINT Count = nsCharCoding::_utf8_encode(EncodedChar, CodePoint);

		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity - CurSize >= Count){
				for (UBINT i = 0; i < Count; i++)this->_SSO.Buffer[CurSize + i] = EncodedChar[i];
				this->_SSO.Buffer[CurSize + Count] = '\0';
				this->_SSO.Buffer[_SSOTail] += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize);
				for (UBINT i = 0; i < Count; i++)NewPos[CurSize + i] = EncodedChar[i];
				NewPos[CurSize + Count] = '\0';

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity - CurSize >= Count){
				for (UBINT i = 0; i < Count; i++)this->_SSO.StrPos[CurSize + i] = EncodedChar[i];
				this->_SSO.StrPos[CurSize + Count] = '\0';
				this->_SSO.StrLen += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, CurSize);
				for (UBINT i = 0; i < Count; i++)NewPos[CurSize + i] = EncodedChar[i];
				NewPos[CurSize + Count] = '\0';

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String& String::append(value_type Char){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity > CurSize){
				this->_SSO.Buffer[CurSize] = Char;
				this->_SSO.Buffer[CurSize + 1] = '\0';
				this->_SSO.Buffer[_SSOTail] += 1;
			}
			else if (this->max_size() == CurSize)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + 2);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize);
				NewPos[CurSize] = Char;
				NewPos[CurSize + 1] = '\0';

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + 1;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity > CurSize){
				this->_SSO.StrPos[CurSize] = Char;
				this->_SSO.StrPos[CurSize + 1] = '\0';
				this->_SSO.StrLen += 1;
			}
			else if (this->max_size() == CurSize)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + 2);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, CurSize);
				NewPos[CurSize] = Char;
				NewPos[CurSize + 1] = '\0';

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + 1;
			}
		}
		return *this;
	}
	String& String::append(value_type Char, size_type Count){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity - CurSize >= Count){
				memset(&this->_SSO.Buffer[CurSize], Char, Count);
				this->_SSO.Buffer[CurSize + Count] = '\0';
				this->_SSO.Buffer[_SSOTail] += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize);
				memset(&NewPos[CurSize], Char, Count);
				NewPos[CurSize + Count] = '\0';

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity - CurSize >= Count){
				memset(&this->_SSO.StrPos[CurSize], Char, Count);
				this->_SSO.StrPos[CurSize + Count] = '\0';
				this->_SSO.StrLen += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, CurSize);
				memset(&NewPos[CurSize], Char, Count);
				NewPos[CurSize + Count] = '\0';

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String& String::append(const char *rhs, size_type Count){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity - CurSize >= Count){
				memcpy(&this->_SSO.Buffer[CurSize], rhs, Count);
				this->_SSO.Buffer[CurSize + Count] = '\0';
				this->_SSO.Buffer[_SSOTail] += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize);
				memcpy(&NewPos[CurSize], rhs, Count);
				NewPos[CurSize + Count] = '\0';

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity - CurSize >= Count){
				memcpy(&this->_SSO.StrPos[CurSize], rhs, Count);
				this->_SSO.StrPos[CurSize + Count] = '\0';
				this->_SSO.StrLen += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, CurSize);
				memcpy(&NewPos[CurSize], rhs, Count);
				NewPos[CurSize + Count] = '\0';

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String& String::append(const char *rhs){
		size_type rhsLen = strnlen(rhs, this->max_size());
		return this->append(rhs, rhsLen);
	}
	String& String::append(const String& rhs){
		size_type rhsLen = rhs.size();
		return this->append((const char *)rhs.cbegin(), rhsLen);
	}
	String& String::insert(size_type Index, value_type Char){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurCapacity > CurSize){
				memmove(&this->_SSO.Buffer[Index + 1], &this->_SSO.Buffer[Index], CurSize - Index + 1);
				this->_SSO.Buffer[Index] = Char;
				this->_SSO.Buffer[_SSOTail]++;
			}
			else if (this->max_size() == CurSize)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + 2);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index);
				NewPos[Index] = Char;
				memcpy(&NewPos[Index + 1], &this->_SSO.Buffer[Index], CurSize - Index + 1);

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + 1;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurCapacity > CurSize){
				memmove(&this->_SSO.StrPos[Index + 1], &this->_SSO.StrPos[Index], CurSize - Index + 1);
				this->_SSO.StrPos[Index] = Char;
				this->_SSO.StrLen++;
			}
			else if (this->max_size() == CurSize)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + 2);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index);
				NewPos[Index] = Char;
				memcpy(&NewPos[Index + 1], &this->_SSO.StrPos[Index], CurSize - Index + 1);

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + 1;
			}
		}
		return *this;
	}
	String& String::insert(size_type Index, value_type Char, size_type Count){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurCapacity - CurSize >= Count){
				memmove(&this->_SSO.Buffer[Index + Count], &this->_SSO.Buffer[Index], CurSize - Index + 1);
				memset(&this->_SSO.Buffer[Index], Char, Count);
				this->_SSO.Buffer[_SSOTail] += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index);
				memset(&NewPos[Index], Char, Count);
				memcpy(&NewPos[Index + Count], &this->_SSO.Buffer[Index], CurSize - Index + 1);

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		else{
			if (Index > this->_SSO.StrLen)throw std::out_of_range("Invalid string index.");
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity - CurSize >= Count){
				memmove(&this->_SSO.StrPos[Index + Count], &this->_SSO.StrPos[Index], CurSize - Index + 1);
				memset(&this->_SSO.StrPos[Index], Char, Count);
				this->_SSO.StrLen += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index);
				memset(&NewPos[Index], Char, Count);
				memcpy(&NewPos[Index + Count], &this->_SSO.StrPos[Index], CurSize - Index + 1);

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String& String::insert(size_type Index, const char *rhs, size_type Count){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity - CurSize >= Count){
				memmove(&this->_SSO.Buffer[Index + Count], &this->_SSO.Buffer[Index], CurSize - Index + 1);
				memcpy(&this->_SSO.Buffer[Index], rhs, Count);
				this->_SSO.Buffer[_SSOTail] += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();

				memcpy(NewPos, this->_SSO.Buffer, Index);
				memcpy(&NewPos[Index], rhs, Count);
				memcpy(&NewPos[Index + Count], &this->_SSO.Buffer[Index], CurSize - Index + 1);

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity - CurSize >= Count){
				memmove(&this->_SSO.StrPos[Index + Count], &this->_SSO.StrPos[Index], CurSize - Index + 1);
				memcpy(&this->_SSO.Buffer[Index], rhs, Count);
				this->_SSO.StrLen += (unsigned char)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index);
				memcpy(&NewPos[Index], rhs, Count);
				memcpy(&NewPos[Index + Count], &this->_SSO.StrPos[Index], CurSize - Index + 1);

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String& String::insert(size_type Index, const char *rhs){
		size_type rhsLen = strnlen(rhs, this->max_size());
		return this->insert(Index, rhs, rhsLen);
	}
	String& String::insert(size_type Index, const String &rhs){
		return this->insert(Index, (const char *)rhs.cbegin(), rhs.size());
	}
	String& String::replace(size_type Index, size_type ReplaceLen, value_type Char){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity > CurSize - ReplaceLen){
				memmove(&this->_SSO.Buffer[Index + 1], &this->_SSO.Buffer[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
				this->_SSO.Buffer[Index] = Char;
				this->_SSO.Buffer[_SSOTail]++;
			}
			else if (this->max_size() == CurSize - ReplaceLen)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + 2);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index);
				NewPos[Index] = Char;
				memcpy(&NewPos[Index + 1], &this->_SSO.Buffer[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + 1;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity > CurSize - ReplaceLen){
				//SSO special case
				if (_SSOTail > CurSize - ReplaceLen + 1){
					unsigned char *CurPos = this->_SSO.StrPos;
					this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)(CurSize - ReplaceLen + 1);
					memcpy(this->_SSO.Buffer, CurPos, Index);
					this->_SSO.Buffer[Index] = Char;
					memcpy(&this->_SSO.Buffer[Index + 1], &CurPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
					nsBasic::GlobalMemFree(CurPos, CurCapacity + 1);
				}
				else{
					memmove(&this->_SSO.StrPos[Index + 1], &this->_SSO.StrPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
					this->_SSO.StrPos[Index] = Char;
					this->_SSO.StrLen -= ReplaceLen - 1;
				}
			}
			else if (this->max_size() == CurSize - ReplaceLen)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + 2);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index);
				NewPos[Index] = Char;
				memcpy(&NewPos[Index + 1], &this->_SSO.StrPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + 1;
			}
		}
		return *this;
	}
	String& String::replace(size_type Index, size_type ReplaceLen, value_type Char, size_type Count){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity + ReplaceLen - CurSize >= Count){
				memmove(&this->_SSO.Buffer[Index + Count], &this->_SSO.Buffer[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
				memset(&this->_SSO.Buffer[Index], Char, Count);
				this->_SSO.Buffer[_SSOTail] -= (unsigned char)(ReplaceLen - Count);
			}
			else if (this->max_size() - (CurSize - ReplaceLen) < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index);
				memset(&NewPos[Index], Char, Count);
				memcpy(&NewPos[Index + Count], &this->_SSO.Buffer[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity >= CurSize - ReplaceLen + Count){
				//SSO special case
				if (_SSOTail > CurSize - ReplaceLen + Count){
					unsigned char *CurPos = this->_SSO.StrPos;
					this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)(CurSize - ReplaceLen + Count);
					memcpy(this->_SSO.Buffer, CurPos, Index);
					memset(&this->_SSO.Buffer[Index], Char, Count);
					memcpy(&this->_SSO.Buffer[Index + Count], &CurPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
					nsBasic::GlobalMemFree(CurPos, CurCapacity + 1);
				}
				else{
					memmove(&this->_SSO.StrPos[Index + Count], &this->_SSO.StrPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
					memset(&this->_SSO.StrPos[Index], Char, Count);
					this->_SSO.StrLen -= ReplaceLen - Count;
				}
			}
			else if (this->max_size() - (CurSize - ReplaceLen) < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index);
				memset(&NewPos[Index], Char, Count);
				memcpy(&NewPos[Index + Count], &this->_SSO.StrPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + Count;
			}
		}
		return *this;
	}
	String& String::replace(size_type Index, size_type ReplaceLen, const char *rhs, size_type Count){
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7F & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity + ReplaceLen - CurSize >= Count){
				memmove(&this->_SSO.Buffer[Index + Count], &this->_SSO.Buffer[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
				memcpy(&this->_SSO.Buffer[Index], rhs, Count);
				this->_SSO.Buffer[_SSOTail] -= (unsigned char)(ReplaceLen - Count);
			}
			else if (this->max_size() - (CurSize - ReplaceLen) < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index);
				memcpy(&NewPos[Index], rhs, Count);
				memcpy(&NewPos[Index + Count], &this->_SSO.Buffer[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity >= CurSize - ReplaceLen + Count){
				//SSO special case
				if (_SSOTail > CurSize - ReplaceLen + Count){
					unsigned char *CurPos = this->_SSO.StrPos;
					this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)(CurSize - ReplaceLen + Count);
					memcpy(this->_SSO.Buffer, CurPos, Index);
					memcpy(&this->_SSO.Buffer[Index], rhs, Count);
					memcpy(&this->_SSO.Buffer[Index + Count], &CurPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
					nsBasic::GlobalMemFree(CurPos, CurCapacity + 1);
				}
				else{
					memmove(&this->_SSO.StrPos[Index + Count], &this->_SSO.StrPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);
					memcpy(&this->_SSO.StrPos[Index], rhs, Count);
					this->_SSO.StrLen -= ReplaceLen - Count;
				}
			}
			else if (this->max_size() - (CurSize - ReplaceLen) < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + Count + 1);
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewLen);
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index);
				memcpy(&NewPos[Index], rhs, Count);
				memcpy(&NewPos[Index + Count], &this->_SSO.StrPos[Index + ReplaceLen], CurSize - Index - ReplaceLen + 1);

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + Count;
			}
		}
		return *this;
	}
	String& String::replace(size_type Index, size_type ReplaceLen, const char *rhs){
		size_type rhsLen = strnlen(rhs, this->max_size());
		return this->replace(Index, ReplaceLen, rhs, rhsLen);
	}
	String& String::replace(size_type Index, size_type ReplaceLen, const String &rhs){
		return this->replace(Index, ReplaceLen, (const char *)rhs.cbegin(), rhs.size());
	}
	String& String::erase(size_type Index, size_type Count){
		if (this->_SSO.Buffer[_SSOTail] & 0x80){
			size_type CurSize = this->_SSO.Buffer[_SSOTail] & 0x7F;
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurSize - Index < Count)Count = CurSize - Index;

			if (Count > 0){
				memmove(&this->_SSO.Buffer[Index], &this->_SSO.Buffer[Index + Count], CurSize - Index - Count + 1);
				this->_SSO.Buffer[_SSOTail] -= (unsigned char)Count;
			}
		}
		else{
			size_type CurSize = this->_SSO.StrLen;
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurSize - Index < Count)Count = CurSize - Index;

			if (Count > 0){
				memmove(&this->_SSO.StrPos[Index], &this->_SSO.StrPos[Index + Count], CurSize - Index - Count + 1);
				this->_SSO.StrLen -= (unsigned char)Count;
			}
		}
		return *this;
	}
	void String::resize(size_type NewSize){
		if (NewSize > this->max_size())throw std::length_error("String length exceeds its max size.");
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurSize = this->_SSO.Buffer[_SSOTail] & 0x7F;
			if (NewSize != CurSize){
				if (NewSize >= _SSOTail){
					unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewSize + 1);
					if (nullptr == NewPos)throw std::bad_alloc();
					memcpy(NewPos, this->_SSO.Buffer, CurSize);
					memset(&NewPos[CurSize], 0, NewSize - CurSize + 1);
					this->_SSO.StrPos = NewPos;
					this->_SSO.MemLen = NewSize + 1;
					this->_SSO.StrLen = NewSize;
				}
				else{
					if (NewSize > CurSize)memset(&this->_SSO.Buffer[CurSize], 0, NewSize - CurSize + 1);
					else this->_SSO.Buffer[NewSize] = '\0';
					this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)NewSize;
				}
			}
		}
		else if (NewSize != this->_SSO.StrLen){
			size_type CurMemLen = this->_SSO.MemLen;
			if (NewSize < _SSOTail){
				size_type CurSize = this->_SSO.StrLen;
				unsigned char *CurPos = this->_SSO.StrPos;
				this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)NewSize;
				if (NewSize > CurSize){
					memcpy(this->_SSO.Buffer, CurPos, CurSize);
					memset(&this->_SSO.Buffer[CurSize], 0, NewSize - CurSize + 1);
				}
				else{
					memcpy(this->_SSO.Buffer, CurPos, NewSize);
					this->_SSO.Buffer[NewSize] = '\0';
				}
				nsBasic::GlobalMemFree(CurPos, CurMemLen);
			}
			else{
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewSize + 1);
				if (nullptr == NewPos)throw std::bad_alloc();
				if (NewSize > this->_SSO.StrLen){
					memcpy(NewPos, this->_SSO.StrPos, this->_SSO.StrLen);
					memset(&NewPos[this->_SSO.StrLen], 0, NewSize - this->_SSO.StrLen + 1);
				}
				else{
					memcpy(NewPos, this->_SSO.StrPos, NewSize);
					NewPos[NewSize] = '\0';
				}
				this->_SSO.MemLen = NewSize + 1;
				this->_SSO.StrLen = NewSize;
				nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen);
				this->_SSO.StrPos = NewPos;
			}
		}
	}
	void String::resize(size_type NewSize, value_type Char){
		if (NewSize > this->max_size())throw std::length_error("String length exceeds its max size.");
		if (0x80 & this->_SSO.Buffer[_SSOTail]){
			size_type CurSize = this->_SSO.Buffer[_SSOTail] & 0x7F;
			if (NewSize != CurSize){
				if (NewSize >= _SSOTail){
					unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewSize + 1);
					if (nullptr == NewPos)throw std::bad_alloc();
					memcpy(NewPos, this->_SSO.Buffer, CurSize);
					memset(&NewPos[CurSize], Char, NewSize - CurSize);
					NewPos[NewSize] = '\0';
					this->_SSO.StrPos = NewPos;
					this->_SSO.MemLen = NewSize + 1;
					this->_SSO.StrLen = NewSize;
				}
				else{
					if (NewSize > CurSize){
						memset(&this->_SSO.Buffer[CurSize], Char, NewSize - CurSize);
						this->_SSO.Buffer[NewSize] = '\0';
					}
					else this->_SSO.Buffer[NewSize] = '\0';
					this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)NewSize;
				}
			}
		}
		else if (NewSize != this->_SSO.StrLen){
			size_type CurMemLen = this->_SSO.MemLen;
			if (NewSize < _SSOTail){
				size_type CurSize = this->_SSO.StrLen;
				unsigned char *CurPos = this->_SSO.StrPos;
				this->_SSO.Buffer[_SSOTail] = 0x80 + (unsigned char)NewSize;
				if (NewSize > CurSize){
					memcpy(this->_SSO.Buffer, CurPos, CurSize);
					memset(&this->_SSO.Buffer[CurSize], Char, NewSize - CurSize);
					this->_SSO.Buffer[NewSize] = '\0';
				}
				else{
					memcpy(this->_SSO.Buffer, CurPos, NewSize);
					this->_SSO.Buffer[NewSize] = '\0';
				}
				nsBasic::GlobalMemFree(CurPos, CurMemLen);
			}
			else{
				unsigned char *NewPos = (unsigned char *)nsBasic::GlobalMemAlloc(NewSize + 1);
				if (nullptr == NewPos)throw std::bad_alloc();
				if (NewSize > this->_SSO.StrLen){
					memcpy(NewPos, this->_SSO.StrPos, this->_SSO.StrLen);
					memset(&NewPos[this->_SSO.StrLen], Char, NewSize - this->_SSO.StrLen);
					NewPos[NewSize] = '\0';
				}
				else{
					memcpy(NewPos, this->_SSO.StrPos, NewSize);
					NewPos[NewSize] = '\0';
				}
				this->_SSO.MemLen = NewSize + 1;
				this->_SSO.StrLen = NewSize;
				nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen);
				this->_SSO.StrPos = NewPos;
			}
		}
	}
	void String::swap(String &rhs){
		String::_SSOStruct TmpSSO = this->_SSO;
		this->_SSO = rhs._SSO;
		rhs._SSO = TmpSSO;
	}

	String::size_type String::find(value_type Char, size_type InitPos) const{
		const_pointer InitPtr, RetPtr;
		if (this->_SSO.Buffer[_SSOTail] & 0x80){
			if (InitPos >= (this->_SSO.Buffer[_SSOTail] & 0x7Fu))return npos;
			else InitPtr = this->_SSO.Buffer;
		}
		else{
			if (InitPos >= this->_SSO.StrLen)return npos;
			else InitPtr = this->_SSO.StrPos;
		}
		RetPtr = (const_pointer)strchr((char *)(&InitPtr[InitPos]), Char);
		if (nullptr == RetPtr)return npos; else return RetPtr - InitPtr;
	}
	String::size_type String::find(const char *SubStr, size_type Count, size_type InitPos) const{
		size_type CurSize=this->size();
		const_pointer InitPtr = this->cbegin();
		if (InitPos >= CurSize)return npos;
		else return nsAlg::Search_Mem_Mem_Fwd(&InitPtr[InitPos], CurSize, SubStr, Count);
	}
	String::size_type String::find(const char *SubStr, size_type InitPos) const{
		size_type CurSize = this->size();
		const_pointer InitPtr = this->cbegin();
		if (InitPos >= CurSize)return npos;
		else try{ return nsAlg::Search_Mem_Str_Fwd(&InitPtr[InitPos], CurSize, SubStr); }
		catch (...){ return npos; } //no exception guarantee
	}
	String::size_type String::find(const String& SubStr, size_type InitPos) const{
		size_type CurSize = this->size();
		const_pointer InitPtr = this->cbegin();
		if (InitPos >= CurSize)return npos;
		else return nsAlg::Search_Mem_Mem_Fwd(&InitPtr[InitPos], CurSize, SubStr.cbegin(), SubStr.size());
	}
	String::size_type String::rfind(value_type Char, size_type InitPos) const{
		const_pointer InitPtr, RetPtr;
		if (this->_SSO.Buffer[_SSOTail] & 0x80){
			if (InitPos >= (this->_SSO.Buffer[_SSOTail] & 0x7Fu))InitPos = this->_SSO.Buffer[_SSOTail] & 0x7F;
			else InitPos++;
			InitPtr = this->_SSO.Buffer;
		}
		else{
			if (InitPos >= this->_SSO.StrLen)InitPos = this->_SSO.StrLen;
			else InitPos++;
			InitPtr = this->_SSO.StrPos;
		}
		RetPtr = (const_pointer)nsAlg::Search_Mem_Chr_Bwd(InitPtr, Char, InitPos);
		if (nullptr == RetPtr)return npos; else return RetPtr - InitPtr;
	}
	String::size_type String::rfind(const char *SubStr, size_type Count, size_type InitPos) const{
		size_type CurSize = this->size();
		if (CurSize < Count)return npos;
		if (InitPos>CurSize - Count)InitPos = CurSize; else InitPos += Count;
		return nsAlg::Search_Mem_Mem_Bwd(this->cbegin(), InitPos, SubStr, Count);
	}
	String::size_type String::rfind(const char *SubStr, size_type InitPos) const{
		size_type rhsLen = strlen(SubStr);
		return this->rfind(SubStr, rhsLen, InitPos);
	}
	String::size_type String::rfind(const String& SubStr, size_type InitPos) const{
		size_type CurSize = this->size();
		size_type Count = SubStr.size();
		if (CurSize < Count)return npos;
		if (InitPos>CurSize - Count)InitPos = CurSize; else InitPos += Count;
		return nsAlg::Search_Mem_Mem_Bwd(this->cbegin(), InitPos, SubStr.cbegin(), Count);
	}

	String String::substr(size_type Index, size_type Count){
		size_type CurSize = this->size();
		if (Index > CurSize)throw std::out_of_range("Invalid string index.");
		if (CurSize - Index < Count)Count = CurSize - Index;

		String RetValue(Count);
		memcpy(RetValue.begin(), this->cbegin() + Index, Count);
		return RetValue;
	}
	int String::compare(const String& rhs) const{
		if (this->size() < rhs.size())return -1;
		else if (this->size() > rhs.size())return 1;
		else return memcmp(this->cbegin(), rhs.cbegin(), this->size());
	}
	inline int String::compare(const char* rhs) const{
		return memcmp(this->cbegin(), rhs, this->size() + 1);
	}

	String& String::operator = (const String& rhs){
		String temp(rhs);
		this->swap(temp);
		return *this;
	}
	String& String::operator = (String&& rhs){
		this->_SSO = rhs._SSO;
		//get rhs ready for destruct
		rhs._SSO.Buffer[_SSOTail] = 0x80;
		return *this;
	}
	String& String::operator = (const char *rhs){
		String temp(rhs);
		this->swap(temp);
		return *this;
	}
	String& String::operator+=(value_type rhs){
		return this->append(rhs);
	}
	String& String::operator+=(const String& rhs){
		return this->append(rhs);
	}
	String& String::operator+=(const char *rhs){
		return this->append(rhs);
	}

	String::~String(){
		if (0 == (0x80 & this->_SSO.Buffer[_SSOTail]))nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen);
	}

	inline String operator+(const String& lhs, const String& rhs){
		String::size_type NewLen = lhs.size() + rhs.size();
		if (NewLen > lhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String RetValue(NewLen);
			memcpy(RetValue.begin(), lhs.cbegin(), lhs.size());
			memcpy(RetValue.begin() + lhs.size(), rhs.cbegin(), rhs.size());
			return RetValue;
		}
	}
	inline String operator+(const String& lhs, const char *rhs){
		String::size_type NewLen = lhs.size() + strnlen(rhs, lhs.max_size());
		if (NewLen > lhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String RetValue(NewLen);
			memcpy(RetValue.begin(), lhs.cbegin(), lhs.size());
			memcpy(RetValue.begin() + lhs.size(), rhs, NewLen - lhs.size());
			return RetValue;
		}
	}
	inline String operator+(const String& lhs, const String::value_type rhs){
		String::size_type NewLen = lhs.size() + 1;
		if (NewLen > lhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String RetValue(NewLen);
			memcpy(RetValue.begin(), lhs.cbegin(), lhs.size());
			RetValue.begin()[lhs.size()] = rhs;
			return RetValue;
		}
	}
	inline String operator+(const char *lhs, const String& rhs){
		String::size_type ChrLen = strnlen(lhs, rhs.max_size());
		String::size_type NewLen = rhs.size() + ChrLen;
		if (NewLen > rhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String RetValue(NewLen);
			memcpy(RetValue.begin(), lhs, ChrLen);
			memcpy(RetValue.begin() + ChrLen, rhs.cbegin(), rhs.size());
			return RetValue;
		}
	}
	inline String operator+(const String::value_type lhs, const String& rhs){
		String::size_type NewLen = rhs.size() + 1;
		if (NewLen > rhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String RetValue(NewLen);
			RetValue.begin()[0] = lhs;
			memcpy(&RetValue.begin()[1], rhs.cbegin(), rhs.size());
			return RetValue;
		}
	}
}

template<> struct std::hash<nsText::String>{
public:
	size_t operator()(const nsText::String &rhs) const{
		//BKDRHash from NOCOW
		size_t seed = 131, hash = 0;
		const unsigned char *CurPtr = rhs.cbegin();
		for (UBINT i = 0; i < rhs.size(); i++)hash = hash * seed + (*CurPtr++);
		return hash;
	}
};

#endif