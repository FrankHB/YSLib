/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Shells.cpp
\ingroup YReader
\brief Shell 实现。
\version 0.3594;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2011-03-18 17:25 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#include <Shells.h>

////////
//测试用声明：资源定义。
extern char gstr[128];
char strtbuf[0x400],
	strtf[0x400],
	strttxt[0x400];

YSL_BEGIN

using namespace Shells;
using namespace Drawing::ColorSpace;


int
MainShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_ACTIVATED:
		try
		{
			NowShellToStored<ShlLoad>();
		}
		catch(...)
		{
			throw LoggedEvent("Run shell failed at end of ShlMain.");
			return -1;
		}
		return 0;

	default:
		return DefShellProc(msg);
	}
}


namespace
{
	//测试函数。

	//背景测试。
	void
	dfa(BitmapPtr buf, SDST x, SDST y)
	{
		//raz2
		buf[y * Global::MainScreenWidth + x] = ARGB16(1, ((x >> 2) + 15) & 31,
			((y >> 2) + 15) & 31, ((~(x * y) >> 2) + 15) & 31);
	}
	void
	dfap(BitmapPtr buf, SDST x, SDST y)
	{
		//bza1
		buf[y * Global::MainScreenWidth + x] = ARGB16(1,
			((x | y << 1) % (y + 2)) & 31, ((~y | x << 1) % 27 + 3) & 31,
			((x << 4) / (y | 1)) & 31);
	}
	void
	dfac1(BitmapPtr buf, SDST x, SDST y)
	{
		//fl1
		buf[y * Global::MainScreenWidth + x] = ARGB16(1, (x + y * y) & 31,
			(x * x + y) & 31, ((x & y) ^ (x | y)) & 31);
	}
	void
	dfac1p(BitmapPtr buf, SDST x, SDST y)
	{
		//rz3
		buf[y * Global::MainScreenWidth + x] = ARGB16(1, ((x * y) | y) & 31,
			((x * y) | x) & 31, ((x ^ y) * (x ^ y)) & 31);
	}
	void
	dfac2(BitmapPtr buf, SDST x, SDST y)
	{
		//v1
		buf[y * Global::MainScreenWidth + x] = ARGB16(1,
			((x + y) % ((y - 2) & 1) + (x << 2)) & 31, (~x % 101 + y) & 31,
			((x << 4) / (y & 1)) & 31);
	}
	void
	dfac2p(BitmapPtr buf, SDST x, SDST y)
	{
		//arz1
		buf[y * Global::MainScreenWidth + x]
			= ARGB16(1, ((x | y) % (y + 2)) & 31, ((~y | x) % 27 + 3) & 31,
			((x << 6) / (y | 1)) & 31);
	}

	////

	//测试用变量。
	BitmapPtr gbuf;
	int nCountInput;
	char strCount[40];

	GHStrong<YImage>&
	GetGlobalImageRef(std::size_t i)
	{
		static GHStrong<YImage> spi[10];

		YAssert(IsInInterval(i, 10u), "Array index out of range @@"
			" GetGlobalImageRef;");

		return spi[i];
	}

