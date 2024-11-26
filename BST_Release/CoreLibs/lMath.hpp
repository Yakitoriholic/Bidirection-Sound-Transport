/* Description:Platform-independent functions for mathematical computations.
* Language:C++
* Author:***
*/

#ifndef LIB_MATH
#define LIB_MATH

#define _USE_MATH_DEFINES

#include "lGeneral.hpp"

namespace nsMath{
	template<class T> extern T invsqrt(const T scl); //returns the approximate (i.e. not the nearest available value) inversed square root of [scl]. Not recommmended when programming on any recent architectures.
	//DO NOT use invsqrt on integers!
	template<class T> extern void mul_complex(T *result, const T *val0, const T *val1); //get the product of two complex numbers. [val0], [val1] and [result] can be equal.
	//Format of complex numbers: [1 i]
	template<class T> extern void mul_quaternion(T *result, const T *val0, const T *val1); //get the product of two quaternions. [val0], [val1] and [result] can be equal.
	template<class T> extern void conj_quaternion(T *result, const T *val); //get the conjugate quaternion. [val] and [result] can overlap in the memory.
	//Format of quaternions: [1 i j k]
	template<UBINT I, class T> extern void add(T *result, const T *vec0, const T *vec1);
	//<I> stands for the length of the vector.
	//[result] = [vec0] + [vec1]. [result] can be equal to [vec0] or [vec1] when [vec0] doesn't overlap with [vec1].
	template<class T> extern void add2(const T *vec0, const T *vec1, T *result); //[result] = [vec0] + [vec1]. [result] can be equal to [vec0] or [vec1] when [vec0] doesn't overlap with [vec1].
	template<class T> extern void add3(const T *vec0, const T *vec1, T *result); //[result] = [vec0] + [vec1]. [result] can be equal to [vec0] or [vec1] when [vec0] doesn't overlap with [vec1].
	template<class T> extern void add4(const T *vec0, const T *vec1, T *result); //[result] = [vec0] + [vec1]. [result] can be equal to [vec0] or [vec1] when [vec0] doesn't overlap with [vec1].
	template<UBINT I, class T> extern void sub(T *result, const T *vec0, const T*vec1);
	//<I> stands for the length of the vector.
	//[result] = [vec0] + [vec1]. [result] can be equal to [vec0] or [vec1] when [vec0] doesn't overlap with [vec1].
	template<class T> extern void sub2(const T *vec0, const T *vec1, T *result); //[result] = [vec0] - [vec1]. [result] can be equal to [vec0] or [vec1] when [vec0] doesn't overlap with [vec1].
	template<class T> extern void sub3(const T *vec0, const T *vec1, T *result); //[result] = [vec0] - [vec1]. [result] can be equal to [vec0] or [vec1] when [vec0] doesn't overlap with [vec1].
	template<class T> extern void sub4(const T *vec0, const T *vec1, T *result); //[result] = [vec0] - [vec1]. [result] can be equal to [vec0] or [vec1] when [vec0] doesn't overlap with [vec1].
	template<class T> extern T det2(const T *Matrix); //returns the determinant of a 2*2 matrix.
	template<class T> extern T det3(const T *Matrix); //returns the determinant of a 3*3 matrix.
	template<class T> extern T det4(const T *Matrix); //returns the determinant of a 4*4 matrix.
	template<class T> extern T det2col(const T *col0, const T *col1); //returns the determinant of a 2*2 matrix given by 2 columns. This function can also serve as a 2d "cross-product".
	template<class T> extern T det3col(const T *col0, const T *col1, const T *col2); //returns the determinant of a 3*3 matrix given by 3 columns.
	template<class T> extern T det4col(const T *col0, const T *col1, const T *col2, const T *col3); //returns the determinant of a 4*4 matrix given by 4 columns.
	template<UBINT I, class T> extern T dot(const T *vec0, const T*vec1);
	//returns the dot product of two vectors of size <I>.
	template<class T> extern T dot2(const T *vec0, const T *vec1); //returns the dot product of two vectors of size 2.
	template<class T> extern T dot3(const T *vec0, const T *vec1); //returns the dot product of two vectors of size 3.
	template<class T> extern T dot4(const T *vec0, const T *vec1); //returns the dot product of two vectors of size 4.
	template<UBINT I, class T> extern void normalize(T *result, const T *vec);
	template<class T> extern void normalize2(const T *vec, T *result); //get the approximate normalized vector of size 2. [vec] and [result] can be equal.
	template<class T> extern void normalize3(const T *vec, T *result); //get the approximate normalized vector of size 3. [vec] and [result] can be equal.
	template<class T> extern void normalize4(const T *vec, T *result); //get the approximate normalized vector of size 4. [vec] and [result] can be equal.
	//DO NOT use normalize on integer vectors!
	template<class T> extern void cross3(T *result, const T *vec0, const T *vec1); //get the cross product of two vectors of size 3.
	template<class T> extern void mul2_sv(const T scl, const T *vec, T *result); //get the product of a vector and a scalar of size 2. [vec] and [result] can be equal.
	template<class T> extern void mul3_sv(const T scl, const T *vec, T *result); //get the product of a vector and a scalar of size 3. [vec] and [result] can be equal.
	template<class T> extern void mul4_sv(const T scl, const T *vec, T *result); //get the product of a vector and a scalar of size 4. [vec] and [result] can be equal.
	template<UBINT I, class T> extern void mul(T *result, const T *vec, const T scl);
	template<class T> extern void mul2_mv(const T *mat, const T *vec, T *result); //get the vector transformed by 2*2 matrix [mat] (column-major order). [vec] and [result] should not overlap in the memory.
	template<class T> extern void mul3_mv(const T *mat, const T *vec, T *result); //get the vector transformed by 3*3 matrix [mat] (column-major order). [vec] and [result] should not overlap in the memory.
	template<class T> extern void mul4_mv(const T *mat, const T *vec, T *result); //get the vector transformed by 4*4 matrix [mat] (column-major order). [vec] and [result] should not overlap in the memory.
	template<class T> extern void mul2_mm(const T *mat0, const T *mat1, T *result); //get the product of [mat0] * [mat1] (column-major order). [mat0], [mat1] and [result] should not overlap in the memory.
	template<class T> extern void mul3_mm(const T *mat0, const T *mat1, T *result); //get the product of [mat0] * [mat1] (column-major order). [mat0], [mat1] and [result] should not overlap in the memory.
	template<class T> extern void mul4_mm(const T *mat0, const T *mat1, T *result); //get the product of [mat0] * [mat1] (column-major order). [mat0], [mat1] and [result] should not overlap in the memory.
	template<class T> extern void identity2(T *result);  //get the 2*2 identity matrix.
	template<class T> extern void identity3(T *result);  //get the 3*3 identity matrix.
	template<class T> extern void identity4(T *result);  //get the 4*4 identity matrix.
	template<class T> extern void transpose2(const T *mat, T *result);  //get the transposed 2*2 matrix. [mat] and [result] should not overlap in the memory.
	template<class T> extern void transpose3(const T *mat, T *result);  //get the transposed 3*3 matrix. [mat] and [result] should not overlap in the memory.
	template<class T> extern void transpose4(const T *mat, T *result);  //get the transposed 4*4 matrix. [mat] and [result] should not overlap in the memory.
	template<class T> extern T inv2(T *result, const T *mat);  //returns the determinant of the original 2*2 matrix. the inversed 2*2 matrix is stored in [result]. [mat] and [result] can overlap in the memory.
	template<class T> extern T inv3(T *result, const T *mat);  //returns the determinant of the original 3*3 matrix. the inversed 3*3 matrix is stored in [result]. [mat] and [result] can overlap in the memory.
	template<class T> extern T inv4(T *result, const T *mat);  //returns the determinant of the original 4*4 matrix. the inversed 4*4 matrix is stored in [result]. [mat] and [result] can overlap in the memory.
	//DO NOT use inverse on integer matrices!
	template<class T> extern void vert2(T *result, const T *vec); //get a vector that is vertical to the given vector of size 3. the vector returned is not normalized. [vec] and [result] should not overlap in the memory.
	template<class T> extern void vert3(T *result, const T *vec); //get a vector that is vertical to the given vector of size 3. the vector returned is not normalized. [vec] and [result] should not overlap in the memory.
	template<class T> extern void vert4(T *result, const T *vec); //get a vector that is vertical to the given vector of size 3. the vector returned is not normalized. [vec] and [result] should not overlap in the memory.
	template<class T> extern void slerp2(T *result, const T scl, const T *vec0, const T *vec1); //get the linear interpolated result of [vec0] and [vec1] in the 2D spherical space. [vec0] and [vec1] is required to be normalized and [scl] should be in [0,1].
	template<class T> extern void slerp3(T *result, const T scl, const T *vec0, const T *vec1); //get the linear interpolated result of [vec0] and [vec1] in the 3D spherical space. [vec0] and [vec1] is required to be normalized and [scl] should be in [0,1].
	template<class T> extern void slerp4(T *result, const T scl, const T *vec0, const T *vec1); //get the linear interpolated result of [vec0] and [vec1] in the 4D spherical space. [vec0] and [vec1] is required to be normalized and [scl] should be in [0,1].
	//DO NOT use slerp on integer vectors!
	/*-------------------------------- IMPLEMENTATION --------------------------------*/
	extern float invsqrt(const float scl){
#ifdef LIBENV_CPU_ACCEL_SSE
		__m128 TmpReg=_mm_load1_ps(&scl);
		TmpReg = _mm_rsqrt_ps(TmpReg);
		return TmpReg.m128_f32[0];
#else
		//Carmack's fast inverse root, optimized by Matthew Robertson. (Robertson M. A Brief History of InvSqrt[D]. UNIVERSITY OF NEW BRUNSWICK, 2012.)
		//Works on little-endian architectures only.
		UINT4b i;
		float x2, y;
		x2 = scl * 0.5f;
		y = scl;
		i = *(UINT4b *)&y;
		i = 0x5F375A86 - (i >> 1);
		y = *(float *)&i;
		y = y * (1.5f - (x2 * y * y)); //iteration. Duplicate this line if necessary.
		return y;
#endif
	}
	extern double invsqrt(const double scl){
		//Carmack's fast inverse root, optimized by Matthew Robertson. (Robertson M. A Brief History of InvSqrt[D]. UNIVERSITY OF NEW BRUNSWICK, 2012.)
		//Works on little-endian architectures only.
		UINT8b i;
		double x2, y;
		x2 = scl * 0.5;
		y = scl;
		i = *(UINT8b *)&y;
		i = 0x5FE6EB50C7B537A9 - (i >> 1);
		y = *(double *)&i;
		y = y * (1.5 - (x2 * y * y)); //iteration. Duplicate this line if necessary.
		return y;
	}
	template<class T> extern inline void mul_complex(T *result, const T *val0, const T *val1){
		//cost:mul * 4, add * 2
		//Some results are stored temporally in case of memory overlapping.
		T realpart = val0[0] * val1[0] - val0[1] * val1[1];
		result[1] = val0[0] * val1[1] + val0[1] * val1[0];
		result[0] = realpart;
	}
	template<class T> extern void mul_quaternion(T *result, const T *val0, const T *val1){
		//cost:mul * 16, add * 12
		//Some results are stored temporally in case of memory overlapping.
		T part_i = val0[0] * val1[1] + val0[1] * val1[0] + val0[2] * val1[3] - val0[3] * val1[2];
		T part_j = val0[0] * val1[2] + val0[2] * val1[0] + val0[3] * val1[1] - val0[1] * val1[3];
		T part_k = val0[0] * val1[3] + val0[3] * val1[0] + val0[1] * val1[2] - val0[2] * val1[1];

		result[0] = val0[0] * val1[0] - val0[1] * val1[1] - val0[2] * val1[2] - val0[3] * val1[3];
		result[1] = part_i;
		result[2] = part_j;
		result[3] = part_j;
	}
	template<class T> extern void conj_quaternion(T *result, const T *val){
		result[0] = val[0]; result[1] = -val[1]; result[2] = -val[2]; result[3] = -val[3];
	}
	template<class T> extern void add2(const T *vec0, const T *vec1, T *result){
		//cost:add * 2
		result[0] = vec0[0] + vec1[0];
		result[1] = vec0[1] + vec1[1];
	}
	template<class T> extern void add3(const T *vec0, const T *vec1, T *result){
		//cost:add * 3
		result[0] = vec0[0] + vec1[0];
		result[1] = vec0[1] + vec1[1];
		result[2] = vec0[2] + vec1[2];
	}
	template<class T> extern void add4(const T *vec0, const T *vec1, T *result){
		//cost:add * 4
		result[0] = vec0[0] + vec1[0];
		result[1] = vec0[1] + vec1[1];
		result[2] = vec0[2] + vec1[2];
		result[2] = vec0[3] + vec1[3];
	}
	template<UBINT I, class T> extern inline void add(T *result, const T *vec0, const T*vec1){
		for (UBINT i = 0; i < I; i++)result[i] = vec0[i] + vec1[i];
	}
	template<class T> extern void sub2(const T *vec0, const T *vec1, T *result){
		//cost:add * 2
		result[0] = vec0[0] - vec1[0];
		result[1] = vec0[1] - vec1[1];
	}
	template<class T> extern void sub3(const T *vec0, const T *vec1, T *result){
		//cost:add * 3
		result[0] = vec0[0] - vec1[0];
		result[1] = vec0[1] - vec1[1];
		result[2] = vec0[2] - vec1[2];
	}
	template<class T> extern void sub4(const T *vec0, const T *vec1, T *result){
		//cost:add * 4
		result[0] = vec0[0] - vec1[0];
		result[1] = vec0[1] - vec1[1];
		result[2] = vec0[2] - vec1[2];
		result[3] = vec0[3] - vec1[3];
	}
	template<UBINT I, class T> extern inline void sub(T *result, const T *vec0, const T*vec1){
		for (UBINT i = 0; i < I; i++)result[i] = vec0[i] - vec1[i];
	}
	template<class T> extern T det2(const T *Matrix){
		//cost:mul * 2, add * 1
		return Matrix[0] * Matrix[3] - Matrix[1] * Matrix[2];
	}
	template<class T> extern T det3(const T *Matrix){
		//cost:mul * 9, add * 5
		return Matrix[0] * (Matrix[4] * Matrix[8] - Matrix[5] * Matrix[7])
			+ Matrix[1] * (Matrix[5] * Matrix[6] - Matrix[3] * Matrix[8])
			+ Matrix[2] * (Matrix[3] * Matrix[7] - Matrix[4] * Matrix[6]);
	}
	template<class T> extern T det4(const T *Matrix){
		//cost:mul * 28, add * 17
		//This algorithm is faster than Gaussian elimination and requires less temporal storage space. it accelerate the computation through minor matrix caching.
		T MinorCache[6];
		MinorCache[0] = Matrix[2 * 4 + 0] * Matrix[3 * 4 + 1] - Matrix[2 * 4 + 1] * Matrix[3 * 4 + 0];
		MinorCache[1] = Matrix[2 * 4 + 0] * Matrix[3 * 4 + 2] - Matrix[2 * 4 + 2] * Matrix[3 * 4 + 0];
		MinorCache[2] = Matrix[2 * 4 + 0] * Matrix[3 * 4 + 3] - Matrix[2 * 4 + 3] * Matrix[3 * 4 + 0];
		MinorCache[3] = Matrix[2 * 4 + 1] * Matrix[3 * 4 + 2] - Matrix[2 * 4 + 2] * Matrix[3 * 4 + 1];
		MinorCache[4] = Matrix[2 * 4 + 1] * Matrix[3 * 4 + 3] - Matrix[2 * 4 + 3] * Matrix[3 * 4 + 1];
		MinorCache[5] = Matrix[2 * 4 + 2] * Matrix[3 * 4 + 3] - Matrix[2 * 4 + 3] * Matrix[3 * 4 + 2];

		return Matrix[0] * (Matrix[1 * 4 + 1] * MinorCache[5] - Matrix[1 * 4 + 2] * MinorCache[4] + Matrix[1 * 4 + 3] * MinorCache[3])
			- Matrix[1] * (Matrix[1 * 4 + 0] * MinorCache[5] - Matrix[1 * 4 + 2] * MinorCache[2] + Matrix[1 * 4 + 3] * MinorCache[1])
			+ Matrix[2] * (Matrix[1 * 4 + 0] * MinorCache[4] - Matrix[1 * 4 + 1] * MinorCache[2] + Matrix[1 * 4 + 3] * MinorCache[0])
			- Matrix[3] * (Matrix[1 * 4 + 0] * MinorCache[3] - Matrix[1 * 4 + 1] * MinorCache[1] + Matrix[1 * 4 + 2] * MinorCache[0]);
	}
	template<class T> extern T det2col(const T *col0, const T *col1){
		//cost:mul * 2, add * 1
		return col0[0] * col1[1] - col1[0] * col0[1];
	}
	template<class T> extern T det3col(const T *col0, const T *col1, const T *col2){
		//cost:mul * 9, add * 5
		return col0[0] * (col1[1] * col2[2] - col1[2] * col2[1])
			+ col0[1] * (col1[2] * col2[0] - col1[0] * col2[2])
			+ col0[2] * (col1[0] * col2[1] - col1[1] * col2[0]);
	}
	template<class T> extern T det4col(const T *col0, const T *col1, const T *col2, const T *col3){
		//cost:mul * 28, add * 17
		//This algorithm is faster than Gaussian elimination and requires less temporal storage space. it accelerate the computation through minor matrix caching.
		T MinorCache[6];
		MinorCache[0] = col2[0] * col3[1] - col2[1] * col3[0];
		MinorCache[1] = col2[0] * col3[2] - col2[2] * col3[0];
		MinorCache[2] = col2[0] * col3[3] - col2[3] * col3[0];
		MinorCache[3] = col2[1] * col3[2] - col2[2] * col3[1];
		MinorCache[4] = col2[1] * col3[3] - col2[3] * col3[1];
		MinorCache[5] = col2[2] * col3[3] - col2[3] * col3[2];

		return col0[0] * (col1[1] * MinorCache[5] - col1[2] * MinorCache[4] + col1[3] * MinorCache[3])
			- col0[1] * (col1[0] * MinorCache[5] - col1[2] * MinorCache[2] + col1[3] * MinorCache[1])
			+ col0[2] * (col1[0] * MinorCache[4] - col1[1] * MinorCache[2] + col1[3] * MinorCache[0])
			- col0[3] * (col1[0] * MinorCache[3] - col1[1] * MinorCache[1] + col1[2] * MinorCache[0]);
	}
	template<UBINT I, class T> extern inline T dot(const T *vec0, const T *vec1){
		T Result = (T)0;
		for (UBINT i = 0; i < I; i++)Result += vec0[i] * vec1[i];
		return Result;
	}
	template<class T> extern T dot2(const T *vec0, const T *vec1){
		//cost:mul * 2, add * 1
		return vec0[0] * vec1[0] + vec0[1] * vec1[1];
	}
	template<class T> extern T dot3(const T *vec0, const T *vec1){
		//cost:mul * 3, add * 2
		return vec0[0] * vec1[0] + vec0[1] * vec1[1] + vec0[2] * vec1[2];
	}
	template<class T> extern T dot4(const T *vec0, const T *vec1){
		//cost:mul * 4, add * 3
		return vec0[0] * vec1[0] + vec0[1] * vec1[1] + vec0[2] * vec1[2] + vec0[3] * vec1[3];
	}
	template<UBINT I, class T> extern inline void normalize(T *result, const T *vec){
		T length_sqr = (T)0;
		for (UBINT i = 0; i < I; i++)length_sqr += vec[i] * vec[i];
		mul<I, T>(result, vec, (T)1 / sqrt(length_sqr));
	}
	template<class T> extern void normalize2(const T *vec, T *result){
		T length_rcp = (T)1.0 / sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
		mul2_sv(length_rcp, vec, result);
	}
	template<class T> extern void normalize3(const T *vec, T *result){
		T length_rcp = (T)1.0 / sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
		mul3_sv(length_rcp, vec, result);
	}
	template<class T> extern void normalize4(const T *vec, T *result){
		T length_rcp = (T)1.0 / sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] + vec[3] * vec[3]);
		mul4_sv(length_rcp, vec, result);
	}
	template<class T> extern void cross3(T *result, const T *vec0, const T *vec1){
		//cost:mul * 6, add * 3
		result[0] = vec0[1] * vec1[2] - vec0[2] * vec1[1];
		result[1] = vec0[2] * vec1[0] - vec0[0] * vec1[2];
		result[2] = vec0[0] * vec1[1] - vec0[1] * vec1[0];
	}
	template<class T> extern void mul2_sv(const T scl, const T *vec, T *result){
		//cost:mul * 2
		result[0] = scl*vec[0]; result[1] = scl*vec[1];
	}
	template<class T> extern void mul3_sv(const T scl, const T *vec, T *result){
		//cost:mul * 3
		result[0] = scl*vec[0]; result[1] = scl*vec[1]; result[2] = scl*vec[2];
	}
	template<class T> extern void mul4_sv(const T scl, const T *vec, T *result){
		//cost:mul * 4
		result[0] = scl*vec[0]; result[1] = scl*vec[1]; result[2] = scl*vec[2]; result[3] = scl*vec[3];
	}
	template<UBINT I, class T> extern inline void mul(T *result, const T *vec, const T scl){
		for (UBINT i = 0; i < I; i++)result[i] = vec[i] * scl;
	}
	template<class T> extern void mul2_mv(const T *mat, const T *vec, T *result){
		//cost:mul * 4, add * 2
		result[0] = mat[0] * vec[0] + mat[2] * vec[1];
		result[1] = mat[1] * vec[0] + mat[3] * vec[1];
	}
	template<class T> extern void mul3_mv(const T *mat, const T *vec, T *result){
		//cost:mul * 9, add * 6
		result[0] = mat[0] * vec[0] + mat[3] * vec[1] + mat[6] * vec[2];
		result[1] = mat[1] * vec[0] + mat[4] * vec[1] + mat[7] * vec[2];
		result[2] = mat[2] * vec[0] + mat[5] * vec[1] + mat[8] * vec[2];
	}
	template<class T> extern void mul4_mv(const T *mat, const T *vec, T *result){
		//cost:mul * 16, add * 12
		result[0] = mat[0] * vec[0] + mat[4] * vec[1] + mat[8] * vec[2] + mat[12] * vec[3];
		result[1] = mat[1] * vec[0] + mat[5] * vec[1] + mat[9] * vec[2] + mat[13] * vec[3];
		result[2] = mat[2] * vec[0] + mat[6] * vec[1] + mat[10] * vec[2] + mat[14] * vec[3];
		result[3] = mat[3] * vec[0] + mat[7] * vec[1] + mat[11] * vec[2] + mat[15] * vec[3];
	}
	template<class T> extern void mul2_mm(const T *mat0, const T *mat1, T *result){
		//cost:mul * 8, add * 4
		result[0] = mat0[0] * mat1[0] + mat0[2] * mat1[1];
		result[1] = mat0[1] * mat1[0] + mat0[3] * mat1[1];
		result[2] = mat0[0] * mat1[2] + mat0[2] * mat1[3];
		result[3] = mat0[1] * mat1[2] + mat0[3] * mat1[3];
	}
	template<class T> extern void mul3_mm(const T *mat0, const T *mat1, T *result){
		//cost:mul * 27, add * 18
		result[0] = mat0[0] * mat1[0] + mat0[3] * mat1[1] + mat0[6] * mat1[2];
		result[1] = mat0[1] * mat1[0] + mat0[4] * mat1[1] + mat0[7] * mat1[2];
		result[2] = mat0[2] * mat1[0] + mat0[5] * mat1[1] + mat0[8] * mat1[2];
		result[3] = mat0[0] * mat1[3] + mat0[3] * mat1[4] + mat0[6] * mat1[5];
		result[4] = mat0[1] * mat1[3] + mat0[4] * mat1[4] + mat0[7] * mat1[5];
		result[5] = mat0[2] * mat1[3] + mat0[5] * mat1[4] + mat0[8] * mat1[5];
		result[6] = mat0[0] * mat1[6] + mat0[3] * mat1[7] + mat0[6] * mat1[8];
		result[7] = mat0[1] * mat1[6] + mat0[4] * mat1[7] + mat0[7] * mat1[8];
		result[8] = mat0[2] * mat1[6] + mat0[5] * mat1[7] + mat0[8] * mat1[8];
	}
	template<class T> extern void mul4_mm(const T *mat0, const T *mat1, T *result){
		//cost:mul * 64, add * 48
		result[0] = mat0[0] * mat1[0] + mat0[4] * mat1[1] + mat0[8] * mat1[2] + mat0[12] * mat1[3];
		result[1] = mat0[1] * mat1[0] + mat0[5] * mat1[1] + mat0[9] * mat1[2] + mat0[13] * mat1[3];
		result[2] = mat0[2] * mat1[0] + mat0[6] * mat1[1] + mat0[10] * mat1[2] + mat0[14] * mat1[3];
		result[3] = mat0[3] * mat1[0] + mat0[7] * mat1[1] + mat0[11] * mat1[2] + mat0[15] * mat1[3];
		result[4] = mat0[0] * mat1[4] + mat0[4] * mat1[5] + mat0[8] * mat1[6] + mat0[12] * mat1[7];
		result[5] = mat0[1] * mat1[4] + mat0[5] * mat1[5] + mat0[9] * mat1[6] + mat0[13] * mat1[7];
		result[6] = mat0[2] * mat1[4] + mat0[6] * mat1[5] + mat0[10] * mat1[6] + mat0[14] * mat1[7];
		result[7] = mat0[3] * mat1[4] + mat0[7] * mat1[5] + mat0[11] * mat1[6] + mat0[15] * mat1[7];
		result[8] = mat0[0] * mat1[8] + mat0[4] * mat1[9] + mat0[8] * mat1[10] + mat0[12] * mat1[11];
		result[9] = mat0[1] * mat1[8] + mat0[5] * mat1[9] + mat0[9] * mat1[10] + mat0[13] * mat1[11];
		result[10] = mat0[2] * mat1[8] + mat0[6] * mat1[9] + mat0[10] * mat1[10] + mat0[14] * mat1[11];
		result[11] = mat0[3] * mat1[8] + mat0[7] * mat1[9] + mat0[11] * mat1[10] + mat0[15] * mat1[11];
		result[12] = mat0[0] * mat1[12] + mat0[4] * mat1[13] + mat0[8] * mat1[14] + mat0[12] * mat1[15];
		result[13] = mat0[1] * mat1[12] + mat0[5] * mat1[13] + mat0[9] * mat1[14] + mat0[13] * mat1[15];
		result[14] = mat0[2] * mat1[12] + mat0[6] * mat1[13] + mat0[10] * mat1[14] + mat0[14] * mat1[15];
		result[15] = mat0[3] * mat1[12] + mat0[7] * mat1[13] + mat0[11] * mat1[14] + mat0[15] * mat1[15];
	}
	template<class T> extern void identity2(T *result){
		result[0] = 1; result[1] = 0;
		result[2] = 0; result[3] = 1;
	}
	template<class T> extern void identity3(T *result){
		result[0] = 1; result[1] = 0; result[2] = 0;
		result[3] = 0; result[4] = 1; result[5] = 0;
		result[6] = 0; result[7] = 0; result[8] = 1;
	}
	template<class T> extern void identity4(T *result){
		result[0] = 1; result[1] = 0; result[2] = 0; result[3] = 0;
		result[4] = 0; result[5] = 1; result[6] = 0; result[7] = 0;
		result[8] = 0; result[9] = 0; result[10] = 1; result[11] = 0;
		result[12] = 0; result[13] = 0; result[14] = 0; result[15] = 1;
	}
	template<class T> extern void transpose2(const T *mat, T *result){
		result[0] = mat[0]; result[1] = mat[2];
		result[2] = mat[1]; result[3] = mat[3];
	}
	template<class T> extern void transpose3(const T *mat, T *result){
		result[0] = mat[0]; result[1] = mat[3]; result[2] = mat[6];
		result[3] = mat[1]; result[4] = mat[4]; result[5] = mat[7];
		result[6] = mat[2]; result[7] = mat[5]; result[8] = mat[8];
	}
	template<class T> extern void transpose4(const T *mat, T *result){
		result[0] = mat[0]; result[1] = mat[4]; result[2] = mat[8]; result[3] = mat[12];
		result[4] = mat[1]; result[5] = mat[5]; result[6] = mat[9]; result[7] = mat[13];
		result[8] = mat[2]; result[9] = mat[6]; result[10] = mat[10]; result[11] = mat[14];
		result[12] = mat[3]; result[13] = mat[7]; result[14] = mat[11]; result[15] = mat[15];
	}
	template<class T> extern T inv2(T *result, const T *mat){
		//cost:mul * 2, div * 4, add * 3
		T TmpMat[4];

		for (UBINT i = 0; i < 4; i++)TmpMat[i] = mat[i];
		T det = det2(mat);
		if (abs(det) < std::numeric_limits<T>::epsilon())return 0;
		else{
			result[0] = TmpMat[3] / det;
			result[1] = -TmpMat[1] / det;
			result[2] = -TmpMat[2] / det;
			result[3] = TmpMat[0] / det;
			return det;
		}
	}
	template<class T> extern T inv3(T *result, const T *mat){
		//cost:mul * 27, div * 9, add * 14
		T TmpMat[9];

		for (UBINT i = 0; i < 9; i++)TmpMat[i] = mat[i];
		T det = det3(mat);
		if (abs(det) < std::numeric_limits<T>::epsilon())return 0;
		else{
			result[0] = TmpMat[4] * TmpMat[8] - TmpMat[5] * TmpMat[7];
			result[1] = TmpMat[7] * TmpMat[2] - TmpMat[8] * TmpMat[1];
			result[2] = TmpMat[1] * TmpMat[5] - TmpMat[2] * TmpMat[4];
			result[3] = TmpMat[5] * TmpMat[6] - TmpMat[3] * TmpMat[8];
			result[4] = TmpMat[8] * TmpMat[0] - TmpMat[6] * TmpMat[2];
			result[5] = TmpMat[2] * TmpMat[3] - TmpMat[0] * TmpMat[5];
			result[6] = TmpMat[3] * TmpMat[7] - TmpMat[4] * TmpMat[6];
			result[7] = TmpMat[6] * TmpMat[1] - TmpMat[7] * TmpMat[0];
			result[8] = TmpMat[0] * TmpMat[4] - TmpMat[1] * TmpMat[3];
			for (UBINT i = 0; i < 9; i++)result[i] = result[i] / det;
			return det;
		}
	}
	template<class T> extern T inv4(T *result, const T *mat){
		//Gauss-Jordan inverse from http://dev.gameres.com/Program/Visual/3D/Mnquick.htm
		T TmpMat[16];
		UBINT XPos[4], YPos[4];
		BINT Flip = 1;
		T Det = 1, Tmp = 0;

		for (UBINT i = 0; i < 16; i++)TmpMat[i] = mat[i];
		for (UBINT k = 0; k < 4; k++){
			//Choose pivot element
			T Pivot = 0;
			for (UBINT i = k; i < 4; i++){
				for (UBINT j = k; j < 4; j++){
					if (abs(TmpMat[i * 4 + j]) > Pivot){ Pivot = abs(TmpMat[i * 4 + j]); XPos[k] = i; YPos[k] = j; }
				}
			}
			if (Pivot < std::numeric_limits<T>::epsilon())return 0;
			//Swap pivot element to lead element
			if (XPos[k] != k){
				Flip = -Flip;

				Tmp = TmpMat[4 * k];
				TmpMat[4 * k] = TmpMat[4 * XPos[k]];
				TmpMat[4 * XPos[k]] = Tmp;

				Tmp = TmpMat[4 * k + 1];
				TmpMat[4 * k + 1] = TmpMat[4 * XPos[k] + 1];
				TmpMat[4 * XPos[k] + 1] = Tmp;

				Tmp = TmpMat[4 * k + 2];
				TmpMat[4 * k + 2] = TmpMat[4 * XPos[k] + 2];
				TmpMat[4 * XPos[k] + 2] = Tmp;

				Tmp = TmpMat[4 * k + 3];
				TmpMat[4 * k + 3] = TmpMat[4 * XPos[k] + 3];
				TmpMat[4 * XPos[k] + 3] = Tmp;
			}
			if (YPos[k] != k){
				Flip = -Flip;

				Tmp = TmpMat[k];
				TmpMat[k] = TmpMat[YPos[k]];
				TmpMat[YPos[k]] = Tmp;

				Tmp = TmpMat[4 + k];
				TmpMat[4 + k] = TmpMat[4 + YPos[k]];
				TmpMat[4 + YPos[k]] = Tmp;

				Tmp = TmpMat[8 + k];
				TmpMat[8 + k] = TmpMat[8 + YPos[k]];
				TmpMat[8 + YPos[k]] = Tmp;

				Tmp = TmpMat[12 + k];
				TmpMat[12 + k] = TmpMat[12 + YPos[k]];
				TmpMat[12 + YPos[k]] = Tmp;
			}
			Det *= TmpMat[5 * k];

			//elimination
			TmpMat[4 * k + k] = 1 / TmpMat[4 * k + k];
			for (UBINT j = 0; j < 4; j++){
				if (j != k)TmpMat[k * 4 + j] *= TmpMat[4 * k + k];
			}
			for (UBINT i = 0; i < 4; i++){
				if (i != k){
					for (UBINT j = 0; j < 4; j++){
						if (j != k)TmpMat[i * 4 + j] -= TmpMat[i * 4 + k] * TmpMat[k * 4 + j];
					}
				}
			}
			for (UBINT i = 0; i < 4; i++){
				if (i != k)TmpMat[i * 4 + k] *= -TmpMat[4 * k + k];
			}
		}

		for (int k = 3; k >= 0; k--){
			if (YPos[k] != k){
				Tmp = TmpMat[4 * k];
				TmpMat[4 * k] = TmpMat[4 * YPos[k]];
				TmpMat[4 * YPos[k]] = Tmp;

				Tmp = TmpMat[4 * k + 1];
				TmpMat[4 * k + 1] = TmpMat[4 * YPos[k] + 1];
				TmpMat[4 * YPos[k] + 1] = Tmp;

				Tmp = TmpMat[4 * k + 2];
				TmpMat[4 * k + 2] = TmpMat[4 * YPos[k] + 2];
				TmpMat[4 * YPos[k] + 2] = Tmp;

				Tmp = TmpMat[4 * k + 3];
				TmpMat[4 * k + 3] = TmpMat[4 * YPos[k] + 3];
				TmpMat[4 * YPos[k] + 3] = Tmp;
			}
			if (XPos[k] != k){
				Tmp = TmpMat[k];
				TmpMat[k] = TmpMat[XPos[k]];
				TmpMat[XPos[k]] = Tmp;

				Tmp = TmpMat[4 + k];
				TmpMat[4 + k] = TmpMat[4 + XPos[k]];
				TmpMat[4 + XPos[k]] = Tmp;

				Tmp = TmpMat[8 + k];
				TmpMat[8 + k] = TmpMat[8 + XPos[k]];
				TmpMat[8 + XPos[k]] = Tmp;

				Tmp = TmpMat[12 + k];
				TmpMat[12 + k] = TmpMat[12 + XPos[k]];
				TmpMat[12 + XPos[k]] = Tmp;
			}
		}
		for (UBINT i = 0; i < 16; i++)result[i] = TmpMat[i];
		return Det * Flip;
	}
	template<class T> extern void vert2(T *result, const T *vec){
		result[0] = vec[1];
		result[1] = -vec[0];
	}
	template<class T> extern void vert3(T *result, const T *vec){
		//According to the hairy ball theorem, a continuous implementation of this function is impossible.
		int i = 0, j = 1, k = 2;
		T MinComponent = abs(vec[0]);
		if (abs(vec[1]) < MinComponent){ MinComponent = abs(vec[1]); i = 1; j = 2; k = 0; }
		if (abs(vec[2]) < MinComponent){ i = 2; j = 0; k = 1; }
		result[i] = 0;
		result[j] = vec[k];
		result[k] = -vec[j];
	}
	template<class T> extern void vert4(T *result, const T *vec){
		result[0] = vec[1];
		result[1] = -vec[0];
		result[2]= vec[3];
		result[3] = -vec[2];
	}
	template<class T> extern void slerp2(T *result, const T scl, const T *vec0, const T *vec1){
		//The framework of this algorithm comes from Jonathan Blow's "Understanding Slerp, Then Not Using It".
		T TmpVec[2];
		T angle_cos = dot2(vec0, vec1);

		if (angle_cos > 0.9999){
			//fall back to nlerp
			sub<2>(TmpVec, vec1, vec0);
			mul<2>(TmpVec, TmpVec, scl);
			add<2>(TmpVec, TmpVec, vec0);
			normalize<2>(result, TmpVec);
		}
		else{
			if (angle_cos < -0.9999)vert2(TmpVec, vec0);
			else{
				//Schmidt orthogonalization
				mul<2>(TmpVec, vec0, angle_cos);
				sub<2>(TmpVec, TmpVec, vec1);
				normalize<2>(TmpVec, TmpVec);
				//now TmpVec is orthogonal to vec0.
			}

			T theta = acos(angle_cos)*scl;
			mul<2>(TmpVec, TmpVec, sin(theta));
			mul<2>(result, vec0, cos(theta));
			add<2>(result, result, TmpVec);
		}
	}
	template<class T> extern void slerp3(T *result, const T scl, const T *vec0, const T *vec1){
		//The framework of this algorithm comes from Jonathan Blow's "Understanding Slerp, Then Not Using It".
		T TmpVec[3];
		T angle_cos = dot3(vec0, vec1);

		if (angle_cos > 0.9999){
			//fall back to nlerp
			sub<3>(TmpVec, vec1, vec0);
			mul<3>(TmpVec, TmpVec, scl);
			add<3>(TmpVec, TmpVec, vec0);
			normalize<3>(result, TmpVec);
		}
		else{
			if (angle_cos < -0.9999)vert3(TmpVec, vec0); //vert3() is not continuous. Thus the result may be instable under minor disturbance.
			else{
				//Schmidt orthogonalization
				mul<3>(TmpVec, vec0, angle_cos);
				sub<3>(TmpVec, TmpVec, vec1);
				normalize<3>(TmpVec, TmpVec);
				//now TmpVec is orthogonal to vec0.
			}

			T theta = acos(angle_cos)*scl;
			mul<3>(TmpVec, TmpVec, sin(theta));
			mul<3>(result, vec0, cos(theta));
			add<3>(result, result, TmpVec);
		}
	}
	template<class T> extern void slerp4(T *result, const T scl, const T *vec0, const T *vec1){
		//The framework of this algorithm comes from Jonathan Blow's "Understanding Slerp, Then Not Using It".
		T TmpVec[4];
		T angle_cos = dot4(vec0, vec1);

		if (angle_cos > 0.9999){
			//fall back to nlerp
			sub<4>(TmpVec, vec1, vec0);
			mul<4>(TmpVec, TmpVec, scl);
			add<4>(TmpVec, TmpVec, vec0);
			normalize<4>(result, TmpVec);
		}
		else{
			if (angle_cos < -0.9999)vert4(TmpVec, vec0);
			else{
				//Schmidt orthogonalization
				mul<4>(TmpVec, vec0, angle_cos);
				sub<4>(TmpVec, TmpVec, vec1);
				normalize<4>(TmpVec, TmpVec);
				//now TmpVec is orthogonal to vec0.
			}

			T theta = acos(angle_cos)*scl;
			mul<4>(TmpVec, TmpVec, sin(theta));
			mul<4>(result, vec0, cos(theta));
			add<4>(result, result, TmpVec);
		}
	}
}
#endif