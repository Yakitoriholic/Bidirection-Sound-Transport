/* Description:Platform-independent mathematical functions for computer graphics.
* Language:C++
* Author:***
*/

#ifndef LIB_MATH_CG
#define LIB_MATH_CG

#include "lMath.hpp"

namespace nsMath{
	template <typename T> struct AABB_2D{
		T Min[2];
		T Max[2];
	};

	template <typename T> struct AABB_3D{
		T Min[4];
		T Max[4];
	};

	// AABB Operations
	template <typename T> void AABB_3D_Merge(AABB_3D<T> *result, const AABB_3D<T> *box1, const AABB_3D<T> *box2);
	// get the AABB produced by merging [box1] and [box2].

	template <typename T> bool AABB_3D_Intersect(const AABB_3D<T> *box1, const AABB_3D<T> *box2);
	// return true when [box1] and [box2] intersect (including the case of contact).

	template <typename T> bool AABB_3D_Inside(const AABB_3D<T> *box, const T *Position);
	// return true when [Position] is inside the box (including the case of contact).

	template <typename T> T AABB_3D_Half_Surface_Area(const AABB_3D<T> *box);
	// returns the half surface area of the bounding box.

	//LHS axis
	template<class T> void Transform4_Pos(const T *Pos, T *result);

	template<class T> void Transform4_Pos_Inv(const T *Pos, T *result);

	template<class T> void Transform4_RotEuler(const T *Angle, T *result);
	//get the 4*4 rotation matrix (column-major order) from the Euler angle.
	//Angle:[0]-yaw (Z Axis), [1]-pitch (Y Axis), [2]-roll (X Axis). rotation matrix = yaw matrix * pitch matrix * roll matrix. The positive angle indicates a counterclockwise rotation.

	template<class T> void Transform4_RotEuler_Inv(const T *Angle, T *result);
	//get the 4*4 inverse rotation matrix (column-major order) from the Euler angle.
	//Angle:[0]-yaw (Z Axis), [1]-pitch (Y Axis), [2]-roll (X Axis). rotation matrix = inverse roll matrix * inverse pitch matrix * inverse yaw matrix. The positive angle indicates a counterclockwise rotation.

	template<class T> void Transform4_Pos_RotEuler(const T *Pos, const T *Angle, T *result);
	//get the 4*4 rigid body transform matrix (column-major order).
	//Angle:[0]-yaw (Z Axis), [1]-pitch (Y Axis), [2]-roll (X Axis). rotation matrix = transpose matrix * yaw matrix * pitch matrix * roll matrix. The positive angle indicates a counterclockwise rotation.

	template<class T> void Transform4_Pos_RotEuler_Inv(const T *Pos, const T *Angle, T *result);
	//get the 4*4 inverse rigid body transform matrix (column-major order).
	//Angle:[0]-yaw (Z Axis), [1]-pitch (Y Axis), [2]-roll (X Axis). rotation matrix = inverse roll matrix * inverse pitch matrix * inverse yaw matrix * inverse transpose matrix. The positive angle indicates a counterclockwise rotation.

	template<class T> void Transform4_Perspective_Area(const T *camera_info, T *result);
	//get the 4*4 perspective transform matrix (column-major order). The direction vector of the camera is (1, 0, 0).
	//camera_info: [0]-aspect ratio (width / height), [1]-cross section area size at distance 1, [2]-near, [3]-far(can be positive infinity)

	template<class T> void Transform4_Perspective_Inf_Area(const T *camera_info, T *result);
	//get the 4*4 perspective transform matrix (column-major order). The direction vector of the camera is (1, 0, 0).
	//The far plane of this transform is infinity.
	//camera_info: [0]-aspect ratio (width / height), [1]-cross section area size at distance 1, [2]-near

	template<class T> void Transform4_Perspective_Frustum(const T *frustum, T *result);
	//get the 4*4 perspective transform matrix (column-major order) from the given frustum. The direction vector of the frustum is (1, 0, 0).
	//frustum: [0]-left, [1]-top, [2]-right, [3]-bottom, [4]-near, [5]-far(can be positive infinity).
	//The diagram below shows the cross section of the frustum at distance 1:
	// (left,top)  -   (right,top)
	//      |               |
	//(left,bottom) - (right,bottom)

	//Sample transform results:
	//(1, left,   0, 1) == (*, 1,  0, 1)
	//(1, right,  0, 1) == (*, -1, 0, 1)
	//(1, bottom, 0, 1) == (*, 0,  1, 1)
	//(1, top,    0, 1) == (*, 0, -1, 1)

	template<class T> void Transform4_Perspective_Inf_Frustum(const T *frustum, T *result);
	//get the 4*4 perspective transform matrix (column-major order) from the given frustum. The direction vector of the frustum is (1, 0, 0).
	//The far plane of this transform is infinity.
	//frustum: [0]-left, [1]-top, [2]-right, [3]-bottom, [4]-near.
	//The diagram below shows the cross section of the frustum at distance 1:
	// (left,top)  -   (right,top)
	//      |               |
	//(left,bottom) - (right,bottom)

	//Sample transform results:
	//(1, left,   0, 1) == (*, 1,  0, 1)
	//(1, right,  0, 1) == (*, -1, 0, 1)
	//(1, bottom, 0, 1) == (*, 0,  1, 1)
	//(1, top,    0, 1) == (*, 0, -1, 1)

	template<class T> void Transform4_Ortho_Area(const T *camera_info, T *result);
	//get the 4*4 orthogonal transform matrix (column-major order). The direction vector of the camera is (1, 0, 0).
	//camera_info: [0]-aspect ratio (width / height), [1]-cross section area size at distance 1, [2]-near, [3]-far

	template<class T> void Transform4_Ortho_ViewBox(const T *box, T *result);
	//get the 4*4 orthogonal transform matrix (column-major order) from the given view box. The direction vector of the frustum is (1, 0, 0).
	//box: [0]-left, [1]-top, [2]-right, [3]-bottom, [4]-near, [5]-far.
	//The diagram below shows the cross section of the frustum at distance 1:
	// (left,top)  -   (right,top)
	//      |               |
	//(left,bottom) - (right,bottom)

	template<class T> void Transform4_CubeMap(const T *mat, T *result, const UBINT face_id);
	//Cubemap_Transform*mat. row transform. [mat] and [result] should not overlap in memory.

	template<class T> void Transform4_CubeMap_Inv(const T *mat, T *result, const UBINT face_id);
	//mat*Cubemap_Transform_Inv. column transform. [mat] and [result] should not overlap in memory.

	template<class T> bool Raytrace2D_Segment(T *Vtx0, T *Vtx1, T *Ray_Position, T *Ray_Direction, T*OutputParams);
	//perform a 2D ray-segment intersection test and output its result to [OutputParams].
	//This function returns true for intersect and false for not intersect.
	//A valid address is required for [OutputParams] as it will always be written whether intersected or not.
	//The data inside [OutputParams] is valid only if there is a intersection.
	//OutputParams: [0]-distance travelled / ||Direction||, [1]-2D barycentric coordinate

	template<class T> bool Raytrace2D_Circle(T *Circle_Pos, T Circle_Radius, T *Ray_Position, T *Ray_Direction, T*Distance);
	//perform a 2D ray-circle intersection test and output its result to [Distance].
	//[Ray_Direction] is required to be normalized.
	//This function returns true for intersect and false for not intersect.
	//If the origin point of the ray is inside the circle. the intersection test will always fail.
	//[Distance]: distance travelled.

	template<class T> bool Raytrace2D_Circle_Inside(T *Circle_Pos, T Circle_Radius, T *Ray_Position, T *Ray_Direction, T*Distance);
	//perform a ray-circle intersection test and output its result to [Distance].
	//[Ray_Direction] is required to be normalized.
	//This function returns true for intersect and false for not intersect.
	//If the origin point of the ray is outside the circle. the intersection test will always fail.
	//[Distance]: distance travelled.

	template<class T> bool Raytrace2D_AABB(T *AABB, T *Ray_Position, T *Ray_Direction, T*Distance);
	//perform a ray-AABB bounding box intersection test and output its result to [Distance].
	//This function returns true for intersect and false for not intersect.
	//If the origin point of the ray is outside the sphere. the intersection test will always fail.
	//[Distance]: distance travelled / ||Direction||.

	template<class T> bool Raytrace_Tri(T *Vtx0, T *Vtx1, T* Vtx2, T *Ray_Position, T *Ray_Direction, T *OutputNorm, T*OutputParams);
	//perform a ray-triangle intersection test and output its result to [OutputNorm] and [OutputParams].
	//This function returns true for intersect and false for not intersect.
	//A valid address is required for [OutputParams] as it will always be written whether intersected or not. the same for [OutputNorm].
	//[OutputNorm] contains the unnormalized normal of the test triangle.
	//The data inside [OutputParams] is valid only if there is a intersection.
	//OutputParams: [0]-distance travelled / ||Direction||, [1]-barycentric coord 1, [2]-barycentric coord 2

