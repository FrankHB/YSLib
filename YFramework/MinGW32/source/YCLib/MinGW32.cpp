/*
	© 2013-2015 FrankHB.

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
\version r578
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 15:35:19 +0800
\par 修改时间:
	2015-01-10 15:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::MinGW32
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_MinGW32_YCLib_MinGW32
#	include YFM_YCLib_FileSystem // for platform::FileOperationFaiure;

using namespace YSLib;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

//! \since build 545
namespace
{

class Win32ErrorCategory : public std::error_category
{
public:
	PDefH(const char*, name, ) const ynothrow override
		ImplRet("Win32Error")
	//! \since build 564
	PDefH(std::string, message, int ev) const override
		// NOTE: For Win32 a %::DWORD can be mapped one-to-one for 32-bit %int.
		ImplRet(Win32Exception::FormatMessage(Win32Exception::ErrorCode(ev)))
};

} // unnamed namespace;

Win32Exception::Win32Exception(ErrorCode ec, const std::string& s, LevelType lv)
	: Exception(ec, GetErrorCategory(), s, lv)
{
	YAssert(ec != 0, "No error should be thrown.");
}

const std::error_category&
Win32Exception::GetErrorCategory()
{
	static const Win32ErrorCategory ecat{};

	return ecat;
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
	CatchIgnore(...)
	return {};
}


bool
CheckWine()
{
	try
	{
		RegistryKey k1(HKEY_CURRENT_USER, L"Software\\Wine");
		RegistryKey k2(HKEY_LOCAL_MACHINE, L"Software\\Wine");

		yunused(k1),
		yunused(k2);
		return true;
	}
	CatchIgnore(Win32Exception&)
	return {};
}


std::string
MBCSToMBCS(const char* str, std::size_t len, int cp_src, int cp_dst)
{
	YAssertNonnull(str);
	if(cp_src == cp_dst)
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
	const int
		r_len(::WideCharToMultiByte(cp, 0, Nonnull(str), len, {}, 0, {}, {}));
	std::string mbcs(r_len, char());

	::WideCharToMultiByte(cp, 0, str, len, &mbcs[0], r_len, {}, {});
	return mbcs;
}

std::wstring
MBCSToWCS(const char* str, std::size_t len, int cp)
{
	const auto w_len(::MultiByteToWideChar(cp, 0, Nonnull(str), len, {}, 0));
	std::wstring res(w_len, wchar_t());
	const auto w_str = &res[0];

	::MultiByteToWideChar(cp, 0, str, len, w_str, w_len);
	return res;
}


DirectoryFindData::DirectoryFindData(std::string name)
	: DirectoryFindData(UTF8ToWCS(name))
{}
DirectoryFindData::DirectoryFindData(std::wstring name)
	: dir_name(ystdex::rtrim(name, L"/\\")), find_data()
{
	YAssert(!dir_name.empty() && dir_name.back() != '\\',
		"Invalid argument found.");

	using platform::FileOperationFailure;
	const auto r(::GetFileAttributesW(dir_name.c_str()));
	yconstexpr const char* msg("Opening directory failed.");

	if(YB_UNLIKELY(r == INVALID_FILE_ATTRIBUTES))
		// TODO: Call %::GetLastError to distinguish concreate errors.
		throw FileOperationFailure(EINVAL, std::generic_category(), msg);
	if(r & FILE_ATTRIBUTE_DIRECTORY)
		dir_name += L"\\*";
	else
		throw FileOperationFailure(ENOTDIR, std::generic_category(), msg);
}

DirectoryFindData::~DirectoryFindData()
{
	if(h_node)
		Close();
}

void
DirectoryFindData::Close() ynothrow
{
	const auto res(::FindClose(h_node));

	YAssert(res, "No valid directory found.");
	yunused(res);
}

std::wstring*
DirectoryFindData::Read()
{
	if(!h_node)
	{
		// NOTE: See MSDN "FindFirstFile function" for details.
		YAssert(!dir_name.empty(), "Invalid directory name found.");
		YAssert(dir_name.back() != L'\\', "Invalid directory name found.");
		if((h_node = ::FindFirstFileW(dir_name.c_str(), &find_data))
			== INVALID_HANDLE_VALUE)
			h_node = {};
	}
	else if(!::FindNextFileW(h_node, &find_data))
	{
		Close();
		h_node = {};
	}
	if(h_node && h_node != INVALID_HANDLE_VALUE)
		d_name = find_data.cFileName;
	return h_node ? &d_name : nullptr;
}

void
DirectoryFindData::Rewind() ynothrow
{
	if(h_node)
	{
		Close();
		h_node = {};
	}
}


void
RegistryKey::Flush()
{
	YF_Raise_Win32Exception_On_Failure(::RegFlushKey(h_key), "RegFlushKey");
}

std::pair<unsigned long, std::vector<ystdex::byte>>
RegistryKey::GetRawValue(const wchar_t* name, unsigned long type) const
{
	unsigned long size;

	YF_Raise_Win32Exception_On_Failure(::RegQueryValueExW(h_key, Nonnull(name),
		{}, type == REG_NONE ? &type : nullptr, {}, &size), "RegQueryValueExW");

	std::vector<ystdex::byte> res(size);

	YF_Raise_Win32Exception_On_Failure(::RegQueryValueExW(h_key, name,
		{}, &type, &res[0], &size), "RegQueryValueExW");
	return {type, std::move(res)};
}
std::size_t
RegistryKey::GetSubKeyCount() const
{
	unsigned long res;

	YF_Raise_Win32Exception_On_Failure(::RegQueryInfoKey(h_key, {}, {}, {},
		&res, {}, {}, {}, {}, {}, {}, {}), "RegQueryInfoKey");
	return size_t(res);
}
std::vector<std::wstring>
RegistryKey::GetSubKeyNames() const
{
	const auto cnt(GetSubKeyCount());
	std::vector<std::wstring> res;

	if(cnt > 0)
	{
		// NOTE: See http://msdn.microsoft.com/en-us/library/windows/desktop/ms724872(v=vs.85).aspx .
		wchar_t name[256];

		for(res.reserve(cnt); res.size() < cnt; res.emplace_back(name))
			YF_Raise_Win32Exception_On_Failure(::RegEnumKeyExW(h_key,
				res.size(), name, {}, {}, {}, {}, {}), "RegEnumKeyExW");
	}
	return res;
}
std::size_t
RegistryKey::GetValueCount() const
{
	unsigned long res;

	YF_Raise_Win32Exception_On_Failure(::RegQueryInfoKey(h_key, {}, {}, {}, {},
		{}, {}, &res, {}, {}, {}, {}), "RegQueryInfoKey");
	return size_t(res);
}
std::vector<std::wstring>
RegistryKey::GetValueNames() const
{
	const auto cnt(GetValueCount());
	std::vector<std::wstring> res;

	if(cnt > 0)
	{
		// NOTE: See http://msdn.microsoft.com/en-us/library/windows/desktop/ms724872(v=vs.85).aspx .
		wchar_t name[16384];

		for(res.reserve(cnt); res.size() < cnt; res.emplace_back(name))
			YF_Raise_Win32Exception_On_Failure(::RegEnumValueW(h_key,
				res.size(), name, {}, {}, {}, {}, {}), "RegEnumValueW");
	}
	return res;
}

std::wstring
FetchRegistryString(const RegistryKey& key, const wchar_t* name)
{
	try
	{
		const auto pr(key.GetRawValue(name, REG_SZ));

		if(pr.first == REG_SZ && !pr.second.empty())
			// TODO: Improve performance?
			return ystdex::rtrim(std::wstring(reinterpret_cast<const wchar_t*>(
				&pr.second[0]), pr.second.size() / 2), L'\0');
	}
	catch(Win32Exception&)
	{}
	return {};
}


std::wstring
FetchSystemPath(size_t s)
{
	const auto res(make_unique<wchar_t[]>(s));

	::GetSystemDirectoryW(&res[0], s);
	return ystdex::rtrim(std::wstring(&res[0]), L'\\') + L'\\';
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

#endif

} // namespace YSLib;

