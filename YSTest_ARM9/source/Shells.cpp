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
\brief Shell 框架逻辑。
\version 0.4872;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2011-08-06 15:00 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#include <Shells.h>
#include <YSLib/UI/progress.h>

////////
//测试用声明：全局资源定义。
//extern char gstr[128];

using namespace ystdex;

YSL_BEGIN

using namespace Shells;
using namespace Drawing::ColorSpace;


namespace
{
	Color
	GenerateRandomColor()
	{
	//使用 std::time(0) 初始化随机数种子在 DeSmuMe 上无效。
	//	std::srand(std::time(0));
		return Color(std::rand(), std::rand(), std::rand(), 1);
	}

	//测试函数。

	//背景测试。
	void
	dfa(BitmapPtr buf, SDst x, SDst y)
	{
		//raz2GBR
		buf[y * MainScreenWidth + x] = ARGB16(1, ((y >> 2) + 15) & 31,
			((~(x * y) >> 2) + 15) & 31, ((x >> 2) + 15) & 31);
	}
	void
	dfap(BitmapPtr buf, SDst x, SDst y)
	{
		//bza1BRG
		buf[y * MainScreenWidth + x] = ARGB16(1,
			((x << 4) / (y | 1)) & 31,
			((x | y << 1) % (y + 2)) & 31, ((~y | x << 1) % 27 + 3) & 31);
	}
	void
	dfac1(BitmapPtr buf, SDst x, SDst y)
	{
		//fl1RBG
		buf[y * MainScreenWidth + x] = ARGB16(1, (x + y * y) & 31,
			((x & y) ^ (x | y)) & 31, (x * x + y) & 31);
	}
	void
	dfac1p(BitmapPtr buf, SDst x, SDst y)
	{
		//rz3GRB
		buf[y * MainScreenWidth + x] = ARGB16(1, ((x * y) | x) & 31,
			((x * y) | y) & 31, ((x ^ y) * (x ^ y)) & 31);
	}
	void
	dfac2(BitmapPtr buf, SDst x, SDst y)
	{
		//v1BGR
		buf[y * MainScreenWidth + x] = ARGB16(1, ((x << 4) / (y & 1)) & 31,
			(~x % 101 + y) & 31,((x + y) % ((y - 2) & 1) + (x << 2)) & 31);
	}
	void
	dfac2p(BitmapPtr buf, SDst x, SDst y)
	{
		//arz1
		buf[y * MainScreenWidth + x] = ARGB16(1, ((x | y) % (y + 2)) & 31,
			((~y | x) % 27 + 3) & 31, ((x << 6) / (y | 1)) & 31);
	}

	////

	//测试用变量。
	int nCountInput;
	char strCount[40];

	shared_ptr<Image>&
	FetchGlobalImage(size_t i)
	{
		static shared_ptr<Image> spi[10];

		YAssert(IsInInterval(i, 10u), "Array index out of range"
			" @ FetchGlobalImage;");

		return spi[i];
	}

	BitmapPtr
	FetchGlobalImageBuffer(size_t i)
	{
		auto& h(FetchGlobalImage(i));

		if(!h)
			h = share_raw(new Image(nullptr, MainScreenWidth,
				MainScreenHeight));
		return h->GetBufferPtr();
	}

	void
	switchShl1()
	{
		CallStored<ShlExplorer>();
	}

