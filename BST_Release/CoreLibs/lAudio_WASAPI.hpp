/* Description: Audio classes using WASAPI as the backend.
 * Language:C++
 * Author:***
 *
 * You must enable multithreading to use the following functions.
 */

#ifndef LIB_AUDIO_WASAPI
#define LIB_AUDIO_WASAPI

#include "lGeneral.hpp"
#include "lAudio.hpp"
#include "lContainer.hpp"

#include <Mmreg.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>


namespace nsAudio{
	class OutputDevice_WASAPI;
	class OutputStream_WASAPI: public OutputStream{
	private:
		static const OutputStream::Interface _I;

		IAudioRenderClient *lpBufMgr;
		IAudioClient *lpClient;

		HANDLE StreamEvent;

		bool Stopped;
		UINT4b FrameSize;
		void *lpFrame;

		static void _Play(OutputStream * const ThisBase);
		static void _Pause(OutputStream * const ThisBase);
		static void _Stop(OutputStream * const ThisBase);
		static void * _WaitFrame(OutputStream * const ThisBase);
		static void _SubmitFrame(OutputStream * const ThisBase);

	public:
		friend class OutputDevice_WASAPI;
		OutputStream_WASAPI(); //this constructor should never be used by anyone except OutputDevice_WASAPI.
	};
	const OutputStream::Interface OutputStream_WASAPI::_I{
		OutputStream_WASAPI::_Play,
		OutputStream_WASAPI::_Pause,
		OutputStream_WASAPI::_Stop,
		OutputStream_WASAPI::_WaitFrame,
		OutputStream_WASAPI::_SubmitFrame
	};

	class OutputDevice_WASAPI: public OutputDevice{
	private:
		static const OutputDevice::Interface _I;

		static OutputStream * _CreateStream_Output(OutputDevice * const ThisBase, UBINT SampleTypeID, UBINT Channels, UBINT SamplePerFrame, UBINT SamplePerSec);
		static void _DestroyStream_Output(OutputDevice * const ThisBase, OutputStream * const lpStream);
		static void _Destroy(OutputDevice * const ThisBase);

