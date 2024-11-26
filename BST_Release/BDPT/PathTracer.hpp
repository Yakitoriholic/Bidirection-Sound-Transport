#include "lGeneral.hpp"
#include "lMath_SIMD.hpp"
#include "lMath_Matrix.hpp"
#include "lMath_MonteCarlo.hpp"
#include "Listener.hpp"

class PathTracer{
private:
	// sound medium, temporary --BEGIN--

	float MaxResponseDelay;
	float SoundSpeed;
	FloatVec_SIMD CentralFreq, DecayCoeff;

	// sound medium, temporary --END--
	struct Status{
		UBINT ClusterCount_Source;
		UBINT ClusterCount_Listener;
		UBINT PathDepth;
		UBINT PathCount;
		UBINT ConnectionCount;
		bool PathStorageValid_Fwd;
		bool PathStorageValid_Bwd;
	} CurrentStat;

	nsMath::RandGenerator_XORShiftPlus RNG;

	//buffers for path tracing kernels
	struct {
		SndSrcInfo *lpSrcInfo;
		nsMath::TinyVector<UINT4b, 2> *lpSrcPathRange;
		UINT4b *lpSrcIndex;
		nsMath::TinyVector<float, 3> *lpInitSamples;

		PathNodeData_Inter *lpPath_Internal;

		PathNodeData *lpPath;
		UINT4b *lpPathBounces;
	} Buffer;

	ConnectionShuffler *lpShuffler;
	UINT4b *lpShufflerIndex;
	nsMath::TinyVector<float, 2> *Connection;
	nsMath::TinyVector<UINT4b, 4> *ConnectIndex;
	FloatVec_SIMD *ConnectIntensity;

	void On_ClusterCountChange(UBINT ClusterCount_Source_New, UBINT ClusterCount_Listener_New);
	void On_PathCountChange(UBINT PathCount_New);
	void On_ConnectionCountChange(UBINT ConnectionCount_New);

#if defined USE_DIFFUSE_CACHE
	struct PathCacheNode{
		UBINT CacheFrameSize;
		UBINT CachedSampleCount;
		UBINT CacheLifeTime;
		float WeightCurrent;
		PathNodeData *pPathNode;
		UBINT *pRefreshToken;
	};
	nsContainer::List<PathCacheNode> PathCache;

	void UpdateDiffuseCache(nsContainer::List<SoundCluster_Source> *lpSrcList, SoundCluster_Listener *lpListener);
	// return the number of the frames hit by the cache.
#endif

public:
	struct StatInfo{
		UINT8b FrameCounter;
		UINT8b SampleCounter_Total;
		UINT8b SampleCounter_Frame;
	} CurrentStatInfo;

	PathTracer(float MaxResponseDelay, UBINT PathDepth, UBINT PathCount = 0, UBINT ConnectionCount = 0);
	void PathAllocate(SoundClusterMgr *lpSoundMgr, UBINT PathCount, UBINT ConnectionCount);
	void LoadSoundClusters(SoundClusterMgr *lpSoundMgr);
	void Trace();
	void Connect();
	void Output(SoundClusterMgr *lpSoundMgr, bool WithDirectContribution = true);
	void PostProcess(SoundClusterMgr *lpSoundMgr);
	~PathTracer();
};

/*-------------------------------- IMPLEMENTATION --------------------------------*/

PathTracer::PathTracer(float MaxResponseDelay, UBINT PathDepth, UBINT PathCount, UBINT ConnectionCount){
	// sound medium, temporary --BEGIN--
	this->MaxResponseDelay = MaxResponseDelay;
	this->SoundSpeed = SoundSpeed_Air(20, 101325, 50);
	this->CentralFreq[0] = 62.5f * sqrt(8 / FloatVec_SIMD::Width);
	for (UBINT i = 1; i < FloatVec_SIMD::Width; i++)this->CentralFreq[i] = this->CentralFreq[i - 1] * 16.0f / (float)FloatVec_SIMD::Width;
	for (UBINT i = 0; i < FloatVec_SIMD::Width; i++)this->DecayCoeff[i] = SoundAttCoeff_Air(20, 101325, 50, this->CentralFreq[i]);
	// sound medium, temporary --END--

	CurrentStat.ClusterCount_Source = 0;
	CurrentStat.ClusterCount_Listener = 0;
	CurrentStat.PathDepth = PathDepth;
	CurrentStat.PathCount = 0;
	CurrentStat.ConnectionCount = 0;
	CurrentStat.PathStorageValid_Fwd = true;
	CurrentStat.PathStorageValid_Bwd = true;

	this->On_ClusterCountChange(0, 1);
	this->On_PathCountChange(PathCount);
	this->On_ConnectionCountChange(ConnectionCount);

	//clear the statistic info
	this->CurrentStatInfo.FrameCounter = 0;
	this->CurrentStatInfo.SampleCounter_Frame = 0;
	this->CurrentStatInfo.SampleCounter_Total = 0;
}

void PathTracer::On_ClusterCountChange(UBINT ClusterCount_Source_New, UBINT ClusterCount_Listener_New){
	if (ClusterCount_Source_New != this->CurrentStat.ClusterCount_Source || ClusterCount_Listener_New != this->CurrentStat.ClusterCount_Listener){
		this->CurrentStat.PathStorageValid_Fwd = false;
		this->CurrentStat.PathStorageValid_Bwd = false;
		try{
			if (0 != this->CurrentStat.ClusterCount_Source && 0 != this->CurrentStat.ClusterCount_Listener){

				nsBasic::MemFree_Arr<SndSrcInfo>(this->Buffer.lpSrcInfo, this->CurrentStat.ClusterCount_Source + this->CurrentStat.ClusterCount_Listener);
				nsBasic::MemFree_Arr<nsMath::TinyVector<UINT4b, 2>>(this->Buffer.lpSrcPathRange, this->CurrentStat.ClusterCount_Source + this->CurrentStat.ClusterCount_Listener);

				nsBasic::MemFree_Arr<ConnectionShuffler>(lpShuffler, this->CurrentStat.ClusterCount_Source * this->CurrentStat.ClusterCount_Listener * this->CurrentStat.PathDepth);
			}
			if (0 != ClusterCount_Source_New && 0 != ClusterCount_Listener_New){
				this->Buffer.lpSrcInfo = nsBasic::MemAlloc_Arr<SndSrcInfo>(ClusterCount_Source_New + ClusterCount_Listener_New);
				this->Buffer.lpSrcPathRange = nsBasic::MemAlloc_Arr<nsMath::TinyVector<UINT4b, 2>>(ClusterCount_Source_New + ClusterCount_Listener_New);

				lpShuffler = nsBasic::MemAlloc_Arr<ConnectionShuffler>(ClusterCount_Source_New * ClusterCount_Listener_New * this->CurrentStat.PathDepth);
			}
			else{
				this->Buffer.lpSrcInfo = nullptr;
				this->Buffer.lpSrcPathRange = nullptr;

				lpShuffler = nullptr;
			}
		}
		catch (...){
			throw;
		}
		this->CurrentStat.ClusterCount_Listener = ClusterCount_Listener_New;
		this->CurrentStat.ClusterCount_Source = ClusterCount_Source_New;
	}
}
void PathTracer::On_PathCountChange(UBINT PathCount_New){
	if (PathCount_New != this->CurrentStat.PathCount){
		this->CurrentStat.PathStorageValid_Fwd = false;
		this->CurrentStat.PathStorageValid_Bwd = false;
		try{
			if (0 != this->CurrentStat.PathCount && 0 != this->CurrentStat.PathDepth){
				nsBasic::MemFree_Arr<PathNodeData_Inter>(this->Buffer.lpPath_Internal, this->CurrentStat.PathDepth * this->CurrentStat.PathCount);
				nsBasic::MemFree_Arr<PathNodeData>(this->Buffer.lpPath, this->CurrentStat.PathDepth * this->CurrentStat.PathCount);

				nsBasic::MemFree_Arr<UINT4b>(this->Buffer.lpSrcIndex, this->CurrentStat.PathCount);
				nsBasic::MemFree_Arr<nsMath::TinyVector<float, 3>>(this->Buffer.lpInitSamples, this->CurrentStat.PathCount);
				nsBasic::MemFree_Arr<UINT4b>(this->Buffer.lpPathBounces, this->CurrentStat.PathCount);
			}
			if (0 != PathCount_New){
				this->Buffer.lpPath_Internal = nsBasic::MemAlloc_Arr<PathNodeData_Inter>(this->CurrentStat.PathDepth * PathCount_New);
				this->Buffer.lpPath = nsBasic::MemAlloc_Arr<PathNodeData>(this->CurrentStat.PathDepth * PathCount_New);

				this->Buffer.lpSrcIndex = nsBasic::MemAlloc_Arr<UINT4b>(PathCount_New);
				this->Buffer.lpInitSamples = nsBasic::MemAlloc_Arr<nsMath::TinyVector<float, 3>>(PathCount_New);
				this->Buffer.lpPathBounces = nsBasic::MemAlloc_Arr<UINT4b>(PathCount_New);
			}
			else{
				this->Buffer.lpPath_Internal = nullptr;
				this->Buffer.lpPath = nullptr;

				this->Buffer.lpSrcIndex = nullptr;
				this->Buffer.lpInitSamples = nullptr;
				this->Buffer.lpPathBounces = nullptr;
			}
		}
		catch (...){
			throw;
		}
	}
	this->CurrentStat.PathCount = PathCount_New;
}
void PathTracer::On_ConnectionCountChange(UBINT ConnectionCount_New){
	if (ConnectionCount_New != this->CurrentStat.ConnectionCount){
		try{
			if (0 != this->CurrentStat.ConnectionCount){
				nsBasic::MemFree_Arr<UINT4b>(lpShufflerIndex, this->CurrentStat.ConnectionCount);
				nsBasic::MemFree_Arr<nsMath::TinyVector<float, 2>>(Connection, this->CurrentStat.ConnectionCount);
				nsBasic::MemFree_Arr<nsMath::TinyVector<UINT4b, 4>>(ConnectIndex, this->CurrentStat.ConnectionCount);
				nsBasic::MemFree_Arr_SIMD<FloatVec_SIMD>(ConnectIntensity, this->CurrentStat.ConnectionCount);
			}

			if (ConnectionCount_New > 0){
				lpShufflerIndex = nsBasic::MemAlloc_Arr<UINT4b>(ConnectionCount_New);
				Connection = nsBasic::MemAlloc_Arr<nsMath::TinyVector<float, 2>>(ConnectionCount_New);
				ConnectIndex = nsBasic::MemAlloc_Arr<nsMath::TinyVector<UINT4b, 4>>(ConnectionCount_New);
				ConnectIntensity = nsBasic::MemAlloc_Arr_SIMD<FloatVec_SIMD>(ConnectionCount_New);
			}
			else{
				lpShufflerIndex = nullptr;
				Connection = nullptr;
				ConnectIndex = nullptr;
				ConnectIntensity = nullptr;
			}
		}
		catch (...){
			throw;
		}
		this->CurrentStat.ConnectionCount = ConnectionCount_New;
	}
}
void PathTracer::PathAllocate(SoundClusterMgr *lpSoundMgr, UBINT PathCount, UBINT ConnectionCount){
	// auto WeightFormula = [](float DistSqr){if (DistSqr < 1.69f)DistSqr = 1.69f; return 167.2291f + 433.7596f / sqrt(DistSqr) - 357.0583f / DistSqr; };
	auto WeightFormula = [](float DistSqr){return 1; };
	UBINT SndClusterCnt_Source = lpSoundMgr->SoundClusterList_Source.size();
	UBINT SndClusterCnt_Listener = lpSoundMgr->SoundClusterList_Listener.size();

	if (SndClusterCnt_Source > 0 && SndClusterCnt_Listener > 0){
		float *Weight = nsBasic::MemAlloc_Arr<float>(SndClusterCnt_Source * SndClusterCnt_Listener);

		for (auto _It_Listener = lpSoundMgr->SoundClusterList_Listener.begin(); _It_Listener != lpSoundMgr->SoundClusterList_Listener.end(); ++_It_Listener){
			float ListenerPos[3];
			ListenerPos[0] = _It_Listener->TransformMatrix[12] / _It_Listener->TransformMatrix[15];
			ListenerPos[1] = _It_Listener->TransformMatrix[13] / _It_Listener->TransformMatrix[15];
			ListenerPos[2] = _It_Listener->TransformMatrix[14] / _It_Listener->TransformMatrix[15];
			nsMath::mul<3>(ListenerPos, ListenerPos, 1.0f / _It_Listener->TransformMatrix[15]);

			for (auto _It_Source = lpSoundMgr->SoundClusterList_Source.begin(); _It_Source != lpSoundMgr->SoundClusterList_Source.end(); ++_It_Source){
				float SrcPos[3];
				SrcPos[0] = _It_Source->TransformMatrix[12] / _It_Source->TransformMatrix[15];
				SrcPos[1] = _It_Source->TransformMatrix[13] / _It_Source->TransformMatrix[15];
				SrcPos[2] = _It_Source->TransformMatrix[14] / _It_Source->TransformMatrix[15];
				float ListenerVec[3], DistSqr;
				nsMath::sub<3>(ListenerVec, SrcPos, ListenerPos);
				DistSqr = nsMath::dot<3>(ListenerVec, ListenerVec);
				*Weight = WeightFormula(DistSqr);
				Weight++;
			}
		}
		Weight -= SndClusterCnt_Source * SndClusterCnt_Listener;

		float WeightSum = 0.0f;
		for (UBINT i = 0; i < SndClusterCnt_Source * SndClusterCnt_Listener; i++)WeightSum += Weight[i];

		// path allocation --BEGIN--
		float *Weight_Source = nsBasic::MemAlloc_Arr<float>(SndClusterCnt_Source);
		float *Weight_Listener = nsBasic::MemAlloc_Arr<float>(SndClusterCnt_Listener);

		for (UBINT i = 0; i < SndClusterCnt_Source; i++)Weight_Source[i] = 0;
		for (UBINT i = 0; i < SndClusterCnt_Listener; i++)Weight_Listener[i] = 0;
		for (UBINT i = 0; i < SndClusterCnt_Listener; i++){
			for (UBINT j = 0; j < SndClusterCnt_Source; j++){
				Weight_Source[j] += Weight[i * SndClusterCnt_Source + j];
				Weight_Listener[i] += Weight[i * SndClusterCnt_Source + j];
			}
		}

		UBINT PathCount_Fwd = PathCount * SndClusterCnt_Source / (SndClusterCnt_Source + SndClusterCnt_Listener);
		UBINT PathCount_Bwd = PathCount - PathCount_Fwd;
		UBINT PathRemain = PathCount_Fwd;
		if (PathRemain < SndClusterCnt_Source){
			for (auto _It_Source = lpSoundMgr->SoundClusterList_Source.begin(); _It_Source != lpSoundMgr->SoundClusterList_Source.end(); ++_It_Source){
				UBINT PathCount_New;
				if (PathRemain > 0){
					PathCount_New = 1;
					PathRemain--;
				}
				else PathCount_New = 0;
				if (PathCount_New != _It_Source->PathCount)this->CurrentStat.PathStorageValid_Fwd = false;
				_It_Source->PathCount = PathCount_New;
			}
		}
		else{
			PathCount_Fwd -= SndClusterCnt_Source; //reserve one ray for each cluster
			auto _It_Source = lpSoundMgr->SoundClusterList_Source.begin();
			for (UBINT i = 0; i < SndClusterCnt_Source; i++){
				UBINT PathCount_New;
				if (i + 1 == SndClusterCnt_Source)PathCount_New = PathRemain;
				else PathCount_New = 1 + (UBINT)floor((*Weight_Source) * (float)PathCount_Fwd / WeightSum);
				if (PathCount_New != _It_Source->PathCount)this->CurrentStat.PathStorageValid_Fwd = false;
				_It_Source->PathCount = PathCount_New;
				PathRemain -= _It_Source->PathCount;
				Weight_Source++;
				++_It_Source;
			}
			Weight_Source -= SndClusterCnt_Source;
		}

		PathRemain = PathCount_Bwd;
		if (PathRemain < SndClusterCnt_Listener){
			for (auto _It_Listener = lpSoundMgr->SoundClusterList_Listener.begin(); _It_Listener != lpSoundMgr->SoundClusterList_Listener.end(); ++_It_Listener){
				UBINT PathCount_New;
				if (PathRemain > 0){
					PathCount_New = 1;
					PathRemain--;
				}
				else PathCount_New = 0;
				if (PathCount_New != _It_Listener->PathCount)this->CurrentStat.PathStorageValid_Bwd = false;
				_It_Listener->PathCount = PathCount_New;
			}
		}
		else{
			PathCount_Bwd -= SndClusterCnt_Listener; //reserve one ray for each cluster
			auto _It_Listener = lpSoundMgr->SoundClusterList_Listener.begin();
			for (UBINT i = 0; i < SndClusterCnt_Listener; i++){
				UBINT PathCount_New;
				if (i + 1 == SndClusterCnt_Listener)PathCount_New = PathRemain;
				else PathCount_New = 1 + (UBINT)floor((*Weight_Listener) * (float)PathCount_Bwd / WeightSum);
				if (PathCount_New != _It_Listener->PathCount)this->CurrentStat.PathStorageValid_Bwd = false;
				_It_Listener->PathCount = PathCount_New;
				PathRemain -= _It_Listener->PathCount;
				Weight_Listener++;
				++_It_Listener;
			}
			Weight_Listener -= SndClusterCnt_Listener;
		}

		nsBasic::MemFree_Arr<float>(Weight_Listener, SndClusterCnt_Listener);
		nsBasic::MemFree_Arr<float>(Weight_Source, SndClusterCnt_Source);
		//path allocation --END--

		//connection allocation --BEGIN--
		//[ClusterCount_Source] * [ClusterCount_Listener] connections are remained for the direcct contribution.
		UBINT IndirectConnectionCount = ConnectionCount - SndClusterCnt_Source * SndClusterCnt_Listener;
		UBINT ConnectionRemain = IndirectConnectionCount;
		UBINT RelationRemain = SndClusterCnt_Source * SndClusterCnt_Listener;
		float *SampleProbability_Actual = nsBasic::MemAlloc_Arr<float>(this->CurrentStat.PathDepth - 1);

		for (auto _It_Listener = lpSoundMgr->SoundClusterList_Listener.begin(); _It_Listener != lpSoundMgr->SoundClusterList_Listener.end(); ++_It_Listener){
			SoundClusterRelation *CurRelation = _It_Listener->lpRelation_Row_Next;
			for (auto _It_Source = lpSoundMgr->SoundClusterList_Source.begin(); _It_Source != lpSoundMgr->SoundClusterList_Source.end(); ++_It_Source){
				UBINT Connection_CurRelation;
				CurRelation->SetPathSpace(this->CurrentStat.PathDepth);

				if (1 == RelationRemain)Connection_CurRelation = ConnectionRemain;
				else Connection_CurRelation = *Weight * (float)(IndirectConnectionCount) / WeightSum;
				if (0 == _It_Source->PathCount || 0 == _It_Listener->PathCount)Connection_CurRelation = 0; //in case that the weight is not zero while no valid path exists

				ConnectionRemain -= Connection_CurRelation;
				//first we'll try to give every path space two samples.
				for (UBINT i = 0; i < this->CurrentStat.PathDepth - 1; i++){
					if (Connection_CurRelation > 2){
						CurRelation->SampleCntPerBounce[i] = 2;
						Connection_CurRelation -= 2;
					}
					else{
						CurRelation->SampleCntPerBounce[i] = Connection_CurRelation;
						Connection_CurRelation = 0;
					}
				}

				if (Connection_CurRelation > 0){
					//clamp the probability --BEGIN--
					for (UBINT i = 0; i < this->CurrentStat.PathDepth - 1; i++){
						SampleProbability_Actual[i] = i * _It_Source->PathCount * _It_Listener->PathCount;

						if (i < this->CurrentStat.PathDepth - 1)SampleProbability_Actual[i] += (float)_It_Source->PathCount + (float)_It_Listener->PathCount;

						SampleProbability_Actual[i] -= 2.0f;
#if defined ADAPTIVE_SAMPLING
						SampleProbability_Actual[i] /= (float)Connection_CurRelation;
#endif
					}
					float TotalProbability = 1.0;
					float ProbabilitySum = 0.0;
					for (UBINT i = 0; i < this->CurrentStat.PathDepth - 1; i++){
#if defined ADAPTIVE_SAMPLING
						if (TotalProbability * CurRelation->SampleProbability[i] > SampleProbability_Actual[i]){
							float TmpFloat = TotalProbability * CurRelation->SampleProbability[i] - SampleProbability_Actual[i];
							if (1.0f == ProbabilitySum + CurRelation->SampleProbability[i])TotalProbability = 0.0f;
							else TotalProbability += TmpFloat / (1.0f - ProbabilitySum - CurRelation->SampleProbability[i]);
						}
						else SampleProbability_Actual[i] = TotalProbability * CurRelation->SampleProbability[i];
#endif
						ProbabilitySum += SampleProbability_Actual[i];
					}
#if !defined ADAPTIVE_SAMPLING
					for (UBINT i = 0; i < this->CurrentStat.PathDepth - 1; i++)SampleProbability_Actual[i] /= ProbabilitySum;
#endif
					//clamp the probability --END--

					CurRelation->SampleCntPerBounce[this->CurrentStat.PathDepth - 2] += Connection_CurRelation;
					for (UBINT i = 0; i < this->CurrentStat.PathDepth - 2; i++){
						CurRelation->SampleCntPerBounce[i] += (UBINT)(Connection_CurRelation * SampleProbability_Actual[i]);
						CurRelation->SampleCntPerBounce[this->CurrentStat.PathDepth - 2] -= (UBINT)(Connection_CurRelation * SampleProbability_Actual[i]);
					}
				}

				CurRelation = CurRelation->Row_Next;
				Weight++;
				RelationRemain--;
			}
		}
		Weight -= SndClusterCnt_Source * SndClusterCnt_Listener;
		nsBasic::MemFree_Arr<float>(SampleProbability_Actual, this->CurrentStat.PathDepth - 1);
		//connection allocation --END--

		nsBasic::MemFree_Arr<float>(Weight, SndClusterCnt_Source * SndClusterCnt_Listener);
	}
}
void PathTracer::LoadSoundClusters(SoundClusterMgr *lpSoundMgr){
	//statistics --BEGIN--
	UBINT SndClusterCnt_Source = 0;
	UBINT SndClusterCnt_Listener = 0;
	
	UBINT NewConnectionCount = 0;
	
	UBINT NewPathCount = 0;
	for (auto _It = lpSoundMgr->SoundClusterList_Source.begin(); _It != lpSoundMgr->SoundClusterList_Source.end(); ++_It){
		SndClusterCnt_Source++;
		NewPathCount += _It->PathCount;
		SoundClusterRelation *TmpRelationPtr = _It->lpRelation_Col_Next;
		NewConnectionCount++; // direct contribution
		for (SoundClusterRelation *TmpRelationPtr = _It->lpRelation_Col_Next; nullptr != TmpRelationPtr; TmpRelationPtr = TmpRelationPtr->Col_Next){
			for (UBINT i = 0; i < this->CurrentStat.PathDepth - 1; i++)NewConnectionCount += TmpRelationPtr->SampleCntPerBounce[i];
		}
	}
	for (auto _It = lpSoundMgr->SoundClusterList_Listener.begin(); _It != lpSoundMgr->SoundClusterList_Listener.end(); ++_It){
		SndClusterCnt_Listener++;
		NewPathCount += _It->PathCount;
	}

	this->On_ClusterCountChange(SndClusterCnt_Source, SndClusterCnt_Listener);
	this->On_PathCountChange(NewPathCount);
	this->On_ConnectionCountChange(NewConnectionCount);
	//statistics --END--

	if (0 != SndClusterCnt_Source && 0 != SndClusterCnt_Listener){
		// first step - generate path index

		UINT4b CurSrcIndex = 0, CurPathIndex = 0;
		for (auto _It = lpSoundMgr->SoundClusterList_Source.begin(); _It != lpSoundMgr->SoundClusterList_Source.end(); ++_It){
			if (_It->Change_Exist || !this->CurrentStat.PathStorageValid_Fwd){
				this->Buffer.lpSrcInfo[CurSrcIndex].Position[0] = _It->TransformMatrix[12] / _It->TransformMatrix[15];
				this->Buffer.lpSrcInfo[CurSrcIndex].Position[1] = _It->TransformMatrix[13] / _It->TransformMatrix[15];
				this->Buffer.lpSrcInfo[CurSrcIndex].Position[2] = _It->TransformMatrix[14] / _It->TransformMatrix[15];
				this->Buffer.lpSrcInfo[CurSrcIndex].Intensity = _It->Intensity;
				this->Buffer.lpSrcInfo[CurSrcIndex].PathValid = false;

				this->Buffer.lpSrcPathRange[CurSrcIndex][0] = CurPathIndex;
				this->Buffer.lpSrcPathRange[CurSrcIndex][1] = _It->PathCount;
#if defined USE_SAMPLE_PATTERN
				// Hammersley sampling
				float TransformMatrix[9];
				nsMath::Distribution_Uniform_Spherical(TransformMatrix, &this->RNG);
				nsMath::Random_Vertical_Vector(&TransformMatrix[3], &TransformMatrix[0], &this->RNG);
				nsMath::cross3(&TransformMatrix[6], &TransformMatrix[3], &TransformMatrix[0]);
				for (UBINT i = 0; i < _It->PathCount; i++){
					float Sequence[3];
					float phi = 2.0f * nsMath::VanDerCorputSequence2(i) - 1.0f;
					float theta = 2.0f * (float)i / (float)_It->PathCount;
					Sequence[0] = phi;
					Sequence[1] = sqrt(1 - phi * phi) * cos(theta);
					Sequence[2] = sqrt(1 - phi * phi) * sin(theta);
					nsMath::mul3_mv(TransformMatrix, Sequence, this->Buffer.lpInitSamples[CurPathIndex + i].Data);
				}
#endif
				for (UBINT i = 0; i < _It->PathCount; i++)this->Buffer.lpSrcIndex[CurPathIndex + i] = CurSrcIndex;
				_It->Change_Exist = false;
			}
			else this->Buffer.lpSrcInfo[CurSrcIndex].PathValid = true;
			CurPathIndex += _It->PathCount;
			CurSrcIndex++;
		}
		UINT4b FirstListenerIndex = CurSrcIndex;
		for (auto _It = lpSoundMgr->SoundClusterList_Listener.begin(); _It != lpSoundMgr->SoundClusterList_Listener.end(); ++_It){
			if (_It->Change_Exist || !this->CurrentStat.PathStorageValid_Bwd){
				this->Buffer.lpSrcInfo[CurSrcIndex].Position[0] = _It->TransformMatrix[12] / _It->TransformMatrix[15];
				this->Buffer.lpSrcInfo[CurSrcIndex].Position[1] = _It->TransformMatrix[13] / _It->TransformMatrix[15];
				this->Buffer.lpSrcInfo[CurSrcIndex].Position[2] = _It->TransformMatrix[14] / _It->TransformMatrix[15];
				this->Buffer.lpSrcInfo[CurSrcIndex].Intensity = _It->Volume;
				this->Buffer.lpSrcInfo[CurSrcIndex].PathValid = false;

				this->Buffer.lpSrcPathRange[CurSrcIndex][0] = CurPathIndex;
				this->Buffer.lpSrcPathRange[CurSrcIndex][1] = _It->PathCount;
#if defined USE_SAMPLE_PATTERN
				// Hammersley sampling
				float TransformMatrix[9];
				nsMath::Distribution_Uniform_Spherical(TransformMatrix, &this->RNG);
				nsMath::Random_Vertical_Vector(&TransformMatrix[3], &TransformMatrix[0], &this->RNG);
				nsMath::cross3(&TransformMatrix[6], &TransformMatrix[3], &TransformMatrix[0]);
				for (UBINT i = 0; i < _It->PathCount; i++){
					float Sequence[3];
					float phi = 2.0f * nsMath::VanDerCorputSequence2(i) - 1.0f;
					float theta = 2.0f * (float)i / (float)_It->PathCount;
					Sequence[0] = phi;
					Sequence[1] = sqrt(1 - phi * phi) * cos(theta);
					Sequence[2] = sqrt(1 - phi * phi) * sin(theta);
					nsMath::mul3_mv(TransformMatrix, Sequence, this->Buffer.lpInitSamples[CurPathIndex + i].Data);
				}
#endif
				for (UBINT i = 0; i < _It->PathCount; i++)this->Buffer.lpSrcIndex[CurPathIndex + i] = CurSrcIndex;
				_It->Change_Exist = false;
			}
			else this->Buffer.lpSrcInfo[CurSrcIndex].PathValid = true;
			CurPathIndex += _It->PathCount;
			CurSrcIndex++;
		}
		this->CurrentStat.PathStorageValid_Fwd = true;
		this->CurrentStat.PathStorageValid_Bwd = true;

		//Initialize the connection index and the shuffler --BEGIN--
		ConnectionShuffler *lpTmpShuffler = this->lpShuffler;
		UINT4b *ShufflerIndexPtr = this->lpShufflerIndex, CurShufflerIndex = 0, CurSrcIndex_Source = 0;
		for (auto _It_Source = lpSoundMgr->SoundClusterList_Source.begin(); _It_Source != lpSoundMgr->SoundClusterList_Source.end(); ++_It_Source){
			SoundClusterRelation *TmpRelationPtr = _It_Source->lpRelation_Col_Next;
			UINT4b CurSrcIndex_Listener = FirstListenerIndex;
			for (auto _It_Listener = lpSoundMgr->SoundClusterList_Listener.begin(); _It_Listener != lpSoundMgr->SoundClusterList_Listener.end(); ++_It_Listener){
				//direct response --BEGIN--

				static const ConnectionShuffler DirectResponseShuffler = {
					0, 0,
					0, 0, // PathSpace
					{ 1, 0, 0 }, // SampleCount
					{ 2, 0, 0 }, // ShuffleCycle
					{ 0, 0, 0 }, // k0
					{ 0, 0, 0 }  // k1
				};

				*lpTmpShuffler = DirectResponseShuffler;
				lpTmpShuffler->Source = CurSrcIndex_Source;
				lpTmpShuffler->Listener = CurSrcIndex_Listener;
				lpTmpShuffler++;

				*ShufflerIndexPtr = CurShufflerIndex;
				ShufflerIndexPtr++;
				CurShufflerIndex++;
				//direct response --END--

				// pay attention to the special boundary case
				for (UBINT i = 1; i < this->CurrentStat.PathDepth; i++){
					UBINT StrategyCount = 2;

					lpTmpShuffler->Source = CurSrcIndex_Source;
					lpTmpShuffler->Listener = CurSrcIndex_Listener;

					lpTmpShuffler->ShuffleCycle[0] = _It_Listener->PathCount;
					lpTmpShuffler->ShuffleCycle[1] = _It_Source->PathCount;

					lpTmpShuffler->MidStrategyCount = i - 1;
					lpTmpShuffler->ShuffleCycle[2] = lpTmpShuffler->MidStrategyCount * _It_Source->PathCount * _It_Listener->PathCount;
					StrategyCount += lpTmpShuffler->MidStrategyCount;

					UINT4b SampleRemain = TmpRelationPtr->SampleCntPerBounce[i - 1];
					if (lpTmpShuffler->ShuffleCycle[0] < lpTmpShuffler->ShuffleCycle[1]){
						if (lpTmpShuffler->ShuffleCycle[0] * StrategyCount <= SampleRemain)lpTmpShuffler->SampleCount[0] = lpTmpShuffler->ShuffleCycle[0]; else lpTmpShuffler->SampleCount[0] = SampleRemain / StrategyCount;
						SampleRemain -= lpTmpShuffler->SampleCount[0];
						StrategyCount--;
						if (lpTmpShuffler->ShuffleCycle[1] * StrategyCount <= SampleRemain)lpTmpShuffler->SampleCount[1] = lpTmpShuffler->ShuffleCycle[1]; else lpTmpShuffler->SampleCount[1] = SampleRemain / StrategyCount;
						SampleRemain -= lpTmpShuffler->SampleCount[1];
					}
					else{
						if (lpTmpShuffler->ShuffleCycle[1] * StrategyCount <= SampleRemain)lpTmpShuffler->SampleCount[1] = lpTmpShuffler->ShuffleCycle[1]; else lpTmpShuffler->SampleCount[1] = SampleRemain / StrategyCount;
						SampleRemain -= lpTmpShuffler->SampleCount[1];
						StrategyCount--;
						if (lpTmpShuffler->ShuffleCycle[0] * StrategyCount <= SampleRemain)lpTmpShuffler->SampleCount[0] = lpTmpShuffler->ShuffleCycle[0]; else lpTmpShuffler->SampleCount[0] = SampleRemain / StrategyCount;
						SampleRemain -= lpTmpShuffler->SampleCount[0];
					}
					lpTmpShuffler->SampleCount[2] = SampleRemain;

					for (UBINT j = 0; j < 3; j++){
						// permutation polynomial: k1 * x + k0

						if (lpTmpShuffler->ShuffleCycle[j] < 2)lpTmpShuffler->ShuffleCycle[j] = 2;
						lpTmpShuffler->k0[j] = (UINT4b)this->RNG() % lpTmpShuffler->ShuffleCycle[j];
						do{
							lpTmpShuffler->k1[j] = (UINT4b)this->RNG() % (lpTmpShuffler->ShuffleCycle[j] - 1) + 1;
						} while (1 != (UINT4b)nsMath::gcd(lpTmpShuffler->k1[j], lpTmpShuffler->ShuffleCycle[j]));
					}

					lpTmpShuffler->PathSpace = i;
					lpTmpShuffler++;

					for (UBINT j = 0; j < TmpRelationPtr->SampleCntPerBounce[i - 1]; j++)ShufflerIndexPtr[j] = CurShufflerIndex;
					ShufflerIndexPtr += TmpRelationPtr->SampleCntPerBounce[i - 1];
					CurShufflerIndex++;
				}

				CurSrcIndex_Listener++;
			}
			TmpRelationPtr = TmpRelationPtr->Col_Next;
			CurSrcIndex_Source++;
		}
		//Initialize the shuffler --END--
	}
}
void PathTracer::Trace(){
	try{
		rtcCommit(Embree_Scene_Solid); EMBREE_CHECK_ERROR;
		rtcCommit(Embree_Scene_Dynamic); EMBREE_CHECK_ERROR;

		EmbreeKernelState MyKernelState;

		MyKernelState.TraceState.In_SrcInfo = this->Buffer.lpSrcInfo;
		MyKernelState.TraceState.In_SrcPathRange = this->Buffer.lpSrcPathRange;
		MyKernelState.TraceState.In_SrcIndex = this->Buffer.lpSrcIndex;
		MyKernelState.TraceState.In_InitSamples = this->Buffer.lpInitSamples;

		MyKernelState.TraceState.Inter_PathNodeData = this->Buffer.lpPath_Internal;

		MyKernelState.TraceState.Out_PathNodeData = this->Buffer.lpPath;
		MyKernelState.TraceState.Out_Bounces = this->Buffer.lpPathBounces;

		MyKernelState.TraceState.PathDepth = this->CurrentStat.PathDepth;

		MyKernelState.In_ShufflerIndex = lpShufflerIndex;
		MyKernelState.In_Shuffler = lpShuffler;

		MyKernelState.Out_Connection = Connection;
		MyKernelState.Out_ConnectIndex = ConnectIndex;
		MyKernelState.Out_ConnectIntensity = ConnectIntensity;

		MyKernelState.SharedState.PathLenUpperBound = MaxResponseDelay * SoundSpeed;
		MyKernelState.SharedState.lpRandGenerator = &this->RNG;

		BDPT_TraceFB(&MyKernelState, this->CurrentStat.PathCount);
	}
	catch (...){
		throw;
	}
}
void PathTracer::Connect(){
	try{
		EmbreeKernelState MyKernelState;

		MyKernelState.TraceState.In_SrcInfo = this->Buffer.lpSrcInfo;
		MyKernelState.TraceState.In_SrcPathRange = this->Buffer.lpSrcPathRange;
		MyKernelState.TraceState.In_SrcIndex = this->Buffer.lpSrcIndex;
		MyKernelState.TraceState.In_InitSamples = this->Buffer.lpInitSamples;

		MyKernelState.TraceState.Inter_PathNodeData = this->Buffer.lpPath_Internal;

		MyKernelState.TraceState.Out_PathNodeData = this->Buffer.lpPath;
		MyKernelState.TraceState.Out_Bounces = this->Buffer.lpPathBounces;

		MyKernelState.TraceState.PathDepth = this->CurrentStat.PathDepth;

		MyKernelState.In_ShufflerIndex = lpShufflerIndex;
		MyKernelState.In_Shuffler = lpShuffler;

		MyKernelState.Out_Connection = Connection;
		MyKernelState.Out_ConnectIndex = ConnectIndex;
		MyKernelState.Out_ConnectIntensity = ConnectIntensity;

		MyKernelState.SharedState.PathLenUpperBound = MaxResponseDelay * SoundSpeed;
		MyKernelState.SharedState.lpRandGenerator = &this->RNG;

		BDPT_TraceConnect(&MyKernelState, this->CurrentStat.ConnectionCount);
	}
	catch (...){
		throw;
	}
}
#if defined USE_DIFFUSE_CACHE
void PathTracer::UpdateDiffuseCache(nsContainer::List<SoundCluster_Source> *lpSrcList, SoundCluster_Listener *lpListener){
	float ListenerPos[3];
	ListenerPos[0] = lpListener->TransformMatrix[12] / lpListener->TransformMatrix[15];
	ListenerPos[1] = lpListener->TransformMatrix[13] / lpListener->TransformMatrix[15];
	ListenerPos[2] = lpListener->TransformMatrix[14] / lpListener->TransformMatrix[15];
	nsMath::mul<3>(ListenerPos, ListenerPos, 1.0f / lpListener->TransformMatrix[15]);

	UBINT Cluster_Index = 0, RetValue = 0;
	SoundClusterRelation *CurRelation = lpListener->lpRelation_Row_Next;
	for (auto _It = lpSrcList->begin(); _It != lpSrcList->end(); ++_It){
		float SrcPos[3];
		SrcPos[0] = _It->TransformMatrix[12] / _It->TransformMatrix[15];
		SrcPos[1] = _It->TransformMatrix[13] / _It->TransformMatrix[15];
		SrcPos[2] = _It->TransformMatrix[14] / _It->TransformMatrix[15];

		lpListener->lpListener->SetCurTrack(lpListener->lpListener->LoadTrack(Cluster_Index));
		UBINT FrameCount = CurRelation->DiffCache.GetFrameCount();
		
		auto _It_DiffCache = CurRelation->DiffCache.GetReadIterator();
		while (FrameCount > PathCache.size()){
			if ((UINT4b)-1 == _It_DiffCache->NodeIndex_Fwd && (UINT4b) - 1 == _It_DiffCache->NodeIndex_Bwd){
				//next frame
				CurRelation->DiffCache.Advance(_It_DiffCache);
				CurRelation->DiffCache.DeletePrevFrame(_It_DiffCache);
				FrameCount--;
				continue;
			}
			CurRelation->DiffCache.Advance(_It_DiffCache);
		}

		auto _It_PathCache = PathCache.begin();
		float CacheWeight = _It_PathCache->WeightCurrent;
		for (; CurRelation->DiffCache.end() != _It_DiffCache; CurRelation->DiffCache.Advance(_It_DiffCache)){
			if ((UINT4b)-1 == _It_DiffCache->NodeIndex_Fwd){
				//special case
				if ((UINT4b)-1 == _It_DiffCache->NodeIndex_Bwd){
					//next frame
					FrameCount--;
					_It_PathCache++;
					CacheWeight = _It_PathCache->WeightCurrent;
				}
			}
			else{
				float PosVector_Listener[3];

				// refresh forward side
				float Intensity_Fwd, Length_Fwd;
				if (FrameCount != _It_PathCache->pRefreshToken[_It_DiffCache->NodeIndex_Fwd]){
					if (0 == _It_DiffCache->NodeIndex_Fwd % this->CurrentStat.PathDepth)_It_PathCache->pRefreshToken[_It_DiffCache->NodeIndex_Fwd] = FrameCount;

					float PosVector_Source[3];
					nsMath::sub<3>(PosVector_Source, SrcPos, _It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Fwd].Position.Data);
					float LengthSqr_Fwd = nsMath::dot<3>(PosVector_Source, PosVector_Source);
					Length_Fwd = sqrt(LengthSqr_Fwd);
					_It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Fwd].Length = Length_Fwd;
					if (LengthSqr_Fwd < 0.01f)LengthSqr_Fwd = 0.01f;
					Intensity_Fwd = nsMath::dot<3>(PosVector_Source, _It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Fwd].Normal.Data) / (LengthSqr_Fwd * sqrt(LengthSqr_Fwd));
					_It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Fwd].Intensity[0] = Intensity_Fwd;
				}
				else{
					Intensity_Fwd = _It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Fwd].Intensity[0];
					Length_Fwd = _It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Fwd].Length;
				}

				nsMath::sub<3>(PosVector_Listener, ListenerPos, _It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Bwd].Position.Data);

				// refresh backward side
				float Intensity_Bwd, Length_Bwd;
				if (FrameCount != _It_PathCache->pRefreshToken[_It_DiffCache->NodeIndex_Bwd]){
					if (0 == _It_DiffCache->NodeIndex_Bwd % this->CurrentStat.PathDepth)_It_PathCache->pRefreshToken[_It_DiffCache->NodeIndex_Bwd] = FrameCount;

					float LengthSqr_Bwd = nsMath::dot<3>(PosVector_Listener, PosVector_Listener);
					Length_Bwd = sqrt(LengthSqr_Bwd);
					_It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Bwd].Length = Length_Bwd;
					if (LengthSqr_Bwd < 0.01f)LengthSqr_Bwd = 0.01f;
					Intensity_Bwd = nsMath::dot<3>(PosVector_Listener, _It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Bwd].Normal.Data) / (LengthSqr_Bwd * sqrt(LengthSqr_Bwd));
					_It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Bwd].Intensity[0] = Intensity_Bwd;
				}
				else{
					Intensity_Bwd = _It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Bwd].Intensity[0];
					Length_Bwd = _It_PathCache->pPathNode[_It_DiffCache->NodeIndex_Bwd].Length;
				}

				lpListener->lpListener->LoadHRTF(PosVector_Listener, &CamTransform.Matrix[0], &CamTransform.Matrix[4], &CamTransform.Matrix[8]);

				if (Intensity_Fwd > 0.0f && Intensity_Bwd > 0.0f){
					FloatVec_SIMD FinalIntensity;
					FinalIntensity.loadu(_It_DiffCache->Intensity);
					FinalIntensity.mul(CacheWeight);

					float Intensity_Path = Intensity_Fwd * Intensity_Bwd;
					float Delay_Path = Length_Fwd + Length_Bwd + _It_DiffCache->Delay_SubPath;
					float SampleDelay = Delay_Path / SoundSpeed;

					if (SampleDelay < MaxResponseDelay){
						lpListener->lpListener->LoadDopplerShift(1 == FrameCount ? 0 : _It_DiffCache->Delay_Prev - SampleDelay);
						// sound medium, temporary --BEGIN--
						FloatVec_SIMD Attenuation = this->DecayCoeff;
						Attenuation.mul(Delay_Path);
						Attenuation.exp();
						FinalIntensity.mul(Attenuation);
						// sound medium, temporary --END--
						FinalIntensity.mul(Intensity_Path);
						lpListener->lpListener->EvaluateImpulse(SampleDelay, FinalIntensity);
					}
					_It_DiffCache->Delay_Prev = SampleDelay;
				}
			}
		}
		RetValue = std::max(RetValue, CurRelation->DiffCache.GetFrameCount());
		CurRelation = CurRelation->Row_Next;
		Cluster_Index++;
	}
}
#endif

