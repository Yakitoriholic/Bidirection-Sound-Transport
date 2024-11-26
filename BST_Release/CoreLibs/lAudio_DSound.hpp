/* Description: Audio classes using DirectSound as the backend.
 * Language:C++
 * Author:***
 *
 * You must enable multithreading and call nsBasic::CreateThreadExtObj() first to use the following functions.
 */

#ifndef LIB_AUDIO_DS
#define LIB_AUDIO_DS

#include "lGeneral.hpp"
#include "lAudio.hpp"
#include "lContainer.hpp"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#include "DSound.h"


namespace nsAudio{
	class OutputDevice_DS;
	class OutputStream_DS: public OutputStream{
	private:
		static const OutputStream::Interface _I;

		LPDIRECTSOUNDBUFFER SoundBuffer;
		LPDIRECTSOUNDNOTIFY8 SoundNotify;
		DSBPOSITIONNOTIFY StreamEvent[2];

		DWORD FrameLen;
		DWORD FrameTick;
		void *lpFrame;

		static void _Play(OutputStream * const ThisBase);
		static void _Pause(OutputStream * const ThisBase);
		static void _Stop(OutputStream * const ThisBase);
		static void * _WaitFrame(OutputStream * const ThisBase);
		static void _SubmitFrame(OutputStream * const ThisBase);

	public:
		friend class OutputDevice_DS;
		OutputStream_DS(); //this constructor should never be used by anyone except OutputDevice_DS.
	};
	const OutputStream::Interface OutputStream_DS::_I{
		OutputStream_DS::_Play,
		OutputStream_DS::_Pause,
		OutputStream_DS::_Stop,
		OutputStream_DS::_WaitFrame,
		OutputStream_DS::_SubmitFrame
	};

	class OutputDevice_DS: public OutputDevice{
	private:
		static const OutputDevice::Interface _I;

		static OutputStream * _CreateStream_Output(OutputDevice * const ThisBase, UBINT SampleTypeID, UBINT Channels, UBINT SamplePerFrame, UBINT SamplePerSec);
		static void _DestroyStream_Output(OutputDevice * const ThisBase, OutputStream * const lpStream);
		static void _Destroy(OutputDevice * const ThisBase);

		LPDIRECTSOUND lpDevice;

		nsContainer::List<OutputStream_DS> OutputStreamList;
	public:
		OutputDevice_DS();
	};
	const OutputDevice::Interface OutputDevice_DS::_I{
		OutputDevice_DS::_CreateStream_Output,
		OutputDevice_DS::_DestroyStream_Output,
		OutputDevice_DS::_Destroy
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	void OutputDevice_DS::_Destroy(OutputDevice * const ThisBase){
		OutputDevice_DS *This = static_cast<OutputDevice_DS *>(ThisBase);

		auto _It = This->OutputStreamList.begin();
		while (_It != This->OutputStreamList.end()){
			CloseHandle(_It->StreamEvent[0].hEventNotify);
			CloseHandle(_It->StreamEvent[1].hEventNotify);
			_It->SoundNotify->Release();
			_It->SoundBuffer->Release();
		}
		This->lpDevice->Release();
	}
	OutputDevice_DS::OutputDevice_DS(){
		if (DS_OK != DirectSoundCreate(nullptr, &this->lpDevice, nullptr)){
			//nsBasic::MemFree((void *)(this->SysInfo), sizeof(IDirectSound));
			throw new std::exception("failed to initiate DirectSound device.");
		}
		if (DS_OK != (this->lpDevice->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY))){
			this->lpDevice->Release();
			//nsBasic::MemFree((void *)(this->SysInfo), sizeof(IDirectSound));
			throw new std::exception("failed to initiate DirectSound device.");
		}
		this->lpInterface = const_cast<OutputDevice::Interface *>(&this->_I);
	}

