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
\version r1237
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 15:35:19 +0800
\par 修改时间:
	2015-11-18 10:45 +0800
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
#	include <functional> // for std::bind, std::placeholders::_1;

using namespace YSLib;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

int
ConvertToErrno(ErrorCode err) ynothrow
{
	// NOTE: This mapping is from Windows Kits 10.0.10150.0,
	//	ucrt/misc/errno.cpp, except for fix of the bug error 124: it shall be
	//	%ERROR_INVALID_LEVEL but not %ERROR_INVALID_HANDLE. See https://connect.microsoft.com/VisualStudio/feedback/details/1641428.
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
	if(IsInClosedInterval<ErrorCode>(err, ERROR_WRITE_PROTECT,
		ERROR_SHARING_BUFFER_EXCEEDED))
		return EACCES;
	if(IsInClosedInterval<ErrorCode>(err, ERROR_INVALID_STARTING_CODESEG,
		ERROR_INFLOOP_IN_RELOC_CHAIN))
		return ENOEXEC;
	return EINVAL;
}


ImplDeDtor(Win32Exception)


void
GlobalDelete::operator()(pointer h) const ynothrow
{
	YCL_CallWin32F_Trace(GlobalFree, h);
}


GlobalLocked::GlobalLocked(::HGLOBAL h)
	: p_locked(YCL_CallWin32F(GlobalLock, h))
{}
GlobalLocked::~GlobalLocked()
{
	YCL_CallWin32F_Trace(GlobalUnlock, p_locked);
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
		ImplRet(Win32Exception::FormatMessage(ErrorCode(ev)))
};


//! \since build 643
//@{
YB_NONNULL(2) string
MBCSToMBCS(int l, const char* str, unsigned cp_src, unsigned cp_dst)
{
	if(cp_src != cp_dst)
	{
		const int
			w_len(::MultiByteToWideChar(cp_src, 0, Nonnull(str), l, {}, 0));

		if(w_len != 0)
		{
			wstring wstr(CheckPositiveScalar<size_t>(w_len), wchar_t());
			const auto w_str(&wstr[0]);

			::MultiByteToWideChar(cp_src, 0, str, l, w_str, w_len);
			if(l == -1 && !wstr.empty())
				wstr.pop_back();
			return WCSToMBCS({w_str, wstr.length()}, cp_dst);
		}
		return {};
	}
	return str;
}

YB_NONNULL(2) wstring
MBCSToWCS(int l, const char* str, unsigned cp)
{
	const int
		w_len(::MultiByteToWideChar(cp, 0, Nonnull(str), l, {}, 0));

	if(w_len != 0)
	{
		wstring res(CheckPositiveScalar<size_t>(w_len), wchar_t());

		::MultiByteToWideChar(cp, 0, str, l, &res[0], w_len);
		if(l == -1 && !res.empty())
			res.pop_back();
		return res;
	}
	return {};
}

YB_NONNULL(2) string
WCSToMBCS(int l, const wchar_t* str, unsigned cp)
{
	const int r_l(::WideCharToMultiByte(cp, 0, Nonnull(str), l, {}, 0, {}, {}));

	if(r_l != 0)
	{
		string res(CheckPositiveScalar<size_t>(r_l), char());

		::WideCharToMultiByte(cp, 0, str, l, &res[0], r_l, {}, {});
		if(l == -1 && !res.empty())
			res.pop_back();
		return res;
	}
	return {};
}
//@}


//! \since build 651
//@{
template<typename _func>
auto
FetchFileInfo(_func f, UniqueHandle::pointer h)
	-> decltype(f(std::declval<::BY_HANDLE_FILE_INFORMATION&>()))
{
	::BY_HANDLE_FILE_INFORMATION info;

	YCL_CallWin32F(GetFileInformationByHandle, h, &info);
	return f(info);
}

template<typename _func, typename... _tParams>
auto
MakeFileToDo(_func f, _tParams&&... args)
	-> decltype(f(UniqueHandle::pointer()))
{
	if(const auto h = MakeFile(yforward(args)...))
		return f(h.get());
	YCL_Raise_Win32Exception("CreateFileW");
}

//! \since build 651
PDefH(FileAttributesAndFlags, FollowToAttr, bool follow_reparse_point) ynothrow
	ImplRet(follow_reparse_point ? FileAttributesAndFlags::NormalWithDirectory
		: FileAttributesAndFlags::NormalAll)
//@}

} // unnamed namespace;

Win32Exception::Win32Exception(ErrorCode ec, string_view msg, RecordLevel lv)
	: Exception(int(ec), GetErrorCategory(), msg, lv)
{
	YAssert(ec != 0, "No error should be thrown.");
}
Win32Exception::Win32Exception(ErrorCode ec, string_view msg, const char* fn,
	RecordLevel lv)
	: Win32Exception(ec, msg.to_string() + " @ " + Nonnull(fn), lv)
{}

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

		// FIXME: For some platforms, no ::LocalFree available. See https://msdn.microsoft.com/zh-cn/library/windows/desktop/ms679351(v=vs.85).aspx.
		::LocalFree(buf);
		return res;
	}
	CatchExpr(..., YTraceDe(Warning, "FormatMessage failed."))
	return {};
}


ModuleProc*
LoadProc(::HMODULE h_module, const char* proc)
{
	return YCL_CallWin32F(GetProcAddress, h_module, proc);
}


