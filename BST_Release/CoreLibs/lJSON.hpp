/* Description: Functions for reading, writing and manipulating JSON data.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_JSON
#define LIB_JSON

#include "lContainer.hpp"
#include "lFile.hpp"
#include "lText_Process.hpp"

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check the following macros.
//These macros provide platform-related headers and static libraries.
#endif

namespace nsJSON{
	class JSONNode_Null;
	class JSONNode_Bool;
	class JSONNode_Int;
	class JSONNode_Float;
	class JSONNode_String;
	class JSONNode_Array;
	class JSONNode_Object;

	class JSONNode{
	protected:
		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };
		JSONNode() = default;
		~JSONNode() = default;
	public:
		enum JSONType{
			JSON_NULL,
			JSON_BOOL,
			JSON_INT,
			JSON_FLOAT,
			JSON_STRING,
			JSON_STRING_W,
			JSON_ARRAY,
			JSON_OBJECT
		};
		JSONType Type;

		inline bool IsNull() const{ return this->Type == JSONType::JSON_NULL; }
		inline bool IsBool() const{ return this->Type == JSONType::JSON_BOOL; }
		inline bool IsInt() const{ return this->Type == JSONType::JSON_INT; }
		inline bool IsFloat() const{ return this->Type == JSONType::JSON_FLOAT; }
		inline bool IsString() const{ return this->Type == JSONType::JSON_STRING; }
		inline bool IsString_W() const{ return this->Type == JSONType::JSON_STRING_W; }
#if defined LIBENV_OS_WIN
		inline bool IsString_Sys() const{ return this->IsString_W(); }
#else if defined LIBENV_OS_LINUX
		inline bool IsString_Sys() const{ return this->IsString(); }
#endif
		inline bool IsArray() const{ return this->Type == JSONType::JSON_ARRAY; }
		inline bool IsObject() const{ return this->Type == JSONType::JSON_OBJECT; }

		INT8b GetInt() const;
		double GetFloat() const;
		bool GetBool() const;
		const nsText::String& GetString() const;
		const nsText::String_W& GetString_W() const;
#if defined LIBENV_OS_WIN
		inline const nsText::String_W& GetString_Sys() const{ return this->GetString_W(); }
#else if defined LIBENV_OS_LINUX
		inline const nsText::String& GetString_Sys() const{ return this->GetString(); }
#endif

		void SetInt(INT8b Value);
		void SetFloat(double Value);
		void SetBool(bool Value);

		nsContainer::Vector<JSONNode *> *GetArray();
		const nsContainer::Vector<JSONNode *> *GetArray() const;
		nsContainer::HashMap<nsText::String, JSONNode *> *GetObject();
		const nsContainer::HashMap<nsText::String, JSONNode *> *GetObject() const;

		UBINT Size() const;

		inline JSONNode *operator[](const nsText::String& String);
		inline const JSONNode *operator[](const nsText::String& String) const;
		inline JSONNode *operator[](const char *lpString){ return this->operator[](nsText::String(lpString)); }
		inline const JSONNode *operator[](const char *lpString) const{ return this->operator[](nsText::String(lpString)); }
		JSONNode *operator[](UBINT Index);
		const JSONNode *operator[](UBINT Index) const;

		void Destroy(); //serves as operator delete
	};

	class JSONNode_Null :public JSONNode{
	public:
		inline void *operator new(size_t size){ return JSONNode::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		inline JSONNode_Null() : JSONNode(){ this->Type = JSONType::JSON_NULL; }
	};
	class JSONNode_Bool :public JSONNode{
	public:
		inline void *operator new(size_t size){ return JSONNode::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		bool Content;
		inline JSONNode_Bool() : JSONNode(){ this->Type = JSONType::JSON_BOOL; }
	};
	class JSONNode_Int :public JSONNode{
	public:
		inline void *operator new(size_t size){ return JSONNode::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		INT8b Content;
		inline JSONNode_Int() : JSONNode(){ this->Type = JSONType::JSON_INT; }
	};
	class JSONNode_Float :public JSONNode{
	public:
		inline void *operator new(size_t size){ return JSONNode::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		double Content;
		inline JSONNode_Float() : JSONNode(){ this->Type = JSONType::JSON_FLOAT; }
	};
	class JSONNode_String :public JSONNode{
	public:
		inline void *operator new(size_t size){ return JSONNode::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		nsText::String Content;
		inline JSONNode_String() : JSONNode(){ this->Type = JSONType::JSON_STRING; }
		~JSONNode_String(){ return; }
	};
	class JSONNode_String_W :public JSONNode{
	public:
		inline void *operator new(size_t size){ return JSONNode::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		nsText::String_W Content;
		inline JSONNode_String_W() : JSONNode(){ this->Type = JSONType::JSON_STRING_W; }
		~JSONNode_String_W(){ return; }
	};
	class JSONNode_Array :public JSONNode{
	public:
		inline void *operator new(size_t size){ return JSONNode::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		nsContainer::Vector<JSONNode *> Content;
		inline JSONNode_Array() : JSONNode(){ this->Type = JSONType::JSON_ARRAY; }
		~JSONNode_Array();
	};
	class JSONNode_Object :public JSONNode{
	public:
		inline void *operator new(size_t size){ return JSONNode::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		nsContainer::HashMap<nsText::String, JSONNode *> Content;
		inline JSONNode_Object() : JSONNode(){ this->Type = JSONType::JSON_OBJECT; }
		~JSONNode_Object();
	};

	class JSONReader{
	private:
		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		struct ReaderStat{
			JSONNode *lpNode;
			UBINT Status;

			static const UBINT IsFirst = 1 << 0;
			static const UBINT IsObject = 1 << 1;
		};

		nsText::Lexer *lpLexer;
		nsContainer::Vector<ReaderStat> StatStack;
		bool GenString_W;

		bool Parse_String(nsText::String *lpString);
		bool Parse_String(nsText::String_W *lpString);
		bool Skip_String();

		JSONNode *Parse_Number();
		bool Skip_Number();
		JSONNode *Parse_KeyWord();
		bool Skip_KeyWord();
	public:
		JSONReader() :lpLexer(){};
		void Generate_String_Sys();
		void Generate_String();
		void Generate_String_W();
		void BeginRead(nsText::Lexer *lpLexer);
		std::pair<const nsText::String *, JSONNode *> ParseNode();
		bool ParseAllChild();
		//returns false when an exception is triggered inside the reader.
		//returns true when no child exists.
		bool SkipAllChild();
		//returns false when an exception is triggered inside the reader.
		//returns true when no child exists.
		void EndRead();
	};

	class JSONWriter{
	private:
		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		static const UBINT IsFirst = 1 << 0;
		static const UBINT IsObject = 1 << 1;

		nsBasic::Stream_W<UINT4b> *lpStream;
		nsContainer::Vector<UBINT> StatStack;

		template <typename T> bool WriteString_Internal(const T *lpString);
		bool Advance_Array();
		bool Advance_Object();
		
	public:
		JSONWriter() :lpStream(){};
		
		void BeginWrite(nsBasic::Stream_W<UINT4b> *lpStream);

		bool BeginArray();
		bool BeginObject();
		bool WriteInt(INT8b Value);
		bool WriteFloat(double Value);
		bool WriteBool(bool Value);
		bool WriteNull();
		template <typename T> bool WriteString(const T *lpString);
		bool WriteNode(const JSONNode *lpNode);

		template <typename T> bool BeginArray_Indexed(const T *Index);
		template <typename T> bool BeginObject_Indexed(const T *Index);
		template <typename T> bool WriteInt_Indexed(const T *Index, INT8b Value);
		template <typename T> bool WriteFloat_Indexed(const T *Index, double Value);
		template <typename T> bool WriteBool_Indexed(const T *Index, bool Value);
		template <typename T> bool WriteNull_Indexed(const T *Index);
		template <typename T1, typename T2> bool WriteString_Indexed(const T1 *Index, const T2 *lpString);
		template <typename T> bool WriteNode_Indexed(const T *Index, const JSONNode *lpNode);

		bool EndArray();
		bool EndObject();

		void EndWrite();
		inline ~JSONWriter(){ this->EndWrite(); }
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	inline INT8b JSONNode::GetInt() const{
		if (JSONType::JSON_INT == this->Type)return static_cast<const JSONNode_Int *>(this)->Content;
		else if (JSONType::JSON_FLOAT == this->Type)return (INT8b)static_cast<const JSONNode_Float *>(this)->Content;
		else if (JSONType::JSON_BOOL == this->Type)return static_cast<const JSONNode_Bool *>(this)->Content ? 1 : 0;
		else return 0;
	}
	inline double JSONNode::GetFloat() const{
		if (JSONType::JSON_FLOAT == this->Type)return static_cast<const JSONNode_Float *>(this)->Content;
		else if (JSONType::JSON_INT == this->Type)return (double)static_cast<const JSONNode_Int *>(this)->Content;
		else if (JSONType::JSON_BOOL == this->Type)return static_cast<const JSONNode_Bool *>(this)->Content ? 1.0 : 0.0;
		else return 0;
	}
	inline bool JSONNode::GetBool() const{
		if (JSONType::JSON_BOOL == this->Type)return static_cast<const JSONNode_Bool *>(this)->Content;
		else if (JSONType::JSON_FLOAT == this->Type)return static_cast<const JSONNode_Float *>(this)->Content != 0.0;
		else if (JSONType::JSON_INT == this->Type)return static_cast<const JSONNode_Int *>(this)->Content != 0;
		else return false;
	}
	inline const nsText::String& JSONNode::GetString() const{
		if (JSONType::JSON_STRING == this->Type)return static_cast<const JSONNode_String *>(this)->Content;
		else throw std::exception("Incorrect JSON node type.");
	}
	inline const nsText::String_W& JSONNode::GetString_W() const{
		if (JSONType::JSON_STRING_W == this->Type)return static_cast<const JSONNode_String_W *>(this)->Content;
		else throw std::exception("Incorrect JSON node type.");
	}
	inline void JSONNode::SetInt(INT8b Value){
		if (JSONType::JSON_INT == this->Type)static_cast<JSONNode_Int *>(this)->Content = Value;
		else if (JSONType::JSON_FLOAT == this->Type)static_cast<JSONNode_Float *>(this)->Content = (float)Value;
		else if (JSONType::JSON_BOOL == this->Type)static_cast<JSONNode_Bool *>(this)->Content = (Value > 0) ? true : false;
	}
	inline void JSONNode::SetFloat(double Value){
		if (isinf(Value) || isnan(Value))Value = 0.0;
		if (JSONType::JSON_FLOAT == this->Type)static_cast<JSONNode_Float *>(this)->Content = Value;
		else if (JSONType::JSON_INT == this->Type)static_cast<JSONNode_Int *>(this)->Content = (INT8b)Value;
		else if (JSONType::JSON_BOOL == this->Type)static_cast<JSONNode_Bool *>(this)->Content = (Value > 0) ? true : false;
	}
	inline void JSONNode::SetBool(bool Value){
		if (JSONType::JSON_BOOL == this->Type)static_cast<JSONNode_Bool *>(this)->Content = Value;
		else if (JSONType::JSON_FLOAT == this->Type)static_cast<JSONNode_Float *>(this)->Content = (float)Value;
		else if (JSONType::JSON_INT == this->Type)static_cast<JSONNode_Int *>(this)->Content = (INT8b)Value;
	}
	inline nsContainer::Vector<JSONNode *> *JSONNode::GetArray(){
		if (JSONType::JSON_ARRAY == this->Type)return &static_cast<JSONNode_Array *>(this)->Content;
		else return nullptr;
	}
	inline const nsContainer::Vector<JSONNode *> *JSONNode::GetArray() const{
		if (JSONType::JSON_ARRAY == this->Type)return &static_cast<const JSONNode_Array *>(this)->Content;
		else return nullptr;
	}
	inline nsContainer::HashMap<nsText::String, JSONNode *> *JSONNode::GetObject(){
		if (JSONType::JSON_OBJECT == this->Type)return &static_cast<JSONNode_Object *>(this)->Content;
		else return nullptr;
	}
	inline const nsContainer::HashMap<nsText::String, JSONNode *> *JSONNode::GetObject() const{
		if (JSONType::JSON_OBJECT == this->Type)return &static_cast<const JSONNode_Object *>(this)->Content;
		else return nullptr;
	}
	inline UBINT JSONNode::Size() const{
		if (JSONType::JSON_ARRAY == this->Type)return static_cast<const JSONNode_Array *>(this)->Content.size();
		if (JSONType::JSON_OBJECT == this->Type)return static_cast<const JSONNode_Object *>(this)->Content.size();
		else return 0;
	}
	JSONNode *JSONNode::operator[](const nsText::String& String){
		if (JSONType::JSON_OBJECT != this->Type)return nullptr;
		else{
			auto _It = (static_cast<JSONNode_Object *>(this))->Content.find(String);
			if (_It == (static_cast<JSONNode_Object *>(this))->Content.end())return nullptr;
			else return _It->second;
		}
	}
	const JSONNode *JSONNode::operator[](const nsText::String& String) const{
		if (JSONType::JSON_OBJECT != this->Type)return nullptr;
		else{
			auto _It = (static_cast<const JSONNode_Object *>(this))->Content.find(String);
			if (_It == (static_cast<const JSONNode_Object *>(this))->Content.cend())return nullptr;
			else return _It->second;
		}
	}
	JSONNode *JSONNode::operator[](UBINT Index){
		if (JSONType::JSON_ARRAY != this->Type)return nullptr;
		else{
			if (static_cast<JSONNode_Array *>(this)->Content.size() > Index)return static_cast<JSONNode_Array *>(this)->Content[Index];
			else return nullptr;
		}
	}
	const JSONNode *JSONNode::operator[](UBINT Index) const{
		if (JSONType::JSON_ARRAY != this->Type)return nullptr;
		else{
			if (static_cast<const JSONNode_Array *>(this)->Content.size() > Index)return static_cast<const JSONNode_Array *>(this)->Content[Index];
			else return nullptr;
		}
	}
	void JSONNode::Destroy(){
		switch (this->Type){
		case JSONType::JSON_OBJECT:
			delete static_cast<JSONNode_Object *>(this);
			break;
		case JSONType::JSON_ARRAY:
			delete static_cast<JSONNode_Array *>(this);
			break;
		case JSONType::JSON_STRING_W:
			delete static_cast<JSONNode_String_W *>(this);
			break;
		case JSONType::JSON_STRING:
			delete static_cast<JSONNode_String *>(this);
			break;
		case JSONType::JSON_FLOAT:
			delete static_cast<JSONNode_Float *>(this);
			break;
		case JSONType::JSON_INT:
			delete static_cast<JSONNode_Int *>(this);
			break;
		case JSONType::JSON_BOOL:
			delete static_cast<JSONNode_Bool *>(this);
			break;
		case JSONType::JSON_NULL:
			delete static_cast<JSONNode_Null *>(this);
			break;
		}
	}
	JSONNode_Array::~JSONNode_Array(){
		for (UBINT i = 0; i < this->Content.size(); i++)this->Content[i]->Destroy();
	}
	JSONNode_Object::~JSONNode_Object(){
		for (auto _it = this->Content.begin(); _it != this->Content.end(); _it++)_it->second->Destroy();
	}
	bool JSONReader::Parse_String(nsText::String *lpString){
		this->lpLexer->LoadNextChar();
		do{
			if ((UINT4b)'\"' == this->lpLexer->LastChar){
				this->lpLexer->LoadNextChar();
				return true;
			}
			else if ((UINT4b)-1 == this->lpLexer->LastChar){
				this->EndRead();
				return false;
			}
			else if ((UINT4b)'\\' == this->lpLexer->LastChar){
				this->lpLexer->LoadNextChar();
				if ((UINT4b)'\"' == this->lpLexer->LastChar ||
					(UINT4b)'\\' == this->lpLexer->LastChar ||
					(UINT4b)'/' == this->lpLexer->LastChar)lpString->append(this->lpLexer->LastChar);
				else if ((UINT4b)'b' == this->lpLexer->LastChar)lpString->append((UINT4b)'\b');
				else if ((UINT4b)'f' == this->lpLexer->LastChar)lpString->append((UINT4b)'\f');
				else if ((UINT4b)'n' == this->lpLexer->LastChar)lpString->append((UINT4b)'\n');
				else if ((UINT4b)'r' == this->lpLexer->LastChar)lpString->append((UINT4b)'\r');
				else if ((UINT4b)'t' == this->lpLexer->LastChar)lpString->append((UINT4b)'\t');
				else if ((UINT4b)'u' == this->lpLexer->LastChar){
					//parse the unicode
					UINT4b CodePoint = 0;
					for (UBINT i = 0; i < 4; i++){
						this->lpLexer->LoadNextChar();
						if (this->lpLexer->LastChar >= (UBINT)'0' && this->lpLexer->LastChar <= (UBINT)'9')CodePoint += this->lpLexer->LastChar - (UBINT)'0';
						else if (this->lpLexer->LastChar >= (UBINT)'a' && this->lpLexer->LastChar <= (UBINT)'f')CodePoint += this->lpLexer->LastChar - (UBINT)'a' + 10;
						else if (this->lpLexer->LastChar >= (UBINT)'A' && this->lpLexer->LastChar <= (UBINT)'F')CodePoint += this->lpLexer->LastChar - (UBINT)'A' + 10;
						else{
							this->EndRead();
							return false;
						}
					}
					lpString->append(CodePoint);
				}
				else{
					this->EndRead();
					return false;
				}
			}
			else lpString->append(this->lpLexer->LastChar);
			this->lpLexer->LoadNextChar();
		} while (true);
	}
	bool JSONReader::Parse_String(nsText::String_W *lpString){
		this->lpLexer->LoadNextChar();
		do{
			if ((UINT4b)'\"' == this->lpLexer->LastChar){
				this->lpLexer->LoadNextChar();
				return true;
			}
			else if ((UINT4b)-1 == this->lpLexer->LastChar){
				this->EndRead();
				return false;
			}
			else if ((UINT4b)'\\' == this->lpLexer->LastChar){
				this->lpLexer->LoadNextChar();
				if ((UINT4b)'\"' == this->lpLexer->LastChar ||
					(UINT4b)'\\' == this->lpLexer->LastChar ||
					(UINT4b)'/' == this->lpLexer->LastChar)lpString->append(this->lpLexer->LastChar);
				else if ((UINT4b)'b' == this->lpLexer->LastChar)lpString->append((UINT4b)'\b');
				else if ((UINT4b)'f' == this->lpLexer->LastChar)lpString->append((UINT4b)'\f');
				else if ((UINT4b)'n' == this->lpLexer->LastChar)lpString->append((UINT4b)'\n');
				else if ((UINT4b)'r' == this->lpLexer->LastChar)lpString->append((UINT4b)'\r');
				else if ((UINT4b)'t' == this->lpLexer->LastChar)lpString->append((UINT4b)'\t');
				else if ((UINT4b)'u' == this->lpLexer->LastChar){
					//parse the unicode
					UINT4b CodePoint = 0;
					for (UBINT i = 0; i < 4; i++){
						this->lpLexer->LoadNextChar();
						if (this->lpLexer->LastChar >= (UBINT)'0' && this->lpLexer->LastChar <= (UBINT)'9')CodePoint += this->lpLexer->LastChar - (UBINT)'0';
						else if (this->lpLexer->LastChar >= (UBINT)'a' && this->lpLexer->LastChar <= (UBINT)'f')CodePoint += this->lpLexer->LastChar - (UBINT)'a' + 10;
						else if (this->lpLexer->LastChar >= (UBINT)'A' && this->lpLexer->LastChar <= (UBINT)'F')CodePoint += this->lpLexer->LastChar - (UBINT)'A' + 10;
						else{
							this->EndRead();
							return false;
						}
					}
					lpString->append(CodePoint);
				}
				else{
					this->EndRead();
					return false;
				}
			}
			else lpString->append(this->lpLexer->LastChar);
			this->lpLexer->LoadNextChar();
		} while (true);
	}
	bool JSONReader::Skip_String(){
		this->lpLexer->LoadNextChar();
		do{
			if ((UINT4b)'\"' == this->lpLexer->LastChar){
				this->lpLexer->LoadNextChar();
				return true;
			}
			else if ((UINT4b)-1 == this->lpLexer->LastChar){
				this->EndRead();
				return false;
			}
			else if ((UINT4b)'\\' == this->lpLexer->LastChar){
				this->lpLexer->LoadNextChar();
				if ((UINT4b)'\"' != this->lpLexer->LastChar &&
					(UINT4b)'\\' != this->lpLexer->LastChar &&
					(UINT4b)'/' != this->lpLexer->LastChar &&
					(UINT4b)'b' != this->lpLexer->LastChar &&
					(UINT4b)'f' != this->lpLexer->LastChar &&
					(UINT4b)'n' != this->lpLexer->LastChar &&
					(UINT4b)'r' != this->lpLexer->LastChar &&
					(UINT4b)'t' != this->lpLexer->LastChar){
					if ((UINT4b)'u' == this->lpLexer->LastChar){
						//parse the unicode
						for (UBINT i = 0; i < 4; i++){
							this->lpLexer->LoadNextChar();
							if ((this->lpLexer->LastChar < (UBINT)'0' || this->lpLexer->LastChar >(UBINT)'9') &&
								(this->lpLexer->LastChar < (UBINT)'a' || this->lpLexer->LastChar >(UBINT)'f') &&
								(this->lpLexer->LastChar < (UBINT)'A' || this->lpLexer->LastChar >(UBINT)'F')){
								this->EndRead();
								return false;
							}
						}
					}
					else{
						this->EndRead();
						return false;
					}
				}
			}
			this->lpLexer->LoadNextChar();
		} while (true);
	}

	JSONNode *JSONReader::Parse_Number(){
		bool IsNegative, IsFloat = false;

		UINT8b InputValue = 0;
		UINT4b LastChar = 0;
		INT4b Exponent = 0, DigitRemain = 19;

		if ((UINT4b)'-' == this->lpLexer->LastChar){
			IsNegative = true;
			this->lpLexer->LoadNextChar();
		}
		else IsNegative = false;

		if ((UINT4b)'0' == this->lpLexer->LastChar)this->lpLexer->LoadNextChar();
		else if ((UINT4b)'1' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar){
			while (true){
				Exponent++;
				if ((UINT4b)'1' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar){
					if (DigitRemain > Exponent){
						for (UBINT i = 0; i < Exponent; i++)InputValue *= 10;
						InputValue += (UINT8b)(this->lpLexer->LastChar - (UINT4b)'0');
						DigitRemain -= Exponent;
						Exponent = 0;
					}
					else LastChar = this->lpLexer->LastChar - (UINT4b)'0';
				}
				else if ((UINT4b)'0' != this->lpLexer->LastChar)break;
				this->lpLexer->LoadNextChar();
			}
			Exponent--;
		}
		else{
			//failed
			this->EndRead();
			return nullptr;
		}

		if ((UBINT)'.' == this->lpLexer->LastChar){
			IsFloat = true; //must be a float
			this->lpLexer->LoadNextChar();

			if ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar){
				INT4b MantissaDigitCount = Exponent;

				while (true){
					MantissaDigitCount++;
					if ((UINT4b)'1' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar){
						if (DigitRemain > MantissaDigitCount){
							for (UBINT i = 0; i < MantissaDigitCount; i++)InputValue *= 10;
							InputValue += (UINT8b)(this->lpLexer->LastChar - (UINT4b)'0');
							Exponent -= MantissaDigitCount;
							DigitRemain -= MantissaDigitCount;
							MantissaDigitCount = 0;
						}
					}
					else if ((UINT4b)'0' != this->lpLexer->LastChar)break;
					this->lpLexer->LoadNextChar();
				}
			}
			else{
				//failed
				this->EndRead();
				return nullptr;
			}
		}

		if ((UINT4b)'E' == this->lpLexer->LastChar || (UINT4b)'e' == this->lpLexer->LastChar){
			IsFloat = true; //must be a float
			INT4b Exponent_Tmp = 0;
			bool Exponent_Tmp_IsNegative;

			this->lpLexer->LoadNextChar();
			if ((UINT4b)'-' == this->lpLexer->LastChar){
				Exponent_Tmp_IsNegative = true;
				this->lpLexer->LoadNextChar();
			}
			else{
				if ((UINT4b)'+' == this->lpLexer->LastChar)this->lpLexer->LoadNextChar();
				Exponent_Tmp_IsNegative = false;
			}

			if ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar){
				do{
					Exponent_Tmp = Exponent_Tmp * 10 + (INT4b)(this->lpLexer->LastChar - (UINT4b)'0');
					this->lpLexer->LoadNextChar();
				} while ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar);
				if (Exponent_Tmp_IsNegative)Exponent_Tmp = -Exponent_Tmp;
				Exponent += Exponent_Tmp;
			}
			else return false;
		}

		if (IsFloat){
			JSONNode_Float *RetValue = new JSONNode_Float;

			if (0 == InputValue || Exponent < -343)RetValue->Content = 0.0;
			else if (Exponent > 308)RetValue->Content = nsMath::NumericTrait<double>::Inf_Positive;
			else{
				nsText::__convert_float_base10_2(InputValue, Exponent, &RetValue->Content);
				if (Exponent < -325){
					UINT8b RetValue_Bits = *reinterpret_cast<UINT8b *>(&RetValue->Content);
					RetValue_Bits &= nsMath::NumericTrait<double>::HighestBit + nsMath::NumericTrait<double>::Exponent_Mask;
					if (RetValue_Bits >= nsMath::NumericTrait<double>::Exponent_Mask)RetValue->Content = 0.0;
				}
				else if (Exponent > 290){
					UINT8b RetValue_Bits = *reinterpret_cast<UINT8b *>(&RetValue->Content);
					RetValue_Bits &= nsMath::NumericTrait<double>::HighestBit + nsMath::NumericTrait<double>::Exponent_Mask;
					if (RetValue_Bits >= nsMath::NumericTrait<double>::Exponent_Mask)RetValue->Content = nsMath::NumericTrait<double>::Inf_Positive;
				}
			}

			if (IsNegative)RetValue->Content = -RetValue->Content;
			return RetValue;
		}
		else{
			//integer
			JSONNode_Int *RetValue = new JSONNode_Int;

			while (Exponent > 0 && InputValue < 1000000000000000000){
				InputValue *= 10;
				Exponent--;
			}
			if (InputValue > 1844674407370955161 || Exponent > 1)InputValue = nsMath::NumericTrait<UINT8b>::Max;
			else if (LastChar > 5)InputValue = nsMath::NumericTrait<UINT8b>::Max;
			else if (Exponent > 0)InputValue = InputValue * 10 + LastChar;

			if (IsNegative){
				if (InputValue > nsMath::NumericTrait<UINT8b>::HighestBit)InputValue = nsMath::NumericTrait<UINT8b>::HighestBit;
				RetValue->Content = -(INT8b)InputValue;
			}
			else{
				if (InputValue >= nsMath::NumericTrait<UINT8b>::HighestBit)InputValue = nsMath::NumericTrait<UINT8b>::HighestBit - 1;
				RetValue->Content = (INT8b)InputValue;
			}

			return RetValue;
		}
	}
	bool JSONReader::Skip_Number(){
		if ((UINT4b)'-' == this->lpLexer->LastChar)this->lpLexer->LoadNextChar();
		if ((UINT4b)'0' == this->lpLexer->LastChar)this->lpLexer->LoadNextChar();
		else if ((UINT4b)'1' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar){
			do{ this->lpLexer->LoadNextChar(); } while ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar);
		}
		else{
			//failed
			this->EndRead();
			return false;
		}

		if ((UBINT)'.' == this->lpLexer->LastChar){
			//must be a float
			this->lpLexer->LoadNextChar();
			if ((UINT4b)'0' > this->lpLexer->LastChar || (UINT4b)'9' < this->lpLexer->LastChar){
				//failed
				this->EndRead();
				return false;
			}
			do{ this->lpLexer->LoadNextChar(); } while ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar);

			if ((UBINT)'E' == this->lpLexer->LastChar || (UBINT)'e' == this->lpLexer->LastChar){
				this->lpLexer->LoadNextChar();
				if ((UINT4b)'-' == this->lpLexer->LastChar)this->lpLexer->LoadNextChar();
				else if ((UINT4b)'+' == this->lpLexer->LastChar)this->lpLexer->LoadNextChar();
				else if ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar){
					do{ this->lpLexer->LoadNextChar(); } while ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar);
				}
				else{
					//failed
					this->EndRead();
					return false;
				}
			}
			return true;
		}
		else if ((UBINT)'E' == this->lpLexer->LastChar || (UBINT)'e' == this->lpLexer->LastChar){
			//must be a float
			this->lpLexer->LoadNextChar();

			if ((UINT4b)'-' == this->lpLexer->LastChar)this->lpLexer->LoadNextChar();
			else if ((UINT4b)'+' == this->lpLexer->LastChar)this->lpLexer->LoadNextChar();
			if ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar){
				do{ this->lpLexer->LoadNextChar(); } while ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar);
			}
			else{
				//failed
				this->EndRead();
				return false;
			}
			return true;
		}
		else return true;
	}
	JSONNode *JSONReader::Parse_KeyWord(){
		if ((UBINT)'t' == this->lpLexer->LastChar){
			UINT4b KeyWord_true[3] = { (UINT4b)'r', (UINT4b)'u', (UINT4b)'e' };
			for (UBINT i = 0; i < 3; i++){
				this->lpLexer->LoadNextChar();
				if (KeyWord_true[i] != this->lpLexer->LastChar){
					//failed
					this->EndRead();
					return nullptr;
				}
			}
			this->lpLexer->LoadNextChar();
			JSONNode_Bool *RetValue = new JSONNode_Bool;
			RetValue->Content = true;
			return RetValue;
		}
		else if ((UBINT)'f' == this->lpLexer->LastChar){
			UINT4b KeyWord_false[4] = { (UINT4b)'a', (UINT4b)'l', (UINT4b)'s', (UINT4b)'e' };
			for (UBINT i = 0; i < 4; i++){
				this->lpLexer->LoadNextChar();
				if (KeyWord_false[i] != this->lpLexer->LastChar){
					//failed
					this->EndRead();
					return nullptr;
				}
			}
			this->lpLexer->LoadNextChar();
			JSONNode_Bool *RetValue = new JSONNode_Bool;
			RetValue->Content = false;
			return RetValue;
		}
		else if ((UBINT)'n' == this->lpLexer->LastChar){
			UINT4b KeyWord_null[3] = { (UINT4b)'u', (UINT4b)'l', (UINT4b)'l' };
			for (UBINT i = 0; i < 3; i++){
				this->lpLexer->LoadNextChar();
				if (KeyWord_null[i] != this->lpLexer->LastChar){
					//failed
					this->EndRead();
					return nullptr;
				}
			}
			this->lpLexer->LoadNextChar();
			return new JSONNode_Null;
		}
		else{
			//failed
			this->EndRead();
			return nullptr;
		}
	}
	bool JSONReader::Skip_KeyWord(){
		if ((UBINT)'t' == this->lpLexer->LastChar){
			UINT4b KeyWord_true[3] = { (UINT4b)'r', (UINT4b)'u', (UINT4b)'e' };
			for (UBINT i = 0; i < 3; i++){
				this->lpLexer->LoadNextChar();
				if (KeyWord_true[i] != this->lpLexer->LastChar){
					//failed
					this->EndRead();
					return false;
				}
			}
			this->lpLexer->LoadNextChar();
			return true;
		}
		else if ((UBINT)'f' == this->lpLexer->LastChar){
			UINT4b KeyWord_false[4] = { (UINT4b)'a', (UINT4b)'l', (UINT4b)'s', (UINT4b)'e' };
			for (UBINT i = 0; i < 4; i++){
				this->lpLexer->LoadNextChar();
				if (KeyWord_false[i] != this->lpLexer->LastChar){
					//failed
					this->EndRead();
					return false;
				}
			}
			this->lpLexer->LoadNextChar();
			return true;
		}
		else if ((UBINT)'n' == this->lpLexer->LastChar){
			UINT4b KeyWord_null[3] = { (UINT4b)'u', (UINT4b)'l', (UINT4b)'l' };
			for (UBINT i = 0; i < 3; i++){
				this->lpLexer->LoadNextChar();
				if (KeyWord_null[i] != this->lpLexer->LastChar){
					//failed
					this->EndRead();
					return true;
				}
			}
			this->lpLexer->LoadNextChar();
			return true;
		}
		else{
			//failed
			this->EndRead();
			return false;
		}
	}

	inline void JSONReader::Generate_String(){ this->GenString_W = false; }
	inline void JSONReader::Generate_String_W(){ this->GenString_W = true; }
	inline void JSONReader::Generate_String_Sys(){ if (nsCharCoding::CHARCODING_SYS == nsCharCoding::UTF16L)this->GenString_W = true; else this->GenString_W = false; }

	inline void JSONReader::EndRead(){
		this->lpLexer = nullptr;
		this->StatStack.clear();
	}
	inline void JSONReader::BeginRead(nsText::Lexer *lpLexer){
		if (nullptr != this->lpLexer)this->EndRead();
		if (nullptr != lpLexer){
			this->lpLexer = lpLexer;
			if ((UBINT)-1 == this->lpLexer->LastChar)this->lpLexer = nullptr;
		}
	}
	std::pair<const nsText::String *, JSONNode *>JSONReader::ParseNode(){
		while (true){
			if (nullptr == this->lpLexer)return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
			else{
				nsText::String Symbol;
				JSONNode *RetValue = nullptr;
				const nsText::String *RetIndex = nullptr;
				ReaderStat *CurStat = nullptr;

				this->lpLexer->SkipBlankAndRet();
				if (!this->StatStack.empty()){
					while (true){
						CurStat = &this->StatStack.back();
						if ((CurStat->Status & ReaderStat::IsObject) > 0){
							if ((UINT4b)'}' == this->lpLexer->LastChar){
								this->StatStack.pop_back();
								this->lpLexer->LoadNextChar();
								if (this->StatStack.empty()){
									this->lpLexer = nullptr;
									return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
								}
								this->lpLexer->SkipBlankAndRet();
							}
							else break;
						}
						else{
							if ((UINT4b)']' == this->lpLexer->LastChar){
								this->StatStack.pop_back();
								this->lpLexer->LoadNextChar();
								if (this->StatStack.empty()){
									this->lpLexer = nullptr;
									return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
								}
								this->lpLexer->SkipBlankAndRet();
							}
							else break;
						}
					}

					if (0 == (CurStat->Status & ReaderStat::IsFirst)){
						if ((UINT4b)',' == this->lpLexer->LastChar){
							this->lpLexer->LoadNextChar();
							this->lpLexer->SkipBlankAndRet();
						}
						else{
							//failed
							this->EndRead();
							return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
						}
					}
					else CurStat->Status ^= ReaderStat::IsFirst;

					if ((CurStat->Status & ReaderStat::IsObject) > 0){
						if (this->Parse_String(&Symbol)){
							this->lpLexer->SkipBlankAndRet();
							if ((UINT4b)':' == this->lpLexer->LastChar){
								this->lpLexer->LoadNextChar();
								this->lpLexer->SkipBlankAndRet();
							}
							else{
								//failed
								this->EndRead();
								return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
							}
						}
						else{
							//failed
							this->EndRead();
							return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
						}
					}
				}

				//parsing
				if ((UINT4b)'{' == this->lpLexer->LastChar){
					RetValue = new JSONNode_Object;
					ReaderStat NewStat = { RetValue, ReaderStat::IsObject + ReaderStat::IsFirst };
					this->StatStack.push_back(NewStat);
					if (this->StatStack.size() > 1)CurStat = &this->StatStack[this->StatStack.size() - 2];
					this->lpLexer->LoadNextChar();
				}
				else if ((UINT4b)'[' == this->lpLexer->LastChar){
					RetValue = new JSONNode_Array;
					ReaderStat NewStat = { RetValue, ReaderStat::IsFirst };
					this->StatStack.push_back(NewStat);
					if (this->StatStack.size() > 1)CurStat = &this->StatStack[this->StatStack.size() - 2];
					this->lpLexer->LoadNextChar();
				}
				else if ((UINT4b)'\"' == this->lpLexer->LastChar){
					if (this->GenString_W){
						nsText::String_W TmpStr;
						if (this->Parse_String(&TmpStr)){
							JSONNode_String_W *_RetValue = new JSONNode_String_W;
							_RetValue->Content = std::move(TmpStr);
							RetValue = _RetValue;
						}
						else{
							this->EndRead();
							return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
						}
					}
					else{
						nsText::String TmpStr;
						if (this->Parse_String(&TmpStr)){
							JSONNode_String *_RetValue = new JSONNode_String;
							_RetValue->Content = std::move(TmpStr);
							RetValue = _RetValue;
						}
						else{
							this->EndRead();
							return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
						}
					}
				}
				else if ((UINT4b)'-' == this->lpLexer->LastChar || ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar)){
					RetValue = this->Parse_Number();
					if (nullptr == RetValue)return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
				}
				else{
					RetValue = this->Parse_KeyWord();
					if (nullptr == RetValue)return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
				}

				if (nullptr != CurStat){
					if ((CurStat->Status & CurStat->IsObject) > 0){
						auto Result = static_cast<JSONNode_Object *>(CurStat->lpNode)->Content.insert(std::pair<const nsText::String, JSONNode *>(Symbol, RetValue));
						if (!Result.second){
							RetValue->Destroy();
							continue;
						}
						else RetIndex = &Result.first->first;
					}
					else{
						try{
							static_cast<JSONNode_Array *>(CurStat->lpNode)->Content.push_back(RetValue);
						}
						catch (...){
							RetValue->Destroy();
							this->EndRead();
							return std::pair<const nsText::String *, JSONNode *>(nullptr, nullptr);
						}
					}
				}

				return std::pair<const nsText::String *, JSONNode *>(RetIndex, RetValue);
			}
		}
	}
	bool JSONReader::ParseAllChild(){
		UBINT StackDepth = this->StatStack.size();
		if (0 == StackDepth)return true;

		while (true){
			if (nullptr == this->lpLexer)return true;
			else{
				nsText::String Symbol;
				JSONNode *RetValue = nullptr;
				ReaderStat *CurStat = nullptr;

				this->lpLexer->SkipBlankAndRet();
				while (true){
					CurStat = &this->StatStack.back();
					if ((CurStat->Status & ReaderStat::IsObject) > 0){
						if ((UINT4b)'}' == this->lpLexer->LastChar){
							this->StatStack.pop_back();
							this->lpLexer->LoadNextChar();
							if (this->StatStack.empty()){
								this->lpLexer = nullptr;
								return true;
							}
							this->lpLexer->SkipBlankAndRet();
						}
						else break;
					}
					else{
						if ((UINT4b)']' == this->lpLexer->LastChar){
							this->StatStack.pop_back();
							this->lpLexer->LoadNextChar();
							if (this->StatStack.empty()){
								this->lpLexer = nullptr;
								return true;
							}
							this->lpLexer->SkipBlankAndRet();
						}
						else break;
					}
				}

				if (this->StatStack.size() < StackDepth)return true;

				if (0 == (CurStat->Status & ReaderStat::IsFirst)){
					if ((UINT4b)',' == this->lpLexer->LastChar){
						this->lpLexer->LoadNextChar();
						this->lpLexer->SkipBlankAndRet();
					}
					else{
						//failed
						this->EndRead();
						return false;
					}
				}
				else CurStat->Status ^= ReaderStat::IsFirst;

				if ((CurStat->Status & ReaderStat::IsObject) > 0){
					if (this->Parse_String(&Symbol)){
						this->lpLexer->SkipBlankAndRet();
						if ((UINT4b)':' == this->lpLexer->LastChar){
							this->lpLexer->LoadNextChar();
							this->lpLexer->SkipBlankAndRet();
						}
						else{
							//failed
							this->EndRead();
							return false;
						}
					}
					else{
						//failed
						this->EndRead();
						return false;
					}
				}

				//parsing
				if ((UINT4b)'{' == this->lpLexer->LastChar){
					RetValue = new JSONNode_Object;
					ReaderStat NewStat = { RetValue, ReaderStat::IsObject + ReaderStat::IsFirst };
					this->StatStack.push_back(NewStat);
					if (this->StatStack.size() > 1)CurStat = &this->StatStack[this->StatStack.size() - 2];
					this->lpLexer->LoadNextChar();
				}
				else if ((UINT4b)'[' == this->lpLexer->LastChar){
					RetValue = new JSONNode_Array;
					ReaderStat NewStat = { RetValue, ReaderStat::IsFirst };
					this->StatStack.push_back(NewStat);
					if (this->StatStack.size() > 1)CurStat = &this->StatStack[this->StatStack.size() - 2];
					this->lpLexer->LoadNextChar();
				}
				else if ((UINT4b)'\"' == this->lpLexer->LastChar){
					if (this->GenString_W){
						nsText::String_W TmpStr;
						if (this->Parse_String(&TmpStr)){
							JSONNode_String_W *_RetValue = new JSONNode_String_W;
							_RetValue->Content = std::move(TmpStr);
							RetValue = _RetValue;
						}
						else{
							this->EndRead();
							return false;
						}
					}
					else{
						nsText::String TmpStr;
						if (this->Parse_String(&TmpStr)){
							JSONNode_String *_RetValue = new JSONNode_String;
							_RetValue->Content = std::move(TmpStr);
							RetValue = _RetValue;
						}
						else{
							this->EndRead();
							return false;
						}
					}
				}
				else if ((UINT4b)'-' == this->lpLexer->LastChar || ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar)){
					RetValue = this->Parse_Number();
					if (nullptr == RetValue)return false;
				}
				else{
					RetValue = this->Parse_KeyWord();
					if (nullptr == RetValue)return false;
				}

				if (nullptr != CurStat){
					if ((CurStat->Status & CurStat->IsObject) > 0){
						auto Result = static_cast<JSONNode_Object *>(CurStat->lpNode)->Content.insert(std::pair<const nsText::String, JSONNode *>(Symbol, RetValue));
						if (!Result.second)RetValue->Destroy();
					}
					else{
						try{
							static_cast<JSONNode_Array *>(CurStat->lpNode)->Content.push_back(RetValue);
						}
						catch (...){
							RetValue->Destroy();
							this->EndRead();
							return false;
						}
					}
				}
			}
		}
	}
	bool JSONReader::SkipAllChild(){
		UBINT StackDepth = this->StatStack.size();
		if (0 == StackDepth)return true;

		while (true){
			if (nullptr == this->lpLexer)return true;
			else{
				ReaderStat *CurStat = nullptr;

				this->lpLexer->SkipBlankAndRet();
				while (true){
					CurStat = &this->StatStack.back();
					if ((CurStat->Status & ReaderStat::IsObject) > 0){
						if ((UINT4b)'}' == this->lpLexer->LastChar){
							this->StatStack.pop_back();
							this->lpLexer->LoadNextChar();
							if (this->StatStack.empty()){
								this->lpLexer = nullptr;
								return true;
							}
							this->lpLexer->SkipBlankAndRet();
						}
						else break;
					}
					else{
						if ((UINT4b)']' == this->lpLexer->LastChar){
							this->StatStack.pop_back();
							this->lpLexer->LoadNextChar();
							if (this->StatStack.empty()){
								this->lpLexer = nullptr;
								return true;
							}
							this->lpLexer->SkipBlankAndRet();
						}
						else break;
					}
				}
				if (this->StatStack.size() < StackDepth)return true;

				if (0 == (CurStat->Status & ReaderStat::IsFirst)){
					if ((UINT4b)',' == this->lpLexer->LastChar){
						this->lpLexer->LoadNextChar();
						this->lpLexer->SkipBlankAndRet();
					}
					else{
						//failed
						this->EndRead();
						return false;
					}
				}
				else CurStat->Status ^= ReaderStat::IsFirst;

				if ((CurStat->Status & ReaderStat::IsObject) > 0){
					if (this->Skip_String()){
						this->lpLexer->SkipBlankAndRet();
						if ((UINT4b)':' == this->lpLexer->LastChar){
							this->lpLexer->LoadNextChar();
							this->lpLexer->SkipBlankAndRet();
						}
						else{
							//failed
							this->EndRead();
							return false;
						}
					}
					else{
						//failed
						this->EndRead();
						return false;
					}
				}

				//parsing
				if ((UINT4b)'{' == this->lpLexer->LastChar){
					ReaderStat NewStat = { nullptr, ReaderStat::IsObject + ReaderStat::IsFirst };
					this->StatStack.push_back(NewStat);
					this->lpLexer->LoadNextChar();
				}
				else if ((UINT4b)'[' == this->lpLexer->LastChar){
					ReaderStat NewStat = { nullptr, ReaderStat::IsFirst };
					this->StatStack.push_back(NewStat);
					this->lpLexer->LoadNextChar();
				}
				else if ((UINT4b)'\"' == this->lpLexer->LastChar){
					if (!this->Skip_String()){
						this->EndRead();
						return false;
					}
				}
				else if ((UINT4b)'-' == this->lpLexer->LastChar || ((UINT4b)'0' <= this->lpLexer->LastChar && (UINT4b)'9' >= this->lpLexer->LastChar)){
					if (!this->Skip_Number())return false;
				}
				else{
					if (!this->Skip_KeyWord())return false;
				}
			}
		}
	}

	template <> bool JSONWriter::WriteString_Internal<char>(const char *lpString){
		unsigned char DecoderState = nsCharCoding::_utf8_decode_success;
		UINT4b CodePoint;
		const UINT4b Quote[2] = { (UINT4b)'\\', (UINT4b)'\"' };

		if (false == lpStream->Write(&Quote[1]))return false;
		while (*lpString != '0'){
			nsCharCoding::_utf8_decode(&DecoderState, &CodePoint, (unsigned char)*lpString);
			if (nsCharCoding::_utf8_decode_success == DecoderState){
				if (Quote[1] == CodePoint){
					if (lpStream->WriteBulk(Quote, 2) < 2)return false;
				}
				else if (false == lpStream->Write(&CodePoint))return false;
			}
			else if (nsCharCoding::_utf8_decode_failed == DecoderState)return false;
			lpString++;
		}
		if (false == lpStream->Write(&Quote[1]))return false;
		return true;
	}
	template <> bool JSONWriter::WriteString_Internal<wchar_t>(const wchar_t *lpString){
		wchar_t DecoderState = 0;
		UINT4b CodePoint;
		const UINT4b Quote[2] = { (UINT4b)'\\', (UINT4b)'\"' };

		if (false == lpStream->Write(&Quote[1]))return false;
		while (*lpString != L'0'){
			if (0x2E == DecoderState){
				if (0x2F != *lpString >> 10)return false;
				CodePoint = ((CodePoint & 0x3FF) << 10) + (*lpString & 0x3FF);
			}
			else CodePoint = *lpString;
			DecoderState = *lpString >> 10;

			if (0x2E != DecoderState){
				if (Quote[1] == CodePoint){
					if (lpStream->WriteBulk(Quote, 2) < 2)return false;
				}
				else if (false == lpStream->Write(&CodePoint))return false;
			}
			lpString++;
		}
		if (false == lpStream->Write(&Quote[1]))return false;
		return true;
	}
	template <> bool JSONWriter::WriteString_Internal<nsText::String>(const nsText::String *lpString){
		unsigned char DecoderState = 0;
		UINT4b CodePoint;
		const UINT4b Quote[2] = { (UINT4b)'\\', (UINT4b)'\"' }, Terminate_Char[6] = { (UINT4b)'\\', (UINT4b)'u', (UINT4b)'0', (UINT4b)'0', (UINT4b)'0', (UINT4b)'0' };

		if (false == lpStream->Write(&Quote[1]))return false;
		const unsigned char *lpChar = lpString->cbegin();
		for (UBINT i = 0; i < lpString->size(); i++){
			nsCharCoding::_utf8_decode(&DecoderState, &CodePoint, lpChar[i]);
			if (nsCharCoding::_utf8_decode_success == DecoderState){
				if (0 == CodePoint){
					if (lpStream->WriteBulk(Terminate_Char, 6) < 6)return false;
				}
				else if (Quote[1] == CodePoint){
					if (lpStream->WriteBulk(Quote, 2) < 2)return false;
				}
				else if (false == lpStream->Write(&CodePoint))return false;
			}
			else if (nsCharCoding::_utf8_decode_failed == DecoderState)return false;
		}
		if (false == lpStream->Write(&Quote[1]))return false;
		return true;
	}
	template <> bool JSONWriter::WriteString_Internal<nsText::String_W>(const nsText::String_W *lpString){
		wchar_t DecoderState = 0;
		UINT4b CodePoint;
		const UINT4b Quote[2] = { (UINT4b)'\\', (UINT4b)'\"' }, Terminate_Char[6] = { (UINT4b)'\\', (UINT4b)'u', (UINT4b)'0', (UINT4b)'0', (UINT4b)'0', (UINT4b)'0' };

		if (false == lpStream->Write(&Quote[1]))return false;
		const wchar_t *lpChar = lpString->cbegin();
		for (UBINT i = 0; i < lpString->size(); i++){
			if (0x2E == DecoderState){
				if (0x2F != lpChar[i] >> 10)return false;
				CodePoint = ((CodePoint & 0x3FF) << 10) + (lpChar[i] & 0x3FF);
			}
			else CodePoint = lpChar[i];
			DecoderState = lpChar[i] >> 10;

			if (0x2E != DecoderState){
				if (0 == CodePoint){
					if (lpStream->WriteBulk(Terminate_Char, 6) < 6)return false;
				}
				else if (Quote[1] == CodePoint){
					if (lpStream->WriteBulk(Quote, 2) < 2)return false;
				}
				else if (false == lpStream->Write(&CodePoint))return false;
			}
		}
		if (false == lpStream->Write(&Quote[1]))return false;
		return true;
	}
	inline bool JSONWriter::Advance_Array(){
		if (!this->StatStack.empty()){
			UBINT Status = this->StatStack.back();
			if ((Status & JSONWriter::IsObject) > 0)return false;
			if ((Status & JSONWriter::IsFirst) > 0)this->StatStack.back() ^= JSONWriter::IsFirst;
			else{
				UINT4b Comma = (UINT4b)',';
				if (false == this->lpStream->Write(&Comma))return false;
			}
		}
		return true;
	}
	inline bool JSONWriter::Advance_Object(){
		if (!this->StatStack.empty()){
			UBINT Status = this->StatStack.back();
			if (0 == (Status & JSONWriter::IsObject))return false;
			if ((Status & JSONWriter::IsFirst) > 0)this->StatStack.back() ^= JSONWriter::IsFirst;
			else{
				UINT4b Comma = (UINT4b)',';
				if (false == this->lpStream->Write(&Comma))return false;
			}
		}
		return true;
	}
	inline void JSONWriter::BeginWrite(nsBasic::Stream_W<UINT4b> *lpStream){
		this->lpStream = lpStream;
	}
	bool JSONWriter::BeginArray(){
		if (nullptr == this->lpStream)return false;

		UINT4b Bracket = (UINT4b)'[';

		if (false == this->Advance_Array()){ this->lpStream = nullptr; return false; }
		if(false ==this->lpStream->Write(&Bracket)){this->lpStream = nullptr; return false; }
		this->StatStack.push_back(JSONWriter::IsFirst);
		return true;
	}
	bool JSONWriter::BeginObject(){
		if (nullptr == this->lpStream)return false;

		UINT4b Bracket = (UINT4b)'{';

		if (false == this->Advance_Array()){ this->lpStream = nullptr; return false; }
		if (false == this->lpStream->Write(&Bracket)){ this->lpStream = nullptr; return false; }
		this->StatStack.push_back(JSONWriter::IsFirst | JSONWriter::IsObject);
		return true;
	}
	bool JSONWriter::WriteInt(INT8b Value){
		if (nullptr == this->lpStream)return false;

		if (false == this->Advance_Array()){ this->lpStream = nullptr; return false; }
		if (false == nsText::WriteInt8b(this->lpStream, Value)){this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}
	bool JSONWriter::WriteFloat(double Value){
		if (nullptr == this->lpStream)return false;

		if (false == this->Advance_Array()){ this->lpStream = nullptr; return false; }

		UINT8b Value_Bits = *reinterpret_cast<UINT8b *>(&Value);
		if ((Value_Bits & nsMath::NumericTrait<double>::Exponent_Mask) >= nsMath::NumericTrait<double>::Exponent_Mask){
			if (false == nsText::WriteDouble_Short(this->lpStream, 0.0)){ this->lpStream = nullptr; return false; }
		}
		else if (false == nsText::WriteDouble_Short(this->lpStream, Value)){this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}
	bool JSONWriter::WriteBool(bool Value){
		if (nullptr == this->lpStream)return false;

		UINT4b KeyWord_true[4] = { (UINT4b)'t', (UINT4b)'r', (UINT4b)'u', (UINT4b)'e' };
		UINT4b KeyWord_false[5] = { (UINT4b)'f', (UINT4b)'a', (UINT4b)'l', (UINT4b)'s', (UINT4b)'e' };

		if (false == this->Advance_Array()){ this->lpStream = nullptr; return false; }
		if (Value){
			if (false == this->lpStream->WriteBulk(KeyWord_true, 4)){this->lpStream = nullptr; return false; }
		}
		else if (false == this->lpStream->WriteBulk(KeyWord_false, 5)){this->lpStream = nullptr; return false; }
		
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}
	bool JSONWriter::WriteNull(){
		if (nullptr == this->lpStream)return false;

		UINT4b KeyWord_null[4] = { (UINT4b)'n', (UINT4b)'u', (UINT4b)'l', (UINT4b)'l' };

		if (false == this->Advance_Array()){ this->lpStream = nullptr; return false; }
		if (false == this->lpStream->WriteBulk(KeyWord_null, 4)){this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}
	template <typename T> bool JSONWriter::WriteString(const T *lpString){
		if (nullptr == this->lpStream)return false;

		if (false == this->Advance_Array()){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(lpString)){ this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}
	bool JSONWriter::EndArray(){
		if (nullptr == this->lpStream)return false;

		if (!this->StatStack.empty() && 0 == (this->StatStack.back() & JSONWriter::IsObject)){
			UINT4b Bracket = (UINT4b)']';
			if (false == this->lpStream->Write(&Bracket)){this->lpStream = nullptr; return false; }
			this->StatStack.pop_back();
			if (this->StatStack.empty())this->lpStream = nullptr;
			return true;
		}
		else {this->lpStream = nullptr; return false; }
	}
	template <typename T> bool JSONWriter::BeginArray_Indexed(const T *Index){
		if (nullptr == this->lpStream)return false;

		UINT4b Colon_Bracket[2] = { (UINT4b)':', (UINT4b)'[' };

		if (false == this->Advance_Object()){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(Index)){ this->lpStream = nullptr; return false; }
		if (this->lpStream->WriteBulk(Colon_Bracket, 2) < 2){ this->lpStream = nullptr; return false; }
		this->StatStack.push_back(JSONWriter::IsFirst);
		return true;
	}
	template <typename T> bool JSONWriter::BeginObject_Indexed(const T *Index){
		if (nullptr == this->lpStream)return false;

		UINT4b Colon_Bracket[2] = { (UINT4b)':', (UINT4b)'{' };

		if (false == this->Advance_Object()){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(Index)){ this->lpStream = nullptr; return false; }
		if (this->lpStream->WriteBulk(Colon_Bracket, 2) < 2){ this->lpStream = nullptr; return false; }
		this->StatStack.push_back(JSONWriter::IsFirst || JSONWriter::IsObject);
		return true;
	}
	template <typename T> bool JSONWriter::WriteInt_Indexed(const T *Index, INT8b Value){
		if (nullptr == this->lpStream)return false;

		UINT4b Colon = (UINT4b)':';

		if (false == this->Advance_Object()){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(Index)){ this->lpStream = nullptr; return false; }
		if (false == this->lpStream->Write(&Colon)){ this->lpStream = nullptr; return false; }
		if (false == nsText::WriteInt8b(this->lpStream, Value)){ this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}

	template <typename T> bool JSONWriter::WriteFloat_Indexed(const T *Index, double Value){
		if (nullptr == this->lpStream)return false;

		UINT4b Colon = (UINT4b)':';
		
		if (false == this->Advance_Object()){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(Index)){ this->lpStream = nullptr; return false; }
		if (false == this->lpStream->Write(&Colon)){ this->lpStream = nullptr; return false; }

		UINT8b Value_Bits = *reinterpret_cast<UINT8b *>(&Value);
		if ((Value_Bits & nsMath::NumericTrait<double>::Exponent_Mask) >= nsMath::NumericTrait<double>::Exponent_Mask){
			if (false == nsText::WriteDouble_Short(this->lpStream, 0.0)){ this->lpStream = nullptr; return false; }
		}
		else if (false == nsText::WriteDouble_Short(this->lpStream, Value)){ this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}

	template <typename T> bool JSONWriter::WriteBool_Indexed(const T *Index, bool Value){
		if (nullptr == this->lpStream)return false;

		UINT4b KeyWord_true[4] = { (UINT4b)'t', (UINT4b)'r', (UINT4b)'u', (UINT4b)'e' };
		UINT4b KeyWord_false[5] = { (UINT4b)'f', (UINT4b)'a', (UINT4b)'l', (UINT4b)'s', (UINT4b)'e' };
		UINT4b Colon = (UINT4b)':';

		if (false == this->Advance_Object()){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(Index)){ this->lpStream = nullptr; return false; }
		if (false == this->lpStream->Write(&Colon)){ this->lpStream = nullptr; return false; }
		if (Value){
			if (false == this->lpStream->WriteBulk(KeyWord_true, 4)){ this->lpStream = nullptr; return false; }
		}
		else if (false == this->lpStream->WriteBulk(KeyWord_false, 5)){ this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}

	template <typename T> bool JSONWriter::WriteNull_Indexed(const T *Index){
		if (nullptr == this->lpStream)return false;

		UINT4b KeyWord_null[4] = { (UINT4b)'n', (UINT4b)'u', (UINT4b)'l', (UINT4b)'l' };
		UINT4b Colon = (UINT4b)':';

		if (false == this->Advance_Object()){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(Index)){ this->lpStream = nullptr; return false; }
		if (false == this->lpStream->Write(&Colon)){ this->lpStream = nullptr; return false; }
		if (false == this->lpStream->WriteBulk(KeyWord_null, 4)){ this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}

	template <typename T1, typename T2> bool JSONWriter::WriteString_Indexed(const T1 *Index, const T2 *lpString){
		if (nullptr == this->lpStream)return false;

		UINT4b Colon = (UINT4b)':';

		if (false == this->Advance_Object()){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(Index)){ this->lpStream = nullptr; return false; }
		if (false == this->lpStream->Write(&Colon)){ this->lpStream = nullptr; return false; }
		if (false == this->WriteString_Internal(lpString)){ this->lpStream = nullptr; return false; }
		if (this->StatStack.empty())this->lpStream = nullptr;
		return true;
	}
	bool JSONWriter::EndObject(){
		if (!this->StatStack.empty() && (this->StatStack.back() & JSONWriter::IsObject) > 0){
			UINT4b Bracket = (UINT4b)'}';
			if (false == this->lpStream->Write(&Bracket)){this->lpStream = nullptr; return false; }
			this->StatStack.pop_back();
			if (this->StatStack.empty())this->lpStream = nullptr;
			return true;
		}
		else return false;
	}
	bool JSONWriter::WriteNode(const JSONNode *lpNode){
		switch (lpNode->Type){
		case JSONNode::JSON_NULL:
			return this->WriteNull();
		case JSONNode::JSON_BOOL:
			return this->WriteBool(static_cast<const JSONNode_Bool *>(lpNode)->Content);
		case JSONNode::JSON_INT:
			return this->WriteInt(static_cast<const JSONNode_Int *>(lpNode)->Content);
		case JSONNode::JSON_FLOAT:
			return this->WriteFloat(static_cast<const JSONNode_Float *>(lpNode)->Content);
		case JSONNode::JSON_STRING:
			return this->WriteString(&static_cast<const JSONNode_String *>(lpNode)->Content);
		case JSONNode::JSON_STRING_W:
			return this->WriteString(&static_cast<const JSONNode_String_W *>(lpNode)->Content);
		case JSONNode::JSON_ARRAY:
		{
			if (false == this->BeginArray())return false;
			const nsContainer::Vector<JSONNode *> *lpArr = &static_cast<const JSONNode_Array *>(lpNode)->Content;
			for (auto _It = lpArr->cbegin(); _It != lpArr->cend(); _It++){
				if (false == this->WriteNode(*_It))return false;
			}
			if (false == this->EndArray())return false;
			return true;
		}
		case JSONNode::JSON_OBJECT:
		{
			if (false == this->BeginObject())return false;
			const nsContainer::HashMap<nsText::String, JSONNode *> *lpMap = &static_cast<const JSONNode_Object *>(lpNode)->Content;
			for (auto _It = lpMap->cbegin(); _It != lpMap->cend(); _It++){
				if (false == this->WriteNode_Indexed(&_It->first, _It->second))return false;
			}
			if (false == this->EndObject())return false;
			return true;
		}
		default:
			return this->WriteNull(); break;
		}
	}
	template <typename T> bool JSONWriter::WriteNode_Indexed(const T *Index, const JSONNode *lpNode){
		switch (lpNode->Type){
		case JSONNode::JSON_NULL:
			return this->WriteNull_Indexed(Index);
		case JSONNode::JSON_BOOL:
			return this->WriteBool_Indexed(Index, static_cast<const JSONNode_Bool *>(lpNode)->Content);
		case JSONNode::JSON_INT:
			return this->WriteInt_Indexed(Index, static_cast<const JSONNode_Int *>(lpNode)->Content);
		case JSONNode::JSON_FLOAT:
			return this->WriteFloat_Indexed(Index, static_cast<const JSONNode_Float *>(lpNode)->Content);
		case JSONNode::JSON_STRING:
			return this->WriteString_Indexed(Index, &static_cast<const JSONNode_String *>(lpNode)->Content);
		case JSONNode::JSON_STRING_W:
			return this->WriteString_Indexed(Index, &static_cast<const JSONNode_String_W *>(lpNode)->Content);
		case JSONNode::JSON_ARRAY:
		{
			if (false == this->BeginArray_Indexed(Index))return false;
			const nsContainer::Vector<JSONNode *> *lpArr = &static_cast<const JSONNode_Array *>(lpNode)->Content;
			for (auto _It = lpArr->cbegin(); _It != lpArr->cend(); _It++){
				if (false == this->WriteNode(*_It))return false;
			}
			if (false == this->EndArray())return false;
			return true;
		}
		case JSONNode::JSON_OBJECT:
		{
			if (false == this->BeginObject_Indexed(Index))return false;
			const nsContainer::HashMap<nsText::String, JSONNode *> *lpMap = &static_cast<const JSONNode_Object *>(lpNode)->Content;
			for (auto _It = lpMap->cbegin(); _It != lpMap->cend(); _It++){
				if (false == this->WriteNode_Indexed(&_It->first, _It->second))return false;
			}
			if (false == this->EndObject())return false;
			return true;
		}
		default:
			return this->WriteNull_Indexed(Index); break;
		}
	}
	void JSONWriter::EndWrite(){
		if (nullptr != this->lpStream){
			UINT4b Bracket1 = (UINT4b)']';
			UINT4b Bracket2 = (UINT4b)'}';

			for (UBINT i = this->StatStack.size(); i > 0; i--){
				if ((this->StatStack[i - 1] & JSONWriter::IsObject) > 0){
					if (false == this->lpStream->Write(&Bracket2))break;
				}
				else{
					if (false == this->lpStream->Write(&Bracket1))break;
				}
			}
			this->StatStack.clear();
			this->lpStream = nullptr;
		}
	}
}
#endif