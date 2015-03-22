/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Win32Control.cpp
\ingroup Helper
\ingroup MinGW32
\brief Win32 控件相关接口。
\version r58
\author FrankHB <frankhb1989@gmail.com>
\since build 387
\par 创建时间:
	2013-03-12 00:21:59 +0800
\par 修改时间:
	2015-03-21 16:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(MinGW32)::Win32Control
*/


#include "Helper/YModules.h"
#include YFM_MinGW32_Helper_Win32Control
#include YFM_Helper_Environment

namespace YSLib
{

namespace Windows
{

namespace UI
{

ImplDeDtor(ViewSignal)


bool
ControlView::HitChildren(const Point& pt) const
{
	const auto h_wnd(Nonnull(GetNativeHandle())), h_child(
		::ChildWindowFromPointEx(h_wnd, {pt.X, pt.Y}, CWP_SKIPINVISIBLE));

	// TODO: Support custom environment.
	return h_child && h_child != h_wnd
		&& !FetchEnvironment().FindWindow(h_child);
}

} // namespace UI;

} // namespace Windows;

} // namespace YSLib;

