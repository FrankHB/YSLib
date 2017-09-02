/*
	© 2015-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file optional.h
\ingroup YStandardEx
\brief 可选值包装类型。
\version r806
\author FrankHB <frankhb1989@gmail.com>
\since build 590
\par 创建时间:
	2015-04-09 21:35:21 +0800
\par 修改时间:
	2017-08-25 17:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Optonal
\see WG21 N4606 20.6[optional] 。
\see http://www.boost.org/doc/libs/1_57_0/libs/optional/doc/html/optional/reference.html 。

除了部分关系操作使用 operators 实现而不保留命名空间内的声明外，
其它接口除命名空间成员扩展外，同 std::experimental::optional 。
*/


#ifndef YB_INC_ystdex_optional_h_
#define YB_INC_ystdex_optional_h_ 1

#include "operators.hpp" // for nullptr_t, is_trivially_destructible, is_cv,
//	std::move, empty_base, is_nothrow_moveable, and_, std::addressof,
//	remove_cv_t, totally_ordered, or_, is_reference, is_same,
//	is_nothrow_destructible, is_object, enable_if_t, is_constructible, decay_t,
//	is_nothrow_swappable, ystdex::constfn_addressof, is_copyable;
#include <stdexcept> // for std::logic_error;
#include "placement.hpp" // for tagged_value;
#include <initializer_list> // for std::initializer_list;
#include "functional.hpp" // for default_last_value, std::accumulate, std::hash;

namespace ystdex
{

//! \since build 590
//@{
//! \see WG21 N4606 20.6.4[optional.nullopt] 。
//@{
//! \brief 无值状态指示。
//@{
yconstexpr const struct nullopt_t
{
	/*!
	\see LWG 2510 。
	\since build 718
	*/
	yimpl(yconstfn
	nullopt_t(nullptr_t)
	{})
} nullopt{yimpl(nullptr)};

static_assert(std::is_literal_type<nullopt_t>(),
	"Invalid implementation found.");
//@}


/*!
\ingroup exceptions
\brief 可选值操作失败异常。
\note 实现定义：<tt>what()</tt> 返回 "bad optional access" 。
\see WG21 N4606 20.6.5[optional.bad_optional_access] 。
*/
class YB_API bad_optional_access : public std::logic_error
{
public:
	bad_optional_access()
		: logic_error("bad optional access")
	{}

	~bad_optional_access() override;
};
//@}

//! \since build 591
//@{
namespace details
{

template<typename _type, bool = std::is_trivially_destructible<_type>::value>
class optional_base : public optional_base<_type, true>
{
public:
	//! \since build 601
	using optional_base<_type, true>::optional_base;
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
	static_assert(!is_cv<_type>(), "Cv-qualified type found.");

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
	optional_base(const optional_base& s)
		: base(s)
	{
		if(has_value())
			base::construct(s.value);
	}
	//! \since build 718
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

	_type&
	get() ynothrow
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


/*!
\brief 可选值对象包装。
\note 值语义。基本接口和语义同 std::experimental::optional 提议
	和 boost::optional （对应接口以前者为准）。
\warning 非虚析构。
\see WG21 N4606 20.6.3[optional.object] 。
\todo allocator_arg 支持。
*/
template<typename _type>
class optional : private details::optional_base<remove_cv_t<_type>>, yimpl(
	private totally_ordered<optional<_type>>, public totally_ordered<optional<
	_type>, _type>, private totally_ordered<optional<_type>, nullopt_t>)
{
	//! \see WG21 N4606 20.6.2[optional.synopsis]/1 。
	static_assert(!or_<is_reference<_type>, is_same<remove_cv_t<_type>,
		in_place_t>, is_same<remove_cv_t<_type>, in_place_t>,
		is_same<remove_cv_t<_type>, nullopt_t>>(),
		"Invalid type found.");
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
	yconstfn
	optional(const _type& v)
		: base(in_place, v)
	{}
	yconstfn
	optional(_type&& v)
		: base(in_place, std::move(v))
	{}
	template<typename... _tParams>
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
	optional(const optional&) yimpl(= default);
	optional(optional&&) ynoexcept(is_nothrow_move_constructible<_type>())
		yimpl(= default);
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
	template<typename _tOther>
	yimpl(enable_if_t)<is_same<decay_t<_tOther>, _type>::value, optional&>
	operator=(_tOther&& v)
	{
		if(has_value())
			this->get() = yforward(v);
		else
			this->construct(yforward(v));
		return *this;
	}

	template<typename... _tParams>
	void
	emplace(_tParams&&... args)
	{
		reset();
		this->construct(yforward(args)...);
	}
	template<typename _tOther, typename... _tParams>
	void
	emplace(std::initializer_list<_tOther> il, _tParams&&... args)
	{
		reset();
		this->construct(il, yforward(args)...);
	}

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
		using std::swap;

		if(has_value() && o.has_value())
			swap(this->get(), o.get());
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

	yconstfn_relaxed _type*
	operator->()
	{
		return std::addressof(this->get());
	}
	yconstfn const _type*
	operator->() const
	{
		return ystdex::constfn_addressof(this->get());
	}

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
	//! \since build 675
	yconstfn _type&&
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

//! \relates optional
//@{
/*!
\brief 关系和比较操作。
\note 未显式声明的部分由 \c operators 提供实现。
\see WG21 N4606 20.6.6[optional.relops] 。
\see WG21 N4606 20.6.7[optional.nullops] 。
\see WG21 N4606 20.6.8[optional.comp_with_t] 。
*/
//@{
template<typename _type>
yconstfn bool
operator==(const optional<_type>& x, const optional<_type>& y)
{
	return bool(x) == bool(y) && (!x || *x == *y);
}
template<typename _type>
yconstfn bool
operator==(const optional<_type>& x, nullopt_t) ynothrow
{
	return !x;
}
template<typename _type>
yconstfn bool
operator==(const optional<_type>& x, const _type& v)
{
	return bool(x) ? *x == v : false;
}

template<typename _type>
yconstfn bool
operator<(const optional<_type>& x, const optional<_type>& y)
{
	return bool(y) && (!x || *x < *y);
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
\brief 从 optional 实例的临时对象取左值引用。
\note YStandardEx 扩展。
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
\brief 合并可选值序列。
\note YStandardEx 扩展。
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

} // namespace ystdex;


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

