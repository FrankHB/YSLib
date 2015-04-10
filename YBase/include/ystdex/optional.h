/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file optional.h
\ingroup YStandardEx
\brief 可选值包装类型。
\version r96
\author FrankHB <frankhb1989@gmail.com>
\since build 590
\par 创建时间:
	2015-04-09 21:35:21 +0800
\par 修改时间:
	2015-04-09 17:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Optonal

\see ISO WG21/N4081 5[optional] 。
\see http://www.boost.org/doc/libs/1_57_0/libs/optional/doc/html/optional/reference.html 。
*/


#ifndef YB_INC_ystdex_optional_h_
#define YB_INC_ystdex_optional_h_ 1

#include "type_op.hpp" // for std::is_trivially_destructible, ystdex::is_cv,
//	ystdex::empty_base, std::move, ystdex::or_, ystdex::is_reference,
//	ystdex::is_same, ystdex::remove_cv_t, ystdex::and_,
//	ystdex::is_nothrow_destructible, ystdex::is_object, std::move,
//	ystdex::enable_if_t, ystdex::is_constructible, ystdex::decay_t;
#include <memory> // for std::addressof;
#include <new> // for placement ::operator new from standard library;
#include <stdexcept> // for std::logic_error;
#include <initializer_list> // for std::initializer_list;

namespace ystdex
{

//! \since build 590
//@{
/*!
\brief 原地构造标记。
\see ISO WG21/N4081 5.4[optional.inplace] 。
*/
//@{
struct in_place_t
{};

yconstexpr in_place_t in_place{};
//@}


//! \see ISO WG21/N4081 5.5[optional.nullopt] 。
//@{
//! \brief 无值状态指示。
//@{
struct nullopt_t
{
	yimpl()
};

static_assert(std::is_literal_type<nullopt_t>(),
	"Invalid implementation found.");

yconstexpr nullopt_t nullopt(yimpl());
//@}


/*!
\ingroup exceptions
\brief 可选值操作失败异常。
\note 实现定义：<tt>what()</tt> 返回 "bad optional access" 。
\see ISO WG21/N4081 5.6[optional.bad_optional_access] 。
*/
class bad_optional_access : public std::logic_error
{
public:
	bad_optional_access()
		: logic_error("bad optional access")
	{}

	~bad_optional_access() override;
};
//@}

} // namespace ystdex;

#endif

