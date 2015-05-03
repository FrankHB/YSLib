/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file string.hpp
\ingroup YStandardEx
\brief ISO C++ 标准字符串扩展。
\version r1263
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-04-26 20:12:19 +0800
\par 修改时间:
	2015-05-03 05:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::String
*/


#ifndef YB_INC_ystdex_string_hpp_
#define YB_INC_ystdex_string_hpp_ 1

#include "memory.hpp" // for ../ydef.h, decay_t, remove_rcv_t, std::declval,
//	has_nested_allocator;
#include <libdefect/string.h> // for std::char_traits, std::initializer_list,
//	std::to_string;
#include "container.hpp" // for ystdex::sort_unique, ystdex::underlying;
#include "array.hpp" // for std::bidirectional_iterator_tag, ystdex::to_array;
#include <istream> // for std::istream;
#include "cstdio.h" // for std::vsnprintf, ystdex::vfmtlen;
#include <cstdarg>

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
	using string_type = decay_t<_tString>;
	using value_type = remove_rcv_t<decltype(std::declval<string_type>()[0])>;
	using traits_type = typename std::char_traits<value_type>;
	//! \since build 592
	//@{
	using allocator_type = typename nested_allocator<string_type>::type;
	using size_type = typename std::allocator_traits<allocator_type>::size_type;
	using difference_type
		= typename std::allocator_traits<allocator_type>::difference_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	//@}
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer
		= typename std::allocator_traits<allocator_type>::const_pointer;
	using initializer = std::initializer_list<value_type>;
};


//! \since build 450
namespace details
{

//! \since build 557
template<typename _type>
struct is_string_class_test
{
	template<typename _tParam>
	static enable_if_t<is_object<decay_t<
		decltype(std::declval<_tParam>()[0])>>::value, true_type>
	test(_tParam*);
	template<typename _tParam>
	static false_type
	test(...);

	static yconstexpr bool value = decltype(test<_type>(nullptr))::value;
};

//! \since build 519
//@{
template<typename _type>
struct string_length_dispatcher
{
	static size_t
	length(const _type& str)
	{
		return str.size();
	}
};

template<typename _type, size_t _vN>
struct string_length_dispatcher<_type[_vN]>
{
	static yconstfn size_t
	length(const _type(&)[_vN]) ynothrow
	{
		return _vN - 1U;
	}
};

template<typename _type>
struct string_length_dispatcher<_type*>
{
	static inline size_t
	length(const _type* str) ynothrow
	{
		return std::char_traits<_type>::length(str);
	}
};
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

} // unnamed namespace;


/*!
\ingroup metafunctions
\brief 判断指定类型是否为字符串类类型。
\since build 557
*/
//@{
template<typename _type>
struct is_string_class : integral_constant<bool, std::is_class<_type>::value
	&& details::is_string_class_test<_type>::value>
{};

//! \note 排除不完整类型的指针。
template<typename _type>
struct is_string_class<_type&> : false_type
{};

//! \note 排除不完整类型的引用。
template<typename _type>
struct is_string_class<_type*> : false_type
{};
//@}


/*!
\ingroup metafunctions
\brief 选择字符串类类型的特定重载避免和其它非字符串类型冲突。
\sa enable_if_t
\since build 483
*/
template<typename _tParam>
using enable_for_string_class_t
	= enable_if_t<is_string_class<decay_t<_tParam>>::value>;


/*!
\note 使用 ADL 访问字符串范围。
\note 同 std::begin 和 std::end ，但字符数组除外。
\note 此处 string_end 语义和 boost::end 相同，但对数组类型不同于 std::end 。
\bug decltype 指定的返回类型不能使用 ADL 。
\see ISO WG21/N3936 20.4.7[iterator.range] 。
\since build 519
*/
//@{
template<class _tRange>
yconstfn auto
string_begin(_tRange& c) -> decltype(c.begin())
{
	using std::begin;

	return begin(c);
}
template<class _tRange>
yconstfn auto
string_begin(const _tRange& c) -> decltype(c.begin())
{
	using std::begin;

	return begin(c);
}
template<typename _type, size_t _vN>
yconstfn _type*
string_begin(_type(&arr)[_vN]) ynothrow
{
	return arr;
}

