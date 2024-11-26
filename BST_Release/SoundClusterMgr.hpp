#ifndef SOUND_CLUSTER_MANAGER
#define SOUND_CLUSTER_MANAGER

#include "lContainer.hpp"
#include "Listener.hpp"
#include "SoundMaterial.hpp"

struct SoundClusterRelation{
	//row for source, column for listener.
	SoundClusterRelation *Row_Prev;
	SoundClusterRelation *Row_Next;

	SoundClusterRelation *Col_Prev;
	SoundClusterRelation *Col_Next;

	bool PrevFrame_Exist;
	float DCDelay_Prev;
	float SampleDelay_Min, SampleDelay_Max;
	float BinWeight[IRBinCount];

	UBINT MaxPathSpaceIndex;
	float *AccumulatedVarQuality;
	float *AccumulatedVar;
	float *SampleProbability;
	UBINT *SampleCntPerBounce;

#if defined USE_DIFFUSE_CACHE
	 DiffCache DiffCache;
#endif

	SoundClusterRelation();
	void SetPathSpace(UBINT MaxPathSpaceIndex);
	~SoundClusterRelation();
};

class SoundCluster_Source{
public:
	float TransformMatrix[16];

	float Intensity;
	float Intensity_Prev[2][FloatVec_SIMD::Width];
	
	UBINT PathCount;

	bool Change_Exist;

	SoundClusterRelation *lpRelation_Col_Prev;
	SoundClusterRelation *lpRelation_Col_Next;

	nsText::String ClusterName;
	nsContainer::Vector<AudioRenderer::InputStream *> StreamList;
	AudioRenderer::Filter *pFilter;
	AudioRenderer::FilterLane *pLane[BandCount];
};

class SoundCluster_Listener{
public:
	float TransformMatrix[16];
	float Volume;

	UBINT PathCount;

	bool Change_Exist;

	SoundClusterRelation *lpRelation_Row_Prev;
	SoundClusterRelation *lpRelation_Row_Next;

	nsText::String ClusterName;

	Listener *lpListener;
};

class SoundClusterMgr{
public:
	nsContainer::List<SoundCluster_Source> SoundClusterList_Source;
	nsContainer::List<SoundCluster_Listener> SoundClusterList_Listener;

	SoundClusterMgr() :SoundClusterList_Source(), SoundClusterList_Listener(){}
	SoundCluster_Source *AddSource(float Intensity);
	SoundCluster_Listener *AddListener(Listener *lpListener, float Volume);
	void DeleteSource(nsContainer::List<SoundCluster_Source>::iterator _It);
	void DeleteListener(nsContainer::List<SoundCluster_Listener>::iterator _It);
	void Clear();
	~SoundClusterMgr();
};

/*-------------------------------- IMPLEMENTATION --------------------------------*/

