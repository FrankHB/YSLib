/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file deref_op.hpp
\ingroup YStandardEx
\brief 解引用操作。
\version r148
\author FrankHB <frankhb1989@gmail.com>
\since build 576
\par 创建时间:
	2015-02-10 13:12:26 +0800
\par 修改时间:
	2016-04-25 09:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::DereferenceOperation
*/


#ifndef YB_INC_ystdex_deref_op_hpp_
#define YB_INC_ystdex_deref_op_hpp_ 1

#include "../ydef.h"

namespace ystdex
{

/*!	\defgroup is_undereferenceable Is Undereferenceable Iterator
\brief 判断迭代器实例是否不可解引用。
\tparam _tIter 迭代器类型。
\note 注意返回 \c false 不表示参数实际可解引用。
\since build 461
*/
//@{
template<typename _tIter>
yconstfn bool
is_undereferenceable(const _tIter&) ynothrow
{
	return {};
}
template<typename _type>
yconstfn bool
is_undereferenceable(_type* p) ynothrow
{
	return !p;
}
//@}


//! \since build 689
//@{
//! \brief 取非空引用或默认值。
//@{
template<typename _tOther, typename _type>
yconstfn auto
nonnull_or(_type p, _tOther&& other = {}) -> decltype(p ? p : other)
{
	return p ? p : other;
}
template<typename _tOther, typename _type, typename _tNull = nullptr_t>
yconstfn auto
nonnull_or(_type p, _tOther&& other, _tNull null)
	-> decltype(!bool(p == null) ? p : other)
{
	return !bool(p == null) ? p : other;
}
//@}


//! \brief 取非空值或默认值。
//@{
template<typename _tOther, typename _type>
yconstfn auto
value_or(_type p, _tOther&& other = {}) -> decltype(p ? *p : other)
{
	return p ? *p : other;
}
template<typename _tOther, typename _type, typename _tSentinal = nullptr_t>
yconstfn auto
value_or(_type p, _tOther&& other, _tSentinal last)
	-> decltype(!bool(p == last) ? *p : other)
{
	return !bool(p == last) ? *p : other;
}
//@}


//! \brief 调用非空值或取默认值。
//@{
template<typename _tOther, typename _func, typename _type>
yconstfn auto
call_value_or(_func f, _type p, _tOther&& other = {})
	-> decltype(p ? f(*p) : other)
{
	return p ? f(*p) : other;
}
template<typename _tOther, typename _func, typename _type,
	typename _tSentinal = nullptr_t>
yconstfn auto
call_value_or(_func f, _type p, _tOther&& other, _tSentinal last)
	-> decltype(!bool(p == last) ? f(*p) : other)
{
	return !bool(p == last) ? f(*p) : other;
}
//@}
//@}

} // namespace ystdex;

#endif