	void
	switchShl2(const_path_t pth)
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
FetchImage(size_t i)
{
	switch(i)
	{
	case 1:
	case 2:
		//色块覆盖测试用程序段。
		if(!FetchGlobalImage(1))
		{
			/*
			gbuf = ynew ScreenBufferType;
			memset(gbuf, 0xEC, sizeof(ScreenBufferType)); //0xF2
			ydelete_array(gbuf);
			*/
			ScrDraw(FetchGlobalImageBuffer(1), dfa);
			ScrDraw(FetchGlobalImageBuffer(2), dfap);
		}
		break;
	case 3:
	case 4:
		if(!FetchGlobalImage(3))
		{
			ScrDraw(FetchGlobalImageBuffer(3), dfac1);
			ScrDraw(FetchGlobalImageBuffer(4), dfac1p);
		}
		break;
	case 5:
	case 6:
	case 7:
		if(!FetchGlobalImage(5))
		{
			ScrDraw(FetchGlobalImageBuffer(5), dfac2);
			ScrDraw(FetchGlobalImageBuffer(6), dfac2p);
		}
		break;
	case 8:
	case 9:
	//	LoadR();
		break;
	default:
		i = 0;
	}
	return FetchGlobalImage(i);
}

void
ReleaseShells()
{
	for(size_t i(0); i != 10; ++i)
		FetchGlobalImage(i).reset();
	ReleaseStored<ShlReader>();
	ReleaseStored<ShlExplorer>();
}

using namespace DS;

YSL_BEGIN_NAMESPACE(Shells)

YMainShell::YMainShell()
	: ShlDS(),
	lblTitle(Rect(50, 20, 100, 22)),
	lblStatus(Rect(60, 80, 80, 22)),
	lblDetails(Rect(30, 20, 160, 22))
{}

int
YMainShell::OnActivated(const Message& msg)
{
	ParentType::OnActivated(msg);

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	YDebugSetStatus(true);

	dsk_up += lblTitle;
	dsk_up += lblStatus;
	dsk_dn += lblDetails;
//	dsk_up.GetBackgroundImagePtr() = FetchImage(1);
	dsk_up.BackColor = ARGB16(1, 30, 27, 24);
//	dsk_dn.BackColor = ARGB16(1, 24, 27, 30);
	dsk_dn.BackColor = FetchGUIShell().Colors[Styles::Desktop];
	lblTitle.Text = YApplication::ProductName;
	lblStatus.Text = "Loading...";
	lblDetails.Text = _ustr("初始化中，请稍后……");
	lblDetails.ForeColor = ColorSpace::White;
	lblDetails.SetTransparent(true);
	dsk_up.SetInvalidation();
	UpdateToScreen();
	//初始化所有图像资源。

	auto& pb(*new ProgressBar(Rect(8, 168, 240, 16), 10));

	dsk_up += pb;
	for(size_t i(0); i < 10; ++i)
	{
		pb.SetValue(i);
//		Invalidate(pb);
		dsk_up.BackColor = Color(255 - i * 255 / 10, 216, 192);
		dsk_up.SetInvalidation();
		Invalidate(dsk_up);
		Validate(dsk_up);
		dsk_up.Update();
//		dsk_up.Refresh(FetchContext(dsk_up), Point::Zero,
//			Rect(Point::Zero, dsk_up.GetSize()));
		FetchImage(i);
	}
	pb.SetValue(10);
	Invalidate(dsk_up);
	Validate(dsk_up);
	dsk_up.Update();
	dsk_up -= pb;
	delete &pb;
	SetShellToStored<ShlExplorer>();
	return 0;
}

int
YMainShell::OnDeactivated(const Message& msg)
{
	reset(GetDesktopUp().GetBackgroundImagePtr());
	reset(GetDesktopDown().GetBackgroundImagePtr());
	ParentType::OnDeactivated(msg);
	return 0;
}

YSL_END_NAMESPACE(Shells)


ShlExplorer::ShlExplorer()
	: ShlDS(),
	lblTitle(Rect(16, 20, 220, 22)), lblPath(Rect(12, 80, 240, 22)),
	fbMain(Rect(4, 6, 248, 128)),
	btnTest(Rect(115, 165, 65, 22)), btnOK(Rect(185, 165, 65, 22)),
	chkTest(Rect(232, 144, 16, 16)),
	pWndTest(), pWndExtra(),
	lblA(Rect(5, 20, 200, 22)),
	lblB(Rect(5, 120, 72, 22)),
	mhMain(*GetDesktopDownHandle())
{
	//对 fbMain 启用缓存。
	fbMain.SetRenderer(unique_raw(new BufferedWidgetRenderer()));
	FetchEvent<KeyPress>(fbMain) += [](IControl&, KeyEventArgs&& e){
		if(e.GetKeyCode() & KeySpace::L)
			switchShl1();
	};
	fbMain.GetViewChanged().Add(*this, &ShlExplorer::OnViewChanged_fbMain);
	fbMain.GetSelected() += [this](IControl&, IndexEventArgs&&){
		SetEnabledOf(btnOK, fbMain.IsSelected() && IO::GetExtensionFrom(
			Text::StringToMBCS(fbMain.GetList()[fbMain.GetSelectedIndex()]))
			== "txt");
	};
	fbMain.GetConfirmed() += OnConfirmed_fbMain;
	SetEnabledOf(btnTest, false);
	SetEnabledOf(btnOK, false);
	FetchEvent<Click>(chkTest) += [this](IControl&, TouchEventArgs&&){
		SetEnabledOf(btnTest, chkTest.IsTicked());
	};
	FetchEvent<Click>(btnTest).Add(*this, &ShlExplorer::OnClick_btnTest);
	FetchEvent<Click>(btnOK).Add(*this, &ShlExplorer::OnClick_btnOK);
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

	void
	SwitchVisible(IWidget& wgt)
	{
		if(wgt.IsVisible())
			Hide(wgt);
		else
			Show(wgt);
	}


	class TestObj
	{
	public:
		shared_ptr<Desktop> h;
		Color c;
		Point l;
		Size s;
		String str;
		TextRegion tr;

	public:
		TestObj(const shared_ptr<Desktop>&);

		void
		Fill();

		static void
		Pause()
		{
			WaitForInput();
		}

		void
		Blit() const;

		void
		Test1(Color);

		void
		Test2();

		void
		Test3(Color);
	};

	TestObj::TestObj(const shared_ptr<Desktop>& h_)
		: h(h_),
		c(ColorSpace::White),
		l(20, 32), s(120, 90)
	{
		tr.SetSize(s.Width, s.Height);
	}

	void
	TestObj::Fill()
	{
		Graphics g(h->GetScreen());

		FillRect(g, l, s, c);
	}

	void
	TestObj::Blit() const
	{
		BlitTo(h->GetScreen().GetBufferPtr(), tr,
			h->GetScreen().GetSize(), l, Point::Zero, tr.GetSize());
	}

	void
	TestObj::Test1(Color c)
	{
		Fill();
		tr.ClearImage();
		tr.ResetPen();
		tr.Color = c;
	}

	void
	TestObj::Test2()
	{
		PutLine(tr, str);
		Blit();
		Pause();
	}

	void
	TestObj::Test3(Color c)
	{
		Test1(c);
		Test2();
	}

	class FPSCounter
	{
	private:
		Timers::TimeSpan last_tick;
		Timers::TimeSpan now_tick;

	public:
		FPSCounter();

		DefGetter(Timers::TimeSpan, LastTick, last_tick)
		DefGetter(Timers::TimeSpan, NowTick, now_tick)

		/*!
		\brief 刷新计数器。
		\return 每秒毫帧数。
		*/
		u32
		Refresh();
	};

	inline
	FPSCounter::FPSCounter()
		: last_tick(0), now_tick(0)
	{}

	u32
	FPSCounter::Refresh()
	{
		if(last_tick != GetRTC())
		{
			last_tick = now_tick;
			now_tick = GetRTC();
		}
		return now_tick == last_tick ? 0 : 1000000 / (now_tick - last_tick);
	}
}


ShlExplorer::TFormTest::TFormTest()
	: Form(Rect(10, 40, 228, 100), shared_ptr<Image>(),
		raw(FetchGlobalInstance().GetDesktopDownHandle())),
	btnEnterTest(Rect(2, 5, 148, 22)), /*FetchImage(6)*/
	btnMenuTest(Rect(152, 5, 60, 22)),
	btnShowWindow(Rect(45, 35, 124, 22)),
	btnPrevBackground(Rect(45, 65, 30, 22)),
	btnNextBackground(Rect(95, 65, 30, 22))
{
	*this += btnEnterTest;
	*this += btnMenuTest;
	*this += btnShowWindow;
	*this += btnPrevBackground;
	*this += btnNextBackground;
	btnEnterTest.Text = _ustr("边界测试");
	btnEnterTest.HorizontalAlignment = MLabel::Right;
	btnEnterTest.VerticalAlignment = MLabel::Up;
	btnMenuTest.Text = _ustr("菜单测试");
	btnShowWindow.Text = _ustr("显示/隐藏窗口");
	btnShowWindow.HorizontalAlignment = MLabel::Left;
	btnShowWindow.VerticalAlignment = MLabel::Down;
	btnPrevBackground.Text = "<<";
	btnNextBackground.Text = ">>";
	BackColor = ARGB16(1, 31, 31, 15);
	SetInvalidation();
	FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging;
	FetchEvent<Enter>(btnEnterTest) += OnEnter_btnEnterTest;
	FetchEvent<Leave>(btnEnterTest) += OnLeave_btnEnterTest;

	static int up_i(1);

	FetchEvent<Click>(btnMenuTest).Add(*this, &TFormTest::OnClick_btnMenuTest);
	FetchEvent<Click>(btnShowWindow) += OnClick_ShowWindow;
	SetEnabledOf(btnPrevBackground, false);
	FetchEvent<Click>(btnPrevBackground) += [this](IControl&, TouchEventArgs&&){
		auto& shl(FetchShell<ShlExplorer>());
		auto& dsk_up_ptr(shl.GetDesktopUp().GetBackgroundImagePtr());
		auto& dsk_dn_ptr(shl.GetDesktopDown().GetBackgroundImagePtr());

		if(up_i > 1)
		{
			--up_i;
			SetEnabledOf(btnNextBackground);
		}
		if(up_i == 1)
			SetEnabledOf(btnPrevBackground, false);
		dsk_up_ptr = FetchImage(up_i);
		dsk_dn_ptr = FetchImage(up_i + 1);
		shl.GetDesktopUp().SetInvalidation();
		shl.GetDesktopDown().SetInvalidation();
	};
	FetchEvent<Click>(btnNextBackground) += [this](IControl&, TouchEventArgs&&){
		auto& shl(FetchShell<ShlExplorer>());
		auto& dsk_up_ptr(shl.GetDesktopUp().GetBackgroundImagePtr());
		auto& dsk_dn_ptr(shl.GetDesktopDown().GetBackgroundImagePtr());

		if(up_i < 5)
		{
			++up_i;
			SetEnabledOf(btnPrevBackground);
		}
		if(up_i == 5)
			SetEnabledOf(btnNextBackground, false);
		dsk_up_ptr = FetchImage(up_i);
		dsk_dn_ptr = FetchImage(up_i + 1);
		shl.GetDesktopUp().SetInvalidation();
		shl.GetDesktopDown().SetInvalidation();
	};
}

void
ShlExplorer::TFormTest::OnEnter_btnEnterTest(IControl& sender,
	TouchEventArgs&& e)
{
	char str[20];

	siprintf(str, "Enter:(%d,%d)", e.Point::X, e.Point::Y);

	auto& btn(dynamic_cast<Button&>(sender));

	btn.Text = str;
	Invalidate(btn);
}
void
ShlExplorer::TFormTest::OnLeave_btnEnterTest(IControl& sender,
	TouchEventArgs&& e)
{
	char str[20];

	siprintf(str, "Leave:(%d,%d)", e.Point::X, e.Point::Y);

	auto& btn(dynamic_cast<Button&>(sender));

	btn.Text = str;
	Invalidate(btn);
}

void
ShlExplorer::TFormTest::OnClick_btnMenuTest(TouchEventArgs&&)
{
	static int t;

	auto& mhMain(FetchShell<ShlExplorer>().mhMain);
	auto& mnu(mhMain[1u]);
	auto& lst(mnu.GetList());

	if(mhMain.IsShowing(1u))
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
		mhMain.Show(1u);
	}
	ResizeForContent(mnu);
	Invalidate(mnu);
	++t;
}

