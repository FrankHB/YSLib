/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlExplorer.cpp
\ingroup YReader
\brief 文件浏览器。
\version r592
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:10:49 +0800
\par 修改时间:
	2013-07-07 05:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ShlExplorer
*/


#include "ShlExplorer.h"
#include "ShlReader.h"

YSL_BEGIN_NAMESPACE(YReader)

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
	static yconstexpr const char* exts[]{
		"txt", "c", "cpp", "h", "hpp", "ini", "xml"};

	for(auto& c : ext)
		c = std::tolower(c);
	return std::any_of(exts, exts + arrlen(exts), [&](const char* str){
		return std::strcmp(ext.c_str(), str) == 0;
	});
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

} // unnamed namespace;


ShlExplorer::ShlExplorer(const IO::Path& path,
	const shared_ptr<Desktop>& h_dsk_up, const shared_ptr<Desktop>& h_dsk_dn)
	: ShlDS(h_dsk_up, h_dsk_dn),
	lblTitle({16, 20, 220, 22}), lblPath({8, 48, 240, 48}),
	lblInfo({8, 100, 240, 64}), fbMain({0, 0, 256, 170}),
	btnOK({192, 170, 64, 22}), btnMenu({0, 170, 72, 22}),
	pnlSetting({10, 40, 224, 100}), cbHex({10, 60, 100, 18}),
	cbFPS({10, 80, 72, 18}), lblDragTest({4, 4, 104, 22}),
	btnEnterTest({8, 32, 104, 22}), btnTestEx({116, 32, 104, 22}),
	btnPrevBackground({120, 60, 30, 22}),
	btnNextBackground({164, 60, 30, 22}),
	pFrmAbout(make_unique<FrmAbout>()), mhMain(*GetDesktopDownHandle()),
	fpsCounter(std::chrono::milliseconds(500))
{
	static int up_i(1);
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	AddWidgets(pnlSetting, cbHex, cbFPS, btnEnterTest, lblDragTest, btnTestEx,
		btnPrevBackground, btnNextBackground),
	AddWidgets(dsk_up, lblTitle, lblPath, lblInfo),
	AddWidgets(dsk_dn, fbMain, btnOK, btnMenu),
	AddWidgetsZ(dsk_dn, DefaultWindowZOrder, pnlSetting, *pFrmAbout),
	//启用缓存。
	fbMain.SetRenderer(make_unique<BufferedRenderer>(true)),
	pnlSetting.SetRenderer(make_unique<BufferedRenderer>()),
	cbHex.SetRenderer(make_unique<BufferedRenderer>()),
	SetVisibleOf(pnlSetting, false),
	SetVisibleOf(*pFrmAbout, false),
	yunseq(
		dsk_up.Background = ImageBrush(FetchImage(0)),
		dsk_dn.Background = SolidBrush(
			FetchGUIState().Colors[Styles::Panel]),
		lblTitle.Text = G_APP_NAME,
		lblPath.AutoWrapLine = true, lblPath.Text = String(path),
		lblInfo.AutoWrapLine = true, lblInfo.Text = u"文件列表：请选择一个文件。",
	// TODO: Show current working directory properly.
		btnOK.Text = u"确定(A)",
#if YCL_MINGW32
		btnMenu.Text = u"菜单(P)",
#else
		btnMenu.Text = u"菜单(Start)",
#endif
		cbHex.Text = u"显示十六进制",
		cbFPS.Text = u"显示 FPS",
		pnlSetting.Background = SolidBrush(Color(248, 248, 120)),
		lblDragTest.HorizontalAlignment = TextAlignment::Left,
	//	btnTestEx.Enabled = {},
		btnTestEx.Font.SetStyle(FontStyle::Bold | FontStyle::Italic),
		btnTestEx.Text = u"附加测试",
		btnTestEx.HorizontalAlignment = TextAlignment::Left,
		btnTestEx.VerticalAlignment = TextAlignment::Down,
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
		dsk_dn.BoundControlPtr = std::bind(&ShlExplorer::GetBoundControlPtr,
			this, std::placeholders::_1),
		FetchEvent<KeyUp>(dsk_dn) += OnKey_Bound_TouchUp,
		FetchEvent<KeyDown>(dsk_dn) += OnKey_Bound_TouchDown,
		FetchEvent<KeyPress>(dsk_dn) += [this](KeyEventArgs&& e){
			if(e.GetKeys()[YCL_KEY(X)])
				SwitchVisible(pnlSetting);
		},
		fbMain.GetViewChanged() += [this](UIEventArgs&&){
			lblPath.Text = String(fbMain.GetPath());
			Invalidate(lblPath);
		},
		fbMain.GetSelected() += [this](IndexEventArgs&&){
			Enable(btnOK, CheckReaderEnability(fbMain, cbHex));
		},
		FetchEvent<Click>(btnOK) += [this](CursorEventArgs&&){
			if(fbMain.IsSelected())
			{
				const auto& path(fbMain.GetPath());
			//	const string s(path);
				const auto category(ClassifyFile(path));

				if(category == FileCategory::Text
					|| category == FileCategory::Binary)
				{
					const auto h_up(GetDesktopUpHandle());
					const auto h_dn(GetDesktopDownHandle());
					const bool b(category == FileCategory::Text
						&& !cbHex.IsTicked());

					PostMessage<SM_TASK>(0xF8, [=]{
						ResetDSDesktops(*h_up, *h_dn);
						if(b)
							NowShellTo(ystdex::make_shared<ShlTextReader>(path,
								h_up, h_dn));
						else
							NowShellTo(ystdex::make_shared<ShlHexBrowser>(path,
								h_up, h_dn));
					});
				}
			}
		},
		FetchEvent<Click>(cbFPS) += [this](CursorEventArgs&&){
			SetInvalidationOf(GetDesktopDown());
		},
		FetchEvent<Click>(cbHex) += [this](CursorEventArgs&&){
			Enable(btnOK, CheckReaderEnability(fbMain, cbHex));
			SetInvalidationOf(GetDesktopDown());
		},
		FetchEvent<Move>(pnlSetting) += [this](UIEventArgs&&){
			lblDragTest.Text = to_string(GetLocationOf(pnlSetting)) + ';';
			Invalidate(lblDragTest);
		},
		FetchEvent<TouchHeld>(pnlSetting) += OnTouchHeld_Dragging,
#if YCL_DS
		FetchEvent<TouchDown>(pnlSetting) += [this](CursorEventArgs&&){
			struct ::mallinfo t(::mallinfo());

			lblInfo.Text = ystdex::sfmt("%d,%d,%d,%d,%d;",
				t.arena, t.ordblks, t.uordblks, t.fordblks, t.keepcost);
			Invalidate(lblInfo);
		},
#endif
		FetchEvent<Click>(pnlSetting) += [this](CursorEventArgs&&){
			yunseq(
				lblDragTest.ForeColor = GenerateRandomColor(),
				lblTitle.ForeColor = GenerateRandomColor()
			);
			Invalidate(pnlSetting);
		},
		FetchEvent<Click>(btnTestEx) += [this](CursorEventArgs&& e){
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
		FetchEvent<Click>(btnPrevBackground) += [this](CursorEventArgs&&){
			auto& dsk_up(GetDesktopUp());
			auto& dsk_dn(GetDesktopDown());

			if(up_i > 0)
			{
				--up_i;
				Enable(btnNextBackground);
			}
			if(up_i == 0)
				Enable(btnPrevBackground, false);
			dsk_up.Background = ImageBrush(FetchImage(up_i));
			SetInvalidationOf(dsk_up),
			SetInvalidationOf(dsk_dn);
		},
		FetchEvent<Click>(btnNextBackground) += [this](CursorEventArgs&&){
			auto& dsk_up(GetDesktopUp());
			auto& dsk_dn(GetDesktopDown());

			if(size_t(up_i + 1) < Image_N)
			{
				++up_i;
				Enable(btnPrevBackground);
			}
			if(size_t(up_i + 1) == Image_N)
				Enable(btnNextBackground, false);
			dsk_up.Background = ImageBrush(FetchImage(up_i));
			SetInvalidationOf(dsk_up),
			SetInvalidationOf(dsk_dn);
		}
	);
	RequestFocusCascade(fbMain),
	SetInvalidationOf(dsk_up),
	SetInvalidationOf(dsk_dn);

	auto& m1(*(ynew Menu({}, share_raw(
		new TextList::ListType{u"测试", u"关于", u"设置(X)", u"退出"}), 1u)));
	auto& m2(*(ynew Menu({},
		share_raw(new TextList::ListType{u"项目1", u"项目2"}), 2u)));

	m1.GetConfirmed() += [this](IndexEventArgs&& e){
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
#if YCL_MINGW32
	m2.GetConfirmed() += [this](IndexEventArgs&& e){
		MinGW32::TestFramework(e.Value);
	},
#endif
	mhMain += m1, mhMain += m2,
	m1 += make_pair(0u, &m2);
	ResizeForContent(m1), ResizeForContent(m2),
	SetLocationOf(m1, Point(btnMenu.GetX(), btnMenu.GetY() - m1.GetHeight()));
	//m1.SetWidth(btnMenu.GetWidth() + 20);
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
			auto& g(ystdex::polymorphic_downcast<BufferedRenderer&>(
				GetDesktopUp().GetRenderer()).GetContext());
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
		if(k[YCL_KEY(A)])
			return &btnOK;
		if(k[YCL_KEY_Start])
			return &btnMenu;
	}
	return nullptr;
}

YSL_END_NAMESPACE(YReader)

