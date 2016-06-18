/*
	© 2013-2016 FrankHB.

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
\version r1791
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 15:35:19 +0800
\par 修改时间:
	2016-06-17 18:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::MinGW32
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_Win32_YCLib_Registry // for platform::FileOperationFailure,
//	RegistryKey, ystdex::pun_storage_t, ystdex::throw_error,
//	std::errc::not_supported, std::invalid_argument;
#	include <cerrno> // for EINVAL, ENOENT, EMFILE, EACCESS, EBADF, ENOMEM,
//	ENOEXEC, EXDEV, EEXIST, EAGAIN, EPIPE, ENOSPC, ECHILD, ENOTEMPTY;
#	include YFM_YSLib_Core_YCoreUtilities // for YSLib::IsInClosedInterval,
//	YSLib::CheckPositiveScalar, YSLib::make_unique_default_init,
//	platform::EndsWithNonSeperator, YSLib::TryInvoke;
#	include <ystdex/container.hpp> // for ystdex::retry_for_vector;
#	include <ystdex/scope_guard.hpp> // for ystdex::unique_guard,
//	ystdex::dismiss, std::bind, std::placeholders::_1;

using namespace YSLib;
#endif

//! \since build 658
using platform::NodeCategory;

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


//! \since build 660
struct REPARSE_DATA_BUFFER
{
	struct tagSymbolicLinkReparseBuffer
	{
		unsigned short SubstituteNameOffset;
		unsigned short SubstituteNameLength;
		unsigned short PrintNameOffset;
		unsigned short PrintNameLength;
		unsigned long Flags;
		wchar_t PathBuffer[1];

		DefGetter(const ynothrow, wstring_view, PrintName,
			{PathBuffer + size_t(PrintNameOffset) / sizeof(wchar_t),
			size_t(PrintNameLength / sizeof(wchar_t))})
	};
	struct tagMountPointReparseBuffer
	{
		unsigned short SubstituteNameOffset;
		unsigned short SubstituteNameLength;
		unsigned short PrintNameOffset;
		unsigned short PrintNameLength;
		wchar_t PathBuffer[1];

		DefGetter(const ynothrow, wstring_view, PrintName,
			{PathBuffer + size_t(PrintNameOffset) / sizeof(wchar_t),
			size_t(PrintNameLength / sizeof(wchar_t))})
	};
	struct tagGenericReparseBuffer
	{
		unsigned char DataBuffer[1];
	};

	unsigned long ReparseTag;
	unsigned short ReparseDataLength;
	unsigned short Reserved;
	union
	{
		tagSymbolicLinkReparseBuffer SymbolicLinkReparseBuffer;
		tagMountPointReparseBuffer MountPointReparseBuffer;
		tagGenericReparseBuffer GenericReparseBuffer;
	};
};

//! \since build 660
yconstexpr const auto FSCTL_GET_REPARSE_POINT(0x000900A8UL);


//! \since build 693
//@{
enum class SystemPaths
{
	System,
	Windows
};

wstring
FetchFixedSystemPath(SystemPaths e, size_t s)
{
	// XXX: Depends right behavior on external API.
	const auto p_buf(make_unique_default_init<wchar_t[]>(s));
	const auto str(p_buf.get());

	switch(e)
	{
	case SystemPaths::System:
		YCL_CallWin32F(GetSystemDirectoryW, str, unsigned(s));
		break;
	case SystemPaths::Windows:
		YCL_CallWin32F(GetSystemWindowsDirectoryW, str, unsigned(s));
		break;
	}
	return ystdex::rtrim(wstring(str), L'\\') + L'\\';
}
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
ImplDeDtor(Win32Exception)

const std::error_category&
Win32Exception::GetErrorCategory()
{
	static const Win32ErrorCategory ecat{};

	return ecat;
}

std::string
Win32Exception::FormatMessage(ErrorCode ec) ynothrow
{
	return TryInvoke([=]{
		try
		{
			wchar_t* buf{};

			YCL_CallWin32F(FormatMessageW, FORMAT_MESSAGE_ALLOCATE_BUFFER
				| FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
				{}, ec, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				reinterpret_cast<wchar_t*>(&buf), 1, {});

			const auto p(unique_raw(buf, LocalDelete()));

			return WCSToMBCS(buf, unsigned(CP_UTF8));
		}
		CatchExpr(..., YTraceDe(Warning, "FormatMessage failed."), throw)
	});
}


ModuleProc*
LoadProc(::HMODULE h_module, const char* proc)
{
	return YCL_CallWin32F(GetProcAddress, h_module, proc);
}


wstring
FetchModuleFileName(::HMODULE h_module, RecordLevel lv)
{
	return ystdex::retry_for_vector<wstring>(MAX_PATH,
		[=](wstring& res, size_t s) -> bool{
		const auto r(size_t(::GetModuleFileNameW(h_module, &res[0], s)));
		const auto e(::GetLastError());

		if(e != ERROR_SUCCESS && e != ERROR_INSUFFICIENT_BUFFER)
			throw Win32Exception(e, "GetModuleFileNameW", lv);
		if(r < s)
		{
			res.resize(r);
			return {};
		}
		return true;
	});
}


void
GlobalDelete::operator()(pointer h) const ynothrow
{
	// NOTE: %::GlobalFree does not ignore null handle value.
	if(h && YB_UNLIKELY(::GlobalFree(h)))
		YTraceDe(Warning, "Error %lu: failed calling GlobalFree @ %s.",
			::GetLastError(), yfsig);
}


GlobalLocked::GlobalLocked(::HGLOBAL h)
	: p_locked(YCL_CallWin32F(GlobalLock, h))
{}
GlobalLocked::~GlobalLocked()
{
	YCL_CallWin32F_Trace(GlobalUnlock, p_locked);
}


void
LocalDelete::operator()(pointer h) const ynothrow
{
	// FIXME: For some platforms, no %::LocalFree available. See https://msdn.microsoft.com/zh-cn/library/windows/desktop/ms679351(v=vs.85).aspx.
	// NOTE: %::LocalFree ignores null handle value.
	if(YB_UNLIKELY(::LocalFree(h)))
		YTraceDe(Warning, "Error %lu: failed calling LocalFree @ %s.",
			::GetLastError(), yfsig);
}


NodeCategory
TryCategorizeNodeAttributes(UniqueHandle::pointer h)
{
	return FetchFileInfo([&](::BY_HANDLE_FILE_INFORMATION& info)
		-> NodeCategory{
		return CategorizeNode(FileAttributes(info.dwFileAttributes));
	}, h);
}

NodeCategory
TryCategorizeNodeDevice(UniqueHandle::pointer h)
{
	NodeCategory res;

	switch(::GetFileType(h))
	{
	case FILE_TYPE_CHAR:
		res = NodeCategory::Character;
		break;
	case FILE_TYPE_PIPE:
		res = NodeCategory::FIFO;
		break;
	case FILE_TYPE_UNKNOWN:
		{
			const auto err(::GetLastError());

			if(err != NO_ERROR)
				throw Win32Exception(err, "GetFileType", Err);
		}
	default:
		res = NodeCategory::Unknown;
	}
	return res;
}

NodeCategory
CategorizeNode(FileAttributes attr, unsigned long reparse_tag) ynothrow
{
	auto res(NodeCategory::Empty);

	if(IsDirectory(attr))
		res |= NodeCategory::Directory;
	if(attr & ReparsePoint)
	{
		switch(reparse_tag)
		{
		case IO_REPARSE_TAG_SYMLINK:
			res |= NodeCategory::SymbolicLink;
			break;
		case IO_REPARSE_TAG_MOUNT_POINT:
			res |= NodeCategory::MountPoint;
		default:
			;
		}
	}
	return res;
}
NodeCategory
CategorizeNode(UniqueHandle::pointer h) ynothrow
{
	TryRet(TryCategorizeNodeAttributes(h) | TryCategorizeNodeDevice(h))
	CatchIgnore(...)
	return NodeCategory::Invalid;
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


void
DirectoryFindData::Deleter::operator()(pointer p) const ynothrowv
{
	FilterExceptions(std::bind(YCL_WrapCallWin32(FindClose, p), yfsig),
		"directory find data deleter");
}


DirectoryFindData::DirectoryFindData(wstring_view name)
	: dir_name(name), find_data()
{
	if(ystdex::rtrim(dir_name, L"/\\").empty())
		dir_name = L'.';
	YAssert(platform::EndsWithNonSeperator(dir_name),
		"Invalid argument found.");

	using platform::FileOperationFailure;
	const auto attr(FileAttributes(::GetFileAttributesW(dir_name.c_str())));
	yconstexpr const auto& msg("Opening directory failed.");

	if(attr != Invalid)
	{
		if(attr & Directory)
			dir_name += L"\\*";
		else
			ystdex::throw_error<FileOperationFailure>(ENOTDIR, msg);
	}
	else
		ystdex::throw_error<FileOperationFailure>(GetErrnoFromWin32(), msg);
}

NodeCategory
DirectoryFindData::GetNodeCategory() const ynothrow
{
	if(p_node && !d_name.empty())
	{
		auto res(CategorizeNode(find_data));
		wstring_view name(GetDirName());

		name.remove_suffix(1);
		TryInvoke([&]{
			auto gd(ystdex::unique_guard([&]() ynothrow{
				res |= NodeCategory::Invalid;
			}));

			// NOTE: Only existed and accessible files are considered.
			// FIXME: Blocked. TOCTTOU access.
			if(const auto h = MakeFile((wstring(name) + d_name).c_str(),
				FileSpecificAccessRights::ReadAttributes,
				FileAttributesAndFlags::NormalWithDirectory))
				res |= TryCategorizeNodeAttributes(h.get())
					| TryCategorizeNodeDevice(h.get());
			ystdex::dismiss(gd);
		});
		return res;
	}
	return NodeCategory::Empty;
}

observer_ptr<wstring>
DirectoryFindData::Read()
{
	const auto chk_err([this](const char* fn, ErrorCode ec) YB_NONNULL(1){
		const auto err(::GetLastError());

		if(err != ec)
			throw Win32Exception(err, fn, Err);
	});

	if(!p_node)
	{
		const auto h_node(::FindFirstFileW(GetDirName().c_str(), &find_data));

		if(h_node != INVALID_HANDLE_VALUE)
			p_node.reset(h_node);
		else
			chk_err("FindFirstFileW", ERROR_FILE_NOT_FOUND);
	}
	else if(!::FindNextFileW(p_node.get(), &find_data))
	{
		chk_err("FindNextFileW", ERROR_NO_MORE_FILES);
		p_node.reset();
	}
	if(p_node)
	{
		d_name = find_data.cFileName;
		if(d_name[0] != wchar_t())
			return make_observer(&d_name);
	}
	return {};
}

void
DirectoryFindData::Rewind() ynothrow
{
	p_node.reset();
}


wstring
ResolveReparsePoint(const wchar_t* path)
{
	return wstring(MakeFileToDo([=](UniqueHandle::pointer h){
		return FetchFileInfo([&](::BY_HANDLE_FILE_INFORMATION& info)
			-> wstring_view{
			if(info.dwFileAttributes & ReparsePoint)
			{
				ystdex::pun_storage_t<byte[MAXIMUM_REPARSE_DATA_BUFFER_SIZE]>
					target_buffer;
				const auto rdb(reinterpret_cast<REPARSE_DATA_BUFFER*>(
					&target_buffer));

				YCL_CallWin32F(DeviceIoControl, h, FSCTL_GET_REPARSE_POINT, {},
					0, &target_buffer, sizeof(target_buffer), {}, {});
				switch(rdb->ReparseTag)
				{
				case IO_REPARSE_TAG_SYMLINK:
					return rdb->SymbolicLinkReparseBuffer.GetPrintName();
				case IO_REPARSE_TAG_MOUNT_POINT:
					return rdb->MountPointReparseBuffer.GetPrintName();
				default:
					YTraceDe(Warning, "Unsupported reparse tag '%lu' found",
						rdb->ReparseTag);
					ystdex::throw_error(std::errc::not_supported);
				}
			}
			throw std::invalid_argument(
				"Specified file is not a reparse point.");
		}, h);
	}, path, FileAttributesAndFlags::NormalAll));
}


size_t
QueryFileLinks(UniqueHandle::pointer h)
{
	return FetchFileInfo([](::BY_HANDLE_FILE_INFORMATION& info){
		return size_t(info.nNumberOfLinks);
	}, h);
}
size_t
QueryFileLinks(const wchar_t* path, bool follow_reparse_point)
{
	return MakeFileToDo<size_t(UniqueHandle::pointer)>(QueryFileLinks, path,
		FollowToAttr(follow_reparse_point));
}

pair<VolumeID, FileID>
QueryFileNodeID(UniqueHandle::pointer h)
{
	return FetchFileInfo([](::BY_HANDLE_FILE_INFORMATION& info) ynothrow
		-> pair<VolumeID, FileID>{
		return {VolumeID(info.dwVolumeSerialNumber),
			FileID(info.nFileIndexHigh) << 32 | info.nFileIndexLow};
	}, h);
}
pair<VolumeID, FileID>
QueryFileNodeID(const wchar_t* path, bool follow_reparse_point)
{
	return MakeFileToDo<pair<VolumeID, FileID>(UniqueHandle::pointer)>(
		QueryFileNodeID, path, FollowToAttr(follow_reparse_point));
}

void
QueryFileTime(UniqueHandle::pointer h, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime)
{
	YCL_CallWin32F(GetFileTime, h, p_ctime, p_atime, p_mtime);
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
SetFileTime(const wchar_t* path, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime, bool follow_reparse_point)
{
	MakeFileToDo(std::bind<void(UniqueHandle::pointer, ::FILETIME*, ::FILETIME*,
		::FILETIME*)>(SetFileTime, std::placeholders::_1, p_ctime, p_atime,
		p_mtime), path, AccessRights::GenericWrite,
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
		ystdex::throw_error(std::errc::not_supported);
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
	ystdex::throw_error(std::errc::not_supported);
}

wstring
ExpandEnvironmentStrings(const wchar_t* p_src)
{
	const auto w_len(YCL_CallWin32F(ExpandEnvironmentStringsW, Nonnull(p_src),
		{}, 0));
	wstring wstr(w_len, wchar_t());

	YCL_CallWin32F(ExpandEnvironmentStringsW, p_src, &wstr[0], w_len);
	return wstr;
}

wstring
FetchSystemPath(size_t s)
{
	return FetchFixedSystemPath(SystemPaths::System, s);
}

wstring
FetchWindowsPath(size_t s)
{
	return FetchFixedSystemPath(SystemPaths::Windows, s);
}

} // inline namespace Windows;

#endif

} // namespace YSLib;

