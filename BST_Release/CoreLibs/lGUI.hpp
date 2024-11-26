/* Description: Functions for basic graphic interface.
 * Language:C++
 * Author:***
 *
 * The following macros should be defined before you use this library:
 * Graphic Interface:LIBENV_GUI_WIN   / LIBENV_GUI_XWINDOW
 * Graphic library  :LIBENV_GLIB_GDI  / LIBENV_GLIB_XWINDOW / LIBENV_GLIB_D3D      / LIBENV_GLIB_OPENGL
 * Event Listener   :LIBENV_EVENT_WIN / LIBENV_EVENT_DINPUT / LIBENV_EVENT_XWINDOW
 *
 * You must call nsBasic::CreateThreadExtObj() first to use the following functions.
 */

#ifndef LIB_GUI
#define LIB_GUI

#include "lGeneral.hpp"
#include "lMath.hpp"
#include "lFile.hpp"
#include "lGUI_Text.hpp"

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check the following macros.
//These macros provide platform-related headers and static libraries.
#endif


#if defined LIBENV_GUI_XWINDOW
#include<X11/X.h>
#include<X11/Xlib.h>
#endif

#if defined LIBENV_GLIB_OPENGL
#include "lGUI_GL.hpp"
#endif

namespace nsGUI{
	extern enum MouseButton{
		MBTN_L = 1,
		MBTN_M = 2,
		MBTN_R = 4,
	};
	extern enum TextLayout{
		TEXTLAYOUT_HORI_LEFT = 0,
		TEXTLAYOUT_HORI_MEDIUM = 1,
		TEXTLAYOUT_HORI_RIGHT = 2,
		TEXTLAYOUT_VERT_TOP = 0,
		TEXTLAYOUT_VERT_MEDIUM = 1 << 2,
		TEXTLAYOUT_VERT_BOTTOM = 2 << 2
	};

	struct Rect{		//Explanation:XMin=min value of x
		BINT XMin;		//			  XMax=max value of x + 1
		BINT YMin;		//So the width of the rectangle is XMax - XMin.
		BINT XMax;		//The same to y.
		BINT YMax;
	};
	struct MouseStat{
		UBINT BtnStat;
		BINT XPos;
		BINT YPos;
		BINT ZPos;		//For mouse wheel
	};

	struct Window{
	private:
		Window() = delete; //using the default constructor is prohibited
		Window(const Window &) = delete; //not copyable
		Window & operator =(const Window &) = delete; //not copyable
		void * operator new(size_t) = delete; //using operator new is prohibited
		void operator delete(void *) = delete; //using operator delete is prohibited
	protected:
		Window(Rect *WndRect, UBCHAR *Title);
		~Window();
	public:
		void *PrevObj;
		void *NextObj;
		void *ChildObjRing;
		UBINT hWnd;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		UBINT hGDC;

		inline operator nsBasic::ObjGeneral() const{ return *(nsBasic::ObjGeneral *)this; }
		void Show();
	};

#if defined LIBENV_GLIB_OPENGL
	struct GLWindow :Window{
	private:
		static UBINT _cdecl _MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	public:
		GLWindow(Rect *WndRect, UBCHAR *Title = nullptr);
		GLDevice *GetGraphicDevice();
		~GLWindow();
	};
#endif

	//struct GraphicDevice{  //This structure is for GDI
	//	UBINT PrevObj;
	//	UBINT NextObj;
	//	UBINT ChildObjRing;
	//	UBINT hDC;
	//	UBINT Type;
	//	UBINT MsgProc;
	//	UBINT ThreadLock;
	//	UBINT hBuffer;
	//};

	/*struct SPaintBuffer{
		UBINT PrevObj;
		UBINT NextObj;
		UBINT Width;
		UBINT Height;
		UBINT Type;
		UBINT MsgProc;
		UBINT ThreadLock;
		UBINT hBuffer;
		};*/

	struct SimpleCtrlData{
		Rect CtrlPos;
		char *CtrlText;
		nsGUIText::FontRaster *CtrlFont;
		UBINT Dock;
		UBINT Status;
		UBINT InnerStatus;
		void(*PaintFunc)(UBINT, Rect *, SimpleCtrlData *);
	};
	struct SingleScrollData{
		Rect CtrlPos;
		char *CtrlText;
		nsGUIText::FontRaster *CtrlFont;
		UBINT Dock;
		UBINT MaxValue;
		UBINT CurrentValue;
		UBINT Status;
		UBINT InnerStatus;
		void(*PaintFunc)(UBINT, Rect *, SingleScrollData *);
	};
	struct RectHitRgn{
	private:
		RectHitRgn() = delete; //using the default constructor is prohibited
		RectHitRgn(const RectHitRgn &) = delete; //not copyable
		RectHitRgn & operator =(const RectHitRgn &) = delete; //not copyable

		static UBINT _cdecl _MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	public:
		void *PrevObj;
		void *NextObj;
		void *ChildObjRing;
		Rect *lpRect;
		UBINT Type;
		nsBasic::MSGPROC MsgProc;
		UBINT ThreadLock;
		nsBasic::ObjGeneral *TgtCtrl;

		RectHitRgn(nsBasic::ObjSet *ParentObj, nsBasic::ObjGeneral *TgtCtrl, Rect *HitRect);
		inline operator nsBasic::ObjGeneral() const{ return *(nsBasic::ObjGeneral *)this; }
		~RectHitRgn(){};
		void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(RectHitRgn)); }
	};

	struct Button{
	private:
		Button() = delete; //using the default constructor is prohibited
		Button(const Button &) = delete; //not copyable
		Button & operator =(const Button &) = delete; //not copyable

		static UBINT _cdecl MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
		//static void Draw_GDI(UBINT hDC, Rect *LimitRect, SimpleCtrlData *BtnInfo);
#if defined LIBENV_GLIB_OPENGL
		static void Draw_GL(UBINT hDC, Rect *LimitRect, SimpleCtrlData *BtnInfo);
#endif

	public:
		nsBasic::ObjGeneral ObjInterface;
		SimpleCtrlData Data;

		Button(nsBasic::ObjSet *ParentObj);
		inline operator nsBasic::ObjGeneral() const{ return *(nsBasic::ObjGeneral *)this; }
		inline RectHitRgn *RegisterHitRgn(nsBasic::ObjSet *ParentObj){ return new RectHitRgn(ParentObj, (nsBasic::ObjGeneral *)this, &(this->Data.CtrlPos)); }
		inline void ToggleEnable(bool enable){
			if (enable && 2 == this->Data.Status)this->Data.Status = 0;
			else if (!enable){
				this->Data.Status = 2;
				this->Data.InnerStatus = 0;
			}
		};
		~Button(){};
		void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(Button)); }
	};
	struct CheckBox{
	private:
		CheckBox() = delete; //using the default constructor is prohibited
		CheckBox(const CheckBox &) = delete; //not copyable
		CheckBox & operator =(const CheckBox &) = delete; //not copyable

		static UBINT _cdecl MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
#if defined LIBENV_GLIB_OPENGL
		static void Draw_GL(UBINT hDC, Rect *LimitRect, SimpleCtrlData *BtnInfo);
#endif

	public:
		nsBasic::ObjGeneral ObjInterface;
		SimpleCtrlData Data;

		CheckBox(nsBasic::ObjSet *ParentObj);
		inline operator nsBasic::ObjGeneral() const{ return *(nsBasic::ObjGeneral *)this; }
		inline RectHitRgn *RegisterHitRgn(nsBasic::ObjSet *ParentObj){ return new RectHitRgn(ParentObj, (nsBasic::ObjGeneral *)this, &(this->Data.CtrlPos)); }
		inline void ToggleEnable(bool enable){
			if (enable && 2 == this->Data.Status)this->Data.Status = 0;
			else if (!enable){
				this->Data.Status = 2;
				this->Data.InnerStatus = 0;
			}
		};
		~CheckBox(){};
		void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(CheckBox)); }
	};

	struct Slider{
	private:
		Slider() = delete; //using the default constructor is prohibited
		Slider(const Slider &) = delete; //not copyable
		Slider & operator =(const Slider &) = delete; //not copyable

		static UBINT _cdecl MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
#if defined LIBENV_GLIB_OPENGL
		static void Draw_GL(UBINT hDC, Rect *LimitRect, SingleScrollData *BtnInfo);
#endif
	public:
		nsBasic::ObjGeneral ObjInterface;
		SingleScrollData Data;

		Slider(nsBasic::ObjSet *ParentObj);
		inline operator nsBasic::ObjGeneral() const{ return *(nsBasic::ObjGeneral *)this; }
		inline RectHitRgn *RegisterHitRgn(nsBasic::ObjSet *ParentObj){ return new RectHitRgn(ParentObj, (nsBasic::ObjGeneral *)this, &(this->Data.CtrlPos)); }
		~Slider(){};
		void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(Slider)); }
	};

	struct ProgressBar{
	private:
		ProgressBar() = delete; //using the default constructor is prohibited
		ProgressBar(const ProgressBar &) = delete; //not copyable
		ProgressBar & operator =(const ProgressBar &) = delete; //not copyable

		static UBINT _cdecl MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
#if defined LIBENV_GLIB_OPENGL
		static void Draw_GL(UBINT hDC, Rect *LimitRect, SingleScrollData *BtnInfo);
#endif

	public:
		nsBasic::ObjGeneral ObjInterface;
		SingleScrollData Data;

		ProgressBar(nsBasic::ObjSet *ParentObj);
		inline operator nsBasic::ObjGeneral() const{ return *(nsBasic::ObjGeneral *)this; }
		~ProgressBar(){};
		void operator delete(void *ptr){ nsBasic::MemFree(ptr, sizeof(ProgressBar)); }
	};

	//struct Point3DInfo{
	//	double XPos,YPos,ZPos;
	//	UBINT Status;
	//	UBINT InnerStatus;
	//	UBINT PaintFlag;			 //Reserved by this library.DO NOT MODIFY ITS VALUE.
	//	void (*PaintFunc)(UBINT,Rect *,Control *);
	//};
	struct Tripod{
	private:
		Tripod(const Tripod &) = delete; //not copyable
		Tripod & operator =(const Tripod &) = delete; //not copyable
		void * operator new(size_t) = delete; //using operator new is prohibited
		void operator delete(void *) = delete; //using operator delete is prohibited

		static UBINT _cdecl MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	public:
		nsBasic::ObjGeneral ObjInterface;
		unsigned char Dummy[8]; //Producing virtual addresses for hitmaps.
		double Pos[3], XPrj[3], YPrj[3];
		UBINT MouseBtnStat, MouseRecX, MouseRecY;
		UBINT Status;
		UBINT CurDummyID;

		void Draw(bool IsHitMap);
		Tripod();
	};

	struct ControlManager{
	private:
		ControlManager(const ControlManager &) = delete; //not copyable
		ControlManager & operator =(const ControlManager &) = delete; //not copyable
		void * operator new(size_t) = delete; //using operator new is prohibited
		void operator delete(void *) = delete; //using operator delete is prohibited

		static UBINT _cdecl MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	public:
		nsBasic::ObjSet ObjInterFace;
		nsBasic::ObjGeneral *MouseFocus;
		nsBasic::ObjGeneral *KeybdFocus;
		UBINT LastMBtnStat;

		ControlManager();
		inline operator nsBasic::ObjGeneral() const{ return *(nsBasic::ObjGeneral *)this; }
		~ControlManager();
	};
	extern UBINT LoadWindowMgr();
	extern UBINT unLoadWindowMgr();

	/*extern UBINT _cdecl MsgProc_GDIWindow(nsBasic::ObjGeneral *lpObj,UBINT Msg,...);
	extern UBINT CreateGDIWindow(Window *lpWindow,Rect *WndRect);*/

	extern UBINT SimpleMsgPump();

	extern UBINT _cdecl MsgProc_GraphicObjSet(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);

