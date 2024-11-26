#include "test/TestData.hpp"

#include "lFile.hpp"
#include "lText_Process.hpp"
#include <cstdio>
#include <cmath>
#include <ctime>

#if defined LIBENV_OS_LINUX
#include <curses.h>
#else
#include <conio.h>
#endif

extern void Test_Simple(){
	UBINT CurTime;
	UINT4b TmpInt1;
	nsFile::ReadFileStream RFS;
	nsFile::WriteFileStream WFS;
	nsFile::ReadStreamBuffer RSB;
	nsFile::WriteStreamBuffer WSB;

	printf("Press any key to start testing.\n");
	_getch();
	CurTime = (UBINT)time(nullptr);
	srand((unsigned int)CurTime);
	
	WFS.Open(L"UTF8_f", true);
	WSB.Destination = &WFS;
	for (int i = 0; i < 0x20; i++){
		if (false == WSB.Write(&TestData_Str_UTF8[i])){ printf("Failed to write bytes into file UTF8_f.Test stopped.\n"); _getch(); return; }
	}
	if (false == WSB.WriteBulk(&TestData_Str_UTF8[0x20], 0x2A)){ printf("Failed to write bytes into file UTF8_f.Test stopped.\n"); _getch(); return; }
	WSB.Flush();
	WFS.Close();
	printf("File UTF8_f created.\n");

	RFS.Open(L"UTF8_f");
	WFS.Open(L"UTF16L_f", true);
	RSB.Source = &RFS;
	WSB.Destination = &WFS;
	nsFile::DecoderStream Decoder1(&RSB,nsCharCoding::UTF8);
	nsFile::CoderStream Coder1(&WSB, nsCharCoding::UTF16L);
	while (!RSB.empty() || !RFS.isEOF()){
		if (false == Decoder1.Read(&TmpInt1))printf("Failed to decode.\n");
		else{
			printf("0x%8X",TmpInt1);
			if (false == Coder1.Write(&TmpInt1))printf(" Failed to write.\n"); else printf(" Written.\n");
		}
	}
	WSB.Flush();
	WFS.Close();
	RSB.clear();
	RFS.Close();
	printf("UTF-8->UTF-16L Conversion finished.\n");

	RFS.Open(L"UTF16L_f");
	WFS.Open(L"UTF16B_f", true);
	RSB.Source = &RFS;
	WSB.Destination = &WFS;
	nsFile::DecoderStream Decoder2(&RSB, nsCharCoding::UTF16L);
	nsFile::CoderStream Coder2(&WSB, nsCharCoding::UTF16B);
	while (!RSB.empty() || !RFS.isEOF()){
		if (false == Decoder2.Read(&TmpInt1))printf("Failed to decode.\n");
		else{
			printf("0x%8X",TmpInt1);
			if (false == Coder2.Write(&TmpInt1))printf(" Failed to write.\n"); else printf(" Written.\n");
		}
	}
	WSB.Flush();
	WFS.Close();
	RSB.clear();
	RFS.Close();
	printf("UTF-16L->UTF-16B Conversion finished.\n");

	RFS.Open(L"UTF16B_f");
	WFS.Open(L"UTF8_f", true);
	RSB.Source = &RFS;
	WSB.Destination = &WFS;
	nsFile::DecoderStream Decoder3(&RSB, nsCharCoding::UTF16B);
	nsFile::CoderStream Coder3(&WSB, nsCharCoding::UTF8);
	while (!RSB.empty() || !RFS.isEOF()){
		if (false == Decoder3.Read(&TmpInt1))printf("Failed to decode.\n");
		else{
			printf("0x%8X",TmpInt1);
			if (false == Coder3.Write(&TmpInt1))printf(" Failed to write.\n"); else printf(" Written.\n");
		}
	}
	WSB.Flush();
	WFS.Close();
	RSB.clear();
	RFS.Close();
	printf("UTF-16B->UTF-8 Conversion finished.\n");

	printf("done in %d seconds.\n", (UBINT)time(nullptr) - CurTime);
	_getch();
}

