#include "lGeneral.hpp"
#include "EmbreeHelper.hpp"

#include "lMath_Rand.hpp"

struct ConnectionShuffler{
	UINT4b Source;
	UINT4b Listener;
	UINT4b PathSpace;
	UINT4b MidStrategyCount;
	UINT4b SampleCount[3];
	UINT4b ShuffleCycle[3];
	UINT4b k0[3];
	UINT4b k1[3];
	// shuffler from 0 to 3: strategy 0-x, strategy x-0, strategy x-x
};

struct SndSrcInfo{
	float Position[3];
	float Intensity;
	bool PathValid;
};

struct _TraceConnectKernelInfo{
	nsMath::TinyVector<UINT4b, 4> ConnectIndex;
	UBINT KernelIndex;
	ConnectionShuffler *lpShuffler;
	float LengthSqr;
	float OutgoingAngle_Cos[2];
};

struct PathNodeData{
	nsMath::TinyVector<float, 3> Position;
	nsMath::TinyVector<float, 3> Normal;
	bool IsBackFace;
	float Length;
	MaterialProperty_Sound_Surface *Material;
	FloatVec_SIMD Intensity;
};

struct PathNodeData_Inter{
	float InvNodeProb;
	float InvSegProb;
	float InvProbSum;
	nsMath::TinyVector<float, 3> Incident_Dir;
};

//Kernel State --BEGIN--
struct EmbreePathTraceState{
	SndSrcInfo *In_SrcInfo;
	nsMath::TinyVector<UINT4b, 2> *In_SrcPathRange;
	UINT4b *In_SrcIndex;
	nsMath::TinyVector<float, 3> *In_InitSamples;

	PathNodeData_Inter *Inter_PathNodeData;

	PathNodeData *Out_PathNodeData;
	UINT4b *Out_Bounces;

	UBINT PathDepth;
	UBINT PathCnt;
};

struct EmbreeSharedState{
	float PathLenUpperBound;
	nsMath::RandGenerator *lpRandGenerator;
};

struct EmbreeKernelState{
	//Buffers
	EmbreePathTraceState TraceState;

	ConnectionShuffler *In_Shuffler;
	UINT4b *In_ShufflerIndex;

	nsMath::TinyVector<float, 2> *Out_Connection;
	nsMath::TinyVector<UINT4b, 4> *Out_ConnectIndex;
	FloatVec_SIMD *Out_ConnectIntensity;

	EmbreeSharedState SharedState;
};
//Kernel State --END-