#if defined LIBENV_GLIB_OPENGL
	extern UBINT _cdecl MsgProc_GL3DScene(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	/*extern UBINT _cdecl MsgProc_GDICtrlScene(nsBasic::ObjGeneral *lpObj,UBINT Msg,...);
	extern nsBasic::ObjSet *CreateGDIControlScene(Window *lpWindow);*/
	extern UBINT _cdecl MsgProc_GLCtrlScene(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...);
	/*extern UBINT _cdecl MsgProc_GLSPaintBuffer(nsBasic::ObjGeneral *lpObj,UBINT Msg,...);
	extern SPaintBuffer *CreateGLSPaintBuffer(Window *lpWindow);*/

	extern inline void GLSetColorToUBINT(UBINT value){
#if defined LIBENV_SYS_INTELX86
		glColor4ub(value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF, (value >> 24) & 0xFF);
#elif defined LIBENV_SYS_INTELX64
		glColor4us(value & 0xFFFF, (value >> 16) & 0xFFFF, (value >> 32) & 0xFFFF, (value >> 48) & 0xFFFF);
#endif
	}
	extern UBINT GLDrawText(nsGUIText::FontRaster *lpRaster, const Rect *lpRect, const char *lpText, UBINT Coding, UBINT Layout);
	//render text inside a rectangle.
	extern UBINT GLDrawText_RefPoint(nsGUIText::FontRaster *lpRaster, const float *RefPoint, const char *lpText, UBINT Coding, UBINT Layout);
	//render text inside a rectangle. The position of the text is calculated in related to the reference point.
#endif

	/*extern UBINT _cdecl MsgProc_BufferHitMap(nsBasic::ObjGeneral *lpObj,UBINT Msg,...);
	extern nsBasic::ObjSetExt *CreateBufferHitMap(nsBasic::ObjSet *ParentObj,SPaintBuffer *lpBuffer);*/

	/*extern UBINT _cdecl MsgProc_Point3D(nsBasic::ObjGeneral *lpObj,UBINT Msg,...);
	extern void GLDrawPoint3D(UBINT hDC,Rect *LimitRect,Control *PointCtrl);
	extern Control *CreatePoint3D(nsBasic::ObjSet *ParentObj);
	extern UBINT _cdecl MsgProc_Tripod(nsBasic::ObjGeneral *lpObj,UBINT Msg,...);*/
}

/*-------------------------------- IMPLEMENTATION --------------------------------*/

namespace nsGUI{
#ifdef LIBENV_GUI_WIN
	LPCWSTR sBasicWndClass=L"BasicWindow";
	static LRESULT CALLBACK _WinMsgProcHack(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		Window *lpWindow = (Window *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		switch (message){
			case WM_SIZE:
			{
				UBINT MsgArr[3] = { MSG_WNDSIZE, LOWORD(lParam), HIWORD(lParam) };
				nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), &MsgArr, sizeof(MsgArr));
				return 0;
			}
			case WM_DESTROY:
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_WNDDESTROY);
				return 0;
			case WM_PAINT:
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				return 0;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	extern UBINT LoadWindowMgr(){
		WNDCLASSEX wcex;
		wcex.cbSize=sizeof(WNDCLASSEX);
		wcex.style=CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc=_WinMsgProcHack;
		wcex.cbClsExtra=0;

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this line.
//I'm not sure if there are 8 bytes after GWLP_USERDATA in 64-bit Windows.
#endif

		wcex.cbWndExtra=0;
		wcex.hInstance=(HINSTANCE)nsEnv::InstanceID;
		wcex.hIcon=NULL;
		wcex.hCursor=LoadCursor(NULL,IDC_ARROW);
		wcex.hbrBackground=NULL;
		wcex.lpszMenuName=NULL;
		wcex.lpszClassName=sBasicWndClass;
		wcex.hIconSm=NULL;
		return !RegisterClassEx(&wcex);
	}
	extern UBINT unLoadWindowMgr(){
		return !UnregisterClass(sBasicWndClass,(HINSTANCE)nsEnv::InstanceID);
	}
	Window::Window(Rect *WndRect,UBCHAR *Title=nullptr){
		if (nullptr == WndRect)this->hWnd = (UBINT)CreateWindow(sBasicWndClass, Title, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr, nullptr, (HINSTANCE)nsEnv::InstanceID, nullptr);
		else{
			RECT WinStyleRect = { (LONG)WndRect->XMin, (LONG)WndRect->YMin, (LONG)WndRect->XMax, (LONG)WndRect->YMax };
			AdjustWindowRect(&WinStyleRect, WS_OVERLAPPEDWINDOW, false);
			this->hWnd = (UBINT)CreateWindow(sBasicWndClass, Title, WS_OVERLAPPEDWINDOW,
				WinStyleRect.left, WinStyleRect.top, WinStyleRect.right - WinStyleRect.left, WinStyleRect.bottom - WinStyleRect.top,
				nullptr, nullptr, (HINSTANCE)nsEnv::InstanceID, nullptr);
		}
		if (0 != this->hWnd){

#ifdef LIBDBG_PLATFORM_TEST
#error PLATFORM TEST CHECKPOINT:Check this function.
//I'm not sure if there are 8 bytes after GWLP_USERDATA in 64-bit Windows.
#endif

			SetWindowLongPtr((HWND)this->hWnd, GWLP_USERDATA, (LONG_PTR)this);
			this->ChildObjRing = 0;
			this->Type = OBJ_WINDOW;
			this->MsgProc = nullptr;
			this->ThreadLock = (UBINT)nullptr;
			this->hGDC = 0;
		}
		else throw new std::exception("Window creation failed.",1);
	}
	Window::~Window(){
		nsBasic::ObjGeneral *Obj1 = (nsBasic::ObjGeneral *)this->ChildObjRing, *Obj2;
		if (NULL != Obj1){
			do{
				Obj2 = (nsBasic::ObjGeneral *)Obj1->NextObj;
				DestroyObject(Obj1);
				Obj1 = Obj2;
			} while (Obj1 != this->ChildObjRing);
			this->ChildObjRing = 0;
		}
		this->hWnd=0;
	}
	void Window::Show(){
		ShowWindow((HWND)this->hWnd, SW_SHOW);
	}
//	extern UBINT _cdecl MsgProc_GDIWindow(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
//		Window *Wnd=(Window *)lpObj;
//		nsBasic::ObjGeneral *Obj1=(nsBasic::ObjGeneral *)Wnd->ChildObjRing,*Obj2;
//
//		RECT WndRect;
//		GraphicDevice *MyDevice=(GraphicDevice *)Wnd->hGDC;
//		HDC CurDC;
//		switch(Msg){
//			case MSG_WNDSIZE:
//				{
//					va_list args;
//					va_start(args,Msg);
//					UBINT Width=va_arg(args,UBINT);
//					UBINT Height=va_arg(args,UBINT);
//					DeleteObject((HBITMAP)MyDevice->hBuffer);
//					MyDevice->hBuffer=(UBINT)CreateCompatibleBitmap((HDC)MyDevice->hDC,(int)Width,(int)Height);
//					SelectObject((HDC)MyDevice->hDC,(HBITMAP)MyDevice->hBuffer);
//					if(NULL!=Obj1){
//						do{
//							if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_REPOS, 0, 0, Width, Height);
//							Obj1=(nsBasic::ObjGeneral *)Obj1->NextObj;
//						}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
//					}
//					return 0;
//				}
//			case MSG_PAINT:
//				GetClientRect((HWND)Wnd->hWnd,&WndRect);
//				if(NULL!=Obj1){
//
//#if defined LIBENV_SYS_INTELX86
//					do{
//						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
//						if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_SPAINT_DWORD, Wnd->hGDC, &WndRect);
//					}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
//#elif defined LIBENV_SYS_INTELX64
//					do{
//						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
//						if(0!=Obj1->MsgProc)SendMsg(Obj1,MSG_SPAINT_DWORD,Wnd->hGDC,&WndRect);
//					}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
//					do{
//						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
//						if(0!=Obj1->MsgProc)SendMsg(Obj1,MSG_SPAINT_DWORDEX,Wnd->hGDC,&WndRect);
//					}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
//#endif
//					do{
//						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
//						if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_PAINT, Wnd->hGDC, &WndRect);
//					}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
//				}
//				CurDC=GetDC((HWND)Wnd->hWnd);
//				BitBlt(CurDC,WndRect.left,WndRect.top,WndRect.right-WndRect.left,WndRect.bottom-WndRect.top,(HDC)MyDevice->hDC,0,0,SRCCOPY);
//				ReleaseDC((HWND)Wnd->hWnd,CurDC);
//				ValidateRect((HWND)Wnd->hWnd,NULL);
//				return 0;
//			case MSG_DESTROY:
//				if(NULL!=Obj1){
//					do{
//						Obj2=(nsBasic::ObjGeneral *)Obj1->NextObj;
//						DestroyObject(Obj1);
//						Obj1=Obj2;
//					}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
//					Wnd->ChildObjRing=0;
//				}
//				DeleteObject((HBITMAP)MyDevice->hBuffer);
//				DeleteDC((HDC)MyDevice->hDC);
//				nsBasic::FreeThreadObject((nsBasic::ObjGeneral *)MyDevice);
//				Wnd->hWnd=0;
//				Wnd->MsgProc=(UBINT)NULL;
//				Wnd->hGDC=0;
//				PostQuitMessage(0);
//				return 0;
//			default:
//				return 0;
//		}
//	}
//	extern UBINT CreateGDIWindow(Window *lpWindow,Rect *WndRect){
//		UBINT flag=1;
//		HDC TmpDC;
//		Rect TmpRect;
//
//		if(0==CreateRawWindow(lpWindow,WndRect)){
//			GraphicDevice *MyDevice=(GraphicDevice *)nsBasic::AllocThreadObject();
//			if(NULL!=MyDevice){
//				TmpDC=GetDC((HWND)lpWindow->hWnd);
//				if(NULL!=TmpDC){
//					MyDevice->hDC=(UBINT)CreateCompatibleDC(TmpDC);
//					if(0!=MyDevice->hDC){
//						GetClientRect((HWND)lpWindow->hWnd,(LPRECT)&TmpRect); //In case that NULL==WndRect
//						MyDevice->hBuffer=(UBINT)CreateCompatibleBitmap(TmpDC,TmpRect.XMax-TmpRect.XMin,TmpRect.YMax-TmpRect.YMin);
//						if(0!=MyDevice->hBuffer){
//							SelectObject((HDC)MyDevice->hDC,(HBITMAP)MyDevice->hBuffer);
//							MyDevice->ChildObjRing=0;
//							MyDevice->Type=OBJ_GRAPHICDEVICE;
//							MyDevice->MsgProc=(UBINT)NULL;
//							MyDevice->ThreadLock=(UBINT)NULL;
//							lpWindow->MsgProc=(UBINT)MsgProc_GDIWindow;
//							lpWindow->hGDC=(UBINT)MyDevice;
//							flag=0;
//						}
//						else DeleteDC((HDC)MyDevice->hDC);
//					}
//					ReleaseDC((HWND)lpWindow->hWnd,TmpDC);
//				}
//				if(flag)nsBasic::FreeThreadObject((nsBasic::ObjGeneral *)MyDevice);
//			}
//			if(flag)DestroyWindow((HWND)lpWindow->hWnd);
//		}
//		return flag;
//	}

#if defined LIBENV_GLIB_OPENGL
	UBINT _cdecl GLWindow::_MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		GLWindow *Wnd = (GLWindow *)lpObj;
		nsBasic::ObjGeneral *Obj1=(nsBasic::ObjGeneral *)Wnd->ChildObjRing;
		
		switch(Msg){
			case MSG_WNDSIZE:
				{
					va_list args;
					va_start(args,Msg);
					UBINT Width=va_arg(args,UBINT);
					UBINT Height=va_arg(args,UBINT);
					glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);
					if(NULL!=Obj1){
						do{
							if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_REPOS, 0, 0, Width, Height);
							Obj1=(nsBasic::ObjGeneral *)Obj1->NextObj;
						}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
					}
					return 0;
				}
			case MSG_PAINT:
				{
					RECT WndRect;
					HDC WndDC;
					GetClientRect((HWND)Wnd->hWnd,&WndRect);
					if(NULL!=Obj1){
#if defined LIBENV_SYS_INTELX86
						do{
							Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
							if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_SPAINT_DWORD, Wnd->hGDC, &WndRect);
						}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
						do{
							Obj1 = (nsBasic::ObjGeneral *)Obj1->PrevObj;
							if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_PAINT, Wnd->hGDC, &WndRect);
						} while (Obj1 != (nsBasic::ObjGeneral *)Wnd->ChildObjRing);
#elif defined LIBENV_SYS_INTELX64
						nsGUI::Rect WndRect64;
						WndRect64.XMin = WndRect.left;
						WndRect64.YMin = WndRect.top;
						WndRect64.XMax = WndRect.right;
						WndRect64.YMax = WndRect.bottom;
						do{
							Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
							if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_SPAINT_DWORD, Wnd->hGDC, &WndRect64);
						}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
						do{
							Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
							if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_SPAINT_DWORDEX, Wnd->hGDC, &WndRect64);
						}while(Obj1!=(nsBasic::ObjGeneral *)Wnd->ChildObjRing);
						do{
							Obj1 = (nsBasic::ObjGeneral *)Obj1->PrevObj;
							if (0 != Obj1->MsgProc)SendMsg(Obj1, MSG_PAINT, Wnd->hGDC, &WndRect64);
						} while (Obj1 != (nsBasic::ObjGeneral *)Wnd->ChildObjRing);