extern void Test_BulkReadWrite(){
	UBINT CurTime;
	UINT8b TmpValue;
	unsigned char TestData[0x3000];
	nsFile::WriteFileStream WFS(L"Test");
	nsFile::WriteStreamBuffer WSB;
	nsFile::ReadStreamBuffer RSB;
	nsFile::MemStream WBS(TestData, 0x3000);
	nsFile::MemStream RBS(TestData, 0x3000);
	nsFile::MemFile<unsigned char> MyFile;

	printf("Press any key to start testing.\n");
	_getch();
	CurTime=(UBINT)time(nullptr);
	WSB.Destination = &WFS;
	for (int i = 0; i<0x3000; i++)TestData[i] = TestData_Str_Pattern(i);
	printf("Write 2048 bytes to file Test.%lld bytes written.\n", TmpValue = WSB.WriteBulk(TestData, 0x800));
	_getch();
	printf("Write 8192 bytes to file Test.%lld bytes written.\n", TmpValue = WSB.WriteBulk(TestData + 0x800, 0x2000));
	_getch();
	printf("Write 2048 bytes to file Test.%lld bytes written.\n", TmpValue = WSB.WriteBulk(TestData + 0x2800, 0x800));
	_getch();
	WSB.Flush();
	WFS.Close();
	printf("File Test created.\n");
	memset(TestData, 0, 0x3000);

	nsFile::ReadFileStream RFS(L"Test");
	RSB.Source = &RFS;
	printf("Request 2048 bytes from file Test.%lld bytes received.\n", TmpValue = RSB.ReadBulk(TestData, 0x800));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	printf("Request 8192 bytes from file Test.%lld bytes received.\n", TmpValue = RSB.ReadBulk(TestData, 0x2000));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	printf("Request 2050 bytes from file Test.%lld bytes received.\n", TmpValue = RSB.ReadBulk(TestData, 0x802));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	RSB.clear();
	RFS.Close();
	printf("Write 2048 bytes to byte stream.%lld bytes written.\n", TmpValue = WBS.WriteBulk(TestData + 0x800, 0x800));
	printf("Request 2048 bytes from byte stream.%lld bytes received.\n", TmpValue = RBS.ReadBulk(TestData + 0x800, 0x800));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	for (int i = 0; i<0x3000; i++)TestData[i] = TestData_Str_Pattern(i);
	printf("Write 2048 bytes to memory file.%d bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData, 0x800));
	printf("Write 2047 bytes to memory file.%d bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x800, 0x7FF));
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)1, 0);
	printf("Write 6143 bytes to memory file.%d bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 1, 0x17FF));
	printf("Write 2047 bytes to memory file.%d bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x1800, 0x7FF));
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)1, 0);
	printf("Write 6143 bytes to memory file.%d bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 1, 0x17FF));
	printf("Write 1 bytes to memory file.%d bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x1800, 1));
	printf("Write 2047 bytes to memory file.%d bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x1801, 0x7FF));
	printf("Write 4096 bytes to memory file.%d bytes written.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_WRITEBULK, TestData + 0x2000, 0x1000));
	SendMsg(&MyFile, MSG_FILE_SETPTR, (INT8b)0, 0);
	printf("Request 2048 bytes from memory file.%d bytes received.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_READBULK, TestData, 0x800));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	printf("Request 8192 bytes from memory file.%d bytes received.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_READBULK, TestData, 0x2000));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	_getch();
	printf("Request 2050 bytes from memory file.%d bytes received.\n", TmpValue = SendMsg(&MyFile, MSG_STREAM_READBULK, TestData, 0x80B));
	for(UBINT i=0;i<TmpValue;i++)putchar(TestData[i]);
	printf("done in %d seconds.\n", (UBINT)time(nullptr) - CurTime);
	_getch();
}
extern void Test_Lexer(){
	char TestData[20]=".1\0-.1\0001.\0-1.\0003.14\0";
	nsFile::MemStream RBS(TestData, 20);
	nsFile::DecoderStream Decoder(&RBS.AsReadStream(), nsCharCoding::UTF8);
	nsFile::MemStream WBS(TestData, 20);
	
	nsText::Lexer MyLexer(&Decoder);
	double Result=0;
	UINT4b TmpChar;

	printf("Press any key to start testing.\n");
	_getch();
	MyLexer.LoadNextChar();
	MyLexer.ReadFloat(&Result);
	printf("%s -> %lf\n",TestData,Result);
	MyLexer.LoadNextChar();
	MyLexer.ReadFloat(&Result);
	printf("%s -> %lf\n",&TestData[3],Result);
	MyLexer.LoadNextChar();
	MyLexer.ReadFloat(&Result);
	printf("%s -> %lf\n",&TestData[7],Result);
	MyLexer.LoadNextChar();
	MyLexer.ReadFloat(&Result);
	printf("%s -> %lf\n",&TestData[10],Result);
	MyLexer.LoadNextChar();
	MyLexer.ReadFloat(&Result);
	printf("%s -> %lf\n",&TestData[14],Result);
	_getch();

	nsFile::CoderStream Coder(&WBS.AsWriteStream(), nsCharCoding::UTF8);

	WBS.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
	nsText::WriteUInt4b(&Coder, 0);
	TmpChar = '\0'; Coder.Write(&TmpChar);
	printf("\n%u -> %s\n",0,TestData);
	WBS.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
	nsText::WriteUInt4b(&Coder, 0xFFFFFFFF);
	TmpChar = '\0'; Coder.Write(&TmpChar);
	printf("%u -> %s\n",0xFFFFFFFF,TestData);
	WBS.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
	nsText::WriteDouble(&Coder, -0.0);
	TmpChar = '\0'; Coder.Write(&TmpChar);
	printf("%lf -> %s\n",0.0,TestData);
	WBS.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
	nsText::WriteDouble(&Coder, 5.0);
	TmpChar = '\0'; Coder.Write(&TmpChar);
	printf("%lf -> %s\n",5.0,TestData);
	WBS.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
	nsText::WriteDouble(&Coder, 0.0123456789);
	TmpChar = '\0'; Coder.Write(&TmpChar);
	printf("%lf -> %s\n",0.0123456789,TestData);
	WBS.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
	nsText::WriteDouble(&Coder, -6227020800.0);
	TmpChar = '\0'; Coder.Write(&TmpChar);
	printf("%lf -> %s\n", -6227020800.0, TestData, 20);
	_getch();
}
