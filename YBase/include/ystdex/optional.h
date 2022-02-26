/*
	© 2015-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file optional.h
\ingroup YStandardEx
\brief 可选值包装类型。
\version r1341
\author FrankHB <frankhb1989@gmail.com>
\since build 590
\par 创建时间:
	2015-04-09 21:35:21 +0800
\par 修改时间:
	2022-02-15 06:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Optonal
\see ISO C++17 [optional] 。
\see WG21 N4606 20.6[optional] 。
\see http://www.boost.org/doc/libs/1_57_0/libs/optional/doc/html/optional/reference.html 。

提供 ISO C++17 标准库头 <optional> 兼容的替代接口和实现。
除了部分关系操作使用 operators 实现而不保留命名空间内的声明外，
其它接口除命名空间成员扩展外，同 std::optional 。
注意因为一些兼容问题， std::experimental::optional 不被可选地使用，
	即使在 placement.hpp 中仍然会被检查而使用其中的标签类型。
和 std::experimental::optional 有以下不同：
LWG 2451 ：支持可选 explicit 的重载构造函数和赋值操作符。
LWG 2806 ：异常 bad_optional_access 的基类从 std::logic_error
	修改为 std::exception 。
LWG 2857 ： optional 的成员函数模板 emplace 不返回值修改为返回新构造的引用。
（另见 LEWG 72 ：https://issues.isocpp.org/show_bug.cgi?id=72 。）
WG21 N3765 ：支持不同的比较操作。
此外，限于核心语言特性的支持， LWG 2740 的解不被支持；
即 ystedex::operator-> 不总是提供和 ISO C++17 对 std::optional 相同的保证；
参见 ystdex::optional 成员 operator-> 的说明。
*/


#ifndef YB_INC_ystdex_optional_h_
#define YB_INC_ystdex_optional_h_ 1

#include "swap.hpp" // for conditional <optional>,
//	__cpp_inheriting_constructors, tagged_value, ystdex::swap_dependent;
#if YB_Has_optional != 1
#	include "operators.hpp" // for or_, is_trivially_destructible, is_cv,
//	std::move, empty_base, is_nothrow_moveable, and_, nullptr_t, remove_cv_t,
//	totally_ordered, not_, nand_, is_reference, is_same,
//	is_nothrow_destructible, is_object, enable_if_t, is_constructible, decay_t,
//	is_nothrow_swappable, ystdex::addressof, is_copyable;
#	include <initializer_list> // for std::initializer_list;
#	include <stdexcept> // for std::logic_error;
#endif
#include "functor.hpp" // for std::accumulate, default_last_value, std::hash;

namespace ystdex
{

/*!
\ingroup YBase_replacement_extensions metafunctions
\brief 保留可选的比较操作对象类型的比较操作结果类型。
\note YBase optional 扩展。
\since build 831
*/
template<typename _type>
using optional_relop_t = enable_if_t<is_convertible<_type, bool>::value, bool>;

#if YB_Has_optional != 1
//! \since build 831
inline namespace cpp2017
{

template<typename>
class optional;

} // inline namespace 2017;

//! \since build 591
//@{
namespace details
{

//! \since build 831
//@{
template<typename _type, typename _tOther>
using converts_from_optional = or_<is_constructible<_type,
	const optional<_tOther>&>, is_constructible<_type, optional<_tOther>&>,
	is_constructible<_type, const optional<_tOther>>, is_constructible<_type,
	optional<_tOther>>, is_convertible<const optional<_tOther>&, _type>,
	is_convertible<optional<_tOther>&, _type>, is_convertible<const
	optional<_tOther>&&, _type>, is_convertible<optional<_tOther>&&, _type>>;

template<typename _type, typename _tOther>
using assigns_from_optional = or_<is_assignable<_type&, const
	optional<_tOther>&>, is_assignable<_type&, optional<_tOther>&>,
	is_assignable<_type&, const optional<_tOther>&&>,
	is_assignable<_type&, optional<_tOther>&&>>;
//@}

template<typename _type, bool = is_trivially_destructible<_type>::value>
class optional_base : public optional_base<_type, true>
{
public:
#if __cpp_inheriting_constructors < 201511L \
	|| (YB_IMPL_GNUCPP >= 80000 && YB_IMPL_GNUCPP < 80200)
	// See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85792.
	yconstfn
	optional_base() = default;
#endif
	//! \since build 601
	using optional_base<_type, true>::optional_base;
	//! \since build 820
	yconstfn
	optional_base(const optional_base&) = default;
	//! \since build 820
	yconstfn
	optional_base(optional_base&&) = default;
	~optional_base()
	{
		if(this->has_value())
			this->destroy_nothrow();
	}

