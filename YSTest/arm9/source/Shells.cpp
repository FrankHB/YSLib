/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Shells.cpp
\ingroup YReader
\brief Shell 实现。
\version 0.3467;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 + 08:00;
\par 修改时间:
	2010-01-02 10:43 + 08:00;
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
	static void
	dfa(BitmapPtr buf, SDST x, SDST y)
	{
		//raz2
		buf[y * SCRW + x] = ARGB16(1, ((x >> 2) + 15) & 31,
			((y >> 2) + 15) & 31, ((~(x * y) >> 2) + 15) & 31);
	}
	static void
	dfap(BitmapPtr buf, SDST x, SDST y)
	{
		//bza1
		buf[y * SCRW + x] = ARGB16(1, ((x | y << 1) % (y + 2)) & 31,
			((~y | x << 1) % 27 + 3) & 31, ((x << 4) / (y | 1)) & 31);
	}
	static void
	dfac1(BitmapPtr buf, SDST x, SDST y)
	{
		//fl1
		buf[y * SCRW + x] = ARGB16(1, (x + y * y) & 31, (x * x + y) & 31,
			((x & y) ^ (x | y)) & 31);
	}
	static void
	dfac1p(BitmapPtr buf, SDST x, SDST y)
	{
		//rz3
		buf[y * SCRW + x] = ARGB16(1, ((x * y) | y) & 31, ((x * y) | x) & 31,
			((x ^ y) * (x ^ y)) & 31);
	}
	static void
	dfac2(BitmapPtr buf, SDST x, SDST y)
	{
		//v1
		buf[y * SCRW + x] = ARGB16(1, ((x + y) % ((y - 2) & 1) + (x << 2)) & 31,
			(~x % 101 + y) & 31, ((x << 4) / (y & 1)) & 31);
	}
	static void
	dfac2p(BitmapPtr buf, SDST x, SDST y)
	{
		//arz1
		buf[y * SCRW + x] = ARGB16(1, ((x | y) % (y + 2)) & 31,
			((~y | x) % 27 + 3) & 31, ((x << 6) / (y | 1)) & 31);
	}

	////

	//测试用变量。
	static BitmapPtr gbuf;
	static int nCountInput;
	static char strCount[40];
	GHStrong<YImage> g_pi[10];

	void
	LoadL()
	{
		//色块覆盖测试用程序段。
		if(!g_pi[1])
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
			g_pi[1] = NewScrImage(dfa, gbuf);
		//	memset(gbuf, 0xF2, sizeof(ScreenBufferType));
			g_pi[2] = NewScrImage(dfap, gbuf);
			ydelete_array(gbuf);
		}
	}

	void
	LoadS()
	{
		if(!g_pi[3])
		{
			try
			{
				gbuf = ynew ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
			g_pi[3] = NewScrImage(dfac1, gbuf);
			g_pi[4] = NewScrImage(dfac1p, gbuf);
			ydelete_array(gbuf);
		}
	}

	void
	LoadA()
	{
		if(!g_pi[5])
		{
			try
			{
				gbuf = ynew ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
			g_pi[5] = NewScrImage(dfac2, gbuf);
			g_pi[6] = NewScrImage(dfac2p, gbuf);
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
			sizeof(AWindow), sizeof(YFrameWindow), sizeof(YForm),
			nCountInput++, pt.X, pt.Y);
	}

	void
	InputCounterAnother(const Point& /*pt*/)
	{
	//	nCountInput++;
	//	std::sprintf(strCount, "%d,%d,%d,%d,",sizeof(YForm),sizeof(YShell),
	//		sizeof(YApplication),sizeof(YFrameWindow));
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
	return g_pi[i];
}

void
ReleaseShells()
{
	for(std::size_t i(0); i != 10; ++i)
		YReset(g_pi[i]);
	GStaticCache<ShlReader, GHHandle<YShell> >::Release();
	GStaticCache<ShlSetting, GHHandle<YShell> >::Release();
	GStaticCache<ShlExplorer, GHHandle<YShell> >::Release();
	GStaticCache<ShlLoad, GHHandle<YShell> >::Release();
	ReleaseGlobalResource<TextRegion>();
}

using namespace DS;

ShlLoad::TFrmLoadUp::TFrmLoadUp()
	: YForm(Rect::FullScreen, GetImage(1), HWND(hDesktopUp)),
	lblTitle(Rect(50, 20, 100, 22), this),
	lblStatus(Rect(60, 80, 80, 22), this)
{
	lblTitle.Text = YApplication::ProductName;
	lblStatus.Text = "Loading...";
	//	lblTitle.Transparent = true;
	Draw();
}

ShlLoad::TFrmLoadDown::TFrmLoadDown()
	: YForm(Rect::FullScreen, GetImage(2), HWND(hDesktopDown)),
	lblStatus(Rect(30, 20, 160, 22), this)
{
	lblStatus.Text = _ustr("初始化中，请稍后……");
	lblStatus.SetTransparent(true);
	Draw();
}

int
ShlLoad::OnActivated(const Message& /*m*/)
{
	//如果不添加此段且没有桌面没有被添加窗口等设置刷新状态的操作，
	//那么任何绘制都不会进行。
	hDesktopUp->SetRefresh(true);
	hDesktopDown->SetRefresh(true);
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
	: YForm(Rect::FullScreen, GetImage(3), HWND(hDesktopUp)),
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
	: YForm(Rect::FullScreen, GetImage(4), HWND(hDesktopDown)),
	fbMain(Rect(6, 10, 210, 150), this),
	btnTest(Rect(115, 165, 65, 22), this),
	btnOK(Rect(185, 165, 65, 22), this),
	sbTestH(Rect(10, 165, 95, 16), this),
	//	tkTestH(Rect(10, 165, 95, 16), this),
	tkTestV(Rect(230, 10, 16, 95), this)
{
	btnTest.Text = _ustr(" 测试(X)");
	btnOK.Text = _ustr(" 确定(R)");
	FetchEvent<EControl::KeyPress>(*this)
		+= &TFrmFileListSelecter::frm_KeyPress;
	//	fbMain.TouchDown += YFileBox::OnClick;
	//	fbMain.Click += &YListBox::_m_OnClick;
	FetchEvent<EControl::KeyPress>(fbMain) += fb_KeyPress;
	fbMain.Selected.Add(*this, &TFrmFileListSelecter::fb_Selected);
	fbMain.Confirmed += fb_Confirmed;
	FetchEvent<EControl::Click>(btnTest).Add(*this,
		&TFrmFileListSelecter::btnTest_Click);
	FetchEvent<EControl::Click>(btnOK).Add(*this,
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
		FetchEvent<EControl::Click>(btnTest)(btnTest, et);
		break;
	case KeySpace::R:
		FetchEvent<EControl::Click>(btnOK)(btnOK, et);
		break;
	default:
		break;
	}
}

void
ShlExplorer::TFrmFileListSelecter::fb_Selected(IndexEventArgs& /*e*/)
{
	YLabel& l(HandleCast<TFrmFileListMonitor>(
		HandleCast<ShlExplorer>(FetchShellHandle())->hWndUp)->lblPath);

	l.Text = fbMain.GetPath();
	l.Refresh();
}

void
ShlExplorer::fb_KeyPress(IVisualControl& /*sender*/, KeyEventArgs& e)
{
	Key x(e);

	if(x & KeySpace::L)
		switchShl1();
}
void
ShlExplorer::fb_Confirmed(IVisualControl& /*sender*/, IndexEventArgs& /*e*/)
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

//	const WPARAM& wParam(msg.GetWParam());
//	YDebugSetStatus();
/*	static YTimer Timer(1250);

	if(Timer.Refresh())
	{
	//	InitYSConsole();
	//	YDebugBegin();
		iprintf("time : %u ticks\n", GetTicks());
		iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\n"
			"W : %u;\nL : %lx;\n", msg.GetMessageID(),
			msg.GetPriority(), msg.GetObjectID(), msg.GetWParam(), msg.GetLParam());
		WaitForInput();
	//	StartTicks();
	}*/
/*
	YDebugBegin();
	iprintf("time : %u ticks\n", GetTicks());
	iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\n"
		"W : %u;\nL : %lx;\n", msg.GetMessageID(), msg.GetPriority(),
		msg.GetObjectID(), msg.GetWParam(), msg.GetLParam());
	WaitForInput();*/

	switch(msg.GetMessageID())
	{
	case SM_INPUT:
/*
		InitYSConsole();
		iprintf("%d,(%d,%d)\n",msg.GetWParam(),
			msg.GetCursorLocation().X, msg.GetCursorLocation().Y);
*/
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
	ReplaceHandle<HWND>(hWndUp, new TFrmFileListMonitor(GHHandle<YShell>(this)));
	ReplaceHandle<HWND>(hWndDown, new TFrmFileListSelecter(GHHandle<YShell>(this)));
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
	SendMessage(NULL, SM_SCRREFRESH, 0x80, DefaultShellHandle->scrType, 0);
}
*/

YSL_END_SHELL(ShlSetting)

HWND ShlSetting::hWndC(NULL);

ShlSetting::TFormA::TFormA()
	: YForm(Rect::FullScreen, GetImage(5), HWND(hDesktopUp)),
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
	: YForm(Rect(10, 40, 228, 70), NULL, HWND(hDesktopDown)),
	btnB(Rect(2, 5, 224, s_size), this), /*GetImage(6)*/
	btnB2(Rect(45, 35, 124, s_size), this)
{
	btnB.Text = _ustr("测试程序");
	btnB2.Text = _ustr("测试程序2");
	BackColor = ARGB16(1, 31, 31, 15);
	FetchEvent<EControl::TouchMove>(*this) += OnDrag;
	//	btnB.TouchMove += &AVisualControl::OnTouchMove;
	FetchEvent<EControl::Enter>(btnB) += btnB_Enter;
	FetchEvent<EControl::Leave>(btnB) += btnB_Leave;
	FetchEvent<EControl::TouchMove>(btnB2) += OnDrag;
	//	btnB2.TouchDown += btnC_Click;
	//	btnB.Enabled = false;
}

void
ShlSetting::TFormB::btnB_Enter(IVisualControl& sender, InputEventArgs& e)
{
	DefDynInitRef(YButton, btn, sender)
	TouchEventArgs& pt(static_cast<TouchEventArgs&>(e));
	char str[20];

	std::sprintf(str, "Enter:(%d,%d)", pt.Point::X, pt.Point::Y);
	btn.Text = str;
	btn.Refresh();
}
void
ShlSetting::TFormB::btnB_Leave(IVisualControl& sender, InputEventArgs& e)
{
	DefDynInitRef(YButton, btn, sender)
	TouchEventArgs& pt(static_cast<TouchEventArgs&>(e));
	char str[20];

	std::sprintf(str, "Leave:(%d,%d)", pt.Point::X, pt.Point::Y);
	btn.Text = str;
	btn.Refresh();
}

ShlSetting::TFormC::TFormC()
	: YForm(Rect(5, 60, 180, 120), NULL, HWND(hDesktopDown)),
	btnC(Rect(13, 45, 184, s_size), this),/*GetImage(7)*/
	btnReturn(Rect(13, 82, 60, s_size), this),
	btnExit(Rect(83, 82, 60, s_size), this)
{
	btnC.Text = _ustr("测试y");
	btnReturn.Text = _ustr("返回");
	btnExit.Text = _ustr("退出");
	BackColor = ARGB16(1, 31, 15, 15);
	FetchEvent<EControl::TouchDown>(*this) += TFormC_TouchDown;
	FetchEvent<EControl::TouchMove>(*this) += OnDrag;
	FetchEvent<EControl::TouchUp>(btnC).Add(*this,
		&TFormC::btnC_TouchUp);
	FetchEvent<EControl::TouchDown>(btnC).Add(*this,
		&TFormC::btnC_TouchDown);
	FetchEvent<EControl::TouchMove>(btnC) += OnDrag;
	FetchEvent<EControl::Click>(btnC).Add(*this, &TFormC::btnC_Click);
	FetchEvent<EControl::KeyPress>(btnC) += btnC_KeyPress;
	//	btnC.Enabled = false;
	btnReturn.BackColor = ARGB16(1, 22, 23, 24);
	FetchEvent<EControl::Click>(btnReturn).Add(*this,
		&TFormC::btnReturn_Click);
	FetchEvent<EControl::Click>(btnExit).Add(*this,
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
	static const int ffilen(theApp.pFontCache->GetFilesN());
	static const int ftypen(theApp.pFontCache->GetTypesN());
	static const int ffacen(theApp.pFontCache->GetFacesN());
	static int itype;
	static YFontCache::FTypes::const_iterator it(
		theApp.pFontCache->GetTypes().begin());

	//	btnC.Transparent ^= 1;
	if(nCountInput & 1)
	{
		//	btnC.Visible ^= 1;
		++itype %= ftypen;
		if(++it == theApp.pFontCache->GetTypes().end())
			it = theApp.pFontCache->GetTypes().begin();
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
		//		theApp.pFontCache->GetTypefacePtr("FZYaoti", "Regular"));
		btnC.Text = strtf;
	}
	//	btnC.Refresh();
}
void
ShlSetting::TFormC::btnC_KeyPress(IVisualControl& sender, KeyEventArgs& e)
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
ShlSetting::TFormC_TouchDown(IVisualControl& sender, TouchEventArgs& /*e*/)
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
	hDesktopDown->BackColor = ARGB16(1, 15, 15, 31);
	hDesktopDown->SetBackground(GetImage(6));
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
	hUp = hDesktopUp->GetBackground();
	hDn = hDesktopDown->GetBackground();
	hDesktopUp->SetBackground(NULL);
	hDesktopDown->SetBackground(NULL);
	hDesktopUp->BackColor = ARGB16(1, 30, 27, 24);
	hDesktopDown->BackColor = ARGB16(1, 24, 27, 30);
	FetchEvent<EControl::Click>(*hDesktopDown).Add(*this,
		&ShlReader::OnClick);
	FetchEvent<EControl::KeyPress>(*hDesktopDown).Add(*this,
		&ShlReader::OnKeyPress);
	RequestFocusCascade(*hDesktopDown);
	UpdateToScreen();
	return 0;
}

int
ShlReader::OnDeactivated(const Message& /*msg*/)
{
	ShlClearBothScreen();
	FetchEvent<EControl::Click>(*hDesktopDown).Remove(*this,
		&ShlReader::OnClick);
	FetchEvent<EControl::KeyPress>(*hDesktopDown).Remove(*this,
		&ShlReader::OnKeyPress);
	hDesktopUp->SetBackground(hUp);
	hDesktopDown->SetBackground(hDn);
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
		hDesktopUp->SetRefresh(true);
		hDesktopDown->SetRefresh(true);
		hDesktopUp->Refresh();
		hDesktopDown->Refresh();
		Reader.PrintText();
		hDesktopUp->Update();
		hDesktopDown->Update();
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

