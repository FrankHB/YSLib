/*
	© 2012-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file string.hpp
\ingroup YStandardEx
\brief ISO C++ 标准字符串扩展。
\version r2871
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-04-26 20:12:19 +0800
\par 修改时间:
	2018-09-05 03:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::String
*/


#ifndef YB_INC_ystdex_string_hpp_
#define YB_INC_ystdex_string_hpp_ 1

#include "memory.hpp" // for allocator_traits, enable_if_t, remove_cvref_t,
//	false_, is_object, decay_t, std::declval, true_, nested_allocator, or_,
//	is_same, is_enum, is_class;
#include "string_view.hpp" // for internal "string_view.hpp" (implying
//	"range.hpp" and "<libdefect/string.h>"), basic_string_view,
//	std::char_traits, std::initializer_list, std::to_string, ntctslen;
#include "container.hpp" // for "container.hpp", make_index_sequence,
//	index_sequence, begin, end, size, sort_unique, underlying, std::hash;
#include "cstdio.h" // for yconstraint, vfmtlen, ystdex::is_null;
#include "array.hpp" // for std::bidirectional_iterator_tag, to_array;
#include <istream> // for std::basic_istream;
#include "ios.hpp" // for rethrow_badstate;
#include <ostream> // for std::basic_ostream;
#include <sstream> // for std::ostringstream, std::wostring_stream;
#include <cstdarg> // for std::va_list;