#endif
					}
					WndDC=GetDC((HWND)Wnd->hWnd);
					SwapBuffers(WndDC);
					ReleaseDC((HWND)Wnd->hWnd,WndDC);
					ValidateRect((HWND)Wnd->hWnd, nullptr);
				}
				return 0;
			case MSG_DESTROY:
				throw new std::exception("this object is only allowed to be created locally.", 1);
				return 1;
			default:
				return 0;
		}
	}
	GLWindow::GLWindow(Rect *WndRect, UBCHAR *Title) :Window(WndRect, Title){
		this->hGDC = (UBINT)new GLDevice((HWND)this->hWnd);
		this->MsgProc = GLWindow::_MsgProc;
	}
	GLDevice *GLWindow::GetGraphicDevice(){ return (GLDevice *)this->hGDC; }
	GLWindow::~GLWindow(){
		delete (GLDevice *)this->hGDC;
		this->MsgProc = nullptr;
		this->hGDC = 0;
	}
#endif

	extern UBINT SimpleMsgPump(){
		MSG SysMsg;
		UBINT UsrMsg,Arg1,Arg2;
		nsBasic::ObjGeneral *TgtObj;
		MouseStat MyMouseStat;
		UBINT PaintSwitch=0;

		while(1){
			if(0==nsBasic::MsgQueue_Read(&(nsBasic::GetThreadExtInfo()->MsgManager),&UsrMsg)){
				switch(UsrMsg){
					case MSG_WNDSIZE:
						nsBasic::MsgQueue_Read(&(nsBasic::GetThreadExtInfo()->MsgManager),&Arg1);
						nsBasic::MsgQueue_Read(&(nsBasic::GetThreadExtInfo()->MsgManager),&Arg2);
						TgtObj=nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager),IO_VIDEO_OUT);
						if (NULL != TgtObj && 0 != TgtObj->MsgProc)SendMsg(TgtObj, MSG_WNDSIZE, Arg1, Arg2);
						break;
					case MSG_WNDDESTROY:
						/*TgtObj = nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager), IO_VIDEO_OUT);
						if (NULL != TgtObj && 0 != TgtObj->MsgProc)SendMsg(TgtObj, MSG_DESTROY);
						break;*/
						PostQuitMessage(0); //Currently we only support single window programs.
					case MSG_PAINT:
						PaintSwitch=1;
						break;
					case MSG_COMMAND:
					case MSG_SCROLL:
					case MSG_DRAGSTOP:
						nsBasic::MsgQueue_Read(&(nsBasic::GetThreadExtInfo()->MsgManager),&Arg1);
						TgtObj=nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager),IO_USER);
						if (NULL != TgtObj && 0 != TgtObj->MsgProc)SendMsg(TgtObj, UsrMsg, Arg1);
						break;
					case MSG_DRAG3D:
						{
							double Drag3DData[3];
							nsBasic::MsgQueue_Read(&(nsBasic::GetThreadExtInfo()->MsgManager),&Arg1);
							nsBasic::MsgQueue_BulkRead(&(nsBasic::GetThreadExtInfo()->MsgManager),Drag3DData,3*sizeof(double));
							TgtObj=nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager),IO_USER);
							if (NULL != TgtObj && 0 != TgtObj->MsgProc)SendMsg(TgtObj, MSG_DRAG3D, Arg1, Drag3DData[0], Drag3DData[1], Drag3DData[2]);
						}
						break;
					default:
						break;
				}
			}
			else if(PeekMessage(&SysMsg,NULL,0,0,PM_REMOVE)){ //GetQueueStatus(QS_ALLEVENTS) may block messages like WM_SYSCOMMAND(minimizing).
				if(WM_QUIT==SysMsg.message)break;
				TranslateMessage(&SysMsg); //This is a must because people may use IME
				switch(SysMsg.message){
					case WM_LBUTTONUP:
					case WM_LBUTTONDOWN:
					case WM_MBUTTONUP:
					case WM_MBUTTONDOWN:
					case WM_RBUTTONUP:
					case WM_RBUTTONDOWN:
					case WM_MOUSEMOVE:
					case WM_MOUSEWHEEL:
						TgtObj=nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager),IO_MOUSE);
						if(NULL!=TgtObj && 0!=TgtObj->MsgProc){
							MyMouseStat.XPos=LOWORD(SysMsg.lParam);
							MyMouseStat.YPos=HIWORD(SysMsg.lParam);
							MyMouseStat.ZPos=HIWORD(SysMsg.wParam);
							if(LOWORD(SysMsg.wParam)&MK_LBUTTON)MyMouseStat.BtnStat=MBTN_L;else MyMouseStat.BtnStat=0;
							if(LOWORD(SysMsg.wParam)&MK_MBUTTON)MyMouseStat.BtnStat|=MBTN_M;
							if(LOWORD(SysMsg.wParam)&MK_RBUTTON)MyMouseStat.BtnStat|=MBTN_R;
							((nsBasic::MSGPROC)TgtObj->MsgProc)(TgtObj,MSG_MOUSEEVENT,&MyMouseStat);
						}
						break;
					case WM_KEYDOWN:
					case WM_KEYUP:
						TgtObj=nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager),IO_KEYBD);
						if (NULL != TgtObj && 0 != TgtObj->MsgProc)SendMsg(TgtObj, MSG_KEYBDEVENT);
						break;
					case WM_PAINT:
						TgtObj=nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager),IO_VIDEO_OUT);
						if (NULL != TgtObj && 0 != TgtObj->MsgProc)SendMsg(TgtObj, MSG_PAINT);
						PaintSwitch=0;
						break;
					default:
						DispatchMessage(&SysMsg);
						break;
				}
			}
			else if(PaintSwitch){
				TgtObj=nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager),IO_VIDEO_OUT);
				if (NULL != TgtObj && 0 != TgtObj->MsgProc)SendMsg(TgtObj, MSG_PAINT);
				PaintSwitch=0;
			}
			else{
				TgtObj=nsBasic::GetMsgPort(&(nsBasic::GetThreadExtInfo()->MsgManager),IO_USER);
				if (NULL != TgtObj && 0 != TgtObj->MsgProc)SendMsg(TgtObj, MSG_IDLE);
				nsBasic::YieldCurThread();
			}
		}
		return 0;
	}
	extern UBINT _cdecl MsgProc_GraphicObjSet(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);
		nsBasic::ObjSet *MyScene=(nsBasic::ObjSet *)lpObj;
		nsBasic::ObjGeneral *Obj1=(nsBasic::ObjGeneral *)MyScene->ChildObjRing,*Obj2;

		switch(Msg){
			case MSG_SPAINT_DWORD:
			case MSG_SPAINT_DWORDEX:
			case MSG_PAINT:
				if(NULL!=Obj1){
					UBINT hDC=va_arg(args,UBINT);
					Rect *lpRect=va_arg(args,Rect *);
					do{
						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
						if (0 != Obj1->MsgProc)SendMsg(Obj1, Msg, hDC, lpRect);
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
				}
				return 0;
			case MSG_DESTROY:
				if(NULL!=Obj1){
					do{
						Obj2=(nsBasic::ObjGeneral *)Obj1->NextObj;
						DestroyObject(Obj1);
						Obj1=Obj2;
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
					MyScene->ChildObjRing=0;
				}
				return 0;
			default:
				return 0;
		}
	}

#if defined LIBENV_GLIB_OPENGL
	extern UBINT _cdecl MsgProc_GL3DScene(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);
		nsBasic::ObjSet *MyScene=(nsBasic::ObjSet *)lpObj;
		nsBasic::ObjGeneral *Obj1=(nsBasic::ObjGeneral *)MyScene->ChildObjRing,*Obj2;

		switch(Msg){
			case MSG_SPAINT_DWORD:
			case MSG_SPAINT_DWORDEX:
			case MSG_PAINT:
				if(NULL!=Obj1){
					UBINT hDC=va_arg(args,UBINT);
					Rect *lpRect=va_arg(args,Rect *);
					glViewport((GLint)lpRect->XMin, (GLint)lpRect->YMin, (GLsizei)(lpRect->XMax - lpRect->XMin), (GLsizei)(lpRect->YMax - lpRect->YMin));
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
					do{
						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
						if (0 != Obj1->MsgProc)SendMsg(Obj1, Msg, hDC, lpRect);
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
				}
				return 0;
			case MSG_DESTROY:
				if(NULL!=Obj1){
					do{
						Obj2=(nsBasic::ObjGeneral *)Obj1->NextObj;
						DestroyObject(Obj1);
						Obj1=Obj2;
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
					MyScene->ChildObjRing=0;
				}
				return 0;
			default:
				return 0;
		}
	}
#endif

	/*extern UBINT _cdecl MsgProc_GDICtrlScene(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);
		nsBasic::ObjSet *MyScene=(nsBasic::ObjSet *)lpObj;
		nsBasic::ObjGeneral *Obj1=(nsBasic::ObjGeneral *)MyScene->ChildObjRing,*Obj2;

		switch(Msg){
			case MSG_REPOS:
				if(NULL!=Obj1){
					do{
						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
						if (0 != Obj1->MsgProc)SendMsg(Obj1, Msg, va_arg(args, UBINT), va_arg(args, UBINT), va_arg(args, UBINT), va_arg(args, UBINT));
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
				}
				return 0;
			case MSG_SPAINT_DWORD:
			case MSG_SPAINT_DWORDEX:
			case MSG_PAINT:
				if(NULL!=Obj1){
					UBINT hDC=va_arg(args,UBINT);
					Rect *lpRect=va_arg(args,Rect *);
					do{
						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
						if (0 != Obj1->MsgProc)SendMsg(Obj1, Msg, hDC, lpRect);
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
				}
				return 0;
			case MSG_DESTROY:
				if(NULL!=Obj1){
					do{
						Obj2=(nsBasic::ObjGeneral *)Obj1->NextObj;
						DestroyObject(Obj1);
						Obj1=Obj2;
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
					MyScene->ChildObjRing=0;
				}
				return 0;
			default:
				return 0;
		}
	}
	extern nsBasic::ObjSet *CreateGDIControlScene(Window *lpWindow){
		nsBasic::ObjSet *MyObj=(nsBasic::ObjSet *)nsBasic::AllocThreadObject();
		if(NULL!=MyObj){
			MyObj->ChildObjRing=0;
			MyObj->Type=OBJ_CTRLSCENE;
			MyObj->MsgProc=MsgProc_GDICtrlScene;
			MyObj->ThreadLock=(UBINT)NULL;
			if(NULL!=lpWindow)nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)MyObj,(void **)&(lpWindow->ChildObjRing));
		}
		return MyObj;
	}*/

#if defined LIBENV_GLIB_OPENGL
	extern UBINT _cdecl MsgProc_GLCtrlScene(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);
		nsBasic::ObjSet *MyScene=(nsBasic::ObjSet *)lpObj;
		nsBasic::ObjGeneral *Obj1=(nsBasic::ObjGeneral *)MyScene->ChildObjRing,*Obj2;

		switch(Msg){
			case MSG_REPOS:
				{
					UBINT XPos=va_arg(args,UBINT),YPos=va_arg(args,UBINT),Width=va_arg(args,UBINT),Height=va_arg(args,UBINT);
					if(NULL!=Obj1){
						do{
							Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
							if (0 != Obj1->MsgProc)SendMsg(Obj1, Msg, XPos, YPos, Width, Height);
						}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
					}
				}
				return 0;
			case MSG_SPAINT_DWORD:
			case MSG_SPAINT_DWORDEX:
			case MSG_PAINT:
				if(NULL!=Obj1){
					UBINT hDC=va_arg(args,UBINT);
					Rect *lpRect=va_arg(args,Rect *);
					glViewport((GLint)lpRect->XMin, (GLint)lpRect->YMin, (GLsizei)(lpRect->XMax - lpRect->XMin), (GLsizei)(lpRect->YMax - lpRect->YMin));
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					glOrtho((GLdouble)lpRect->XMin, (GLdouble)lpRect->XMax, (GLdouble)lpRect->YMax, (GLdouble)lpRect->YMin, -1.0, 1.0);
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
					glDisable(GL_DEPTH_TEST);
					do{
						Obj1=(nsBasic::ObjGeneral *)Obj1->PrevObj;
						if (0 != Obj1->MsgProc)SendMsg(Obj1, Msg, hDC, lpRect);
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
				}
				return 0;
			case MSG_DESTROY:
				if(NULL!=Obj1){
					do{
						Obj2=(nsBasic::ObjGeneral *)Obj1->NextObj;
						DestroyObject(Obj1);
						Obj1=Obj2;
					}while(Obj1!=(nsBasic::ObjGeneral *)MyScene->ChildObjRing);
					MyScene->ChildObjRing=0;
				}
				return 0;
			default:
				return 0;
		}
	}
#endif

	//extern UBINT _cdecl MsgProc_GLSPaintBuffer(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
	//	va_list args;
	//	va_start(args,Msg);
	//	SPaintBuffer *MyBuffer=(SPaintBuffer *)lpObj;

	//	switch(Msg){
	//		case MSG_SPAINT_DWORD:
	//			{
	//				UBINT hDC=va_arg(args,UBINT);
	//				Rect *lpRect=va_arg(args,Rect *);
	//				if(lpRect->XMax-lpRect->XMin != MyBuffer->Width || lpRect->YMax-lpRect->YMin != MyBuffer->Height){
	//					nsBasic::MemFree(nsBasic::GetThreadExtInfo()->MemAllocator,(void *)(MyBuffer->hBuffer),MyBuffer->Width*MyBuffer->Height*sizeof(UBINT));
	//					MyBuffer->Width=lpRect->XMax-lpRect->XMin;
	//					MyBuffer->Height=lpRect->YMax-lpRect->YMin;
	//					MyBuffer->hBuffer=(UBINT)nsBasic::MemAlloc(nsBasic::GetThreadExtInfo()->MemAllocator,MyBuffer->Width*MyBuffer->Height*sizeof(UBINT));
	//				}
	//				if(0==MyBuffer->hBuffer){
	//					MyBuffer->Width=0;
	//					MyBuffer->Height=0;
	//				}
	//				else{
	//					//glPixelStorei(GL_PACK_LSB_FIRST,true);
	//					glReadBuffer(GL_BACK);
	//					glReadPixels(lpRect->XMin,lpRect->YMin,MyBuffer->Width,MyBuffer->Height,GL_RGBA,GL_UNSIGNED_BYTE,(GLvoid *)MyBuffer->hBuffer);
	//				}
	//				return 0;
	//			}
	//		case MSG_SPAINT_DWORDEX:
	//			{
	//				UBINT hDC=va_arg(args,UBINT);
	//				Rect *lpRect=va_arg(args,Rect *);

	//				if(0==MyBuffer->hBuffer){
	//					MyBuffer->Width=0;
	//					MyBuffer->Height=0;
	//				}
	//				else{
	//					glReadBuffer(GL_BACK);
	//					glReadPixels(lpRect->XMin,lpRect->YMin,MyBuffer->Width,MyBuffer->Height,GL_RGBA,GL_UNSIGNED_BYTE,(GLvoid *)(MyBuffer->hBuffer+MyBuffer->Width*MyBuffer->Height*4));
	//				}
	//				return 0;
	//			}
	//		case MSG_DESTROY:
	//			nsBasic::MemFree(nsBasic::GetThreadExtInfo()->MemAllocator,(void *)(MyBuffer->hBuffer),MyBuffer->Width*MyBuffer->Height*sizeof(UBINT));
	//			MyBuffer->hBuffer=0;
	//			MyBuffer->Width=0;
	//			MyBuffer->Height=0;
	//		default:
	//			return 0;
	//	}
	//}
	//extern SPaintBuffer *CreateGLSPaintBuffer(Window *lpWindow){
	//	SPaintBuffer *MyObj=(SPaintBuffer *)nsBasic::AllocThreadObject();
	//	if(NULL!=MyObj){
	//		MyObj->Width=0;
	//		MyObj->Height=0;
	//		MyObj->Type=OBJ_CTRLSCENE;
	//		MyObj->MsgProc=(UBINT)MsgProc_GLSPaintBuffer;
	//		MyObj->ThreadLock=(UBINT)NULL;
	//		MyObj->hBuffer=0;
	//		if(NULL!=lpWindow)nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)MyObj,(void **)&(lpWindow->ChildObjRing));
	//	}
	//	return MyObj;
	//}
#else
#endif
	
	UBINT _cdecl ControlManager::MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);

		nsGUI::ControlManager *CtrlMgr=(nsGUI::ControlManager *)lpObj;
		nsBasic::ObjGeneral *Obj1 = (nsBasic::ObjGeneral *)CtrlMgr->ObjInterFace.ChildObjRing;

		switch(Msg){
			case MSG_MOUSEEVENT:
				{
					MouseStat *MyMouseStat=va_arg(args,MouseStat *);
					nsBasic::ObjGeneral *NewMouseFocus=NULL; 
					if(0==(CtrlMgr->LastMBtnStat & MyMouseStat->BtnStat)){ //Then the mouse focus may change.
						if(NULL!=Obj1){
							do{
								if (0 != Obj1->MsgProc)NewMouseFocus = (nsBasic::ObjGeneral *)SendMsg(Obj1, MSG_MOUSECHECK, MyMouseStat);
								if(NULL!=NewMouseFocus)break;
								Obj1=(nsBasic::ObjGeneral *)Obj1->NextObj;
							} while (Obj1 != CtrlMgr->ObjInterFace.ChildObjRing);
						}
						//Now we have the new focus.Say goodbye to the last focus.
						if(NewMouseFocus!=CtrlMgr->MouseFocus){
							if (NULL != CtrlMgr->MouseFocus)SendMsg(CtrlMgr->MouseFocus, MSG_MOUSELEAVE);
							CtrlMgr->MouseFocus=NewMouseFocus;
							if (NULL != NewMouseFocus)SendMsg(NewMouseFocus, MSG_MOUSEENTER);
						}
						if (NULL != NewMouseFocus)SendMsg(CtrlMgr->MouseFocus, MSG_MOUSEEVENT, MyMouseStat);
						if(MyMouseStat->BtnStat>0 && NewMouseFocus!=CtrlMgr->KeybdFocus){
							//Ask the new focus whether it needs keyboard
							if(NULL!=NewMouseFocus){
								if (SendMsg(NewMouseFocus, MSG_KEYBDENTER)>0){
									if (NULL != CtrlMgr->KeybdFocus)SendMsg(CtrlMgr->KeybdFocus, MSG_KEYBDLEAVE);
									CtrlMgr->KeybdFocus=NewMouseFocus;
								}
							}
							else{
								SendMsg(CtrlMgr->KeybdFocus, MSG_KEYBDLEAVE);
								CtrlMgr->KeybdFocus=NULL;
							}
						}
					}
					else{ //The mouse focus won't change.
						if (NULL != CtrlMgr->MouseFocus)SendMsg(CtrlMgr->MouseFocus, MSG_MOUSEEVENT, MyMouseStat);
					}
					CtrlMgr->LastMBtnStat=MyMouseStat->BtnStat;
					return 0;
				}
			case MSG_KEYBDEVENT:
				if (NULL != CtrlMgr->KeybdFocus)SendMsg(CtrlMgr->KeybdFocus, MSG_KEYBDEVENT);
				return 0;
			case MSG_DESTROY:
				throw new std::exception("this object is only allowed to be created locally.", 1);
				return 1;
			default:
				return 0;
		}
	}
	ControlManager::ControlManager(){
		this->ObjInterFace.ChildObjRing=nullptr;
		this->ObjInterFace.Type = OBJ_CONTROL_MANAGER;
		this->ObjInterFace.MsgProc = ControlManager::MsgProc;
		this->ObjInterFace.ThreadLock = (UBINT)nullptr;
		this->MouseFocus = nullptr;
		this->KeybdFocus = nullptr;
		this->LastMBtnStat = 0;
	}
	ControlManager::~ControlManager(){
		nsBasic::ObjGeneral *Obj1 = (nsBasic::ObjGeneral *)this->ObjInterFace.ChildObjRing, *Obj2;
		if (NULL != Obj1){
			do{
				Obj2 = (nsBasic::ObjGeneral *)Obj1->NextObj;
				DestroyObject(Obj1);
				Obj1 = Obj2;
			} while (Obj1 != this->ObjInterFace.ChildObjRing);
			this->ObjInterFace.ChildObjRing = nullptr;
		}
	}
	/*extern UBINT _cdecl MsgProc_BufferHitMap(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		if(MSG_MOUSECHECK==Msg){
			va_list args;
			va_start(args,Msg);
			nsBasic::ObjSetExt *MyBitmap=(nsBasic::ObjSetExt *)lpObj;
			MouseStat *MyMouseStat=va_arg(args,MouseStat *);
			SPaintBuffer *Buffer=(SPaintBuffer *)MyBitmap->ExtInfo;
			if(0==Buffer->hBuffer)return 0;
			else{
				UBINT Addr=*(UBINT *)(Buffer->hBuffer+(Buffer->Width*(Buffer->Height-MyMouseStat->YPos-1)+MyMouseStat->XPos)*4);
#if defined LIBENV_SYS_INTELX64
				UBINT Addr+=(*(UBINT *)(Buffer->hBuffer+(Buffer->Width*(2*Buffer->Height-MyMouseStat->YPos-1)+MyMouseStat->XPos)*4))<<32;
#endif
				return Addr;
			}
		}
		else return 0;
	}
	extern nsBasic::ObjSetExt *CreateBufferHitMap(nsBasic::ObjSet *ParentObj,SPaintBuffer *lpBuffer){
		if(NULL==lpBuffer)return NULL;
		nsBasic::ObjSetExt *MyHitMap=(nsBasic::ObjSetExt *)nsBasic::AllocThreadObject();
		if(NULL!=MyHitMap){
			MyHitMap->ChildObjRing=0;
			MyHitMap->Type=OBJ_HITMAP_BUFFER;
			MyHitMap->MsgProc=(UBINT)MsgProc_BufferHitMap;
			MyHitMap->ThreadLock=(UBINT)NULL;
			MyHitMap->ExtInfo=(UBINT)lpBuffer;
			if(NULL!=ParentObj)nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)MyHitMap,&(ParentObj->ChildObjRing));
		}
		return MyHitMap;
	}*/
	UBINT _cdecl RectHitRgn::_MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		if(MSG_MOUSECHECK==Msg){
			va_list args;
			va_start(args,Msg);
			RectHitRgn *MyRgn=(RectHitRgn *)lpObj;
			Rect *MyRect=(Rect *)(MyRgn->lpRect);
			MouseStat *MyMouseStat=va_arg(args,MouseStat *);
			if(MyMouseStat->XPos>=((Rect *)(MyRgn->lpRect))->XMin &&
			   MyMouseStat->XPos<((Rect *)(MyRgn->lpRect))->XMax &&
			   MyMouseStat->YPos>=((Rect *)(MyRgn->lpRect))->YMin &&
			   MyMouseStat->YPos<((Rect *)(MyRgn->lpRect))->YMax){
				return (UBINT)(MyRgn->TgtCtrl);
			}
			else return 0;
		}
		else return 0;
	}
	RectHitRgn::RectHitRgn(nsBasic::ObjSet *ParentObj, nsBasic::ObjGeneral *TgtCtrl, Rect *HitRect){
		this->ChildObjRing = nullptr;
		this->lpRect = HitRect;
		this->Type = OBJ_HITRGN_RECT;
		this->MsgProc = RectHitRgn::_MsgProc;
		this->ThreadLock = (UBINT)nullptr;
		this->TgtCtrl = TgtCtrl;
		if (NULL != ParentObj)nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)this, &(ParentObj->ChildObjRing));
	}

