/*
	© 2015-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file tstring_view.hpp
\ingroup YStandardEx
\brief 指定结束字符的只读字符串视图。
\version r327
\author FrankHB <frankhb1989@gmail.com>
\since build 640
\par 创建时间:
	2015-10-01 22:56:52 +0800
\par 修改时间:
	2018-11-16 17:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TStringView
*/


#ifndef YB_INC_ystdex_tstring_view_hpp_
#define YB_INC_ystdex_tstring_view_hpp_ 1

#include "string_view.hpp" // for std::char_traits, basic_string_view,
//	std::basic_string;
#include "operators.hpp" // for totally_ordered;
#include <memory>// for std::allocator;
#include <stdexcept> // for std::out_of_range;
#include "cstring.h" // for ystdex::ntctscmp;

namespace ystdex
{

/*!
\brief 指定结束字符的只读字符串视图。
\since build 640
\sa basic_string_view
\todo 实现一般非空字符作为结束字符的情形。
*/
//@{
template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
	char32_t _vTerm = 0>
class basic_tstring_view;

template<typename _tChar, class _tTraits>
class basic_tstring_view<_tChar, _tTraits, 0>
	: private basic_string_view<_tChar, _tTraits>,
	yimpl(private totally_ordered<basic_tstring_view<_tChar, _tTraits, 0>>)
{
public:
	using traits_type = _tTraits;
	using value_type = _tChar;
	using pointer = _tChar*;
	using const_pointer = const _tChar*;
	using reference = _tChar&;
	using const_reference = const _tChar&;
	using const_iterator = yimpl(const _tChar*);
	using iterator = const_iterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using base = basic_string_view<_tChar, _tTraits>;
	using base::npos;
	static yconstexpr value_type term = {};

	yconstfn
	basic_tstring_view() = default;
	YB_NONNULL(2) yconstfn
	basic_tstring_view(const _tChar* str)
		: base(str)
	{}
	YB_NONNULL(2) yconstfn
	basic_tstring_view(const _tChar* str, size_type len)
		: base(verify(str, len) ? str
		: (throw std::invalid_argument("Invalid string found."), str), len)
	{}
	//! \since build 641
	basic_tstring_view(base sv)
		: basic_tstring_view(sv.data(), sv.length())
	{}
	template<class _tAlloc>
	basic_tstring_view(const std::basic_string<_tChar, _tTraits, _tAlloc>& str)
		ynothrow
		: basic_tstring_view(str.data())
	{}
	yconstfn
	basic_tstring_view(const basic_tstring_view&) ynothrow = default;

	basic_tstring_view&
	operator=(const basic_tstring_view&) ynothrow = default;

	using base::begin;

	using base::end;

	using base::cbegin;

	using base::cend;

	using base::rbegin;

	using base::rend;

	using base::crbegin;

	using base::crend;

	using base::size;

	using base::length;

	using base::max_size;

	using base::empty;

	using base::operator[];

	using base::at;

	using base::front;

	using base::back;

	using base::data;

	using base::remove_prefix;

	yconstfn_relaxed void
	swap(basic_tstring_view& s) ynothrow
	{
		base::swap(static_cast<base&>(s));
	}

	template<class _tAlloc = std::allocator<_tChar>>
	explicit
	operator std::basic_string<_tChar, _tTraits, _tAlloc>() const
	{
		return std::basic_string<_tChar, _tTraits, _tAlloc>(
			static_cast<const base&>(*this));
	}

	operator base() const ynothrow
	{
		return *this;
	}

	using base::copy;

	yconstfn basic_tstring_view
	substr(size_type pos = 0) const
	{
		return pos <= size()
			? basic_tstring_view(data() + pos, size_type(size() - pos))
			: throw std::out_of_range("tstring_view::substr");
	}

	yconstfn int
	compare(basic_tstring_view s) const ynothrow
	{
		return compare_remained(s, ystdex::ntctscmp(data(), s.data(),
			std::min<size_type>(size(), s.size())));
	}
	yconstfn int
	compare(size_type pos1, size_type n1, basic_tstring_view s) const
	{
		return substr(pos1, n1).compare(s);
	}
	yconstfn int
	compare(size_type pos1, size_type n1, basic_tstring_view s, size_type pos2,
		size_type n2) const
	{
		return substr(pos1, n1).compare(s.substr(pos2, n2));
	}
	yconstfn int
	compare(const _tChar* s) const
	{
		return compare(basic_tstring_view(s));
	}
	yconstfn int
	compare(size_type pos1, size_type n1, const _tChar* s) const
	{
		return substr(pos1, n1).compare(basic_tstring_view(s));
	}
	yconstfn int
	compare(size_type pos1, size_type n1, const _tChar* s, size_type n2) const
	{
		return substr(pos1, n1).compare(basic_tstring_view(s, n2));
	}

private:
	yconstfn int
	compare_remained(basic_tstring_view s, int ret) const ynothrow
	{
		return ret == 0 ? (size() == s.size() ? 0
			: (size() < s.size() ? -1 : 1)) : ret;
	}

public:
	using base::find;
	using base::rfind;
	using base::find_first_of;
	using base::find_last_of;
	using base::find_first_not_of;
	using base::find_last_not_of;

	//! \since build 641
	//@{
	static YB_NONNULL(1) bool
	verify(const _tChar* str, size_type len) ynothrowv
	{
		yconstraint(str);
		return traits_type::length(str) == len;
	}

	static YB_NONNULL(1) bool
	verify_term(const _tChar* str, size_type len) ynothrowv
	{
		yconstraint(str);
		return str[len] == term;
	}
	//@}
};

/*!
\relates basic_tstring_view
\since build 642
*/
//@{
template<typename _tChar, class _tTraits>
yconstfn bool
operator==(basic_tstring_view<_tChar, _tTraits> x,
	basic_tstring_view<_tChar, _tTraits> y) ynothrow
{
	return x.compare(y) == 0;
}
template<typename _tChar, class _tTraits>
yconstfn bool
operator==(basic_tstring_view<_tChar, _tTraits> x,
	yimpl(decay_t<basic_tstring_view<_tChar, _tTraits>>) y) ynothrow
{
	return x.compare(y) == 0;
}
template<typename _tChar, class _tTraits>
yconstfn bool
operator==(yimpl(decay_t<basic_tstring_view<_tChar, _tTraits>>) x,
	basic_tstring_view<_tChar, _tTraits> y) ynothrow
{
	return x.compare(y) == 0;
}

template<typename _tChar, class _tTraits>
yconstfn bool
operator<(basic_tstring_view<_tChar, _tTraits> x,
	basic_tstring_view<_tChar, _tTraits> y) ynothrow
{
	return x.compare(y) < 0;
}
template<typename _tChar, class _tTraits>
yconstfn bool
operator<(basic_tstring_view<_tChar, _tTraits> x,
	yimpl(decay_t<basic_tstring_view<_tChar, _tTraits>>) y) ynothrow
{
	return x.compare(y) < 0;
}
template<typename _tChar, class _tTraits>
yconstfn bool
operator<(yimpl(decay_t<basic_tstring_view<_tChar, _tTraits>>) x,
	basic_tstring_view<_tChar, _tTraits> y) ynothrow
{
	return x.compare(y) < 0;
}

template<typename _tChar, class _tTraits>
std::basic_ostream<_tChar, _tTraits>&
operator<<(std::basic_ostream<_tChar, _tTraits>& os,
	basic_tstring_view<_tChar, _tTraits> str)
{
	// XXX: Better implementation?
	return os << str.to_string();
}
//@}


using tstring_view = basic_tstring_view<char>;
using u16tstring_view = basic_tstring_view<char16_t>;
using u32tstring_view = basic_tstring_view<char32_t>;
using wtstring_view = basic_tstring_view<wchar_t>;
//@}

} // namespace ystdex;

#endif

