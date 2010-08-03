// YReader -> Shells by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-6 21:38:16;
// UTime = 2010-8-2 15:46;
// Version = 0.2622;


#ifndef INCLUDED_SHELLS_H_
#define INCLUDED_SHELLS_H_

// Shells ： Shell 声明。

//#include "../YSLib/Helper/shlds.h"
#include "DSReader.h"

YSL_BEGIN

//全局常量。
//extern CPATH DEF_DIRECTORY;

using namespace Components;
using namespace Components::Widgets;
using namespace Components::Controls;
using namespace Components::Forms;
using namespace Runtime;
using namespace Exceptions;

using namespace DS;
using namespace DS::Components;

GHResource<YImage>&
GetImage(int);

class ShlLoad : public ShlDS
{
private:
	struct TFrmLoadUp
		: public YForm
	{
		YLabel lblTitle, lblStatus;

		TFrmLoadUp(HSHL hShl)
			: YForm(SRect::FullScreen, GetImage(1), pDesktopUp, hShl),
			lblTitle(HWND(this), G_APP_NAME, SRect(50, 20, 100, 22)),
			lblStatus(HWND(this), "Loading...", SRect(60, 80, 80, 22))
		{
		//	lblTitle.Transparent = true;
			Draw();
		}
	};
	struct TFrmLoadDown : public YForm
	{
		YLabel lblStatus;

		TFrmLoadDown(HSHL hShl)
			: YForm(SRect::FullScreen, GetImage(2), pDesktopDown, hShl),
			lblStatus(HWND(this), L"初始化中，请稍后……", SRect(30, 20, 160, 22))
		{
			lblStatus.Transparent = true;
			Draw();
		}
	};

public:
	//	InitializeComponents();
	virtual LRES
	OnActivated(const MMSG&);
};


class ShlS : public ShlDS
{
private:
	struct TFrmFileListMonitor
	: public YForm
	{
		YLabel lblTitle, lblPath;

		TFrmFileListMonitor(HSHL hShl)
			: YForm(SRect::FullScreen, GetImage(3), pDesktopUp, hShl),
			lblTitle(HWND(this), "FileList : Press \"A\" to continue...", SRect(30, 20, 220, 22)),
			lblPath(HWND(this), "/[Path]", SRect(20, 80, 240, 22))
		{
		//	lblTitle.Transparent = true;
		//	lblPath.Transparent = true;
			Draw();
		}
	};
	struct TFrmFileListSelecter : public YForm
	{
		YFileBox fbMain;
		YLabel btnTest, btnOK;

		TFrmFileListSelecter(HSHL hShl)
			: YForm(SRect::FullScreen, GetImage(4), pDesktopDown, hShl),
			fbMain(HWND(this), SRect(12, 10, 224, 150)),
			btnTest(HWND(this), L" 测试(X)", SRect(60, 165, 70, 22)),
			btnOK(HWND(this), L" 确定(R)", SRect(140, 165, 70, 22))
		{
			KeyPress += &TFrmFileListSelecter::frm_KeyPress;
		//	fbMain.TouchDown += YFileBox::OnClick;
		//	fbMain.Click += &YListBox::_m_OnClick;
			fbMain.KeyPress += fb_KeyPress;
			fbMain.Selected.Add(*this, &TFrmFileListSelecter::fb_Selected);
			fbMain.Confirmed += fb_Confirmed;
			btnTest.Click.Add(*this, &TFrmFileListSelecter::btnTest_Click);
			btnOK.Click.Add(*this, &TFrmFileListSelecter::btnOK_Click);
			btnOK.SetTransparent(false);
			Draw();
		}
		void
		frm_KeyPress(const MKeyEventArgs&);
		void
		fb_Selected(const MIndexEventArgs&);
		void
		btnTest_Click(const MTouchEventArgs&);
		void
		btnOK_Click(const MTouchEventArgs&);
	};
	void LoadNextWindows();

public:
	static void
	fb_KeyPress(IVisualControl&, const MKeyEventArgs&);

	static void
	fb_Confirmed(IVisualControl&, const MIndexEventArgs&);

	virtual LRES
	ShlProc(const MMSG&);

	virtual LRES
	OnActivated(const MMSG&);
};


class ShlA : public ShlDS
{
public:
	typedef ShlDS ParentType;

	static const SPOS left = 5;
	static const SDST size = 22;

	static HWND hWndC;

