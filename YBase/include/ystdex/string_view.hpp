﻿/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file string_view.hpp
\ingroup YStandardEx
\brief 只读字符串视图。
\version r450
\author FrankHB <frankhb1989@gmail.com>
\since build 640
\par 创建时间:
	2015-09-28 12:04:58 +0800
\par 修改时间:
	2015-10-04 17:12 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::StringView
\see ISO WG21/N4081 7[string_view] 。
\bug 关系操作使用 operators 导致不必要的 ODR-used 。

除了部分关系操作使用 operators 实现而不保留命名空间内的声明及散列支持提供偏特化外，
其它接口同 std::experimental::string_view 。
*/


#ifndef YB_INC_ystdex_string_view_hpp_
#define YB_INC_ystdex_string_view_hpp_ 1

#include "string.hpp" // for std::char_traits, std::basic_string,
//	std::allocator, 'str_find*', 'str_rfind*', std::basic_ostream;
#include "operators.hpp" // for totally_ordered;
#include <limits> // for std::numeric_limits;
#include <stdexcept> // for std::out_of_range;
#include "hash.hpp" // for ystdex::hash_range;

namespace ystdex
{

//! \since build 640
//@{
template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_string_view
	: yimpl(public totally_ordered<basic_string_view<_tChar, _tTraits>>)
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
	static yconstexpr size_type npos = size_type(-1);

private:
	const_pointer data_;
	size_type size_;

public:
	yconstfn
	basic_string_view() ynothrow
		: data_(), size_()
	{}
	yconstfn YB_NONNULL(2)
	basic_string_view(const _tChar* str)
		: data_(str), size_(traits_type::length(str))
	{}
	yconstfn YB_NONNULL(2)
	basic_string_view(const _tChar* str, size_type len)
		: data_(str), size_(len)
	{}
	template<class _tAlloc>
	basic_string_view(const std::basic_string<_tChar, _tTraits, _tAlloc>& str)
		ynothrow
		: data_(str.data()), size_(str.size())
	{}
	yconstfn
	basic_string_view(const basic_string_view&) ynothrow = default;

	basic_string_view&
	operator=(const basic_string_view&) ynothrow = default;

	yconstfn const_iterator
	begin() const ynothrow
	{
		return data_;
	}

	yconstfn const_iterator
	end() const ynothrow
	{
		return data_ + size_;
	}

	yconstfn const_iterator
	cbegin() const ynothrow
	{
		return data_;
	}

	yconstfn const_iterator
	cend() const ynothrow
	{
		return data_ + size_;
	}

	const_reverse_iterator
	rbegin() const ynothrow
	{
		return const_reverse_iterator(end());
	}

