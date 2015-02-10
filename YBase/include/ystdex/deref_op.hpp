/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file deref_op.hpp
\ingroup YStandardEx
\brief 解引用操作。
\version r59
\author FrankHB <frankhb1989@gmail.com>
\since build 576
\par 创建时间:
	2015-02-10 13:12:26 +0800
\par 修改时间:
	2015-02-10 13:15 +0800
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

} // namespace ystdex;

#endif

