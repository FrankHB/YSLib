/*
	© 2009-2016, 2018-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstring.h
\ingroup YStandardEx
\brief ISO C 标准字符串扩展。
\version r2618
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2009-12-27 17:31:14 +0800
\par 修改时间:
	2019-11-04 17:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CString

扩展标准库头 \c \<cstring> ，提供可在翻译时求值的 C 风格字符串操作等接口。
*/


#ifndef YB_INC_ystdex_cstring_h_
#define YB_INC_ystdex_cstring_h_ 1

#include "type_pun.hpp" // for is_in_types, enable_if_t, nor_,
//	is_trivially_replaceable, cond_t, replace_cast, enable_if_replaceable_t;
#include <cstring> // for std::strlen, std::strcpy, std::strncpy;
#include <libdefect/string.h> // for std::char_traits;
#include "cassert.h" // for yconstraint;
#include "cctype.h" // for ystdex::tolower;
#include <cwchar> // for std::wcscpy, std::wcsncpy;
#include <algorithm> // for std::min, std::lexicographical_compare;

namespace ystdex
{

/*!
\brief 带空指针检查的字符串长度计算。
\return 当字符指针非空时为 std::strlen 计算的串长，否则为 0 。
*/
YB_PURE inline size_t
strlen_n(const char* s)
{
	return s ? std::strlen(s) : 0;
}

/*!
\brief 带空指针检查的字符串复制。
\return 成功时为复制的字符串，失败时为空指针。

当目标字符串和源字符串都非空时用 std::strcpy 复制字符串。
*/
inline char*
strcpy_n(char* d, const char* s)
{
	return d && s ? std::strcpy(d, s) : nullptr;
}

/*!
\brief 带空指针检查的字符串连接。
\return 返回目标参数。

对传入参数进行非空检查后串接指定的两个字符串，结果复制至指定位置。
*/
YB_API char*
strcpycat(char*, const char*, const char*);

/*!
\brief 带空指针检查的字符串连接复制。
\return 目标参数。

对传入参数进行非空检查后串接指定的两个字符串，
结果复制至用指定分配函数（默认为 std::malloc）新分配的空间。
*/
YB_API char*
strcatdup(const char*, const char*, void*(*)(size_t) = std::malloc);


/*!
\ingroup unary_type_traits
\brief 判断字符类型是否被 ISO C++ 指定提供 <tt>std::char_traits</tt> 的特化。
\note 同时是 WG21 N4100 指定的编码字符类型。
\see WG21 N4100 5[fs.req]/1 。
\since build 544
*/
template<typename _tChar>
struct is_char_specialized_in_std
	: is_in_types<_tChar, char, wchar_t, char16_t, char32_t>
{};


//! \since build 630
//@{
/*!
\ingroup metafunctions
\brief 选择不和 char 或 wchar_t 可替换字符类类型的特定重载以避免冲突。
*/
template<typename _tChar, typename _type = void>
using enable_if_irreplaceable_char_t = enable_if_t<nor_<
	is_trivially_replaceable<_tChar, char>,
	is_trivially_replaceable<_tChar, wchar_t>>::value, _type>;


/*!
\brief 指定和 \c wchar_t 互相替换存储的非 \c char 内建字符类型。
\warning 不同类型的非空字符的值是否可以替换取决于实现定义。
\note 若存在这样的类型，为 \c char16_t 或 \c char32_t 之一，否则为 \c void 。
*/
using uchar_t = cond_t<is_trivially_replaceable<wchar_t, char16_t>, char16_t,
	cond_t<is_trivially_replaceable<wchar_t, char32_t>, char32_t, void>>;
//@}


//! \since build 653
//@{
//! \brief 使用 <tt>std::char_traits::eq</tt> 判断是否为空字符。
template<typename _tChar>
YB_STATELESS yconstfn bool
is_null(_tChar c) ynothrow
{
	return std::char_traits<_tChar>::eq(c, _tChar());
}


/*!	\defgroup NTCTSUtil null-terminated character string utilities
\ingroup nonmodifying_algorithms
\ingroup string_algorithms
\brief 简单 NTCTS 操作。
\pre 指针指定的字符串或指针和长度指定的范围为 NTCTS 。
\see ISO C++03 (17.1.12, 17.3.2.1.3.2) 。

以指针或指针和长度指定的范围参数作为 NTCTS 对字符串序列进行非修改操作。
NTCTS(null-terminated character string) 即空字符标记结束的字符串，
除了结束字符外没有空字符。简单指不包括 NTMBS(null-terminated mutibyte string) ，
按等宽字符考虑。
*/
//@{
//! \pre 断言： <tt>s1 && s2</tt> 。
//@{
//! \brief 按字典序比较简单 NTCTS 。
//@{
//! \note 语义同 std::basic_string<_tChar>::compare ，但忽略指定长度。
//@{
template<typename _tChar>
YB_NONNULL(1, 2) YB_PURE int
ntctscmp(const _tChar* s1, const _tChar* s2) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);

	while(*s1 == *s2 && !ystdex::is_null(*s1))
		yunseq(++s1, ++s2);
	return int(*s1 - *s2);
}
//! \since build 658
//@{
YB_NONNULL(1, 2) YB_PURE inline int
ntctscmp(const char* s1, const char* s2) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);
	return std::strcmp(s1, s2);
}
YB_NONNULL(1, 2) YB_PURE inline int
ntctscmp(const wchar_t* s1, const wchar_t* s2) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);
	return std::wcscmp(s1, s2);
}
YB_NONNULL(1, 2) YB_PURE inline int
ntctscmp(const uchar_t* s1, const uchar_t* s2) ynothrowv
{
	return ntctscmp(replace_cast<const wchar_t*>(s1),
		replace_cast<const wchar_t*>(s2));
}
//@}
//@}
//! \note 语义同 std::basic_string<_tChar>::compare 。
template<typename _tChar>
YB_NONNULL(1, 2) YB_PURE int
ntctscmp(const _tChar* s1, const _tChar* s2, size_t n) ynothrowv
{
	return yconstraint(s1), yconstraint(s2),
		std::char_traits<_tChar>::compare(s1, s2, n);
}
//! \note 语义同 std::lexicographical_compare 。
template<typename _tChar>
YB_NONNULL(1, 2) YB_PURE int
ntctscmp(const _tChar* s1, const _tChar* s2, size_t n1, size_t n2) ynothrowv
{
	return yconstraint(s1), yconstraint(s2),
		std::lexicographical_compare(s1, s1 + n1, s2, s2 + n2);
}

