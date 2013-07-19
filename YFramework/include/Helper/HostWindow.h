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
\version r259
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:16:53 +0800
\par 修改时间:
	2013-07-19 16:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#ifndef INC_Helper_HostWindow_h_
#define INC_Helper_HostWindow_h_ 1

#include "Helper/yglobal.h"
#if YCL_MINGW32
#	include <YCLib/Win32GUI.h>
#endif

YSL_BEGIN

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

#	if YCL_MINGW32
//! \since build 427
using namespace platform_ex;
#	endif

/*!
\brief 宿主环境支持的窗口。
\since build 379
*/
class YF_API Window : public MinGW32::HostWindow
{
private:
	//! \since build 380
	std::reference_wrapper<Environment> env;

public:
	/*!
	\exception LoggedEvent 异常中立：窗口类名不是 WindowClassName 。
	\since build 429
	*/
	//@{
	Window(NativeWindowHandle);
	Window(NativeWindowHandle, Environment&);
	//@}
	virtual
	~Window();

	DefGetter(const ynothrow, Environment&, Host, env)
	/*!
	\brief 取预定的指针设备输入响应有效区域的左上角和右下角坐标。
	\note 坐标相对于客户区。
	\since build 388
	*/
	virtual pair<YSLib::Drawing::Point, YSLib::Drawing::Point>
	GetInputBounds() const ynothrow;

	void
	OnLostFocus() override;

	/*!
	\brief 刷新：保持渲染状态同步。
	\since build 407
	*/
	virtual PDefH(void, Refresh, )
		ImplExpr(void())
};

YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif

