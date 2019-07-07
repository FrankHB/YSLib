/*
	© 2013-2017, 2019 FrankHB.

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
\version r2197
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 15:35:19 +0800
\par 修改时间:
	2019-07-07 20:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::MinGW32
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_Win32_YCLib_Registry // for platform::NodeCategory, ERROR_*,
//	std::string, UniqueHandle, ::BY_HANDLE_FILE_INFORMATION, YCL_CallF_Win32,
//	YCL_Raise_Win32E, NO_ERROR, ::OVERLAPPED, ::GetSystemDirectoryW,
//	::GetSystemWindowsDirectoryW, ystdex::rtrim, ::FormatMessageW, ::HMODULE,
//	::GetProcAddress, ::GetModuleFileNameW, ::CreateFileW, INVALID_HANDLE_VALUE,
//	RegistryKey, HKEY_*, FindClose, ystdex::throw_error, ::FindFirstFileW,
//	::FindNextFileW, std::errc::not_supported, std::invalid_argument,
//	::LARGE_INTEGER, ::GetFileTime, ::SetFileTime, ::LockFileEx, ::UnlockFileEx,
//	::WaitForSingleObject;
#	include <cerrno> // for EINVAL, ENOENT, EMFILE, EACCESS, EBADF, ENOMEM,
//	ENOEXEC, EXDEV, EEXIST, EAGAIN, EPIPE, ENOSPC, ECHILD, ENOTEMPTY;
#	include YFM_YSLib_Core_YCoreUtilities // for YSLib::IsInClosedInterval,
//	YSLib::make_unique_default_init, YSLib::TryInvoke,
//	platform::EndsWithNonSeperator;
#	include YFM_Win32_YCLib_NLS // for WCSToUTF8;
#	include <ystdex/container.hpp> // for ystdex::retry_for_vector;
#	include <ystdex/scope_guard.hpp> // for ystdex::unique_guard,
//	ystdex::dismiss, std::bind, std::placeholders::_1;

using namespace YSLib;
//! \since build 658
using platform::NodeCategory;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

int
ConvertToErrno(ErrorCode err) ynothrow
{
	// NOTE: This mapping is from universal CRT in Windows SDK 10.0.10150.0,
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

class Win32ErrorCategory : public std::error_category
{
public:
	PDefH(const char*, name, ) const ynothrow override
		ImplRet("Win32Error")
	//! \since build 564
	PDefH(std::string, message, int ev) const override
		// NOTE: For Win32 a %::DWORD can be mapped one-to-one for 32-bit %int.
		ImplRet("Error " + std::to_string(ev) + ": "
			+ Win32Exception::FormatMessage(ErrorCode(ev)))
};


//! \since build 651
//@{
template<typename _func>
auto
FetchFileInfo(_func f, UniqueHandle::pointer h)
	-> decltype(f(std::declval<::BY_HANDLE_FILE_INFORMATION&>()))
{
	::BY_HANDLE_FILE_INFORMATION info;

	YCL_CallF_Win32(GetFileInformationByHandle, h, &info);
	return f(info);
}

template<typename _func, typename... _tParams>
auto
MakeFileToDo(_func f, _tParams&&... args)
	-> decltype(f(UniqueHandle::pointer()))
{
	if(const auto h = MakeFile(yforward(args)...))
		return f(h.get());
	YCL_Raise_Win32E("CreateFileW", yfsig);
}

yconstfn
PDefH(FileAttributesAndFlags, FollowToAttr, bool follow_reparse_point) ynothrow
	ImplRet(follow_reparse_point ? FileAttributesAndFlags::NormalWithDirectory
		: FileAttributesAndFlags::NormalAll)
//@}


//! \since build 660
yconstexpr const auto FSCTL_GET_REPARSE_POINT(0x000900A8UL);


//! \since build 721
//@{
// TODO: Extract to %YCLib.NativeAPI?
yconstfn PDefH(unsigned long, High32, std::uint64_t val) ynothrow
	ImplRet(static_cast<unsigned long>(val >> 32UL))

yconstfn PDefH(unsigned long, Low32, std::uint64_t val) ynothrow
	ImplRet(static_cast<unsigned long>(val))

template<typename _func>
auto
DoWithDefaultOverlapped(_func f, std::uint64_t off)
	-> decltype(f(std::declval<::OVERLAPPED&>()))
{
	::OVERLAPPED overlapped{0, 0, {Low32(off), High32(off)}, {}};

	return f(overlapped);
}
//@}


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
		YCL_CallF_Win32(GetSystemDirectoryW, str, unsigned(s));
		break;
	case SystemPaths::Windows:
		YCL_CallF_Win32(GetSystemWindowsDirectoryW, str, unsigned(s));
		break;
	}
	return ystdex::rtrim(wstring(str), L'\\') + L'\\';
}
//@}

} // unnamed namespace;


Win32Exception::Win32Exception(ErrorCode ec, const char* str, RecordLevel lv)
	: Exception(int(ec), GetErrorCategory(), str, lv)
{
	YAssert(ec != 0, "No error should be thrown.");
}
Win32Exception::Win32Exception(ErrorCode ec, string_view sv, RecordLevel lv)
	: Exception(int(ec), GetErrorCategory(), sv, lv)
{
	YAssert(ec != 0, "No error should be thrown.");
}
Win32Exception::Win32Exception(ErrorCode ec, const std::string& str,
	RecordLevel lv)
	: Exception(int(ec), GetErrorCategory(), str, lv)
{
	YAssert(ec != 0, "No error should be thrown.");
}
Win32Exception::Win32Exception(ErrorCode ec, string_view sv, const char* sig,
	RecordLevel lv)
	: Win32Exception(ec, platform::ComposeMessageWithSignature(sv, sig), lv)
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

			YCL_CallF_Win32(FormatMessageW, FORMAT_MESSAGE_ALLOCATE_BUFFER
				| FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
				{}, ec, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				reinterpret_cast<wchar_t*>(&buf), 1, {});

			const auto p(unique_raw(buf, LocalDelete()));

			return to_std_string(ystdex::rtrim(WCSToUTF8(buf)));
		}
		CatchExpr(..., YTraceDe(Warning, "FormatMessage failed."), throw)
	});
}


ModuleProc*
LoadProc(::HMODULE h_module, const char* proc)
{
	return YCL_CallF_Win32(GetProcAddress, h_module, proc);
}


wstring
FetchModuleFileName(::HMODULE h_module, RecordLevel lv)
{
	// TODO: Avoid retry for NT 6 %::GetModuleFileNameW?
	return ystdex::retry_for_vector<wstring>(MAX_PATH,
		[=](wstring& res, size_t s) -> bool{
		const auto r(size_t(::GetModuleFileNameW(h_module, &res[0],
			static_cast<unsigned long>(s))));
		const auto err(::GetLastError());

		if(err != ERROR_SUCCESS && err != ERROR_INSUFFICIENT_BUFFER)
			throw Win32Exception(err, "GetModuleFileNameW", lv);
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
		YCL_Trace_Win32E(Warning, GlobalFree, yfsig);
}


GlobalLocked::GlobalLocked(::HGLOBAL h)
	: p_locked(YCL_CallF_Win32(GlobalLock, h))
{}
GlobalLocked::~GlobalLocked()
{
	YCL_TraceCallF_Win32(GlobalUnlock, p_locked);
}


void
LocalDelete::operator()(pointer h) const ynothrow
{
	// FIXME: For some platforms, no %::LocalFree available. See https://msdn.microsoft.com/zh-cn/library/windows/desktop/ms679351(v=vs.85).aspx.
	// NOTE: %::LocalFree ignores null handle value.
	if(YB_UNLIKELY(::LocalFree(h)))
		YCL_Trace_Win32E(Warning, LocalFree, yfsig);
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
		YB_ATTR_fallthrough;
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


void
DirectoryFindData::Deleter::operator()(pointer p) const ynothrowv
{
	FilterExceptions(std::bind(YCL_WrapCall_Win32(FindClose, p), yfsig),
		"directory find data deleter");
}


DirectoryFindData::DirectoryFindData(wstring name)
	: dir_name(std::move(name)), find_data()
{
	if(ystdex::rtrim(dir_name, L"/\\").empty())
		dir_name = L'.';
	YAssert(platform::EndsWithNonSeperator(dir_name),
		"Invalid argument found.");

	const auto attr(FileAttributes(::GetFileAttributesW(dir_name.c_str())));

	if(attr != Invalid)
	{
		if(attr & Directory)
			dir_name += L"\\*";
		else
			ystdex::throw_error(ENOTDIR, yfsig);
	}
	else
		YCL_Raise_Win32E("GetFileAttributesW", yfsig);
}

NodeCategory
DirectoryFindData::GetNodeCategory() const ynothrow
{
	if(p_node && find_data.cFileName[0] != wchar_t())
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
			if(const auto h = MakeFile((wstring(name) + GetEntryName()).c_str(),
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

bool
DirectoryFindData::Read()
{
	const auto chk_err(
		[this](const char* fn, ErrorCode ec) YB_ATTR_LAMBDA(nonnull(1)){
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
		return find_data.cFileName[0] != wchar_t();
	find_data.cFileName[0] = wchar_t();
	return {};
}

void
DirectoryFindData::Rewind() ynothrow
{
	p_node.reset();
}


struct ReparsePointData::Data final
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


ReparsePointData::ReparsePointData()
	: pun(ystdex::default_init, &target_buffer)
{
	static_assert(ystdex::is_aligned_storable<decltype(target_buffer), Data>(),
		"Invalid buffer found.");
}
ImplDeDtor(ReparsePointData)


wstring
ResolveReparsePoint(const wchar_t* path)
{
	return wstring(ResolveReparsePoint(path, ReparsePointData().Get()));
}
wstring_view
ResolveReparsePoint(const wchar_t* path, ReparsePointData::Data& rdb)
{
	return MakeFileToDo([=, &rdb](UniqueHandle::pointer h){
		return FetchFileInfo([&](::BY_HANDLE_FILE_INFORMATION& info)
			-> wstring_view{
			if(info.dwFileAttributes & ReparsePoint)
			{
				YCL_CallF_Win32(DeviceIoControl, h, FSCTL_GET_REPARSE_POINT, {},
					0, &rdb, MAXIMUM_REPARSE_DATA_BUFFER_SIZE, {}, {});
				switch(rdb.ReparseTag)
				{
				case IO_REPARSE_TAG_SYMLINK:
					return rdb.SymbolicLinkReparseBuffer.GetPrintName();
				case IO_REPARSE_TAG_MOUNT_POINT:
					return rdb.MountPointReparseBuffer.GetPrintName();
				default:
					YTraceDe(Warning, "Unsupported reparse tag '%lu' found",
						rdb.ReparseTag);
					ystdex::throw_error(std::errc::not_supported, yfsig);
				}
			}
			throw std::invalid_argument(
				"Specified file is not a reparse point.");
		}, h);
	}, path, FileAttributesAndFlags::NormalAll);
}


wstring
ExpandEnvironmentStrings(const wchar_t* p_src)
{
	const auto w_len(YCL_CallF_Win32(ExpandEnvironmentStringsW, Nonnull(p_src),
		{}, 0));
	wstring wstr(w_len, wchar_t());

	YCL_CallF_Win32(ExpandEnvironmentStringsW, p_src, &wstr[0], w_len);
	return wstr;
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

std::uint64_t
QueryFileSize(UniqueHandle::pointer h)
{
	::LARGE_INTEGER sz;

	YCL_CallF_Win32(GetFileSizeEx, h, &sz);

	if(sz.QuadPart >= 0)
		return std::uint64_t(sz.QuadPart);
	throw std::invalid_argument("Negative file size found.");
}
std::uint64_t
QueryFileSize(const wchar_t* path)
{
	return MakeFileToDo<std::uint64_t(UniqueHandle::pointer)>(
		QueryFileSize, path, FileAttributesAndFlags::NormalWithDirectory);
}

void
QueryFileTime(UniqueHandle::pointer h, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime)
{
	YCL_CallF_Win32(GetFileTime, h, p_ctime, p_atime, p_mtime);
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

	YCL_CallF_Win32(SetFileTime, h, p_ctime, p_atime, p_mtime);
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
		ystdex::throw_error(std::errc::not_supported, yfsig);
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
	ystdex::throw_error(std::errc::not_supported, yfsig);
}


void
LockFile(UniqueHandle::pointer h, std::uint64_t off, std::uint64_t len,
	bool exclusive, bool immediate)
{
	if(YB_UNLIKELY(!TryLockFile(h, off, len, exclusive, immediate)))
		YCL_Raise_Win32E("LockFileEx", yfsig);
}

bool
TryLockFile(UniqueHandle::pointer h, std::uint64_t off, std::uint64_t len,
	bool exclusive, bool immediate) ynothrow
{
	return DoWithDefaultOverlapped([=](::OVERLAPPED& overlapped) ynothrow{
		// NOTE: Since it can always be checked as result and this is called by
		//	%LockFile, no logging with %YCL_TraceCallF_Win32 is here.
		return ::LockFileEx(h, (exclusive ? LOCKFILE_EXCLUSIVE_LOCK : 0UL)
			| (immediate ? LOCKFILE_FAIL_IMMEDIATELY : 0UL), yimpl(0),
			Low32(len), High32(len), &overlapped);
	}, off);
}

bool
UnlockFile(UniqueHandle::pointer h, std::uint64_t off, std::uint64_t len)
	ynothrow
{
	return DoWithDefaultOverlapped([=](::OVERLAPPED& overlapped) ynothrow{
		return
			::UnlockFileEx(h, yimpl(0), Low32(len), High32(len), &overlapped);
	}, off);
}


void
WaitUnique(UniqueHandle::pointer h, unsigned long ms)
{
	if(YB_UNLIKELY(::WaitForSingleObject(h, ms) == WAIT_FAILED))
		YCL_Raise_Win32E("WaitForSingleObject", yfsig);
}

bool
TryWaitUnique(UniqueHandle::pointer h, unsigned long ms) ynothrow
{
	return ::WaitForSingleObject(h, ms) == WAIT_OBJECT_0;
}


Mutex::Mutex(const wchar_t* name)
	// TODO: Save last error and detect %ERROR_ALREADY_EXISTS?
	// TODO: Use desired attributes with %::CreateMutexW?
	: h_mutex(YCL_CallF_Win32(CreateMutexW, {}, {}, name))
{}

void
Mutex::unlock() ynothrowv
{
	const auto res(YCL_TraceCallF_Win32(ReleaseMutex, native_handle()));

	YAssert(res, "Narrow contract violated.");
	yunused(res);
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

