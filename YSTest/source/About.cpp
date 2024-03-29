﻿/*
	© 2012-2016, 2019, 2021-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file About.cpp
\ingroup YReader
\brief 关于界面。
\version r291
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:06:35 +0800
\par 修改时间:
	2023-04-05 04:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::About
*/


#include "About.h" // for ColorSpace, FetchVCSRevisionString, String, G_APP_VER,
//	YSL_SemVerStr_Short, FetchEvent, TouchHeld, Click, ValueNode, FetchRoot;

namespace YReader
{

//! \since build 434
namespace
{

const char TU_About[]{R"NPL(root
($type "Panel")($bounds "0 0 208 144")
(lblTitle
	($type "Label")($bounds "8 4 192 28"))
(lblVersion
	($type "Label")($bounds "8 36 192 40"))
(lblCopyright
	($type "Label")($bounds "8 80 192 20"))
(btnSysInfo
	($type "Button")($bounds "12 106 60 22"))
(btnExit
	($type "Button")($bounds "84 106 60 22"))
)NPL"};

} // unnamed namespace;

AboutPanel::AboutPanel()
	: DialogPanel({8, 24, 208, 144}),
	dynWgts(FetchWidgetLoader(), TU_About)
{
	auto& node(dynWgts.WidgetNode);
	DeclDynWidget(Panel, root, node)
	DeclDynWidgetNode(Label, lblTitle)
	DeclDynWidgetNode(Label, lblVersion)
	DeclDynWidgetNode(Label, lblCopyright)
	DeclDynWidgetNode(Button, btnSysInfo)
	DeclDynWidgetNode(Button, btnExit)
	const auto p(FetchVCSRevisionString());

	AddWidgetsZ(*this, 32, root),
	lblTitle.Font.SetSize(20),
	lblTitle.Font.SetStyle(FontStyle::Bold),
	yunseq(
	lblTitle.Background = nullptr,
	lblTitle.Text = G_APP_NAME,
	lblTitle.HorizontalAlignment = TextAlignment::Left,
	lblTitle.VerticalAlignment = TextAlignment::Down,
	lblTitle.ForeColor = ColorSpace::Blue,
	lblVersion.Background = nullptr,
	lblVersion.AutoWrapLine = true,
	lblVersion.Text = String(G_APP_VER, lblVersion.Text.get_allocator())
		+ u' ' + u"" YSL_SemVerStr_Short + u' '
		+ String(p, lblVersion.Text.get_allocator()),
	lblVersion.ForeColor = ColorSpace::Green,
	lblCopyright.Background = nullptr,
	lblCopyright.Text = String("(C)2009-2023 by ") + G_COMP_NAME,
	lblCopyright.ForeColor = ColorSpace::Maroon,
	btnSysInfo.Text = u"系统信息",
	btnExit.Text = u"退出",
	root.Background = SolidBrush({248, 120, 120}),
	FetchEvent<TouchDown>(root) += [&](CursorEventArgs&& e){
		root.Background = SolidBrush(GenerateRandomColor());
		SetInvalidationOf(root);
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			e.Handled = true;
			RequestFocusCascade(*this);
		}
	},
	FetchEvent<TouchHeld>(root) += std::bind(OnTouchHeld_DraggingRaw,
		std::placeholders::_1, std::ref(*this)),
	FetchEvent<Click>(btnSysInfo) += std::bind(std::ref(OnCommand),
		Command::SystemInfo),
	FetchEvent<Click>(btnSysInfo) += std::bind(Close, std::ref(*this)),
	FetchEvent<Click>(btnExit)
		+= std::bind(YSLib::PostQuitMessage, 0, DefaultQuitPriority)
	);
	SetInvalidationOf(*this);
}
ImplDeDtor(AboutPanel)


SystemInformationPanel::SystemInformationPanel()
	: DialogPanel({0, 0, 256, 170}),
	view({0, 24, 256, 170}), lbl_title({3, 2, 80, 20})
{
	AddWidgets(*this, view, lbl_title),
	yunseq(
	lbl_title.Background = nullptr,
	lbl_title.Text = u"系统信息"
	);
}
ImplDeDtor(SystemInformationPanel)

void
SystemInformationPanel::UpdateContents()
{
	view.GetExtractText() = [](const ValueNode& nd) -> String{
		String n(nd.GetName(), nd.get_allocator());

		TryRet(std::move(n) + u": " + TreeList::DefaultExtractText(nd))
		CatchIgnore(bad_any_cast&)
		return n;
	};
	view.GetTreeRootRef() = FetchRoot();
	view.BindView();
}

} // namespace YReader;

