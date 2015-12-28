/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cwctype.h
\ingroup YStandardEx
\brief ISO C 宽字符分类操作扩展。
\version r78
\author FrankHB <frankhb1989@gmail.com>
\since build 513
\par 创建时间:
	2014-06-29 15:34:34 +0800
\par 修改时间:
	2015-12-27 21:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CWideCharacterType
*/


#ifndef YB_INC_ystdex_cwtype_h_
#define YB_INC_ystdex_cwtype_h_ 1

#include "cctype.h"
#include <cwctype> // for std::iswspace;

namespace ystdex
{

//! \since build 662
//@{
/*!
\brief 区域无关的 std::isprint 实现。
\note 当前使用兼容 Unicode 的简单实现（参考 MUSL libc ）。
\note 可作为替代 MSVCRT 的实现的变通（测试表明 iswprintf 和 isprint 有类似缺陷）。
\sa https://connect.microsoft.com/VisualStudio/feedback/details/799287/isprint-incorrectly-classifies-t-as-printable-in-c-locale
*/
YB_API bool
iswprint(wchar_t) ynothrow;

/*!
\brief 区域无关的 std::iswspace 实现。
\note 当前使用 Unicode 5.2 实现（参考 newlib ）。
\note 可作为替代 Android 2.3.1 的实现的变通（测试表明 iswspace 对个别字符可能返回 8 ）。
\since build 514
*/
YB_API bool
iswspace(wchar_t) ynothrow;

/*!
\brief 区域无关的 std::iswgraph 实现。
\see ISO C11 7.30.2.1.6 。
*/
inline bool
iswgraph(wchar_t wc) ynothrow
{
	return !iswspace(wc) && iswprint(wc);
}
//@}

} // namespace ystdex;

#endif