	template<class T> bool Raytrace_Sphere(T *Sphere_Pos, T Sphere_Radius, T *Ray_Position, T *Ray_Direction, T*Distance);
	//perform a ray-sphere intersection test and output its result to [Distance].
	//[Ray_Direction] is required to be normalized.
	//This function returns true for intersect and false for not intersect.
	//If the origin point of the ray is inside the sphere. the intersection test will always fail.
	//[Distance]: distance travelled.

	template<class T> bool Raytrace_Sphere_Inside(T *Sphere_Pos, T Sphere_Radius, T *Ray_Position, T *Ray_Direction, T*Distance);
	//perform a ray-sphere intersection test and output its result to [Distance].
	//[Ray_Direction] is required to be normalized.
	//This function returns true for intersect and false for not intersect.
	//If the origin point of the ray is outside the sphere. the intersection test will always fail.
	//[Distance]: distance travelled.

	template<class T> bool Raytrace_AABB(T *AABB, T *Ray_Position, T *Ray_Direction, T*Distance);
	//perform a ray-AABB bounding box intersection test and output its result to [Distance].
	//This function returns true for intersect and false for not intersect.
	//If the origin point of the ray is outside the sphere. the intersection test will always fail.
	//[Distance]: distance travelled / ||Direction||.

	template <class T> bool inside_AABB(T *AABB, T *Position);
	//return true when [Position] is inside the AABB bounding box.

	template <class T> bool _Intersect_View_8PointHull(const T *Projection_Matrix, T *PointCloud);
	//return true when the convex hull of the 8 points in [PointCloud] probably intersects with the view frustum.

#if defined LIBENV_CPU_ACCEL_SSE
	bool _Intersect_View_8PointHull_SSE(const float *Projection_Matrix, __m128 *PointCloud);
	//return true when the convex hull of the 8 points in [PointCloud] probably intersects with the view frustum.
#endif

#if defined LIBENV_CPU_ACCEL_AVX
	bool _Intersect_View_8PointHull_AVX(const double *Projection_Matrix, __m256d *PointCloud);
	//return true when the convex hull of the 8 points in [PointCloud] probably intersects with the view frustum.
#endif

	template <class T> bool Intersect_View_AABB_Coarse(const T *Projection_Matrix, const T *AABB, const T *Transform_AABB);
	//return true when the view frustum and the AABB intersects.

	template <class T> bool Intersect_View_Frustum_Coarse(const T *Projection_Matrix, const T *Frustum, const T *Transform_Frustum);
	//return true when the view frustum and the frustum intersects.
	//The direction vector of the frustum is (1, 0, 0).
	//The far plane of the frustum cannot be infinity.

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	template <typename T> struct _AABB_3D_Merge{
		static void Func(AABB_3D<T> *result, const AABB_3D<T> *box1, const AABB_3D<T> *box2);
	};

	template <typename T> void _AABB_3D_Merge<T>::Func(AABB_3D<T> *result, const AABB_3D<T> *box1, const AABB_3D<T> *box2){
		result->Min[0] = std::min(box1->Min[0], box2->Min[0]);
		result->Min[1] = std::min(box1->Min[1], box2->Min[1]);
		result->Min[2] = std::min(box1->Min[2], box2->Min[2]);
		result->Max[0] = std::max(box1->Max[0], box2->Max[0]);
		result->Max[1] = std::max(box1->Max[1], box2->Max[1]);
		result->Max[2] = std::max(box1->Max[2], box2->Max[2]);
	}

#if defined LIBENV_CPU_ACCEL_SSE
	template <> void _AABB_3D_Merge<float>::Func(AABB_3D<float> *result, const AABB_3D<float> *box1, const AABB_3D<float> *box2){
		_mm_storeu_ps(result->Min, _mm_min_ps(_mm_loadu_ps(box1->Min), _mm_loadu_ps(box2->Min)));
		_mm_storeu_ps(result->Max, _mm_max_ps(_mm_loadu_ps(box1->Max), _mm_loadu_ps(box2->Max)));
	}
#endif

#if defined LIBENV_CPU_ACCEL_AVX
	template <> void _AABB_3D_Merge<double>::Func(AABB_3D<double> *result, const AABB_3D<double> *box1, const AABB_3D<double> *box2){
		_mm256_storeu_pd(result->Min, _mm256_min_pd(_mm256_load_pd(box1->Min), _mm256_load_pd(box2->Min)));
		_mm256_storeu_pd(result->Max, _mm256_max_pd(_mm256_load_pd(box1->Max), _mm256_load_pd(box2->Max)));
	}
#endif

	template <typename T> inline void AABB_3D_Merge(AABB_3D<T> *result, const AABB_3D<T> *box1, const AABB_3D<T> *box2){
		_AABB_3D_Merge<T>::Func(result, box1, box2);
	}

	template <typename T> struct _AABB_3D_Intersect{
		static bool Func(const AABB_3D<T> *box1, const AABB_3D<T> *box2);
	};

	template <typename T> bool _AABB_3D_Intersect<T>::Func(const AABB_3D<T> *box1, const AABB_3D<T> *box2){
		if (box1->Min[0] > box2->Max[0] || box1->Max[0] < box2->Min[0])return false;
		if (box1->Min[1] > box2->Max[1] || box1->Max[1] < box2->Min[1])return false;
		if (box1->Min[2] > box2->Max[2] || box1->Max[2] < box2->Min[2])return false;
		return true;
	}

	template <typename T> inline bool AABB_3D_Intersect(const AABB_3D<T> *box1, const AABB_3D<T> *box2){
		return _AABB_3D_Intersect<T>::Func(box1, box2);
	}

	template <typename T> struct _AABB_3D_Inside{
		static bool Func(const AABB_3D<T> *box1, const T *Position);
	};

	template <typename T> bool _AABB_3D_Inside<T>::Func(const AABB_3D<T> *box1, const T *Position){
		if (Position[0] > box2->Max[0] || Position[0] < box2->Min[0])return false;
		if (Position[1] > box2->Max[1] || Position[1] < box2->Min[1])return false;
		if (Position[2] > box2->Max[2] || Position[2] < box2->Min[2])return false;
		return true;
	}

	template <typename T> inline bool AABB_3D_Inside(const AABB_3D<T> *box, const T *Position){
		return _AABB_3D_Inside<T>::Func(box, Position);
	}

	template <typename T> struct _AABB_3D_Half_Surface_Area{
		static T Func(const AABB_3D<T> *box);
	};

	template <typename T> T _AABB_3D_Half_Surface_Area<T>::Func(const AABB_3D<T> *box){
		return (box->Max[0] - box->Min[0]) * (box->Max[1] - box->Min[1]) + (box->Max[1] - box->Min[1]) * (box->Max[2] - box->Min[2]) + (box->Max[2] - box->Min[2]) * (box->Max[0] - box->Min[0]);
	}

#if defined LIBENV_CPU_ACCEL_SSE
	template <> float _AABB_3D_Half_Surface_Area<float>::Func(const AABB_3D<float> *box){
		__m128 BoxSize = _mm_sub_ps(_mm_loadu_ps(box->Max), _mm_loadu_ps(box->Min));
		__m128 BoxSize_Shuffled = _mm_shuffle_ps(BoxSize, BoxSize, 0xC9); // 11 00 10 01
		BoxSize = _mm_mul_ps(BoxSize, BoxSize_Shuffled);
		return BoxSize.m128_f32[0] + BoxSize.m128_f32[1] + BoxSize.m128_f32[2];
	}
#endif

#if defined LIBENV_CPU_ACCEL_AVX
	template <> double _AABB_3D_Half_Surface_Area<double>::Func(const AABB_3D<double> *box){
		__m256d BoxSize = _mm256_sub_pd(_mm256_loadu_pd(box->Max), _mm256_loadu_pd(box->Min));
		BoxSize.m256d_f64[3] = BoxSize.m256d_f64[1]; // [0] [1] [2] [1]
		__m256d BoxSize_Shuffled = _mm256_permute2f128_pd(BoxSize, BoxSize, 0x1); // 0000 0001
		// [2] [1] [0] [1]
		BoxSize_Shuffled = _mm256_permute_pd(BoxSize_Shuffled, 0x9); // 1 0 0 1
		// [1] [2] [0] [1]
		BoxSize = _mm256_mul_pd(BoxSize, BoxSize_Shuffled);
		return BoxSize.m256d_f64[0] + BoxSize.m256d_f64[1] + BoxSize.m256d_f64[2];
	}
#endif

	template <typename T> inline T AABB_3D_Half_Surface_Area(const AABB_3D<T> *box){
		return _AABB_3D_Half_Surface_Area<T>::Func(box);
	}

	template<class T> void Transform4_Pos(const T *Pos, T *result){
		result[0] = 1; result[1] = 0; result[2] = 0; result[3] = 0;
		result[4] = 0; result[5] = 1; result[6] = 0; result[7] = 0;
		result[8] = 0; result[9] = 0; result[10] = 1; result[11] = 0;
		result[12] = Pos[0];
		result[13] = Pos[1];
		result[14] = Pos[2];
		result[15] = 1;
	}
	template<class T> void Transform4_Pos_Inv(const T *Pos, T *result){
		result[0] = 1; result[1] = 0; result[2] = 0; result[3] = 0;
		result[4] = 0; result[5] = 1; result[6] = 0; result[7] = 0;
		result[8] = 0; result[9] = 0; result[10] = 1; result[11] = 0;
		result[12] = -Pos[0];
		result[13] = -Pos[1];
		result[14] = -Pos[2];
		result[15] = 1;
	}
	template<class T> void Transform4_RotEuler(const T *Angle, T *result){
		//cost:sin * 25, mul * 14, add * 6

		T Buf1 = sin(Angle[1])*cos(Angle[2]);
		T Buf2 = sin(Angle[1])*sin(Angle[2]);

		result[0] = cos(Angle[0])*cos(Angle[1]);
		result[1] = -sin(Angle[0])*cos(Angle[1]);
		result[2] = sin(Angle[1]);
		result[3] = 0;
		result[4] = cos(Angle[0])*Buf2 + sin(Angle[0])*cos(Angle[2]);
		result[5] = cos(Angle[0])*cos(Angle[2]) - sin(Angle[0])*Buf2;
		result[6] = -cos(Angle[1])*sin(Angle[2]);
		result[7] = 0;
		result[8] = sin(Angle[0])*sin(Angle[2]) - cos(Angle[0])*Buf1;
		result[9] = sin(Angle[0])*Buf1 + cos(Angle[0])*sin(Angle[2]);
		result[10] = cos(Angle[1])*cos(Angle[2]);
		result[11] = 0;
		result[12] = 0;
		result[13] = 0;
		result[14] = 0;
		result[15] = 1;
	}
	template<class T> void Transform4_RotEuler_Inv(const T *Angle, T *result){
		//cost:sin * 25, mul * 14, add * 6

		T Buf1 = sin(Angle[1])*cos(Angle[0]);
		T Buf2 = sin(Angle[1])*sin(Angle[0]);

		result[0] = cos(Angle[1])*cos(Angle[0]);
		result[1] = cos(Angle[2])*sin(Angle[0]) + sin(Angle[2])*Buf1;
		result[2] = sin(Angle[2])*sin(Angle[0]) - cos(Angle[2])*Buf1;
		result[3] = 0;
		result[4] = -cos(Angle[1])*sin(Angle[0]);
		result[5] = cos(Angle[2])*cos(Angle[0]) - sin(Angle[2])*Buf2;
		result[6] = sin(Angle[2])*cos(Angle[0]) + cos(Angle[2])*Buf2;
		result[7] = 0;
		result[8] = sin(Angle[1]);
		result[9] = -sin(Angle[2])*cos(Angle[1]);
		result[10] = cos(Angle[2])*cos(Angle[1]);
		result[11] = 0;
		result[12] = 0;
		result[13] = 0;
		result[14] = 0;
		result[15] = 1;
	}
	template<class T> void Transform4_Pos_RotEuler(const T *Pos, const T *Angle, T *result){
		//cost:sin * 25, mul * 14, add * 6

		T Buf1 = sin(Angle[1])*cos(Angle[2]);
		T Buf2 = sin(Angle[1])*sin(Angle[2]);

		result[0] = cos(Angle[0])*cos(Angle[1]);
		result[1] = -sin(Angle[0])*cos(Angle[1]);
		result[2] = sin(Angle[1]);
		result[3] = 0;
		result[4] = cos(Angle[0])*Buf2 + sin(Angle[0])*cos(Angle[2]);
		result[5] = cos(Angle[0])*cos(Angle[2]) - sin(Angle[0])*Buf2;
		result[6] = -cos(Angle[1])*sin(Angle[2]);
		result[7] = 0;
		result[8] = sin(Angle[0])*sin(Angle[2]) - cos(Angle[0])*Buf1;
		result[9] = sin(Angle[0])*Buf1 + cos(Angle[0])*sin(Angle[2]);
		result[10] = cos(Angle[1])*cos(Angle[2]);
		result[11] = 0;
		result[12] = Pos[0];
		result[13] = Pos[1];
		result[14] = Pos[2];
		result[15] = 1;
	}
	template<class T> void Transform4_Pos_RotEuler_Inv(const T *Pos, const T *Angle, T *result){
		//cost:sin * 25, mul * 23, add * 15

		T Buf1 = sin(Angle[1])*cos(Angle[0]);
		T Buf2 = sin(Angle[1])*sin(Angle[0]);

		result[0] = cos(Angle[1])*cos(Angle[0]);
		result[1] = cos(Angle[2])*sin(Angle[0]) + sin(Angle[2])*Buf1;
		result[2] = sin(Angle[2])*sin(Angle[0]) - cos(Angle[2])*Buf1;
		result[3] = 0;
		result[4] = -cos(Angle[1])*sin(Angle[0]);
		result[5] = cos(Angle[2])*cos(Angle[0]) - sin(Angle[2])*Buf2;
		result[6] = sin(Angle[2])*cos(Angle[0]) + cos(Angle[2])*Buf2;
		result[7] = 0;
		result[8] = sin(Angle[1]);
		result[9] = -sin(Angle[2])*cos(Angle[1]);
		result[10] = cos(Angle[2])*cos(Angle[1]);
		result[11] = 0;
		result[12] = -result[0] * Pos[0] - result[4] * Pos[1] - result[8] * Pos[2];
		result[13] = -result[1] * Pos[0] - result[5] * Pos[1] - result[9] * Pos[2];
		result[14] = -result[2] * Pos[0] - result[6] * Pos[1] - result[10] * Pos[2];
		result[15] = 1;
	}
	template<class T> void Transform4_Perspective_Area(const T *camera_info, T *result){
		result[0] = 0; result[1] = 0;
		result[3] = 1;
		result[4] = -2 / sqrt(camera_info[0] * camera_info[1]);
		result[5] = 0; result[6] = 0; result[7] = 0;
		result[8] = 0;
		result[9] = 2 / sqrt(camera_info[1] / camera_info[0]);
		result[10] = 0; result[11] = 0;
		result[12] = 0; result[13] = 0;
		result[15] = 0;
		if (frustum[5] == nsMath::NumericTrait<T>::Inf_Positive){
			result[2] = 1;
			result[14] = -2 * camera_info[2];
		}
		else{
			result[2] = -(camera_info[2] + camera_info[3]) / (camera_info[2] - camera_info[3]);
			result[14] = 2 * camera_info[2] * camera_info[3] / (camera_info[2] - camera_info[3]);
		}
	}
	template<class T> void Transform4_Perspective_Inf_Area(const T *camera_info, T *result){
		result[0] = 0; result[1] = 0;
		result[2] = 1;
		result[3] = 1;
		result[4] = -2 / sqrt(camera_info[0] * camera_info[1]);
		result[5] = 0; result[6] = 0; result[7] = 0;
		result[8] = 0;
		result[9] = 2 / sqrt(camera_info[1] / camera_info[0]);
		result[10] = 0; result[11] = 0;
		result[12] = 0; result[13] = 0;
		result[14] = -2 * camera_info[2];
		result[15] = 0;
	}
	template<class T> void Transform4_Perspective_Frustum(const T *frustum, T *result){
		result[0] = -(frustum[2] + frustum[0]) / (frustum[2] - frustum[0]);
		result[1] = -(frustum[3] + frustum[1]) / (frustum[3] - frustum[1]);
		result[3] = 1;
		result[4] = -2 / (frustum[2] - frustum[0]);
		result[5] = 0; result[6] = 0; result[7] = 0;
		result[8] = 0;
		result[9] = 2 / (frustum[3] - frustum[1]);
		result[10] = 0; result[11] = 0;
		result[12] = 0; result[13] = 0;
		result[15] = 0;
		if (frustum[5] == nsMath::NumericTrait<T>::Inf_Positive){
			result[2] = 1;
			result[14] = -2 * frustum[4];
		}
		else{
			result[2] = -(frustum[4] + frustum[5]) / (frustum[4] - frustum[5]);
			result[14] = 2 * frustum[4] * frustum[5] / (frustum[4] - frustum[5]);
		}
	}
	template<class T> void Transform4_Perspective_Inf_Frustum(const T *frustum, T *result){
		result[0] = -(frustum[2] + frustum[0]) / (frustum[2] - frustum[0]);
		result[1] = -(frustum[3] + frustum[1]) / (frustum[3] - frustum[1]);
		result[2] = 1;
		result[3] = 1;
		result[4] = -2 / (frustum[2] - frustum[0]);
		result[5] = 0; result[6] = 0; result[7] = 0;
		result[8] = 0;
		result[9] = 2 / (frustum[3] - frustum[1]);
		result[10] = 0; result[11] = 0;
		result[12] = 0; result[13] = 0;
		result[14] = -2 * frustum[4];
		result[15] = 0;
	}
	template<class T> void Transform4_Ortho_Area(const T *camera_info, T *result){
		result[0] = 0; result[1] = 0;
		result[2] = 2 / (camera_info[3] - camera_info[2]);
		result[3] = 0;
		result[4] = -2 / sqrt(camera_info[0] * camera_info[1]);
		result[5] = 0; result[6] = 0; result[7] = 0;
		result[8] = 0;
		result[9] = 2 / sqrt(camera_info[1] / camera_info[0]);
		result[10] = 0; result[11] = 0;
		result[12] = 0; result[13] = 0; result[14] = 0; result[15] = 1;
	}
	template<class T> void Transform4_Ortho_ViewBox(const T *box, T *result){
		result[0] = 0; result[1] = 0;
		result[2] = 2 / (box[5] - box[4]);
		result[3] = 0;
		result[4] = -2 / (box[2] - box[0]);
		result[5] = 0; result[6] = 0; result[7] = 0;
		result[8] = 0;
		result[9] = 2 / (box[3] - box[1]);
		result[10] = 0; result[11] = 0;
		result[12] = (box[0] + box[2]) / (box[0] - box[2]);
		result[13] = -(box[1] + box[3]) / (box[1] - box[3]);
		result[14] = (box[4] + box[5]) / (box[4] - box[5]);
		result[15] = 1;
	}
	template<class T> void Transform4_CubeMap(const T *mat, T *result, const UBINT face_id){
		switch (face_id){
		case 0:
			memcpy(result, mat, 16 * sizeof(T));
			break;
		case 1:
			result[0] = -mat[0]; result[1] = -mat[1]; result[2] = -mat[2]; result[3] = -mat[3];
			result[4] = -mat[4]; result[5] = -mat[5]; result[6] = -mat[6]; result[7] = -mat[7];
			result[8] = mat[8]; result[9] = mat[9]; result[10] = mat[10]; result[11] = mat[11];
			result[12] = mat[12]; result[13] = mat[13]; result[14] = mat[14]; result[15] = mat[15];
			break;
		case 2:
			result[0] = -mat[4]; result[1] = -mat[5]; result[2] = -mat[6]; result[3] = -mat[7];
			result[4] = mat[0]; result[5] = mat[1]; result[6] = mat[2]; result[7] = mat[3];
			result[8] = mat[8]; result[9] = mat[9]; result[10] = mat[10]; result[11] = mat[11];
			result[12] = mat[12]; result[13] = mat[13]; result[14] = mat[14]; result[15] = mat[15];
			break;
		case 3:
			result[0] = mat[4]; result[1] = mat[5]; result[2] = mat[6]; result[3] = mat[7];
			result[4] = -mat[0]; result[5] = -mat[1]; result[6] = -mat[2]; result[7] = -mat[3];
			result[8] = mat[8]; result[9] = mat[9]; result[10] = mat[10]; result[11] = mat[11];
			result[12] = mat[12]; result[13] = mat[13]; result[14] = mat[14]; result[15] = mat[15];
			break;
		case 4:
			result[0] = mat[8]; result[1] = mat[9]; result[2] = mat[10]; result[3] = mat[11];
			result[4] = mat[4]; result[5] = mat[5]; result[6] = mat[6]; result[7] = mat[7];
			result[8] = -mat[0]; result[9] = -mat[1]; result[10] = -mat[2]; result[11] = -mat[3];
			result[12] = mat[12]; result[13] = mat[13]; result[14] = mat[14]; result[15] = mat[15];
			break;
		case 5:
			result[0] = -mat[8]; result[1] = -mat[9]; result[2] = -mat[10]; result[3] = -mat[11];
			result[4] = mat[4]; result[5] = mat[5]; result[6] = mat[6]; result[7] = mat[7];
			result[8] = mat[0]; result[9] = mat[1]; result[10] = mat[2]; result[11] = mat[3];
			result[12] = mat[12]; result[13] = mat[13]; result[14] = mat[14]; result[15] = mat[15];
			break;
		}
	}
	template<class T> void Transform4_CubeMap_Inv(const T *mat, T *result, const UBINT face_id){
		switch (face_id){
		case 0:
			memcpy(result, mat, 16 * sizeof(T));
			break;
		case 1:
			result[0] = -mat[0]; result[1] = -mat[1]; result[2] = mat[2]; result[3] = mat[3];
			result[4] = -mat[4]; result[5] = -mat[5]; result[6] = mat[6]; result[7] = mat[7];
			result[8] = -mat[8]; result[9] = -mat[9]; result[10] = mat[10]; result[11] = mat[11];
			result[12] = -mat[12]; result[13] = -mat[13]; result[14] = mat[14]; result[15] = mat[15];
			break;
		case 2:
			result[0] = -mat[1]; result[1] = mat[0]; result[2] = mat[2]; result[3] = mat[3];
			result[4] = -mat[5]; result[5] = mat[4]; result[6] = mat[6]; result[7] = mat[7];
			result[8] = -mat[9]; result[9] = mat[8]; result[10] = mat[10]; result[11] = mat[11];
			result[12] = -mat[13]; result[13] = mat[12]; result[14] = mat[14]; result[15] = mat[15];
			break;
		case 3:
			result[0] = mat[1]; result[1] = -mat[0]; result[2] = mat[2]; result[3] = mat[3];
			result[4] = mat[5]; result[5] = -mat[4]; result[6] = mat[6]; result[7] = mat[7];
			result[8] = mat[9]; result[9] = -mat[8]; result[10] = mat[10]; result[11] = mat[11];
			result[12] = mat[13]; result[13] = -mat[12]; result[14] = mat[14]; result[15] = mat[15];
			break;
		case 4:
			result[0] = mat[2]; result[1] = mat[1]; result[2] = -mat[0]; result[3] = mat[3];
			result[4] = mat[6]; result[5] = mat[5]; result[6] = -mat[4]; result[7] = mat[7];
			result[8] = mat[10]; result[9] = mat[9]; result[10] = -mat[8]; result[11] = mat[11];
			result[12] = mat[14]; result[13] = mat[13]; result[14] = -mat[12]; result[15] = mat[15];
			break;
		case 5:
			result[0] = -mat[2]; result[1] = mat[1]; result[2] = mat[0]; result[3] = mat[3];
			result[4] = -mat[6]; result[5] = mat[5]; result[6] = mat[4]; result[7] = mat[7];
			result[8] = -mat[10]; result[9] = mat[9]; result[10] = mat[8]; result[11] = mat[11];
			result[12] = -mat[14]; result[13] = mat[13]; result[14] = mat[12]; result[15] = mat[15];
			break;
		}
	}

