// YReader -> ShlMain by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-6 21:38:16;
// UTime = 2010-9-1 21:21;
// Version = 0.2975;


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
ShlProc(const MMSG& msg)
{
/*
	const HSHL& hShl(msg.GetShellHandle());
	const WPARAM& wParam(msg.GetWParam());
	const LPARAM& lParam(msg.GetLParam());
	const SPoint& pt(msg.GetCursorLocation());
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
			throw MLoggedEvent("Run shell failed at end of ShlMain.");
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
				gbuf = new SCRBUF;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
		//	memset(gbuf, 0xEC, sizeof(SCRBUF));
			g_pi[1] = NewScrImage(dfa, gbuf);
		//	memset(gbuf, 0xF2, sizeof(SCRBUF));
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
				gbuf = new SCRBUF;
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
				gbuf = new SCRBUF;
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
		CallStored<ShlA>();
	}

	void
	switchShl2(CPATH pth)
	{
		ShlReader::path = pth;
		CallStored<ShlReader>();
	}

	static void
	InputCounter(const SPoint& pt)
	{
		sprintf(strCount, "%d,%d,%d;Count = %d, Pos = (%d, %d);", sizeof(AWindow), sizeof(YFrameWindow), sizeof(YForm), nCountInput++, pt.X, pt.Y);
	}

	static void
	InputCounterAnother(const SPoint& pt)
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
ShlLoad::OnActivated(const MMSG& m)
{
	//如果不添加此段且没有桌面没有被添加窗口等设置刷性状态的操作，那么任何绘制都不会进行。
	pDesktopUp->SetRefresh();
	pDesktopDown->SetRefresh();
	//新建窗口。
	hWndUp = NewWindow<TFrmLoadUp>(this);
	hWndDown = NewWindow<TFrmLoadDown>(this);
	DispatchWindows();
	UpdateToScreen();
	try
	{
		SetShellToStored<ShlS>();
	}
	catch(...)
	{
		throw MLoggedEvent("Run shell failed at end of ShlLoad.");
		return -1;
	}
	return 0;
}

void
ShlS::TFrmFileListSelecter::frm_KeyPress(const MKeyEventArgs& e)
{
	switch(e)
	{
	case Keys::X:
		btnTest.Click(btnTest, MTouchEventArgs::FullScreen);
		break;
	case Keys::R:
		btnOK.Click(btnOK, MTouchEventArgs::FullScreen);
		break;
	default:
		break;
	}
}

void
ShlS::TFrmFileListSelecter::fb_Selected(const MIndexEventArgs& e)
{
	YLabel& l(HandleCast<TFrmFileListMonitor>(HandleCast<ShlS>(hShell)->hWndUp)->lblPath);

	l.Text = fbMain.List[e.index];
	l.DrawForeground();
	l.Refresh();
}

void
ShlS::fb_KeyPress(IVisualControl& sender, const MKeyEventArgs& e)
{
	Keys x(e);

	if(x & Keys::L)
		switchShl1();
}

void
ShlS::fb_Confirmed(IVisualControl& sender, const MIndexEventArgs& e)
{
	if(e.index == 2)
		switchShl1();
}
void
ShlS::TFrmFileListSelecter::btnTest_Click(const MTouchEventArgs&)
{
	if(fbMain.IsSelected())
		switchShl1();
}

void
ShlS::TFrmFileListSelecter::btnOK_Click(const MTouchEventArgs&)
{
	if(fbMain.IsSelected())
		switchShl2("/test.txt");
	//"/Data/test.txt";
}

LRES
ShlS::ShlProc(const MMSG& msg)
{

//	const WPARAM& wParam(msg.GetWParam());
//	YDebugSetStatus();
/*	static YTimer Timer(1250);

	if(Timer.Refresh())
	{
	//	InitYSConsole();
	//	YDebugBegin();
		iprintf("time : %u ticks\n", GetTicks());
		iprintf("MMSG : 0x%04X;\nPrior : 0x%02X;\nObj : %d\nW : %u;\nL : %lx;\n", msg.GetMsgID(), msg.GetPriority(), msg.GetID(), msg.GetWParam(), msg.GetLParam());
		waitForInput();
	//	StartTicks();
	}*/
/*
	YDebugBegin();
	iprintf("time : %u ticks\n", GetTicks());
	iprintf("MMSG : 0x%04X;\nPrior : 0x%02X;\nObj : %d\nW : %u;\nL : %lx;\n", msg.GetMsgID(), msg.GetPriority(), msg.GetID(), msg.GetWParam(), msg.GetLParam());
	waitForInput();*/

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
ShlS::OnActivated(const MMSG&)
{
	hWndUp = NewWindow<TFrmFileListMonitor>(this);
	hWndDown = NewWindow<TFrmFileListSelecter>(this);
/*
	ReplaceHandle<HWND>(hWndUp, new TFrmFileListMonitor(HSHL(this)));
	ReplaceHandle<HWND>(hWndDown, new TFrmFileListSelecter(HSHL(this)));
*/
	//	HandleCast<TFrmFileListSelecter>(hWndDown)->fbMain.RequestFocus();
	//	hWndDown->RequestFocus();
	RequestFocusCascade(HandleCast<TFrmFileListSelecter>(hWndDown)->fbMain);
	DispatchWindows();
	UpdateToScreen();
	return 0;
}


YSL_BEGIN_SHELL(ShlA)

//屏幕刷新消息生成函数。
//输出设备刷新函数。
/*static void ScrRefresh()
{
	InsertMessage(NULL, SM_SCRREFRESH, 0x80, hShellMain->scrType, 0);
}
*/

YSL_END_SHELL(ShlA)

HWND ShlA::hWndC(NULL);

void
ShlA::TFormC::lblC_TouchUp(const MTouchEventArgs& e)
{
	InputCounter(e);
	HandleCast<ShlA>(hShell)->ShowString(strCount);
	lblC.Refresh();
}

void
ShlA::TFormC::lblC_TouchDown(const MTouchEventArgs& e)
{
	InputCounterAnother(e);
	HandleCast<ShlA>(hShell)->ShowString(strCount);
//	lblC.Refresh();
}

void
ShlA::TFormC::lblC_Click(const MTouchEventArgs& e)
{

	static const int ffilen(pDefaultFontCache->GetFilesN());
	static const int ftypen(pDefaultFontCache->GetTypesN());
	static const int ffacen(pDefaultFontCache->GetFacesN());
	static int itype;
	static YFontCache::FTypes::const_iterator it(pDefaultFontCache->GetTypes().begin());

	//	lblC.Transparent ^= 1;
	if(nCountInput & 1)
	{
		//	lblC.Visible ^= 1;
		++itype %= ftypen;
		if(++it == pDefaultFontCache->GetTypes().end())
			it = pDefaultFontCache->GetTypes().begin();
		lblC.Font = MFont(*(*it)->GetFontFamilyPtr()/*GetDefaultFontFamily()*/, 18 - (itype << 1), EFontStyle::Regular);
		sprintf(strtf, "%d, %d file(s), %d type(s), %d faces(s);\n", lblC.Font.GetSize(), ffilen, ftypen, ffacen);
		lblC.Text = strtf;
	}
	else
	{
		sprintf(strtf, "%d/%d;%s:%s;\n", itype + 1, ftypen, (*it)->GetFamilyName(), (*it)->GetStyleName());
		//	sprintf(strtf, "B%p\n", pDefaultFontCache->GetTypefacePtr("FZYaoti", "Regular"));
		lblC.Text = strtf;
	}
	//	lblC.Refresh();
}

void
ShlA::TFormC::lblC_KeyPress(IVisualControl& sender, const MKeyEventArgs& e)
{
	//测试程序。

	u32 k(e);

	YLabel& lbl(static_cast<TFormA&>(*(static_cast<ShlA&>(*NowShell()).hWndUp)).lblA2);
	lbl.Transparent ^= 1;
	++lbl.ForeColor;
	--lbl.BackColor;
	sprintf(strttxt, "%d;\n", k);
	lbl.Text = strttxt;
	lbl.Refresh();
/*
	YLabel& lbl(static_cast<YLabel&>(sender));

	if(nCountInput & 1)
	{
		sprintf(strtf, "测试键盘...");
		lbl.Text = strtf;
	}*/
}

void
ShlA::TFormC::btnReturn_Click(const MTouchEventArgs&)
{
	CallStored<ShlS>();
}

LRES
ShlA::ShlProc(const MMSG& msg)
{
	using namespace YSL_SHL(ShlA);
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

LRES
ShlA::OnActivated(const MMSG& msg)
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
ShlA::OnDeactivated(const MMSG& m)
{
	ParentType::OnDeactivated(m);
	YDelete(hWndC);
	return 0;
}


std::string ShlReader::path;

ShlReader::ShlReader()
: ShlGUI(),
TextFile(path.c_str()), Reader(TextFile), hUp(NULL), hDn(NULL), bgDirty(true)
{
}

void
ShlReader::UpdateToScreen()
{
	if(bgDirty)
	{
		ParentType::UpdateToScreen();
		Reader.PrintText();
		bgDirty = false;
	}
}

void
ShlReader::OnClick(const MTouchEventArgs& e)
{
	CallStored<ShlS>();
}

void
ShlReader::OnKeyPress(const MKeyEventArgs& e)
{
	u32 k(e);

	bgDirty = true;
	switch(k)
	{
	case Keys::Enter:
		Reader.Update();
		break;

	case Keys::ESC:
		CallStored<ShlS>();
		break;

	case Keys::Up:
	case Keys::Down:
	case Keys::PgUp:
	case Keys::PgDn:
		{
			switch(k)
			{
			case Keys::Up:
				Reader.LineUp();
				break;
			case Keys::Down:
				Reader.LineDown();
				break;
			case Keys::PgUp:
				Reader.ScreenUp();
				break;
			case Keys::PgDn:
				Reader.ScreenDown();
				break;
			}
		}
		break;

	case Keys::X:
		Reader.SetLineGap(5);
		Reader.Update();
		break;

	case Keys::Y:
		Reader.SetLineGap(8);
		Reader.Update();
		break;

	case Keys::Left:
		//Reader.SetFontSize(Reader.GetFontSize()+1);
		Reader.SetLineGap(Reader.GetLineGap() + 1);
		Reader.Update();
		break;

	case Keys::Right:
		//PixelType cc(Reader.GetColor());
		//Reader.SetColor(ARGB16(1,(cc&(15<<5))>>5,cc&29,(cc&(31<<10))>>10));
		Reader.SetLineGap(Reader.GetLineGap() - 1);
		Reader.Update();
		break;

	default:
		return;
	}
}

LRES
ShlReader::ShlProc(const MMSG& msg)
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

LRES
ShlReader::OnActivated(const MMSG& msg)
{
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
//	InsertMessage(NULL, 0x10000, 0x1F);
	UpdateToScreen();
	return 0;
}

LRES
ShlReader::OnDeactivated(const MMSG& msg)
{
	ShlClearBothScreen();
	pDesktopDown->Click.Remove(*this, &ShlReader::OnClick);
	pDesktopDown->KeyPress.Remove(*this, &ShlReader::OnKeyPress);
	pDesktopUp->SetBackground(hUp);
	pDesktopDown->SetBackground(hDn);
	return 0;
}

YSL_END

