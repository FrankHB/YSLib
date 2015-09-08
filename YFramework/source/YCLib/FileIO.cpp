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
\version r1188
\author FrankHB <frankhb1989@gmail.com>
\since build 615
\par 创建时间:
	2015-07-14 18:53:12 +0800
\par 修改时间:
	2015-09-08 22:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileIO
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_FileIO
#include YFM_YCLib_Debug // for Nonnull;
#include YFM_YCLib_Reference // for unique_ptr;
#include YFM_YCLib_NativeAPI // for std::is_same, ystdex::underlying_type_t,
//	Mode, O_RDONLY, ::close, ::stat, ::fstat, ::fcntl, F_GETFL, O_NONBLOCK,
//	::ftruncate, ::getcwd, ::_wgetcwd;
#if YCL_DS
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
//	platform_ex::QueryFileTime, platform_ex::UniqueHandle::pointer,
//	platform_ex::ConvertTime;

//! \since build 540
using platform_ex::UTF8ToWCS;
//! \since build 549
using platform_ex::DirectoryFindData;
#elif YCL_API_POSIXFileSystem
#	include "CHRLib/YModules.h"
#	include YFM_CHRLib_CharacterProcessing
#	include <dirent.h>

//! \since build 475
using namespace CHRLib;
#else
#	error "Unsupported platform found."
#endif

