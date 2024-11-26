/* Description: Functions for text processing.
 * Language: C++
 * Author: ***
 *
 */

#ifndef LIB_TEXT_PROCESS
#define LIB_TEXT_PROCESS

#include "lFile.hpp"
#include "lText.hpp"
#include "lText_Process_Numeric.hpp"

namespace nsText{
	extern inline UBINT IsBlankChar(UBINT Char){
		return ((UBINT)' ' == Char
			|| 0xFFFE == Char //Unicode zero-width no-break space
			|| (UBINT)'\t' == Char
			|| (UBINT)'\v' == Char
			|| 0 == Char
			);
	}
	extern inline UBINT IsRetChar(UBINT Char){
		return ((UBINT)'\r' == Char
			|| (UBINT)'\n' == Char
			);
	}

	struct Lexer{
	private:
		Lexer(const Lexer &) = delete; //not copyable
		Lexer & operator =(const Lexer &) = delete; //not copyable
		void * operator new(size_t) = delete; //using operator new is prohibited
		void operator delete(void *) = delete; //using operator delete is prohibited
		operator nsBasic::ObjGeneral() = delete; //not a general object
	public:
		nsBasic::Stream_R<UINT4b> *Source;
		UBINT Status;
		UINT4b LastChar;

		// constructors
		Lexer();
		Lexer(nsBasic::Stream_R<UINT4b> *);

		inline bool isEOF(){ return (UINT4b)-1 == this->LastChar; }

		inline void Reset(){ this->Source = nullptr; this->LastChar = (UINT4b)-1; this->Status = 0; }
		inline void LoadNextChar(){ if (nullptr == this->Source || false == this->Source->Read(&this->LastChar))this->LastChar = (UINT4b)-1; }
		inline void LoadNewStream(nsBasic::Stream_R<UINT4b> *lpStreamSrc){ this->Source = lpStreamSrc; this->Status = 0; this->LoadNextChar(); }
		inline void SkipBlank(){ while (nsText::IsBlankChar(this->LastChar))this->LoadNextChar(); }
		inline void SkipBlankAndRet(){ while (nsText::IsBlankChar(this->LastChar) || nsText::IsRetChar(this->LastChar))this->LoadNextChar(); }
		inline void SkipLine(){ while ((!nsText::IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar)this->LoadNextChar(); }
		bool ReadUInt(UINT4b *lpDest);
		bool ReadUInt(UINT8b *lpDest);
		bool ReadInt(INT4b *lpDest);
		bool ReadInt(INT8b *lpDest);
		bool ReadFloat(double *lpDest);
		bool ReadFloat(float *lpDest);
		UBINT ReadWord(UBINT Coding, void *Buffer, UBINT MaxBytes); //does not guarantee that the output ends with '\0', for we don't know how large a '\0' is.
		void ReadWord(String *OutputString);
		void ReadWord(String_W *OutputString);
		void ReadLine(String *OutputString);
		void ReadLine(String_W *OutputString);
		void ReadFilePath(String *OutputString);
		void ReadFilePath(String_W *OutputString);
	};