template<class _tRange>
yconstfn auto
string_end(_tRange& c) -> decltype(c.end())
{
	using std::end;

	return end(c);
}
template<class _tRange>
yconstfn auto
string_end(const _tRange& c) -> decltype(c.end())
{
	using std::end;

	return end(c);
}
template<typename _type, size_t _vN>
yconstfn _type*
string_end(_type(&arr)[_vN]) ynothrow
{
	return arr + _vN - 1U;
}
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
\since build 519
*/
template<typename _type>
yconstfn size_t
string_length(const _type& str)
{
	return details::string_length_dispatcher<_type>::length(str);
}


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

//! \since build 592
//@{
//! \brief 删除字符串中指定的连续字符左侧的字符串。
//@{
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_left(typename string_traits<_tString>::size_type pos, _tString&& str)
{
	return static_cast<_tString&&>(
		pos != decay_t<_tString>::npos ? str.erase(0, pos) : str);
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_left(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return static_cast<_tString&&>(ystdex::erase_left(str.find_last_of(c), str));
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_left(_tString&& str, const remove_reference_t<_tString>& t)
{
	return static_cast<_tString&&>(ystdex::erase_left(str.find_last_of(t), str));
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_left(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return static_cast<_tString&&>(ystdex::erase_left(str.find_last_of(t), str));
}
//@}

//! \brief 删除字符串中指定的连续字符右侧的字符串。
//@{
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_right(typename string_traits<_tString>::size_type pos, _tString&& str)
{
	return static_cast<_tString&&>(
		pos != decay_t<_tString>::npos ? str.erase(pos + 1) : str);
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_right(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return static_cast<_tString&&>(ystdex::erase_right(str.find_last_of(c),
		str));
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_right(_tString&& str, const remove_reference_t<_tString>& t)
{
	return static_cast<_tString&&>(ystdex::erase_right(str.find_last_of(t),
		str));
}
template<class _tString>
inline yimpl(enable_if_t)<is_class<decay_t<_tString>>::value, _tString&&>
erase_right(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
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
inline _tString&&
ltrim(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return static_cast<_tString&&>(str.erase(0, str.find_first_not_of(c)));
}
template<class _tString>
inline _tString&&
ltrim(_tString&& str, const _tString& t)
{
	return static_cast<_tString&&>(str.erase(0, str.find_first_not_of(t)));
}
//! \since build 474
template<class _tString>
inline _tString&&
ltrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return static_cast<_tString&&>(str.erase(0, str.find_first_not_of(t)));
}
//@}

//! \brief 删除字符串中指定的连续后缀字符。
//@{
template<class _tString>
inline _tString&&
rtrim(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return static_cast<_tString&&>(str.erase(str.find_last_not_of(c) + 1));
}
template<class _tString>
inline _tString&&
rtrim(_tString&& str, const remove_reference_t<_tString>& t)
{
	return static_cast<_tString&&>(str.erase(str.find_last_not_of(t) + 1));
}
template<class _tString>
inline _tString&&
rtrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return static_cast<_tString&&>(str.erase(str.find_last_not_of(t) + 1));
}
//@}

//! \brief 删除字符串中指定的连续前缀与后缀字符。
//@{
template<class _tString>
inline _tString&&
trim(_tString&& str, typename string_traits<_tString>::value_type c)
{
	return yforward(ystdex::ltrim(yforward(ystdex::rtrim(yforward(str), c))));
}
template<class _tString>
inline _tString&&
trim(_tString&& str, const _tString& t)
{
	return yforward(ystdex::ltrim(yforward(ystdex::rtrim(yforward(str), t))));
}
template<class _tString>
inline _tString&&
trim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return yforward(ystdex::ltrim(yforward(ystdex::rtrim(yforward(str), t))));
}
//@}
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

//! \note 只保留非空结果，不保留分隔字符。
//@{
/*!
\brief 以指定字符分割字符序列。
\since build 304
*/
template<typename _fPred, typename _fInsert, typename _tIn>
void
split(_tIn b, _tIn e, _fPred is_delim, _fInsert insert)
{
	while(b != e)
	{
		_tIn i(std::find_if_not(b, e, is_delim));

		b = std::find_if(i, e, is_delim);
		if(i != b)
			insert(i, b);
		else
			break;
	}
}
/*!
\brief 以指定字符分割范围指定的字符串。
\since build 399
*/
template<typename _fPred, typename _fInsert, typename _tRange>
inline void
split(_tRange&& c, _fPred is_delim, _fInsert insert)
{
	split(string_begin(c), string_end(c), is_delim, insert);
}
//@}

/*!
\brief 以满足迭代器谓词的指定字符分割字符串。
\since build 545
*/
//@{
template<typename _fPred, typename _fInsert, typename _func, typename _tIn>
void
split_if_iter(_tIn b, _tIn e, _fPred is_delim, _fInsert insert, _func pred)
{
	while(b != e)
	{
		_tIn i(b);
		while(i != e && is_delim(*i) && pred(i))
			++i;
		for(b = i; b != e; ++b)
		{
			b = std::find_if(b, e, is_delim);
			if(pred(b))
				break;
		}
		if(i != b)
			insert(i, b);
		else
			break;
	}
}
template<typename _fPred, typename _fInsert, typename _func, typename _tRange>
inline void
split_if_iter(_tRange&& c, _fPred is_delim, _fInsert insert, _func pred)
{
	split_if_iter(string_begin(c), string_end(c), is_delim, insert, pred);
}
//@}

//! \note 只保留非空结果；保留起始分隔字符，除非无法匹配分隔字符。
//@{
/*!
\brief 以指定字符分割字符序列。
\since build 408
*/
template<typename _fPred, typename _fInsert, typename _tIn>
_tIn
split_l(_tIn b, _tIn e, _fPred is_delim, _fInsert insert)
{
	_tIn i(b);

	while(b != e)
	{
		if(is_delim(*b) && i != b)
		{
			insert(i, b);
			i = b;
		}
		++b;
	}
	if(i != b)
		insert(i, b);
	return i;
}
/*!
\brief 以指定字符分割范围指定的字符串。
\since build 408
*/
template<typename _fPred, typename _fInsert, typename _tRange>
inline void
split_l(_tRange&& c, _fPred is_delim, _fInsert insert)
{
	split_l(string_begin(c), string_end(c), is_delim, insert);
}
//@}
//@}


/*!
\brief 从输入流中取字符串。
\since build 565
*/
//@{
//! \note 同 \c std::getline ，除判断分隔符及附带的副作用由参数的函数对象决定。
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
			// XXX: Rethrow?
			is.setstate(std::ios_base::badbit);
		}
	}
	if(n == 0)
		st |= std::ios_base::failbit;
	if(st)
		is.setstate(st);
	return is;
}