#if defined LIBENV_GLIB_OPENGL
	extern UBINT GLDrawText(nsGUIText::FontRaster *lpRaster,const Rect *lpRect,const char *lpText,UBINT Coding,UBINT Layout){
		nsFile::MemStream RS((void * const)lpText, UBINT_MAX);
		nsFile::DecoderStream Decoder(&RS.AsReadStream(), Coding);

		//GetTextMetrics()
		UINT4b CodePoint;
		UBINT CurTex = 0, ReloadTex;
		BINT XMin = 0, XMax = 0, YMin = ((FT_Face)(lpRaster->ExtInfo))->size->metrics.ascender >> 6, YMax = ((FT_Face)(lpRaster->ExtInfo))->size->metrics.descender >> 6;
		BINT PXMin,PXMax,PYMin,PYMax;
		GLfloat TXMin,TXMax,TYMin,TYMax;
		GLuint TexID = 0;

		nsGUIText::GlyphInfo *MyGlyph;
		while(1){
			if (false == Decoder.Read(&CodePoint))return 1;
			if(0==CodePoint)break;
			MyGlyph=lpRaster->LoadGlyph(CodePoint,&ReloadTex);
			if(NULL==MyGlyph)return 1;
			XMax+=MyGlyph->Adv_H;
		}

		switch(Layout&0x3){
			case TEXTLAYOUT_HORI_LEFT:
				XMin = lpRect->XMin;
				break;
			case TEXTLAYOUT_HORI_MEDIUM:
				XMin = (lpRect->XMax + lpRect->XMin - XMax) / 2;
				break;
			case TEXTLAYOUT_HORI_RIGHT:
				XMin = lpRect->XMax - XMax;
				break;
		}
		XMax+=XMin;
		if (XMax > lpRect->XMax)XMax = lpRect->XMax;
		//Calculate baseline position as YMin
		switch(Layout&0xC){
			case TEXTLAYOUT_VERT_TOP:
				YMin = lpRect->YMin + YMin;
				break;
			case TEXTLAYOUT_VERT_MEDIUM:
				YMin = (lpRect->YMax + lpRect->YMin + YMax + YMin) / 2;
				break;
			case TEXTLAYOUT_VERT_BOTTOM:
				YMin = lpRect->YMax + YMax;
				break;
		}

		RS.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glGenTextures(1, &TexID);
		glBindTexture(GL_TEXTURE_2D, TexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
		do{
			if (false == Decoder.Read(&CodePoint))return 1;
			if(0==CodePoint)break;
			MyGlyph=lpRaster->LoadGlyph(CodePoint,&ReloadTex);
			if(NULL==MyGlyph)return 1;
			if(ReloadTex || MyGlyph->BmpIndex!=CurTex){
				glEnd();
				glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA,0x100,0x100,0,GL_ALPHA,GL_UNSIGNED_BYTE,(void *)lpRaster->GetTextBitmap(MyGlyph->BmpIndex));
				CurTex=MyGlyph->BmpIndex;
				glBegin(GL_QUADS);
			}

			PXMin = XMin + MyGlyph->XBearing_H;
			PXMax = PXMin + MyGlyph->XSpan;
			PYMin = YMin - MyGlyph->YBearing_H;
			PYMax = PYMin + MyGlyph->YSpan;
			TXMin = MyGlyph->XPos;
			TXMax = TXMin + MyGlyph->XSpan;
			TYMin = MyGlyph->YPos;
			TYMax = TYMin + MyGlyph->YSpan;

			if(PXMin<lpRect->XMin){
				TXMin+=lpRect->XMin-PXMin;
				PXMin=lpRect->XMin;
			}
			if(PXMax>lpRect->XMax){
				TXMax-=PXMax-lpRect->XMax;
				PXMax=lpRect->XMax;
			}
			if(PYMin<lpRect->YMin){
				TYMin+=lpRect->YMin-PYMin;
				PYMin=lpRect->YMin;
			}
			if(PYMax>lpRect->YMax){
				TYMax-=PYMax-lpRect->YMax;
				PYMax=lpRect->YMax;
			}
			TXMin/=0x100;
			TXMax/=0x100;
			TYMin/=0x100;
			TYMax/=0x100;

			if(PXMax>PXMin && PYMax>PYMin){
				glTexCoord2f(TXMin, TYMin); glVertex2i((GLint)PXMin, (GLint)PYMin);
				glTexCoord2f(TXMin, TYMax); glVertex2i((GLint)PXMin, (GLint)PYMax);
				glTexCoord2f(TXMax, TYMax); glVertex2i((GLint)PXMax, (GLint)PYMax);
				glTexCoord2f(TXMax, TYMin); glVertex2i((GLint)PXMax, (GLint)PYMin);
			}
			XMin+=MyGlyph->Adv_H;
		}while(XMin<XMax);
		glEnd();
		if (0 != TexID)glDeleteTextures(1, &TexID);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		return 0;
	}
	extern UBINT GLDrawText_RefPoint(nsGUIText::FontRaster *lpRaster, const float *RefPoint, const char *lpText, UBINT Coding, UBINT Layout){
		nsFile::MemStream RS((void * const)lpText, UBINT_MAX);
		nsFile::DecoderStream Decoder(&RS.AsReadStream(), Coding);

		//GetTextMetrics()
		UINT4b CodePoint;
		UBINT CurTex = 0, ReloadTex;
		BINT XMin = 0, XMax = 0, YMin = ((FT_Face)(lpRaster->ExtInfo))->size->metrics.ascender >> 6, YMax = ((FT_Face)(lpRaster->ExtInfo))->size->metrics.descender >> 6;
		BINT PXMin, PXMax, PYMin, PYMax;
		GLfloat TXMin, TXMax, TYMin, TYMax;
		GLuint TexID = 0;

		nsGUIText::GlyphInfo *MyGlyph;
		while (1){
			if (false == Decoder.Read(&CodePoint))return 1;
			if (0 == CodePoint)break;
			MyGlyph = lpRaster->LoadGlyph(CodePoint, &ReloadTex);
			if (NULL == MyGlyph)return 1;
			XMax += MyGlyph->Adv_H;
		}

		switch (Layout & 0x3){
		case TEXTLAYOUT_HORI_LEFT:
			XMin = (BINT)RefPoint[0];
			break;
		case TEXTLAYOUT_HORI_MEDIUM:
			XMin = (BINT)RefPoint[0] - XMax / 2;
			break;
		case TEXTLAYOUT_HORI_RIGHT:
			XMin = (BINT)RefPoint[0] - XMax;
			break;
		}
		XMax += XMin;
		//Calculate baseline position as YMin
		switch (Layout & 0xC){
		case TEXTLAYOUT_VERT_TOP:
			YMin = (BINT)RefPoint[1] - YMin;
			break;
		case TEXTLAYOUT_VERT_MEDIUM:
			YMin = (BINT)RefPoint[1] + (YMax - YMin) / 2;
			break;
		case TEXTLAYOUT_VERT_BOTTOM:
			YMin = (BINT)RefPoint[1] + YMax;
			break;
		}

		RS.Seek(nsBasic::StreamSeekType::FROM_BEGIN, 0);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glGenTextures(1, &TexID);
		glBindTexture(GL_TEXTURE_2D, TexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
		do{
			if (false == Decoder.Read(&CodePoint))return 1;
			if (0 == CodePoint)break;
			MyGlyph = lpRaster->LoadGlyph(CodePoint, &ReloadTex);
			if (NULL == MyGlyph)return 1;
			if (ReloadTex || MyGlyph->BmpIndex != CurTex){
				glEnd();
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 0x100, 0x100, 0, GL_ALPHA, GL_UNSIGNED_BYTE, (void *)lpRaster->GetTextBitmap(MyGlyph->BmpIndex));
				CurTex = MyGlyph->BmpIndex;
				glBegin(GL_QUADS);
			}

			PXMin = XMin + MyGlyph->XBearing_H;
			PXMax = PXMin + MyGlyph->XSpan;
			PYMin = YMin - MyGlyph->YBearing_H;
			PYMax = PYMin + MyGlyph->YSpan;
			TXMin = MyGlyph->XPos;
			TXMax = TXMin + MyGlyph->XSpan;
			TYMin = MyGlyph->YPos;
			TYMax = TYMin + MyGlyph->YSpan;

			TXMin /= 0x100;
			TXMax /= 0x100;
			TYMin /= 0x100;
			TYMax /= 0x100;

			if (PXMax > PXMin && PYMax > PYMin){
				glTexCoord2f(TXMin, TYMin); glVertex2i((GLint)PXMin, (GLint)PYMin);
				glTexCoord2f(TXMin, TYMax); glVertex2i((GLint)PXMin, (GLint)PYMax);
				glTexCoord2f(TXMax, TYMax); glVertex2i((GLint)PXMax, (GLint)PYMax);
				glTexCoord2f(TXMax, TYMin); glVertex2i((GLint)PXMax, (GLint)PYMin);
			}
			XMin += MyGlyph->Adv_H;
		} while (XMin < XMax);
		glEnd();
		if (0 != TexID)glDeleteTextures(1, &TexID);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		return 0;
	}