namespace ystdex
{

//! \since build 833
//@{
inline namespace cpp2017
{

// XXX: Although adopted features from LWG issues have some concerns on
//	compatibility, there is already feature testing macro for the major feature,
//	i.e. WG21 P0254R2 (see WG21 P0941R2).
#if __cpp_lib_string_view >= 201606L || __cplusplus >= 201606L
#	define YB_Impl_String_has_P0254R2 true
#endif

#if YB_Impl_String_has_P0254R2
using std::basic_string;
#else
/*!
\note 所有 find 相关的函数的 noexcept 异常规范同 std::basic_string 。
\note 支持和 std::basic_string 转换以避免修改多数用户代码。
*/
template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
	class _tAlloc = std::allocator<_tChar>>
class basic_string : yimpl(public) std::basic_string<_tChar, _tTraits, _tAlloc>
{
private:
	using base = std::basic_string<_tChar, _tTraits, _tAlloc>;
	using alloc_traits = allocator_traits<_tAlloc>;
	template<typename _tTrait>
	using equal_alloc_or = or_<_tTrait, typename alloc_traits::is_always_equal>;
	using equal_alloc_or_pocma = equal_alloc_or<typename
		alloc_traits::propagate_on_container_move_assignment>;
	using sv_type = basic_string_view<_tChar, _tTraits>;
	template<class _type, class _tRes = void>
	// XXX: Ready for LWG 2946, which is not adopted here yet as it is not in
	//	ISO C++17.
	using enable_if_sv_t = enable_if_t<and_<is_convertible<const _type&,
		sv_type>, not_<is_convertible<const _type*, const base*>>,
		not_<is_convertible<const _type&, const _tChar*>>>::value, _tRes>;

public:
	using size_type = typename base::size_type;
	using iterator = typename base::iterator;
	using const_iterator = typename base::const_iterator;

	static yconstexpr const auto npos{static_cast<size_type>(-1)};

	// NOTE: All signatures in ISO C++17 different from ISO C++11 are addressed
	//	here explicitly. Note the parameter has %base type which reflects the
	//	original %basic_string type, and the return type %basic_string is this
	//	class.

	//! \see LWG 2193 。
	basic_string() ynoexcept_spec(_tAlloc())
		: base()
	{}
	//! \see LWG 2193 。
	explicit
	basic_string(const _tAlloc& a) ynothrow
		: base(a)
	{}
	//! \see LWG 2583 。
	basic_string(const base& str, size_type pos, const _tAlloc& a = _tAlloc())
		: base(str, pos, base::npos, a)
	{}
	basic_string(const base& str, size_type pos, size_type n,
		const _tAlloc& a = _tAlloc())
		: base(str, pos, n, a)
	{}
	//! \see LWG 2742 。
	template<class _type, yimpl(typename = enable_if_sv_t<_type>)>
	basic_string(const _type& t, size_type pos, size_type n,
		const _tAlloc& a = _tAlloc())
		: basic_string(sv_type(t).substr(pos, n), a)
	{}
	//! \see WG21 P0254R2 。
	explicit
	basic_string(sv_type sv, const _tAlloc& a = _tAlloc())
		: basic_string(sv.data(), sv.size(), a)
	{}
	// NOTE: See $2018-07 @ %Documentation::Workflow::Annual2018.
	// XXX: Names would not be safely hidden.
#if __cpp_inheriting_constructors >= 201511L
	using base::base;
#else
	//! \since build 838
	//@{
	YB_NONNULL(2)
	basic_string(const _tChar* s, size_type n, const _tAlloc& a = _tAlloc())
		: base(s, n, a)
	{}
	YB_NONNULL(2)
	basic_string(const _tChar* s, const _tAlloc& a = _tAlloc())
		: base(s, a)
	{}
	basic_string(size_type n, _tChar c, const _tAlloc& a = _tAlloc())
		: base(n, c, a)
	{}
	template<typename _tIn>
	basic_string(_tIn begin, _tIn end, const _tAlloc& a = _tAlloc())
		: base(begin, end, a)
	{}
	basic_string(std::initializer_list<_tChar>, const _tAlloc& = _tAlloc())
	{}
	basic_string(const base& str, const _tAlloc& a)
		: base(str, a)
	{}
	basic_string(base&& str, const _tAlloc& a)
		: base(std::move(str), a)
	{}
	//@}
#endif
	basic_string(const base& str)
		: base(str)
	{}
	basic_string(base&& str)
		: base(std::move(str))
	{}
	basic_string(const basic_string&) = default;
	basic_string(basic_string&&) ynothrow = default;

	//! \see WG21 P0254R2 。
	basic_string&
	operator=(sv_type sv)
	{
		return assign(sv);
	}
	YB_NONNULL(2) basic_string&
	operator=(const _tChar* s)
	{
		return *this = basic_string(s);
	}
	basic_string&
	operator=(_tChar c)
	{
		return *this = basic_string(1, c);
	}
	basic_string&
	operator=(std::initializer_list<_tChar> il)
	{
		return *this = basic_string(il);
	}
	basic_string&
	operator=(const base& str)
	{
		static_cast<base&>(*this) = str;
		return *this;
	}
	basic_string&
	operator=(base&& str) ynoexcept(equal_alloc_or_pocma::value)
	{
		static_cast<base&>(*this) = std::move(str);
		return *this;
	}
	basic_string&
	operator=(const basic_string&) = default;
	basic_string&
	operator=(basic_string&&) ynoexcept(equal_alloc_or_pocma::value) = default;

	basic_string&
	operator+=(const base& str)
	{
		static_cast<base&>(*this) += str;
		return *this;
	}
	basic_string&
	operator+=(const basic_string& str)
	{
		return *this += static_cast<const base&>(str);
	}
	//! \see WG21 P0254R2 。
	basic_string&
	operator+=(sv_type sv)
	{
		static_cast<base&>(*this) += sv;
		return *this;
	}
	YB_NONNULL(2) basic_string&
	operator+=(const _tChar* s)
	{
		static_cast<base&>(*this) += s;
		return *this;
	}
	basic_string&
	operator+=(_tChar c)
	{
		static_cast<base&>(*this) += c;
		return *this;
	}
	basic_string&
	operator+=(std::initializer_list<_tChar> il)
	{
		static_cast<base&>(*this) += il;
		return *this;
	}

	//! \see WG21 P0254R2 。
	operator sv_type() const ynothrow
	{
		return sv_type(this->data(), this->size());
	}

	basic_string&
	append(const base& str)
	{
		base::append(str);
		return *this;
	}
	//! \see LWG 2268 。
	basic_string&
	append(const base& str, size_type pos, size_type n = npos)
	{
		base::append(str, pos, n);
		return *this;
	}
	//! \see WG21 P0254R2 。
	//@{
	basic_string&
	append(sv_type sv)
	{
		return append(sv.data(), sv.size());
	}
	//! \see LWG 2758 。
	template<typename _type>
	yimpl(enable_if_sv_t)<_type, basic_string&>
	append(const _type& t, size_type pos, size_type n = npos)
	{
		sv_type sv = t;

		if(!(pos > sv.size()))
		{
			const size_type r(sv.size() - pos);

			return append(sv.data() + pos, n < r ? n : r);
		}
		throw std::out_of_range("basic_string::append");
	}
	//@}
	YB_NONNULL(2) basic_string&
	append(const _tChar* s)
	{
		base::append(s);
		return *this;
	}
	YB_NONNULL(2) basic_string&
	append(const _tChar* s, size_type n)
	{
		base::append(s, n);
		return *this;
	}
	basic_string&
	append(size_type n, _tChar c)
	{
		base::append(n, c);
		return *this;
	}
	template<typename _tIn>
	basic_string&
	append(_tIn first, _tIn last)
	{
		base::append(first, last);
		return *this;
	}
	basic_string&
	append(std::initializer_list<_tChar> il)
	{
		base::append(il);
		return *this;
	}

	basic_string&
	assign(const base& str)
	{
		return *this = str;
	}
	basic_string&
	assign(base&& str) ynoexcept(equal_alloc_or_pocma::value)
	{
		return *this = std::move(str);
	}
	//! \see LWG 2268 。
	basic_string&
	assign(const base& str, size_type pos, size_type n = npos)
	{
		base::assign(str, pos, n);
		return *this;
	}
	//! \see WG21 P0254R2 。
	//@{
	basic_string&
	assign(sv_type sv)
	{
		return assign(sv.data(), sv.size());
	}
	//! \see LWG 2758 。
	template<typename _type>
	yimpl(enable_if_sv_t)<_type, basic_string&>
	assign(const _type& t, size_type pos, size_type n = npos)
	{
		sv_type sv = t;

		if(!(pos > sv.size()))
		{
			const size_type r(sv.size() - pos);

			return assign(sv.data() + pos, n < r ? n : r);
		}
		throw std::out_of_range("basic_string::assign");
	}
	//@}
	YB_NONNULL(2) basic_string&
	assign(const _tChar* s)
	{
		base::assign(s);
		return *this;
	}
	YB_NONNULL(2) basic_string&
	assign(const _tChar* s, size_type n)
	{
		base::assign(s, n);
		return *this;
	}
	basic_string&
	assign(size_type n, _tChar c)
	{
		base::assign(n, c);
		return *this;
	}
	template<typename _tIn>
	basic_string&
	assign(_tIn first, _tIn last)
	{
		base::assign(first, last);
		return *this;
	}
	basic_string&
	assign(std::initializer_list<_tChar> il)
	{
		base::assign(il);
		return *this;
	}

	basic_string&
	insert(size_type pos, const base& str)
	{
		base::insert(pos, str);
		return *this;
	}
	//! \see LWG 2268 。
	basic_string&
	insert(size_type pos1, const base& str, size_type pos2, size_type n = npos)
	{
		base::insert(pos1, str, pos2, n);
		return *this;
	}
	//! \see WG21 P0254R2 。
	//@{
	basic_string&
	insert(size_type pos, sv_type sv)
	{
		return insert(pos, sv.data(), sv.size());
	}
	//! \see LWG 2758 。
	template<typename _type>
	yimpl(enable_if_sv_t)<_type, basic_string&>
	insert(size_type pos1, const _type& t, size_type pos2, size_type n = npos)
	{
		sv_type sv = t;

		if(!(pos1 > sv.size() || pos2 > sv.size()))
		{
			const size_type r(sv.size() - pos2);

			return insert(pos1, sv.data() + pos2, n < r ? n : r);
		}
		throw std::out_of_range("basic_string::insert");
	}
	//@}
	YB_NONNULL(3) basic_string&
	insert(size_type pos, const _tChar* s)
	{
		base::insert(pos, s);
		return *this;
	}

	YB_NONNULL(3) basic_string&
	insert(size_type pos, const _tChar* s, size_type n)
	{
		base::insert(pos, s, n);
		return *this;
	}
	basic_string&
	insert(size_type pos, size_type n, _tChar c)
	{
		base::insert(pos, n, c);
		return *this;
	}
	iterator
	insert(const_iterator p, _tChar c)
	{
		return base::insert(p, c);
	}
	iterator
	insert(const_iterator p, size_type n, _tChar c)
	{
		return base::insert(p, n, c);
	}
	template<typename _tIn>
	iterator
	insert(const_iterator p, _tIn first, _tIn last)
	{
		return base::insert(p, first, last);
	}
	iterator
	insert(const_iterator p, std::initializer_list<_tChar> il)
	{
		return base::insert(p, il);
	}

	basic_string&
	replace(size_type pos1, size_type n1, const base& str)
	{
		base::replace(pos1, n1, str);
		return *this;
	}
	//! \see LWG 2268 。
	basic_string&
	replace(size_type pos1, size_type n1, const base& str,
		size_type pos2, size_type n2 = npos)
	{
		base::replace(pos1, n1, str, pos2, n2);
		return *this;
	}
	//! \see WG21 P0254R2 。
	//@{
	basic_string&
	replace(size_type pos1, size_type n1, sv_type sv)
	{
		return replace(pos1, n1, sv.data(), sv.size());
	}
	//! \see LWG 2758 。
	template<typename _type>
	yimpl(enable_if_sv_t)<_type, basic_string&>
	replace(size_type pos1, size_type n1, const _type& t, size_type pos2,
		size_type n2 = npos)
	{
		sv_type sv = t;

		if(!(pos1 > sv.size() || pos2 > sv.size()))
		{
			const size_type r(sv.size() - pos2);

			return replace(pos1, n1, sv.data() + pos2, n2 < r ? n2 : r);
		}
		throw std::out_of_range("basic_string::replace");
	}
	//@}
	YB_NONNULL(4) basic_string&
	replace(size_type pos, size_type n, const _tChar* s)
	{
		base::replace(pos, n, s);
		return *this;
	}
	YB_NONNULL(4) basic_string&
	replace(size_type pos1, size_type n1, const _tChar* s, size_type n2 = npos)
	{
		base::replace(pos1, n1, s, n2);
		return *this;
	}
	basic_string&
	replace(size_type pos1, size_type n1, size_type n2, _tChar c)
	{
		base::replace(pos1, n1, base(n2, c));
		return *this;
	}
	basic_string&
	replace(const_iterator i1, const_iterator i2, const base& str)
	{
		base::replace(i1, i2, str);
		return *this;
	}
	//! \see WG21 P0254R2 。
	basic_string&
	replace(const_iterator i1, const_iterator i2, sv_type sv)
	{
		return replace(i1 - this->begin(), i2 - i1, sv);
	}
	YB_NONNULL(4) basic_string&
	replace(const_iterator i1, const_iterator i2, const _tChar* s, size_type n)
	{
		base::replace(i1, i2, s, n);
		return *this;
	}
	basic_string&
	replace(const_iterator i1, const_iterator i2, size_type n, _tChar c)
	{
		base::replace(i1, i2, n, c);
		return *this;
	}
	template<typename _tIn>
	basic_string&
	replace(const_iterator i1, const_iterator i2, _tIn j1, _tIn j2)
	{
		base::replace(i1, i2, j1, j2);
		return *this;
	}
	basic_string&
	replace(const_iterator i1, const_iterator i2,
		std::initializer_list<_tChar> il)
	{
		base::replace(i1, i2, il);
		return *this;
	}

	void
	swap(basic_string& s) ynoexcept(equal_alloc_or<typename
		alloc_traits::propagate_on_container_swap>::value)
	{
		base::swap(s);
	}
	//! \since build 834
	friend void
	swap(basic_string& lhs, basic_string& rhs) ynoexcept_spec(lhs.swap(rhs))
	{
		lhs.swap(rhs);
	}

	// XXX: For simplicity, 'find*' and 'compare' funtions are not reworded as
	//	the standard by dispatching to %basic_string_view overloads. This
	//	simplifies the implementation and speed-up translation time.
	// TODO: The overloads with noexcept exception (even changed in ISO C++ 14)
	//	are also not explicitly specified here to simplify the evolution because
	//	they are still open to be resolved, e.g. LWG 2836.

	//! \see WG21 P0254R2 。
	size_type
	find(sv_type sv, size_type pos = 0) const
		ynothrow
	{
		return find(sv.data(), sv.size(), pos);
	}
	using base::find;

	//! \see WG21 P0254R2 。
	size_type
	rfind(sv_type sv, size_type pos = npos) const ynothrow
	{
		return rfind(sv.data(), sv.size(), pos);
	}
	using base::rfind;

	//! \see WG21 P0254R2 。
	size_type
	find_first_of(sv_type sv, size_type pos = 0) const ynothrow
	{
		return find_first_of(sv.data(), sv.size(), pos);
	}
	using base::find_first_of;

	//! \see WG21 P0254R2 。
	size_type
	find_last_of(sv_type sv, size_type pos = npos) const ynothrow
	{
		return find_last_of(sv.data(), sv.size(), pos);
	}
	using base::find_last_of;

	//! \see WG21 P0254R2 。
	size_type
	find_first_not_of(sv_type sv, size_type pos = 0) const ynothrow
	{
		return find_first_not_of(sv.data(), sv.size(), pos);
	}
	using base::find_first_not_of;

	//! \see WG21 P0254R2 。
	size_type
	find_last_not_of(sv_type sv, size_type pos = npos) const ynothrow
	{
		return find_last_not_of(sv.data(), sv.size(), pos);
	}
	using base::find_last_not_of;

	//! \see WG21 P0254R2 。
	//@{
	int
	compare(sv_type sv) const ynothrow
	{
		return sv_type(this->data(), this->size()).compare(sv);
	}
	//! \see LWG 2771 。
	//@{
	int
	compare(size_type pos1, size_type n1, sv_type sv) const
	{
		return sv_type(this->data(), this->size()).substr(pos1, n1).compare(sv);
	}
	//! \see LWG 2758 。
	template<typename _type>
	yimpl(enable_if_sv_t)<_type, int>
	compare(size_type pos1, size_type n1, const _type& t, size_type pos2,
		size_type n2 = npos) const
	{
		sv_type sv = t;

		return sv_type(this->data(), this->size()).substr(pos1, n1)
			.compare(sv.substr(pos2, n2));
	}
	//@}
	//@}
	//! \see LWG 2268 。
	using base::compare;

	friend basic_string
	operator+(const basic_string& lhs, const basic_string& rhs)
	{
		return basic_string(lhs).append(rhs);
	}
	friend basic_string
	operator+(basic_string&& lhs, const basic_string& rhs)
	{
		return std::move(lhs.append(rhs));
	}
	friend basic_string
	operator+(const basic_string& lhs, basic_string&& rhs)
	{
		return std::move(rhs.insert(0, lhs));
	}
	friend basic_string
	operator+(basic_string&& lhs, basic_string&& rhs)
	{
		return std::move(lhs.append(rhs));
	}
	YB_NONNULL(1) friend basic_string
	operator+(const _tChar* lhs, const basic_string& rhs)
	{
		return basic_string(lhs) + rhs;
	}
	YB_NONNULL(1) friend basic_string
	operator+(const _tChar* lhs, basic_string&& rhs)
	{
		return std::move(rhs.insert(0, lhs));
	}
	friend basic_string
	operator+(_tChar lhs, const basic_string& rhs)
	{
		return basic_string(1, lhs) + rhs;
	}
	friend basic_string
	operator+(_tChar lhs, basic_string&& rhs)
	{
		return std::move(rhs.insert(0, 1, lhs));
	}
	YB_NONNULL(2) friend basic_string
	operator+(const basic_string& lhs, const _tChar* rhs)
	{
		return lhs + basic_string(rhs);
	}
	YB_NONNULL(2) friend basic_string
	operator+(basic_string&& lhs, const _tChar* rhs)
	{
		return std::move(lhs.append(rhs));
	}
	friend basic_string
	operator+(const basic_string& lhs, _tChar rhs)
	{
		return lhs + basic_string(1, rhs);
	}
	friend basic_string
	operator+(basic_string&& lhs, _tChar rhs)
	{
		return std::move(lhs.append(1, rhs));
	}
};
#endif
//@}

using string = basic_string<char>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;
using wstring = basic_string<wchar_t>;

} // inline namesapce cpp2017;


/*!
\brief 字符串特征。
\note 支持字符类型指针表示的 C 风格字符串和随机序列容器及 basic_string 。
\since build 304
*/
template<typename _tString>
struct string_traits
{
	using string_type = remove_cvref_t<_tString>;
	using value_type = remove_cvref_t<decltype(std::declval<string_type>()[0])>;
	using traits_type = typename std::char_traits<value_type>;
	//! \since build 592
	//@{
	using allocator_type = _t<nested_allocator<string_type>>;
	using size_type = typename std::allocator_traits<allocator_type>::size_type;
	using difference_type
		= typename std::allocator_traits<allocator_type>::difference_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	//@}
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using initializer = std::initializer_list<value_type>;
};


//! \since build 450
namespace details
{

//! \since build 640
//@{
template<typename _type, typename = void>
struct is_string_like : false_
{};

template<typename _type>
struct is_string_like<_type, enable_if_t<
	is_object<decay_t<decltype(std::declval<_type>()[0])>>::value>> : true_
{};
//@}


//! \todo 支持 std::forward_iterator_tag 重载。
template<typename _tFwd1, typename _tFwd2, typename _fPred>
bool
ends_with_iter_dispatch(_tFwd1 b, _tFwd1 e, _tFwd2 bt, _tFwd2 et,
	_fPred comp, std::bidirectional_iterator_tag)
{
	auto i(e);
	auto it(et);

	while(i != b && it != bt)
		if(!comp(*--i, *--it))
			return {};
	return it == bt;
}


//! \since build 640
//@{
template<size_t>
struct str_algos;

template<>
struct str_algos<0>
{
	//! \since build 659
	template<class _tString,
		typename _tSize = typename string_traits<_tString>::size_type>
	static yconstfn auto
	erase_left(_tString& s, _tSize n) ynothrowv
		-> yimpl(decltype(s.remove_prefix(n), s))
	{
		return yconstraint(n <= s.size()), s.remove_prefix(n), s;
	}

