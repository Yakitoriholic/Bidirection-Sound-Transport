/* Description:UTF16 String Class. Don't include this header directly.
 * Language:C++
 * Author:***
 *
 * This class is highly unreliable. Check the correctness of its member functions when necessary.
 */

#ifndef LIB_TEXT_STRING_W
#define LIB_TEXT_STRING_W

#include "lGeneral.hpp"
#include "lAlg.hpp"

namespace nsText{
	class String_W{
	public:
		// custom typedefs

		// container typedefs, mandated by the C++ standard
		typedef wchar_t			value_type;
		typedef wchar_t&		reference;
		typedef const wchar_t&	const_reference;
		typedef wchar_t*		pointer;
		typedef const wchar_t*	const_pointer;
		typedef UBINT			size_type;
		typedef UBINT			difference_type;

		// container typedefs of iterator, mandated by the C++ standard
		typedef pointer			iterator;
		typedef const_pointer	const_iterator;

		// constants
		static const size_type npos = (size_type)-1;

	protected:
		union _SSOStruct{
			wchar_t Buffer[3 * sizeof(UBINT) / sizeof(wchar_t)]; //This works when 3 * sizeof(UBINT) < 0xFFFF.
			struct{
				wchar_t *StrPos;
				UBINT StrLen;
				UBINT MemLen;
			};
		} _SSO;

		static const size_type _SSOTail = (3 * sizeof(UBINT) / sizeof(wchar_t)) - 1;
	public:
		//constructors
		String_W();
		String_W(size_type Count);
		String_W(size_type Count, value_type Char);
		String_W(const String_W &rhs);
		String_W(String_W &&rhs);
		String_W(const String &rhs);
		String_W(const wchar_t *rhs);
		String_W(const wchar_t *rhs, size_type Count);

		// iterator functions, mandated by the C++ standard
		iterator begin();
		const_iterator cbegin() const;
		iterator end();
		const_iterator cend() const;

		//element access functions
		inline reference operator[](size_type n){ return this->begin()[n]; }

		//capacity functions
		inline bool empty() const { return 0x8000 == this->_SSO.Buffer[0]; }
		size_type size() const;
		size_type max_size() const;
		void reserve(size_type NewCapacity);
		size_type capacity() const;
		void shrink_to_fit();

		//modifiers
		void clear();
		String_W& append(UINT4b CodePoint); //Here [CodePoint] is the Unicode code point of this character.
		String_W& append(value_type Char);
		String_W& append(value_type Char, size_type Count);
		String_W& append(const wchar_t *rhs, size_type Count);
		String_W& append(const wchar_t *rhs);
		String_W& append(const String_W &rhs);
		String_W& insert(size_type Index, value_type Char);
		String_W& insert(size_type Index, value_type Char, size_type Count);
		String_W& insert(size_type Index, const wchar_t *rhs, size_type Count);
		String_W& insert(size_type Index, const wchar_t *rhs);
		String_W& insert(size_type Index, const String_W &rhs);
		String_W& replace(size_type Index, size_type ReplaceLen, value_type Char);
		String_W& replace(size_type Index, size_type ReplaceLen, value_type Char, size_type Count);
		String_W& replace(size_type Index, size_type ReplaceLen, const wchar_t *rhs, size_type Count);
		String_W& replace(size_type Index, size_type ReplaceLen, const wchar_t *rhs);
		String_W& replace(size_type Index, size_type ReplaceLen, const String_W &rhs);
		String_W& erase(size_type Index = 0, size_type Count = 1);
		void resize(size_type NewSize);
		void resize(size_type NewSize, value_type Char);
		void swap(String_W &rhs);

		//query functions
		/*size_type find(value_type Char, size_type InitPos = 0) const;
		size_type find(const wchar_t *SubStr, size_type Count, size_type InitPos = 0) const;
		size_type find(const wchar_t *SubStr, size_type InitPos = 0) const;
		size_type find(const String_W& SubStr, size_type InitPos = 0) const;
		size_type rfind(value_type Char, size_type InitPos = npos) const;
		size_type rfind(const wchar_t *SubStr, size_type Count, size_type InitPos = npos) const;
		size_type rfind(const wchar_t *SubStr, size_type InitPos = npos) const;
		size_type rfind(const String_W& SubStr, size_type InitPos = npos) const;*/

		//other operations
		String_W substr(size_type Index = 0, size_type Count = npos);
		int compare(const String_W& rhs) const;
		inline int compare(const wchar_t* rhs) const;

		//operators
		String_W& operator=(const String_W& rhs);
		String_W& operator=(String_W&& rhs);
		String_W& operator=(const wchar_t *rhs);
		String_W& operator+=(value_type rhs);
		String_W& operator+=(const String_W& rhs);
		String_W& operator+=(const wchar_t *rhs);

		//destructor
		~String_W();
	};

	inline String_W operator+(const String_W& lhs, const String_W& rhs);
	inline String_W operator+(const String_W& lhs, const wchar_t *rhs);
	inline String_W operator+(const String_W& lhs, const String_W::value_type rhs);
	inline String_W operator+(const wchar_t *lhs, const String_W& rhs);
	inline String_W operator+(const String_W::value_type lhs, const String_W& rhs);

	inline bool operator==(const String_W& lhs, const String_W& rhs){ return 0 == lhs.compare(rhs); }
	inline bool operator!=(const String_W& lhs, const String_W& rhs){ return 0 != lhs.compare(rhs); }
	inline bool operator<(const String_W& lhs, const String_W& rhs){ return 0 > lhs.compare(rhs); }
	inline bool operator<=(const String_W& lhs, const String_W& rhs){ return 0 >= lhs.compare(rhs); }
	inline bool operator>(const String_W& lhs, const String_W& rhs){ return 0 < lhs.compare(rhs); }
	inline bool operator>=(const String_W& lhs, const String_W& rhs){ return 0 <= lhs.compare(rhs); }

