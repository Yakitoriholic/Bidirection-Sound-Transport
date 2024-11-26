/* Description: Informations about character coding. DO NOT include this header directly.
* Language:C++
* Author:***
*/

namespace nsCharCoding{
	const UBINT ANSI = 0;
	const UBINT UTF8 = 1;
	const UBINT UTF16L = 2;
	const UBINT UTF16B = 3;

	extern const UBINT ZeroCharLen[] = {
		1, //ANSI
		1, //UTF8
		2, //UTF16L
		2, //UTF16B
	};

#if defined LIBENV_OS_WIN
	extern const UBINT CHARCODING_SYS = nsCharCoding::UTF16L;
#elif defined LIBENV_OS_LINUX
	extern const UBINT CODING_TEXT_DEFAULT = nsCharCoding::UTF8;
#endif

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

	// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
	// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

	const unsigned char _utf8_decode_success = 0;
	const unsigned char _utf8_decode_failed = 12;

	const unsigned char _utf8_decode_table[] = {
		// char type
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 11, 6, 6, 6, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,

		// status transition table
		0, 12, 24, 36, 60, 96, 84, 12, 12, 12, 48, 72, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
		12, 0, 12, 12, 12, 12, 12, 0, 12, 0, 12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 24, 12, 12,
		12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12,
		12, 12, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12,
		12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	};

	void inline _utf8_decode(unsigned char *DecoderState, UINT4b *lpCodePoint, unsigned char Value) {
		unsigned char CharType = _utf8_decode_table[Value];

		if (*DecoderState == _utf8_decode_success)*lpCodePoint = (0xFF >> CharType) & Value;
		else *lpCodePoint = (Value & 0x3F) | (*lpCodePoint << 6);

		*DecoderState = _utf8_decode_table[0x100 + *DecoderState + CharType];
	}

	UBINT inline _utf8_encode(unsigned char *lpEncodedChar, UINT4b CodePoint) {
		if (CodePoint >= 0x110000 || 0xD800 == (CodePoint & 0x10F800))return 0; //Failed. restricted by RFC 3629
		else if (CodePoint < 0x80){
			lpEncodedChar[0] = (unsigned char)CodePoint;
			return 1;
		}
		else if (CodePoint < 0x800){
			lpEncodedChar[0] = (unsigned char)(((CodePoint >> 6) | 0xC0));
			lpEncodedChar[1] = (unsigned char)((CodePoint & 0x3F) | 0x80);
			return 2;
		}
		else if (CodePoint < 0x10000){
			lpEncodedChar[0] = (unsigned char)((CodePoint >> 12) | 0xE0);
			lpEncodedChar[1] = (unsigned char)(((CodePoint >> 6) & 0x3F) | 0x80);
			lpEncodedChar[2] = (unsigned char)((CodePoint & 0x3F) | 0x80);
			return 3;
		}
		else{
			lpEncodedChar[0] = (unsigned char)((CodePoint >> 18) | 0xF0);
			lpEncodedChar[1] = (unsigned char)(((CodePoint >> 12) & 0x3F) | 0x80);
			lpEncodedChar[2] = (unsigned char)(((CodePoint >> 6) & 0x3F) | 0x80);
			lpEncodedChar[3] = (unsigned char)((CodePoint & 0x3F) | 0x80);
			return 4;
		}
	}
}