SoundClusterRelation::SoundClusterRelation(){
	this->DCDelay_Prev = 0.0f;
	this->SampleDelay_Min = 0.0f; this->SampleDelay_Max = 0.0f;
	this->PrevFrame_Exist = false;
	nsMath::set_simd_unaligned(this->BinWeight, 1.0f, IRBinCount);
	this->MaxPathSpaceIndex = 0;
}
void SoundClusterRelation::SetPathSpace(UBINT MaxPathSpaceIndex){
	if (this->MaxPathSpaceIndex != MaxPathSpaceIndex){
		if (this->MaxPathSpaceIndex > 0){
			nsBasic::MemFree_Arr<UBINT>(this->SampleCntPerBounce, MaxPathSpaceIndex - 1);
			nsBasic::MemFree_Arr<float>(this->SampleProbability, MaxPathSpaceIndex - 1);
			nsBasic::MemFree_Arr<float>(this->AccumulatedVarQuality, MaxPathSpaceIndex - 1);
			nsBasic::MemFree_Arr<float>(this->AccumulatedVar, IRBinCount * (MaxPathSpaceIndex - 1));
			this->MaxPathSpaceIndex = 0;
			return;
		}
		else{
			float *AccumulatedVar_Tmp, *AccumulatedVarQuality_Tmp, *SampleProbability_Tmp;
			UBINT *SampleCntPerBounce_Tmp;

			AccumulatedVar_Tmp = nsBasic::MemAlloc_Arr<float>(IRBinCount * (MaxPathSpaceIndex - 1));
			if (nullptr != AccumulatedVar_Tmp){
				AccumulatedVarQuality_Tmp = nsBasic::MemAlloc_Arr<float>(MaxPathSpaceIndex - 1);
				if (nullptr != AccumulatedVarQuality_Tmp){
					SampleProbability_Tmp = nsBasic::MemAlloc_Arr<float>(MaxPathSpaceIndex - 1);
					if (nullptr != SampleProbability_Tmp){
						SampleCntPerBounce_Tmp = nsBasic::MemAlloc_Arr<UBINT>(MaxPathSpaceIndex - 1);
						if (nullptr != SampleCntPerBounce_Tmp){
							if (this->MaxPathSpaceIndex > 1){
								nsBasic::MemMove_Arr<float>(AccumulatedVar_Tmp, this->AccumulatedVar, IRBinCount * std::min(MaxPathSpaceIndex - 1, this->MaxPathSpaceIndex - 1));
								float LastSpaceVariance = this->AccumulatedVar[this->MaxPathSpaceIndex - 2];
								for (UBINT i = this->MaxPathSpaceIndex - 1; i < MaxPathSpaceIndex - 1; i++){
									for (UBINT j = 0; j < IRBinCount; j++){
										AccumulatedVar_Tmp[i * IRBinCount + j] = this->AccumulatedVar[(this->MaxPathSpaceIndex - 2) * IRBinCount + j];
									}
								}

								nsBasic::MemMove_Arr<float>(AccumulatedVarQuality_Tmp, this->AccumulatedVarQuality, std::min(MaxPathSpaceIndex - 1, this->MaxPathSpaceIndex - 1));
								for (UBINT i = this->MaxPathSpaceIndex - 1; i < MaxPathSpaceIndex - 1; i++)AccumulatedVarQuality_Tmp[i] = 0.0f;

								nsBasic::MemFree_Arr<UBINT>(this->SampleCntPerBounce, this->MaxPathSpaceIndex - 1);
								nsBasic::MemFree_Arr<float>(this->SampleProbability, this->MaxPathSpaceIndex - 1);
								nsBasic::MemFree_Arr<float>(this->AccumulatedVar, IRBinCount * (this->MaxPathSpaceIndex - 1));
							}
							else{
								for (UBINT i = 0; i < IRBinCount * (MaxPathSpaceIndex - 1); i++)AccumulatedVar_Tmp[i] = 1.0f;
								for (UBINT i = 0; i < MaxPathSpaceIndex - 1; i++)AccumulatedVarQuality_Tmp[i] = 0.0f;
							}
							for (UBINT i = 0; i < MaxPathSpaceIndex - 1; i++)SampleProbability_Tmp[i] = 1.0f / (float)(MaxPathSpaceIndex - 1);

							this->AccumulatedVar = AccumulatedVar_Tmp;
							this->AccumulatedVarQuality = AccumulatedVarQuality_Tmp;
							this->SampleProbability = SampleProbability_Tmp;
							this->SampleCntPerBounce = SampleCntPerBounce_Tmp;
							this->MaxPathSpaceIndex = MaxPathSpaceIndex;
							return;
						}
						nsBasic::MemFree_Arr<float>(SampleProbability_Tmp, MaxPathSpaceIndex - 1);
					}
					nsBasic::MemFree_Arr<float>(AccumulatedVarQuality_Tmp, MaxPathSpaceIndex - 1);
				}
				nsBasic::MemFree_Arr<float>(AccumulatedVar_Tmp, MaxPathSpaceIndex - 1);
			}
			throw std::bad_alloc();
		}
	}
}
SoundClusterRelation::~SoundClusterRelation(){
	if (this->MaxPathSpaceIndex > 0){
		nsBasic::MemFree_Arr<UBINT>(this->SampleCntPerBounce, MaxPathSpaceIndex - 1);
		nsBasic::MemFree_Arr<float>(this->SampleProbability, MaxPathSpaceIndex - 1);
		nsBasic::MemFree_Arr<float>(this->AccumulatedVarQuality, MaxPathSpaceIndex - 1);
		nsBasic::MemFree_Arr<float>(this->AccumulatedVar, IRBinCount * (MaxPathSpaceIndex - 1));
	}
}