	//! \since build 659
	template<class _tString,
		typename _tSize = typename string_traits<_tString>::size_type>
	static yconstfn auto
	erase_right(_tString& s, _tSize n) ynothrowv
		-> yimpl(decltype(s.remove_suffix(n), s))
	{
		return yconstraint(n < s.size()), trim_right_pos(s, n);
	}

	//! \since build 781
	template<class _tString,
		typename _tSize = typename string_traits<_tString>::size_type>
	static yconstfn auto
	trim_left_pos(_tString& s, _tSize n) ynothrowv
		-> yimpl(decltype(s.remove_prefix(n), s))
	{
		return yconstraint(n < s.size() || n == _tSize(-1)),
			erase_left(s, n != _tSize(-1) ? n : _tSize(s.size())), s;
	}

	//! \since build 659
	template<class _tString,
		typename _tSize = typename string_traits<_tString>::size_type>
	static yconstfn auto
	trim_right_pos(_tString& s, _tSize n) ynothrowv
		-> yimpl(decltype(s.remove_suffix(n), s))
	{
		return yconstraint(n < s.size() || n == _tSize(-1)),
			s.remove_suffix(s.size() - n - 1), s;
	}
};

template<>
struct str_algos<1>
{
	//! \since build 659
	template<class _tString,
		typename _tSize = typename string_traits<_tString>::size_type>
	static yconstfn auto
	erase_left(_tString& s, _tSize n) ynothrowv
		-> yimpl(decltype(s.erase(0, n)))
	{
		return yconstraint(n <= s.size()), s.erase(0, n);
	}

