/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostedUI.h
\ingroup Helper
\brief 宿主环境支持的用户界面。
\version r85
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:29 +0800
\par 修改时间:
	2013-04-26 20:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostedUI
*/


#ifndef INC_Helper_HostedUI_h_
#define INC_Helper_HostedUI_h_ 1

#include "Helper/GUIApplication.h"
#include "YSLib/UI/yrender.h"
#include "YSLib/UI/ywidget.h"

YSL_BEGIN

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

/*!
\brief 取宿主渲染器指针。
\return 若渲染器类型能转换为 HostRenderer 则返回转换后的指针；否则为空。
\since build 389
*/
YF_API UI::BufferedRenderer*
GetHostRendererPtrOf(UI::IWidget&);

/*!
\brief 取宿主渲染器对应的窗口。
\return 若渲染器类型能转换为 HostRenderer 且有对应窗口则返回窗口指针；否则为空。
\since build 389
*/
YF_API Window*
GetWindowPtrOf(UI::IWidget&);


/*!
\brief 制造新的宿主渲染器。
\return unique_ptr 包装的渲染器，保证实际动态类型为 HostRenderer 。
\since build 389
*/
YF_API unique_ptr<UI::BufferedRenderer>
MakeHostRenderer(UI::IWidget&, std::function<NativeWindowHandle()>);


//! \since build 401
template<typename _tParam>
void
WrapRenderer(UI::Widget& wgt, _tParam&& arg)
{
	wgt.SetRenderer(MakeHostRenderer(wgt, yforward(arg)));
}
//! \since build 401
template<typename... _tParams>
void
WrapRenderer(UI::Widget& wgt, _tParams&&... args)
{
	wgt.SetRenderer(MakeHostRenderer(wgt, std::bind(yforward(args)...)));
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif

