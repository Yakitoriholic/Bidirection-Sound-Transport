#ifndef LIB_RENDER_RESMGR_STATIC
#define LIB_RENDER_RESMGR_STATIC

#include "lGeneral.hpp"
#include "lMath_Matrix.hpp"
#include "lContainer.hpp"
#include "lGUI.hpp"

template <typename T> class TableObj{
private:
	struct HashNode{
		char *KeyStr;
		UBINT KeyStrLen;
		TableObj<T> *Value;
		HashNode *Next;
	};
	HashNode **BucketArr;
	UBINT BucketArrSize;
	UBINT ElemSize;
	UBINT RefCounter;

	static UBINT StrHash(const char *Key){
		//BKDRHash from NOCOW
		UBINT seed = 131, hash = 0;
		unsigned const char *TmpChr = (unsigned const char *)Key;
		while (0 != *TmpChr)hash = hash * seed + (*TmpChr++);
		return hash;
	}
	bool Rehash(UBINT NewArrSize){
		HashNode **NewBucketArr = (HashNode **)nsBasic::GlobalMemAlloc(NewArrSize*sizeof(HashNode *));
		if (nullptr == NewBucketArr)return false;
		else{
			memset(NewBucketArr, 0, NewArrSize*sizeof(HashNode *));

			//insert all the nodes
			for (UBINT i = 0; i < this->BucketArrSize; i++){
				HashNode *BucketPtr = this->BucketArr[i];
				while (nullptr != BucketPtr){
					HashNode *CurPtr = BucketPtr;
					BucketPtr = CurPtr->Next;
					UBINT HashVal = TableObj<T>::StrHash(CurPtr->KeyStr);
					CurPtr->Next = NewBucketArr[HashVal%NewArrSize];
					NewBucketArr[HashVal%NewArrSize] = CurPtr;
				}
			}

			nsBasic::GlobalMemFree(this->BucketArr, this->BucketArrSize*sizeof(HashNode *));
			this->BucketArr = NewBucketArr;
			this->BucketArrSize = NewArrSize;
			return true;
		}
	}


	TableObj<T>& operator =(const TableObj<T>&) = delete;
	void *operator new(size_t size){ return nsBasic::GlobalMemAlloc(size); }
	void operator delete(void *ptr){ nsBasic::GlobalMemFree(ptr, sizeof(TableObj<T>)); }
public:
	class iterator_base{
	public:
		HashNode *CurNode;
		HashNode **BucketArr;
		UBINT BucketArrSize;
		UBINT CurHash;

	public:
		void inc(){
			if (nullptr != this->CurNode->Next)this->CurNode = this->CurNode->Next;
			else{
				for (UBINT i = CurHash + 1; i < BucketArrSize; i++){
					if (nullptr != this->BucketArr[i]){
						this->CurNode = this->BucketArr[i];
						this->CurHash = i;
						return;
					}
				}
				this->CurHash = this->BucketArrSize;
			}
		}
		inline bool valid(){ return this->CurHash < this->BucketArrSize; }
		inline TableObj<T> *ref(){ return this->CurNode->Value; }
	};

	T Value;

	//constructors
	TableObj() :Value(){
		this->BucketArr = nullptr;
		this->BucketArrSize = 0;
		this->ElemSize = 0;
		this->RefCounter = (UBINT)-1;
	}
	static TableObj<T> *Create(){
		TableObj<T> *RetValue = new TableObj<T>();
		RetValue->RefCounter = 0;
		return RetValue;
	}

	//capacity
	inline bool empty(){ return 0 == this->ElemSize; }
	//indexing
	TableObj<T> *find(const char *Key){
		//return nullptr if Failed
		//return a existing table if the key exists
		if (0 == this->ElemSize)return nullptr;
		else{
			UBINT HashVal = TableObj<T>::StrHash(Key);
			HashNode *BucketPtr = this->BucketArr[HashVal%this->BucketArrSize];
			while (nullptr != BucketPtr){
				if (0 == strcmp(Key, BucketPtr->KeyStr))return BucketPtr->Value;
				BucketPtr = BucketPtr->Next;
			}
			return nullptr;
		}
	}
	TableObj<T> *find(const nsText::String &Key){
		//return nullptr if Failed
		//return a existing table if the key exists
		return this->find((const char *)Key.cbegin());
	}
	inline TableObj<T>& operator[](const char *Key){
		if (0 == this->ElemSize)throw new std::exception("Invalid string index.");
		else{
			UBINT HashVal = TableObj<T>::StrHash(Key);
			HashNode *BucketPtr = this->BucketArr[HashVal%this->BucketArrSize];
			while (nullptr != BucketPtr){
				if (0 == strcmp(Key, BucketPtr->KeyStr))return *BucketPtr->Value;
				BucketPtr = BucketPtr->Next;
			}
			throw new std::exception("Invalid string index.");
		}
	}
	iterator_base begin(){
		iterator_base RetValue;
		RetValue.BucketArr = this->BucketArr;
		RetValue.BucketArrSize = this->BucketArrSize;
		RetValue.CurHash = 0;
		if (RetValue.BucketArrSize > 0){
			for (UBINT i = 0; i < this->BucketArrSize; i++){
				if (nullptr != this->BucketArr[i]){
					RetValue.CurNode = this->BucketArr[i];
					RetValue.CurHash = i;
					break;
				}
			}
		}
		else{
			RetValue.CurNode = nullptr;
			RetValue.CurHash = 0;
		}
		return RetValue;
	}

	//reference counting
	inline void refer_inc(){
		if ((UBINT)-1 != this->RefCounter)this->RefCounter++;
	}
	inline void refer_dec(){
		if ((UBINT)-1 != this->RefCounter){
			if (0 == --this->RefCounter)delete this;
		}
	}

	//modifier
	void clear(){
		if (this->BucketArrSize > 0){
			for (UBINT i = 0; i < this->BucketArrSize; i++){
				HashNode *BucketPtr = this->BucketArr[i];
				while (nullptr != BucketPtr){
					HashNode *CurPtr = BucketPtr;
					BucketPtr = CurPtr->Next;
					CurPtr->Value->refer_dec();
					nsBasic::GlobalMemFree(CurPtr->KeyStr, CurPtr->KeyStrLen);
					nsBasic::GlobalMemFree(CurPtr, sizeof(HashNode));
				}
			}
			nsBasic::GlobalMemFree(this->BucketArr, this->BucketArrSize*sizeof(HashNode *));
			this->BucketArr = nullptr;
			this->BucketArrSize = 0;
			this->ElemSize = 0;
		}
	}
	TableObj<T> *insert(const char *Key, TableObj<T> *Value){
		//return nullptr if Failed
		//return a existing table if the key exists

		UBINT HashVal = TableObj<T>::StrHash(Key);
		UBINT StrLen = strlen(Key) + 1;

		if (0 != this->ElemSize){
			HashNode *BucketPtr = this->BucketArr[HashVal%this->BucketArrSize];
			while (nullptr != BucketPtr){
				if (0 == strcmp(Key, BucketPtr->KeyStr))return BucketPtr->Value;
				BucketPtr = BucketPtr->Next;
			}
		}

		HashNode *NewNode = (HashNode *)nsBasic::GlobalMemAlloc(sizeof(HashNode));
		if (nullptr == NewNode)return nullptr;
		NewNode->KeyStr = (char *)nsBasic::GlobalMemAlloc(StrLen);
		if (nullptr == NewNode->KeyStr){
			nsBasic::GlobalMemFree(NewNode, sizeof(HashNode));
			return nullptr;
		}
		NewNode->KeyStrLen = StrLen;
		memcpy(NewNode->KeyStr, Key, StrLen);
		NewNode->Value = Value;

		if (this->ElemSize == this->BucketArrSize){
			//need rehash
			UBINT NewArrSize;
			if (0 == this->BucketArrSize) NewArrSize = 4; else NewArrSize = this->BucketArrSize * 2; //very bad, need to repair this later
			if (false == Rehash(NewArrSize)){
				nsBasic::GlobalMemFree(NewNode->KeyStr, StrLen);
				nsBasic::GlobalMemFree(NewNode, sizeof(HashNode));
				return nullptr;
			}
		}

		NewNode->Next = this->BucketArr[HashVal%this->BucketArrSize];
		this->BucketArr[HashVal%this->BucketArrSize] = NewNode;
		this->ElemSize++;
		Value->refer_inc();
		return NewNode->Value;
	}
	TableObj<T> *insert(const nsText::String &Key, TableObj<T> *Value){
		return this->insert((const char *)Key.cbegin(), Value);
	}
	TableObj<T> *erase(const char *Key){
		//return a vaild pointer for success

		UBINT HashVal = TableObj<T>::StrHash(Key);

		if (0 != this->ElemSize){
			HashNode **PtrSlot = &this->BucketArr[HashVal%this->BucketArrSize];
			HashNode *BucketPtr = *PtrSlot;
			while (nullptr != BucketPtr){
				if (0 == strcmp(Key, BucketPtr->KeyStr)){
					TableObj<T> *RetValue = BucketPtr->Value;
					*PtrSlot = BucketPtr->Next;
					BucketPtr->Value->refer_dec();
					nsBasic::GlobalMemFree(BucketPtr->KeyStr, BucketPtr->KeyStrLen);
					nsBasic::GlobalMemFree(BucketPtr, sizeof(HashNode));

					this->ElemSize--;
					if (0 == this->ElemSize){
						nsBasic::GlobalMemFree(this->BucketArr, this->BucketArrSize*sizeof(HashNode *));
						this->BucketArr = nullptr;
						this->BucketArrSize = 0;
					}
					else if (this->ElemSize <= this->BucketArrSize && this->BucketArrSize > 4)Rehash(this->BucketArrSize / 2);
					return RetValue;
				}
				PtrSlot = &BucketPtr->Next;
				BucketPtr = *PtrSlot;
			}
			return nullptr;
		}
		else return nullptr;
	}

	inline ~TableObj(){
		this->clear();
	}
	inline static void Destroy(TableObj<T> *Obj){ delete Obj; }
};

