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
\version r5645
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-06 21:38:16 +0800
\par 修改时间:
	2012-12-08 23:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::Shells
*/


#include "Shells.h"
#include "ShlReader.h"
#include <ytest/timing.hpp>

////////
//测试用声明：全局资源定义。
//extern char gstr[128];

using namespace ystdex;

YSL_BEGIN_NAMESPACE(YReader)

namespace
{

ResourceMap GlobalResourceMap;
//! \since build 319
double gfx_init_time;

} // unnamed namespace;

DeclResource(GR_BGs)

namespace
{

using namespace YReader;

//测试函数。

template<typename _tTarget>
_tTarget&
FetchGlobalResource(ResourceIndex idx)
{
	if(!GlobalResourceMap[idx])
		GlobalResourceMap[idx] = MakeValueObjectByPtr(new _tTarget());
	return GlobalResourceMap[GR_BGs].GetObject<_tTarget>();
}

shared_ptr<Image>&
FetchGlobalImage(size_t idx)
{
	auto& spi(FetchGlobalResource<array<shared_ptr<Image>, 10>>(GR_BGs));

	YAssert(IsInInterval(idx, 10u), "Index is out of range.");
	return spi[idx];
}

} // unnamed namespace;

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
	static Color(*const p_bg[10])(SDst, SDst){nullptr,
		[](SDst x, SDst y)->Color{
			return {~(x * y) >> 2, x | y | 128, 240 - ((x & y) >> 1)};
		},
		[](SDst x, SDst y)->Color{
			return {(x << 4) / (y | 1), (x | y << 1) % (y + 2),
				(~y | x << 1) % 27 + 3};
		},
		[](SDst x, SDst y)->Color{
			return {~(x * y) >> 2, x | y | 128, 240 - ((x & y) >> 1)};
		},
		[](SDst x, SDst y)->Color{
			return {x + y * y, (x & y) ^ (x | y), x * x + y};
		},
		[](SDst x, SDst y)->Color{
			return {(x << 4) / ((y & 1) | 1), ~x % 101 + y,
				(x + y) % (((y - 2) & 1) | 129) + (x << 2)};
		},
		[](SDst x, SDst y)->Color{
			return {(x | y) % (y + 2), (~y | x) % 27 + 3, (x << 6) / (y | 1)};
		}
	};

	if(!FetchGlobalImage(i) && p_bg[i])
	{
		auto& h(FetchGlobalImage(i));

		if(!h)
			h = make_shared<Image>(nullptr, MainScreenWidth, MainScreenHeight);
		gfx_init_time += ytest::timing::once(Timers::HighResolutionClock::now,
			ScrDraw<BitmapPtr, decltype(*p_bg)>,
			h->GetBufferPtr(), p_bg[i]).count() / 1e9;
	}
	return FetchGlobalImage(i);
}


