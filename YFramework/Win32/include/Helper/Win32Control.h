/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Win32Control.h
\ingroup Helper
\ingroup Win32
\brief Win32 控件相关接口。
\version r142
\author FrankHB <frankhb1989@gmail.com>
\since build 570
\par 创建时间:
	2013-03-12 00:24:05 +0800
\par 修改时间:
	2015-09-12 13:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(Win32)::Win32Control
*/


#ifndef INC_Helper_Win32Control_h_
#define INC_Helper_Win32Control_h_ 1

#include "Helper/YModules.h"
#include YFM_Helper_GUIApplication
#include YFM_YSLib_Core_YMessage
#include YFM_YSLib_UI_YWidgetView
#include YFM_Helper_HostWindow

namespace YSLib
{

#if YCL_Win32

namespace Windows
{

namespace UI
{

using namespace YSLib::UI;

/*!
\brief 控件视图。
\note 引用现有 Win32 控件实现，当操作失败时抛出 ViewSignal 。
\since build 570
*/
class YF_API ControlView : public UI::AView, private WindowReference
{
public:
	ControlView(NativeWindowHandle h)
		: AView(), WindowReference(h)
	{
		if(!::IsWindow(h))
			throw std::invalid_argument("Invalid native window found.");
	}
	DefDeCopyMoveCtorAssignment(ControlView)

	DefPredBase(const ynothrow ImplI(AView), Visible, WindowReference)

#define YF_Impl_TryWin32View(...) \
	[&, this]{ \
		TryRet(__VA_ARGS__) \
		CatchThrow(Win32Exception& e, ViewSignal(e.what())) \
	}()

	DefGetter(const override, Rect, Bounds,
		YF_Impl_TryWin32View(WindowReference::GetClientBounds()))
	DefGetter(const ImplI(AView), Point, Location,
		YF_Impl_TryWin32View(WindowReference::GetClientLocation()))
	DefGetter(const ImplI(AView), Size, Size,
		YF_Impl_TryWin32View(WindowReference::GetClientSize()))

	PDefH(void, SetBounds, const Rect& r) override
		ImplExpr(YF_Impl_TryWin32View(WindowReference::SetClientBounds(r)))
	PDefH(void, SetLocation, const Point& pt) ImplI(AView)
		ImplExpr(YF_Impl_TryWin32View(WindowReference::MoveClient(pt)))
	PDefH(void, SetSize, const Size& s) ImplI(AView)
		ImplExpr(YF_Impl_TryWin32View(WindowReference::ResizeClient(s)))
	PDefH(void, SetVisible, bool v) ImplI(AView)
		ImplExpr(YF_Impl_TryWin32View(
			WindowReference::Show(v ? SW_SHOWNORMAL : SW_HIDE)))

#undef YF_Impl_TryWin32View

	//! \since build 578
	bool
	HitChildren(const Point&) const override;

	DefClone(const ImplI(AView), ControlView)
};

} // namespace UI;

} // namespace Windows;

#endif

} // namespace YSLib;

#endif