	const_reverse_iterator
	rend() const ynothrow
	{
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator
	crbegin() const ynothrow
	{
		return const_reverse_iterator(cend());
	}

	const_reverse_iterator
	crend() const ynothrow
	{
		return const_reverse_iterator(cbegin());
	}

	yconstfn size_type
	size() const ynothrow
	{
		return size_;
	}

	yconstfn size_type
	length() const ynothrow
	{
		return size_;
	}

	yconstfn size_type
	max_size() const ynothrow
	{
		// NOTE: This is same to libc++ but not libstdc++.
		return std::numeric_limits<size_type>::max();
	}

	yconstfn bool
	empty() const ynothrow
	{
		return size_ == 0;
	}

	yconstfn const_reference
	operator[](size_type pos) const
	{
		return yconstraint(pos < size_), data_[pos];
	}

	yconstfn const_reference
	at(size_type pos) const
	{
		return pos < size_ ? data_[pos]
			: (throw std::out_of_range("string_view::at"), data_[0]);
	}

	yconstfn const_reference
	front() const
	{
		return yconstraint(0 < size_), data_[0];
	}

	yconstfn const_reference
	back() const
	{
		return yconstraint(0 < size_), data_[size_ - 1];
	}

	yconstfn const_pointer
	data() const ynothrow
	{
		return data_;
	}

	yconstfn_relaxed void
	clear() ynothrow
	{
		yunseq(data_ = {}, size_ = 0);
	}

	yconstfn_relaxed void
	remove_prefix(size_type n) ynothrowv
	{
		yconstraint(n <= size_), yunseq(data_ += n, size_ -= n);
	}

	yconstfn_relaxed void
	remove_suffix(size_type n) ynothrowv
	{
		yconstraint(n <= size_), size_ -= n;
	}

	yconstfn_relaxed void
	swap(basic_string_view& s) ynothrow
	{
		std::swap(data_, s.data_),
		std::swap(size_, s.size_);
	}

	template<class _tAlloc>
	explicit
	operator std::basic_string<_tChar, _tTraits, _tAlloc>() const
	{
		return {data_, size_};
	}

	template<class _tAlloc = std::allocator<_tChar>>
	std::basic_string<_tChar, _tTraits, _tAlloc>
	to_string(const _tAlloc& a = _tAlloc()) const
	{
		return {data_, size_, a};
	}

	YB_NONNULL(2) size_type
	copy(_tChar* s, size_type n, size_type pos = 0) const
	{
		yconstraint(s);
		if(pos <= size_)
		{
			const auto rlen(std::min<size_type>(n, size_ - pos));

			std::copy_n(data_ + pos, rlen, s);
			return rlen;
		}
		throw std::out_of_range("string_view::copy");
	}

	yconstfn basic_string_view
	substr(size_type pos = 0, size_type n = npos) const
	{
		return pos <= size_ ? basic_string_view(data_ + pos,
			std::min(n, size_type(size_ - pos)))
			: throw std::out_of_range("string_view::substr");
	}

	yconstfn int
	compare(basic_string_view s) const ynothrow
	{
		return compare_remained(s, _tTraits::compare(data(), s.data(),
			std::min<size_type>(size(), s.size())));
	}
	yconstfn int
	compare(size_type pos1, size_type n1, basic_string_view s) const
	{
		return substr(pos1, n1).compare(s);
	}
	yconstfn int
	compare(size_type pos1, size_type n1, basic_string_view s, size_type pos2,
		size_type n2) const
	{
		return substr(pos1, n1).compare(s.substr(pos2, n2));
	}
	yconstfn YB_NONNULL(2) int
	compare(const _tChar* s) const
	{
		return compare(basic_string_view(s));
	}
	yconstfn YB_NONNULL(4) int
	compare(size_type pos1, size_type n1, const _tChar* s) const
	{
		return substr(pos1, n1).compare(basic_string_view(s));
	}
	yconstfn YB_NONNULL(4) int
	compare(size_type pos1, size_type n1, const _tChar* s, size_type n2) const
	{
		return substr(pos1, n1).compare(basic_string_view(s, n2));
	}

private:
	yconstfn int
	compare_remained(basic_string_view s, int ret) const ynothrow
	{
		return ret == 0 ? (size() == s.size() ? 0
			: (size() < s.size() ? -1 : 1)) : ret;
	}

public:
#define YB_Impl_StringView_search_fn_head(_n, _attr, _spec, ...) \
	yconstfn_relaxed _attr size_type \
	_n(__VA_ARGS__) const _spec \
	{
#define YB_Impl_StringView_search1(_n, _arg) \
	YB_Impl_StringView_search_fn_head(_n,, ynothrow, basic_string_view s, \
		size_type pos = _arg) \
		return ystdex::str_##_n<value_type, size_type, traits_type, \
			npos>(data_, size_, s.data_, pos, s.size_); \
	}
#define YB_Impl_StringView_search34(_n, _arg) \
	YB_Impl_StringView_search_fn_head(_n, YB_NONNULL(2),, const _tChar* s, \
		size_type pos, size_type n) \
		return yconstraint(s), _n(basic_string_view(s, n), pos); \
	} \
	YB_Impl_StringView_search_fn_head(_n, YB_NONNULL(2),, const _tChar* s, \
		size_type pos = _arg) \
		return _n(s, pos, traits_type::length(s)); \
	}
