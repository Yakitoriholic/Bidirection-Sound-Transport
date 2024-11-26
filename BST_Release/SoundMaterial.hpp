#ifndef SOUND_MATERIAL
#define SOUND_MATERIAL

#include "lMath_SIMD.hpp"

struct FloatVec_SIMD8{
	static const UBINT Width = 8;
#if defined LIBENV_CPU_ACCEL_AVX
	__m256 Data;
#elif defined LIBENV_CPU_ACCEL_SSE
	__m128 Data[2];
#else
	float Data[8];
#endif

	inline float& operator[](UBINT n){
#if defined LIBENV_CPU_ACCEL_AVX
		return this->Data.m256_f32[n];
#elif defined LIBENV_CPU_ACCEL_SSE
		return this->Data[n >> 2].m128_f32[n & 3];
#else
		return this->Data[n];
#endif
	}
	inline const float& operator[](UBINT n) const{
#if defined LIBENV_CPU_ACCEL_AVX
		return this->Data.m256_f32[n];
#elif defined LIBENV_CPU_ACCEL_SSE
		return this->Data[n >> 2].m128_f32[n & 3];
#else
		return this->Data[n];
#endif
	}
	inline void loadu(const float *lpValue){
#if defined LIBENV_CPU_ACCEL_AVX
		this->Data = _mm256_loadu_ps(lpValue);
#elif defined LIBENV_CPU_ACCEL_SSE
		this->Data[0] = _mm_loadu_ps(&lpValue[0]);
		this->Data[1] = _mm_loadu_ps(&lpValue[4]);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] = lpValue[i];
#endif
	}
	inline void storeu(float *lpValue){
#if defined LIBENV_CPU_ACCEL_AVX
		_mm256_storeu_ps(lpValue, this->Data);
#elif defined LIBENV_CPU_ACCEL_SSE
		_mm_storeu_ps(&lpValue[0], this->Data[0]);
		_mm_storeu_ps(&lpValue[4], this->Data[1]);
#else
		for (UBINT i = 0; i < 8; i++)lpValue[i] = this->Data[i];
#endif
	}
	inline void set(float Value){
#if defined LIBENV_CPU_ACCEL_AVX
		this->Data = _mm256_broadcast_ss(&Value);
#elif defined LIBENV_CPU_ACCEL_SSE
		this->Data[0] = _mm_set_ps1(Value);
		this->Data[1] = this->Data[0];
#else
		for(UBINT i = 0; i < 8; i++)this->Data[i] = Value;
#endif
	}
	inline void add(const FloatVec_SIMD8& Value){
#if defined LIBENV_CPU_ACCEL_AVX
		this->Data = _mm256_add_ps(this->Data, Value.Data);
#elif defined LIBENV_CPU_ACCEL_SSE
		this->Data[0] = _mm_add_ps(this->Data[0], Value.Data[0]);
		this->Data[1] = _mm_add_ps(this->Data[1], Value.Data[1]);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] += Value.Data[i];
#endif
	}
	inline void sub(const FloatVec_SIMD8& Value){
#if defined LIBENV_CPU_ACCEL_AVX
		this->Data = _mm256_sub_ps(this->Data, Value.Data);
#elif defined LIBENV_CPU_ACCEL_SSE
		this->Data[0] = _mm_sub_ps(this->Data[0], Value.Data[0]);
		this->Data[1] = _mm_sub_ps(this->Data[1], Value.Data[1]);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] -= Value.Data[i];
#endif
	}
	inline void mul(const FloatVec_SIMD8& Value){
#if defined LIBENV_CPU_ACCEL_AVX
		this->Data = _mm256_mul_ps(this->Data, Value.Data);
#elif defined LIBENV_CPU_ACCEL_SSE
		this->Data[0] = _mm_mul_ps(this->Data[0], Value.Data[0]);
		this->Data[1] = _mm_mul_ps(this->Data[1], Value.Data[1]);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] *= Value.Data[i];
#endif
	}
	inline void mul(float Value){
#if defined LIBENV_CPU_ACCEL_AVX
		this->Data = _mm256_mul_ps(this->Data, _mm256_broadcast_ss(&Value));
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 Value_SSE = _mm_set_ps1(Value);
		this->Data[0] = _mm_mul_ps(this->Data[0], Value_SSE);
		this->Data[1] = _mm_mul_ps(this->Data[1], Value_SSE);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] *= Value;
