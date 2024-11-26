struct ClusterCtrl;

struct ClusterCtrlHitRgn{
private:
	ClusterCtrlHitRgn() = delete; //using the default constructor is prohibited
	ClusterCtrlHitRgn(const ClusterCtrlHitRgn &) = delete; //not copyable
	ClusterCtrlHitRgn & operator =(const ClusterCtrlHitRgn &) = delete; //not copyable

	static UBINT _cdecl _MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
public:
	void *PrevObj;
	void *NextObj;
	void *ChildObjRing;
	UBINT _Reserved;
	UBINT Type;
	nsBasic::MSGPROC MsgProc;
	UBINT ThreadLock;
	ClusterCtrl *TgtCtrl;

	ClusterCtrlHitRgn(nsBasic::ObjSet *ParentObj, ClusterCtrl *TgtCtrl);
	~ClusterCtrlHitRgn(){};
	void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(ClusterCtrlHitRgn)); }
};

struct ClusterCtrl{
private:
	ClusterCtrl() = delete; //using the default constructor is prohibited
	ClusterCtrl(const ClusterCtrl &) = delete; //not copyable
	ClusterCtrl & operator =(const ClusterCtrl &) = delete; //not copyable

	static UBINT _cdecl MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	//static void Draw_GDI(UBINT hDC, Rect *LimitRect, SimpleCtrlData *BtnInfo);
#if defined LIBENV_GLIB_OPENGL
	static void Draw_GL(UBINT hDC, nsGUI::Rect *LimitRect, ClusterCtrl *CtrlInfo);
#endif

public:
	nsBasic::ObjGeneral ObjInterface;
	nsContainer::Vector<std::pair<nsGUI::Rect, UBINT>> SrcHitMap;
	UBINT MouseStat;
	UBINT ClusterIndex_Selected;
	UBINT ClusterIndex_MouseOver;

	ClusterCtrl(nsBasic::ObjSet *ParentObj);
	inline ClusterCtrlHitRgn *RegisterHitRgn(nsBasic::ObjSet *ParentObj){ return new ClusterCtrlHitRgn(ParentObj, this); }
	~ClusterCtrl(){};
	void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(ClusterCtrl)); }
};

/*-------------------------------- IMPLEMENTATION --------------------------------*/

ClusterCtrlHitRgn::ClusterCtrlHitRgn(nsBasic::ObjSet *ParentObj, ClusterCtrl *TgtCtrl){
	this->ChildObjRing = nullptr;
	this->Type = OBJ_USERDEF;
	this->MsgProc = ClusterCtrlHitRgn::_MsgProc;
	this->ThreadLock = (UBINT)nullptr;
	this->TgtCtrl = TgtCtrl;
	if (nullptr != ParentObj)nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)this, &(ParentObj->ChildObjRing));
}
UBINT _cdecl ClusterCtrlHitRgn::_MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...){
	va_list args;
	va_start(args, Msg);
	ClusterCtrlHitRgn *lpRgn = (ClusterCtrlHitRgn *)lpObj;

	if (MSG_MOUSECHECK == Msg){
		nsGUI::MouseStat *MyMouseStat = va_arg(args, nsGUI::MouseStat *);
		for (UBINT i = 0; i < lpRgn->TgtCtrl->SrcHitMap.size(); i++){
			nsGUI::Rect *MyRect = &lpRgn->TgtCtrl->SrcHitMap[i].first;
			if (MyMouseStat->XPos >= MyRect->XMin &&
				MyMouseStat->XPos < MyRect->XMax &&
				MyMouseStat->YPos >= MyRect->YMin &&
				MyMouseStat->YPos < MyRect->YMax){
				lpRgn->TgtCtrl->ClusterIndex_MouseOver = lpRgn->TgtCtrl->SrcHitMap[i].second;
				return (UBINT)(&lpRgn->TgtCtrl->ObjInterface);
			}
		}
		return 0;
	}
	else if (MSG_DESTROY == Msg)delete lpRgn;
	return 0;
}

ClusterCtrl::ClusterCtrl(nsBasic::ObjSet *ParentObj) :SrcHitMap(){
	this->ObjInterface.Type = OBJ_USERDEF;
	this->ObjInterface.MsgProc = ClusterCtrl::MsgProc;
	this->MouseStat = 0;
	this->ClusterIndex_Selected = (UBINT)-1;
	this->ClusterIndex_MouseOver = (UBINT)-1;
	if (NULL != ParentObj)nsBasic::ObjRing_Attach(&(this->ObjInterface), &(ParentObj->ChildObjRing));
}
UBINT _cdecl ClusterCtrl::MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...){
	va_list args;
	va_start(args, Msg);
	ClusterCtrl *lpCtrl = (ClusterCtrl *)((UBINT)lpObj - offsetof(ClusterCtrl, ObjInterface));

	switch (Msg){
	case MSG_MOUSEENTER:
		lpCtrl->MouseStat = 1;
		nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
		return 0;
	case MSG_MOUSEEVENT:
	{
		nsGUI::MouseStat *MyMouseStat = va_arg(args, nsGUI::MouseStat *);
		UBINT NewStat;

		if (MyMouseStat->BtnStat & nsGUI::MBTN_L){
			NewStat = 2;
			if (1 == lpCtrl->MouseStat){
				//button down
				lpCtrl->ClusterIndex_Selected = lpCtrl->ClusterIndex_MouseOver;
			}
			nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
		}
		else{
			NewStat = 1;
			if (2 == lpCtrl->MouseStat){
				//button up
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
		}

		lpCtrl->MouseStat = NewStat;
		return 0;
	}
	case MSG_MOUSELEAVE:
		lpCtrl->MouseStat = 0;
		lpCtrl->ClusterIndex_MouseOver = (UBINT)-1;
		nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
		return 0;
	case MSG_PAINT:
		{
			UBINT hDC = va_arg(args, UBINT);
			nsGUI::Rect *lpRect = va_arg(args, nsGUI::Rect *);
			Draw_GL(hDC, lpRect, lpCtrl);
		}
		return 0;
	case MSG_DESTROY:
		delete lpCtrl;
		return 0;
	default:
		return 0;
	}
}
void ClusterCtrl::Draw_GL(UBINT hDC, nsGUI::Rect *LimitRect, ClusterCtrl *CtrlInfo){
	float WorldMat[16], ProjMat[16];
	nsMath::Transform4_Perspective_Inf_Frustum(MyCamera.Frustum, ProjMat);
	nsMath::inv4(WorldMat, CamTransform.Matrix);
	glPointSize(10.0f);

	CtrlInfo->SrcHitMap.clear();
	UBINT ClusterIndex = 0;
	for (auto _It = MySoundClusterMgr->SoundClusterList_Source.begin(); _It != MySoundClusterMgr->SoundClusterList_Source.end(); ++_It){
		float TmpVec1[4] = { _It->TransformMatrix[12], _It->TransformMatrix[13], _It->TransformMatrix[14], _It->TransformMatrix[15] }, TmpVec2[4], SndSrcPos2D[2];
		nsMath::mul4_mv(WorldMat, TmpVec1, TmpVec2);
		nsMath::mul4_mv(ProjMat, TmpVec2, TmpVec1);
		if (TmpVec1[3] > 0.0){
			TmpVec1[0] /= TmpVec1[3]; TmpVec1[1] /= TmpVec1[3]; TmpVec1[2] /= TmpVec1[3];
			SndSrcPos2D[0] = ((1 - TmpVec1[0]) * ScreenRect.XMin + (1 + TmpVec1[0]) * ScreenRect.XMax) / 2;
			SndSrcPos2D[1] = ((1 - TmpVec1[1]) * ScreenRect.YMax + (1 + TmpVec1[1]) * ScreenRect.YMin) / 2;
			if (SndSrcPos2D[0] >= ScreenRect.XMin && SndSrcPos2D[0] <= ScreenRect.XMax && SndSrcPos2D[1] >= ScreenRect.YMin && SndSrcPos2D[1] <= ScreenRect.YMax){
				if (CtrlInfo->ClusterIndex_Selected == ClusterIndex){
					if (CtrlInfo->ClusterIndex_MouseOver == ClusterIndex)glColor3f(1.0f, 1.0f, 1.0f);
					else glColor3f(1.0f, 1.0f, 0.0f);
				}
				else{
					if (CtrlInfo->ClusterIndex_MouseOver == ClusterIndex)glColor3f(0.0f, 1.0f, 0.0f);
					else glColor3f(1.0f, 0.0f, 0.0f);
				}

				glEnable(GL_POINT_SMOOTH);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBegin(GL_POINTS);
				glVertex2fv(SndSrcPos2D);
				glEnd();
				glDisable(GL_BLEND);
				glDisable(GL_POINT_SMOOTH);

				if (TmpVec1[2] < 0.96f)nsGUI::GLDrawText_RefPoint(Font2, SndSrcPos2D, (const char *)_It->ClusterName.begin(), nsCharCoding::UTF8, nsGUI::TEXTLAYOUT_HORI_MEDIUM | nsGUI::TEXTLAYOUT_VERT_TOP);
				else nsGUI::GLDrawText_RefPoint(Font1, SndSrcPos2D, (const char *)_It->ClusterName.begin(), nsCharCoding::UTF8, nsGUI::TEXTLAYOUT_HORI_MEDIUM | nsGUI::TEXTLAYOUT_VERT_TOP);

				nsGUI::Rect HitRect = { (BINT)SndSrcPos2D[0] - 5, (BINT)SndSrcPos2D[1] - 5, (BINT)SndSrcPos2D[0] + 5, (BINT)SndSrcPos2D[1] + 5 };
				CtrlInfo->SrcHitMap.push_back(std::pair<nsGUI::Rect, UBINT>(HitRect, ClusterIndex));
			}
		}
		ClusterIndex++;
	}
}