void
FixConsoleHandler(int(WINAPI* handler)(unsigned long), bool add)
{
	YCL_CallWin32F(SetConsoleCtrlHandler, handler ? handler : ConsoleHandler,
		add);
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
MBCSToMBCS(const char* str, unsigned cp_src, unsigned cp_dst)
{
	return MBCSToMBCS(-1, str, cp_src, cp_dst);
}
string
MBCSToMBCS(string_view sv, unsigned cp_src, unsigned cp_dst)
{
	return sv.length() != 0 ? MBCSToMBCS(CheckNonnegativeScalar<int>(
		sv.length()), sv.data(), cp_src, cp_dst) : string();
}

wstring
MBCSToWCS(const char* str, unsigned cp)
{
	return MBCSToWCS(-1, str, cp);
}
wstring
MBCSToWCS(string_view sv, unsigned cp)
{
	return sv.length() != 0 ? MBCSToWCS(CheckNonnegativeScalar<int>(
		sv.length()), sv.data(), cp) : wstring();
}

string
WCSToMBCS(const wchar_t* str, unsigned cp)
{
	return WCSToMBCS(-1, str, cp);
}
string
WCSToMBCS(wstring_view sv, unsigned cp)
{
	return sv.length() != 0 ? WCSToMBCS(CheckNonnegativeScalar<int>(
		sv.length()), sv.data(), cp) : string();
}


DirectoryFindData::DirectoryFindData(const string& name)
	: DirectoryFindData(UTF8ToWCS(name))
{}
DirectoryFindData::DirectoryFindData(const wstring& name)
	: dir_name(name), find_data()
{
	if(ystdex::rtrim(dir_name, L"/\\").empty())
		dir_name = L'.';
	YAssert(dir_name.back() != '\\', "Invalid argument found.");

	using platform::FileOperationFailure;
	const auto attr(FileAttributes(::GetFileAttributesW(dir_name.c_str())));
	yconstexpr const auto& msg("Opening directory failed.");

	if(attr != FileAttributes::Invalid)
	{
		if(attr & FileAttributes::Directory)
			dir_name += L"\\*";
		else
			ystdex::throw_error<FileOperationFailure>(ENOTDIR, msg);
	}
	else
		ystdex::throw_error<FileOperationFailure>(GetErrnoFromWin32(), msg);
}

DirectoryFindData::~DirectoryFindData()
{
	if(h_node)
		Close();
}

void
DirectoryFindData::Close() ynothrow
{
	FilterExceptions(std::bind(YCL_WrapCallWin32(FindClose, h_node), yfsig), yfsig);
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
	return h_node && d_name[0] != wchar_t() ? &d_name : nullptr;
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


pair<VolumeID, FileID>
QueryFileNodeID(UniqueHandle::pointer h)
{
	return FetchFileInfo([](::BY_HANDLE_FILE_INFORMATION& info) ynothrow
		-> pair<VolumeID, FileID>{
		return {VolumeID(info.dwVolumeSerialNumber),
			FileID(info.nFileSizeHigh) << 32 | info.nFileSizeLow};
	}, h);
}
pair<VolumeID, FileID>
QueryFileNodeID(const wchar_t* path)
{
	return MakeFileToDo<pair<VolumeID, FileID>(UniqueHandle::pointer)>(
		QueryFileNodeID, path);
}

size_t
QueryFileLinks(UniqueHandle::pointer h)
{
	return FetchFileInfo([](::BY_HANDLE_FILE_INFORMATION& info){
	//	if(info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		//	ProcessReparsePoint(info);
		return size_t(info.nNumberOfLinks);
	}, h);
}
size_t
QueryFileLinks(const wchar_t* path)
{
	return MakeFileToDo<size_t(UniqueHandle::pointer)>(QueryFileLinks, path);
}

void
QueryFileTime(UniqueHandle::pointer h, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime)
{
	YCL_CallWin32F(GetFileTime, h, p_ctime, p_atime, p_mtime);
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
	MakeFileToDo(std::bind<void(UniqueHandle::pointer, ::FILETIME*, ::FILETIME*,
		::FILETIME*)>(QueryFileTime, std::placeholders::_1, p_ctime, p_atime,
		p_mtime), path, AccessRights::GenericRead,
		FollowToAttr(follow_reparse_point));
}

void
SetFileTime(UniqueHandle::pointer h, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime)
{
	using ::SetFileTime;

	YCL_CallWin32F(SetFileTime, h, p_ctime, p_atime, p_mtime);
}
void
SetFileTime(const char* path, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime, bool follow_reparse_point)
{
	SetFileTime(UTF8ToWCS(path).c_str(), p_ctime, p_atime, p_mtime,
		follow_reparse_point);
}
void
SetFileTime(const wchar_t* path, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime, bool follow_reparse_point)
{
	MakeFileToDo(std::bind<void(UniqueHandle::pointer, ::FILETIME*, ::FILETIME*,
		::FILETIME*)>(SetFileTime, std::placeholders::_1, p_ctime, p_atime,
		p_mtime), path, AccessRights::GenericRead,
		FollowToAttr(follow_reparse_point));
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
::FILETIME
ConvertTime(std::chrono::nanoseconds file_time)
{
	::FILETIME res;
	::LARGE_INTEGER date;

	date.QuadPart = file_time.count() / 100LL + 116444736000000000LL;
	yunseq(res.dwHighDateTime = static_cast<unsigned long>(date.HighPart),
		res.dwLowDateTime = date.LowPart);
	if(res.dwLowDateTime != 0 || res.dwHighDateTime != 0)
		return res;
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

