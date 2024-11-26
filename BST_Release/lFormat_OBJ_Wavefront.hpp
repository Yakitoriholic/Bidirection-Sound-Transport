#include "lMath.hpp"
#include "lFile.hpp"
#include "lText_Process.hpp"

#include "RenderCore.hpp"
#include "SoundMaterial.hpp"

void LoadObjFile(TableObj<RenderData> *ObjRoot, TableObj<RenderData> *MtlRoot, nsBasic::Stream_R<unsigned char> *lpStream);

/*-------------------------------- IMPLEMENTATION --------------------------------*/

void LoadMtlFile(TableObj<RenderData> *Parent, nsBasic::Stream_R<unsigned char> *lpStream){
	nsFile::DecoderStream Decoder(lpStream, nsCharCoding::UTF8);
	nsText::Lexer MyLexer(&Decoder);
	nsText::String_Sys TmpPath;
	nsText::String Symbol;

	nsScene::MaterialProperty CurMtlProp;
	MaterialProperty_Sound_Surface CurMtlProp_Sound;

	TableObj<RenderData> *CurMtl = nullptr;

	auto GetUnalignedBMP=[&](void *Dest, void *Src, UBINT Width, UBINT Height, UBINT Granularity){ //BMP alignment repack
		char *DestPos = (char *)Dest, *SrcPos = (char *)Src;
		for (UBINT i = 0; i < Height; i++){
			memcpy(DestPos, SrcPos, Width * Granularity);
			DestPos += Width * Granularity;
			SrcPos += ((Width + 1) * Granularity) & (-4);
		}
	};

	do{
		while (nsText::IsBlankChar(MyLexer.LastChar) || nsText::IsRetChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
		if ((UBINT)'#' == MyLexer.LastChar)
		while ((!nsText::IsRetChar(MyLexer.LastChar)) && (UINT4b)-1 != MyLexer.LastChar)MyLexer.LoadNextChar(); //line skip
		else{
			MyLexer.ReadWord(&Symbol);
			if ("Kd" == Symbol){
				while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
				MyLexer.ReadFloat(&CurMtlProp.Diffuse[0]);
				while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
				MyLexer.ReadFloat(&CurMtlProp.Diffuse[1]);
				while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
				MyLexer.ReadFloat(&CurMtlProp.Diffuse[2]);
			}
			else if ("Ks" == Symbol){
				while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
				MyLexer.ReadFloat(&CurMtlProp.Specular[0]);
				while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
				MyLexer.ReadFloat(&CurMtlProp.Specular[1]);
				while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
				MyLexer.ReadFloat(&CurMtlProp.Specular[2]);
			}
			else if ("Ns" == Symbol){
				while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
				MyLexer.ReadFloat(&CurMtlProp.Glossiness);
				if (CurMtlProp.Glossiness < 0.0f)CurMtlProp.Glossiness = 0.0f;
				if (CurMtlProp.Glossiness > 1000.0f)CurMtlProp.Glossiness = 1000.0f;
			}
			else if ("d" == Symbol){
				while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
				MyLexer.ReadFloat(&CurMtlProp.Opacity);
				if (CurMtlProp.Opacity < 0.0f)CurMtlProp.Glossiness = 0.0f;
				if (CurMtlProp.Opacity > 1.0f)CurMtlProp.Glossiness = 1.0f;
			}
			else if ("Kd_snd" == Symbol){
				for (UBINT i = 0; i < FloatVec_SIMD::Width; i++){
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&CurMtlProp_Sound.Diffuse[i]);
				}
			}
			else if ("Ks_snd" == Symbol){
				for (UBINT i = 0; i < FloatVec_SIMD::Width; i++){
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&CurMtlProp_Sound.Specular[i]);
				}
			}
			else if ("map_Kd" == Symbol){
				if (CurMtl != nullptr){
					MyLexer.ReadFilePath(&TmpPath);

					if (0 != TmpPath[0]){
						FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
						FIBITMAP *dib;

						fif = FreeImage_GetFileTypeU(TmpPath.cbegin());
						if (fif == FIF_UNKNOWN)fif = FreeImage_GetFIFFromFilenameU(TmpPath.cbegin());
						if (fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading(fif)){
							TableObj<RenderData> *NewMtl_Diffuse = TableObj<RenderData>::Create();

							dib = FreeImage_LoadU(fif, TmpPath.cbegin());
							dib = FreeImage_ConvertTo24Bits(dib);

							UBINT ImgWidth = FreeImage_GetWidth(dib);
							UBINT ImgHeight = FreeImage_GetHeight(dib);

							nsMath::Matrix<unsigned char> TexMat(ImgWidth, ImgHeight, 3);
							GetUnalignedBMP(TexMat.data(), FreeImage_GetBits(dib), ImgWidth, ImgHeight, 3);
							NewMtl_Diffuse->Value.CPUData = std::move(TexMat);
							NewMtl_Diffuse->Value.DataStat = RenderData::CPU_Avail;
							FreeImage_Unload(dib);

							CurMtlProp.Status |= nsScene::MaterialProperty::DIFFUSE_MAP_EXIST;

							CurMtl->insert("Map_Diff", NewMtl_Diffuse);
						}
					}
				}
			}
			else if ("map_Ns" == Symbol){
				if (CurMtl != nullptr){
					MyLexer.ReadFilePath(&TmpPath);

					if (0 != TmpPath[0]){
						FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
						FIBITMAP *dib;

						fif = FreeImage_GetFileTypeU(TmpPath.cbegin());
						if (fif == FIF_UNKNOWN)fif = FreeImage_GetFIFFromFilenameU(TmpPath.cbegin());
						if (fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading(fif)){
							TableObj<RenderData> *NewMtl_Spec_Exponent = TableObj<RenderData>::Create();

							dib = FreeImage_LoadU(fif, TmpPath.cbegin());
							dib = FreeImage_ConvertTo8Bits(dib);

							UBINT ImgWidth = FreeImage_GetWidth(dib);
							UBINT ImgHeight = FreeImage_GetHeight(dib);

							nsMath::Matrix<unsigned char> TexMat(ImgWidth, ImgHeight, 1);
							GetUnalignedBMP(TexMat.data(), FreeImage_GetBits(dib), ImgWidth, ImgHeight, 1);
							NewMtl_Spec_Exponent->Value.CPUData = std::move(TexMat);
							NewMtl_Spec_Exponent->Value.DataStat = RenderData::CPU_Avail;
							FreeImage_Unload(dib);

							CurMtlProp.Status |= nsScene::MaterialProperty::DIFFUSE_MAP_EXIST;

							CurMtl->insert("Map_Spec_Exp", NewMtl_Spec_Exponent);
						}
					}
				}
			}
			else if ("map_d" == Symbol){
				if (CurMtl != nullptr){
					MyLexer.ReadFilePath(&TmpPath);

					if (0 != TmpPath[0]){
						FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
						FIBITMAP *dib;

						fif = FreeImage_GetFileTypeU(TmpPath.cbegin());
						if (fif == FIF_UNKNOWN)fif = FreeImage_GetFIFFromFilenameU(TmpPath.cbegin());
						if (fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading(fif)){
							TableObj<RenderData> *NewMtl_Opacity = TableObj<RenderData>::Create();

							dib = FreeImage_LoadU(fif, TmpPath.cbegin());
							dib = FreeImage_ConvertTo8Bits(dib);

							UBINT ImgWidth = FreeImage_GetWidth(dib);
							UBINT ImgHeight = FreeImage_GetHeight(dib);

							nsMath::Matrix<unsigned char> TexMat(ImgWidth, ImgHeight, 1);
							GetUnalignedBMP(TexMat.data(), FreeImage_GetBits(dib), ImgWidth, ImgHeight, 1);
							NewMtl_Opacity->Value.CPUData = std::move(TexMat);
							NewMtl_Opacity->Value.DataStat = RenderData::CPU_Avail;
							FreeImage_Unload(dib);

							CurMtlProp.Status |= nsScene::MaterialProperty::OPACITY_MAP_EXIST;

							CurMtl->insert("Map_Opacity", NewMtl_Opacity);
						}
					}
				}
			}
			else if ("map_bump" == Symbol || "bump" == Symbol){
				if (CurMtl != nullptr){
					MyLexer.ReadFilePath(&TmpPath);

					if (0 != TmpPath[0]){
						FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
						FIBITMAP *dib;

						fif = FreeImage_GetFileTypeU(TmpPath.cbegin());
						if (fif == FIF_UNKNOWN)fif = FreeImage_GetFIFFromFilenameU(TmpPath.cbegin());
						if (fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading(fif)){
							TableObj<RenderData> *NewMtl_Bump = TableObj<RenderData>::Create();

							dib = FreeImage_LoadU(fif, TmpPath.cbegin());
							if (8 == FreeImage_GetBPP(dib)){
								UBINT ImgWidth = FreeImage_GetWidth(dib);
								UBINT ImgHeight = FreeImage_GetHeight(dib);

								nsMath::Matrix<unsigned char> TexMat(ImgWidth, ImgHeight, 1);
								GetUnalignedBMP(TexMat.data(), FreeImage_GetBits(dib), ImgWidth, ImgHeight, 1);
								NewMtl_Bump->Value.CPUData = std::move(TexMat);
								NewMtl_Bump->Value.DataStat = RenderData::CPU_Avail;
								CurMtlProp.Status |= nsScene::MaterialProperty::NORMAL_MAP_EXIST;
								CurMtlProp.Status |= nsScene::MaterialProperty::NORMAL_MAP_DEPTH;
							}
							else{
								dib = FreeImage_ConvertTo24Bits(dib);

								UBINT ImgWidth = FreeImage_GetWidth(dib);
								UBINT ImgHeight = FreeImage_GetHeight(dib);

								nsMath::Matrix<unsigned char> TexMat(ImgWidth, ImgHeight, 3);
								GetUnalignedBMP(TexMat.data(), FreeImage_GetBits(dib), ImgWidth, ImgHeight, 3);
								NewMtl_Bump->Value.CPUData = std::move(TexMat);
								NewMtl_Bump->Value.DataStat = RenderData::CPU_Avail;
								CurMtlProp.Status |= nsScene::MaterialProperty::NORMAL_MAP_EXIST;
							}
							NewMtl_Bump->Value.DataStat = RenderData::CPU_Avail;
							FreeImage_Unload(dib);

							CurMtl->insert("Map_Bump", NewMtl_Bump);
						}
					}
				}
			}
			else if ("newmtl" == Symbol){
				if (nullptr != CurMtl){
					nsMath::Matrix<nsScene::MaterialProperty> PropMat(1);
					PropMat(0) = CurMtlProp;
					nsMath::Matrix<MaterialProperty_Sound_Surface> PropMat_Snd(1);
					PropMat_Snd(0) = CurMtlProp_Sound;

					TableObj<RenderData> *NewMtl_Prop = TableObj<RenderData>::Create();
					NewMtl_Prop->Value.CPUData = std::move(PropMat);
					NewMtl_Prop->Value.DataStat = RenderData::CPU_Avail;
					CurMtl->insert("Prop", NewMtl_Prop);

					TableObj<RenderData> *NewMtl_Prop_Snd = TableObj<RenderData>::Create();
					NewMtl_Prop_Snd->Value.CPUData = std::move(PropMat_Snd);
					NewMtl_Prop_Snd->Value.DataStat = RenderData::CPU_Avail;
					CurMtl->insert("Prop_Snd", NewMtl_Prop_Snd);
				}

				MyLexer.ReadWord(&Symbol);
				TableObj<RenderData> *NewMtl = TableObj<RenderData>::Create();
				if (NewMtl != Parent->insert(Symbol, NewMtl))TableObj<RenderData>::Destroy(NewMtl);
				else{
					CurMtl = NewMtl;

					//set the default material property
					CurMtlProp.Status = 0;
					CurMtlProp.Diffuse[0] = 1.0f; CurMtlProp.Diffuse[1] = 1.0f; CurMtlProp.Diffuse[2] = 1.0f;
					CurMtlProp.Specular[0] = 0.0f; CurMtlProp.Specular[1] = 0.0f; CurMtlProp.Specular[2] = 0.0f;
					CurMtlProp.Opacity = 1.0f; CurMtlProp.Glossiness = 0.0f;

					for (UBINT i = 0; i < FloatVec_SIMD::Width; i++)CurMtlProp_Sound.Diffuse[i] = 0.9f;
					for (UBINT i = 0; i < FloatVec_SIMD::Width; i++)CurMtlProp_Sound.Specular[i] = 0.0f;
				}
			}
			while ((!nsText::IsRetChar(MyLexer.LastChar)) && (UINT4b)-1 != MyLexer.LastChar)MyLexer.LoadNextChar(); //line skip
		}
	} while ((UINT4b)-1 != MyLexer.LastChar);

	if (nullptr != CurMtl){
		nsMath::Matrix<nsScene::MaterialProperty> PropMat(1);
		PropMat(0) = CurMtlProp;
		nsMath::Matrix<MaterialProperty_Sound_Surface> PropMat_Snd(1);
		PropMat_Snd(0) = CurMtlProp_Sound;

		TableObj<RenderData> *NewMtl_Prop = TableObj<RenderData>::Create();
		NewMtl_Prop->Value.CPUData = std::move(PropMat);
		NewMtl_Prop->Value.DataStat = RenderData::CPU_Avail;
		CurMtl->insert("Prop", NewMtl_Prop);

		TableObj<RenderData> *NewMtl_Prop_Snd = TableObj<RenderData>::Create();
		NewMtl_Prop_Snd->Value.CPUData = std::move(PropMat_Snd);
		NewMtl_Prop_Snd->Value.DataStat = RenderData::CPU_Avail;
		CurMtl->insert("Prop_Snd", NewMtl_Prop_Snd);
	}
}
void LoadObjFile(TableObj<RenderData> *ObjRoot, TableObj<RenderData> *MtlRoot, nsBasic::Stream_R<unsigned char> *lpStream){
	nsFile::DecoderStream Decoder(lpStream, nsCharCoding::UTF8);
	nsText::Lexer MyLexer(&Decoder);
	nsText::String_Sys TmpPath;
	nsText::String Symbol, MatSymbol;

	struct IndexHash{
		public:
			size_t operator()(const nsMath::TinyVector<UINT4b, 3> &rhs) const{
				//BKDRHash from NOCOW
				size_t seed = 131, hash = 0;
				for (UBINT i = 0; i < 3; i++)hash = hash * seed + rhs[i];
				return hash;
			}
	};

	nsContainer::Vector<nsMath::TinyVector<float, 3>> VP, VN;
	nsContainer::Vector<nsMath::TinyVector<float, 2>> VT;

	nsContainer::Vector<nsMath::TinyVector<float, 3>> CurVP, CurVN;
	nsContainer::Vector<nsMath::TinyVector<float, 2>> CurVT;

	TableObj<RenderData> *CurObj = nullptr;
	TableObj<nsContainer::Vector<nsMath::TinyVector<UINT4b, 3>>> IndexSet, *lpCurIndex = nullptr;

	nsContainer::HashMap<nsMath::TinyVector<UINT4b, 3>, UINT4b, IndexHash> IndexMap;
	UBINT IndexCount = 0;
	
	auto OutputNewObj = [&](){
		if (!IndexSet.empty()){
			TableObj<RenderData> *NewObj_Pos, *NewObj_Norm, *NewObj_TexCoord;

			nsMath::Matrix<float> PosMat(CurVP.size(), 3);
			memcpy(PosMat.data(), CurVP.begin(), CurVP.size()*sizeof(nsMath::TinyVector<float, 3>));
			NewObj_Pos = TableObj<RenderData>::Create();
			NewObj_Pos->Value.CPUData = std::move(PosMat);
			NewObj_Pos->Value.DataStat = RenderData::CPU_Avail;
			
			nsMath::Matrix<float> NormMat(CurVP.size(), 3);
			if (CurVN.empty())memset(NormMat.data(), 0, CurVP.size()*sizeof(nsMath::TinyVector<float, 3>));
			else memcpy(NormMat.data(), CurVN.begin(), CurVP.size()*sizeof(nsMath::TinyVector<float, 3>));
			NewObj_Norm = TableObj<RenderData>::Create();
			NewObj_Norm->Value.CPUData = std::move(NormMat);
			NewObj_Norm->Value.DataStat = RenderData::CPU_Avail;

			nsMath::Matrix<float> TexCoordMat(CurVP.size(), 2);
			if (CurVT.empty())memset(TexCoordMat.data(), 0, CurVP.size()*sizeof(nsMath::TinyVector<float, 2>));
			else memcpy(TexCoordMat.data(), CurVT.begin(), CurVP.size()*sizeof(nsMath::TinyVector<float, 2>));
			NewObj_TexCoord = TableObj<RenderData>::Create();
			NewObj_TexCoord->Value.CPUData = std::move(TexCoordMat);
			NewObj_TexCoord->Value.DataStat = RenderData::CPU_Avail;

			for (auto _It = IndexSet.begin(); _It.valid(); _It.inc()){
				TableObj<RenderData> *NewAtom = TableObj<RenderData>::Create();

				if (CurVN.empty()){
					//calculate the normal
					for (UBINT i = 0; i < _It.CurNode->Value->Value.size(); i++){
						nsMath::TinyVector<UINT4b, 3> CurIndex = _It.CurNode->Value->Value[i];
						nsMath::TinyVector<float, 3> Vtx0 = CurVP[CurIndex[0]], Vtx1 = CurVP[CurIndex[1]], Vtx2 = CurVP[CurIndex[2]];
						nsMath::sub<3>(Vtx1.Data, Vtx1.Data, Vtx0.Data);
						nsMath::sub<3>(Vtx2.Data, Vtx2.Data, Vtx0.Data);
						nsMath::cross3(Vtx0.Data, Vtx1.Data, Vtx2.Data);
						nsMath::normalize<3>(Vtx0.Data, Vtx0.Data);

						nsMath::TinyVector<float, 3> *lpNorm = (nsMath::TinyVector<float, 3> *)NewObj_Norm->Value.CPUData.data();
						nsMath::add<3>(lpNorm[CurIndex[0]].Data, lpNorm[CurIndex[0]].Data, Vtx0.Data);
						nsMath::add<3>(lpNorm[CurIndex[1]].Data, lpNorm[CurIndex[1]].Data, Vtx0.Data);
						nsMath::add<3>(lpNorm[CurIndex[2]].Data, lpNorm[CurIndex[2]].Data, Vtx0.Data);
					}
				}

				NewAtom->insert("Pos", NewObj_Pos);
				NewAtom->insert("Norm", NewObj_Norm);
				NewAtom->insert("TexCoord", NewObj_TexCoord);

				nsMath::Matrix<UINT4b> IndexMat(_It.CurNode->Value->Value.size(), 3);
				memcpy(IndexMat.data(), _It.CurNode->Value->Value.begin(), _It.CurNode->Value->Value.size()*sizeof(nsMath::TinyVector<UINT4b, 3>));
				TableObj<RenderData> *NewObj_Index = TableObj<RenderData>::Create();
				NewObj_Index->Value.CPUData = std::move(IndexMat);
				NewObj_Index->Value.DataStat = RenderData::CPU_Avail;
				NewAtom->insert("Index", NewObj_Index);

				NewAtom->insert("Mat", MtlRoot->find(_It.CurNode->KeyStr));

				CurObj->insert(_It.CurNode->KeyStr, NewAtom);
			}

			if (CurVN.empty()){
				nsMath::TinyVector<float, 3> *lpNorm = (nsMath::TinyVector<float, 3> *)NewObj_Norm->Value.CPUData.data();
				for (UBINT i = 0; i < CurVP.size(); i++)nsMath::normalize<3>(lpNorm[i].Data, lpNorm[i].Data);
			}

			IndexSet.clear();
		}
	};

	do{
		while (nsText::IsBlankChar(MyLexer.LastChar) || nsText::IsRetChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
		if ((UBINT)'#' == MyLexer.LastChar)
		while ((!nsText::IsRetChar(MyLexer.LastChar)) && (UINT4b)-1 != MyLexer.LastChar)MyLexer.LoadNextChar(); //line skip
		else{
			MyLexer.ReadWord(&Symbol);
			//parse command
			if ('v' == Symbol[0]){
				if ('\0' == Symbol[1]){
					nsMath::TinyVector<float, 3> TmpVal;
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&TmpVal[0]);
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&TmpVal[1]);
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&TmpVal[2]);
					VP.push_back(TmpVal);
				}
				else if ('n' == Symbol[1] && '\0' == Symbol[2]){
					nsMath::TinyVector<float, 3> TmpVal;
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&TmpVal[0]);
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&TmpVal[1]);
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&TmpVal[2]);
					VN.push_back(TmpVal);
				}
				else if ('t' == Symbol[1] && '\0' == Symbol[2]){
					nsMath::TinyVector<float, 2> TmpVal;
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&TmpVal[0]);
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					MyLexer.ReadFloat(&TmpVal[1]);
					VT.push_back(TmpVal);
				}
			}
			else if ("g" == Symbol){
				if (nullptr != CurObj)OutputNewObj();
				MyLexer.ReadWord(&Symbol);
				TableObj<RenderData> *NewObj = TableObj<RenderData>::Create();
				if (NewObj != ObjRoot->insert(Symbol, NewObj))TableObj<RenderData>::Destroy(NewObj);
				else{
					//success
					MatSymbol.clear();
					CurObj = NewObj;

					CurVP.clear();
					CurVN.clear();
					CurVT.clear();
					IndexCount = 0;
					lpCurIndex = nullptr;
				}
			}
			else if ("mtllib" == Symbol){
				MyLexer.ReadFilePath(&TmpPath);
				nsFile::ReadFileStream RFS(TmpPath.cbegin());
				nsFile::ReadStreamBuffer RSB(&RFS);
				LoadMtlFile(MtlRoot, &RSB);
			}
			else if ("usemtl" == Symbol){
				nsText::String TmpMatSymbol;
				MyLexer.ReadWord(&TmpMatSymbol);
				TableObj<RenderData> *CurMtl = MtlRoot->find(TmpMatSymbol);
				if (nullptr != CurMtl){
					//valid material
					MatSymbol = TmpMatSymbol;
					lpCurIndex = IndexSet.find(MatSymbol);
				}
			}
			else if ("f" == Symbol){
				//create new faces
				nsMath::TinyVector<UINT4b, 3> NewIndex;
				UINT4b QuadIndex[2] = { (UINT4b)-1, (UINT4b)-1 };

				if (nullptr == lpCurIndex){
					lpCurIndex = TableObj<nsContainer::Vector<nsMath::TinyVector<UINT4b, 3>>>::Create();
					IndexSet.insert(MatSymbol, lpCurIndex);
				}

				while (true){
					while (nsText::IsBlankChar(MyLexer.LastChar))MyLexer.LoadNextChar(); //blank skip
					if (nsText::IsRetChar(MyLexer.LastChar) || (UINT4b)-1 == MyLexer.LastChar)break; //leave if there is a return or eof

					//new point index
					UBINT TmpInt;
					if ((UBINT)'-' == MyLexer.LastChar){
						MyLexer.LoadNextChar();
						MyLexer.ReadUInt(&TmpInt);
						if (VP.size() < TmpInt)throw std::exception("Invalid relative index.");
						NewIndex[0] = (UINT4b)(VP.size() - TmpInt);
					}
					else{
						MyLexer.ReadUInt(&TmpInt);
						NewIndex[0] = (UINT4b)(TmpInt - 1);
					}
					if ((UBINT)'/' == MyLexer.LastChar){
						MyLexer.LoadNextChar();
						if ((UBINT)'/' == MyLexer.LastChar)NewIndex[1] = (UINT4b)-1;
						else{
							if ((UBINT)'-' == MyLexer.LastChar){
								MyLexer.LoadNextChar();
								MyLexer.ReadUInt(&TmpInt);
								if (VN.size() < TmpInt)throw std::exception("Invalid relative index.");
								NewIndex[1] = (UINT4b)(VN.size() - TmpInt);
							}
							else{
								MyLexer.ReadUInt(&TmpInt);
								NewIndex[1] = (UINT4b)(TmpInt - 1);
							}
						}
					}
					else NewIndex[1] = (UINT4b)-1;
					if ((UBINT)'/' == MyLexer.LastChar){
						MyLexer.LoadNextChar();
						if ((UBINT)'-' == MyLexer.LastChar){
							MyLexer.LoadNextChar();
							MyLexer.ReadUInt(&TmpInt);
							if (VT.size() < TmpInt)throw std::exception("Invalid relative index.");
							NewIndex[2] = (UINT4b)(VT.size() - TmpInt);
						}
						else{
							MyLexer.ReadUInt(&TmpInt);
							NewIndex[2] = (UINT4b)(TmpInt - 1);
						}
					}
					else NewIndex[2] = (UINT4b)-1;

					//compare with the old points
					UINT4b TmpIndex = (UINT4b)IndexCount;
					std::pair<const nsMath::TinyVector<UINT4b, 3>, UINT4b> TmpEntry(NewIndex, IndexCount);
					auto _TmpIt = IndexMap.insert(TmpEntry);
					if (false == _TmpIt.second){
						//collision happened
						TmpIndex = _TmpIt.first->second;
					}
					else{
						//push in the new point
						CurVP.push_back(VP[NewIndex[0]]);
						if ((UINT4b)-1 != NewIndex[1]){
							CurVT.push_back(VT[NewIndex[1]]);
						}
						if ((UINT4b)-1 != NewIndex[2]){
							CurVN.push_back(VN[NewIndex[2]]);
						}
						IndexCount++;
					}
					if ((UINT4b)-1 != QuadIndex[0] && (UINT4b)-1 != QuadIndex[1]){
						nsMath::TinyVector<UINT4b, 3> TmpVal = { QuadIndex[0], QuadIndex[1], TmpIndex };
						lpCurIndex->Value.push_back(TmpVal);
					}
					if ((UINT4b)-1 == QuadIndex[0])QuadIndex[0] = TmpIndex;
					else QuadIndex[1] = TmpIndex;
				}
			}
			while ((!nsText::IsRetChar(MyLexer.LastChar)) && (UINT4b)-1 != MyLexer.LastChar)MyLexer.LoadNextChar(); //line skip
		}
	} while ((UINT4b)-1 != MyLexer.LastChar);

	if (nullptr != CurObj)OutputNewObj();
}