#define YB_Impl_StringView_search_mf(_n, _arg) \
	YB_Impl_StringView_search1(_n, _arg) \
	YB_Impl_StringView_search_fn_head(_n,, ynothrow, _tChar c, \
		size_type pos = _arg) \
		return ystdex::str_##_n<value_type, size_type, traits_type, \
			npos>(data_, size_, c, pos); \
	} \
	YB_Impl_StringView_search34(_n, _arg)
#define YB_Impl_StringView_search_mf2(_n, _arg, _n2) \
	YB_Impl_StringView_search1(_n, _arg) \
	YB_Impl_StringView_search_fn_head(_n,, ynothrow, _tChar c, \
		size_type pos = _arg) \
		return _n2(c, pos); \
	} \
	YB_Impl_StringView_search34(_n, _arg)

	YB_Impl_StringView_search_mf(find, 0)

	YB_Impl_StringView_search_mf(rfind, npos)

	YB_Impl_StringView_search_mf2(find_first_of, 0, find)

	YB_Impl_StringView_search_mf2(find_last_of, npos, rfind)

	YB_Impl_StringView_search_mf(find_first_not_of, 0)

	YB_Impl_StringView_search_mf(find_last_not_of, npos)

#undef YB_Impl_StringView_search_mf
#undef YB_Impl_StringView_search_mf2
#undef YB_Impl_StringView_search1
#undef YB_Impl_StringView_search34
#undef YB_Impl_StringView_search_fn_head
};

//! \relates basic_string_view
//@{
template<typename _tChar, class _tTraits>
yconstfn bool
operator==(basic_string_view<_tChar, _tTraits> x,
	basic_string_view<_tChar, _tTraits> y) ynothrow
{
	return x.compare(y) == 0;
}
template<typename _tChar, class _tTraits>
yconstfn bool
operator==(basic_string_view<_tChar, _tTraits> x,
	yimpl(decay_t<basic_string_view<_tChar, _tTraits>>) y) ynothrow
{
	return x.compare(y) == 0;
}
//! \since build 642
template<typename _tChar, class _tTraits>
yconstfn bool
operator==(yimpl(decay_t<basic_string_view<_tChar, _tTraits>>) x,
	basic_string_view<_tChar, _tTraits> y) ynothrow
{
	return x.compare(y) == 0;
}

template<typename _tChar, class _tTraits>
yconstfn bool
operator<(basic_string_view<_tChar, _tTraits> x,
	basic_string_view<_tChar, _tTraits> y) ynothrow
{
	return x.compare(y) < 0;
}
//! \since build 642
template<typename _tChar, class _tTraits>
yconstfn bool
operator<(basic_string_view<_tChar, _tTraits> x,
	yimpl(decay_t<basic_string_view<_tChar, _tTraits>>) y) ynothrow
{
	return x.compare(y) < 0;
}
//! \since build 642
template<typename _tChar, class _tTraits>
yconstfn bool
operator<(yimpl(decay_t<basic_string_view<_tChar, _tTraits>>) x,
	basic_string_view<_tChar, _tTraits> y) ynothrow
{
	return x.compare(y) < 0;
}

template<typename _tChar, class _tTraits>
std::basic_ostream<_tChar, _tTraits>&
operator<<(std::basic_ostream<_tChar, _tTraits>& os,
	basic_string_view<_tChar, _tTraits> str)
{
	// XXX: Better implementation?
	return os << str.to_string();
}
//@}


using string_view = basic_string_view<char>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
using wstring_view = basic_string_view<wchar_t>;
//@}

} // namespace ystdex;


namespace std
{

/*!
\brief ystdex::optional 散列支持。
\see ISO WG21/N4081 5.11[optional.hash] 。
\since build 640
*/
//@{
template<typename>
struct hash;

template<typename _tChar, class _tTraits>
struct hash<ystdex::basic_string_view<_tChar, _tTraits>>
{
	size_t
	operator()(const ystdex::string_view& k) const yimpl(ynothrow)
	{
		return ystdex::hash_range(k.data(), k.data() + k.size());
	}
};
//@}

} // namespace std;

#endif

