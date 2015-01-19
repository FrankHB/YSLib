/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r2117
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:41:35 +0800
\par 修改时间:
	2015-01-19 10:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_FileSystem
#include YFM_YCLib_Debug
#include YFM_YCLib_NativeAPI
#include <cstring> // for std::strcpy, std::strchr;
#include <fcntl.h>
#if YCL_DS
#	include YFM_CHRLib_CharacterProcessing

//! \since build 341
extern "C" int
_EXFUN(fileno, (FILE*));
//! \since build 566
extern "C" int
_EXFUN(pclose, (FILE*));
//! \since build 566
extern "C" FILE*
_EXFUN(popen, (const char*, const char*));

//! \since build 475
using namespace CHRLib;
#elif YCL_Win32
#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// At least one headers of <stdlib.h>, <stdio.h>, <Windows.h>, <Windef.h>
//	(and probably more) should have been included to make the MinGW-W64 macro
//	available if it is really being used.
//! \since build 464
//@{
extern "C" _CRTIMP int __cdecl __MINGW_NOTHROW
_fileno(::FILE*);

extern "C" _CRTIMP ::FILE* __cdecl __MINGW_NOTHROW
_wfopen(const wchar_t*, const wchar_t*);
//@}
#	endif
#	include <Shlwapi.h> // for ::PathIsRelativeW;
#	include YFM_MinGW32_YCLib_MinGW32 // for platform_ex::UTF8ToWCS,
//	platform_ex::ConvertTime;

//! \since build 540
using platform_ex::UTF8ToWCS;
//! \since build 549
using platform_ex::DirectoryFindData;
#elif YCL_API_POSIXFileSystem
#	include YFM_CHRLib_CharacterProcessing
#	include <dirent.h>
#	include <sys/stat.h>

//! \since build 475
using namespace CHRLib;
#endif

namespace platform
{

namespace
{

std::string
ensure_str(const char* s)
{
	return s;
}
std::string
ensure_str(const char16_t* s)
{
#if YCL_Win32
	return platform_ex::WCSToMBCS(reinterpret_cast<const wchar_t*>(s));
#elif YCL_API_POSIXFileSystem
	return MakeMBCS(s);
#else
#	error "Unsupported platform found."
#endif
}

//! \since build 544
template<typename _tChar>
std::chrono::nanoseconds
GetFileModificationTimeOfImpl(const _tChar* filename)
{
	if(const std::unique_ptr<int, FileDescriptorDeleter>
		fdw{platform::uopen(filename, O_RDONLY)})
		return GetFileModificationTimeOf(*fdw.get());
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file time of \"" + ensure_str(filename) + "\".");
}


//! \since build 560
template<typename _type>
NodeCategory
FetchNodeCategoryFromStat(_type& st)
{
	auto res(NodeCategory::Empty);
	const auto m(st.st_mode & S_IFMT);

	if(m & S_IFDIR)
		res |= NodeCategory::Directory;
#if !YCL_Win32
	if(m & S_IFLNK)
		res |= NodeCategory::Link;
#endif
	if(m & S_IFREG)
		res |= NodeCategory::Regular;
	if(YB_UNLIKELY(m & S_IFCHR))
		res |= NodeCategory::Character;
	else if(YB_UNLIKELY(m & S_IFCHR))
		res |= NodeCategory::Block;
	if(YB_UNLIKELY(m & S_IFIFO))
		res |= NodeCategory::FIFO;
#if !YCL_Win32
	if(m & S_IFSOCK)
		res |= NodeCategory::Socket;
#endif
	return res;;
}

} // unnamed namespace;


FileDescriptor::FileDescriptor(std::FILE* fp) ynothrow
#if YCL_Win32
	: desc(fp ? ::_fileno(fp) : -1)
#else
	: desc(fp ? fileno(fp) : -1)
#endif
{}

int
FileDescriptor::SetMode(int mode) const ynothrow
{
#if YCL_Win32
	return ::_setmode(desc, mode);
#elif defined(_NEWLIB_VERSION) && defined(__SCLE)
	// TODO: Other platforms.
	return ::setmode(desc, mode);
#else
	// NOTE: No effect.
	yunused(mode);
	return 0;
#endif
}


void
FileDescriptorDeleter::operator()(FileDescriptorDeleter::pointer p) ynothrow
{
	if(p)
		::close(*p);
}


// XXX: Catch %std::bad_alloc?
#define YCL_Impl_RetTryCatchAll(...) \
	TryRet(__VA_ARGS__) \
	CatchIgnore(...)

int
uaccess(const char* path, int amode) ynothrow
{
	YAssertNonnull(path);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_waccess(UTF8ToWCS(path).c_str(), amode))
	return -1;
#else
	return ::access(path, amode);
#endif
}
int
uaccess(const char16_t* path, int amode) ynothrow
{
	YAssertNonnull(path);
#if YCL_Win32
	return ::_waccess(reinterpret_cast<const wchar_t*>(path), amode);
#else
	YCL_Impl_RetTryCatchAll(::access(MakeMBCS(path).c_str(), amode))
	return -1;
#endif
}

int
uopen(const char* filename, int oflag) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wopen(UTF8ToWCS(filename).c_str(), oflag))
	return -1;
#else
	return ::open(filename, oflag);
#endif
}
int
uopen(const char* filename, int oflag, int pmode) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wopen(UTF8ToWCS(filename).c_str(), oflag, pmode))
	return -1;
#else
	return ::open(filename, oflag, pmode);
#endif
}
int
uopen(const char16_t* filename, int oflag) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag);
#else
	YCL_Impl_RetTryCatchAll(::open(MakeMBCS(filename).c_str(), oflag))
	return -1;
#endif
}
int
uopen(const char16_t* filename, int oflag, int pmode) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag, pmode);
#else
	YCL_Impl_RetTryCatchAll(::open(MakeMBCS(filename).c_str(), oflag, pmode))
	return -1;
#endif
}

std::FILE*
ufopen(const char* filename, const char* mode) ynothrow
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wfopen(UTF8ToWCS(filename).c_str(),
		UTF8ToWCS(mode).c_str()))
	return {};
#else
	return std::fopen(filename, mode);
#endif
}
std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrow
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	return ::_wfopen(reinterpret_cast<const wchar_t*>(filename),
		reinterpret_cast<const wchar_t*>(mode));
#else
	YCL_Impl_RetTryCatchAll(std::fopen(MakeMBCS(filename).c_str(),
		MakeMBCS(mode).c_str()))
	return {};
#endif
}

bool
ufexists(const char* filename) ynothrow
{
#if YCL_Win32
	YAssertNonnull(filename);
	if(const auto file = ufopen(filename, "rb"))
	{
		std::fclose(file);
		return true;
	}
	return {};
#else
	return ystdex::fexists(filename);
#endif
}
bool
ufexists(const char16_t* filename) ynothrow
{
	if(const auto file = ufopen(filename, u"rb"))
	{
		std::fclose(file);
		return true;
	}
	return {};
}

int
upclose(std::FILE* fp) ynothrow
{
	YAssertNonnull(fp);
#if YCL_Win32
	return ::_pclose(fp);
#else
	return ::pclose(fp);
#endif
}

std::FILE*
upopen(const char* filename, const char* mode) ynothrow
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wpopen(UTF8ToWCS(filename).c_str(),
		UTF8ToWCS(mode).c_str()))
	return {};
#else
	return ::popen(filename, mode);
#endif
}
std::FILE*
upopen(const char16_t* filename, const char16_t* mode) ynothrow
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	return ::_wpopen(reinterpret_cast<const wchar_t*>(filename),
		reinterpret_cast<const wchar_t*>(mode));
#else
	YCL_Impl_RetTryCatchAll(::popen(MakeMBCS(filename).c_str(),
		MakeMBCS(mode).c_str()))
	return {};
#endif
}

char16_t*
u16getcwd_n(char16_t* buf, std::size_t size) ynothrow
{
	if(YB_UNLIKELY(!buf || size == 0))
		errno = EINVAL;
	else
	{
		using namespace std;

#if YCL_Win32
		return reinterpret_cast<char16_t*>(
			::_wgetcwd(reinterpret_cast<wchar_t*>(buf), size));
#else
		if(const auto cwd = ::getcwd(reinterpret_cast<char*>(buf), size))
			try
			{
				const auto res(MakeUCS2LE(cwd));
				const auto len(res.length());

				if(size < len + 1)
					errno = ERANGE;
				else
				{
					std::copy(res.begin(), res.end(), buf);
					return buf;
				}
			}
			CatchExpr(std::bad_alloc&, errno = ENOMEM)
#endif
	}
	return {};
}

//! \since build 476
#define YCL_Impl_FileSystem_ufunc_1(_n) \
bool \
_n(const char* path) ynothrow \
{ \
	YAssertNonnull(path); \

#if YCL_Win32
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	YCL_Impl_RetTryCatchAll(_wfn(UTF8ToWCS(path).c_str()) == 0) \
	return false; \
}
#else
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	return _fn(path) == 0; \
}
#endif