ShlExplorer::TFormExtra::TFormExtra()
	: Form(Rect(5, 60, 208, 120), shared_ptr<Image>(), /*FetchImage(7)*/
		raw(FetchGlobalInstance().GetDesktopDownHandle())),
	btnDragTest(Rect(13, 15, 184, 22)),
	btnTestEx(Rect(13, 52, 168, 22)),
	btnClose(Rect(13, 82, 60, 22)),
	btnExit(Rect(83, 82, 60, 22))
{
	*this += btnDragTest;
	*this += btnTestEx;
	*this += btnClose;
	*this += btnExit;
	btnDragTest.Text = _ustr("测试拖放控件");
	btnDragTest.HorizontalAlignment = MLabel::Left;
	btnTestEx.Text = _ustr("直接屏幕绘制测试");
	btnClose.Text = _ustr("关闭");
	btnExit.Text = _ustr("退出");
	BackColor = ARGB16(1, 31, 15, 15);
	SetInvalidation();
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
	btnClose.BackColor = ARGB16(1, 22, 23, 24);
	FetchEvent<Click>(btnClose).Add(*this, &TFormExtra::OnClick_btnClose);
	FetchEvent<Click>(btnExit) += [](IControl&, TouchEventArgs&&){
		PostQuitMessage(0);
	};
}


