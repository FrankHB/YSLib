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
\version r583
\author FrankHB <frankhb1989@gmail.com>
\since build 590
\par 创建时间:
	2015-04-09 21:35:21 +0800
\par 修改时间:
	2015-11-28 13:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Optonal
\see ISO WG21/N4081 5[optional] 。
\see http://www.boost.org/doc/libs/1_57_0/libs/optional/doc/html/optional/reference.html 。

除了部分关系操作使用 operators 实现而不保留命名空间内的声明外，
其它接口除命名空间成员扩展外，同 std::experimental::optional 。
*/


#ifndef YB_INC_ystdex_optional_h_
#define YB_INC_ystdex_optional_h_ 1

#include "utility.hpp" // for is_trivially_destructible, is_cv, std::move,
//	empty_base, is_nothrow_moveable, and_, remove_cv_t, or_, is_reference,
//	is_same, is_nothrow_destructible, is_object, enable_if_t, is_constructible,
//	decay_t, is_nothrow_swappable, is_copyable;
#include <stdexcept> // for std::logic_error;
#include <new> // for placement ::operator new from standard library;
#include "memory.hpp" // for std::addressof, ystdex::constfn_addressof;
#include "operators.hpp" // for totally_ordered, totally_ordered2;
#include <initializer_list> // for std::initializer_list;

namespace ystdex
{

//! \since build 590
//@{
/*!
\brief 原地构造标记。
\see ISO WG21/N4081 5.4[optional.inplace] 。
*/
yconstexpr const struct in_place_t{} in_place{};


//! \see ISO WG21/N4081 5.5[optional.nullopt] 。
//@{
//! \brief 无值状态指示。
//@{
yconstexpr const struct nullopt_t
{
	yimpl()
} nullopt{yimpl()};

static_assert(std::is_literal_type<nullopt_t>(),
	"Invalid implementation found.");
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
		if(this->engaged)
			this->destroy_raw();
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
class optional_base<_type, true>
{
	// NOTE: See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3770.html#FI15.
	//	Also http://wg21.cmeerw.net/cwg/issue1776.
	static_assert(!is_cv<_type>(), "Cv-qualified type found.");

protected:
	union
	{
		empty_base<> empty;
		mutable _type value;
	};
	bool engaged = {};

public:
	yconstfn
	optional_base() ynothrow
		: empty()
	{}
	template<typename... _tParams>
	explicit yconstfn
	optional_base(in_place_t, _tParams&&... args)
		: value(yforward(args)...), engaged(true)
	{}
	optional_base(const optional_base& s)
		: engaged(s.engaged)
	{
		if(engaged)
			construct_raw(s.value);
	}
	optional_base(const optional_base&& s)
		ynoexcept_spec(is_nothrow_move_constructible<_type>())
		: engaged(s.engaged)
	{
		if(engaged)
			construct_raw(std::move(s.value));
	}
	/*!
	\brief 析构：空实现。
	\note 不使用默认函数以避免派生此类的非平凡析构函数非合式而被删除。
	\since build 609
	*/
	~optional_base()
	{}

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
		if(engaged && arg.engaged)
			value = yforward(arg);
		else if(arg.engaged)
			construct(yforward(arg));
		else
			reset();
		return *this;
	}

	template<typename... _tParams>
	void
	construct(_tParams&&... args)
	{
		construct_raw(yforward(args)...);
		engaged = true;
	}

private:
	template<typename... _tParams>
	void
	construct_raw(_tParams&&... args)
	{
		::new(std::addressof(value)) _type(yforward(args)...);
	}

public:
	void
	destroy() ynothrow
	{
		engaged = {};
		destroy_raw();
	}

protected:
	void
	destroy_raw() ynothrow
	{
		ynoexcept_assert("Invalid type found.", value.~type());

		value.~_type();
	}

public:
	_type&
	get() ynothrow
	{
		return value;
	}

	bool
	is_engaged() const ynothrow
	{
		return engaged;
	}

	void
	reset() ynothrow
	{
		if(engaged)
			destroy();
	}
};

} // namespace details;


