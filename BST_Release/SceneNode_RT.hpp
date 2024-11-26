#ifndef LIB_SCENENODE_RT
#define LIB_SCENENODE_RT

#include "SceneNode_General.hpp"
#include "SceneNode_Atom.hpp"
#include "EmbreeHelper.hpp"

namespace nsScene{

	//class RTAtom --BEGIN--

	class RTAtom :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef RTAtom this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		float AABB[6];

		unsigned int EmbreeID;
		bool EmbreeLoaded;

	public:
		nsBasic::IBase *DepthProgram;
		nsBasic::IBase *NormalProgram;
		TableObj<RenderData> *Data;
		//implementation of interfaces

		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

		//ISceneNode
		static void ObjectQuery(nsBasic::IBase * const ThisBase, RenderList **lpRList, UBINT QueryType, QueryInfo *QueryData);

		//IRenderAtom
		static nsBasic::IBase *GetRenderProgram(nsBasic::IBase * const ThisBase, UBINT Type);
		static TableObj<RenderData> *GetRenderProgramData(nsBasic::IBase * const ThisBase, UBINT Type);
		static void Render(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Type);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const ISceneNode ISceneNode_Inst;
		static const IRenderAtom IRenderAtom_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(ISceneNode))return &this_type::ISceneNode_Inst;
			else if (IID == __typeid(IRenderAtom))return &this_type::IRenderAtom_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;

		//constructors
		RTAtom(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->Data = nullptr;
			this->EmbreeLoaded = false;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods
		void EmbreeLoad();
		void EmbreeUnload();

		void RefreshGeometry();

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~RTAtom(){
			if (this->EmbreeLoaded)EmbreeUnload();
		}
	};

	const nsBasic::IBase RTAtom::IBase_Inst = {
		RTAtom::_GetInterface,
		nullptr,
		RTAtom::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		RTAtom::Destroy
	};
	const ISceneNode RTAtom::ISceneNode_Inst = {
		nullptr,
		RTAtom::ObjectQuery,
	};
	const IRenderAtom RTAtom::IRenderAtom_Inst = {
		RTAtom::GetRenderProgram,
		RTAtom::GetRenderProgramData,
		RTAtom::Render
	};

	//class RTAtom --END--

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	//class RTAtom --BEGIN--

	void RTAtom::ObjectQuery(nsBasic::IBase * const ThisBase, RenderList **lpRList, UBINT QueryType, QueryInfo *QueryData){
		this_type *This = static_cast<this_type *>(ThisBase);

		if (QueryType == __typeid(QueryInfoExt_View)){
			QueryInfoExt_View *QueryData_View = static_cast<QueryInfoExt_View *>(QueryData->ExtInfo);
			if (false == nsMath::Intersect_View_AABB_Coarse(QueryData_View->MatProj, This->AABB, QueryData->Transform))return;
		}

		if (nullptr != lpRList){
			RenderList *NewNode = new RenderList;
			if (nullptr != NewNode){
				memcpy(NewNode->Transform, QueryData->Transform, 16 * sizeof(float));
				NewNode->Ptr = ThisBase;
				NewNode->Next = *lpRList;
				*lpRList = NewNode;
			}
		}
	}

	nsBasic::IBase *RTAtom::GetRenderProgram(nsBasic::IBase * const ThisBase, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (Type == IRenderAtom::GBUFFER)return This->NormalProgram;
		else if (Type == IRenderAtom::DEPTH_ONLY)return This->DepthProgram;
		else return nullptr;
	}
	TableObj<RenderData> *RTAtom::GetRenderProgramData(nsBasic::IBase * const ThisBase, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (nullptr != This->Data){
			TableObj<RenderData> *SubData = This->Data->find("Mat");
			if (nullptr != SubData)return SubData;
		}
		return nullptr;
	}
	void RTAtom::Render(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (nullptr != This->Data){
			TableObj<RenderData> *SubIndex;
			SubIndex = This->Data->find("Index");
			if (nullptr != SubIndex){
				TableObj<RenderData> *SubData;
				SubData = This->Data->find("Pos");
				if (nullptr != SubData){
					SubData->Value.GPUBind_VertexData();
					SubData->Value.SetAttrib_VertexData(0, RenderData::DataType::FLOAT4b, 3);
				}
				SubData = This->Data->find("Norm");
				if (nullptr != SubData){
					SubData->Value.GPUBind_VertexData();
					SubData->Value.SetAttrib_VertexData(1, RenderData::DataType::FLOAT4b, 3);
				}
				SubData = This->Data->find("TexCoord");
				if (nullptr != SubData){
					SubData->Value.GPUBind_VertexData();
					SubData->Value.SetAttrib_VertexData(2, RenderData::DataType::FLOAT4b, 2);
				}
				SubIndex->Value.GPUBind_VertexIndex();
				SubIndex->Value.Draw_VertexIndex(RenderData::DataType::UINT4b, SubIndex->Value.CPUData.size());
			}
		}
	}
	void RTAtom::RefreshGeometry(){
		if (nullptr == this->Data)memset(this->AABB, 0, 6 * sizeof(float));
		else{
			TableObj<RenderData> *SubVertex, *SubIndex;
			SubVertex = this->Data->find("Pos");
			SubIndex = this->Data->find("Index");
			if (nullptr == SubVertex || nullptr == SubIndex)memset(this->AABB, 0, 6 * sizeof(float));
			else{
				this->AABB[0] = nsMath::NumericTrait<float>::Inf_Positive;
				this->AABB[1] = nsMath::NumericTrait<float>::Inf_Positive;
				this->AABB[2] = nsMath::NumericTrait<float>::Inf_Positive;
				this->AABB[3] = nsMath::NumericTrait<float>::Inf_Negative;
				this->AABB[4] = nsMath::NumericTrait<float>::Inf_Negative;
				this->AABB[5] = nsMath::NumericTrait<float>::Inf_Negative;

				float *lpVertex = (float *)SubVertex->Value.CPUData.data();
				UINT4b *lpIndex = (UINT4b *)SubIndex->Value.CPUData.data();
				for (UBINT i = 0; i < SubIndex->Value.CPUData.size(); i++){
					UINT4b CurIndex = lpIndex[i] * 3;
					this->AABB[0] = std::min(this->AABB[0], lpVertex[CurIndex + 0]);
					this->AABB[1] = std::min(this->AABB[1], lpVertex[CurIndex + 1]);
					this->AABB[2] = std::min(this->AABB[2], lpVertex[CurIndex + 2]);
					this->AABB[3] = std::max(this->AABB[3], lpVertex[CurIndex + 0]);
					this->AABB[4] = std::max(this->AABB[4], lpVertex[CurIndex + 1]);
					this->AABB[5] = std::max(this->AABB[5], lpVertex[CurIndex + 2]);
				}
			}
		}
	}
	void RTAtom::EmbreeLoad(){
		if (nullptr != this->Data){
			TableObj<RenderData> *SubVertex, *SubIndex, *SubMat;
			SubVertex = this->Data->find("Pos");
			SubIndex = this->Data->find("Index");
			SubMat = this->Data->find("Mat");
			if (nullptr != SubVertex && nullptr != SubIndex){
				try{
					struct Vertex { float x, y, z, a; };
					struct Index { int v0, v1, v2; };

					this->EmbreeID = rtcNewTriangleMesh(Embree_Scene_Solid, RTC_GEOMETRY_STATIC, SubIndex->Value.CPUData.dim_size(0), SubVertex->Value.CPUData.dim_size(0));  EMBREE_CHECK_ERROR;

					Vertex *lpVtxBuffer = (Vertex *)rtcMapBuffer(Embree_Scene_Solid, this->EmbreeID, RTC_VERTEX_BUFFER);
					for (UBINT i = 0; i < SubVertex->Value.CPUData.dim_size(0); i++){
						float *TmpPtr = &((float *)SubVertex->Value.CPUData.data())[3 * i];
						lpVtxBuffer[i].x = TmpPtr[0]; lpVtxBuffer[i].y = TmpPtr[1]; lpVtxBuffer[i].z = TmpPtr[2];
					}
					rtcUnmapBuffer(Embree_Scene_Solid, this->EmbreeID, RTC_VERTEX_BUFFER);

					Index *lpIndexBuffer = (Index *)rtcMapBuffer(Embree_Scene_Solid, this->EmbreeID, RTC_INDEX_BUFFER);
					memcpy(lpIndexBuffer, SubIndex->Value.CPUData.data(), SubIndex->Value.CPUData.byte_size());
					rtcUnmapBuffer(Embree_Scene_Solid, this->EmbreeID, RTC_INDEX_BUFFER);

					TableObj<RenderData> *SndMtlData = SubMat->find("Prop_Snd");
					if (nullptr != SndMtlData)Embree_MaterialMap->try_emplace(this->EmbreeID, (MaterialProperty_Sound_Surface *)SndMtlData->Value.CPUData.data());

					this->EmbreeLoaded = true;
				}
				catch (...){
					throw;
				}
			}
		}
	}
	void RTAtom::EmbreeUnload(){
		if (this->EmbreeLoaded){
			rtcDeleteGeometry(Embree_Scene_Solid, this->EmbreeID); EMBREE_CHECK_ERROR;
		}
	}
	//class RTAtom --END--
}

#endif