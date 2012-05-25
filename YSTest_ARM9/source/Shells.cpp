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
\version r6429;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2012-05-25 18:48 +0800;
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
		~(x * y) >> 2,
		x | y | 128,
		240 - ((x & y) >> 1)
	);
}

void
dfap(BitmapPtr buf, SDst x, SDst y)
{
	//bza1BRGx
	buf[y * MainScreenWidth + x] = Color(
		(x << 4) / (y | 1),
		(x | y << 1) % (y + 2),
		(~y | x << 1) % 27 + 3
	);
}

void
dfac1(BitmapPtr buf, SDst x, SDst y)
{
	//fl1RBGx
	buf[y * MainScreenWidth + x] = Color(
		x + y * y,
		(x & y) ^ (x | y),
		x * x + y
	);
}

void
dfac1p(BitmapPtr buf, SDst x, SDst y)
{
	//rz3GRBx
	buf[y * MainScreenWidth + x] = Color(
		(x * y) | x,
		(x * y) | y,
		(x ^ y) * (x ^ y)
	);
}

void
dfac2(BitmapPtr buf, SDst x, SDst y)
{
	//v1BGRx1
	buf[y * MainScreenWidth + x] = Color(
		(x << 4) / ((y & 1) | 1),
		~x % 101 + y,
		(x + y) % (((y - 2) & 1) | 129) + (x << 2)
	);
}