	template<class T> bool Raytrace2D_Segment(T *Vtx0, T *Vtx1, T *Ray_Position, T *Ray_Direction, T*OutputParams){
		T Vec0[2], Vec1[2], detrcp;

		nsMath::sub<2>(Vec0, Vtx1, Vtx0);
		nsMath::sub<2>(Vec1, Vtx0, Ray_Position);
		detrcp = T(1) / nsMath::det2col(Ray_Direction, Vec0);

		OutputParams[0] = nsMath::det2col(Vec1, Vec0);
		OutputParams[1] = nsMath::det2col(Vec1, Ray_Direction);
		mul<2>(OutputParams, OutputParams, detrcp);

		return ((OutputParams[0] > 0) & (OutputParams[0] < std::numeric_limits<T>::max()) & (OutputParams[1] >= 0) & (OutputParams[1] <= 1));
	}
	template<class T> bool Raytrace2D_Circle(T *Circle_Pos, T Circle_Radius, T *Ray_Position, T *Ray_Direction, T*Distance){
		T DistVec[2];
		nsMath::sub<2>(DistVec, Circle_Pos, Ray_Position);
		T InnerProduct = nsMath::dot2(DistVec, Ray_Direction);
		T DistSqr = nsMath::dot2(DistVec, DistVec);
		T Radius_Sqr = Sphere_Radius*Sphere_Radius;
		if (InnerProduct > 0 && DistSqr > Radius_Sqr){ //If the origin point of the ray is inside the circle.the intersection test will always fail.
			float TmpVal = InnerProduct * InnerProduct + Radius_Sqr - DistSqr; //TmpVal=Delta
			if (TmpVal >= 0){ //intersected
				*Distance = InnerProduct - sqrt(TmpVal);
				return true;
			}
		}
		return false;
	}
	template<class T> bool Raytrace2D_Circle_Inside(T *Circle_Pos, T Circle_Radius, T *Ray_Position, T *Ray_Direction, T*Distance){
		T DistVec[2];
		nsMath::sub<2>(DistVec, Circle_Pos, Ray_Position);
		T DistSqr = nsMath::dot2(DistVec, DistVec);
		T Radius_Sqr = Sphere_Radius*Sphere_Radius;
		if (DistSqr <= Radius_Sqr){ //If the origin point of the ray is inside the circle.the intersection test will always fail.
			T InnerProduct = nsMath::dot2(DistVec, Ray_Direction);
			float TmpVal = InnerProduct * InnerProduct + Radius_Sqr - DistSqr; //TmpVal=Delta
			*Distance = InnerProduct + sqrt(TmpVal);
			return true;
		}
		return false;
	}
	template<class T> bool Raytrace2D_AABB(T *AABB, T *Ray_Position, T *Ray_Direction, T*Distance){
		//slab method
		T Ray_Direction_Inv[3], MinDist, MaxDist, TmpDist0, TmpDist1;
		Ray_Direction_Inv[0] = T(1) / Ray_Direction[0]; Ray_Direction_Inv[1] = T(1) / Ray_Direction[1];

		TmpDist0 = (AABB[0] - Ray_Position[0])*Ray_Direction_Inv[0];
		TmpDist1 = (AABB[2] - Ray_Position[0])*Ray_Direction_Inv[0];
		MinDist = std::min(TmpDist0, TmpDist1);
		MaxDist = std::max(TmpDist0, TmpDist1);

		TmpDist0 = (AABB[1] - Ray_Position[1])*Ray_Direction_Inv[1];
		TmpDist1 = (AABB[3] - Ray_Position[1])*Ray_Direction_Inv[1];
		MinDist = std::max(MinDist, std::min(TmpDist0, TmpDist1));
		MaxDist = std::min(MaxDist, std::max(TmpDist0, TmpDist1));

		*Distance = MinDist;
		return ((MaxDist >= MinDist) & (MinDist > 0));
	}
	template<class T> bool Raytrace_Tri(T *Vtx0, T *Vtx1, T* Vtx2, T *Ray_Position, T *Ray_Direction, T *OutputNorm, T*OutputParams){
		T Vec0[3], Vec1[3], Vec2[3], TmpVec[3], detrcp;

		nsMath::sub<3>(Vec0, Vtx0, Ray_Position);
		nsMath::sub<3>(Vec1, Vtx1, Vtx0);
		nsMath::sub<3>(Vec2, Vtx0, Vtx2);
		nsMath::cross3(Vec1, Vec2, OutputNorm);

		detrcp = T(1) / nsMath::dot3(Ray_Direction, OutputNorm);
		mul<2>(Vec0, Vec0, detrcp);
		OutputParams[0] = nsMath::dot3(Vec0, OutputNorm);
		
		nsMath::cross3(TmpVec, Vec0, Ray_Direction); //currently TmpVec is (Direction cross (Position-Vtx0)) / determinant.
		OutputParams[1] = nsMath::dot3(TmpVec, Vec2);
		OutputParams[2] = nsMath::dot3(TmpVec, Vec1);

		return ((OutputParams[0] > 0) & (OutputParams[0] < std::numeric_limits<T>::max()) & (OutputParams[1] >= 0) & (OutputParams[2] >= 0) & (OutputParams[1] + OutputParams[2] <= 1));
	}
	template<class T> bool Raytrace_Sphere(T *Sphere_Pos, T Sphere_Radius, T *Ray_Position, T *Ray_Direction, T*Distance){
		T DistVec[3];
		nsMath::sub<3>(DistVec, Sphere_Pos, Ray_Position);
		T InnerProduct = nsMath::dot3(DistVec, Ray_Direction);
		T DistSqr = nsMath::dot3(DistVec, DistVec);
		T Radius_Sqr = Sphere_Radius*Sphere_Radius;
		if (InnerProduct > 0 && DistSqr > Radius_Sqr){ //If the origin point of the ray is inside the sphere.the intersection test will always fail.
			float TmpVal = InnerProduct * InnerProduct + Radius_Sqr - DistSqr; //TmpVal=Delta
			if (TmpVal >= 0){ //intersected
				*Distance = InnerProduct - sqrt(TmpVal);
				return true;
			}
		}
		return false;
	}
	template<class T> bool Raytrace_Sphere_Inside(const T *Sphere_Pos, const T Sphere_Radius, const T *Ray_Position, const T *Ray_Direction, const T *Distance){
		T DistVec[3];
		nsMath::sub<3>(DistVec, Sphere_Pos, Ray_Position);
		T DistSqr = nsMath::dot3(DistVec, DistVec);
		T Radius_Sqr = Sphere_Radius*Sphere_Radius;
		if (DistSqr <= Radius_Sqr){ //If the origin point of the ray is outside the sphere. the intersection test will always fail.
			T InnerProduct = nsMath::dot3(DistVec, Ray_Direction);
			float TmpVal = InnerProduct * InnerProduct + Radius_Sqr - DistSqr; //TmpVal=Delta
			*Distance = InnerProduct + sqrt(TmpVal);
			return true;
		}
		return false;
	}
	template<class T> bool Raytrace_AABB(T *AABB, T *Ray_Position, T *Ray_Direction, T*Distance){
		//slab method
		T Ray_Direction_Inv[3], MinDist, MaxDist, TmpDist0, TmpDist1;
		Ray_Direction_Inv[0] = T(1) / Ray_Direction[0]; Ray_Direction_Inv[1] = T(1) / Ray_Direction[1]; Ray_Direction_Inv[2] = T(1) / Ray_Direction[2];
		
		TmpDist0 = (AABB[0] - Ray_Position[0])*Ray_Direction_Inv[0];
		TmpDist1 = (AABB[3] - Ray_Position[0])*Ray_Direction_Inv[0];
		MinDist = std::min(TmpDist0, TmpDist1);
		MaxDist = std::max(TmpDist0, TmpDist1);

		TmpDist0 = (AABB[1] - Ray_Position[1])*Ray_Direction_Inv[1];
		TmpDist1 = (AABB[4] - Ray_Position[1])*Ray_Direction_Inv[1];
		MinDist = std::max(MinDist, std::min(TmpDist0, TmpDist1));
		MaxDist = std::min(MaxDist, std::max(TmpDist0, TmpDist1));

		TmpDist0 = (AABB[2] - Ray_Position[2])*Ray_Direction_Inv[2];
		TmpDist1 = (AABB[5] - Ray_Position[2])*Ray_Direction_Inv[2];
		MinDist = std::max(MinDist, std::min(TmpDist0, TmpDist1));
		MaxDist = std::min(MaxDist, std::max(TmpDist0, TmpDist1));

		*Distance = MinDist;
		return ((MaxDist >= MinDist) & (MinDist > 0));
	}
	template<class T> bool inside_AABB(T *AABB, T *Position){
		return ((Position[0] > AABB[0]) & (Position[1] > AABB[1]) & (Position[2] > AABB[2]) & (Position[0] < AABB[3]) & (Position[0] < AABB[4]) & (Position[0] < AABB[5]));
	}

