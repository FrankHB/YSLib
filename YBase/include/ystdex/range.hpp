/*
	© 2015-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file range.hpp
\ingroup YStandardEx
\brief 范围操作。
\version r890
\author FrankHB <frankhb1989@gmail.com>
\since build 624
\par 创建时间:
	2015-08-18 22:33:54 +0800
\par 修改时间:
	2018-08-06 18:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Range
*/


#ifndef YB_INC_ystdex_range_hpp_
#define YB_INC_ystdex_range_hpp_ 1

#include "addressof.hpp" // for <initializer_list>, ystdex::addressof;
#include <iterator> // for <iteartor>, std::reverse_iterator, std::begin,
//	std::end;
// XXX: For efficiency, <valarray> is not supported here. Use %std names
//	instead.

/*!
\brief \<iterator\> 特性测试宏。
\see WG21 P0941R2 2.2 。
*/
//@{
/*!
\see P0031R0 。
\since build 834
*/
//@{
#ifndef __cpp_lib_array_constexpr
#	if (YB_IMPL_MSCPP >= 1911 && _MSVC_LANG >= 201603) || __cplusplus >= 201703L
#		define __cpp_lib_array_constexpr 201603
#	endif
#endif
//@}
/*!
\see LWG 2285 。
\see https://blogs.msdn.microsoft.com/vcblog/2016/01/22/vs-2015-update-2s-stl-is-c17-so-far-feature-complete/ 。
\since build 832
*/
//@{
#ifndef __cpp_lib_make_reverse_iterator
#	if YB_IMPL_MSCPP >= 1800 || __cplusplus >= 201402L
#		define __cpp_lib_make_reverse_iterator 201210
#	endif
#endif
//@}
/*!
\see N4280 。
\see https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance 。
\since build 834
*/
//@{
#ifndef __cpp_lib_nonmember_container_access
#	if YB_IMPL_MSCPP >= 1800 || __cplusplus >= 201703L
#		define __cpp_lib_nonmember_container_access 201411
#	endif
#endif
//@}
//@}

