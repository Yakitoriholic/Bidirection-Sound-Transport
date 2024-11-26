#ifndef DIFFUSE_CACHE
#define DIFFUSE_CACHE

#include "lGeneral.hpp"
#include "lContainer.hpp"
#include "SoundMaterial.hpp"

struct DiffCacheItem{
	UINT4b NodeIndex_Fwd;
	UINT4b NodeIndex_Bwd;
	float Delay_Prev; // sample delay in the previous frame
	float Delay_SubPath; // time delay of the subpath between two cache nodes
	float Intensity[FloatVec_SIMD::Width];
};

// NodeIndex_Fwd = (UBINT)-1, NodeIndex_Fwd = 0         -> deleted
// NodeIndex_Fwd = (UBINT)-1, NodeIndex_Fwd = (UBINT)-1 -> new frame

class DiffCache;

struct DiffCache_Iterator{
private:
	nsContainer::List<DiffCacheItem *>::iterator PageAddr;
	UBINT Offset;

	//types used inside the class
	typedef DiffCache_Iterator	this_type;

public:
	friend class DiffCache;
	bool operator==(const this_type& rhs) const { return (PageAddr == rhs.PageAddr) && (Offset == rhs.Offset); }
	bool operator!=(const this_type& rhs) const { return (PageAddr != rhs.PageAddr) || (Offset != rhs.Offset); }
	DiffCacheItem& operator*() const { return (*PageAddr)[Offset]; }
	DiffCacheItem* operator->() const { return &((*PageAddr)[Offset]); }
};

class DiffCache{
private:
	static const UBINT PageSize = 0x1000;
	
	nsContainer::List<DiffCacheItem *> PageAddrList;
	UBINT FirstPageOffset;
	UBINT LastPageOffset;
	UBINT FrameCount;
public:
	typedef DiffCache_Iterator iterator;

	DiffCache(){
		this->FirstPageOffset = 0;
		this->LastPageOffset = PageSize;
		this->FrameCount = 0;
	}
	inline iterator end(){ iterator RetValue; RetValue.PageAddr = this->PageAddrList.end(); RetValue.Offset = 0; return RetValue; }
	DiffCacheItem *GetWriteAddr(){
		if (LastPageOffset >= PageSize){
			try{ PageAddrList.push_back(nullptr); }
			catch (...){
				throw std::exception("Failed to allocate a new cache page.");
			}
			DiffCacheItem *NewPage = nsBasic::MemAlloc_Arr<DiffCacheItem>(PageSize);
			if (nullptr == NewPage){
				PageAddrList.pop_back();
				throw std::exception("Failed to allocate a new cache page.");
			}
			PageAddrList.back() = NewPage;
			LastPageOffset = 0;
		}

		LastPageOffset++;
		return PageAddrList.back() + LastPageOffset - 1;
	}
	void EndFrame(){
		if (LastPageOffset >= PageSize){
			try{ PageAddrList.push_back(nullptr); }
			catch (...){
				throw std::exception("Failed to allocate a new cache page.");
			}
			DiffCacheItem *NewPage = nsBasic::MemAlloc_Arr<DiffCacheItem>(PageSize);
			if (nullptr == NewPage){
				PageAddrList.pop_back();
				throw std::exception("Failed to allocate a new cache page.");
			}
			PageAddrList.back() = NewPage;
			LastPageOffset = 0;
		}

		PageAddrList.back()[LastPageOffset].NodeIndex_Fwd = (UINT4b)-1;
		PageAddrList.back()[LastPageOffset].NodeIndex_Bwd = (UINT4b)-1;
		LastPageOffset++;
		FrameCount++;
	}
	inline UBINT GetFrameCount(){ return this->FrameCount; }
	iterator GetReadIterator(){
		iterator RetValue;
		if (this->PageAddrList.empty())RetValue = this->end();
		else{
			RetValue.PageAddr = this->PageAddrList.begin();
			RetValue.Offset = this->FirstPageOffset;
		}
		return RetValue;
	}
	void Advance(iterator &_It){
		_It.Offset++;
		if (_It.Offset >= PageSize){
			++_It.PageAddr;
			if (_It.PageAddr == this->PageAddrList.end())_It = this->end();
			else _It.Offset = 0;
		}
		else if(_It.Offset == this->LastPageOffset){
			if (--(this->PageAddrList.end()) == _It.PageAddr)_It = this->end();
		}
	}
	void DeletePrevFrame(iterator &_It){
		// One should never call this function unless the iterator points to the end of the first frame.
		while (true){
			auto _It_PageList = _It.PageAddr;
			--_It_PageList;
			if (this->PageAddrList.end() == _It_PageList)break;
			nsBasic::MemFree_Arr<DiffCacheItem>(*_It_PageList, PageSize);
			this->PageAddrList.erase(_It_PageList);
		}
		this->FirstPageOffset = _It.Offset;
		if (this->PageAddrList.empty())this->LastPageOffset = PageSize;
		FrameCount--;
	}
	~DiffCache(){
		for (auto _It = this->PageAddrList.begin(); _It != this->PageAddrList.end(); ++_It)nsBasic::MemFree_Arr<DiffCacheItem>(*_It, PageSize);
	}
};

#endif