	struct TFormA : public YForm
	{
		YLabel lblA;
		YLabel lblA2;

		TFormA(HSHL hShl) : YForm(SRect::FullScreen, GetImage(5), pDesktopUp, hShl),
			lblA(HWND(this), G_APP_NAME, SRect(left, 20, 200, size)),
			lblA2(HWND(this), "程序测试", SRect(left, 80, 72, size))
		{
			lblA2.Transparent = true;
		}

		void ShowString(const MString& s)
		{
			lblA.Text = s;
			lblA.DrawForeground();
			lblA.Refresh();
		}
	};
	struct TFormB : public YForm
	{
		YLabel lblB, lblB2;

		TFormB(HSHL hShl) : YForm(SRect(10, 40, 228, 70), /*GetImage(6)*/NULL, pDesktopDown, hShl),
			lblB(HWND(this), L"测试程序", SRect(2, 5, 224, size)),
			lblB2(HWND(this), L"测试程序2", SRect(45, 35, 124, size))
		{
			BackColor = ARGB16(1, 31, 31, 15);
			TouchDown += OnTouchDown;
			TouchHeld += YVisualControl::OnTouchHeld;
			lblB.TouchHeld += YVisualControl::OnTouchHeld;
			lblB2.TouchHeld += YVisualControl::OnTouchHeld;
		//	lblB2.TouchDown += lblC_Click;

		//	lblB.Enabled = false;
		}
	};
	struct TFormC : public YForm
	{
		YLabel lblC;
		YLabel btnReturn;

		TFormC(HSHL hShl) : YForm(SRect(5, 60, 180, 120), /*GetImage(7)*/NULL, pDesktopDown, hShl),
			lblC(HWND(this), L"测试y", SRect(13, 45, 184, size)),
			btnReturn(HWND(this), L"返回", SRect(13, 82, 60, size))
		{
			BackColor = ARGB16(1, 31, 15, 15);
			TouchDown += TFormC_TouchDown;
			TouchHeld += YVisualControl::OnTouchHeld;
			lblC.TouchUp.Add(*this, &TFormC::lblC_TouchUp);
			lblC.TouchDown.Add(*this, &TFormC::lblC_TouchDown);
			lblC.TouchHeld += YVisualControl::OnTouchHeld;
			lblC.Click.Add(*this, &TFormC::lblC_Click);
			lblC.KeyPress += lblC_KeyPress;
		//	lblC.Enabled = false;
			btnReturn.BackColor = ARGB16(1, 22, 23, 24);
			btnReturn.Click.Add(*this, &TFormC::btnReturn_Click);
		}

		void
		lblC_TouchUp(const MTouchEventArgs&);

		void
		lblC_TouchDown(const MTouchEventArgs&);

		void
		lblC_Click(const MTouchEventArgs&);

		static void
		lblC_KeyPress(IVisualControl& sender, const MKeyEventArgs& e);

		void
		btnReturn_Click(const MTouchEventArgs&);
	};

	void ShowString(const MString& s)
	{
		GHHandle<TFormA>(hWndUp)->ShowString(s);
	}
	void ShowString(const char* s)
	{
		ShowString(MString(s));
	}

	static void
	TFormC_TouchDown(IVisualControl& sender, const MTouchEventArgs& e)
	{
		try
		{
			TFormC& frm(dynamic_cast<TFormC&>(sender));

			frm.BackColor = ARGB16(1, rand(), rand(), rand());
			frm.Refresh();
		}
		catch(std::bad_cast&)
		{}
	}

	virtual LRES
	ShlProc(const MMSG&);

	virtual LRES
	OnActivated(const MMSG&);

	virtual LRES
	OnDeactivated(const MMSG&);
};


class ShlReader : public ShlGUI
{
public:
	typedef ShlGUI ParentType;

//	static MString path;

	YTextFile TextFile;
	MDualScreenReader Reader;

	GHResource<YImage> hUp, hDn;
	bool bgDirty;

	ShlReader();
	virtual
	~ShlReader() ythrow()
	{}

	virtual void
	UpdateToScreen();

	void
	OnClick(const MTouchEventArgs&);

	void
	OnKeyPress(const MKeyEventArgs&);

	virtual LRES
	ShlProc(const MMSG&);

	virtual LRES
	OnActivated(const MMSG&);

	virtual LRES
	OnDeactivated(const MMSG&);
};

YSL_END;

#endif