namespace ystdex
{

/*!	\defgroup iterators Iterators
\brief 迭代器。
*/

/*!	\defgroup iterator_adaptors Iterator Adaptors
\ingroup iterators
\brief 迭代器适配器。
*/


//! \since build 833
inline namespace cpp2017
{

// NOTE: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86734.
// XXX: If resolution from LWG 1052 is adopted, Microsoft VC++ 15.7 would be
//	conforming.
// NOTE: '__cpp_lib_array_constexpr >= 201603' is not enough as LWG issues are
//	not respected, including (at least) LWG 2188 and LWG 2438. Note LWG 2188 is
//	adopted by WG21 N3936, it is effective since ISO C++14.
#if __cplusplus >= 201703L && !__GLIBCXX__ && !YB_IMPL_MSCPP
#	define YB_Use_std_reverse_iterator true
#endif
#if YB_Use_std_reverse_iterator
using std::reverse_iterator;
#else
/*!
\note nodiscard 标记是类似 P0600R1 理由的兼容扩展。
\see WG21 P0031R0 。
*/
template<typename _tIter>
class reverse_iterator
{
private:
	using traits_type = std::iterator_traits<_tIter>;

public:
	using iterator_type = _tIter;
	using iterator_category = typename traits_type::iterator_category;
	using value_type = typename traits_type::value_type;
	using difference_type = typename traits_type::difference_type;
	using pointer = typename traits_type::pointer;
	using reference = typename traits_type::reference;

protected:
	_tIter current;

public:
	/*!
	\see LWG 235 。
	\see LWG 1012 。
	*/
	yconstfn
	reverse_iterator()
		: current()
	{}

	explicit yconstfn
	reverse_iterator(_tIter x)
		: current(x)
	{}
	template<class _tOther>
	yconstfn
	reverse_iterator(const reverse_iterator<_tOther>& u)
		: current(u.current)
	{}

	template<class _tOther>
	yconstfn_relaxed reverse_iterator&
	operator=(const reverse_iterator<_tOther>& u)
	{
		current = u.base();
		return *this;
	}

	YB_ATTR_nodiscard yconstfn _tIter
	base() const
	{
		return current;
	}

	YB_ATTR_nodiscard yconstfn_relaxed reference
	operator*() const
	{
		_tIter tmp = current;

		return *--tmp;
	}

	/*!
	\see LWG 1052 。
	\see LWG 2188 。
	*/
	YB_ATTR_nodiscard yconstfn pointer
	operator->() const
	{
		return ystdex::addressof(operator*());
	}

	yconstfn_relaxed reverse_iterator&
	operator++()
	{
		--current;
		return *this;
	}
	yconstfn_relaxed reverse_iterator
	operator++(int)
	{
		reverse_iterator tmp = *this;

		--current;
		return tmp;
	}

	yconstfn_relaxed reverse_iterator&
	operator--()
	{
		++current;
		return *this;
	}
	yconstfn_relaxed reverse_iterator
	operator--(int)
	{
		reverse_iterator tmp = *this;

		++current;
		return tmp;
	}

	yconstfn_relaxed reverse_iterator&
	operator+=(difference_type n)
	{
		current -= n;
		return *this;
	}

	YB_ATTR_nodiscard yconstfn reverse_iterator
	operator+(difference_type n) const
	{
		return reverse_iterator(current - n);
	}

	yconstfn_relaxed reverse_iterator&
	operator-=(difference_type n)
	{
		current += n;
		return *this;
	}

	YB_ATTR_nodiscard yconstfn reverse_iterator
	operator-(difference_type n) const
	{
		return reverse_iterator(current + n);
	}

	//! \see LWG 386 。
	YB_ATTR_nodiscard yconstfn yimpl(reference)
	operator[](difference_type n) const
	{
		return *(*this + n);
	}

	/*!
	\since build 834
	\see LWG 280 。
	*/
	//@{
	template<typename _tIter2>
	YB_ATTR_nodiscard friend yconstfn bool
	operator==(const reverse_iterator& x, const reverse_iterator<_tIter2>& y)
	{
		return x.current == y.current;
	}

	template<typename _tIter2>
	YB_ATTR_nodiscard friend yconstfn bool
	operator<(const reverse_iterator& x, const reverse_iterator<_tIter2>& y)
	{
		return x.current > y.current;
	}

	template<typename _tIter2>
	YB_ATTR_nodiscard friend yconstfn bool
	operator!=(const reverse_iterator& x, const reverse_iterator<_tIter2>& y)
	{
		return x.current != y.current;
	}

	template<typename _tIter2>
	YB_ATTR_nodiscard friend yconstfn bool
	operator>(const reverse_iterator& x, const reverse_iterator<_tIter2>& y)
	{
		return x.current < y.current;
	}

	template<typename _tIter2>
	YB_ATTR_nodiscard friend yconstfn bool
	operator>=(const reverse_iterator& x, const reverse_iterator<_tIter2>& y)
	{
		return x.current <= y.current;
	}

	template<typename _tIter2>
	YB_ATTR_nodiscard friend yconstfn bool
	operator<=(const reverse_iterator& x, const reverse_iterator<_tIter2>& y)
	{
		return x.current >= y.current;
	}
	//@}

	template<typename _tIter2>
	YB_ATTR_nodiscard friend auto
	operator-(const reverse_iterator& x, const reverse_iterator<_tIter2>& y)
		-> decltype(y.base() - x.base())
	{
		return y.current - x.current;
	}

	YB_ATTR_nodiscard friend reverse_iterator
	operator+(difference_type n, const reverse_iterator& x)
	{
		return reverse_iterator(x.current - n);
	}
};
#endif


#if __cpp_lib_array_constexpr >= 201603
//! \since build 624
using std::begin;
//! \since build 624
using std::end;
#else
template<class _tRange>
yconstfn auto
begin(_tRange& c) -> decltype(c.begin())
{
	return c.begin();
}
template<class _tRange>
yconstfn auto
begin(const _tRange& c) -> decltype(c.begin())
{
	return c.begin();
}
// NOTE: To avoid ambiguity, overloads for arrays are reimplemented here even
//	they do not change in P0031R0.
//! \since build 834
template<typename _type, size_t _vN>
yconstfn _type*
begin(_type(&array)[_vN]) ynothrow
{
	return array;
}

template<class _tRange>
yconstfn auto
end(_tRange& c) -> decltype(c.end())
{
	return c.end();
}
template<class _tRange>
yconstfn auto
end(const _tRange& c) -> decltype(c.end())
{
	return c.end();
}
// NOTE: See above.
//! \since build 834
template<typename _type, size_t _vN>
yconstfn _type*
end(_type(&array)[_vN]) ynothrow
{
	return array + _vN;
}
#endif

} // inline namespace cpp2017;

/*!
\since build 833
\note YStandardEx 扩展。
*/
//@{
//! \since build 664
template<typename _type, size_t _vN>
yconstfn _type*
begin(_type(&&array)[_vN]) ynothrow
{
	return array;
}

//! \since build 664
template<typename _type, size_t _vN>
yconstfn _type*
end(_type(&&array)[_vN]) ynothrow
{
	return array + _vN;
}

template<typename _type, size_t _vN>
yconstfn reverse_iterator<_type*>
rbegin(_type(&&array)[_vN])
{
	return reverse_iterator<_type*>(array + _vN);
}

template<typename _type, size_t _vN>
yconstfn reverse_iterator<_type*>
rend(_type(&&array)[_vN])
{
	return reverse_iterator<_type*>(array);
}
//@}

//! \since build 624
//@{
inline namespace cpp2014
{

#if (__cpp_lib_nonmember_container_access >= 201411 \
	|| (__cplusplus >= 201402L && (!defined(__GLIBCXX__) \
	|| __GLIBCXX__ >= 20150119))) || (_LIBCXX_VERSION >= 1101 \
	&& _LIBCPP_STD_VER > 11) || YB_IMPL_MSCPP >= 1800
using std::cbegin;
using std::cend;
#else
//! \since build 834
template<typename _tRange>
yconstfn auto
cbegin(const _tRange& c) ynoexcept_spec(ystdex::begin(c))
	-> decltype(ystdex::begin(c))
{
	return ystdex::begin(c);
}

//! \since build 834
template<typename _tRange>
yconstfn auto
cend(const _tRange& c) ynoexcept_spec(ystdex::end(c))
	-> decltype(ystdex::end(c))
{
	return ystdex::end(c);
}
#endif

} // inline namespace cpp2014;
//@}