void PathTracer::Output(SoundClusterMgr *lpSoundMgr, bool WithDirectContribution){
	float *IntensityBin = nsBasic::MemAlloc_Arr<float>(IRBinCount * (this->CurrentStat.PathDepth - 1));
	float *IntensitySqrBin = nsBasic::MemAlloc_Arr<float>(IRBinCount * (this->CurrentStat.PathDepth - 1));
	if (nullptr == IntensityBin || nullptr == IntensitySqrBin)throw std::bad_alloc();

	this->CurrentStatInfo.SampleCounter_Frame = 0;
	UINT8b SampleCounter_Indirect = 0;
	for (auto _It = lpSoundMgr->SoundClusterList_Listener.begin(); _It != lpSoundMgr->SoundClusterList_Listener.end(); ++_It){
		_It->lpListener->Volume = _It->Volume;
		_It->lpListener->BeginWrite();
	}

	nsContainer::List<SoundCluster_Source>::iterator _It_Source = lpSoundMgr->SoundClusterList_Source.end();
	nsContainer::List<SoundCluster_Listener>::iterator _It_Listener;
	SoundClusterRelation *CurRelation;
	float SrcPos[3], ListenerPos[3];
	UBINT Cluster_Index = 0, Connection_Counter = 0;

	UBINT Connection_Index = 0;
	for (auto _It_Source = lpSoundMgr->SoundClusterList_Source.begin(); _It_Source != lpSoundMgr->SoundClusterList_Source.end(); _It_Source++){
		SrcPos[0] = _It_Source->TransformMatrix[12] / _It_Source->TransformMatrix[15];
		SrcPos[1] = _It_Source->TransformMatrix[13] / _It_Source->TransformMatrix[15];
		SrcPos[2] = _It_Source->TransformMatrix[14] / _It_Source->TransformMatrix[15];
		CurRelation = _It_Source->lpRelation_Col_Next;
		for (auto _It_Listener = lpSoundMgr->SoundClusterList_Listener.begin(); _It_Listener != lpSoundMgr->SoundClusterList_Listener.end(); _It_Listener++){

			// PREPROCESS --BEGIN--

			//refresh connection counter
			UBINT Connection_Counter = 1; // direct contribution
			for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++)Connection_Counter += CurRelation->SampleCntPerBounce[j];

			//clear the intensity accumulator
			nsMath::setzero_simd_unaligned(IntensityBin, IRBinCount * (this->CurrentStat.PathDepth - 1));
			nsMath::setzero_simd_unaligned(IntensitySqrBin, IRBinCount * (this->CurrentStat.PathDepth - 1));

			//refresh direct contribution
			ListenerPos[0] = _It_Listener->TransformMatrix[12] / _It_Listener->TransformMatrix[15];
			ListenerPos[1] = _It_Listener->TransformMatrix[13] / _It_Listener->TransformMatrix[15];
			ListenerPos[2] = _It_Listener->TransformMatrix[14] / _It_Listener->TransformMatrix[15];
			nsMath::mul<3>(ListenerPos, ListenerPos, 1.0f / _It_Listener->TransformMatrix[15]);

			float DCVector[3];
			nsMath::sub<3>(DCVector, ListenerPos, SrcPos);
			float DCLengthSqr = nsMath::dot<3>(DCVector, DCVector);
			float SampleDelay = sqrt(DCLengthSqr) / SoundSpeed;

			IRTrack *CurTrack = _It_Listener->lpListener->LoadTrack(Cluster_Index);
			_It_Listener->lpListener->SetCurTrack(CurTrack);

			_It_Listener->lpListener->LoadHRTF(DCVector, &_It_Listener->TransformMatrix[0], &_It_Listener->TransformMatrix[4], &_It_Listener->TransformMatrix[8]);

			if (Connection_Counter > 0){
				//direct response
				nsMath::TinyVector<float, 2> RayInfo = Connection[Connection_Index];

				if (CurRelation->PrevFrame_Exist){
					_It_Listener->lpListener->LoadDopplerShift((INT4b)CurRelation->DCDelay_Prev - (INT4b)SampleDelay);
					CurRelation->DCDelay_Prev = SampleDelay;
				}
				else{
					CurRelation->PrevFrame_Exist = true;
					_It_Listener->lpListener->LoadDopplerShift(0);
					CurRelation->DCDelay_Prev = SampleDelay;
				}

				if (WithDirectContribution && RayInfo[0] > 0.0f && SampleDelay < MaxResponseDelay){
					FloatVec_SIMD DCIntensity = ConnectIntensity[Connection_Index];
					// sound medium, temporary --BEGIN--
					FloatVec_SIMD Attenuation = this->DecayCoeff;
					Attenuation.mul(SampleDelay * SoundSpeed);
					Attenuation.exp();
					DCIntensity.mul(Attenuation);
					// sound medium, temporary --END--
					DCIntensity.mul(_It_Source->Intensity);
					this->CurrentStatInfo.SampleCounter_Frame++;
					_It_Listener->lpListener->EvaluateImpulse(SampleDelay, DCIntensity);
				}
				else{
					for (UBINT j = 0; j < FloatVec_SIMD::Width; j++)_It_Source->Intensity_Prev[0][j] = 0.0f;
					for (UBINT j = 0; j < FloatVec_SIMD::Width; j++)_It_Source->Intensity_Prev[1][j] = 0.0f;
				}
#if !defined USE_DIFFUSE_CACHE
				_It_Listener->lpListener->LoadDopplerShift(0);
#endif
				Connection_Index++;
				Connection_Counter--;
			}
			else{
				for (UBINT j = 0; j < FloatVec_SIMD::Width; j++)_It_Source->Intensity_Prev[0][j] = 0.0f;
				for (UBINT j = 0; j < FloatVec_SIMD::Width; j++)_It_Source->Intensity_Prev[1][j] = 0.0f;
			}

			// PREPROCESS --END--

			float SampleDelay_Min = MaxResponseDelay, SampleDelay_Max = SampleDelay; // Here [SampleDelay] is the delay of the direct response.
			if (CurRelation->SampleDelay_Min < SampleDelay)CurRelation->SampleDelay_Min = SampleDelay;

			for (UBINT i = 0; i < Connection_Counter; i++){
				nsMath::TinyVector<float, 2> RayInfo = Connection[Connection_Index];
				UINT4b FwdPathIndex = ConnectIndex[Connection_Index][0];
				UINT4b FwdRayBounce = ConnectIndex[Connection_Index][1];
				UINT4b BwdPathIndex = ConnectIndex[Connection_Index][2];
				UINT4b BwdRayBounce = ConnectIndex[Connection_Index][3];
				FloatVec_SIMD Energy = ConnectIntensity[Connection_Index];

				if (FwdRayBounce < this->Buffer.lpPathBounces[FwdPathIndex] && BwdRayBounce < this->Buffer.lpPathBounces[BwdPathIndex] && RayInfo[0] > 0){
					//valid path

#if defined USE_MIS
					Energy.mul(_It_Source->Intensity * RayInfo[1] / CurRelation->SampleCntPerBounce[FwdRayBounce + BwdRayBounce - 1]);
#else
					Energy.mul(_It_Source->Intensity / CurRelation->SampleCntPerBounce[FwdRayBounce + BwdRayBounce - 1]);
#endif

					UBINT BinIndex = (UBINT)((float)IRBinCount * RayInfo[0] / (SoundSpeed * MaxResponseDelay));
					UBINT PathSpaceIndex = FwdRayBounce + BwdRayBounce - 1;
					IntensityBin[PathSpaceIndex * IRBinCount + BinIndex] += Energy.sum();
					IntensitySqrBin[PathSpaceIndex * IRBinCount + BinIndex] += Energy.sum() * Energy.sum();

#if defined USE_DIFFUSE_CACHE
					UINT4b CacheIndex_Fwd, CacheIndex_Bwd;
					float EnergyMultiplier;

					if (FwdRayBounce > 0){
						if (BwdRayBounce > 0){
							CacheIndex_Fwd = FwdPathIndex * this->CurrentStat.PathDepth;
							CacheIndex_Bwd = BwdPathIndex * this->CurrentStat.PathDepth;
							RayInfo[0] -= this->Buffer.lpPath[CacheIndex_Fwd + 1].Length + this->Buffer.lpPath[CacheIndex_Bwd + 1].Length;
							EnergyMultiplier = this->Buffer.lpPath_Internal[CacheIndex_Fwd + 1].InvSegProb * this->Buffer.lpPath_Internal[CacheIndex_Bwd + 1].InvSegProb;
						}
						else{
							CacheIndex_Fwd = FwdPathIndex * this->CurrentStat.PathDepth;
							CacheIndex_Bwd = FwdPathIndex * this->CurrentStat.PathDepth + FwdRayBounce;
							nsMath::TinyVector<float, 3> DirectConnectVec;
							nsMath::sub<3>(DirectConnectVec.Data, ListenerPos, this->Buffer.lpPath[CacheIndex_Bwd].Position.Data);
							float DirectConnectVec_LenSqr = nsMath::dot<3>(DirectConnectVec.Data, DirectConnectVec.Data);
							RayInfo[0] -= this->Buffer.lpPath[CacheIndex_Fwd + 1].Length + sqrt(DirectConnectVec_LenSqr);
							EnergyMultiplier = this->Buffer.lpPath_Internal[CacheIndex_Fwd + 1].InvSegProb * (sqrt(DirectConnectVec_LenSqr) * DirectConnectVec_LenSqr / nsMath::dot<3>(DirectConnectVec.Data, this->Buffer.lpPath[CacheIndex_Bwd].Normal.Data));
						}
					}
					else{
						CacheIndex_Fwd = BwdPathIndex * this->CurrentStat.PathDepth + BwdRayBounce;
						CacheIndex_Bwd = BwdPathIndex * this->CurrentStat.PathDepth;
						nsMath::TinyVector<float, 3> DirectConnectVec;
						nsMath::sub<3>(DirectConnectVec.Data, SrcPos, this->Buffer.lpPath[CacheIndex_Fwd].Position.Data);
						float DirectConnectVec_LenSqr = nsMath::dot<3>(DirectConnectVec.Data, DirectConnectVec.Data);
						RayInfo[0] -= sqrt(DirectConnectVec_LenSqr) + this->Buffer.lpPath[CacheIndex_Bwd + 1].Length;
						EnergyMultiplier = this->Buffer.lpPath_Internal[CacheIndex_Bwd + 1].InvSegProb * (sqrt(DirectConnectVec_LenSqr) * DirectConnectVec_LenSqr / nsMath::dot<3>(DirectConnectVec.Data, this->Buffer.lpPath[CacheIndex_Fwd].Normal.Data));
					}
					Energy.mul(EnergyMultiplier);
#else
					float PosVector_Listener[3];
					if (0 == BwdRayBounce)nsMath::sub<3>(PosVector_Listener, ListenerPos, this->Buffer.lpPath[FwdPathIndex * this->CurrentStat.PathDepth + FwdRayBounce].Position.Data);
					else nsMath::sub<3>(PosVector_Listener, ListenerPos, this->Buffer.lpPath[BwdPathIndex * this->CurrentStat.PathDepth + 1].Position.Data);
					_It_Listener->lpListener->LoadHRTF(PosVector_Listener, &_It_Listener->TransformMatrix[0], &_It_Listener->TransformMatrix[4], &_It_Listener->TransformMatrix[8]);
#endif

					float SampleDelay = RayInfo[0] / SoundSpeed;

					if (SampleDelay < MaxResponseDelay){
#if defined USE_DIFFUSE_CACHE
						DiffCacheItem *CurItem = CurRelation->DiffCache.GetWriteAddr();
						CurItem->NodeIndex_Fwd = CacheIndex_Fwd;
						CurItem->NodeIndex_Bwd = CacheIndex_Bwd;
						CurItem->Delay_SubPath = RayInfo[0];
						Energy.storeu(CurItem->Intensity);
#else
						// sound medium, temporary --BEGIN--
						FloatVec_SIMD Attenuation = this->DecayCoeff;
						Attenuation.mul(SampleDelay * SoundSpeed);
						Attenuation.exp();
						Energy.mul(Attenuation);
						// sound medium, temporary --END--
						_It_Listener->lpListener->EvaluateImpulse(SampleDelay, Energy);
#endif
						SampleCounter_Indirect++;
						if (SampleDelay < SampleDelay_Min)SampleDelay_Min = SampleDelay;
						if (SampleDelay > SampleDelay_Max)SampleDelay_Max = SampleDelay;
					}
				}
				Connection_Index++;
			}
			if (0 == SampleCounter_Indirect){
				SampleDelay_Min = CurRelation->DCDelay_Prev;
				SampleDelay_Max = MaxResponseDelay;
			}
			else{
				SampleDelay_Min -= 0.1f;
				SampleDelay_Max += 0.1f;
				if (SampleDelay_Min < CurRelation->DCDelay_Prev)SampleDelay_Min = CurRelation->DCDelay_Prev;
				if (SampleDelay_Max > MaxResponseDelay)SampleDelay_Max = MaxResponseDelay;
			}
			if (SampleDelay_Min < CurRelation->SampleDelay_Min)CurRelation->SampleDelay_Min = SampleDelay_Min; else CurRelation->SampleDelay_Min = 0.5 * (CurRelation->SampleDelay_Min + SampleDelay_Min);
			if (SampleDelay_Max > CurRelation->SampleDelay_Min)CurRelation->SampleDelay_Max = SampleDelay_Max; else CurRelation->SampleDelay_Max = 0.5 * (CurRelation->SampleDelay_Max + SampleDelay_Max);

			// POSTPROCESS --BEGIN--

			UBINT BinIndex_Min = (UBINT)((float)IRBinCount * SampleDelay_Min / MaxResponseDelay);
			UBINT BinIndex_Max = (UBINT)((float)IRBinCount * (0.2f * SampleDelay_Min + 0.8f * SampleDelay_Max) / MaxResponseDelay);

			//calculate the squared deviation
			for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++){
				for (UBINT k = 0; k < IRBinCount; k++)IntensitySqrBin[j * IRBinCount + k] = (IntensitySqrBin[j * IRBinCount + k] - (IntensityBin[j * IRBinCount + k] * IntensityBin[j * IRBinCount + k]) / (float)CurRelation->SampleCntPerBounce[j]) / (float)(CurRelation->SampleCntPerBounce[j] - 1);
				// now [IntensitySqrBin] is the variance estimation of the current frame. 
				if ((float)CurRelation->SampleCntPerBounce[j] > VarEstimateQuality){
					// use it directly
					for (UBINT k = 0; k < IRBinCount; k++)CurRelation->AccumulatedVar[j * IRBinCount + k] = IntensitySqrBin[j * IRBinCount + k];
					CurRelation->AccumulatedVarQuality[j] = (float)CurRelation->SampleCntPerBounce[j];
				}
				else if (CurRelation->AccumulatedVarQuality[j] + (float)CurRelation->SampleCntPerBounce[j] < VarEstimateQuality){
					// maximize quality
					float QualitySum = CurRelation->AccumulatedVarQuality[j] + (float)CurRelation->SampleCntPerBounce[j];
					for (UBINT k = 0; k < IRBinCount; k++){
						CurRelation->AccumulatedVar[j * IRBinCount + k] = CurRelation->AccumulatedVar[j * IRBinCount + k] * CurRelation->AccumulatedVarQuality[j];
						CurRelation->AccumulatedVar[j * IRBinCount + k] += IntensitySqrBin[j * IRBinCount + k] * (float)CurRelation->SampleCntPerBounce[j];
						CurRelation->AccumulatedVar[j * IRBinCount + k] /= QualitySum;
					}
					CurRelation->AccumulatedVarQuality[j] += (float)CurRelation->SampleCntPerBounce[j];
				}
				else if (VarEstimateQuality == CurRelation->AccumulatedVarQuality[j]){
					// keep the quality stable
					float QualitySum = CurRelation->AccumulatedVarQuality[j] + (float)CurRelation->SampleCntPerBounce[j];
					for (UBINT k = 0; k < IRBinCount; k++){
						CurRelation->AccumulatedVar[j * IRBinCount + k] = CurRelation->AccumulatedVar[j * IRBinCount + k] * (CurRelation->AccumulatedVarQuality[j] - (float)CurRelation->SampleCntPerBounce[j]);
						CurRelation->AccumulatedVar[j * IRBinCount + k] += IntensitySqrBin[j * IRBinCount + k] * (float)(2 * CurRelation->SampleCntPerBounce[j]);
						CurRelation->AccumulatedVar[j * IRBinCount + k] /= QualitySum;
					}
				}
				else{
					float gamma = CurRelation->AccumulatedVarQuality[j] - sqrt(CurRelation->AccumulatedVarQuality[j] * (float)CurRelation->SampleCntPerBounce[j] * ((CurRelation->AccumulatedVarQuality[j] + (float)CurRelation->SampleCntPerBounce[j]) / VarEstimateQuality - 1.0f));
					gamma /= CurRelation->AccumulatedVarQuality[j] + (float)CurRelation->SampleCntPerBounce[j];
					for (UBINT k = 0; k < IRBinCount; k++){
						CurRelation->AccumulatedVar[j * IRBinCount + k] = CurRelation->AccumulatedVar[j * IRBinCount + k] * gamma;
						CurRelation->AccumulatedVar[j * IRBinCount + k] += IntensitySqrBin[j * IRBinCount + k] * (1 - gamma);
					}
					CurRelation->AccumulatedVarQuality[j] = VarEstimateQuality;
				}
			}

			//calculate SNR
			float *ExpSNR = nsBasic::MemAlloc_Arr<float>(IRBinCount * (this->CurrentStat.PathDepth - 1));
			for (UBINT k = 0; k < IRBinCount; k++){
				if (k >= BinIndex_Min && k <= BinIndex_Max){
					for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++){
						ExpSNR[j * IRBinCount + k] = CurRelation->AccumulatedVar[j * IRBinCount + k] / CurRelation->SampleProbability[j];
						if (ExpSNR[j * IRBinCount + k] < 1E-30f)ExpSNR[j * IRBinCount + k] = 1E-30f;
					}
				}
				else for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++)ExpSNR[j * IRBinCount + k] = 0.0f;
			}

			// schedule new samples