#endif
	}
	inline bool less_than(const FloatVec_SIMD8& Value) const{
#if defined LIBENV_CPU_ACCEL_AVX
		__m256 CmpResult = _mm256_cmp_ps(this->Data, Value.Data, _CMP_GE_OS);

		if (0 != _mm256_movemask_ps(CmpResult))return false; else return true;
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 CmpResult0 = _mm_cmpge_ps(this->Data[0], Value.Data[0]);
		__m128 CmpResult1 = _mm_cmpge_ps(this->Data[0], Value.Data[1]);
		if (0 != (_mm_movemask_ps(CmpResult0) | _mm_movemask_ps(CmpResult1)))return false; else return true;
#else
		for (UBINT i = 0; i < 8; i++){
			if (this->Data[i] > Value.Data[i])return false;
		}
		return true;
#endif
	}
	inline bool less_than(float Value) const{
#if defined LIBENV_CPU_ACCEL_AVX
		__m256 Value_SSE = _mm256_broadcast_ss(&Value);
		__m256 CmpResult = _mm256_cmp_ps(this->Data, Value_SSE, _CMP_GE_OS);

		if (0 != _mm256_movemask_ps(CmpResult))return false; else return true;
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 Value_SSE = _mm_set_ps1(Value);
		__m128 CmpResult0 = _mm_cmpge_ps(this->Data[0], Value_SSE);
		__m128 CmpResult1 = _mm_cmpge_ps(this->Data[1], Value_SSE);
		if (0 != (_mm_movemask_ps(CmpResult0) | _mm_movemask_ps(CmpResult1)))return false; else return true;
#else
		for (UBINT i = 0; i < 8; i++){
			if(this->Data[i] > Value)return false;
		}
		return true;
#endif
	}
	inline void sqrt(){
#if defined LIBENV_CPU_ACCEL_AVX
		this->Data = _mm256_sqrt_ps(this->Data);
#elif defined LIBENV_CPU_ACCEL_SSE
		this->Data[0] = _mm_sqrt_ps(this->Data[0]);
		this->Data[1] = _mm_sqrt_ps(this->Data[1]);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] = sqrt(this->Data[i]);
#endif
	}
	inline void rcp(){
#if defined LIBENV_CPU_ACCEL_AVX
		float TmpVal = 1.0f;
		__m256 Value_SSE = _mm256_broadcast_ss(&TmpVal);
		this->Data = _mm256_div_ps(Value_SSE, this->Data);
#elif defined LIBENV_CPU_ACCEL_SSE
		float TmpVal = 1.0f;
		__m128 Value_SSE = _mm_set_ps1(TmpVal);
		this->Data[0] = _mm_div_ps(Value_SSE, this->Data[0]);
		this->Data[1] = _mm_div_ps(Value_SSE, this->Data[1]);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] = 1.0f / this->Data[i];
#endif
	}
	inline void abs(){
#if defined LIBENV_CPU_ACCEL_AVX
		__m256 Value_SSE = _mm256_broadcast_ss(&nsMath::NumericTrait<float>::NaN_Positive);
		this->Data = _mm256_and_ps(Value_SSE, this->Data);
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 Value_SSE = _mm_set_ps1(nsMath::NumericTrait<float>::NaN_Positive);
		this->Data[0] = _mm_and_ps(Value_SSE, this->Data[0]);
		this->Data[1] = _mm_and_ps(Value_SSE, this->Data[1]);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] = fabs(this->Data[i]);
