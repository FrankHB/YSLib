﻿/*
	© 2014-2015, 2020-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cwctype.h
\ingroup YStandardEx
\brief ISO C 宽字符分类操作扩展。
\version r111
\author FrankHB <frankhb1989@gmail.com>
\since build 513
\par 创建时间:
	2014-06-29 15:34:34 +0800
\par 修改时间:
	2021-12-12 18:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CWideCharacterType
*/


#ifndef YB_INC_ystdex_cwtype_h_
#define YB_INC_ystdex_cwtype_h_ 1

#include "../ydef.h"
#include <cwctype> // for <cwctype>, std::iswspace, std::towlower,
//	std::towupper;
#include <cwchar> // for std::wint_t;

namespace ystdex
{

//! \since build 245
using std::wint_t;

//! \since build 662
//@{
/*!
\brief 区域无关的 std::isprint 实现。
\note 当前使用兼容 Unicode 的简单实现（参考 MUSL libc ）。
\note 可作为替代 MSVCRT 的实现的变通（测试表明 iswprintf 和 isprint 有类似缺陷）。
\see https://connect.microsoft.com/VisualStudio/feedback/details/799287/isprint-incorrectly-classifies-t-as-printable-in-c-locale 。
*/
YB_API YB_STATELESS bool
iswprint(wchar_t) ynothrow;

/*!
\brief 区域无关的 std::iswspace 实现。
\note 当前使用 Unicode 5.2 实现（参考 newlib ）。
\note 可作为替代 Android 2.3.1 的实现的变通（测试表明个别字符 iswspace 返回 8 ）。
\since build 514
*/
YB_API YB_STATELESS bool
iswspace(wchar_t) ynothrow;

/*!
\brief 区域无关的 std::iswgraph 实现。
\see ISO C11 7.30.2.1.6 。
*/
YB_STATELESS inline bool
iswgraph(wchar_t wc) ynothrow
{
	return !iswspace(wc) && iswprint(wc);
}
//@}

/*!
\brief 转换大小写字符。
\note 和 ISO C 标准库对应接口不同而和 ISO C++ \<locale> 中的接口类似，
	参数和返回值是字符类型而不是对应的整数类型。
\since build 823
*/
//@{
template<typename _tChar>
YB_ATTR_nodiscard inline _tChar
towlower(_tChar c) ynothrow
{
	return c >= 0 ? _tChar(std::towlower(wint_t(c))) : c;
}

template<typename _tChar>
YB_ATTR_nodiscard inline _tChar
towupper(_tChar c) ynothrow
{
	return c >= 0 ? _tChar(std::towupper(wint_t(c))) : c;
}
//@}

} // namespace ystdex;

#endif