	OutputStream *OutputDevice_DS::_CreateStream_Output(OutputDevice * const ThisBase, UBINT SampleTypeID, UBINT Channels, UBINT SamplePerFrame, UBINT SamplePerSec){
		OutputDevice_DS *This = static_cast<OutputDevice_DS *>(ThisBase);

		UBINT BytesPerSample;
		if (__typeid(UINT1b) == SampleTypeID)BytesPerSample = 1;
		else if (__typeid(INT2b) == SampleTypeID)BytesPerSample = 2;
		else return nullptr;

		This->OutputStreamList.emplace_back();
		OutputStream_DS *TmpStream = &This->OutputStreamList.back();

		DSBUFFERDESC TmpDescriptor;
		WAVEFORMATEX TmpWaveFormat;
		TmpWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
		TmpWaveFormat.nChannels = (WORD)Channels;
		TmpWaveFormat.nSamplesPerSec = (DWORD)SamplePerSec;
		TmpWaveFormat.nAvgBytesPerSec = (DWORD)(Channels*SamplePerSec*BytesPerSample);
		TmpWaveFormat.nBlockAlign = (WORD)(Channels*BytesPerSample);
		TmpWaveFormat.wBitsPerSample = (WORD)BytesPerSample * 8;
		TmpWaveFormat.cbSize = 0;
		TmpDescriptor.dwSize = sizeof(DSBUFFERDESC);
		TmpDescriptor.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY;
		TmpDescriptor.dwBufferBytes = (DWORD)(Channels*SamplePerFrame*BytesPerSample * 2);
		TmpDescriptor.dwReserved = 0;
		TmpDescriptor.lpwfxFormat = &TmpWaveFormat;
		TmpDescriptor.guid3DAlgorithm = DS3DALG_DEFAULT;
		if (DS_OK == (This->lpDevice->CreateSoundBuffer(&TmpDescriptor, &(TmpStream->SoundBuffer), nullptr))){
			if (DS_OK == TmpStream->SoundBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&(TmpStream->SoundNotify))){
				TmpStream->StreamEvent[0].dwOffset = 0;
				if (nullptr != (TmpStream->StreamEvent[0].hEventNotify = CreateEvent(nullptr, FALSE, FALSE, nullptr))){
					TmpStream->StreamEvent[1].dwOffset = TmpDescriptor.dwBufferBytes / 2;
					if (nullptr != (TmpStream->StreamEvent[1].hEventNotify = CreateEvent(nullptr, FALSE, FALSE, nullptr))){
						if (DS_OK == TmpStream->SoundNotify->SetNotificationPositions(2, TmpStream->StreamEvent)){
							LPVOID AudioPtr1, AudioPtr2;
							DWORD Size1, Size2;
							TmpStream->SoundBuffer->Lock(0, 0, &AudioPtr1, &Size1, &AudioPtr2, &Size2, DSBLOCK_ENTIREBUFFER);
							memset(AudioPtr1, 0, TmpDescriptor.dwBufferBytes);
							TmpStream->SoundBuffer->Unlock(AudioPtr1, Size1, AudioPtr2, Size2);

							TmpStream->FrameLen = TmpDescriptor.dwBufferBytes / 2;
							TmpStream->FrameTick = 0;
							return TmpStream;
						}
						CloseHandle(TmpStream->StreamEvent[1].hEventNotify);
					}
					CloseHandle(TmpStream->StreamEvent[0].hEventNotify);
				}
				TmpStream->SoundNotify->Release();
			}
			TmpStream->SoundBuffer->Release();
		}
		This->OutputStreamList.pop_back();
		return nullptr;
	}
	void OutputDevice_DS::_DestroyStream_Output(OutputDevice * const ThisBase, OutputStream * const lpStream){
		OutputStream_DS *lpStream_DS = static_cast<OutputStream_DS *>(lpStream);
		CloseHandle(lpStream_DS->StreamEvent[0].hEventNotify);
		CloseHandle(lpStream_DS->StreamEvent[1].hEventNotify);
		lpStream_DS->SoundNotify->Release();
		lpStream_DS->SoundBuffer->Release();
	}

	OutputStream_DS::OutputStream_DS(){ this->lpInterface = const_cast<OutputStream::Interface *>(&this->_I); }
	void OutputStream_DS::_Play(OutputStream * const ThisBase){
		OutputStream_DS *This = static_cast<OutputStream_DS *>(ThisBase);
		This->SoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}
	void OutputStream_DS::_Pause(OutputStream * const ThisBase){
		OutputStream_DS *This = static_cast<OutputStream_DS *>(ThisBase);
		This->SoundBuffer->Stop();
	}
	void OutputStream_DS::_Stop(OutputStream * const ThisBase){ //call this before destroy the object
		OutputStream_DS *This = static_cast<OutputStream_DS *>(ThisBase);
		This->SoundBuffer->Stop();
		SetEvent(This->StreamEvent[0].hEventNotify);
		SetEvent(This->StreamEvent[1].hEventNotify);
	}
	void *OutputStream_DS::_WaitFrame(OutputStream * const ThisBase){
		OutputStream_DS *This = static_cast<OutputStream_DS *>(ThisBase);
		//Remind that the notify is not accurate. Better check the play position every time.
		LPVOID AudioPtr2;
		DWORD Size1, Size2;
		if (0 == This->FrameTick){
			WaitForSingleObject(This->StreamEvent[1].hEventNotify, INFINITE);
			if (!This->_Playing)return nullptr;
			ResetEvent(This->StreamEvent[0].hEventNotify);
			This->SoundBuffer->Lock(0, (DWORD)This->FrameLen, &This->lpFrame, &Size1, &AudioPtr2, &Size2, 0);
		}
		else{
			WaitForSingleObject(This->StreamEvent[0].hEventNotify, INFINITE);
			if (!This->_Playing)return nullptr;
			ResetEvent(This->StreamEvent[1].hEventNotify);
			This->SoundBuffer->Lock((DWORD)This->FrameLen, This->FrameLen, &This->lpFrame, &Size1, &AudioPtr2, &Size2, 0);
		}
		return This->lpFrame;
	}
	void OutputStream_DS::_SubmitFrame(OutputStream * const ThisBase){
		OutputStream_DS *This = static_cast<OutputStream_DS *>(ThisBase);
		if (0 == This->FrameTick){
			This->SoundBuffer->Unlock(This->lpFrame, (DWORD)This->FrameLen, NULL, 0);
			This->FrameTick = 1;
		}
		else{
			This->SoundBuffer->Unlock(This->lpFrame, (DWORD)This->FrameLen, NULL, 0);
			This->FrameTick = 0;
		}
	}
}
#endif