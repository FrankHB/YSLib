/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostWindow.h
\ingroup Helper
\brief 宿主环境窗口。
\version r116
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:16:53 +0800
\par 修改时间:
	2013-04-13 13:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#ifndef INC_Helper_HostWindow_h_
#define INC_Helper_HostWindow_h_ 1

#include "Helper/GUIApplication.h"

YSL_BEGIN

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

/*!
\brief 宿主窗口。
\since build 379
\todo 处理 Windows API 返回值。
*/
class YF_API Window
{

private:
	//! \since build 380
	std::reference_wrapper<Environment> env;
	//! \since build 389
	NativeWindowHandle h_wnd;

public:
	/*!
	\throw LoggedEvent 窗口类名不是 WindowClassName 。
	\since build 398
	*/
	Window(NativeWindowHandle, Environment& = FetchEnvironment());
	DefDelCopyCtor(Window)
	DefDelMoveCtor(Window)
	virtual
	~Window();

	//! \since build 389
	DefGetter(const ynothrow, NativeWindowHandle, NativeHandle, h_wnd)
	DefGetter(const ynothrow, Environment&, Host, env)
	/*!
	\brief 取预定的指针设备输入响应有效区域的左上角和右下角坐标。
	\note 坐标相对于客户区。
	\since build 388
	*/
	virtual pair<Drawing::Point, Drawing::Point>
	GetInputBounds() const ynothrow;

	//! \note 线程安全：跨线程调用时使用基于消息队列的异步设置。
	void
	Close();

	/*!
	\brief 调整窗口大小。
	\note 线程安全：跨线程调用时使用基于消息队列的异步设置。
	\since build 388
	*/
	void
	Resize(const Drawing::Size&);

	/*!
	\brief 按客户区调整窗口大小。
	\note 线程安全：跨线程调用时使用基于消息队列的异步设置。
	\since build 388
	*/
	void
	ResizeClient(const Drawing::Size&);

	virtual void
	OnDestroy();

	virtual void
	OnLostFocus();

	virtual void
	OnPaint();

	//! \since build 381
	void
	Show() ynothrow;
};

YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif

