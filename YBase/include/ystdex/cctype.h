/*
	© 2014-2016, 2018-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cctype.h
\ingroup YStandardEx
\brief ISO C 字符分类操作扩展。
\version r274
\author FrankHB <frankhb1989@gmail.com>
\since build 513
\par 创建时间:
	2014-06-29 13:42:39 +0800
\par 修改时间:
	2020-05-13 16:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CCharacterType
*/


#ifndef YB_INC_ystdex_cctype_h_
#define YB_INC_ystdex_cctype_h_ 1

#include "../ydef.h" // for yconstfn, CHAR_MIN, yimpl;
#include <cwctype> // for std::towlower, std::towupper;
#include <cctype> // for std::tolower, std::toupper;

namespace ystdex
{

/*!
\note 区域无关默认行为即使用 C 区域。
\note 保证此处引入声明的函数没有其它重载，以便直接用于 lambda 表达式等上下文。
\note 保证适当检查参数的值以避免引起 ISO C 未定义行为而要求 narrow constraint 。
\see ISO C11 7.4/1 。
\see ISO C11 7.30.1/5 。
\since build 738
*/
//@{
//! \brief 判断指定字符是否为 ISO/IEC 2022 定义的 C0 控制字符。
YB_ATTR_nodiscard yconstfn bool
iscntrl_C0(char c) ynothrow
{
#if CHAR_MIN < 0
	return c >= 0 && c < 0x20;
#else
	return c < 0x20;
#endif
}

//! \brief 判断指定字符是否为 ISO/IEC 2022 定义的 C1 控制字符。
YB_ATTR_nodiscard yconstfn bool
iscntrl_C1(char c) ynothrow
{
	return (unsigned(c) & 0xFFU) >= 0x80 && (unsigned(c) & 0xFFU) < 0xA0;
}

/*!
\brief 使用 US-ASCII 字符集的 std::isprint 。
\see ISO C11 7.4/3 脚注。
*/
YB_ATTR_nodiscard yconstfn bool
iscntrl_ASCII(char c) ynothrow
{
	return iscntrl_C0(c) || c == 0x7F;
}

//! \brief 使用 ISO/IEC 8859-11(Latin/Thai) 字符集的 std::iscntrl 。
YB_ATTR_nodiscard yconstfn bool
iscntrl_ISO8859_11(char c) ynothrow
{
	return iscntrl_C0(c)
		|| ((unsigned(c) & 0xFFU) >= 0x7F && (unsigned(c) & 0xFFU) <= 0xA0);
}

//! \brief 使用 ISO/IEC 8859-1 字符集的 std::iscntrl 。
YB_ATTR_nodiscard yconstfn bool
iscntrl_ISO8859_1(char c) ynothrow
{
	return iscntrl_ISO8859_11(c) || (unsigned(c) & 0xFFU) == 0xAD;
}


/*!
\brief 使用 US-ASCII 字符集的 std::isprint 。
\see ISO C11 7.4/3 脚注。
*/
YB_ATTR_nodiscard yconstfn bool
isprint_ASCII(char c) ynothrow
{
	return c >= 0x20 && c < 0x7F;
}

//! \brief 使用 ISO/IEC 8859-1 字符集的 std::isprint 。
YB_ATTR_nodiscard yconstfn bool
isprint_ISO8859_1(char c) ynothrow
{
	return isprint_ASCII(c) || unsigned(c) > 0xA0;
}

/*!
\brief 区域无关的 std::isprint 。
\note 当前使用基于 ISO/IEC 8859-1 的 Unicode 方案实现。
\note 可作为替代 MSVCRT 的实现的部分变通。
\sa https://connect.microsoft.com/VisualStudio/feedback/details/799287/isprint-incorrectly-classifies-t-as-printable-in-c-locale
*/
YB_ATTR_nodiscard yconstfn bool
isprint(char c) ynothrow
{
	return yimpl(isprint_ISO8859_1(c));
}

/*!
\brief 区域无关的 std::isdigit 。
\note ISO C 和 ISO C++ 保证基本字符集中的数字字符具有从小到大连续的整数值表示。
\note 可作为替代 MSVCRT 的实现的变通。
\sa http://stackoverflow.com/questions/2898228/can-isdigit-legitimately-be-locale-dependent-in-c 。
\see ISO C11 5.2.1/3 。
\see ISO C++11 2.3/3 。
*/
YB_ATTR_nodiscard yconstfn bool
isdigit(char c) ynothrow
{
	return (unsigned(c) - '0') < 10U;
}

/*!
\brief 区域无关的 std::isspace 。
\see ISO C11 7.4.1.10 。
\since build 823
*/
YB_ATTR_nodiscard yconstfn bool
isspace(char c) ynothrow
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}


/*!
\brief 转换大小写字符。
\note 和 ISO C 标准库对应接口不同而和 ISO C++ \<locale> 中的接口类似，
	参数和返回值是字符类型而不是对应的整数类型。
*/
//@{
YB_ATTR_nodiscard inline char
tolower(char c) ynothrow
{
#if CHAR_MIN < 0
	return c >= 0 ? char(std::tolower(c)) : c;
#else
	return char(std::tolower(c));
#endif
}

//! \since build 823
template<typename _tChar>
YB_ATTR_nodiscard inline _tChar
towlower(_tChar c) ynothrow
{
	return c >= 0 ? _tChar(std::towlower(wint_t(c))) : c;
}


YB_ATTR_nodiscard inline char
toupper(char c) ynothrow
{
#if CHAR_MIN < 0
	return c >= 0 ? char(std::toupper(c)) : c;
#else
	return char(std::toupper(c));
#endif
}

//! \since build 823
template<typename _tChar>
YB_ATTR_nodiscard inline _tChar
towupper(_tChar c) ynothrow
{
	return c >= 0 ? _tChar(std::towupper(wint_t(c))) : c;
}
//@}
//@}

} // namespace ystdex;

#endif

