/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cctype.h
\ingroup YStandardEx
\brief ISO C 字符分类操作扩展。
\version r128
\author FrankHB <frankhb1989@gmail.com>
\since build 513
\par 创建时间:
	2014-06-29 13:42:39 +0800
\par 修改时间:
	2014-07-01 02:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CCharacterType
*/


#ifndef YB_INC_ystdex_cctype_h_
#define YB_INC_ystdex_cctype_h_ 1

#include "../ydef.h" // for yconstfn, CHAR_MIN, yimpl;

namespace ystdex
{

//! \since build 513
//@{
//! \brief 判断指定字符是否为 ISO/IEC 2022 定义的 C0 控制字符。
yconstfn bool
iscntrl_C0(char c)
{
#if CHAR_MIN < 0
	return c >= 0 && c < 0x20;
#else
	return c < 0x20;
#endif
}

//! \brief 判断指定字符是否为 ISO/IEC 2022 定义的 C1 控制字符。
yconstfn bool
iscntrl_C1(char c)
{
	return (c & 0xFFU) >= 0x80 && (c & 0xFFU) < 0xA0;
}

/*!
\brief 使用 US-ASCII 字符集的 std::isprint 实现。
\see ISO C11 7.4/3 脚注。
*/
yconstfn bool
iscntrl_ASCII(char c)
{
	return iscntrl_C0(c) || c == 0x7F;
}

//! \brief 使用 ISO/IEC 8859-11(Latin/Thai) 字符集的 std::iscntrl 实现。
yconstfn bool
iscntrl_ISO8859_11(char c)
{
	return iscntrl_C0(c) || ((c & 0xFFU) >= 0x7F && (c & 0xFFU) <= 0xA0);
}

//! \brief 使用 ISO/IEC 8859-1 字符集的 std::iscntrl 实现。
yconstfn bool
iscntrl_ISO8859_1(char c)
{
	return iscntrl_ISO8859_11(c) || (c & 0xFFU) == 0xAD;
}


/*!
\brief 使用 US-ASCII 字符集的 std::isprint 实现。
\see ISO C11 7.4/3 脚注。
*/
yconstfn bool
isprint_ASCII(char c)
{
	return c >= 0x20 && c < 0x7F;
}

//! \brief 使用 ISO/IEC 8859-1 字符集的 std::isprint 实现。
yconstfn bool
isprint_ISO8859_1(char c)
{
	return isprint_ASCII(c) || unsigned(c) > 0xA0;
}

/*!
\brief 区域无关的 std::isprint 实现。
\note 当前使用基于 ISO/IEC 8859-1 的 Unicode 方案实现。
\note 可作为替代 MSVCRT 的实现的变通。
\sa https://connect.microsoft.com/VisualStudio/feedback/details/799287/isprint-incorrectly-classifies-t-as-printable-in-c-locale
*/
yconstfn bool
isprint(char c)
{
	return yimpl(isprint_ISO8859_1(c));
}
//@}

/*!
\brief 区域无关的 std::isdigit 实现。
\note ISO C 和 ISO C++ 保证基本字符集中的数字字符具有从小到大连续的整数值表示。
\see ISO C11 5.2.1/3 。
\see ISO C++11 2.3/3 。
\since build 514
*/
yconstfn bool
isdigit(char c)
{
	return (unsigned(c) - '0') < 10U;
}

} // namespace ystdex;

#endif