	void
	LoadL()
	{
		//色块覆盖测试用程序段。
		if(!GetGlobalImageRef(1))
		{
			try
			{
				gbuf = ynew ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
		//	memset(gbuf, 0xEC, sizeof(ScreenBufferType));
			GetGlobalImageRef(1) = NewScrImage(dfa, gbuf);
		//	memset(gbuf, 0xF2, sizeof(ScreenBufferType));
			GetGlobalImageRef(2) = NewScrImage(dfap, gbuf);
			ydelete_array(gbuf);
		}
	}

	void
	LoadS()
	{
		if(!GetGlobalImageRef(3))
		{
			try
			{
				gbuf = ynew ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
			GetGlobalImageRef(3) = NewScrImage(dfac1, gbuf);
			GetGlobalImageRef(4) = NewScrImage(dfac1p, gbuf);
			ydelete_array(gbuf);
		}
	}

	void
	LoadA()
	{
		if(!GetGlobalImageRef(5))
		{
			try
			{
				gbuf = ynew ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
			GetGlobalImageRef(5) = NewScrImage(dfac2, gbuf);
			GetGlobalImageRef(6) = NewScrImage(dfac2p, gbuf);
			ydelete_array(gbuf);
		}
	}

	void
	switchShl1()
	{
		CallStored<ShlSetting>();
	}

	void
	switchShl2(CPATH pth)
	{
		ShlReader::path = pth;
		CallStored<ShlReader>();
	}

	void
	InputCounter(const Point& pt)
	{
		sprintf(strCount, "%d,%d,%d;Count = %d, Pos = (%d, %d);",
			sizeof(AWindow), sizeof(YWindow), sizeof(YForm),
			nCountInput++, pt.X, pt.Y);
	}

	void
	InputCounterAnother(const Point& /*pt*/)
	{
	//	nCountInput++;
	//	std::sprintf(strCount, "%d,%d,%d,%d,",sizeof(YForm),sizeof(YShell),
	//		sizeof(YApplication),sizeof(YWindow));
		struct mallinfo t(mallinfo());

	/*	std::sprintf(strCount, "%d,%d,%d,%d,%d;",
			t.arena,    // total space allocated from system 2742496
			t.ordblks,  // number of non-inuse chunks 37
			t.smblks,   // unused -- always zero 0
			t.hblks,    // number of mmapped regions 0
			t.hblkhd   // total space in mmapped regions 0
		);*/
	/*	std::sprintf(strCount, "%d,%d,%d,%d,%d;",
			t.usmblks,  // unused -- always zero 0
			t.fsmblks,  // unused -- always zero 0
			t.uordblks, // total allocated space 2413256, 1223768
			t.fordblks, // total non-inuse space 329240, 57760
			t.keepcost // top-most, releasable (via malloc_trim) space
			//46496,23464
			);*/
		std::sprintf(strCount, "%d,%d,%d,%d,%d;",
			t.arena,
			t.ordblks,
			t.uordblks,
			t.fordblks,
			t.keepcost);
	}
}

GHStrong<YImage>&
GetImage(int i)
{
	switch(i)
	{
	case 1:
	case 2:
		LoadL();
		break;
	case 3:
	case 4:
		LoadS();
		break;
	case 5:
	case 6:
	case 7:
		LoadA();
		break;
	case 8:
	case 9:
	//	LoadR();
		break;
	default:
		i = 0;
	}
	return GetGlobalImageRef(i);
}

void
ReleaseShells()
{
	for(std::size_t i(0); i != 10; ++i)
		YReset(GetGlobalImageRef(i));
	ReleaseStored<ShlReader>();
	ReleaseStored<ShlSetting>();
	ReleaseStored<ShlExplorer>();
	ReleaseStored<ShlLoad>();
	ReleaseGlobalResource<TextRegion>();
}

using namespace DS;

ShlLoad::TFrmLoadUp::TFrmLoadUp()
	: YForm(Rect::FullScreen, GetImage(1),
		HWND(theApp.GetPlatformResource().GetDesktopUpHandle())),
	lblTitle(Rect(50, 20, 100, 22), this),
	lblStatus(Rect(60, 80, 80, 22), this)
{
	lblTitle.Text = YApplication::ProductName;
	lblStatus.Text = "Loading...";
	//	lblTitle.Transparent = true;
	Draw();
}

ShlLoad::TFrmLoadDown::TFrmLoadDown()
	: YForm(Rect::FullScreen, GetImage(2),
		HWND(theApp.GetPlatformResource().GetDesktopDownHandle())),
	lblStatus(Rect(30, 20, 160, 22), this)
{
	lblStatus.Text = _ustr("初始化中，请稍后……");
	lblStatus.SetTransparent(true);
	Draw();
}

int
ShlLoad::OnActivated(const Message& /*m*/)
{
	YDebugSetStatus(true);
	//如果不添加此段且没有桌面没有被添加窗口等设置刷新状态的操作，
	//那么任何绘制都不会进行。
	theApp.GetPlatformResource().GetDesktopUp().SetRefresh(true);
	theApp.GetPlatformResource().GetDesktopDown().SetRefresh(true);
	//新建窗口。
	hWndUp = NewWindow<TFrmLoadUp>();
	hWndDown = NewWindow<TFrmLoadDown>();
	DispatchWindows();
	UpdateToScreen();
	try
	{
		SetShellToStored<ShlExplorer>();
	}
	catch(...)
	{
		throw LoggedEvent("Run shell failed at end of ShlLoad.");
		return -1;
	}
	return 0;
}


ShlExplorer::TFrmFileListMonitor::TFrmFileListMonitor()
	: YForm(Rect::FullScreen, GetImage(3),
		HWND(theApp.GetPlatformResource().GetDesktopUpHandle())),
	lblTitle(Rect(16, 20, 220, 22), this),
	lblPath(Rect(12, 80, 240, 22), this)
{
	lblTitle.Text = "文件列表：请选择一个文件。";
	lblPath.Text = "/";
	//	lblTitle.Transparent = true;
	//	lblPath.Transparent = true;
	Draw();
}

ShlExplorer::TFrmFileListSelecter::TFrmFileListSelecter()
	: YForm(Rect::FullScreen, GetImage(4),
	HWND(theApp.GetPlatformResource().GetDesktopDownHandle())),
	fbMain(Rect(6, 10, 224, 150), this),
	btnTest(Rect(115, 165, 65, 22), this),
	btnOK(Rect(185, 165, 65, 22), this)
{
	btnTest.Text = _ustr(" 测试(X)");
	btnOK.Text = _ustr(" 确定(R)");
	FetchEvent<KeyPress>(*this)
		+= &TFrmFileListSelecter::frm_KeyPress;
	//	fbMain.TouchDown += YFileBox::OnClick;
	//	fbMain.Click += &YListBox::_m_OnClick;
	FetchEvent<KeyPress>(fbMain) += fb_KeyPress;
	fbMain.GetViewChanged().Add(*this, &TFrmFileListSelecter::fb_ViewChanged);
	fbMain.GetConfirmed() += fb_Confirmed;
	FetchEvent<Click>(btnTest).Add(*this,
		&TFrmFileListSelecter::btnTest_Click);
	FetchEvent<Click>(btnOK).Add(*this,
		&TFrmFileListSelecter::btnOK_Click);
	btnOK.SetTransparent(false);
	Draw();
}

void
ShlExplorer::TFrmFileListSelecter::frm_KeyPress(KeyEventArgs& e)
{
	TouchEventArgs et(TouchEventArgs::FullScreen);

	switch(e.GetKey())
	{
	case KeySpace::X:
		FetchEvent<Click>(btnTest)(btnTest, et);
		break;
	case KeySpace::R:
		FetchEvent<Click>(btnOK)(btnOK, et);
		break;
	default:
		break;
	}
}

void
ShlExplorer::TFrmFileListSelecter::fb_ViewChanged(EventArgs&)
{
	YLabel& l(HandleCast<TFrmFileListMonitor>(
		HandleCast<ShlExplorer>(FetchShellHandle())->hWndUp)->lblPath);

	l.Text = fbMain.GetPath();
	l.Refresh();
}

void
ShlExplorer::fb_KeyPress(IControl& /*sender*/, KeyEventArgs& e)
{
	Key x(e);

	if(x & KeySpace::L)
		switchShl1();
}
void
ShlExplorer::fb_Confirmed(IControl& /*sender*/, IndexEventArgs& /*e*/)
{
//	if(e.Index == 2)
//		switchShl1();
}

void
ShlExplorer::TFrmFileListSelecter::btnTest_Click(TouchEventArgs&)
{
	switchShl1();
/*	if(fbMain.IsSelected())
	{
		YConsole con(*hScreenUp);

		Activate(con, Color::Silver);

		iprintf("Current Working Directory:\n%s\n",
			IO::GetNowDirectory().c_str());
		iprintf("FileBox Path:\n%s\n", fbMain.GetPath().c_str());
	//	std::fprintf(stderr, "err");
		WaitForInput();
	}
	else
	{
		YConsole con(*hScreenDown);
		Activate(con, Color::Yellow, ColorSpace::Green);
		iprintf("FileBox Path:\n%s\n", fbMain.GetPath().c_str());
		puts("OK");
		WaitForInput();
	}*/
}

void
ShlExplorer::TFrmFileListSelecter::btnOK_Click(TouchEventArgs&)
{
	if(fbMain.IsSelected())
	{
		const string& s(fbMain.GetPath().GetNativeString());
/*	YConsole con;
	Activate(con);
	iprintf("%s\n%s\n%s\n%d,%d\n",fbMain.GetDirectory().c_str(),
		StringToMBCS(fbMain.YListBox::GetList()[fbMain.GetSelected()]).c_str(),
		s.c_str(),IO::ValidateDirectory(s), ystdex::fexists(s.c_str()));
	WaitForABXY();
	Deactivate(con);*/
		if(!IO::ValidateDirectory(s) && ystdex::fexists(s.c_str()))
			switchShl2(s.c_str());
	}
}

int
ShlExplorer::ShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_INPUT:
		ResponseInput(msg);
		UpdateToScreen();
		return 0;

	default:
		return ShlDS::ShlProc(msg);
	}
}

int
ShlExplorer::OnActivated(const Message&)
{
	hWndUp = NewWindow<TFrmFileListMonitor>();
	hWndDown = NewWindow<TFrmFileListSelecter>();
/*
	ReplaceHandle<HWND>(hWndUp,
		new TFrmFileListMonitor(GHHandle<YShell>(this)));
	ReplaceHandle<HWND>(hWndDown,
		new TFrmFileListSelecter(GHHandle<YShell>(this)));
*/
	//	HandleCast<TFrmFileListSelecter>(
	//		hWndDown)->fbMain.RequestFocus(GetZeroElement<EventArgs>());
	//	hWndDown->RequestFocus(GetZeroElement<EventArgs>());
	RequestFocusCascade(HandleCast<TFrmFileListSelecter>(hWndDown)->fbMain);
	DispatchWindows();
	UpdateToScreen();
	return 0;
}


YSL_BEGIN_SHELL(ShlSetting)

//屏幕刷新消息生成函数。
//输出设备刷新函数。
/*static void ScrRefresh()
{
	SendMessage(GetCurrentShellHandle(), SM_SCRREFRESH, 0x80,
		DefaultShellHandle->scrType, 0);
}
*/

YSL_END_SHELL(ShlSetting)

HWND ShlSetting::hWndC(NULL);

ShlSetting::TFormA::TFormA()
	: YForm(Rect::FullScreen, GetImage(5),
	HWND(theApp.GetPlatformResource().GetDesktopUpHandle())),
	lblA(Rect(s_left, 20, 200, s_size), this),
	lblA2(Rect(s_left, 80, 72, s_size), this)
{
	lblA.Text = YApplication::ProductName;
	lblA2.Text = "程序测试";
	lblA2.SetTransparent(true);
}

void
ShlSetting::TFormA::ShowString(const String& s)
{
	lblA.Text = s;
	lblA.Refresh();
}

ShlSetting::TFormB::TFormB()
	: YForm(Rect(10, 40, 228, 70), NULL,
		HWND(theApp.GetPlatformResource().GetDesktopDownHandle())),
	btnB(Rect(2, 5, 224, s_size), this), /*GetImage(6)*/
	btnB2(Rect(45, 35, 124, s_size), this)
{
	btnB.Text = _ustr("测试程序");
	btnB2.Text = _ustr("测试程序2");
	BackColor = ARGB16(1, 31, 31, 15);
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
	//	btnB.TouchMove += &Control::OnTouchMove;
	FetchEvent<Enter>(btnB) += btnB_Enter;
	FetchEvent<Leave>(btnB) += btnB_Leave;
	FetchEvent<TouchMove>(btnB2) += OnTouchMove_Dragging;
	//	btnB2.TouchDown += btnC_Click;
	//	btnB.Enabled = false;
}

void
ShlSetting::TFormB::btnB_Enter(IControl& sender, InputEventArgs& e)
{
	DefDynInitRef(YButton, btn, sender)
	TouchEventArgs& pt(static_cast<TouchEventArgs&>(e));
	char str[20];

	std::sprintf(str, "Enter:(%d,%d)", pt.Point::X, pt.Point::Y);
	btn.Text = str;
	btn.Refresh();
}
void
ShlSetting::TFormB::btnB_Leave(IControl& sender, InputEventArgs& e)
{
	DefDynInitRef(YButton, btn, sender)
	TouchEventArgs& pt(static_cast<TouchEventArgs&>(e));
	char str[20];

	std::sprintf(str, "Leave:(%d,%d)", pt.Point::X, pt.Point::Y);
	btn.Text = str;
	btn.Refresh();
}

ShlSetting::TFormC::TFormC()
	: YForm(Rect(5, 60, 180, 120), NULL,
		HWND(theApp.GetPlatformResource().GetDesktopDownHandle())),
	btnC(Rect(13, 15, 184, s_size), this),/*GetImage(7)*/
	btnD(Rect(13, 52, 60, s_size), this),
	btnReturn(Rect(13, 82, 60, s_size), this),
	btnExit(Rect(83, 82, 60, s_size), this)
{
	btnC.Text = _ustr("测试拖放控件");
	btnD.Text = _ustr("测试");
	btnReturn.Text = _ustr("返回");
	btnExit.Text = _ustr("退出");
	BackColor = ARGB16(1, 31, 15, 15);
	FetchEvent<TouchDown>(*this) += TFormC_TouchDown;
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
	FetchEvent<TouchUp>(btnC).Add(*this,
		&TFormC::btnC_TouchUp);
	FetchEvent<TouchDown>(btnC).Add(*this,
		&TFormC::btnC_TouchDown);
	FetchEvent<TouchMove>(btnC) += OnTouchMove_Dragging;
	FetchEvent<Click>(btnC).Add(*this, &TFormC::btnC_Click);
	FetchEvent<Click>(btnD).Add(*this, &TFormC::btnD_Click);
	FetchEvent<KeyPress>(btnC) += btnC_KeyPress;
	//	btnC.Enabled = false;
	btnReturn.BackColor = ARGB16(1, 22, 23, 24);
	FetchEvent<Click>(btnReturn).Add(*this,
		&TFormC::btnReturn_Click);
	FetchEvent<Click>(btnExit).Add(*this,
		&TFormC::btnExit_Click);
}

void
ShlSetting::TFormC::btnC_TouchUp(TouchEventArgs& e)
{
	InputCounter(e);
	HandleCast<ShlSetting>(FetchShellHandle())->ShowString(strCount);
	btnC.Refresh();
}
void
ShlSetting::TFormC::btnC_TouchDown(TouchEventArgs& e)
{
	InputCounterAnother(e);
	HandleCast<ShlSetting>(FetchShellHandle())->ShowString(strCount);
//	btnC.Refresh();
}
void
ShlSetting::TFormC::btnC_Click(TouchEventArgs& /*e*/)
{
	static YFontCache& fc(theApp.GetFontCache());
	static const int ffilen(fc.GetFilesN());
	static const int ftypen(fc.GetTypesN());
	static const int ffacen(fc.GetFacesN());
	static int itype;
	static YFontCache::FTypes::const_iterator it(fc.GetTypes().begin());

	//	btnC.Transparent ^= 1;
	if(nCountInput & 1)
	{
		//	btnC.Visible ^= 1;
		++itype %= ftypen;
		if(++it == fc.GetTypes().end())
			it = fc.GetTypes().begin();
		btnC.Font = Font(*(*it)->GetFontFamilyPtr(), 18 - (itype << 1),
			EFontStyle::Regular);
	//	btnC.Font = Font(*(*it)->GetFontFamilyPtr(), //GetDefaultFontFamily(),
	//		18 - (itype << 1), EFontStyle::Regular);
		sprintf(strtf, "%d, %d file(s), %d type(s), %d faces(s);\n",
			btnC.Font.GetSize(), ffilen, ftypen, ffacen);
		btnC.Text = strtf;
		btnC.ForeColor = Color(std::rand(), std::rand(), std::rand());
		btnReturn.ForeColor = Color(std::rand(), std::rand(), std::rand());
	}
	else
	{
		sprintf(strtf, "%d/%d;%s:%s;\n", itype + 1, ftypen,
			(*it)->GetFamilyName(), (*it)->GetStyleName());
		//	sprintf(strtf, "B%p\n",
		//		fc.GetTypefacePtr("FZYaoti", "Regular"));
		btnC.Text = strtf;
	}
	//	btnC.Refresh();
}

void
ShlSetting::TFormC::btnC_KeyPress(IControl& sender, KeyEventArgs& e)
{
	//测试程序。

	u32 k(static_cast<KeyEventArgs::Key>(e));

	DefDynInitRef(YButton, lbl, sender);
//	YButton& lbl(dynamic_cast<TFormA&>(
//		*(dynamic_cast<ShlSetting&>(*FetchShellHandle()).hWndUp)).lblA2);
	lbl.SetTransparent(!lbl.IsTransparent());
//	++lbl.ForeColor;
//	--lbl.BackColor;
	sprintf(strttxt, "%d;\n", k);
	lbl.Text = strttxt;
	lbl.Refresh();
/*
	YButton& lbl(static_cast<YButton&>(sender));

	if(nCountInput & 1)
	{
		sprintf(strtf, "测试键盘...");
		lbl.Text = strtf;
	}*/
}

/*void
ShlSetting::TFormC::btnD_Click(TouchEventArgs&)
{

}*/


void
ShlSetting::TFormC::btnReturn_Click(TouchEventArgs&)
{
	CallStored<ShlExplorer>();
}

void
ShlSetting::TFormC::btnExit_Click(TouchEventArgs&)
{
	Shells::PostQuitMessage(0);
}

void
ShlSetting::ShowString(const String& s)
{
	general_handle_cast<TFormA>(hWndUp)->ShowString(s);
}
void
ShlSetting::ShowString(const char* s)
{
	ShowString(String(s));
}

void
ShlSetting::TFormC_TouchDown(IControl& sender, TouchEventArgs& /*e*/)
{
	try
	{
		TFormC& frm(dynamic_cast<TFormC&>(sender));

		frm.BackColor = ARGB16(1, std::rand(), std::rand(), std::rand());
		frm.Refresh();
	}
	catch(std::bad_cast&)
	{}
}


int
ShlSetting::OnActivated(const Message& /*msg*/)
{
	theApp.GetPlatformResource().GetDesktopDown().BackColor
		= ARGB16(1, 15, 15, 31);
	theApp.GetPlatformResource().GetDesktopDown().SetBackground(GetImage(6));
	hWndUp = NewWindow<TFormA>();
	hWndDown = NewWindow<TFormB>();
	hWndC = NewWindow<TFormC>();
	hWndUp->Draw();
	hWndDown->Draw();
	hWndC->Draw();
	UpdateToScreen();
	return 0;
}

int
ShlSetting::OnDeactivated(const Message& m)
{
	ParentType::OnDeactivated(m);
	*FetchGUIShellHandle() -= hWndC;
	YReset(hWndC);
	return 0;
}

int
ShlSetting::ShlProc(const Message& msg)
{
	using namespace YSL_SHL(ShlSetting);
//	ClearDefaultMessageQueue();

//	const WPARAM& wParam(msg.GetWParam());

//	UpdateToScreen();

	switch(msg.GetMessageID())
	{
	case SM_INPUT:
		ResponseInput(msg);
		UpdateToScreen();
		return 0;

	default:
		break;
	}
	return DefShellProc(msg);
}


string ShlReader::path;

ShlReader::ShlReader()
	: ShlGUI(),
	Reader(), pTextFile(NULL), hUp(NULL), hDn(NULL), bgDirty(false)
{}

int
ShlReader::OnActivated(const Message& /*msg*/)
{
	pTextFile = ynew YTextFile(path.c_str());
	Reader.LoadText(*pTextFile);
	bgDirty = true;
	hUp = theApp.GetPlatformResource().GetDesktopUp().GetBackground();
	hDn = theApp.GetPlatformResource().GetDesktopDown().GetBackground();
	theApp.GetPlatformResource().GetDesktopUp().SetBackground(NULL);
	theApp.GetPlatformResource().GetDesktopDown().SetBackground(NULL);
	theApp.GetPlatformResource().GetDesktopUp().BackColor
		= ARGB16(1, 30, 27, 24);
	theApp.GetPlatformResource().GetDesktopDown().BackColor
		= ARGB16(1, 24, 27, 30);
	FetchEvent<Click>(*theApp.GetPlatformResource().GetDesktopDownHandle())
		.Add(*this, &ShlReader::OnClick);
	FetchEvent<KeyPress>(*theApp.GetPlatformResource().GetDesktopDownHandle())
		.Add(*this, &ShlReader::OnKeyPress);
	RequestFocusCascade(*theApp.GetPlatformResource().GetDesktopDownHandle());
	UpdateToScreen();
	return 0;
}

int
ShlReader::OnDeactivated(const Message& /*msg*/)
{
	ShlClearBothScreen();
	FetchEvent<Click>(*theApp.GetPlatformResource().GetDesktopDownHandle())
		.Remove(*this, &ShlReader::OnClick);
	FetchEvent<KeyPress>(*theApp.GetPlatformResource().GetDesktopDownHandle())
		.Remove(*this, &ShlReader::OnKeyPress);
	theApp.GetPlatformResource().GetDesktopUp().SetBackground(hUp);
	theApp.GetPlatformResource().GetDesktopDown().SetBackground(hDn);
	Reader.UnloadText();
	safe_delete_obj()(pTextFile);
	return 0;
}

int
ShlReader::ShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_INPUT:
		ResponseInput(msg);
		UpdateToScreen();
		return 0;

	default:
		break;
	}
	return DefShellProc(msg);
}

void
ShlReader::UpdateToScreen()
{
	if(bgDirty)
	{
		bgDirty = false;
		//强制刷新背景。
		theApp.GetPlatformResource().GetDesktopUp().SetRefresh(true);
		theApp.GetPlatformResource().GetDesktopDown().SetRefresh(true);
		theApp.GetPlatformResource().GetDesktopUp().Refresh();
		theApp.GetPlatformResource().GetDesktopDown().Refresh();
		Reader.PrintText();
		theApp.GetPlatformResource().GetDesktopUp().Update();
		theApp.GetPlatformResource().GetDesktopDown().Update();
	}
}

void
ShlReader::OnClick(TouchEventArgs& /*e*/)
{
	CallStored<ShlExplorer>();
}

void
ShlReader::OnKeyPress(KeyEventArgs& e)
{
	u32 k(static_cast<KeyEventArgs::Key>(e));

	bgDirty = true;
	switch(k)
	{
	case KeySpace::Enter:
		Reader.Update();
		break;

	case KeySpace::ESC:
		CallStored<ShlExplorer>();
		break;

	case KeySpace::Up:
	case KeySpace::Down:
	case KeySpace::PgUp:
	case KeySpace::PgDn:
		{
			switch(k)
			{
			case KeySpace::Up:
				Reader.LineUp();
				break;
			case KeySpace::Down:
				Reader.LineDown();
				break;
			case KeySpace::PgUp:
				Reader.ScreenUp();
				break;
			case KeySpace::PgDn:
				Reader.ScreenDown();
				break;
			}
		}
		break;

	case KeySpace::X:
		Reader.SetLineGap(5);
		Reader.Update();
		break;

	case KeySpace::Y:
		Reader.SetLineGap(8);
		Reader.Update();
		break;

	case KeySpace::Left:
		//Reader.SetFontSize(Reader.GetFontSize()+1);
		if(Reader.GetLineGap() != 0)
		{
			Reader.SetLineGap(Reader.GetLineGap() - 1);
			Reader.Update();
		}
		break;

	case KeySpace::Right:
		//PixelType cc(Reader.GetColor());
		//Reader.SetColor(ARGB16(1,(cc&(15<<5))>>5,cc&29,(cc&(31<<10))>>10));
		if(Reader.GetLineGap() != 12)
		{
			Reader.SetLineGap(Reader.GetLineGap() + 1);
			Reader.Update();
		}
		break;

	default:
		return;
	}
}

YSL_END

