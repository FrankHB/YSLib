/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup DS
\brief ARM9 主源文件。
\version 0.2151;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 21:26:30 +0800;
\par 修改时间:
	2011-09-11 20:41 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	Platform::DS::Main;
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
#ifndef PLATFORM_H
#define PLATFORM_H <platform.h>
#endif
#include PLATFORM_H
#include <api.h>

#include YSL_YSBUILD_H
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
	void
	YSDebug_MSG_Print(const Message& msg)
	{
		iprintf("time : %u ticks\n", GetTicks());

		auto c(msg.GetContent());

		iprintf("Message : 0x%04X;\n""Prior : 0x%02X;\nObject ID : %d\n"
			"Context pointer: %p\n",
			msg.GetMessageID(), msg.GetPriority(), msg.GetObjectID(), &c);
		//	"W : %u;\nL : %lx;\n";
		WaitForInput();
		StartTicks();
	}
}

void
YSDebug_MSG_Insert(const Message& msg)
{
	YDebugBegin();
	puts("-YSLIB_DEBUG : MSG_Insert");
	YSDebug_MSG_Print(msg);
}

void
YSDebug_MSG_Peek(const Message& msg)
{
	YDebugBegin();
	puts("-YSLIB_DEBUG : MSG_Peek");
	YSDebug_MSG_Print(msg);
}

using namespace Components;
using namespace Drawing;

YSL_BEGIN_NAMESPACE(Shells)

YMainShell::YMainShell()
	: ShlDS(),
	lblTitle(Rect(50, 20, 100, 22)),
	lblStatus(Rect(60, 80, 80, 22)),
	lblDetails(Rect(30, 20, 160, 22))
{}

int
YMainShell::OnActivated(const Message& msg)
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
	lblTitle.Text = YApplication::ProductName;
	lblStatus.Text = "Loading...";
	lblDetails.Text = _ustr("初始化中，请稍后……");
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
		Validate(dsk_up);
		dsk_up.Update();
//		dsk_up.Refresh(FetchContext(dsk_up), Point::Zero,
//			Rect(Point::Zero, dsk_up.GetSize()));
		YReader::FetchImage(i);
	}
	pb.SetValue(10);
	Invalidate(dsk_up);
	Validate(dsk_up);
	dsk_up.Update();
	dsk_up -= pb;
	delete &pb;
	YReader::SetShellToStored<YReader::ShlExplorer>();
	return 0;
}

int
YMainShell::OnDeactivated(const Message& msg)
{
	reset(GetDesktopUp().GetBackgroundImagePtr());
	reset(GetDesktopDown().GetBackgroundImagePtr());
	ParentType::OnDeactivated(msg);
	return 0;
}

YSL_END_NAMESPACE(Shells)

YSL_END