#endif

	UBINT _cdecl Button::MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);
		Button *Btn=(Button *)lpObj;

		switch(Msg){
			case MSG_MOUSEENTER:
				if (2 != Btn->Data.Status){
					Btn->Data.InnerStatus |= 1;
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
				}
				return 0;
			case MSG_MOUSEEVENT:
				if (2 != Btn->Data.Status){
					MouseStat *MyMouseStat=va_arg(args,MouseStat *);
					UBINT NewStat;
					if(MyMouseStat->XPos>=Btn->Data.CtrlPos.XMin &&
					   MyMouseStat->XPos<Btn->Data.CtrlPos.XMax &&
					   MyMouseStat->YPos>=Btn->Data.CtrlPos.YMin &&
					   MyMouseStat->YPos<Btn->Data.CtrlPos.YMax){
						//Then the mouse is in
						if(MyMouseStat->BtnStat & MBTN_L)NewStat=Btn->Data.InnerStatus|3;else NewStat=(Btn->Data.InnerStatus&4)+1;
					}
					else{
						if(MyMouseStat->BtnStat & MBTN_L)NewStat=Btn->Data.InnerStatus&6;else NewStat=Btn->Data.InnerStatus&4;
					}
					if(NewStat!=Btn->Data.InnerStatus){
						nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
						if(3==(Btn->Data.InnerStatus&3) && 1==(NewStat&3)){
							UBINT MsgArr[2] = { MSG_COMMAND, (UBINT)Btn };
							nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager),MsgArr,sizeof(MsgArr));
						}
						Btn->Data.InnerStatus=NewStat;
					}
					return 0;
				}
			case MSG_MOUSELEAVE:
				if(Btn->Data.InnerStatus&3){
					Btn->Data.InnerStatus&=4;
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				}
				return 0;
			case MSG_KEYBDENTER:
				if (2 == Btn->Data.Status)return 0; else{
					if (0 == (Btn->Data.InnerStatus & 4)){
						Btn->Data.InnerStatus |= 4;
						nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
					}
					return 1; //Require keyboard focus
				}
			case MSG_KEYBDEVENT:
				if(GetKeyState(VK_RETURN)&0xFF80){
					UBINT MsgArr[2] = { MSG_COMMAND, (UBINT)Btn };
					nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
				}
				return 0;
			case MSG_KEYBDLEAVE:
				if(Btn->Data.InnerStatus&4){
					Btn->Data.InnerStatus&=3;
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				}
				return 0;
			case MSG_PAINT:
				{
					UBINT hDC=va_arg(args,UBINT);
					Rect *lpRect=va_arg(args,Rect *);
					Btn->Data.PaintFunc(hDC,lpRect,&(Btn->Data));
				}
				return 0;
			case MSG_DESTROY:
				delete Btn;
				return 0;
			default:
				return 0;
		}
	}
	/*void Button::Draw_GDI(UBINT hDC, Rect *LimitRect, SimpleCtrlData *BtnData){
		nsGUI::GraphicDevice *MyDevice=(nsGUI::GraphicDevice *)hDC;
		HBRUSH MyBrush=CreateSolidBrush(RGB(128-(BtnData->InnerStatus&3)*25,128-(BtnData->InnerStatus&3)*25,255));
		FillRect((HDC)MyDevice->hDC,(RECT *)&(BtnData->CtrlPos),MyBrush);
		if(BtnData->InnerStatus&4){
			FrameRect((HDC)MyDevice->hDC,(RECT *)&(BtnData->CtrlPos),(HBRUSH)GetStockObject(BLACK_BRUSH));
		}
		DeleteObject(MyBrush);
	}*/

#if defined LIBENV_GLIB_OPENGL
	void Button::Draw_GL(UBINT hDC,Rect *LimitRect,SimpleCtrlData *BtnData){
		if(BtnData->InnerStatus&4){
			glColor3f(0.2f,0.2f,0.5f);
			glBegin(GL_QUADS);
			glVertex2i((GLint)BtnData->CtrlPos.XMin, (GLint)BtnData->CtrlPos.YMin);
			glVertex2i((GLint)BtnData->CtrlPos.XMin, (GLint)BtnData->CtrlPos.YMax);
			glVertex2i((GLint)BtnData->CtrlPos.XMax, (GLint)BtnData->CtrlPos.YMax);
			glVertex2i((GLint)BtnData->CtrlPos.XMax, (GLint)BtnData->CtrlPos.YMin);
			glEnd();
			glColor3f(0.5f-(BtnData->InnerStatus&3)*0.1f,0.5f-(BtnData->InnerStatus&3)*0.1f,1.0f);
			glBegin(GL_QUADS);
			glVertex2i((GLint)BtnData->CtrlPos.XMin + 2, (GLint)BtnData->CtrlPos.YMin + 2);
			glVertex2i((GLint)BtnData->CtrlPos.XMin + 2, (GLint)BtnData->CtrlPos.YMax - 2);
			glVertex2i((GLint)BtnData->CtrlPos.XMax - 2, (GLint)BtnData->CtrlPos.YMax - 2);
			glVertex2i((GLint)BtnData->CtrlPos.XMax - 2, (GLint)BtnData->CtrlPos.YMin + 2);
			glEnd();
		}
		else{
			if (2 == BtnData->Status)glColor3f(0.7f, 0.7f, 0.7f);
			else glColor3f(0.5f-(BtnData->InnerStatus&3)*0.1f,0.5f-(BtnData->InnerStatus&3)*0.1f,1.0f);
			glBegin(GL_QUADS);
			glVertex2i((GLint)BtnData->CtrlPos.XMin, (GLint)BtnData->CtrlPos.YMin);
			glVertex2i((GLint)BtnData->CtrlPos.XMin, (GLint)BtnData->CtrlPos.YMax);
			glVertex2i((GLint)BtnData->CtrlPos.XMax, (GLint)BtnData->CtrlPos.YMax);
			glVertex2i((GLint)BtnData->CtrlPos.XMax, (GLint)BtnData->CtrlPos.YMin);
			glEnd();
		}
		if(NULL!=BtnData->CtrlText && NULL!=BtnData->CtrlFont){
			glColor3f(1.0,1.0,1.0);
			GLDrawText(BtnData->CtrlFont, &BtnData->CtrlPos, BtnData->CtrlText, nsCharCoding::UTF8, TEXTLAYOUT_HORI_MEDIUM | TEXTLAYOUT_VERT_MEDIUM);
		}
	}
#endif

	Button::Button(nsBasic::ObjSet *ParentObj){
		this->ObjInterface.Type = OBJ_BUTTON;
		this->ObjInterface.MsgProc = Button::MsgProc;
#if defined LIBENV_GLIB_GDI
		this->Data.PaintFunc=Button::Draw_GDI;
#elif defined LIBENV_GLIB_OPENGL
		this->Data.PaintFunc = Button::Draw_GL;
#endif
		if (NULL != ParentObj)nsBasic::ObjRing_Attach(&(this->ObjInterface), &(ParentObj->ChildObjRing));
	}
	UBINT _cdecl CheckBox::MsgProc(nsBasic::ObjGeneral *lpObj, UBINT Msg, ...){
		va_list args;
		va_start(args, Msg);
		CheckBox *ChkBox=(CheckBox *)lpObj;

		switch (Msg){
		case MSG_MOUSEENTER:
			if (2 != ChkBox->Data.Status){
				ChkBox->Data.InnerStatus |= 1;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
			return 0;
		case MSG_MOUSEEVENT:
			if (2 != ChkBox->Data.Status){
				MouseStat *MyMouseStat = va_arg(args, MouseStat *);
				UBINT NewStat;
				if (MyMouseStat->XPos >= ChkBox->Data.CtrlPos.XMin &&
					MyMouseStat->XPos < ChkBox->Data.CtrlPos.XMax &&
					MyMouseStat->YPos >= ChkBox->Data.CtrlPos.YMin &&
					MyMouseStat->YPos < ChkBox->Data.CtrlPos.YMax){
					//Then the mouse is in
					if (MyMouseStat->BtnStat & MBTN_L)NewStat = ChkBox->Data.InnerStatus | 3; else NewStat = (ChkBox->Data.InnerStatus & 4) + 1;
				}
				else{
					if (MyMouseStat->BtnStat & MBTN_L)NewStat = ChkBox->Data.InnerStatus & 6; else NewStat = ChkBox->Data.InnerStatus & 4;
				}
				if (NewStat != ChkBox->Data.InnerStatus){
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
					if (3 == (ChkBox->Data.InnerStatus & 3) && 1 == (NewStat & 3)){
						ChkBox->Data.Status = !ChkBox->Data.Status;
						UBINT MsgArr[2] = { MSG_COMMAND, (UBINT)ChkBox };
						nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
					}
					ChkBox->Data.InnerStatus = NewStat;
				}
			}
			return 0;
		case MSG_MOUSELEAVE:
			if (ChkBox->Data.InnerStatus & 3){
				ChkBox->Data.InnerStatus &= 4;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
			return 0;
		case MSG_KEYBDENTER:
			if (2 == ChkBox->Data.Status)return 0; else{
				if (0 == (ChkBox->Data.InnerStatus & 4)){
					ChkBox->Data.InnerStatus |= 4;
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
				}
				return 1;
			}
		case MSG_KEYBDEVENT:
			if (GetKeyState(VK_RETURN) & 0xFF80){
				ChkBox->Data.Status = !ChkBox->Data.Status;
				UBINT MsgArr[2] = { MSG_COMMAND, (UBINT)ChkBox };
				nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
			return 0;
		case MSG_KEYBDLEAVE:
			if (ChkBox->Data.InnerStatus & 4){
				ChkBox->Data.InnerStatus &= 3;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager), MSG_PAINT);
			}
			return 0;
		case MSG_PAINT:
		{
			UBINT hDC = va_arg(args, UBINT);
			Rect *lpRect = va_arg(args, Rect *);
			ChkBox->Data.PaintFunc(hDC, lpRect, &(ChkBox->Data));
		}
			return 0;
		case MSG_DESTROY:
			delete ChkBox;
			return 0;
		default:
			return 0;
		}
	}

#if defined LIBENV_GLIB_OPENGL
	void CheckBox::Draw_GL(UBINT hDC, Rect *LimitRect, SimpleCtrlData *ChkBoxInfo){
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
		glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin, (GLint)ChkBoxInfo->CtrlPos.YMin);
		glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin, (GLint)ChkBoxInfo->CtrlPos.YMax);
		glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax, (GLint)ChkBoxInfo->CtrlPos.YMax);
		glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax, (GLint)ChkBoxInfo->CtrlPos.YMin);
		glEnd();
		if (1 == ChkBoxInfo->Status)glColor3f(0.5f - (ChkBoxInfo->InnerStatus & 3)*0.1f, 0.5f - (ChkBoxInfo->InnerStatus & 3)*0.1f, 1.0f);
		else if (ChkBoxInfo->InnerStatus & 3)glColor3f(0.9f - (ChkBoxInfo->InnerStatus & 3)*0.1f, 0.9f - (ChkBoxInfo->InnerStatus & 3)*0.1f, 1.0f);
		if (ChkBoxInfo->InnerStatus & 2){
			glBegin(GL_QUADS);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin + 5, (GLint)ChkBoxInfo->CtrlPos.YMin + 5);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin + 5, (GLint)ChkBoxInfo->CtrlPos.YMax - 5);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax - 5, (GLint)ChkBoxInfo->CtrlPos.YMax - 5);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax - 5, (GLint)ChkBoxInfo->CtrlPos.YMin + 5);
			glEnd();
		}
		else{
			glBegin(GL_QUADS);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin + 3, (GLint)ChkBoxInfo->CtrlPos.YMin + 3);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin + 3, (GLint)ChkBoxInfo->CtrlPos.YMax - 3);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax - 3, (GLint)ChkBoxInfo->CtrlPos.YMax - 3);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax - 3, (GLint)ChkBoxInfo->CtrlPos.YMin + 3);
			glEnd();
		}
		glLineWidth(2.0);
		if (ChkBoxInfo->InnerStatus & 4)glColor3f(0.2f, 0.2f, 0.5f); else glColor3f(0.5f, 0.5f, 0.6f);
		glBegin(GL_LINE_LOOP);
		glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin, (GLint)ChkBoxInfo->CtrlPos.YMin);
		glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin, (GLint)ChkBoxInfo->CtrlPos.YMax);
		glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax, (GLint)ChkBoxInfo->CtrlPos.YMax);
		glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax, (GLint)ChkBoxInfo->CtrlPos.YMin);
		glEnd();
		if (2 == ChkBoxInfo->Status){
			glBegin(GL_LINES);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin, (GLint)ChkBoxInfo->CtrlPos.YMin);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax, (GLint)ChkBoxInfo->CtrlPos.YMax);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMin, (GLint)ChkBoxInfo->CtrlPos.YMax);
			glVertex2i((GLint)ChkBoxInfo->CtrlPos.XMax, (GLint)ChkBoxInfo->CtrlPos.YMin);
			glEnd();
		}
	}
#endif

	CheckBox::CheckBox(nsBasic::ObjSet *ParentObj){
		this->ObjInterface.Type = OBJ_CHECKBOX;
		this->ObjInterface.MsgProc = CheckBox::MsgProc;;
#if defined LIBENV_GLIB_OPENGL
		this->Data.PaintFunc = CheckBox::Draw_GL;
#endif
		if (NULL != ParentObj)nsBasic::ObjRing_Attach(&(this->ObjInterface), &(ParentObj->ChildObjRing));
	}
UBINT _cdecl Slider::MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);
		Slider *MySlider = (Slider *)lpObj;

		switch(Msg){
			case MSG_MOUSEENTER:
				MySlider->Data.InnerStatus|=1;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				return 0;
			case MSG_MOUSEEVENT:
				{
					MouseStat *MyMouseStat=va_arg(args,MouseStat *);
					UBINT NewStat,NewValue=MySlider->Data.CurrentValue;
					if(MyMouseStat->XPos>=MySlider->Data.CtrlPos.XMin &&
					   MyMouseStat->XPos<MySlider->Data.CtrlPos.XMax &&
					   MyMouseStat->YPos>=MySlider->Data.CtrlPos.YMin &&
					   MyMouseStat->YPos<MySlider->Data.CtrlPos.YMax){
						//Then the mouse is in
						if(MyMouseStat->BtnStat & MBTN_L){
							NewStat=MySlider->Data.InnerStatus|3;
							NewValue=2*(MyMouseStat->XPos-MySlider->Data.CtrlPos.XMin)*MySlider->Data.MaxValue+(MySlider->Data.CtrlPos.XMax-MySlider->Data.CtrlPos.XMin+1);
							NewValue/=2*(MySlider->Data.CtrlPos.XMax-MySlider->Data.CtrlPos.XMin);
						}else NewStat=(MySlider->Data.InnerStatus&4)+1;
					}
					else{
						if(MyMouseStat->BtnStat & MBTN_L){
							NewStat=MySlider->Data.InnerStatus&6;
							if(MyMouseStat->XPos<MySlider->Data.CtrlPos.XMin)NewValue=0;
							else if(MyMouseStat->XPos>=MySlider->Data.CtrlPos.XMax)NewValue=MySlider->Data.MaxValue;
							else{
								NewValue=2*(MyMouseStat->XPos-MySlider->Data.CtrlPos.XMin)*MySlider->Data.MaxValue+(MySlider->Data.CtrlPos.XMax-MySlider->Data.CtrlPos.XMin+1);
								NewValue/=2*(MySlider->Data.CtrlPos.XMax-MySlider->Data.CtrlPos.XMin);
							}
						}else NewStat=MySlider->Data.InnerStatus&4;
					}
					if(NewStat!=MySlider->Data.InnerStatus || NewValue!=MySlider->Data.CurrentValue){
						nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
						if(3==(MySlider->Data.InnerStatus&3) && 1==(NewStat&3)){
							UBINT MsgArr[2] = { MSG_COMMAND, (UBINT)(&MySlider->ObjInterface) };
							nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
						}
						if(NewValue!=MySlider->Data.CurrentValue){
							UBINT MsgArr[2] = { MSG_COMMAND, (UBINT)(&MySlider->ObjInterface) };
							nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
						}
						MySlider->Data.InnerStatus=NewStat;
						MySlider->Data.CurrentValue=NewValue;
					}
					return 0;
				}
			case MSG_MOUSELEAVE:
				if(MySlider->Data.InnerStatus&3){
					MySlider->Data.InnerStatus&=4;
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				}
				return 0;
			case MSG_KEYBDENTER:
				if(0==(MySlider->Data.InnerStatus&4)){
					MySlider->Data.InnerStatus|=4;
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				}
				return 1;
			case MSG_KEYBDEVENT:
				if(GetKeyState(VK_LEFT)&0xFF80){
					if(MySlider->Data.CurrentValue>0){
						MySlider->Data.CurrentValue--;
						UBINT MsgArr[2] = { MSG_SCROLL, (UBINT)(&MySlider->ObjInterface) };
						nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
					}
				}
				else if(GetKeyState(VK_RIGHT)&0xFF80){
					if(MySlider->Data.CurrentValue<MySlider->Data.MaxValue){
						MySlider->Data.CurrentValue++;
						UBINT MsgArr[2] = { MSG_SCROLL, (UBINT)(&MySlider->ObjInterface) };
						nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
					}
				}
				return 0;
			case MSG_KEYBDLEAVE:
				if(MySlider->Data.InnerStatus&4){
					MySlider->Data.InnerStatus&=3;
					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				}
				return 0;
			case MSG_PAINT:
				{
					UBINT hDC=va_arg(args,UBINT);
					Rect *lpRect=va_arg(args,Rect *);
					MySlider->Data.PaintFunc(hDC,lpRect,&(MySlider->Data));
				}
				return 0;
			case MSG_DESTROY:
				delete MySlider;
				return 0;
			default:
				return 0;
		}
	}

#if defined LIBENV_GLIB_OPENGL
	void Slider::Draw_GL(UBINT hDC,Rect *LimitRect,SingleScrollData *SliderInfo){
		UBINT SliderX,MidY=(SliderInfo->CtrlPos.YMin+SliderInfo->CtrlPos.YMax)/2;
		if(SliderInfo->CurrentValue>0)SliderX=((SliderInfo->CtrlPos.XMax-SliderInfo->CtrlPos.XMin)*(2*SliderInfo->CurrentValue)-1)/(2*SliderInfo->MaxValue)+SliderInfo->CtrlPos.XMin;else SliderX=SliderInfo->CtrlPos.XMin;
		glLineWidth(1.0);
		glColor3f(0.5,0.5,1.0);
		glBegin(GL_QUADS);
		glVertex2i((GLint)SliderInfo->CtrlPos.XMin, (GLint)MidY - 2);
		glVertex2i((GLint)SliderInfo->CtrlPos.XMin, (GLint)MidY + 2);
		glVertex2i((GLint)SliderX, (GLint)MidY + 2);
		glVertex2i((GLint)SliderX, (GLint)MidY - 2);
		glEnd();
		if (SliderInfo->InnerStatus & 4)glColor3f(0.2f, 0.2f, 0.5f); else glColor3f(0.5f, 0.5f, 0.6f);
		glBegin(GL_LINE_LOOP);
		glVertex2i((GLint)SliderInfo->CtrlPos.XMin, (GLint)MidY - 2);
		glVertex2i((GLint)SliderInfo->CtrlPos.XMin, (GLint)MidY + 2);
		glVertex2i((GLint)SliderInfo->CtrlPos.XMax, (GLint)MidY + 2);
		glVertex2i((GLint)SliderInfo->CtrlPos.XMax, (GLint)MidY - 2);
		glEnd();
		glColor3f(0.5f - (SliderInfo->InnerStatus & 3)*0.1f, 0.5f - (SliderInfo->InnerStatus & 3)*0.1f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2i((GLint)SliderX - 3, (GLint)SliderInfo->CtrlPos.YMin);
		glVertex2i((GLint)SliderX - 3, (GLint)SliderInfo->CtrlPos.YMax);
		glVertex2i((GLint)SliderX + 3, (GLint)SliderInfo->CtrlPos.YMax);
		glVertex2i((GLint)SliderX + 3, (GLint)SliderInfo->CtrlPos.YMin);
		glEnd();
	}
#endif

	Slider::Slider(nsBasic::ObjSet *ParentObj){
		this->ObjInterface.Type = OBJ_SLIDER;
		this->ObjInterface.MsgProc = Slider::MsgProc;;
#if defined LIBENV_GLIB_OPENGL
		this->Data.PaintFunc = Slider::Draw_GL;
#endif
		if (NULL != ParentObj)nsBasic::ObjRing_Attach(&(this->ObjInterface), &(ParentObj->ChildObjRing));
	}
	UBINT _cdecl ProgressBar::MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);
		ProgressBar *ProgBar = (ProgressBar *)lpObj;

		switch(Msg){
			case MSG_PAINT:
				{
					UBINT hDC=va_arg(args,UBINT);
					Rect *lpRect=va_arg(args,Rect *);
					ProgBar->Data.PaintFunc(hDC,lpRect,&(ProgBar->Data));
				}
				return 0;
			case MSG_DESTROY:
				delete ProgBar;
				return 0;
			default:
				return 0;
		}
	}

#if defined LIBENV_GLIB_OPENGL
	void ProgressBar::Draw_GL(UBINT hDC,Rect *LimitRect,SingleScrollData *ProgBarInfo){
		UBINT ProgHead,ProgTail;
		ProgHead=ProgBarInfo->CurrentValue*(ProgBarInfo->CtrlPos.XMax-ProgBarInfo->CtrlPos.XMin)/ProgBarInfo->MaxValue+ProgBarInfo->CtrlPos.XMin;
		if(ProgHead>ProgBarInfo->CtrlPos.XMin+20)ProgTail=ProgHead-20;else ProgTail=ProgBarInfo->CtrlPos.XMin;
		glBegin(GL_QUADS);
		glColor3f(1.0f,1.0f,1.0f);
		glVertex2i((GLint)ProgBarInfo->CtrlPos.XMin, (GLint)ProgBarInfo->CtrlPos.YMin);
		glColor3f(0.9f,0.9f,1.0f);
		glVertex2i((GLint)ProgBarInfo->CtrlPos.XMin, (GLint)ProgBarInfo->CtrlPos.YMax);
		glColor3f(0.8f,0.8f,1.0f);
		glVertex2i((GLint)ProgTail, (GLint)ProgBarInfo->CtrlPos.YMax);
		glColor3f(0.9f,0.9f,1.0f);
		glVertex2i((GLint)ProgTail, (GLint)ProgBarInfo->CtrlPos.YMin);
		glVertex2i((GLint)ProgTail, (GLint)ProgBarInfo->CtrlPos.YMin);
		glColor3f(0.8f,0.8f,1.0f);
		glVertex2i((GLint)ProgTail, (GLint)ProgBarInfo->CtrlPos.YMax);
		glColor3f(0.4f,0.4f,1.0f);
		glVertex2i((GLint)ProgHead, (GLint)ProgBarInfo->CtrlPos.YMax);
		glColor3f(0.5f,0.5f,1.0f);
		glVertex2i((GLint)ProgHead, (GLint)ProgBarInfo->CtrlPos.YMin);
		glEnd();
		glLineWidth(1.0f);
		glColor3f(0.5f,0.5f,0.6f);
		glBegin(GL_LINE_LOOP);
		glVertex2i((GLint)ProgBarInfo->CtrlPos.XMin, (GLint)ProgBarInfo->CtrlPos.YMin);
		glVertex2i((GLint)ProgBarInfo->CtrlPos.XMin, (GLint)ProgBarInfo->CtrlPos.YMax);
		glVertex2i((GLint)ProgBarInfo->CtrlPos.XMax, (GLint)ProgBarInfo->CtrlPos.YMax);
		glVertex2i((GLint)ProgBarInfo->CtrlPos.XMax, (GLint)ProgBarInfo->CtrlPos.YMin);
		glEnd();
	}
#endif

	ProgressBar::ProgressBar(nsBasic::ObjSet *ParentObj){
		this->ObjInterface.Type = OBJ_PROGBAR;
		this->ObjInterface.MsgProc = ProgressBar::MsgProc;;
#if defined LIBENV_GLIB_OPENGL
		this->Data.PaintFunc = ProgressBar::Draw_GL;
#endif
		if (NULL != ParentObj)nsBasic::ObjRing_Attach(&(this->ObjInterface), &(ParentObj->ChildObjRing));
	}

	//extern UBINT _cdecl MsgProc_Point3D(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
	//	va_list args;
	//	va_start(args,Msg);
	//	Control *Point3D=(Control *)lpObj;
	//	Point3DInfo *Ctrl=(Point3DInfo *)Point3D->Ctrl;

	//	switch(Msg){
	//		case MSG_MOUSEENTER:
	//			Ctrl->InnerStatus|=1;
	//			nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
	//			return 0;
	//		case MSG_MOUSEEVENT:
	//			{
	//				MouseStat *MyMouseStat=va_arg(args,MouseStat *);
	//				UBINT NewStat;

	//				if(MyMouseStat->BtnStat & MBTN_L)NewStat=2;else NewStat=1;
	//				if(NewStat!=Ctrl->InnerStatus){
	//					nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
	//					if(2==Ctrl->InnerStatus && 1==NewStat){
	//						UBINT MsgArr[2] = { MSG_COMMAND, (UBINT)Point3D };
	//						nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
	//					}
	//					Ctrl->InnerStatus=NewStat;
	//				}
	//				return 0;
	//			}
	//			return 0;
	//		case MSG_MOUSELEAVE:
	//			Ctrl->InnerStatus=0;
	//			nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
	//			return 0;
	//		case MSG_KEYBDENTER:
	//			return 0; //Doesn't receive keyboard message
	//		case MSG_KEYBDLEAVE:
	//			return 0;
	//		case MSG_SPAINT_DWORD:
	//			{
	//				UBINT hDC=va_arg(args,UBINT);
	//				Rect *lpRect=va_arg(args,Rect *);
	//				Ctrl->PaintFlag=1;
	//				Ctrl->PaintFunc(hDC,lpRect,Point3D);
	//			}
	//			return 0;
	//		case MSG_SPAINT_DWORDEX:
	//			{
	//				UBINT hDC=va_arg(args,UBINT);
	//				Rect *lpRect=va_arg(args,Rect *);
	//				Ctrl->PaintFlag=2;
	//				Ctrl->PaintFunc(hDC,lpRect,Point3D);
	//			}
	//			return 0;
	//		case MSG_PAINT:
	//			{
	//				UBINT hDC=va_arg(args,UBINT);
	//				Rect *lpRect=va_arg(args,Rect *);
	//				Ctrl->PaintFlag=0;
	//				Ctrl->PaintFunc(hDC,lpRect,Point3D);
	//			}
	//			return 0;
	//		case MSG_DESTROY:
	//			nsBasic::MemFree(nsBasic::GetThreadExtInfo()->MemAllocator,(void *)(Point3D->Ctrl),sizeof(Point3DInfo));
	//			return 0;
	//		default:
	//			return 0;
	//	}
	//}
	//extern void GLDrawPoint3D(UBINT hDC,Rect *LimitRect,Control *PointCtrl){
	//	GLdouble ModelMat[16],ProjMat[16],TmpX,TmpY,TmpZ,RX,RY,RZ;
	//	GLint Viewport[4];
	//	Point3DInfo *Ctrl=(Point3DInfo *)PointCtrl->Ctrl;

	//	glGetDoublev(GL_MODELVIEW_MATRIX,ModelMat);
	//	glGetDoublev(GL_PROJECTION_MATRIX,ProjMat);
	//	glGetIntegerv(GL_VIEWPORT,Viewport);
	//	gluProject(Ctrl->XPos,Ctrl->YPos,Ctrl->ZPos,ModelMat,ProjMat,Viewport,&TmpX,&TmpY,&TmpZ);

	//	if(Ctrl->PaintFlag>0){
	//		glDisable(GL_DEPTH_TEST);
	//		if(1==Ctrl->PaintFlag)GLSetColorToUBINT((UBINT)PointCtrl);
	//		else if(2==Ctrl->PaintFlag)GLSetColorToUBINTEx((UBINT)&PointCtrl);
	//		glBegin(GL_TRIANGLE_FAN);
	//	}
	//	else{
	//		glLineWidth(1.0);
	//		if(0==Ctrl->InnerStatus)glColor3f(0.0,0.0,0.0);
	//		else if(1==Ctrl->InnerStatus)glColor3f(0.5,0.5,0.0);
	//		else glColor3f(1.0,0.5,0.0);
	//		glBegin(GL_LINE_LOOP);
	//	}
	//	gluUnProject(TmpX-3,TmpY-3,TmpZ,ModelMat,ProjMat,Viewport,&RX,&RY,&RZ);
	//	glVertex3d(RX,RY,RZ);
	//	gluUnProject(TmpX-3,TmpY+3,TmpZ,ModelMat,ProjMat,Viewport,&RX,&RY,&RZ);
	//	glVertex3d(RX,RY,RZ);
	//	gluUnProject(TmpX+3,TmpY+3,TmpZ,ModelMat,ProjMat,Viewport,&RX,&RY,&RZ);
	//	glVertex3d(RX,RY,RZ);
	//	gluUnProject(TmpX+3,TmpY-3,TmpZ,ModelMat,ProjMat,Viewport,&RX,&RY,&RZ);
	//	glVertex3d(RX,RY,RZ);
	//	glEnd();
	//}
	//extern Control *CreatePoint3D(nsBasic::ObjSet *ParentObj){
	//	Control *MyCtrl=(Control *)nsBasic::AllocThreadObject();
	//	if(NULL!=MyCtrl){
	//		MyCtrl->Ctrl=(UBINT)nsBasic::MemAlloc(nsBasic::GetThreadExtInfo()->MemAllocator,sizeof(Point3DInfo));
	//		if(0!=MyCtrl->Ctrl){
	//			((Point3DInfo *)MyCtrl->Ctrl)->InnerStatus=0;
	//			((Point3DInfo *)MyCtrl->Ctrl)->PaintFlag=0;
	//			MyCtrl->ChildObjRing=0;
	//			MyCtrl->Type=OBJ_POINT3D;
	//			MyCtrl->MsgProc=(UBINT)MsgProc_Point3D;
	//			MyCtrl->ThreadLock=(UBINT)NULL;
	//			if(NULL!=ParentObj)nsBasic::ObjRing_Attach((nsBasic::ObjGeneral *)MyCtrl,&(ParentObj->ChildObjRing));
	//		}
	//		else{
	//			nsBasic::FreeThreadObject((nsBasic::ObjGeneral *)MyCtrl);
	//			MyCtrl=NULL;
	//		}
	//	}
	//	return MyCtrl;
	//}
	//extern UBINT _cdecl MsgProc_TripodDummy(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
	//	va_list args;
	//	va_start(args,Msg);
	//	Control *TripodDummy=(Control *)lpObj,*Tripod=(Control *)TripodDummy->Ctrl;
	//	TripodInfo *Ctrl=(TripodInfo *)Tripod->Ctrl;
	//	UBINT Index=TripodDummy-Ctrl->ChildCtrl+1;
	//	switch(Msg){
	//		case MSG_MOUSEENTER:
	//		case MSG_MOUSELEAVE:
	//			return SendMsg(Tripod, Msg, Index);
	//		case MSG_MOUSEEVENT:
	//			{
	//				MouseStat *MyMouseStat=va_arg(args,MouseStat *);
	//				return SendMsg(Tripod, Msg, MyMouseStat);
	//			}
	//		case MSG_KEYBDENTER:
	//		case MSG_KEYBDLEAVE:
	//			return SendMsg(Tripod, Msg);
	//		default:
	//			return 0;
	//	}
	//}
	UBINT _cdecl Tripod::MsgProc(nsBasic::ObjGeneral *lpObj,UBINT Msg,...){
		va_list args;
		va_start(args,Msg);
		Tripod *MyTripod = (Tripod *)lpObj;

		switch(Msg){
			case MSG_MOUSEENTER:
				MyTripod->MouseBtnStat = 0;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				return 0;
			case MSG_MOUSEEVENT:
				{
					MouseStat *MyMouseStat=va_arg(args,MouseStat *);
					if ((MyMouseStat->BtnStat & MBTN_L) && (MyTripod->MouseBtnStat & MBTN_L)){
						double Drag[3]={0,0,0};
						switch(MyTripod->CurDummyID){
							case 1:
								Drag[0]+=(MyTripod->XPrj[0]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[0]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[0]*MyTripod->XPrj[0]+MyTripod->YPrj[0]*MyTripod->YPrj[0]);
								break;
							case 2:
								Drag[1]+=(MyTripod->XPrj[1]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[1]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[1]*MyTripod->XPrj[1]+MyTripod->YPrj[1]*MyTripod->YPrj[1]);
								Drag[2]+=(MyTripod->XPrj[2]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[2]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[2]*MyTripod->XPrj[2]+MyTripod->YPrj[2]*MyTripod->YPrj[2]);
								break;
							case 3:
								Drag[1]+=(MyTripod->XPrj[1]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[1]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[1]*MyTripod->XPrj[1]+MyTripod->YPrj[1]*MyTripod->YPrj[1]);
								break;
							case 4:
								Drag[0]+=(MyTripod->XPrj[0]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[0]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[0]*MyTripod->XPrj[0]+MyTripod->YPrj[0]*MyTripod->YPrj[0]);
								Drag[2]+=(MyTripod->XPrj[2]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[2]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[2]*MyTripod->XPrj[2]+MyTripod->YPrj[2]*MyTripod->YPrj[2]);
								break;
							case 5:
								Drag[2]+=(MyTripod->XPrj[2]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[2]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[2]*MyTripod->XPrj[2]+MyTripod->YPrj[2]*MyTripod->YPrj[2]);
								break;
							case 6:
								Drag[0]+=(MyTripod->XPrj[0]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[0]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[0]*MyTripod->XPrj[0]+MyTripod->YPrj[0]*MyTripod->YPrj[0]);
								Drag[1]+=(MyTripod->XPrj[1]*((double)MyMouseStat->XPos-(double)MyTripod->MouseRecX)+MyTripod->YPrj[1]*((double)MyMouseStat->YPos-(double)MyTripod->MouseRecY))/(MyTripod->XPrj[1]*MyTripod->XPrj[1]+MyTripod->YPrj[1]*MyTripod->YPrj[1]);
								break;
						}
						struct{
							UBINT msg;
							nsGUI::Tripod *MyTripod;
							double Offset[3];
						}TmpMsgDrag3D = { MSG_DRAG3D, MyTripod, { Drag[0], Drag[1], Drag[2] } };
						nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), &TmpMsgDrag3D, sizeof(TmpMsgDrag3D));
					}
					else if(0==(MyMouseStat->BtnStat & MBTN_L) && (MyTripod->MouseBtnStat & MBTN_L)){
						UBINT MsgArr[2] = { MSG_DRAGSTOP, (UBINT)MyTripod };
						nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
					}
					MyTripod->MouseBtnStat=MyMouseStat->BtnStat;
					MyTripod->MouseRecX=MyMouseStat->XPos;
					MyTripod->MouseRecY=MyMouseStat->YPos;
					return 0;
				}
				return 0;
			case MSG_MOUSELEAVE:
				if(MyTripod->MouseBtnStat & MBTN_L){
					UBINT MsgArr[2] = { MSG_DRAGSTOP, (UBINT)MyTripod };
					nsBasic::MsgQueue_BulkWrite(&(nsBasic::GetThreadExtInfo()->MsgManager), MsgArr, sizeof(MsgArr));
				}
				MyTripod->CurDummyID=0;
				nsBasic::MsgQueue_Write(&(nsBasic::GetThreadExtInfo()->MsgManager),MSG_PAINT);
				return 0;
			case MSG_KEYBDENTER:
				return 1;
			case MSG_KEYBDLEAVE:
				return 0;
			case MSG_DESTROY:
				throw new std::exception("this object is only allowed to be created locally.", 1);
				return 1;
			default:
				return 0;
		}
	}
	void Tripod::Draw(bool IsHitMap){
#if defined LIBENV_GLIB_OPENGL
		GLdouble ModelMat[16],ProjMat[16],TmpX,TmpY,TmpZ,HairLen;
		GLint Viewport[4];

		glGetDoublev(GL_MODELVIEW_MATRIX,ModelMat);
		glGetDoublev(GL_PROJECTION_MATRIX,ProjMat);
		glGetIntegerv(GL_VIEWPORT,Viewport);
		TmpX=ModelMat[0]*this->Pos[0]+ModelMat[4]*this->Pos[1]+ModelMat[8]*this->Pos[2]+ModelMat[12];
		TmpY=ModelMat[1]*this->Pos[0]+ModelMat[5]*this->Pos[1]+ModelMat[9]*this->Pos[2]+ModelMat[13];
		TmpZ=ModelMat[2]*this->Pos[0]+ModelMat[6]*this->Pos[1]+ModelMat[10]*this->Pos[2]+ModelMat[14];
		HairLen=ModelMat[3]*this->Pos[0]+ModelMat[7]*this->Pos[1]+ModelMat[11]*this->Pos[2]+ModelMat[15];
		TmpX/=HairLen;
		TmpY/=HairLen;
		TmpZ/=HairLen;
		HairLen=sqrt(TmpX*TmpX+TmpY*TmpY+TmpZ*TmpZ)/4.0;

		gluProject(this->Pos[0]+1,this->Pos[1],this->Pos[2],ModelMat,ProjMat,Viewport,&(this->XPrj[0]),&(this->YPrj[0]),&TmpZ);
		gluProject(this->Pos[0],this->Pos[1]+1,this->Pos[2],ModelMat,ProjMat,Viewport,&(this->XPrj[1]),&(this->YPrj[1]),&TmpZ);
		gluProject(this->Pos[0],this->Pos[1],this->Pos[2]+1,ModelMat,ProjMat,Viewport,&(this->XPrj[2]),&(this->YPrj[2]),&TmpZ);
		gluProject(this->Pos[0],this->Pos[1],this->Pos[2],ModelMat,ProjMat,Viewport,&TmpX,&TmpY,&TmpZ);
		this->XPrj[0] -= TmpX;
		this->XPrj[1] -= TmpX;
		this->XPrj[2] -= TmpX;
		this->YPrj[0]=TmpY-this->YPrj[0];
		this->YPrj[1]=TmpY-this->YPrj[1];
		this->YPrj[2]=TmpY-this->YPrj[2];

		if (IsHitMap)glLineWidth(9.0); else glLineWidth(3.0);
		if (IsHitMap)GLSetColorToUBINT((UBINT)&(this->Dummy[1]));
		else if (1 == this->CurDummyID)glColor3f(1.0, 1.0, 0.0);
		else glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex3d(this->Pos[0],this->Pos[1],this->Pos[2]);
		glVertex3d(this->Pos[0]+HairLen,this->Pos[1],this->Pos[2]);
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(this->Pos[0] + HairLen, this->Pos[1], this->Pos[2]);
		glVertex3d(this->Pos[0] + 0.75*HairLen, this->Pos[1] + 0.05*HairLen, this->Pos[2] + 0.05*HairLen);
		glVertex3d(this->Pos[0] + 0.75*HairLen, this->Pos[1] - 0.05*HairLen, this->Pos[2] + 0.05*HairLen);
		glVertex3d(this->Pos[0] + 0.75*HairLen, this->Pos[1] - 0.05*HairLen, this->Pos[2] - 0.05*HairLen);
		glVertex3d(this->Pos[0] + 0.75*HairLen, this->Pos[1] + 0.05*HairLen, this->Pos[2] - 0.05*HairLen);
		glVertex3d(this->Pos[0] + 0.75*HairLen, this->Pos[1] + 0.05*HairLen, this->Pos[2] + 0.05*HairLen);
		glEnd();
		if (IsHitMap)GLSetColorToUBINT((UBINT)&(this->Dummy[2]));
		else if (2 == this->CurDummyID)glColor3f(1.0, 1.0, 0.0);
		else glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
		glVertex3d(this->Pos[0],this->Pos[1]+0.5*HairLen,this->Pos[2]);
		glVertex3d(this->Pos[0],this->Pos[1]+0.5*HairLen,this->Pos[2]+0.5*HairLen);
		glVertex3d(this->Pos[0],this->Pos[1],this->Pos[2]+0.5*HairLen);
		glEnd();

		if (IsHitMap)GLSetColorToUBINT((UBINT)&(this->Dummy[3]));
		else if (3 == this->CurDummyID)glColor3f(1.0, 1.0, 0.0);
		else glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
		glVertex3d(this->Pos[0],this->Pos[1],this->Pos[2]);
		glVertex3d(this->Pos[0],this->Pos[1]+HairLen,this->Pos[2]);
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(this->Pos[0], this->Pos[1] + HairLen, this->Pos[2]);
		glVertex3d(this->Pos[0] + 0.05*HairLen, this->Pos[1] + 0.75*HairLen, this->Pos[2] + 0.05*HairLen);
		glVertex3d(this->Pos[0] - 0.05*HairLen, this->Pos[1] + 0.75*HairLen, this->Pos[2] + 0.05*HairLen);
		glVertex3d(this->Pos[0] - 0.05*HairLen, this->Pos[1] + 0.75*HairLen, this->Pos[2] - 0.05*HairLen);
		glVertex3d(this->Pos[0] + 0.05*HairLen, this->Pos[1] + 0.75*HairLen, this->Pos[2] - 0.05*HairLen);
		glVertex3d(this->Pos[0] + 0.05*HairLen, this->Pos[1] + 0.75*HairLen, this->Pos[2] + 0.05*HairLen);
		glEnd();
		if (IsHitMap)GLSetColorToUBINT((UBINT)&(this->Dummy[4]));
		else if (4 == this->CurDummyID)glColor3f(1.0, 1.0, 0.0);
		else glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINE_STRIP);
		glVertex3d(this->Pos[0]+0.5*HairLen,this->Pos[1],this->Pos[2]);
		glVertex3d(this->Pos[0]+0.5*HairLen,this->Pos[1],this->Pos[2]+0.5*HairLen);
		glVertex3d(this->Pos[0],this->Pos[1],this->Pos[2]+0.5*HairLen);
		glEnd();

		if (IsHitMap)GLSetColorToUBINT((UBINT)&(this->Dummy[5]));
		else if (5 == this->CurDummyID)glColor3f(1.0, 1.0, 0.0);
		else glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3d(this->Pos[0],this->Pos[1],this->Pos[2]);
		glVertex3d(this->Pos[0],this->Pos[1],this->Pos[2]+HairLen);
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(this->Pos[0], this->Pos[1], this->Pos[2] + HairLen);
		glVertex3d(this->Pos[0] + 0.05*HairLen, this->Pos[1] + 0.05*HairLen, this->Pos[2] + 0.75*HairLen);
		glVertex3d(this->Pos[0] - 0.05*HairLen, this->Pos[1] + 0.05*HairLen, this->Pos[2] + 0.75*HairLen);
		glVertex3d(this->Pos[0] - 0.05*HairLen, this->Pos[1] - 0.05*HairLen, this->Pos[2] + 0.75*HairLen);
		glVertex3d(this->Pos[0] + 0.05*HairLen, this->Pos[1] - 0.05*HairLen, this->Pos[2] + 0.75*HairLen);
		glVertex3d(this->Pos[0] + 0.05*HairLen, this->Pos[1] + 0.05*HairLen, this->Pos[2] + 0.75*HairLen);
		glEnd();
		if (IsHitMap)GLSetColorToUBINT((UBINT)&(this->Dummy[6]));
		else if (6 == this->CurDummyID)glColor3f(1.0, 1.0, 0.0);
		else glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINE_STRIP);
		glVertex3d(this->Pos[0]+0.5*HairLen,this->Pos[1],this->Pos[2]);
		glVertex3d(this->Pos[0]+0.5*HairLen,this->Pos[1]+0.5*HairLen,this->Pos[2]);
		glVertex3d(this->Pos[0],this->Pos[1]+0.5*HairLen,this->Pos[2]);
		glEnd();
#endif
	}
	Tripod::Tripod(){
		this->CurDummyID=0;
		this->ObjInterface.Type = OBJ_TRIPOD;
		this->ObjInterface.MsgProc = Tripod::MsgProc;
	}
}

#endif