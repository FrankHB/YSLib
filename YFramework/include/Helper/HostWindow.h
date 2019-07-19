/*
	© 2013-2016, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostWindow.h
\ingroup Helper
\brief 宿主环境窗口。
\version r525
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:16:53 +0800
\par 修改时间:
	2019-07-08 19:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#ifndef INC_Helper_HostWindow_h_
#define INC_Helper_HostWindow_h_ 1

#include "YModules.h"
#include YFM_Helper_YGlobal
#include YFM_Helper_ScreenBuffer
#include YFM_YCLib_HostedGUI
#if YCL_Win32
#	include YFM_YSLib_Core_YString // for YSLib::String;
#	include YFM_YSLib_UI_YWidget // for UI::IWidget;
#	include <atomic>
#endif

namespace YSLib
{

#if YF_Hosted
namespace Host
{

/*!
\brief 宿主环境支持的窗口。
\since build 379
*/
class YF_API Window : public HostWindow
{
private:
	//! \since build 689
	lref<GUIHost> host;

#	if YCL_Win32
public:
	/*!
	\brief 标记是否使用不透明性成员。
	\note 使用 Windows 层叠窗口实现，但和 WindowReference 实现不同：使用
		::UpdateLayeredWindow 而非 WM_PAINT 更新窗口。
	\warning 使用不透明性成员时在此窗口上调用 ::SetLayeredWindowAttributes 、
		GetOpacity 或 SetOpacity 可能出错。
	\since build 435
	*/
	bool UseOpacity{};
	/*!
	\brief 不透明性。
	\note 仅当窗口启用 WS_EX_LAYERED 样式且 UseOpacity 设置为 true 时有效。
	\since build 435
	*/
	Drawing::AlphaType Opacity{0xFF};

	//! \since build 689
	WindowInputHost InputHost;
#	endif

public:
	/*!
	\exception LoggedEvent 异常中立：窗口类名不是 WindowClassName 。
	\since build 429
	*/
	//@{
	Window(NativeWindowHandle);
	//! \since build 689
	Window(NativeWindowHandle, GUIHost&);
	//@}
	~Window() override;

	//! \since build 689
	DefGetter(const ynothrow, GUIHost&, GUIHostRef, host)

	/*!
	\brief 刷新：保持渲染状态同步。
	\since build 407
	*/
	virtual PDefH(void, Refresh, )
		ImplExpr(void())

	/*!
	\brief 更新：同步缓冲区。
	\pre 间接断言：指针参数非空。
	\note 根据 UseOpacity 选择更新操作。
	\since build 589
	*/
	YB_NONNULL(2) void
	UpdateFrom(Drawing::ConstBitmapPtr, ScreenBuffer&);

#if YCL_Win32
	/*!
	\brief 更新：同步指定边界和源偏移量的缓冲区。
	\pre 间接断言：指针参数非空。
	\note 根据 UseOpacity 选择更新操作。
	\since build 591
	*/
	YB_NONNULL(2) void
	UpdateFromBounds(Drawing::ConstBitmapPtr, ScreenBuffer&,
		const Drawing::Rect&, const Drawing::Point& = {});

	/*!
	\brief 更新文本焦点：根据指定的部件和相对部件的位置调整状态。
	\since build 518
	*/
	virtual void
	UpdateTextInputFocus(UI::IWidget&, const Drawing::Point&);
#endif
};

} // namespace Host;
#endif

} // namespace YSLib;

#endif

