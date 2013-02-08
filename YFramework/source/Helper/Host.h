/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Host.h
\ingroup Helper
\brief 宿主环境。
\version r145
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:28:03 +0800
\par 修改时间:
	2013-02-08 02:38 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::Host
*/


#ifndef Inc_Helper_Host_h_
#define Inc_Helper_Host_h_ 1

#include "Helper/DSMain.h" // for Host::Window;
#include "YCLib/NativeAPI.h"
#if YCL_MULTITHREAD == 1
#	include <thread>
#	include <mutex>
#	include <condition_variable>
#endif

YSL_BEGIN

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

/*!
\brief 宿主窗口。
\since build 379
\todo 处理 Windows API 返回值。
*/
class Window
{
public:
	typedef ::HWND NativeHandle;

private:
	Environment* p_env;
	NativeHandle h_wnd;

public:
	Window(NativeHandle, Environment& = FetchGlobalInstance().GetHost());
	DefDelCopyCtor(Window)
	DefDelMoveCtor(Window)
	virtual
	~Window();

	DefGetter(const ynothrow, NativeHandle, NativeHandle, h_wnd)
	DefGetter(const ynothrow, Environment&, Host, *p_env)

	void
	Close();

	virtual void
	OnDestroy();

	virtual void
	OnLostFocus();

	virtual void
	OnPaint();

	void
	Show();
};


/*!
\brief 宿主环境。
\since build 378
*/
class Environment
{
#	if YCL_MULTITHREAD == 1
private:
	//! \brief 宿主环境互斥量。
	std::mutex mtx;
	//! \brief 宿主环境就绪条件。
	std::condition_variable init;
	//! \brief 初始化条件。
	std::condition_variable full_init;
#		if YCL_MINGW32
	//! \brief 本机主窗口指针。
	shared_ptr<Window> p_main_wnd;
#		endif
	//! \brief 宿主背景线程。
	std::thread host_thrd;
#	endif

public:
	/*!
	\brief 本机窗口对象映射。
	\note 不使用 ::SetWindowLongPtr 等 Windows API 保持跨平台及避免和其它代码冲突。
	\warning 销毁窗口前移除映射，否则可能导致未定义行为。
	\since build 379
	*/
	map<Window::NativeHandle, Window*> WindowsMap;

	Environment();
	~Environment();

	/*!
	\brief 宿主消息循环。
	\since build 379
	*/
	static void
	HostLoop();

#	if YCL_MULTITHREAD == 1
	//! \brief 初始化宿主资源和本机消息循环线程。
	void
	HostTask();

	void
	Notify();

	//! \since build 379
	const shared_ptr<Window>&
	Wait();
#	endif
};

YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif

