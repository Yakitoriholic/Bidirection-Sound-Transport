#ifndef CROSSOVER_FILTER
#define CROSSOVER_FILTER

#include "lGeneral.hpp"
#include "SoundMaterial.hpp"

class CrossoverFilter{
public:
	// 4th order Linkwitz-Riley Crossover, bilinear transformed

	// FloatVec_SIMD A_Lowpass[3]; A_Lowpass[0] = 1; A_Lowpass[1] = 2; A_Lowpass[2] = 1;
	// FloatVec_SIMD A_Highpass[3]; A_Highpass[0] = 1; A_Highpass[1] = -2; A_Highpass[2] = 1;
	FloatVec_SIMD B_Lowpass[3];
	FloatVec_SIMD B_Highpass[3];

	FloatVec_SIMD SBuffer_In[4][2]; // 2 for lowpass filter, 2 for highpass filter
	FloatVec_SIMD SBuffer_Out[4][2]; // the same

	// initialization
	CrossoverFilter(){
		FloatVec_SIMD omega_lowpass, omega_highpass, omega_lowpass_sqr, omega_highpass_sqr;
		omega_lowpass[0] = 2.0f * (float)M_PI * 62.5f * sqrt(2.0f) * 8.0f / ((float)FloatVec_SIMD::Width * (float)AudioSampleRate); //base frequency
		for (UBINT i = 1; i < FloatVec_SIMD::Width; i++)omega_lowpass[i] = omega_lowpass[i - 1] * 16.0f / (float)FloatVec_SIMD::Width;
		omega_highpass = omega_lowpass;
		omega_highpass.mul((float)FloatVec_SIMD::Width / 16.0f);
		
		// The filter has a low response at 20Hz and 20KHz. but the boundary below should not be used. Otherwise the output could be funny.
		//omega_highpass[0] = 2.0f * (float)M_PI * 20.0f / (float)AudioSampleRate;
		//omega_lowpass[7] = 2.0f * (float)M_PI * 22050.0f / (float)AudioSampleRate;

		// distortion correction for bilinear transform
		for (UBINT i = 0; i < FloatVec_SIMD::Width; i++)omega_lowpass[i] = 1.0f / tan(0.5f * omega_lowpass[i]);
		for (UBINT i = 0; i < FloatVec_SIMD::Width; i++)omega_highpass[i] = tan(0.5f * omega_highpass[i]);

		omega_lowpass_sqr = omega_lowpass; omega_lowpass_sqr.mul(omega_lowpass);
		omega_highpass_sqr = omega_highpass; omega_highpass_sqr.mul(omega_highpass);

		FloatVec_SIMD omega_lowpass_sqrt_2, omega_highpass_sqrt_2;
		omega_lowpass_sqrt_2 = omega_lowpass; omega_lowpass_sqrt_2.mul(sqrt(2.0f));
		omega_highpass_sqrt_2 = omega_highpass; omega_highpass_sqrt_2.mul(sqrt(2.0f));

		// lowpass filter
		B_Lowpass[0].set(1.0f); B_Lowpass[0].add(omega_lowpass_sqrt_2); B_Lowpass[0].add(omega_lowpass_sqr); //b_0 = 1 + B1*omega + omega_sqr;
		B_Lowpass[1].set(1.0f); B_Lowpass[1].sub(omega_lowpass_sqr); B_Lowpass[1].add(B_Lowpass[1]); //b_1 = 2 * (1 - omega_sqr);
		B_Lowpass[2].set(1.0f); B_Lowpass[2].sub(omega_lowpass_sqrt_2); B_Lowpass[2].add(omega_lowpass_sqr); //b_2 = 1 - B1*omega + omega_sqr;

		// highpass filter
		B_Highpass[0].set(1.0f); B_Highpass[0].add(omega_highpass_sqrt_2); B_Highpass[0].add(omega_highpass_sqr); //b_0 = 1 + B1*omega + omega_sqr;
		B_Highpass[1].set(-1.0f); B_Highpass[1].add(omega_highpass_sqr); B_Highpass[1].add(B_Highpass[1]); //b_1 = 2 * (omega_sqr - 1);
		B_Highpass[2].set(1.0f); B_Highpass[2].sub(omega_highpass_sqrt_2); B_Highpass[2].add(omega_highpass_sqr); //b_2 = 1 - B1*omega + omega_sqr;

		//let b_0 here be the reciprocal of itself. Otherwise we'll need divisions in the filter step.
		B_Lowpass[0].rcp(); B_Highpass[0].rcp();
	}
	
	// observer
	bool isNegligible() const{
		FloatVec_SIMD AbsVec, CompVec;
		CompVec.set(1e-4f);
		for (UBINT i = 0; i < 4; i++){
			AbsVec = SBuffer_In[i][0];
			AbsVec.abs();
			if (!(AbsVec.sum() < 1e-4f))return false;
			AbsVec = SBuffer_In[i][1];
			AbsVec.abs();
			if (!(AbsVec.sum() < 1e-4f))return false;
		}
		for (UBINT i = 0; i < 4; i++){
			AbsVec = SBuffer_Out[i][0];
			AbsVec.abs();
			if (!(AbsVec.sum() < 1e-4f))return false;
			AbsVec = SBuffer_Out[i][1];
			AbsVec.abs();
			if (!(AbsVec.sum() < 1e-4f))return false;
		}
		return true;
	}

