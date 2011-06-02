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
\brief Shell 抽象。
\version 0.4484;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2011-06-02 13:04 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#include <Shells.h>

////////
//测试用声明：全局资源定义。
//extern char gstr[128];

using namespace ystdex;

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
		}
		return 0;

	default:
		return DefShellProc(msg);
	}
}


namespace
{
	Color
	GenerateRandomColor()
	{
	//使用 std::time(0) 初始化随机数种子在模拟器上无效。
	//	std::srand(std::time(0));
		return Color(std::rand(), std::rand(), std::rand(), 1);
	}

	//测试函数。

	//背景测试。
	void
	dfa(BitmapPtr buf, SDst x, SDst y)
	{
		//raz2GBR
		buf[y * Global::MainScreenWidth + x] = ARGB16(1, ((y >> 2) + 15) & 31,
			((~(x * y) >> 2) + 15) & 31, ((x >> 2) + 15) & 31);
	}
	void
	dfap(BitmapPtr buf, SDst x, SDst y)
	{
		//bza1BRG
		buf[y * Global::MainScreenWidth + x] = ARGB16(1,
			((x << 4) / (y | 1)) & 31,
			((x | y << 1) % (y + 2)) & 31, ((~y | x << 1) % 27 + 3) & 31);
	}
	void
	dfac1(BitmapPtr buf, SDst x, SDst y)
	{
		//fl1RBG
		buf[y * Global::MainScreenWidth + x] = ARGB16(1, (x + y * y) & 31,
			((x & y) ^ (x | y)) & 31, (x * x + y) & 31);
	}
	void
	dfac1p(BitmapPtr buf, SDst x, SDst y)
	{
		//rz3GRB
		buf[y * Global::MainScreenWidth + x] = ARGB16(1, ((x * y) | x) & 31,
			((x * y) | y) & 31, ((x ^ y) * (x ^ y)) & 31);
	}
	void
	dfac2(BitmapPtr buf, SDst x, SDst y)
	{
		//v1BGR
		buf[y * Global::MainScreenWidth + x] = ARGB16(1,
			((x << 4) / (y & 1)) & 31,
			(~x % 101 + y) & 31,((x + y) % ((y - 2) & 1) + (x << 2)) & 31);
	}
	void
	dfac2p(BitmapPtr buf, SDst x, SDst y)
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

	shared_ptr<Image>&
	GetGlobalImage(size_t i)
	{
		static shared_ptr<Image> spi[10];

		YAssert(IsInInterval(i, 10u), "Array index out of range"
			" @ GetGlobalImage;");

		return spi[i];
	}

	void
	LoadL()
	{
		//色块覆盖测试用程序段。
		if(!GetGlobalImage(1))
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
			GetGlobalImage(1) = CreateSharedScreenImage(dfa, gbuf);
		//	memset(gbuf, 0xF2, sizeof(ScreenBufferType));
			GetGlobalImage(2) = CreateSharedScreenImage(dfap, gbuf);
			ydelete_array(gbuf);
		}
	}

	void
	LoadS()
	{
		if(!GetGlobalImage(3))
		{
			try
			{
				gbuf = ynew ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
			GetGlobalImage(3) = CreateSharedScreenImage(dfac1, gbuf);
			GetGlobalImage(4) = CreateSharedScreenImage(dfac1p, gbuf);
			ydelete_array(gbuf);
		}
	}

	void
	LoadA()
	{
		if(!GetGlobalImage(5))
		{
			try
			{
				gbuf = ynew ScreenBufferType;
			}
			catch(std::bad_alloc&)
			{
				return;
			}
			GetGlobalImage(5) = CreateSharedScreenImage(dfac2, gbuf);
			GetGlobalImage(6) = CreateSharedScreenImage(dfac2p, gbuf);
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
		siprintf(strCount, "%d,%d,%d;Count = %d, Pos = (%d, %d);",
			sizeof(AWindow), sizeof(Frame), sizeof(Form),
			nCountInput++, pt.X, pt.Y);
	}

	void
	InputCounterAnother(const Point&)
	{
	//	nCountInput++;
	//	siprintf(strCount, "%d,%d,%d,%d,",sizeof(Form),sizeof(YShell),
	//		sizeof(YApplication),sizeof(YWindow));
		struct mallinfo t(mallinfo());

	/*	siprintf(strCount, "%d,%d,%d,%d,%d;",
			t.arena,    // total space allocated from system 2742496
			t.ordblks,  // number of non-inuse chunks 37
			t.smblks,   // unused -- always zero 0
			t.hblks,    // number of mmapped regions 0
			t.hblkhd   // total space in mmapped regions 0
		);*/
	/*	siprintf(strCount, "%d,%d,%d,%d,%d;",
			t.usmblks,  // unused -- always zero 0
			t.fsmblks,  // unused -- always zero 0
			t.uordblks, // total allocated space 2413256, 1223768
			t.fordblks, // total non-inuse space 329240, 57760
			t.keepcost // top-most, releasable (via malloc_trim) space
			//46496,23464
			);*/
		siprintf(strCount, "%d,%d,%d,%d,%d;",
			t.arena,
			t.ordblks,
			t.uordblks,
			t.fordblks,
			t.keepcost);
	}
}

shared_ptr<Image>&
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
	return GetGlobalImage(i);
}