namespace
{

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

EnrtySpace::EntryType
GetEntryType(const string& path)
{
	using namespace EnrtySpace;

	if(path.empty())
		return Empty;
	if(path == IO::FS_Now)
		return Now;
	if(path == IO::FS_Parent)
		return Parent;
	if(*path.rbegin() == YCL_PATH_DELIMITER)
		return Directory;

	static yconstexpr const char* exts[]{
		"txt", "c", "cpp", "h", "hpp", "ini", "xml"};
	const auto ext(IO::GetExtensionOf(path).c_str());

	if(std::any_of(exts, exts + arrlen(exts), [&](const char* str){
		return ::strcasecmp(ext, str) == 0;
	}))
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

//! \since build 330
void
CheckBackgroundPreview(CheckButton& cbPreview, size_t up_i, size_t dn_i)
{
	if(cbPreview.IsTicked())
	{
		auto& app(FetchGlobalInstance());

		app.GetScreenUp().Update(FetchImage(up_i)->GetBufferPtr());
		app.GetScreenDown().Update(FetchImage(dn_i)->GetBufferPtr());
		platform::WaitForInput();
	}
}

} // unnamed namespace;


ShlExplorer::ShlExplorer(const IO::Path& path)
	: ShlDS(),
	lblTitle(Rect(16, 20, 220, 22)), lblPath(Rect(8, 48, 240, 48)),
	lblInfo(Rect(8, 100, 240, 64)), fbMain(Rect(4, 6, 248, 128)),
	btnTest(Rect(115, 165, 65, 22)), btnOK(Rect(185, 165, 65, 22)),
	btnMenu(Rect(4, 165, 72, 22)), pnlSetting(Rect(10, 40, 228, 108)),
	cbHex(Rect(142, 142, 103, 18)), cbFPS(Rect(10, 62, 73, 18)),
	cbPreview(Rect(10, 82, 115, 18)),
	btnEnterTest(Rect(4, 4, 146, 22)), btnShowWindow(Rect(8, 32, 104, 22)),
	btnPrevBackground(Rect(110, 64, 30, 22)),
	btnNextBackground(Rect(160, 64, 30, 22)),
	pWndExtra(make_unique<TFormExtra>()), mhMain(*GetDesktopDownHandle()),
	fpsCounter(500000000ULL)
{
	static int up_i(1);
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	AddWidgets(pnlSetting, cbFPS, cbPreview, btnEnterTest,
		btnShowWindow, btnPrevBackground, btnNextBackground),
	AddWidgets(dsk_up, lblTitle, lblPath, lblInfo),
	AddWidgets(dsk_dn, fbMain, btnTest, btnOK, btnMenu, cbHex),
	AddWidgetsZ(dsk_dn, DefaultWindowZOrder, pnlSetting, *pWndExtra),
	//启用缓存。
	fbMain.SetRenderer(make_unique<BufferedRenderer>(true)),
	pnlSetting.SetRenderer(make_unique<BufferedRenderer>()),
	SetVisibleOf(pnlSetting, false),
	SetVisibleOf(*pWndExtra, false),
	cbHex.Background = SolidBrush(Color(0xFF, 0xFF, 0xE0)),
	Enable(btnPrevBackground, false),
	yunseq(
		dsk_up.Background = ImageBrush(FetchImage(1)),
		dsk_dn.Background = ImageBrush(FetchImage(2)),
		lblTitle.Text = u"YReader",
		lblPath.AutoWrapLine = true, lblPath.Text = path,
		lblInfo.AutoWrapLine = true, lblInfo.Text = u"文件列表：请选择一个文件。",
	// TODO: Show current working directory properly.
		btnTest.Text = u"设置(X)",
		btnOK.Text = u"确定(A)",
#if YCL_MINGW32
		btnMenu.Text = u"菜单(P)",
#else
		btnMenu.Text = u"菜单(Start)",
#endif
		cbHex.Text = u"显示十六进制",
		cbFPS.Text = u"显示 FPS",
		cbPreview.Text = u"切换背景时预览",
		pnlSetting.Background = SolidBrush(Color(248, 248, 120)),
		btnEnterTest.Text = u"边界测试",
		btnEnterTest.HorizontalAlignment = TextAlignment::Right,
		btnEnterTest.VerticalAlignment = TextAlignment::Up,
		btnShowWindow.Text = u"显示/隐藏窗口",
		btnShowWindow.HorizontalAlignment = TextAlignment::Left,
		btnShowWindow.VerticalAlignment = TextAlignment::Down,
		btnPrevBackground.Text = u"<<",
		btnNextBackground.Text = u">>",
		fbMain.SetPath(path),
		Enable(btnTest, true),
		Enable(btnOK, false),
		dsk_dn.BoundControlPtr = std::bind(&ShlExplorer::GetBoundControlPtr,
			this, std::placeholders::_1),
		FetchEvent<KeyUp>(dsk_dn) += OnKey_Bound_TouchUpAndLeave,
		FetchEvent<KeyDown>(dsk_dn) += OnKey_Bound_EnterAndTouchDown,
		FetchEvent<KeyPress>(dsk_dn) += OnKey_Bound_Click,
		fbMain.GetViewChanged() += [this](UIEventArgs&&){
			lblPath.Text = fbMain.GetPath();
			Invalidate(lblPath);
		},
		fbMain.GetSelected() += [this](IndexEventArgs&&){
			Enable(btnOK, CheckReaderEnability(fbMain, cbHex));
		},
		FetchEvent<Click>(btnTest) += [this](TouchEventArgs&&){
			SwitchVisible(pnlSetting);
		},
		FetchEvent<Click>(btnOK) += [this](TouchEventArgs&&){
			if(fbMain.IsSelected())
			{
				const auto& path(fbMain.GetPath());
				const string& s(path.GetNativeString());

				if(!IO::ValidatePath(s) && ufexists(s))
				{
					if(GetEntryType(s) == EnrtySpace::Text
						&& !cbHex.IsTicked())
					// TODO: Use G++ 4.8 or later.
					//	SetShellTo(make_shared<ShlTextReader>(path));
						SetShellTo(share_raw(new ShlTextReader(path)));
					//	SetShellToNew<ShlTextReader>(path);
					else
					// TODO: Use G++ 4.8 or later.
					//	SetShellTo(make_shared<ShlHexBrowser>(path));
						SetShellTo(share_raw(new ShlHexBrowser(path)));
					//	SetShellToNew<ShlHexBrowser>(path);
				}
			}
		},
		FetchEvent<Click>(cbFPS) += [this](TouchEventArgs&&){
			SetInvalidationOf(GetDesktopDown());
		},
		FetchEvent<Click>(cbHex) += [this](TouchEventArgs&&){
			Enable(btnOK, CheckReaderEnability(fbMain, cbHex));
			SetInvalidationOf(GetDesktopDown());
		},
		FetchEvent<TouchMove>(pnlSetting) += OnTouchMove_Dragging,
		FetchEvent<Enter>(btnEnterTest) += [](TouchEventArgs&& e){
			auto& btn(ystdex::polymorphic_downcast<Button&>(e.GetSender()));

			btn.Text = u"Enter: " + String(to_string(e));
			Invalidate(btn);
		},
		FetchEvent<Leave>(btnEnterTest) += [](TouchEventArgs&& e){
			auto& btn(ystdex::polymorphic_downcast<Button&>(e.GetSender()));

			btn.Text = u"Leave: " + String(to_string(e));
			Invalidate(btn);
		},
		FetchEvent<Click>(btnMenu) += [this](TouchEventArgs&&){
			static int t;

			auto& mnu(mhMain[1u]);
			auto& lst(mnu.GetList());

			if(mhMain.IsShowing(1u))
			{
				if(lst.size() > 4)
					lst.clear();
				lst.push_back("TMI" + to_string(t));
				ResizeForContent(mnu);
			}
			else
			{
				lst.push_back(u"TestMenuItem1");
				ResizeForContent(mnu);
				mnu.SetWidth(btnMenu.GetWidth() + 20);
				SetLocationOf(mnu, Point(btnMenu.GetX(),
					btnMenu.GetY() - mnu.GetHeight()));
				mhMain.Show(1u);
			}
			Invalidate(mnu);
			++t;
		},
		FetchEvent<Click>(btnShowWindow) += OnClick_ShowWindow,
		FetchEvent<Click>(btnPrevBackground) += [this](TouchEventArgs&&){
			auto& dsk_up(GetDesktopUp());
			auto& dsk_dn(GetDesktopDown());

			if(up_i > 1)
			{
				--up_i;
				Enable(btnNextBackground);
			}
			if(up_i == 1)
				Enable(btnPrevBackground, false);
			yunseq(dsk_up.Background = ImageBrush(FetchImage(up_i)),
				dsk_dn.Background = ImageBrush(FetchImage(up_i + 1)));
			CheckBackgroundPreview(cbPreview, up_i, up_i + 1);
			SetInvalidationOf(dsk_up),
			SetInvalidationOf(dsk_dn);
		},
		FetchEvent<Click>(btnNextBackground) += [this](TouchEventArgs&&){
			auto& dsk_up(GetDesktopUp());
			auto& dsk_dn(GetDesktopDown());

			if(up_i < 5)
			{
				++up_i;
				Enable(btnPrevBackground);
			}
			if(up_i == 5)
				Enable(btnNextBackground, false);
			yunseq(dsk_up.Background = ImageBrush(FetchImage(up_i)),
				dsk_dn.Background = ImageBrush(FetchImage(up_i + 1)));
			CheckBackgroundPreview(cbPreview, up_i, up_i + 1);
			SetInvalidationOf(dsk_up),
			SetInvalidationOf(dsk_dn);
		}
	);
	RequestFocusCascade(fbMain),
	SetInvalidationOf(dsk_up),
	SetInvalidationOf(dsk_dn);
	// FIXME: potential memory leaks;
	mhMain += *(ynew Menu(Rect(),
		share_raw(new TextList::ListType{u"A:MenuItem"}), 1u)),
	mhMain += *(ynew Menu(Rect(),
		share_raw(new TextList::ListType{u"B:MenuItem"}), 2u));
	mhMain[1u] += make_pair(1u, &mhMain[2u]);
	ResizeForContent(mhMain[2u]);
}


ShlExplorer::TFormExtra::TFormExtra()
	: Form(Rect(5, 60, 208, 120), shared_ptr<Image>()), /*FetchImage(7)*/
	btnDragTest(Rect(13, 15, 184, 22)),
	btnTestEx(Rect(13, 52, 168, 22)),
	btnClose(Rect(13, 82, 60, 22)),
	btnExit(Rect(83, 82, 60, 22))
{
	AddWidgets(*this, btnDragTest, btnTestEx, btnClose, btnExit),
	yunseq(
		//	btnDragTest.Enabled = false,
		btnDragTest.Text = u"测试拖放控件",
		btnDragTest.HorizontalAlignment = TextAlignment::Left,
		btnTestEx.Text = u"附加测试",
		btnClose.Text = u"关闭",
		btnExit.Text = u"退出",
		Background = SolidBrush(Color(248, 120, 120)),
		btnClose.Background = SolidBrush(Color(176, 184, 192)),
		FetchEvent<TouchDown>(*this) += [this](TouchEventArgs&& e){
			Background = SolidBrush(GenerateRandomColor());
			SetInvalidationOf(*this);
			if(e.Strategy == RoutedEventArgs::Direct)
				e.Handled = true;
		},
		FetchEvent<TouchMove>(*this) += OnTouchMove_Dragging,
		FetchEvent<Move>(btnDragTest) += [this](UIEventArgs&&){
			btnDragTest.Text = to_string(GetLocationOf(btnDragTest)) + ';';
			Invalidate(btnDragTest);
		},
		FetchEvent<TouchDown>(btnDragTest) += [this](TouchEventArgs&&){
#if YCL_DS
			struct ::mallinfo t(::mallinfo());
			auto& lblInfo(FetchShell<ShlExplorer>().lblInfo);

			lblInfo.Text = ystdex::sfmt("%d,%d,%d,%d,%d;",
				t.arena, t.ordblks, t.uordblks, t.fordblks, t.keepcost);
			Invalidate(lblInfo);
#endif
		},
		FetchEvent<TouchMove>(btnDragTest) += OnTouchMove_Dragging,
		FetchEvent<Click>(btnDragTest) += [this](TouchEventArgs&&){
			yunseq(
				btnDragTest.ForeColor = GenerateRandomColor(),
				btnClose.ForeColor = GenerateRandomColor()
			);
			Invalidate(*this);
		//	Enable(btnClose);
		},
		FetchEvent<Click>(btnTestEx) += [](TouchEventArgs&& e){
			const auto& k(e.GetKeys());
			auto& btn(polymorphic_downcast<Button&>(e.GetSender()));
			auto& shl(FetchShell<ShlExplorer>());
			auto& lblTitle(shl.lblTitle);
			auto& lblInfo(shl.lblInfo);

			if(lblTitle.Background)
				lblTitle.Background = nullptr;
			else
				lblTitle.Background = SolidBrush(ColorSpace::White);
			lblInfo.Text = btn.Text + u", " + String(to_string(gfx_init_time))
				+ u";\n" + String(k.to_string());
			Invalidate(lblTitle),
			Invalidate(lblInfo);
		},
		FetchEvent<Click>(btnClose) += [this](TouchEventArgs&&){
			Hide(*this);
		},
		FetchEvent<Click>(btnExit) += [](TouchEventArgs&&){
			YSLib::PostQuitMessage(0);
		}
	);
	SetInvalidationOf(*this);
}


void
ShlExplorer::OnPaint()
{
	// NOTE: Overriding member function %OnInput using %SM_TASK is also valid
	//	because the %SM_INPUT message is sent continuously, but less efficient.
	if(cbFPS.IsTicked())
	{
		using namespace ColorSpace;

		const u32 t(fpsCounter.Refresh());

		if(t != 0)
		{
			auto& g(GetDesktopUp().GetContext());
			yconstexpr Rect r(176, 0, 80, 20);
			char strt[20];

			std::sprintf(strt, "FPS: %u.%03u", t / 1000, t % 1000);
			FillRect(g, r, Blue);
			DrawText(g, r, strt, DefaultMargin, White, false);
			bUpdateUp = true;
		}
	}
}

IWidget*
ShlExplorer::GetBoundControlPtr(const KeyInput& k)
{
	if(k.count() == 1)
	{
		if(k[YCL_KEY(X)])
			return &btnTest;
		if(k[YCL_KEY(A)])
			return &btnOK;
	}
	return nullptr;
}

void
ShlExplorer::OnClick_ShowWindow(TouchEventArgs&&)
{
	const auto& pWnd(FetchShell<ShlExplorer>().pWndExtra);

	YAssert(bool(pWnd), "Null pointer found.");

	SwitchVisible(*pWnd);
}

YSL_END_NAMESPACE(YReader)

