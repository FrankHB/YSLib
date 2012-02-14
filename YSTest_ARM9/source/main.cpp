/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup DS
\brief ARM9 主源文件。
\version r2188;
\author FrankHB<frankhb1989@gmail.com>
\since build 1 。
\par 创建时间:
	2009-11-12 21:26:30 +0800;
\par 修改时间:
	2012-02-14 20:02 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Main_ARM9;
*/


//标识主源文件。
#ifdef YSL_ARM9_MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro \
		YSL_ARM9_MAIN_CPP_ be defined iff. once in the project.
#endif

#define YSL_ARM9_MAIN_CPP_ YSL_MAIN_CPP_

#ifdef YSL_MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro \
		YSL_MAIN_CPP_ be defined iff. once in the project.
#endif
#define YSL_MAIN_CPP_

//定义使用 EFSLib 。
//#define USE_EFS

//包含文件。
#include "Shells.h"

using namespace platform;
using std::puts;

YSL_BEGIN

const char* G_COMP_NAME = "Franksoft";
const char* G_APP_NAME = "YShell Test";
const char* G_APP_VER = "0.2600";

//调试函数。
namespace
{

}

using namespace Components;
using namespace Drawing;

YSL_BEGIN_NAMESPACE(Shells)

MainShell::MainShell()
	: ShlDS(),
	lblTitle(Rect(50, 20, 100, 22)),
	lblStatus(Rect(60, 80, 80, 22)),
	lblDetails(Rect(30, 20, 160, 22))
{}

int
MainShell::OnActivated(const Message& msg)
{
	ParentType::OnActivated(msg);

	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	platform::YDebugSetStatus(true);

	dsk_up += lblTitle;
	dsk_up += lblStatus;
	dsk_dn += lblDetails;
//	dsk_up.GetBackgroundImagePtr() = FetchImage(1);
	dsk_up.BackColor = Color(240, 216, 192);
//	dsk_dn.BackColor = Color(240, 216, 240);
	dsk_dn.BackColor = FetchGUIShell().Colors[Styles::Desktop];
	lblTitle.Text = Application::ProductName;
	lblStatus.Text = "Loading...";
	lblDetails.Text = u"初始化中，请稍后……";
	lblDetails.ForeColor = ColorSpace::White;
	lblDetails.SetTransparent(true);
	SetInvalidationOf(dsk_up);
	UpdateToScreen();
	//初始化所有图像资源。

	auto& pb(*new ProgressBar(Rect(8, 168, 240, 16), 10));

	dsk_up += pb;
	for(size_t i(0); i < 10; ++i)
	{
		pb.SetValue(i);
//		Invalidate(pb);
		dsk_up.BackColor = Color(255 - i * 255 / 10, 216, 192);
		SetInvalidationOf(dsk_up);
		Invalidate(dsk_up);
		dsk_up.Validate();
		dsk_up.Update();
		YReader::FetchImage(i);
	}
	pb.SetValue(10);
	Invalidate(dsk_up);
	dsk_up.Validate();
	dsk_up.Update();
	dsk_up -= pb;
	delete &pb;
	YReader::SetShellToStored<YReader::ShlExplorer>();
	return 0;
}

int
MainShell::OnDeactivated()
{
	reset(GetDesktopUp().GetBackgroundImagePtr());
	reset(GetDesktopDown().GetBackgroundImagePtr());
	ParentType::OnDeactivated();
	return 0;
}

YSL_END_NAMESPACE(Shells)

YSL_END

