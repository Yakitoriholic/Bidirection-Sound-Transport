/* Description: Implementation of audio samplers.
 * Language:C++
 * Author:***
 */

#ifndef LIB_AUDIO_FILTER
#define LIB_AUDIO_FILTER

#include "lGeneral.hpp"

namespace nsAudio{

	class TrivialFilter: public StreamFilter{
		//This filter stream the input directly to the output.
	private:
		static const StreamFilter::Interface _I;

		static UBINT _StreamIn(StreamFilter * const ThisBase, const float *lpSrc, UBINT InputSampleCount);
		static UBINT _ZeroFill(StreamFilter * const ThisBase, UBINT InputSampleCount);
		static void _Flush(StreamFilter * const ThisBase);
		static void _Reset(StreamFilter * const ThisBase){}
		static void _Destroy(StreamFilter * const ThisBase){}

	public:
		TrivialFilter(){ this->lpInterface = const_cast<StreamFilter::Interface *>(&this->_I); }
	};

	const StreamFilter::Interface TrivialFilter::_I{
		TrivialFilter::_StreamIn,
		TrivialFilter::_ZeroFill,
		TrivialFilter::_Flush,
		TrivialFilter::_Reset,
		TrivialFilter::_Destroy
	};

	class LinearSampler: public StreamFilter{
		//A simple sampler which uses linear interpolation for up sampling.
	private:
		static const StreamFilter::Interface _I;

		static UBINT _StreamIn(StreamFilter * const ThisBase, const float *lpSrc, UBINT InputSampleCount);
		static UBINT _ZeroFill(StreamFilter * const ThisBase, UBINT InputSampleCount);
		static void _Flush(StreamFilter * const ThisBase);
		static void _Reset(StreamFilter * const ThisBase);
		static void _Destroy(StreamFilter * const ThisBase){}

		UBINT SampleDelay;

		UBINT Rate_Output;
		UBINT Rate_Input;
		float Pace_Float;
		UBINT Remainder_Bresenham;
		float Remainder_Float;
		float SampleBuffer[2];
	public:
		LinearSampler(UBINT Rate_Output, UBINT Rate_Input);
	};

	const StreamFilter::Interface LinearSampler::_I{
		LinearSampler::_StreamIn,
		LinearSampler::_ZeroFill,
		LinearSampler::_Flush,
		LinearSampler::_Reset,
		LinearSampler::_Destroy
	};

	class LinearDownSampler : public StreamFilter{
	private:
		static const StreamFilter::Interface _I;
		//A simple sampler which uses trilinear interpolation for down sampling.

		static UBINT _StreamIn(StreamFilter * const ThisBase, const float *lpSrc, UBINT InputSampleCount);
		static UBINT _ZeroFill(StreamFilter * const ThisBase, UBINT InputSampleCount){ return 0; }
		static void _Flush(StreamFilter * const ThisBase){}
		static void _Reset(StreamFilter * const ThisBase);
		static void _Destroy(StreamFilter * const ThisBase){}

		UBINT SampleRequest;
		UBINT ScaleLevel;
		
		UBINT Rate_Input;
		float Weight_Level1;
		float Weight_Level2;
		float LevelInterp_Ratio;

		UBINT Pace_Bresenham;
		float Pace_Float;
		UBINT Remainder_Bresenham;
		float Remainder_Float;
		float SampleBuffer_Level1[4];
		float SampleBuffer_Level2[2];
		UBINT SampleTick;

		bool OutputSample();
	public:
		LinearDownSampler(UBINT Rate_Output, UBINT Rate_Input);
	};

	const StreamFilter::Interface LinearDownSampler::_I{
		LinearDownSampler::_StreamIn,
		LinearDownSampler::_ZeroFill,
		LinearDownSampler::_Flush,
		LinearDownSampler::_Reset,
		LinearDownSampler::_Destroy
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	UBINT TrivialFilter::_StreamIn(StreamFilter * const ThisBase, const float *lpSrc, UBINT InputSampleCount){
		TrivialFilter *This = static_cast<TrivialFilter *>(ThisBase);

		if (InputSampleCount > This->OutputLen - This->OutputCounter)InputSampleCount = This->OutputLen - This->OutputCounter;
		memcpy(&This->lpDest[This->OutputCounter], lpSrc, InputSampleCount*sizeof(float));
		This->OutputCounter += InputSampleCount;
		return InputSampleCount;
	}
	UBINT TrivialFilter::_ZeroFill(StreamFilter * const ThisBase, UBINT InputSampleCount){
		TrivialFilter *This = static_cast<TrivialFilter *>(ThisBase);

		if (InputSampleCount > This->OutputLen - This->OutputCounter)InputSampleCount = This->OutputLen - This->OutputCounter;
		memset(&This->lpDest[This->OutputCounter], 0, InputSampleCount*sizeof(float));
		This->OutputCounter += InputSampleCount;
		return InputSampleCount;
	}
	void TrivialFilter::_Flush(StreamFilter * const ThisBase){
		TrivialFilter *This = static_cast<TrivialFilter *>(ThisBase);

		memset(&This->lpDest[This->OutputCounter], 0, (This->OutputLen - This->OutputCounter)*sizeof(float));
	}

	LinearSampler::LinearSampler(UBINT Rate_Output, UBINT Rate_Input){
		this->lpInterface = const_cast<StreamFilter::Interface *>(&this->_I);

		this->Rate_Output = Rate_Output;
		this->Rate_Input = Rate_Input;
		this->Pace_Float = (float)this->Rate_Input / (float)this->Rate_Output;
		this->SampleBuffer[0] = 0.0f;
		this->SampleDelay = 1;
	}
	UBINT LinearSampler::_StreamIn(StreamFilter * const ThisBase, const float *lpSrc, UBINT InputSampleCount){
		LinearSampler *This = static_cast<LinearSampler *>(ThisBase);

		UBINT InputPtr = 0;
		if (This->SampleDelay > 0){
			This->SampleBuffer[1] = lpSrc[InputPtr];
			This->Remainder_Bresenham = This->Rate_Input;
			This->Remainder_Float = This->Pace_Float;

			InputPtr++;
			This->SampleDelay--;
			if (InputPtr >= InputSampleCount)return InputPtr;
		}

		UBINT i;
		for (i = This->OutputCounter; i < This->OutputLen; i++){
			while (This->Remainder_Bresenham < This->Rate_Input){
				if (InputPtr >= InputSampleCount){
					This->OutputCounter = i;
					return InputPtr;
				}
				else{
					if (0 == This->Remainder_Bresenham)This->Remainder_Float = 0.0f; //correct the accumulated float error
					This->SampleBuffer[0] = This->SampleBuffer[1];
					This->SampleBuffer[1] = lpSrc[InputPtr];
					InputPtr++;
					This->Remainder_Bresenham += This->Rate_Output;
					This->Remainder_Float += 1.0f;
				}
			}
			This->Remainder_Bresenham -= This->Rate_Input;
			This->Remainder_Float -= This->Pace_Float;
			This->lpDest[i] = This->SampleBuffer[1] + (This->SampleBuffer[0] - This->SampleBuffer[1])*This->Remainder_Float;
		}
		This->OutputCounter = i;
		return InputPtr;
	}
	UBINT LinearSampler::_ZeroFill(StreamFilter * const ThisBase, UBINT InputSampleCount){
		LinearSampler *This = static_cast<LinearSampler *>(ThisBase);

		UBINT InputPtr = 0;
		if (This->SampleDelay > 0){
			This->SampleBuffer[1] = 0.0f;
			This->Remainder_Bresenham = This->Rate_Input;
			This->Remainder_Float = This->Pace_Float;

			InputPtr++;
			This->SampleDelay--;
			if (InputPtr >= InputSampleCount)return InputPtr;
		}

		UBINT i;
		for (i = This->OutputCounter; i < This->OutputLen; i++){
			while (This->Remainder_Bresenham < This->Rate_Input){
				if (InputPtr >= InputSampleCount){
					This->OutputCounter = i;
					return InputPtr;
				}
				else{
					if (0 == This->Remainder_Bresenham)This->Remainder_Float = 0.0f; //correct the accumulated float error
					This->SampleBuffer[0] = This->SampleBuffer[1];
					This->SampleBuffer[1] = 0.0f;
					InputPtr++;
					This->Remainder_Bresenham += This->Rate_Output;
					This->Remainder_Float += 1.0f;
				}
			}
			This->Remainder_Bresenham -= This->Rate_Input;
			This->Remainder_Float -= This->Pace_Float;
			This->lpDest[i] = This->SampleBuffer[1] + (This->SampleBuffer[0] - This->SampleBuffer[1])*This->Remainder_Float;
		}
		This->OutputCounter = i;
		return InputPtr;
	}
	void LinearSampler::_Flush(StreamFilter * const ThisBase){
		LinearSampler *This = static_cast<LinearSampler *>(ThisBase);

		if (This->SampleDelay > 0){
			This->SampleBuffer[1] = 0.0f;
			This->Remainder_Bresenham = This->Rate_Input;
			This->Remainder_Float = This->Pace_Float;
			This->SampleDelay = 0;
		}

		for (UBINT i = This->OutputCounter; i < This->OutputLen; i++){
			while (This->Remainder_Bresenham < This->Rate_Input){
				if (0 == This->Remainder_Bresenham)This->Remainder_Float = 0.0f; //correct the accumulated float error
				This->SampleBuffer[0] = This->SampleBuffer[1];
				This->SampleBuffer[1] = 0.0f;
				This->Remainder_Bresenham += This->Rate_Output;
				This->Remainder_Float += 1.0f;
			}
			This->Remainder_Bresenham -= This->Rate_Input;
			This->Remainder_Float -= This->Pace_Float;
			This->lpDest[i] = This->SampleBuffer[1] + (This->SampleBuffer[0] - This->SampleBuffer[1])*This->Remainder_Float;
		}
	}

	void LinearSampler::_Reset(StreamFilter * const ThisBase){
		LinearSampler *This = static_cast<LinearSampler *>(ThisBase);
		This->SampleBuffer[0] = 0.0f;
		This->SampleDelay = 1;
	}
	LinearDownSampler::LinearDownSampler(UBINT Rate_Output, UBINT Rate_Input){
		if (Rate_Input < Rate_Output)throw std::exception("Invalid sample rate.");

		this->lpInterface = const_cast<StreamFilter::Interface *>(&this->_I);

		this->Rate_Input = Rate_Input;
		this->ScaleLevel = nsMath::floor2power(this->Rate_Input / Rate_Output);
		this->SampleRequest = this->ScaleLevel * 2;

		this->Weight_Level1 = 1.0f / (float)this->ScaleLevel;
		this->Weight_Level2 = 1.0f / (float)this->SampleRequest;
		this->LevelInterp_Ratio = (this->Pace_Float - (float)this->ScaleLevel) / (float)this->ScaleLevel;
		
		this->Pace_Bresenham = Rate_Output * this->ScaleLevel;
		this->Remainder_Bresenham = this->Pace_Bresenham;
		this->Pace_Float = (float)this->Rate_Input / (float)(Rate_Output * this->ScaleLevel);
		this->Remainder_Float = this->Pace_Float;

		this->SampleBuffer_Level1[0] = 0.0f;
		this->SampleBuffer_Level1[1] = 0.0f;
		this->SampleBuffer_Level2[0] = 0.0f;
		this->SampleTick = 0;
	}
	bool LinearDownSampler::OutputSample(){
		float Level1_Sum, Level2_Sum;

		this->Remainder_Bresenham -= this->Pace_Bresenham;
		this->Remainder_Float -= this->Pace_Float;
		if (0 == this->Remainder_Bresenham)this->Remainder_Float = 0.0f; //correct the accumulated float error
		if (this->Remainder_Float > 1.0f){
			Level1_Sum = this->SampleBuffer_Level1[0] * (this->Remainder_Float - 1.0f);
			Level1_Sum += this->SampleBuffer_Level1[1] * (2.0f - this->Remainder_Float);
		}
		else Level1_Sum = this->SampleBuffer_Level1[2] + (this->SampleBuffer_Level1[1] - this->SampleBuffer_Level1[2]) * this->Remainder_Float;
		Level2_Sum = this->SampleBuffer_Level2[1] + (this->SampleBuffer_Level2[0] - this->SampleBuffer_Level2[1]) * this->Remainder_Float * 0.5f;
		this->lpDest[this->OutputCounter] = Level1_Sum + (Level2_Sum - Level1_Sum)*this->LevelInterp_Ratio;
		this->OutputCounter++;
		if (this->OutputCounter >= this->OutputLen)return true;

		if (this->Remainder_Bresenham >= this->Pace_Bresenham){
			this->Remainder_Bresenham -= this->Pace_Bresenham;
			this->Remainder_Float -= this->Pace_Float;
			if (0 == this->Remainder_Bresenham)this->Remainder_Float = 0.0f; //correct the accumulated float error
			Level1_Sum = this->SampleBuffer_Level1[2] + (this->SampleBuffer_Level1[1] - this->SampleBuffer_Level1[2]) * this->Remainder_Float;
			Level2_Sum = this->SampleBuffer_Level2[1] + (this->SampleBuffer_Level2[0] - this->SampleBuffer_Level2[1]) * this->Remainder_Float * 0.5f;
			this->lpDest[this->OutputCounter] = Level1_Sum + (Level2_Sum - Level1_Sum)*this->LevelInterp_Ratio;
			this->OutputCounter++;
			if (this->OutputCounter >= this->OutputLen)return true;
		}
		return false;
	}
	UBINT LinearDownSampler::_StreamIn(StreamFilter * const ThisBase, const float *lpSrc, UBINT InputSampleCount){
		LinearDownSampler *This = static_cast<LinearDownSampler *>(ThisBase);

		UBINT InputPtr = 0;
		if (This->SampleRequest > 0){
			for (UBINT i = 0; i < This->SampleRequest; i++){
				if (i >= InputSampleCount){ //If there aren't enough samples to fill up the sample buffer
					This->SampleRequest -= i;
					return InputSampleCount;
				}
				This->SampleBuffer_Level1[This->SampleTick + 2] += lpSrc[i];
				if (This->SampleRequest - i - 1 == This->ScaleLevel){
					This->SampleBuffer_Level1[2] *= This->Weight_Level1;
					This->SampleTick = 1;
					This->SampleBuffer_Level2[1] += This->SampleBuffer_Level1[2] * 0.5f;
				}
			}
			This->SampleBuffer_Level1[3] *= This->Weight_Level1;
			This->SampleTick = 0;
			This->SampleBuffer_Level2[1] += This->SampleBuffer_Level1[3] * 0.5f;

			if (This->OutputSample())return This->SampleRequest;

			InputPtr += This->SampleRequest;
			This->SampleRequest = 0;
		}

		//UBINT i;
		//for (i = This->OutputCounter; i < This->OutputLen; i++){
		//	if (This->Remainder_Bresenham < This->Rate_Input){
		//		if (InputPtr >= InputSampleCount)break;
		//		else{
		//			if (0 == This->Remainder_Bresenham)This->Remainder_Float = 0.0f; //correct the accumulated float error
		//			This->SampleBuffer[0] = This->SampleBuffer[1];
		//			This->SampleBuffer[1] = lpSrc[InputPtr];
		//			InputPtr++;
		//			This->Remainder_Bresenham += This->Rate_Output;
		//			This->Remainder_Float += 1.0f;
		//		}
		//	}
		//	This->Remainder_Bresenham -= This->Rate_Input;
		//	This->Remainder_Float -= This->Pace_Float;
		//	This->lpDest[i] = This->Remainder_Float*This->SampleBuffer[0] + (1.0f - This->Remainder_Float)*This->SampleBuffer[1];
		//}
		//This->OutputCounter = i;
		return InputPtr;
	}
	void LinearDownSampler::_Reset(StreamFilter * const ThisBase){
		LinearDownSampler *This = static_cast<LinearDownSampler *>(ThisBase);

		This->SampleRequest = This->ScaleLevel * 2;

		This->SampleBuffer_Level1[0] = 0.0f;
		This->SampleBuffer_Level1[1] = 0.0f;
		This->SampleBuffer_Level2[0] = 0.0f;
		This->SampleTick = 0;

		This->Remainder_Bresenham = This->Pace_Bresenham;
		This->Remainder_Float = This->Pace_Float;
	}
}
#endif