void
ShlExplorer::TFormExtra::OnMove_btnDragTest(EventArgs&&)
{
	static char sloc[20];

	siprintf(sloc, "(%d, %d);", btnDragTest.GetX(), btnDragTest.GetY());
	btnDragTest.Text = sloc;
	Invalidate(btnDragTest);
}

void
ShlExplorer::TFormExtra::OnTouchUp_btnDragTest(TouchEventArgs&& e)
{
	InputCounter(e);
	FetchShell<ShlExplorer>().ShowString(strCount);
	Invalidate(btnDragTest);
}
void
ShlExplorer::TFormExtra::OnTouchDown_btnDragTest(TouchEventArgs&& e)
{
	InputCounterAnother(e);
	FetchShell<ShlExplorer>().ShowString(strCount);
//	btnDragTest.Refresh();
}

void
ShlExplorer::TFormExtra::OnClick_btnDragTest(TouchEventArgs&&)
{
	static FontCache& fc(FetchGlobalInstance().GetFontCache());
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
		btnDragTest.Font = Font(*(*it)->GetFontFamilyPtr(),
			16 - (itype << 1), FontStyle::Regular);
	//	btnDragTest.Font = Font(*(*it)->GetFontFamilyPtr(),
	//	GetDefaultFontFamily(), 16 - (itype << 1), FontStyle::Regular);
		siprintf(strtf, "%d, %d file(s), %d type(s), %d faces(s);\n",
			btnDragTest.Font.GetSize(), ffilen, ftypen, ffacen);
		btnDragTest.Text = strtf;
		btnDragTest.ForeColor = GenerateRandomColor();
		btnClose.ForeColor = GenerateRandomColor();
		SetEnabledOf(btnClose);
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
ShlExplorer::TFormExtra::OnKeyPress_btnDragTest(IControl& sender,
	KeyEventArgs&& e)
{
	u32 k(static_cast<KeyEventArgs::InputType>(e));
	char strt[100];
	auto& lbl(dynamic_cast<Label&>(sender));

//	Button& lbl(dynamic_cast<TFormUp&>(
//		*(dynamic_cast<ShlExplorer&>(*FetchShellHandle()).hWndUp)).lblB);
	lbl.SetTransparent(!lbl.IsTransparent());
//	++lbl.ForeColor;
//	--lbl.BackColor;
	siprintf(strt, "%d;\n", k);
	lbl.Text = strt;
	Invalidate(lbl);
/*
	Button& lbl(static_cast<Button&>(sender));

	if(nCountInput & 1)
		lbl.Text = _ustr("测试键盘...");
*/
}

void
ShlExplorer::TFormExtra::OnClick_btnClose(TouchEventArgs&&)
{
	Hide(*this);
}

void
ShlExplorer::TFormExtra::OnClick_btnTestEx(TouchEventArgs&& e)
{
	using namespace Drawing;
/*
	uchar_t* tstr(Text::ucsdup("Abc测试", Text::CS_Local));
	String str(tstr);

	std::free(tstr);
*/
	TestObj t(FetchGlobalInstance().GetDesktopDownHandle());
//	const auto& g(FetchContext(*t.h));

	t.str = String(_ustr("Abc测试"));
	switch(e.X * 4 / btnTestEx.GetWidth())
	{
	case 0:
		t.Fill();
		t.Pause();
	//	tr.BeFilledWith(ColorSpace::Black);
		t.Test2();
		t.Test3(ColorSpace::Black);
		t.Test3(ColorSpace::Blue);
	case 1:
		t.tr.SetSize(t.s.Width, t.s.Height);
	//	t.Pause();
	//	tr.BeFilledWith(ColorSpace::Black);
		t.Test3(ColorSpace::White);
		t.Test3(ColorSpace::Black);
		t.Test3(ColorSpace::Red);
		break;
	case 2:
		t.c = ColorSpace::Lime;
		t.Fill();
		t.Pause();
	//	tr.BeFilledWith(ColorSpace::Black);
		t.Test2();
		t.Test3(ColorSpace::Black);
		t.Test3(ColorSpace::Blue);
	case 3:
		t.c = ColorSpace::Lime;
		t.tr.SetSize(t.s.Width, t.s.Height);
	//	t.Pause();
	//	t.tr.BeFilledWith(ColorSpace::Black);
		t.Test3(ColorSpace::White);
		t.Test3(ColorSpace::Black);
		t.Test3(ColorSpace::Red);
	default:
		break;
	}
}


int
ShlExplorer::ShlProc(const Message& msg)
{
//	ClearDefaultMessageQueue();

//	UpdateToScreen();

	switch(msg.GetMessageID())
	{
	case SM_INPUT:
		ResponseInput(msg);
		SendPaintMessage();
		return 0;
	default:
		break;
	}
	return ShlDS::ShlProc(msg);
}

int
ShlExplorer::OnActivated(const Message& msg)
{
	ParentType::OnActivated(msg);

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	// parent-init-seg 0;
	dsk_up += lblTitle;
	dsk_up += lblPath;
	dsk_dn += fbMain;
	dsk_dn += btnTest;
	dsk_dn += btnOK;
	dsk_dn += chkTest;
	dsk_up += lblA;
	dsk_up += lblB;
	// init-seg 1;
	dsk_up.GetBackgroundImagePtr() = FetchImage(1);
	dsk_dn.GetBackgroundImagePtr() = FetchImage(2);
	// init-seg 2;
	lblTitle.Text = "文件列表：请选择一个文件。";
	lblPath.Text = "/";
//	lblTitle.Transparent = true;
//	lblPath.Transparent = true;
	btnOK.SetTransparent(false);
	btnTest.Text = _ustr("测试(X)");
	btnOK.Text = _ustr("确定(A)");
	// init-seg 3;
	dsk_dn.BoundControlPtr = std::bind(
		std::mem_fn(&ShlExplorer::GetBoundControlPtr), this,
		std::placeholders::_1);
	FetchEvent<KeyUp>(dsk_dn) += OnKey_Bound_TouchUpAndLeave;
	FetchEvent<KeyDown>(dsk_dn) += OnKey_Bound_EnterAndTouchDown;
	FetchEvent<KeyPress>(dsk_dn) += OnKey_Bound_Click;

	static FPSCounter fps_counter;

	FetchEvent<Paint>(dsk_dn) += [&](IControl&, EventArgs&&){
		char strt[60];
		auto& g(FetchContext(dsk_dn));
		using namespace ColorSpace;

		{
			const Rect r(0, 172, 72, 20);
			u32 t(fps_counter.Refresh());

			siprintf(strt, "FPS: %u.%03u", t/1000, t%1000);
			FillRect(g, r, Blue);
			DrawText(g, r, strt, Padding(), White);
		}
		{
			const Rect r(4, 144, 120, 20);
			Rect ri;

			dsk_dn.GetRenderer().GetInvalidatedArea(ri);
			siprintf(strt, "(%d, %d, %u, %u)",
				ri.X, ri.Y, ri.Width, ri.Height);
			FillRect(g, r, Green);
			DrawText(g, r, strt, Padding(), Yellow);
		}
	};
/*
	ReplaceHandle<IWindow*>(hWndUp,
		new TFrmFileListMonitor(shared_ptr<YShell>(this)));
	ReplaceHandle<IWindow*>(hWndDown,
		new TFrmFileListSelecter(shared_ptr<YShell>(this)));
*/
	//	dynamic_pointer_cast<TFrmFileListSelecter>(
	//		hWndDown)->fbMain.RequestFocus(GetZeroElement<EventArgs>());
	//	hWndDown->RequestFocus(GetZeroElement<EventArgs>());
	RequestFocusCascade(fbMain);
	// init-seg 4;
	dsk_dn.BackColor = ARGB16(1, 15, 15, 31);
	dsk_dn.SetInvalidation();
	pWndTest = unique_raw(new TFormTest());
	pWndExtra = unique_raw(new TFormExtra());
	pWndTest->SetVisible(false);
	pWndExtra->SetVisible(false);
	dsk_dn += *pWndTest;
	dsk_dn += *pWndExtra;
//	pWndTest->DrawContents();
//	pWndExtra->DrawContents();
	// init-seg 5;
/*	Menu& mnu(*new Menu(Rect::Empty, GenerateList(_ustr("TestMenuItem0")), 1u));

	FetchEvent<Click>(mnu) += OnClick_ShowWindow;
	mhMain += mnu;*/
	mhMain += *new Menu(Rect::Empty, GenerateList(_ustr("A:MenuItem")), 1u);
	mhMain += *new Menu(Rect::Empty, GenerateList(_ustr("B:MenuItem")), 2u);
	mhMain[1u] += make_pair(1u, &mhMain[2u]);
	ResizeForContent(mhMain[2u]);
	UpdateToScreen();
	return 0;
}

int
ShlExplorer::OnDeactivated(const Message& msg)
{
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	// uninit-seg 1;
	reset(dsk_up.GetBackgroundImagePtr());
	reset(dsk_dn.GetBackgroundImagePtr());
	// uninit-seg 3;
	dsk_dn.BoundControlPtr = std::bind(
		std::mem_fn(&Control::GetBoundControlPtr), &dsk_dn,
		std::placeholders::_1);
	FetchEvent<KeyUp>(dsk_dn) -= OnKey_Bound_TouchUpAndLeave;
	FetchEvent<KeyDown>(dsk_dn) -= OnKey_Bound_EnterAndTouchDown;
	FetchEvent<KeyPress>(dsk_dn) -= OnKey_Bound_Click;
	FetchEvent<Paint>(dsk_dn).Clear();
	// uninit-seg 4;
	reset(pWndTest);
	reset(pWndExtra);
	// uninit-seg 5;
	mhMain.Clear();
	// parent-uninit-seg 0;
	ParentType::OnDeactivated(msg);
	return 0;
}

IControl*
ShlExplorer::GetBoundControlPtr(const KeyCode& k)
{
	if(k == KeySpace::X)
		return &btnTest;
	if(k == KeySpace::A)
		return &btnOK;
	return nullptr;
}

void
ShlExplorer::ShowString(const String& s)
{
	lblA.Text = s;
	Invalidate(lblA);
}
void
ShlExplorer::ShowString(const char* s)
{
	ShowString(String(s));
}

void
ShlExplorer::OnClick_btnTest(TouchEventArgs&&)
{
	YAssert(is_null(pWndTest), "err: pWndTest is null;");

	SwitchVisible(*pWndTest);
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
	Invalidate(lblPath);
}

void
ShlExplorer::OnConfirmed_fbMain(IControl&, IndexEventArgs&&)
{
//	if(e.Index == 2)
//		switchShl1();
}

void
ShlExplorer::OnClick_ShowWindow(IControl&, TouchEventArgs&&)
{
	auto& pWnd(FetchShell<ShlExplorer>().pWndExtra);

	YAssert(is_null(pWnd), "err: pWndExtra is null;");

	SwitchVisible(*pWnd);
}

void
ShlExplorer::OnTouchDown_FormExtra(IControl& sender, TouchEventArgs&&)
{
	try
	{
		TFormExtra& frm(dynamic_cast<TFormExtra&>(sender));

		frm.BackColor = GenerateRandomColor();
		frm.SetInvalidation();
	}
	catch(std::bad_cast&)
	{}
}


string ShlReader::path;

ShlReader::ShlReader()
	: ShlDS(),
	Reader(), pTextFile(), hUp(), hDn(), bgDirty(false),
	mhMain(*GetDesktopDownHandle())
{}

int
ShlReader::ShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_INPUT:
		ResponseInput(msg);
		SendPaintMessage();
		return 0;
	default:
		break;
	}
	return ShlDS::ShlProc(msg);
}

