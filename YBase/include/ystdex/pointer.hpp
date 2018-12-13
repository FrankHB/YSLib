/*
	© 2012-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file pointer.hpp
\ingroup YStandardEx
\brief 通用指针。
\version r618
\author FrankHB <frankhb1989@gmail.com>
\since build 600
\par 创建时间:
	2015-05-24 14:38:11 +0800
\par 修改时间:
	2018-12-09 08:06 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Pointer

间接扩展标准库头 <iterator> ，提供指针的迭代器适配器包装及其它和指针类型相关的模板。
*/


#ifndef YB_INC_ystdex_pointer_hpp_
#define YB_INC_ystdex_pointer_hpp_ 1

#include "iterator_op.hpp" // for "iterator_op.hpp", <memory>, pointer_traits,
//	nullptr_t, bool_, not_, is_function, true_, totally_ordered,
//	equality_comparable, add_pointer_t, add_lvalue_reference_t, yconstraint,
//	iterator_operators_t, std::iterator_traits;
#include <functional> // for std::equal_to, std::less, std::hash;
#include "type_traits.hpp" // for add_ptr_t, add_ref_t, ystdex::swap_dependent;

namespace ystdex
{

//! \since build 685
namespace details
{

//! \since build 846
//@{
template<class _tPointer>
YB_ATTR_nodiscard inline auto
to_address(nullptr_t, const _tPointer& p)
	-> decltype(std::pointer_traits<_tPointer>::to_address(p))
{
	return std::pointer_traits<_tPointer>::to_address(p);
}
template<class _tPointer>
YB_ATTR_nodiscard inline auto
to_address(void*, const _tPointer& p)
	-> decltype(details::to_address(nullptr, p.operator->()))
{
	return details::to_address(nullptr, p.operator->());
}
//@}

template<typename _type>
using nptr_eq1 = bool_<_type() == _type()>;
template<typename _type>
using nptr_eq2 = bool_<_type(nullptr) == nullptr>;

} // namespace details;


/*!
\ingroup YBase_replacement_features
\brief 转换指针。
\since build 846
\see WG21 P0653R2 。
*/
//@{
template<class _tPointer>
YB_ATTR_nodiscard inline auto
to_address(const _tPointer& p) ynothrow
	-> yimpl(decltype(details::to_address(p)))
{
	return details::to_address(nullptr, p);
}
template<typename _type>
YB_ATTR_nodiscard YB_STATELESS yconstfn _type*
to_address(_type* p) ynothrow
{
	static_assert(not_<is_function<_type>>(), "Invalid type found.");

	return p;
}
//@}


//! \since build 560
//@{
/*!
\brief 可空指针包装：满足 \c NullablePointer 要求同时满足转移后为空。
\tparam _type 被包装的指针。
\pre _type 满足 \c NullablePointer 要求。
*/
template<typename _type>
class nptr : private totally_ordered<nptr<_type>>
{
	//! \since build 630
	static_assert(is_nothrow_copyable<_type>(), "Invalid type found.");
	static_assert(is_destructible<_type>(), "Invalid type found.");
	static_assert(detected_or_t<true_, details::nptr_eq1, _type>(),
		"Invalid type found.");
	static_assert(detected_or_t<true_, details::nptr_eq2, _type>(),
		"Invalid type found.");

public:
	using pointer = _type;

private:
	pointer ptr{};

public:
	nptr() = default;
	//! \since build 628
	//@{
	yconstfn
	nptr(std::nullptr_t) ynothrow
		: nptr()
	{}
	nptr(pointer p) ynothrow
		: ptr(p)
	{}
	//@}
	nptr(const nptr&) = default;
	nptr(nptr&& np) ynothrow
	{
		np.swap(*this);
	}

	nptr&
	operator=(const nptr&) = default;
	nptr&
	operator=(nptr&& np) ynothrow
	{
		np.swap(*this);
		return *this;
	}

	YB_ATTR_nodiscard YB_PURE yconstfn bool
	operator!() const ynothrow
	{
		return !bool(*this);
	}

	//! \since build 613
	//@{
	//! \pre 表达式 \c *ptr 合式。
	//@{
	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed auto
	operator*() ynothrow -> decltype(*ptr)
	{
		return *ptr;
	}
	YB_ATTR_nodiscard YB_PURE yconstfn auto
	operator*() const ynothrow -> decltype(*ptr)
	{
		return *ptr;
	}
	//@}

	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed pointer&
	operator->() ynothrow
	{
		return ptr;
	}
	YB_ATTR_nodiscard YB_PURE yconstfn const pointer&
	operator->() const ynothrow
	{
		return ptr;
	}
	//@}

