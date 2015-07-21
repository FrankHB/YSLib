/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileIO.cpp
\ingroup YCLib
\brief 平台相关的文件访问和输入/输出接口。
\version r525
\author FrankHB <frankhb1989@gmail.com>
\since build 615
\par 创建时间:
	2015-07-14 18:53:12 +0800
\par 修改时间:
	2015-07-21 09:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileIO
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_FileIO
#include YFM_YCLib_Debug // for Nonnull;
#include YFM_YCLib_Reference // for unique_ptr;
#include <fcntl.h> // for O_RDONLY;
#if YCL_DS
#	include YFM_YCLib_NativeAPI // for ::close, ::getcwd, ::ftruncate;
#	include "CHRLib/YModules.h"
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
#	include YFM_MinGW32_YCLib_MinGW32 // for platform_ex::UTF8ToWCS,
//	platform_ex::ConvertTime;

//! \since build 540
using platform_ex::UTF8ToWCS;
//! \since build 549
using platform_ex::DirectoryFindData;
#elif YCL_API_POSIXFileSystem
#	include YFM_YCLib_NativeAPI // for ::close, ::getcwd, ::ftruncate;
#	include "CHRLib/YModules.h"
#	include YFM_CHRLib_CharacterProcessing
#	include <dirent.h>
#	include <sys/stat.h>
#	include <time.h> // for ::localtime_r;

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
	if(const unique_ptr<int, FileDescriptorDeleter>
		fdw{platform::uopen(filename, O_RDONLY)})
		return GetFileModificationTimeOf(*fdw.get());
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file time of \"" + ensure_str(filename) + "\".");
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


void
SetBinaryIO(std::FILE* stream) ynothrow
{
#if YCL_Win32
	FileDescriptor(Nonnull(stream)).SetMode(_O_BINARY);
#else
	// NOTE: No effect.
	Nonnull(stream);
#endif
}

int
TryClose(std::FILE* fp) ynothrow
{
	int err = 0;

	errno = 0;
	do
		err = std::fclose(fp);
	while(err != 0 && errno == EINTR);
	return err;
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
std::FILE*
ufopen(const char* filename, std::ios_base::openmode mode) ynothrow
{
	if(const auto c_mode = ystdex::openmode_conv(mode))
		return ufopen(filename, c_mode);
	return {};
}
std::FILE*
ufopen(const char16_t* filename, std::ios_base::openmode mode) ynothrow
{
	YAssertNonnull(filename);
	if(const auto c_mode = ystdex::openmode_conv(mode))
#if YCL_Win32
		YCL_Impl_RetTryCatchAll(::_wfopen(reinterpret_cast<const wchar_t*>(
			filename), UTF8ToWCS(c_mode).c_str()))
#else
		YCL_Impl_RetTryCatchAll(std::fopen(MakeMBCS(filename).c_str(), c_mode))
#endif
	return {};
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
u16getcwd_n(char16_t* buf, size_t size) ynothrow
{
	if(YB_UNLIKELY(!buf || size == 0))
		errno = EINVAL;
	else
	{
		using namespace std;

#if YCL_Win32
		return reinterpret_cast<char16_t*>(
			::_wgetcwd(reinterpret_cast<wchar_t*>(buf), int(size)));
#else
		if(const auto cwd = ::getcwd(reinterpret_cast<char*>(buf), size))
			try
			{
				const auto res(MakeUCS2LE(cwd));
				const auto len(res.length());

				if(size < len + 1)
					errno = ERANGE;
				else
					return ystdex::ntctscpy(buf, res.data(), len);
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
truncate(std::FILE* fp, size_t size) ynothrow
{
#if YCL_Win32
	return ::_chsize(::_fileno(fp), long(size)) == 0;
#else
	return ::ftruncate(fileno(fp), ::off_t(size)) == 0;
#endif
}


ImplDeDtor(FileOperationFailure)


std::chrono::nanoseconds
GetFileModificationTimeOf(int fd)
{
#if YCL_Win32
	// NOTE: The %::FILETIME has resolution of 100 nanoseconds.
	::FILETIME file_time;

	// XXX: Error handling for indirect calls.
	YCL_CallWin32(GetFileTime, "GetFileModificationTimeOf",
		::HANDLE(::_get_osfhandle(fd)), {}, {}, &file_time);
	TryRet(platform_ex::ConvertTime(file_time))
	CatchThrow(std::system_error& e, FileOperationFailure(e.code(), std::string(
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
		return std::uint64_t(sz.QuadPart);
	// TODO: Get correct error condition.
#else
	struct ::stat st;

	if(::fstat(fd, &st) == 0)
		// TODO: Use YSLib::CheckNonnegativeScalar<std::uint64_t>?
		// XXX: No negative file size should be found. See also:
		//	http://stackoverflow.com/questions/12275831/why-is-the-st-size-field-in-struct-stat-signed . 
		return std::uint64_t(st.st_size);
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

} // namespace platform;

