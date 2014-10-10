/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file string.hpp
\ingroup YStandardEx
\brief ISO C++ 标准字符串扩展。
\version r749
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-04-26 20:12:19 +0800
\par 修改时间:
	2014-10-07 12:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::String
*/


#ifndef YB_INC_ystdex_string_hpp_
#define YB_INC_ystdex_string_hpp_ 1

#include "array.hpp" // for ../ydef.h, ystdex::to_array;
#include "container.hpp" // for ystdex::sort_unique;
#include <libdefect/string.h> // for std::char_traits, std::initializer_list,
//	and std::to_string;
#include <cstdio> // for std::vsnprintf
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
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using initializer = std::initializer_list<value_type>;
};


/*!
\ingroup metafunctions
\brief 选择字符串类类型的特定重载避免和其它非字符串类型冲突。
\sa enable_if_t
\since build 483
*/
template<typename _tParam,
	typename = yimpl(decltype(std::declval<_tParam>()[0]))>
using enable_for_string_class_t
	= enable_if_t<is_class<decay_t<_tParam>>::value>;


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


//! \since build 519
namespace details
{

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

} // namespace details;

/*!
\ingroup string_algorithms
\brief 计算字符串长度。
\since build 519
*/
template<typename _type>
yconstfn size_t
string_length(const _type& str)
{
	return details::string_length_dispatcher<_type>::length(str);
}
//@}


//! \since build 450
namespace details
{

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

} // namespace details;

/*!
\ingroup string_algorithms
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
		str.append(&str[0], str.length());
		if(n % 2 != 0)
			str.append(&str[0], len);
	}
}

/*!
\ingroup string_algorithms
\since build 474
*/
//@{
//! \brief 删除字符串中指定的连续前缀字符。
template<class _tString>
inline _tString&&
ltrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return static_cast<_tString&&>(str.erase(0, str.find_first_not_of(t)));
}

//! \brief 删除字符串中指定的连续后缀字符。
template<class _tString>
inline _tString&&
rtrim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return static_cast<_tString&&>(str.erase(str.find_last_not_of(t) + 1));
}

//! \brief 删除字符串中指定的连续前缀与后缀字符。
template<class _tString>
inline _tString&&
trim(_tString&& str, typename string_traits<_tString>::const_pointer t
	= &to_array<typename string_traits<_tString>::value_type>("\n\r\t\v ")[0])
{
	return static_cast<_tString&&>(ystdex::ltrim(ystdex::rtrim(str, t)));
}
//@}

/*!
\ingroup string_algorithms
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

/*!
\ingroup string_algorithms
\brief 以指定字符分割字符序列。
\note 只保留非空结果，不保留分隔字符。
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
\ingroup string_algorithms
\brief 以指定字符分割范围指定的字符串。
\note 只保留非空结果，不保留分隔字符。
\since build 399
*/
template<typename _fPred, typename _fInsert, typename _tRange>
inline void
split(_tRange&& c, _fPred is_delim, _fInsert insert)
{
	split(string_begin(c), string_end(c), is_delim, insert);
}

/*!
\ingroup string_algorithms
\brief 以指定字符分割字符序列。
\note 只保留非空的结果；
	在起始保留分隔字符，除首个字符为非分隔字符时的第一次匹配。
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
\ingroup string_algorithms
\brief 以指定字符分割范围指定的字符串。
\note 只保留除了分隔字符外非空的结果；
	结果保留起始分隔字符，除非是起始非分隔字符第一次匹配。
\since build 408
*/
template<typename _fPred, typename _fInsert, typename _tRange>
inline void
split_l(_tRange&& c, _fPred is_delim, _fInsert insert)
{
	split_l(string_begin(c), string_end(c), is_delim, insert);
}

/*!
\brief 转换为字符串。
\note 可与 <tt>std::to_string</tt> 共用以避免某些类型转换警告，
	如 G++ 的 [-Wsign-promo] 。
\since build 308
*/
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
inline std::string
to_string(_type val, yimpl(enable_if_t<is_enum<_type>::value>* = {}))
{
	using std::to_string;
	using ystdex::to_string;

	return to_string(underlying_type_t<_type>(val));
}
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
\since build 488
\bug char 以外的模板参数非正确实现。
*/
template<typename _tChar>
std::basic_string<_tChar>
vsfmt(const _tChar* fmt, std::va_list args)
{
	std::string str(size_t(std::vsnprintf({}, 0, fmt, args)), _tChar());

	std::vsprintf(&str[0], fmt, args);
	va_end(args);
	return str;
}

/*!
\brief 以 C 标准输出格式的输出 std::basic_string 实例的对象。
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

	std::string str(vsfmt(fmt, args));

	va_end(args);
	return str;
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

} // namespace ystdex;

#endif

