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
\version r463
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:10:49 +0800
\par 修改时间:
	2013-05-12 11:23 +0800
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
	auto npath(IO::Path(path).GetExtension().GetNativeString());

	for(auto& c : npath)
		c = std::tolower(c);

	const auto ext(IO::Path(path).GetExtension().GetNativeString());

	if(std::any_of(exts, exts + arrlen(exts), [&](const char* str){
		return std::strcmp(ext.c_str(), str) == 0;
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
		auto& app(FetchGlobalInstance<DSApplication>());

		app.GetScreenUp().Update(FetchImage(up_i)->GetBufferPtr());
		app.GetScreenDown().Update(FetchImage(dn_i)->GetBufferPtr());
		platform::WaitForInput();
	}
}

} // unnamed namespace;


ShlExplorer::ShlExplorer(const IO::Path& path,
	const shared_ptr<Desktop>& h_dsk_up, const shared_ptr<Desktop>& h_dsk_dn)
	: ShlDS(h_dsk_up, h_dsk_dn),
	lblTitle({16, 20, 220, 22}), lblPath({8, 48, 240, 48}),
	lblInfo({8, 100, 240, 64}), fbMain({0, 0, 256, 152}),
	btnTest({128, 170, 64, 22}), btnOK({192, 170, 64, 22}),
	btnMenu({0, 170, 72, 22}), pnlSetting({10, 40, 224, 136}),
	cbHex({166, 156, 90, 13}), cbFPS({10, 90, 73, 18}),
	cbPreview({10, 110, 115, 18}), lblDragTest({4, 4, 104, 22}),
	btnEnterTest({8, 32, 104, 22}), btnTestEx({48, 60, 156, 22}),
	btnPrevBackground({114, 90, 30, 22}),
	btnNextBackground({164, 90, 30, 22}),
	pFrmAbout(make_unique<FrmAbout>()), mhMain(*GetDesktopDownHandle()),
	fpsCounter(std::chrono::milliseconds(500))
{
	static int up_i(1);
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	AddWidgets(pnlSetting, cbFPS, cbPreview, btnEnterTest, lblDragTest,
		btnTestEx, btnPrevBackground, btnNextBackground),
	AddWidgets(dsk_up, lblTitle, lblPath, lblInfo),
	AddWidgets(dsk_dn, fbMain, btnTest, btnOK, btnMenu, cbHex),
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
		lblDragTest.HorizontalAlignment = TextAlignment::Left,
		//btnTestEx.Enabled = false,
		btnTestEx.Text = u"附加测试",
		btnTestEx.HorizontalAlignment = TextAlignment::Left,
		btnTestEx.VerticalAlignment = TextAlignment::Down,
		btnEnterTest.Text = u"边界测试",
		btnEnterTest.HorizontalAlignment = TextAlignment::Right,
		btnEnterTest.VerticalAlignment = TextAlignment::Up,
		btnPrevBackground.Text = u"<<",
		btnNextBackground.Text = u">>",
		fbMain.SetPath(path),
		Enable(btnTest),
		Enable(btnOK, false),
		Enable(btnPrevBackground, false),
		dsk_dn.BoundControlPtr = std::bind(&ShlExplorer::GetBoundControlPtr,
			this, std::placeholders::_1),
		FetchEvent<KeyUp>(dsk_dn) += OnKey_Bound_TouchUpAndLeave,
		FetchEvent<KeyDown>(dsk_dn) += OnKey_Bound_EnterAndTouchDown,
		FetchEvent<KeyPress>(dsk_dn) += OnKey_Bound_Click,
		fbMain.GetViewChanged() += [this](UIEventArgs&&){
			lblPath.Text = String(fbMain.GetPath());
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
					const auto h_up(GetDesktopUpHandle());
					const auto h_dn(GetDesktopDownHandle());
					const bool b(GetEntryType(s) == EnrtySpace::Text
						&& !cbHex.IsTicked());

					PostMessage<SM_TASK>(0xF8, [=]{
						ResetDSDesktops(*h_up, *h_dn);
						if(b)
							NowShellTo(ystdex::make_shared<ShlTextReader>(path,
								h_up, h_dn));
						else
							NowShellTo(ystdex::make_shared<ShlHexBrowser>(path,
								h_up, h_dn));
					// TODO: Use G++ 4.8 or later.
					//	SetShellTo(make_shared<ShlTextReader>(path));
					//	SetShellToNew<ShlTextReader>(path);
					});
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
		FetchEvent<Move>(pnlSetting) += [this](UIEventArgs&&){
			lblDragTest.Text = to_string(GetLocationOf(pnlSetting)) + ';';
			Invalidate(lblDragTest);
		},
		FetchEvent<TouchMove>(pnlSetting) += OnTouchMove_Dragging,
#if YCL_DS
		FetchEvent<TouchDown>(pnlSetting) += [this](TouchEventArgs&&){
			struct ::mallinfo t(::mallinfo());

			lblInfo.Text = ystdex::sfmt("%d,%d,%d,%d,%d;",
				t.arena, t.ordblks, t.uordblks, t.fordblks, t.keepcost);
			Invalidate(lblInfo);
		},
#endif
		FetchEvent<Click>(pnlSetting) += [this](TouchEventArgs&&){
			yunseq(
				lblDragTest.ForeColor = GenerateRandomColor(),
				lblTitle.ForeColor = GenerateRandomColor()
			);
			Invalidate(pnlSetting);
		},
		FetchEvent<Click>(btnTestEx) += [this](TouchEventArgs&& e){
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
		mhMain.Roots[&btnMenu] = 1u,
		FetchEvent<Click>(btnMenu) += [this](TouchEventArgs&&){
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
		FetchEvent<Click>(btnPrevBackground) += [this](TouchEventArgs&&){
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
			dsk_up.Background = ImageBrush(FetchImage(up_i));
			CheckBackgroundPreview(cbPreview, up_i, up_i + 1);
			SetInvalidationOf(dsk_up),
			SetInvalidationOf(dsk_dn);
		}
	);
	RequestFocusCascade(fbMain),
	SetInvalidationOf(dsk_up),
	SetInvalidationOf(dsk_dn);

	auto& m1(*(ynew Menu({},
		share_raw(new TextList::ListType{u"测试", u"关于", u"退出"}), 1u)));
	auto& m2(*(ynew Menu({},
		share_raw(new TextList::ListType{u"项目1", u"项目2"}), 2u)));

	m1.GetConfirmed() += [this](IndexEventArgs&& e){
		switch(e.Value)
		{
		case 1U:
			YAssert(bool(pFrmAbout), "Null pointer found");

			Show(*pFrmAbout);
			break;
		case 2U:
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
		if(k[YCL_KEY(X)])
			return &btnTest;
		if(k[YCL_KEY(A)])
			return &btnOK;
	}
	return nullptr;
}

YSL_END_NAMESPACE(YReader)