class RenderData{
public:
	static enum DataUsageEnum{
		NONE,
		VERTEX_ARRAY,
		VERTEX_INDEX,
		TEXTURE_2D,
	};
	static enum DataType{
		UINT1b,
		UINT2b,
		UINT4b,
		UINT8b,
		INT1b,
		INT2b,
		INT4b,
		INT8b,
		FLOAT4b,
		FLOAT8b
	};
	static const UBINT CPU_Avail = 1;
	static const UBINT GPU_Avail = 2;

	nsMath::Matrix_Base CPUData;
	GLuint GPUDataStub;
	UBINT DataUsage;
	UBINT DataStat;

	RenderData() :CPUData(0){
		this->GPUDataStub = 0;
		this->DataUsage = RenderData::DataUsageEnum::NONE;
		this->DataStat = 0;
	}
	void GPUBind_VertexData(){
		if (this->DataUsage == RenderData::DataUsageEnum::NONE && RenderData::CPU_Avail == this->DataStat){
			GLuint GLHandle;
			glGenBuffers(1, &GLHandle);
			glBindBuffer(GL_ARRAY_BUFFER, GLHandle);
			glBufferData(GL_ARRAY_BUFFER, this->CPUData.byte_size(), this->CPUData.data(), GL_STATIC_DRAW);
			this->GPUDataStub = GLHandle;
			this->DataUsage = RenderData::DataUsageEnum::VERTEX_ARRAY;
			this->DataStat += RenderData::GPU_Avail;
		}
		else if ((RenderData::GPU_Avail & this->DataStat) > 0){
			glBindBuffer(GL_ARRAY_BUFFER, this->GPUDataStub);
		}
		else throw std::exception();
	}
	void GPUBind_VertexIndex(){
		if (this->DataUsage == RenderData::DataUsageEnum::NONE && RenderData::CPU_Avail == this->DataStat){
			GLuint GLHandle;

			glGenBuffers(1, &GLHandle);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->CPUData.byte_size(), this->CPUData.data(), GL_STATIC_DRAW);

			this->GPUDataStub = GLHandle;
			this->DataUsage = RenderData::DataUsageEnum::VERTEX_INDEX;
			this->DataStat += RenderData::GPU_Avail;
		}
		else if ((RenderData::GPU_Avail & this->DataStat) > 0){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->GPUDataStub);
		}
		else throw std::exception();
	}
	void GPUBind_Tex2D(UBINT TexID){
		if (this->DataUsage == RenderData::DataUsageEnum::NONE && RenderData::CPU_Avail == this->DataStat){
			GLuint GLHandle;

			glGenTextures(1, &GLHandle);
			glActiveTexture(GL_TEXTURE0 + TexID);
			glBindTexture(GL_TEXTURE_2D, GLHandle);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			if (3 == this->CPUData.dim_size(2)){
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->CPUData.dim_size(0), this->CPUData.dim_size(1), 0, GL_BGR, GL_UNSIGNED_BYTE, this->CPUData.data());
			}
			else if (1 == this->CPUData.dim_size(2)){
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, this->CPUData.dim_size(0), this->CPUData.dim_size(1), 0, GL_ALPHA, GL_UNSIGNED_BYTE, this->CPUData.data());
			}
			glGenerateMipmap(GL_TEXTURE_2D);
			this->GPUDataStub = GLHandle;
			this->DataUsage = RenderData::DataUsageEnum::TEXTURE_2D;
			this->DataStat += RenderData::GPU_Avail;
		}
		else if ((RenderData::GPU_Avail & this->DataStat) > 0){
			glActiveTexture(GL_TEXTURE0 + TexID);
			glBindTexture(GL_TEXTURE_2D, this->GPUDataStub);
		}
		else throw std::exception();
	}
	void SetAttrib_VertexData(UBINT ID, UBINT Type, UBINT LastDim){
		static const GLenum _DataType_GL[] = {
			GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_NONE,
			GL_BYTE, GL_SHORT, GL_INT, GL_NONE,
			GL_FLOAT, GL_DOUBLE };
		if (GL_NONE == _DataType_GL[Type])throw std::exception();
		else glVertexAttribPointer(ID, LastDim, _DataType_GL[Type], 0, 0, 0);
	}
	void Draw_VertexIndex(UBINT Type, UBINT Count){
		static const GLenum _DataType_GL[] = {
			GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_NONE,
			GL_BYTE, GL_SHORT, GL_INT, GL_NONE,
			GL_NONE, GL_NONE };
		if (GL_NONE == _DataType_GL[Type])throw std::exception();
		else glDrawElements(GL_TRIANGLES, Count, _DataType_GL[Type], 0);
	}
	inline void CPUDelete(){ this->CPUData.clear(); this->DataStat &= RenderData::GPU_Avail; }
	void GPUDelete(){
		if (this->DataUsage != RenderData::DataUsageEnum::NONE && (this->DataStat & RenderData::GPU_Avail) > 0){
			GLuint GLHandle = this->GPUDataStub;
			switch (this->DataUsage){
			case RenderData::DataUsageEnum::VERTEX_ARRAY:
			case RenderData::DataUsageEnum::VERTEX_INDEX:
				glDeleteBuffers(1, &GLHandle);
				break;
			case  RenderData::DataUsageEnum::TEXTURE_2D:
				glDeleteTextures(1, &GLHandle);
				break;
			}
			this->GPUDataStub = 0;
			this->DataUsage = RenderData::DataUsageEnum::NONE;
			this->DataStat -= RenderData::GPU_Avail;
		}
	}
	inline ~RenderData(){ GPUDelete(); }
};

#endif