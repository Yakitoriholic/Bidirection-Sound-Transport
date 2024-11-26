#ifndef LIB_SCENENODE_ATOM
#define LIB_SCENENODE_ATOM

#include "SceneNode_General.hpp"

namespace nsScene{

	struct IRenderAtom{
		nsBasic::IBase *(*GetRenderProgram)(nsBasic::IBase * const, UBINT);
		TableObj<RenderData> *(*GetRenderProgramData)(nsBasic::IBase * const, UBINT);
		void(*Render)(nsBasic::IBase * const, nsGUI::GLDevice *, UBINT);

		enum RenderType{
			DEPTH_ONLY,
			GBUFFER
		};
	};

	//class Cube --BEGIN--

	class Cube :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Cube this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

	public:
		float Size;
		//implementation of interfaces

		//IGeometry
		static void Render(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Type);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IGeometry IGeometry_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IGeometry))return &this_type::IGeometry_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;

		//constructors
		Cube(){ *static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst; }
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
	};

	const nsBasic::IBase Cube::IBase_Inst = {
		Cube::_GetInterface,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Cube::Destroy
	};
	const IGeometry Cube::IGeometry_Inst = {
		Cube::Render
	};

	//class Cube --END--

	//class Frustum --BEGIN--

	class Frustum :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Frustum this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

	public:
		float Data[6];
		//implementation of interfaces

		//IGeometry
		static void Render(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Type);

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const IGeometry IGeometry_Inst;

		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){
			if (IID == __typeid(IGeometry))return &this_type::IGeometry_Inst;
			else return nullptr;
		}
	public:
		static const nsBasic::IBase IBase_Inst;

		//constructors
		Frustum(){ *static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst; }
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
	};

	const nsBasic::IBase Frustum::IBase_Inst = {
		Frustum::_GetInterface,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Frustum::Destroy
	};
	const IGeometry Frustum::IGeometry_Inst = {
		Frustum::Render
	};

	//class Frustum --END--

	//class Sphere --BEGIN--

	class Sphere :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Sphere this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

	public:
		nsBasic::IBase *DepthProgram;
		nsBasic::IBase *NormalProgram;
		TableObj<RenderData> *Data;
		float Radius;
		bool CastShadow;
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
		Sphere(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->Data = nullptr;
			this->Radius = 1.0f;
			this->CastShadow = true;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~Sphere(){}
	};

	const nsBasic::IBase Sphere::IBase_Inst = {
		Sphere::_GetInterface,
		nullptr,
		Sphere::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Sphere::Destroy
	};
	const ISceneNode Sphere::ISceneNode_Inst = {
		nullptr,
		Sphere::ObjectQuery,
	};
	const IRenderAtom Sphere::IRenderAtom_Inst = {
		Sphere::GetRenderProgram,
		Sphere::GetRenderProgramData,
		Sphere::Render
	};

	//class Sphere --END--

	//class SimpleAtom --BEGIN--

	class SimpleAtom :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef SimpleAtom this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		float AABB[6];
	public:
		nsBasic::IBase *DepthProgram;
		nsBasic::IBase *NormalProgram;
		TableObj<RenderData> *Data;
		bool CastShadow;
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
		SimpleAtom(){
			*static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst;
			this->Data = nullptr;
			this->CastShadow = true;
		}
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		void RefreshGeometry();

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~SimpleAtom(){}
	};

	const nsBasic::IBase SimpleAtom::IBase_Inst = {
		SimpleAtom::_GetInterface,
		nullptr,
		SimpleAtom::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		SimpleAtom::Destroy
	};
	const ISceneNode SimpleAtom::ISceneNode_Inst = {
		nullptr,
		SimpleAtom::ObjectQuery,
	};
	const IRenderAtom SimpleAtom::IRenderAtom_Inst = {
		SimpleAtom::GetRenderProgram,
		SimpleAtom::GetRenderProgramData,
		SimpleAtom::Render
	};

	//class SimpleAtom --END--

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	//class Cube --BEGIN--

	void Cube::Render(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		GLfloat Point[24];
		const GLuint Index[36] = {
			0, 1, 2,
			0, 2, 3,
			4, 0, 3,
			4, 3, 7,
			5, 4, 7,
			5, 7, 6,
			1, 5, 6,
			1, 6, 2,
			3, 2, 6,
			3, 6, 7,
			4, 5, 1,
			4, 1, 0
		};

		Point[0] = -This->Size; Point[1] = -This->Size; Point[2] = This->Size;
		Point[3] = This->Size; Point[4] = -This->Size; Point[5] = This->Size;
		Point[6] = This->Size; Point[7] = -This->Size; Point[8] = -This->Size;
		Point[9] = -This->Size; Point[10] = -This->Size; Point[11] = -This->Size;
		Point[12] = -This->Size; Point[13] = This->Size; Point[14] = This->Size;
		Point[15] = This->Size; Point[16] = This->Size; Point[17] = This->Size;
		Point[18] = This->Size; Point[19] = This->Size; Point[20] = -This->Size;
		Point[21] = -This->Size; Point[22] = This->Size; Point[23] = -This->Size;

		GLuint VtxHandle = lpDevice->CreateArrayBuffer(24 * sizeof(GLfloat), GL_STATIC_DRAW, Point);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

		GLuint IndexHandle = lpDevice->CreateElemArrayBuffer(36 * sizeof(GLuint), GL_STATIC_DRAW, Index);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		lpDevice->DestroyElemArrayBuffer(IndexHandle, 36 * sizeof(GLuint), GL_STATIC_DRAW);
		lpDevice->DestroyArrayBuffer(VtxHandle, 24 * sizeof(GLfloat), GL_STATIC_DRAW);
	}

	//class Cube --END--

	//class Frustum --BEGIN--

	void Frustum::Render(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		GLfloat Point[24];
		const GLuint Index[36] = {
			0, 1, 2,
			0, 2, 3,
			4, 0, 3,
			4, 3, 7,
			5, 4, 7,
			5, 7, 6,
			1, 5, 6,
			1, 6, 2,
			3, 2, 6,
			3, 6, 7,
			4, 5, 1,
			4, 1, 0
		};

		Point[0] = This->Data[4]; Point[1] = -This->Data[0] * This->Data[4]; Point[2] = This->Data[1] * This->Data[4];
		Point[3] = This->Data[4]; Point[4] = -This->Data[2] * This->Data[4]; Point[5] = Point[2];
		Point[6] = This->Data[4]; Point[7] = Point[4]; Point[8] = This->Data[3] * This->Data[4];
		Point[9] = This->Data[4]; Point[10] = Point[1]; Point[11] = Point[8];
		Point[12] = This->Data[5]; Point[13] = -This->Data[0] * This->Data[5]; Point[14] = This->Data[1] * This->Data[5];
		Point[15] = This->Data[5]; Point[16] = -This->Data[2] * This->Data[5]; Point[17] = Point[14];
		Point[18] = This->Data[5]; Point[19] = Point[16]; Point[20] = This->Data[3] * This->Data[5];
		Point[22] = Point[13]; Point[21] = This->Data[5]; Point[23] = Point[20];

		GLuint VtxHandle = lpDevice->CreateArrayBuffer(24 * sizeof(GLfloat), GL_STATIC_DRAW, Point);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

		GLuint IndexHandle = lpDevice->CreateElemArrayBuffer(36 * sizeof(GLuint), GL_STATIC_DRAW, Index);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDisable(GL_CULL_FACE);

		lpDevice->DestroyElemArrayBuffer(IndexHandle, 36 * sizeof(GLuint), GL_STATIC_DRAW);
		lpDevice->DestroyArrayBuffer(VtxHandle, 24 * sizeof(GLfloat), GL_STATIC_DRAW);
	}

	//class Frustum --END--

	//class SimpleAtom --BEGIN--

	void SimpleAtom::ObjectQuery(nsBasic::IBase * const ThisBase, RenderList **lpRList, UBINT QueryType, QueryInfo *QueryData){
		this_type *This = static_cast<this_type *>(ThisBase);

		if (QueryInfo::OBJECT_SHADOW == QueryData->QueryObjType && false == This->CastShadow)return;

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

	nsBasic::IBase *SimpleAtom::GetRenderProgram(nsBasic::IBase * const ThisBase, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (Type == IRenderAtom::GBUFFER)return This->NormalProgram;
		else if (Type == IRenderAtom::DEPTH_ONLY)return This->DepthProgram;
		else return nullptr;
	}
	TableObj<RenderData> *SimpleAtom::GetRenderProgramData(nsBasic::IBase * const ThisBase, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (nullptr != This->Data){
			TableObj<RenderData> *SubData = This->Data->find("Mat");
			if (nullptr != SubData)return SubData;
		}
		return nullptr;
	}
	void SimpleAtom::Render(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Type){
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
	void SimpleAtom::RefreshGeometry(){
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

	//class SimpleAtom --END--

	//class Sphere --BEGIN--

	void Sphere::ObjectQuery(nsBasic::IBase * const ThisBase, RenderList **lpRList, UBINT QueryType, QueryInfo *QueryData){
		this_type *This = static_cast<this_type *>(ThisBase);

		if (QueryInfo::OBJECT_SHADOW == QueryData->QueryObjType && false == This->CastShadow)return;
		if (QueryType == __typeid(QueryInfoExt_View)){
			QueryInfoExt_View *QueryData_View = static_cast<QueryInfoExt_View *>(QueryData->ExtInfo);
			float AABB[6] = { -This->Radius, -This->Radius, -This->Radius, This->Radius, This->Radius, This->Radius };
			if (false == nsMath::Intersect_View_AABB_Coarse(QueryData_View->MatProj, AABB, QueryData->Transform))return;
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

	nsBasic::IBase *Sphere::GetRenderProgram(nsBasic::IBase * const ThisBase, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (Type == IRenderAtom::GBUFFER)return This->NormalProgram;
		else if (Type == IRenderAtom::DEPTH_ONLY)return This->DepthProgram;
		else return nullptr;
	}
	TableObj<RenderData> *Sphere::GetRenderProgramData(nsBasic::IBase * const ThisBase, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (nullptr != This->Data)return This->Data;
		else return nullptr;
	}
	void Sphere::Render(nsBasic::IBase * const ThisBase, nsGUI::GLDevice *lpDevice, UBINT Type){
		this_type *This = static_cast<this_type *>(ThisBase);
		if (nullptr != This->Data){
			GLfloat Point[24];
			const GLuint Index[36] = {
				0, 1, 2,
				0, 2, 3,
				4, 0, 3,
				4, 3, 7,
				5, 4, 7,
				5, 7, 6,
				1, 5, 6,
				1, 6, 2,
				3, 2, 6,
				3, 6, 7,
				4, 5, 1,
				4, 1, 0
			};

			Point[0] = -This->Radius; Point[1] = -This->Radius; Point[2] = This->Radius;
			Point[3] = This->Radius; Point[4] = -This->Radius; Point[5] = This->Radius;
			Point[6] = This->Radius; Point[7] = -This->Radius; Point[8] = -This->Radius;
			Point[9] = -This->Radius; Point[10] = -This->Radius; Point[11] = -This->Radius;
			Point[12] = -This->Radius; Point[13] = This->Radius; Point[14] = This->Radius;
			Point[15] = This->Radius; Point[16] = This->Radius; Point[17] = This->Radius;
			Point[18] = This->Radius; Point[19] = This->Radius; Point[20] = -This->Radius;
			Point[21] = -This->Radius; Point[22] = This->Radius; Point[23] = -This->Radius;

			GLuint VtxHandle = lpDevice->CreateArrayBuffer(24 * sizeof(GLfloat), GL_STATIC_DRAW, Point);
			glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

			GLuint IndexHandle = lpDevice->CreateElemArrayBuffer(36 * sizeof(GLuint), GL_STATIC_DRAW, Index);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glDisable(GL_CULL_FACE);

			lpDevice->DestroyElemArrayBuffer(IndexHandle, 36 * sizeof(GLuint), GL_STATIC_DRAW);
			lpDevice->DestroyArrayBuffer(VtxHandle, 24 * sizeof(GLfloat), GL_STATIC_DRAW);
		}
	}

	//class Sphere --END--

	//class Buffer_AtomList --BEGIN--

	class Buffer_AtomList :public nsBasic::IBase{ //public inherit from IBase
	private:
		typedef Buffer_AtomList this_type;

		//using new and delete operator directly is prohibited
		inline void *operator new(size_t size){ return ::operator new(size); }
		inline void operator delete(void *ptr){ ::operator delete(ptr); };

		struct ListNode{
			nsBasic::IBase *Material;
			TableObj<RenderData> *Data;
			RenderList *NodeValue;
		};
		nsContainer::Vector<ListNode> NodeVector;
	public:
		//implementation of interfaces
		//IBase
		static bool on_Link_Twrd(nsBasic::IBase * const ThisBase, IBase * const Src, void *Data){ return true; }

	protected:
		//implementation of GetHandler. declared as protected so all classes derived from this class can see this function.
		static const void *_GetInterface(nsBasic::IBase * const ThisBase, UBINT IID){ return nullptr; }
	public:
		static const nsBasic::IBase IBase_Inst;

		//constructors
		Buffer_AtomList() : NodeVector(){ *static_cast<nsBasic::IBase * const>(this) = this_type::IBase_Inst; }
		static this_type * const Create(){ //equivalent to the new operator
			this_type *RetValue = new this_type();
			RetValue->RefCounter = 0; //enable the reference counter
			return RetValue;
		}

		//public methods
		void Push(nsBasic::IBase *Material, TableObj<RenderData> *Data, RenderList *Object){
			ListNode NewNode = { Material, Data, Object };
			this->NodeVector.push_back(NewNode);
		}
		bool Pop(nsBasic::IBase **Material, TableObj<RenderData> **Data, RenderList **Object){
			if (!this->NodeVector.empty()){
				*Material = this->NodeVector.back().Material;
				*Data = this->NodeVector.back().Data;
				*Object = this->NodeVector.back().NodeValue;
				this->NodeVector.pop_back();
				return true;
			}
			else return false;
		}
		void Sort(){
			std::sort(this->NodeVector.begin(), this->NodeVector.end(), [](ListNode i, ListNode j){
				if (i.Material == j.Material)return i.Data < j.Data;
				else return i.Material < j.Material;
			});
		}

		//destructors
		static void Destroy(nsBasic::IBase * const ThisBase){  //equivalent to the delete operator
			this_type *This = static_cast<this_type *>(ThisBase);
			delete This;
		}
		~Buffer_AtomList(){};
	};

	const nsBasic::IBase Buffer_AtomList::IBase_Inst = {
		Buffer_AtomList::_GetInterface,
		nullptr,
		Buffer_AtomList::on_Link_Twrd,
		nullptr,
		nullptr,
		(UBINT)-1, //reference counter is disabled by default
		0,
		Buffer_AtomList::Destroy
	};

	//class Buffer_AtomList --END--
}

#endif