namespace platform
{

//! \since build 627
static_assert(std::is_same<::mode_t, ystdex::underlying_type_t<Mode>>(),
	"Mismatched mode types found.");

namespace
{

//! \since build 625
//@{
template<typename _func>
auto
RetryOnInterrupted(_func f) -> decltype(RetryOnError(f, errno, EINTR))
{
	return RetryOnError(f, errno, EINTR);
}

template<typename _func, typename _tBuf>
size_t
SafeReadWrite(_func f, int fd, _tBuf buf, size_t nbyte) ynothrowv
{
	return size_t(RetryOnInterrupted([&]{
		return f(fd, buf, nbyte);
	}));
}

template<int _vErr, typename _func, typename _tByteBuf>
_tByteBuf
FullReadWrite(_func f, _tByteBuf ptr, size_t nbyte)
{
	while(nbyte > 0)
	{
		const auto n(f(ptr, nbyte));

		if(n == size_t(-1))
			break;
		if(n == 0)
		{
			errno = _vErr;
			break;
		}
		yunseq(ptr += n, nbyte -= n);
	}
	return ptr;
}
//@}

#if YCL_Win32
//! \since build 629
void
QueryFileTime(int fd, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime = {})
{
	platform_ex::QueryFileTime(platform_ex::UniqueHandle::pointer(
		::_get_osfhandle(fd)), p_ctime, p_atime, p_mtime);
}

//! \since build 631
//@{
string
ensure_str(const wstring& s)
{
	return platform_ex::WCSToMBCS(s);
}

template<typename _func>
auto
GetFileTimeOfImpl(_func f, const wstring& filename)
	-> ystdex::result_of_t<_func(const wchar_t*)>
{
	try
	{
		return f(filename.c_str());
	}
	CatchThrow(std::system_error& e, FileOperationFailure(e.code(),
		"Failed getting file time of \"" + ensure_str(filename) + "\"."))
	CatchThrow(..., FileOperationFailure(errno, std::generic_category(),
		"Failed getting file time of \"" + ensure_str(filename) + "\"."))
}

wstring
ensure_wstr(const char16_t* s)
{
	return wcast(s);
}
wstring
ensure_wstr(const char* s)
{
	return UTF8ToWCS(s);
}
//@}
#else
//! \since build 631
//@{
int
ensure_stat(int fd)
{
	return fd;
}
const char*
ensure_stat(const char* s)
{
	return s;
}
string
ensure_stat(const char16_t* s)
{
	return MakeMBCS(s);
}
const char*
ensure_stat(const string& s)
{
	return &s[0];
}

template<typename _fStat, typename _func, typename _tParam>
auto
FetchFileTimeStat(_fStat fs, _func f, _tParam arg)
	-> ystdex::result_of_t<_func(struct ::stat&)>
{
	// TODO: Get more precise time count.
	struct ::stat st;

	if(fs(ensure_stat(ensure_stat(arg)), &st) == 0)
		return f(st);
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed querying file time.");
}
//@}
#endif

//! \since build 631
template<typename _func>
auto
FetchFileTime(_func f, int desc)
#if YCL_Win32
	-> ystdex::result_of_t<_func(int)>
#else
	-> ystdex::result_of_t<_func(struct ::stat&)>
#endif
{
#if YCL_Win32
	// NOTE: The %::FILETIME has resolution of 100 nanoseconds.
	// XXX: Error handling for indirect calls.
	TryRet(f(desc))
	CatchThrow(std::system_error& e, FileOperationFailure(e.code(), std::string(
		"Failed querying file time: ") + e.what() + "."))
#else
	return FetchFileTimeStat(::fstat, f, desc);
#endif
}

#if YCL_Win32
//! \since build 631
//@{
const auto get_st_atime([](int fd){
	::FILETIME atime{};

	QueryFileTime(fd, {}, &atime);
	return platform_ex::ConvertTime(atime);
});
const auto get_st_mtime([](int fd){
	::FILETIME mtime{};

	QueryFileTime(fd, {}, {}, &mtime);
	return platform_ex::ConvertTime(mtime);
});
const auto get_st_matime([](int fd){
	::FILETIME mtime{}, atime{};

	QueryFileTime(fd, {}, &atime, &mtime);
	return array<FileTime, 2>{platform_ex::ConvertTime(mtime),
		platform_ex::ConvertTime(atime)};
});
//@}
#else
//! \since build 631
//@{
const auto get_st_atime([](struct ::stat& st){
	return FileTime(st.st_atime);
});
const auto get_st_mtime([](struct ::stat& st){
	return FileTime(st.st_mtime);
});
const auto get_st_matime([](struct ::stat& st){
	return array<FileTime, 2>{FileTime(st.st_mtime), FileTime(st.st_atime)};
});
//@}
#endif

//! \since build 631
template<typename _tChar>
FileTime
GetFileAccessTimeOfImpl(const _tChar* path)
{
#if YCL_Win32
	return GetFileTimeOfImpl([](const wchar_t* fn){
		::FILETIME atime{};

		platform_ex::QueryFileTime(fn, {}, &atime, {});
		return platform_ex::ConvertTime(atime);
	}, ensure_wstr(path));
#elif YCL_DS
	return FetchFileTimeStat(::stat, get_st_atime, Nonnull(path));
#else
	return FetchFileTimeStat(::lstat, get_st_atime, Nonnull(path));
#endif
}

//! \since build 628
template<typename _tChar>
FileTime
GetFileModificationTimeOfImpl(const _tChar* path)
{
#if YCL_Win32
	return GetFileTimeOfImpl([](const wchar_t* fn){
		::FILETIME mtime{};

		platform_ex::QueryFileTime(fn, {}, {}, &mtime);
		return platform_ex::ConvertTime(mtime);
	}, ensure_wstr(path));
#elif YCL_DS
	return FetchFileTimeStat(::stat, get_st_mtime, Nonnull(path));
#else
	return FetchFileTimeStat(::lstat, get_st_mtime, Nonnull(path));
#endif
}

//! \since build 631
template<typename _tChar>
array<FileTime, 2>
GetFileModificationAndAccessTimeOfImpl(const _tChar* path)
{
#if YCL_Win32
	return GetFileTimeOfImpl([](const wchar_t* fn){
		::FILETIME atime{}, mtime{};

		platform_ex::QueryFileTime(fn, {}, &atime, &mtime);
		return array<FileTime, 2>{platform_ex::ConvertTime(mtime),
			platform_ex::ConvertTime(atime)};
	}, ensure_wstr(path));
#elif YCL_DS
	return FetchFileTimeStat(::stat, get_st_matime, Nonnull(path));
#else
	return FetchFileTimeStat(::lstat, get_st_matime, Nonnull(path));
#endif
}

} // unnamed namespace;


void
FileDescriptor::Deleter::operator()(pointer p) ynothrow
{
	if(p)
		::close(*p);
}


FileDescriptor::FileDescriptor(std::FILE* fp) ynothrow
#if YCL_Win32
	: desc(fp ? ::_fileno(fp) : -1)
#else
	: desc(fp ? fileno(fp) : -1)
#endif
{}

FileTime
FileDescriptor::GetAccessTime() const
{
	return FetchFileTime(get_st_atime, desc);
}
FileTime
FileDescriptor::GetModificationTime() const
{
	return FetchFileTime(get_st_mtime, desc);
}
array<FileTime, 2>
FileDescriptor::GetModificationAndAccessTime() const
{
	return FetchFileTime(get_st_matime, desc);
}
std::uint64_t
FileDescriptor::GetSize() const
{
#if YCL_Win32
	::LARGE_INTEGER sz;

	// XXX: Error handling for indirect calls.
	if(::GetFileSizeEx(::HANDLE(::_get_osfhandle(desc)), &sz) != 0
		&& YB_LIKELY(sz.QuadPart >= 0))
		return std::uint64_t(sz.QuadPart);
	throw FileOperationFailure(platform_ex::GetErrnoFromWin32(),
		std::generic_category(), "Failed getting file size.");
#else
	struct ::stat st;

	if(::fstat(desc, &st) == 0)
		// TODO: Use YSLib::CheckNonnegativeScalar<std::uint64_t>?
		// XXX: No negative file size should be found. See also:
		//	http://stackoverflow.com/questions/12275831/why-is-the-st-size-field-in-struct-stat-signed . 
		return std::uint64_t(st.st_size);
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file size.");
#endif
}

bool
FileDescriptor::SetBlocking() const ynothrow
{
#if !YCL_Win32
	// NOTE: Read-modify-write operation is need for compatibility.
	//	See. http://pubs.opengroup.org/onlinepubs/9699919799/ .
	const int flags(::fcntl(desc, F_GETFL));

	if(flags != -1 && flags & O_NONBLOCK)
	{
		::fcntl(desc, F_SETFL, flags & ~O_NONBLOCK);
		return true;
	}
#endif
	return {};
}
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
bool
FileDescriptor::SetNonblocking() const ynothrow
{
#if !YCL_Win32
	// NOTE: Read-modify-write operation is need for compatibility.
	//	See. http://pubs.opengroup.org/onlinepubs/9699919799/ .
	const int flags(::fcntl(desc, F_GETFL));

	if(flags != -1 && !(flags & O_NONBLOCK))
	{
		::fcntl(desc, F_SETFL, flags | O_NONBLOCK);
		return true;
	}
#endif
	return {};
}
bool
FileDescriptor::SetSize(size_t size) ynothrow
{
#if YCL_Win32
	return ::_chsize(desc, long(size)) == 0;
#else
	// FIXME: It seems that on Android 32-bit size is always used even if
	//	'_FILE_OFFSET_BITS == 1'. Thus it is not safe and would better to be
	//	ignored slightly.
	return ::ftruncate(desc, ::off_t(size)) == 0;
#endif
}

size_t
FileDescriptor::FullRead(void* buf, size_t nbyte) ynothrowv
{
	using namespace std::placeholders;
	const auto p_buf(static_cast<char*>(buf));

	return size_t(FullReadWrite<0>(
		std::bind(&FileDescriptor::Read, this, _1, _2), p_buf, nbyte) - p_buf);
}

size_t
FileDescriptor::FullWrite(const void* buf, size_t nbyte) ynothrowv
{
	using namespace std::placeholders;
	const auto p_buf(static_cast<const char*>(buf));

	return size_t(FullReadWrite<ENOSPC>(std::bind(&FileDescriptor::Write, this,
		_1, _2), p_buf, nbyte) - p_buf);
}

size_t
FileDescriptor::Read(void* buf, size_t nbyte) ynothrowv
{
	return SafeReadWrite(::read, desc, buf, nbyte);
}

size_t
FileDescriptor::Write(const void* buf, size_t nbyte) ynothrowv
{
	return SafeReadWrite(::write, desc, buf, nbyte);
}


mode_t
GetDefaultPermissionMode() ynothrow
{
#if YCL_Win32
	// XXX: For compatibility with newer version of MSVCRT, no %_umask call
	//	would be considered. See https://msdn.microsoft.com/en-us/library/z0kc8e3z.aspx .
	return mode_t(Mode::UserReadWrite);
#else
	return mode_t(Mode::UserReadWrite | Mode::GroupReadWrite | Mode::OtherRead);
#endif
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
	// NOTE: However, on some implementations, '::close' and some other
	//	function calls may always cause file descriptor to be closed
	//	even if returning 'EINTR'. Thus it should be ignored. See https://www.python.org/dev/peps/pep-0475/#modified-functions .
	return RetryOnInterrupted([=]{
		return std::fclose(fp);
	});
}


int
omode_conv(std::ios_base::openmode mode)
{
	using namespace std;

	switch(unsigned((mode &= ~ios_base::ate) & ~ios_base::binary))
	{
	case ios_base::out:
		return O_CREAT | O_WRONLY;
	case ios_base::out | ios_base::trunc:
		return O_CREAT | O_WRONLY | O_TRUNC;
	case ios_base::out | ios_base::app:
	case ios_base::app:
		return O_CREAT | O_WRONLY | O_APPEND;
	case ios_base::in:
		return O_RDONLY;
	case ios_base::in | ios_base::out:
		return O_RDWR;
	case ios_base::in | ios_base::out | ios_base::trunc:
		return O_CREAT | O_RDWR | O_TRUNC;
	case ios_base::in | ios_base::out | ios_base::app:
	case ios_base::in | ios_base::app:
		return O_CREAT | O_RDWR | O_APPEND;
	default:
		return 0;
	}
}

int
omode_convb(std::ios_base::openmode mode)
{
#if YCL_Win32
	const int res(omode_conv(mode));

	return res | (mode & std::ios_base::binary ? _O_BINARY : _O_TEXT);
#else
	return omode_conv(mode);
#endif
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
	return ::_waccess(wcast(path), amode);
#else
	YCL_Impl_RetTryCatchAll(::access(MakeMBCS(path).c_str(), amode))
	return -1;
#endif
}

int
uopen(const char* filename, int oflag, mode_t pmode) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wopen(UTF8ToWCS(filename).c_str(), oflag,
		int(pmode)))
	return -1;
