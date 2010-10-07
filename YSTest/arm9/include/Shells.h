// YReader -> Shells by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-06 21:38:16 + 08:00;
// UTime = 2010-10-05 18:06 + 08:00;
// Version = 0.2755;


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
using namespace Drawing;
using namespace Runtime;

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
		: YForm(Rect::FullScreen, GetImage(1), pDesktopUp, hShl),
			lblTitle(HWND(this), G_APP_NAME, Rect(50, 20, 100, 22)),
			lblStatus(HWND(this), "Loading...", Rect(60, 80, 80, 22))
		{
		//	lblTitle.Transparent = true;
			Draw();
		}
	};
	struct TFrmLoadDown : public YForm
	{
		YLabel lblStatus;

		TFrmLoadDown(HSHL hShl)
		: YForm(Rect::FullScreen, GetImage(2), pDesktopDown, hShl),
			lblStatus(HWND(this), FS("初始化中，请稍后……"), Rect(30, 20, 160, 22))
		{
			lblStatus.Transparent = true;
			Draw();
		}
	};

public:
	virtual LRES
	OnActivated(const Message&);
	//	InitializeComponents();
};


class ShlExplorer : public ShlDS
{
private:
	struct TFrmFileListMonitor
	: public YForm
	{
		YLabel lblTitle, lblPath;

		TFrmFileListMonitor(HSHL hShl)
		: YForm(Rect::FullScreen, GetImage(3), pDesktopUp, hShl),
			lblTitle(HWND(this), "文件列表：请选择一个文件。", Rect(16, 20, 220, 22)),
			lblPath(HWND(this), "/", Rect(12, 80, 240, 22))
		{
		//	lblTitle.Transparent = true;
		//	lblPath.Transparent = true;
			Draw();
		}
	};
	struct TFrmFileListSelecter : public YForm
	{
		YFileBox fbMain;
		YButton btnTest, btnOK;

		TFrmFileListSelecter(HSHL hShl)
		: YForm(Rect::FullScreen, GetImage(4), pDesktopDown, hShl),
			fbMain(HWND(this), Rect(12, 10, 224, 150)),
			btnTest(HWND(this), FS(" 测试(X)"), Rect(60, 165, 70, 22)),
			btnOK(HWND(this), FS(" 确定(R)"), Rect(140, 165, 70, 22))
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
	virtual LRES
	OnActivated(const Message&);

	virtual LRES
	ShlProc(const Message&);

	static void
	fb_KeyPress(IVisualControl&, const MKeyEventArgs&);

	static void
	fb_Confirmed(IVisualControl&, const MIndexEventArgs&);
};


class ShlSetting : public ShlDS
{
public:
	typedef ShlDS ParentType;

	static const SPOS left = 5;
	static const SDST size = 22;

	static HWND hWndC;

	struct TFormA : public YForm
	{
		YButton lblA;
		YButton lblA2;

		TFormA(HSHL hShl) : YForm(Rect::FullScreen, GetImage(5), pDesktopUp, hShl),
			lblA(HWND(this), G_APP_NAME, Rect(left, 20, 200, size)),
			lblA2(HWND(this), "程序测试", Rect(left, 80, 72, size))
		{
			lblA2.Transparent = true;
		}

		void ShowString(const String& s)
		{
			lblA.Text = s;
			lblA.DrawForeground();
			lblA.Refresh();
		}
	};
	struct TFormB : public YForm
	{
		YButton btnB, btnB2;

		TFormB(HSHL hShl) : YForm(Rect(10, 40, 228, 70), /*GetImage(6)*/NULL, pDesktopDown, hShl),
			btnB(HWND(this), FS("测试程序"), Rect(2, 5, 224, size)),
			btnB2(HWND(this), FS("测试程序2"), Rect(45, 35, 124, size))
		{
			BackColor = ARGB16(1, 31, 31, 15);
			TouchMove += &YVisualControl::OnTouchMove;
		//	btnB.TouchMove += &YVisualControl::OnTouchMove;
			btnB.Enter += btnB_Enter;
			btnB.Leave += btnB_Leave;
			btnB2.TouchMove += &YVisualControl::OnTouchMove;
		//	btnB2.TouchDown += btnC_Click;

		//	btnB.Enabled = false;
		}

		static void
		btnB_Enter(IVisualControl& sender, const MInputEventArgs&);
		static void
		btnB_Leave(IVisualControl& sender, const MInputEventArgs&);
	};

	struct TFormC : public YForm
	{
		YButton btnC;
		YButton btnReturn;

		TFormC(HSHL hShl) : YForm(Rect(5, 60, 180, 120), /*GetImage(7)*/NULL, pDesktopDown, hShl),
			btnC(HWND(this), FS("测试y"), Rect(13, 45, 184, size)),
			btnReturn(HWND(this), FS("返回"), Rect(13, 82, 60, size))
		{
			BackColor = ARGB16(1, 31, 15, 15);
			TouchDown += TFormC_TouchDown;
			TouchMove += &YVisualControl::OnTouchMove;
			btnC.TouchUp.Add(*this, &TFormC::btnC_TouchUp);
			btnC.TouchDown.Add(*this, &TFormC::btnC_TouchDown);
			btnC.TouchMove += &YVisualControl::OnTouchMove;
			btnC.Click.Add(*this, &TFormC::btnC_Click);
			btnC.KeyPress += btnC_KeyPress;
		//	btnC.Enabled = false;
			btnReturn.BackColor = ARGB16(1, 22, 23, 24);
			btnReturn.Click.Add(*this, &TFormC::btnReturn_Click);
		}

		void
		btnC_TouchUp(const MTouchEventArgs&);
		void
		btnC_TouchDown(const MTouchEventArgs&);
		void
		btnC_Click(const MTouchEventArgs&);
		static void
		btnC_KeyPress(IVisualControl& sender, const MKeyEventArgs& e);

		void
		btnReturn_Click(const MTouchEventArgs&);
	};

	void ShowString(const String& s)
	{
		GHHandle<TFormA>(hWndUp)->ShowString(s);
	}
	void ShowString(const char* s)
	{
		ShowString(String(s));
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
	OnActivated(const Message&);

	virtual LRES
	OnDeactivated(const Message&);

	virtual LRES
	ShlProc(const Message&);
};


class ShlReader : public Shells::ShlGUI
{
public:
	typedef ShlGUI ParentType;

	static std::string path;

	MDualScreenReader Reader;
	YTextFile* pTextFile;

	GHResource<YImage> hUp, hDn;
	bool bgDirty;

	ShlReader();
	virtual
	~ShlReader() ythrow()
	{}

	virtual LRES
	OnActivated(const Message&);

	virtual LRES
	OnDeactivated(const Message&);

	virtual LRES
	ShlProc(const Message&);

	virtual void
	UpdateToScreen();

	void
	OnClick(const MTouchEventArgs&);

	void
	OnKeyPress(const MKeyEventArgs&);
};

YSL_END;

#endif