/*!
\brief 可选值对象包装。
\note 值语义。基本接口和语义同 std::experimental::optional 提议
	和 boost::optional （对应接口以前者为准）。
\warning 非虚析构。
\see ISO WG21/N4081 5.3[optional.object] 。
\todo allocator_arg 支持。
*/
template<typename _type>
class optional : private details::optional_base<remove_cv_t<_type>>, yimpl(
	public totally_ordered<optional<_type>>, public totally_ordered2<optional<
	_type>, _type>, public totally_ordered2<optional<_type>, nullopt_t>)
{
	//! \see ISO WG21/N4081 5.2[optional.synopsis]/1 。
	static_assert(!or_<is_reference<_type>, is_same<remove_cv_t<_type>,
		in_place_t>, is_same<remove_cv_t<_type>, nullopt_t>>(),
		"Invalid type found.");
	//! \see ISO WG21/N4081 5.3[optional.object]/3 。
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
		this->reset();
		return *this;
	}
	optional&
	operator=(const optional& o)
	{
		get_base() = o.get_base();
	}
	//! \since build 630
	optional&
	operator=(optional&& o) ynoexcept(is_nothrow_moveable<_type>())
	{
		get_base() = std::move(o.get_base());
	}
	template<typename _tOther>
	yimpl(enable_if_t)<is_same<decay_t<_tOther>, _type>::value, optional&>
	operator=(_tOther&& v)
	{
		if(this->is_engaged())
			this->get() = yforward(v);
		else
			this->construct(yforward(v));
	}

	template<typename... _tParams>
	void
	emplace(_tParams&&... args)
	{
		this->reset();
		this->construct(yforward(args)...);
	}
	template<typename _tOther, typename... _tParams>
	void
	emplace(std::initializer_list<_tOther> il, _tParams&&... args)
	{
		this->reset();
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

		if(this->is_engaged() && o.is_engaged())
			swap(this->get(), o.get());
		else if(this->is_engaged())
		{
			o.construct(std::move(this->get()));
			this->destroy();
		}
		else if(o.is_engaged())
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
	yconstfn_relaxed _type
	operator*() &&
	{
		return std::move(this->get());
	}
	yconstfn _type
	operator*() const&&
	{
		return std::move(this->get());
	}

	explicit yconstfn
	operator bool() const ynothrow
	{
		return this->is_engaged();
	}

	yconstfn_relaxed _type&
	value() &
	{
		return this->is_engaged() ? this->get() : (throw bad_optional_access(),
			this->get());
	}
	yconstfn const _type&
	value() const&
	{
		return this->is_engaged() ? this->get() : (throw bad_optional_access(),
			this->get());
	}
	yconstfn_relaxed _type
	value() &&
	{
		return this->is_engaged() ? std::move(this->get())
			: (throw bad_optional_access(), std::move(this->get()));
	}
	yconstfn _type
	value() const&&
	{
		return this->is_engaged() ? std::move(this->get())
			: (throw bad_optional_access(), std::move(this->get()));
	}

	template<typename _tOther>
	yconstfn _type
	value_or(_tOther&& other) const&
	{
		static_assert(is_copyable<_type>(), "Invalid type found.");

		return this->is_engaged() ? this->get()
			: static_cast<_type>(yforward(other));
	}
	template<typename _tOther>
	yconstfn_relaxed _type
	value_or(_tOther&& other) &&
	{
		static_assert(is_copyable<_type>(), "Invalid type found.");

		return this->is_engaged() ? std::move(this->get())
			: static_cast<_type>(yforward(other));
	}
};

//! \relates optional
//@{
/*!
\brief 关系和比较操作。
\note 未显式声明的部分由 \c operators 提供实现。
\see ISO WG21/N4081 5.7[optional.relops] 。
\see ISO WG21/N4081 5.8[optional.nullops] 。
\see ISO WG21/N4081 5.9[optional.comp_with_t] 。
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

//! \see ISO WG21/N4081 5.10[optional.specalg] 。
//@{
template<typename _type> void
swap(optional<_type>& x, optional<_type>& y) ynoexcept_spec(x.swap(y))
{
	return x.swap(y);
}
template<typename _type>
yconstfn optional<decay_t<_type>>
make_optional(_type&& v)
{
	return optional<decay_t<_type>>(yforward(v));
}
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
//@}

} // namespace ystdex;


namespace std
{

/*!
\brief ystdex::optional 散列支持。
\see ISO WG21/N4081 5.11[optional.hash] 。
\since build 591
*/
//@{
template<typename>
struct hash;

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
//@}

} // namespace std;

#endif

