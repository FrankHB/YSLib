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
\version r206
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:16:53 +0800
\par 修改时间:
	2013-07-15 15:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#ifndef INC_Helper_HostWindow_h_
#define INC_Helper_HostWindow_h_ 1

#include "Helper/GUIApplication.h"
#if YCL_MULTITHREAD == 1
#	include <atomic>
#endif

YSL_BEGIN

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

//! \since build 382
yconstexpr wchar_t WindowClassName[]{L"YFramework Window"};

/*!
\brief 宿主窗口。
\since build 379
*/
class YF_API Window : private MinGW32::WindowReference
{
private:
	//! \since build 380
	std::reference_wrapper<Environment> env;

public:
	/*!
	\brief 限制指针设备响应在窗口边界内。
	\bug 必须支持 <tt>std::atomic</tt> 。
	\since build 427
	*/
	std::atomic<bool> BoundsLimited{false};

	/*!
	\throw LoggedEvent 窗口类名不是 WindowClassName 。
	\since build 398
	*/
	Window(NativeWindowHandle, Environment& = FetchEnvironment());
	DefDelCopyCtor(Window)
	DefDelMoveCtor(Window)
	virtual
	~Window();

	DefGetter(const ynothrow, Environment&, Host, env)
	/*!
	\brief 取预定的指针设备输入响应有效区域的左上角和右下角坐标。
	\note 坐标相对于客户区。
	\since build 388
	*/
	virtual pair<Drawing::Point, Drawing::Point>
	GetInputBounds() const ynothrow;
	//! \since build 427
	//@{
	using WindowReference::GetLocation;
	using WindowReference::GetNativeHandle;
	using WindowReference::GetSize;

	//! \since build 428
	using WindowReference::SetText;

	using WindowReference::Close;

	//! \since build 429
	using WindowReference::Invalidate;

	using WindowReference::Move;
	//@}

	virtual void
	OnDestroy();

	virtual void
	OnLostFocus();

	virtual void
	OnPaint();

	/*!
	\brief 刷新：保持渲染状态同步。
	\since build 407
	*/
	virtual PDefH(void, Refresh, )
		ImplExpr(void())

	//! \since build 427
	//@{
	using WindowReference::Resize;

	using WindowReference::ResizeClient;

	using WindowReference::Show;
	//@}
};

YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif

