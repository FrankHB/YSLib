﻿/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostedUI.h
\ingroup Helper
\brief 宿主环境支持的用户界面。
\version r175
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:29 +0800
\par 修改时间:
	2014-10-04 15:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostedUI
*/


#ifndef INC_Helper_HostedUI_h_
#define INC_Helper_HostedUI_h_ 1

#include "YModules.h"
#include YFM_Helper_HostRenderer

namespace YSLib
{

#if YF_Hosted
namespace Host
{

/*!
\brief 取宿主渲染器指针。
\return 若渲染器类型能转换为 HostRenderer 则返回转换后的指针；否则为空。
\since build 430
*/
inline PDefH(HostRenderer*, GetHostRendererPtrOf, UI::IWidget& wgt)
	ImplRet(dynamic_cast<HostRenderer*>(&wgt.GetRenderer()))

/*!
\brief 取宿主渲染器对应的窗口。
\return 若渲染器类型能转换为 HostRenderer 且有对应窗口则返回窗口指针；否则为空。
\since build 430
*/
inline Window*
GetWindowPtrOf(UI::IWidget& wgt)
{
	if(const auto p_r = dynamic_cast<HostRenderer*>(&wgt.GetRenderer()))
		return p_r->GetWindowPtr();
	return {};
}

/*!
\brief 等待宿主渲染器窗口就绪。
\return 宿主渲染器对应的窗口引用。
\exception std::bad_cast 异常中立：渲染器类型转换为 HostRenderer 失败。
\since build 428
*/
YF_API Window&
WaitForHostWindow(UI::IWidget&);


/*!
\brief 制造新的宿主渲染器。
\return unique_ptr 包装的渲染器。
\since build 430
*/
template<typename _func, typename... _tParams>
unique_ptr<HostRenderer>
MakeHostRenderer(UI::IWidget& wgt, _func&& f, _tParams&&... args)
{
	return make_unique<HostRenderer>(wgt, yforward(f), yforward(args)...);
}

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

#	if !YCL_Android

/*!
\brief 按指针设备输入事件指定的光标位置移动宿主窗口。
\sa UI::OnTouchHeld_Dragging
\since build 426
*/
YF_API void
DragWindow(Window&, UI::CursorEventArgs&&);
#	endif
#	if YCL_Win32

/*!
\brief 以指定 Windows 窗口样式和标题栏文字显示部件为顶层窗口。
\note WS_EX_LAYERED 被设置时默认透明，可对宿主窗口 Opacity 成员设置不透明性。
\note 阻塞等待宿主窗口指针非空。
\since build 430
*/
YF_API void
ShowTopLevel(UI::Widget&, ::DWORD = WS_POPUP, ::DWORD = WS_EX_LAYERED,
	const wchar_t* = L"");
#	endif
#	if !YCL_Android

/*!
\brief 显示控件为顶层可拖动的 GUI 对象。
\note 可能会因为等待顶层对象就绪的宿主渲染器窗口就绪阻塞。
\note 使用 DragWindow 实现拖动宿主窗口，因此需要能支持 UI::TouchHeld 事件。
\since build 428
*/
YF_API void
ShowTopLevelDraggable(UI::Widget&);
#	endif

} // namespace Host;
#endif

} // namespace YSLib;

#endif