SoundCluster_Source *SoundClusterMgr::AddSource(float Intensity){
	this->SoundClusterList_Source.emplace_back();
	SoundCluster_Source *RetValue = &this->SoundClusterList_Source.back();
	RetValue->Intensity = Intensity;
	for (UBINT i = 0; i < FloatVec_SIMD::Width; i++)RetValue->Intensity_Prev[0][i] = 0.0f;
	for (UBINT i = 0; i < FloatVec_SIMD::Width; i++)RetValue->Intensity_Prev[1][i] = 0.0f;
	
	RetValue->PathCount = 0;
	RetValue->Change_Exist = true;

	RetValue->lpRelation_Col_Prev = nullptr;
	RetValue->lpRelation_Col_Next = nullptr;
	for (auto _It = this->SoundClusterList_Listener.begin(); this->SoundClusterList_Listener.end() != _It; ++_It){
		SoundClusterRelation *NewSoundRelation;
		try{ NewSoundRelation = new SoundClusterRelation; }
		catch (...){
			while (RetValue->lpRelation_Col_Prev != nullptr){
				SoundClusterRelation *TmpPtr = RetValue->lpRelation_Col_Prev;
				RetValue->lpRelation_Col_Prev = TmpPtr->Col_Prev;
				delete TmpPtr;
			}
			this->SoundClusterList_Source.pop_back();
			throw std::bad_alloc();
		}
		NewSoundRelation->Col_Prev = RetValue->lpRelation_Col_Prev;
		if (nullptr == RetValue->lpRelation_Col_Prev)RetValue->lpRelation_Col_Next = NewSoundRelation;
		else RetValue->lpRelation_Col_Prev->Col_Next = NewSoundRelation;
		RetValue->lpRelation_Col_Prev = NewSoundRelation;
	}
	if (nullptr != RetValue->lpRelation_Col_Prev)RetValue->lpRelation_Col_Prev->Col_Next = nullptr;
	if (nullptr != RetValue->lpRelation_Col_Next)RetValue->lpRelation_Col_Next->Col_Prev = nullptr;

	SoundClusterRelation *TmpRelationPtr = RetValue->lpRelation_Col_Next;
	for (auto _It = this->SoundClusterList_Listener.begin(); this->SoundClusterList_Listener.end() != _It; ++_It){
		TmpRelationPtr->Row_Prev = _It->lpRelation_Row_Prev;
		TmpRelationPtr->Row_Next = nullptr;
		if (nullptr == _It->lpRelation_Row_Prev)_It->lpRelation_Row_Next = TmpRelationPtr;
		else _It->lpRelation_Row_Prev->Row_Next = TmpRelationPtr;
		_It->lpRelation_Row_Prev = TmpRelationPtr;
		TmpRelationPtr = TmpRelationPtr->Col_Next;
	}

	return RetValue;
}

SoundCluster_Listener *SoundClusterMgr::AddListener(Listener *lpListener, float Volume){
	this->SoundClusterList_Listener.emplace_back();
	SoundCluster_Listener *RetValue = &this->SoundClusterList_Listener.back();
	RetValue->lpListener = lpListener;
	RetValue->Volume = Volume;
	RetValue->PathCount = 0;
	RetValue->Change_Exist = true;

	RetValue->lpRelation_Row_Prev = nullptr;
	RetValue->lpRelation_Row_Next = nullptr;
	for (auto _It = this->SoundClusterList_Source.begin(); this->SoundClusterList_Source.end() != _It; ++_It){
		SoundClusterRelation *NewSoundRelation;
		try{ NewSoundRelation = new SoundClusterRelation; }
		catch (...){
			while (RetValue->lpRelation_Row_Prev != nullptr){
				SoundClusterRelation *TmpPtr = RetValue->lpRelation_Row_Prev;
				RetValue->lpRelation_Row_Prev = TmpPtr->Row_Prev;
				delete TmpPtr;
			}
			this->SoundClusterList_Listener.pop_back();
			throw std::bad_alloc();
		}
		NewSoundRelation->Row_Prev = RetValue->lpRelation_Row_Prev;
		if (nullptr == RetValue->lpRelation_Row_Prev)RetValue->lpRelation_Row_Next = NewSoundRelation;
		else RetValue->lpRelation_Row_Prev->Row_Next = NewSoundRelation;
		RetValue->lpRelation_Row_Prev = NewSoundRelation;
	}
	if (nullptr != RetValue->lpRelation_Row_Prev)RetValue->lpRelation_Row_Prev->Row_Next = nullptr;
	if (nullptr != RetValue->lpRelation_Row_Next)RetValue->lpRelation_Row_Next->Row_Prev = nullptr;

	SoundClusterRelation *TmpRelationPtr = RetValue->lpRelation_Row_Next;
	for (auto _It = this->SoundClusterList_Source.begin(); this->SoundClusterList_Source.end() != _It; ++_It){
		TmpRelationPtr->Col_Prev = _It->lpRelation_Col_Prev;
		TmpRelationPtr->Col_Next = nullptr;
		if (nullptr == _It->lpRelation_Col_Prev)_It->lpRelation_Col_Next = TmpRelationPtr;
		else _It->lpRelation_Col_Prev->Col_Next = TmpRelationPtr;
		_It->lpRelation_Col_Prev = TmpRelationPtr;
		TmpRelationPtr = TmpRelationPtr->Row_Next;
	}

	return RetValue;
}
void SoundClusterMgr::DeleteSource(nsContainer::List<SoundCluster_Source>::iterator _It){
	SoundClusterRelation *TmpRelationPtr = _It->lpRelation_Col_Next;
	for (auto _It2 = this->SoundClusterList_Listener.begin(); _It2 != this->SoundClusterList_Listener.end(); ++_It2){
		if (nullptr == TmpRelationPtr->Row_Prev)_It2->lpRelation_Row_Next = TmpRelationPtr->Row_Next;
		else TmpRelationPtr->Row_Prev->Row_Next = TmpRelationPtr->Row_Next;
		if (nullptr == TmpRelationPtr->Row_Next)_It2->lpRelation_Row_Prev = TmpRelationPtr->Row_Prev;
		else TmpRelationPtr->Row_Next->Row_Prev = TmpRelationPtr->Row_Prev;
		
		SoundClusterRelation *TmpPtr = TmpRelationPtr;
		TmpRelationPtr = TmpRelationPtr->Col_Next;
		nsBasic::GlobalMemFree(TmpPtr, sizeof(SoundClusterRelation));
	}
	this->SoundClusterList_Source.erase(_It);
}
void SoundClusterMgr::DeleteListener(nsContainer::List<SoundCluster_Listener>::iterator _It){
	SoundClusterRelation *TmpRelationPtr = _It->lpRelation_Row_Next;
	for (auto _It2 = this->SoundClusterList_Source.begin(); _It2 != this->SoundClusterList_Source.end(); ++_It2){
		if (nullptr == TmpRelationPtr->Col_Prev)_It2->lpRelation_Col_Next = TmpRelationPtr->Col_Next;
		else TmpRelationPtr->Col_Prev->Col_Next = TmpRelationPtr->Col_Next;
		if (nullptr == TmpRelationPtr->Col_Next)_It2->lpRelation_Col_Prev = TmpRelationPtr->Col_Prev;
		else TmpRelationPtr->Col_Next->Col_Prev = TmpRelationPtr->Col_Prev;

		SoundClusterRelation *TmpPtr = TmpRelationPtr;
		TmpRelationPtr = TmpRelationPtr->Row_Next;
		delete TmpPtr;
	}
	this->SoundClusterList_Listener.erase(_It);
}
void SoundClusterMgr::Clear(){
	for (auto _It = this->SoundClusterList_Source.begin(); _It != this->SoundClusterList_Source.end(); ++_It){
		while (_It->lpRelation_Col_Next != nullptr){
			SoundClusterRelation *TmpPtr = _It->lpRelation_Col_Next;
			_It->lpRelation_Col_Next = TmpPtr->Col_Next;
			delete TmpPtr;
		}
	}
	this->SoundClusterList_Listener.clear();
	this->SoundClusterList_Source.clear();
}
inline SoundClusterMgr::~SoundClusterMgr(){ this->Clear(); }
#endif