template <UBINT I> void BDPT_Trace_Kernel(EmbreePathTraceState *lpState, EmbreeSharedState *lpState_Shared, UBINT RayCount, UBINT *KernelIndex){
	struct PathDataStruct{
		float Length;
		float InvProbSum;
		float OutDirCos;
		float LastInvNodeProb;
		float LastInvSegProb;
	} PathData[I];
	FloatVec_SIMD Intensity[I];
	RTCRayCluster<I> MyRayCluster;

	UINT4b ValidMask[I];
	UBINT PathIndex[I];
	for (UBINT i = 0; i < I; i++)ValidMask[i] = 0x0;
	for (UBINT i = 0; i < RayCount; i++)PathIndex[i] = KernelIndex[i] * lpState->PathDepth;
	for (UBINT i = 0; i < RayCount; i++){
		MyRayCluster.orgx[i] = lpState->In_SrcInfo[lpState->In_SrcIndex[KernelIndex[i]]].Position[0];
		MyRayCluster.orgy[i] = lpState->In_SrcInfo[lpState->In_SrcIndex[KernelIndex[i]]].Position[1];
		MyRayCluster.orgz[i] = lpState->In_SrcInfo[lpState->In_SrcIndex[KernelIndex[i]]].Position[2];

#if defined USE_SAMPLE_PATTERN
		MyRayCluster.dirx[i] = lpState->In_InitSamples[KernelIndex[i]][0];
		MyRayCluster.diry[i] = lpState->In_InitSamples[KernelIndex[i]][1];
		MyRayCluster.dirz[i] = lpState->In_InitSamples[KernelIndex[i]][2];
#else
		nsMath::TinyVector<float, 3> RandVector;
		nsMath::Distribution_Uniform_Spherical(RandVector.Data, lpState_Shared->lpRandGenerator);

		MyRayCluster.dirx[i] = RandVector[0];
		MyRayCluster.diry[i] = RandVector[1];
		MyRayCluster.dirz[i] = RandVector[2];
#endif

		MyRayCluster.tnear[i] = 0.0001f;
		MyRayCluster.tfar[i] = 1.e27f;
		MyRayCluster.time[i] = 0.0f;
		MyRayCluster.mask[i] = 0xFFFFFFFF;
		
		MyRayCluster.geomID[i] = 0xFFFFFFFF;
		MyRayCluster.instID[i] = 0xFFFFFFFF;

		ValidMask[i] = 0xFFFFFFFF;

		UBINT CurPathIndex = PathIndex[i];

		PathData[i].Length = 0.0f;
		PathData[i].InvProbSum = 0.0f;
		Intensity[i].set(1.0f);
		// Length=0.0,
		// Intensity / Probability = 1.0
		// ** Intensity=1.0 / (4 * M_PI), Probability density=1.0 / (4 * M_PI) **
		// Inverse probability sum = 0.0
		PathData[i].OutDirCos = 1.0f; // cos(M_PI_2) = 1.0
		PathData[i].LastInvNodeProb = 4.0f * (float)M_PI;
		PathData[i].LastInvSegProb = 0.0f;

		lpState->Out_PathNodeData[CurPathIndex].Position[0] = MyRayCluster.orgx[i];
		lpState->Out_PathNodeData[CurPathIndex].Position[1] = MyRayCluster.orgy[i];
		lpState->Out_PathNodeData[CurPathIndex].Position[2] = MyRayCluster.orgz[i];
		lpState->Out_PathNodeData[CurPathIndex].Normal[0] = 1.0f;
		lpState->Out_PathNodeData[CurPathIndex].Normal[1] = 0.0f;
		lpState->Out_PathNodeData[CurPathIndex].Normal[2] = 0.0f;
		lpState->Out_PathNodeData[CurPathIndex].Length = PathData[i].Length;

		lpState->Inter_PathNodeData[CurPathIndex].InvNodeProb = 0.0f;
		lpState->Inter_PathNodeData[CurPathIndex].InvSegProb = 0.0f;
		lpState->Inter_PathNodeData[CurPathIndex].InvProbSum = PathData[i].InvProbSum;
	}

	UBINT ValidRayCount = RayCount;

	for (UBINT i = 1; i < lpState->PathDepth; i++){
		_rtcIntersect(ValidMask, Embree_Scene_Solid, MyRayCluster); EMBREE_CHECK_ERROR;
		for (UBINT j = 0; j < RayCount; j++){
			if (0xFFFFFFFF == ValidMask[j]){
				if (0xFFFFFFFF == MyRayCluster.geomID[j] || PathData[j].Length + MyRayCluster.tfar[j] >= lpState_Shared->PathLenUpperBound){
					ValidMask[j] = 0x0;
					ValidRayCount -= 1;
					lpState->Out_Bounces[KernelIndex[j]] = (UINT4b)i;
				}
				else{
					//material
					PathIndex[j]++;
					UBINT CurPathIndex = PathIndex[j];

					MyRayCluster.orgx[j] += MyRayCluster.tfar[j] * MyRayCluster.dirx[j];
					MyRayCluster.orgy[j] += MyRayCluster.tfar[j] * MyRayCluster.diry[j];
					MyRayCluster.orgz[j] += MyRayCluster.tfar[j] * MyRayCluster.dirz[j];

					lpState->Out_PathNodeData[CurPathIndex].Position[0] = MyRayCluster.orgx[j];
					lpState->Out_PathNodeData[CurPathIndex].Position[1] = MyRayCluster.orgy[j];
					lpState->Out_PathNodeData[CurPathIndex].Position[2] = MyRayCluster.orgz[j];
					lpState->Inter_PathNodeData[CurPathIndex].InvNodeProb = PathData[j].LastInvNodeProb;

					nsMath::TinyVector<float, 3> Norm = { MyRayCluster.Ngx[j], MyRayCluster.Ngy[j], MyRayCluster.Ngz[j] };
					nsMath::normalize<3>(Norm.Data, Norm.Data);
					nsMath::TinyVector<float, 3> DirVec = { MyRayCluster.dirx[j], MyRayCluster.diry[j], MyRayCluster.dirz[j] };
					if (nsMath::dot<3>(Norm.Data, DirVec.Data) > 0.0f){
						Norm[0] = -Norm[0]; Norm[1] = -Norm[1]; Norm[2] = -Norm[2];
						lpState->Out_PathNodeData[CurPathIndex].IsBackFace = true;
					}
					else lpState->Out_PathNodeData[CurPathIndex].IsBackFace = false;
					lpState->Out_PathNodeData[CurPathIndex].Material = Embree_MaterialMap->find(MyRayCluster.geomID[j])->second;

					PathData[j].Length += MyRayCluster.tfar[j];
					PathData[j].InvProbSum += PathData[j].LastInvSegProb;
					lpState->Out_PathNodeData[CurPathIndex].Length = PathData[j].Length;
					lpState->Inter_PathNodeData[CurPathIndex].InvProbSum = PathData[j].InvProbSum;

					//get the material info and apply
					lpState->Out_PathNodeData[CurPathIndex].Intensity = Intensity[j];
					Intensity[j].mul(lpState->Out_PathNodeData[CurPathIndex].Material->Diffuse);

					lpState->Out_PathNodeData[CurPathIndex].Normal[0] = Norm[0];
					lpState->Out_PathNodeData[CurPathIndex].Normal[1] = Norm[1];
					lpState->Out_PathNodeData[CurPathIndex].Normal[2] = Norm[2];
					lpState->Inter_PathNodeData[CurPathIndex].InvSegProb = -MyRayCluster.tfar[j] * MyRayCluster.tfar[j] / (PathData[j].OutDirCos * nsMath::dot<3>(DirVec.Data, Norm.Data));

					nsMath::normalize3(DirVec.Data, DirVec.Data);
					DirVec[0] = -DirVec[0]; DirVec[1] = -DirVec[1]; DirVec[2] = -DirVec[2];
					lpState->Inter_PathNodeData[CurPathIndex].Incident_Dir = DirVec;

					nsMath::TinyVector<float, 3> RandVector;
					nsMath::Distribution_CosWeighted_Hemispherical(RandVector.Data, lpState_Shared->lpRandGenerator);
					PathData[j].OutDirCos = RandVector[0];
					PathData[j].LastInvNodeProb = (float)M_PI; //current inverse material probablity
					PathData[j].LastInvSegProb = PathData[j].LastInvNodeProb * lpState->Inter_PathNodeData[CurPathIndex].InvSegProb * lpState->Inter_PathNodeData[CurPathIndex].InvNodeProb;
					// current inverse material probablity * last inverse material probablity * propagation probability

					nsMath::TinyVector<float, 3> D0, D1;
					nsMath::vert3(D0.Data, Norm.Data);
					nsMath::normalize3(D0.Data, D0.Data);
					nsMath::cross3(D1.Data, Norm.Data, D0.Data);

					MyRayCluster.dirx[j] = Norm[0] * RandVector[0] + D0[0] * RandVector[1] + D1[0] * RandVector[2];
					MyRayCluster.diry[j] = Norm[1] * RandVector[0] + D0[1] * RandVector[1] + D1[1] * RandVector[2];
					MyRayCluster.dirz[j] = Norm[2] * RandVector[0] + D0[2] * RandVector[1] + D1[2] * RandVector[2];

					MyRayCluster.tfar[j] = 100000.0f;

					MyRayCluster.geomID[j] = 0xFFFFFFFF;
					MyRayCluster.instID[j] = 0xFFFFFFFF;
				}
			}
		}
		if (0 == ValidRayCount)break;
	}
	for (UBINT i = 0; i < RayCount; i++){
		if (0xFFFFFFFF == ValidMask[i])lpState->Out_Bounces[KernelIndex[i]] = (UINT4b)lpState->PathDepth;
	}
}