//! \since build 834
inline namespace cpp2017
{

//! \since build 624
//@{
#if __cpp_lib_array_constexpr >= 201603
using std::rbegin;
using std::rend;
using std::crbegin;
using std::crend;
#else
template<class _tRange>
yconstfn auto
rbegin(_tRange& c) -> decltype(c.rbegin())
{
	return c.rbegin();
}
template<class _tRange>
yconstfn auto
rbegin(const _tRange& c) -> decltype(c.rbegin())
{
	return c.rbegin();
}
//! \since build 833
template<typename _type, size_t _vN>
yconstfn reverse_iterator<_type*>
rbegin(_type(&array)[_vN])
{
	return reverse_iterator<_type*>(array + _vN);
}

//! \since build 833
template<typename _tElem>
yconstfn reverse_iterator<const _tElem*>
rbegin(std::initializer_list<_tElem> il)
{
	return reverse_iterator<const _tElem*>(il.end());
}

template<class _tRange>
yconstfn auto
rend(_tRange& c) -> decltype(c.rend())
{
	return c.rend();
}
template<class _tRange>
yconstfn auto
rend(const _tRange& c) -> decltype(c.rend())
{
	return c.rend();
}
//! \since build 833
template<typename _type, size_t _vN>
yconstfn reverse_iterator<_type*>
rend(_type(&array)[_vN])
{
	return reverse_iterator<_type*>(array);
}
//! \since build 833
template<typename _tElem>
yconstfn reverse_iterator<const _tElem*>
rend(std::initializer_list<_tElem> il)
{
	return reverse_iterator<const _tElem*>(il.begin());
}

template<typename _tRange>
yconstfn auto
crbegin(const _tRange& c) -> decltype(ystdex::rbegin(c))
{
	return ystdex::rbegin(c);
}

template<typename _tRange>
yconstfn auto
crend(const _tRange& c) -> decltype(ystdex::rend(c))
{
	return ystdex::rend(c);
}
#endif

} // inline namespace cpp2017;

