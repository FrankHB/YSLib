// YReader -> ShlMain by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-06 21:38:16 + 08:00;
// UTime = 2010-10-21 09:48 + 08:00;
// Version = 0.3153;


#include <Shells.h>

////////
//测试用声明：资源定义。
extern char gstr[128];
char strtbuf[0x400],
	strtf[0x400],
	strttxt[0x400];

YSL_BEGIN

YSL_BEGIN_SHELL(ShlMain)

LRES
ShlProc(const Message& msg)
{
/*
	const HSHL& hShl(msg.GetShellHandle());
	const WPARAM& wParam(msg.GetWParam());
	const LPARAM& lParam(msg.GetLParam());
	const Point& pt(msg.GetCursorLocation());
*/
	switch(msg.GetMsgID())
	{
	case SM_CREATE:
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

//	case SM_DESTROY:
//		GStaticCache<ShlLoad>::Release();

	default:
		return DefShellProc(msg);
	}
}

YSL_END_SHELL(ShlMain)

namespace
{
	//测试函数。

	//背景测试。
	static void
	dfa(BitmapPtr buf, SDST x, SDST y)
	{
		//raz2
		buf[y * SCRW + x] = ARGB16(1, ((x >> 2) + 15) & 31, ((y >> 2) + 15) & 31, ((~(x * y) >> 2) + 15) & 31);
	}
	static void
	dfap(BitmapPtr buf, SDST x, SDST y)
	{
		//bza1
		buf[y * SCRW + x] = ARGB16(1, ((x | y << 1) % (y + 2)) & 31, ((~y | x << 1) % 27 + 3) & 31, ((x << 4) / (y | 1)) & 31);
	}
	static void
	dfac1(BitmapPtr buf, SDST x, SDST y)
	{
		//fl1
		buf[y * SCRW + x] = ARGB16(1, (x + y * y) & 31, (x * x + y) & 31, ((x & y) ^ (x | y)) & 31);
	}
	static void
	dfac1p(BitmapPtr buf, SDST x, SDST y)
	{
		//rz3
		buf[y * SCRW + x] = ARGB16(1, ((x * y) | y) & 31, ((x * y) | x) & 31, ((x ^ y) * (x ^ y)) & 31);
	}
	static void
	dfac2(BitmapPtr buf, SDST x, SDST y)
	{
		//v1
		buf[y * SCRW + x] = ARGB16(1, ((x + y) % ((y - 2) & 1) + (x << 2)) & 31, (~x % 101 + y) & 31, ((x << 4) / (y & 1)) & 31);
	}
	static void
	dfac2p(BitmapPtr buf, SDST x, SDST y)
	{
		//arz1
		buf[y * SCRW + x] = ARGB16(1, ((x | y) % (y + 2)) & 31, ((~y | x) % 27 + 3) & 31, ((x << 6) / (y | 1)) & 31);
	}

	////

	//测试用变量。
	static BitmapPtr gbuf;
	static int nCountInput;
	static char strCount[40];
	GHResource<YImage> g_pi[10];

	void
	LoadL()
	{
		//色块覆盖测试用程序段。
		if(!g_pi[1])
		{
			try
			{
				gbuf = new ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
		//	memset(gbuf, 0xEC, sizeof(ScreenBufferType));
			g_pi[1] = NewScrImage(dfa, gbuf);
		//	memset(gbuf, 0xF2, sizeof(ScreenBufferType));
			g_pi[2] = NewScrImage(dfap, gbuf);
			delete gbuf;
		}
	}

	void
	LoadS()
	{
		if(!g_pi[3])
		{
			try
			{
				gbuf = new ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
			g_pi[3] = NewScrImage(dfac1, gbuf);
			g_pi[4] = NewScrImage(dfac1p, gbuf);
			delete gbuf;
		}
	}

	void
	LoadA()
	{
		if(!g_pi[5])
		{
			try
			{
				gbuf = new ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
			g_pi[5] = NewScrImage(dfac2, gbuf);
			g_pi[6] = NewScrImage(dfac2p, gbuf);
			delete gbuf;
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

	static void
	InputCounter(const Point& pt)
	{
		sprintf(strCount, "%d,%d,%d;Count = %d, Pos = (%d, %d);", sizeof(AWindow), sizeof(YFrameWindow), sizeof(YForm), nCountInput++, pt.X, pt.Y);
	}

	static void
	InputCounterAnother(const Point& pt)
	{
	//	nCountInput++;
		//	sprintf(strCount, "%d,%d,%d,%d,",sizeof(YForm),sizeof(YShell),sizeof(YApplication),sizeof(YFrameWindow));
		struct mallinfo t(mallinfo());
		/*	sprintf(strCount, "%d,%d,%d,%d,%d;",
		t.arena,    // total space allocated from system 2742496
		t.ordblks,  // number of non-inuse chunks 37
		t.smblks,   // unused -- always zero 0
		t.hblks,    // number of mmapped regions 0
		t.hblkhd   // total space in mmapped regions 0
		);*/
		sprintf(strCount, "%d,%d,%d,%d,%d;",
			t.usmblks,  // unused -- always zero 0
			t.fsmblks,  // unused -- always zero 0
			t.uordblks, // total allocated space 2413256
			t.fordblks, // total non-inuse space 329240
			t.keepcost // top-most, releasable (via malloc_trim) space 46496
			);
	}
}

GHResource<YImage>&
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

using namespace DS;

LRES
ShlLoad::OnActivated(const Message& m)
{
	//如果不添加此段且没有桌面没有被添加窗口等设置刷性状态的操作，那么任何绘制都不会进行。
	pDesktopUp->SetRefresh(true);
	pDesktopDown->SetRefresh(true);
	//新建窗口。
	hWndUp = NewWindow<TFrmLoadUp>(this);
	hWndDown = NewWindow<TFrmLoadDown>(this);
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

void
ShlExplorer::TFrmFileListSelecter::frm_KeyPress(const MKeyEventArgs& e)
{
	switch(e.GetKey())
	{
	case KeySpace::X:
		btnTest.Click(btnTest, MTouchEventArgs::FullScreen);
		break;
	case KeySpace::R:
		btnOK.Click(btnOK, MTouchEventArgs::FullScreen);
		break;
	default:
		break;
	}
}

void
ShlExplorer::TFrmFileListSelecter::fb_Selected(const MIndexEventArgs& e)
{
	YLabel& l(HandleCast<TFrmFileListMonitor>(HandleCast<ShlExplorer>(hShell)->hWndUp)->lblPath);

	l.Text = fbMain.GetPath();
	l.Refresh();
}

void
ShlExplorer::fb_KeyPress(IVisualControl& sender, const MKeyEventArgs& e)
{
	Key x(e);

	if(x & KeySpace::L)
		switchShl1();
}

void
ShlExplorer::fb_Confirmed(IVisualControl& sender, const MIndexEventArgs& e)
{
//	if(e.Index == 2)
//		switchShl1();
}

void
ShlExplorer::TFrmFileListSelecter::btnTest_Click(const MTouchEventArgs&)
{
	switchShl1();
/*	if(fbMain.IsSelected())
	{
		YConsole con(*pScreenUp);

		con.Activate(Color::Silver);

		iprintf("Current Working Directory:\n%s\n", IO::GetNowDirectory().c_str());
		iprintf("FileBox Path:\n%s\n", fbMain.GetPath().c_str());
	//	std::fprintf(stderr, "err");
		WaitForInput();
	}
	else
	{
		YConsole con(*pScreenDown);
		con.Activate(Color::Yellow, ColorSpace::Green);
		iprintf("FileBox Path:\n%s\n", fbMain.GetPath().c_str());
		puts("OK");
		WaitForInput();
	}*/
}

void
ShlExplorer::TFrmFileListSelecter::btnOK_Click(const MTouchEventArgs&)
{
	if(fbMain.IsSelected())
	{
		const string& s(fbMain.GetPath().GetNativeString());
/*	YConsole con;
	con.Activate();
	iprintf("%s\n%s\n%s\n%d,%d\n",fbMain.GetDirectory().c_str(), StringToMBCS(fbMain.YListBox::GetList()[fbMain.GetSelected()]).c_str(),
		s.c_str(),IO::ValidateDirectory(s), stdex::fexists(s.c_str()));
	WaitForABXY();
	con.Deactivate();*/
		if(!IO::ValidateDirectory(s) && stdex::fexists(s.c_str()))
			switchShl2(s.c_str());
	}
}

LRES
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
		iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\nW : %u;\nL : %lx;\n", msg.GetMsgID(), msg.GetPriority(), msg.GetID(), msg.GetWParam(), msg.GetLParam());
		WaitForInput();
	//	StartTicks();
	}*/
/*
	YDebugBegin();
	iprintf("time : %u ticks\n", GetTicks());
	iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\nW : %u;\nL : %lx;\n", msg.GetMsgID(), msg.GetPriority(), msg.GetID(), msg.GetWParam(), msg.GetLParam());
	WaitForInput();*/

	switch(msg.GetMsgID())
	{
	case SM_INPUT:
/*
		InitYSConsole();
		iprintf("%d,(%d,%d)\n",msg.GetWParam(), msg.GetCursorLocation().X, msg.GetCursorLocation().Y);
*/
		ResponseInput(msg);
		UpdateToScreen();
		return 0;

	default:
		return ShlDS::ShlProc(msg);
	}
}

LRES
ShlExplorer::OnActivated(const Message&)
{
	hWndUp = NewWindow<TFrmFileListMonitor>(this);
	hWndDown = NewWindow<TFrmFileListSelecter>(this);
/*
	ReplaceHandle<HWND>(hWndUp, new TFrmFileListMonitor(HSHL(this)));
	ReplaceHandle<HWND>(hWndDown, new TFrmFileListSelecter(HSHL(this)));
*/
	//	HandleCast<TFrmFileListSelecter>(hWndDown)->fbMain.RequestFocus(GetZeroElement<MEventArgs>());
	//	hWndDown->RequestFocus(GetZeroElement<MEventArgs>());
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
	InsertMessage(NULL, SM_SCRREFRESH, 0x80, hShellMain->scrType, 0);
}
*/

YSL_END_SHELL(ShlSetting)

HWND ShlSetting::hWndC(NULL);

void
ShlSetting::TFormB::btnB_Enter(IVisualControl& sender, const MInputEventArgs& e)
{
	DefDynInitRef(YButton, btn, sender)
	const MTouchEventArgs& pt(static_cast<const MTouchEventArgs&>(e));
	char str[20];

	std::sprintf(str, "Enter:(%d,%d)", pt.Point::X, pt.Point::Y);
	btn.Text = str;
	btn.Refresh();
}
void
ShlSetting::TFormB::btnB_Leave(IVisualControl& sender, const MInputEventArgs& e)
{
	DefDynInitRef(YButton, btn, sender)
	const MTouchEventArgs& pt(static_cast<const MTouchEventArgs&>(e));
	char str[20];

	std::sprintf(str, "Leave:(%d,%d)", pt.Point::X, pt.Point::Y);
	btn.Text = str;
	btn.Refresh();
}

void
ShlSetting::TFormC::btnC_TouchUp(const MTouchEventArgs& e)
{
	InputCounter(e);
	HandleCast<ShlSetting>(hShell)->ShowString(strCount);
	btnC.Refresh();
}
void
ShlSetting::TFormC::btnC_TouchDown(const MTouchEventArgs& e)
{
	InputCounterAnother(e);
	HandleCast<ShlSetting>(hShell)->ShowString(strCount);
//	btnC.Refresh();
}
void
ShlSetting::TFormC::btnC_Click(const MTouchEventArgs& e)
{

	static const int ffilen(pDefaultFontCache->GetFilesN());
	static const int ftypen(pDefaultFontCache->GetTypesN());
	static const int ffacen(pDefaultFontCache->GetFacesN());
	static int itype;
	static YFontCache::FTypes::const_iterator it(pDefaultFontCache->GetTypes().begin());

	//	btnC.Transparent ^= 1;
	if(nCountInput & 1)
	{
		//	btnC.Visible ^= 1;
		++itype %= ftypen;
		if(++it == pDefaultFontCache->GetTypes().end())
			it = pDefaultFontCache->GetTypes().begin();
		btnC.Font = Font(*(*it)->GetFontFamilyPtr(), 18 - (itype << 1), EFontStyle::Regular);
	//	btnC.Font = Font(*(*it)->GetFontFamilyPtr()/*GetDefaultFontFamily()*/, 18 - (itype << 1), EFontStyle::Regular);
		sprintf(strtf, "%d, %d file(s), %d type(s), %d faces(s);\n", btnC.Font.GetSize(), ffilen, ftypen, ffacen);
		btnC.Text = strtf;
	}
	else
	{
		sprintf(strtf, "%d/%d;%s:%s;\n", itype + 1, ftypen, (*it)->GetFamilyName(), (*it)->GetStyleName());
		//	sprintf(strtf, "B%p\n", pDefaultFontCache->GetTypefacePtr("FZYaoti", "Regular"));
		btnC.Text = strtf;
	}
	//	btnC.Refresh();
}
void
ShlSetting::TFormC::btnC_KeyPress(IVisualControl& sender, const MKeyEventArgs& e)
{
	//测试程序。

	u32 k(static_cast<MKeyEventArgs::Key>(e));

	DefDynInitRef(YButton, lbl, sender);
//	YButton& lbl(dynamic_cast<TFormA&>(*(dynamic_cast<ShlSetting&>(*NowShell()).hWndUp)).lblA2);
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
ShlSetting::TFormC::btnReturn_Click(const MTouchEventArgs&)
{
	CallStored<ShlExplorer>();
}

LRES
ShlSetting::OnActivated(const Message& msg)
{
	pDesktopDown->BackColor = ARGB16(1, 15, 15, 31);
	pDesktopDown->SetBackground(GetImage(6));
	hWndUp = NewWindow<TFormA>(this);
	hWndDown = NewWindow<TFormB>(this);
	hWndC = NewWindow<TFormC>(this);
	hWndUp->Draw();
	hWndDown->Draw();
	hWndC->Draw();
	UpdateToScreen();
	return 0;
}

LRES
ShlSetting::OnDeactivated(const Message& m)
{
	ParentType::OnDeactivated(m);
	YDelete(hWndC);
	return 0;
}

LRES
ShlSetting::ShlProc(const Message& msg)
{
	using namespace YSL_SHL(ShlSetting);
//	ClearDefaultMessageQueue();

//	const WPARAM& wParam(msg.GetWParam());

//	UpdateToScreen();

	switch(msg.GetMsgID())
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

LRES
ShlReader::OnActivated(const Message& msg)
{
	pTextFile = new YTextFile(path.c_str());
	Reader.LoadText(*pTextFile);
	bgDirty = true;
	hUp = pDesktopUp->GetBackground();
	hDn = pDesktopDown->GetBackground();
	pDesktopUp->SetBackground(NULL);
	pDesktopDown->SetBackground(NULL);
	pDesktopUp->BackColor = ARGB16(1, 30, 27, 24);
	pDesktopDown->BackColor = ARGB16(1, 24, 27, 30);
	pDesktopDown->Click.Add(*this, &ShlReader::OnClick);
	pDesktopDown->KeyPress.Add(*this, &ShlReader::OnKeyPress);
	RequestFocusCascade(*pDesktopDown);
	UpdateToScreen();
	return 0;
}

LRES
ShlReader::OnDeactivated(const Message& msg)
{
	ShlClearBothScreen();
	pDesktopDown->Click.Remove(*this, &ShlReader::OnClick);
	pDesktopDown->KeyPress.Remove(*this, &ShlReader::OnKeyPress);
	pDesktopUp->SetBackground(hUp);
	pDesktopDown->SetBackground(hDn);
	Reader.UnloadText();
	safe_delete_obj()(pTextFile);
	return 0;
}

LRES
ShlReader::ShlProc(const Message& msg)
{
	switch(msg.GetMsgID())
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
		pDesktopUp->SetRefresh(true);
		pDesktopDown->SetRefresh(true);
		pDesktopUp->Refresh();
		pDesktopDown->Refresh();
		Reader.PrintText();
		pDesktopUp->Update();
		pDesktopDown->Update();
	}
}

void
ShlReader::OnClick(const MTouchEventArgs& e)
{
	CallStored<ShlExplorer>();
}

void
ShlReader::OnKeyPress(const MKeyEventArgs& e)
{
	u32 k(static_cast<MKeyEventArgs::Key>(e));

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