#endif
	}
	inline float sum() const{
#if defined LIBENV_CPU_ACCEL_AVX
		__m128 Value_SSE_0 = _mm256_extractf128_ps(this->Data, 1);
		__m128 Value_SSE_1 = _mm256_extractf128_ps(this->Data, 0);
		Value_SSE_0 = _mm_add_ps(Value_SSE_0, Value_SSE_1);
		Value_SSE_1 = _mm_movehl_ps(Value_SSE_0, Value_SSE_0);
		Value_SSE_0 = _mm_add_ps(Value_SSE_1, Value_SSE_0);
		Value_SSE_1 = _mm_shuffle_ps(Value_SSE_0, Value_SSE_0, 1);
		return _mm_add_ss(Value_SSE_0, Value_SSE_1).m128_f32[0];
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 Value_SSE = _mm_add_ps(this->Data[0], this->Data[1]);
#if defined LIBENV_CPU_ACCEL_SSE3
		Value_SSE = _mm_hadd_ps(Value_SSE, Value_SSE);
		return Value_SSE.m128_f32[0] + Value_SSE.m128_f32[1];
#else
		return Value_SSE.m128_f32[0] + Value_SSE.m128_f32[1] + Value_SSE.m128_f32[2] + Value_SSE.m128_f32[3];
#endif
#else
		float TmpVal = this->Data[0];
		for (UBINT i = 1; i < 8; i++)TmpVal += this->Data[i];
		return TmpVal;
#endif
	}
	inline float sum_addsub() const{
#if defined LIBENV_CPU_ACCEL_AVX
		__m128 Value_SSE_0 = _mm256_extractf128_ps(this->Data, 1);
		__m128 Value_SSE_1 = _mm256_extractf128_ps(this->Data, 0);
		Value_SSE_0 = _mm_add_ps(Value_SSE_0, Value_SSE_1);
		Value_SSE_1 = _mm_movehl_ps(Value_SSE_0, Value_SSE_0);
		Value_SSE_0 = _mm_add_ps(Value_SSE_1, Value_SSE_0);
		Value_SSE_1 = _mm_shuffle_ps(Value_SSE_0, Value_SSE_0, 1);
		return _mm_sub_ss(Value_SSE_0, Value_SSE_1).m128_f32[0];
#elif defined LIBENV_CPU_ACCEL_SSE
		__m128 Value_SSE = _mm_add_ps(this->Data[0], this->Data[1]);
#if defined LIBENV_CPU_ACCEL_SSE3
		Value_SSE = _mm_hsub_ps(Value_SSE, Value_SSE);
		return Value_SSE.m128_f32[0] + Value_SSE.m128_f32[1];
#else
		return Value_SSE.m128_f32[0] - Value_SSE.m128_f32[1] + Value_SSE.m128_f32[2] - Value_SSE.m128_f32[3];
#endif
#else
		float TmpVal = this->Data[0];
		for (UBINT i = 1; i < 4; i++)TmpVal += this->Data[i * 2];
		for (UBINT i = 0; i < 4; i++)TmpVal -= this->Data[i * 2 + 1];
		return TmpVal;
#endif
	}
	inline void exp(){
#if defined LIBENV_CPU_ACCEL_SSE
		this->Data = nsMath::_exp_simd_AVX_lowres(this->Data);
#else
		for (UBINT i = 0; i < 8; i++)this->Data[i] = exp(this->Data[i]);
#endif
	}
};

struct FloatVec_SIMD4{
	static const UBINT Width = 4;
#if defined LIBENV_CPU_ACCEL_SSE
	__m128 Data;
#else
	float Data[4];
#endif

	inline float& operator[](UBINT n){
#if defined LIBENV_CPU_ACCEL_SSE
		return this->Data.m128_f32[n];
#else
		return this->Data[n];
#endif
	}
	inline const float& operator[](UBINT n) const{
#if defined LIBENV_CPU_ACCEL_SSE
		return this->Data.m128_f32[n];
#else
		return this->Data[n];
#endif
	}
	inline void loadu(const float *lpValue){
#if defined LIBENV_CPU_ACCEL_SSE
		this->Data = _mm_loadu_ps(lpValue);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] = lpValue[i];
#endif
	}
	inline void storeu(float *lpValue){
#if defined LIBENV_CPU_ACCEL_SSE
		_mm_storeu_ps(lpValue, this->Data);
#else
		for (UBINT i = 0; i < 4; i++)lpValue[i] = this->Data[i];
#endif
	}
	inline void set(float Value){
#if defined LIBENV_CPU_ACCEL_SSE
		this->Data = _mm_set_ps1(Value);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] = Value;
#endif
	}
	inline void add(const FloatVec_SIMD4& Value){
#if defined LIBENV_CPU_ACCEL_SSE
		this->Data = _mm_add_ps(this->Data, Value.Data);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] += Value.Data[i];
#endif
	}
	inline void sub(const FloatVec_SIMD4& Value){
#if defined LIBENV_CPU_ACCEL_SSE
		this->Data = _mm_sub_ps(this->Data, Value.Data);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] -= Value.Data[i];
