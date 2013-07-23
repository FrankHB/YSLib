/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MinGW32.cpp
\ingroup YCLib
\ingroup MinGW32
\brief YCLib MinGW32 平台扩展公共头文件。
\version r82
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 15:35:19 +0800
\par 修改时间:
	2013-07-22 16:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::MinGW32
*/


#include "YCLib/MinGW32.h"

using namespace YSLib;

namespace platform_ex
{

Exception::Exception(const std::string& s, LevelType l) ynothrow
	: LoggedEvent(s, l)
{}

namespace Windows
{

Win32Exception::Win32Exception(::DWORD e, const std::string& s, LevelType l)
	ynothrow
	: Exception(s, l),
	err(e)
{
	YAssert(e != 0, "No error should be thrown.");
}


std::string
UTF8ToMBCS(const char* str, std::size_t len, int cp)
{
	if (cp == CP_UTF8)
		return str;

	const int w_len(::MultiByteToWideChar(CP_UTF8, 0, str, len, {}, 0));
	std::wstring wstr(w_len, wchar_t());
	wchar_t* w_str = &wstr[0];

	::MultiByteToWideChar(CP_UTF8, 0, str, len, w_str, w_len);

	return WCSToMBCS(w_str, w_len, cp);
}

std::string
WCSToMBCS(const wchar_t* str, std::size_t len, int cp)
{
	const int r_len(::WideCharToMultiByte(cp, 0, str, len,
		nullptr, 0, nullptr, nullptr));
	std::string mbcs(r_len, char());

	::WideCharToMultiByte(cp, 0, str, len, &mbcs[0], r_len, {}, {});
	return mbcs;
}

std::wstring
MBCSToWCS(const char* str, std::size_t len, int cp)
{
	const auto w_len(::MultiByteToWideChar(cp, 0, str, len, {}, 0));
	std::wstring res(w_len, wchar_t());
	const auto w_str = &res[0];

	::MultiByteToWideChar(cp, 0, str, len, w_str, w_len);

	return std::move(res);
}

} // namespace Windows;

} // namespace YSLib;

