/*
	© 2012-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file string.hpp
\ingroup YStandardEx
\brief ISO C++ 标准字符串扩展。
\version r1987
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-04-26 20:12:19 +0800
\par 修改时间:
	2017-04-09 11:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::String
*/


#ifndef YB_INC_ystdex_string_hpp_
#define YB_INC_ystdex_string_hpp_ 1

#include "memory.hpp" // for remove_rcv_t, false_, is_object, decay_t,
//	std::declval, true_, nested_allocator, is_enum, is_class;
#include <libdefect/string.h> // for std::char_traits, std::initializer_list,
//	std::to_string;
#include "container.hpp" // for "container.hpp", make_index_sequence,
//	index_sequence, begin, end, size, sort_unique, underlying;
#include "cstdio.h" // for yconstraint, vfmtlen;
#include "cstring.h" // for ntctslen;
#include "array.hpp" // for std::bidirectional_iterator_tag, to_array;
#include <istream> // for std::basic_istream;
#include "ios.hpp" // for rethrow_badstate;
#include <ostream> // for std::basic_ostream;
#include <sstream> // for std::ostringstream, std::wostring_stream;
#include <cstdarg> // for std::va_list;

namespace ystdex
{

/*!
\brief 字符串特征。
\note 支持字符类型指针表示的 C 风格字符串和随机序列容器及 std::basic_string 。
\since build 304
*/
template<typename _tString>
struct string_traits
{
	using string_type = remove_rcv_t<_tString>;
	using value_type = remove_rcv_t<decltype(std::declval<string_type>()[0])>;
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
\note 同 std::begin 和 std::end ，但字符数组除外。
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
	return begin(c);
}
template<class _tRange>
yconstfn auto
string_begin(const _tRange& c) -> decltype(c.begin())
{
	return begin(c);
}
//! \since build 664
//@{
template<typename _tChar>
yconstfn _tChar*
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
	return end(c);
}
template<class _tRange>
yconstfn auto
string_end(const _tRange& c) -> decltype(c.end())
{
	return end(c);
}
//! \since build 664
//@{
template<typename _tChar>
yconstfn _tChar*
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
\brief 计算字符串长度。
\since build 664
*/
//@{
template<typename _type>
inline size_t
string_length(const _type* str) ynothrow
{
	return std::char_traits<_type>::length(str);
}
template<typename _type>
yconstfn auto
string_length(const _type& str) -> decltype(size(str))
{
	return size(str);
}
#if __cplusplus <= 201402L
//! \see CWG 1591 。
template<typename _tElem>
yconstfn size_t
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

//! \pre 字符串类型满足 std::basic_string 或 basic_string_view 的操作及异常规范。
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
\since build 598
*/
//@{
template<typename _tString>
inline _tString
quote(const _tString& str, typename string_traits<_tString>::value_type c
	= typename string_traits<_tString>::value_type('"'))
{
	return c + str + c;
}
template<typename _tString, typename _tString2>
inline yimpl(enable_if_t)<!is_convertible<_tString2,
	typename string_traits<_tString>::value_type>::value ,_tString>
quote(const _tString& str, const _tString2& s)
{
	return s + str + s;
}
template<typename _tString, typename _tPrefix, typename _tSuffix>
inline _tString
quote(const _tString& str, const _tPrefix& pfx, const _tSuffix& sfx)
{
	return pfx + str + sfx;
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
	std::string::size_type n(0);
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


/*!
\brief 转换为字符串： std::basic_string 的实例对象。
\note 可与标准库的同名函数共用以避免某些类型转换警告，如 G++ 的 [-Wsign-promo] 。
*/
//@{
//! \since build 308
//@{
inline std::string
to_string(unsigned char val)
{
	return std::to_string(unsigned(val));
}
inline std::string
to_string(unsigned short val)
{
	return std::to_string(unsigned(val));
}
//! \since build 538
template<typename _type>
inline yimpl(enable_if_t)<is_enum<_type>::value, std::string>
to_string(_type val)
{
	using std::to_string;
	using ystdex::to_string;

	return to_string(ystdex::underlying(val));
}
//! \since build 664
template<typename _type, class _tStream = std::ostringstream>
yimpl(enable_if_t)<is_class<_type>::value,
	decltype(std::declval<_tStream&>().str())>
to_string(const _type& x)
{
	_tStream oss;

	oss << x;
	return oss.str();
}
//@}

//! \since build 565
//@{
inline std::wstring
to_wstring(unsigned char val)
{
	return std::to_wstring(unsigned(val));
}
inline std::wstring
to_wstring(unsigned short val)
{
	return std::to_wstring(unsigned(val));
}
//! \since build 664
template<typename _type>
inline yimpl(enable_if_t)<is_enum<_type>::value, std::wstring>
to_wstring(_type val)
{
	using std::to_wstring;
	using ystdex::to_wstring;

	return to_wstring(ystdex::underlying(val));
}
template<typename _type, class _tStream = std::wostringstream>
yimpl(enable_if_t)<is_class<_type>::value,
	decltype(std::declval<_tStream&>().str())>
to_wstring(const _type& x)
{
	_tStream oss;

	oss << x;
	return oss.str();
}
//@}
//@}


//! \since build 542
namespace details
{

template<typename _tString, typename _type>
struct ston_dispatcher;

#define YB_Impl_String_ston_begin(_tString, _type, _n, ...) \
	template<> \
	struct ston_dispatcher<_tString, _type> \
	{ \
		static _type \
		cast(const _tString& str, __VA_ARGS__) \
		{ \
			return _n(str
#define YB_Impl_String_ston_end \
			); \
		} \
	};

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


/*!
\pre 间接断言：第一参数非空。
\since build 640
\bug \c char 以外的模板参数非正确实现。
*/
//@{
/*!
\brief 以 C 标准输出格式的输出 std::basic_string 实例的对象。
\throw std::runtime_error 格式化字符串输出失败。
\note 对 _tString 构造异常中立。
*/
template<typename _tChar, class _tString = std::basic_string<_tChar>>
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
\brief 以 C 标准输出格式的输出 std::basic_string 实例的对象。
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
\brief 显式实例化：以 C 标准输出格式的输出 std::string 对象。
\sa ystdex::sfmt
\since build 350
*/
template YB_ATTR_gnu_printf(1, 2) YB_NONNULL(1) std::string
sfmt<char>(const char*, ...);


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

#endif