	//! \since build 600
	YB_ATTR_nodiscard YB_PURE friend yconstfn bool
	operator==(const nptr& x, const nptr& y) ynothrow
	{
		return std::equal_to<pointer>()(x.ptr, y.ptr);
	}

	//! \since build 600
	YB_ATTR_nodiscard YB_PURE friend yconstfn bool
	operator<(const nptr& x, const nptr& y) ynothrow
	{
		return std::less<pointer>()(x.ptr, y.ptr);
	}

	//! \since build 628
	YB_ATTR_nodiscard YB_PURE explicit yconstfn
	operator bool() const ynothrow
	{
		return bool(ptr);
	}

	//! \since build 566
	YB_ATTR_nodiscard YB_PURE yconstfn const pointer&
	get() const ynothrow
	{
		return ptr;
	}

	YB_ATTR_nodiscard YB_PURE yconstfn_relaxed pointer&
	get_ref() ynothrow
	{
		return ptr;
	}

	/*!
	\note 使用 ADL swap 或 std::swap 。
	\since build 628
	*/
	void
	swap(nptr& np) ynothrow
	{
		ystdex::swap_dependent(ptr, np.ptr);
	}
};

/*!
\relates nptr
\since build 563
*/
template<typename _type>
inline void
swap(nptr<_type>& x, nptr<_type>& y) ynothrow
{
	x.swap(y);
}
//@}


//! \since build 669
//@{
/*!
\brief 观察者指针：无所有权的智能指针。
\see WG21 N4758 5.2[memory.observer.ptr] 。
*/
template<typename _type>
class observer_ptr : private totally_ordered<observer_ptr<_type>>,
	private equality_comparable<observer_ptr<_type>, nullptr_t>
{
public:
	using element_type = _type;
	using pointer = add_pointer_t<_type>;
	using reference = add_lvalue_reference_t<_type>;

private:
	_type* ptr{};

public:
	//! \post <tt>get() == nullptr</tt> 。
	//@{
	yconstfn
	observer_ptr() ynothrow yimpl(= default);
	yconstfn
	observer_ptr(nullptr_t) ynothrow
		: ptr()
	{}
	//@}
	explicit yconstfn
	observer_ptr(pointer p) ynothrow
		: ptr(p)
	{}
	//! \since build 847
	template<typename _tOther,
		yimpl(typename = enable_if_convertible_t<_tOther*, _type*>)>
	yconstfn
	observer_ptr(observer_ptr<_tOther> other) ynothrow
		: ptr(other.get())
	{}

	//! \pre 断言： <tt>get() != nullptr</tt> 。
	YB_ATTR_nodiscard YB_STATELESS yconstfn_relaxed reference
	operator*() const ynothrowv
	{
		return yconstraint(get() != nullptr), *ptr;
	}

	YB_ATTR_nodiscard YB_STATELESS yconstfn pointer
	operator->() const ynothrow
	{
		return ptr;
	}

	//! \since build 675
	YB_ATTR_nodiscard YB_STATELESS friend yconstfn bool
	operator==(observer_ptr p, nullptr_t) ynothrow
	{
		return !p.ptr;
	}

	YB_ATTR_nodiscard YB_STATELESS explicit yconstfn
	operator bool() const ynothrow
	{
		return ptr;
	}

	YB_ATTR_nodiscard YB_STATELESS explicit yconstfn
	operator pointer() const ynothrow
	{
		return ptr;
	}

	YB_ATTR_nodiscard YB_STATELESS yconstfn pointer
	get() const ynothrow
	{
		return ptr;
	}

	yconstfn_relaxed pointer
	release() ynothrow
	{
		const auto res(ptr);

		reset();
		return res;
	}

	yconstfn_relaxed void
	reset(pointer p = {}) ynothrow
	{
		ptr = p;
	}

	yconstfn_relaxed void
	swap(observer_ptr& other) ynothrow
	{
		std::swap(ptr, other.ptr);
	}
};

//! \relates observer_ptr
//@{
//! \since build 675
//@{
template<typename _type1, typename _type2>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
operator==(observer_ptr<_type1> p1, observer_ptr<_type2> p2) ynothrowv
{
	return p1.get() == p2.get();
}

template<typename _type1, typename _type2>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
operator!=(observer_ptr<_type1> p1, observer_ptr<_type2> p2) ynothrowv
{
	return !(p1 == p2);
}

template<typename _type1, typename _type2>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
operator<(observer_ptr<_type1> p1, observer_ptr<_type2> p2) ynothrowv
{
	return std::less<common_type_t<_type1*, _type2*>>()(p1.get(), p2.get());
}
//@}

template<typename _type>
inline void
swap(observer_ptr<_type>& p1, observer_ptr<_type>& p2) ynothrow
{
	p1.swap(p2);
}
template<typename _type>
YB_ATTR_nodiscard YB_STATELESS inline observer_ptr<_type>
make_observer(_type* p) ynothrow
{
	return observer_ptr<_type>(p);
}
//@}
//@}


//! \since build 755
template<typename _type>
using tidy_ptr = nptr<observer_ptr<_type>>;


/*!
\ingroup iterator_adaptors
\brief 指针迭代器。
\note 转换为 bool 、有序比较等操作使用转换为对应指针实现。
\warning 非虚析构。
\since build 290

转换指针为类类型的随机访问迭代器。
\todo 和 std::pointer_traits 交互。
*/
template<typename _type>
class pointer_iterator : public iterator_operators_t<pointer_iterator<_type>,
	std::iterator_traits<_type*>>
{
public:
	using iterator_type = _type*;
	using iterator_category
		= typename std::iterator_traits<iterator_type>::iterator_category;
	using value_type = typename std::iterator_traits<iterator_type>::value_type;
	using difference_type
		= typename std::iterator_traits<iterator_type>::difference_type;
	using pointer = typename std::iterator_traits<iterator_type>::pointer;
	using reference = typename std::iterator_traits<iterator_type>::reference;

protected:
	//! \since build 415
	pointer raw;

public:
	yconstfn
	pointer_iterator(nullptr_t = {})
		: raw()
	{}
	//! \since build 347
	template<typename _tPointer>
	yconstfn
	pointer_iterator(_tPointer&& ptr)
		: raw(yforward(ptr))
	{}
	inline
	pointer_iterator(const pointer_iterator&) = default;

	//! \since build 585
	//@{
	yconstfn_relaxed pointer_iterator&
	operator+=(difference_type n) ynothrowv
	{
		yconstraint(raw);
		raw += n;
		return *this;
	}

	yconstfn_relaxed pointer_iterator&
	operator-=(difference_type n) ynothrowv
	{
		yconstraint(raw);
		raw -= n;
		return *this;
	}

	//! \since build 461
	YB_ATTR_nodiscard YB_STATELESS yconstfn reference
	operator*() const ynothrowv
	{
		return yconstraint(raw), *raw;
	}

	yconstfn_relaxed pointer_iterator&
	operator++() ynothrowv
	{
		yconstraint(raw);
		++raw;
		return *this;
	}

	yconstfn_relaxed pointer_iterator&
	operator--() ynothrowv
	{
		--raw;
		return *this;
	}

	//! \since build 600
	YB_ATTR_nodiscard YB_STATELESS friend yconstfn bool
	operator==(const pointer_iterator& x, const pointer_iterator& y) ynothrow
	{
		return x.raw == y.raw;
	}

	//! \since build 666
	YB_ATTR_nodiscard YB_STATELESS friend yconstfn bool
	operator<(const pointer_iterator& x, const pointer_iterator& y) ynothrow
	{
		return x.raw < y.raw;
	}

	YB_ATTR_nodiscard YB_STATELESS yconstfn
	operator pointer() const ynothrow
	{
		return raw;
	}
	//@}
};


/*!
\ingroup transformation_traits
\brief 指针包装为类类型迭代器。
\since build 290

若参数是指针类型则包装为 pointer_iterator 。
*/
//@{
template<typename _type>
struct pointer_classify
{
	using type = _type;
};

template<typename _type>
struct pointer_classify<_type*>
{
	using type = pointer_iterator<_type>;
};
//@}

} // namespace ystdex;

namespace std
{

/*!
\brief ystdex::nptr 散列支持。
\since build 847
*/
template<typename _type>
struct hash<ystdex::nptr<_type>>
{
	YB_ATTR_nodiscard YB_STATELESS size_t
	operator()(const ystdex::observer_ptr<_type>& p) const
		ynoexcept_spec(hash<_type>(p.get()))
	{
		return hash<_type>(p.get());
	}
};

/*!
\brief ystdex::observer_ptr 散列支持。
\see ISO WG21 N4529 8.12.7[memory.observer.ptr.hash] 。
\since build 674
*/
template<typename _type>
struct hash<ystdex::observer_ptr<_type>>
{
	YB_ATTR_nodiscard YB_STATELESS size_t
	operator()(const ystdex::observer_ptr<_type>& p) const yimpl(ynothrow)
	{
		return hash<_type*>(p.get());
	}
};

} // namespace std;

#endif

