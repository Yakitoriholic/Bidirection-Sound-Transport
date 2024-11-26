#ifndef AUDIO_RENDERER
#define AUDIO_RENDERER

#include "lGeneral.hpp"
#include "lAudio.hpp"
#include "lMath_SIMD.hpp"

class AudioRenderer{
public:
	class FFTProvider{
	private:
		nsContainer::Vector<fftwf_plan> PlanList_FFT;
		nsContainer::Vector<fftwf_plan> PlanList_IFFT;
		nsContainer::Vector<float *> BufferList_FFT;
		nsContainer::Vector<std::complex<float> *> BufferList_IFFT;
		nsContainer::Vector<float *> BufferList_IFFT_Dest;
		nsContainer::Vector<float *> InterpCoeffList;
		UBINT MinFFTSize;
	public:
		inline FFTProvider(UBINT MinFFTSize){ this->MinFFTSize = MinFFTSize; }
		void FFT(std::complex<float> *pDest, float *pSrc, UBINT Size);
		void IFFT(float *pDest, std::complex<float> *pSrc, UBINT Size);
		float *GetFFTBuffer(UBINT Size);
		std::complex<float> *GetIFFTBuffer(UBINT Size);
		float *GetIFFTBuffer_Dest(UBINT Size);
		float *GetInterpCoeff(UBINT Size);
		~FFTProvider();
	};

	class InputStream{
	public:
		friend class AudioRenderer;
	private:
		nsBasic::Stream_RS<unsigned char> *lpAudioStream;
		nsFormat::AudioReader *lpAudioDecoder;

		struct Buffer{
		public:
			BINT Start;
			BINT End;
			float *pData; // [Start, end)
		};

	public:
		float Volume;
		Buffer Buffer;
		BINT NextInterval_Start, NextInterval_End;
		InputStream(nsBasic::Stream_RS<unsigned char> *lpAudioStream, nsFormat::AudioReader *lpAudioDecoder, float Volume);

		void Advance(UBINT Step);

		~InputStream(){
			if (nullptr != this->Buffer.pData)nsBasic::MemFree(this->Buffer.pData, this->Buffer.End - this->Buffer.Start);
			this->lpAudioDecoder->Delete();
			this->lpAudioStream->Delete();
		}
	};

	struct MixCommand{
		InputStream *pStream;
		float Volume;
		BINT Start;
		UBINT Length;
	};

	class OutputStream{
	public:
		friend class AudioRenderer;
	private:
		nsContainer::Vector<MixCommand> CommandList;
		UBINT Length;
	public:
		float *pData;
		inline void ClearInput(){ this->CommandList.clear(); }
		inline void AddInput(MixCommand &InputCommand){ this->CommandList.push_back(InputCommand); }
		inline OutputStream(UBINT Length){ this->pData = nsBasic::MemAlloc_Arr_SIMD<float>(Length); this->Length = Length; }
		inline ~OutputStream(){ nsBasic::MemFree_Arr_SIMD(this->pData, this->Length); }
	};

	class FilterLane{
	public:
		nsContainer::Vector<std::complex<float> *> FilterBuffer_F_Prev;
		nsContainer::Vector<std::complex<float> *> FilterBuffer_F;
		nsContainer::Vector<float *> OutputBuffer_T;
		nsContainer::Vector<bool> Layer_Updated;
		OutputStream *pOutput;
		UBINT BlockCount_Prev;
		UBINT LayerCount_Prev;
		UBINT BlockCount; // max block count of updated IR
		UBINT LayerCount; // max layer count of updated IR
	};

	class Filter{
	public:
		friend class AudioRenderer;
	private:
		static const UBINT LayerSize = 2; // must be a power of 2
		static const UBINT MaxLayerCount = 6;

		nsContainer::Vector<float *> InputBuffer_T;
		nsContainer::Vector<std::complex<float> *> InputBuffer_F;
		UBINT RandSeed;
		UBINT FrameLen;
		UBINT BlockCount_Prev;
		UBINT LayerCount_Prev;
		UBINT BlockCount; // max block count of updated IR
		UBINT LayerCount; // max layer count of updated IR
		FFTProvider *pFFT;

		nsContainer::Vector<MixCommand> CommandList;
		nsContainer::List<FilterLane> LaneList;

		UBINT GetLayerCount(UBINT LastSamplePos);
		UBINT GetBlockCount(UBINT LastSamplePos, UBINT LayerCount);
		UBINT GetBlockSizeByIndex(UBINT BlockIndex);
		UBINT GetBlockSizeByLayer(UBINT LayerIndex);
		float *Advance();
		void ProcessInput();
		void Output();
	public:
		Filter(UBINT FrameLen, UBINT RandSeed, FFTProvider *pFFTProvider){
			this->RandSeed = RandSeed;
			this->FrameLen = FrameLen;
			this->BlockCount_Prev = 0;
			this->LayerCount_Prev = 0;
			this->BlockCount = 0;
			this->LayerCount = 0;
			this->pFFT = pFFTProvider;
		}
		FilterLane *AddLane(OutputStream *pOutput);
		inline void ClearInput(){ this->CommandList.clear(); }
		inline void AddInput(MixCommand &InputCommand){ this->CommandList.push_back(InputCommand); }
		void UpdateLane(FilterLane *pLane, float *Data, UBINT Pos_End);
		void EndUpdate();
		void DeleteLane(FilterLane *pLane);
		~Filter();
	};

private:
	FFTProvider MyFFTProvider;
	nsMath::RandGenerator_XORShiftPlus RandGenerator;
	UBINT FrameLen;
	float *MixerBuffer;

	//input management --BEGIN--
	nsContainer::Concurrent::List<InputStream> InputSet;
	//input management --END--

	//filter management --BEGIN--
	nsContainer::Concurrent::List<Filter> FilterSet;
	//filter management --END--

	//output management --BEGIN--
	nsContainer::Concurrent::List<OutputStream> OutputSet;
	//output management --END--

	//compiler --END--
	
	void ExecMixCommand(float *pDest, nsContainer::Vector<MixCommand> *pCmdList);

public:
	AudioRenderer(UBINT FrameLen) : MyFFTProvider(FrameLen){
		if (FrameLen < nsEnv::SIMDAlignment)throw std::exception("The length of the sound window should not be smaller than the SIMD instruction alignment.");
		if (!nsMath::is2power(FrameLen))throw std::exception("The length of the sound window must be the power of 2.");
		this->FrameLen = FrameLen;
		this->MixerBuffer = nsBasic::MemAlloc_Arr_SIMD<float>(FrameLen);
	}

	inline UBINT GetFrameLen(){ return this->FrameLen; }
	void RenderBegin();

	//input management --BEGIN--
	InputStream *AddInput(nsBasic::Stream_RS<unsigned char> *lpAudioStream, nsFormat::AudioReader *lpAudioDecoder, float Volume = 1.0f);
	void DeleteInput(const InputStream *pStream);
	inline void AcquireInput(InputStream *pStream){ this->InputSet.exclusive_acquire(pStream); }
	inline void ReleaseInput(InputStream *pStream){ this->InputSet.exclusive_release(pStream); }
	void ReadyForInput();
	void LoadInput();
	//input management --END--

	//filter management --BEGIN--
	Filter *AddFilter();
	void DeleteFilter(const Filter *pFilter);
	inline void AcquireFilter(Filter *pFilter){ this->FilterSet.exclusive_acquire(pFilter); };
	inline void ReleaseFilter(Filter *pFilter){ this->FilterSet.exclusive_release(pFilter); }
	void ExecuteFilter();
	//filter management --END--
	 
	//output management --BEGIN--
	OutputStream *AddOutput();
	void DeleteOutput(const OutputStream *pStream);
	inline void AcquireOutput(OutputStream *pStream){ this->OutputSet.exclusive_acquire(pStream); };
	inline void ReleaseOutput(OutputStream *pStream){ this->OutputSet.exclusive_release(pStream); }
	void ExecuteOutput();
	//output management --END--

	void Clear();
	inline ~AudioRenderer(){ this->Clear(); nsBasic::MemFree_Arr_SIMD<float>(this->MixerBuffer, FrameLen); }
};

/*-------------------------------- IMPLEMENTATION --------------------------------*/

void AudioRenderer::FFTProvider::FFT(std::complex<float> *pDest, float *pSrc, UBINT Size){
	UBINT FFTIndex = nsMath::log2intsim(Size / this->MinFFTSize);
	if (this->PlanList_FFT.size() < FFTIndex)this->PlanList_FFT.resize(FFTIndex, nullptr);
	FFTIndex--;
	if (nullptr == this->PlanList_FFT[FFTIndex]){
		int n = (int)Size;
		this->PlanList_FFT[FFTIndex] = fftwf_plan_dft_r2c(1, &n, pSrc, (fftwf_complex *)pDest, FFTW_ESTIMATE);
	}
	fftwf_execute_dft_r2c(this->PlanList_FFT[FFTIndex], pSrc, (fftwf_complex *)pDest);
}
void AudioRenderer::FFTProvider::IFFT(float *pDest, std::complex<float> *pSrc, UBINT Size){
	UBINT FFTIndex = nsMath::log2intsim(Size / this->MinFFTSize);
	if (this->PlanList_IFFT.size() < FFTIndex)this->PlanList_IFFT.resize(FFTIndex, nullptr);
	FFTIndex--;
	if (nullptr == this->PlanList_IFFT[FFTIndex]){
		int n = (int)Size;
		this->PlanList_IFFT[FFTIndex] = fftwf_plan_dft_c2r(1, &n, (fftwf_complex *)pSrc, pDest, FFTW_ESTIMATE);
	}
	fftwf_execute_dft_c2r(this->PlanList_IFFT[FFTIndex], (fftwf_complex *)pSrc, pDest);
}
float *AudioRenderer::FFTProvider::GetFFTBuffer(UBINT Size){
	UBINT FFTIndex = nsMath::log2intsim(Size / this->MinFFTSize);
	if (this->BufferList_FFT.size() < FFTIndex)this->BufferList_FFT.resize(FFTIndex, nullptr);
	FFTIndex--;
	if (nullptr == this->BufferList_FFT[FFTIndex]){
		this->BufferList_FFT[FFTIndex] = nsBasic::MemAlloc_Arr_SIMD<float>(Size);
		nsMath::setzero_simd_aligned(&this->BufferList_FFT[FFTIndex][Size / 2], Size / 2);
	}
	return this->BufferList_FFT[FFTIndex];
}
std::complex<float> *AudioRenderer::FFTProvider::GetIFFTBuffer(UBINT Size){
	UBINT FFTIndex = nsMath::log2intsim(Size / this->MinFFTSize);
	if (this->BufferList_IFFT.size() < FFTIndex)this->BufferList_IFFT.resize(FFTIndex, nullptr);
	FFTIndex--;
	if (nullptr == this->BufferList_IFFT[FFTIndex])this->BufferList_IFFT[FFTIndex] = nsBasic::MemAlloc_Arr_SIMD<std::complex<float>>(Size);
	return this->BufferList_IFFT[FFTIndex];
}
float *AudioRenderer::FFTProvider::GetIFFTBuffer_Dest(UBINT Size){
	UBINT FFTIndex = nsMath::log2intsim(Size / this->MinFFTSize);
	if (this->BufferList_IFFT_Dest.size() < FFTIndex)this->BufferList_IFFT_Dest.resize(FFTIndex, nullptr);
	FFTIndex--;
	if (nullptr == this->BufferList_IFFT_Dest[FFTIndex])this->BufferList_IFFT_Dest[FFTIndex] = nsBasic::MemAlloc_Arr_SIMD<float>(Size);
	return this->BufferList_IFFT_Dest[FFTIndex];
}
float *AudioRenderer::FFTProvider::GetInterpCoeff(UBINT Size){
	UBINT FFTIndex = nsMath::log2intsim(Size / this->MinFFTSize);
	if (this->InterpCoeffList.size() < FFTIndex)this->InterpCoeffList.resize(FFTIndex, nullptr);
	FFTIndex--;
	if (nullptr == this->InterpCoeffList[FFTIndex]){
		this->InterpCoeffList[FFTIndex] = nsBasic::MemAlloc_Arr_SIMD<float>(Size);
		for (UBINT i = 0; i < Size; i++)this->InterpCoeffList[FFTIndex][i] = (float)i / (float)Size;
	}
	return this->InterpCoeffList[FFTIndex];
}
AudioRenderer::FFTProvider::~FFTProvider(){
	for (auto _It = this->PlanList_FFT.begin(); _It != this->PlanList_FFT.end(); ++_It){
		if (nullptr != *_It)fftwf_destroy_plan(*_It);
	}
	for (auto _It = this->PlanList_IFFT.begin(); _It != this->PlanList_IFFT.end(); ++_It){
		if (nullptr != *_It)fftwf_destroy_plan(*_It);
	}
	UBINT TmpSize = this->MinFFTSize;
	for (UBINT i = 0; i < this->BufferList_FFT.size(); i++){
		if (nullptr != this->BufferList_FFT[i])nsBasic::MemFree_Arr_SIMD(this->BufferList_FFT[i], TmpSize);
		TmpSize *= 2;
	}
	TmpSize = this->MinFFTSize;
	for (UBINT i = 0; i < this->BufferList_IFFT.size(); i++){
		if (nullptr != this->BufferList_IFFT[i])nsBasic::MemFree_Arr_SIMD(this->BufferList_IFFT[i], TmpSize);
		TmpSize *= 2;
	}
	TmpSize = this->MinFFTSize;
	for (UBINT i = 0; i < this->BufferList_IFFT_Dest.size(); i++){
		if (nullptr != this->BufferList_IFFT_Dest[i])nsBasic::MemFree_Arr_SIMD(this->BufferList_IFFT_Dest[i], TmpSize);
		TmpSize *= 2;
	}
	TmpSize = this->MinFFTSize;
	for (UBINT i = 0; i < this->InterpCoeffList.size(); i++){
		if (nullptr != this->InterpCoeffList[i])nsBasic::MemFree_Arr_SIMD(this->InterpCoeffList[i], TmpSize);
		TmpSize *= 2;
	}
}

