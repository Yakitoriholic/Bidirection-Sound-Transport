/* Description:Functions for Bitmaps.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_FORMAT
#define LIB_FORMAT

#include "lGeneral.hpp"
#include "lFile.hpp"

namespace nsFormat{

	struct InfoBlk_BMP_Header{
		UINT2b bfType;
		UINT4b bfSize;
		UINT2b bfReserved1;
		UINT2b bfReserved2;
		UINT4b bfOffBits;
	};
	struct InfoBlk_BMP_CoreInfo{
		UINT4b biSize;
		UINT4b biWidth;
		UINT4b biHeight;
		UINT2b biPlanes;
		UINT2b biBitCount;
		UINT4b biCompression;
		UINT4b biSizeImage;
		UINT4b biXPelsPerMeter;
		UINT4b biYPelsPerMeter;
		UINT4b biClrUsed;
		UINT4b biClrImportant;
	};

	extern UBINT Serialize_BMP_Default(nsBasic::Stream_W<unsigned char> *lpStream, UBINT Width, UBINT Height, void *BitmapData);
	//Remind that the default BMP color order is B-G-R.
	extern UBINT Serialize_WAV_Info(nsBasic::Stream_W<unsigned char> *lpStream, UBINT Channels, UBINT BitPerSample, UBINT SamplePerSec, UBINT SampleCount);
	//This function only support PCM format. And I'm not going to support other formats in the future.
	//This function may produce unpredictable result when waveform data size (or data size per sec) is greater than 4GB.

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	extern UBINT Serialize_BMP_Default(nsBasic::Stream_W<unsigned char> *lpStream, UBINT Width, UBINT Height, void *BitmapData){
		//this function only support simple BMP format.
		unsigned char TmpHeader[14]="BM\0\0\0\0\0\0\0\0\0\0\0";
		unsigned char Blanks[3]="\0\0";
		InfoBlk_BMP_CoreInfo TmpInfo;

		*(DWORD *)&TmpHeader[2] = (DWORD)(Width*Height * 3 + (Width % 4)*Height + 14 + sizeof(TmpInfo));
		*(DWORD *)&TmpHeader[10] = (DWORD)(14 + sizeof(TmpInfo));

		TmpInfo.biSize=sizeof(TmpInfo);
		TmpInfo.biWidth=Width;
		TmpInfo.biHeight=Height;
		TmpInfo.biPlanes=1;
		TmpInfo.biBitCount=24;
		TmpInfo.biCompression=0; //Uncompressed
		TmpInfo.biSizeImage=Width*Height*3+(Width%4)*Height;
		TmpInfo.biXPelsPerMeter=0;
		TmpInfo.biYPelsPerMeter=0;
		TmpInfo.biClrUsed=0;
		TmpInfo.biClrImportant=0;

		if (14 > lpStream->WriteBulk((const unsigned char *)TmpHeader, 14))return 1;
		if (sizeof(TmpInfo) > lpStream->WriteBulk((const unsigned char *)&TmpInfo, sizeof(TmpInfo)))return 1;
		if (0 == Width % 4){
			if (Width*Height * 3>lpStream->WriteBulk((const unsigned char *)BitmapData, Width*Height * 3))return 1;
		}
		else{
			for (UBINT i = 0; i<Height; i++){
				if (Width * 3>lpStream->WriteBulk((const unsigned char *)BitmapData + i*Width * 3, Width * 3))return 1;
				if (Width % 4 > lpStream->WriteBulk((const unsigned char *)Blanks, Width % 4))return 1;
			}
		}
		return 0;
	}
	extern UBINT Serialize_WAV_Info(nsBasic::Stream_W<unsigned char> *lpStream,UBINT Channels,UBINT BitPerSample,UBINT SamplePerSec,UBINT SampleCount){
		DWORD WavInfo[11] = {0x46464952, //'RIFF'
			36, //FileLen-8
			0x45564157, //'WAVE'
			0x20746D66, //'fmt '
			0x10, //format chunk size
			1, //PCM Format + WORD Channels
			0, //SamplePerSec
			0, //AvgBytesPerSec
			0, //WORD BlockAlign + WORD BitPerSample
			0x61746164, //'data'
			0};//Wave Length
		if (SamplePerSec < 0xFFFF && BitPerSample < 0xFFFF){
			WavInfo[5] += Channels << 16;
			WavInfo[6] = SamplePerSec;
			WavInfo[7] = ((BitPerSample + 7) / 8)*SamplePerSec*Channels;
			WavInfo[8] = ((BitPerSample + 7) / 8)*Channels + (BitPerSample << 16);
			WavInfo[10] = ((BitPerSample + 7) / 8)*SampleCount*Channels;
			WavInfo[1] += WavInfo[10];
			if (44>lpStream->WriteBulk((const unsigned char *)WavInfo, 44))return 1; else return 0;
		}
		else return 1;
	}
}
#endif