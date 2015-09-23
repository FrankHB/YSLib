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
\ingroup Win32
\brief YCLib MinGW32 平台公共扩展。
\version r1003
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 15:35:19 +0800
\par 修改时间:
	2015-09-24 23:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::MinGW32
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_Win32_YCLib_Registry // for RegistryKey;
#	include <cerrno>
#	include YFM_YSLib_Core_YCoreUtilities // for YSLib::IsInClosedInterval,
//	YSLib::CheckPositiveScalar;
#	include YFM_YCLib_FileIO // for platform::FileOperationFailure;

using namespace YSLib;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

int
ConvertToErrno(unsigned long err) ynothrow
{
	// NOTE: This mapping is from Windows Kits 10.0.10150.0,
	//	ucrt/misc/errno.cpp, except for fix of the bug error 124: it shall be
	//	%ERROR_INVALID_LEVEL but not %ERROR_INVALID_HANDLE. See https://connect.microsoft.com/VisualStudio/feedback/details/1641428 .
	switch(err)
	{
	case ERROR_INVALID_FUNCTION:
		return EINVAL;
	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
		return ENOENT;
	case ERROR_TOO_MANY_OPEN_FILES:
		return EMFILE;
	case ERROR_ACCESS_DENIED:
		return EACCES;
	case ERROR_INVALID_HANDLE:
		return EBADF;
	case ERROR_ARENA_TRASHED:
	case ERROR_NOT_ENOUGH_MEMORY:
	case ERROR_INVALID_BLOCK:
		return ENOMEM;
	case ERROR_BAD_ENVIRONMENT:
		return E2BIG;
	case ERROR_BAD_FORMAT:
		return ENOEXEC;
	case ERROR_INVALID_ACCESS:
	case ERROR_INVALID_DATA:
		return EINVAL;
	case ERROR_INVALID_DRIVE:
		return ENOENT;
	case ERROR_CURRENT_DIRECTORY:
		return EACCES;
	case ERROR_NOT_SAME_DEVICE:
		return EXDEV;
	case ERROR_NO_MORE_FILES:
		return ENOENT;
	case ERROR_LOCK_VIOLATION:
		return EACCES;
	case ERROR_BAD_NETPATH:
		return ENOENT;
	case ERROR_NETWORK_ACCESS_DENIED:
		return EACCES;
	case ERROR_BAD_NET_NAME:
		return ENOENT;
	case ERROR_FILE_EXISTS:
		return EEXIST;
	case ERROR_CANNOT_MAKE:
	case ERROR_FAIL_I24:
		return EACCES;
	case ERROR_INVALID_PARAMETER:
		return EINVAL;
	case ERROR_NO_PROC_SLOTS:
		return EAGAIN;
	case ERROR_DRIVE_LOCKED:
		return EACCES;
	case ERROR_BROKEN_PIPE:
		return EPIPE;
	case ERROR_DISK_FULL:
		return ENOSPC;
	case ERROR_INVALID_TARGET_HANDLE:
		return EBADF;
	case ERROR_INVALID_LEVEL:
		return EINVAL;
	case ERROR_WAIT_NO_CHILDREN:
	case ERROR_CHILD_NOT_COMPLETE:
		return ECHILD;
	case ERROR_DIRECT_ACCESS_HANDLE:
		return EBADF;
	case ERROR_NEGATIVE_SEEK:
		return EINVAL;
	case ERROR_SEEK_ON_DEVICE:
		return EACCES;
	case ERROR_DIR_NOT_EMPTY:
		return ENOTEMPTY;
	case ERROR_NOT_LOCKED:
		return EACCES;
	case ERROR_BAD_PATHNAME:
		return ENOENT;
	case ERROR_MAX_THRDS_REACHED:
		return EAGAIN;
	case ERROR_LOCK_FAILED:
		return EACCES;
	case ERROR_ALREADY_EXISTS:
		return EEXIST;
	case ERROR_FILENAME_EXCED_RANGE:
		return ENOENT;
	case ERROR_NESTING_NOT_ALLOWED:
		return EAGAIN;
	case ERROR_NOT_ENOUGH_QUOTA:
		return ENOMEM;
	default:
		break;
	}
	if(IsInClosedInterval<unsigned long>(err, ERROR_WRITE_PROTECT,
		ERROR_SHARING_BUFFER_EXCEEDED))
		return EACCES;
	if(IsInClosedInterval<unsigned long>(err, ERROR_INVALID_STARTING_CODESEG,
		ERROR_INFLOOP_IN_RELOC_CHAIN))
		return ENOEXEC;
	return EINVAL;
}


