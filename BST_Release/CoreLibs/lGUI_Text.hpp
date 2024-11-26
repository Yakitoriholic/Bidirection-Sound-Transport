/* Description:Platform-independent functions for text rendering
 * Language:C++
 * Author:***
 *
 * This library is based on FreeType library.
 * This library does not support ClearType currently.
 *
 * You must call nsBasic::CreateThreadExtObj() first to use the following functions.
 */

#ifndef LIB_GUI_TEXT
#define LIB_GUI_TEXT

#include "lGeneral.hpp"

#ifdef LIBENV_CPLR_VS
#ifdef _DEBUG
#pragma comment(lib,"VS_Debug/freetype252MT.lib")
#else
#pragma comment(lib,"VS_Release/freetype252MT.lib")
#endif
#endif

#include "freetype/ft2build.h"
#include FT_FREETYPE_H

namespace nsGUIText{
	struct GlyphInfo{
		unsigned short NextGlyphSect; //Reserved by this library.DO NOT MODIFY ITS VALUE.
		unsigned char NextGlyphIndex; //Reserved by this library.DO NOT MODIFY ITS VALUE.
		unsigned char BmpIndex;		  //0 = glyph not rendered yet.
		char XBearing_H;
		char YBearing_H;
		char XBearing_V;
		char YBearing_V;
		short Adv_H;
		short Adv_V;
		unsigned char XPos;
		unsigned char YPos;
		unsigned char XSpan;		  //0 = an empty GlyphInfo struct.
		unsigned char YSpan;
	};

	/*--- Private structures ---*/
	struct _TxtBmpPageTag{
		UBINT LastCreated;
		UBINT NextEmpty;
		UBINT BmpRing;
		UBINT ExtInfo;
	};
	struct _TxtBmp{
		UBINT DataAddr;
		UBINT PrevBmp;
		UBINT NextBmp;
		UBINT GlyphStack;
	};
	struct _GlyphSect{
	private:
		static UBINT _cdecl _MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	public:
		void *PrevObj;
		void *NextObj;
		UBINT SectAddr;
		UBINT FirstGlyph;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		UBINT CurGlyphCnt;

		_GlyphSect();
		~_GlyphSect();
		void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(_GlyphSect)); }
	};
	/*--- End ---*/
	struct FontRaster;
	struct FontEngine{
	private:
		FontEngine(const FontEngine &) = delete; //not copyable
		FontEngine & operator =(const FontEngine &) = delete; //not copyable
		void * operator new(size_t) = delete; //using operator new is prohibited
		void operator delete(void *) = delete; //using operator delete is prohibited

		static UBINT _cdecl _MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	public:
		void *PrevObj;
		void *NextObj;
		void *ChildObjRing;
		UBINT _Reserved2;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		FT_Library ExtInfo;

		FontEngine();
		FontRaster *CreateFontRaster(const char *FontPath, UBINT FontSize);
		~FontEngine();
	};
	struct FontRaster{
	private:
		FontRaster(const FontRaster &) = delete; //not copyable
		FontRaster & operator =(const FontRaster &) = delete; //not copyable

		FontRaster(){}
		_TxtBmp *_CreateNewTxtBmp();
		static UBINT _cdecl _MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
		~FontRaster();
	public:
		friend struct FontEngine;

		void *PrevObj;
		void *NextObj;
		_GlyphSect *GlyphSectRing;
		UBINT BitmapPage;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		FT_Face ExtInfo;

		GlyphInfo *LoadGlyph(UBINT CodePoint, UBINT *Repainted);
		UBINT UnloadGlyph(UBINT CodePoint);
		inline UBINT GetTextBitmap(UBINT Index){ return *(UBINT *)(this->BitmapPage + Index*sizeof(_TxtBmp)); }
		void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(FontRaster)); }
	};
	
	
}

/*-------------------------------- IMPLEMENTATION --------------------------------*/

namespace nsGUIText{
	UBINT _cdecl FontEngine::_MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		switch(Msg){
			case MSG_DESTROY:
				throw new std::exception("this object is only allowed to be created locally.", 1);
				return 1;
			default:
				return 0;
		}
	}
	FontEngine::FontEngine(){
		if (FT_Init_FreeType((FT_Library *)&(this->ExtInfo)))throw new std::exception("failed to initialize FreeType library.", 1); 
			this->ChildObjRing=nullptr;
			this->Type = OBJ_FONTENGINE;
			this->MsgProc = FontEngine::_MsgProc;
			this->ThreadLock = (UBINT)NULL;
	}
	FontRaster *FontEngine::CreateFontRaster(const char *FontPath, UBINT FontSize){
		FontRaster *MyFont = new FontRaster();
		MyFont->GlyphSectRing = nullptr;
		if (0 == FT_New_Face(this->ExtInfo, FontPath, 0, (FT_Face *)&MyFont->ExtInfo)){
			if (0 == FT_Set_Char_Size(MyFont->ExtInfo, 0, (FT_F26Dot6)(FontSize << 6), 0, 0)){
				MyFont->BitmapPage = (UBINT)nsBasic::MemAlloc(sizeof(_TxtBmpPageTag)+0xFF * sizeof(_TxtBmp));
				if (0 != MyFont->BitmapPage){
					//Format BitmapPage
					UBINT TmpPtr = MyFont->BitmapPage;
					((_TxtBmpPageTag *)TmpPtr)->LastCreated = 0;
					((_TxtBmpPageTag *)TmpPtr)->NextEmpty = TmpPtr + sizeof(_TxtBmp);
					((_TxtBmpPageTag *)TmpPtr)->BmpRing = 0;
					((_TxtBmpPageTag *)TmpPtr)->ExtInfo = 0;
					TmpPtr += sizeof(_TxtBmpPageTag);
					for (int i = 2; i<0x100; i++){
						*(UBINT *)TmpPtr = TmpPtr + sizeof(_TxtBmp);
						TmpPtr += sizeof(_TxtBmp);
					}
					*(UBINT *)TmpPtr = 0;

					MyFont->GlyphSectRing = nullptr;
					MyFont->Type = OBJ_FONTRASTER;
					MyFont->MsgProc = FontRaster::_MsgProc;
					MyFont->ThreadLock = (UBINT)NULL;
					nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)MyFont, &this->ChildObjRing);
					return MyFont;
				}
			}
			FT_Done_Face((FT_Face)MyFont->ExtInfo);
		}
		delete MyFont;
		throw new std::exception("font creation failed.", 1);
		return nullptr;
	}
	FontEngine::~FontEngine(){
		nsBasic::ObjGeneral *Obj1 = (nsBasic::ObjGeneral *)this->ChildObjRing, *Obj2;
		if (NULL != Obj1){
			do{
				Obj2 = (nsBasic::ObjGeneral *)Obj1->NextObj;
				DestroyObject(Obj1);
				Obj1 = Obj2;
			} while (Obj1 != (nsBasic::ObjGeneral *)this->ChildObjRing);
			FT_Done_FreeType(this->ExtInfo);
		}
	}

	static UBINT _BinPacking_BR_NR(unsigned char *Dest,unsigned char *Src,UBINT HeightWidth){
		/* This function performs 2D bin packing on a 256*256 bitmap with 1-byte pixel format.
		 * We assume that all bitmaps here arrange pixels in a left-to-right,top-to-bottom sequence.
		 * The pointer *Dest points to the 256*256 bitmap.
		 * The pointer *Src points to the bitmap you are going to pack.
		 * HeightWidth = height of your bitmap * 256 + width of your bitmap.
		 * Bitmap *Dest should be preprocessed.An empty 256*256 bitmap should be preprocessed like this:
		 * 0xFF 0xFF ... 0xFF
         * ...  ...  ... ...
		 * 0x0  0x0  ... 0x0
		 *
		 * This function adds a 1-pixel wide border 0f 0x0 on the top & left of the packed bitmap.
		 * It returns the position of the top-left most pixel of the packed bitmap.
		 * The return value is YPos * 256 + XPos.0 indicates a failure.
		 *
		 * This function uses a incorrect BOTTOM-RIGHT FIRST packing strategy to pack bitmaps.
		 * This function does not rotate your bitmap.
		 */
		unsigned char *Stack=(unsigned char *)nsBasic::MemAlloc(0x300);
		if(NULL==Stack)return 0;

		UBINT LastBest=0,j,Seg1,Seg2,LSeg1,LSeg2,RSeg1,RSeg2;

		for(UBINT i=0;i<0x100-(HeightWidth&0xFF);i++){
			if(0!=*Dest){
				j=i;
				Seg1=0;
				LSeg1=0;
				LSeg2=0xFF00;
				do{
					Seg2=*(Dest+Seg1)<<8;
					if(Seg1>LSeg1)RSeg1=Seg1;else RSeg1=LSeg1;
					if(Seg2<LSeg2)RSeg2=Seg2;else RSeg2=LSeg2;
					if(RSeg2>RSeg1 && RSeg2>=(LastBest&0xFF00) && RSeg2-RSeg1>=(HeightWidth&0xFF00)){
						//This is a possible position.
						if(j-i>=(HeightWidth&0xFF))LastBest=RSeg2+j; //New best
						else{
							//Push in.
							if(0!=*(Dest+1)){
								*Stack=*(Dest+Seg2);
								*(Stack+0x100)=(unsigned char)(LSeg1>>8);
								*(Stack+0x200)=(unsigned char)(LSeg2>>8);
								LSeg1=RSeg1;
								LSeg2=RSeg2;
								Dest++;
								Stack++;
								j++;
								Seg1=0;
								continue;
							}
						}
					}
					Seg1=*(Dest+Seg2)<<8;
					while(0==Seg1){
						//Fallback
						if(j<=i)break;
						j--;
						Dest--;
						Stack--;
						Seg1=*Stack<<8;
						LSeg1=*(Stack+0x100)<<8;
						LSeg2=*(Stack+0x200)<<8;
					}
				}while(j>i || 0!=Seg1);
			}
			Dest++;
		}
		Dest-=0x100-(HeightWidth&0xFF);

		if(0!=LastBest){ //Reprocess bitmap
			j=LastBest-HeightWidth;
			Dest+=j&0xFF;
			for(UBINT i=0;i<=(HeightWidth&0xFF);i++){
				if(0!=*Dest){
					if(j<=0x200)*Dest=0;
					else{
						LSeg2=0;
						Seg1=0;
						do{
							Seg2=*(Dest+Seg1)<<8;
							if((j&0xFF00)<=Seg2 && (LastBest&0xFF00)>=Seg1){
								//A cut happened.
								if((j&0xFF00)>Seg1+0x100){
									//cut 1
									LSeg2=(j&0xFF00)-0x100;
									*(Dest+Seg1)=(unsigned char)(LSeg2>>8);
									Seg1=LSeg2;
								}
								if((LastBest&0xFF00)+0x100<Seg2){
									//cut 2
									Seg1=(LastBest&0xFF00)+0x100;
									*(Dest+LSeg2)=(unsigned char)(Seg1>>8);
									*(Dest+Seg1)=(unsigned char)(Seg2>>8);
									LSeg2=Seg2;
								}
							}
							else{
								if(LSeg2>0)*(Dest+LSeg2)=(unsigned char)(Seg1>>8);
								LSeg2=Seg2;
							}
							Seg1=*(Dest+Seg2)<<8;
						}while(0!=Seg1);
						*(Dest+LSeg2)=0;
					}
				}
				Dest++;
			}
			Dest-=(HeightWidth&0xFF)+1;
			Dest+=j&0xFF00;

			LastBest=j;
			for(UBINT i=0;i<=(HeightWidth&0xFF00);i+=0x100){
				*Dest=0;
				Dest+=0x100;
			}
			Dest-=(HeightWidth&0xFF00)+0x100;
			for(UBINT i=0;i<(HeightWidth&0xFF);i++){
				Dest++;
				*Dest=0;
			}
			//Src+=(HeightWidth>>8)*(HeightWidth&0xFF);
			for(UBINT i=0;i<(HeightWidth&0xFF00);i+=0x100){
				Dest-=(HeightWidth&0xFF);
				Dest+=0x100;
				for(j=0;j<(HeightWidth&0xFF);j++){
					Dest++;
					*Dest=*Src;
					*Src++;
				}
			}
			LastBest+=0x101;
		}
		nsBasic::MemFree(Stack,0x300);
		return LastBest;
	}

	UBINT _cdecl _GlyphSect::_MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		if(MSG_DESTROY==Msg){
			_GlyphSect *MySect=(_GlyphSect *)lpObj;
			delete MySect;
			return 0;
		}
		else return 0;
	}
	_GlyphSect::_GlyphSect(){
		this->SectAddr=(UBINT)nsBasic::MemAlloc(0x100*sizeof(GlyphInfo));
		if (0 == this->SectAddr)throw new std::bad_alloc;
		//Format GlyphSect
		GlyphInfo *TmpPtr=(GlyphInfo *)this->SectAddr;
		for(int i=0;i<0x100;i++){
			TmpPtr->BmpIndex=0;
			TmpPtr++;
		}
		this->Type = SECT_GLYPH;
		this->MsgProc = _GlyphSect::_MsgProc;
		this->ThreadLock = (UBINT)NULL;
		this->CurGlyphCnt = 0;
	}
	_GlyphSect::~_GlyphSect(){
		nsBasic::MemFree((void *)(this->SectAddr), 0x100 * sizeof(GlyphInfo));
	}

	UBINT _cdecl FontRaster::_MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		FontRaster *MyRaster=(FontRaster *)lpObj;
		nsBasic::ObjGeneral *Obj1 = (nsBasic::ObjGeneral *)MyRaster->GlyphSectRing;
		//_TxtBmp *TmpBmp;

		switch(Msg){
			case MSG_DESTROY:
				delete MyRaster;
				return 0;
			default:
				return 0;
		}
	}
	UBINT FontRaster::UnloadGlyph(UBINT CodePoint){
		if(0==this->GlyphSectRing || 0==CodePoint)return 0;

		_GlyphSect *TgtSect=this->GlyphSectRing;
		do{
			if(TgtSect->FirstGlyph==(CodePoint&0xFFFF00))break;
			TgtSect=(_GlyphSect *)TgtSect->NextObj;
		}
		while(TgtSect!=this->GlyphSectRing);
		if(TgtSect->FirstGlyph!=(CodePoint&0xFFFF00))return 0;

		GlyphInfo *MyGlyphInfo=(GlyphInfo *)(TgtSect->SectAddr)+(CodePoint&0xFF);
		UBINT RetValue=0;

		if(0!=MyGlyphInfo->XSpan){
			if(0!=MyGlyphInfo->BmpIndex){
				RetValue=((UBINT)MyGlyphInfo->NextGlyphSect)<<8;
				RetValue+=MyGlyphInfo->NextGlyphIndex;
				MyGlyphInfo->BmpIndex=0;
			}
			MyGlyphInfo->XSpan=0;
			TgtSect->CurGlyphCnt--;
			if(0==TgtSect->CurGlyphCnt){
				nsBasic::ObjRing_Detach((nsBasic::ObjGeneral *)TgtSect,(void **)&this->GlyphSectRing);
				nsBasic::DestroyObject((nsBasic::ObjGeneral *)TgtSect);
			}
		}
		return RetValue;
	}
	_TxtBmp *FontRaster::_CreateNewTxtBmp(){
		_TxtBmp *TmpBmp;
		UBINT TmpInt;
		unsigned char *TmpPtr;

		if (0 == ((_TxtBmpPageTag *)this->BitmapPage)->NextEmpty){
			//Delete useless pages
			TmpBmp=(_TxtBmp *)((_TxtBmpPageTag *)this->BitmapPage)->BmpRing;
			for(int i=0;i<127;i++){
				TmpBmp=(_TxtBmp *)TmpBmp->PrevBmp;
				nsBasic::MemFree((void *)(TmpBmp->DataAddr),0x10000);
				TmpInt=TmpBmp->GlyphStack;
				while (TmpInt)TmpInt = this->UnloadGlyph(TmpInt);
				*(UBINT *)TmpBmp = ((_TxtBmpPageTag *)this->BitmapPage)->NextEmpty;
				((_TxtBmpPageTag *)this->BitmapPage)->NextEmpty = (UBINT)TmpBmp;
			}
			TmpBmp=(_TxtBmp *)TmpBmp->PrevBmp;
			TmpBmp->NextBmp = ((_TxtBmpPageTag *)this->BitmapPage)->BmpRing;
		}

		TmpBmp = (_TxtBmp *)((_TxtBmpPageTag *)this->BitmapPage)->NextEmpty;
		TmpInt=(UBINT)nsBasic::MemAlloc(0x10000);
		if ((UBINT)NULL == TmpInt)return NULL;
		else{
			((_TxtBmpPageTag *)this->BitmapPage)->NextEmpty = *(UBINT *)(((_TxtBmpPageTag *)this->BitmapPage)->NextEmpty);
			if (0 == ((_TxtBmpPageTag *)this->BitmapPage)->BmpRing){
				TmpBmp->PrevBmp=(UBINT)TmpBmp;
				TmpBmp->NextBmp=(UBINT)TmpBmp;
			}
			else{
				TmpBmp->NextBmp=((_TxtBmpPageTag *)this->BitmapPage)->BmpRing;
				TmpBmp->PrevBmp=((_TxtBmp *)(((_TxtBmpPageTag *)this->BitmapPage)->BmpRing))->PrevBmp;
				((_TxtBmp *)(((_TxtBmpPageTag *)this->BitmapPage)->BmpRing))->PrevBmp=(UBINT)TmpBmp;
				((_TxtBmp *)TmpBmp->PrevBmp)->NextBmp=(UBINT)TmpBmp;
			}
			((_TxtBmpPageTag *)this->BitmapPage)->BmpRing=(UBINT)TmpBmp;
			TmpBmp->DataAddr=TmpInt;
			TmpBmp->GlyphStack=0;
			((_TxtBmpPageTag *)this->BitmapPage)->LastCreated=(UBINT)TmpBmp;

			//Preprocess bitmap
			TmpPtr=(unsigned char *)TmpBmp->DataAddr;
			for(int i=0;i<0x100;i++){
				*TmpPtr=0xFF;
				TmpPtr++;
			}
			TmpPtr+=0xFE00;
			for(int i=0;i<0x100;i++){
				*TmpPtr=0;
				TmpPtr++;
			}

			return TmpBmp;
		}
	}
	GlyphInfo *FontRaster::LoadGlyph(UBINT CodePoint,UBINT *Repainted){
		*Repainted=0;

		if(0==CodePoint)return NULL;
		_GlyphSect *TgtSect=this->GlyphSectRing;
		if(NULL!=TgtSect){
			do{
				if(TgtSect->FirstGlyph==(CodePoint&0xFFFF00))break;
				TgtSect=(_GlyphSect *)TgtSect->NextObj;
			}
			while (TgtSect != this->GlyphSectRing);
			if(TgtSect->FirstGlyph!=(CodePoint&0xFFFF00))TgtSect=NULL;
		}
		if(NULL==TgtSect){
			TgtSect=new _GlyphSect;
			if(NULL==TgtSect)return NULL;
			else{
				TgtSect->FirstGlyph=CodePoint&0xFFFF00;
				nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)TgtSect, (void **)&this->GlyphSectRing);
			}
		}

		GlyphInfo *MyGlyphInfo=((GlyphInfo *)TgtSect->SectAddr)+(CodePoint&0xFF);
		if(MyGlyphInfo->BmpIndex>0)return MyGlyphInfo;
		else{
			//Load new glyph into memory
			if (FT_Load_Glyph((FT_Face)this->ExtInfo, FT_Get_Char_Index(this->ExtInfo, (FT_ULong)CodePoint), FT_LOAD_RENDER) || (this->ExtInfo)->glyph->bitmap.width >= 0x100 || (this->ExtInfo)->glyph->bitmap.width >= 0x100){ this->UnloadGlyph(CodePoint); return NULL; }

			FT_GlyphSlot MyGlyphSlot=(this->ExtInfo)->glyph;
			_TxtBmp *MyBmp=(_TxtBmp *)(((_TxtBmpPageTag *)this->BitmapPage)->LastCreated);
			if(NULL==MyBmp){
				MyBmp = this->_CreateNewTxtBmp();
				if(NULL==MyBmp){this->UnloadGlyph(CodePoint);return NULL;}
			}

			UBINT PackedPos=_BinPacking_BR_NR((unsigned char *)MyBmp->DataAddr,(unsigned char *)MyGlyphSlot->bitmap.buffer,MyGlyphSlot->bitmap.rows*0x100+MyGlyphSlot->bitmap.width);
			if(0==PackedPos){
				MyBmp = this->_CreateNewTxtBmp();
				if(NULL==MyBmp){this->UnloadGlyph(CodePoint);return NULL;}
				PackedPos=_BinPacking_BR_NR((unsigned char *)MyBmp->DataAddr,(unsigned char *)MyGlyphSlot->bitmap.buffer,MyGlyphSlot->bitmap.rows*0x100+MyGlyphSlot->bitmap.width);
			}

			MyGlyphInfo->NextGlyphSect=(unsigned short)(MyBmp->GlyphStack>>8);
			MyGlyphInfo->NextGlyphIndex=(unsigned char)(MyBmp->GlyphStack&0xFF);
			MyGlyphInfo->BmpIndex=(unsigned char)(((UBINT)MyBmp-this->BitmapPage)/sizeof(_TxtBmp));
			MyGlyphInfo->XPos=(unsigned char)(PackedPos&0xFF);
			MyGlyphInfo->YPos=(unsigned char)(PackedPos>>8);
			MyGlyphInfo->XSpan=(unsigned char)MyGlyphSlot->bitmap.width;
			MyGlyphInfo->YSpan=(unsigned char)MyGlyphSlot->bitmap.rows;
			MyGlyphInfo->XBearing_V=(char)(MyGlyphSlot->metrics.vertBearingX>>6);
			MyGlyphInfo->YBearing_V=(char)(MyGlyphSlot->metrics.vertBearingY>>6);
			MyGlyphInfo->XBearing_H=(char)(MyGlyphSlot->metrics.horiBearingX>>6);
			MyGlyphInfo->YBearing_H=(char)(MyGlyphSlot->metrics.horiBearingY>>6);
			MyGlyphInfo->Adv_H=(short)(MyGlyphSlot->metrics.horiAdvance>>6);
			MyGlyphInfo->Adv_V=(short)(MyGlyphSlot->metrics.vertAdvance>>6);

			MyBmp->GlyphStack=CodePoint;
			TgtSect->CurGlyphCnt++;
			*Repainted=1;

			return MyGlyphInfo;
		}
	}
	FontRaster::~FontRaster(){
		nsBasic::ObjGeneral *Obj1 = (nsBasic::ObjGeneral *)this->GlyphSectRing, *Obj2;
		_TxtBmp *TmpBmp;
		if (NULL != Obj1){
			do{
				Obj2 = (nsBasic::ObjGeneral *)Obj1->NextObj;
				DestroyObject(Obj1);
				Obj1 = Obj2;
			} while (Obj1 != (nsBasic::ObjGeneral *)this->GlyphSectRing);
			TmpBmp = (_TxtBmp *)(((_TxtBmpPageTag *)(this->BitmapPage))->BmpRing);
			if (NULL != TmpBmp){
				do{
					nsBasic::MemFree((void *)(TmpBmp->DataAddr), 0x10000);
					TmpBmp = (_TxtBmp *)TmpBmp->NextBmp;
				} while (TmpBmp != (_TxtBmp *)(((_TxtBmpPageTag *)(this->BitmapPage))->BmpRing));
			}
			nsBasic::MemFree((void *)(this->BitmapPage), 0x100 * sizeof(_TxtBmp));
			FT_Done_Face((FT_Face)this->ExtInfo);
		}
	}
}

#endif