	optional_base&
	operator=(const optional_base& s)
	{
		return this->assign(s.value);
	}
	optional_base&
	operator=(optional_base&& s)
	{
		return this->assign(std::move(s.value));
	}
};

template<typename _type>
class optional_base<_type, true> : protected tagged_value<bool, _type>
{
	// NOTE: See WG21 N3770 comment FI 15, also CWG 1776.
	static_assert(!is_cv<_type>::value, "Cv-qualified type found.");

protected:
	//! \since build 718
	using base = tagged_value<bool, _type>;

	//! \since build 718
	using base::value;

public:
	yconstfn
	optional_base() = default;
	template<typename... _tParams>
	explicit yconstfn
	optional_base(in_place_t, _tParams&&... args)
		: base(true, in_place, yforward(args)...)
	{}
	yconstfn_relaxed
	optional_base(const optional_base& s)
		: base(s)
	{
		if(has_value())
			base::construct(s.value);
	}
	//! \since build 718
	yconstfn_relaxed
	optional_base(optional_base&& s)
		: base(s)
	{
		if(has_value())
			base::construct(std::move(s.value));
	}
	/*!
	\brief 析构：空实现。
	\since build 609
	*/
	~optional_base() = default;

	optional_base&
	operator=(const optional_base& s)
	{
		return assign(s.value);
	}
	//! \since build 630
	optional_base&
	operator=(optional_base&& s) ynoexcept(is_nothrow_moveable<_type>())
	{
		return assign(std::move(s.value));
	}

	//! \since build 630
	template<typename _tParam>
	optional_base&
	assign(_tParam&& arg) ynoexcept(and_<is_rvalue_reference<_tParam&&>,
		is_nothrow_moveable<_type>>())
	{
		if(has_value() && arg.has_value())
			value = yforward(arg);
		else if(arg.has_value())
			construct(yforward(arg));
		else
			reset();
		return *this;
	}

	template<typename... _tParams>
	void
	construct(_tParams&&... args)
	{
		base::construct(yforward(args)...);
		base::token = true;
	}

	void
	destroy() ynothrow
	{
		base::token = {};
		base::destroy_nothrow();
	}

	yconstfn_relaxed _type&
	get() ynothrow
	{
		return value;
	}
	//! \since build 831
	yconstfn const _type&
	get() const ynothrow
	{
		return value;
	}

	//! \since build 718
	bool
	has_value() const ynothrow
	{
		return base::token;
	}

	void
	reset() ynothrow
	{
		if(has_value())
			destroy();
	}
};

} // namespace details;
#endif


//! \since build 831
inline namespace cpp2017
{

#if YB_Has_optional == 1
//! \since build 831
//@{
using std::optional;
using std::bad_optional_access;
using std::nullopt_t;
using std::nullopt;
using std::make_optional;
//@}
#elif YB_Has_optional == 2
//! \since build 831
//@{
using std::experimental::optional;
using std::experimental::bad_optional_access;
using std::experimental::nullopt_t;
using std::experimental::nullopt;
using std::experimental::make_optional;
//@}
#else
//! \since build 590
//@{
//! \see WG21 N4606 20.6.4[optional.nullopt] 。
//@{
/*!
\ingroup tags
\brief 无值状态指示。
*/
//@{
#if YB_IMPL_MSCPP
struct nullopt_t
#else
yconstexpr_inline const struct nullopt_t
#endif
{
	/*!
	\see LWG 2510 。
	\since build 718
	*/
	yimpl(explicit yconstfn
	nullopt_t(nullptr_t)
	{})
#if YB_IMPL_MSCPP
};

yconstexpr_inline const nullopt_t nullopt{yimpl(nullptr)};
#else
} nullopt{yimpl(nullptr)};
#endif
//@}


/*!
\ingroup YBase_replacement_features exceptions
\brief 可选值操作失败异常。
\see ISO C++17 [optional.bad_optional_access] 。
\see LWG 2806 。
\see LEWG 72 ：https://issues.isocpp.org/show_bug.cgi?id=72 。
*/
class YB_API bad_optional_access : public std::exception
{
public:
	bad_optional_access()
		: exception()
	{}
	//! \since build 841
	bad_optional_access(const bad_optional_access&) = default;
	~bad_optional_access() override;

	/*!
	\return 实现定义："bad optional access" 。
	\since build 831
	*/
	YB_ATTR_returns_nonnull yimpl(YB_STATELESS) virtual const char*
	what() const ynothrow override;
};
//@}


/*!
\ingroup YBase_replacement_features
\brief 可选值对象包装。
\note 值语义。基本接口和语义同 std::experimental::optional 提议
	和 boost::optional （对应接口以前者为准）。
\warning 非虚析构。
\see WG21 N4606 20.6.3[optional.object] 。
\todo allocator_arg 支持。
\todo 符合 WG21 N4700 要求的 explicit 重载。
*/
template<typename _type>
class optional : private details::optional_base<remove_cv_t<_type>>, yimpl(
	private totally_ordered<optional<_type>, nullopt_t>)
{
	//! \see WG21 N4606 20.6.2[optional.synopsis]/1 。
	static_assert(!or_<is_reference<_type>, is_same<remove_cv_t<_type>,
		in_place_t>, is_same<remove_cv_t<_type>, in_place_t>,
		is_same<remove_cv_t<_type>, nullopt_t>>(), "Invalid type found.");
	//! \see WG21 N4606 20.6.3[optional.object]/3 。
	static_assert(and_<is_nothrow_destructible<_type>, is_object<_type>>(),
		"Invalid type found.");

public:
	using value_type = _type;

private:
	using base = details::optional_base<_type>;

public:
	yconstfn
	optional() ynothrow
	{}
	yconstfn
	optional(nullopt_t) ynothrow
	{}
	/*!
	\since build 831
	\see LWG 2756 。
	*/
	//@{
	template<typename _tOther = _type, yimpl(enable_if_t<and_<not_<is_same<
		optional<_type>, decay_t<_tOther>>>, not_<is_same<in_place_t,
		decay_t<_tOther>>>, is_constructible<_type, _tOther>,
		is_convertible<_tOther&&, _type>>::value, bool> = true)>
	yconstfn
	optional(_tOther&& v)
		: base(in_place, yforward(v))
	{}
	template<typename _tOther = _type, yimpl(enable_if_t<and_<not_<is_same<
		optional<_type>, decay_t<_tOther>>>, not_<is_same<in_place_t,
		decay_t<_tOther>>>, is_constructible<_type, _tOther>, not_<
		is_convertible<_tOther&&, _type>>>::value, bool> = false)>
	explicit yconstfn
	optional(_tOther&& v)
		: base(in_place, yforward(v))
	{}
	template<typename _tOther, yimpl(enable_if_t<and_<not_<is_same<_type,
		_tOther>>, is_constructible<_type, const _tOther&>, is_convertible<const
		_tOther&, _type>, not_<
		details::converts_from_optional<_type, _tOther>>>::value, bool> = true)>
	yconstfn_relaxed
	optional(const optional<_tOther>& t)
	{
		// XXX: Need to support non copyable and movable objects before mandated
		//	copy ellision available in ISO C++17.
		if(t)
			emplace(*t);
	}
	template<typename _tOther, yimpl(enable_if_t<and_<not_<is_same<_type,
		_tOther>>, is_constructible<_type, const _tOther&>, not_<is_convertible<
		const _tOther&, _type>>, not_<details::converts_from_optional<_type,
		_tOther>>>::value, bool> = false)>
	explicit yconstfn_relaxed
	optional(const optional<_tOther>& t)
	{
		// XXX: Ditto.
		if(t)
			emplace(*t);
	}
	template<typename _tOther, yimpl(enable_if_t<and_<not_<is_same<_type,
		_tOther>>, is_constructible<_type, _tOther>, is_convertible<_tOther&&,
		_type>, not_<details::converts_from_optional<_type, _tOther>>>::value,
		bool> = true)>
	yconstfn_relaxed
	optional(optional<_tOther>&& t)
	{
		// XXX: Ditto.
		if(t)
			emplace(std::move(*t));
	}
	template<typename _tOther, yimpl(enable_if_t<and_<not_<is_same<_type,
		_tOther>>, is_constructible<_type, _tOther>, not_<is_convertible<
		_tOther&&, _type>>, not_<details::converts_from_optional<_type,
		_tOther>>>::value, bool> = false)>
	explicit yconstfn_relaxed
	optional(optional<_tOther>&& t)
	{
		// XXX: Ditto.
		if(t)
			emplace(std::move(*t));
	}
	//@}
	template<typename... _tParams, yimpl(
		typename = enable_if_t<is_constructible<_type, _tParams...>::value>)>
	explicit yconstfn
	optional(in_place_t, _tParams&&... args)
		: base(in_place, yforward(args)...)
	{}
	template<typename _tOther, typename... _tParams,
		yimpl(typename = enable_if_t<is_constructible<_type,
		std::initializer_list<_tOther>&, _tParams&&...>::value>)>
	explicit yconstfn
	optional(in_place_t, std::initializer_list<_tOther> il, _tParams&&... args)
		: base(in_place, il, yforward(args)...)
	{}
	yconstfn
	optional(const optional&) yimpl(= default);
	yconstfn
	optional(optional&& o) ynoexcept(is_nothrow_move_constructible<_type>())
		: base(std::move(o))
	{}
	~optional() yimpl(= default);

	optional&
	operator=(nullopt_t) ynothrow
	{
		reset();
		return *this;
	}
	optional&
	operator=(const optional& o)
	{
		get_base() = o.get_base();
		return *this;
	}
	//! \since build 630
	optional&
	operator=(optional&& o) ynoexcept(is_nothrow_moveable<_type>())
	{
		get_base() = std::move(o.get_base());
		return *this;
	}
	/*!
	\see LWG 2756 。
	\since build 831
	*/
	//@{
	template<typename _tOther = _type>
	enable_if_t<and_<not_<is_same<optional<_type>, decay_t<_tOther>>>,
		is_constructible<_type, _tOther>, nand_<is_scalar<_type>,
		is_same<_type, decay_t<_tOther>>>,
		is_assignable<_type&, _tOther>>::value, optional&>
	operator=(_tOther&& v)
	{
		if(has_value())
			this->get() = yforward(v);
		else
			this->construct(yforward(v));
		return *this;
	}
	template<typename _tOther>
	yimpl(enable_if_t)<and_<not_<is_same<_type, _tOther>>,
		is_constructible<_type, const _tOther&>, is_assignable<_type&, _tOther>,
		not_<details::converts_from_optional<_type, _tOther>>,
		not_<details::assigns_from_optional<_type, _tOther>>>::value, optional&>
	operator=(const optional<_tOther>& u)
	{
		if(u)
		{
			if(this->has_value())
				this->get() = *u;
			else
				this->construct(*u);
		}
		else
			this->reset();
		return *this;
	}
	template<typename _tOther>
	enable_if_t<and_<not_<is_same<_type, _tOther>>,
		is_constructible<_type, _tOther>, is_assignable<_type&, _tOther>,
		not_<details::converts_from_optional< _type, _tOther>>,
		not_<details::assigns_from_optional<_type, _tOther>>>::value, optional&>
	operator=(optional<_tOther>&& u)
	{
		if(u)
		{
			if(this->has_value())
				this->get() = std::move(*u);
			else
				this->construct(std::move(*u));
		}
		else
			this->reset();
		return *this;
	}
	//@}

	/*!
	\since build 831
	\see LWG 2857 。
	*/
	//{@
	template<typename... _tParams>
	_type&
	emplace(_tParams&&... args)
	{
		reset();
		this->construct(yforward(args)...);
		return this->get();
	}
	template<typename _tOther, typename... _tParams>
	_type&
	emplace(std::initializer_list<_tOther> il, _tParams&&... args)
	{
		reset();
		this->construct(il, yforward(args)...);
		return this->get();
	}
	//@}

private:
	base&
	get_base() ynothrow
	{
		return static_cast<base&>(*this);
	}
	const base&
	get_base() const ynothrow
	{
		return static_cast<const base&>(*this);
	}

public:
	void
	swap(optional& o) ynoexcept(and_<is_nothrow_move_constructible<value_type>,
		is_nothrow_swappable<value_type>>())
	{
		if(has_value() && o.has_value())
			ystdex::swap_dependent(this->get(), o.get());
		else if(has_value())
		{
			o.construct(std::move(this->get()));
			this->destroy();
		}
		else if(o.has_value())
		{
			this->construct(std::move(this->get()));
			o.destroy();
		}
	}

	/*!
	\warning ISO C++17 前不保证常量表达式要求。
	\see LWG 2740 。
	*/
	//@{
	yconstfn_relaxed _type*
	operator->()
	{
		return ystdex::addressof(this->get());
	}
	yconstfn const _type*
	operator->() const
	{
		return ystdex::addressof(this->get());
	}
	//@}

	yconstfn_relaxed _type&
	operator*() &
	{
		return this->get();
	}
	yconstfn const _type&
	operator*() const&
	{
		return this->get();
	}
	//! \since build 675
	yconstfn_relaxed _type&&
	operator*() &&
	{
		return std::move(this->get());
	}
	//! \since build 834
	yconstfn const _type&&
	operator*() const&&
	{
		return std::move(this->get());
	}

	explicit yconstfn
	operator bool() const ynothrow
	{
		return has_value();
	}

	//! \since build 718
	using base::has_value;

	yconstfn_relaxed _type&
	value() &
	{
		return has_value() ? this->get()
			: (throw bad_optional_access(), this->get());
	}
	yconstfn const _type&
	value() const&
	{
		return has_value() ? this->get()
			: (throw bad_optional_access(), this->get());
	}
	//! \since build 675
	yconstfn_relaxed _type&&
	value() &&
	{
		return has_value() ? std::move(this->get())
			: (throw bad_optional_access(), std::move(this->get()));
	}
	//! \since build 675
	yconstfn _type&&
	value() const&&
	{
		return has_value() ? std::move(this->get())
			: (throw bad_optional_access(), std::move(this->get()));
	}

	template<typename _tOther>
	yconstfn _type
	value_or(_tOther&& other) const&
	{
		static_assert(is_copyable<_type>(), "Invalid type found.");

		return has_value() ? this->get() : static_cast<_type>(yforward(other));
	}
	template<typename _tOther>
	yconstfn_relaxed _type
	value_or(_tOther&& other) &&
	{
		static_assert(is_copyable<_type>(), "Invalid type found.");

		return has_value() ? std::move(this->get())
			: static_cast<_type>(yforward(other));
	}

	//! \since build 718
	using base::reset;
};

/*!
\relates optional
\since build 831
*/
//@{
//! \brief 关系和比较操作。
//@{
//! \see ISO C++17 [optional.relops] 。
//@{
template<typename _type, typename _tOther>
yconstfn auto
operator==(const optional<_type>& x, const optional<_tOther>& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() == std::declval<_tOther>())>
{
	return bool(x) == bool(y) && (!x || *x == *y);
}

template<typename _type, typename _tOther>
yconstfn auto
operator!=(const optional<_type>& x, const optional<_tOther>& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() != std::declval<_tOther>())>
{
	return bool(x) != bool(y) || (bool(x) && *x != *y);
}

template<typename _type, typename _tOther>
yconstfn auto
operator<(const optional<_type>& x, const optional<_tOther>& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() < std::declval<_tOther>())>
{
	return bool(y) && (!x || *x < *y);
}

template<typename _type, typename _tOther>
yconstfn auto
operator>(const optional<_type>& x, const optional<_tOther>& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() > std::declval<_tOther>())>
{
	return bool(x) && (!y || *x > *y);
}

template<typename _type, typename _tOther>
yconstfn auto
operator<=(const optional<_type>& x, const optional<_tOther>& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() <= std::declval<_tOther>())>
{
	return !x || (bool(y) && *x <= *y);
}

template<typename _type, typename _tOther>
yconstfn auto
operator>=(const optional<_type>& x, const optional<_tOther>& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() >= std::declval<_tOther>())>
{
	return !y || (bool(x) && *x >= *y);
}
//@}

/*!
\note 未显式声明的部分由 \c operators 提供实现。
\see ISO C++17 [optional.nullops] 。
*/
//@{
template<typename _type>
yconstfn bool
operator==(const optional<_type>& x, nullopt_t) ynothrow
{
	return !x;
}

template<typename _type>
yconstfn bool
operator<(const optional<_type>&, nullopt_t) ynothrow
{
	return {};
}
template<typename _type>
yconstfn bool
operator<(nullopt_t, const optional<_type>& y) ynothrow
{
	return bool(y);
}
//@}

//! \see ISO C++17 [optional.comp_with_t] 。
//@{
template<typename _type, typename _tOther>
yconstfn auto
operator==(const optional<_type>& x, const _tOther& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() == std::declval<_tOther>())>
{
	return x && *x == y;
}
template<typename _type, typename _tOther>
yconstfn auto
operator==(const _tOther& x, const optional<_type>& y)
	-> optional_relop_t<decltype(
	std::declval<_tOther>() == std::declval<_type>())>
{
	return y && x == *y;
}

template<typename _type, typename _tOther>
yconstfn auto
operator!=(const optional<_type>& x, const _tOther& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() != std::declval<_tOther>())>
{
	return !x || *x != y;
}
template<typename _type, typename _tOther>
yconstfn auto
operator!=(const _tOther& x, const optional<_type>& y)
	-> optional_relop_t<decltype(
	std::declval<_tOther>() != std::declval<_type>())>
{
	return !y || x != *y;
}

template<typename _type, typename _tOther>
yconstfn auto
operator<(const optional<_type>& x, const _tOther& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() < std::declval<_tOther>())>
{
	return !x || *x < y;
}
template<typename _type, typename _tOther>
yconstfn auto
operator<(const _tOther& x, const optional<_type>& y)
	-> optional_relop_t<decltype(
	std::declval<_tOther>() < std::declval<_type>())>
{
	return y && x < *y;
}

template<typename _type, typename _tOther>
yconstfn auto
operator>(const optional<_type>& x, const _tOther& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() > std::declval<_tOther>())>
{
	return x && *x > y;
}
template<typename _type, typename _tOther>
yconstfn auto
operator>(const _tOther& x, const optional<_type>& y)
	-> optional_relop_t<decltype(
	std::declval<_tOther>() > std::declval<_type>())>
{
	return !y || x > *y;
}

template<typename _type, typename _tOther>
yconstfn auto
operator<=(const optional<_type>& x, const _tOther& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() <= std::declval<_tOther>())>
{
	return !x || *x <= y;
}
template<typename _type, typename _tOther>
yconstfn auto
operator<=(const _tOther& x, const optional<_type>& y)
	-> optional_relop_t<decltype(
	std::declval<_tOther>() <= std::declval<_type>())>
{
	return y && x <= *y;
}

template<typename _type, typename _tOther>
yconstfn auto
operator>=(const optional<_type>& x, const _tOther& y)
	-> optional_relop_t<decltype(
	std::declval<_type>() >= std::declval<_tOther>())>
{
	return x && *x >= y;
}
template<typename _type, typename _tOther>
yconstfn auto
operator>=(const _tOther& x, const optional<_type>& y)
	-> optional_relop_t<decltype(
	std::declval<_tOther>() >= std::declval<_type>())>
{
	return !y || x >= *y;
}
//@}

//! \see WG21 N4606 20.6.9[optional.specalg] 。
//@{
template<typename _type> void
swap(optional<_type>& x, optional<_type>& y) ynoexcept_spec(x.swap(y))
{
	return x.swap(y);
}

/*!
\ingroup helper_functions
\brief 创建 optional 对象。
\see WG21 P0032R3 。
\since build 717
*/
//@{
template<typename _type, typename... _tParams>
optional<_type>
make_optional(_tParams&&... args)
{
	return optional<_type>(in_place, yforward(args)...);
}
template<typename _type, typename _tOther, typename... _tParams>
optional<_type>
make_optional(std::initializer_list<_tOther> il, _tParams&&... args)
{
	return optional<_type>(in_place, il, yforward(args)...);
}
//@}
//@}
#endif

/*!
\note YBase optional 扩展。
\since build 655
*/
//@{
//! \brief 使用指定参数构造指定 optional 实例的值。
template<typename _type, typename... _tParams>
yconstfn optional<_type>
make_optional_inplace(_tParams&&... args)
{
	return optional<_type>(in_place, yforward(args)...);
}

/*!
\ingroup YBase_replacement_extensions
\brief 从 optional 实例的临时对象取左值引用。
\warning 应仅在确认生存期时使用。
*/
template<typename _type>
yconstfn _type&
ref_opt(optional<_type>&& o) ynothrowv
{
	return *o.operator->();
}
template<typename _type>
yconstfn _type&
ref_opt() ynothrowv
{
	return ref_opt(make_optional_inplace<_type>());
}
//@}


/*!
\ingroup YBase_replacement_extensions
\brief 合并可选值序列。
\note 语义同 Boost.Signal2 的 \c boost::optional_last_value 。
\since build 675
*/
//@{
template<typename _type>
struct optional_last_value
{
	template<typename _tIn>
	optional<_type>
	operator()(_tIn first, _tIn last) const
	{
		return std::accumulate(first, last, optional<_type>(),
			[](optional<_type>&, decltype(*first) val){
			return yforward(val);
		});
	}
};

template<>
struct optional_last_value<void> : default_last_value<void>
{};
//@}
//@}

} // inline namespace cpp2017;

} // namespace ystdex;

#if YB_Has_optional != 1
namespace std
{

/*!
\brief ystdex::optional 散列支持。
\see WG21 N4606 20.6.10[optional.hash] 。
\since build 591
*/
template<typename _type>
struct hash<ystdex::optional<_type>>
{
	size_t
	operator()(const ystdex::optional<_type>& k) const
		yimpl(ynoexcept_spec(hash<_type>{}(*k)))
	{
		// NOTE: The unspecified value is randomly picked.
		return k ? hash<_type>{}(*k) : yimpl(-4242);
	}
};

} // namespace std;
#endif

#endif

