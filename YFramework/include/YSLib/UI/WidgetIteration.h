/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file WidgetIteration.h
\ingroup UI
\brief 部件迭代接口。
\version r179
\author FrankHB <frankhb1989@gmail.com>
\since build 356
\par 创建时间:
	2012-11-25 16:36:34 +0800
\par 修改时间:
	2013-08-05 21:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::WidgetIteration
*/


#ifndef YSL_INC_UI_WidgetIteration_h_
#define YSL_INC_UI_WidgetIteration_h_ 1

#include "ywidget.h"

namespace YSLib
{

/*!
\def DefSubscriptor
\brief 成员下标访问操作。
\since build 356
*/
#define DefSubscriptor(_q, _type, ...) \
	_type& \
	operator[](size_t idx) _q \
	{ \
		return std::initializer_list<std::reference_wrapper<_type>> \
			{__VA_ARGS__}.begin()[idx]; \
	}
/*!
\def DefSubscriptorBase
\brief 基于基类的成员下标访问操作。
\since build 357
*/
#define DefSubscriptorBase(_q, _type, _tBase, _vBaseN, ...) \
	_type& \
	operator[](size_t idx) _q \
	{ \
		return idx < _vBaseN ? _tBase::operator[](idx) \
			: std::initializer_list<std::reference_wrapper<_type>> \
			{__VA_ARGS__}.begin()[idx - _vBaseN].get(); \
	}


namespace UI
{

/*!
\def DefWidgetSubscriptor
\brief 部件成员下标访问操作。
\since build 356
*/
#define DefWidgetSubscriptor(...) \
	DefSubscriptor(ynoexcept, IWidget, __VA_ARGS__) \
	DefGetter(const ynothrow, size_t, ChildrenCount, \
		ystdex::sizeof_params(__VA_ARGS__))

/*!
\def DefWidgetSubscriptorBase
\brief 基于基类的部件成员下标访问操作。
\since build 357
*/
#define DefWidgetSubscriptorBase(_tBase, ...) \
	DefSubscriptorBase(ynoexcept, IWidget, _tBase, _tBase::GetChildrenCount(), \
		__VA_ARGS__) \
	DefGetter(const ynothrow, size_t, ChildrenCount, \
		_tBase::GetChildrenCount() + ystdex::sizeof_params(__VA_ARGS__))

/*!
\def DefWidgetBegin
\brief 使用指定部件引用和索引定义迭代器起始。
\since build 356
*/
#define DefWidgetBegin(_tIter) \
	_tIter \
	GetBegin() \
	{ \
		return _tIter(*this, 0); \
	}
/*!
\def DefWidgetEnd
\brief 使用指定部件引用和索引定义迭代器结束。
\since build 356
*/
#define DefWidgetEnd(_tIter) \
	_tIter \
	GetEnd() \
	{ \
		return _tIter(*this, GetChildrenCount()); \
	}

/*!
\def DefWidgetChildrenGetter
\brief 定义部件成员迭代器范围操作。
\since build 357

使用 GetBegin() 和 GetEnd() 实现 GetChildren() 。
*/
#define DefWidgetChildrenGetter \
	DefGetter(override, WidgetRange, Children, \
		WidgetRange(GetBegin(), GetEnd()))

/*!
\def DefWidgetMemberIterationOperations
\brief 定义部件成员迭代器起始、终止和范围操作。
\since build 356
*/
#define DefWidgetMemberIterationOperations(_tIter) \
	DefWidgetBegin(_tIter) \
	DefWidgetEnd(_tIter) \
	DefWidgetChildrenGetter

/*!
\def DefWidgetMemberIteration
\brief 定义部件成员下标及部件成员迭代器起始、终止和范围操作。
\since build 356
*/
#define DefWidgetMemberIteration(...) \
	DefWidgetMemberIterationOperations(Iterator) \
	DefWidgetSubscriptor(__VA_ARGS__)
/*!
\def DefWidgetMemberIterationBase
\brief 定义基于基类的部件成员下标及部件成员迭代器起始、终止和范围操作。
\since build 357
*/
#define DefWidgetMemberIterationBase(_tBase, ...) \
	DefWidgetMemberIterationOperations(Iterator) \
	DefWidgetSubscriptorBase(_tBase, __VA_ARGS__)

} // namespace UI;

} // namespace YSLib;

#endif