#if defined ADAPTIVE_SAMPLING
			float *SNRGradient = nsBasic::MemAlloc_Arr<float>(this->CurrentStat.PathDepth - 1);
			for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++)SNRGradient[j] = 0.0f;
			UBINT IRBinCount_Actual = 0;
			for (UBINT j = 0; j < IRBinCount; j++){
				float TotalSNR = 0.0f;
				for (UBINT k = 0; k < this->CurrentStat.PathDepth - 1; k++)TotalSNR += ExpSNR[k * IRBinCount + j];
				if (TotalSNR > 0.0f){
					for (UBINT k = 0; k < this->CurrentStat.PathDepth - 1; k++)SNRGradient[k] += ExpSNR[k * IRBinCount + j] / TotalSNR;
					IRBinCount_Actual++;
				}
			}
			if (0 == IRBinCount_Actual)for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++)SNRGradient[j] = 1.0f / (float)(this->CurrentStat.PathDepth - 1);
			else for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++)SNRGradient[j] /= (float)IRBinCount_Actual;

			for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++)CurRelation->SampleProbability[j] = (CurRelation->SampleProbability[j] + SNRGradient[j]) / 2.0f;
			nsBasic::MemFree_Arr<float>(SNRGradient, this->CurrentStat.PathDepth - 1);