//class AudioRenderer::InputStream --BEGIN--
AudioRenderer::InputStream::InputStream(nsBasic::Stream_RS<unsigned char> *lpAudioStream, nsFormat::AudioReader *lpAudioDecoder, float Volume){
	this->lpAudioStream = lpAudioStream;
	this->lpAudioDecoder = lpAudioDecoder;
	this->Volume = Volume;

	this->Buffer.Start = 0;
	this->Buffer.End = 0;
	this->Buffer.pData = nullptr;

	lpAudioDecoder->BeginRead(lpAudioStream);
}
void AudioRenderer::InputStream::Advance(UBINT Step){
	if (this->Buffer.End - (BINT)Step > this->NextInterval_End)this->NextInterval_End = this->Buffer.End - (BINT)Step;
	if (this->NextInterval_Start > this->NextInterval_End - (this->Buffer.End - this->Buffer.Start))this->NextInterval_Start = this->NextInterval_End - (this->Buffer.End - this->Buffer.Start); // buffer size never decrease
	
	// calculate copy interval
	UBINT Src_LBound, Src_RBound, Dest_LBound, Dest_RBound;
	if (this->NextInterval_Start < this->Buffer.Start - (BINT)Step){
		Dest_LBound = this->Buffer.Start - (BINT)Step - this->NextInterval_Start;
		Src_LBound = 0;
	}
	else{
		Dest_LBound = 0;
		Src_LBound = this->NextInterval_Start - (this->Buffer.Start - (BINT)Step); // may exceed src length
	}
	if (this->Buffer.End - this->Buffer.Start > (BINT)Src_LBound){
		Dest_RBound = this->Buffer.End - (BINT)Step - this->NextInterval_Start;
		Src_RBound = this->Buffer.End - this->Buffer.Start;
	}
	else{
		Dest_RBound = Dest_LBound;
		Src_RBound = Src_LBound;
	}

	bool Reallocate = this->NextInterval_End - this->NextInterval_Start > this->Buffer.End - this->Buffer.Start;
	float *pNewData;
	if (Reallocate){
		pNewData = nsBasic::MemAlloc_Arr<float>(this->NextInterval_End - this->NextInterval_Start);
		if (nullptr == pNewData)throw std::bad_alloc();
	}
	else pNewData = this->Buffer.pData;

	if ((BINT)Src_LBound >= this->Buffer.End - this->Buffer.Start){
		// skip samples -- BEGIN --
		UBINT SampleToSkip = Src_LBound - (this->Buffer.End - this->Buffer.Start);
		float *pData_Temp = nsBasic::MemAlloc_Arr<float>(SampleToSkip);
		this->lpAudioDecoder->Read_Mono(pData_Temp, SampleToSkip);
		// skip samples -- END --

		this->lpAudioDecoder->Read_Mono(pNewData, this->NextInterval_End - this->NextInterval_Start);
	}
	else{
		nsMath::setzero_simd_unaligned(pNewData, Dest_LBound);
		nsBasic::MemMove_Arr(&pNewData[Dest_LBound], &this->Buffer.pData[Src_LBound], Dest_RBound - Dest_LBound);
		this->lpAudioDecoder->Read_Mono(&pNewData[Dest_RBound], this->NextInterval_End - this->NextInterval_Start - Dest_RBound);	
	}
	if (this->Volume != 1.0f)nsMath::mul_simd_unaligned(&pNewData[Dest_RBound], &pNewData[Dest_RBound], this->Volume, this->NextInterval_End - this->NextInterval_Start - Dest_RBound);

	if (Reallocate && nullptr != this->Buffer.pData){
		nsBasic::MemFree_Arr(this->Buffer.pData, this->Buffer.End - this->Buffer.Start);
	}
	this->Buffer.pData = pNewData;
	this->Buffer.Start = this->NextInterval_Start;
	this->Buffer.End = this->NextInterval_End;
}
//class AudioRenderer::InputStream --END--

//class AudioRenderer::Filter --BEGIN--
UBINT AudioRenderer::Filter::GetLayerCount(UBINT IRLength){
	if (0 == IRLength)return 0;
	UBINT RetValue = (nsMath::log2intsim(((IRLength - 1) * (Filter::LayerSize - 1)) / (Filter::LayerSize * this->FrameLen) + 1) - 1) / (nsMath::log2intsim(Filter::LayerSize) - 1) + 1;
	if (RetValue > Filter::MaxLayerCount)RetValue = Filter::MaxLayerCount;
	return RetValue;
}
UBINT AudioRenderer::Filter::GetBlockCount(UBINT IRLength, UBINT LayerCount){
	if (0 == LayerCount)return 0;
	UBINT LargestBlockSize = this->FrameLen << ((nsMath::log2intsim(Filter::LayerSize) - 1) * (LayerCount - 1));
	UBINT RetValue = (LayerCount - 1) * Filter::LayerSize + (IRLength + LargestBlockSize - 1 - Filter::LayerSize * (LargestBlockSize - this->FrameLen) / (Filter::LayerSize - 1)) / LargestBlockSize;
	return RetValue;
}
UBINT AudioRenderer::Filter::GetBlockSizeByIndex(UBINT BlockIndex){
	UBINT LayerIndex = BlockIndex / Filter::LayerSize;
	if (LayerIndex >= Filter::MaxLayerCount)LayerIndex = Filter::MaxLayerCount - 1;
	return this->FrameLen << ((nsMath::log2intsim(Filter::LayerSize) - 1) * LayerIndex);
}
inline UBINT AudioRenderer::Filter::GetBlockSizeByLayer(UBINT LayerIndex){
	return this->FrameLen << ((nsMath::log2intsim(Filter::LayerSize) - 1) * LayerIndex);
}
float *AudioRenderer::Filter::Advance(){
	this->RandSeed++;

	UBINT CurBlockCount = std::max(this->BlockCount, this->BlockCount_Prev);
	UBINT CurLayerCount = std::max(this->LayerCount, this->LayerCount_Prev);
	// reallocate memory
	if (this->InputBuffer_T.size() < CurBlockCount){
		UBINT Size_Prev = this->InputBuffer_T.size();
		this->InputBuffer_T.resize(CurBlockCount, nullptr);
		this->InputBuffer_F.resize(CurBlockCount, nullptr);
		for (UBINT i = Size_Prev; i < CurBlockCount; i++){
			if (0 == i % Filter::LayerSize && i / Filter::LayerSize < Filter::MaxLayerCount){
				this->InputBuffer_T[i] = nsBasic::MemAlloc_Arr_SIMD<float>(2 * this->GetBlockSizeByIndex(i));
				this->InputBuffer_F[i] = nsBasic::MemAlloc_Arr_SIMD<std::complex<float>>(2 * this->GetBlockSizeByIndex(i));
				nsMath::setzero_simd_aligned(this->InputBuffer_T[i], this->GetBlockSizeByIndex(i));
				nsMath::setzero_simd_aligned(this->InputBuffer_F[i], this->GetBlockSizeByIndex(i) + 1);
			}
		}
	}
	else{
		for (UBINT i = CurBlockCount; i < this->InputBuffer_T.size(); i++){
			if (nullptr != this->InputBuffer_T[i])nsBasic::MemFree_Arr_SIMD(this->InputBuffer_T[i], 2 * this->GetBlockSizeByIndex(i));
			if (nullptr != this->InputBuffer_F[i])nsBasic::MemFree_Arr_SIMD(this->InputBuffer_F[i], 2 * this->GetBlockSizeByIndex(i));
		}
		this->InputBuffer_T.resize(CurBlockCount);
		this->InputBuffer_F.resize(CurBlockCount);
	}

	UBINT LayerSize_Bits = nsMath::log2intsim(Filter::LayerSize) - 1;
	UBINT IndexMask = (1 << (LayerSize_Bits * CurLayerCount)) / Filter::LayerSize;
	for (UBINT i = 0; i < CurLayerCount; i++){
		UBINT LayerIndex = CurLayerCount - i - 1, FirstBlockIndex = LayerIndex * Filter::LayerSize;
		UBINT Status = this->RandSeed & (IndexMask - 1);
		if (0 == Status){
			UBINT ActualLayerSize;
			if (0 == i)ActualLayerSize = CurBlockCount - FirstBlockIndex;
			else ActualLayerSize = Filter::LayerSize;

			float *CurInput_T = this->InputBuffer_T[FirstBlockIndex + ActualLayerSize - 1];
			std::complex<float> *CurInput_F = this->InputBuffer_F[FirstBlockIndex + ActualLayerSize - 1];
			for (UBINT j = 1; j < ActualLayerSize; j++){
				this->InputBuffer_T[FirstBlockIndex + ActualLayerSize - j] = this->InputBuffer_T[FirstBlockIndex + ActualLayerSize - j - 1];
				this->InputBuffer_F[FirstBlockIndex + ActualLayerSize - j] = this->InputBuffer_F[FirstBlockIndex + ActualLayerSize - j - 1];
			}
			this->InputBuffer_T[FirstBlockIndex] = CurInput_T;
			this->InputBuffer_F[FirstBlockIndex] = CurInput_F;

			UBINT BlockSampleCount = this->GetBlockSizeByLayer(LayerIndex);
			if (nullptr == this->InputBuffer_T[FirstBlockIndex]){
				this->InputBuffer_T[FirstBlockIndex] = nsBasic::MemAlloc_Arr_SIMD<float>(2 * BlockSampleCount);
				this->InputBuffer_F[FirstBlockIndex] = nsBasic::MemAlloc_Arr_SIMD<std::complex<float>>(2 * BlockSampleCount);
			}

			float *Addr_Dest = this->InputBuffer_T[FirstBlockIndex];
			float *Addr_LastFrame;
			if (ActualLayerSize > 1)Addr_LastFrame = this->InputBuffer_T[FirstBlockIndex + 1]; else Addr_LastFrame = this->InputBuffer_T[FirstBlockIndex];
			if (nullptr != Addr_LastFrame)nsBasic::MemMove_Arr(&Addr_Dest[BlockSampleCount], Addr_LastFrame, BlockSampleCount);
			else nsMath::setzero_simd_aligned(&Addr_Dest[BlockSampleCount], BlockSampleCount);
			
			if (LayerIndex > 0){
				BlockSampleCount /= Filter::LayerSize;
				for (UBINT j = 0; j < Filter::LayerSize; j++){
					UBINT TargetIndex_Src = FirstBlockIndex - j - 1;
					if (nullptr == this->InputBuffer_T[TargetIndex_Src])nsMath::setzero_simd_aligned(&Addr_Dest[j * BlockSampleCount], BlockSampleCount);
					else nsBasic::MemMove_Arr(&Addr_Dest[j * BlockSampleCount], this->InputBuffer_T[TargetIndex_Src], BlockSampleCount);
				}
				this->pFFT->FFT(this->InputBuffer_F[FirstBlockIndex], Addr_Dest, 2 * BlockSampleCount * Filter::LayerSize);
			}
			else{
				nsMath::setzero_simd_aligned(this->InputBuffer_T[FirstBlockIndex], BlockSampleCount);
			}
		}
		IndexMask /= Filter::LayerSize;
	}
	return this->InputBuffer_T[0];
}
inline void AudioRenderer::Filter::ProcessInput(){
	this->pFFT->FFT(this->InputBuffer_F[0], this->InputBuffer_T[0], 2 * this->FrameLen);
}
void AudioRenderer::Filter::Output(){
	this->BlockCount_Prev = 0;
	this->LayerCount_Prev = 0;
	for (auto _It = this->LaneList.begin(); _It != this->LaneList.end(); ++_It){
		UBINT LayerCount_CurLane = std::max(_It->LayerCount, _It->LayerCount_Prev);
		if (_It->OutputBuffer_T.size() > LayerCount_CurLane){
			for (UBINT i = LayerCount_CurLane; i < _It->OutputBuffer_T.size(); i++){
				if (nullptr != _It->OutputBuffer_T[i]){
					nsBasic::MemFree_Arr_SIMD(_It->OutputBuffer_T[i], 2 * this->GetBlockSizeByLayer(i));
					_It->OutputBuffer_T[i] = nullptr;
				}
			}
			_It->OutputBuffer_T.resize(LayerCount_CurLane);
			_It->Layer_Updated.resize(LayerCount_CurLane);
		}

		UBINT Status, IndexMask = 1, BlockSize_Current = this->FrameLen;
		for (UBINT i = 0; i < LayerCount_CurLane; i++){
			Status = this->RandSeed & (IndexMask - 1);
			if (0 == Status){
				// refresh this layer

				// check whether this layer exists
				if (_It->OutputBuffer_T.size() <= i){
					float *pNewData;
					for (UBINT j = _It->OutputBuffer_T.size(); j < i; j++){
						pNewData = nsBasic::MemAlloc_Arr_SIMD<float>(2 * this->GetBlockSizeByLayer(j));
						nsMath::setzero_simd_aligned(pNewData, this->GetBlockSizeByLayer(j));
						_It->OutputBuffer_T.push_back(pNewData);
						_It->Layer_Updated.push_back(false);
					}
					pNewData = nsBasic::MemAlloc_Arr_SIMD<float>(2 * this->GetBlockSizeByLayer(i));
					_It->OutputBuffer_T.push_back(pNewData);
					_It->Layer_Updated.push_back(false);
				}

				float *pBuffer_T = this->pFFT->GetIFFTBuffer_Dest(2 * BlockSize_Current);
				std::complex<float> *pBuffer_F = this->pFFT->GetIFFTBuffer(2 * BlockSize_Current);
				nsMath::setzero_simd_aligned(pBuffer_F, 2 * BlockSize_Current);

				UBINT FirstBlockIndex = i * Filter::LayerSize;
				if (!_It->Layer_Updated[i]){
					if (FirstBlockIndex < _It->BlockCount_Prev){
						UBINT ActualLayerSize;
						if (i == _It->LayerCount_Prev - 1)ActualLayerSize = _It->BlockCount_Prev - FirstBlockIndex;
						else ActualLayerSize = Filter::LayerSize;

						for (UBINT j = 0; j < ActualLayerSize; j++){
							if (nullptr != this->InputBuffer_F[FirstBlockIndex + j] && nullptr != _It->FilterBuffer_F_Prev[FirstBlockIndex + j]){
								nsMath::muladd_simd_aligned(pBuffer_F, this->InputBuffer_F[FirstBlockIndex + j], _It->FilterBuffer_F_Prev[FirstBlockIndex + j], BlockSize_Current + 1);
							}
						}
						this->pFFT->IFFT(_It->OutputBuffer_T[i], pBuffer_F, 2 * BlockSize_Current);
					}
					else nsMath::setzero_simd_aligned(_It->OutputBuffer_T[i], BlockSize_Current);

					nsMath::setzero_simd_aligned(pBuffer_F, 2 * BlockSize_Current);
					if (FirstBlockIndex < _It->BlockCount){
						UBINT ActualLayerSize;
						if (i == _It->LayerCount - 1)ActualLayerSize = _It->BlockCount - FirstBlockIndex;
						else ActualLayerSize = Filter::LayerSize;

						for (UBINT j = 0; j < ActualLayerSize; j++){
							if (nullptr != this->InputBuffer_F[FirstBlockIndex + j] && nullptr != _It->FilterBuffer_F[FirstBlockIndex + j]){
								nsMath::muladd_simd_aligned(pBuffer_F, this->InputBuffer_F[FirstBlockIndex + j], _It->FilterBuffer_F[FirstBlockIndex + j], BlockSize_Current + 1);
							}
						}
						this->pFFT->IFFT(pBuffer_T, pBuffer_F, 2 * BlockSize_Current);
					}
					else nsMath::setzero_simd_aligned(pBuffer_T, BlockSize_Current);

					// update FilterBuffer_F_Prev
					{
						UBINT ActualLayerSize;
						if (i == _It->LayerCount - 1)ActualLayerSize = _It->BlockCount - FirstBlockIndex;
						else ActualLayerSize = Filter::LayerSize;
						if (_It->LayerCount_Prev < i + 1)_It->LayerCount_Prev = i + 1;
						if (_It->BlockCount_Prev < FirstBlockIndex + ActualLayerSize)_It->BlockCount_Prev = FirstBlockIndex + ActualLayerSize;
						if (_It->FilterBuffer_F_Prev.size() < _It->BlockCount_Prev)_It->FilterBuffer_F_Prev.resize(_It->BlockCount_Prev, nullptr);

						if (i == _It->LayerCount_Prev - 1)ActualLayerSize = _It->BlockCount_Prev - FirstBlockIndex;
						else ActualLayerSize = Filter::LayerSize;

						for (UBINT j = 0; j < ActualLayerSize; j++){
							if (FirstBlockIndex + j < _It->FilterBuffer_F.size())std::swap(_It->FilterBuffer_F[FirstBlockIndex + j], _It->FilterBuffer_F_Prev[FirstBlockIndex + j]);
							else _It->FilterBuffer_F_Prev[FirstBlockIndex + j] = nullptr;
						}
					}

					float *pInterCoeff = this->pFFT->GetInterpCoeff(BlockSize_Current);
					nsMath::lerp_simd_aligned(_It->OutputBuffer_T[i], _It->OutputBuffer_T[i], pBuffer_T, pInterCoeff, BlockSize_Current);
					_It->Layer_Updated[i] = true;
				}
				else{
					if (FirstBlockIndex < _It->BlockCount_Prev){
						UBINT ActualLayerSize;
						if (i == _It->LayerCount_Prev - 1)ActualLayerSize = _It->BlockCount_Prev - FirstBlockIndex;
						else ActualLayerSize = Filter::LayerSize;

						for (UBINT j = 0; j < ActualLayerSize; j++){
							if (nullptr != this->InputBuffer_F[FirstBlockIndex + j] && nullptr != _It->FilterBuffer_F_Prev[FirstBlockIndex + j]){
								nsMath::muladd_simd_aligned(pBuffer_F, this->InputBuffer_F[FirstBlockIndex + j], _It->FilterBuffer_F_Prev[FirstBlockIndex + j], BlockSize_Current + 1);
								//nsMath::add_simd_aligned(pBuffer_F, pBuffer_F, this->InputBuffer_F[FirstBlockIndex + j], BlockSize_Current + 1);
							}
						}
						this->pFFT->IFFT(_It->OutputBuffer_T[i], pBuffer_F, 2 * BlockSize_Current);
					}
					else nsMath::setzero_simd_aligned(_It->OutputBuffer_T[i], BlockSize_Current);
				}
				nsMath::mul_simd_aligned(_It->OutputBuffer_T[i], _It->OutputBuffer_T[i], 1.0f / (2 * BlockSize_Current), BlockSize_Current);
			}
			if (_It->OutputBuffer_T.size() > i)nsMath::add_simd_aligned(_It->pOutput->pData, _It->pOutput->pData, &_It->OutputBuffer_T[i][Status * this->FrameLen], this->FrameLen);
			BlockSize_Current *= Filter::LayerSize;
			IndexMask *= Filter::LayerSize;
		}

		this->BlockCount_Prev = std::max(this->BlockCount_Prev, _It->BlockCount_Prev);
		this->LayerCount_Prev = std::max(this->LayerCount_Prev, _It->LayerCount_Prev);
	}
}
AudioRenderer::FilterLane *AudioRenderer::Filter::AddLane(OutputStream *pOutput){
	this->LaneList.emplace_back();
	FilterLane *RetValue = &this->LaneList.back();
	RetValue->BlockCount_Prev = 0;
	RetValue->LayerCount_Prev = 0;
	RetValue->pOutput = pOutput;
	return RetValue;
}
void AudioRenderer::Filter::UpdateLane(FilterLane *pLane, float *Data, UBINT Length){
	// reallocate
	UBINT NewLayerCount = this->GetLayerCount(Length);
	pLane->LayerCount = NewLayerCount;
	UBINT NewBlockCount = this->GetBlockCount(Length, NewLayerCount);
	pLane->BlockCount = NewBlockCount;

	if (pLane->FilterBuffer_F.size() < NewBlockCount)pLane->FilterBuffer_F.resize(NewBlockCount, nullptr);
	else{
		for (UBINT i = NewBlockCount; i < pLane->FilterBuffer_F.size(); i++){
			if (nullptr != pLane->FilterBuffer_F[i]){
				nsBasic::MemFree_Arr_SIMD(pLane->FilterBuffer_F[i], 2 * this->GetBlockSizeByIndex(i));
				pLane->FilterBuffer_F[i] = nullptr;
			}
		}
		pLane->FilterBuffer_F.resize(NewBlockCount);
	}

	// transform
	UBINT Pos_Current = 0;
	for (UBINT i = 0; i < pLane->FilterBuffer_F.size(); i++){
		UBINT BlockSize_Current = this->GetBlockSizeByIndex(i);
		if (Pos_Current < Length){
			if (nullptr == pLane->FilterBuffer_F[i]){
				pLane->FilterBuffer_F[i] = nsBasic::MemAlloc_Arr_SIMD<std::complex<float>>(2 * BlockSize_Current);
			}
			// new transform
			float *pBuffer_F = this->pFFT->GetFFTBuffer(2 * BlockSize_Current);
			if (Length < Pos_Current + BlockSize_Current){
				nsBasic::MemMove_Arr(pBuffer_F, &Data[Pos_Current], Length - Pos_Current);
				nsMath::setzero_simd_unaligned(&pBuffer_F[Length - Pos_Current], Pos_Current + BlockSize_Current - Length);
			}
			else nsBasic::MemMove_Arr(pBuffer_F, &Data[Pos_Current], BlockSize_Current);
			pFFT->FFT(pLane->FilterBuffer_F[i], pBuffer_F, 2 * BlockSize_Current);
		}
		else{
			if (nullptr != pLane->FilterBuffer_F[i]){
				nsBasic::MemFree_Arr_SIMD(pLane->FilterBuffer_F[i], 2 * BlockSize_Current);
				pLane->FilterBuffer_F[i] = nullptr;
			}
		}

		Pos_Current += BlockSize_Current;
	}
	for (UBINT i = 0; i < pLane->Layer_Updated.size(); i++)pLane->Layer_Updated[i] = false;
}
void AudioRenderer::Filter::EndUpdate(){
	this->LayerCount = 1;
	this->BlockCount = 1;
	for (auto _It = this->LaneList.begin(); _It != this->LaneList.end(); ++_It){
		this->LayerCount = std::max(this->LayerCount, _It->LayerCount);
		this->BlockCount = std::max(this->BlockCount, _It->BlockCount);
	}
}
void AudioRenderer::Filter::DeleteLane(FilterLane *pLane){
	for (UBINT i = 0; i < pLane->FilterBuffer_F.size(); i++){
		if (nullptr != pLane->FilterBuffer_F[i])nsBasic::MemFree_Arr_SIMD(pLane->FilterBuffer_F[i], 2 * this->GetBlockSizeByIndex(i));
	}
	for (UBINT i = 0; i < pLane->OutputBuffer_T.size(); i++){
		if (nullptr != pLane->OutputBuffer_T[i])nsBasic::MemFree_Arr_SIMD(pLane->OutputBuffer_T[i], 2 * this->GetBlockSizeByLayer(i));
	}
	this->LaneList.erase(pLane);
}
AudioRenderer::Filter::~Filter(){
	for (auto _It = this->LaneList.begin(); _It != this->LaneList.end(); ++_It){
		for (UBINT i = 0; i < _It->FilterBuffer_F.size(); i++){
			if (nullptr != _It->FilterBuffer_F[i])nsBasic::MemFree_Arr_SIMD(_It->FilterBuffer_F[i], 2 * this->GetBlockSizeByIndex(i));
		}
		for (UBINT i = 0; i < _It->OutputBuffer_T.size(); i++){
			if (nullptr != _It->OutputBuffer_T[i])nsBasic::MemFree_Arr_SIMD(_It->OutputBuffer_T[i], 2 * this->GetBlockSizeByLayer(i));
		}
	}
	for (UBINT i = 0; i < this->InputBuffer_T.size(); i++){
		if (nullptr != this->InputBuffer_T[i])nsBasic::MemFree_Arr_SIMD(this->InputBuffer_T[i], 2 * this->GetBlockSizeByIndex(i));
		if (nullptr != this->InputBuffer_F[i])nsBasic::MemFree_Arr_SIMD(this->InputBuffer_F[i], 2 * this->GetBlockSizeByIndex(i));
	}
}
//class AudioRenderer::Filter --END--