	inline bool operator==(const String_W& lhs, const wchar_t* rhs){ return 0 == lhs.compare(rhs); }
	inline bool operator!=(const String_W& lhs, const wchar_t* rhs){ return 0 != lhs.compare(rhs); }
	inline bool operator<(const String_W& lhs, const wchar_t* rhs){ return 0 > lhs.compare(rhs); }
	inline bool operator<=(const String_W& lhs, const wchar_t* rhs){ return 0 >= lhs.compare(rhs); }
	inline bool operator>(const String_W& lhs, const wchar_t* rhs){ return 0 < lhs.compare(rhs); }
	inline bool operator>=(const String_W& lhs, const wchar_t* rhs){ return 0 <= lhs.compare(rhs); }

	inline bool operator==(const wchar_t* lhs, const String_W& rhs){ return 0 == rhs.compare(lhs); }
	inline bool operator!=(const wchar_t* lhs, const String_W& rhs){ return 0 != rhs.compare(lhs); }
	inline bool operator<(const wchar_t* lhs, const String_W& rhs){ return 0 < rhs.compare(lhs); }
	inline bool operator<=(const wchar_t* lhs, const String_W& rhs){ return 0 <= rhs.compare(lhs); }
	inline bool operator>(const wchar_t* lhs, const String_W& rhs){ return 0 > rhs.compare(lhs); }
	inline bool operator>=(const wchar_t* lhs, const String_W& rhs){ return 0 >= rhs.compare(lhs); }
	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	String_W::String_W(){
		this->_SSO.Buffer[_SSOTail] = 0x8000;
		this->_SSO.Buffer[0] = L'\0';
	}
	String_W::String_W(size_type Count){
		if (Count > this->max_size())Count = this->max_size();
		if (Count < _SSOTail){
			this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)Count;
			this->_SSO.Buffer[Count] = L'\0';
		}
		else{
			this->_SSO.StrPos = (wchar_t *)nsBasic::GlobalMemAlloc((Count + 1) * sizeof(wchar_t));
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.MemLen = Count + 1;
				this->_SSO.StrLen = Count;
				this->_SSO.StrPos[Count] = L'\0';
			}
		}
	}
	String_W::String_W(size_type Count, value_type Char){
		if (Count > this->max_size())Count = this->max_size();
		if (Count < _SSOTail){
			this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)Count;
			for (UBINT i = 0; i < Count; i++)this->_SSO.Buffer[i] = Char;
			this->_SSO.Buffer[Count] = L'\0';
		}
		else{
			this->_SSO.StrPos = (wchar_t *)nsBasic::GlobalMemAlloc((Count + 1) * sizeof(wchar_t));
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.MemLen = Count + 1;
				this->_SSO.StrLen = Count;
				for (UBINT i = 0; i < Count; i++)this->_SSO.Buffer[i] = Char;
				this->_SSO.StrPos[Count] = L'\0';
			}
		}
	}
	String_W::String_W(const String_W &rhs){
		if (0x8000 & rhs._SSO.Buffer[_SSOTail])this->_SSO = rhs._SSO;
		else{
			this->_SSO.StrPos = (wchar_t *)nsBasic::GlobalMemAlloc((rhs._SSO.MemLen) * sizeof(wchar_t));
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.MemLen = rhs._SSO.MemLen;
				this->_SSO.StrLen = rhs._SSO.StrLen;
				memcpy(this->_SSO.StrPos, rhs._SSO.StrPos, (this->_SSO.StrLen + 1) * sizeof(wchar_t));
			}
		}
	}
	String_W::String_W(String_W &&rhs){
		this->_SSO = rhs._SSO;
		//get rhs ready for destruct
		rhs._SSO.Buffer[_SSOTail] = 0x8000;
	}
	String_W::String_W(const wchar_t *rhs){
		size_t rhsLen = wcsnlen(rhs, this->max_size());
		if (rhsLen >= _SSOTail){
			this->_SSO.StrPos = (wchar_t *)nsBasic::GlobalMemAlloc((rhsLen + 1) * sizeof(wchar_t));
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.StrLen = rhsLen;
				this->_SSO.MemLen = rhsLen + 1;
				memcpy(this->_SSO.StrPos, rhs, rhsLen * sizeof(wchar_t));
				this->_SSO.StrPos[rhsLen] = L'\0';
			}
		}
		else{
			this->_SSO.Buffer[_SSOTail] = (wchar_t)(0x8000 + rhsLen);
			memcpy(this->_SSO.Buffer, rhs, (rhsLen + 1) * sizeof(wchar_t));
		}
	}
	String_W::String_W(const wchar_t *rhs, size_type Count){
		if (Count >= _SSOTail){
			this->_SSO.StrPos = (wchar_t *)nsBasic::GlobalMemAlloc((Count + 1) * sizeof(wchar_t));
			if (nullptr == this->_SSO.StrPos)throw std::bad_alloc();
			else{
				this->_SSO.StrLen = Count;
				this->_SSO.MemLen = Count + 1;
				memcpy(this->_SSO.StrPos, rhs, Count * sizeof(wchar_t));
				this->_SSO.StrPos[Count] = L'\0';
			}
		}
		else{
			this->_SSO.Buffer[_SSOTail] = (wchar_t)(0x8000 + Count);
			memcpy(this->_SSO.Buffer, rhs, Count * sizeof(wchar_t));
			this->_SSO.Buffer[Count] = L'\0';
		}
	}
	inline String_W::iterator String_W::begin(){
		if (0x8000 & this->_SSO.Buffer[_SSOTail])return this->_SSO.Buffer;
		else return this->_SSO.StrPos;
	}
	inline String_W::const_iterator String_W::cbegin() const{
		if (0x8000 & this->_SSO.Buffer[_SSOTail])return this->_SSO.Buffer;
		else return this->_SSO.StrPos;
	}
	inline String_W::iterator String_W::end(){
		if (0x8000 & this->_SSO.Buffer[_SSOTail])return &this->_SSO.Buffer[(0x7FFF & this->_SSO.Buffer[_SSOTail])];
		else return this->_SSO.StrPos + this->_SSO.StrLen;
	}
	inline String_W::const_iterator String_W::cend() const{
		if (0x8000 & this->_SSO.Buffer[_SSOTail])return &this->_SSO.Buffer[(0x7FFF & this->_SSO.Buffer[_SSOTail])];
		else return this->_SSO.StrPos + this->_SSO.StrLen;
	}
	inline String_W::size_type String_W::size() const{
		if (0x8000 & this->_SSO.Buffer[_SSOTail])return 0x7FFF & this->_SSO.Buffer[_SSOTail];
		else return this->_SSO.StrLen;
	}
	inline String_W::size_type String_W::max_size() const{
#if defined LIBENV_SYS_INTELX64
		return 0x7FFFFFFFFFFFFFFF;
#elif defined LIBENV_SYS_INTELX86
		return 0x7FFFFFFF;
#endif
	}
	void String_W::reserve(size_type NewCapacity){
		if (NewCapacity > this->max_size())throw std::length_error("String length exceeds its max size.");
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			if (NewCapacity >= _SSOTail){
				size_type CurSize = this->_SSO.Buffer[_SSOTail] & 0x7FFF;
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc((NewCapacity + 1) * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, (CurSize + 1) * sizeof(wchar_t));

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
				wchar_t *CurPos = this->_SSO.StrPos;
				this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)CurSize;
				memcpy(this->_SSO.Buffer, CurPos, (CurSize + 1) * sizeof(wchar_t));
				nsBasic::GlobalMemFree(CurPos, CurMemLen * sizeof(wchar_t));
			}
			else{
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc((NewCapacity + 1) * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, (this->_SSO.StrLen + 1) * sizeof(wchar_t));
				this->_SSO.MemLen = NewCapacity + 1;
				nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
			}
		}
	}
	String_W::size_type String_W::capacity() const{
		if (0x8000 & this->_SSO.Buffer[_SSOTail])return _SSOTail - 1;
		else return this->_SSO.MemLen - 1;
	}
	void String_W::shrink_to_fit(){
		if (0 == (0x8000 & this->_SSO.Buffer[_SSOTail]) && this->_SSO.StrLen + 1 < this->_SSO.MemLen){
			size_type CurMemLen = this->_SSO.MemLen;
			if (this->_SSO.StrLen < _SSOTail){
				size_type CurSize = this->_SSO.StrLen;
				this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)CurSize;
				memcpy(this->_SSO.Buffer, this->_SSO.StrPos, (CurSize + 1) * sizeof(wchar_t));
				nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen * sizeof(wchar_t));
			}
			else{
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc((this->_SSO.StrLen + 1) * sizeof(wchar_t));
				if (nullptr != NewPos){
					memcpy(NewPos, this->_SSO.StrPos, (this->_SSO.StrLen + 1) * sizeof(wchar_t));
					this->_SSO.MemLen = this->_SSO.StrLen + 1;
					nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen * sizeof(wchar_t));
					this->_SSO.StrPos = NewPos;
				}
			}
		}
	}
	inline void String_W::clear(){
		if (0 == (0x8000 & this->_SSO.Buffer[_SSOTail]))nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
		this->_SSO.Buffer[_SSOTail] = 0x8000;
		this->_SSO.Buffer[0] = L'\0';
	}
	String_W& String_W::append(UINT4b CodePoint){
		//Encode the codepoint
		wchar_t EncodedChar[2];
		UBINT Count;
		if (CodePoint >= 0x110000 || 0xD800 == (CodePoint & 0x1FF800))return *this; //Failed. restricted by RFC 3629
		else if (CodePoint < 0x10000){
			EncodedChar[0] = (wchar_t)CodePoint;
			Count = 1;
		}
		else{
			CodePoint -= 0x10000;
			EncodedChar[0] = (wchar_t)((CodePoint >> 10) + 0xD800);
			EncodedChar[1] = (wchar_t)((CodePoint & 0x3FF) + 0xDC00);
			Count = 2;
		}

		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity - CurSize >= Count){
				for (UBINT i = 0; i < Count; i++)this->_SSO.Buffer[CurSize + i] = EncodedChar[i];
				this->_SSO.Buffer[CurSize + Count] = L'\0';
				this->_SSO.Buffer[_SSOTail] += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)NewPos[CurSize + i] = EncodedChar[i];
				NewPos[CurSize + Count] = L'\0';

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
				this->_SSO.StrPos[CurSize + Count] = L'\0';
				this->_SSO.StrLen += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, CurSize * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)NewPos[CurSize + i] = EncodedChar[i];
				NewPos[CurSize + Count] = L'\0';

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String_W& String_W::append(value_type Char){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity > CurSize){
				this->_SSO.Buffer[CurSize] = Char;
				this->_SSO.Buffer[CurSize + 1] = L'\0';
				this->_SSO.Buffer[_SSOTail] += 1;
			}
			else if (this->max_size() == CurSize)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + 2);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize * sizeof(wchar_t));
				NewPos[CurSize] = Char;
				NewPos[CurSize + 1] = L'\0';

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
				this->_SSO.StrPos[CurSize + 1] = L'\0';
				this->_SSO.StrLen += 1;
			}
			else if (this->max_size() == CurSize)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + 2);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, CurSize * sizeof(wchar_t));
				NewPos[CurSize] = Char;
				NewPos[CurSize + 1] = L'\0';

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + 1;
			}
		}
		return *this;
	}
	String_W& String_W::append(value_type Char, size_type Count){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity - CurSize >= Count){
				for (UBINT i = 0; i < Count; i++)this->_SSO.Buffer[CurSize + i] = Char;
				this->_SSO.Buffer[CurSize + Count] = L'\0';
				this->_SSO.Buffer[_SSOTail] += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)NewPos[CurSize + i] = Char;
				NewPos[CurSize + Count] = L'\0';

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity - CurSize >= Count){
				for (UBINT i = 0; i < Count; i++)this->_SSO.StrPos[CurSize + i] = Char;
				this->_SSO.StrPos[CurSize + Count] = L'\0';
				this->_SSO.StrLen += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, CurSize * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)NewPos[CurSize + i] = Char;
				NewPos[CurSize + Count] = L'\0';

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String_W& String_W::append(const wchar_t *rhs, size_type Count){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity - CurSize >= Count){
				memcpy(&this->_SSO.Buffer[CurSize], rhs, Count * sizeof(wchar_t));
				this->_SSO.Buffer[CurSize + Count] = L'\0';
				this->_SSO.Buffer[_SSOTail] += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, CurSize * sizeof(wchar_t));
				memcpy(&NewPos[CurSize], rhs, Count * sizeof(wchar_t));
				NewPos[CurSize + Count] = L'\0';

				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity - CurSize >= Count){
				memcpy(&this->_SSO.StrPos[CurSize], rhs, Count * sizeof(wchar_t));
				this->_SSO.StrPos[CurSize + Count] = L'\0';
				this->_SSO.StrLen += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, CurSize * sizeof(wchar_t));
				memcpy(&NewPos[CurSize], rhs, Count * sizeof(wchar_t));
				NewPos[CurSize + Count] = L'\0';

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String_W& String_W::append(const wchar_t *rhs){
		size_type rhsLen = wcsnlen(rhs, this->max_size());
		return this->append(rhs, rhsLen);
	}
	String_W& String_W::append(const String_W& rhs){
		size_type rhsLen = rhs.size();
		return this->append((const wchar_t *)rhs.cbegin(), rhsLen);
	}
	String_W& String_W::insert(size_type Index, value_type Char){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurCapacity > CurSize){
				memmove(&this->_SSO.Buffer[Index + 1], &this->_SSO.Buffer[Index], (CurSize - Index + 1) * sizeof(wchar_t));
				this->_SSO.Buffer[Index] = Char;
				this->_SSO.Buffer[_SSOTail]++;
			}
			else if (this->max_size() == CurSize)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + 2);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index * sizeof(wchar_t));
				NewPos[Index] = Char;
				memcpy(&NewPos[Index + 1], &this->_SSO.Buffer[Index], (CurSize - Index + 1) * sizeof(wchar_t));

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
				memmove(&this->_SSO.StrPos[Index + 1], &this->_SSO.StrPos[Index], (CurSize - Index + 1) * sizeof(wchar_t));
				this->_SSO.StrPos[Index] = Char;
				this->_SSO.StrLen++;
			}
			else if (this->max_size() == CurSize)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + 2);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index * sizeof(wchar_t));
				NewPos[Index] = Char;
				memcpy(&NewPos[Index + 1], &this->_SSO.StrPos[Index], (CurSize - Index + 1) * sizeof(wchar_t));

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + 1;
			}
		}
		return *this;
	}
	String_W& String_W::insert(size_type Index, value_type Char, size_type Count){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurCapacity - CurSize >= Count){
				memmove(&this->_SSO.Buffer[Index + Count], &this->_SSO.Buffer[Index], (CurSize - Index + 1) * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)this->_SSO.Buffer[Index + i] = Char;
				this->_SSO.Buffer[_SSOTail] += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)NewPos[Index + i] = Char;
				memcpy(&NewPos[Index + Count], &this->_SSO.Buffer[Index], (CurSize - Index + 1) * sizeof(wchar_t));

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
				memmove(&this->_SSO.StrPos[Index + Count], &this->_SSO.StrPos[Index], (CurSize - Index + 1) * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)this->_SSO.StrPos[Index + i] = Char;
				this->_SSO.StrLen += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)NewPos[Index + i] = Char;
				memcpy(&NewPos[Index + Count], &this->_SSO.StrPos[Index], (CurSize - Index + 1) * sizeof(wchar_t));

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String_W& String_W::insert(size_type Index, const wchar_t *rhs, size_type Count){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (CurCapacity - CurSize >= Count){
				memmove(&this->_SSO.Buffer[Index + Count], &this->_SSO.Buffer[Index], (CurSize - Index + 1) * sizeof(wchar_t));
				memcpy(&this->_SSO.Buffer[Index], rhs, Count * sizeof(wchar_t));
				this->_SSO.Buffer[_SSOTail] += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();

				memcpy(NewPos, this->_SSO.Buffer, Index * sizeof(wchar_t));
				memcpy(&NewPos[Index], rhs, Count * sizeof(wchar_t));
				memcpy(&NewPos[Index + Count], &this->_SSO.Buffer[Index], (CurSize - Index + 1) * sizeof(wchar_t));

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		else{
			size_type CurCapacity = this->_SSO.MemLen - 1;
			size_type CurSize = this->_SSO.StrLen;
			if (CurCapacity - CurSize >= Count){
				memmove(&this->_SSO.StrPos[Index + Count], &this->_SSO.StrPos[Index], (CurSize - Index + 1) * sizeof(wchar_t));
				memcpy(&this->_SSO.Buffer[Index], rhs, Count * sizeof(wchar_t));
				this->_SSO.StrLen += (wchar_t)Count;
			}
			else if (this->max_size() - CurSize < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index * sizeof(wchar_t));
				memcpy(&NewPos[Index], rhs, Count * sizeof(wchar_t));
				memcpy(&NewPos[Index + Count], &this->_SSO.StrPos[Index], (CurSize - Index + 1) * sizeof(wchar_t));

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize + Count;
			}
		}
		return *this;
	}
	String_W& String_W::insert(size_type Index, const wchar_t *rhs){
		size_type rhsLen = wcsnlen(rhs, this->max_size());
		return this->insert(Index, rhs, rhsLen);
	}
	String_W& String_W::insert(size_type Index, const String_W &rhs){
		return this->insert(Index, (const wchar_t *)rhs.cbegin(), rhs.size());
	}
	String_W& String_W::replace(size_type Index, size_type ReplaceLen, value_type Char){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity > CurSize - ReplaceLen){
				memmove(&this->_SSO.Buffer[Index + 1], &this->_SSO.Buffer[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
				this->_SSO.Buffer[Index] = Char;
				this->_SSO.Buffer[_SSOTail]++;
			}
			else if (this->max_size() == CurSize - ReplaceLen)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + 2);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index * sizeof(wchar_t));
				NewPos[Index] = Char;
				memcpy(&NewPos[Index + 1], &this->_SSO.Buffer[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));

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
					wchar_t *CurPos = this->_SSO.StrPos;
					this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)(CurSize - ReplaceLen + 1);
					memcpy(this->_SSO.Buffer, CurPos, Index * sizeof(wchar_t));
					this->_SSO.Buffer[Index] = Char;
					memcpy(&this->_SSO.Buffer[Index + 1], &CurPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
					nsBasic::GlobalMemFree(CurPos, (CurCapacity + 1) * sizeof(wchar_t));
				}
				else{
					memmove(&this->_SSO.StrPos[Index + 1], &this->_SSO.StrPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
					this->_SSO.StrPos[Index] = Char;
					this->_SSO.StrLen -= ReplaceLen - 1;
				}
			}
			else if (this->max_size() == CurSize - ReplaceLen)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + 2);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index * sizeof(wchar_t));
				NewPos[Index] = Char;
				memcpy(&NewPos[Index + 1], &this->_SSO.StrPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + 1;
			}
		}
		return *this;
	}
	String_W& String_W::replace(size_type Index, size_type ReplaceLen, value_type Char, size_type Count){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity + ReplaceLen - CurSize >= Count){
				memmove(&this->_SSO.Buffer[Index + Count], &this->_SSO.Buffer[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)this->_SSO.Buffer[Index + i] = Char;
				this->_SSO.Buffer[_SSOTail] -= (wchar_t)(ReplaceLen - Count);
			}
			else if (this->max_size() - (CurSize - ReplaceLen) < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)NewPos[Index + i] = Char;
				memcpy(&NewPos[Index + Count], &this->_SSO.Buffer[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));

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
					wchar_t *CurPos = this->_SSO.StrPos;
					this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)(CurSize - ReplaceLen + Count);
					memcpy(this->_SSO.Buffer, CurPos, Index * sizeof(wchar_t));
					for (UBINT i = 0; i < Count; i++)this->_SSO.Buffer[Index + i] = Char;
					memcpy(&this->_SSO.Buffer[Index + Count], &CurPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
					nsBasic::GlobalMemFree(CurPos, (CurCapacity + 1) * sizeof(wchar_t));
				}
				else{
					memmove(&this->_SSO.StrPos[Index + Count], &this->_SSO.StrPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
					for (UBINT i = 0; i < Count; i++)this->_SSO.StrPos[Index + i] = Char;
					this->_SSO.StrLen -= ReplaceLen - Count;
				}
			}
			else if (this->max_size() - (CurSize - ReplaceLen) < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index * sizeof(wchar_t));
				for (UBINT i = 0; i < Count; i++)NewPos[Index + i] = Char;
				memcpy(&NewPos[Index + Count], &this->_SSO.StrPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + Count;
			}
		}
		return *this;
	}
	String_W& String_W::replace(size_type Index, size_type ReplaceLen, const wchar_t *rhs, size_type Count){
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurCapacity = _SSOTail - 1;
			size_type CurSize = 0x7FFF & this->_SSO.Buffer[_SSOTail];
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (ReplaceLen > CurSize - Index)ReplaceLen = CurSize - Index;
			if (CurCapacity + ReplaceLen - CurSize >= Count){
				memmove(&this->_SSO.Buffer[Index + Count], &this->_SSO.Buffer[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
				memcpy(&this->_SSO.Buffer[Index], rhs, Count * sizeof(wchar_t));
				this->_SSO.Buffer[_SSOTail] -= (wchar_t)(ReplaceLen - Count);
			}
			else if (this->max_size() - (CurSize - ReplaceLen) < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.Buffer, Index * sizeof(wchar_t));
				memcpy(&NewPos[Index], rhs, Count * sizeof(wchar_t));
				memcpy(&NewPos[Index + Count], &this->_SSO.Buffer[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));

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
					wchar_t *CurPos = this->_SSO.StrPos;
					this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)(CurSize - ReplaceLen + Count);
					memcpy(this->_SSO.Buffer, CurPos, Index * sizeof(wchar_t));
					memcpy(&this->_SSO.Buffer[Index], rhs, Count * sizeof(wchar_t));
					memcpy(&this->_SSO.Buffer[Index + Count], &CurPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
					nsBasic::GlobalMemFree(CurPos, (CurCapacity + 1) * sizeof(wchar_t));
				}
				else{
					memmove(&this->_SSO.StrPos[Index + Count], &this->_SSO.StrPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));
					memcpy(&this->_SSO.StrPos[Index], rhs, Count * sizeof(wchar_t));
					this->_SSO.StrLen -= ReplaceLen - Count;
				}
			}
			else if (this->max_size() - (CurSize - ReplaceLen) < Count)throw std::length_error("String length exceeds its max size.");
			else{
				size_type NewLen = nsMath::ceil2power(CurSize - ReplaceLen + Count + 1);
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc(NewLen * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				memcpy(NewPos, this->_SSO.StrPos, Index * sizeof(wchar_t));
				memcpy(&NewPos[Index], rhs, Count * sizeof(wchar_t));
				memcpy(&NewPos[Index + Count], &this->_SSO.StrPos[Index + ReplaceLen], (CurSize - Index - ReplaceLen + 1) * sizeof(wchar_t));

				nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
				this->_SSO.MemLen = NewLen;
				this->_SSO.StrLen = CurSize - ReplaceLen + Count;
			}
		}
		return *this;
	}
	String_W& String_W::replace(size_type Index, size_type ReplaceLen, const wchar_t *rhs){
		size_type rhsLen = wcsnlen(rhs, this->max_size());
		return this->replace(Index, ReplaceLen, rhs, rhsLen);
	}
	String_W& String_W::replace(size_type Index, size_type ReplaceLen, const String_W &rhs){
		return this->replace(Index, ReplaceLen, (const wchar_t *)rhs.cbegin(), rhs.size());
	}
	String_W& String_W::erase(size_type Index, size_type Count){
		if (this->_SSO.Buffer[_SSOTail] & 0x8000){
			size_type CurSize = this->_SSO.Buffer[_SSOTail] & 0x7FFF;
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurSize - Index < Count)Count = CurSize - Index;

			if (Count > 0){
				memmove(&this->_SSO.Buffer[Index], &this->_SSO.Buffer[Index + Count], (CurSize - Index - Count + 1) * sizeof(wchar_t));
				this->_SSO.Buffer[_SSOTail] -= (wchar_t)Count;
			}
		}
		else{
			size_type CurSize = this->_SSO.StrLen;
			if (Index > CurSize)throw std::out_of_range("Invalid string index.");
			if (CurSize - Index < Count)Count = CurSize - Index;

			if (Count > 0){
				memmove(&this->_SSO.StrPos[Index], &this->_SSO.StrPos[Index + Count], (CurSize - Index - Count + 1) * sizeof(wchar_t));
				this->_SSO.StrLen -= (wchar_t)Count;
			}
		}
		return *this;
	}
	void String_W::resize(size_type NewSize){
		if (NewSize > this->max_size())throw std::length_error("String length exceeds its max size.");
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurSize = this->_SSO.Buffer[_SSOTail] & 0x7FFF;
			if (NewSize != CurSize){
				if (NewSize >= _SSOTail){
					wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc((NewSize + 1) * sizeof(wchar_t));
					if (nullptr == NewPos)throw std::bad_alloc();
					memcpy(NewPos, this->_SSO.Buffer, CurSize * sizeof(wchar_t));
					for (UBINT i = CurSize; i < NewSize + 1; i++)NewPos[i] = 0;
					this->_SSO.StrPos = NewPos;
					this->_SSO.MemLen = NewSize + 1;
					this->_SSO.StrLen = NewSize;
				}
				else{
					if (NewSize > CurSize)for (UBINT i = CurSize; i < NewSize + 1; i++)this->_SSO.Buffer[i] = 0;
					else this->_SSO.Buffer[NewSize] = L'\0';
					this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)NewSize;
				}
			}
		}
		else if (NewSize != this->_SSO.StrLen){
			size_type CurMemLen = this->_SSO.MemLen;
			if (NewSize < _SSOTail){
				size_type CurSize = this->_SSO.StrLen;
				wchar_t *CurPos = this->_SSO.StrPos;
				this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)NewSize;
				if (NewSize > CurSize){
					memcpy(this->_SSO.Buffer, CurPos, CurSize * sizeof(wchar_t));
					for (UBINT i = CurSize; i < NewSize + 1; i++)this->_SSO.Buffer[i] = 0;
				}
				else{
					memcpy(this->_SSO.Buffer, CurPos, NewSize * sizeof(wchar_t));
					this->_SSO.Buffer[NewSize] = L'\0';
				}
				nsBasic::GlobalMemFree(CurPos, CurMemLen * sizeof(wchar_t));
			}
			else{
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc((NewSize + 1) * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				if (NewSize > this->_SSO.StrLen){
					memcpy(NewPos, this->_SSO.StrPos, this->_SSO.StrLen * sizeof(wchar_t));
					for (UBINT i = this->_SSO.StrLen; i < NewSize + 1; i++)NewPos[i] = 0;
				}
				else{
					memcpy(NewPos, this->_SSO.StrPos, NewSize * sizeof(wchar_t));
					NewPos[NewSize] = L'\0';
				}
				this->_SSO.MemLen = NewSize + 1;
				this->_SSO.StrLen = NewSize;
				nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
			}
		}
	}
	void String_W::resize(size_type NewSize, value_type Char){
		if (NewSize > this->max_size())throw std::length_error("String length exceeds its max size.");
		if (0x8000 & this->_SSO.Buffer[_SSOTail]){
			size_type CurSize = this->_SSO.Buffer[_SSOTail] & 0x7FFF;
			if (NewSize != CurSize){
				if (NewSize >= _SSOTail){
					wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc((NewSize + 1) * sizeof(wchar_t));
					if (nullptr == NewPos)throw std::bad_alloc();
					memcpy(NewPos, this->_SSO.Buffer, CurSize * sizeof(wchar_t));
					for (UBINT i = CurSize; i < NewSize; i++)NewPos[i] = Char;
					NewPos[NewSize] = L'\0';
					this->_SSO.StrPos = NewPos;
					this->_SSO.MemLen = NewSize + 1;
					this->_SSO.StrLen = NewSize;
				}
				else{
					if (NewSize > CurSize){
						for (UBINT i = CurSize; i < NewSize; i++)this->_SSO.Buffer[i] = Char;
						this->_SSO.Buffer[NewSize] = L'\0';
					}
					else this->_SSO.Buffer[NewSize] = L'\0';
					this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)NewSize;
				}
			}
		}
		else if (NewSize != this->_SSO.StrLen){
			size_type CurMemLen = this->_SSO.MemLen;
			if (NewSize < _SSOTail){
				size_type CurSize = this->_SSO.StrLen;
				wchar_t *CurPos = this->_SSO.StrPos;
				this->_SSO.Buffer[_SSOTail] = 0x8000 + (wchar_t)NewSize;
				if (NewSize > CurSize){
					memcpy(this->_SSO.Buffer, CurPos, CurSize * sizeof(wchar_t));
					for (UBINT i = CurSize; i < NewSize; i++)this->_SSO.Buffer[i] = Char;
					this->_SSO.Buffer[NewSize] = L'\0';
				}
				else{
					memcpy(this->_SSO.Buffer, CurPos, NewSize * sizeof(wchar_t));
					this->_SSO.Buffer[NewSize] = L'\0';
				}
				nsBasic::GlobalMemFree(CurPos, CurMemLen * sizeof(wchar_t));
			}
			else{
				wchar_t *NewPos = (wchar_t *)nsBasic::GlobalMemAlloc((NewSize + 1) * sizeof(wchar_t));
				if (nullptr == NewPos)throw std::bad_alloc();
				if (NewSize > this->_SSO.StrLen){
					memcpy(NewPos, this->_SSO.StrPos, this->_SSO.StrLen * sizeof(wchar_t));
					for (UBINT i = this->_SSO.StrLen; i < NewSize; i++)NewPos[i] = Char;
					NewPos[NewSize] = L'\0';
				}
				else{
					memcpy(NewPos, this->_SSO.StrPos, NewSize * sizeof(wchar_t));
					NewPos[NewSize] = L'\0';
				}
				this->_SSO.MemLen = NewSize + 1;
				this->_SSO.StrLen = NewSize;
				nsBasic::GlobalMemFree(this->_SSO.StrPos, CurMemLen * sizeof(wchar_t));
				this->_SSO.StrPos = NewPos;
			}
		}
	}
	void String_W::swap(String_W &rhs){
		String_W::_SSOStruct TmpSSO = this->_SSO;
		this->_SSO = rhs._SSO;
		rhs._SSO = TmpSSO;
	}

	//all the following find implementations are wrong.
	//String_W::size_type String_W::find(value_type Char, size_type InitPos) const{
	//	const_pointer InitPtr, RetPtr;
	//	if (this->_SSO.Buffer[_SSOTail] & 0x8000){
	//		if (InitPos >= (this->_SSO.Buffer[_SSOTail] & 0x7FFF))return npos;
	//		else InitPtr = this->_SSO.Buffer;
	//	}
	//	else{
	//		if (InitPos >= this->_SSO.StrLen)return npos;
	//		else InitPtr = this->_SSO.StrPos;
	//	}
	//	RetPtr = (const_pointer)wcschr((wchar_t *)(&InitPtr[InitPos]), Char);
	//	if (nullptr == RetPtr)return npos; else return RetPtr - InitPtr;
	//}
	//String_W::size_type String_W::find(const wchar_t *SubStr, size_type Count, size_type InitPos) const{
	//	size_type CurSize=this->size();
	//	const_pointer InitPtr = this->cbegin();
	//	if (InitPos >= CurSize)return npos;
	//	else return nsAlg::Search_Mem_Mem_Fwd(&InitPtr[InitPos], CurSize, SubStr, Count);
	//}
	//String_W::size_type String_W::find(const wchar_t *SubStr, size_type InitPos) const{
	//	size_type CurSize = this->size();
	//	const_pointer InitPtr = this->cbegin();
	//	if (InitPos >= CurSize)return npos;
	//	else try{ return nsAlg::Search_Mem_Str_Fwd(&InitPtr[InitPos], CurSize, SubStr); }
	//	catch (...){ return npos; } //no exception guarantee
	//}
	//String_W::size_type String_W::find(const String_W& SubStr, size_type InitPos) const{
	//	size_type CurSize = this->size();
	//	const_pointer InitPtr = this->cbegin();
	//	if (InitPos >= CurSize)return npos;
	//	else return nsAlg::Search_Mem_Mem_Fwd(&InitPtr[InitPos], CurSize, SubStr.cbegin(), SubStr.size());
	//}
	//String_W::size_type String_W::rfind(value_type Char, size_type InitPos) const{
	//	const_pointer InitPtr, RetPtr;
	//	if (this->_SSO.Buffer[_SSOTail] & 0x8000){
	//		if (InitPos >= (this->_SSO.Buffer[_SSOTail] & 0x7FFF))InitPos = this->_SSO.Buffer[_SSOTail] & 0x7FFF;
	//		else InitPos++;
	//		InitPtr = this->_SSO.Buffer;
	//	}
	//	else{
	//		if (InitPos >= this->_SSO.StrLen)InitPos = this->_SSO.StrLen;
	//		else InitPos++;
	//		InitPtr = this->_SSO.StrPos;
	//	}
	//	RetPtr = (const_pointer)nsAlg::Search_Mem_Chr_Bwd(InitPtr, Char, InitPos);
	//	if (nullptr == RetPtr)return npos; else return RetPtr - InitPtr;
	//}
	//String_W::size_type String_W::rfind(const wchar_t *SubStr, size_type Count, size_type InitPos) const{
	//	size_type CurSize = this->size();
	//	if (CurSize < Count)return npos;
	//	if (InitPos>CurSize - Count)InitPos = CurSize; else InitPos += Count;
	//	return nsAlg::Search_Mem_Mem_Bwd(this->cbegin(), InitPos, SubStr, Count);
	//}
	//String_W::size_type String_W::rfind(const wchar_t *SubStr, size_type InitPos) const{
	//	size_type rhsLen = wcslen(SubStr);
	//	return this->rfind(SubStr, rhsLen, InitPos);
	//}
	//String_W::size_type String_W::rfind(const String_W& SubStr, size_type InitPos) const{
	//	size_type CurSize = this->size();
	//	size_type Count = SubStr.size();
	//	if (CurSize < Count)return npos;
	//	if (InitPos>CurSize - Count)InitPos = CurSize; else InitPos += Count;
	//	return nsAlg::Search_Mem_Mem_Bwd(this->cbegin(), InitPos, SubStr.cbegin(), Count);
	//}

	String_W String_W::substr(size_type Index, size_type Count){
		size_type CurSize = this->size();
		if (Index > CurSize)throw std::out_of_range("Invalid string index.");
		if (CurSize - Index < Count)Count = CurSize - Index;

		String_W RetValue(Count);
		memcpy(RetValue.begin(), this->cbegin() + Index, Count * sizeof(wchar_t));
		return RetValue;
	}
	int String_W::compare(const String_W& rhs) const{
		if (this->size() < rhs.size())return -1;
		else if (this->size() > rhs.size())return 1;
		else return memcmp(this->cbegin(), rhs.cbegin(), this->size());
	}
	inline int String_W::compare(const wchar_t* rhs) const{
		return memcmp(this->cbegin(), rhs, this->size() + 1);
	}

	String_W& String_W::operator = (const String_W& rhs){
		String_W temp(rhs);
		this->swap(temp);
		return *this;
	}
	String_W& String_W::operator = (String_W&& rhs){
		this->_SSO = rhs._SSO;
		//get rhs ready for destruct
		rhs._SSO.Buffer[_SSOTail] = 0x8000;
		return *this;
	}
	
	String_W& String_W::operator = (const wchar_t *rhs){
		String_W temp(rhs);
		this->swap(temp);
		return *this;
	}
	String_W& String_W::operator+=(value_type rhs){
		return this->append(rhs);
	}
	String_W& String_W::operator+=(const String_W& rhs){
		return this->append(rhs);
	}
	String_W& String_W::operator+=(const wchar_t *rhs){
		return this->append(rhs);
	}

	String_W::~String_W(){
		if (0 == (0x8000 & this->_SSO.Buffer[_SSOTail]))nsBasic::GlobalMemFree(this->_SSO.StrPos, this->_SSO.MemLen * sizeof(wchar_t));
	}

	inline String_W operator+(const String_W& lhs, const String_W& rhs){
		String_W::size_type NewLen = lhs.size() + rhs.size();
		if (NewLen > lhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String_W RetValue(NewLen);
			memcpy(RetValue.begin(), lhs.cbegin(), lhs.size() * sizeof(wchar_t));
			memcpy(RetValue.begin() + lhs.size(), rhs.cbegin(), rhs.size() * sizeof(wchar_t));
			return RetValue;
		}
	}
	inline String_W operator+(const String_W& lhs, const wchar_t *rhs){
		String_W::size_type NewLen = lhs.size() + wcsnlen(rhs, lhs.max_size());
		if (NewLen > lhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String_W RetValue(NewLen);
			memcpy(RetValue.begin(), lhs.cbegin(), lhs.size() * sizeof(wchar_t));
			memcpy(RetValue.begin() + lhs.size(), rhs, (NewLen - lhs.size()) * sizeof(wchar_t));
			return RetValue;
		}
	}
	inline String_W operator+(const String_W& lhs, const String_W::value_type rhs){
		String_W::size_type NewLen = lhs.size() + 1;
		if (NewLen > lhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String_W RetValue(NewLen);
			memcpy(RetValue.begin(), lhs.cbegin(), lhs.size() * sizeof(wchar_t));
			RetValue.begin()[lhs.size()] = rhs;
			return RetValue;
		}
	}
	inline String_W operator+(const wchar_t *lhs, const String_W& rhs){
		String_W::size_type ChrLen = wcsnlen(lhs, rhs.max_size());
		String_W::size_type NewLen = rhs.size() + ChrLen;
		if (NewLen > rhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String_W RetValue(NewLen);
			memcpy(RetValue.begin(), lhs, ChrLen * sizeof(wchar_t));
			memcpy(RetValue.begin() + ChrLen, rhs.cbegin(), rhs.size() * sizeof(wchar_t));
			return RetValue;
		}
	}
	inline String_W operator+(const String_W::value_type lhs, const String_W& rhs){
		String_W::size_type NewLen = rhs.size() + 1;
		if (NewLen > rhs.max_size())throw std::length_error("String length exceeds its max size.");
		else{
			String_W RetValue(NewLen);
			RetValue.begin()[0] = lhs;
			memcpy(&RetValue.begin()[1], rhs.cbegin(), rhs.size() * sizeof(wchar_t));
			return RetValue;
		}
	}
}

template<> struct std::hash<nsText::String_W>{
public:
	size_t operator()(const nsText::String_W &rhs) const{
		//BKDRHash from NOCOW
		size_t seed = 131, hash = 0;
		const wchar_t *CurPtr = rhs.cbegin();
		for (UBINT i = 0; i < rhs.size(); i++)hash = hash * seed + (*CurPtr++);
		return hash;
	}
};

#endif