	template <typename T> struct _ToString{ static String Value(const T& rhs); };
	template <typename T> extern inline String ToString(const T& rhs){ return _ToString<T>::Value(rhs); }
	extern bool WriteInt4b(nsBasic::Stream_W<UINT4b> *lpStream, INT4b Value);
	extern bool WriteInt8b(nsBasic::Stream_W<UINT4b> *lpStream, INT8b Value);
	template <bool UpperCase = true> extern bool WriteInt4b_Hex(nsBasic::Stream_W<UINT4b> *lpStream, INT4b Value);
	template <bool UpperCase = true> extern bool WriteInt8b_Hex(nsBasic::Stream_W<UINT4b> *lpStream, INT8b Value);
	extern bool WriteUInt4b(nsBasic::Stream_W<UINT4b> *lpStream, UINT4b Value);
	extern bool WriteUInt8b(nsBasic::Stream_W<UINT4b> *lpStream, UINT8b Value);
	template <bool UpperCase = true> extern bool WriteUInt4b_Hex(nsBasic::Stream_W<UINT4b> *lpStream, UINT4b Value);
	template <bool UpperCase = true> extern bool WriteUInt8b_Hex(nsBasic::Stream_W<UINT4b> *lpStream, UINT8b Value);
	extern bool WriteFloat(nsBasic::Stream_W<UINT4b> *lpStream, float Value);
	extern bool WriteDouble(nsBasic::Stream_W<UINT4b> *lpStream, double Value);
	extern bool WriteFloat_Short(nsBasic::Stream_W<UINT4b> *lpStream, float Value);
	extern bool WriteDouble_Short(nsBasic::Stream_W<UINT4b> *lpStream, double Value);
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const char *lpString);
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const char *lpString, UBINT Length);
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const wchar_t *lpString);
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const wchar_t *lpString, UBINT Length);
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const nsText::String_W *lpString);
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const nsText::String_W *lpString);

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	Lexer::Lexer(){
		this->Source = nullptr;
		this->LastChar = (UINT4b)-1;
		this->Status = 0;
	}
	inline Lexer::Lexer(nsBasic::Stream_R<UINT4b> *lpStreamSrc){
		this->LoadNewStream(lpStreamSrc);
	}
	bool Lexer::ReadUInt(UINT4b *lpDest){
		const UBINT DigitCnt = 10;

		if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
			//valid input

			*lpDest = this->LastChar - (UINT4b)'0';
			this->LoadNextChar();

			do{
				if ((UINT4b)'0' > this->LastChar || (UINT4b)'9' < this->LastChar)break;
				*lpDest = *lpDest * 10 + this->LastChar - (UINT4b)'0';
				this->LoadNextChar();
			} while (*lpDest < 100000000);

			if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
				if (*lpDest > 429496729)*lpDest = nsMath::NumericTrait<UINT4b>::Max;
				else if (this->LastChar > (UINT4b)'5')*lpDest = nsMath::NumericTrait<UINT4b>::Max;
				else{
					*lpDest = *lpDest * 10 + this->LastChar - (UINT4b)'0';
					this->LoadNextChar();
					if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
						do{ this->LoadNextChar(); } while ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar);
						*lpDest = nsMath::NumericTrait<UINT4b>::Max;
					}
				}
			}

			return true;
		}
		else return false;
	}
	bool Lexer::ReadUInt(UINT8b *lpDest){
		const UBINT DigitCnt = 20;

		if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
			//valid input

			*lpDest = (UINT8b)(this->LastChar - (UINT4b)'0');
			this->LoadNextChar();

			do{
				if ((UINT4b)'0' > this->LastChar || (UINT4b)'9' < this->LastChar)break;
				*lpDest = *lpDest * 10 + this->LastChar - (UINT4b)'0';
				this->LoadNextChar();
			} while (*lpDest < 1000000000000000000);

			if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
				if (*lpDest > 1844674407370955161)*lpDest = nsMath::NumericTrait<UINT8b>::Max;
				else if (this->LastChar > (UINT4b)'5')*lpDest = nsMath::NumericTrait<UINT8b>::Max;
				else{
					*lpDest = *lpDest * 10 + (UINT8b)(this->LastChar - (UINT4b)'0');
					this->LoadNextChar();
					if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
						do{ this->LoadNextChar(); } while ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar);
						*lpDest = nsMath::NumericTrait<UINT8b>::Max;
					}
				}
			}

			return true;
		}
		else return false;
	}
	bool Lexer::ReadInt(INT4b *lpDest){
		bool IsNegative;
		
		if ((UINT4b)'-' == this->LastChar){
			IsNegative = true;
			this->LoadNextChar();
		}
		else IsNegative = false;

		if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
			//valid input

			UINT4b InputValue = this->LastChar - (UINT4b)'0';
			this->LoadNextChar();

			do{
				if ((UINT4b)'0' > this->LastChar || (UINT4b)'9' < this->LastChar)break;
				InputValue = InputValue * 10 + this->LastChar - (UINT4b)'0';
				this->LoadNextChar();
			}while (InputValue < 100000000);

			if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
				if (InputValue > 429496729)InputValue = nsMath::NumericTrait<UINT4b>::Max;
				else if (this->LastChar > (UINT4b)'5')InputValue = nsMath::NumericTrait<UINT4b>::Max;
				else{
					InputValue = InputValue * 10 + this->LastChar - (UINT4b)'0';
					this->LoadNextChar();
					if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
						do{ this->LoadNextChar(); } while ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar);
						InputValue = nsMath::NumericTrait<UINT4b>::Max;
					}
				}
			}

			if (IsNegative){
				if (InputValue > nsMath::NumericTrait<UINT4b>::HighestBit)InputValue = nsMath::NumericTrait<UINT4b>::HighestBit;
				*lpDest = -(INT4b)InputValue;
			}
			else{
				if (InputValue >= nsMath::NumericTrait<UINT4b>::HighestBit)InputValue = nsMath::NumericTrait<UINT4b>::HighestBit - 1;
				*lpDest = (INT4b)InputValue;
			}
			return true;
		}
		else return false;
	}
	bool Lexer::ReadInt(INT8b *lpDest){
		const UBINT DigitCnt = 20;
		bool IsNegative;

		if ((UINT4b)'-' == this->LastChar){
			IsNegative = true;
			this->LoadNextChar();
		}
		else IsNegative = false;

		if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
			//valid input

			UINT8b InputValue = (UINT8b)(this->LastChar - (UINT4b)'0');
			this->LoadNextChar();

			do{
				if ((UINT4b)'0' > this->LastChar || (UINT4b)'9' < this->LastChar)break;
				InputValue = InputValue * 10 + this->LastChar - (UINT4b)'0';
				this->LoadNextChar();
			} while (InputValue < 1000000000000000000);

			if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
				if (InputValue > 1844674407370955161)InputValue = nsMath::NumericTrait<UINT8b>::Max;
				else if (this->LastChar > (UINT4b)'5')InputValue = nsMath::NumericTrait<UINT8b>::Max;
				else{
					InputValue = InputValue * 10 + (UINT8b)(this->LastChar - (UINT4b)'0');
					this->LoadNextChar();
					if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
						do{ this->LoadNextChar(); } while ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar);
						InputValue = nsMath::NumericTrait<UINT8b>::Max;
					}
				}
			}

			if (IsNegative){
				if (InputValue > nsMath::NumericTrait<UINT8b>::HighestBit)InputValue = nsMath::NumericTrait<UINT8b>::HighestBit;
				*lpDest = -(INT8b)InputValue;
			}
			else{
				if (InputValue >= nsMath::NumericTrait<UINT8b>::HighestBit)InputValue = nsMath::NumericTrait<UINT8b>::HighestBit - 1;
				*lpDest = (INT8b)InputValue;
			}
			return true;
		}
		else return false;
	}
	/*
	An inaccurate but fast version:

	bool Lexer::ReadFloat(double *lpDest){
		UBINT TmpStat1 = 0, TmpStat2 = 0;
		double Pos=1,Sign=1;
		*lpDest=0;
		while(1){
			switch(TmpStat1){
				case 0:
					if ((UINT4b)'-' == this->LastChar){ Sign = -1; this->LoadNextChar(); }
					TmpStat1=1;
					break;
				case 1:
					if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){ *lpDest = *lpDest * 10.0 + (double)(this->LastChar - (UINT4b)'0'); TmpStat2 = 1; }
					else if ((UINT4b)'.' == this->LastChar)TmpStat1 = 2;
					else if (0 == TmpStat2)return false; else{ *lpDest *= Sign; return true; }
					this->LoadNextChar();
					break;
				case 2:
					if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){ Pos *= 0.1; *lpDest = *lpDest + Pos*(double)(this->LastChar - (UINT4b)'0'); TmpStat2 = 1; }
					else if (0 == TmpStat2)return false; else{ *lpDest *= Sign; return true; }
					this->LoadNextChar();
					break;
			}
		}
	}*/
	bool Lexer::ReadFloat(double *lpDest){
		double RetValue;
		bool IsNegative;

		if ((UINT4b)'-' == this->LastChar){
			IsNegative = true;
			this->LoadNextChar();
		}
		else IsNegative = false;

		if ((UINT4b)'I' == this->LastChar || (UINT4b)'i' == this->LastChar){
			this->LoadNextChar();
			if ((UINT4b)'N' != this->LastChar && (UINT4b)'n' != this->LastChar)return false;
			this->LoadNextChar();
			if ((UINT4b)'F' != this->LastChar && (UINT4b)'f' != this->LastChar)return false;
			RetValue = nsMath::NumericTrait<double>::Inf_Positive;
		}
		else if ((UINT4b)'N' == this->LastChar || (UINT4b)'n' == this->LastChar){
			this->LoadNextChar();
			if ((UINT4b)'A' != this->LastChar && (UINT4b)'a' != this->LastChar)return false;
			this->LoadNextChar();
			if ((UINT4b)'N' != this->LastChar && (UINT4b)'n' != this->LastChar)return false;
			RetValue = nsMath::NumericTrait<double>::NaN_Positive;
		}
		else{
			UINT8b InputValue = 0;
			INT4b Exponent = 0, DigitRemain = 19;
			bool IntegerPartExist = false;
			bool FloatPartExist = false;

			if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
				// integer part
				IntegerPartExist = true;
				FloatPartExist = true;

				while ((UINT4b)'0' == this->LastChar)this->LoadNextChar();

				while (true){
					Exponent++;
					if ((UINT4b)'1' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
						if (DigitRemain > Exponent){
							for (UBINT i = 0; i < (UBINT)Exponent; i++)InputValue *= 10;
							InputValue += (UINT8b)(this->LastChar - (UINT4b)'0');
							DigitRemain -= Exponent;
							Exponent = 0;
						}
					}
					else if ((UINT4b)'0' != this->LastChar)break;
					this->LoadNextChar();
				}
				Exponent--;
			}

			if ((UINT4b)'.' == this->LastChar){
				this->LoadNextChar();
				if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
					INT4b MantissaDigitCount = Exponent;
					FloatPartExist = true;

					while (true){
						MantissaDigitCount++;
						if ((UINT4b)'1' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
							if (DigitRemain > MantissaDigitCount){
								for (UBINT i = 0; i < (UBINT)MantissaDigitCount; i++)InputValue *= 10;
								InputValue += (UINT8b)(this->LastChar - (UINT4b)'0');
								Exponent -= MantissaDigitCount;
								DigitRemain -= MantissaDigitCount;
								MantissaDigitCount = 0;
							}
						}
						else if ((UINT4b)'0' != this->LastChar)break;
						this->LoadNextChar();
					}
				}
				else if (false == IntegerPartExist)return false;
			}

			if (FloatPartExist){
				if ((UINT4b)'E' == this->LastChar || (UINT4b)'e' == this->LastChar){
					INT4b Exponent_Tmp = 0;
					bool Exponent_Tmp_IsNegative;

					this->LoadNextChar();
					if ((UINT4b)'-' == this->LastChar){
						Exponent_Tmp_IsNegative = true;
						this->LoadNextChar();
					}
					else{
						if ((UINT4b)'+' == this->LastChar)this->LoadNextChar();
						Exponent_Tmp_IsNegative = false;
					}

					if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
						do{
							Exponent_Tmp = Exponent_Tmp * 10 + (INT4b)(this->LastChar - (UINT4b)'0');
							this->LoadNextChar();
						} while ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar);
						if (Exponent_Tmp_IsNegative)Exponent_Tmp = -Exponent_Tmp;
						Exponent += Exponent_Tmp;
					}
					else return false;
				}
			}
			else return false;

			if (0 == InputValue || Exponent < -343)RetValue = 0.0;
			else if (Exponent > 308)RetValue = nsMath::NumericTrait<double>::Inf_Positive;
			else{
				__convert_float_base10_2(InputValue, Exponent, &RetValue);
				if (Exponent < -325){
					UINT8b RetValue_Bits = *reinterpret_cast<UINT8b *>(&RetValue);
					RetValue_Bits &= nsMath::NumericTrait<double>::HighestBit + nsMath::NumericTrait<double>::Exponent_Mask;
					if (RetValue_Bits >= nsMath::NumericTrait<double>::Exponent_Mask)RetValue = 0.0;
				}
				else if (Exponent > 290){
					UINT8b RetValue_Bits = *reinterpret_cast<UINT8b *>(&RetValue);
					RetValue_Bits &= nsMath::NumericTrait<double>::HighestBit + nsMath::NumericTrait<double>::Exponent_Mask;
					if (RetValue_Bits >= nsMath::NumericTrait<double>::Exponent_Mask)RetValue = nsMath::NumericTrait<double>::Inf_Positive;
				}
			}
		}

		if (IsNegative)RetValue = -RetValue;
		*lpDest = RetValue;
		return true;
	}
	bool Lexer::ReadFloat(float *lpDest){
		float RetValue;
		bool IsNegative;

		if ((UINT4b)'-' == this->LastChar){
			IsNegative = true;
			this->LoadNextChar();
		}
		else IsNegative = false;

		if ((UINT4b)'I' == this->LastChar || (UINT4b)'i' == this->LastChar){
			this->LoadNextChar();
			if ((UINT4b)'N' != this->LastChar && (UINT4b)'n' != this->LastChar)return false;
			this->LoadNextChar();
			if ((UINT4b)'F' != this->LastChar && (UINT4b)'f' != this->LastChar)return false;
			RetValue = nsMath::NumericTrait<float>::Inf_Positive;
		}
		else if ((UINT4b)'N' == this->LastChar || (UINT4b)'n' == this->LastChar){
			this->LoadNextChar();
			if ((UINT4b)'A' != this->LastChar && (UINT4b)'a' != this->LastChar)return false;
			this->LoadNextChar();
			if ((UINT4b)'N' != this->LastChar && (UINT4b)'n' != this->LastChar)return false;
			RetValue = nsMath::NumericTrait<float>::NaN_Positive;
		}
		else{
			UINT4b InputValue = 0;
			INT4b Exponent = 0, DigitRemain = 9;
			bool IntegerPartExist = false;
			bool FloatPartExist = false;

			if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
				// integer part
				IntegerPartExist = true;
				FloatPartExist = true;

				while ((UINT4b)'0' == this->LastChar)this->LoadNextChar();

				while (true){
					Exponent++;
					if ((UINT4b)'1' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
						if (DigitRemain > Exponent){
							for (UBINT i = 0; i < (UBINT)Exponent; i++)InputValue *= 10;
							InputValue += (this->LastChar - (UINT4b)'0');
							DigitRemain -= Exponent;
							Exponent = 0;
						}
					}
					else if ((UINT4b)'0' != this->LastChar)break;
					this->LoadNextChar();
				}
				Exponent--;
			}

			if ((UINT4b)'.' == this->LastChar){
				this->LoadNextChar();
				if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
					INT4b MantissaDigitCount = Exponent;
					FloatPartExist = true;

					while (true){
						MantissaDigitCount++;
						if ((UINT4b)'1' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
							if (DigitRemain > MantissaDigitCount){
								for (UBINT i = 0; i < (UBINT)MantissaDigitCount; i++)InputValue *= 10;
								InputValue += (this->LastChar - (UINT4b)'0');
								Exponent -= MantissaDigitCount;
								DigitRemain -= MantissaDigitCount;
								MantissaDigitCount = 0;
							}
						}
						else if ((UINT4b)'0' != this->LastChar)break;
						this->LoadNextChar();
					}
				}
				else if (false == IntegerPartExist)return false;
			}

			if (FloatPartExist){
				if ((UINT4b)'E' == this->LastChar || (UINT4b)'e' == this->LastChar){
					INT4b Exponent_Tmp = 0;
					bool Exponent_Tmp_IsNegative;

					this->LoadNextChar();
					if ((UINT4b)'-' == this->LastChar){
						Exponent_Tmp_IsNegative = true;
						this->LoadNextChar();
					}
					else{
						if ((UINT4b)'+' == this->LastChar)this->LoadNextChar();
						Exponent_Tmp_IsNegative = false;
					}
					if ((UINT4b)'+' == this->LastChar)this->LoadNextChar();

					if ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar){
						do{
							Exponent_Tmp = Exponent_Tmp * 10 + (INT4b)(this->LastChar - (UINT4b)'0');
							this->LoadNextChar();
						} while ((UINT4b)'0' <= this->LastChar && (UINT4b)'9' >= this->LastChar);
						if (Exponent_Tmp_IsNegative)Exponent_Tmp = -Exponent_Tmp;
						Exponent += Exponent_Tmp;
					}
					else return false;
				}
			}
			else return false;

			if (0 == InputValue || Exponent < -54)RetValue = 0.0;
			else if (Exponent > 38)RetValue = nsMath::NumericTrait<float>::Inf_Positive;
			else{
				__convert_float_base10_2(InputValue, Exponent, &RetValue);
				if (Exponent < -46){
					UINT4b RetValue_Bits = *reinterpret_cast<UINT4b *>(&RetValue);
					RetValue_Bits &= nsMath::NumericTrait<float>::HighestBit + nsMath::NumericTrait<float>::Exponent_Mask;
					if (RetValue_Bits >= nsMath::NumericTrait<float>::Exponent_Mask)RetValue = 0.0;
				}
				else if (Exponent > 29){
					UINT4b RetValue_Bits = *reinterpret_cast<UINT4b *>(&RetValue);
					RetValue_Bits &= nsMath::NumericTrait<float>::HighestBit + nsMath::NumericTrait<float>::Exponent_Mask;
					if (RetValue_Bits >= nsMath::NumericTrait<float>::Exponent_Mask)RetValue = nsMath::NumericTrait<float>::Inf_Positive;
				}
			}
		}

		if (IsNegative)RetValue = -RetValue;
		*lpDest = RetValue;
		return true;
	}
	UBINT Lexer::ReadWord(UBINT Coding, void *Buffer, UBINT MaxBytes){
		if (MaxBytes < nsCharCoding::ZeroCharLen[Coding])return 0;

		nsFile::MemStream TmpStream(Buffer, MaxBytes);
		nsFile::CoderStream TmpCoder(&TmpStream.AsWriteStream(), Coding);
		UBINT LastSuccess = 0;

		this->SkipBlank();
		while ((!IsBlankChar(this->LastChar)) && (!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){ //if not a blank
			if (LastSuccess >= MaxBytes - nsCharCoding::ZeroCharLen[Coding] || false == TmpCoder.Write(&this->LastChar)){
				while ((!IsBlankChar(this->LastChar)) && (!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar)this->LoadNextChar();
				break;
			}
			LastSuccess = TmpStream.Offset();
			this->LoadNextChar();
		}
		UINT4b TmpChar = 0;
		TmpCoder.Write(&TmpChar);
		return TmpStream.Offset();
	}
	void Lexer::ReadWord(String *OutputString){
		OutputString->clear();
		this->SkipBlank();
		while ((!IsBlankChar(this->LastChar)) && (!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){ //if not a blank
			OutputString->append(this->LastChar);
			this->LoadNextChar();
		}
	}
	void Lexer::ReadWord(String_W *OutputString){
		OutputString->clear();
		this->SkipBlank();
		while ((!IsBlankChar(this->LastChar)) && (!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){ //if not a blank
			OutputString->append(this->LastChar);
			this->LoadNextChar();
		}
	}
	void Lexer::ReadFilePath(String *OutputString){
		OutputString->clear();
		this->SkipBlank();
		if ((UBINT)"\"" == this->LastChar){
			this->LoadNextChar();
			while ((UBINT)"\"" != this->LastChar && (!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){
				OutputString->append(this->LastChar);
				this->LoadNextChar();
			}
			if ((UBINT)"\"" == this->LastChar)this->LoadNextChar();
		}
		else while ((!IsBlankChar(this->LastChar)) && (!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){ //if not a blank
			OutputString->append(this->LastChar);
			this->LoadNextChar();
		}
	}
	void Lexer::ReadFilePath(String_W *OutputString){
		OutputString->clear();
		this->SkipBlank();
		if ((UBINT)"\"" == this->LastChar){
			this->LoadNextChar();
			while ((UBINT)"\"" != this->LastChar && (!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){
				OutputString->append(this->LastChar);
				this->LoadNextChar();
			}
			if ((UBINT)"\"" == this->LastChar)this->LoadNextChar();
		}
		else while ((!IsBlankChar(this->LastChar)) && (!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){ //if not a blank
			OutputString->append(this->LastChar);
			this->LoadNextChar();
		}
	}
	void Lexer::ReadLine(String *OutputString){
		OutputString->clear();
		while ((!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){ //if not a blank
			OutputString->append(this->LastChar);
			this->LoadNextChar();
		}
	}
	void Lexer::ReadLine(String_W *OutputString){
		OutputString->clear();
		while ((!IsRetChar(this->LastChar)) && (UINT4b)-1 != this->LastChar){ //if not a blank
			OutputString->append(this->LastChar);
			this->LoadNextChar();
		}
	}

	template <> struct _ToString<float>{
		static String Value(const float& rhs){
			const UBINT DigitCnt = 10;
			char Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
			UINT4b Value_Bits = *reinterpret_cast<const UINT4b *>(&rhs);
			bool IsNegative;

			if ((Value_Bits & nsMath::NumericTrait<float>::HighestBit) > 0){
				Value_Bits ^= nsMath::NumericTrait<float>::HighestBit;
				IsNegative = true;
			}
			else IsNegative = false;

			if (nsMath::NumericTrait<float>::Exponent_Mask == (Value_Bits & nsMath::NumericTrait<float>::Exponent_Mask)){
				UBINT OutputLength = 3;
				DigitPtr -= OutputLength;
				if (0 == (Value_Bits & nsMath::NumericTrait<float>::Mantissa_Mask)){
					DigitPtr[0] = 'I';
					DigitPtr[1] = 'n';
					DigitPtr[2] = 'f';
				}
				else{
					DigitPtr[0] = 'N';
					DigitPtr[1] = 'a';
					DigitPtr[2] = 'N';
				}
				if (IsNegative){
					OutputLength++;
					(--DigitPtr)[0] = '-';
				}
				return std::move(String(DigitPtr, OutputLength));
			}
			else if (0 == Value_Bits){
				UBINT OutputLength = 3;
				DigitPtr -= OutputLength;
				DigitPtr[0] = '0';
				DigitPtr[1] = '.';
				DigitPtr[2] = '0';
				if (IsNegative){
					OutputLength++;
					(--DigitPtr)[0] = '-';
				}
				return std::move(String(DigitPtr, OutputLength));
			}
			else{
				UINT4b Mantissa;
				INT4b Exponent;
				String RetValue;

				__convert_float_base2_10(&Mantissa, &Exponent, *reinterpret_cast<float *>(&Value_Bits));

				BINT Mantissa_Length = __convert_uint4b_2_sym(Digit, Mantissa);
				DigitPtr -= Mantissa_Length;

				if (IsNegative)RetValue.append((unsigned char)'-');

				if (Exponent <= 0 && Exponent >= -Mantissa_Length){
					if (0 == Exponent){
						RetValue.append(DigitPtr, Mantissa_Length);
						RetValue.append(".0", 2);
					}
					else if (-Mantissa_Length == Exponent){
						RetValue.append("0.", 2);
						RetValue.append(DigitPtr, Mantissa_Length);
					}
					else{
						RetValue.append(DigitPtr, Mantissa_Length + Exponent);
						DigitPtr += Mantissa_Length + Exponent;
						RetValue.append((unsigned char)'.');
						RetValue.append(DigitPtr, -Exponent);
					}
				}
				else{
					RetValue.append(DigitPtr, Mantissa_Length);
					RetValue.append((unsigned char)'e');
					if (Exponent < 0){
						RetValue.append((unsigned char)'-');
						Exponent = -Exponent;
					}
					BINT Exponent_Length = __convert_uint4b_2_sym(Digit, (UINT4b)Exponent);
					DigitPtr = Digit + DigitCnt - Exponent_Length;
					RetValue.append(DigitPtr, Exponent_Length);
				}
				return std::move(RetValue);
			}
		}
	};
	template <> struct _ToString<double>{
		static String Value(const double& rhs){
			const UBINT DigitCnt = 20;
			char Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
			UINT8b Value_Bits = *reinterpret_cast<const UINT8b *>(&rhs);
			bool IsNegative;

			if ((Value_Bits & nsMath::NumericTrait<double>::HighestBit) > 0){
				Value_Bits ^= nsMath::NumericTrait<double>::HighestBit;
				IsNegative = true;
			}
			else IsNegative = false;

			if (nsMath::NumericTrait<double>::Exponent_Mask == (Value_Bits & nsMath::NumericTrait<double>::Exponent_Mask)){
				UBINT OutputLength = 3;
				DigitPtr -= OutputLength;
				if (0 == (Value_Bits & nsMath::NumericTrait<double>::Mantissa_Mask)){
					DigitPtr[0] = 'I';
					DigitPtr[1] = 'n';
					DigitPtr[2] = 'f';
				}
				else{
					DigitPtr[0] = 'N';
					DigitPtr[1] = 'a';
					DigitPtr[2] = 'N';
				}
				if (IsNegative){
					OutputLength++;
					(--DigitPtr)[0] = '-';
				}
				return std::move(String(DigitPtr, OutputLength));
			}
			else if (0 == Value_Bits){
				UBINT OutputLength = 3;
				DigitPtr -= OutputLength;
				DigitPtr[0] = '0';
				DigitPtr[1] = '.';
				DigitPtr[2] = '0';
				if (IsNegative){
					OutputLength++;
					(--DigitPtr)[0] = '-';
				}
				return std::move(String(DigitPtr, OutputLength));
			}
			else{
				UINT8b Mantissa;
				INT4b Exponent;
				String RetValue;

				__convert_float_base2_10(&Mantissa, &Exponent, *reinterpret_cast<double *>(&Value_Bits));

				BINT Mantissa_Length = __convert_uint8b_2_sym(Digit, Mantissa);
				DigitPtr -= Mantissa_Length;

				if (IsNegative)RetValue.append((unsigned char)'-');

				if (Exponent <= 0 && Exponent >= -Mantissa_Length){
					if (0 == Exponent){
						RetValue.append(DigitPtr, Mantissa_Length);
						RetValue.append(".0", 2);
					}
					else if (-Mantissa_Length == Exponent){
						RetValue.append("0.", 2);
						RetValue.append(DigitPtr, Mantissa_Length);
					}
					else{
						RetValue.append(DigitPtr, Mantissa_Length + Exponent);
						DigitPtr += Mantissa_Length + Exponent;
						RetValue.append((unsigned char)'.');
						RetValue.append(DigitPtr, -Exponent);
					}
				}
				else{
					RetValue.append(DigitPtr, Mantissa_Length);
					RetValue.append((unsigned char)'e');
					if (Exponent < 0){
						RetValue.append((unsigned char)'-');
						Exponent = -Exponent;
					}
					BINT Exponent_Length = __convert_uint8b_2_sym(Digit, (UINT8b)Exponent);
					DigitPtr = Digit + DigitCnt - Exponent_Length;
					RetValue.append(DigitPtr, Exponent_Length);
				}
				return std::move(RetValue);
			}
		}
	};
	template <> struct _ToString<INT4b>{
		static String Value(const INT4b& rhs){
			const UBINT DigitCnt = 11;
			char Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;

			UINT4b Value_Unsigned = (UINT4b)Value;
			if (Value < 0)Value_Unsigned = (~Value_Unsigned) + 1;

			UBINT OutputLength = __convert_uint4b_2_sym(Digit + 1, Value_Unsigned);
			DigitPtr -= OutputLength;
			if (Value < 0){
				OutputLength++; (--DigitPtr)[0] = (UINT4b)'-';
			}

			return std::move(String(DigitPtr, OutputLength));
		}
	};
	template <> struct _ToString<INT8b>{
		static String Value(const INT8b& rhs){
			const UBINT DigitCnt = 21;
			char Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;

			UINT8b Value_Unsigned = (UINT8b)Value;
			if (Value < 0)Value_Unsigned = (~Value_Unsigned) + 1;

			UBINT OutputLength = __convert_uint8b_2_sym(Digit + 1, Value_Unsigned);
			DigitPtr -= OutputLength;
			if (Value < 0){
				OutputLength++; (--DigitPtr)[0] = (UINT4b)'-';
			}

			return std::move(String(DigitPtr, OutputLength));
		}
	};
	template <> struct _ToString<UINT4b>{
		static String Value(const UINT4b& rhs){
			const UBINT DigitCnt = 10;
			char Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;

			UBINT OutputLength = __convert_uint4b_2_sym(Digit, rhs);
			DigitPtr -= OutputLength;

			return std::move(String(DigitPtr, OutputLength));
		}
	};
	template <> struct _ToString<UINT8b>{
		static String Value(const UINT8b& rhs){
			const UBINT DigitCnt = 20;
			char Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;

			UBINT OutputLength = __convert_uint8b_2_sym(Digit, rhs);
			DigitPtr -= OutputLength;

			return std::move(String(DigitPtr, OutputLength));
		}
	};

	extern bool WriteInt4b(nsBasic::Stream_W<UINT4b> *lpStream, INT4b Value){ //wrong. won't work for 0x80000000
		const UBINT DigitCnt = 11;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;

		UINT4b Value_Unsigned = (UINT4b)Value;
		if (Value < 0)Value_Unsigned = (~Value_Unsigned) + 1;

		UBINT OutputLength = __convert_uint4b_2_sym(Digit + 1, Value_Unsigned);
		DigitPtr -= OutputLength;
		if (Value < 0){
			OutputLength++; (--DigitPtr)[0] = (UINT4b)'-';
		}

		if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
	}
	extern bool WriteInt8b(nsBasic::Stream_W<UINT4b> *lpStream, INT8b Value){ //wrong. won't work for 0x8000000000000000
		const UBINT DigitCnt = 21;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;

		UINT8b Value_Unsigned = (UINT8b)Value;
		if (Value < 0)Value_Unsigned = (~Value_Unsigned) + 1;

		UBINT OutputLength = __convert_uint8b_2_sym(Digit + 1, Value_Unsigned);
		DigitPtr -= OutputLength;
		if (Value < 0){
			OutputLength++; (--DigitPtr)[0] = (UINT4b)'-';
		}

		if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
	}
	template <bool UpperCase> extern bool WriteInt4b_Hex(nsBasic::Stream_W<UINT4b> *lpStream, INT4b Value){
		const UINT4b HexOffset = UpperCase ? 'A' - 10 : 'a' - 10;
		const UBINT DigitCnt = 9;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
		if (0 == Value){
			*Digit = (UINT4b)'0';
			return lpStream->Write(Digit);
		}
		else{
			UINT4b Value_Unsigned = (UINT4b)Value;
			if (Value < 0)Value_Unsigned = (~Value_Unsigned) + 1;

			while (Value_Unsigned > 0){
				DigitPtr--;
				DigitPtr[0] = (UINT4b)Value_Unsigned & 0xF;
				if (DigitPtr[0] < 10)DigitPtr[0] += (UINT4b)'0'; else DigitPtr[0] += HexOffset;
				Value_Unsigned >>= 4;
			}
			if (Value < 0)(--DigitPtr)[0] = (UINT4b)'-';
			UBINT OutputLength = Digit + DigitCnt - DigitPtr;
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
	}
	template <bool UpperCase> extern bool WriteInt8b_Hex(nsBasic::Stream_W<UINT4b> *lpStream, INT8b Value){
		const UINT4b HexOffset = UpperCase ? 'A' - 10 : 'a' - 10;
		const UBINT DigitCnt = 17;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
		if (0 == Value){
			*Digit = (UINT4b)'0';
			return lpStream->Write(Digit);
		}
		else{
			UINT8b Value_Unsigned = (UINT8b)Value;
			if (Value < 0)Value_Unsigned = (~Value_Unsigned) + 1;

			while (Value_Unsigned > 0){
				DigitPtr--;
				DigitPtr[0] = (UINT4b)Value_Unsigned & 0xF;
				if (DigitPtr[0] < 10)DigitPtr[0] += (UINT4b)'0'; else DigitPtr[0] += HexOffset;
				Value_Unsigned >>= 4;
			}
			if (Value < 0)(--DigitPtr)[0] = (UINT4b)'-';
			UBINT OutputLength = Digit + DigitCnt - DigitPtr;
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
	}
	extern bool WriteUInt4b(nsBasic::Stream_W<UINT4b> *lpStream, UINT4b Value){
		const UBINT DigitCnt = 10;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;

		UBINT OutputLength = __convert_uint4b_2_sym(Digit, Value);
		DigitPtr -= OutputLength;

		if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
	}
	extern bool WriteUInt8b(nsBasic::Stream_W<UINT4b> *lpStream, UINT8b Value){
		const UBINT DigitCnt = 20;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;

		UBINT OutputLength = __convert_uint8b_2_sym(Digit, Value);
		DigitPtr -= OutputLength;

		if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
	}
	template <bool UpperCase> extern bool WriteUInt4b_Hex(nsBasic::Stream_W<UINT4b> *lpStream, UINT4b Value){
		const UINT4b HexOffset = UpperCase ? 'A' - 10 : 'a' - 10;
		const UBINT DigitCnt = 8;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
		if (0 == Value){
			*Digit = (UINT4b)'0';
			return lpStream->Write(Digit);
		}
		else{
			while (Value > 0){
				DigitPtr--;
				DigitPtr[0] = (UINT4b)Value & 0xF;
				if (DigitPtr[0] < 10)DigitPtr[0] += (UINT4b)'0'; else DigitPtr[0] += HexOffset;
				Value >>= 4;
			}
			UBINT OutputLength = Digit + DigitCnt - DigitPtr;
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
	}
	template <bool UpperCase> extern bool WriteUInt8b_Hex(nsBasic::Stream_W<UINT4b> *lpStream, UINT8b Value){
		const UINT4b HexOffset = UpperCase ? 'A' - 10 : 'a' - 10;
		const UBINT DigitCnt = 16;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
		if (0 == Value){
			*Digit = (UINT4b)'0';
			return lpStream->Write(Digit);
		}
		else{
			while (Value > 0){
				DigitPtr--;
				DigitPtr[0] = (UINT4b)Value & 0xF;
				if (DigitPtr[0] < 10)DigitPtr[0] += (UINT4b)'0'; else DigitPtr[0] += HexOffset;
				Value >>= 4;
			}
			UBINT OutputLength = Digit + DigitCnt - DigitPtr;
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
	}
	extern bool WriteDouble(nsBasic::Stream_W<UINT4b> *lpStream, double Value){
		const UBINT DigitCnt = 20;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
		UINT8b Value_Bits = *reinterpret_cast<UINT8b *>(&Value);
		bool IsNegative;

		if ((Value_Bits & nsMath::NumericTrait<double>::HighestBit) > 0){
			Value_Bits ^= nsMath::NumericTrait<double>::HighestBit;
			IsNegative = true;
		}
		else IsNegative = false;

		if (nsMath::NumericTrait<double>::Exponent_Mask == (Value_Bits & nsMath::NumericTrait<double>::Exponent_Mask)){
			UBINT OutputLength = 3;
			DigitPtr -= OutputLength;
			if (0 == (Value_Bits & nsMath::NumericTrait<double>::Mantissa_Mask)){
				DigitPtr[0] = (UINT4b)'I';
				DigitPtr[1] = (UINT4b)'n';
				DigitPtr[2] = (UINT4b)'f';
			}
			else{
				DigitPtr[0] = (UINT4b)'N';
				DigitPtr[1] = (UINT4b)'a';
				DigitPtr[2] = (UINT4b)'N';
			}
			if (IsNegative){
				OutputLength++;
				(--DigitPtr)[0] = (UINT4b)'-';
			}
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
		else if (0 == Value_Bits){
			UBINT OutputLength = 3;
			DigitPtr -= OutputLength;
			DigitPtr[0] = (UINT4b)'0';
			DigitPtr[1] = (UINT4b)'.';
			DigitPtr[2] = (UINT4b)'0';
			if (IsNegative){
				OutputLength++;
				(--DigitPtr)[0] = (UINT4b)'-';
			}
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
		else{
			UINT8b Mantissa;
			INT4b Exponent;
			__convert_float_base2_10(&Mantissa, &Exponent, *reinterpret_cast<double *>(&Value_Bits));

			BINT Mantissa_Length = __convert_uint8b_2_sym(Digit, Mantissa);
			DigitPtr -= Mantissa_Length;

			UINT4b Symbol = (UINT4b)'-';
			if (IsNegative){
				if (!lpStream->Write(&Symbol))return false;
			}
			if (Exponent >= 0){
				if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
				Symbol = (UINT4b)'0';
				for (INT4b i = 0; i < Exponent; i++){
					if (!lpStream->Write(&Symbol))return false;
				}
				Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
				Symbol = (UINT4b)'0'; if (!lpStream->Write(&Symbol))return false;
			}
			else if (Mantissa_Length + Exponent > 0){
				lpStream->WriteBulk(DigitPtr, Mantissa_Length + Exponent);
				DigitPtr += Mantissa_Length + Exponent;
				Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
				if (lpStream->WriteBulk(DigitPtr, -Exponent) < -Exponent)return false;
			}
			else{
				Symbol = (UINT4b)'0'; if (!lpStream->Write(&Symbol))return false;
				Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
				Symbol = (UINT4b)'0';
				for (INT4b i = 0; i < -Mantissa_Length - Exponent; i++){
					if (!lpStream->Write(&Symbol))return false;
				}
				if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
			}
			return true;
		}
	}

	extern bool WriteFloat(nsBasic::Stream_W<UINT4b> *lpStream, float Value){
		const UBINT DigitCnt = 10;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
		UINT4b Value_Bits = *reinterpret_cast<UINT4b *>(&Value);
		bool IsNegative;

		if ((Value_Bits & nsMath::NumericTrait<float>::HighestBit) > 0){
			Value_Bits ^= nsMath::NumericTrait<float>::HighestBit;
			IsNegative = true;
		}
		else IsNegative = false;

		if (nsMath::NumericTrait<float>::Exponent_Mask == (Value_Bits & nsMath::NumericTrait<float>::Exponent_Mask)){
			UBINT OutputLength = 3;
			DigitPtr -= OutputLength;
			if (0 == (Value_Bits & nsMath::NumericTrait<float>::Mantissa_Mask)){
				DigitPtr[0] = (UINT4b)'I';
				DigitPtr[1] = (UINT4b)'n';
				DigitPtr[2] = (UINT4b)'f';
			}
			else{
				DigitPtr[0] = (UINT4b)'N';
				DigitPtr[1] = (UINT4b)'a';
				DigitPtr[2] = (UINT4b)'N';
			}
			if (IsNegative){
				OutputLength++;
				(--DigitPtr)[0] = (UINT4b)'-';
			}
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
		else if (0 == Value_Bits){
			UBINT OutputLength = 3;
			DigitPtr -= OutputLength;
			DigitPtr[0] = (UINT4b)'0';
			DigitPtr[1] = (UINT4b)'.';
			DigitPtr[2] = (UINT4b)'0';
			if (IsNegative){
				OutputLength++;
				(--DigitPtr)[0] = (UINT4b)'-';
			}
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
		else{
			UINT4b Mantissa;
			INT4b Exponent;
			__convert_float_base2_10(&Mantissa, &Exponent, *reinterpret_cast<float *>(&Value_Bits));

			BINT Mantissa_Length = __convert_uint4b_2_sym(Digit, Mantissa);
			DigitPtr -= Mantissa_Length;

			UINT4b Symbol = (UINT4b)'-';
			if (IsNegative){
				if (!lpStream->Write(&Symbol))return false;
			}
			if (Exponent >= 0){
				if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
				Symbol = (UINT4b)'0';
				for (INT4b i = 0; i < Exponent; i++){
					if (!lpStream->Write(&Symbol))return false;
				}
				Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
				Symbol = (UINT4b)'0'; if (!lpStream->Write(&Symbol))return false;
			}
			else if (Mantissa_Length + Exponent > 0){
				lpStream->WriteBulk(DigitPtr, Mantissa_Length + Exponent);
				DigitPtr += Mantissa_Length + Exponent;
				Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
				if (lpStream->WriteBulk(DigitPtr, -Exponent) < -Exponent)return false;
			}
			else{
				Symbol = (UINT4b)'0'; if (!lpStream->Write(&Symbol))return false;
				Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
				Symbol = (UINT4b)'0';
				for (INT4b i = 0; i < -Mantissa_Length - Exponent; i++){
					if (!lpStream->Write(&Symbol))return false;
				}
				if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
			}
			return true;
		}
	}
	extern bool WriteDouble_Short(nsBasic::Stream_W<UINT4b> *lpStream, double Value){
		const UBINT DigitCnt = 20;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
		UINT8b Value_Bits = *reinterpret_cast<UINT8b *>(&Value);
		bool IsNegative;

		if ((Value_Bits & nsMath::NumericTrait<double>::HighestBit) > 0){
			Value_Bits ^= nsMath::NumericTrait<double>::HighestBit;
			IsNegative = true;
		}
		else IsNegative = false;

		if (nsMath::NumericTrait<double>::Exponent_Mask == (Value_Bits & nsMath::NumericTrait<double>::Exponent_Mask)){
			UBINT OutputLength = 3;
			DigitPtr -= OutputLength;
			if (0 == (Value_Bits & nsMath::NumericTrait<double>::Mantissa_Mask)){
				DigitPtr[0] = (UINT4b)'I';
				DigitPtr[1] = (UINT4b)'n';
				DigitPtr[2] = (UINT4b)'f';
			}
			else{
				DigitPtr[0] = (UINT4b)'N';
				DigitPtr[1] = (UINT4b)'a';
				DigitPtr[2] = (UINT4b)'N';
			}
			if (IsNegative){
				OutputLength++;
				(--DigitPtr)[0] = (UINT4b)'-';
			}
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
		else if (0 == Value_Bits){
			UBINT OutputLength = 3;
			DigitPtr -= OutputLength;
			DigitPtr[0] = (UINT4b)'0';
			DigitPtr[1] = (UINT4b)'.';
			DigitPtr[2] = (UINT4b)'0';
			if (IsNegative){
				OutputLength++;
				(--DigitPtr)[0] = (UINT4b)'-';
			}
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
		else{
			UINT8b Mantissa;
			INT4b Exponent;
			__convert_float_base2_10(&Mantissa, &Exponent, *reinterpret_cast<double *>(&Value_Bits));

			BINT Mantissa_Length = __convert_uint8b_2_sym(Digit, Mantissa);
			DigitPtr -= Mantissa_Length;

			UINT4b Symbol = (UINT4b)'-';
			if (IsNegative){
				if (!lpStream->Write(&Symbol))return false;
			}

			if (Exponent <= 0 && Exponent >= -Mantissa_Length){
				if (0 == Exponent){
					if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
					Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
					Symbol = (UINT4b)'0'; if (!lpStream->Write(&Symbol))return false;
				}
				else if (-Mantissa_Length == Exponent){
					Symbol = (UINT4b)'0'; if (!lpStream->Write(&Symbol))return false;
					Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
					if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
				}
				else{
					lpStream->WriteBulk(DigitPtr, Mantissa_Length + Exponent);
					DigitPtr += Mantissa_Length + Exponent;
					Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
					if (lpStream->WriteBulk(DigitPtr, -Exponent) < -Exponent)return false;
				}
			}
			else{
				if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
				Symbol = (UINT4b)'e'; if (!lpStream->Write(&Symbol))return false;
				if (Exponent < 0){
					Symbol = (UINT4b)'-';
					if (!lpStream->Write(&Symbol))return false;
					Exponent = -Exponent;
				}
				BINT Exponent_Length = __convert_uint8b_2_sym(Digit, (UINT8b)Exponent);
				DigitPtr = Digit + DigitCnt - Exponent_Length;
				if (lpStream->WriteBulk(DigitPtr, Exponent_Length) < (UBINT)Exponent_Length)return false;
			}
			return true;
		}
	}
	extern bool WriteFloat_Short(nsBasic::Stream_W<UINT4b> *lpStream, float Value){
		const UBINT DigitCnt = 10;
		UINT4b Digit[DigitCnt], *DigitPtr = Digit + DigitCnt;
		UINT4b Value_Bits = *reinterpret_cast<UINT4b *>(&Value);
		bool IsNegative;

		if ((Value_Bits & nsMath::NumericTrait<float>::HighestBit) > 0){
			Value_Bits ^= nsMath::NumericTrait<float>::HighestBit;
			IsNegative = true;
		}
		else IsNegative = false;

		if (nsMath::NumericTrait<float>::Exponent_Mask == (Value_Bits & nsMath::NumericTrait<float>::Exponent_Mask)){
			UBINT OutputLength = 3;
			DigitPtr -= OutputLength;
			if (0 == (Value_Bits & nsMath::NumericTrait<float>::Mantissa_Mask)){
				DigitPtr[0] = (UINT4b)'I';
				DigitPtr[1] = (UINT4b)'n';
				DigitPtr[2] = (UINT4b)'f';
			}
			else{
				DigitPtr[0] = (UINT4b)'N';
				DigitPtr[1] = (UINT4b)'a';
				DigitPtr[2] = (UINT4b)'N';
			}
			if (IsNegative){
				OutputLength++;
				(--DigitPtr)[0] = (UINT4b)'-';
			}
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
		else if (0 == Value_Bits){
			UBINT OutputLength = 3;
			DigitPtr -= OutputLength;
			DigitPtr[0] = (UINT4b)'0';
			DigitPtr[1] = (UINT4b)'.';
			DigitPtr[2] = (UINT4b)'0';
			if (IsNegative){
				OutputLength++;
				(--DigitPtr)[0] = (UINT4b)'-';
			}
			if (lpStream->WriteBulk(DigitPtr, OutputLength) >= OutputLength)return true; else return false;
		}
		else{
			UINT4b Mantissa;
			INT4b Exponent;
			__convert_float_base2_10(&Mantissa, &Exponent, *reinterpret_cast<float *>(&Value_Bits));

			BINT Mantissa_Length = __convert_uint4b_2_sym(Digit, Mantissa);
			DigitPtr -= Mantissa_Length;

			UINT4b Symbol = (UINT4b)'-';
			if (IsNegative){
				if (!lpStream->Write(&Symbol))return false;
			}

			if (Exponent <= 0 && Exponent >= -Mantissa_Length){
				if (0 == Exponent){
					if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
					Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
					Symbol = (UINT4b)'0'; if (!lpStream->Write(&Symbol))return false;
				}
				else if (-Mantissa_Length == Exponent){
					Symbol = (UINT4b)'0'; if (!lpStream->Write(&Symbol))return false;
					Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
					if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
				}
				else{
					lpStream->WriteBulk(DigitPtr, Mantissa_Length + Exponent);
					DigitPtr += Mantissa_Length + Exponent;
					Symbol = (UINT4b)'.'; if (!lpStream->Write(&Symbol))return false;
					if (lpStream->WriteBulk(DigitPtr, -Exponent) < -Exponent)return false;
				}
			}
			else{
				if (lpStream->WriteBulk(DigitPtr, Mantissa_Length) < (UBINT)Mantissa_Length)return false;
				Symbol = (UINT4b)'e'; if (!lpStream->Write(&Symbol))return false;
				if (Exponent < 0){
					Symbol = (UINT4b)'-';
					if (!lpStream->Write(&Symbol))return false;
					Exponent = -Exponent;
				}
				BINT Exponent_Length = __convert_uint4b_2_sym(Digit, (UINT4b)Exponent);
				DigitPtr = Digit + DigitCnt - Exponent_Length;
				if (lpStream->WriteBulk(DigitPtr, Exponent_Length) < (UBINT)Exponent_Length)return false;
			}
			return true;
		}
	}
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const char *lpString){
		unsigned char DecoderState = nsCharCoding::_utf8_decode_success;
		UINT4b CodePoint;

		while (*lpString != '\0'){
			nsCharCoding::_utf8_decode(&DecoderState, &CodePoint, (unsigned char)*lpString);
			if (nsCharCoding::_utf8_decode_success == DecoderState){
				if (false == lpStream->Write(&CodePoint))return false;
			}
			else if (nsCharCoding::_utf8_decode_failed == DecoderState)return false;
			lpString++;
		}
		return true;
	}
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const char *lpString, UBINT Length){
		unsigned char DecoderState = nsCharCoding::_utf8_decode_success;
		UINT4b CodePoint;

		for (UBINT i = 0; i < Length; i++){
			nsCharCoding::_utf8_decode(&DecoderState, &CodePoint, (unsigned char)lpString[i]);
			if (nsCharCoding::_utf8_decode_success == DecoderState){
				if (false == lpStream->Write(&CodePoint))return false;
			}
			else if (nsCharCoding::_utf8_decode_failed == DecoderState)return false;
		}
		return true;
	}
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const wchar_t *lpString){
		wchar_t DecoderState = 0;
		UINT4b CodePoint;

		while (*lpString != L'\0'){
			if (0x2E == DecoderState){
				if (0x2F != *lpString >> 10)return false;
				CodePoint = ((CodePoint & 0x3FF) << 10) + (*lpString & 0x3FF);
			}
			else CodePoint = *lpString;
			DecoderState = *lpString >> 10;

			if (0x2E != DecoderState){
				if (false == lpStream->Write(&CodePoint))return false;
			}
			lpString++;
		}
		return true;
	}
	extern bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const wchar_t *lpString, UBINT Length){
		wchar_t DecoderState = 0;
		UINT4b CodePoint;

		for (UBINT i = 0; i < Length; i++){
			if (0x2E == DecoderState){
				if (0x2F != lpString[i] >> 10)return false;
				CodePoint = ((CodePoint & 0x3FF) << 10) + (lpString[i] & 0x3FF);
			}
			else CodePoint = lpString[i];
			DecoderState = lpString[i] >> 10;

			if (0x2E != DecoderState){
				if (false == lpStream->Write(&CodePoint))return false;
			}
		}
		return true;
	}
	extern inline bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const nsText::String *lpString){
		return WriteString(lpStream, (const char *)lpString->cbegin(), lpString->size());
	}
	extern inline bool WriteString(nsBasic::Stream_W<UINT4b> *lpStream, const nsText::String_W *lpString){
		return WriteString(lpStream, lpString->cbegin(), lpString->size());
	}
}
#endif