//class AudioRenderer --BEGIN--
void AudioRenderer::ExecMixCommand(float *pDest, nsContainer::Vector<MixCommand> *pCmdList){
	for (auto _It = pCmdList->begin(); _It != pCmdList->end(); ++_It){
		if (_It->Length != FrameLen){
			// doppler
			nsAudio::LinearSampler TmpSampler(FrameLen, _It->Length);
			TmpSampler.SetOutput(this->MixerBuffer, FrameLen);
			TmpSampler.StreamIn(&_It->pStream->Buffer.pData[_It->Start - _It->pStream->Buffer.Start], _It->Length);
			if (1.0f != _It->Volume)nsMath::muladd_simd_aligned(pDest, this->MixerBuffer, _It->Volume, FrameLen);
			else nsMath::add_simd_aligned(pDest, pDest, this->MixerBuffer, FrameLen);
			_It->Start += (BINT)_It->Length - FrameLen;
			_It->Length = FrameLen;
		}
		else{
			if (1.0f != _It->Volume)nsMath::muladd_simd_unaligned(pDest, &_It->pStream->Buffer.pData[_It->Start - _It->pStream->Buffer.Start], _It->Volume, FrameLen);
			else nsMath::add_simd_unaligned(pDest, pDest, &_It->pStream->Buffer.pData[_It->Start - _It->pStream->Buffer.Start], FrameLen);
		}
	}
}
void AudioRenderer::RenderBegin(){
	auto OutputSet_Locked = this->OutputSet.lock_write();
	for (auto _It = OutputSet_Locked->begin(); _It != OutputSet_Locked->end(); ++_It)nsMath::setzero_simd_aligned(_It->pData, this->FrameLen);
	this->OutputSet.unlock_write();
}

