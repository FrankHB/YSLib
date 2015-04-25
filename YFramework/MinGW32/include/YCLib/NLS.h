/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NLS.h
\ingroup YCLib
\ingroup MinGW32
\brief Win32 平台自然语言处理支持扩展接口。
\version r76
\author FrankHB <frankhb1989@gmail.com>
\since build 556
\par 创建时间:
	2014-11-25 17:30:48 +0800
\par 修改时间:
	2015-04-24 04:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::NLS
*/


#ifndef YCL_MinGW32_INC_NLS_h_
#define YCL_MinGW32_INC_NLS_h_ 1

#include "YCLib/YModules.h"
#include YFM_MinGW32_YCLib_MinGW32

namespace platform_ex
{

inline namespace Windows
{

/*!
\brief 取注册表中 NLS 键中指定名称的值。
\pre 间接断言：字符串参数非空。
\since build 593
*/
YF_API YB_NONNULL(1) wstring
FetchNLSItemFromRegistry(const wchar_t*);

/*!
\brief 取注册表中指定代码页的 NLS 文件路径。
\since build 593
*/
inline PDefH(wstring, FetchCPFileNameFromRegistry, int cp)
	ImplRet(FetchNLSItemFromRegistry(to_wstring(cp).c_str()))

/*!
\brief 取指定双字节字符集代码页的映射表。
\return 成功则为映射表的起始指针，否则为空指针。
\note 非双字节字符集视为失败。
\since build 552
*/
YF_API const unsigned short*
FetchDBCSOffset(int) ynothrow;

} // inline namespace Windows;

} // namespace platform_ex;

#endif

