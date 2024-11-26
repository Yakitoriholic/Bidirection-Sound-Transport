/* Description:Classes for WAV audio format encoding/decoding.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_FORMAT_WAV
#define LIB_FORMAT_WAV

#include "lFormat_Audio.hpp"

namespace nsFormat{
	struct InfoBlk_RIFF_Chunk{
		UINT4b FourCC;
		UINT4b ChunkSize; // total chunk size - sizeof(InfoBlk_RIFF_Chunk)
	};
	struct InfoBlk_RIFF_List{
		UINT4b Dword_List; //should always be 0x46464952 -- 'RIFF' or 0x5453494C -- 'LIST'.
		UINT4b ListSize; // total list size - 8
		UINT4b FourCC;
	};

	struct RIFFParser{
		static const UBINT StackSize = 8;

		nsBasic::Stream_RS<unsigned char> *lpStreamSrc;
		UINT8b StreamPtr;
		UINT8b RIFFStack_ChunkAddr[StackSize];
		UINT4b RIFFStack_ChunkSize[StackSize];
		UINT4b RIFFStack_FourCC[StackSize];
		UBINT RIFFStackPtr;
		bool IsLeaf;

		inline void Initialize(nsBasic::Stream_RS<unsigned char> *lpStreamSrc);
		inline void Initialize(nsBasic::Stream_RS<unsigned char> *lpStreamSrc, UINT8b Position);
		bool EnterChunk();
		bool LeaveChunk();
		UINT4b ReadBulk(void *Dest, UINT4b Count);
		inline UINT8b ChunkAddr();
		inline UINT4b ChunkSize();
		inline UINT4b FourCC();
	};

	class FormatReader_WAV:public AudioReader{
	private:
		static const AudioReader::Interface _I;

		UINT8b CurOffset;
		UINT8b WavOffset;
		bool Is24bit;

		static void _BeginRead(AudioReader * const ThisBase, nsBasic::Stream_RS<unsigned char> *lpStreamSrc);
		static UBINT _Read(AudioReader * const ThisBase, void *lpDest, UBINT SampleCnt);
		static void _EndRead(AudioReader * const ThisBase);
		static void _ResetStream(AudioReader * const ThisBase);
		static void _Delete(AudioReader * const ThisBase);
		static void _Destroy(AudioReader * const ThisBase);
	public:
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); }

		FormatReader_WAV();
	};

	const AudioReader::Interface FormatReader_WAV::_I{
		FormatReader_WAV::_BeginRead,
		FormatReader_WAV::_Read,
		FormatReader_WAV::_EndRead,
		FormatReader_WAV::_ResetStream,
		FormatReader_WAV::_Delete,
		FormatReader_WAV::_Destroy
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	void RIFFParser::Initialize(nsBasic::Stream_RS<unsigned char> *lpStreamSrc){
		this->lpStreamSrc = lpStreamSrc;
		this->StreamPtr = lpStreamSrc->GetPtr();
		this->RIFFStackPtr = 0;
		this->IsLeaf = false;
	}
	void RIFFParser::Initialize(nsBasic::Stream_RS<unsigned char> *lpStreamSrc, UINT8b Position){
		if (lpStreamSrc->Seek(nsBasic::StreamSeekType::FROM_BEGIN, Position)){
			this->lpStreamSrc = lpStreamSrc;
			this->StreamPtr = Position;
			this->RIFFStackPtr = 0;
			this->IsLeaf = false;
		}
		else throw std::exception("failed to initialize RIFF parser.");
	}
	bool RIFFParser::EnterChunk(){
		if (this->RIFFStackPtr >= this->StackSize || this->IsLeaf)return false;
		const unsigned char FourCC_RIFF[4] = { 'R', 'I', 'F', 'F' };
		const unsigned char FourCC_LIST[4] = { 'L', 'I', 'S', 'T' };

		UINT4b RIFFHead[2];
		if (this->lpStreamSrc->ReadBulk(RIFFHead, 2 * sizeof(UINT4b)) < 2 * sizeof(UINT4b))return false;
		if (RIFFHead[0] != *(UINT4b *)FourCC_RIFF && RIFFHead[0] != *(UINT4b *)FourCC_LIST){
			this->RIFFStack_ChunkAddr[this->RIFFStackPtr] = this->StreamPtr + 2 * sizeof(UINT4b);
			this->StreamPtr += 2 * sizeof(UINT4b);
			this->IsLeaf = true;
		}
		else{
			if (this->lpStreamSrc->ReadBulk(RIFFHead, sizeof(UINT4b)) < sizeof(UINT4b))return false;
			this->RIFFStack_ChunkAddr[this->RIFFStackPtr] = this->StreamPtr + 3 * sizeof(UINT4b);
			this->StreamPtr += 3 * sizeof(UINT4b);
			this->IsLeaf = false;
		}
		this->RIFFStack_ChunkSize[this->RIFFStackPtr] = RIFFHead[1];
		this->RIFFStack_FourCC[this->RIFFStackPtr] = RIFFHead[0];
		this->RIFFStackPtr++;
		return true;
	}
	bool RIFFParser::LeaveChunk(){
		if (0 == this->RIFFStackPtr)return false;
		UINT8b NextIndex = this->ChunkAddr() + this->ChunkSize();
		if (NextIndex > this->StreamPtr){
			if (!this->lpStreamSrc->Seek(nsBasic::StreamSeekType::FROM_BEGIN, NextIndex))return false;
			this->StreamPtr = NextIndex;
		}
		this->RIFFStackPtr--;
		this->IsLeaf = false;
		return true;
	}
	UINT4b RIFFParser::ReadBulk(void *Dest, UINT4b Count){
		if (0 == this->RIFFStackPtr)return 0;
		UINT8b MaxCount = (UINT8b)this->ChunkSize() - this->StreamPtr - this->ChunkAddr();
		if (MaxCount < (UINT8b)Count)Count = (UINT4b)MaxCount;
		if (Count > 0){
			UINT8b BytesReturn = this->lpStreamSrc->ReadBulk(Dest, Count);
			this->StreamPtr += BytesReturn;
			return (UINT4b)BytesReturn;
		}
		else return 0;
	}
	inline UINT8b RIFFParser::ChunkAddr(){ return this->RIFFStack_ChunkAddr[this->RIFFStackPtr - 1]; }
	inline UINT4b RIFFParser::ChunkSize(){ return this->RIFFStack_ChunkSize[this->RIFFStackPtr - 1]; }
	inline UINT4b RIFFParser::FourCC(){ return this->RIFFStack_FourCC[this->RIFFStackPtr - 1]; }

	FormatReader_WAV::FormatReader_WAV():AudioReader(){
		this->lpInterface = const_cast<AudioReader::Interface *>(&this->_I);
	}
	void FormatReader_WAV::_BeginRead(AudioReader * const ThisBase, nsBasic::Stream_RS<unsigned char> *lpStreamSrc){
		FormatReader_WAV *This = static_cast<FormatReader_WAV *>(ThisBase);

		RIFFParser WavRIFFParser;
		const unsigned char FourCC_WAVE[4] = { 'W', 'A', 'V', 'E' };
		const unsigned char FourCC_fmt[4] = { 'f', 'm', 't', ' ' };
		const unsigned char FourCC_data[4] = { 'd', 'a', 't', 'a' };

#pragma pack(push, 2)
		struct WAVChunk_fmt{
			UINT2b FormatTag;
			UINT2b Channels;
			UINT4b SamplePerSec;
			UINT4b AvgBytesPerSec;
			UINT2b BlockAlign;
			UINT2b BitsPerSample;
		} CurWavfmt;
#pragma pack(pop)

		This->lpStreamSrc = lpStreamSrc;
		try{
			WavRIFFParser.Initialize(lpStreamSrc);
			if (!WavRIFFParser.EnterChunk() || WavRIFFParser.IsLeaf || *(UINT4b *)FourCC_WAVE != WavRIFFParser.FourCC())throw std::exception("Invalid WAV format.");
			if (!WavRIFFParser.EnterChunk() || !WavRIFFParser.IsLeaf || *(UINT4b *)FourCC_fmt != WavRIFFParser.FourCC())throw std::exception("Invalid WAV format.");
			if (WavRIFFParser.ChunkSize() < sizeof(WAVChunk_fmt))throw std::exception("Invalid WAV format.");
			if (WavRIFFParser.ReadBulk(&CurWavfmt, sizeof(WAVChunk_fmt)) < sizeof(WAVChunk_fmt))throw std::exception("Invalid WAV format.");

			if (CurWavfmt.FormatTag != 1 || (CurWavfmt.BitsPerSample / 8) > 4 || 0 != CurWavfmt.BitsPerSample % 8)throw std::exception("Unsupported WAV format.");
			//Only 8-bit ~ 32-bit simple PCM formats are supported.

			if (!WavRIFFParser.LeaveChunk())throw std::exception("Invalid WAV format.");

			while (true){
				if (!WavRIFFParser.EnterChunk())throw std::exception("Invalid WAV format.");
				if (*(UINT4b *)FourCC_data == WavRIFFParser.FourCC()){
					This->Is24bit = (24 == CurWavfmt.BitsPerSample);
					if (8 == CurWavfmt.BitsPerSample){
						This->CurAudioInfo.BytesPerSample = 1;
						This->CurAudioInfo.SampleType = nsMath::TypeID_UINT;
					}
					else if (16 == CurWavfmt.BitsPerSample){
						This->CurAudioInfo.BytesPerSample = 2;
						This->CurAudioInfo.SampleType = nsMath::TypeID_INT;
					}
					else{
						This->CurAudioInfo.BytesPerSample = 4;
						This->CurAudioInfo.SampleType = nsMath::TypeID_INT;
					}

					if (This->Is24bit)This->CurAudioInfo.SampleCount = WavRIFFParser.ChunkSize() / (3 * CurWavfmt.Channels);
					else This->CurAudioInfo.SampleCount = WavRIFFParser.ChunkSize() / (This->CurAudioInfo.BytesPerSample * CurWavfmt.Channels);
					This->CurAudioInfo.Channels = CurWavfmt.Channels;
					This->CurAudioInfo.SamplePerSec = CurWavfmt.SamplePerSec;

					This->CurOffset = WavRIFFParser.StreamPtr;
					This->WavOffset = WavRIFFParser.StreamPtr;
					break;
				}
				else if (!WavRIFFParser.LeaveChunk())throw std::exception("Invalid WAV format.");
			}
		}
		catch (...){
			This->lpStreamSrc = nullptr;
			throw;
		}
	}
	UBINT FormatReader_WAV::_Read(AudioReader * const ThisBase, void *lpDest, UBINT SampleCnt){
		FormatReader_WAV *This = static_cast<FormatReader_WAV *>(ThisBase);
		if (true == This->Is24bit){
			UINT8b LengthRead = This->lpStreamSrc->ReadBulk(lpDest, SampleCnt * 3 * This->CurAudioInfo.Channels);
			UBINT SampleRead = LengthRead / (3 * This->CurAudioInfo.Channels);
			This->CurOffset += LengthRead;

			//24-bits widening
			nsAudio::Audio_Expand24BitTo32Bit(lpDest, lpDest, This->CurAudioInfo.Channels * SampleCnt);
			return SampleRead;
		}
		else{
			UINT8b LengthRead = This->lpStreamSrc->ReadBulk(lpDest, SampleCnt * This->CurAudioInfo.BytesPerSample * This->CurAudioInfo.Channels);
			UBINT SampleRead = LengthRead / (This->CurAudioInfo.BytesPerSample * This->CurAudioInfo.Channels);
			This->CurOffset += LengthRead;
			return SampleRead;
		}
	}
	void FormatReader_WAV::_EndRead(AudioReader * const ThisBase){}
	void FormatReader_WAV::_ResetStream(AudioReader * const ThisBase){
		FormatReader_WAV *This = static_cast<FormatReader_WAV *>(ThisBase);
		if (!This->lpStreamSrc->Seek(nsBasic::StreamSeekType::FROM_BEGIN, This->WavOffset))throw std::exception("Failed to reset the WAV audio stream.");
	}
	void FormatReader_WAV::_Delete(AudioReader * const ThisBase){ delete static_cast<FormatReader_WAV *>(ThisBase); }
	void FormatReader_WAV::_Destroy(AudioReader * const ThisBase){}
}
#endif