	void Clear(){
		for (UBINT i = 0; i < 4; i++){ SBuffer_In[i][0].set(0.0f); SBuffer_In[i][1].set(0.0f); }
		for (UBINT i = 0; i < 4; i++){ SBuffer_Out[i][0].set(0.0f); SBuffer_Out[i][1].set(0.0f); }
	}
	FloatVec_SIMD FilterRound(FloatVec_SIMD& Input){
		FloatVec_SIMD TmpVec_Mul, TmpVec_Add, TmpVec_Result;
		// lowpass 1
		// A_Lowpass[0] = 1; A_Lowpass[1] = 2; A_Lowpass[2] = 1;
		TmpVec_Add = Input;
		TmpVec_Add.add(SBuffer_In[0][0]);
		TmpVec_Add.add(SBuffer_In[0][0]);
		TmpVec_Add.add(SBuffer_In[0][1]);
		SBuffer_In[0][1] = SBuffer_In[0][0]; SBuffer_In[0][0] = Input;

		TmpVec_Mul = SBuffer_Out[0][0];
		TmpVec_Mul.mul(B_Lowpass[1]);
		TmpVec_Add.sub(TmpVec_Mul);
		TmpVec_Mul = SBuffer_Out[0][1];
		TmpVec_Mul.mul(B_Lowpass[2]);
		TmpVec_Add.sub(TmpVec_Mul);
		TmpVec_Add.mul(B_Lowpass[0]);
		SBuffer_Out[0][1] = SBuffer_Out[0][0]; SBuffer_Out[0][0] = TmpVec_Add;
		TmpVec_Result = TmpVec_Add;

		// lowpass 2
		// A_Lowpass[0] = 1; A_Lowpass[1] = 2; A_Lowpass[2] = 1;
		TmpVec_Add = TmpVec_Result;
		TmpVec_Add.add(SBuffer_In[1][0]);
		TmpVec_Add.add(SBuffer_In[1][0]);
		TmpVec_Add.add(SBuffer_In[1][1]);
		SBuffer_In[1][1] = SBuffer_In[1][0]; SBuffer_In[1][0] = TmpVec_Result;

		TmpVec_Mul = SBuffer_Out[1][0];
		TmpVec_Mul.mul(B_Lowpass[1]);
		TmpVec_Add.sub(TmpVec_Mul);
		TmpVec_Mul = SBuffer_Out[1][1];
		TmpVec_Mul.mul(B_Lowpass[2]);
		TmpVec_Add.sub(TmpVec_Mul);
		TmpVec_Add.mul(B_Lowpass[0]);
		SBuffer_Out[1][1] = SBuffer_Out[1][0]; SBuffer_Out[1][0] = TmpVec_Add;
		TmpVec_Result = TmpVec_Add;

		// highpass 1
		// A_Highpass[0] = 1; A_Highpass[1] = -2; A_Highpass[2] = 1;
		TmpVec_Add = TmpVec_Result;
		TmpVec_Add.sub(SBuffer_In[2][0]);
		TmpVec_Add.sub(SBuffer_In[2][0]);
		TmpVec_Add.add(SBuffer_In[2][1]);
		SBuffer_In[2][1] = SBuffer_In[2][0]; SBuffer_In[2][0] = TmpVec_Result;

		TmpVec_Mul = SBuffer_Out[2][0];
		TmpVec_Mul.mul(B_Highpass[1]);
		TmpVec_Add.sub(TmpVec_Mul);
		TmpVec_Mul = SBuffer_Out[2][1];
		TmpVec_Mul.mul(B_Highpass[2]);
		TmpVec_Add.sub(TmpVec_Mul);
		TmpVec_Add.mul(B_Highpass[0]);
		SBuffer_Out[2][1] = SBuffer_Out[2][0]; SBuffer_Out[2][0] = TmpVec_Add;
		TmpVec_Result = TmpVec_Add;

		// highpass 2
		// A_Highpass[0] = 1; A_Highpass[1] = -2; A_Highpass[2] = 1;
		TmpVec_Add = TmpVec_Result;
		TmpVec_Add.sub(SBuffer_In[3][0]);
		TmpVec_Add.sub(SBuffer_In[3][0]);
		TmpVec_Add.add(SBuffer_In[3][1]);
		SBuffer_In[3][1] = SBuffer_In[3][0]; SBuffer_In[3][0] = TmpVec_Result;

		TmpVec_Mul = SBuffer_Out[3][0];
		TmpVec_Mul.mul(B_Highpass[1]);
		TmpVec_Add.sub(TmpVec_Mul);
		TmpVec_Mul = SBuffer_Out[3][1];
		TmpVec_Mul.mul(B_Highpass[2]);
		TmpVec_Add.sub(TmpVec_Mul);
		TmpVec_Add.mul(B_Highpass[0]);
		SBuffer_Out[3][1] = SBuffer_Out[3][0]; SBuffer_Out[3][0] = TmpVec_Add;

		return TmpVec_Add;
	}
	void StreamIn(float *Output, FloatVec_SIMD *Input, UBINT Length){
		if (FloatVec_SIMD::Width > 1){
			for (UBINT i = 0; i < Length; i++){
				FloatVec_SIMD FilterResult = FilterRound(*Input);
				*Output += FilterResult.sum_addsub(); // remember that two adjacent Linkwitz-Riley band filter has a 180 degree phase lag.
				Input++;
				Output++;
			}
		}
		else nsBasic::MemMove_Arr(Output, (float *)Input, Length);
	}
	void ZeroFill(float *Output, UBINT Length){
		if (FloatVec_SIMD::Width > 1){
			for (UBINT i = 0; i < Length; i++){
				FloatVec_SIMD ZeroInput;
				ZeroInput.set(0.0f);
				FloatVec_SIMD FilterResult = FilterRound(ZeroInput);
				*Output += FilterResult.sum_addsub();
				Output++;
			}
		}
		else nsMath::setzero_simd_unaligned(Output, Length);
	}
};

#endif