#else
			for (UBINT j = 0; j < this->CurrentStat.PathDepth - 1; j++)CurRelation->SampleProbability[j] = 0.0f;
#endif

			//refresh the sample probability.

			nsBasic::MemFree_Arr<float>(ExpSNR, IRBinCount * (this->CurrentStat.PathDepth - 1));

			// POSTPROCESS --END--

#if defined USE_DIFFUSE_CACHE
			CurRelation->DiffCache.EndFrame();
#endif

			//move to the next relation
			CurRelation = CurRelation->Col_Next;
		}
		Cluster_Index++;
	}
	this->CurrentStatInfo.SampleCounter_Frame += SampleCounter_Indirect;

#if defined USE_DIFFUSE_CACHE
	PathCacheNode NewNode;
	NewNode.CacheFrameSize = this->CurrentStat.PathDepth * this->CurrentStat.PathCount;
	NewNode.CachedSampleCount = SampleCounter_Indirect;
	NewNode.CacheLifeTime = DiffCacheMaxLifetime;

	NewNode.pRefreshToken = nsBasic::MemAlloc_Arr<UBINT>(NewNode.CacheFrameSize);
	if (nullptr == NewNode.pRefreshToken)throw std::bad_alloc();
	nsMath::setzero_simd_unaligned(NewNode.pRefreshToken, NewNode.CacheFrameSize);

	NewNode.pPathNode = nsBasic::MemAlloc_Arr<PathNodeData>(NewNode.CacheFrameSize);
	if (nullptr == NewNode.pPathNode)throw std::bad_alloc();
	nsBasic::MemMove_Arr(NewNode.pPathNode, this->Buffer.lpPath, NewNode.CacheFrameSize);

	// Hacking.
	// Sometimes the forward cache index and the backward cache index are the same. So I move some data around to prevent the two index from pointing to the same address.
	for (UBINT i = 0; i < this->CurrentStat.PathCount; i++){
		UBINT CacheIndex = i * this->CurrentStat.PathDepth;
		NewNode.pPathNode[CacheIndex] = NewNode.pPathNode[CacheIndex + 1];
	}

	this->PathCache.push_back(NewNode);

	// delete stale cache pages
	{
		UINT8b TotalSampleCount = 0;
		auto _It_PathCache = --this->PathCache.end();
		while (_It_PathCache != this->PathCache.end()){
			if (0 == _It_PathCache->CacheLifeTime || TotalSampleCount >= DiffCacheSize)break;
			_It_PathCache->CacheLifeTime--;
			TotalSampleCount += _It_PathCache->CachedSampleCount;
			--_It_PathCache;
		}
		while (_It_PathCache != this->PathCache.end()){
			if (_It_PathCache->CacheFrameSize > 0){
				nsBasic::MemFree_Arr(_It_PathCache->pPathNode, _It_PathCache->CacheFrameSize);
				nsBasic::MemFree_Arr(_It_PathCache->pRefreshToken, _It_PathCache->CacheFrameSize);
			}
			auto _It_Next = _It_PathCache;
			--_It_PathCache;
			this->PathCache.erase(_It_Next);
		}
		for (_It_PathCache = this->PathCache.begin(); _It_PathCache != this->PathCache.end(); ++_It_PathCache){
			if (TotalSampleCount > 0)_It_PathCache->WeightCurrent = (float)_It_PathCache->CachedSampleCount / (float)TotalSampleCount;
			else _It_PathCache->WeightCurrent = 0;
		}
	}