//@}

/*!
\brief 按字典序比较简单 NTCTS （忽略大小写）。
\note 语义同 std::basic_string<_tChar>::compare ，但忽略指定长度和大小写，
	使用字符串结束符判断结束。
*/
template<typename _tChar>
YB_NONNULL(1, 2) YB_PURE int
ntctsicmp(const _tChar* s1, const _tChar* s2) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);

	while(ystdex::towlower(*s1) == ystdex::towlower(*s2) && !ystdex::is_null(s2))
		yunseq(++s1, ++s2);
	return int(ystdex::towlower(*s1) - ystdex::towlower(*s2));
}
/*!
\brief 按字典序比较不超过指定长度的简单 NTCTS （忽略大小写）。
\note 语义同 std::basic_string<_tChar>::compare ，但忽略大小写，
	使用字符串结束符判断结束。
*/
template<typename _tChar>
YB_NONNULL(1, 2) YB_PURE int
ntctsicmp(const _tChar* s1, const _tChar* s2, size_t n) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);

	using int_type = typename std::char_traits<_tChar>::int_type;
	int_type d(0);

	while(n-- != 0 && (d = int_type(ystdex::tolower(*s1))
		- int_type(ystdex::tolower(*s2))) == int_type(0)
		&& !ystdex::is_null(*s2))
		yunseq(++s1, ++s2);
	return int(d);
}