AudioRenderer::InputStream *AudioRenderer::AddInput(nsBasic::Stream_RS<unsigned char> *lpAudioStream, nsFormat::AudioReader *lpAudioDecoder, float Volume){
	auto InputSet_Locked = this->InputSet.lock_write();
	InputSet_Locked->emplace_back(lpAudioStream, lpAudioDecoder, Volume);
	InputStream *RetValue = &InputSet_Locked->back();
	this->InputSet.unlock_write();
	return RetValue;
}
inline void AudioRenderer::DeleteInput(const InputStream *pStream){
	this->InputSet.erase(pStream);
}
void AudioRenderer::ReadyForInput(){
	auto Set_Locked = this->InputSet.lock_write();
	for (auto _It = Set_Locked->begin(); _It != Set_Locked->end(); ++_It){
		_It->NextInterval_Start = 0;
		_It->NextInterval_End = 0;
	}
	auto FilterSet_Locked = this->FilterSet.lock_write();
	for (auto _It = FilterSet_Locked->begin(); _It != FilterSet_Locked->end(); ++_It){
		for (auto _It_Command = _It->CommandList.begin(); _It_Command != _It->CommandList.end(); ++_It_Command){
			_It_Command->pStream->NextInterval_Start = std::min(_It_Command->pStream->NextInterval_Start, _It_Command->Start);
			_It_Command->pStream->NextInterval_End = std::max(_It_Command->pStream->NextInterval_End, _It_Command->Start + (BINT)_It_Command->Length);
		}
	}
	this->FilterSet.unlock_write();

	auto OutputSet_Locked = this->OutputSet.lock_write();
	for (auto _It = OutputSet_Locked->begin(); _It != OutputSet_Locked->end(); ++_It){
		for (auto _It_Command = _It->CommandList.begin(); _It_Command != _It->CommandList.end(); ++_It_Command){
			_It_Command->pStream->NextInterval_Start = std::min(_It_Command->pStream->NextInterval_Start, _It_Command->Start);
			_It_Command->pStream->NextInterval_End = std::max(_It_Command->pStream->NextInterval_End, _It_Command->Start + (BINT)_It_Command->Length);
		}
	}
	this->OutputSet.unlock_write();
	this->InputSet.unlock_write();
}
void AudioRenderer::LoadInput(){
	auto Set_Locked = this->InputSet.lock_write();
	for (auto _It = Set_Locked->begin(); _It != Set_Locked->end(); ++_It){
		_It->Advance(this->FrameLen);
	}
	this->InputSet.unlock_write();
}

