/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cctype.h
\ingroup YStandardEx
\brief ISO C 字符分类操作扩展。
\version r191
\author FrankHB <frankhb1989@gmail.com>
\since build 513
\par 创建时间:
	2014-06-29 13:42:39 +0800
\par 修改时间:
	2016-11-09 15:53 +0800
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

//! \since build 738
//@{
//! \brief 判断指定字符是否为 ISO/IEC 2022 定义的 C0 控制字符。
yconstfn bool
iscntrl_C0(char c) ynothrow
{
#if CHAR_MIN < 0
	return c >= 0 && c < 0x20;
#else
	return c < 0x20;
#endif
}

//! \brief 判断指定字符是否为 ISO/IEC 2022 定义的 C1 控制字符。
yconstfn bool
iscntrl_C1(char c) ynothrow
{
	return (unsigned(c) & 0xFFU) >= 0x80 && (unsigned(c) & 0xFFU) < 0xA0;
}

/*!
\brief 使用 US-ASCII 字符集的 std::isprint 实现。
\see ISO C11 7.4/3 脚注。
*/
yconstfn bool
iscntrl_ASCII(char c) ynothrow
{
	return iscntrl_C0(c) || c == 0x7F;
}

//! \brief 使用 ISO/IEC 8859-11(Latin/Thai) 字符集的 std::iscntrl 实现。
yconstfn bool
iscntrl_ISO8859_11(char c) ynothrow
{
	return iscntrl_C0(c)
		|| ((unsigned(c) & 0xFFU) >= 0x7F && (unsigned(c) & 0xFFU) <= 0xA0);
}

//! \brief 使用 ISO/IEC 8859-1 字符集的 std::iscntrl 实现。
yconstfn bool
iscntrl_ISO8859_1(char c) ynothrow
{
	return iscntrl_ISO8859_11(c) || (unsigned(c) & 0xFFU) == 0xAD;
}


/*!
\brief 使用 US-ASCII 字符集的 std::isprint 实现。
\see ISO C11 7.4/3 脚注。
*/
yconstfn bool
isprint_ASCII(char c) ynothrow
{
	return c >= 0x20 && c < 0x7F;
}

//! \brief 使用 ISO/IEC 8859-1 字符集的 std::isprint 实现。
yconstfn bool
isprint_ISO8859_1(char c) ynothrow
{
	return isprint_ASCII(c) || unsigned(c) > 0xA0;
}

/*!
\brief 区域无关的 std::isprint 实现。
\note 当前使用基于 ISO/IEC 8859-1 的 Unicode 方案实现。
\note 可作为替代 MSVCRT 的实现的部分变通。
\sa https://connect.microsoft.com/VisualStudio/feedback/details/799287/isprint-incorrectly-classifies-t-as-printable-in-c-locale
*/
yconstfn bool
isprint(char c) ynothrow
{
	return yimpl(isprint_ISO8859_1(c));
}

/*!
\brief 区域无关的 std::isdigit 实现。
\note ISO C 和 ISO C++ 保证基本字符集中的数字字符具有从小到大连续的整数值表示。
\note 可作为替代 MSVCRT 的实现的变通。
\sa http://stackoverflow.com/questions/2898228/can-isdigit-legitimately-be-locale-dependent-in-c 。
\see ISO C11 5.2.1/3 。
\see ISO C++11 2.3/3 。
*/
yconstfn bool
isdigit(char c) ynothrow
{
	return (unsigned(c) - '0') < 10U;
}


/*!
\brief 转换大小写字符。
\note 和 ISO C 标准库对应接口不同而和 ISO C++ \<locale\> 中的接口类似，
	参数和返回值是字符类型而不是对应的整数类型。
*/
//@{
template<typename _tChar>
yconstfn _tChar
tolower(_tChar c) ynothrow
{
	return _tChar(std::towupper(wint_t(c)));
}
yconstfn char
tolower(char c) ynothrow
{
	return char(std::tolower(c));
}


template<typename _tChar>
yconstfn _tChar
toupper(_tChar c) ynothrow
{
	return _tChar(std::towlower(wint_t(c)));
}
yconstfn char
toupper(char c) ynothrow
{
	return char(std::toupper(c));
}
//@}
//@}

} // namespace ystdex;

#endif

