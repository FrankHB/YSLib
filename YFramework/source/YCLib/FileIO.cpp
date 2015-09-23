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
\version r1630
\author FrankHB <frankhb1989@gmail.com>
\since build 615
\par 创建时间:
	2015-07-14 18:53:12 +0800
\par 修改时间:
	2015-09-23 22:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileIO
*/


#undef __STRICT_ANSI__ // for fileno, ::pclose, ::popen for platform DS;
//	_fileno, _wfopen for MinGW.org;
#include "YCLib/YModules.h"
#include YFM_YCLib_FileIO
#include YFM_YCLib_Debug // for Nonnull, ystdex::temporary_buffer;
#include YFM_YCLib_Reference // for unique_ptr;
#include YFM_YCLib_NativeAPI // for std::is_same, ystdex::underlying_type_t,
//	Mode, struct ::stat, ::fstat, ::stat, ::lstat, ::close, ::fcntl, F_GETFL,
//	O_*, ::fchmod, ::_chsize, ::ftruncate, ::setmode, ::_wgetcwd, ::getcwd,
//	!defined(__STRICT_ANSI__) API;
#include <ystdex/streambuf.hpp> // for ystdex::streambuf_equal;
#if YCL_DS
#	include "CHRLib/YModules.h"
#	include YFM_CHRLib_CharacterProcessing

//! \since build 475
using namespace CHRLib;
#elif YCL_Win32
#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// At least one headers of <stdlib.h>, <stdio.h>, <Windows.h>, <Windef.h>
//	(and probably more) should have been included to make the MinGW-W64 macro
//	available if it is really being used.
#		undef _fileno
#	endif
#	include YFM_Win32_YCLib_MinGW32 // for platform_ex::QueryFileTime,
//	platform_ex::UniqueHandle, platform::WCSToMBCS, platform_ex::UTF8ToWCS,
//	platform_ex::ConvertTime, platform_ex::GetErrnoFromWin32;

//! \since build 632
//@{
using platform_ex::QueryFileTime;
using platform_ex::UniqueHandle;
using platform_ex::WCSToMBCS;
//! \since build 540
using platform_ex::UTF8ToWCS;
using platform_ex::ConvertTime;
//@}
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

//! \since build 637
template<typename _tParam>
YB_NORETURN void
ThrowFileOperationFailure(_tParam&& arg)
{
	ystdex::throw_error<FileOperationFailure>(errno, yforward(arg));
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
//! \since build 632
//@{
void
QueryFileTime(int fd, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime)
{
	QueryFileTime(platform_ex::UniqueHandle::pointer(
		::_get_osfhandle(fd)), p_ctime, p_atime, p_mtime);
}

// TODO: Use ISO C++14 generic lambda expressions.
yconstexpr const struct
{
	template<typename _tParam, typename... _tParams>
	FileTime
	operator()(_tParam arg, _tParams... args) const
	{
		::FILETIME atime{};

		QueryFileTime(arg, {}, &atime, {}, args...);
		return ConvertTime(atime);
	}
} get_st_atime;
yconstexpr const struct
{
	template<typename _tParam, typename... _tParams>
	FileTime
	operator()(_tParam arg, _tParams... args) const
	{
		::FILETIME mtime{};

		QueryFileTime(arg, {}, {}, &mtime, args...);
		return ConvertTime(mtime);
	}
} get_st_mtime;
yconstexpr const struct
{
	template<typename _tParam, typename... _tParams>
	array<FileTime, 2>
	operator()(_tParam arg, _tParams... args) const
	{
		::FILETIME mtime{}, atime{};

		QueryFileTime(arg, {}, &atime, &mtime, args...);
		return array<FileTime, 2>{ConvertTime(mtime), ConvertTime(atime)};
	}
} get_st_matime;
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

//! \since build 632
//@{
YB_NONNULL(2) int
estat(struct ::stat& st, const char* path, bool follow_link)
{
#	if YCL_DS
	yunused(follow_link);
	return ::stat(Nonnull(path), &st);
#	else
	return (follow_link ? ::stat : ::lstat)(Nonnull(path), &st);
#	endif
}
YB_NONNULL(2) int
estat(struct ::stat& st, const char16_t* path, bool follow_link)
{
	return estat(st, MakeMBCS(path).c_str(), follow_link);
}
#endif
int
estat(struct ::stat& st, int fd)
{
	return ::fstat(fd, &st);
}
//@}

//! \since build 632
template<typename _func, typename... _tParams>
auto
FetchFileTime(_func f, _tParams... args)
#if YCL_Win32
	-> ystdex::result_of_t<_func(_tParams&...)>
#else
	-> ystdex::result_of_t<_func(struct ::stat&)>
#endif
{
#if YCL_Win32
	// NOTE: The %::FILETIME has resolution of 100 nanoseconds.
	// XXX: Error handling for indirect calls.
	TryRet(f(args...))
	CatchThrow(std::system_error& e, FileOperationFailure(e.code(),
		std::string("Failed querying file time: ") + e.what() + "."))
#else
	// TODO: Get more precise time count.
	struct ::stat st;

	if(estat(st, args...) == 0)
		return f(st);
	ThrowFileOperationFailure("Failed querying file time.");
#endif
}

//! \since build 637
template<typename _type>
void
CopyFileForSource(_type&& ofile, const char* src)
{
#if YCL_Win32
	if(UniqueFile p_ifile{uopen(src, O_RDONLY | O_BINARY)})
#else
	if(UniqueFile p_ifile{uopen(src, O_RDONLY)})
#endif
		FileDescriptor::WriteContent(yforward(ofile), p_ifile.get());
	ThrowFileOperationFailure(
		"Failed opening source file '" + string(src) +"'.");
}

} // unnamed namespace;