		IMMDevice *lpDevice;
		nsContainer::List<OutputStream_WASAPI> OutputStreamList;
	public:
		OutputDevice_WASAPI();
	};
	const OutputDevice::Interface OutputDevice_WASAPI::_I{
		OutputDevice_WASAPI::_CreateStream_Output,
		OutputDevice_WASAPI::_DestroyStream_Output,
		OutputDevice_WASAPI::_Destroy
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	void OutputDevice_WASAPI::_Destroy(OutputDevice * const ThisBase){
		OutputDevice_WASAPI *This = static_cast<OutputDevice_WASAPI *>(ThisBase);

		auto _It = This->OutputStreamList.begin();
		while (_It != This->OutputStreamList.end()){
			CloseHandle(_It->StreamEvent);
			_It->lpBufMgr->Release();
			_It->lpClient->Release();
		}
		This->lpDevice->Release();
		CoUninitialize();
	}
	OutputDevice_WASAPI::OutputDevice_WASAPI(){
		IMMDeviceEnumerator *lpEnumerator;
		if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))throw std::exception("failed to initiate multimedia device.");
		if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&lpEnumerator))){
			CoUninitialize();
			throw std::exception("failed to initiate multimedia device.");
		}
		if (FAILED(lpEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &this->lpDevice))){
			lpEnumerator->Release();
			CoUninitialize();
			throw std::exception("failed to initiate multimedia device.");
		}
		lpEnumerator->Release();
		this->lpInterface = const_cast<OutputDevice::Interface *>(&this->_I);
	}

	OutputStream *OutputDevice_WASAPI::_CreateStream_Output(OutputDevice * const ThisBase, UBINT SampleTypeID, UBINT Channels, UBINT SamplePerFrame, UBINT SamplePerSec){
		OutputDevice_WASAPI *This = static_cast<OutputDevice_WASAPI *>(ThisBase);

		UBINT BytesPerSample;
		if (__typeid(UINT1b) == SampleTypeID)BytesPerSample = 1;
		else if (__typeid(INT2b) == SampleTypeID)BytesPerSample = 2;
		else return nullptr;

		This->OutputStreamList.emplace_back();
		OutputStream_WASAPI *TmpStream = &This->OutputStreamList.back();

		if (SUCCEEDED(This->lpDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&TmpStream->lpClient))){
			WAVEFORMATEX TmpWaveFormat;
			TmpWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			TmpWaveFormat.nChannels = (WORD)Channels;
			TmpWaveFormat.nSamplesPerSec = (DWORD)SamplePerSec;
			TmpWaveFormat.nAvgBytesPerSec = (DWORD)(Channels*SamplePerSec*BytesPerSample);
			TmpWaveFormat.nBlockAlign = (WORD)(Channels*BytesPerSample);
			TmpWaveFormat.wBitsPerSample = (WORD)BytesPerSample * 8;
			TmpWaveFormat.cbSize = 0;

			/*WAVEFORMATEX *ClosestFormat;
			HRESULT TmpResult = TmpStream->lpClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &TmpWaveFormat, &ClosestFormat);
			CoTaskMemFree(ClosestFormat);*/

			if (SUCCEEDED(TmpStream->lpClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, (REFERENCE_TIME)(2 * SamplePerFrame * (1.0E7 / TmpWaveFormat.nSamplesPerSec) + 0.5), 0, &TmpWaveFormat, nullptr))){
				UINT4b MaxBufSize;
				if (SUCCEEDED(TmpStream->lpClient->GetBufferSize((UINT32 *)&MaxBufSize)) && (UBINT)MaxBufSize >= 2 * SamplePerFrame){
					TmpStream->StreamEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
					if (nullptr != TmpStream->StreamEvent && SUCCEEDED(TmpStream->lpClient->SetEventHandle(TmpStream->StreamEvent))){
						if (SUCCEEDED(TmpStream->lpClient->GetService(__uuidof(IAudioRenderClient), (void **)&TmpStream->lpBufMgr))){
							TmpStream->FrameSize = (UINT4b)SamplePerFrame;
							TmpStream->lpBufMgr->GetBuffer(TmpStream->FrameSize, (BYTE **)&TmpStream->lpFrame);
							TmpStream->lpBufMgr->ReleaseBuffer(TmpStream->FrameSize, AUDCLNT_BUFFERFLAGS_SILENT);
							return TmpStream;
						}
					}
					CloseHandle(TmpStream->StreamEvent);
				}
				TmpStream->lpClient->Release();
			}
		}
		This->OutputStreamList.pop_back();
		return nullptr;
	}
	void OutputDevice_WASAPI::_DestroyStream_Output(OutputDevice * const ThisBase, OutputStream * const lpStream){
		OutputStream_WASAPI *lpStream_DS = static_cast<OutputStream_WASAPI *>(lpStream);
		CloseHandle(lpStream_DS->StreamEvent);
		lpStream_DS->lpBufMgr->Release();
		lpStream_DS->lpClient->Release();
	}

	OutputStream_WASAPI::OutputStream_WASAPI(){
		this->lpInterface = const_cast<OutputStream::Interface *>(&this->_I);
		this->Stopped = false;
	}
	void OutputStream_WASAPI::_Play(OutputStream * const ThisBase){
		OutputStream_WASAPI *This = static_cast<OutputStream_WASAPI *>(ThisBase);
		This->lpClient->Start();
	}
	void OutputStream_WASAPI::_Pause(OutputStream * const ThisBase){
		OutputStream_WASAPI *This = static_cast<OutputStream_WASAPI *>(ThisBase);
		This->lpClient->Stop();
	}
	void OutputStream_WASAPI::_Stop(OutputStream * const ThisBase){ //call this before destroy the object
		OutputStream_WASAPI *This = static_cast<OutputStream_WASAPI *>(ThisBase);
		This->lpClient->Stop();
		This->lpClient->Reset();
		This->Stopped = true;
		SetEvent(This->StreamEvent);
	}
	void *OutputStream_WASAPI::_WaitFrame(OutputStream * const ThisBase){
		OutputStream_WASAPI *This = static_cast<OutputStream_WASAPI *>(ThisBase);
		//DWORD a = WaitForSingleObject(This->StreamEvent, 200);
		UINT32 b = 0;
		This->lpClient->GetCurrentPadding(&b);
		do{
			//why doesn't WASAPI fire the damn event?!
			This->lpClient->GetCurrentPadding(&b);
			nsBasic::YieldCurThread();
			if (This->Stopped)break;
			/*DWORD a = WaitForSingleObject(This->StreamEvent, 0);
			if (0 == a){
				a = 1;
			}*/
		} while (b >= This->FrameSize);
		if (!This->_Playing)return nullptr;
		//ResetEvent(This->StreamEvent);
		This->lpBufMgr->GetBuffer(This->FrameSize, (BYTE **)&This->lpFrame);
		return This->lpFrame;
	}
	void OutputStream_WASAPI::_SubmitFrame(OutputStream * const ThisBase){
		OutputStream_WASAPI *This = static_cast<OutputStream_WASAPI *>(ThisBase);
		HRESULT b = This->lpBufMgr->ReleaseBuffer(This->FrameSize, 0);
	}
}
#endif