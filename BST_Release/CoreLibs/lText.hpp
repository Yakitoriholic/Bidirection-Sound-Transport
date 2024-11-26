/* Description: Basic String Classes.
 * Language: C++
 * Author: ***
 *
 */

#ifndef LIB_TEXT
#define LIB_TEXT

#include "lGeneral.hpp"
#include "lAlg.hpp"

namespace nsText{
	class String;
	class String_W;

#if defined LIBENV_OS_WIN
	typedef String_W String_Sys;
#else
	typedef String String_Sys;
#endif
};

#include "lText_String.hpp"
#include "lText_String_W.hpp"

//mutual conversion
namespace nsText{
	String::String(const String_W &rhs) :String(){
		this->reserve(rhs.size());
		const wchar_t *lpStr = rhs.cbegin();
		for (UBINT i = 0; i < rhs.size(); i++){
			UINT4b CodePoint = (UINT4b)lpStr[i];

			if (0xDC00 != (CodePoint & 0xFC00)){
				if (0xD800 == (CodePoint & 0xFC00)){
					i++;
					if (i >= rhs.size())break;
					wchar_t NextChar = lpStr[i];
					if (0xDC00 == (NextChar & 0xFC00)){
						CodePoint = ((CodePoint & 0x3FF) << 10) + 0x10000;
						CodePoint += (UINT4b)NextChar & 0x3FF;
						if (CodePoint < 0x110000){ //restricted by RFC 3629
							this->append(CodePoint);
						}
					}
				}
				else this->append(CodePoint);
			}
		}
	}
	String_W::String_W(const String &rhs) : String_W(){
		this->reserve(rhs.size() / 4);
		const unsigned char *lpStr = rhs.cbegin();
		unsigned char DecoderState = nsCharCoding::_utf8_decode_success;
		UINT4b CodePoint = 0;
		for (UBINT i = 0; i < rhs.size(); i++){
			nsCharCoding::_utf8_decode(&DecoderState, &CodePoint, lpStr[i]);
			if (nsCharCoding::_utf8_decode_success == DecoderState)this->append(CodePoint);
			else if (nsCharCoding::_utf8_decode_failed == DecoderState){
				DecoderState = nsCharCoding::_utf8_decode_success;
				CodePoint = 0;
			}
		}
	}
};

#endif