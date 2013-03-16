/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.h
\ingroup UI
\brief 图形用户界面焦点特性。
\version r1714
\author FrankHB <frankhb1989@gmail.com>
\since build 168
\par 创建时间:
	2010-05-01 13:52:56 +0800
\par 修改时间:
	2013-03-13 12:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YFocus
*/


#ifndef YSL_INC_UI_yfocus_h_
#define YSL_INC_UI_yfocus_h_ 1

#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

/*!
\brief 判断部件是否取得焦点。
*/
YF_API bool
IsFocused(const IWidget&);

/*!
\brief 向部件容器请求获得焦点，并指定 GotFocus 事件发送控件。
\param release_event 是否在需要释放已有焦点时触发事件。
\return 是否成功。
\since build 315
*/
YF_API bool
DoRequestFocus(IWidget&, bool release_event = false);

/*!
\brief 释放焦点，并指定 LostFocus 事件发送控件。
\return 是否成功。
\since build 315
*/
YF_API bool
DoReleaseFocus(IWidget&);

/*!
\brief 向部件容器请求获得焦点，并指定 GotFocus 事件发送控件。
\param dst 事件目标。
\param src 事件源。
\note 若成功则在 dst 上触发 src 发送的 GotFocus 事件。
*/
YF_API void
RequestFocusFrom(IWidget& dst, IWidget& src);

/*!
\brief 释放焦点，并指定 LostFocus 事件发送控件。
\param dst 事件目标。
\param src 事件源。
\note 若成功则在 dst 上触发 src 发送的 LostFocus 事件。
*/
YF_API void
ReleaseFocusFrom(IWidget& dst, IWidget& src);

/*!
\ingroup helper_functions
\brief 向部件容器释放获得焦点，成功后向自身发送 GotFocus 事件。
*/
inline void
RequestFocus(IWidget& wgt)
{
	RequestFocusFrom(wgt, wgt);
}

/*!
\ingroup helper_functions
\brief 释放焦点，成功后向自身发送 LostFocus 事件。
*/
inline void
ReleaseFocus(IWidget& wgt)
{
	ReleaseFocusFrom(wgt, wgt);
}

/*!
\brief 清除焦点指针，同时以此部件作为事件源，调用被清除焦点部件的 LostFocus 事件。
\note 若此部件非容器则无效。
\since build 258
*/
YF_API void
ClearFocusingOf(IWidget&);

/*!
\brief 级联请求部件及上层容器焦点。
*/
YF_API void
RequestFocusCascade(IWidget&);

/*!
\brief 级联释放部件及上层容器焦点。
*/
YF_API void
ReleaseFocusCascade(IWidget&);

YSL_END_NAMESPACE(UI)

YSL_END

#endif

