/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlExplorer.cpp
\ingroup YReader
\brief 文件浏览器。
\version r1489
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:10:49 +0800
\par 修改时间:
	2015-09-18 14:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ShlExplorer
*/


#include "ShlExplorer.h"
#include "ShlReader.h"
#include YFM_YSLib_UI_ExStyle
#include <ystdex/functional.hpp> // for ystdex::bind1;

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
	Empty = yimpl(int)(NodeCategory::Empty),
	Unknown = yimpl(int)(NodeCategory::Regular),
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
	CatchIgnore(std::out_of_range&)
	return {};
}

FileCategory
ClassifyFile(const Path& pth)
{
	switch(ClassifyNode(pth))
	{
	case NodeCategory::Regular:
		return CheckTextFileExtensions(GetExtensionOf(pth).GetMBCS())
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

//! \since build 480
bool
CheckReaderEnability(FileBox& fb, RadioBox& hex)
{
	if(fb.IsSelected())
	{
		switch(ClassifyFile(fb.GetPath()))
		{
		case FileCategory::Text:
			return true;
		case FileCategory::Binary:
			return hex.GetState() == &hex;
		default:
			;
		}
	}
	return {};
}


//! \since build 487
inline bool
CheckMenuKey(const KeyInput& k)
{
#if YCL_Win32
	auto ke(k);

	unseq_apply([&](int vk) ynothrow{
		ke.set(size_t(vk), {});
	}, VK_CONTROL, VK_LCONTROL, VK_RCONTROL);
	return ke.none() && k[VK_CONTROL];
#else
	return k.count() == 1 && k[YCL_KEY_Start];
#endif
}


//! \since build 585
//@{
yconstexpr const auto PI = 3.141592F;
yconstexpr const auto PI_2 = PI * 2;

//! \since build 452
void
DrawStar(Graphics& g, const Rect& bounds, Color c, const Point& pt, SDst r,
	float a, size_t n = 5)
{
	static yconstexpr const auto PI_4 = PI * 4;
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
	($type "Panel")($bounds "10 40 224 100")($z 128)
	(ddlStyle
		($type "DropDownList")($bounds "10 24 80 22"))
	(rbTxt
		($type "RadioButton")($bounds "120 18 100 18"))
	(rbHex
		($type "RadioButton")($bounds "120 36 100 18"))
	(cbFPS
		($type "CheckButton")($bounds "10 60 72 18"))
	(btnPrevBackground
		($type "Button")($bounds "120 60 30 22"))
	(btnNextBackground
		($type "Button")($bounds "164 60 30 22"))
)
(pnlTest1
	($type "Panel")($bounds "10 20 224 144")($z 128)
	(tcTest1
		($type "TabControl")
		($bounds "3 3 218 138")
		(pnlPage1
			($type "Panel")
			(btnEnterTest
				($type "Button")($bounds "8 32 104 22"))
			(lblDragTest
				($type "Label")($bounds "4 4 104 22"))
		)
		(pnlPage2
			($type "Panel")
			(btnTestAni
				($type "Button")($bounds "8 32 104 22"))
		)
		(pnlPage3
			($type "Panel")
			(tbTest
				($type "TextBox")($bounds "8 4 104 22"))
			(btnTestEx
				($type "Button")($bounds "8 32 104 22"))
			(cbDisableSetting
				($type "CheckButton")($bounds "8 64 104 22"))
			(cbShowTextBoxContent
				($type "CheckButton")($bounds "8 88 104 22"))
			(tvNodes
				($type "TreeView")($bounds "116 4 100 108"))
		)
	)
)
)NPL"};

} // unnamed namespace;


IO::Path
FetchDefaultShellDirectory()
{
#if YCL_Android
	return
		Path(AccessChild<string>(FetchRoot()["YFramework"], "DataDirectory"));
#else
	return Path(IO::FetchCurrentWorkingDirectory());
#endif
}

SwitchScreensButton::SwitchScreensButton(ShlDS& shl, const Point& pt)
	: Button({pt, {22, 22}}, 60),
	shell(shl)
{
	yunseq(
	Text = u"％",
	FetchEvent<Click>(*this) += [this]{
		shell.get().SwapScreens();
	}
	);
}
ImplDeDtor(SwitchScreensButton)


