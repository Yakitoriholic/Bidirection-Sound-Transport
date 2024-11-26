/* Description:Functions for Bitmaps.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_FORMAT_AUDIO
#define LIB_FORMAT_AUDIO

#include "lFormat.hpp"
#include "lAudio.hpp"

namespace nsFormat{
	struct AudioInfo{
		UINT8b SampleCount;
		UINT4b Channels;
		UINT4b SamplePerSec;
		UINT4b BytesPerSample;
		UINT4b SampleType;

		static const UINT8b SAMPLE_COUNT_UNKNOWN = (UINT8b)-1;

		static const UBINT Count_TypeID = nsMath::TypeID_UINT_UBound + nsMath::TypeID_INT_UBound + nsMath::TypeID_FLOAT_UBound - nsMath::TypeID_FLOAT_LBound + 3;

		UBINT GetTypeID();
	};

	class AudioReader{
	protected:
		typedef AudioReader this_type;

		struct Interface{
			void(*_BeginRead)(this_type * const, nsBasic::Stream_RS<unsigned char> *);
			UBINT(*_Read)(this_type * const, void *, UBINT);
			void(*_EndRead)(this_type * const);
			void(*_ResetStream)(this_type * const);
			void(*_Delete)(this_type * const);
			void(*_Destroy)(this_type * const);
		};

		nsBasic::Stream_RS<unsigned char> *lpStreamSrc;
		Interface *lpInterface;

		void *ConvertBuffer;
		const UBINT ConvertBufferLen = 0x40000;

		UBINT TypeID;
		UBINT BytesPerBlock;

		template < typename T > UBINT Read_Linear(T *lpDest, UBINT SampleCnt);
		template < typename T > UBINT Read_Linear_Mono(T *lpDest, UBINT SampleCnt);

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); }

		inline void Destroy();
	public:
		// read only
		AudioInfo CurAudioInfo;
		UINT8b CurSamplePos;

		// read / write
		bool Loop;

		AudioReader();
		inline void BeginRead(nsBasic::Stream_RS<unsigned char> *lpStreamSrc);
		template < typename T > UBINT Read(T *lpDest, UBINT SampleCnt);
		template < typename T > UBINT Read_Mono(T *lpDest, UBINT SampleCnt);
		inline void EndRead();
		inline void ResetStream();
		inline void Delete();
		inline ~AudioReader();
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	UBINT AudioInfo::GetTypeID(){
		UBINT RetValue = nsMath::TypeID_INVALID;
		if (0 != this->BytesPerSample && nsMath::is2power(this->BytesPerSample)){
			if (nsMath::TypeID_UINT == this->SampleType){
				RetValue = nsMath::log2intsim(this->BytesPerSample) - 1;
				if (RetValue > nsMath::TypeID_UINT_UBound)return nsMath::TypeID_INVALID;
				RetValue += nsMath::TypeID_UINT;
			}
			else if (nsMath::TypeID_INT == this->SampleType){
				RetValue = nsMath::log2intsim(this->BytesPerSample) - 1;
				if (RetValue > nsMath::TypeID_INT_UBound)return nsMath::TypeID_INVALID;
				RetValue += nsMath::TypeID_INT;
			}
			else if (nsMath::TypeID_FLOAT == this->SampleType){
				RetValue = nsMath::log2intsim(this->BytesPerSample) - 1;
				if (RetValue > nsMath::TypeID_FLOAT_UBound || RetValue < nsMath::TypeID_FLOAT_LBound)return nsMath::TypeID_INVALID;
				RetValue += nsMath::TypeID_FLOAT;
			}
		}
		return RetValue;
	}

	AudioReader::AudioReader(){
		this->lpStreamSrc = nullptr;
		this->ConvertBuffer = nullptr;
		this->Loop = false;
	}

	void AudioReader::BeginRead(nsBasic::Stream_RS<unsigned char> *lpStreamSrc){
		if (nullptr != this->lpStreamSrc)this->lpInterface->_EndRead(this);
		this->lpInterface->_BeginRead(this, lpStreamSrc);
		this->TypeID = this->CurAudioInfo.GetTypeID();
		if (nsMath::TypeID_INVALID == this->TypeID)throw std::exception("Invalid sample format.");
		this->BytesPerBlock = this->CurAudioInfo.BytesPerSample*this->CurAudioInfo.Channels;
		CurSamplePos = 0;
	}
	template < typename T > UBINT AudioReader::Read_Linear(T *lpDest, UBINT SampleCnt){
		typedef void (*ConvertFunc)(void *, void *, UBINT);

		ConvertFunc JumpTable[AudioInfo::Count_TypeID] = {
			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_UINT + 0>::Type>::Func,
			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_UINT + 1>::Type>::Func,
			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_UINT + 2>::Type>::Func,
			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_UINT + 3>::Type>::Func,

			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_INT + 0>::Type>::Func,
			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_INT + 1>::Type>::Func,
			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_INT + 2>::Type>::Func,
			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_INT + 3>::Type>::Func,

			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_FLOAT + 2>::Type>::Func,
			nsAudio::_Audio_Convert_Bulk<T, LocalID_To_Type<nsMath::TypeID_FLOAT + 3>::Type>::Func
			//Anyone knows a more elegant way of implementing this?
		};

		if (this->TypeID == __typeid(T)){
			UBINT SampleRead = this->lpInterface->_Read(this, lpDest, SampleCnt);
			this->CurSamplePos += SampleRead;
			lpDest += (size_t)(SampleRead * BytesPerBlock);

			if (SampleRead < SampleCnt){
				//If the length of the audio is unknown, consider it as the end of the audio 
				if (AudioInfo::SAMPLE_COUNT_UNKNOWN == this->CurAudioInfo.SampleCount)this->CurAudioInfo.SampleCount = this->CurSamplePos;
				else memset(lpDest, 0, (size_t)((SampleCnt - SampleRead)*BytesPerBlock)); //fill with zero
			}
			return SampleRead;
		}
		else if (this->BytesPerBlock > ConvertBufferLen)return 0; //can't handle such situation
		else{
			if (nullptr == this->ConvertBuffer){
				//allocate a piece of buffer for conversion
				this->ConvertBuffer = nsBasic::GlobalMemAlloc(this->ConvertBufferLen);
				if (nullptr == this->ConvertBuffer)return 0;
			}
			UBINT JumpIndex = this->TypeID;
			if (JumpIndex >= nsMath::TypeID_FLOAT){
				JumpIndex -= nsMath::TypeID_FLOAT;
				JumpIndex += nsMath::TypeID_UINT_UBound + nsMath::TypeID_INT_UBound - nsMath::TypeID_FLOAT_LBound + 2;
			} 
			else if (JumpIndex >= nsMath::TypeID_INT)JumpIndex = JumpIndex - nsMath::TypeID_INT + nsMath::TypeID_UINT_UBound + 1;

			UBINT BufferedSampleCount = this->ConvertBufferLen / this->BytesPerBlock;
			UBINT SampleRead_Total = 0;

			while (SampleRead_Total < SampleCnt){
				UBINT SampleRead_Expected = BufferedSampleCount;
				if (SampleCnt - SampleRead_Total < SampleRead_Expected)SampleRead_Expected = SampleCnt - SampleRead_Total;
				UBINT SampleRead = this->lpInterface->_Read(this, this->ConvertBuffer, SampleRead_Expected);

				//conversion
				JumpTable[JumpIndex](lpDest, this->ConvertBuffer, SampleRead * this->CurAudioInfo.Channels);

				SampleRead_Total += SampleRead;
				lpDest += (size_t)(SampleRead * this->CurAudioInfo.Channels);
				if (SampleRead < SampleRead_Expected)break;
			}
			
			this->CurSamplePos += SampleRead_Total;
			if (SampleRead_Total < SampleCnt){
				//If the length of the audio is unknown, consider it as the end of the audio 
				if (AudioInfo::SAMPLE_COUNT_UNKNOWN == this->CurAudioInfo.SampleCount)this->CurAudioInfo.SampleCount = this->CurSamplePos;
				else nsMath::setzero_simd_unaligned(lpDest, ((SampleCnt - SampleRead_Total) * this->CurAudioInfo.Channels)); //fill with zero
			}
			return SampleRead_Total;
		}
	}
	template < typename T > UBINT AudioReader::Read_Linear_Mono(T *lpDest, UBINT SampleCnt){
		typedef void(*ConvertFunc)(void *, void *, UBINT, UBINT);

		ConvertFunc JumpTable[AudioInfo::Count_TypeID] = {
			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_UINT + 0>::Type>,
			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_UINT + 1>::Type>,
			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_UINT + 2>::Type>,
			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_UINT + 3>::Type>,

			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_INT + 0>::Type>,
			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_INT + 1>::Type>,
			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_INT + 2>::Type>,
			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_INT + 3>::Type>,

			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_FLOAT + 2>::Type>,
			nsAudio::_Audio_BulkChannelMerge<T, LocalID_To_Type<nsMath::TypeID_FLOAT + 3>::Type>
			//Anyone knows a more elegant way of implementing this?
		};

		if (this->BytesPerBlock > ConvertBufferLen)return 0; //can't handle such situation
		else{
			if (nullptr == this->ConvertBuffer){
				//allocate a piece of buffer for conversion
				this->ConvertBuffer = nsBasic::GlobalMemAlloc(this->ConvertBufferLen);
				if (nullptr == this->ConvertBuffer)return 0;
			}
			UBINT JumpIndex = this->TypeID;
			if (JumpIndex >= nsMath::TypeID_FLOAT){
				JumpIndex -= nsMath::TypeID_FLOAT;
				JumpIndex += nsMath::TypeID_UINT_UBound + nsMath::TypeID_INT_UBound - nsMath::TypeID_FLOAT_LBound + 2;
			}
			else if (JumpIndex >= nsMath::TypeID_INT)JumpIndex = JumpIndex - nsMath::TypeID_INT + nsMath::TypeID_UINT_UBound + 1;

			UBINT BufferedSampleCount = this->ConvertBufferLen / this->BytesPerBlock;
			UBINT SampleRead_Total = 0;

			while (SampleRead_Total < SampleCnt){
				UBINT SampleRead_Expected = BufferedSampleCount;
				if (SampleCnt - SampleRead_Total < SampleRead_Expected)SampleRead_Expected = SampleCnt - SampleRead_Total;
				UBINT SampleRead = this->lpInterface->_Read(this, this->ConvertBuffer, SampleRead_Expected);

				//conversion
				JumpTable[JumpIndex](lpDest, this->ConvertBuffer, this->CurAudioInfo.Channels, SampleRead);

				SampleRead_Total += SampleRead;
				lpDest += (size_t)SampleRead;
				if (SampleRead < SampleRead_Expected)break;
			}

			this->CurSamplePos += SampleRead_Total;
			if (SampleRead_Total < SampleCnt){
				//If the length of the audio is unknown, consider it as the end of the audio 
				if (AudioInfo::SAMPLE_COUNT_UNKNOWN == this->CurAudioInfo.SampleCount)this->CurAudioInfo.SampleCount = this->CurSamplePos;
				else nsMath::setzero_simd_unaligned(lpDest, SampleCnt - SampleRead_Total); //fill with zero
			}
			return SampleRead_Total;
		}
	}
	template < typename T > UBINT AudioReader::Read(T *lpDest, UBINT SampleCnt){
		if (nullptr != this->lpStreamSrc && SampleCnt > 0){
			if (this->Loop){
				if (AudioInfo::SAMPLE_COUNT_UNKNOWN == this->CurAudioInfo.SampleCount){
					UBINT SampleRead = this->Read_Linear(lpDest, SampleCnt);
					if (SampleRead < SampleCnt){
						//we have reached the end of the stream.
						this->ResetStream();
						T *lpDest_Tmp = lpDest + SampleRead * this->CurAudioInfo.Channels;
						if ((UINT8b)SampleCnt <= this->CurAudioInfo.SampleCount)SampleRead += this->Read_Linear(lpDest_Tmp, SampleCnt - SampleRead);
						else{
							SampleRead += this->Read_Linear(lpDest_Tmp, (UBINT)this->CurAudioInfo.SampleCount - SampleRead);
							lpDest_Tmp = lpDest + SampleRead * this->CurAudioInfo.Channels;

							SampleCnt -= SampleRead;
							SampleRead += SampleCnt;
							for (; SampleCnt > (UBINT)this->CurAudioInfo.SampleCount; SampleCnt -= (UBINT)this->CurAudioInfo.SampleCount){
								memcpy(lpDest_Tmp, lpDest, (UBINT)this->CurAudioInfo.SampleCount * this->BytesPerBlock);
								lpDest_Tmp += (UBINT)this->CurAudioInfo.SampleCount * this->CurAudioInfo.Channels;
							}
							memcpy(lpDest_Tmp, lpDest, SampleCnt * this->BytesPerBlock);
						}
					}
					return SampleRead;
				}
				else{
					//the audio length is known
					if ((UINT8b)SampleCnt < this->CurAudioInfo.SampleCount - this->CurSamplePos)return this->Read_Linear(lpDest, SampleCnt);
					else{
						UBINT SampleRead = 0;
						if (this->CurAudioInfo.SampleCount != this->CurSamplePos)SampleRead += this->Read_Linear(lpDest, (UBINT)(this->CurAudioInfo.SampleCount - this->CurSamplePos));
						this->ResetStream();
						T *lpDest_Tmp = lpDest + SampleRead * this->CurAudioInfo.Channels;
						if ((UINT8b)SampleCnt <= this->CurAudioInfo.SampleCount)SampleRead += this->Read_Linear(lpDest_Tmp, SampleCnt - SampleRead);
						else{
							SampleRead += this->Read_Linear(lpDest_Tmp, (UBINT)this->CurAudioInfo.SampleCount - SampleRead);
							lpDest_Tmp = lpDest + SampleRead * this->CurAudioInfo.Channels;

							SampleCnt -= SampleRead;
							SampleRead += SampleCnt;
							for (; SampleCnt > (UBINT)this->CurAudioInfo.SampleCount; SampleCnt -= (UBINT)this->CurAudioInfo.SampleCount){
								memcpy(lpDest_Tmp, lpDest, (UBINT)this->CurAudioInfo.SampleCount * this->BytesPerBlock);
								lpDest_Tmp += (UBINT)this->CurAudioInfo.SampleCount * this->CurAudioInfo.Channels;
							}
							memcpy(lpDest_Tmp, lpDest, SampleCnt * this->BytesPerBlock);
						}
						return SampleRead;
					}
				}
			}
			else{
				//If the audio length is known, clamp [SampleCnt] so that the pointer won't pass the end.
				if (AudioInfo::SAMPLE_COUNT_UNKNOWN != this->CurAudioInfo.SampleCount && (UINT8b)SampleCnt > this->CurAudioInfo.SampleCount - this->CurSamplePos)SampleCnt = (UBINT)(this->CurAudioInfo.SampleCount - this->CurSamplePos);
				return this->Read_Linear(lpDest, SampleCnt);
			}
		}
		else return 0;
	}
	template < typename T > UBINT AudioReader::Read_Mono(T *lpDest, UBINT SampleCnt){
		if (1 == this->CurAudioInfo.Channels)return this->Read(lpDest, SampleCnt);
		else if (nullptr != this->lpStreamSrc && SampleCnt > 0){
			if (this->Loop){
				if (AudioInfo::SAMPLE_COUNT_UNKNOWN == this->CurAudioInfo.SampleCount){
					UBINT SampleRead = this->Read_Linear_Mono(lpDest, SampleCnt);
					if (SampleRead < SampleCnt){
						//we have reached the end of the stream.
						this->ResetStream();
						T *lpDest_Tmp = lpDest + SampleRead;
						if ((UINT8b)SampleCnt <= this->CurAudioInfo.SampleCount)SampleRead += this->Read_Linear_Mono(lpDest_Tmp, SampleCnt - SampleRead);
						else{
							SampleRead += this->Read_Linear_Mono(lpDest_Tmp, (UBINT)this->CurAudioInfo.SampleCount - SampleRead);
							lpDest_Tmp = lpDest + SampleRead;

							SampleCnt -= SampleRead;
							SampleRead += SampleCnt;
							for (; SampleCnt > (UBINT)this->CurAudioInfo.SampleCount; SampleCnt -= (UBINT)this->CurAudioInfo.SampleCount){
								memcpy(lpDest_Tmp, lpDest, (UBINT)this->CurAudioInfo.SampleCount * this->CurAudioInfo.BytesPerSample);
								lpDest_Tmp += (UBINT)this->CurAudioInfo.SampleCount * this->CurAudioInfo.Channels;
							}
							memcpy(lpDest_Tmp, lpDest, SampleCnt * this->CurAudioInfo.BytesPerSample);
						}
					}
					return SampleRead;
				}
				else{
					//the audio length is known
					if ((UINT8b)SampleCnt < this->CurAudioInfo.SampleCount - this->CurSamplePos)return this->Read_Linear_Mono(lpDest, SampleCnt);
					else{
						UBINT SampleRead = 0;
						if (this->CurAudioInfo.SampleCount != this->CurSamplePos)SampleRead += this->Read_Linear_Mono(lpDest, (UBINT)(this->CurAudioInfo.SampleCount - this->CurSamplePos));
						this->ResetStream();
						T *lpDest_Tmp = lpDest + SampleRead;
						if ((UINT8b)SampleCnt <= this->CurAudioInfo.SampleCount)SampleRead += this->Read_Linear_Mono(lpDest_Tmp, SampleCnt - SampleRead);
						else{
							SampleRead += this->Read_Linear_Mono(lpDest_Tmp, (UBINT)this->CurAudioInfo.SampleCount - SampleRead);
							lpDest_Tmp = lpDest + SampleRead;

							SampleCnt -= SampleRead;
							SampleRead += SampleCnt;
							for (; SampleCnt > (UBINT)this->CurAudioInfo.SampleCount; SampleCnt -= (UBINT)this->CurAudioInfo.SampleCount){
								memcpy(lpDest_Tmp, lpDest, (UBINT)this->CurAudioInfo.SampleCount * this->CurAudioInfo.BytesPerSample);
								lpDest_Tmp += (UBINT)this->CurAudioInfo.SampleCount * this->CurAudioInfo.Channels;
							}
							memcpy(lpDest_Tmp, lpDest, SampleCnt * this->CurAudioInfo.BytesPerSample);
						}
						return SampleRead;
					}
				}
			}
			else{
				//If the audio length is known, clamp [SampleCnt] so that the pointer won't pass the end.
				if (AudioInfo::SAMPLE_COUNT_UNKNOWN != this->CurAudioInfo.SampleCount && (UINT8b)SampleCnt > this->CurAudioInfo.SampleCount - this->CurSamplePos)SampleCnt = (UBINT)(this->CurAudioInfo.SampleCount - this->CurSamplePos);
				return this->Read_Linear_Mono(lpDest, SampleCnt);
			}
		}
		else return 0;
	}
	void AudioReader::EndRead(){
		if (nullptr != this->lpStreamSrc)this->lpInterface->_EndRead(this);
	}
	void AudioReader::ResetStream(){
		if (nullptr != this->lpStreamSrc)this->lpInterface->_ResetStream(this);
		CurSamplePos = 0;
	}
	void AudioReader::Delete(){
		if (nullptr != this->lpStreamSrc)this->lpInterface->_EndRead(this);
		this->lpInterface->_Delete(this);
	}
	void AudioReader::Destroy(){
		if (nullptr != this->lpStreamSrc)this->lpInterface->_EndRead(this);
		this->lpInterface->_Destroy(this);
	}
	AudioReader::~AudioReader(){
		if (nullptr != this->ConvertBuffer)nsBasic::MemFree(this->ConvertBuffer, this->ConvertBufferLen);
		this->Destroy();
	}
}
#endif