AudioRenderer::Filter *AudioRenderer::AddFilter(){
	auto FilterSet_Locked = this->FilterSet.lock_write();
	FilterSet_Locked->emplace_back(this->FrameLen, this->RandGenerator(), &this->MyFFTProvider);
	Filter *RetValue = &FilterSet_Locked->back();
	this->FilterSet.unlock_write();
	return RetValue;
}
inline void AudioRenderer::DeleteFilter(const Filter *pFilter){
	this->FilterSet.erase(pFilter);
}

void AudioRenderer::ExecuteFilter(){
	auto FilterSet_Locked = this->FilterSet.lock_write();
	UBINT InputPos = 0;
	auto InputSet_Locked = this->InputSet.lock_read();
	for (auto _It = FilterSet_Locked->begin(); _It != FilterSet_Locked->end(); ++_It){
		ExecMixCommand(_It->Advance(), &_It->CommandList);
		_It->ProcessInput();
		_It->Output();
		InputPos++;
	}
	this->InputSet.unlock_read();
	this->FilterSet.unlock_write();
}

AudioRenderer::OutputStream *AudioRenderer::AddOutput(){
	auto OutputSet_Locked = this->OutputSet.lock_write();
	OutputSet_Locked->emplace_back(this->FrameLen);
	OutputStream *RetValue = &OutputSet_Locked->back();
	this->OutputSet.unlock_write();
	return RetValue;
}
inline void AudioRenderer::DeleteOutput(const OutputStream *pStream){
	this->OutputSet.erase(pStream);
}

void AudioRenderer::ExecuteOutput(){
	auto InputSet_Locked = this->InputSet.lock_read();
	auto OutputSet_Locked = this->OutputSet.lock_write();
	UBINT InputPos = 0;
	for (auto _It = OutputSet_Locked->begin(); _It != OutputSet_Locked->end(); ++_It){
		ExecMixCommand(_It->pData, &_It->CommandList);
	}
	this->OutputSet.unlock_write();
	this->InputSet.unlock_read();
}

void AudioRenderer::Clear(){
	this->InputSet.clear();
	this->FilterSet.clear();
	this->OutputSet.clear();
}
//class AudioRenderer --END--
#endif