template <UBINT I> void BDPT_TraceConnect_Kernel(EmbreeKernelState *lpState, _TraceConnectKernelInfo *lpInfo, UBINT RayCount){
	RTCRayCluster<I> MyRayCluster;
	
	UINT4b ValidMask[I];

	for (UBINT i = 0; i < RayCount; i++){
		UBINT PathIndex0 = lpInfo[i].ConnectIndex[0] * lpState->TraceState.PathDepth + lpInfo[i].ConnectIndex[1];
		UBINT PathIndex1 = lpInfo[i].ConnectIndex[2] * lpState->TraceState.PathDepth + lpInfo[i].ConnectIndex[3];

		MyRayCluster.orgx[i] = lpState->TraceState.Out_PathNodeData[PathIndex0].Position[0];
		MyRayCluster.orgy[i] = lpState->TraceState.Out_PathNodeData[PathIndex0].Position[1];
		MyRayCluster.orgz[i] = lpState->TraceState.Out_PathNodeData[PathIndex0].Position[2];

		MyRayCluster.dirx[i] = lpState->TraceState.Out_PathNodeData[PathIndex1].Position[0] - MyRayCluster.orgx[i];
		MyRayCluster.diry[i] = lpState->TraceState.Out_PathNodeData[PathIndex1].Position[1] - MyRayCluster.orgy[i];
		MyRayCluster.dirz[i] = lpState->TraceState.Out_PathNodeData[PathIndex1].Position[2] - MyRayCluster.orgz[i];

		MyRayCluster.tnear[i] = 0.000001f;
		MyRayCluster.tfar[i] = 1.0f - 0.000001f;
		MyRayCluster.time[i] = 0.0f;
		MyRayCluster.mask[i] = 0xFFFFFFFF;

		MyRayCluster.geomID[i] = 0xFFFFFFFF;
		MyRayCluster.instID[i] = 0xFFFFFFFF;

		ValidMask[i] = 0xFFFFFFFF;
	}
	for (UBINT i = RayCount; i < I; i++){
		ValidMask[i] = 0x0;
	}
	_rtcOccluded(ValidMask, Embree_Scene_Solid, MyRayCluster); EMBREE_CHECK_ERROR;
	for (UBINT i = 0; i < RayCount; i++){
		UBINT PathIndex0 = lpInfo[i].ConnectIndex[0] * lpState->TraceState.PathDepth + lpInfo[i].ConnectIndex[1];
		UBINT PathIndex1 = lpInfo[i].ConnectIndex[2] * lpState->TraceState.PathDepth + lpInfo[i].ConnectIndex[3];
		UBINT PathSpace = lpInfo[i].ConnectIndex[1] + lpInfo[i].ConnectIndex[3];

		if (0 != MyRayCluster.geomID[i]){
			//visible
			FloatVec_SIMD Intensity[2];
			float InvProb[2];

			if (0 == lpInfo[i].ConnectIndex[1]){
				Intensity[0].set(1.0f / (float)(4 * M_PI));
#if defined USE_MIS
				InvProb[0] = 4.0f;
#endif
			}
			else{
				Intensity[0] = *(FloatVec_SIMD *)&lpState->TraceState.Out_PathNodeData[PathIndex0].Intensity;
				//get the material info and apply
				Intensity[0].mul(lpState->TraceState.Out_PathNodeData[PathIndex0].Material->Diffuse);
				Intensity[0].mul(lpInfo[i].OutgoingAngle_Cos[0] / (float)(M_PI * sqrt(lpInfo[i].LengthSqr)));
#if defined USE_MIS
				InvProb[0] = sqrt(lpInfo[i].LengthSqr) / lpInfo[i].OutgoingAngle_Cos[0];
#endif
			}

			if (0 == lpInfo[i].ConnectIndex[3]){
				Intensity[1].set(1.0f / (float)(4 * M_PI));
#if defined USE_MIS
				InvProb[1] = 4.0f;
#endif
			}
			else{
				Intensity[1] = *(FloatVec_SIMD *)&lpState->TraceState.Out_PathNodeData[PathIndex1].Intensity;
				//get the material info and apply
				Intensity[1].mul(lpState->TraceState.Out_PathNodeData[PathIndex1].Material->Diffuse);
				Intensity[1].mul(lpInfo[i].OutgoingAngle_Cos[1] / (float)(M_PI * sqrt(lpInfo[i].LengthSqr)));
#if defined USE_MIS
				InvProb[1] = sqrt(lpInfo[i].LengthSqr) / lpInfo[i].OutgoingAngle_Cos[1];
#endif
			}

			Intensity[0].mul(Intensity[1]);
			Intensity[0].mul(1.0f / lpInfo[i].LengthSqr);

#if defined USE_MIS
			InvProb[0] *= InvProb[1] * lpInfo[i].LengthSqr;

			float MatInvProb_Forward = (float)M_PI;
			float MatInvProb_Backward = (float)M_PI;
			InvProb[0] *= MatInvProb_Forward * MatInvProb_Backward;

			// scale the factor according to sample distribution
			float MidStrategy_SampleCnt = (float)lpInfo[i].lpShuffler->SampleCount[2] / (float)lpInfo[i].lpShuffler->MidStrategyCount;

			float InvProbSum = InvProb[0], CurStrategySampleCnt, TmpInvProb;
			if (0 == lpInfo[i].ConnectIndex[1])CurStrategySampleCnt = (float)lpInfo[i].lpShuffler->SampleCount[0];
			else if (0 == lpInfo[i].ConnectIndex[3])CurStrategySampleCnt = (float)lpInfo[i].lpShuffler->SampleCount[1];
			else CurStrategySampleCnt = MidStrategy_SampleCnt;
			InvProbSum *= CurStrategySampleCnt;

			// calculating MIS factor
			TmpInvProb = MatInvProb_Forward * lpState->TraceState.Inter_PathNodeData[PathIndex0].InvSegProb * lpState->TraceState.Inter_PathNodeData[PathIndex0].InvNodeProb;
			if (lpInfo[i].ConnectIndex[1] <= 1){
				TmpInvProb *= (float)lpInfo[i].lpShuffler->SampleCount[0];
				InvProbSum += TmpInvProb;
			}
			else{
				TmpInvProb *= MidStrategy_SampleCnt;
				InvProbSum += TmpInvProb;

				TmpInvProb = lpState->TraceState.Inter_PathNodeData[PathIndex0].InvProbSum;
				TmpInvProb -= lpState->TraceState.Inter_PathNodeData[PathIndex0 - (lpInfo[i].ConnectIndex[1] - 2)].InvProbSum;
				TmpInvProb *= MidStrategy_SampleCnt;
				InvProbSum += TmpInvProb;

				TmpInvProb = lpState->TraceState.Inter_PathNodeData[PathIndex0 - (lpInfo[i].ConnectIndex[1] - 2)].InvProbSum;
				TmpInvProb *= (float)lpInfo[i].lpShuffler->SampleCount[0];
				InvProbSum += TmpInvProb;
			}

			TmpInvProb = MatInvProb_Backward * lpState->TraceState.Inter_PathNodeData[PathIndex1].InvSegProb * lpState->TraceState.Inter_PathNodeData[PathIndex1].InvNodeProb;
			if (lpInfo[i].ConnectIndex[3] <= 1){
				TmpInvProb *= (float)lpInfo[i].lpShuffler->SampleCount[1];
				InvProbSum += TmpInvProb;
			}
			else{
				TmpInvProb *= MidStrategy_SampleCnt;
				InvProbSum += TmpInvProb;

				TmpInvProb = lpState->TraceState.Inter_PathNodeData[PathIndex1].InvProbSum;
				TmpInvProb -= lpState->TraceState.Inter_PathNodeData[PathIndex1 - (lpInfo[i].ConnectIndex[3] - 2)].InvProbSum;
				TmpInvProb *= MidStrategy_SampleCnt;
				InvProbSum += TmpInvProb;

				TmpInvProb = lpState->TraceState.Inter_PathNodeData[PathIndex1 - (lpInfo[i].ConnectIndex[3] - 2)].InvProbSum;
				TmpInvProb *= (float)lpInfo[i].lpShuffler->SampleCount[1];
				InvProbSum += TmpInvProb;
			}
#endif

			lpState->Out_Connection[lpInfo[i].KernelIndex][0] = lpState->TraceState.Out_PathNodeData[PathIndex0].Length + lpState->TraceState.Out_PathNodeData[PathIndex1].Length + sqrt(lpInfo[i].LengthSqr);
			lpState->Out_ConnectIntensity[lpInfo[i].KernelIndex] = Intensity[0];

#if defined USE_MIS
			lpState->Out_Connection[lpInfo[i].KernelIndex][1] = InvProb[0] * (float)(lpInfo[i].lpShuffler->SampleCount[0] + lpInfo[i].lpShuffler->SampleCount[1] + lpInfo[i].lpShuffler->SampleCount[2]) / InvProbSum;
#endif
		}
		else{
			lpState->Out_Connection[lpInfo[i].KernelIndex][0] = 0.0f; lpState->Out_Connection[lpInfo[i].KernelIndex][1] = 0.0f;
			lpState->Out_ConnectIntensity[lpInfo[i].KernelIndex].set(0.0f);
		}
	}
}

