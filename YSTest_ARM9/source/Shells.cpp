/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Shells.cpp
\ingroup YReader
\brief Shell 框架逻辑。
\version r5649;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2012-03-12 09:16 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#include "Shells.h"
#include "ShlReader.h"

////////
//测试用声明：全局资源定义。
//extern char gstr[128];

using namespace ystdex;

YSL_BEGIN_NAMESPACE(YReader)

namespace
{
	ResourceMap GlobalResourceMap;
}

DeclResource(GR_BGs)

namespace
{
	using namespace YReader;

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
		//hypY1
		buf[y * MainScreenWidth + x] = Color(
			(~(x * y) >> 2),
			(x | y | 128),
			(240 - ((x & y) >> 1))
		);
	}
	void
	dfap(BitmapPtr buf, SDst x, SDst y)
	{
		//bza1BRGx
		buf[y * MainScreenWidth + x] = Color(
			((x << 4) / (y | 1)),
			((x | y << 1) % (y + 2)),
			((~y | x << 1) % 27 + 3)
		);
	}
	void
	dfac1(BitmapPtr buf, SDst x, SDst y)
	{
		//fl1RBGx
		buf[y * MainScreenWidth + x] = Color(
			(x + y * y),
			((x & y) ^ (x | y)),
			(x * x + y)
		);
	}
	void
	dfac1p(BitmapPtr buf, SDst x, SDst y)
	{
		//rz3GRBx
		buf[y * MainScreenWidth + x] = Color(
			((x * y) | x),
			((x * y) | y),
			((x ^ y) * (x ^ y))
		);
	}
	void
	dfac2(BitmapPtr buf, SDst x, SDst y)
	{
		//v1BGRx
		buf[y * MainScreenWidth + x] = Color(
			((x << 4) / (y & 1)),
			(~x % 101 + y),
			((x + y) % ((y - 2) & 1) + (x << 2))
		);
	}
	void
	dfac2p(BitmapPtr buf, SDst x, SDst y)
	{
		//arz1x
		buf[y * MainScreenWidth + x] = Color(
			((x | y) % (y + 2)),
			((~y | x) % 27 + 3),
			((x << 6) / (y | 1))
		);
	}

	template<typename _tTarget>
	_tTarget&
	FetchGlobalResource(ResourceIndex idx)
	{
		if(GlobalResourceMap[idx].IsEmpty())
			GlobalResourceMap[idx] = MakeValueObjectByPtr(new _tTarget());
		return GlobalResourceMap[GR_BGs].GetObject<_tTarget>();
	}

	shared_ptr<Image>&
	FetchGlobalImage(size_t idx)
	{
		auto& spi(FetchGlobalResource<array<shared_ptr<Image>, 10>>(GR_BGs));

		YAssert(IsInInterval(idx, 10u), "Array index out of range"
			" @ FetchGlobalImage;");
		return spi[idx];
	}
}

YSL_END_NAMESPACE(YReader)


YSL_BEGIN

void
ReleaseShells()
{
	using namespace YReader;

	for(size_t i(0); i != 10; ++i)
		FetchGlobalImage(i).reset();
	GlobalResourceMap.clear();
	ReleaseStored<ShlReader>();
	ReleaseStored<ShlExplorer>();
}

YSL_END


YSL_BEGIN_NAMESPACE(YReader)

using namespace Shells;
using namespace Drawing::ColorSpace;

shared_ptr<Image>&
FetchImage(size_t i)
{
	//色块覆盖测试用程序段。
	const PPDRAW p_bg[10] = {nullptr, dfa, dfap, dfac1, dfac1p, dfac2, dfac2p};

	if(!FetchGlobalImage(i) && p_bg[i])
	{
		auto& h(FetchGlobalImage(i));

		if(!h)
			h = make_shared<Image>(nullptr, MainScreenWidth, MainScreenHeight);
		ScrDraw(h->GetBufferPtr(), p_bg[i]);
	}
	return FetchGlobalImage(i);
}


