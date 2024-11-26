/* Description:Classes for OGG audio format encoding/decoding.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_FORMAT_OGG
#define LIB_FORMAT_OGG

#include "lFormat_Audio.hpp"

#include "ogg/ogg.h"
#include "vorbis/codec.h"

#ifdef LIBENV_CPLR_VS
#ifdef _DEBUG
#pragma comment(lib,"VS_Debug/libogg.lib")
#pragma comment(lib,"VS_Debug/libvorbis.lib")
#else
#pragma comment(lib,"VS_Release/libogg.lib")
#pragma comment(lib,"VS_Release/libvorbis.lib")
#endif
#endif

namespace nsFormat{
	class FormatReader_OGG:public AudioReader{
	private:
		static const AudioReader::Interface _I;

		ogg_page CurPage;

		ogg_stream_state OggStream;
		ogg_packet CurPacket;

		bool IsEndOfStream;
		UINT8b PageOffset;
		UINT8b FirstPageOffset;

		vorbis_info CurVorbisInfo;
		vorbis_comment CurVorbisComment;

		vorbis_dsp_state CurDSPState;
		vorbis_block CurAudioBlock;

		bool ReadOggPage();
		bool ReadOggPacket_Header();
		bool ReadOggPacket_Data();

		static void _BeginRead(AudioReader * const ThisBase, nsBasic::Stream_RS<unsigned char> *lpStreamSrc);
		static UBINT _Read(AudioReader * const ThisBase, void *lpDest, UBINT SampleCnt);
		static void _EndRead(AudioReader * const ThisBase);
		static void _ResetStream(AudioReader * const ThisBase);
		static void _Delete(AudioReader * const ThisBase);
		static void _Destroy(AudioReader * const ThisBase);
	public:
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); }

		FormatReader_OGG();
	};

	const AudioReader::Interface FormatReader_OGG::_I{
		FormatReader_OGG::_BeginRead,
		FormatReader_OGG::_Read,
		FormatReader_OGG::_EndRead,
		FormatReader_OGG::_ResetStream,
		FormatReader_OGG::_Delete,
		FormatReader_OGG::_Destroy
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	FormatReader_OGG::FormatReader_OGG():AudioReader(){
		this->lpInterface = const_cast<AudioReader::Interface *>(&this->_I);

		this->CurPage.body = (unsigned char *)nsBasic::GlobalMemAlloc(255 * 255);
		if (nullptr == this->CurPage.body)throw new std::exception("Failed to create Ogg page buffer.");
		this->CurPage.header = (unsigned char *)nsBasic::GlobalMemAlloc(27 + 255);
		if (nullptr == this->CurPage.header){
			nsBasic::MemFree(this->CurPage.body, 255 * 255);
			throw new std::exception("Failed to create Ogg page buffer.");
		}
		if (-1 == ogg_stream_init(&this->OggStream, 0)){
			nsBasic::MemFree(this->CurPage.header, 27 + 255);
			nsBasic::MemFree(this->CurPage.body, 255 * 255);
			throw new std::exception("Cannot initialize Ogg stream.");
		}
		this->CurPage.header_len = 0;
		this->CurPage.body_len = 0;
	}
	bool FormatReader_OGG::ReadOggPage(){
		if (this->lpStreamSrc->ReadBulk(this->CurPage.header, 27) < 27)return false;

		unsigned char MagicNumber[4] = {'O','g','g','S'};
		if (*(UINT4b *)this->CurPage.header != *(UINT4b *)MagicNumber)return false; //Invalid Ogg page format
		if (0 != this->CurPage.header[4])return false; //Unsupported Ogg format version (the only supported version is version 0)

		this->CurPage.header_len = 27 + this->CurPage.header[26];
		this->CurPage.body_len = 0;
		if (this->CurPage.header[26] > 0){
			if (this->lpStreamSrc->ReadBulk(&this->CurPage.header[27], this->CurPage.header[26]) < this->CurPage.header[26]){
				this->CurPage.header_len = 0;
				return false;
			}
			for (UBINT i = 27; i < (UBINT)this->CurPage.header[26] + 27; i++)this->CurPage.body_len += this->CurPage.header[i];
		}
		if (this->CurPage.body_len > 0){
			if (this->lpStreamSrc->ReadBulk(this->CurPage.body, this->CurPage.body_len) < this->CurPage.body_len){
				this->CurPage.header_len = 0;
				this->CurPage.body_len = 0;
				return false;
			}
		}
		this->PageOffset += this->CurPage.header_len + this->CurPage.body_len;
		return true;
	}
	bool FormatReader_OGG::ReadOggPacket_Header(){
		int Status = ogg_stream_packetout(&this->OggStream, &this->CurPacket);
		while (0 == Status){
			if (this->IsEndOfStream || !this->ReadOggPage())return false;
			this->OggStream.serialno = ogg_page_serialno(&this->CurPage);
			this->IsEndOfStream = ogg_page_eos(&this->CurPage) > 0;
			if (-1 == ogg_stream_pagein(&this->OggStream, &this->CurPage))return false;
			Status = ogg_stream_packetout(&this->OggStream, &this->CurPacket);
		};
		if (1 == Status)return true; else return false;
	}
	bool FormatReader_OGG::ReadOggPacket_Data(){
		int Status = ogg_stream_packetout(&this->OggStream, &this->CurPacket);
		while (0 == Status){
			if (this->IsEndOfStream || !this->ReadOggPage())return false;
			if (this->OggStream.serialno == ogg_page_serialno(&this->CurPage)){
				this->IsEndOfStream = ogg_page_eos(&this->CurPage) > 0;
				if (-1 == ogg_stream_pagein(&this->OggStream, &this->CurPage))return false;
				Status = ogg_stream_packetout(&this->OggStream, &this->CurPacket);
			}
		};
		if (1 == Status)return true; else return false;
	}
	void FormatReader_OGG::_BeginRead(AudioReader * const ThisBase, nsBasic::Stream_RS<unsigned char> *lpStreamSrc){
		FormatReader_OGG *This = static_cast<FormatReader_OGG *>(ThisBase);

		ogg_page CurPage = { nullptr, 0, nullptr, 0 };

		This->lpStreamSrc = lpStreamSrc;
		This->IsEndOfStream = false;
		try{
			This->PageOffset = This->lpStreamSrc->GetPtr();

			bool HeaderFound = false;
			UINT8b TmpOffset = This->FirstPageOffset;
			while (This->ReadOggPacket_Header()){
				HeaderFound = (1 == vorbis_synthesis_idheader(&This->CurPacket));
				if (HeaderFound)break;
			}
			if (!HeaderFound){
				ogg_stream_reset(&This->OggStream);
				throw std::exception("Cannot find the audio header.");
			}

			vorbis_info_init(&This->CurVorbisInfo);
			vorbis_comment_init(&This->CurVorbisComment);

			try{
				if (0 != vorbis_synthesis_headerin(&This->CurVorbisInfo, &This->CurVorbisComment, &This->CurPacket))throw std::exception("Invalid Ogg vorbis format.");

				if (!This->ReadOggPacket_Header() || 0 != vorbis_synthesis_headerin(&This->CurVorbisInfo, &This->CurVorbisComment, &This->CurPacket))throw std::exception("Invalid Ogg vorbis format.");
				if (!This->ReadOggPacket_Header() || 0 != vorbis_synthesis_headerin(&This->CurVorbisInfo, &This->CurVorbisComment, &This->CurPacket))throw std::exception("Invalid Ogg vorbis format.");

				if (0 != vorbis_synthesis_init(&This->CurDSPState, &This->CurVorbisInfo))throw std::exception("Cannot initialize vorbis decoder.");
			}
			catch (...){
				vorbis_comment_clear(&This->CurVorbisComment);
				vorbis_info_clear(&This->CurVorbisInfo);
				ogg_stream_reset(&This->OggStream);
				throw;
			}

			if (0 != vorbis_block_init(&This->CurDSPState, &This->CurAudioBlock)){
				vorbis_dsp_clear(&This->CurDSPState);
				vorbis_comment_clear(&This->CurVorbisComment);
				vorbis_info_clear(&This->CurVorbisInfo);
				ogg_stream_reset(&This->OggStream);
				throw std::exception("Cannot initialize audio block.");
			}
		}
		catch (...){
			This->lpStreamSrc = nullptr;
			throw;
		}

		This->CurAudioInfo.SampleCount = AudioInfo::SAMPLE_COUNT_UNKNOWN;
		This->CurAudioInfo.Channels = This->CurVorbisInfo.channels;
		This->CurAudioInfo.SamplePerSec = This->CurVorbisInfo.rate;
		This->CurAudioInfo.BytesPerSample = sizeof(float);
		This->CurAudioInfo.SampleType = nsMath::TypeID_FLOAT;

		This->FirstPageOffset = This->PageOffset;
	}
	UBINT FormatReader_OGG::_Read(AudioReader * const ThisBase, void *lpDest, UBINT SampleCnt){
		FormatReader_OGG *This = static_cast<FormatReader_OGG *>(ThisBase);
		float *lpDest_Float = (float *)lpDest;
		float **SamplePtr;
		UBINT SampleRead_Total = 0;

		while (SampleRead_Total < SampleCnt){
			UBINT SampleRead = vorbis_synthesis_pcmout(&This->CurDSPState, &SamplePtr);
			
			if (0 == SampleRead){
				if (!This->ReadOggPacket_Header())break;
				if (0 != vorbis_synthesis(&This->CurAudioBlock, &This->CurPacket))break;
				if (0 != vorbis_synthesis_blockin(&This->CurDSPState, &This->CurAudioBlock))break;
			}
			else{
				if (SampleCnt - SampleRead_Total < SampleRead)SampleRead = SampleCnt - SampleRead_Total;
				for (UBINT i = 0; i < This->CurAudioInfo.Channels; i++){
					for (UBINT j = 0; j < SampleRead; j++){
						lpDest_Float[j*This->CurAudioInfo.Channels + i] = SamplePtr[i][j];
					}
				}
				vorbis_synthesis_read(&This->CurDSPState, (int)SampleRead);
				lpDest_Float += This->CurAudioInfo.Channels * SampleRead;
				SampleRead_Total += SampleRead;
			}
		}
		return SampleRead_Total;
	}
	void FormatReader_OGG::_EndRead(AudioReader * const ThisBase){
		FormatReader_OGG *This = static_cast<FormatReader_OGG *>(ThisBase);

		vorbis_block_clear(&This->CurAudioBlock);
		vorbis_dsp_clear(&This->CurDSPState);
		vorbis_comment_clear(&This->CurVorbisComment);
		vorbis_info_clear(&This->CurVorbisInfo);
		ogg_stream_reset(&This->OggStream);
		This->PageOffset = 0;
		This->FirstPageOffset = 0;
		This->lpStreamSrc = nullptr;
	}
	void FormatReader_OGG::_ResetStream(AudioReader * const ThisBase){
		FormatReader_OGG *This = static_cast<FormatReader_OGG *>(ThisBase);

		if (!This->lpStreamSrc->Seek(nsBasic::StreamSeekType::FROM_BEGIN, This->FirstPageOffset))throw std::exception("Failed to reset the OGG audio stream.");
		else{
			vorbis_synthesis_restart(&This->CurDSPState);
			ogg_stream_reset(&This->OggStream);
			This->PageOffset = This->FirstPageOffset;
			This->IsEndOfStream = false;
		}
	}
	void FormatReader_OGG::_Delete(AudioReader * const ThisBase){ delete static_cast<FormatReader_OGG *>(ThisBase); }
	void FormatReader_OGG::_Destroy(AudioReader * const ThisBase){
		FormatReader_OGG *This = static_cast<FormatReader_OGG *>(ThisBase);

		nsBasic::MemFree(This->CurPage.header, 27 + 255);
		nsBasic::MemFree(This->CurPage.body, 255 * 255);
		ogg_stream_clear(&This->OggStream);
	}
}
#endif