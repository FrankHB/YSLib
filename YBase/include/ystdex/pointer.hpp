/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file pointer.hpp
\ingroup YStandardEx
\brief 通用指针。
\version r340
\author FrankHB <frankhb1989@gmail.com>
\since build 600
\par 创建时间:
	2015-05-24 14:38:11 +0800
\par 修改时间:
	2016-01-26 11:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Pointer

间接扩展标准库头 <iterator> ，提供指针的迭代器适配器包装和其它模板。
*/


#ifndef YB_INC_ystdex_pointer_hpp_
#define YB_INC_ystdex_pointer_hpp_ 1

#include "iterator_op.hpp" // for totally_ordered,
//	iterator_operators_t, std::iterator_traits, yconstraint;
#include <functional> // for std::equal_to, std::less;

namespace ystdex
{

//! \since build 560
//@{
/*!
\brief 可空指针包装：满足 \c NullablePointer 要求同时满足转移后为空。
\tparam _type 被包装的指针。
\pre _type 满足 \c NullablePointer 要求。
*/
template<typename _type>
class nptr : public totally_ordered<nptr<_type>>
{
	//! \since build 630
	static_assert(is_nothrow_copyable<_type>(), "Invalid type found.");
	static_assert(is_destructible<_type>(), "Invalid type found.");
	static_assert(_type() == _type(), "Invalid type found.");
	static_assert(_type(nullptr) == nullptr, "Invalid type found.");

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

	yconstfn bool
	operator!() const ynothrow
	{
		return bool(*this);
	}

	//! \since build 613
	//@{
	//! \pre 表达式 \c *ptr 合式。
	//@{
	yconstfn_relaxed auto
	operator*() ynothrow -> decltype(*ptr)
	{
		return *ptr;
	}
	yconstfn auto
	operator*() const ynothrow -> decltype(*ptr)
	{
		return *ptr;
	}
	//@}

	yconstfn_relaxed pointer&
	operator->() ynothrow
	{
		return ptr;
	}
	yconstfn const pointer&
	operator->() const ynothrow
	{
		return ptr;
	}
	//@}

	//! \since build 600
	friend yconstfn bool
	operator==(const nptr& x, const nptr& y) ynothrow
	{
		return std::equal_to<pointer>()(x.ptr, y.ptr);
	}

	//! \since build 600
	friend yconstfn bool
	operator<(const nptr& x, const nptr& y) ynothrow
	{
		return std::less<pointer>()(x.ptr, y.ptr);
	}

	//! \since build 628
	yconstfn explicit
	operator bool() const ynothrow
	{
		return bool(ptr);
	}

	//! \since build 566
	yconstfn const pointer&
	get() const ynothrow
	{
		return ptr;
	}

	yconstfn_relaxed pointer&
	get_ref() ynothrow
	{
		return ptr;
	}

	//! \since build 628
	void
	swap(nptr& np) ynothrow
	{
		using std::swap;

		swap(ptr, np.ptr);
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
	explicit yconstfn
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
	yconstfn reference
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
	friend yconstfn bool
	operator==(const pointer_iterator& x, const pointer_iterator& y) ynothrow
	{
		return x.raw == y.raw;
	}

	//! \since build 666
	friend yconstfn bool
	operator<(const pointer_iterator& x, const pointer_iterator& y) ynothrow
	{
		return x.raw < y.raw;
	}

	yconstfn
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

#endif

