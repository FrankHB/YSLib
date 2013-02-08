﻿/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSMain.cpp
\ingroup Helper
\brief DS 平台框架。
\version r2805
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2012-03-25 12:48:49 +0800
\par 修改时间:
	2013-02-07 02:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::DSMain
*/


#include "DSScreen.h"
#include "Host.h"
#include "Helper/Initialization.h"
#include "YSLib/Adaptor/Font.h"
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#if YCL_MULTITHREAD == 1
#	include <thread> // for std::this_thread::*;
#endif
#ifdef YCL_DS
#	include "YSLib/Service/yblit.h" // for Drawing::FillPixel;
#endif
#include "YCLib/Debug.h"

YSL_BEGIN

using namespace Drawing;

namespace
{

#if YCL_MINGW32
yconstexpr double g_max_free_fps(1000);
std::chrono::nanoseconds idle_sleep(u64(1000000000 / g_max_free_fps));
#endif


//注册的应用程序指针。
DSApplication* pApp;

} // unnamed namespace;


namespace
{

/*!
\brief 取空闲消息。
\since build 320
*/
inline Message
FetchIdleMessage()
{
	return Message(SM_INPUT);
}

/*!
\brief 后台消息处理程序。
\since build 320
*/
inline void
Idle(Messaging::Priority prior)
{
	// Note: Wait for GUI input of any shells. Post message for specific shell
	//	would cause low performance when there are many candidate messages
	//	of distinct shells.
	PostMessage(FetchIdleMessage(), prior);
}

} // unnamed namespace;


DSApplication::DSApplication()
try	: Application(),
#if YCL_HOSTED
	p_hosted(),
#endif
	pFontCache(), pScreenUp(), pScreenDown(),
	UIResponseLimit(0x40), Root()
{
	using Devices::DSScreen;

	YAssert(!YSLib::pApp, "Duplicate instance found.");

	//注册全局应用程序实例。
	YSLib::pApp = this;

	//全局初始化。
	InitializeEnviornment();
#if YCL_HOSTED
	p_hosted = make_unique<Host::Environment>();
#endif

	//若有必要，启动本机消息循环线程后完成应用程序实例其它部分的初始化（注意顺序）。

	//初始化：检查程序是否被正确安装并读取配置。
	Root = InitializeInstalled();
	//初始化系统字体资源。
	try
	{
		pFontCache = make_unique<FontCache>();
	}
	catch(...)
	{
		throw LoggedEvent("Error occurred in creating font cache.");
	}
	{
		const auto& node(FetchGlobalInstance().Root["YFramework"]);

		InitializeSystemFontCache(AccessChild<string>(node, "FontFile"),
			AccessChild<string>(node, "FontDirectory"));
	}
	//初始化系统设备。
#if YCL_DS
	InitVideo();
#endif
	try
	{
		pScreenUp = make_unique<DSScreen>(false);
		pScreenDown = make_unique<DSScreen>(true);
	}
	catch(...)
	{
		throw LoggedEvent("Screen initialization failed.");
	}
#if YCL_DS
	ystdex::polymorphic_downcast<DSScreen&>(*pScreenUp)
		.Update(ColorSpace::Blue),
	ystdex::polymorphic_downcast<DSScreen&>(*pScreenDown)
		.Update(ColorSpace::Green);
#elif YCL_HOSTED
	p_hosted->Notify();
#endif
}
catch(FatalError& e)
{
	HandleFatalError(e);
}

DSApplication::~DSApplication()
{
#if YCL_HOSTED
	p_hosted.reset();
#endif

	//等待并确保所有 Shell 被释放。
//	hShell = nullptr;

	//释放全局非静态资源。

	//清理消息队列（必要，保证所有Shell在Application前析构）。
	Queue.Clear();

	//当主 Shell 句柄为静态存储期对象时需要通过 reset 释放。

	//释放默认字体资源。
	reset(pFontCache);

	//释放设备。
	reset(pScreenUp),
	reset(pScreenDown);
	Uninitialize();
}

FontCache&
DSApplication::GetFontCache() const ynothrow
{
	YAssert(bool(pFontCache), "Null pointer found.");

	return *pFontCache;
}
#if YCL_HOSTED
Host::Environment&
DSApplication::GetHost()
{
	YAssert(bool(p_hosted), "Null pointer found.");

	return *p_hosted;
}
#endif
Devices::Screen&
DSApplication::GetScreenUp() const ynothrow
{
	YAssert(bool(pScreenUp), "Null pointer found.");

	return *pScreenUp;
}
Devices::Screen&
DSApplication::GetScreenDown() const ynothrow
{
	YAssert(bool(pScreenDown), "Null pointer found.");

	return *pScreenDown;
}

bool
DSApplication::DealMessage()
{
	using namespace Shells;

	if(Queue.IsEmpty())
	{
	//	Idle(UIResponseLimit);
		OnGotMessage(FetchIdleMessage());
#if YCL_MINGW32
	//	std::this_thread::yield();
		std::this_thread::sleep_for(idle_sleep);
#endif
	}
	else
	{
		// TODO: Consider the application queue to be locked for thread safety.
		const auto i(Queue.GetBegin());

		if(YB_UNLIKELY(i->second.GetMessageID() == SM_QUIT))
			return false;
		if(i->first < UIResponseLimit)
		{
			Idle(UIResponseLimit);
#if YCL_MINGW32
			std::this_thread::sleep_for(idle_sleep);
#endif
		}
		OnGotMessage(i->second);
		Queue.Erase(i);
	}
	return true;
}


#if YCL_MINGW32
// workaround
::HWND
FetchGlobalWindowHandle()
{
	return FetchGlobalInstance().GetHost().Wait()->GetNativeHandle();
}
#endif


Application&
FetchAppInstance()
{
	return FetchGlobalInstance();
}

DSApplication&
FetchGlobalInstance() ynothrow
{
	YAssert(pApp, "Null pointer found.");

	return *pApp;
}

bool
#if YCL_DS
InitConsole(Devices::Screen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
	using namespace platform;

	if(&FetchGlobalInstance().GetScreenUp() == &scr)
		YConsoleInit(true, fc, bc);
	else if(&FetchGlobalInstance().GetScreenDown() == &scr)
		YConsoleInit(false, fc, bc);
	else
		return false;
#elif YCL_MINGW32
InitConsole(Devices::Screen&, Drawing::PixelType, Drawing::PixelType)
{
#else
#	error Unsupported platform found!
#endif
	return true;
}

void
ShowFatalError(const char* s)
{
	using namespace platform;

	YDebugSetStatus();
	YDebugBegin();
	std::printf("Fatal Error:\n%s\n", s);
	terminate();
}

YSL_END