/*!
\brief 类容器访问。
\see WG21 N4280 。
\see ISO C++17 [iterator.container] 。
\since build 663
*/
//@{
//! \since build 834
inline namespace cpp2017
{

#if __cpp_lib_nonmember_container_access >= 201411
using std::size;
using std::empty;
using std::data;
#else

template<class _tRange>
yconstfn auto
size(const _tRange& c) -> decltype(c.size())
{
	return c.size();
}
template<typename _type, size_t _vN>
yconstfn size_t
size(const _type(&)[_vN]) ynothrow
{
	return _vN;
}
#if __cplusplus <= 201402L
//! \see CWG 1591 。
template<typename _tElem>
yconstfn size_t
size(std::initializer_list<_tElem> il) ynothrow
{
	return il.size();
}
#endif

template<class _tRange>
YB_ATTR_nodiscard yconstfn auto
empty(const _tRange& c) -> decltype(c.empty())
{
	return c.empty();
}
template<typename _type, size_t _vN>
YB_ATTR_nodiscard yconstfn bool
empty(const _type(&)[_vN]) ynothrow
{
	return {};
}
template<typename _tElem>
YB_ATTR_nodiscard yconstfn bool
empty(std::initializer_list<_tElem> il) ynothrow
{
	return il.size() == 0;
}

template<typename _tRange>
yconstfn auto
data(_tRange& c) -> decltype(c.data())
{
	return c.data();
}
template<typename _tRange>
yconstfn auto
data(const _tRange& c) -> decltype(c.data())
{
	return c.data();
}
template<typename _type, size_t _vN>
yconstfn _type*
data(_type(&array)[_vN]) ynothrow
{
	return array;
}
template<typename _tElem>
yconstfn const _tElem*
data(std::initializer_list<_tElem> il) ynothrow
{
	return il.begin();
}
#endif

} // inline namespace cpp2017;

//! \note YStandardEx 扩展。
template<typename _type, size_t _vN>
yconstfn _type*
data(_type(&&array)[_vN]) ynothrow
{
	return array;
}
//@}

//! \since build 624
inline namespace cpp2014
{

//! \since build 664
//@{
#if __cplusplus <= 201402L
//! \see CWG 1591 。
//@{
template<typename _tElem>
yconstfn const _tElem*
cbegin(std::initializer_list<_tElem> il) ynothrow
{
	return il.begin();
}

//! \see CWG 1591 。
template<typename _tElem>
yconstfn const _tElem*
cend(std::initializer_list<_tElem> il) ynothrow
{
	return il.end();
}

//! \since build 833
template<typename _tElem>
yconstfn reverse_iterator<const _tElem*>
crbegin(std::initializer_list<_tElem> il) ynothrow
{
	return ystdex::rbegin(il);
}

//! \since build 833
template<typename _tElem>
yconstfn reverse_iterator<const _tElem*>
crend(std::initializer_list<_tElem> il) ynothrow
{
	return ystdex::rend(il);
}
//@}
#endif
//@}

} // inline namespace cpp2014;

} // namespace ystdex;

#endif