string
MakePathString(const char16_t* s)
{
#if YCL_Win32
	return platform_ex::WCSToUTF8(wcast(s));
#else
	return MakeMBCS(s);
#endif
}


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
int
FileDescriptor::GetFlags() const ynothrow
{
#if YCL_API_POSIXFileSystem
	return ::fcntl(desc, F_GETFL);
#else
	errno = ENOSYS;
	return -1;
#endif
}
mode_t
FileDescriptor::GetMode() const ynothrow
{
	struct ::stat st;

	return estat(st, desc) == 0 ? st.st_mode : 0;
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
	ThrowFileOperationFailure("Failed getting file size.");
#endif
}

bool
FileDescriptor::SetBlocking() const ynothrow
{
#if YCL_API_POSIXFileSystem
	// NOTE: Read-modify-write operation is need for compatibility.
	//	See http://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html .
	const int flags(GetFlags());

	if(flags != -1 && flags & O_NONBLOCK)
		return SetFlags(flags & ~O_NONBLOCK);
#else
	errno = ENOSYS;
#endif
	return {};
}
bool
FileDescriptor::SetFlags(int flags) const ynothrow
{
#if YCL_API_POSIXFileSystem
	return ::fcntl(desc, F_SETFL, flags) != -1;
#else
	// TODO: Try using NT6 %::SetFileInformationByHandle for Win32.
	yunused(flags);
	errno = ENOSYS;
	return {};
#endif
}
bool
FileDescriptor::SetMode(mode_t mode) const ynothrow
{
#if YCL_API_POSIXFileSystem
	return ::fchmod(desc, mode) != -1;
#else
	yunused(mode);
	errno = ENOSYS;
	return {};
#endif
}
bool
FileDescriptor::SetNonblocking() const ynothrow
{
#if YCL_API_POSIXFileSystem
	// NOTE: Read-modify-write operation is need for compatibility.
	//	See http://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html .
	const int flags(GetFlags());

	if(flags != -1 && !(flags & O_NONBLOCK))
		return SetFlags(flags | O_NONBLOCK);
#else
	errno = ENOSYS;
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
int
FileDescriptor::SetTranslationMode(int mode) const ynothrow
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

void
FileDescriptor::WriteContent(FileDescriptor ofd, FileDescriptor ifd,
	byte* buf, size_t size)
{
	YAssertNonnull(ifd),
	YAssertNonnull(ofd),
	YAssertNonnull(buf),
	YAssert(size != 0, "Invalid size found.");

	ystdex::retry_on_cond([&](size_t len){
		if(len == size_t(-1))
			ThrowFileOperationFailure(
				"Failed reading source file '" + to_string(*ifd) +"'.");
		if(ofd.FullWrite(buf, len) == size_t(-1))
			ThrowFileOperationFailure(
				"Failed writing destination file '" + to_string(*ofd) +"'.");
		return len != 0;
#if YB_IMPL_GNUCPP < 50000 && !defined(NDEBUG)
	// TODO: Use newer G++ to get away with the workaround.
	}, [&]{
		return ifd.Read(buf, size);
	});
#else
	}, &FileDescriptor::Read, &ifd, buf, size);
