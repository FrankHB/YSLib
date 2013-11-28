/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlExplorer.cpp
\ingroup YReader
\brief 文件浏览器。
\version r1037
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:10:49 +0800
\par 修改时间:
	2013-11-26 20:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ShlExplorer
*/


#include "ShlExplorer.h"
#include "ShlReader.h"

namespace YReader
{

using ystdex::polymorphic_downcast;

namespace
{

//! \since build 410
//@{
using namespace IO;

enum class FileCategory
{
	Empty = NodeCategory::Empty,
	Unknown = NodeCategory::Normal,
	Binary,
	Text
};

bool
CheckTextFileExtensions(string ext)
{
	for(auto& c : ext)
		c = std::tolower(c);
	try
	{
		const auto& m(FetchMIMEBiMapping().GetExtensionMap());
		const auto i(m.find(ext));

		// TODO: Compare for multiple values.
		if(i != m.end())
		{
			const auto& pth(i->second);

			return !pth.empty() && (pth.front() == "text"
				|| ystdex::ends_with(pth.back(), "xml"));
		}
	}
	catch(std::out_of_range&)
	{}
	return false;
}

FileCategory
ClassifyFile(const Path& pth)
{
	switch(ClassifyNode(pth))
	{
	case NodeCategory::Normal:
		return CheckTextFileExtensions(GetExtensionOf(pth).GetMBCS(CS_Path))
			? FileCategory::Text : FileCategory::Binary;
	case NodeCategory::Unknown:
		break;
	default:
		return FileCategory::Empty;
	}
	// TODO: Verifying.
	// TODO: Implementation for other categories.
	return FileCategory::Unknown;
}
//@}

bool
CheckReaderEnability(FileBox& fb, CheckBox& hex)
{
	if(fb.IsSelected())
	{
		switch(ClassifyFile(fb.GetPath()))
		{
		case FileCategory::Text:
			return true;
		case FileCategory::Binary:
			return hex.IsTicked();
		default:
			;
		}
	}
	return false;
}


//! \since build 451
//@{
static yconstexpr auto PI = 3.14159265358979323;
static yconstexpr auto PI_2 = PI * 2;
static yconstexpr auto PI_4 = PI * 4;

//! \since build 452
void
DrawStar(Graphics& g, const Rect& bounds, Color c, const Point& pt, SDst r,
	float a, size_t n = 5)
{
	static yconstexpr auto PI_4 = PI * 4;
	vector<Point> pts(n);

	for(size_t i = 0; i < n; ++i)
		pts[i] = {int(-std::cos(PI_4 / n * i + a) * r + pt.X),
			int(std::sin(PI_4 / n * i + a) * r + pt.Y)};
	DrawPolygon(g, bounds, pts.cbegin(), pts.cend(), c);
}
//@}


//! \since build 436
const char TU_Explorer_Main[]{u8R"NPL(root
($type "Panel")($bounds "0 0 256 192")
(lblTitle
	($type "Label")($bounds "16 20 220 22"))
(lblPath
	($type "Label")($bounds "8 48 240 48"))
(lblInfo
	($type "Label")($bounds "8 100 240 64"))
)NPL"};
//! \since build 436
const char TU_Explorer_Sub[]{u8R"NPL(root
($type "Panel")($bounds "0 0 256 192")
(fbMain
	($type "FileBox")($bounds "0 0 256 170"))
(btnOK
	($type "Button")($bounds "170 170 64 22"))
(btnMenu
	($type "Button")($bounds "0 170 72 22"))
(pnlSetting
	($type "Panel")($bounds "10 40 224 100")
	(cbHex
		($type "CheckButton")($bounds "10 60 100 18"))
	(cbFPS
		($type "CheckButton")($bounds "10 80 72 18"))
	(btnPrevBackground
		($type "Button")($bounds "120 60 30 22"))
	(btnNextBackground
		($type "Button")($bounds "164 60 30 22"))
)
(pnlTest1
	($type "Panel")($bounds "10 20 224 144")
	(btnEnterTest
		($type "Button")($bounds "8 32 104 22"))
	(lblDragTest
		($type "Label")($bounds "4 4 104 22"))
	(btnTestEx
		($type "Button")($bounds "116 32 104 22"))
	(btnTestAni
		($type "Button")($bounds "8 64 104 22"))
)
)NPL"};

} // unnamed namespace;



SwitchScreensButton::SwitchScreensButton(ShlDS& shl, const Point& pt)
	: Button({pt, {22, 22}}, 60),
	shell(shl)
{
	yunseq(
	Text = u"％",
	FetchEvent<Click>(*this) += [this](CursorEventArgs&&){
		shell.get().SwapScreens();
	}
	);
}


ShlExplorer::ShlExplorer(const IO::Path& path,
	const shared_ptr<Desktop>& h_dsk_up, const shared_ptr<Desktop>& h_dsk_dn)
	: ShlDS(h_dsk_up, h_dsk_dn),
	dynWgts_Main(FetchWidgetLoader(), TU_Explorer_Main),
	dynWgts_Sub(FetchWidgetLoader(), TU_Explorer_Sub),
	pFrmAbout(make_unique<FrmAbout>()), mhMain(*GetSubDesktopHandle()),
	fpsCounter(std::chrono::milliseconds(500)),
	btnSwitchMain(*this, {234, 170}), btnSwitchSub(*this, {234, 170})
{
	static int up_i(1);
	auto& dsk_m(GetMainDesktop());
	auto& dsk_s(GetSubDesktop());
	auto& node(dynWgts_Main.WidgetNode);
	DeclDynWidget(Panel, root, node)
	DeclDynWidgetNode(Label, lblTitle)
	DeclDynWidgetNode(Label, lblPath)
	DeclDynWidgetNode(Label, lblInfo)
	auto& node_sub(dynWgts_Sub.WidgetNode);
	DeclDynWidget(Panel, root_sub, node_sub)
	DeclDynWidgetN(FileBox, fbMain, node_sub)
	DeclDynWidgetN(Button, btnOK, node_sub)
	DeclDynWidgetN(Button, btnMenu, node_sub)
	auto& node_pnlSetting(node_sub.at("$children").at("pnlSetting"));
	DeclDynWidget(Panel, pnlSetting, node_pnlSetting)
	DeclDynWidgetN(CheckButton, cbHex, node_pnlSetting)
	DeclDynWidgetN(CheckButton, cbFPS, node_pnlSetting)
	DeclDynWidgetN(Button, btnPrevBackground, node_pnlSetting)
	DeclDynWidgetN(Button, btnNextBackground, node_pnlSetting)
	auto& node_pnlTest1(node_sub.at("$children").at("pnlTest1"));
	DeclDynWidget(Panel, pnlTest1, node_pnlTest1)
	DeclDynWidgetN(Label, lblDragTest, node_pnlTest1)
	DeclDynWidgetN(Button, btnEnterTest, node_pnlTest1)
	DeclDynWidgetN(Button, btnTestEx, node_pnlTest1)
	DeclDynWidgetN(Button, btnTestAni, node_pnlTest1)

	p_border.reset(new BorderResizer(pnlTest1, 4));
	p_ChkFPS = &cbFPS;
	dsk_m += root,
	dsk_m.Add(btnSwitchMain, 96),
	dsk_s += root_sub,
	// XXX: NPL script should be used to describe Z-order without re-attaching.
	root_sub -= pnlSetting,
	root_sub -= pnlTest1;
	dsk_s.Add(btnSwitchSub, 96),
	AddWidgetsZ(dsk_s, DefaultWindowZOrder, pnlSetting, pnlTest1, *pFrmAbout);
	fbMain.SetRenderer(make_unique<BufferedRenderer>(true)),
	pnlSetting.SetRenderer(make_unique<BufferedRenderer>()),
	pnlTest1.SetRenderer(make_unique<BufferedRenderer>()),
	cbHex.SetRenderer(make_unique<BufferedRenderer>()),
	SetVisibleOf(pnlSetting, false),
	SetVisibleOf(pnlTest1, false),
	SetVisibleOf(*pFrmAbout, false),
	WrapForSwapScreens(dsk_m, SwapMask),
	WrapForSwapScreens(dsk_s, SwapMask),
	ani.Reset(&pnlTest1),
	yunseq(
	dsk_m.Background = ImageBrush(FetchImage(0)),
	dsk_s.Background = SolidBrush(FetchGUIState().Colors[Styles::Panel]),
	root.Background = nullptr,
	root_sub.Background = nullptr,
	lblTitle.Text = G_APP_NAME,
	lblPath.AutoWrapLine = true, lblPath.Text = String(path),
	lblInfo.AutoWrapLine = true, lblInfo.Text = u"文件列表：请选择一个文件。",
// TODO: Show current working directory properly.
	btnOK.Text = u"确定(A)",
#if YCL_Win32
	btnMenu.Text = u"菜单(P)",
#else
	btnMenu.Text = u"菜单(Start)",
#endif
	cbHex.Text = u"显示十六进制",
	cbFPS.Text = u"显示 FPS",
	pnlSetting.Background = SolidBrush({248, 120, 248}),
	pnlTest1.Background = SolidBrush({248, 248, 120}),
	lblDragTest.HorizontalAlignment = TextAlignment::Left,
//	btnTestEx.Enabled = {},
	btnTestEx.Font.SetStyle(FontStyle::Bold | FontStyle::Italic),
	btnTestEx.Text = u"附加测试",
	btnTestEx.HorizontalAlignment = TextAlignment::Left,
	btnTestEx.VerticalAlignment = TextAlignment::Down,
	btnTestAni.Text = u"开始动画",
	btnEnterTest.Font.SetStyle(FontStyle::Italic),
	btnEnterTest.Text = u"边界测试",
	btnEnterTest.HorizontalAlignment = TextAlignment::Right,
	btnEnterTest.VerticalAlignment = TextAlignment::Up,
	btnPrevBackground.Font.SetStyle(FontStyle::Bold),
	btnPrevBackground.Text = u"<<",
	btnNextBackground.Font.SetStyle(FontStyle::Bold),
	btnNextBackground.Text = u">>",
	fbMain.SetPath(path),
	Enable(btnOK, false),
	Enable(btnPrevBackground, false),
	dsk_s.BoundControlPtr = [&, this](const KeyInput& k)->IWidget*{
		if(k.count() == 1)
		{
			if(k[YCL_KEY(A)])
				return &btnOK;
			if(k[YCL_KEY_Start])
				return &btnMenu;
		}
		return nullptr;
	},
	FetchEvent<KeyUp>(dsk_s) += OnKey_Bound_TouchUp,
	FetchEvent<KeyDown>(dsk_s) += OnKey_Bound_TouchDown,
	FetchEvent<KeyPress>(dsk_s) += [&](KeyEventArgs&& e){
		if(e.GetKeys()[YCL_KEY(X)])
			SwitchVisible(pnlSetting);
	},
	fbMain.GetViewChanged() += [&](UIEventArgs&&){
		lblPath.Text = String(fbMain.GetPath());
		Invalidate(lblPath);
	},
	fbMain.GetSelected() += [&](IndexEventArgs&&){
		Enable(btnOK, CheckReaderEnability(fbMain, cbHex));
	},
	FetchEvent<Click>(btnOK) += [&](CursorEventArgs&&){
		if(fbMain.IsSelected())
		{
			const auto& path(fbMain.GetPath());
		//	const string s(path);
			const auto category(ClassifyFile(path));

			if(category == FileCategory::Text
				|| category == FileCategory::Binary)
			{
				const auto h_up(GetMainDesktopHandle());
				const auto h_dn(GetSubDesktopHandle());
				const bool b(category == FileCategory::Text
					&& !cbHex.IsTicked());

				PostTask([=]{
					ResetDSDesktops(*h_up, *h_dn);
					if(b)
						NowShellTo(ystdex::make_shared<ShlTextReader>(path,
							h_up, h_dn));
					else
						NowShellTo(ystdex::make_shared<ShlHexBrowser>(path,
							h_up, h_dn));
				}, 0xF8);
			}
		}
	},
	FetchEvent<Click>(cbFPS) += [this](CursorEventArgs&&){
		SetInvalidationOf(GetSubDesktop());
	},
	FetchEvent<Click>(cbHex) += [&](CursorEventArgs&&){
		Enable(btnOK, CheckReaderEnability(fbMain, cbHex));
		SetInvalidationOf(GetSubDesktop());
	},
	FetchEvent<Move>(pnlSetting) += [&](UIEventArgs&&){
		lblDragTest.Text = to_string(GetLocationOf(pnlSetting)) + ';';
		Invalidate(lblDragTest);
	},
	FetchEvent<TouchHeld>(pnlSetting) += OnTouchHeld_Dragging,
#if YCL_DS
	FetchEvent<TouchDown>(pnlSetting) += [&](CursorEventArgs&&){
		struct ::mallinfo t(::mallinfo());

		lblInfo.Text = ystdex::sfmt("%d,%d,%d,%d,%d;",
			t.arena, t.ordblks, t.uordblks, t.fordblks, t.keepcost);
		Invalidate(lblInfo);
	},
#endif
	FetchEvent<Click>(pnlSetting) += [&](CursorEventArgs&&){
		yunseq(
		lblDragTest.ForeColor = GenerateRandomColor(),
		lblTitle.ForeColor = GenerateRandomColor()
		);
		Invalidate(pnlSetting);
	},
	FetchEvent<TouchHeld>(pnlTest1) += OnTouchHeld_Dragging,
	FetchEvent<Paint>(pnlTest1) += [&, this](PaintEventArgs&& e){
		auto& g(e.Target);
		const auto& pt(GetLocationOf(pnlTest1));
		auto& r(e.ClipArea);

		DrawStar(g, r, ColorSpace::Red, pt + Point{96, 96}, 48, rad);
		DrawStar(g, r, ColorSpace::Green, pt + Point{96, 96}, 48, rad + PI);
		rad += 0.02;
		if(rad > PI_2)
			rad -= PI_2;
		UpdateClipArea(e, {{}, GetSizeOf(e.GetSender())});
	},
	FetchEvent<Click>(btnTestEx) += [&](CursorEventArgs&& e){
		const auto& k(e.GetKeys());
		auto& btn(polymorphic_downcast<Button&>(e.GetSender()));

		if(lblTitle.Background)
			lblTitle.Background = nullptr;
		else
			lblTitle.Background = SolidBrush(GenerateRandomColor());
		lblInfo.Text = btn.Text + u", " + String(to_string(
			FetchImageLoadTime())) + u";\n" + String(k.to_string());
		Invalidate(lblTitle),
		Invalidate(lblInfo);
	},
	FetchEvent<Click>(btnTestAni) += [&](CursorEventArgs&&){
		auto& conn(ani.GetConnectionRef());

		if(conn.Ready)
			yunseq(btnTestAni.Text = u"开始动画", conn.Ready = {});
		else
		{
			yunseq(btnTestAni.Text = u"停止动画", conn.Ready = true),
			ani.Start();
		}
		Invalidate(btnTestAni);
	},
	FetchEvent<Enter>(btnEnterTest) += [](CursorEventArgs&& e){
		auto& btn(ystdex::polymorphic_downcast<Button&>(e.GetSender()));

		btn.Text = u"Enter: " + String(to_string(e));
		Invalidate(btn);
	},
	FetchEvent<Leave>(btnEnterTest) += [](CursorEventArgs&& e){
		auto& btn(ystdex::polymorphic_downcast<Button&>(e.GetSender()));

		btn.Text = u"Leave: " + String(to_string(e));
		Invalidate(btn);
	},
	mhMain.Roots[&btnMenu] = 1u,
	FetchEvent<Click>(btnMenu) += [this](CursorEventArgs&&){
		auto& mnu(mhMain[1u]);

		if(mhMain.IsShowing(1u))
		{
			mhMain.HideAll();
			mnu.ClearSelected();
		}
		else
			mhMain.Show(1u);
		Invalidate(mnu);
	},
	FetchEvent<Click>(btnPrevBackground) += [&](CursorEventArgs&&){
		auto& dsk_m(GetMainDesktop());
		auto& dsk_s(GetSubDesktop());

		if(up_i > 0)
		{
			--up_i;
			Enable(btnNextBackground);
		}
		if(up_i == 0)
			Enable(btnPrevBackground, false);
		dsk_m.Background = ImageBrush(FetchImage(up_i));
		SetInvalidationOf(dsk_m),
		SetInvalidationOf(dsk_s);
	},
	FetchEvent<Click>(btnNextBackground) += [&](CursorEventArgs&&){
		auto& dsk_m(GetMainDesktop());
		auto& dsk_s(GetSubDesktop());

		if(size_t(up_i + 1) < Image_N)
		{
			++up_i;
			Enable(btnPrevBackground);
		}
		if(size_t(up_i + 1) == Image_N)
			Enable(btnNextBackground, false);
		dsk_m.Background = ImageBrush(FetchImage(up_i));
		SetInvalidationOf(dsk_m),
		SetInvalidationOf(dsk_s);
	}
	);
	RequestFocusCascade(fbMain),
	SetInvalidationOf(dsk_m),
	SetInvalidationOf(dsk_s);

	auto& m1(*(ynew Menu({}, share_raw(
		new TextList::ListType{u"测试", u"关于", u"设置(X)", u"退出"}), 1u)));
	auto& m2(*(ynew Menu({},
		share_raw(new TextList::ListType{u"项目1", u"项目2"}), 2u)));

	m1.GetConfirmed() += [&](IndexEventArgs&& e){
		switch(e.Value)
		{
		case 1U:
			Show(*pFrmAbout);
			break;
		case 2U:
			SwitchVisible(pnlSetting);
			break;
		case 3U:
			YSLib::PostQuitMessage(0);
		}
	},
	m2.GetConfirmed() += [&](IndexEventArgs&& e){
#if YCL_Win32
		MinGW32::TestFramework(e.Value);
#endif
		if(e.Value == 0)
			SwitchVisible(pnlTest1);
	},
	mhMain += m1, mhMain += m2,
	m1 += make_pair(0u, &m2);
	ResizeForContent(m1), ResizeForContent(m2),
	SetLocationOf(m1, Point(btnMenu.GetX(), btnMenu.GetY() - m1.GetHeight()));
	//m1.SetWidth(btnMenu.GetWidth() + 20);
}

void
ShlExplorer::OnPaint()
{
	// NOTE: Hack for performance.
//	DeclDynWidgetN(CheckButton, cbFPS, dynWgts_Sub.WidgetNode, "pnlSetting")
	auto& cbFPS(*p_ChkFPS);

	// NOTE: Overriding member function %OnInput using %SM_TASK is also valid
	//	because the %SM_Input message is sent continuously, but less efficient.
	if(cbFPS.IsTicked())
	{
		using namespace ColorSpace;

		const u32 t(fpsCounter.Refresh());

		if(t != 0)
		{
			auto& g(ystdex::polymorphic_downcast<BufferedRenderer&>(
				GetMainDesktop().GetRenderer()).GetContext());
			yconstexpr Rect r(176, 0, 80, 20);
			char strt[20];

			std::sprintf(strt, "FPS: %u.%03u", t / 1000, t % 1000);
			FillRect(g, r, Blue);
			DrawText(g, r, strt, DefaultMargin, White, false);
			bUpdateUp = true;
		}
	}
}

} // namespace YReader;