//! \pre 复制的 NTCTS 存储不重叠。
//@{
//! \brief 复制 NTCTS 。
//@{
template<typename _tChar>
YB_NONNULL(1, 2) yimpl(enable_if_irreplaceable_char_t<_tChar, _tChar*>)
ntctscpy(_tChar* s1, const _tChar* s2) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);

	const auto res(s1);

	while(!ystdex::is_null(*s1++ = *s2++))
		;
	return res;
}
YB_NONNULL(1, 2) inline char*
ntctscpy(char* s1, const char* s2) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);
	return std::strcpy(s1, s2);
}
YB_NONNULL(1, 2) inline wchar_t*
ntctscpy(wchar_t* s1, const wchar_t* s2) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);
	return std::wcscpy(s1, s2);
}
YB_NONNULL(1, 2) inline wchar_t*
ntctscpy(uchar_t* s1, const uchar_t* s2) ynothrowv
{
	return ntctscpy(replace_cast<wchar_t*>(s1),
		replace_cast<const wchar_t*>(s2));
}
template<typename _tChar,
	yimpl(typename = enable_if_replaceable_t<_tChar, char>)>
YB_NONNULL(1, 2) inline _tChar*
ntctscpy(_tChar* s1, const _tChar* s2) ynothrowv
{
	return ystdex::replace_cast<_tChar*>(ystdex::ntctscpy(ystdex::replace_cast<
		char*>(s1), ystdex::replace_cast<const char*>(s2)));
}
template<typename _tChar>
YB_NONNULL(1, 2) inline yimpl(enable_if_replaceable_t)<_tChar, wchar_t, _tChar*>
ntctscpy(_tChar* s1, const _tChar* s2) ynothrowv
{
	return ystdex::replace_cast<_tChar*>(ystdex::ntctscpy(ystdex::replace_cast<
		wchar_t*>(s1), ystdex::replace_cast<const wchar_t*>(s2)));
}
//@}
//! \brief 复制确定源长度的 NTCTS 。
template<typename _tChar>
YB_NONNULL(1, 2) _tChar*
ntctscpy(_tChar* s1, const _tChar* s2, size_t n) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);
	return yunseq(std::char_traits<_tChar>::copy(s1, s2, n), s1[n] = _tChar());
}

/*!
\brief 复制不超过指定长度的 NTCTS 。
\note 目标字符串短于指定长度的部分会被填充空字符。
\warning 源字符串在指定长度内没有空字符则目标字符串不以空字符结尾。
*/
//@{
template<typename _tChar>
YB_NONNULL(1, 2) yimpl(enable_if_irreplaceable_char_t<_tChar, _tChar*>)
ntctsncpy(_tChar* s1, const _tChar* s2, size_t n) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);

	const auto res(s1);

	while(n != 0)
	{
		--n;
		if(ystdex::is_null(*s1++ = *s2++))
			break;
	}
	while(n-- != 0)
		*s1++ = _tChar();
	return res;
}
YB_NONNULL(1, 2) inline char*
ntctsncpy(char* s1, const char* s2, size_t n) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);
	return std::strncpy(s1, s2, n);
}
YB_NONNULL(1, 2) inline wchar_t*
ntctsncpy(wchar_t* s1, const wchar_t* s2, size_t n) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);
	return std::wcsncpy(s1, s2, n);
}
YB_NONNULL(1, 2) inline wchar_t*
ntctsncpy(uchar_t* s1, const uchar_t* s2, size_t n) ynothrowv
{
	return ntctsncpy(replace_cast<wchar_t*>(s1),
		replace_cast<const wchar_t*>(s2), n);
}
template<typename _tChar,
	yimpl(typename = enable_if_replaceable_t<_tChar, char>)>
YB_NONNULL(1, 2) inline _tChar*
ntctsncpy(_tChar* s1, const _tChar* s2, size_t n) ynothrowv
{
	return ystdex::replace_cast<_tChar*>(ystdex::ntctsncpy(ystdex::replace_cast<
		char*>(s1), ystdex::replace_cast<const char*>(s2), n));
}
template<typename _tChar>
YB_NONNULL(1, 2) inline yimpl(enable_if_replaceable_t)<_tChar, wchar_t, _tChar*>
ntctsncpy(_tChar* s1, const _tChar* s2, size_t n) ynothrowv
{
	return ystdex::replace_cast<_tChar*>(ystdex::ntctsncpy(ystdex::replace_cast<
		wchar_t*>(s1), ystdex::replace_cast<const wchar_t*>(s2), n));
}
//@}
//@}

/*!
\brief 转移 NTCTS 。
\note 复制可能重叠的 NTCTS 存储。
*/
template<typename _tChar>
YB_NONNULL(1, 2) _tChar*
ntctsmove(_tChar* s1, const _tChar* s2, size_t n) ynothrowv
{
	yconstraint(s1),
	yconstraint(s2);
	return yunseq(std::char_traits<_tChar>::move(s1, s2, n), s1[n] = _tChar());
}
//@}

//! \pre 断言： \c s 。
//@{
/*!
\brief 在简单 NTCTS 内查找字符。
\return 若未找到为空指针值，否则为指向首个查找到的字符的指针值。
*/
//@{
template<typename _tChar>
YB_NONNULL(1) YB_PURE _tChar*
ntctschr(_tChar* s, remove_const_t<_tChar> c) ynothrowv
{
	yconstraint(s);
	for(auto p(s); !ystdex::is_null(*p); ++p)
		if(*p == c)
			return p;
	return {};
}
YB_NONNULL(1) YB_PURE inline char*
ntctschr(char* s, char c) ynothrowv
{
	yconstraint(s);
	return std::strchr(s, c);
}
YB_NONNULL(1) YB_PURE inline const char*
ntctschr(const char* s, char c) ynothrowv
{
	yconstraint(s);
	return std::strchr(s, c);
}
YB_NONNULL(1) YB_PURE inline wchar_t*
ntctschr(wchar_t* s, wchar_t c) ynothrowv
{
	yconstraint(s);
	return std::wcschr(s, c);
}
YB_NONNULL(1) YB_PURE inline const wchar_t*
ntctschr(const wchar_t* s, wchar_t c) ynothrowv
{
	yconstraint(s);
	return std::wcschr(s, c);
}
YB_NONNULL(1) YB_PURE inline uchar_t*
ntctschr(uchar_t* s, uchar_t c) ynothrowv
{
	return replace_cast<uchar_t*>(
		ntctschr(replace_cast<wchar_t*>(s), wchar_t(c)));
}
YB_NONNULL(1) YB_PURE inline const uchar_t*
ntctschr(const uchar_t* s, uchar_t c) ynothrowv
{
	return replace_cast<const uchar_t*>(
		ntctschr(replace_cast<const wchar_t*>(s), wchar_t(c)));
}
//@}
/*!
\brief 在简单 NTCTS 的指定前若干字符内查找字符。
\pre <tt>[s, s + n)</tt> 有效。
\note 语义同 std::char_traits<_tChar>::find 。
*/
template<typename _tChar>
YB_NONNULL(1) YB_PURE inline size_t
ntctsfind(const _tChar* s, size_t n, _tChar c) ynothrowv
{
	return yconstraint(s), std::char_traits<_tChar>::find(s, n, c);
}

/*!
\brief 计算简单 NTCTS 长度。
\note 语义同 std::char_traits<_tChar>::length 。
*/
template<typename _tChar>
YB_NONNULL(1) YB_PURE inline size_t
ntctslen(const _tChar* s) ynothrowv
{
	return yconstraint(s), std::char_traits<_tChar>::length(s);
}
//! \brief 计算不超过指定长度的简单 NTCTS 长度。
template<typename _tChar>
YB_NONNULL(1) YB_PURE inline size_t
ntctslen(const _tChar* s, size_t n) ynothrowv
{
	return ystdex::ntctsfind(s, n, _tChar());
}
//@}
//@}


//! \ingroup constexpr_algorithms
//@{
/*!
\brief 计算简单 NTCTS 长度。
\note 语义同 std::char_traits<_tChar>::length 。
*/
template<typename _tChar>
YB_NONNULL(1) YB_PURE yconstfn size_t
const_ntctslen(const _tChar* s) ynothrowv
{
	return ystdex::is_null(*s) ? 0 : ystdex::const_ntctslen(s + 1) + 1;
}

//! \brief 计算简单 NTCTS 中的指定字符数。
template<typename _tChar>
YB_NONNULL(1) YB_PURE yconstfn size_t
const_ntctscnt(const _tChar* s, _tChar c) ynothrowv
{
	return ystdex::is_null(*s) ? 0 : ystdex::const_ntctscnt(s + 1, c)
		+ std::char_traits<_tChar>::eq(*s, c);
}

/*!
\brief 比较简单 NTCTS 。
\note 语义同 std::basic_string<_tChar>::compare ，但忽略指定长度。
*/
template<typename _tChar>
YB_NONNULL(1, 2) YB_PURE yconstfn typename std::char_traits<_tChar>::int_type
const_ntctscmp(const _tChar* s1, const _tChar* s2) ynothrowv
{
	return !std::char_traits<_tChar>::eq(*s1, *s2) || ystdex::is_null(*s1)
		|| ystdex::is_null(*s2) ? *s1 - *s2
		: ystdex::const_ntctscmp(s1 + 1, s2 + 1);
}
/*!
\brief 比较不超过指定长度的简单 NTCTS 。
\note 语义同 std::basic_string<_tChar>::compare 。
*/
template<typename _tChar>
YB_NONNULL(1, 2) YB_PURE yconstfn typename std::char_traits<_tChar>::int_type
const_ntctscmp(const _tChar* s1, const _tChar* s2, size_t n) ynothrowv
{
	return n == 0 ? _tChar() : (!std::char_traits<_tChar>::eq(*s1, *s2)
		|| ystdex::is_null(*s1) || ystdex::is_null(*s2) ? *s1 - *s2
		: ystdex::const_ntctscmp(s1 + 1, s2 + 1, n - 1));
}

/*!
\brief 在简单 NTCTS 中顺序查找指定字符。
\return 在查找结束时经过的字符数。
*/
template<typename _tChar>
YB_NONNULL(1) YB_PURE yconstfn size_t
const_ntctsfind(const _tChar* s, _tChar c) ynothrowv
{
	return ystdex::is_null(*s) || std::char_traits<_tChar>::eq(*s, c)
		? 0 : ystdex::const_ntctsfind(s + 1, c) + 1;
}
/*!
\brief 在简单 NTCTS 中顺序查找第指定次数出现的指定字符。
\return 在查找结束时经过的字符数。
*/
template<typename _tChar>
YB_NONNULL(1) YB_PURE yconstfn size_t
const_ntctsfind(const _tChar* s, _tChar c, size_t n) ynothrowv
{
	return n == 0 || ystdex::is_null(*s) ? 0 : (std::char_traits<_tChar>
		::eq(*s, c) ? ystdex::const_ntctsfind(s + 1, c, n - 1) + (n != 1)
		: ystdex::const_ntctsfind(s + 1, c, n) + 1);
}
/*!
\brief 在简单 NTCTS 中顺序查找作为子串的指定 NTCTS 。
\return 在查找结束时经过的字符数。
*/
template<typename _tChar>
YB_NONNULL(1) YB_PURE yconstfn size_t
const_ntctsfind(const _tChar* s1, const _tChar* s2) ynothrowv
{
	return ystdex::is_null(*s1) ? 0 : (ystdex::const_ntctscmp(s1, s2,
		ystdex::const_ntctslen(s2)) == _tChar()? 0
		: ystdex::const_ntctsfind(s1 + 1, s2) + 1);
}
//@}
//@}


//! \brief 查找字符或子串。
//@{
template<typename _tChar, typename _tSize,
	class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_find(const _tChar* p, _tSize sz, _tChar c, _tSize pos) ynothrowv
{
	yconstraint(p || sz == 0);
	if(pos < sz)
		if(const _tChar* ret = _tTraits::find(p + pos, sz - pos, c))
			return _tSize(ret - p);
	return _vNPos;
}
template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_find(const _tChar* p, _tSize sz, const _tChar* s, _tSize pos, _tSize n)
	ynothrowv
{
	yconstraint((p || sz == 0) && (s || n == 0));
	if(n == 0)
		return pos <= sz ? pos : _vNPos;
	if(n <= sz)
		for(; pos <= sz - n; ++pos)
			if(_tTraits::eq(p[pos], s[0])
				&& _tTraits::compare(p + pos + 1, s + 1, n - 1) == 0)
				return pos;
	return _vNPos;
}

template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_rfind(const _tChar* p, _tSize sz, _tChar c, _tSize pos) ynothrowv
{
	yconstraint(p || sz == 0);
	if(0 < sz)
	{
		if(pos < --sz)
			sz = pos;
		for(++sz; sz-- > 0; )
			if(_tTraits::eq(*(p + sz), c))
				return sz;
	}
	return _vNPos;
}
template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_rfind(const _tChar* p, _tSize sz, const _tChar* s, _tSize pos, _tSize n)
	ynothrowv
{
	yconstraint((p || sz == 0) && (s || n == 0));
	if(n <= sz)
	{
		pos = std::min(_tSize(sz - n), pos);
		do
			if(_tTraits::compare(p + pos, s, n) == 0)
				return pos;
		while(pos-- > 0);
	}
	return _vNPos;
}

template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_find_first_of(const _tChar* p, _tSize sz, const _tChar* s, _tSize pos,
	_tSize n) ynothrowv
{
	yconstraint((p || sz == 0) && (s || n == 0));
	if(n != 0)
	{
		for(; pos < sz; ++pos)
			if(_tTraits::find(s, n, p[pos]))
				return pos;
	}
	return _vNPos;
}

template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_find_last_of(const _tChar* p, _tSize sz, const _tChar* s, _tSize pos,
	_tSize n) ynothrowv
{
	yconstraint((p || sz == 0) && (s || n == 0));
	if(sz != 0 && n != 0)
	{
		if(--sz > pos)
			sz = pos;
		do
			if(_tTraits::find(s, n, p[sz]))
				return sz;
		while(sz-- != 0);
	}
	return _vNPos;
}

template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_find_first_not_of(const _tChar* p, _tSize sz, _tChar c, _tSize pos)
	ynothrowv
{
	yconstraint(p || sz == 0);
	for(; pos < sz; ++pos)
		if(!_tTraits::eq(p[pos], c))
			return pos;
	return _vNPos;
}
template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_find_first_not_of(const _tChar* p, _tSize sz, const _tChar* s, _tSize pos,
	_tSize n) ynothrowv
{
	yconstraint((p || sz == 0) && (s || n == 0));
	for(; pos < sz; ++pos)
		if(!_tTraits::find(s, n, p[pos]))
			return pos;
	return _vNPos;
}

template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_find_last_not_of(const _tChar* p, _tSize sz, _tChar c, _tSize pos) ynothrowv
{
	yconstraint(p || sz == 0);
	if(sz != 0)
	{
		if(--sz > pos)
			sz = pos;
		do
			if(!_tTraits::eq(p[sz], c))
				return sz;
		while(sz-- != 0);
	}
	return _vNPos;
}
template<typename _tChar, typename _tSize,
class _tTraits = std::char_traits<_tChar>, _tSize _vNPos = _tSize(-1)>
yconstfn_relaxed _tSize
str_find_last_not_of(const _tChar* p, _tSize sz, const _tChar* s, _tSize pos,
	_tSize n) ynothrowv
{
	yconstraint((p || sz == 0) && (s || n == 0));
	if(sz != 0)
	{
		if(--sz > pos)
			sz = pos;
		do
			if(!_tTraits::find(s, n, p[sz]))
				return sz;
		while(sz -- != 0);
	}
	return _vNPos;
}
//@}
//@}

} // namespace ystdex;

#endif

