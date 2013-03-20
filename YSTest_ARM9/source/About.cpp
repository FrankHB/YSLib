/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file About.cpp
\ingroup YReader
\brief 关于界面。
\version r75
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:06:35 +0800
\par 修改时间:
	2013-03-20 21:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::About
*/


#include "About.h"

YSL_BEGIN_NAMESPACE(YReader)

FrmAbout::FrmAbout()
	: Form(Rect(5, 60, 208, 144), shared_ptr<Image>()),
	lblTitle(Rect(8, 4, 192, 28)), lblVersion(Rect(8, 36, 192, 40)),
	lblCopyright(Rect(8, 80, 192, 20)),
	btnClose(Rect(12, 106, 60, 22)),
	btnExit(Rect(84, 106, 60, 22))
{
	AddWidgets(*this, lblTitle, lblVersion, lblCopyright, btnClose, btnExit),
	lblTitle.Font.SetSize(20),
	yunseq(
		lblTitle.Background = nullptr,
		lblTitle.Text = G_APP_NAME,
		lblTitle.HorizontalAlignment = TextAlignment::Left,
		lblTitle.VerticalAlignment = TextAlignment::Down,
		lblTitle.ForeColor = ColorSpace::Blue,
		lblVersion.Background = nullptr,
		lblVersion.AutoWrapLine = true,
		lblVersion.Text = G_APP_VER + String(" @ " __DATE__ ", " __TIME__),
		lblVersion.ForeColor = ColorSpace::Green,
		lblCopyright.Background = nullptr,
		lblCopyright.Text = String("(C)2009-2013 by ") + G_COMP_NAME,
		lblCopyright.ForeColor = ColorSpace::Maroon,
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
		FetchEvent<Click>(btnClose) += [this](TouchEventArgs&&){
			Hide(*this);
		},
		FetchEvent<Click>(btnExit) += [](TouchEventArgs&&){
			YSLib::PostQuitMessage(0);
		}
	);
	SetInvalidationOf(*this);
}

YSL_END_NAMESPACE(YReader)