#define YCL_Impl_FileSystem_ufunc(_n, _fn, _wfn) \
	YCL_Impl_FileSystem_ufunc_1(_n) \
	YCL_Impl_FileSystem_ufunc_2(_fn, _wfn)

YCL_Impl_FileSystem_ufunc(uchdir, ::chdir, ::_wchdir)

YCL_Impl_FileSystem_ufunc_1(umkdir)
#if YCL_Win32
	YCL_Impl_FileSystem_ufunc_2(_unused_, ::_wmkdir)
#else
	return ::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0;
}
#endif

YCL_Impl_FileSystem_ufunc(urmdir, ::rmdir, ::_wrmdir)

YCL_Impl_FileSystem_ufunc(uunlink, ::unlink, ::_wunlink)

YCL_Impl_FileSystem_ufunc(uremove, std::remove, ::_wremove)

#undef YCL_Impl_FileSystem_ufunc_1
#undef YCL_Impl_FileSystem_ufunc_2
#undef YCL_Impl_FileSystem_ufunc

bool
truncate(std::FILE* fp, std::size_t size) ynothrow
{
#if YCL_Win32
	return ::_chsize(::_fileno(fp), long(size)) == 0;
#else
	return ::ftruncate(fileno(fp), ::off_t(size)) == 0;
#endif
}


std::chrono::nanoseconds
GetFileModificationTimeOf(int fd)
{
#if YCL_Win32
	// NOTE: The %::FILETIME has resolution of 100 nanoseconds.
	::FILETIME file_time;

	// XXX: Error handling for indirect calls.
	if(!::GetFileTime(::HANDLE(::_get_osfhandle(fd)), {}, {}, &file_time))
		YF_Raise_Win32Exception("GetFileTime");
	TryRet(platform_ex::ConvertTime(file_time))
	CatchExpr(std::system_error& e,
		throw FileOperationFailure(e.code(), std::string(
		"Failed querying file modification time: ") + e.what() + "."))
#else
	// TODO: Get more precise time count.
	struct ::stat st;

	if(::fstat(fd, &st) == 0)
		return std::chrono::seconds(st.st_mtime);
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file size.");
#endif
}
std::chrono::nanoseconds
GetFileModificationTimeOf(std::FILE* fp)
{
	YAssertNonnull(fp);
#if YCL_Win32
	return GetFileModificationTimeOf(::_fileno(fp));
#else
	return GetFileModificationTimeOf(fileno(fp));
#endif
}
std::chrono::nanoseconds
GetFileModificationTimeOf(const char* filename)
{
	return GetFileModificationTimeOfImpl(filename);
}
std::chrono::nanoseconds
GetFileModificationTimeOf(const char16_t* filename)
{
	return GetFileModificationTimeOfImpl(filename);
}

std::uint64_t
GetFileSizeOf(int fd)
{
#if YCL_Win32
	::LARGE_INTEGER sz;

	// XXX: Error handling for indirect calls.
	if(::GetFileSizeEx(::HANDLE(::_get_osfhandle(fd)), &sz) != 0
		&& YB_LIKELY(sz.QuadPart >= 0))
		return sz.QuadPart;
	// TODO: Get correct error condition.
#else
	struct ::stat st;

	if(::fstat(fd, &st) == 0)
		return st.st_size;
#endif
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file size.");
}
std::uint64_t
GetFileSizeOf(std::FILE* fp)
{
	YAssertNonnull(fp);
#if YCL_Win32
	return GetFileSizeOf(::_fileno(fp));
#else
	return GetFileSizeOf(fileno(fp));
#endif
}


DirectorySession::DirectorySession(const char* path)
#if YCL_Win32
	: dir(new DirectoryFindData(path && *path != '\0' ? path : "."))
#else
	: sDirPath(path && *path != '\0' ? path : "."),
	dir(::opendir(sDirPath.c_str()))
#endif
{
	if(!dir)
		throw FileOperationFailure(errno, std::generic_category(),
			"Opening directory failed.");
#if !YCL_Win32
	ystdex::rtrim(sDirPath, YCL_PATH_DELIMITER);
	YAssert(sDirPath.empty() || sDirPath.back() != YCL_PATH_DELIMITER,
		"Invalid directory name state found.");
	sDirPath += YCL_PATH_DELIMITER;
#endif
}
DirectorySession::~DirectorySession()
{
#if !YCL_Win32
	const auto res(::closedir(dir));

	YAssert(res == 0, "No valid directory found.");
	yunused(res);
#else
	delete static_cast<DirectoryFindData*>(dir);
#endif
}

