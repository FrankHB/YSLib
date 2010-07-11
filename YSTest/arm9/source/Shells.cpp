// YReader -> ShlMain by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-6 21:38:16;
// UTime = 2010-7-10 14:32;
// Version = 0.2801;


#include <Shells.h>

////////
//测试用声明。
//测试用资源定义。
extern char gstr[128];
char strtbuf[0x400],
	strtf[0x400],
	strttxt[0x400];

//测试用常量。
//static CPATH TXTPATH("/test.txt");//"/Data/test.txt";

YSL_BEGIN

//测试用变量。
static BitmapPtr gbuf;
static int nCountInput;
static char strCount[40];

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
			theApp.log.FatalError("Run shell failed at end of ShlMain.");
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
	void
	switchShl1()
	{
		CallStored<ShlA>();
	}
	void
	switchShl2()
	{
		CallStored<ShlReader>();
	}
}

using namespace DS;

HResource<YImage> ShlLoad::pi, ShlLoad::pip;

void
ShlLoad::Load()
{
	static bool unInit(true);

	if(unInit)
	{
		try
		{
			gbuf = new SCRBUF;
		}
		catch(...)
		{
			return;
		}
		pi = NewScrImage(dfa, gbuf);
		pip = NewScrImage(dfap, gbuf);
		delete gbuf;
		unInit = false;
	}
}

LRES
ShlLoad::OnActivated(const MMSG& m)
{
	try
	{
		Load();
		hWndUp = NewWindow<TFrmLoadUp>(this);
		hWndDown = NewWindow<TFrmLoadDown>(this);
		DrawWindows();
		UpdateToScreen();
	}
	catch(...)
	{}
	try
	{
		SetShellToStored<ShlS>();
	}
	catch(...)
	{
		theApp.log.FatalError("Run shell failed at end of ShlLoad.");
		return -1;
	}
	return 0;
}

LRES
ShlLoad::OnDeactivated(const MMSG& m)
{
	NowShellClearBothScreen();
	YDelete(hWndUp);
	YDelete(hWndDown);
	return 0;
}


HResource<YImage> ShlS::pic1, ShlS::pic1p;

void
ShlS::Load()
{
	//色块覆盖测试用程序段。
	static bool unInit(true);

	if(unInit)
	{
		try
		{
			gbuf = new SCRBUF;
		}
		catch(...)
		{
			return;
		}
//		memset(gbuf, 0xEC, sizeof(SCRBUF));
		pic1 = NewScrImage(dfac1, gbuf);
//		memset(gbuf, 0xF2, sizeof(SCRBUF));
		pic1p = NewScrImage(dfac1p, gbuf);
		delete gbuf;
		unInit = false;
	}
}

void
ShlS::TFrmFileListSelecter::frm_KeyPress(const YKeyEventArgs& e)
{
	switch(e)
	{
	case Keys::X:
		btnTest.Click(btnTest, YTouchEventArgs::FullScreen);
		break;
	case Keys::R:
		btnOK.Click(btnOK, YTouchEventArgs::FullScreen);
		break;
	default:
		break;
	}
}

void
ShlS::TFrmFileListSelecter::fb_Selected(const YIndexEventArgs& e)
{
	YLabel& l(HandleCast<TFrmFileListMonitor>(HandleCast<ShlS>(hShell)->hWndUp)->lblPath);

	l.Text = fbMain.List[e.index];
	l.DrawForeground();
	l.Refresh();
}

void
ShlS::fb_KeyPress(IVisualControl& sender, const YKeyEventArgs& e)
{
	Keys x(e);

	if(x & Keys::L)
		switchShl1();
}

void
ShlS::fb_Confirmed(IVisualControl& sender, const YIndexEventArgs& e)
{
	if(e.index == 2)
		switchShl1();
}
void
ShlS::TFrmFileListSelecter::btnTest_Click(const YTouchEventArgs&)
{
	if(fbMain.IsSelected())
		switchShl1();
}

void
ShlS::TFrmFileListSelecter::btnOK_Click(const YTouchEventArgs&)
{
	if(fbMain.IsSelected())
		switchShl2();
}

