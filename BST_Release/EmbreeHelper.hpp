#ifndef EMBREE_HELPER
#define EMBREE_HELPER

#include "embree2/rtcore.h"
#include "embree2/rtcore_ray.h"
#pragma comment(lib, "embree.lib")

#include "SoundMaterial.hpp"

#define EMBREE_CHECK_ERROR if (RTC_NO_ERROR != rtcGetError())throw std::exception()

#if defined LIBENV_CPU_ACCEL_AVX
#define RTCRAY_ALIGNMENT 32
const RTCAlgorithmFlags Embree_SIMDScale = RTC_INTERSECT8;
const UBINT Embree_RayClusterSize = 8;
#elif defined LIBENV_CPU_ACCEL_SSE
#define RTCRAY_ALIGNMENT 16
const RTCAlgorithmFlags Embree_SIMDScale = RTC_INTERSECT4;
const UBINT Embree_RayClusterSize = 4;
#else
#define RTCRAY_ALIGNMENT 1
const RTCAlgorithmFlags Embree_SIMDScale = RTC_INTERSECT1;
const UBINT Embree_RayClusterSize = 1;
#endif

template <UBINT I> struct RTCORE_ALIGN(RTCRAY_ALIGNMENT) RTCRayCluster
{
	/* ray data */
public:
	float orgx[I];
	float orgy[I];
	float orgz[I];

	float dirx[I];
	float diry[I];
	float dirz[I];

	float tnear[I];
	float tfar[I];

	float time[I];
	int   mask[I];

	/* hit data */
public:
	float Ngx[I];
	float Ngy[I];
	float Ngz[I];

	float u[I];
	float v[I];

	int   geomID[I];
	int   primID[I];
	int   instID[I];
};

template <UBINT I> void _rtcIntersect(const UINT4b *valid, RTCScene scene, RTCRayCluster<I> &ray);
template <UBINT I> void _rtcOccluded(const UINT4b *valid, RTCScene scene, RTCRayCluster<I> &ray);
template <typename T, UBINT I> struct __rtcSetIntersectFunction{
	static void Func(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(const UINT4b *, T *, RTCRayCluster<I>&, size_t));
};
template <typename T, UBINT I> struct __rtcSetOccludedFunction{
	static void Func(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(const UINT4b *, T *, RTCRayCluster<I>&, size_t));
};

template <typename T, UBINT I> inline void _rtcSetIntersectFunction(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(const UINT4b *, T *, RTCRayCluster<I>&, size_t)){
	__rtcSetIntersectFunction<T, I>::Func(scene, GeometryID, lpCallback);
}
template <typename T, UBINT I> inline void _rtcSetOccludedFunction(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(const UINT4b *, T *, RTCRayCluster<I>&, size_t)){
	__rtcSetOccludedFunction<T, I>::Func(scene, GeometryID, lpCallback);
}

template <> inline void _rtcIntersect<1>(const UINT4b *valid, RTCScene scene, RTCRayCluster<1> &ray){ if (0xFFFFFFFF == *valid)rtcIntersect(scene, (RTCRay&)ray); }
template <> inline void _rtcIntersect<4>(const UINT4b *valid, RTCScene scene, RTCRayCluster<4> &ray){ rtcIntersect4(valid, scene, (RTCRay4&)ray); }
template <> inline void _rtcIntersect<8>(const UINT4b *valid, RTCScene scene, RTCRayCluster<8> &ray){ rtcIntersect8(valid, scene, (RTCRay8&)ray); }

template <> inline void _rtcOccluded<1>(const UINT4b *valid, RTCScene scene, RTCRayCluster<1> &ray){ if (0xFFFFFFFF == *valid)rtcOccluded(scene, (RTCRay&)ray); }
template <> inline void _rtcOccluded<4>(const UINT4b *valid, RTCScene scene, RTCRayCluster<4> &ray){ rtcOccluded4(valid, scene, (RTCRay4&)ray); }
template <> inline void _rtcOccluded<8>(const UINT4b *valid, RTCScene scene, RTCRayCluster<8> &ray){ rtcOccluded8(valid, scene, (RTCRay8&)ray); }

