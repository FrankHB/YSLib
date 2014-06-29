/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cwtype.h
\ingroup YStandardEx
\brief ISO C 宽字符分类操作扩展。
\version r61
\author FrankHB <frankhb1989@gmail.com>
\since build 513
\par 创建时间:
	2014-06-29 15:34:34 +0800
\par 修改时间:
	2014-06-29 19:07 +0800
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

//! \since build 513
//@{
/*!
\brief 区域无关的 std::isprint 实现。
\note 当前使用基本字符集内兼容 Unicode 的简单实现（参考 MUSL libc ）。
\note 可作为替代 MSVCRT 的实现的变通（测试表明 iswprintf 和 isprint 有类似缺陷）。
\sa https://connect.microsoft.com/VisualStudio/feedback/details/799287/isprint-incorrectly-classifies-t-as-printable-in-c-locale
*/
YB_API bool
iswprint(wchar_t);

/*!
\breif 区域无关的 std::iswgraph 实现。
\see ISO C11 7.30.2.1.6 。
*/
inline bool
iswgraph(wchar_t wc)
{
	return !std::iswspace(wc) && iswprint(wc);
}
//@}

} // namespace ystdex;

#endif