LRES
ShlS::ShlProc(const MMSG& msg)
{

//	const WPARAM& wParam(msg.GetWParam());
	YDebugSetStatus();
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
	try
	{
		ShlS::Load();
		hWndUp = NewWindow<TFrmFileListMonitor>(this);
		hWndDown = NewWindow<TFrmFileListSelecter>(this);
/*
		ReplaceHandle<HWND>(hWndUp, new TFrmFileListMonitor(HSHL(this)));
		ReplaceHandle<HWND>(hWndDown, new TFrmFileListSelecter(HSHL(this)));
*/
	//	HandleCast<TFrmFileListSelecter>(hWndDown)->fbMain.RequestFocus();
	//	hWndDown->RequestFocus();
		RequestFocusCascade(HandleCast<TFrmFileListSelecter>(hWndDown)->fbMain);
		DrawWindows();
		UpdateToScreen();
	}
	catch(...)
	{}
	return 0;
}

LRES
ShlS::OnDeactivated(const MMSG&)
{
	NowShellClearBothScreen();
	YDelete(hWndUp);
	YDelete(hWndDown);
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

HResource<YImage> ShlA::pic2, ShlA::pic2p;

HWND ShlA::hWndC(NULL);

static void
InputCounter(const SPoint& pt)
{
	sprintf(strCount, "%d,%d,%d;Count = %d, Pos = (%d, %d);", sizeof(AWindow), sizeof(YFrameWindow), sizeof(YForm), nCountInput++, pt.X, pt.Y);
}
static void
InputCounterAnother(const SPoint& pt)
{
	nCountInput++;
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

void
ShlA::TFormC::lblC_TouchUp(const YTouchEventArgs& e)
{
	InputCounter(e);
	HandleCast<ShlA>(hShell)->ShowString(strCount);
	lblC.Refresh();
}

void
ShlA::TFormC::lblC_TouchDown(const YTouchEventArgs& e)
{
	InputCounterAnother(e);
	HandleCast<ShlA>(hShell)->ShowString(strCount);
	lblC.Refresh();
}

void
ShlA::TFormC::lblC_Click(const YTouchEventArgs& e)
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
		++itype;
		++it;
		sprintf(strtf, "%d file(s), %d type(s), %d faces(s);\n", ffilen, ftypen, ffacen);
		lblC.Text = strtf;
	}
	else
	{
		itype %= ftypen;
		if(it == pDefaultFontCache->GetTypes().end())
			it = pDefaultFontCache->GetTypes().begin();
		sprintf(strtf, "%d/%d;%s:%s;\n", itype + 1, ftypen, (*it)->GetFamilyName(), (*it)->GetStyleName());
		//	sprintf(strtf, "B%p\n", pDefaultFontCache->GetTypefacePtr("FZYaoti", "Regular"));
		lblC.Text = strtf;
	}
	//	lblC.Refresh();
}

void
ShlA::TFormC::lblC_KeyPress(IVisualControl& sender, const YKeyEventArgs& e)
{
	//测试程序。

	u32 x(e);
/*
	if(x & Keys::A)
		InsertMessage(NULL, SM_DSR_INIT, 0xE0);
	else if(x & Keys::Up)
		ShlReader::pdsr->LineUp();
	else if(x & Keys::Down)
		ShlReader::pdsr->LineDown();
	else if(x & Keys::Left)
		ShlReader::pdsr->ScreenUp();
	else if(x & Keys::Right)
		ShlReader::pdsr->ScreenDown();
	else if(x & Keys::X)
	{
		ShlReader::pdsr->SetLineGap(5);
		ShlReader::pdsr->Update();
	}
	else if(x & Keys::Y)
	{
		ShlReader::pdsr->SetLineGap(8);
		ShlReader::pdsr->Update();
	}
	else if(x & Keys::L)
	{
		//ShlReader::pdsr->SetFontSize(ShlReader::pdsr->GetFontSize()+1);
		ShlReader::pdsr->SetLineGap(ShlReader::pdsr->GetLineGap() + 1);
		ShlReader::pdsr->Update();
	}
	else if(x & Keys::R)
	{
		//PixelType cc(ShlReader::pdsr->GetColor());
		//ShlReader::pdsr->SetColor(ARGB16(1,(cc&(15<<5))>>5,cc&29,(cc&(31<<10))>>10));
		ShlReader::pdsr->SetLineGap(ShlReader::pdsr->GetLineGap() - 1);
		ShlReader::pdsr->Update();
	}*/

	YLabel& lbl(static_cast<TFormA&>(*(static_cast<ShlA&>(*NowShell()).hWndUp)).lblA2);
	lbl.Transparent ^= 1;
	++lbl.ForeColor;
	--lbl.BackColor;
	sprintf(strttxt, "%d;\n", x);
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
ShlA::TFormC::btnReturn_Click(const YTouchEventArgs&)
{
	CallStored<ShlS>();
}

void
ShlA::Load()
{
	static bool unInit(true);

	if(unInit)
	{
		try
		{
			gbuf = new SCRBUF;
		}
		catch(...)
		{
			return;
		}
		ShlA::pic2 = NewScrImage(dfac2, gbuf);
		ShlA::pic2p = NewScrImage(dfac2p, gbuf);
		delete gbuf;
		unInit = false;
	}
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
	try
	{
		ShlA::Load();
		pDesktopDown->BackColor = ARGB16(1, 15, 15, 31);
		pDesktopDown->SetBackground(*ShlA::pic2p);
		hWndUp = NewWindow<TFormA>(this);
		hWndDown = NewWindow<TFormB>(this);
		hWndC = NewWindow<TFormC>(this);
		hWndUp->Draw();
		hWndDown->Draw();
		hWndC->Draw();
	/*	DrawWindows();*/
		UpdateToScreen();
	}
	catch(...)
	{}
	return 0;
}
LRES
ShlA::OnDeactivated(const MMSG& msg)
{
	NowShellClearBothScreen();
	YDelete(hWndUp);
	YDelete(hWndDown);
	YDelete(hWndC);
	return 0;
}


MDualScreenReader* ShlReader::pdsr(NULL);
YTextFile* ShlReader::ptf(NULL);

void
ShlReader::UpdateToScreen()
{
	ParentType::UpdateToScreen();
//	ShlReader::pdsr->Refresh();
}

LRES
ShlReader::ShlProc(const MMSG& msg)
{
	using namespace YSL_SHL(ShlA);
//	ClearDefaultMessageQueue();

	const WPARAM& wParam(msg.GetWParam());
/*
		InitYSConsole();
	//	YDebugBegin();
		iprintf("time : %u ticks\n", GetTicks());
		iprintf("MMSG : 0x%04X;\nPrior : 0x%02X;\nObj : %d\nW : %u;\nL : %lx;\n", msg.GetMsgID(), msg.GetPriority(), msg.GetID(), msg.GetWParam(), msg.GetLParam());
		waitForInput();
*/
//	UpdateToScreen();

	switch(msg.GetMsgID())
	{
	case SM_INPUT:
		ResponseInput(msg);
		UpdateToScreen();
		return 0;

	case SM_DSR_INIT:
		ShlReader::DSReaderInit("/test.txt"); //"/Data/test.txt";
		ShlReader::pdsr->Refresh();
		return 0;

	case SM_DSR_REFRESH:
		InsertMessage(NULL, SM_DSR_PRINTTEXT, 0xD8, reinterpret_cast<WPARAM>(ShlReader::pdsr));
		return 0;

	case SM_DSR_PRINTTEXT:
		UpdateToScreen();
		reinterpret_cast<MDualScreenReader*>(wParam)->PrintText();
		return 0;

	default:
		break;
	}
	return DefShellProc(msg);
}

LRES
ShlReader::OnActivated(const MMSG& msg)
{
	try
	{
	//	ShlReader::Load();
		pDesktopDown->BackColor = ARGB16(1, 15, 31, 31);
		hWndUp = NewWindow<TFormUp>(this);
		hWndDown = NewWindow<TFormDn>(this);
		InsertMessage(NULL, SM_DSR_INIT, 0xF8);
		hWndUp->Draw();
		hWndDown->Draw();
/*		DrawWindows();*/
		UpdateToScreen();
	}
	catch(...)
	{}
	return 0;
}

LRES
ShlReader::OnDeactivated(const MMSG& msg)
{
	NowShellClearBothScreen();
	YDelete(hWndUp);
	YDelete(hWndDown);
	return 0;
}

YSL_END