template <typename T> struct __rtcSetIntersectFunction<T, 1>{
	static inline void Func(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(T *, RTCRayCluster<1>&, size_t)){
		rtcSetIntersectFunction(scene, GeometryID, (RTCIntersectFunc)lpCallback);
	}
};
template <typename T> struct __rtcSetIntersectFunction<T, 4>{
	static inline void Func(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(const UINT4b *, T *, RTCRayCluster<4>&, size_t)){
		rtcSetIntersectFunction4(scene, GeometryID, (RTCIntersectFunc4)lpCallback);
	}
};

template <typename T> struct __rtcSetIntersectFunction<T, 8>{
	static inline void Func(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(const UINT4b *, T *, RTCRayCluster<8>&, size_t)){
		rtcSetIntersectFunction8(scene, GeometryID, (RTCIntersectFunc8)lpCallback);
	}
};

template <typename T> struct __rtcSetOccludedFunction<T, 1>{
	static inline void Func(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(T *, RTCRayCluster<1>&, size_t)){
		rtcSetOccludedFunction(scene, GeometryID, (RTCOccludedFunc)lpCallback);
	}
};
template <typename T> struct __rtcSetOccludedFunction<T, 4>{
	static inline void Func(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(const UINT4b *, T *, RTCRayCluster<4>&, size_t)){
		rtcSetOccludedFunction4(scene, GeometryID, (RTCOccludedFunc4)lpCallback);
	}
};

template <typename T> struct __rtcSetOccludedFunction<T, 8>{
	static inline void Func(RTCScene scene, unsigned int GeometryID, void(*lpCallback)(const UINT4b *, T *, RTCRayCluster<8>&, size_t)){
		rtcSetOccludedFunction8(scene, GeometryID, (RTCOccludedFunc8)lpCallback);
	}
};

RTCScene Embree_Scene_Solid;
RTCScene Embree_Scene_Dynamic;
nsContainer::HashMap<unsigned int, MaterialProperty_Sound_Surface *> *Embree_MaterialMap;
void Embree_Init(){
	try{
		rtcInit(); EMBREE_CHECK_ERROR;
		Embree_Scene_Solid = rtcNewScene(RTC_SCENE_DYNAMIC | RTC_SCENE_INCOHERENT, Embree_SIMDScale); EMBREE_CHECK_ERROR;
		Embree_Scene_Dynamic = rtcNewScene(RTC_SCENE_DYNAMIC | RTC_SCENE_INCOHERENT, Embree_SIMDScale); EMBREE_CHECK_ERROR;
		Embree_MaterialMap = new nsContainer::HashMap<unsigned int, MaterialProperty_Sound_Surface *>;
	}
	catch (...){
		throw;
	}
}
void Embree_ClearScene(){
	Embree_MaterialMap->clear();
	rtcDeleteScene(Embree_Scene_Dynamic); EMBREE_CHECK_ERROR;
	rtcDeleteScene(Embree_Scene_Solid); EMBREE_CHECK_ERROR;
	Embree_Scene_Solid = rtcNewScene(RTC_SCENE_DYNAMIC | RTC_SCENE_INCOHERENT, Embree_SIMDScale); EMBREE_CHECK_ERROR;
	Embree_Scene_Dynamic = rtcNewScene(RTC_SCENE_DYNAMIC | RTC_SCENE_INCOHERENT, Embree_SIMDScale); EMBREE_CHECK_ERROR;
	rtcCommit(Embree_Scene_Solid); EMBREE_CHECK_ERROR;
	rtcCommit(Embree_Scene_Dynamic); EMBREE_CHECK_ERROR;
}
void Embree_Destroy(){
	try{
		delete Embree_MaterialMap;
		rtcDeleteScene(Embree_Scene_Dynamic); EMBREE_CHECK_ERROR;
		rtcDeleteScene(Embree_Scene_Solid); EMBREE_CHECK_ERROR;
		rtcExit(); EMBREE_CHECK_ERROR;
	}
	catch (...){
		throw;
	}
	
}

#endif