void BDPT_TraceFB(EmbreeKernelState *lpState, UBINT RayCount){
	//SIMD ray tracing is probably not a good idea.
	//Intel SIMD instruction set doesn't support gather/scatter memory access, and our ray has little coherence. We'll definitely meet branches inside the raytracer.
#if defined LIBENV_CPU_ACCEL_AVX
	const UBINT RayClusterSize = 8;
#elif defined LIBENV_CPU_ACCEL_SSE
	const UBINT RayClusterSize = 4;
#else
	const UBINT RayClusterSize = 1;
#endif

	UBINT RayIndex[RayClusterSize], ValidRayCount = 0;
	UBINT KernelIndex = 0;
	for (UBINT i = 0; i < RayCount; i++){
		if (!lpState->TraceState.In_SrcInfo[lpState->TraceState.In_SrcIndex[i]].PathValid){
			RayIndex[ValidRayCount] = i;
			ValidRayCount++;
			if (ValidRayCount >= RayClusterSize){
				BDPT_Trace_Kernel<RayClusterSize>(&lpState->TraceState, &lpState->SharedState, ValidRayCount, RayIndex);
				ValidRayCount = 0;
			}
		}
	}
	if (ValidRayCount > 0)BDPT_Trace_Kernel<RayClusterSize>(&lpState->TraceState, &lpState->SharedState, ValidRayCount, RayIndex);
}

