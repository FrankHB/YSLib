/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file WidgetIteration.h
\ingroup UI
\brief 部件迭代接口。
\version r70
\author FrankHB<frankhb1989@gmail.com>
\since build 356
\par 创建时间:
	2012-11-25 16:36:34 +0800
\par 修改时间:
	2012-11-25 00:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::WidgetIteration
*/


#ifndef YSL_INC_UI_WIDGETITERATION_H_
#define YSL_INC_UI_WIDGETITERATION_H_ 1

#include "ywidget.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

#define DefWidgetSubscriptor(...) \
	DefSubscriptor(ynoexcept, IWidget, __VA_ARGS__) \
	DefGetter(const ynothrow, size_t, ChildrenCount, \
		ystdex::sizeof_params(__VA_ARGS__))

#define DefWidgetBegin(_tIter) \
	_tIter \
	GetBegin() \
	{ \
		return _tIter(*this, 0); \
	}

#define DefWidgetEnd(_tIter) \
	_tIter \
	GetEnd() \
	{ \
		return _tIter(*this, GetChildrenCount()); \
	}

#define DefWidgetMemberIterationOperations(_tIter) \
	DefWidgetBegin(_tIter) \
	DefWidgetEnd(_tIter) \
	virtual pair<WidgetIterator, WidgetIterator> \
	GetChildren() \
	{ \
		return {GetBegin(), GetEnd()}; \
	}

#define DefWidgetMemberIteration(...) \
	DefWidgetMemberIterationOperations(Iterator) \
	DefWidgetSubscriptor(__VA_ARGS__)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

