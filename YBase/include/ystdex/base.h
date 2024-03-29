﻿/*
	© 2013-2015, 2017-2019, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file base.h
\ingroup YStandardEx
\brief 基类实用设施。
\version r262
\author FrankHB <frankhb1989@gmail.com>
\since build 556
\par 创建时间:
	2014-11-28 11:59:15 +0800
\par 修改时间:
	2022-02-08 03:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Base

提供若干基类。
一些基类相对显式定义 = delete 的特殊成员函数兼容性更友好，
	避免如 Clang++ 警告 [-Wdeprecated] 的问题。
一些基类被设计为空类（非虚析构），以允许 ISO C++11 保证对标准布局的类启用空基类优化；
	参见 ISO C++11 9.1/7 和 ISO C++11 9.2/19 ，以及
	https://stackoverflow.com/questions/10788823 的讨论。
ISO C++20 的规则（自 WG21 P840R2 ）进一步明确了空基类的情形，但没有实质变化。
Microsoft VC++ 因为 ABI 兼容问题不会在短期实现符合 ISO C++11 要求的空基类优化，
	而同时需要使用 __declspec(empty_bases) 变通，参见：	
https://devblogs.microsoft.com/cppblog/optimizing-the-layout-of-empty-base-classes-in-vs2015-update-2-3/ ；
https://developercommunity.visualstudio.com/t/no-unique-address-attribute-isnt-working/954963 。
*/


#ifndef YB_INC_ystdex_base_h_
#define YB_INC_ystdex_base_h_ 1

#include "../ydef.h"

namespace ystdex
{

/*!
\brief 隔离 ADL 的基类命名空间。
\note 避免基类在 ADL 时引入 ystdex 命名空间下的名称。
\since build 834
*/
namespace bases
{

//! \warning 非虚析构。
//@{
/*!
\brief 不可复制对象：禁止派生类调用默认原型的复制构造函数和复制赋值操作符。
\note WG21 N2600 在 ISO C++11 之前提议一个更简单的实现，但没有被标准采纳。
\since build 373
\see WG21 N2600 。
*/
class noncopyable
{
protected:
	/*!
	\brief \c protected 构造：默认实现。
	\note 保护非多态类。
	*/
	yconstfn
	noncopyable() = default;
	//! \brief \c protected 析构：默认实现。
	~noncopyable() = default;

public:
	//! \brief 禁止复制构造。
	yconstfn
	noncopyable(const noncopyable&) = delete;
	/*!
	\brief 允许转移构造。
	\since build 551
	*/
	yconstfn
	noncopyable(noncopyable&&) = default;

	//! \brief 禁止复制赋值。
	noncopyable&
	operator=(const noncopyable&) = delete;
	/*!
	\brief 允许转移赋值。
	\since build 551
	*/
	noncopyable&
	operator=(noncopyable&&) = default;
};


/*!
\brief 不可转移对象：禁止继承此类的对象调用默认原型的转移构造函数和转移赋值操作符。
\warning 非虚析构。
\since build 373
*/
class nonmovable
{
protected:
	/*!
	\brief \c protected 构造：默认实现。
	\note 保护非多态类。
	*/
	yconstfn
	nonmovable() = default;
	//! \brief \c protected 析构：默认实现。
	~nonmovable() = default;

public:
	//! \since build 551
	//@{
	//! \brief 允许复制构造。
	yconstfn
	nonmovable(const nonmovable&) = default;
	//! \brief 禁止转移构造。
	yconstfn
	nonmovable(nonmovable&&) = delete;

	//! \brief 允许复制赋值。
	nonmovable&
	operator=(const nonmovable&) = default;
	//! \brief 禁止转移赋值。
	nonmovable&
	operator=(nonmovable&&) = delete;
	//@}
};
//@}


/*!
\brief 可动态复制的抽象基类。
\since build 475
*/
class YB_API YB_ATTR_novtable cloneable
{
public:
	//! \since build 503
	cloneable() = default;
	//! \since build 503
	cloneable(const cloneable&) = default;
	//! \brief 虚析构：类定义外默认实现。
	virtual
	~cloneable();

	//! \since build 844
	cloneable&
	operator=(const cloneable&) = default;

	virtual cloneable*
	clone() const = 0;
};


/*!
\brief 间接操作：返回派生类自身引用。
\note 可用于 ystdex::indirect_input_iterator 和转换函数访问。
\since build 556
\todo 提供接口允许用户指定使用 ystdex::polymorphic_cast 等检查转换。
*/
template<typename _type>
struct deref_self
{
	_type&
	operator*() ynothrow
	{
		return *static_cast<_type*>(this);
	}
	const _type&
	operator*() const ynothrow
	{
		return *static_cast<const _type*>(this);
	}
	volatile _type&
	operator*() volatile ynothrow
	{
		return *static_cast<volatile _type*>(this);
	}
	const volatile _type&
	operator*() const volatile ynothrow
	{
		return *static_cast<const volatile _type*>(this);
	}
};


/*!
\brief 派生类实体。
\note 附加的模板参数保证不同的类型。
\warning 可能非虚析构：当且仅当基类非虚析构。
\since build 756
*/
template<class _tBase, typename...>
class derived_entity : public _tBase
{
public:
	using base = _tBase;

	derived_entity() = default;
	/*!
	\warning 语义视基类对象构造函数的形式可能不同。
	\sa __cpp_inheriting_constructors
	\see WG21 P0136R1 。
	*/
	using base::base;
	derived_entity(const base& b) ynoexcept_spec(base(b))
		: base(b)
	{}
	derived_entity(base&& b) ynoexcept_spec(base(std::move(b)))
		: base(std::move(b))
	{}
	derived_entity(const derived_entity&) = default;
	derived_entity(derived_entity&&) = default;

	derived_entity&
	operator=(const derived_entity&) = default;
	derived_entity&
	operator=(derived_entity&&) = default;
};

} // namespace bases;

//! \since build 834
//@{
using noncopyable = bases::noncopyable;

using nonmovable = bases::nonmovable;

using cloneable = bases::cloneable;

template<typename _type>
using deref_self = bases::deref_self<_type>;

template<class _tBase, typename... _tParams>
using derived_entity = bases::derived_entity<_tBase, _tParams...>;
//@}

} // namespace ystdex;

#endif