void
dfac2p(BitmapPtr buf, SDst x, SDst y)
{
	//arz1x
	buf[y * MainScreenWidth + x] = Color(
		(x | y) % (y + 2),
		(~y | x) % 27 + 3,
		(x << 6) / (y | 1)
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
	if(*path.rbegin() == YCL_PATH_DELIMITER)
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

} // unnamed namespace;


ShlExplorer::ShlExplorer(const IO::Path& path)
	: ShlDS(),
	lblTitle(Rect(16, 20, 220, 22)), lblPath(Rect(8, 48, 240, 48)),
	lblInfo(Rect(8, 100, 240, 64)), fbMain(Rect(4, 6, 248, 128)),
	btnTest(Rect(115, 165, 65, 22)), btnOK(Rect(185, 165, 65, 22)),
	pnlSetting(Rect(10, 40, 228, 100)), cbHex(Rect(142, 142, 103, 18)),
	cbFPS(Rect(10, 62, 73, 18)), btnEnterTest(Rect(4, 4, 146, 22)),
	btnMenuTest(Rect(152, 32, 60, 22)), btnShowWindow(Rect(8, 32, 104, 22)),
	btnPrevBackground(Rect(95, 64, 30, 22)),
	btnNextBackground(Rect(145, 64, 30, 22)),
	pWndExtra(make_unique<TFormExtra>()), mhMain(*GetDesktopDownHandle()),
	fpsCounter(500000000ULL)
{
	static int up_i(1);
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	AddWidgets(pnlSetting, cbFPS, btnEnterTest, btnMenuTest, btnShowWindow,
		btnPrevBackground, btnNextBackground),
	AddWidgets(dsk_up, lblTitle, lblPath, lblInfo),
	AddWidgets(dsk_dn, fbMain, btnTest, btnOK, cbHex),
	AddWidgetsZ(dsk_dn, DefaultWindowZOrder, pnlSetting, *pWndExtra),
	//启用缓存。
	fbMain.SetRenderer(make_unique<BufferedRenderer>(true)),
	pnlSetting.SetRenderer(make_unique<BufferedRenderer>()),
	SetVisibleOf(pnlSetting, false),
	SetVisibleOf(*pWndExtra, false),
	cbFPS.SetTransparent(true),
	Enable(btnPrevBackground, false),
	yunseq(
		dsk_up.Background = ImageBrush(FetchImage(1)),
		dsk_dn.Background = ImageBrush(FetchImage(2)),
		lblTitle.Text = u"YReader",
		lblPath.AutoWrapLine = true, lblPath.Text = path,
		lblInfo.AutoWrapLine = true, lblInfo.Text = u"文件列表：请选择一个文件。",
	// TODO: show current working directory properly;
		btnTest.Text = u"设置(X)",
		btnOK.Text = u"确定(A)",
		cbHex.Text = u"显示十六进制",
		cbFPS.Text = u"显示 FPS",
		pnlSetting.Background = SolidBrush(Color(248, 248, 120)),
		btnEnterTest.Text = u"边界测试",
		btnEnterTest.HorizontalAlignment = TextAlignment::Right,
		btnEnterTest.VerticalAlignment = TextAlignment::Up,
		btnMenuTest.Text = u"菜单测试",
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
					// TODO: use g++ 4.7 later;
					//	SetShellTo(make_shared<ShlTextReader>(path));
						SetShellTo(share_raw(new ShlTextReader(path)));
					//	SetShellToNew<ShlTextReader>(path);
					else
					// TODO: use g++ 4.7 later;
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
		FetchEvent<Click>(btnMenuTest) +=[this](TouchEventArgs&&){
			static int t;

			auto& mnu(mhMain[1u]);
			auto& lst(mnu.GetList());

			if(mhMain.IsShowing(1u))
			{
				if(lst.size() > 4)
					lst.clear();
				lst.push_back("TMI" + to_string(t));
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
			SetInvalidationOf(dsk_up),
			SetInvalidationOf(dsk_dn);
		}
	);
	RequestFocusCascade(fbMain),
	SetInvalidationOf(dsk_up),
	SetInvalidationOf(dsk_dn);
	// FIXME: potential memory leaks;
	mhMain += *(ynew Menu(Rect::Empty, GenerateList(u"A:MenuItem"), 1u)),
	mhMain += *(ynew Menu(Rect::Empty, GenerateList(u"B:MenuItem"), 2u));
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
		tie(btnDragTest.Text, btnDragTest.HorizontalAlignment, btnTestEx.Text,
			btnClose.Text, btnExit.Text, Background, btnClose.Background)
			= forward_as_tuple(u"测试拖放控件", TextAlignment::Left,
			u"附加测试", u"关闭", u"退出",
			SolidBrush(Color(248, 120, 120)), SolidBrush(Color(176, 184, 192))),
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
#ifndef YCL_MINGW32
			char strMemory[40];
			struct mallinfo t(mallinfo());

			std::sprintf(strMemory, "%d,%d,%d,%d,%d;",
				t.arena, t.ordblks, t.uordblks, t.fordblks, t.keepcost);
			auto& lblInfo(FetchShell<ShlExplorer>().lblInfo);

			lblInfo.Text = strMemory;
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

			lblTitle.SetTransparent(!lblTitle.IsTransparent()),
			lblInfo.Text = btn.Text + u";\n" + String(k.to_string());
			Invalidate(lblTitle),
			Invalidate(lblInfo);
		},
		FetchEvent<Click>(btnClose) += [this](TouchEventArgs&&){
			Hide(*this);
		},
		FetchEvent<Click>(btnExit) += [](TouchEventArgs&&){
			YSL_ PostQuitMessage(0);
		}
	);
	SetInvalidationOf(*this);
}


void
ShlExplorer::OnPaint()
{
	// NOTE: overwriting member function OnInput using SM_TASK is also valid due
	//	to the SM_INPUT message is sent continuously, but with less efficiency.
	auto& dsk_dn(GetDesktopDown());

	if(cbFPS.IsTicked())
	{
		using namespace ColorSpace;

		const u32 t(fpsCounter.Refresh());

		if(t != 0)
		{
			auto& g(dsk_dn.GetContext());
			yconstexpr Rect r(0, 172, 80, 20);
			char strt[20];

			std::sprintf(strt, "FPS: %u.%03u", t / 1000, t % 1000);
			FillRect(g, r, Blue);
			DrawText(g, r, strt, DefaultMargin, White, false);
			bUpdateDown = true;
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
	auto& pWnd(FetchShell<ShlExplorer>().pWndExtra);

	YAssert(bool(pWnd), "Null pointer found.");

	SwitchVisible(*pWnd);
}

YSL_END_NAMESPACE(YReader)