//! \note 同 \c std::getline ，除字符串结尾包含分隔符。
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
\brief 转换为字符串： \c std::basic_string 的实例对象。
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
template<typename _type>
inline yimpl(enable_if_t)<is_enum<_type>::value, std::wstring>
to_wstring(_type val)
{
	using std::to_wstring;
	using ystdex::to_wstring;

	return to_wstring(ystdex::underlying(val));
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
		static inline _type \
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
\brief 以 C 标准输出格式的输出 std::basic_string 实例的对象。
\pre 间接断言：第一参数非空。
\throw std::runtime_error 格式化字符串输出失败。
\since build 488
\bug char 以外的模板参数非正确实现。
*/
template<typename _tChar>
std::basic_string<_tChar>
vsfmt(const _tChar* fmt, std::va_list args)
{
	std::va_list ap;

	va_copy(ap, args);

	const auto l(ystdex::vfmtlen(fmt, ap));

	va_end(ap);
	if(l == size_t(-1))
		throw std::runtime_error("Failed to write formatted string.");

	std::basic_string<_tChar> str(l, _tChar());

	yassume(str.length() > 0 && str[0] == _tChar());
	std::vsprintf(&str[0], fmt, args);
	return str;
}

/*!
\brief 以 C 标准输出格式的输出 std::basic_string 实例的对象。
\pre 间接断言：第一参数非空。
\note Clang++ 对模板声明 attribute 直接提示格式字符串类型错误。
\since build 322
\todo 提供 char 以外的模板参数的正确实现。
*/
template<typename _tChar>
std::basic_string<_tChar>
sfmt(const _tChar* fmt, ...)
{
	std::va_list args;

	va_start(args, fmt);
	try
	{
		std::basic_string<_tChar> str(vsfmt(fmt, args));

		va_end(args);
		return str;
	}
	catch(...)
	{
		va_end(args);
		throw;
	}
}

/*!
\brief 显式实例化：以 C 标准输出格式的输出 std::string 对象。
\sa ystdex::sfmt
\since build 350
*/
template YB_ATTR(format (printf, 1, 2)) YB_NONNULL(1) std::string
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
		ynoexcept(is_nothrow_move_assignable<decay_t<_tString>>::value){
		val = std::move(s);
	});
	return std::move(val);
}

} // namespace ystdex;

#endif

