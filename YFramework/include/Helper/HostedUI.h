/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostedUI.h
\ingroup Helper
\brief 宿主环境支持的用户界面。
\version r214
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:29 +0800
\par 修改时间:
	2015-01-10 15:52 +0800
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
\return 设置的宿主渲染器引用。
\exception LoggedEvent 宽或高不大于 0 。
\note WS_EX_LAYERED 被设置时默认透明，同时设置窗口 UseOpacity 成员指定不透明性。
\note 在 UseOpacity 时可对宿主窗口 Opacity 成员设置整体不透明性。
\note 最后一个参数指定是否按部件位置设置顶层窗口位置。
\note 复位部件位置为原点、设置宿主渲染器并阻塞等待宿主窗口指针非空。
\since build 564
*/
YF_API HostRenderer&
ShowTopLevel(UI::Widget&, unsigned long = WS_POPUP, unsigned long
	= WS_EX_LAYERED, int = SW_SHOWNORMAL, const wchar_t* = L"", bool = true);
#	endif
#	if !YCL_Android

/*!
\brief 显示控件为顶层可拖动的 GUI 对象。
\note 可能会因为等待顶层对象就绪的宿主渲染器窗口就绪阻塞。
\note 使用 DragWindow 实现拖动宿主窗口，因此需要能支持 UI::TouchHeld 事件。
\note Win32 平台：首先使用 Host::ShowTopLevel 显示窗口。
\since build 428
*/
YF_API void
ShowTopLevelDraggable(UI::Widget&);
#	endif

} // namespace Host;
#endif

} // namespace YSLib;

#endif