ShlExplorer::ShlExplorer(const IO::Path& path,
	const shared_ptr<Desktop>& h_dsk_up, const shared_ptr<Desktop>& h_dsk_dn)
	: ShlDS(h_dsk_up, h_dsk_dn),
	dynWgts_Main(FetchWidgetLoader(), TU_Explorer_Main),
	dynWgts_Sub(FetchWidgetLoader(), TU_Explorer_Sub),
	pFrmAbout(make_unique<FrmAbout>()),
	fpsCounter(std::chrono::milliseconds(500)),
	btnSwitchMain(*this, {234, 170}), btnSwitchSub(*this, {234, 170})
{
	using namespace std;
	using namespace placeholders;
	using ystdex::bind1;
	static struct Init
	{
		Init()
		{
			YTraceDe(Debug, "First time to load extra styles.");
			Styles::InitExStyles();
		}
	} init;
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
	auto& node_pnlSetting(AccessWidgetNode(node_sub, "pnlSetting"));
	DeclDynWidget(Panel, pnlSetting, node_pnlSetting)
	DeclDynWidgetN(DropDownList, ddlStyle, node_pnlSetting)
	DeclDynWidgetN(RadioButton, rbTxt, node_pnlSetting)
	DeclDynWidgetN(RadioButton, rbHex, node_pnlSetting)
	DeclDynWidgetN(CheckButton, cbFPS, node_pnlSetting)
	DeclDynWidgetN(Button, btnPrevBackground, node_pnlSetting)
	DeclDynWidgetN(Button, btnNextBackground, node_pnlSetting)
	auto& node_pnlTest1(AccessWidgetNode(node_sub, "pnlTest1"));
	auto& node_tcTest1(AccessWidgetNode(node_pnlTest1, "tcTest1"));
	DeclDynWidget(Panel, pnlTest1, node_pnlTest1)
	DeclDynWidgetN(TabControl, tcTest1, node_pnlTest1)
	auto& node_pnlPage1(AccessWidgetNode(node_tcTest1, "pnlPage1"));
	auto& node_pnlPage2(AccessWidgetNode(node_tcTest1, "pnlPage2"));
	auto& node_pnlPage3(AccessWidgetNode(node_tcTest1, "pnlPage3"));
	DeclDynWidget(Panel, pnlPage2, node_pnlPage2)
	DeclDynWidgetN(Label, lblDragTest, node_pnlPage1)
	DeclDynWidgetN(Button, btnEnterTest, node_pnlPage1)
	DeclDynWidgetN(Button, btnTestAni, node_pnlPage2)
	DeclDynWidgetN(TextBox, tbTest, node_pnlPage3)
	DeclDynWidgetN(Button, btnTestEx, node_pnlPage3)
	DeclDynWidgetN(CheckButton, cbDisableSetting, node_pnlPage3)
	DeclDynWidgetN(CheckButton, cbShowTextBoxContent, node_pnlPage3)

	YTraceDe(Debug, "Initialization of ShlExplorer began.");
	{
		DeclDynWidgetN(TreeView, tvNodes, node_pnlPage3)

		tvNodes.GetExtractText() = [](const ValueNode& nd) -> String{
			TryRet(TreeList::DefaultExtractText(nd))
			CatchIgnore(ystdex::bad_any_cast&)
			return "<NONE>";
		};
		tvNodes.GetTreeRootRef() = FetchRoot();
		tvNodes.BindView();
	}
	AddButtonToTabBar(tcTest1, node_pnlTest1, "btnTab1", u"基本测试");
	AddButtonToTabBar(tcTest1, node_pnlTest1, "btnTab2", u"动画测试");
	AddButtonToTabBar(tcTest1, node_pnlTest1, "btnTab3", u"附加测试");
	tcTest1.UpdateTabPages();
	p_border.reset(new BorderResizer(pnlTest1, 4)),
	p_ChkFPS = &cbFPS,
	tpDefault.Text = u"请输入文本",
	tpDefault.BindByFocus(tbTest, bind(&TextPlaceholder::SwapTextBox<TextBox>,
		_1, _2));
	tpDefault.Font.SetStyle(FontStyle::Italic);
	dsk_m += root,
	dsk_m.Add(btnSwitchMain, 96),
	dsk_s += root_sub,
	// XXX: NPL script should be used to describe Z-order variable.
	dsk_s.Add(btnSwitchSub, 96),
	AddWidgetsZ(root_sub, DefaultWindowZOrder, *pFrmAbout);
	fbMain.SetRenderer(make_unique<BufferedRenderer>(true)),
	pnlSetting.SetRenderer(make_unique<BufferedRenderer>()),
	pnlTest1.SetRenderer(make_unique<BufferedRenderer>()),
	unseq_apply(bind1(SetVisibleOf, false), pnlSetting, pnlTest1, *pFrmAbout),
	unseq_apply(bind(&ShlDS::WrapForSwapScreens, this, _1, ref(SwapMask)),
		dsk_m, dsk_s),
	ani.Reset(&pnlTest1),
	ddlStyle.SetList(FetchVisualStyleNames()),
	rbTxt.ShareTo(rbHex),
	rbTxt.Select(),
	unseq_apply(bind1(Enable, false), btnOK, btnPrevBackground),
	yunseq(
	dsk_m.Background = ImageBrush(FetchImage(0)),
	dsk_s.Background
		= SolidBrush(FetchGUIConfiguration().Colors[Styles::Panel]),
	root.Background = nullptr,
	root_sub.Background = nullptr,
	lblTitle.Text = G_APP_NAME,
	lblPath.AutoWrapLine = true, lblPath.Text = String(path),
	lblInfo.AutoWrapLine = true, lblInfo.Text = u"文件列表：请选择一个文件。",
// TODO: Show current working directory properly.
	btnOK.Text = u"确定(A)",
#if YCL_Win32
	btnMenu.Text = u"菜单(Ctrl)",
#else
	btnMenu.Text = u"菜单(Start)",
#endif
	tbTest.Text = u"测试文本框输入字符串",
	tbTest.MaskChar = u'●',
	tbTest.MaxLength = 8,
	ddlStyle.Text = [](const TextList::ListType& lst){
		const auto& name(FetchGUIConfiguration().Styles.GetCurrent()->first);

		YAssert(!lst.empty(), "Invalid list found.");
		return name.empty() ? lst[0] : String(name);
	}(ddlStyle.GetList()),
	rbTxt.Text = u"文本阅读",
	rbHex.Text = u"十六进制浏览",
	cbFPS.Text = u"显示 FPS",
	pnlSetting.Background = SolidBrush({160, 252, 160}),
	pnlTest1.Background = SolidBrush({248, 248, 120}),
	lblDragTest.HorizontalAlignment = TextAlignment::Left,
//	btnTestEx.Enabled = {},
	btnTestEx.Font.SetStyle(FontStyle::Bold | FontStyle::Italic),
	btnTestEx.Text = u"附加测试",
	btnTestEx.HorizontalAlignment = TextAlignment::Left,
	btnTestEx.VerticalAlignment = TextAlignment::Down,
	btnTestAni.Text = u"开始动画",
	cbDisableSetting.Text = u"禁用设置选择框",
	cbShowTextBoxContent.Text = u"显示文本框内容",
	lblDragTest.Text = u"移动设置面板位置",
	btnEnterTest.Font.SetStyle(FontStyle::Italic),
	btnEnterTest.Text = u"边界测试",
	btnEnterTest.HorizontalAlignment = TextAlignment::Right,
	btnEnterTest.VerticalAlignment = TextAlignment::Up,
	btnPrevBackground.Font.SetStyle(FontStyle::Bold),
	btnPrevBackground.Text = u"<<",
	btnNextBackground.Font.SetStyle(FontStyle::Bold),
	btnNextBackground.Text = u">>",
	fbMain.SetPath(path),
	dsk_s.BoundControlPtr = [&, this](const KeyInput& k)->IWidget*{
		if(k.count() == 1)
		{
			if(k[YCL_KEY(A)])
				return &btnOK;
		}
		else if(CheckMenuKey(k))
			return &btnMenu;
		return {};
	},
	FetchEvent<KeyUp>(dsk_s) += OnKey_Bound_TouchUp,
	FetchEvent<KeyDown>(dsk_s) += OnKey_Bound_TouchDown,
	FetchEvent<KeyDown>(dsk_s) += [&](KeyEventArgs&& e){
		if(e.Strategy != RoutedEventArgs::Bubble && e.GetKeys()[YCL_KEY(X)])
		{
			SwitchVisibleToFront(pnlSetting);
			RequestFocusCascade(pnlSetting);
		}
	},
	// TODO: Improve UI action when %ViewChanged being called?
	fbMain.GetSelected() += [&]{
		lblPath.Text = String(fbMain.GetPath()),
		Invalidate(lblPath),
		Enable(btnOK, CheckReaderEnability(fbMain, rbHex));
	},
	FetchEvent<Click>(btnOK) += [&]{
		if(fbMain.IsSelected())
		{
			const auto& fb_path(fbMain.GetPath());
		//	const string s(fb_path);
			const auto category(ClassifyFile(fb_path));

			if(category == FileCategory::Text
				|| category == FileCategory::Binary)
			{
				const auto h_up(GetMainDesktopHandle());
				const auto h_dn(GetSubDesktopHandle());
				const bool b(category == FileCategory::Text
					&& rbTxt.IsSelected());

				PostTask([=]{
					ResetDSDesktops(*h_up, *h_dn);
					NowShellTo(b ? shared_ptr<Shell>(make_shared<ShlTextReader>(
						fb_path, h_up, h_dn)) : shared_ptr<Shell>(
						make_shared<ShlHexBrowser>(fb_path, h_up, h_dn)));
				}, 0xF8);
			}
		}
	},
	FetchEvent<Click>(cbFPS) += [this]{
		SetInvalidationOf(GetSubDesktop());
	},
	FetchEvent<Click>(rbHex) += [&]{
		Enable(btnOK, CheckReaderEnability(fbMain, rbHex));
		SetInvalidationOf(GetSubDesktop());
	},
	FetchEvent<Move>(pnlSetting) += [&]{
		lblDragTest.Text = to_string(GetLocationOf(pnlSetting)) + ';';
		Invalidate(lblDragTest);
	},
	FetchEvent<TouchHeld>(pnlSetting) += OnTouchHeld_Dragging,
#if YCL_DS
	FetchEvent<TouchDown>(pnlSetting) += [&]{
		struct ::mallinfo t(::mallinfo());

		lblInfo.Text = sfmt("%d,%d,%d,%d,%d;", t.arena, t.ordblks, t.uordblks,
			t.fordblks, t.keepcost);
		Invalidate(lblInfo);
	},
#endif
	FetchEvent<Click>(pnlSetting) += [&]{
		yunseq(
		lblDragTest.ForeColor = GenerateRandomColor(),
		lblTitle.ForeColor = GenerateRandomColor()
		);
		Invalidate(pnlSetting);
	},
	FetchEvent<TouchHeld>(pnlTest1) += OnTouchHeld_Dragging,
	FetchEvent<Paint>(pnlPage2) += [&, this](PaintEventArgs&& e){
		auto& g(e.Target);
		const auto& pt(GetLocationOf(pnlTest1));
		auto& bounds(e.ClipArea);

		DrawStar(g, bounds, ColorSpace::Red, pt + Point{96, 96}, 48, rad);
		DrawStar(g, bounds, ColorSpace::Green, pt + Point{96, 96}, 48,
			rad + PI);
		if(ani.GetConnectionRef().Ready)
		{
			rad += 0.02F;
			if(rad > PI_2)
				rad -= PI_2;
		}
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
		InvalidateWidgets(lblTitle, lblInfo);
	},
	FetchEvent<Click>(btnTestAni) += [&]{
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

		btn.Text = u"Enter: " + String(to_string(e.Position));
		Invalidate(btn);
	},
	FetchEvent<Leave>(btnEnterTest) += [&](CursorEventArgs&& e){
		btnEnterTest.Text = u"Leave: " + String(to_string(e.Position));
		Invalidate(btnEnterTest);
	},
	FetchEvent<Click>(btnMenu) += [this]{
		auto& mnu(*p_m0);

		if(mhMain.IsShowing(mnu))
		{
			mhMain.HideAll();
			mnu.ClearSelected();
		}
		else
			mhMain.Show(mnu);
		Invalidate(mnu);
	},
	FetchEvent<Click>(btnPrevBackground) += [&]{
		if(up_i > 0)
		{
			--up_i;
			Enable(btnNextBackground);
		}
		if(up_i == 0)
			Enable(btnPrevBackground, false);
		GetMainDesktop().Background = ImageBrush(FetchImage(size_t(up_i)));
		unseq_apply(SetInvalidationOf, GetMainDesktop(), GetSubDesktop());
	},
	FetchEvent<Click>(btnNextBackground) += [&]{
		if(size_t(up_i + 1) < Image_N)
		{
			++up_i;
			Enable(btnPrevBackground);
		}
		if(size_t(up_i + 1) == Image_N)
			Enable(btnNextBackground, false);
		GetMainDesktop().Background = ImageBrush(FetchImage(size_t(up_i)));
		SetInvalidationOf(GetMainDesktop()),
		SetInvalidationOf(GetSubDesktop());
	},
	cbDisableSetting.Ticked += [&](CheckBox::TickedArgs&& e){
		unseq_apply(bind1(SetEnabledOf, !e), cbFPS, rbTxt, rbHex);
		InvalidateWidgets(cbFPS, rbTxt, rbHex);
	},
	cbShowTextBoxContent.Ticked += [&](CheckBox::TickedArgs&& e){
		(tpDefault.GetCapturedPtr() == &tbTest ? tpDefault.MaskChar
			: tbTest.MaskChar) = e ? ucs4_t() : u'●';
		Invalidate(tbTest);
	},
	ddlStyle.GetConfirmed() += [&, this]{
		FetchGUIConfiguration().Styles.Switch(ddlStyle.Text.GetMBCS());
		InvalidateWidgets(dsk_m, dsk_s);
	}
	);
	RequestFocusCascade(fbMain),
	unseq_apply(SetInvalidationOf, dsk_m, dsk_s);

	{
		p_m0.reset(new Menu({}, make_shared<Menu::ListType, String>(
			{u"测试", u"关于", u"设置(X)", u"退出"})));
		p_m1.reset(new Menu({},
			make_shared<Menu::ListType, String>({u"项目1", u"项目2"})));
		auto& m0(*p_m0);
		auto& m1(*p_m1);

		m0.Confirmed += [&](IndexEventArgs&& e){
			switch(e.Value)
			{
			case 1U:
				Show(*pFrmAbout);
				break;
			case 2U:
				SwitchVisibleToFront(pnlSetting);
				break;
			case 3U:
				YSLib::PostQuitMessage(0);
			}
		},
		m1.Confirmed += [&](IndexEventArgs&& e){
#if YCL_Win32
			Windows::TestFramework(e.Value);
#endif
			if(e.Value == 0)
				SwitchVisibleToFront(pnlTest1);
		},
		AddWidgetsZ(GetSubDesktop(), DefaultMenuZOrder, m0, m1),
		mhMain += m0, mhMain += m1,
		m0 += {0u, &m1};
		mhMain.Roots[&btnMenu] = &m0;
		unseq_apply(ResizeForContent, m0, m1),
		// XXX: Conversion to 'SPos' might be implementation-defined.
		SetLocationOf(m0,
			Point(btnMenu.GetX(), btnMenu.GetY() - SPos(m0.GetHeight())));
		//m0.SetWidth(btnMenu.GetWidth() + 20);
	}
	YTraceDe(Debug, "Initialization of ShlExplorer ended.");
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
		const auto t(fpsCounter.Refresh());

		if(t != 0)
		{
			const auto g(ystdex::polymorphic_downcast<BufferedRenderer&>(
				GetMainDesktop().GetRenderer()).GetContext());
			static yconstexpr const Rect r(176, 0, 80, 20);
			char strt[20];

			std::sprintf(strt, "FPS: %u.%03u", unsigned(t / 1000),
				unsigned(t % 1000));
			FillRect(g, r, Blue);
			DrawText(g, r, strt, DefaultMargin, White, false);
			bUpdateUp = true;
		}
	}
}

} // namespace YReader;