	template <class T> bool _Intersect_View_8PointHull(const T *Projection_Matrix, T *PointCloud){
		//calculate the equation for six planes
		T ViewPlane[6 * 4];
		ViewPlane[0 * 4 + 0] = Projection_Matrix[3] - Projection_Matrix[0]; ViewPlane[0 * 4 + 1] = Projection_Matrix[7] - Projection_Matrix[4]; ViewPlane[0 * 4 + 2] = Projection_Matrix[11] - Projection_Matrix[8]; ViewPlane[0 * 4 + 3] = Projection_Matrix[15] - Projection_Matrix[12];
		ViewPlane[1 * 4 + 0] = Projection_Matrix[3] + Projection_Matrix[0]; ViewPlane[1 * 4 + 1] = Projection_Matrix[7] + Projection_Matrix[4]; ViewPlane[1 * 4 + 2] = Projection_Matrix[11] + Projection_Matrix[8]; ViewPlane[1 * 4 + 3] = Projection_Matrix[15] + Projection_Matrix[12];
		ViewPlane[2 * 4 + 0] = Projection_Matrix[3] - Projection_Matrix[1]; ViewPlane[2 * 4 + 1] = Projection_Matrix[7] - Projection_Matrix[5]; ViewPlane[2 * 4 + 2] = Projection_Matrix[11] - Projection_Matrix[9]; ViewPlane[2 * 4 + 3] = Projection_Matrix[15] - Projection_Matrix[13];
		ViewPlane[3 * 4 + 0] = Projection_Matrix[3] + Projection_Matrix[1]; ViewPlane[3 * 4 + 1] = Projection_Matrix[7] + Projection_Matrix[5]; ViewPlane[3 * 4 + 2] = Projection_Matrix[11] + Projection_Matrix[9]; ViewPlane[3 * 4 + 3] = Projection_Matrix[15] + Projection_Matrix[13];
		ViewPlane[4 * 4 + 0] = Projection_Matrix[3] - Projection_Matrix[2]; ViewPlane[4 * 4 + 1] = Projection_Matrix[7] - Projection_Matrix[6]; ViewPlane[4 * 4 + 2] = Projection_Matrix[11] - Projection_Matrix[10]; ViewPlane[4 * 4 + 3] = Projection_Matrix[15] - Projection_Matrix[14];
		ViewPlane[5 * 4 + 0] = Projection_Matrix[3] + Projection_Matrix[2]; ViewPlane[5 * 4 + 1] = Projection_Matrix[7] + Projection_Matrix[6]; ViewPlane[5 * 4 + 2] = Projection_Matrix[11] + Projection_Matrix[10]; ViewPlane[5 * 4 + 3] = Projection_Matrix[15] + Projection_Matrix[14];

		for (UBINT i = 0; i < 6; i++){
			bool Culled = true;
			for (UBINT j = 0; j < 8; j++){
				T result = dot<4, T>(&ViewPlane[i * 4], &PointCloud[j * 4]);
				if (result >= 0)Culled = false;
			}
			if (Culled)return false;
		}
		return true;
	}

#if defined LIBENV_CPU_ACCEL_SSE
	bool _Intersect_View_8PointHull_SSE(const float *Projection_Matrix, __m128 *PointCloud){
		__m128 ViewPlane[6];

		__m128 ProjMatSIMD[4], ProjMatSIMD_T[4];
		for (UBINT i = 0; i < 4; i++)ProjMatSIMD[i] = _mm_loadu_ps(&Projection_Matrix[4 * i]);
		__m128 t0 = _mm_unpacklo_ps(ProjMatSIMD[0], ProjMatSIMD[1]); // t0 = (0_0, 1_0, 0_1, 1_1)
		__m128 t1 = _mm_unpackhi_ps(ProjMatSIMD[0], ProjMatSIMD[1]); // t1 = (0_2, 1_2, 0_3, 1_3)
		__m128 t2 = _mm_unpacklo_ps(ProjMatSIMD[2], ProjMatSIMD[3]); // t2 = (2_0, 3_0, 2_1, 3_1)
		__m128 t3 = _mm_unpackhi_ps(ProjMatSIMD[2], ProjMatSIMD[3]); // t3 = (2_2, 3_2, 2_3, 3_3)
		ProjMatSIMD_T[0] = _mm_movelh_ps(t0, t2); // (0_0, 1_0, 2_0, 3_0)
		ProjMatSIMD_T[1] = _mm_movehl_ps(t2, t0); // (0_1, 1_1, 2_1, 3_1) * the way that movehl shuffle words is different from movelh *
		ProjMatSIMD_T[2] = _mm_movelh_ps(t1, t3); // (0_2, 1_2, 2_2, 3_2)
		ProjMatSIMD_T[3] = _mm_movehl_ps(t3, t1); // (0_3, 1_3, 2_3, 3_3)

		ViewPlane[0] = _mm_sub_ps(ProjMatSIMD_T[3], ProjMatSIMD_T[0]);
		ViewPlane[1] = _mm_add_ps(ProjMatSIMD_T[3], ProjMatSIMD_T[0]);
		ViewPlane[2] = _mm_sub_ps(ProjMatSIMD_T[3], ProjMatSIMD_T[1]);
		ViewPlane[3] = _mm_add_ps(ProjMatSIMD_T[3], ProjMatSIMD_T[1]);
		ViewPlane[4] = _mm_sub_ps(ProjMatSIMD_T[3], ProjMatSIMD_T[2]);
		ViewPlane[5] = _mm_add_ps(ProjMatSIMD_T[3], ProjMatSIMD_T[2]);

		__m128 cmpbase = _mm_setzero_ps();
		for (UBINT i = 0; i < 6; i++){
			__m128 C0 = _mm_set_ps1(ViewPlane[i].m128_f32[0]);
			__m128 C1 = _mm_set_ps1(ViewPlane[i].m128_f32[1]);
			__m128 C2 = _mm_set_ps1(ViewPlane[i].m128_f32[2]);
			__m128 C3 = _mm_set_ps1(ViewPlane[i].m128_f32[3]);
			__m128 Result0, Result1;

			Result0 = _mm_mul_ps(C0, PointCloud[0]);
			Result0 = _mm_add_ps(Result0, _mm_mul_ps(C1, PointCloud[1]));
			Result0 = _mm_add_ps(Result0, _mm_mul_ps(C2, PointCloud[2]));
			Result0 = _mm_add_ps(Result0, _mm_mul_ps(C3, PointCloud[3]));

			Result1 = _mm_mul_ps(C0, PointCloud[4]);
			Result1 = _mm_add_ps(Result1, _mm_mul_ps(C1, PointCloud[5]));
			Result1 = _mm_add_ps(Result1, _mm_mul_ps(C2, PointCloud[6]));
			Result1 = _mm_add_ps(Result1, _mm_mul_ps(C3, PointCloud[7]));

			Result0 = _mm_cmpge_ps(Result0, cmpbase);
			Result1 = _mm_cmpge_ps(Result1, cmpbase);

			if (0 == (_mm_movemask_ps(Result0) | _mm_movemask_ps(Result1)))return false;
		}
		return true;
	}
#endif

#if defined LIBENV_CPU_ACCEL_AVX
	bool _Intersect_View_8PointHull_AVX(const double *Projection_Matrix, __m256d *PointCloud){
		__m256d ViewPlane[6];

		__m256d ProjMatSIMD[4], ProjMatSIMD_T[4];
		for (UBINT i = 0; i < 4; i++)ProjMatSIMD[i] = _mm256_loadu_pd(&Projection_Matrix[4 * i]);
		__m256d t0 = _mm256_unpacklo_pd(ProjMatSIMD[0], ProjMatSIMD[2]); // t0 = (0_0, 2_0, 0_1, 2_1)
		__m256d t1 = _mm256_unpackhi_pd(ProjMatSIMD[0], ProjMatSIMD[2]); // t1 = (0_2, 2_2, 0_3, 2_3)
		__m256d t2 = _mm256_unpacklo_pd(ProjMatSIMD[1], ProjMatSIMD[3]); // t2 = (1_0, 3_0, 1_1, 3_1)
		__m256d t3 = _mm256_unpackhi_pd(ProjMatSIMD[1], ProjMatSIMD[3]); // t3 = (1_2, 3_2, 1_3, 3_3)
		ProjMatSIMD_T[0] = _mm256_unpacklo_pd(t0, t2); // (0_0, 1_0, 2_0, 3_0)
		ProjMatSIMD_T[1] = _mm256_unpackhi_pd(t0, t2); // (0_1, 1_1, 2_1, 3_1)
		ProjMatSIMD_T[2] = _mm256_unpacklo_pd(t1, t3); // (0_2, 1_2, 2_2, 3_2)
		ProjMatSIMD_T[3] = _mm256_unpackhi_pd(t1, t3); // (0_3, 1_3, 2_3, 3_3)

		ViewPlane[0] = _mm256_sub_pd(ProjMatSIMD_T[3], ProjMatSIMD_T[0]);
		ViewPlane[1] = _mm256_add_pd(ProjMatSIMD_T[3], ProjMatSIMD_T[0]);
		ViewPlane[2] = _mm256_sub_pd(ProjMatSIMD_T[3], ProjMatSIMD_T[1]);
		ViewPlane[3] = _mm256_add_pd(ProjMatSIMD_T[3], ProjMatSIMD_T[1]);
		ViewPlane[4] = _mm256_sub_pd(ProjMatSIMD_T[3], ProjMatSIMD_T[2]);
		ViewPlane[5] = _mm256_add_pd(ProjMatSIMD_T[3], ProjMatSIMD_T[2]);

		__m256d cmpbase = _mm256_setzero_pd();
		for (UBINT i = 0; i < 6; i++){
			__m256d C0 = _mm256_broadcast_sd(&ViewPlane[i].m256d_f64[0]);
			__m256d C1 = _mm256_broadcast_sd(&ViewPlane[i].m256d_f64[1]);
			__m256d C2 = _mm256_broadcast_sd(&ViewPlane[i].m256d_f64[2]);
			__m256d C3 = _mm256_broadcast_sd(&ViewPlane[i].m256d_f64[3]);
			__m256d Result0, Result1;

			Result0 = _mm256_mul_pd(C0, PointCloud[0]);
			Result0 = _mm256_add_pd(Result0, _mm256_mul_pd(C1, PointCloud[1]));
			Result0 = _mm256_add_pd(Result0, _mm256_mul_pd(C2, PointCloud[2]));
			Result0 = _mm256_add_pd(Result0, _mm256_mul_pd(C3, PointCloud[3]));

			Result1 = _mm256_mul_pd(C0, PointCloud[4]);
			Result1 = _mm256_add_pd(Result1, _mm256_mul_pd(C1, PointCloud[5]));
			Result1 = _mm256_add_pd(Result1, _mm256_mul_pd(C2, PointCloud[6]));
			Result1 = _mm256_add_pd(Result1, _mm256_mul_pd(C3, PointCloud[7]));

			Result0 = _mm256_cmp_pd(Result0, cmpbase, _CMP_GE_OS);
			Result1 = _mm256_cmp_pd(Result1, cmpbase, _CMP_GE_OS);

			if (0 == (_mm256_movemask_pd(Result0) | _mm256_movemask_pd(Result1)))return false;
		}
		return true;
	}
#endif

