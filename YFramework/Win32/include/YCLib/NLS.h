/*
	© 2014-2016, 2018, 2020, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NLS.h
\ingroup YCLib
\ingroup Win32
\brief Win32 平台自然语言处理支持扩展接口。
\version r270
\author FrankHB <frankhb1989@gmail.com>
\since build 556
\par 创建时间:
	2014-11-25 17:30:48 +0800
\par 修改时间:
	2023-03-26 10:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::NLS
*/


#ifndef YCL_Win32_INC_NLS_h_
#define YCL_Win32_INC_NLS_h_ 1

#include "YCLib/YModules.h"
#include YFM_Win32_YCLib_MinGW32 // for string, CP_UTF8, CP_ACP, string_view,
//	wstring, wstring_view;

namespace platform_ex
{

inline namespace Windows
{

/*!	\defgroup native_encoding_conv Native Encoding Conversion
\brief 本机文本编码转换。
\exception YSLib::LoggedEvent 长度为负数或溢出 int 。

转换第一个 \c unsigned 参数指定编码的字符串为第二个 \c unsigned 参数指定的编码。
对 string_view 参数，当且仅当长度等于 0 时结果是空串；
否则忽略长度，参数的数据指针视为 NTBS 。
*/
//!@{
//! \pre 间接断言：字符串指针参数非空。
//!@{
//! \since build 970
YB_ATTR_nodiscard YF_API YB_NONNULL(1) std::string
MBCSToMBCS(const char*, unsigned = CP_UTF8, unsigned = CP_ACP);
//! \since build 905
YB_ATTR_nodiscard YF_API YB_NONNULL(2) string
MBCSToMBCS(const string::allocator_type&, const char*, unsigned = CP_UTF8,
	unsigned = CP_ACP);
//!@}
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
//!@{
//! \since build 970
YB_ATTR_nodiscard YF_API std::string
MBCSToMBCS(string_view, unsigned = CP_UTF8, unsigned = CP_ACP);
//! \since build 905
YB_ATTR_nodiscard YF_API string
MBCSToMBCS(const string::allocator_type&, string_view, unsigned = CP_UTF8,
	unsigned = CP_ACP);
//!@}

//! \pre 间接断言：字符串指针参数非空。
//!@{
//! \since build 970
YB_ATTR_nodiscard YF_API YB_NONNULL(1) std::wstring
MBCSToWCS(const char*, unsigned = CP_ACP);
//! \since build 905
YB_ATTR_nodiscard YF_API YB_NONNULL(2) wstring
MBCSToWCS(const wstring::allocator_type&, const char*, unsigned = CP_ACP);
//!@}
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
//!@{
//! \since build 970
YB_ATTR_nodiscard YF_API std::wstring
MBCSToWCS(string_view, unsigned = CP_ACP);
//! \since build 905
YB_ATTR_nodiscard YF_API wstring
MBCSToWCS(const wstring::allocator_type&, string_view, unsigned = CP_ACP);
//!@}

//! \pre 间接断言：字符串指针参数非空。
//!@{
//! \since build 970
YB_ATTR_nodiscard YF_API YB_NONNULL(1) std::string
WCSToMBCS(const wchar_t*, unsigned = CP_ACP);
//! \since build 905
YB_ATTR_nodiscard YF_API YB_NONNULL(2) string
WCSToMBCS(const string::allocator_type&, const wchar_t*, unsigned = CP_ACP);
//!@}
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
//!@{
//! \since build 970
YB_ATTR_nodiscard YF_API std::string
WCSToMBCS(wstring_view, unsigned = CP_ACP);
//! \since build 905
YB_ATTR_nodiscard YF_API string
WCSToMBCS(const string::allocator_type&, wstring_view, unsigned = CP_ACP);
//!@}

//! \since build 970
//!@{
//! \pre 间接断言：字符串指针参数非空。
//!@{
YB_NONNULL(1) inline PDefH(std::wstring, UTF8ToWCS, const char* str)
	ImplRet(MBCSToWCS(str, CP_UTF8))
YB_NONNULL(1) inline PDefH(wstring, UTF8ToWCS, const char* str,
	const wstring::allocator_type& a)
	ImplRet(MBCSToWCS(a, str, CP_UTF8))
//!@}
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
//!@{
inline PDefH(std::wstring, UTF8ToWCS, string_view sv)
	ImplRet(MBCSToWCS(sv, CP_UTF8))
inline PDefH(wstring, UTF8ToWCS, string_view sv,
	const wstring::allocator_type& a)
	ImplRet(MBCSToWCS(a, sv, CP_UTF8))
//!@}

//! \pre 间接断言：字符串指针参数非空。
//!@{
YB_NONNULL(1) inline PDefH(std::string, WCSToUTF8, const wchar_t* str)
	ImplRet(WCSToMBCS(str, CP_UTF8))
YB_NONNULL(1) inline PDefH(string, WCSToUTF8, const wchar_t* str,
	const string::allocator_type& a)
	ImplRet(WCSToMBCS(a, str, CP_UTF8))
//!@}
//! \pre 长度参数非零且不上溢 \c int 时间接断言：字符串指针参数非空。
//!@{
inline PDefH(std::string, WCSToUTF8, wstring_view sv)
	ImplRet(WCSToMBCS(sv, CP_UTF8))
inline
	PDefH(string, WCSToUTF8, wstring_view sv, const string::allocator_type& a)
	ImplRet(WCSToMBCS(a, sv, CP_UTF8))
//!@}
//!@}
//!@}


/*!
\brief 取注册表中 NLS 键中指定名称的值。
\pre 间接断言：字符串参数非空。
\since build 593
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) wstring
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
YB_ATTR_nodiscard YF_API YB_PURE const unsigned short*
FetchDBCSOffset(int) ynothrow;

} // inline namespace Windows;

} // namespace platform_ex;

#endif