u32
FPSCounter::Refresh()
{
	using namespace platform;

	if(last_tick != GetHighResolutionTicks())
	{
		last_tick = now_tick;
		now_tick = GetHighResolutionTicks();
	}
	return now_tick == last_tick ? 0
		: 1000000000000ULL / (now_tick - last_tick);
}


namespace
{
	shared_ptr<TextList::ListType>
	GenerateList(const String& str)
	{
		auto p(new TextList::ListType());

		p->push_back(str);

		char cstr[40];

		std::sprintf(cstr, "%p;", p);
		p->push_back(cstr);
		return share_raw(p);
	}

	void
	SwitchVisible(IWidget& wgt)
	{
		SetVisibleOf(wgt, !IsVisible(wgt));
		Invalidate(wgt);
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

	namespace EnrtySpace
	{
		typedef enum
		{
			Empty,
			Now,
			Parent,
			Directory,
			Text,
			Hex
		} EntryType;
	}

	bool
	ReaderPathFilter(const string& path)
	{
		const auto ext(IO::GetExtensionOf(path).c_str());
		
		return !strcasecmp(ext, "txt")
			|| !strcasecmp(ext, "c")
			|| !strcasecmp(ext, "cpp")
			|| !strcasecmp(ext, "h")
			|| !strcasecmp(ext, "hpp")
			|| !strcasecmp(ext, "ini")
			|| !strcasecmp(ext, "xml");
	}

	EnrtySpace::EntryType
	GetEntryType(const string& path)
	{
		using namespace EnrtySpace;

		if(path.length() == 0)
			return Empty;
		if(path == IO::FS_Now)
			return Now;
		if(path == IO::FS_Parent)
			return Parent;
		if(*path.rbegin() == '/')
			return Directory;
		if(ReaderPathFilter(path))
			return Text;
		return Hex;
	}
	EnrtySpace::EntryType
	GetEntryType(const IO::Path& path)
	{
		return GetEntryType(path.GetNativeString());
	}

	bool
	CheckReaderEnability(FileBox& fb, CheckBox& hex)
	{
		if(fb.IsSelected())
		{
			using namespace EnrtySpace;

			switch(GetEntryType(fb.GetList()[fb.GetSelectedIndex()]))
			{
			case Text:
				return true;
			case Hex:
				return hex.IsTicked();
			default:
				;
			}
		}
		return false;
	}
}


ShlExplorer::ShlExplorer()
	: ShlDS(),
	lblTitle(Rect(16, 20, 220, 22)), lblPath(Rect(12, 80, 240, 22)),
	fbMain(Rect(4, 6, 248, 128)),
	btnTest(Rect(115, 165, 65, 22)), btnOK(Rect(185, 165, 65, 22)),
	chkFPS(Rect(208, 144, 16, 16)), chkHex(Rect(232, 144, 16, 16)),
	pWndTest(), pWndExtra(),
	lblA(Rect(5, 20, 200, 22)),
	lblB(Rect(5, 120, 72, 22)),
	mhMain(*GetDesktopDownHandle())
{
	//对 fbMain 启用缓存。
	fbMain.SetRenderer(make_unique<BufferedRenderer>(true));
	yunseq(
		FetchEvent<KeyPress>(fbMain) += [](KeyEventArgs&& e){
			if(e.GetKeyCode() & KeySpace::L)
				CallStored<ShlExplorer>();
		},
		fbMain.GetViewChanged() += [this](UIEventArgs&&){
			lblPath.Text = fbMain.GetPath();
			Invalidate(lblPath);
		},
		fbMain.GetSelected() += [this](IndexEventArgs&&){
			Enable(btnOK, CheckReaderEnability(fbMain, chkHex));
		},
		fbMain.GetConfirmed() += OnConfirmed_fbMain,
		FetchEvent<Click>(btnTest) += [this](TouchEventArgs&&){
			YAssert(bool(pWndTest), "err: pWndTest is null;");

			SwitchVisible(*pWndTest);
		},
		FetchEvent<Click>(btnOK) += [this](TouchEventArgs&&){
			if(fbMain.IsSelected())
			{
				const auto& path(fbMain.GetPath());
				const string& s(path.GetNativeString());

				if(!IO::ValidateDirectory(s) && fexists(s.c_str()))
				{
					ShlReader::CurrentPath = path;
					ShlReader::CurrentIsText = GetEntryType(s)
						== EnrtySpace::Text && !chkHex.IsTicked();
					CallStored<ShlReader>();
				}
			}
		},
		FetchEvent<Click>(chkFPS) += [this](TouchEventArgs&&){
			SetInvalidationOf(GetDesktopDown());
		},
		FetchEvent<Click>(chkHex) += [this](TouchEventArgs&&){
			Enable(btnOK, CheckReaderEnability(fbMain, chkHex));
			SetInvalidationOf(GetDesktopDown());
		},
		lblA.Text = Application::ProductName,
		lblB.Text = "程序测试"
	);
	lblB.SetTransparent(true);
	yunseq(Enable(btnTest, true), Enable(btnOK, false));
}


ShlExplorer::TFormTest::TFormTest()
	: Form(Rect(10, 40, 228, 100), shared_ptr<Image>()),
	btnEnterTest(Rect(4, 4, 146, 22)), /*FetchImage(6)*/
	btnMenuTest(Rect(152, 4, 60, 22)),
	btnShowWindow(Rect(8, 32, 104, 22)),
	btnPrevBackground(Rect(45, 64, 30, 22)),
	btnNextBackground(Rect(95, 64, 30, 22))
//Rect(120, 32, 96, 22)
{
	*this += btnEnterTest,
	*this += btnMenuTest,
	*this += btnShowWindow,
	*this += btnPrevBackground,
	*this += btnNextBackground,
	yunseq(
		BackColor = Color(248, 248, 120),
		btnEnterTest.Text = u"边界测试",
		btnEnterTest.HorizontalAlignment = TextAlignment::Right,
		btnEnterTest.VerticalAlignment = TextAlignment::Up,
		btnMenuTest.Text = u"菜单测试",
		btnShowWindow.Text = u"显示/隐藏窗口",
		btnShowWindow.HorizontalAlignment = TextAlignment::Left,
		btnShowWindow.VerticalAlignment = TextAlignment::Down,
		btnPrevBackground.Text = "<<",
		btnNextBackground.Text = ">>"
	);
	SetInvalidationOf(*this);

	static int up_i(1);

	yunseq(
		FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging,
		FetchEvent<Enter>(btnEnterTest) += [](TouchEventArgs&& e){
			char str[20];

			std::sprintf(str, "Enter: (%d, %d)", e.GetX(), e.GetY());

			auto& btn(ystdex::polymorphic_downcast<Button&>(e.GetSender()));

			btn.Text = str;
			Invalidate(btn);
		},
		FetchEvent<Leave>(btnEnterTest) += [](TouchEventArgs&& e){
			char str[20];

			std::sprintf(str, "Leave: (%d, %d)", e.GetX(), e.GetY());

			auto& btn(ystdex::polymorphic_downcast<Button&>(e.GetSender()));

			btn.Text = str;
			Invalidate(btn);
		},
		FetchEvent<Click>(btnMenuTest) +=[this](TouchEventArgs&&){
			static int t;

			auto& mhMain(FetchShell<ShlExplorer>().mhMain);
			auto& mnu(mhMain[1u]);
			auto& lst(mnu.GetList());

			if(mhMain.IsShowing(1u))
			{
				if(lst.size() > 4)
					lst.clear();

				char stra[4];

				std::sprintf(stra, "%d", t);
				lst.push_back(string("TMI") + stra);
			}
			else
			{
				SetLocationOf(mnu, Point(
					btnMenuTest.GetX() - btnMenuTest.GetWidth(),
					btnMenuTest.GetY() + btnMenuTest.GetHeight()));
				mnu.SetWidth(btnMenuTest.GetWidth() + 20);
				lst.push_back(u"TestMenuItem1");
				mhMain.Show(1u);
			}
			ResizeForContent(mnu);
			Invalidate(mnu);
			++t;
		},
		FetchEvent<Click>(btnShowWindow) += OnClick_ShowWindow,
		FetchEvent<Click>(btnPrevBackground) += [this](TouchEventArgs&&){
			auto& shl(FetchShell<ShlExplorer>());
			auto& dsk_up_ptr(shl.GetDesktopUp().GetBackgroundImagePtr());
			auto& dsk_dn_ptr(shl.GetDesktopDown().GetBackgroundImagePtr());

			if(up_i > 1)
			{
				--up_i;
				Enable(btnNextBackground);
			}
			if(up_i == 1)
				Enable(btnPrevBackground, false);
			dsk_up_ptr = FetchImage(up_i);
			dsk_dn_ptr = FetchImage(up_i + 1);
			SetInvalidationOf(shl.GetDesktopUp());
			SetInvalidationOf(shl.GetDesktopDown());
		},
		FetchEvent<Click>(btnNextBackground) += [this](TouchEventArgs&&){
			auto& shl(FetchShell<ShlExplorer>());
			auto& dsk_up_ptr(shl.GetDesktopUp().GetBackgroundImagePtr());
			auto& dsk_dn_ptr(shl.GetDesktopDown().GetBackgroundImagePtr());

			if(up_i < 5)
			{
				++up_i;
				Enable(btnPrevBackground);
			}
			if(up_i == 5)
				Enable(btnNextBackground, false);
			dsk_up_ptr = FetchImage(up_i);
			dsk_dn_ptr = FetchImage(up_i + 1);
			SetInvalidationOf(shl.GetDesktopUp());
			SetInvalidationOf(shl.GetDesktopDown());
		}
	);
	Enable(btnPrevBackground, false);
}

ShlExplorer::TFormExtra::TFormExtra()
	: Form(Rect(5, 60, 208, 120), shared_ptr<Image>()), /*FetchImage(7)*/
	btnDragTest(Rect(13, 15, 184, 22)),
	btnTestEx(Rect(13, 52, 168, 22)),
	btnClose(Rect(13, 82, 60, 22)),
	btnExit(Rect(83, 82, 60, 22))
{
	*this += btnDragTest,
	*this += btnTestEx,
	*this += btnClose,
	*this += btnExit;
	yunseq(
		btnDragTest.Text = u"测试拖放控件",
		btnDragTest.HorizontalAlignment = TextAlignment::Left,
		btnTestEx.Text = u"直接屏幕绘制测试",
		btnClose.Text = u"关闭",
		btnExit.Text = u"退出",
		BackColor = Color(248, 120, 120),
		//	btnDragTest.Enabled = false,
		btnClose.BackColor = Color(176, 184, 192)
	);
	SetInvalidationOf(*this);
	yunseq(
		FetchEvent<TouchDown>(*this) += [this](TouchEventArgs&& e){
			BackColor = GenerateRandomColor();
			SetInvalidationOf(*this);
			if(e.Strategy == RoutedEventArgs::Direct)
				e.Handled = true;
		},
		FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging,
		FetchEvent<Move>(btnDragTest) += [this](UIEventArgs&&){
			char sloc[20];

			std::sprintf(sloc, "(%d, %d);", btnDragTest.GetX(),
				btnDragTest.GetY());
			btnDragTest.Text = sloc;
			Invalidate(btnDragTest);
		},
		FetchEvent<TouchDown>(btnDragTest) += [this](TouchEventArgs&&){
			struct mallinfo t(mallinfo());
			char strMemory[40];

			/*	std::sprintf(strMemory, "%d,%d,%d,%d,%d;",
					t.arena,    // total space allocated from system 2742496
					t.ordblks,  // number of non-inuse chunks 37
					t.smblks,   // unused -- always zero 0
					t.hblks,    // number of mmapped regions 0
					t.hblkhd   // total space in mmapped regions 0
				);*/
			/*	std::sprintf(strMemory, "%d,%d,%d,%d,%d;",
					t.usmblks,  // unused -- always zero 0
					t.fsmblks,  // unused -- always zero 0
					t.uordblks, // total allocated space 2413256, 1223768
					t.fordblks, // total non-inuse space 329240, 57760
					t.keepcost // top-most, releasable (via malloc_trim) space
					//46496,23464
					);*/
			std::sprintf(strMemory, "%d,%d,%d,%d,%d;",
				t.arena,
				t.ordblks,
				t.uordblks,
				t.fordblks,
				t.keepcost);

			auto& lblA(FetchShell<ShlExplorer>().lblA);

			lblA.Text = strMemory;
			Invalidate(lblA);
		},
		FetchEvent<TouchMove>(btnDragTest) += OnTouchMove_Dragging,
		FetchEvent<Click>(btnDragTest) += [this](TouchEventArgs&&){
#if 0
			auto hList(FetchShell<ShlExplorer>().hFontFamilyNames);
			char strtf[0x100];

			std::sprintf(strtf, "1/%d;%s;", hList->size(),
				(*hList)[0].GetMBCS().c_str());
#endif
			yunseq(
//				btnDragTest.Text = strtf,
				btnDragTest.ForeColor = GenerateRandomColor(),
				btnClose.ForeColor = GenerateRandomColor()
			);
			Invalidate(*this);
		//	Enable(btnClose);
		},
		FetchEvent<Click>(btnTestEx) += [this](TouchEventArgs&& e){
			using namespace Drawing;
			using namespace ColorSpace;

			TestObj t(FetchGlobalInstance().GetDesktopDownHandle());

			t.str = u"Abc测试";
			switch(e.X * 4 / btnTestEx.GetWidth())
			{
			case 0:
				t.Fill();
				t.Pause();
			//	tr.BeFilledWith(ColorSpace::Black);
				t.Test2();
				t.Test3(Black);
				t.Test3(Blue);
			case 1:
				t.tr.SetSize(t.s.Width, t.s.Height);
			//	t.Pause();
			//	tr.BeFilledWith(ColorSpace::Black);
				t.Test3(White);
				t.Test3(Black);
				t.Test3(Red);
				break;
			case 2:
				t.c = Lime;
				t.Fill();
				t.Pause();
			//	tr.BeFilledWith(ColorSpace::Black);
				t.Test2();
				t.Test3(Black);
				t.Test3(Blue);
			case 3:
				t.c = Lime;
				t.tr.SetSize(t.s.Width, t.s.Height);
			//	t.Pause();
			//	t.tr.BeFilledWith(ColorSpace::Black);
				t.Test3(White);
				t.Test3(Black);
				t.Test3(Red);
			default:
				break;
			}
		},
		FetchEvent<KeyPress>(btnDragTest) += [](KeyEventArgs&& e){
			u32 k(static_cast<KeyEventArgs::InputType>(e));
			char strt[100];
			auto& lbl(polymorphic_downcast<Label&>(e.GetSender()));

			lbl.SetTransparent(!lbl.IsTransparent());
			std::sprintf(strt, "%d;\n", k);
			lbl.Text = strt;
			Invalidate(lbl);
#if 0
			Button& lbl(static_cast<Button&>(e.GetSender()));

			lbl.Text = u"测试键盘...";
#endif
		},
		FetchEvent<Click>(btnClose) += [this](TouchEventArgs&&){
			Hide(*this);
		},
		FetchEvent<Click>(btnExit) += [](TouchEventArgs&&){
			PostQuitMessage(0);
		}
	);
}


void
ShlExplorer::OnActivated(const Message& msg)
{
	ParentType::OnActivated(msg);

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	// parent-init-seg 0;
	dsk_up += lblTitle,
	dsk_up += lblPath;
	dsk_dn += fbMain,
	dsk_dn += btnTest,
	dsk_dn += btnOK,
	dsk_dn += chkFPS,
	dsk_dn += chkHex;
	dsk_up += lblA,
	dsk_up += lblB;
	// init-seg 1;
	yunseq(
		dsk_up.GetBackgroundImagePtr() = FetchImage(1),
		dsk_dn.GetBackgroundImagePtr() = FetchImage(2),
	// init-seg 2;
		lblTitle.Text = "文件列表：请选择一个文件。",
		lblPath.Text = "/",
	//	lblTitle.Transparent = true,
	//	lblPath.Transparent = true;
		btnTest.Text = u"测试(X)",
		btnOK.Text = u"确定(A)"
	);
	btnOK.SetTransparent(false);
	// init-seg 3;
	yunseq(
		dsk_dn.BoundControlPtr = std::bind(&ShlExplorer::GetBoundControlPtr,
			this, std::placeholders::_1),
		FetchEvent<KeyUp>(dsk_dn) += OnKey_Bound_TouchUpAndLeave,
		FetchEvent<KeyDown>(dsk_dn) += OnKey_Bound_EnterAndTouchDown,
		FetchEvent<KeyPress>(dsk_dn) += OnKey_Bound_Click
	);
	RequestFocusCascade(fbMain);
	// init-seg 4;
	yunseq(
		dsk_dn.BackColor = Color(120, 120, 248),
		pWndTest = make_unique<TFormTest>(),
		pWndExtra = make_unique<TFormExtra>()
	);
	SetVisibleOf(*pWndTest, false),
	SetVisibleOf(*pWndExtra, false);
	dsk_dn += *pWndTest,
	dsk_dn += *pWndExtra;
	SetInvalidationOf(dsk_dn);
//	pWndTest->DrawContents();
//	pWndExtra->DrawContents();
	// init-seg 5;
/*	Menu& mnu(*(ynew Menu(Rect::Empty, GenerateList(u"TestMenuItem0"), 1u)));

	FetchEvent<Click>(mnu) += OnClick_ShowWindow;
	mhMain += mnu;*/
	mhMain += *(ynew Menu(Rect::Empty, GenerateList(u"A:MenuItem"), 1u)),
	mhMain += *(ynew Menu(Rect::Empty, GenerateList(u"B:MenuItem"), 2u));
	mhMain[1u] += make_pair(1u, &mhMain[2u]);
	ResizeForContent(mhMain[2u]);
}

void
ShlExplorer::OnDeactivated()
{
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	// uninit-seg 1;
	yunseq(
		reset(dsk_up.GetBackgroundImagePtr()),
		reset(dsk_dn.GetBackgroundImagePtr()),
	// uninit-seg 3;
		dsk_dn.BoundControlPtr = std::bind(&Control::GetBoundControlPtr,
			&dsk_dn, std::placeholders::_1),
		FetchEvent<KeyUp>(dsk_dn) -= OnKey_Bound_TouchUpAndLeave,
		FetchEvent<KeyDown>(dsk_dn) -= OnKey_Bound_EnterAndTouchDown,
		FetchEvent<KeyPress>(dsk_dn) -= OnKey_Bound_Click
	);
	// uninit-seg 4;
	dsk_dn -= *pWndTest,
	dsk_dn -= *pWndExtra;
	yunseq(
		reset(pWndTest),
		reset(pWndExtra)
	);
	// uninit-seg 5;
	mhMain.Clear();
	// parent-uninit-seg 0;
	ParentType::OnDeactivated();
}

void
ShlExplorer::OnPaint()
{
	// NOTE: overwriting member function OnInput using SM_TASK is also valid due
	//	to the SM_INPUT message is sent continuously, but with less efficiency.
	auto& dsk_dn(GetDesktopDown());

	if(chkFPS.IsTicked())
	{
		using namespace ColorSpace;

		auto& g(dsk_dn.GetContext());
		const Rect r(0, 172, 80, 20);
		const u32 t(fpsCounter.Refresh());
		char strt[20];

		std::sprintf(strt, "FPS: %u.%03u", t / 1000, t % 1000);
		FillRect(g, r, Blue);
		DrawText(g, r, strt, DefaultMargin, White);
	}
}

IWidget*
ShlExplorer::GetBoundControlPtr(const KeyCode& k)
{
	if(k == KeySpace::X)
		return &btnTest;
	if(k == KeySpace::A)
		return &btnOK;
	return nullptr;
}

void
ShlExplorer::OnConfirmed_fbMain(IndexEventArgs&&)
{
//	if(e.Index == 2)
//		CallStored<ShlExplorer>();
}

void
ShlExplorer::OnClick_ShowWindow(TouchEventArgs&&)
{
	auto& pWnd(FetchShell<ShlExplorer>().pWndExtra);

	YAssert(bool(pWnd), "err: pWndExtra is null;");

	SwitchVisible(*pWnd);
}

YSL_END_NAMESPACE(YReader)