	template<class T> struct _Intersect_View_AABB_Coarse{
		static bool Func(const T *Projection_Matrix, const T *AABB, const T *Transform_AABB);
	};

	template<class T> bool _Intersect_View_AABB_Coarse<T>::Func(const T *Projection_Matrix, const T *AABB, const T *Transform_AABB){
		T PointCloud[8 * 4], PointCloud_Transformed[8 * 4];
		PointCloud[0 * 4 + 0] = AABB[0]; PointCloud[0 * 4 + 1] = AABB[1]; PointCloud[0 * 4 + 2] = AABB[2]; PointCloud[0 * 4 + 3] = (T)1;
		PointCloud[1 * 4 + 0] = AABB[0]; PointCloud[1 * 4 + 1] = AABB[1]; PointCloud[1 * 4 + 2] = AABB[5]; PointCloud[1 * 4 + 3] = (T)1;
		PointCloud[2 * 4 + 0] = AABB[0]; PointCloud[2 * 4 + 1] = AABB[4]; PointCloud[2 * 4 + 2] = AABB[2]; PointCloud[2 * 4 + 3] = (T)1;
		PointCloud[3 * 4 + 0] = AABB[0]; PointCloud[3 * 4 + 1] = AABB[4]; PointCloud[3 * 4 + 2] = AABB[5]; PointCloud[3 * 4 + 3] = (T)1;
		PointCloud[4 * 4 + 0] = AABB[3]; PointCloud[4 * 4 + 1] = AABB[1]; PointCloud[4 * 4 + 2] = AABB[2]; PointCloud[4 * 4 + 3] = (T)1;
		PointCloud[5 * 4 + 0] = AABB[3]; PointCloud[5 * 4 + 1] = AABB[1]; PointCloud[5 * 4 + 2] = AABB[5]; PointCloud[5 * 4 + 3] = (T)1;
		PointCloud[6 * 4 + 0] = AABB[3]; PointCloud[6 * 4 + 1] = AABB[4]; PointCloud[6 * 4 + 2] = AABB[2]; PointCloud[6 * 4 + 3] = (T)1;
		PointCloud[7 * 4 + 0] = AABB[3]; PointCloud[7 * 4 + 1] = AABB[4]; PointCloud[7 * 4 + 2] = AABB[5]; PointCloud[7 * 4 + 3] = (T)1;

		for (UBINT i = 0; i < 8; i++){
			mul4_mv(Transform_AABB, &PointCloud[i * 4], &PointCloud_Transformed[i * 4]);
		}

		return _Intersect_View_8PointHull(Projection_Matrix, PointCloud_Transformed);
	}

#if defined LIBENV_CPU_ACCEL_SSE
	template <> bool _Intersect_View_AABB_Coarse<float>::Func(const float *Projection_Matrix, const float *AABB, const float *Transform_AABB){
		__m128 PointCloud[4], PointCloud_Transformed[8];

		PointCloud[0] = _mm_set_ps1(AABB[0]);
		PointCloud[1] = _mm_set_ps(AABB[1], AABB[1], AABB[4], AABB[4]);
		PointCloud[2] = _mm_set_ps(AABB[2], AABB[5], AABB[2], AABB[5]);
		
		PointCloud[3] = _mm_set_ps1(AABB[3]);

		__m128 TmpVec;

		for (UBINT i = 0; i < 4; i++){
			TmpVec = _mm_set_ps1(Transform_AABB[0 * 4 + i]);
			PointCloud_Transformed[i] = _mm_mul_ps(TmpVec, PointCloud[0]);
			TmpVec = _mm_set_ps1(Transform_AABB[1 * 4 + i]);
			PointCloud_Transformed[i] = _mm_add_ps(PointCloud_Transformed[i], _mm_mul_ps(TmpVec, PointCloud[1]));
			TmpVec = _mm_set_ps1(Transform_AABB[2 * 4 + i]);
			PointCloud_Transformed[i] = _mm_add_ps(PointCloud_Transformed[i], _mm_mul_ps(TmpVec, PointCloud[2]));
			TmpVec = _mm_set_ps1(Transform_AABB[3 * 4 + i]);
			PointCloud_Transformed[i] = _mm_add_ps(PointCloud_Transformed[i], TmpVec);
		}

		for (UBINT i = 0; i < 4; i++){
			TmpVec = _mm_set_ps1(Transform_AABB[0 * 4 + i]);
			PointCloud_Transformed[i + 4] = _mm_mul_ps(TmpVec, PointCloud[3]);
			TmpVec = _mm_set_ps1(Transform_AABB[1 * 4 + i]);
			PointCloud_Transformed[i + 4] = _mm_add_ps(PointCloud_Transformed[i + 4], _mm_mul_ps(TmpVec, PointCloud[1]));
			TmpVec = _mm_set_ps1(Transform_AABB[2 * 4 + i]);
			PointCloud_Transformed[i + 4] = _mm_add_ps(PointCloud_Transformed[i + 4], _mm_mul_ps(TmpVec, PointCloud[2]));
			TmpVec = _mm_set_ps1(Transform_AABB[3 * 4 + i]);
			PointCloud_Transformed[i + 4] = _mm_add_ps(PointCloud_Transformed[i + 4], TmpVec);
		}

		return _Intersect_View_8PointHull_SSE(Projection_Matrix, PointCloud_Transformed);
	}
#endif
#if defined LIBENV_CPU_ACCEL_AVX
	template <> bool _Intersect_View_AABB_Coarse<double>::Func(const double *Projection_Matrix, const double *AABB, const double *Transform_AABB){
		__m256d PointCloud[4], PointCloud_Transformed[8];

		PointCloud[0] = _mm256_broadcast_sd(&AABB[0]);
		PointCloud[1] = _mm256_set_pd(AABB[1], AABB[1], AABB[4], AABB[4]);
		PointCloud[2] = _mm256_set_pd(AABB[2], AABB[5], AABB[2], AABB[5]);

		PointCloud[3] = _mm256_broadcast_sd(&AABB[3]);

		__m256d TmpVec;

		for (UBINT i = 0; i < 4; i++){
			TmpVec = _mm256_broadcast_sd(&Transform_AABB[0 * 4 + i]);
			PointCloud_Transformed[i] = _mm256_mul_pd(TmpVec, PointCloud[0]);
			TmpVec = _mm256_broadcast_sd(&Transform_AABB[1 * 4 + i]);
			PointCloud_Transformed[i] = _mm256_add_pd(PointCloud_Transformed[i], _mm256_mul_pd(TmpVec, PointCloud[1]));
			TmpVec = _mm256_broadcast_sd(&Transform_AABB[2 * 4 + i]);
			PointCloud_Transformed[i] = _mm256_add_pd(PointCloud_Transformed[i], _mm256_mul_pd(TmpVec, PointCloud[2]));
			TmpVec = _mm256_broadcast_sd(&Transform_AABB[3 * 4 + i]);
			PointCloud_Transformed[i] = _mm256_add_pd(PointCloud_Transformed[i], TmpVec);
		}

		for (UBINT i = 0; i < 4; i++){
			TmpVec = _mm256_broadcast_sd(&Transform_AABB[0 * 4 + i]);
			PointCloud_Transformed[i + 4] = _mm256_mul_pd(TmpVec, PointCloud[3]);
			TmpVec = _mm256_broadcast_sd(&Transform_AABB[1 * 4 + i]);
			PointCloud_Transformed[i + 4] = _mm256_add_pd(PointCloud_Transformed[i + 4], _mm256_mul_pd(TmpVec, PointCloud[1]));
			TmpVec = _mm256_broadcast_sd(&Transform_AABB[2 * 4 + i]);
			PointCloud_Transformed[i + 4] = _mm256_add_pd(PointCloud_Transformed[i + 4], _mm256_mul_pd(TmpVec, PointCloud[2]));
			TmpVec = _mm256_broadcast_sd(&Transform_AABB[3 * 4 + i]);
			PointCloud_Transformed[i + 4] = _mm256_add_pd(PointCloud_Transformed[i + 4], TmpVec);
		}

		return _Intersect_View_8PointHull_AVX(Projection_Matrix, PointCloud_Transformed);
	}
#endif