#endif
}
void
FileDescriptor::WriteContent(FileDescriptor ofd, FileDescriptor ifd,
	size_t size)
{
	ystdex::temporary_buffer<byte> buf(size);

	WriteContent(ofd, ifd, buf.get().get(), buf.size());
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
	FileDescriptor(Nonnull(stream)).SetTranslationMode(_O_BINARY);
#else
	// NOTE: No effect.
	Nonnull(stream);
#endif
}

int
RetryClose(std::FILE* fp) ynothrow
{
	// NOTE: However, on some implementations, '::close' and some other
	//	function calls may always cause file descriptor to be closed
	//	even if returning 'EINTR'. Thus it should be ignored. See https://www.python.org/dev/peps/pep-0475/#modified-functions .
	return RetryOnInterrupted([=]{
		return std::fclose(Nonnull(fp));
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
GetFileAccessTimeOf(const char* filename, bool follow_link)
{
	return FetchFileTime(get_st_atime, filename, follow_link);
}
FileTime
GetFileAccessTimeOf(const char16_t* filename, bool follow_link)
{
	return FetchFileTime(get_st_atime, wcast(filename), follow_link);
}

FileTime
GetFileModificationTimeOf(const char* filename, bool follow_link)
{
	return FetchFileTime(get_st_mtime, filename, follow_link);
}
FileTime
GetFileModificationTimeOf(const char16_t* filename, bool follow_link)
{
	return FetchFileTime(get_st_mtime, wcast(filename), follow_link);
}

array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const char* filename, bool follow_link)
{
	return FetchFileTime(get_st_matime, filename, follow_link);
}
array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const char16_t* filename, bool follow_link)
{
	return FetchFileTime(get_st_matime, wcast(filename), follow_link);
}

std::uint64_t
GetFileSizeOf(std::FILE* fp)
{
	return FileDescriptor(fp).GetSize();
}


void
CopyFile(UniqueFile p_ofile, FileDescriptor ifd)
{
	FileDescriptor::WriteContent(Nonnull(p_ofile.get()), Nonnull(ifd));
}
void
CopyFile(UniqueFile p_ofile, const char* src)
{
	CopyFileForSource(Nonnull(p_ofile.get()), src);
}
void
CopyFile(const char* dst, FileDescriptor ifd, mode_t mode)
{
	FileDescriptor::WriteContent(EnsureUniqueFile(dst, mode).get(),
		Nonnull(ifd));
}
void
CopyFile(const char* dst, const char* src, mode_t mode)
{
	CopyFileForSource(EnsureUniqueFile(dst, mode).get(), src);
}

UniqueFile
EnsureUniqueFile(const char* dst, mode_t mode)
{
	TryUnlink(dst);
	if(UniqueFile p_file{uopen(dst, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL
#if YCL_Win32
		| O_BINARY
#endif
		, mode & mode_t(Mode::User))})
		return p_file;
	ThrowFileOperationFailure("Failed creating file '" + string(dst) +"'.");
}

bool
HaveSameContents(const char* path_a, const char* path_b)
{
	filebuf fb_a, fb_b;

	// FIXME: Check %st_ino.
	errno = 0;
	if(!fb_a.open(Nonnull(path_a), std::ios_base::in | std::ios_base::binary))
		ThrowFileOperationFailure("Failed opening first file '" + string(path_a)
			+"'.");
	if(!fb_b.open(Nonnull(path_b), std::ios_base::in | std::ios_base::binary))
		ThrowFileOperationFailure("Failed opening second file '"
			+ string(path_b) +"'.");
	return ystdex::streambuf_equal(fb_a, fb_b);
}
bool
HaveSameContents(UniqueFile p_a, UniqueFile p_b)
{
	filebuf fb_a, fb_b;

	// FIXME: Check %st_ino.
	errno = 0;
	// FIXME: Implement for streams without open-by-raw-file extension.
	if(!fb_a.open(std::move(p_a), std::ios_base::in | std::ios_base::binary))
		ThrowFileOperationFailure("Failed opening first file.");
	if(!fb_b.open(std::move(p_b), std::ios_base::in | std::ios_base::binary))
		ThrowFileOperationFailure("Failed opening second file.");
	return ystdex::streambuf_equal(fb_a, fb_b);
}

} // namespace platform;