	//! \since build 659
	template<class _tString,
		typename _tSize = typename string_traits<_tString>::size_type>
	static yconstfn auto
	erase_right(_tString& s, _tSize n) ynothrowv
		-> yimpl(decltype(s.erase(n + 1)))
	{
		return yconstraint(n < s.size()), s.erase(n + 1);
	}

	//! \since build 781
	template<class _tString,
		typename _tSize = typename string_traits<_tString>::size_type>
	static yconstfn auto
	trim_left_pos(_tString& s, _tSize n) ynothrowv
		-> yimpl(decltype(s.erase(0, n)))
	{
		return yconstraint(n < s.size() || n == _tSize(-1)), s.erase(0, n);
	}

	//! \since build 781
	template<class _tString,
		typename _tSize = typename string_traits<_tString>::size_type>
	static yconstfn auto
	trim_right_pos(_tString& s, _tSize n) ynothrowv
		-> yimpl(decltype(s.erase(n + 1)))
	{
		return yconstraint(n < s.size() || n == _tSize(-1)), s.erase(n + 1);
	}
};


template<yimpl(typename = make_index_sequence<2>)>
struct str_algo;

template<>
struct str_algo<index_sequence<>>
{
	void
	erase_left() = delete;

	void
	erase_right() = delete;

	//! \since build 781
	void
	trim_left_pos() = delete;