void
DirectorySession::Rewind() ynothrow
{
	YAssert(dir, "Invalid native handle found.");
#if YCL_Win32
	static_cast<DirectoryFindData*>(dir)->Rewind();
#else
	::rewinddir(dir);
#endif
}


HDirectory&
HDirectory::operator++()
{
	YAssert(!p_dirent || bool(GetNativeHandle()), "Invariant violation found.");
#if YCL_Win32
	p_dirent = static_cast<DirectoryFindData*>(GetNativeHandle())->Read();
#else
	p_dirent = ::readdir(GetNativeHandle());
#endif
	return *this;
}

NodeCategory
HDirectory::GetNodeCategory() const ynothrow
{
	if(p_dirent)
	{
		YAssert(bool(GetNativeHandle()), "Invariant violation found.");

		NodeCategory res(NodeCategory::Empty);
#if YCL_Win32
		const auto&
			dir_data(Deref(static_cast<DirectoryFindData*>(GetNativeHandle())));
		const auto& find_data(dir_data.GetFindData());

		if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			res |= NodeCategory::Directory;
		if(find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			switch(find_data.dwReserved0)
			{
			case IO_REPARSE_TAG_SYMLINK:
				res |= NodeCategory::SymbolicLink;
				break;
			case IO_REPARSE_TAG_MOUNT_POINT:
				res |= NodeCategory::MountPoint;
			default:
				// TODO: Implement for other tags?
				;
			}
		}

		struct ::_stat st;
		auto name(dir_data.GetDirName());

		YAssert(!name.empty() && name.back() == L'*', "Invalid state found.");
		name.pop_back();
		YAssert(!name.empty() && name.back() == L'\\', "Invalid state found.");
		if(::_wstat((name
			+ Deref(static_cast<std::wstring*>(p_dirent))).c_str(), &st) == 0)
			res |= FetchNodeCategoryFromStat(st);
#else
		auto name(sDirPath + Deref(p_dirent).d_name);
		struct ::stat st;

#	if !YCL_DS
		// TODO: Set error properly.
		if(::lstat(name.c_str(), &st) == 0)
			res |= FetchNodeCategoryFromStat(st);
#	endif
		if(bool(res & NodeCategory::Link) && ::stat(name.c_str(), &st) == 0)
			res |= FetchNodeCategoryFromStat(st);
#endif
		return res != NodeCategory::Empty ? res : NodeCategory::Invalid;
	}
	return NodeCategory::Empty;
}

const char*
HDirectory::GetName() const ynothrow
{
	if(p_dirent)
	{
#if !YCL_Win32
		return p_dirent->d_name;
#else
		YCL_Impl_RetTryCatchAll(utf8_name = platform_ex::WCSToUTF8(
			Deref(static_cast<std::wstring*>(p_dirent))), &utf8_name[0])
#endif
	}
	return ".";
}

#undef YCL_Impl_RetTryCatchAll


bool
IsAbsolute(const char* path)
{
#if YCL_Win32
	return Deref(path) == YCL_PATH_DELIMITER
		|| (*path != '\0' && path[1] == ':');
#else
	if(Deref(path) == '/')
		return true;

	const auto p(std::strstr(path, ":/"));

	return p && p != path && !std::strstr(p, ":/");
#endif
}
bool
IsAbsolute(const char16_t* path)
{
#if YCL_Win32
	return Deref(path) == u'\\' || (*path != u'\0' && path[1] == u':');
#else
	if(Deref(path) == u'/')
		return true;

	const std::u16string upath(path);
	const auto n(upath.find(u":/"));

	// TODO: Optimize for performance.
	return n != std::u16string::npos && n != 0
		&& upath.substr(n).find(u":/") == std::u16string::npos;
#endif
}

std::size_t
GetRootNameLength(const char* path)
{
	const char* p(std::strchr(Nonnull(path), ':'));

	return !p ? 0 : p - path + 1;
}

}

namespace platform_ex
{

#if !YCL_Win32
char16_t
FS_IsRoot(const char16_t* str)
{
	const std::u16string ustr(str);

	return ustr == u"/"
#	if YCL_DS
		|| ustr == u"fat:/" || ustr == u"sd:/"
#	endif
	;
}
#endif

}