ImplDeDtor(Win32Exception)


void
GlobalDelete::operator()(pointer h) const ynothrow
{
	YCL_CallWin32_Trace(GlobalFree, "GlobalDelete::operator()", h);
}


GlobalLocked::GlobalLocked(::HGLOBAL h)
	: p_locked(YCL_CallWin32(GlobalLock, "GlobalLocked::GlobalLocked", h))
{}
GlobalLocked::~GlobalLocked()
{
	YCL_CallWin32_Trace(GlobalUnlock, "GlobalLocked::~GlobalLocked", p_locked);
}


UniqueHandle
MakeFile(const wchar_t* path, FileAccessRights desired_access,
	FileShareMode shared_mode, CreationDisposition creation_disposition,
	FileAttributesAndFlags attributes_and_flags) ynothrowv
{
	using ystdex::underlying;
	const auto h(::CreateFileW(Nonnull(path), underlying(
		desired_access), underlying(shared_mode), {}, underlying(
		creation_disposition), underlying(attributes_and_flags), {}));

	return UniqueHandle(h != INVALID_HANDLE_VALUE ? h
		: UniqueHandle::pointer());
}


//! \since build 545
namespace
{

//! \since build 565
int WINAPI
ConsoleHandler(unsigned long ctrl)
{
	switch (ctrl)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		std::_Exit(int(STATUS_CONTROL_C_EXIT));
	}
	return 0;
}

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

Win32Exception::Win32Exception(ErrorCode ec, const std::string& s,
	RecordLevel lv)
	: Exception(int(ec), GetErrorCategory(), s, lv)
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

		auto res(WCSToMBCS(buf, unsigned(CP_UTF8)));

		// FIXME: For some platforms, no ::LocalFree available. See https://msdn.microsoft.com/zh-cn/library/windows/desktop/ms679351(v=vs.85).aspx .
		::LocalFree(buf);
		return res;
	}
	CatchExpr(..., YTraceDe(Warning, "FormatMessage failed."))
	return {};
}


