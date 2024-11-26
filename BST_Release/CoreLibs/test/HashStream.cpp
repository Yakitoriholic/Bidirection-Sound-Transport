#include "lGeneral.hpp"
#include <cstdio>
#include <cmath>
#include <ctime>

#include "lCrypt.hpp"

#if defined LIBENV_OS_LINUX
#include <curses.h>
#else
#include <conio.h>
#endif

extern void Test_HashStream(){
	char TestStr[] = "The quick brown fox jumps over the lazy dog";
	printf("Data: %s\n", TestStr);

	UINT4b CRC32;
	nsCrypt::HashStream_CRC32 MyStream_CRC32;
	for (UBINT i = 0; i < strlen((const char *)TestStr); i++)MyStream_CRC32 << TestStr[i];
	CRC32 = MyStream_CRC32.EndWrite();
	printf("CRC32: %08X\n", CRC32);
	// 414fa339

	UINT4b MD5Value[4];
	nsCrypt::HashStream_MD5 MyStream_MD5;
	for (UBINT i = 0; i < strlen((const char *)TestStr); i++)MyStream_MD5 << TestStr[i];
	MyStream_MD5.EndWrite(MD5Value);
	printf("MD5: %08X%08X%08X%08X\n", MD5Value[0], MD5Value[1], MD5Value[2], MD5Value[3]);
	// 9e107d9d372bb6826bd81d3542a419d6

	UINT4b SHA1Value[5];
	nsCrypt::HashStream_SHA1 MyStream_SHA1;
	for (UBINT i = 0; i < strlen((const char *)TestStr); i++)MyStream_SHA1 << TestStr[i];
	MyStream_SHA1.EndWrite(SHA1Value);
	printf("SHA-1: %08X%08X%08X%08X%08X\n", SHA1Value[0], SHA1Value[1], SHA1Value[2], SHA1Value[3], SHA1Value[4]);
	// 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12

	UINT4b SHA256Value[8];
	nsCrypt::HashStream_SHA256 MyStream_SHA256;
	for (UBINT i = 0; i < strlen((const char *)TestStr); i++)MyStream_SHA256 << TestStr[i];
	MyStream_SHA256.EndWrite(SHA256Value);
	printf("SHA-256: %08X%08X%08X%08X%08X%08X%08X%08X\n", SHA256Value[0], SHA256Value[1], SHA256Value[2], SHA256Value[3], SHA256Value[4], SHA256Value[5], SHA256Value[6], SHA256Value[7]);
	// d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592
	
	UINT8b SHA512Value[8];
	nsCrypt::HashStream_SHA512 MyStream_SHA512;
	for (UBINT i = 0; i < strlen((const char *)TestStr); i++)MyStream_SHA512 << TestStr[i];
	MyStream_SHA512.EndWrite(SHA512Value);
	printf("SHA-512:\n%016llX%016llX%016llX%016llX\n%016llX%016llX%016llX%016llX\n", SHA512Value[0], SHA512Value[1], SHA512Value[2], SHA512Value[3], SHA512Value[4], SHA512Value[5], SHA512Value[6], SHA512Value[7]);
	// 07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb64
	// 2e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6

	_getch();
}