	template<class T> inline bool Intersect_View_AABB_Coarse(const T *Projection_Matrix, const T *AABB, const T *Transform_AABB){ return _Intersect_View_AABB_Coarse<T>::Func(Projection_Matrix, AABB, Transform_AABB); }

	template<class T> struct _Intersect_View_Frustum_Coarse{
		static bool Func(const T *Projection_Matrix, const T *Frustum, const T *Transform_Frustum);
	};

	template<class T> bool _Intersect_View_Frustum_Coarse<T>::Func(const T *Projection_Matrix, const T *Frustum, const T *Transform_Frustum){
		//calculate the equation for six planes
		if (nsMath::NumericTrait<T>::Inf_Positive == Frustum[5]){
			//not processed yet...
			return true;
		}
		else{
			T PointCloud[8 * 4], PointCloud_Transformed[8 * 4];
			PointCloud[0 * 4 + 0] = Frustum[4]; PointCloud[0 * 4 + 1] = -Frustum[0] * Frustum[4]; PointCloud[0 * 4 + 2] = Frustum[1] * Frustum[4]; PointCloud[0 * 4 + 3] = (T)1;
			PointCloud[1 * 4 + 0] = Frustum[4]; PointCloud[1 * 4 + 1] = -Frustum[0] * Frustum[4]; PointCloud[1 * 4 + 2] = Frustum[3] * Frustum[4]; PointCloud[1 * 4 + 3] = (T)1;
			PointCloud[2 * 4 + 0] = Frustum[4]; PointCloud[2 * 4 + 1] = -Frustum[2] * Frustum[4]; PointCloud[2 * 4 + 2] = Frustum[1] * Frustum[4]; PointCloud[2 * 4 + 3] = (T)1;
			PointCloud[3 * 4 + 0] = Frustum[4]; PointCloud[3 * 4 + 1] = -Frustum[2] * Frustum[4]; PointCloud[3 * 4 + 2] = Frustum[3] * Frustum[4]; PointCloud[3 * 4 + 3] = (T)1;
			PointCloud[4 * 4 + 0] = Frustum[5]; PointCloud[4 * 4 + 1] = -Frustum[0] * Frustum[5]; PointCloud[4 * 4 + 2] = Frustum[1] * Frustum[5]; PointCloud[4 * 4 + 3] = (T)1;
			PointCloud[5 * 4 + 0] = Frustum[5]; PointCloud[5 * 4 + 1] = -Frustum[0] * Frustum[5]; PointCloud[5 * 4 + 2] = Frustum[3] * Frustum[5]; PointCloud[5 * 4 + 3] = (T)1;
			PointCloud[6 * 4 + 0] = Frustum[5]; PointCloud[6 * 4 + 1] = -Frustum[2] * Frustum[5]; PointCloud[6 * 4 + 2] = Frustum[1] * Frustum[5]; PointCloud[6 * 4 + 3] = (T)1;
			PointCloud[7 * 4 + 0] = Frustum[5]; PointCloud[7 * 4 + 1] = -Frustum[2] * Frustum[5]; PointCloud[7 * 4 + 2] = Frustum[3] * Frustum[5]; PointCloud[7 * 4 + 3] = (T)1;

			for (UBINT i = 0; i < 8; i++){
				mul4_mv(Transform_Frustum, &PointCloud[i * 4], &PointCloud_Transformed[i * 4]);
			}

			return _Intersect_View_8PointHull(Projection_Matrix, PointCloud_Transformed);
		}
	}

#if defined LIBENV_CPU_ACCEL_SSE
	template <> bool _Intersect_View_Frustum_Coarse<float>::Func(const float *Projection_Matrix, const float *Frustum, const float *Transform_Frustum){
		if (nsMath::NumericTrait<float>::Inf_Positive == Frustum[5]){
			//not processed yet...
			return true;
		}
		else{
			__m128 PointCloud[6], PointCloud_Transformed[8];

			PointCloud[0] = _mm_set_ps1(1.0f);
			PointCloud[1] = _mm_set_ps(-Frustum[0], -Frustum[0], -Frustum[2], -Frustum[2]);
			PointCloud[2] = _mm_set_ps(Frustum[1], Frustum[3], Frustum[1], Frustum[3]);

			__m128 TmpVec = _mm_set_ps1(Frustum[5]);
			PointCloud[3] = _mm_mul_ps(PointCloud[0], TmpVec);
			PointCloud[4] = _mm_mul_ps(PointCloud[1], TmpVec);
			PointCloud[5] = _mm_mul_ps(PointCloud[2], TmpVec);

			TmpVec = _mm_set_ps1(Frustum[4]);
			PointCloud[0] = _mm_mul_ps(PointCloud[0], TmpVec);
			PointCloud[1] = _mm_mul_ps(PointCloud[1], TmpVec);
			PointCloud[2] = _mm_mul_ps(PointCloud[2], TmpVec);

			for (UBINT i = 0; i < 4; i++){
				TmpVec = _mm_set_ps1(Transform_Frustum[0 * 4 + i]);
				PointCloud_Transformed[i] = _mm_mul_ps(TmpVec, PointCloud[0]);
				TmpVec = _mm_set_ps1(Transform_Frustum[1 * 4 + i]);
				PointCloud_Transformed[i] = _mm_add_ps(PointCloud_Transformed[i], _mm_mul_ps(TmpVec, PointCloud[1]));
				TmpVec = _mm_set_ps1(Transform_Frustum[2 * 4 + i]);
				PointCloud_Transformed[i] = _mm_add_ps(PointCloud_Transformed[i], _mm_mul_ps(TmpVec, PointCloud[2]));
				TmpVec = _mm_set_ps1(Transform_Frustum[3 * 4 + i]);
				PointCloud_Transformed[i] = _mm_add_ps(PointCloud_Transformed[i], TmpVec);
			}

			for (UBINT i = 0; i < 4; i++){
				TmpVec = _mm_set_ps1(Transform_Frustum[0 * 4 + i]);
				PointCloud_Transformed[i + 4] = _mm_mul_ps(TmpVec, PointCloud[3]);
				TmpVec = _mm_set_ps1(Transform_Frustum[1 * 4 + i]);
				PointCloud_Transformed[i + 4] = _mm_add_ps(PointCloud_Transformed[i + 4], _mm_mul_ps(TmpVec, PointCloud[4]));
				TmpVec = _mm_set_ps1(Transform_Frustum[2 * 4 + i]);
				PointCloud_Transformed[i + 4] = _mm_add_ps(PointCloud_Transformed[i + 4], _mm_mul_ps(TmpVec, PointCloud[5]));
				TmpVec = _mm_set_ps1(Transform_Frustum[3 * 4 + i]);
				PointCloud_Transformed[i + 4] = _mm_add_ps(PointCloud_Transformed[i + 4], TmpVec);
			}
			return _Intersect_View_8PointHull_SSE(Projection_Matrix, PointCloud_Transformed);
		}
	}
#endif
#if defined LIBENV_CPU_ACCEL_AVX
	template <> bool _Intersect_View_Frustum_Coarse<double>::Func(const double *Projection_Matrix, const double *Frustum, const double *Transform_Frustum){
		if (nsMath::NumericTrait<double>::Inf_Positive == Frustum[5]){
			//not processed yet...
			return true;
		}
		else{
			__m256d PointCloud[6], PointCloud_Transformed[8];
			const double TmpVal = 1.0;

			PointCloud[0] = _mm256_broadcast_sd(&TmpVal);
			PointCloud[1] = _mm256_set_pd(-Frustum[0], -Frustum[0], -Frustum[2], -Frustum[2]);
			PointCloud[2] = _mm256_set_pd(Frustum[1], Frustum[3], Frustum[1], Frustum[3]);

			__m256d TmpVec = _mm256_broadcast_sd(&Frustum[5]);
			PointCloud[3] = _mm256_mul_pd(PointCloud[0], TmpVec);
			PointCloud[4] = _mm256_mul_pd(PointCloud[1], TmpVec);
			PointCloud[5] = _mm256_mul_pd(PointCloud[2], TmpVec);

			TmpVec = _mm256_broadcast_sd(&Frustum[4]);
			PointCloud[0] = _mm256_mul_pd(PointCloud[0], TmpVec);
			PointCloud[1] = _mm256_mul_pd(PointCloud[1], TmpVec);
			PointCloud[2] = _mm256_mul_pd(PointCloud[2], TmpVec);

			for (UBINT i = 0; i < 4; i++){
				TmpVec = _mm256_broadcast_sd(&Transform_Frustum[0 * 4 + i]);
				PointCloud_Transformed[i] = _mm256_mul_pd(TmpVec, PointCloud[0]);
				TmpVec = _mm256_broadcast_sd(&Transform_Frustum[1 * 4 + i]);
				PointCloud_Transformed[i] = _mm256_add_pd(PointCloud_Transformed[i], _mm256_mul_pd(TmpVec, PointCloud[1]));
				TmpVec = _mm256_broadcast_sd(&Transform_Frustum[2 * 4 + i]);
				PointCloud_Transformed[i] = _mm256_add_pd(PointCloud_Transformed[i], _mm256_mul_pd(TmpVec, PointCloud[2]));
				TmpVec = _mm256_broadcast_sd(&Transform_Frustum[3 * 4 + i]);
				PointCloud_Transformed[i] = _mm256_add_pd(PointCloud_Transformed[i], TmpVec);
			}

			for (UBINT i = 0; i < 4; i++){
				TmpVec = _mm256_broadcast_sd(&Transform_Frustum[0 * 4 + i]);
				PointCloud_Transformed[i + 4] = _mm256_mul_pd(TmpVec, PointCloud[3]);
				TmpVec = _mm256_broadcast_sd(&Transform_Frustum[1 * 4 + i]);
				PointCloud_Transformed[i + 4] = _mm256_add_pd(PointCloud_Transformed[i + 4], _mm256_mul_pd(TmpVec, PointCloud[4]));
				TmpVec = _mm256_broadcast_sd(&Transform_Frustum[2 * 4 + i]);
				PointCloud_Transformed[i + 4] = _mm256_add_pd(PointCloud_Transformed[i + 4], _mm256_mul_pd(TmpVec, PointCloud[5]));
				TmpVec = _mm256_broadcast_sd(&Transform_Frustum[3 * 4 + i]);
				PointCloud_Transformed[i + 4] = _mm256_add_pd(PointCloud_Transformed[i + 4], TmpVec);
			}
			return _Intersect_View_8PointHull_AVX(Projection_Matrix, PointCloud_Transformed);
		}
	}
#endif

	template<class T> inline bool Intersect_View_Frustum_Coarse(const T *Projection_Matrix, const T *Frustum, const T *Transform_Frustum){
		return _Intersect_View_Frustum_Coarse<T>::Func(Projection_Matrix, Frustum, Transform_Frustum);
	}
}
#endif