#endif
	}
	inline void mul(const FloatVec_SIMD4& Value){
#if defined LIBENV_CPU_ACCEL_SSE
		this->Data = _mm_mul_ps(this->Data, Value.Data);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] *= Value.Data[i];
#endif
	}
	inline void mul(float Value){
#if defined LIBENV_CPU_ACCEL_SSE
		__m128 Value_SSE = _mm_set_ps1(Value);
		this->Data = _mm_mul_ps(this->Data, Value_SSE);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] *= Value;
#endif
	}
	inline bool less_than(const FloatVec_SIMD4& Value) const{
#if defined LIBENV_CPU_ACCEL_SSE
		__m128 CmpResult = _mm_cmpge_ps(this->Data, Value.Data);
		if (0 != _mm_movemask_ps(CmpResult))return false; else return true;
#else
		for (UBINT i = 0; i < 4; i++){
			if (this->Data[i] > Value.Data[i])return false;
		}
		return true;
#endif
	}
	inline bool less_than(float Value) const{
#if defined LIBENV_CPU_ACCEL_SSE
		__m128 Value_SSE = _mm_set_ps1(Value);
		__m128 CmpResult = _mm_cmpge_ps(this->Data, Value_SSE);
		if (0 != _mm_movemask_ps(CmpResult))return false; else return true;
#else
		for (UBINT i = 0; i < 4; i++){
			if (this->Data[i] > Value)return false;
		}
		return true;
#endif
	}
	inline void sqrt(){
#if defined LIBENV_CPU_ACCEL_SSE
		this->Data = _mm_sqrt_ps(this->Data);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] = sqrt(this->Data[i]);
#endif
	}
	inline void rcp(){
#if defined LIBENV_CPU_ACCEL_SSE
		float TmpVal = 1.0f;
		__m128 Value_SSE = _mm_set_ps1(TmpVal);
		this->Data = _mm_div_ps(Value_SSE, this->Data);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] = 1.0f / this->Data[i];
#endif
	}
	inline void abs(){
#if defined LIBENV_CPU_ACCEL_SSE
		__m128 Value_SSE = _mm_set_ps1(nsMath::NumericTrait<float>::NaN_Positive);
		this->Data = _mm_and_ps(Value_SSE, this->Data);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] = fabs(this->Data[i]);
#endif
	}
	inline float sum() const{
#if defined LIBENV_CPU_ACCEL_SSE
		return this->Data.m128_f32[0] + this->Data.m128_f32[1] + this->Data.m128_f32[2] + this->Data.m128_f32[3];
#else
		return this->Data[0] + this->Data[1] + this->Data[2] + this->Data[3];
#endif
	}
	inline float sum_addsub() const{
#if defined LIBENV_CPU_ACCEL_SSE
		return this->Data.m128_f32[0] - this->Data.m128_f32[1] + this->Data.m128_f32[2] - this->Data.m128_f32[3];
#else
		return this->Data[0] - this->Data[1] + this->Data[2] - this->Data[3];
#endif
	}
	inline void exp(){
#if defined LIBENV_CPU_ACCEL_SSE
		this->Data = nsMath::_exp_simd_SSE_lowres(this->Data);
#else
		for (UBINT i = 0; i < 4; i++)this->Data[i] = exp(this->Data[i]);
#endif
	}
};

struct FloatVec_SIMD1{
	static const UBINT Width = 1;
	float Data;

	inline float& operator[](UBINT n){ return this->Data; }
	inline const float& operator[](UBINT n) const{ return this->Data; }
	inline void loadu(const float *lpValue){ this->Data = *lpValue; }
	inline void storeu(float *lpValue){ *lpValue = this->Data; }
	inline void set(float Value){ this->Data = Value; }
	inline void add(const FloatVec_SIMD1& Value){ this->Data += Value.Data; }
	inline void sub(const FloatVec_SIMD1& Value){ this->Data -= Value.Data; }
	inline void mul(const FloatVec_SIMD1& Value){ this->Data *= Value.Data; }
	inline void mul(float Value){ this->Data *= Value; }
	inline bool less_than(const FloatVec_SIMD1& Value) const{ return this->Data < Value.Data; }
	inline bool less_than(float Value) const{ return this->Data < Value; }
	inline void sqrt(){ this->Data = std::sqrt(this->Data); }
	inline void rcp(){ this->Data = 1.0f / this->Data; }
	inline void abs(){ this->Data = std::abs(this->Data); }
	inline float sum() const{ return this->Data; }
	inline float sum_addsub() const{ return this->Data; }
	inline void exp(){ this->Data = std::exp(this->Data); }
};

