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
\version r326
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:29 +0800
\par 修改时间:
	2015-02-02 00:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostedUI
*/


#ifndef INC_Helper_HostedUI_h_
#define INC_Helper_HostedUI_h_ 1

#include "YModules.h"
#include YFM_Helper_HostRenderer
#include YFM_YSLib_UI_Hover
#include YFM_YSLib_UI_Label

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
\note 最后一个参数决定是否为根模式。
\sa UI::OnTouchHeld_Dragging
\sa HostRenderer::RootMode
\since build 571
*/
YF_API void
DragWindow(Window&, UI::CursorEventArgs&&, bool = {});
#	endif
#	if YCL_Win32

/*!
\brief 以指定 Windows 窗口样式和标题栏文字显示部件为顶层窗口。
\return 设置的宿主渲染器引用。
\exception LoggedEvent 宽或高不大于 0 。
\since build 570

WS_EX_LAYERED 被设置时默认透明，同时设置窗口 UseOpacity 成员指定不透明性。
在 UseOpacity 时可对宿主窗口 Opacity 成员设置整体不透明性。
当部件位置不为 Point::Invalid 时设置顶层窗口位置。
设置宿主渲染器并阻塞等待宿主窗口指针非空。
*/
YF_API HostRenderer&
ShowTopLevel(UI::Widget&, unsigned long = WS_POPUP, unsigned long
	= WS_EX_LAYERED, int = SW_SHOWNORMAL, const wchar_t* = L"");
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
/*!
\note 第一参数指定悬停时引起动作的部件。
\note 第二参数指定显示为顶层窗口的部件。
\since build 567
*/
//@{
#	if YCL_Win32
/*!
\brief 设置悬停操作时显示指定部件为顶层窗口。
\since build 573
*/
template<typename _func>
void
ActOnHover_ShowTopLevel(UI::IWidget& sender, UI::Widget& wgt, _func f,
	int wstyle_ex = WS_EX_LAYERED | WS_EX_TOOLWINDOW)
{
	UI::ActOnHover(sender, [&]{
		f();
		ShowTopLevel(wgt, WS_POPUP, wstyle_ex, SW_SHOWNOACTIVATE);
	});
}
#	endif
/*!
\brief 设置悬停操作时在指定位置显示指定部件为顶层窗口。
\note 使用 ADL \c SetLocationOf 设置部件位置。
\todo 非 Win32 宿主平台实现。
*/
template<typename _func>
void
ActOnHover_ShowTopLevelAt(UI::IWidget& sender, UI::Widget& wgt, _func f)
{
#	if YCL_Win32
	Host::ActOnHover_ShowTopLevel(sender, wgt, [&, f]{
		SetLocationOf(wgt, f());
	});
#	else
	yunused(sender), yunused(wgt), yunused(f);
#	endif
}
//@}

/*!
\brief 以参数指定的悬停状态，绑定悬停指定部件上时在宿主作为提示显示的顶层部件。
\return 插入被悬停部件的事件 CursorOver 和 Leave 的迭代器。
\exception BadEvent 异常中立：由 FetchEvent 抛出。
\exception std::bad_cast 异常中立：由 FetchEvent 抛出。
\note 不完全强异常安全：只保证由 FetchEvent 调用抛出异常时未添加事件处理器。
\sa ActOnHover_ShowTopLevelAt
\since build 573

在第二参数指定为被悬停的部件上的 CursorOver 和 Leave 事件中插入合适的事件处理器，
以支持在宿主中显示第三参数指定的悬停部件在超过第一参数指定的时限后，
作为顶层部件显示。
被悬停部件的事件通过 CursorOver 和 Leave 访问。仅访问这两个事件。
访问失败时可能由控制器或 FetchEvent 抛出异常。
*/
YF_API array<GEvent<UI::HCursorEvent::FuncType>::iterator, 2>
BindTimedTips(UI::TimedHoverState&, UI::IWidget&, UI::Widget&);

/*!
\brief 以参数指定的悬停状态，设置边框并调用 BindTimedTips 绑定悬停的标签。
\sa UI::SetupContentsOf
\sa BindTimedTips
\since build 573
*/
YF_API void
SetupTimedTips(UI::TimedHoverState&, UI::IWidget&, UI::Label&,
	const String&, const Drawing::Rect& = Drawing::Rect::Invalid,
	const Drawing::Font& = {},
	const Drawing::Padding& = Drawing::DefaultMargin * 2);

} // namespace Host;
#endif

} // namespace YSLib;

#endif