void
ReleaseShells()
{
	for(size_t i(0); i != 10; ++i)
		GetGlobalImage(i).reset();
	ReleaseStored<ShlReader>();
	ReleaseStored<ShlSetting>();
	ReleaseStored<ShlExplorer>();
	ReleaseStored<ShlLoad>();
	ReleaseGlobalResource<TextRegion>();
}

using namespace DS;

ShlLoad::ShlLoad()
	: ShlDS(),
	lblTitle(Rect(50, 20, 100, 22)),
	lblStatus(Rect(60, 80, 80, 22)),
	lblDetails(Rect(30, 20, 160, 22))
{}

int
ShlLoad::OnActivated(const Message& msg)
{
	YDebugSetStatus(true);
	//新建窗口。
//	hWndUp = NewWindow<TFrmLoadUp>();
//	hWndDown = NewWindow<TFrmLoadDown>();
	GetDesktopUp() += lblTitle;
	GetDesktopUp() += lblStatus;
	GetDesktopDown() += lblDetails;
	GetDesktopUp().GetBackgroundImagePtr() = GetImage(1);
	GetDesktopDown().GetBackgroundImagePtr() = GetImage(2);
	lblTitle.Text = YApplication::ProductName;
	lblStatus.Text = "Loading...";
	lblDetails.Text = _ustr("初始化中，请稍后……");
	lblDetails.SetTransparent(true);
//	lblTitle.Transparent = true;
//	DrawContents();
	ParentType::OnActivated(msg);
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

int
ShlLoad::OnDeactivated(const Message& msg)
{
	reset(GetDesktopUp().GetBackgroundImagePtr());
	reset(GetDesktopDown().GetBackgroundImagePtr());
	ParentType::OnDeactivated(msg);
	return 0;
}


ShlExplorer::ShlExplorer()
	: ShlDS(),
	lblTitle(Rect(16, 20, 220, 22)), lblPath(Rect(12, 80, 240, 22)),
	fbMain(Rect(6, 10, 224, 150)),
	btnTest(Rect(115, 165, 65, 22)), btnOK(Rect(185, 165, 65, 22)),
	chkTest(Rect(45, 165, 16, 16))
{
	FetchEvent<KeyPress>(fbMain) += OnKeyPress_fbMain;
	fbMain.GetViewChanged().Add(*this, &ShlExplorer::OnViewChanged_fbMain);
	fbMain.GetConfirmed() += OnConfirmed_fbMain;
	FetchEvent<Click>(btnTest).Add(*this, &ShlExplorer::OnClick_btnTest);
	FetchEvent<Click>(btnOK).Add(*this, &ShlExplorer::OnClick_btnOK);
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
ShlExplorer::OnActivated(const Message& msg)
{
	GetDesktopUp() += lblTitle;
	GetDesktopUp() += lblPath;
	GetDesktopDown() += fbMain;
	GetDesktopDown() += btnTest;
	GetDesktopDown() += btnOK;
	GetDesktopDown() += chkTest;
	GetDesktopUp().GetBackgroundImagePtr() = GetImage(3);
	GetDesktopDown().GetBackgroundImagePtr() = GetImage(4);
	lblTitle.Text = "文件列表：请选择一个文件。";
	lblPath.Text = "/";
//	lblTitle.Transparent = true;
//	lblPath.Transparent = true;
	btnTest.Text = _ustr("测试(X)");
	btnOK.Text = _ustr("确定(R)");
	FetchEvent<KeyUp>(GetDesktopDown()).Add(*this, &ShlExplorer::OnKeyUp_frm);
	FetchEvent<KeyDown>(GetDesktopDown()).Add(*this,
		&ShlExplorer::OnKeyDown_frm);
	FetchEvent<KeyPress>(GetDesktopDown()).Add(*this,
		&ShlExplorer::OnKeyPress_frm);
	btnOK.SetTransparent(false);
/*
	ReplaceHandle<IWindow*>(hWndUp,
		new TFrmFileListMonitor(shared_ptr<YShell>(this)));
	ReplaceHandle<IWindow*>(hWndDown,
		new TFrmFileListSelecter(shared_ptr<YShell>(this)));
*/
	//	dynamic_pointer_cast<TFrmFileListSelecter>(
	//		hWndDown)->fbMain.RequestFocus(GetZeroElement<EventArgs>());
	//	hWndDown->RequestFocus(GetZeroElement<EventArgs>());
	ParentType::OnActivated(msg);
	RequestFocusCascade(fbMain);
	UpdateToScreen();
	return 0;
}

int
ShlExplorer::OnDeactivated(const Message& msg)
{
	FetchEvent<KeyUp>(GetDesktopDown()).Remove(*this,
		&ShlExplorer::OnKeyUp_frm);
	FetchEvent<KeyDown>(GetDesktopDown()).Remove(*this,
		&ShlExplorer::OnKeyDown_frm);
	FetchEvent<KeyPress>(GetDesktopDown()).Remove(*this,
		&ShlExplorer::OnKeyPress_frm);
	reset(GetDesktopUp().GetBackgroundImagePtr());
	reset(GetDesktopDown().GetBackgroundImagePtr());
	ParentType::OnDeactivated(msg);
	return 0;
}

void
ShlExplorer::OnKeyUp_frm(KeyEventArgs&& e)
{
	TouchEventArgs et(TouchEventArgs::FullScreen);

	switch(e.GetKey())
	{
	case KeySpace::X:
		CallEvent<Leave>(btnTest, et);
		break;
	case KeySpace::R:
		CallEvent<Leave>(btnOK, et);
		break;
	default:
		return;
	}
	e.Handled = true;
}

void
ShlExplorer::OnKeyDown_frm(KeyEventArgs&& e)
{
	TouchEventArgs et(TouchEventArgs::FullScreen);

	switch(e.GetKey())
	{
	case KeySpace::X:
		CallEvent<Enter>(btnTest, et);
		break;
	case KeySpace::R:
		CallEvent<Enter>(btnOK, et);
		break;
	default:
		return;
	}
	e.Handled = true;
}

void
ShlExplorer::OnKeyPress_frm(KeyEventArgs&& e)
{
	TouchEventArgs et(TouchEventArgs::FullScreen);

	switch(e.GetKey())
	{
	case KeySpace::X:
		CallEvent<Click>(btnTest, et);
		break;
	case KeySpace::R:
		CallEvent<Click>(btnOK, et);
		break;
	default:
		return;
	}
	e.Handled = true;
}

void
ShlExplorer::OnClick_btnTest(TouchEventArgs&&)
{
	switchShl1();
/*	if(fbMain.IsSelected())
	{
		YConsole console(*hScreenUp);

		Activate(console, Color::Silver);

		iprintf("Current Working Directory:\n%s\n",
			IO::GetNowDirectory().c_str());
		iprintf("FileBox Path:\n%s\n", fbMain.GetPath().c_str());
	//	std::fprintf(stderr, "err");
		WaitForInput();
	}
	else
	{
		YConsole console(*hScreenDown);
		Activate(console, Color::Yellow, ColorSpace::Green);
		iprintf("FileBox Path:\n%s\n", fbMain.GetPath().c_str());
		puts("OK");
		WaitForInput();
	}*/
}

void
ShlExplorer::OnClick_btnOK(TouchEventArgs&&)
{
	if(fbMain.IsSelected())
	{
		const string& s(fbMain.GetPath().GetNativeString());
/*	YConsole console;
	Activate(console);
	iprintf("%s\n%s\n%s\n%d,%d\n",fbMain.GetDirectory().c_str(),
		StringToMBCS(fbMain.YListBox::GetList()[fbMain.GetSelected()]).c_str(),
		s.c_str(),IO::ValidateDirectory(s), fexists(s.c_str()));
	WaitForABXY();
	Deactivate(console);*/
		if(!IO::ValidateDirectory(s) && fexists(s.c_str()))
			switchShl2(s.c_str());
	}
}

void
ShlExplorer::OnViewChanged_fbMain(EventArgs&&)
{
	lblPath.Text = fbMain.GetPath();
	lblPath.Refresh();
}

void
ShlExplorer::OnKeyPress_fbMain(IControl&, KeyEventArgs&& e)
{
	Key x(e);

	if(x & KeySpace::L)
		switchShl1();
}

void
ShlExplorer::OnConfirmed_fbMain(IControl&, IndexEventArgs&&)
{
//	if(e.Index == 2)
//		switchShl1();
}


ShlSetting::ShlSetting()
	: pWndTest(), pWndExtra(),
	lblA(Rect(5, 20, 200, 22)),
	lblB(Rect(5, 80, 72, 22))
{
	lblA.Text = YApplication::ProductName;
	lblB.Text = "程序测试";
	lblB.SetTransparent(true);
}


namespace
{
	shared_ptr<TextList::ListType>
	GenerateList(const String& str)
	{
		auto p(new TextList::ListType());

		p->push_back(str);

		char cstr[80];

		sprintf(cstr, "%p;", p);
		p->push_back(cstr);
		return share_raw(p);
	}

	MenuHost* s_pMenuHost;
}


ShlSetting::TFormTest::TFormTest()
	: Form(Rect(10, 40, 228, 70), shared_ptr<Image>(),
		raw(FetchGlobalInstance().GetDesktopDownHandle())),
	btnEnterTest(Rect(2, 5, 148, 22)), /*GetImage(6)*/
	btnMenuTest(Rect(152, 5, 60, 22)),
	btnShowWindow(Rect(45, 35, 124, 22))
{
	*this += btnEnterTest;
	*this += btnMenuTest;
	*this += btnShowWindow;
	btnEnterTest.Text = _ustr("边界测试");
	btnEnterTest.HorizontalAlignment = MLabel::Right;
	btnEnterTest.VerticalAlignment = MLabel::Up;
	btnMenuTest.Text = _ustr("菜单测试");
	btnShowWindow.Text = _ustr("显示/隐藏窗口");
	btnShowWindow.HorizontalAlignment = MLabel::Left;
	btnShowWindow.VerticalAlignment = MLabel::Down;
	BackColor = ARGB16(1, 31, 31, 15);
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
//	FetchEvent<TouchMove>(btnEnterTest) += &Control::OnTouchMove;
	FetchEvent<Enter>(btnEnterTest) += OnEnter_btnEnterTest;
	FetchEvent<Leave>(btnEnterTest) += OnLeave_btnEnterTest;
	FetchEvent<Click>(btnMenuTest).Add(*this, &TFormTest::OnClick_btnMenuTest);
	FetchEvent<Click>(btnShowWindow) += OnClick_ShowWindow;
//	FetchEvent<TouchMove>(btnShowWindow) += OnTouchMove_Dragging;
//	FetchEvent<TouchDown>(btnShowWindow) += OnClick_btnDragTest;
}

void
ShlSetting::TFormTest::OnEnter_btnEnterTest(IControl& sender,
	TouchEventArgs&& e)
{
	char str[20];

	siprintf(str, "Enter:(%d,%d)", e.Point::X, e.Point::Y);

	auto& btn(dynamic_cast<Button&>(sender));

	btn.Text = str;
	btn.Refresh();
}
void
ShlSetting::TFormTest::OnLeave_btnEnterTest(IControl& sender,
	TouchEventArgs&& e)
{
	char str[20];

	siprintf(str, "Leave:(%d,%d)", e.Point::X, e.Point::Y);

	auto& btn(dynamic_cast<Button&>(sender));

	btn.Text = str;
	btn.Refresh();
}

void
ShlSetting::TFormTest::OnClick_btnMenuTest(TouchEventArgs&&)
{
	static int t;
	
	YAssert(s_pMenuHost, "err: null menu host pointer found;");

	auto& mnu((*s_pMenuHost)[1u]);
	auto& lst(mnu.GetList());

	if(s_pMenuHost->IsShowing(1u))
	{
		if(lst.size() > 4)
			lst.clear();

		char stra[4];
		
		siprintf(stra, "%d", t);
		lst.push_back(String((string("TMI") + stra).c_str()));
	}
	else
	{
		mnu.SetLocation(Point(
			btnMenuTest.GetX() - btnMenuTest.GetWidth(),
			btnMenuTest.GetY() + btnMenuTest.GetHeight()));
		mnu.SetWidth(btnMenuTest.GetWidth() + 20);
		lst.push_back(_ustr("TestMenuItem1"));
		s_pMenuHost->Show(1u);
	}
	ResizeForContent(mnu);
	mnu.Refresh();
	++t;
}

ShlSetting::TFormExtra::TFormExtra()
	: Form(Rect(5, 60, 208, 120), shared_ptr<Image>(), /*GetImage(7)*/
		raw(FetchGlobalInstance().GetDesktopDownHandle())),
	btnDragTest(Rect(13, 15, 184, 22)),
	btnTestEx(Rect(13, 52, 168, 22)),
	btnReturn(Rect(13, 82, 60, 22)),
	btnExit(Rect(83, 82, 60, 22))
{
	*this += btnDragTest;
	*this += btnTestEx;
	*this += btnReturn;
	*this += btnExit;
	btnDragTest.Text = _ustr("测试拖放控件");
	btnDragTest.HorizontalAlignment = MLabel::Left;
	btnTestEx.Text = _ustr("直接屏幕绘制测试");
	btnReturn.Text = _ustr("返回");
	btnReturn.SetEnabled(false);
	btnExit.Text = _ustr("退出");
	BackColor = ARGB16(1, 31, 15, 15);
	FetchEvent<TouchDown>(*this) += OnTouchDown_FormExtra;
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
	FetchEvent<Move>(btnDragTest).Add(*this, &TFormExtra::OnMove_btnDragTest);
	FetchEvent<TouchUp>(btnDragTest).Add(*this,
		&TFormExtra::OnTouchUp_btnDragTest);
	FetchEvent<TouchDown>(btnDragTest).Add(*this,
		&TFormExtra::OnTouchDown_btnDragTest);
	FetchEvent<TouchMove>(btnDragTest) += OnTouchMove_Dragging;
	FetchEvent<Click>(btnDragTest).Add(*this, &TFormExtra::OnClick_btnDragTest);
	FetchEvent<Click>(btnTestEx).Add(*this, &TFormExtra::OnClick_btnTestEx);
	FetchEvent<KeyPress>(btnDragTest) += OnKeyPress_btnDragTest;
//	btnDragTest.Enabled = false;
	btnReturn.BackColor = ARGB16(1, 22, 23, 24);
	FetchEvent<Click>(btnReturn).Add(*this, &TFormExtra::OnClick_btnReturn);
	FetchEvent<Click>(btnExit).Add(*this, &TFormExtra::OnClick_btnExit);
}


void
ShlSetting::TFormExtra::OnMove_btnDragTest(EventArgs&&)
{
	static char sloc[20];

	siprintf(sloc, "(%d, %d);", btnDragTest.GetX(), btnDragTest.GetY());
	btnDragTest.Text = sloc;
	btnDragTest.Refresh();
}

void
ShlSetting::TFormExtra::OnTouchUp_btnDragTest(TouchEventArgs&& e)
{
	InputCounter(e);
	FetchShell<ShlSetting>().ShowString(strCount);
	btnDragTest.Refresh();
}
void
ShlSetting::TFormExtra::OnTouchDown_btnDragTest(TouchEventArgs&& e)
{
	InputCounterAnother(e);
	FetchShell<ShlSetting>().ShowString(strCount);
//	btnDragTest.Refresh();
}

void
ShlSetting::TFormExtra::OnClick_btnDragTest(TouchEventArgs&&)
{
	static FontCache& fc(FetchAppInstance().GetFontCache());
	static const int ffilen(fc.GetFilesN());
	static const int ftypen(fc.GetTypesN());
	static const int ffacen(fc.GetFacesN());
	static int itype;
	static FontCache::FaceSet::const_iterator it(fc.GetTypes().begin());
	static char strtf[0x400];

	//	btnDragTest.Transparent ^= 1;
	if(nCountInput & 1)
	{
		//	btnDragTest.Visible ^= 1;
		++itype %= ftypen;
		if(++it == fc.GetTypes().end())
			it = fc.GetTypes().begin();
		btnDragTest.Font = Font(*(*it)->GetFontFamilyPtr(), 16 - (itype << 1),
			FontStyle::Regular);
	//	btnDragTest.Font = Font(*(*it)->GetFontFamilyPtr(),
	//	GetDefaultFontFamily(),
	//		16 - (itype << 1), FontStyle::Regular);
		siprintf(strtf, "%d, %d file(s), %d type(s), %d faces(s);\n",
			btnDragTest.Font.GetSize(), ffilen, ftypen, ffacen);
		btnDragTest.Text = strtf;
		btnDragTest.ForeColor = GenerateRandomColor();
		btnReturn.ForeColor = GenerateRandomColor();
		btnReturn.SetEnabled(true);
	}
	else
	{
		siprintf(strtf, "%d/%d;%s:%s;", itype + 1, ftypen,
			(*it)->GetFamilyName().c_str(), (*it)->GetStyleName().c_str());
		//	sprintf(strtf, "B%p\n",
		//		fc.GetTypefacePtr("FZYaoti", "Regular"));
		btnDragTest.Text = strtf;
	}
	//	btnDragTest.Refresh();
}

void
ShlSetting::TFormExtra::OnKeyPress_btnDragTest(IControl& sender,
	KeyEventArgs&& e)
{
	u32 k(static_cast<KeyEventArgs::Key>(e));
	char strt[100];
	auto& lbl(dynamic_cast<Label&>(sender));

//	Button& lbl(dynamic_cast<TFormUp&>(
//		*(dynamic_cast<ShlSetting&>(*FetchShellHandle()).hWndUp)).lblB);
	lbl.SetTransparent(!lbl.IsTransparent());
//	++lbl.ForeColor;
//	--lbl.BackColor;
	siprintf(strt, "%d;\n", k);
	lbl.Text = strt;
	lbl.Refresh();
/*
	Button& lbl(static_cast<Button&>(sender));

	if(nCountInput & 1)
		lbl.Text = _ustr("测试键盘...");
*/
}

/*void
ShlSetting::TFormExtra::OnClick_btnTestEx(TouchEventArgs&)
{

}*/

void
ShlSetting::TFormExtra::OnClick_btnReturn(TouchEventArgs&&)
{
	CallStored<ShlExplorer>();
}

void
ShlSetting::TFormExtra::OnClick_btnExit(TouchEventArgs&&)
{
	PostQuitMessage(0);
}

void
ShlSetting::TFormExtra::OnClick_btnTestEx(TouchEventArgs&& e)
{
	using namespace Drawing;

	class TestObj
	{
	public:
		shared_ptr<Desktop> h;
		Color c;
		Point l;
		Size s;

		TestObj(const shared_ptr<Desktop>& h_)
			: h(h_),
			c(ColorSpace::White),
			l(20, 32), s(120, 90)
		{}

		void
		Fill()
		{
			Graphics g(h->GetScreen().GetBufferPtr(), h->GetSize());

			FillRect(g, l, s, c);
		}

		void
		Pause()
		{
			WaitForInput();
		}

		void
		Blit(const TextRegion& tr)
		{
			BlitTo(h->GetScreen().GetBufferPtr(), tr,
				h->GetScreen().GetSize(), l, Point::Zero, tr.GetSize());
		}

		void
		Test1(TextRegion& tr, Color c)
		{
			Fill();				
			tr.ClearImage();
			tr.ResetPen();
			tr.Color = c;
		}
	};

//	static int test_state = 0;
	{
	/*
		uchar_t* tstr(Text::ucsdup("Abc测试", Text::CS_Local));
		String str(tstr);

		std::free(tstr);
	*/
		String str(_ustr("Abc测试"));

		TestObj t(FetchGlobalInstance().GetDesktopDownHandle());
	//	const Graphics& g(t.h->GetContext());

		TextRegion tr;
		tr.SetSize(t.s.Width, t.s.Height);
		TextRegion& tr2(*GetGlobalResource<TextRegion>());

		switch(e.X * 4 / btnTestEx.GetWidth())
		{
		case 0:
			t.Fill();
			t.Pause();

		//	tr.BeFilledWith(ColorSpace::Black);
			PutLine(tr, str);
			t.Blit(tr);
			t.Pause();

			t.Test1(tr, ColorSpace::Black);
			PutLine(tr, str);
			t.Blit(tr);
			t.Pause();

			t.Test1(tr, ColorSpace::Blue);
			PutLine(tr, str);
			t.Pause();

			t.Blit(tr);
			t.Pause();

		case 1:
			tr2.SetSize(t.s.Width, t.s.Height);

		//	t.Pause();
		//	tr2.BeFilledWith(ColorSpace::Black);
			t.Test1(tr2, ColorSpace::White);
			PutLine(tr2, str);
			t.Blit(tr2);
			t.Pause();

			t.Test1(tr2, ColorSpace::Black);
			PutLine(tr2, str);
			t.Blit(tr2);
			t.Pause();

			t.Test1(tr2, ColorSpace::Red);
			t.Blit(tr2);
			PutLine(tr2, str);
			t.Pause();

			t.Blit(tr2);
			t.Pause();
			break;

		case 2:
			t.c = ColorSpace::Lime;

			t.Fill();
			t.Pause();

		//	tr.BeFilledWith(ColorSpace::Black);
			PutLine(tr, str);
			t.Blit(tr);
			t.Pause();

			t.Test1(tr, ColorSpace::Black);
			PutLine(tr, str);
			t.Blit(tr);
			t.Pause();

			t.Test1(tr, ColorSpace::Blue);
			PutLine(tr, str);
			t.Pause();

			t.Blit(tr);
			t.Pause();

		case 3:
			t.c = ColorSpace::Lime;

			tr2.SetSize(t.s.Width, t.s.Height);

		//	t.Pause();
		//	tr2.BeFilledWith(ColorSpace::Black);
			t.Test1(tr2, ColorSpace::White);
			PutLine(tr2, str);
			t.Blit(tr2);
			t.Pause();

			t.Test1(tr2, ColorSpace::Black);
			PutLine(tr2, str);
			t.Blit(tr2);
			t.Pause();

			t.Test1(tr2, ColorSpace::Red);
			t.Blit(tr2);
			PutLine(tr2, str);
			t.Pause();

			t.Blit(tr2);
			t.Pause();

		default:
			break;
		}
	}
}

int
ShlSetting::ShlProc(const Message& msg)
{
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

int
ShlSetting::OnActivated(const Message& msg)
{
	GetDesktopUp() += lblA;
	GetDesktopUp() += lblB;
	GetDesktopUp().GetBackgroundImagePtr() = GetImage(5);
	GetDesktopDown().BackColor = ARGB16(1, 15, 15, 31);
	GetDesktopDown().GetBackgroundImagePtr() = GetImage(6);
	pWndTest = unique_raw(new TFormTest());
	pWndExtra = unique_raw(new TFormExtra());
	GetDesktopDown() += *pWndTest;
	GetDesktopDown() += *pWndExtra;
//	pWndTest->DrawContents();
//	pWndExtra->DrawContents();

	s_pMenuHost = new MenuHost(*GetDesktopDownHandle());

/*	Menu& mnu(*new Menu(Rect::Empty, GenerateList(_ustr("TestMenuItem0")), 1u));

	FetchEvent<Click>(mnu) += OnClick_ShowWindow;
	*s_pMenuHost += mnu;*/
	*s_pMenuHost
		+= *new Menu(Rect::Empty, GenerateList(_ustr("ATestMenuItem")), 1u);
	*s_pMenuHost
		+= *new Menu(Rect::Empty, GenerateList(_ustr("BTestMenuItem")), 2u);
	(*s_pMenuHost)[1u] += make_pair(1u, &(*s_pMenuHost)[2u]);
	ResizeForContent((*s_pMenuHost)[2u]);
	ParentType::OnActivated(msg);
	UpdateToScreen();
	return 0;
}

int
ShlSetting::OnDeactivated(const Message& msg)
{
	s_pMenuHost->Clear();
	delete s_pMenuHost;
	reset(GetDesktopUp().GetBackgroundImagePtr());
	reset(GetDesktopDown().GetBackgroundImagePtr());
	ParentType::OnDeactivated(msg);
	reset(pWndTest);
	reset(pWndExtra);
	return 0;
}

void
ShlSetting::ShowString(const String& s)
{
	lblA.Text = s;
	lblA.Refresh();
}
void
ShlSetting::ShowString(const char* s)
{
	ShowString(String(s));
}

void
ShlSetting::OnClick_ShowWindow(IControl&, TouchEventArgs&&)
{
	auto& pWnd(FetchShell<ShlSetting>().pWndExtra);

	if(pWnd)
	{
		if(pWnd->IsVisible())
			Hide(*pWnd);
		else
			Show(*pWnd);
	}
}

void
ShlSetting::OnTouchDown_FormExtra(IControl& sender, TouchEventArgs&&)
{
	try
	{
		TFormExtra& frm(dynamic_cast<TFormExtra&>(sender));

		frm.BackColor = GenerateRandomColor();
		frm.Refresh();
	}
	catch(std::bad_cast&)
	{}
}


string ShlReader::path;

ShlReader::ShlReader()
	: ShlDS(),
	Reader(), pTextFile(), hUp(), hDn(), bgDirty(false)
{}

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

int
ShlReader::OnActivated(const Message& msg)
{
	pTextFile = ynew TextFile(path.c_str());
	Reader.LoadText(*pTextFile);
	bgDirty = true;
	std::swap(hUp, GetDesktopUp().GetBackgroundImagePtr());
	std::swap(hDn, GetDesktopDown().GetBackgroundImagePtr());
	GetDesktopUp().BackColor = ARGB16(1, 30, 27, 24);
	GetDesktopDown().BackColor = ARGB16(1, 24, 27, 30);
	FetchEvent<Click>(GetDesktopDown()).Add(*this, &ShlReader::OnClick);
	FetchEvent<KeyDown>(GetDesktopDown()).Add(*this, &ShlReader::OnKeyDown);
	FetchEvent<KeyHeld>(GetDesktopDown()) += OnKeyHeld;
	ParentType::OnActivated(msg);
	RequestFocusCascade(GetDesktopDown());
	UpdateToScreen();
	return 0;
}

int
ShlReader::OnDeactivated(const Message&)
{
	GetDesktopUp().ClearContents();
	GetDesktopDown().ClearContents();
	FetchEvent<Click>(GetDesktopDown()).Remove(*this, &ShlReader::OnClick);
	FetchEvent<KeyDown>(GetDesktopDown()).Remove(*this, &ShlReader::OnKeyDown);
	FetchEvent<KeyHeld>(GetDesktopDown()) -= OnKeyHeld;
	std::swap(hUp, GetDesktopUp().GetBackgroundImagePtr());
	std::swap(hDn, GetDesktopDown().GetBackgroundImagePtr());
	Reader.UnloadText();
	safe_delete_obj()(pTextFile);
	return 0;
}

void
ShlReader::UpdateToScreen()
{
	if(bgDirty)
	{
		bgDirty = false;
		//强制刷新背景。
		GetDesktopUp().SetRefresh(true);
		GetDesktopDown().SetRefresh(true);
		GetDesktopUp().Refresh();
		GetDesktopDown().Refresh();
		Reader.PrintText(GetDesktopUp().GetContext(),
			GetDesktopDown().GetContext());
		GetDesktopUp().Update();
		GetDesktopDown().Update();
	}
}

void
ShlReader::OnClick(TouchEventArgs&&)
{
	CallStored<ShlExplorer>();
}

void
ShlReader::OnKeyDown(KeyEventArgs&& e)
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