#endif
	nsBasic::MemFree_Arr(IntensitySqrBin, IRBinCount * (this->CurrentStat.PathDepth - 1));
	nsBasic::MemFree_Arr(IntensityBin, IRBinCount * (this->CurrentStat.PathDepth - 1));

#if defined USE_DIFFUSE_CACHE
	for (auto _It = lpSoundMgr->SoundClusterList_Listener.begin(); _It != lpSoundMgr->SoundClusterList_Listener.end(); ++_It){
		this->UpdateDiffuseCache(&lpSoundMgr->SoundClusterList_Source, &(*_It));
	}
#endif

	this->CurrentStatInfo.SampleCounter_Total += this->CurrentStatInfo.SampleCounter_Frame;
	this->CurrentStatInfo.FrameCounter++;
}
void PathTracer::PostProcess(SoundClusterMgr *lpSoundMgr){
	for (auto _It = lpSoundMgr->SoundClusterList_Listener.begin(); _It != lpSoundMgr->SoundClusterList_Listener.end(); ++_It){
		UBINT Cluster_Index = 0;
		for (auto _It2 = lpSoundMgr->SoundClusterList_Source.begin(); _It2 != lpSoundMgr->SoundClusterList_Source.end(); ++_It2){
			IRTrack *CurTrack = _It->lpListener->LoadTrack(Cluster_Index);
			CurTrack->StreamList = _It2->StreamList;
			CurTrack->SetRenderParams(_It2->pFilter, _It2->pLane);
			_It->lpListener->SetCurTrack(CurTrack);
			_It->lpListener->EndWrite_CurTrack();
			Cluster_Index++;
		}
	}
}
PathTracer::~PathTracer(){
#if defined USE_DIFFUSE_CACHE
	for (auto _It_PathCache = this->PathCache.begin(); _It_PathCache != this->PathCache.end(); ++_It_PathCache){
		nsBasic::MemFree_Arr(_It_PathCache->pPathNode, _It_PathCache->CacheFrameSize);
		nsBasic::MemFree_Arr(_It_PathCache->pRefreshToken, _It_PathCache->CacheFrameSize);
	}
#endif
	this->On_ClusterCountChange(0, 0);
	this->On_PathCountChange(0);
	this->On_ConnectionCountChange(0);
}