int
ShlReader::OnActivated(const Message& msg)
{
	ParentType::OnActivated(msg);
	pTextFile = ynew TextFile(path.c_str());
	Reader.LoadText(*pTextFile);
	bgDirty = true;

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	std::swap(hUp, dsk_up.GetBackgroundImagePtr());
	std::swap(hDn, dsk_dn.GetBackgroundImagePtr());
	dsk_up.BackColor = ARGB16(1, 30, 27, 24);
	dsk_dn.BackColor = ARGB16(1, 24, 27, 30);
	dsk_up.SetInvalidation();
	dsk_dn.SetInvalidation();
	FetchEvent<Click>(dsk_dn).Add(*this, &ShlReader::OnClick);
	FetchEvent<KeyDown>(dsk_dn).Add(*this, &ShlReader::OnKeyDown);
	FetchEvent<KeyHeld>(dsk_dn) += OnKeyHeld;
//	FetchEvent<Paint>(dsk_up).Add(*this, &ShlReader::OnPaint_Up);
//	FetchEvent<Paint>(dsk_dn).Add(*this, &ShlReader::OnPaint_Down);
	dsk_up += Reader.AreaUp;
	dsk_dn += Reader.AreaDown;

	{
		auto hList(share_raw(new Menu::ListType));
		auto& lst(*hList);

		lst.push_back("返回");
		lst.push_back(_ustr("2"));
		lst.push_back("3");

		Menu& mnu(*new Menu(Rect::Empty, std::move(hList), 1u));

		mnu.GetConfirmed() += [this](IControl&, IndexEventArgs&& e)
		{
			if(e.Index == 0)
				CallStored<ShlExplorer>();
		};
		mhMain += mnu;
		/*
		mhMain += *new Menu(Rect::Empty, GenerateList("返回"), 1u);
		mhMain += *new Menu(Rect::Empty, GenerateList("向上一行"), 2u);
		mhMain += *new Menu(Rect::Empty, GenerateList("向下一行"), 3u);
		mhMain += *new Menu(Rect::Empty, GenerateList("ll"), 4u);
		mhMain[1u] += make_pair(1u, &mhMain[2u]);
		mhMain[1u] += make_pair(2u, &mhMain[3u]);
		mhMain[1u] += make_pair(3u, &mhMain[4u]);
		*/
	}
	ResizeForContent(mhMain[1u]);
	RequestFocusCascade(dsk_dn);
	UpdateToScreen();
	return 0;
}