typedef FloatVec_SIMD8 FloatVec_SIMD;

struct MaterialProperty_Sound_Surface{
	FloatVec_SIMD Diffuse;
	FloatVec_SIMD Specular;
};

struct MaterialProperty_Sound_Medium{
	FloatVec_SIMD DecayCoeff;
};

float SoundSpeed_Air(float temperature, float pressure, float humidity){
	// temperature is in Celsius degree(must be above 0), pressure is in Pascal(greater than 0). Humidity(relative) is in percent(0 ~ 100).

	// See the source code of http://www.npl.co.uk/acoustics/techguides/speedair for an implementation.

	// satuation vapor pressure formula (extended Antoine equation), accepted by CIPM in 1991.
	// see R.S. Davis, "Equation for the Determination of the Density of Moist Air(1981 / 91)".
	float temp_kelvin = temperature + 273.15f;
	float P_SV = exp(temp_kelvin * (temp_kelvin * 1.2378847e-5f - 1.9121316e-2f) + 33.93711047f - 6.3431645e3f / temp_kelvin);

	// fraction of carbon dioxide and water vapor
	float enhance_factor = 1.00062f + 3.14e-8f * pressure + 5.6e-7f * temperature * temperature; // 3.14 is irrelevant to pi here :)
	float X_w = (humidity / 100.0f) * enhance_factor * P_SV / pressure;

	float X_c = 0.0004f; // fraction of carbon dioxide

	// see Owen Cramer, "The variation of the specific heat ratio and the speed of sound in air with temperature, pressure, humidity, and CO2 concentration".
	float C0 = 331.5024f + 0.603055f * temperature - 5.28e-4f * temperature * temperature;
	float C1 = (51.471935f + 0.1495874f * temperature - 7.82e-4f * temperature * temperature) * X_w;
	float C2 = (-1.82e-7f + 3.73e-8f * temperature - 2.93e-10f * temperature * temperature) * pressure;
	float C3 = (-85.20931f - 0.228525f * temperature + 5.91e-5f * temperature * temperature) * X_c;
	float C4 = -2.835149f * X_w * X_w - 2.15e-13f * pressure * pressure + 29.179762f * X_c * X_c + 4.86e-4f * X_w * pressure * X_c;

	return C0 + C1 + C2 + C3 + C4;
}

float SoundAttCoeff_Air(float temperature, float pressure, float humidity, float frequency){
	// temperature is in Celsius degree(must be above 0), pressure is in Pascal(greater than 0). Humidity(relative) is in percent(0 ~ 100), frequency is in Hz.

	// see the source code of http://www.sengpielaudio.com/calculator-air.htm for an implementation.
	float temp_kelvin = temperature + 273.15f;
	float temp_relative = temp_kelvin / 293.15f;
	float pres_relative = pressure / 101325.0f;

	float P_SV_over_P_r = pow(10.0f, 4.6151f - 6.8346f * pow(273.16f / temp_kelvin, 1.261f)); // 273.16 is the triple point of water.
	// we can see that this formula is less accurate than that of R.S. Davis's. But whatever. THIS IS THE ISO STANDARD!
	float hum = humidity * P_SV_over_P_r / pres_relative;

	float fr_O = pres_relative * (24.0f + 40400.0f * hum * (0.02f + hum) / (0.391 + hum));
	float fr_N = pres_relative * (9.0f + 280.0f * hum * exp(4.17f - 4.17f * pow(temp_relative, -1.0f / 3.0f))) / sqrt(temp_relative);

	float X_C = 1.84e-11f * sqrt(temp_relative) / pres_relative;
	float X_O = 0.01275f * exp(-2239.1f / temp_kelvin) / (fr_O + (frequency * frequency / fr_O));
	float X_N = 0.1068f * exp(-3352.0f / temp_kelvin) / (fr_N + (frequency * frequency / fr_N));

	// Attenuation in dB/meter
	// float alpha = 20.0f * M_LOG10E * frequency * frequency * (X_C + pow(temp_relative, -2.5f) * (X_O + X_N));
	// return -(0.1f * alpha) * M_LN10;
	return -2.0f * frequency * frequency * (X_C + pow(temp_relative, -2.5f) * (X_O + X_N));
}

#endif