void BDPT_TraceConnect(EmbreeKernelState *lpState, UBINT ConnectionCount){
	//SIMD ray tracing is probably not a good idea.
	//Intel SIMD instruction set doesn't support gather/scatter memory access, and our ray has little coherence. We'll definitely meet branches inside the raytracer.
#if defined LIBENV_CPU_ACCEL_AVX
	const UBINT RayClusterSize = 8;
#elif defined LIBENV_CPU_ACCEL_SSE
	const UBINT RayClusterSize = 4;
#else
	const UBINT RayClusterSize = 1;
#endif

	_TraceConnectKernelInfo MyConnectKernelInfo[RayClusterSize];

	UBINT ValidConnectionCount = 0;
	for (UBINT i = 0; i < ConnectionCount; i++){
		//randomly generate connection segments --BEGIN--
		UINT4b CurSourceCluster, CurListenerCluster;
		UINT4b FwdPathIndex, BwdPathIndex, FwdRayBounce, BwdRayBounce;

		auto ShufflerDecoder = [&](ConnectionShuffler &Shuffler, UBINT Index){
			CurSourceCluster = Shuffler.Source;
			CurListenerCluster = Shuffler.Listener;

			if (0 == Shuffler.PathSpace){ FwdPathIndex = 0; BwdPathIndex = 0; FwdRayBounce = 0; BwdRayBounce = 0; }
			else{
				UBINT TypeIndex = Index % (Shuffler.SampleCount[0] + Shuffler.SampleCount[1] + Shuffler.SampleCount[2]);

				UBINT SampleType = 0;
				if (TypeIndex >= Shuffler.SampleCount[0]){
					SampleType++; TypeIndex -= Shuffler.SampleCount[0];
					if (TypeIndex >= Shuffler.SampleCount[1]){ SampleType++; TypeIndex -= Shuffler.SampleCount[1]; }
				}

				UINT8b RndNumber = Index * Shuffler.k1[SampleType] + Shuffler.k0[SampleType];
				UINT4b TmpInt = (UINT4b)(RndNumber % Shuffler.ShuffleCycle[SampleType]);

				if (0 == SampleType){
					FwdPathIndex = 0;
					BwdPathIndex = TmpInt;
					FwdRayBounce = 0;
				}
				else if (1 == SampleType){
					FwdPathIndex = TmpInt;
					BwdPathIndex = 0;
					FwdRayBounce = Shuffler.PathSpace;
				}
				else{
					FwdRayBounce = 1;
					if (Shuffler.PathSpace > lpState->TraceState.PathDepth)FwdRayBounce += Shuffler.PathSpace - lpState->TraceState.PathDepth;

					FwdPathIndex = TmpInt % lpState->TraceState.In_SrcPathRange[CurSourceCluster][1];
					TmpInt /= lpState->TraceState.In_SrcPathRange[CurSourceCluster][1];
					BwdPathIndex = TmpInt % lpState->TraceState.In_SrcPathRange[CurListenerCluster][1];
					TmpInt /= lpState->TraceState.In_SrcPathRange[CurListenerCluster][1];
					FwdRayBounce += TmpInt;
				}
				BwdRayBounce = Shuffler.PathSpace - FwdRayBounce;
			}
		};

		ShufflerDecoder(lpState->In_Shuffler[lpState->In_ShufflerIndex[i]], i);
		FwdPathIndex += lpState->TraceState.In_SrcPathRange[CurSourceCluster][0];
		BwdPathIndex += lpState->TraceState.In_SrcPathRange[CurListenerCluster][0];

		//randomly generate connection segments --END--

		lpState->Out_ConnectIndex[i][0] = FwdPathIndex;
		lpState->Out_ConnectIndex[i][1] = FwdRayBounce;
		lpState->Out_ConnectIndex[i][2] = BwdPathIndex;
		lpState->Out_ConnectIndex[i][3] = BwdRayBounce;

		UINT4b RayDepth0 = lpState->TraceState.Out_Bounces[FwdPathIndex];
		UINT4b RayDepth1 = lpState->TraceState.Out_Bounces[BwdPathIndex];

		if (FwdRayBounce < lpState->TraceState.Out_Bounces[FwdPathIndex] && BwdRayBounce < lpState->TraceState.Out_Bounces[BwdPathIndex]){
			UINT4b RayPathIndex0 = FwdPathIndex * lpState->TraceState.PathDepth + FwdRayBounce;
			UINT4b RayPathIndex1 = BwdPathIndex * lpState->TraceState.PathDepth + BwdRayBounce;

			nsMath::TinyVector<float, 3> NewRayDirection;
			nsMath::sub<3>(NewRayDirection.Data, lpState->TraceState.Out_PathNodeData[RayPathIndex1].Position.Data, lpState->TraceState.Out_PathNodeData[RayPathIndex0].Position.Data);
			MyConnectKernelInfo[ValidConnectionCount].LengthSqr = nsMath::dot<3>(NewRayDirection.Data, NewRayDirection.Data);

			if (FwdRayBounce > 0)MyConnectKernelInfo[ValidConnectionCount].OutgoingAngle_Cos[0] = nsMath::dot<3>(lpState->TraceState.Out_PathNodeData[RayPathIndex0].Normal.Data, NewRayDirection.Data);
			else MyConnectKernelInfo[ValidConnectionCount].OutgoingAngle_Cos[0] = 1.0f;
			if (BwdRayBounce > 0)MyConnectKernelInfo[ValidConnectionCount].OutgoingAngle_Cos[1] = -nsMath::dot<3>(lpState->TraceState.Out_PathNodeData[RayPathIndex1].Normal.Data, NewRayDirection.Data);
			else MyConnectKernelInfo[ValidConnectionCount].OutgoingAngle_Cos[1] = 1.0f;

			float TotalLength = lpState->TraceState.Out_PathNodeData[RayPathIndex0].Length + lpState->TraceState.Out_PathNodeData[RayPathIndex1].Length + sqrt(MyConnectKernelInfo[ValidConnectionCount].LengthSqr);

			if (MyConnectKernelInfo[ValidConnectionCount].OutgoingAngle_Cos[0] > 0.0f && MyConnectKernelInfo[ValidConnectionCount].OutgoingAngle_Cos[1] > 0.0f && TotalLength < lpState->SharedState.PathLenUpperBound){
				MyConnectKernelInfo[ValidConnectionCount].ConnectIndex = lpState->Out_ConnectIndex[i];
				MyConnectKernelInfo[ValidConnectionCount].KernelIndex = i;
				MyConnectKernelInfo[ValidConnectionCount].lpShuffler = &lpState->In_Shuffler[lpState->In_ShufflerIndex[i]];
				ValidConnectionCount++;
			}
			else{
				lpState->Out_Connection[i][0] = 0.0f; lpState->Out_Connection[i][1] = 0.0f;
				lpState->Out_ConnectIntensity[i].set(0.0f);
			}

			if (ValidConnectionCount >= RayClusterSize){
				BDPT_TraceConnect_Kernel<RayClusterSize>(lpState, MyConnectKernelInfo, RayClusterSize);
				ValidConnectionCount = 0;
			}
		}
		else{
			lpState->Out_Connection[i][0] = 0.0f; lpState->Out_Connection[i][1] = 0.0f;
			lpState->Out_ConnectIntensity[i].set(0.0f);
		}
	}
	if (ValidConnectionCount > 0)BDPT_TraceConnect_Kernel<RayClusterSize>(lpState, MyConnectKernelInfo, ValidConnectionCount);
}