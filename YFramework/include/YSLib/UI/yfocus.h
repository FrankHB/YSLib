/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.h
\ingroup UI
\brief 图形用户界面焦点特性。
\version r2646;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 +0800;
\par 修改时间:
	2011-11-09 15:22 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YFocus;
*/


#ifndef YSL_INC_UI_YFOCUS_HPP_
#define YSL_INC_UI_YFOCUS_HPP_

#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 判断部件是否取得焦点。
*/
bool
IsFocused(const IWidget&);

/*!
\brief 向部件容器请求获得焦点，并指定 GotFocus 事件发送控件。
\param dst 事件目标。
\param src 事件源。
\note 若成功则在 dst 上触发 src 发送的 GotFocus 事件。
*/
void
RequestFocusFrom(IWidget& dst, IWidget& src);

/*!
\brief 释放焦点，并指定 LostFocus 事件发送控件。
\param dst 事件目标。
\param src 事件源。
\note 若成功则在 dst 上触发 src 发送的 LostFocus 事件。
*/
void
ReleaseFocusFrom(IWidget& dst, IWidget& src);

/*!
\ingroup HelperFunctions
\brief 向部件容器释放获得焦点，成功后向自身发送 GotFocus 事件。
*/
inline void
RequestFocus(IWidget& wgt)
{
	RequestFocusFrom(wgt, wgt);
}

/*!
\ingroup HelperFunctions
\brief 释放焦点，成功后向自身发送 LostFocus 事件。
*/
inline void
ReleaseFocus(IWidget& wgt)
{
	ReleaseFocusFrom(wgt, wgt);
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