int
ShlReader::OnDeactivated(const Message& msg)
{
	mhMain.Clear();

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	FetchEvent<Click>(dsk_dn).Remove(*this, &ShlReader::OnClick);
	FetchEvent<KeyDown>(dsk_dn).Remove(*this, &ShlReader::OnKeyDown);
	FetchEvent<KeyHeld>(dsk_dn) -= OnKeyHeld;
//	FetchEvent<Paint>(dsk_up).Remove(*this, &ShlReader::OnPaint_Up);
//	FetchEvent<Paint>(dsk_dn).Remove(*this, &ShlReader::OnPaint_Down);
	dsk_up -= Reader.AreaUp;
	dsk_dn -= Reader.AreaDown;
	std::swap(hUp, dsk_up.GetBackgroundImagePtr());
	std::swap(hDn, dsk_dn.GetBackgroundImagePtr());
	Reader.UnloadText();
	safe_delete_obj()(pTextFile);
	ParentType::OnDeactivated(msg);
	return 0;
}

void
ShlReader::UpdateToScreen()
{
	if(bgDirty)
	{
		bgDirty = false;
		//强制刷新背景。
		GetDesktopUp().SetInvalidation();
		GetDesktopDown().SetInvalidation();
	}
	ParentType::UpdateToScreen();
}

void
ShlReader::OnClick(TouchEventArgs&& e)
{
	mhMain[1u].SetLocation(e);
	mhMain.Show(1u);
}

void
ShlReader::OnKeyDown(KeyEventArgs&& e)
{
	u32 k(static_cast<KeyEventArgs::InputType>(e));

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
		//Reader.SetColor(ARGB16(1, (cc&(15<<5))>>5, cc&29,
		//	(cc&(31<<10))>>10));
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

/*
void ShlReader::OnPaint_Up(EventArgs&&)
{
	Reader.PrintTextUp(FetchContext(GetDesktopUp()));
}

void ShlReader::OnPaint_Down(EventArgs&&)
{
	Reader.PrintTextDown(FetchContext(GetDesktopDown()));
}
*/

YSL_END

