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
\version r973
\author FrankHB <frankhb1989@gmail.com>
\since build 615
\par 创建时间:
	2015-07-14 18:53:12 +0800
\par 修改时间:
	2015-09-03 16:00 +0800
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
#endif
}

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

//! \since build 629
template<typename _tChar>
FileTime
GetFileModificationTimeOfImpl(const _tChar* filename)
{
	try
	{
		::FILETIME mtime{};

		platform_ex::QueryFileTime(reinterpret_cast<const ystdex::conditional_t<
			std::is_same<_tChar, char>::value, char, wchar_t>*>(filename), {},
			{}, &mtime);
		return platform_ex::ConvertTime(mtime);
	}
	CatchThrow(std::system_error& e, FileOperationFailure(e.code(),
		"Failed getting file time of \"" + ensure_str(filename) + "\"."))
	CatchThrow(..., FileOperationFailure(errno, std::generic_category(),
		"Failed getting file time of \"" + ensure_str(filename) + "\"."))
}
#else
//! \since build 628
template<typename _tChar>
FileTime
GetFileModificationTimeOfImpl(const _tChar* filename)
{
	if(const UniqueFile p{platform::uopen(filename, O_RDONLY)})
		return p->GetModificationTime();
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file time of \"" + ensure_str(filename) + "\".");
}
#endif

//! \since build 628
template<typename _func>
auto
#if YCL_Win32
FetchFileTime(_func f)
	-> ystdex::result_of_t<_func()>
#else
FetchFileTime(_func f, int desc)
	-> ystdex::result_of_t<_func(struct ::stat&)>
#endif
{
#if YCL_Win32
	// NOTE: The %::FILETIME has resolution of 100 nanoseconds.
	// XXX: Error handling for indirect calls.
	TryRet(f())
	CatchThrow(std::system_error& e, FileOperationFailure(e.code(), std::string(
		"Failed querying file time: ") + e.what() + "."))
#else
	// TODO: Get more precise time count.
	struct ::stat st;

	if(::fstat(desc, &st) == 0)
		return f(st);
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed querying file time.");
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
#if YCL_Win32
	::FILETIME atime{};

	return FetchFileTime([&]{
		QueryFileTime(desc, {}, &atime);
		return platform_ex::ConvertTime(atime);
	});
#else
	return FetchFileTime([&](struct ::stat& st){
		return std::chrono::seconds(st.st_atime); 
	}, desc);
#endif
}
FileTime
FileDescriptor::GetModificationTime() const
{
#if YCL_Win32
	::FILETIME mtime{};

	return FetchFileTime([&]{
		QueryFileTime(desc, {}, {}, &mtime);
		return platform_ex::ConvertTime(mtime);
	});
#else
	return FetchFileTime([&](struct ::stat& st){
		return std::chrono::seconds(st.st_mtime); 
	}, desc);
#endif

}
array<FileTime, 2>
FileDescriptor::GetModificationAndAccessTime() const
{
#if YCL_Win32
	::FILETIME mtime{}, atime{};

	return FetchFileTime([&]{
		QueryFileTime(desc, {}, &atime, &mtime);
		return array<FileTime, 2>{platform_ex::ConvertTime(mtime),
			platform_ex::ConvertTime(atime)};
	});
#else
	return FetchFileTime([&](struct ::stat& st){
		return array<FileTime, 2>{std::chrono::seconds(st.st_mtime),
			std::chrono::seconds(st.st_atime)};
	}, desc);
#endif

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
	return ::_waccess(reinterpret_cast<const wchar_t*>(path), amode);
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
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag,
		int(pmode));
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
GetFileModificationTimeOf(std::FILE* fp)
{
	return FileDescriptor(fp).GetModificationTime();
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

std::uint64_t
GetFileSizeOf(std::FILE* fp)
{
	return FileDescriptor(fp).GetSize();
}

} // namespace platform;