#else
	return ::open(filename, oflag, pmode);
#endif
}
int
uopen(const char16_t* filename, int oflag, mode_t pmode) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	return ::_wopen(wcast(filename), oflag, int(pmode));
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
	return ::_wfopen(wcast(filename),
		wcast(mode));
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
		YCL_Impl_RetTryCatchAll(::_wfopen(wcast(
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
	return ::_wpopen(wcast(filename), wcast(mode));
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
		return ucast(::_wgetcwd(wcast(buf), int(size)));
#else
		if(const auto cwd
			= ::getcwd(ystdex::aligned_store_cast<char*>(buf), size))
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
	return ::mkdir(path, mode_t(Mode::Access)) == 0;
}
#endif

YCL_Impl_FileSystem_ufunc(urmdir, ::rmdir, ::_wrmdir)

YCL_Impl_FileSystem_ufunc(uunlink, ::unlink, ::_wunlink)

YCL_Impl_FileSystem_ufunc(uremove, std::remove, ::_wremove)

#undef YCL_Impl_FileSystem_ufunc_1
#undef YCL_Impl_FileSystem_ufunc_2
#undef YCL_Impl_FileSystem_ufunc


ImplDeDtor(FileOperationFailure)


FileTime
GetFileAccessTimeOf(const char* filename)
{
	return GetFileAccessTimeOfImpl(filename);
}
FileTime
GetFileAccessTimeOf(const char16_t* filename)
{
	return GetFileAccessTimeOfImpl(filename);
}

FileTime
GetFileModificationTimeOf(const char* filename)
{
	return GetFileModificationTimeOfImpl(filename);
}
FileTime
GetFileModificationTimeOf(const char16_t* filename)
{
	return GetFileModificationTimeOfImpl(filename);
}

array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const char* filename)
{
	return GetFileModificationAndAccessTimeOfImpl(filename);
}
array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const char16_t* filename)
{
	return GetFileModificationAndAccessTimeOfImpl(filename);
}

std::uint64_t
GetFileSizeOf(std::FILE* fp)
{
	return FileDescriptor(fp).GetSize();
}

} // namespace platform;

