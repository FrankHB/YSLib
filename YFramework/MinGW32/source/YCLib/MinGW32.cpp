/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MinGW32.cpp
\ingroup YCLib
\ingroup MinGW32
\brief YCLib MinGW32 平台公共扩展。
\version r174
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 15:35:19 +0800
\par 修改时间:
	2014-10-11 15:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::MinGW32
*/


#include "YCLib/YModules.h"
#include YFM_MinGW32_YCLib_MinGW32

using namespace YSLib;

namespace platform_ex
{

inline namespace Windows
{

Win32Exception::Win32Exception(ErrorCode ec, const std::string& s, LevelType l)
	ynothrow
	: Exception([&]()->std::string{
		try
		{
			return s + ": " + FormatMessage(ec);
		}
		catch(...)
		{}
		return s;
	}(), l),
	err(ec)
{
	YAssert(ec != 0, "No error should be thrown.");
}

std::string
Win32Exception::FormatMessage(ErrorCode ec) ynothrow
{
	try
	{
		wchar_t* buf{};

		::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, {},
			ec, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			reinterpret_cast<wchar_t*>(&buf), 1, {});

		auto res(WCSToMBCS(buf, CP_UTF8));

		::LocalFree(buf);
		return res;
	}
	catch(...)
	{}
	return {};
}


bool
CheckWine()
{
	try
	{
		RegisterKey k1(HKEY_CURRENT_USER, L"Software\\Wine");
		RegisterKey k2(HKEY_LOCAL_MACHINE, L"Software\\Wine");
		
		yunused(k1),
		yunused(k2);
		return true;
	}
	catch(Win32Exception&)
	{}
	return {};
}


std::string
MBCSToMBCS(const char* str, std::size_t len, int cp_src, int cp_dst)
{
	if (cp_src == cp_dst)
		return str;

	const int w_len(::MultiByteToWideChar(cp_src, 0, str, len, {}, 0));
	std::wstring wstr(w_len, wchar_t());
	wchar_t* w_str = &wstr[0];

	::MultiByteToWideChar(cp_src, 0, str, len, w_str, w_len);

	return WCSToMBCS(w_str, w_len, cp_dst);
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

	return res;
}


std::pair<UniqueHandle, UniqueHandle>
MakePipe()
{
	::HANDLE h_raw_read, h_raw_write;

	if(!::CreatePipe(&h_raw_read, &h_raw_write, {}, 0))
		YF_Raise_Win32Exception("CreatePipe");

	UniqueHandle h_read(h_raw_read), h_write(h_raw_write);

	if(!::SetHandleInformation(h_write.get(), HANDLE_FLAG_INHERIT,
		HANDLE_FLAG_INHERIT))
		YF_Raise_Win32Exception("SetHandleInformation");
	return {std::move(h_read), std::move(h_write)};
}


std::chrono::nanoseconds
ConvertTime(::FILETIME& file_time)
{
	if(file_time.dwLowDateTime != 0 || file_time.dwHighDateTime != 0)
	{
		// FIXME: Local time conversion for FAT volumes.
		// NOTE: See $2014-10 @ %Documentation::Workflow::Annual2014.
		::LARGE_INTEGER date;

		// NOTE: The epoch is Jan. 1, 1601: 134774 days to Jan. 1, 1970, i.e.
		//	11644473600 seconds, or 116444736000000000 * 100 nanoseconds.
		// TODO: Strip away the magic number;
		yunseq(date.HighPart = file_time.dwHighDateTime,
			date.LowPart = file_time.dwLowDateTime);
		return std::chrono::nanoseconds((date.QuadPart - 116444736000000000ULL)
			* 100U);
	}
	else
		throw std::system_error(ENOSYS, std::generic_category());
}

} // inline namespace Windows;

} // namespace YSLib;