void
FixConsoleHandler(int(WINAPI* handler)(unsigned long), bool add)
{
	YCL_CallWin32(SetConsoleCtrlHandler, "FixConsoleHandler", handler
		? handler : ConsoleHandler, add);
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


string
MBCSToMBCS(size_t len, const char* str, unsigned cp_src, unsigned cp_dst)
{
	if(len != 0)
	{
		if(cp_src != cp_dst)
		{
			const auto l(CheckPositiveScalar<int>(len));
			const int
				w_len(::MultiByteToWideChar(cp_src, 0, Nonnull(str), l, {}, 0));
			wstring wstr(CheckPositiveScalar<size_t>(w_len), wchar_t());
			const auto w_str(&wstr[0]);

			::MultiByteToWideChar(cp_src, 0, str, l, w_str, w_len);

			return WCSToMBCS(wstr.length(), w_str, cp_dst);
		}
		return str;
	}
	return {};
}

string
WCSToMBCS(size_t len, const wchar_t* str, unsigned cp)
{
	if(len != 0)
	{
		const auto l(CheckPositiveScalar<int>(len));
		const int
			r_l(::WideCharToMultiByte(cp, 0, Nonnull(str), l, {}, 0, {}, {}));
		string mbcs(CheckNonnegativeScalar<size_t>(r_l), char());

		::WideCharToMultiByte(cp, 0, str, l, &mbcs[0], r_l, {}, {});
		return mbcs;
	}
	return {};
}

wstring
MBCSToWCS(size_t len, const char* str, unsigned cp)
{
	if(len != 0)
	{
		const auto l(CheckPositiveScalar<int>(len));
		const int w_len(::MultiByteToWideChar(cp, 0, Nonnull(str), l, {}, 0));
		wstring res(CheckNonnegativeScalar<size_t>(w_len), wchar_t());

		::MultiByteToWideChar(cp, 0, str, l, &res[0], w_len);
		return res;
	}
	return {};
}


DirectoryFindData::DirectoryFindData(string name)
	: DirectoryFindData(UTF8ToWCS(name))
{}
DirectoryFindData::DirectoryFindData(wstring name)
	: dir_name(ystdex::rtrim(name, L"/\\")), find_data()
{
	if(dir_name.empty())
		dir_name = L'.';
	YAssert(dir_name.back() != '\\', "Invalid argument found.");

	using platform::FileOperationFailure;
	const auto r(::GetFileAttributesW(dir_name.c_str()));
	yconstexpr const char* const msg("Opening directory failed.");

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
	FilterExceptions(YCL_WrapCallWin32(FindClose, "DirectoryFindData::Close",
		h_node), "DirectoryFindData::Close");
}

wstring*
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


size_t
QueryFileLinks(UniqueHandle::pointer h)
{
	::BY_HANDLE_FILE_INFORMATION info;

	YCL_CallWin32(GetFileInformationByHandle, "QueryFileLinks", h, &info);
//	if(info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
//		ProcessReparsePoint(info);
	return size_t(info.nNumberOfLinks);
}
size_t
QueryFileLinks(const wchar_t* path)
{
	if(const auto h = MakeFile(path))
		return QueryFileLinks(h.get());
	YCL_Raise_Win32Exception("CreateFileW");
}

void
QueryFileTime(UniqueHandle::pointer h, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime)
{
	YCL_CallWin32(GetFileTime, "QueryFileTime", h, p_ctime, p_atime, p_mtime);
}
void
QueryFileTime(const char* path, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime, bool follow_reparse_point)
{
	QueryFileTime(UTF8ToWCS(path).c_str(), p_ctime, p_atime, p_mtime,
		follow_reparse_point);
}
void
QueryFileTime(const wchar_t* path, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime, bool follow_reparse_point)
{
	if(const auto h = MakeFile(path, AccessRights::GenericRead,
		follow_reparse_point ? FileAttributesAndFlags::NormalWithDirectory
		: FileAttributesAndFlags::NormalAll))
		QueryFileTime(h.get(), p_ctime, p_atime, p_mtime);
	else
		YCL_Raise_Win32Exception("CreateFileW");
}

std::chrono::nanoseconds
ConvertTime(const ::FILETIME& file_time)
{
	if(file_time.dwLowDateTime != 0 || file_time.dwHighDateTime != 0)
	{
		// FIXME: Local time conversion for FAT volumes.
		// NOTE: See $2014-10 @ %Documentation::Workflow::Annual2014.
		::LARGE_INTEGER date;

		// NOTE: The epoch is Jan. 1, 1601: 134774 days to Jan. 1, 1970, i.e.
		//	11644473600 seconds, or 116444736000000000 * 100 nanoseconds.
		// TODO: Strip away the magic number;
		yunseq(date.HighPart = long(file_time.dwHighDateTime),
			date.LowPart = file_time.dwLowDateTime);
		return std::chrono::nanoseconds((date.QuadPart - 116444736000000000LL)
			* 100U);
	}
	else
		throw std::system_error(ENOSYS, std::generic_category());
}

wstring
ExpandEnvironmentStrings(const wchar_t* p_src, size_t len)
{
	if(p_src && len != 0)
	{
		const size_t w_len(::ExpandEnvironmentStringsW(p_src, {}, 0));

		if(w_len != 0)
		{
			wstring wstr(w_len, wchar_t());

			if(::ExpandEnvironmentStringsW(p_src, &wstr[0],
				static_cast<unsigned long>(w_len)) != 0)
				return wstr;
		}
		YCL_Raise_Win32Exception("ExpandEnvironmentStringsW");
	}
	return {};
}

wstring
FetchSystemPath(size_t s)
{
	// XXX: Depends right behavior on external API.
	const auto res(make_unique_default_init<wchar_t[]>(s));

	::GetSystemDirectoryW(&res[0], unsigned(s));
	return ystdex::rtrim(wstring(&res[0]), L'\\') + L'\\';
}

} // inline namespace Windows;

#endif

} // namespace YSLib;