	//! \since build 781
	void
	trim_right_pos() = delete;
};

template<size_t _vIdx, size_t... _vSeq>
struct str_algo<index_sequence<_vIdx, _vSeq...>>
	: str_algos<_vIdx>, str_algo<index_sequence<_vSeq...>>
{
	using str_algos<_vIdx>::erase_left;
	using str_algo<index_sequence<_vSeq...>>::erase_left;
	using str_algos<_vIdx>::erase_right;
	using str_algo<index_sequence<_vSeq...>>::erase_right;
	//! \since build 781
	//@{
	using str_algos<_vIdx>::trim_left_pos;
	using str_algo<index_sequence<_vSeq...>>::trim_left_pos;
	using str_algos<_vIdx>::trim_right_pos;
	using str_algo<index_sequence<_vSeq...>>::trim_right_pos;
	//@}
};
//@}

} // unnamed namespace;


//! \ingroup unary_type_traits
//@{
/*!
\brief 判断指定类型是否为类字符串类型。
\since build 695
*/
template<typename _type>
struct is_string_like : details::is_string_like<_type>
{};

/*!
\brief 判断指定类型是否为字符串类类型。
\since build 557
*/
template<typename _type>
struct is_string_class : and_<is_class<_type>, is_string_like<_type>>
{};
//@}


/*!
\ingroup metafunctions
\brief 选择字符串类类型的特定重载避免和其它非字符串类型冲突。
\sa enable_if_t
\since build 620
*/
template<typename _tParam, typename _type = void>
using enable_for_string_class_t
	= enable_if_t<is_string_class<decay_t<_tParam>>::value, _type>;


/*!
\note 使用 ADL 访问字符串范围。
\note 同 ystdex::begin 和 ystdex::end ，但字符数组除外。
\note 此处 string_end 语义和 boost::end 相同，但对数组类型不同于 std::end 。
\bug decltype 指定的返回类型不能使用 ADL 。
\see WG21 N3936 20.4.7[iterator.range] 。
\since build 519
*/
//@{
template<class _tRange>
yconstfn auto
string_begin(_tRange& c) -> decltype(c.begin())
{
	return ystdex::begin(c);
}
template<class _tRange>
yconstfn auto
string_begin(const _tRange& c) -> decltype(c.begin())
{
	return ystdex::begin(c);
}
//! \since build 664
//@{
template<typename _tChar>
YB_NONNULL(1) yconstfn _tChar*
string_begin(_tChar* str) ynothrow
{
	return yconstraint(str), str;
}
#if __cplusplus <= 201402L
//! \see CWG 1591 。
template<typename _tElem>
yconstfn auto
string_begin(std::initializer_list<_tElem> il) -> decltype(il.begin())
{
	return il.begin();
}
#endif
//@}

template<class _tRange>
yconstfn auto
string_end(_tRange& c) -> decltype(c.end())
{
	return ystdex::end(c);
}
template<class _tRange>
yconstfn auto
string_end(const _tRange& c) -> decltype(c.end())
{
	return ystdex::end(c);
}
//! \since build 664
//@{
template<typename _tChar>
YB_NONNULL(1) yconstfn _tChar*
string_end(_tChar* str) ynothrow
{
	return str + ystdex::ntctslen(str);
}
#if __cplusplus <= 201402L
//! \see CWG 1591 。
template<typename _tElem>
yconstfn auto
string_end(std::initializer_list<_tElem> il) -> decltype(il.end())
{
	return il.end();
}
#endif
//@}
//@}


/*!	\defgroup string_algorithms String Algorithms
\addtogroup algorithms
\brief 字符串算法。
\note 模板形参关键字 \c class 表示仅支持类类型对象字符串。
\since build 304
*/
//@{
/*!
\brief 判断字符串是否为空。
\since build 835
*/
//@{
//! \pre 指针指定的字符串为 NTCTS 。
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE yconstfn bool
string_empty(const _tChar* str) ynothrowv
{
	return ystdex::is_null(*str);
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn auto
string_empty(const _type& str) -> decltype(size(str))
{
	return empty(str);
}
#if __cplusplus <= 201402L
//! \see CWG 1591 。
template<typename _tElem>
YB_ATTR_nodiscard YB_PURE yconstfn size_t
string_empty(std::initializer_list<_tElem> il)
{
	return il.empty();
}
#endif
//@}

/*!
\brief 计算字符串长度。
\since build 664
*/
//@{
/*!
\pre 指针指定的字符串为 NTCTS 。
\since build 835
*/
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE inline size_t
string_length(const _tChar* str) ynothrowv
{
	return std::char_traits<_tChar>::length(str);
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn auto
string_length(const _type& str) -> decltype(size(str))
{
	return size(str);
}
#if __cplusplus <= 201402L
//! \see CWG 1591 。
template<typename _tElem>
YB_ATTR_nodiscard YB_PURE yconstfn size_t
string_length(std::initializer_list<_tElem> il)
{
	return il.size();
}
#endif
//@}


/*!
\note 使用 ADL string_begin 和 string_end 指定范围迭代器。
\note 除 ADL 外接口同 Boost.StringAlgo 。
\sa ystdex::string_begin, ystdex::string_end
\since build 450
*/
//@{
//! \brief 判断第一个参数指定的串是否以第二个参数起始。
//@{
//! \since build 519
template<typename _tRange1, typename _tRange2, typename _fPred>
bool
begins_with(const _tRange1& input, const _tRange2& test, _fPred comp)
{
	return ystdex::string_length(test) <= ystdex::string_length(input)
		&& std::equal(string_begin(test), string_end(test),
		string_begin(input), comp);
}
//! \since build 519
template<typename _tRange1, typename _tRange2>
inline bool
begins_with(const _tRange1& input, const _tRange2& test)
{
	return ystdex::begins_with(input, test, is_equal());
}
//@}

//! \brief 判断第一个参数指定的串是否以第二个参数结束。
//@{
template<typename _tRange1, typename _tRange2, typename _fPred>
inline bool
ends_with(const _tRange1& input, const _tRange2& test, _fPred comp)
{
	// NOTE: See $2014-07 @ %Documentation::Workflow::Annual2014.
	return details::ends_with_iter_dispatch(string_begin(input),
		string_end(input), string_begin(test), string_end(test), comp, typename
		std::iterator_traits<remove_reference_t<decltype(string_begin(input))>>
		::iterator_category());
}
template<typename _tRange1, typename _tRange2>
inline bool
ends_with(const _tRange1& input, const _tRange2& test)
{
	return ystdex::ends_with(input, test, is_equal());
}
//@}
//@}


/*!
\brief 判断是否存在子串。
\since build 546
\todo 添加序列容器子串重载版本；优化：避免构造子串对象。
*/
//@{
template<class _tString, typename _type>
inline bool
exists_substr(const _tString& str, const _type& sub)
{
	return str.find(sub) != _tString::npos;
}
template<class _tString, typename _type>
inline bool
exists_substr(const _tString& str, const typename _tString::value_type* p_sub)
{
	yconstraint(p_sub);

	return str.find(p_sub) != _tString::npos;
}
//@}


//! \ingroup NTCTSUtil
//@{
/*!
\brief NTCTS 正规化：保留空字符之前的字符。
\post <tt>str.length() == ystdex::ntctslen(str.c_str())</tt> 。
\since build 606
*/
template<class _tString>
auto
normalize(_tString& str) -> decltype(str.resize(ystdex::ntctslen(str.c_str())))
{
	return str.resize(ystdex::ntctslen(str.c_str()));
}

/*!
\brief 复制不超过指定长度的 NTCTS 。
\note 目标字符串短于指定长度的部分可能被填充空字符。
\warning 源字符串在指定长度内没有空字符则目标字符串不保证以空字符结尾。
\since build 604
*/
//@{
//! \pre 断言：指针参数非空。
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline _tString&&
ntctsncpy(_tString&& str, const typename string_traits<_tString>::const_pointer
	s, const typename string_traits<_tString>::size_type n)
{
	yconstraint(s);
	return static_cast<_tString&&>(str = decay_t<_tString>(s, n));
}
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline _tString&&
ntctsncpy(_tString&& str, _tString&& s,
	const typename string_traits<_tString>::size_type n)
{
	auto sub(yforward(s));

	sub.resize(n);
	return static_cast<_tString&&>(str = std::move(sub));
}
//@}
//@}


/*!
\brief 限制长度不超过指定参数。
\since build 608
*/
template<class _tString>
inline _tString&&
restrict_length(_tString&& str,
	const typename string_traits<_tString>::size_type n)
{
	if(n < str.length())
		str.resize(n);
	return static_cast<_tString&&>(str);
}


/*!
\brief 取字母表：有序的字符串的不重复序列。
\since build 414
*/
template<class _tString>
_tString
alph(_tString& str)
{
	_tString res(str);

	ystdex::sort_unique(res);
	return res;
}

/*!
\brief 重复串接。
\param str 被串接的字符串的引用。
\param n 串接结果包含原字符串的重复次数。
\pre 断言： <tt>1 < n</tt> 。
\since build 414
\todo 检查 reserve 。
*/
template<class _tString>
void
concat(_tString& str, size_t n)
{
	yconstraint(n != 0);
	if(1 < n)
	{
		const auto len(str.length());

		ystdex::concat(str, n / 2);
		str.append(str.data(), str.length());
		if(n % 2 != 0)
			str.append(str.data(), len);
	}
}

//! \pre 字符串类型满足 basic_string 或 basic_string_view 的操作及异常规范。
//@{
//! \since build 592
//@{
//! \brief 删除字符串中指定位置或指定字符最先出现的位置的左侧的字符串。
//@{
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_left(typename string_traits<_tString>::size_type pos, _tString&& str)
{
	return static_cast<_tString&&>(pos != decay_t<_tString>::npos
		? details::str_algo<>::erase_left(str, pos) : str);
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_left(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return
		static_cast<_tString&&>(ystdex::erase_left(str.find_first_of(c), str));
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_left(_tString&& str, const remove_reference_t<_tString>& t)
{
	return
		static_cast<_tString&&>(ystdex::erase_left(str.find_first_of(t), str));
}
/*!
\pre 断言：指针参数非空。
\since build 600
*/
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_left(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>(" \f\n\r\t\v")[0])
{
	yconstraint(t);
	return
		static_cast<_tString&&>(ystdex::erase_left(str.find_first_of(t), str));
}
//@}

//! \brief 删除字符串中指定位置或指定字符最后出现的位置的右侧的字符串。
//@{
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_right(typename string_traits<_tString>::size_type pos, _tString&& str)
{
	return static_cast<_tString&&>(pos != decay_t<_tString>::npos
		? details::str_algo<>::erase_right(str, pos) : str);
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_right(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return static_cast<_tString&&>(ystdex::erase_right(str.find_last_of(c),
		str));
}
//! \since build 659
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_right(_tString&& str, const remove_reference_t<_tString>& t)
{
	return static_cast<_tString&&>(ystdex::erase_right(str.find_last_of(t),
		str));
}
/*!
\pre 断言：指针参数非空。
\since build 600
*/
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_right(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>(" \f\n\r\t\v")[0])
{
	yconstraint(t);
	return static_cast<_tString&&>(ystdex::erase_right(str.find_last_of(t),
		str));
}
//@}
//@}

//! \since build 552
//@{
//! \brief 删除字符串中指定的连续前缀字符。
//@{
template<class _tString>
yconstfn _tString&&
ltrim(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return static_cast<_tString&&>(
		details::str_algo<>::trim_left_pos(str, str.find_first_not_of(c)));
}
//! \since build 659
template<class _tString>
inline _tString&&
ltrim(_tString&& str, const _tString& t)
{
	return static_cast<_tString&&>(
		details::str_algo<>::trim_left_pos(str, str.find_first_not_of(t)));
}
/*!
\pre 断言：指针参数非空。
\since build 600
*/
template<class _tString>
yconstfn _tString&&
ltrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>(" \f\n\r\t\v")[0])
{
	return yconstraint(t), static_cast<_tString&&>(
		details::str_algo<>::trim_left_pos(str, str.find_first_not_of(t)));
}
//@}

//! \brief 删除字符串中指定的连续后缀字符。
//@{
template<class _tString>
yconstfn _tString&&
rtrim(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return static_cast<_tString&&>(
		details::str_algo<>::trim_right_pos(str, str.find_last_not_of(c)));
}
//! \since build 659
template<class _tString>
yconstfn _tString&&
rtrim(_tString&& str, const remove_reference_t<_tString>& t) ynothrowv
{
	return static_cast<_tString&&>(
		details::str_algo<>::trim_right_pos(str, str.find_last_not_of(t)));
}
/*!
\pre 断言：指针参数非空。
\since build 600
*/
template<class _tString>
yconstfn _tString&&
rtrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>(" \f\n\r\t\v")[0])
{
	return yconstraint(t), static_cast<_tString&&>(
		details::str_algo<>::trim_right_pos(str, str.find_last_not_of(t)));
}
//@}

//! \brief 删除字符串中指定的连续前缀与后缀字符。
//@{
template<class _tString>
yconstfn _tString&&
trim(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return
		yforward(ystdex::ltrim(yforward(ystdex::rtrim(yforward(str), c)), c));
}
//! \since build 659
template<class _tString>
yconstfn _tString&&
trim(_tString&& str, const _tString& t) ynothrowv
{
	return
		yforward(ystdex::ltrim(yforward(ystdex::rtrim(yforward(str), t)), t));
}
/*!
\pre 断言：指针参数非空。
\since build 600
*/
template<class _tString>
yconstfn _tString&&
trim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>(" \f\n\r\t\v")[0])
{
	return yconstraint(t),
		yforward(ystdex::ltrim(yforward(ystdex::rtrim(yforward(str), t)), t));
}
//@}
//@}
//@}


//! \since build 599
//@{
//! \brief 取前缀子字符串。
//@{
template<typename _tString, typename... _tParams>
inline _tString
find_prefix(const _tString& str, _tParams&&... args)
{
	if(!str.empty())
	{
		const auto pos(str.find(yforward(args)...));

		if(pos != _tString::npos)
			return str.substr(0, pos);
	}
	return {};
}
//@}

//! \brief 取后缀子字符串。
//@{
template<typename _tString, typename... _tParams>
inline _tString
find_suffix(const _tString& str, _tParams&&... args)
{
	if(!str.empty())
	{
		const auto pos(str.rfind(yforward(args)...));

		if(pos != _tString::npos)
			return str.substr(pos + 1);
	}
	return {};
}
//@}
//@}

/*!
\brief 取相同的前缀和后缀元素。
\pre 取前缀和后缀操作无副作用。
\pre 字符串非空。
\since build 697
\todo 检查非成员 \c front 和 back \c 。
\todo 支持前缀和后缀字符串。
\todo 扩展到一般容器。
*/
//@{
template<typename _tString>
yconstfn typename string_traits<_tString>::value_type
get_quote_mark_nonstrict(const _tString& str)
{
	return str.front() == str.back() ? str.front()
		: typename string_traits<_tString>::value_type();
}
//@}

/*!
\brief 取添加前缀和后缀的字符串。
\pre 断言：删除的字符串不大于串长。
\since build 801
*/
//@{
template<typename _tString>
inline decay_t<_tString>
quote(_tString&& str, typename string_traits<decay_t<_tString>>::value_type c
	= typename string_traits<decay_t<_tString>>::value_type('"'))
{
	return c + yforward(str) + c;
}
template<typename _tString, typename _tString2>
inline yimpl(enable_if_t)<!is_convertible<_tString2, typename
	string_traits<decay_t<_tString>>::value_type>::value, decay_t<_tString>>
quote(_tString&& str, const _tString2& s)
{
	return s + yforward(str) + s;
}
template<typename _tString, typename _tPrefix, typename _tSuffix>
inline auto
quote(_tString&& str, _tPrefix&& pfx, _tSuffix&& sfx)
	-> decltype(yforward(pfx) + yforward(str) + yforward(sfx))
{
	return yforward(pfx) + yforward(str) + yforward(sfx);
}
//@}

/*!
\brief 取删除前缀和后缀的子字符串。
\pre 断言：删除的字符串不大于串长。
\since build 304
*/
//@{
template<typename _tString>
inline _tString
get_mid(const _tString& str, typename _tString::size_type l = 1)
{
	yassume(!(str.size() < l * 2));
	return str.substr(l, str.size() - l * 2);
}
template<typename _tString>
inline _tString
get_mid(const _tString& str, typename _tString::size_type l,
	typename _tString::size_type r)
{
	yassume(!(str.size() < l + r));
	return str.substr(l, str.size() - l - r);
}
//@}
//@}



/*!
\brief 从输入流中取字符串。
\since build 565
*/
//@{
//! \note 同 std::getline ，除判断分隔符及附带的副作用由参数的函数对象决定。
template<typename _tChar, class _tTraits, class _tAlloc, typename _func>
std::basic_istream<_tChar, _tTraits>&
extract(std::basic_istream<_tChar, _tTraits>& is,
	std::basic_string<_tChar, _tTraits, _tAlloc>& str, _func f)
{
	typename std::basic_string<_tChar, _tTraits, _tAlloc>::size_type n(0);
	auto st(std::ios_base::goodbit);

	if(const auto k
		= typename std::basic_istream<_tChar, _tTraits>::sentry(is, true))
	{
		const auto msize(str.max_size());
		const auto p_buf(is.rdbuf());

		yassume(p_buf);
		str.erase();
		try
		{
			const auto eof(_tTraits::eof());

			for(auto c(p_buf->sgetc()); ; c = p_buf->snextc())
			{
				if(_tTraits::eq_int_type(c, eof))
				{
					st |= std::ios_base::eofbit;
					break;
				}
				if(f(c, *p_buf))
				{
					++n;
					break;
				}
				if(!(str.length() < msize))
				{
					st |= std::ios_base::failbit;
					break;
				}
				str.append(1, _tTraits::to_char_type(c));
				++n;
			}
		}
		catch(...)
		{
			// NOTE: See LWG 91.
			rethrow_badstate(is, std::ios_base::badbit);
		}
	}
	if(n == 0)
		st |= std::ios_base::failbit;
	if(st)
		is.setstate(st);
	return is;
}

//! \note 同 std::getline ，除字符串结尾包含分隔符。
//@{
template<typename _tChar, class _tTraits, class _tAlloc>
std::basic_istream<_tChar, _tTraits>&
extract_line(std::basic_istream<_tChar, _tTraits>& is,
	std::basic_string<_tChar, _tTraits, _tAlloc>& str, _tChar delim)
{
	const auto d(_tTraits::to_int_type(delim));

	return ystdex::extract(is, str,
		[d, &str](typename std::basic_istream<_tChar, _tTraits>::int_type c,
		std::basic_streambuf<_tChar, _tTraits>& sb) -> bool{
		if(_tTraits::eq_int_type(c, d))
		{
			sb.sbumpc();
			// NOTE: If not appended here, this function template shall be
			//	equivalent with %std::getline.
			str.append(1, d);
			return true;
		}
		return {};
	});
}
template<typename _tChar, class _tTraits, class _tAlloc>
inline std::basic_istream<_tChar, _tTraits>&
extract_line(std::basic_istream<_tChar, _tTraits>& is,
	std::basic_string<_tChar, _tTraits, _tAlloc>& str)
{
	return ystdex::extract_line(is, str, is.widen('\n'));
}

/*!
\brief 同 \c ystdex::extract_line ，但允许分隔符包含附加的前缀字符。
\note 默认 \c LF 作为基准分隔符，前缀为 \c CR ，即接受 \c LF 和 <tt>CR+LF</tt> 。
\note 一般配合二进制方式打开的流使用，以避免不必要的分隔符转换。
*/
template<typename _tChar, class _tTraits, class _tAlloc>
std::basic_istream<_tChar, _tTraits>&
extract_line_cr(std::basic_istream<_tChar, _tTraits>& is,
	std::basic_string<_tChar, _tTraits, _tAlloc>& str, _tChar delim_cr = '\r',
	_tChar delim = '\n')
{
	const auto cr(_tTraits::to_int_type(delim_cr));
	const auto d(_tTraits::to_int_type(delim));

	return ystdex::extract(is, str,
		[cr, d, &str](typename std::basic_istream<_tChar, _tTraits>::int_type c,
		std::basic_streambuf<_tChar, _tTraits>& sb) -> bool{
		if(_tTraits::eq_int_type(c, d))
			str.append(1, d);
		else if(_tTraits::eq_int_type(c, cr)
			&& _tTraits::eq_int_type(sb.sgetc(), d))
		{
			sb.sbumpc();
			str.append(1, cr);
			str.append(1, d);
		}
		else
			return {};
		sb.sbumpc();
		return true;
	});
}
//@}
//@}


/*!
\brief 非格式输出。
\since build 599
*/
//@{
template<typename _tChar, class _tString>
std::basic_ostream<_tChar, typename _tString::traits_type>&
write(std::basic_ostream<_tChar, typename _tString::traits_type>& os,
	const _tString& str, typename _tString::size_type pos = 0,
	typename _tString::size_type n = _tString::npos)
{
	const auto len(str.length());

	if(pos < len)
		os.write(&str[pos], std::streamsize(std::min(n, len - pos)));
	return os;
}
//! \since build 619
template<typename _tChar, class _tTraits, size_t _vN>
std::basic_ostream<_tChar, _tTraits>&
write(std::basic_ostream<_tChar, _tTraits>& os, const _tChar(&s)[_vN])
{
	return os.write(std::addressof(s[0]), std::streamsize(size(s)));
}

/*!
\note 参数数组作为字符串字面量。
\since build 619
*/
template<typename _tChar, class _tTraits, size_t _vN>
std::basic_ostream<_tChar, _tTraits>&
write_literal(std::basic_ostream<_tChar, _tTraits>& os, const _tChar(&s)[_vN])
{
	static_assert(0 < _vN, "Empty string literal found.");

	return
		os.write(std::addressof(s[0]), std::streamsize(size(s) - 1));
}
//@}

//! \since build 833
template<class _tStream>
using stream_str_t = decltype(std::declval<_tStream&>().str());

//! \since build 833
namespace details
{

template<class _tStream, class _tString, typename = void>
struct stream_str_det;

template<class _tStream, class _tString>
struct stream_str_det<_tStream, _tString, void_t<stream_str_t<_tStream>>>
{
	using result_type = stream_str_t<_tStream>;
	using type = cond_t<is_convertible<result_type, _tString>,
		result_type, _tString>;
};

template<typename _type>
using excluded_tostr
	= or_<is_same<_type, unsigned short>, is_same<_type, unsigned char>>;

template<typename _type>
inline auto
arithmetic_to_string(_type val, false_) -> decltype(std::to_string(val))
{
	return std::to_string(val);
}
inline string
arithmetic_to_string(unsigned val, true_)
{
	return std::to_string(val);
}

template<typename _type>
inline auto
arithmetic_to_wstring(_type val, false_) -> decltype(std::to_wstring(val))
{
	return std::to_wstring(val);
}
inline wstring
arithmetic_to_wstring(unsigned val, true_)
{
	return std::to_wstring(val);
}

} // namespace details;

/*!
\brief 转换为字符串： basic_string 的实例对象。
\note 可与标准库的同名函数共用以避免某些类型转换警告，如 G++ 的 [-Wsign-promo] 。
\since build 833
*/
//@{
template<typename _type>
inline yimpl(enable_if_t)<is_arithmetic<_type>::value, string>
to_string(_type val)
{
	return details::arithmetic_to_string(val, details::excluded_tostr<_type>());
}
template<typename _type>
inline yimpl(enable_if_t)<is_enum<_type>::value, string>
to_string(_type val)
{
	using ystdex::to_string;

	return to_string(ystdex::underlying(val));
}
template<typename _type, class _tStream = std::ostringstream,
	class _tString = string>
yimpl(enable_if_t)<is_class<_type>::value,
	_t<details::stream_str_det<_tStream, _tString>>>
to_string(const _type& x)
{
	_tStream oss;

	oss << x;
	return oss.str();
}

template<typename _type>
inline yimpl(enable_if_t)<is_arithmetic<_type>::value, wstring>
to_wstring(_type val)
{
	return details::arithmetic_to_wstring(val, details::excluded_tostr<_type>());
}
template<typename _type>
inline yimpl(enable_if_t)<is_enum<_type>::value, wstring>
to_wstring(_type val)
{
	using ystdex::to_wstring;

	return to_wstring(ystdex::underlying(val));
}
template<typename _type, class _tStream = std::wostringstream,
	class _tString = wstring>
yimpl(enable_if_t)<is_class<_type>::value,
	_t<details::stream_str_det<_tStream, _tString>>>
to_wstring(const _type& x)
{
	_tStream oss;

	oss << x;
	return oss.str();
}
//@}


//! \since build 542
namespace details
{

//! \since build 833
//@{
template<typename _tString, typename _type, typename = void>
struct ston_dispatcher;

template<typename _tChar, typename _type, class _tTraits, class _tAlloc>
struct ston_dispatcher<basic_string<_tChar, _tTraits, _tAlloc>, _type,
	enable_if_t<not_<is_same<basic_string<_tChar, _tTraits, _tAlloc>,
	std::basic_string<_tChar, _tTraits, _tAlloc>>>::value>>
	: ston_dispatcher<std::basic_string<_tChar, _tTraits, _tAlloc>, _type>
{};

#define YB_Impl_String_ston_begin(_tString, _type, _n, ...) \
	template<> \
	struct ston_dispatcher<_tString, _type, void> \
	{ \
		static _type \
		cast(const _tString& str, __VA_ARGS__) \
		{ \
			return _n(str
#define YB_Impl_String_ston_end \
			); \
		} \
	};
//@}

#define YB_Impl_String_ston_i(_tString, _type, _n) \
	YB_Impl_String_ston_begin(_tString, _type, _n, size_t* idx = {}, \
		int base = 10), idx, base \
	YB_Impl_String_ston_end
#define YB_Impl_String_ston_i_std(_type, _n) \
	YB_Impl_String_ston_i(std::string, _type, std::_n)
YB_Impl_String_ston_i_std(int, stoi)
YB_Impl_String_ston_i_std(long, stol)
YB_Impl_String_ston_i_std(unsigned long, stoul)
YB_Impl_String_ston_i_std(long long, stoll)
YB_Impl_String_ston_i_std(unsigned long long, stoull)
#undef YB_Impl_String_ston_i_std
#undef YB_Impl_String_ston_i

#define YB_Impl_String_ston_f(_tString, _type, _n) \
	YB_Impl_String_ston_begin(_tString, _type, _n, size_t* idx = {}), idx \
	YB_Impl_String_ston_end
#define YB_Impl_String_ston_f_std(_type, _n) \
	YB_Impl_String_ston_f(std::string, _type, std::_n)
#	ifndef __BIONIC__
YB_Impl_String_ston_f_std(float, stof)
YB_Impl_String_ston_f_std(double, stod)
YB_Impl_String_ston_f_std(long double, stold)
#	endif
#undef YB_Impl_String_ston_f_std
#undef YB_Impl_String_ston_f

#undef YB_Impl_String_ston_end
#undef YB_Impl_String_ston_begin

} // namespace details;

/*!
\brief 转换表示数的字符串。
\since build 542
\todo 支持 std::string 以外类型字符串。
\todo 支持标准库以外的转换。
*/
template<typename _type, typename _tString, typename... _tParams>
inline _type
ston(const _tString& str, _tParams&&... args)
{
	return details::ston_dispatcher<decay_t<_tString>, _type>::cast(str,
		yforward(args)...);
}


//! \since build 833
//@{
/*!
\pre 间接断言：第一参数非空。
\bug \c char 以外的模板参数非正确实现。
*/
//@{
/*!
\brief 以 C 标准输出格式的输出 basic_string 实例的对象。
\throw std::runtime_error 格式化字符串输出失败。
\note 对 _tString 构造异常中立。
*/
template<typename _tChar, class _tString = basic_string<_tChar>>
YB_NONNULL(1) _tString
vsfmt(const _tChar* fmt, std::va_list args)
{
	std::va_list ap;

	va_copy(ap, args);

	const auto l(ystdex::vfmtlen(fmt, ap));

	va_end(ap);
	if(l == size_t(-1))
		throw std::runtime_error("Failed to write formatted string.");

	_tString str(l, _tChar());

	if(l != 0)
	{
		yassume(str.length() > 0 && str[0] == _tChar());
		std::vsprintf(&str[0], fmt, args);
	}
	return str;
}

/*!
\brief 以 C 标准输出格式的输出 basic_string 实例的对象。
\note 使用 ADL 访问可变参数。
\note Clang++ 对模板声明 attribute 直接提示格式字符串类型错误。
*/
template<typename _tChar>
YB_NONNULL(1) auto
sfmt(const _tChar* fmt, ...)
	-> decltype(vsfmt(fmt, std::declval<std::va_list>()))
{
	std::va_list args;

	va_start(args, fmt);
	try
	{
		auto str(vsfmt(fmt, args));

		va_end(args);
		return str;
	}
	catch(...)
	{
		va_end(args);
		throw;
	}
}
//@}

/*!
\brief 显式实例化：以 C 标准输出格式的输出 string 对象。
\sa ystdex::sfmt
*/
template YB_ATTR_gnu_printf(1, 2) YB_NONNULL(1) string
sfmt<char>(const char*, ...);
//@}


/*!
\ingroup string_algorithms
\brief 过滤前缀：存在前缀时处理移除前缀后的子串。
\since build 520
*/
template<typename _type, typename _tString, typename _func>
bool
filter_prefix(const _tString& str, const _type& prefix, _func f)
{
	if(ystdex::begins_with(str, prefix))
	{
		auto&& sub(str.substr(string_length(prefix)));

		if(!sub.empty())
			f(std::move(sub));
		return true;
	}
	return {};
}

/*!
\brief 选择性过滤前缀。
\return 指定前缀存在时为去除前缀的部分，否则为参数指定的替代值。
\since build 565
*/
template<typename _type, typename _tString>
_tString
cond_prefix(const _tString& str, const _type& prefix, _tString&& val = {})
{
	ystdex::filter_prefix(str, prefix, [&](_tString&& s)
		ynoexcept(is_nothrow_move_assignable<decay_t<_tString>>()){
		val = std::move(s);
	});
	return std::move(val);
}

} // namespace ystdex;


#if !YB_Impl_String_has_P0254R2
//! \since build 833
namespace std
{

/*!
\brief ystdex::basic_string 散列支持。
\since build 833
*/
template<typename _tChar, class _tTraits, class _tAlloc>
struct hash<ystdex::basic_string<_tChar, _tTraits, _tAlloc>>
	: hash<basic_string<_tChar, _tTraits, _tAlloc>>
{};

} // namespace std;
#endif

#undef YB_Impl_String_has_P0254R2

#endif

