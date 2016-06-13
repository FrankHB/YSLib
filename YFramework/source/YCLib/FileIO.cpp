/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileIO.cpp
\ingroup YCLib
\brief 平台相关的文件访问和输入/输出接口。
\version r2313
\author FrankHB <frankhb1989@gmail.com>
\since build 615
\par 创建时间:
	2015-07-14 18:53:12 +0800
\par 修改时间:
	2016-06-14 02:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileIO
*/


#undef __STRICT_ANSI__ // for fileno, ::pclose, ::popen for POSIX platforms;
//	_fileno, _wfopen for MinGW.org;
#include "YCLib/YModules.h"
#include YFM_YCLib_FileIO
#include YFM_YCLib_Debug // for Nonnull, ystdex::throw_error,
//	ystdex::temporary_buffer;
#include YFM_YCLib_Reference // for unique_ptr;
#include YFM_YCLib_NativeAPI // for std::is_same, ystdex::underlying_type_t,
//	Mode, ::HANDLE, struct ::stat, ::fstat, ::stat, ::lstat, ::close, ::fcntl,
//	F_GETFL, _O_*, O_*, ::fchmod, ::_chsize, ::ftruncate, ::setmode, ::_wgetcwd,
//	::getcwd, !defined(__STRICT_ANSI__) API, platform_ex::futimens;
#include <ystdex/functional.hpp> // for ystdex::compose, ystdex::addrof;
#include <ystdex/streambuf.hpp> // for ystdex::streambuf_equal;
#if YCL_DS
#	include "CHRLib/YModules.h"
#	include YFM_CHRLib_CharacterProcessing // for CHRLib::MakeMBCS;

//! \since build 475
using namespace CHRLib;
#elif YCL_Win32
#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// At least one headers of <stdlib.h>, <stdio.h>, <Windows.h>, <Windef.h>
//	(and probably more) should have been included to make the MinGW-W64 macro
//	available if it is really being used.
#		undef _fileno
#	endif
#	include YFM_Win32_YCLib_MinGW32 // for platform_ex::FileAttributes,
//	platform_ex::GetErrnoFromWin32, platform_ex::QueryFileLinks,
//	platform_ex::QueryFileNodeID, platform_ex::QueryFileTime,
//	platform_ex::WCSToUTF8, platform_ex::UTF8ToWCS, platform_ex::ConvertTime,
//	platform_ex::SetFileTime;

//! \since build 639
using platform_ex::FileAttributes;
using platform_ex::GetErrnoFromWin32;
//! \since build 639
using platform_ex::QueryFileLinks;
//! \since build 632
//@{
using platform_ex::QueryFileNodeID;
using platform_ex::QueryFileTime;
//! \since build 540
using platform_ex::UTF8ToWCS;
//! \since build 639
using platform_ex::WCSToUTF8;
using platform_ex::ConvertTime;
//@}
#elif YCL_API_POSIXFileSystem
#	include "CHRLib/YModules.h"
#	include YFM_CHRLib_CharacterProcessing // for MakeMBCS;
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

// XXX: Catch %std::bad_alloc?
#define YCL_Impl_RetTryCatchAll(...) \
	TryRet(__VA_ARGS__) \
	CatchExpr(std::bad_alloc&, errno = ENOMEM) \
	CatchIgnore(...)

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
#if YCL_Win64
SafeReadWrite(_func f, int fd, _tBuf buf, unsigned nbyte) ynothrowv
#else
SafeReadWrite(_func f, int fd, _tBuf buf, size_t nbyte) ynothrowv
#endif
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
//! \since build 651
inline PDefH(::HANDLE, ToHandle, int fd) ynothrow
	ImplRet(::HANDLE(::_get_osfhandle(fd)))

//! \since build 660
void
QueryFileTime(const char* path, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime, bool follow_reparse_point)
{
	QueryFileTime(UTF8ToWCS(path).c_str(), p_ctime, p_atime, p_mtime,
		follow_reparse_point);
}
//! \since build 632
//@{
void
QueryFileTime(int fd, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime)
{
	QueryFileTime(ToHandle(fd), p_ctime, p_atime, p_mtime);
}

// TODO: Blocked. Use C++14 generic lambda expressions.
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
} get_st_atime{};
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
} get_st_mtime{};
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
} get_st_matime{};
//@}

//! \since build 639
//@{
YB_NONNULL(1) bool
UnlinkWithAttr(const wchar_t* path, FileAttributes attr) ynothrow
{
	return !(attr & FileAttributes::ReadOnly) || ::SetFileAttributesW(path,
		attr & ~FileAttributes::ReadOnly) ? ::_wunlink(path) == 0
		: (errno = GetErrnoFromWin32(), false);
}

template<typename _func>
YB_NONNULL(2) bool
CallFuncWithAttr(_func f, const char* path)
{
	const auto& wpath(UTF8ToWCS(path));
	const auto& wstr(wpath.c_str());
	const auto attr(FileAttributes(::GetFileAttributesW(wstr)));

	return attr != platform_ex::Invalid ? f(wstr, attr)
		: (errno = GetErrnoFromWin32(), false);
}
//@}
#else
//! \since build 651
inline PDefH(::timespec, ToTimeSpec, FileTime ft) ynothrow
	ImplRet({std::time_t(ft.count() / 1000000000LL),
		long(ft.count() % 1000000000LL)})

//! \since build 651
YB_NONNULL(2) void
TrySetFileTime(int fd, const ::timespec* times)
{
#if YCL_DS
	// XXX: Hack.
#ifndef UTIME_OMIT
#	define UTIME_OMIT (-1L)
#endif
	yunused(fd), yunused(times);
	ystdex::throw_error(std::errc::function_not_supported);
#else
	if(::futimens(fd, times) != 0)
		ThrowFileOperationFailure("Failed setting file time.");
#endif
}

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

//! \since build 638
//@{
static_assert(std::is_integral<::dev_t>(),
	"Nonconforming '::dev_t' type found.");
static_assert(std::is_unsigned<::ino_t>(),
	"Nonconforming '::ino_t' type found.");

inline PDefH(FileNodeID, get_file_node_id, struct ::stat& st) ynothrow
	ImplRet({std::uint64_t(st.st_dev), std::uint64_t(st.st_ino)})
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
		std::string("Failed querying file time: ") + e.what()))
#else
	// TODO: Get more precise time count.
	struct ::stat st;

	if(estat(st, args...) == 0)
		return f(st);
	ThrowFileOperationFailure("Failed querying file time.");
#endif
}

//! \since build 660
bool
IsNodeShared_Impl(const char* a, const char* b, bool follow_link) ynothrow
{
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(QueryFileNodeID(UTF8ToWCS(a).c_str(), follow_link)
		== QueryFileNodeID(UTF8ToWCS(b).c_str(), follow_link))
	return {};
#else
	struct ::stat st;

	if(estat(st, a, follow_link) != 0)
		return {};

	const auto id(get_file_node_id(st));

	if(estat(st, b, follow_link) != 0)
		return {};

	return IsNodeShared(id, get_file_node_id(st));
#endif
}
//! \since build 660
bool
IsNodeShared_Impl(const char16_t* a, const char16_t* b, bool follow_link) ynothrow
{
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(QueryFileNodeID(wcast(a), follow_link)
		== QueryFileNodeID(wcast(b), follow_link))
	return {};
#else
	return IsNodeShared_Impl(MakeMBCS(a).c_str(), MakeMBCS(b).c_str(), follow_link);
#endif
}

//! \since build 701
template<typename _tChar>
YB_NONNULL(1, 2) pair<UniqueFile, UniqueFile>
HaveSameContents_Impl(const _tChar* path_a, const _tChar* path_b, mode_t mode)
{
	if(UniqueFile p_a{uopen(path_a,
#if YCL_Win32
		_O_RDONLY | _O_BINARY
#else
		O_RDONLY
#endif
		, mode)})
		if(UniqueFile p_b{uopen(path_b,
#if YCL_Win32
			_O_RDONLY | _O_BINARY
#else
			O_RDONLY
#endif
		, mode)})
			return {std::move(p_a), std::move(p_b)};
	return {};
}

} // unnamed namespace;


string
MakePathString(const char16_t* s)
{
#if YCL_Win32
	return WCSToUTF8(wcast(s));
#else
	return MakeMBCS(s);
#endif
}


NodeCategory
CategorizeNode(mode_t st_mode) ynothrow
{
	auto res(NodeCategory::Empty);
	const auto m(Mode(st_mode) & Mode::FileType);

	if((m & Mode::Directory) == Mode::Directory)
		res |= NodeCategory::Directory;
#if !YCL_Win32
	if((m & Mode::Link) == Mode::Link)
		res |= NodeCategory::Link;
#endif
	if((m & Mode::Regular) == Mode::Regular)
		res |= NodeCategory::Regular;
	if(YB_UNLIKELY((m & Mode::Character) == Mode::Character))
		res |= NodeCategory::Character;
#if !YCL_Win32
	else if(YB_UNLIKELY((m & Mode::Block) == Mode::Block))
		res |= NodeCategory::Block;
#endif
	if(YB_UNLIKELY((m & Mode::FIFO) == Mode::FIFO))
		res |= NodeCategory::FIFO;
#if !YCL_Win32
	if((m & Mode::Socket) == Mode::Socket)
		res |= NodeCategory::Socket;
#endif
	return res;
}


void
FileDescriptor::Deleter::operator()(pointer p) const ynothrow
{
	if(p)
		// NOTE: Error is ignored.
		//	See $2016-03 @ %Documentation::Workflow::Annual2016.
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
NodeCategory
FileDescriptor::GetCategory() const ynothrow
{
#if YCL_Win32
	return platform_ex::CategorizeNode(ToHandle(desc));
#else
	struct ::stat st;

	return estat(st, desc) == 0 ? CategorizeNode(st.st_mode)
		: NodeCategory::Invalid;
#endif
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
FileNodeID
FileDescriptor::GetNodeID() const ynothrow
{
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(QueryFileNodeID(ToHandle(desc)))
#else
	struct ::stat st;

	if(estat(st, desc) == 0)
		return get_file_node_id(st);
#endif
	return FileNodeID();
}
size_t
FileDescriptor::GetNumberOfLinks() const ynothrow
{
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(QueryFileLinks(ToHandle(desc)))
#else
	struct ::stat st;
	static_assert(std::is_unsigned<decltype(st.st_nlink)>(),
		"Unsupported '::nlink_t' type found.");

	if(estat(st, desc) == 0)
		return size_t(st.st_nlink);
#endif
	return size_t();
}
std::uint64_t
FileDescriptor::GetSize() const
{
#if YCL_Win32
	::LARGE_INTEGER sz;

	// XXX: Error handling for indirect calls.
	if(::GetFileSizeEx(ToHandle(desc), &sz) != 0 && YB_LIKELY(sz.QuadPart >= 0))
		return std::uint64_t(sz.QuadPart);
	ystdex::throw_error<FileOperationFailure>(GetErrnoFromWin32(),
		"Failed getting file size.");
#else
	struct ::stat st;

	if(estat(st, desc) == 0)
		// TODO: Use YSLib::CheckNonnegativeScalar<std::uint64_t>?
		// XXX: No negative file size should be found. See also:
		//	http://stackoverflow.com/questions/12275831/why-is-the-st-size-field-in-struct-stat-signed.
		return std::uint64_t(st.st_size);
	ThrowFileOperationFailure("Failed getting file size.");
#endif
}

void
FileDescriptor::SetAccessTime(FileTime ft) const
{
#if YCL_Win32
	auto atime(ConvertTime(ft));

	platform_ex::SetFileTime(ToHandle(desc), {}, &atime, {});
#else
	const ::timespec times[]{ToTimeSpec(ft), {yimpl(0), UTIME_OMIT}};

	TrySetFileTime(desc, times);
#endif
}
bool
FileDescriptor::SetBlocking() const ynothrow
{
#if YCL_API_POSIXFileSystem
	// NOTE: Read-modify-write operation is need for compatibility.
	//	See http://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html.
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
void
FileDescriptor::SetModificationTime(FileTime ft) const
{
#if YCL_Win32
	auto mtime(ConvertTime(ft));

	platform_ex::SetFileTime(ToHandle(desc), {}, {}, &mtime);
#else
	const ::timespec times[]{{yimpl(0), UTIME_OMIT}, ToTimeSpec(ft)};

	TrySetFileTime(desc, times);
#endif
}
void
FileDescriptor::SetModificationAndAccessTime(array<FileTime, 2> fts) const
{
#if YCL_Win32
	auto atime(ConvertTime(fts[0])), mtime(ConvertTime(fts[1]));

	platform_ex::SetFileTime(ToHandle(desc), {}, &atime, &mtime);
#else
	const ::timespec times[]{ToTimeSpec(fts[0]), ToTimeSpec(fts[1])};

	TrySetFileTime(desc, times);
#endif
}
bool
FileDescriptor::SetNonblocking() const ynothrow
{
#if YCL_API_POSIXFileSystem
	// NOTE: Read-modify-write operation is need for compatibility.
	//	See http://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html.
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
#if YCL_Win64
	return SafeReadWrite(::read, desc, buf, unsigned(nbyte));
#else
	return SafeReadWrite(::read, desc, buf, nbyte);
#endif
}

size_t
FileDescriptor::Write(const void* buf, size_t nbyte) ynothrowv
{
#if YCL_Win64
	return SafeReadWrite(::write, desc, buf, unsigned(nbyte));
#else
	return SafeReadWrite(::write, desc, buf, nbyte);
#endif
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
				"Failed reading source file '" + to_string(*ifd) + "'.");
		if(ofd.FullWrite(buf, len) == size_t(-1))
			ThrowFileOperationFailure(
				"Failed writing destination file '" + to_string(*ofd) + "'.");
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
DefaultPMode() ynothrow
{
#if YCL_Win32
	// XXX: For compatibility with newer version of MSVCRT, no %_umask call
	//	would be considered. See https://msdn.microsoft.com/en-us/library/z0kc8e3z.aspx.
	return mode_t(Mode::UserReadWrite);
#else
	return mode_t(Mode::UserReadWrite | Mode::GroupReadWrite | Mode::OtherRead);
#endif
}

void
SetBinaryIO(std::FILE* stream) ynothrowv
{
#if YCL_Win32
	FileDescriptor(Nonnull(stream)).SetTranslationMode(_O_BINARY);
#else
	// NOTE: No effect.
	Nonnull(stream);
#endif
}


int
omode_conv(std::ios_base::openmode mode) ynothrow
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
omode_convb(std::ios_base::openmode mode) ynothrow
{
#if YCL_Win32
	const int res(omode_conv(mode));

	return res | (mode & std::ios_base::binary ? _O_BINARY : _O_TEXT);
#else
	return omode_conv(mode);
#endif
}


int
uaccess(const char* path, int amode) ynothrowv
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
uaccess(const char16_t* path, int amode) ynothrowv
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
uopen(const char* filename, int oflag, mode_t pmode) ynothrowv
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
uopen(const char16_t* filename, int oflag, mode_t pmode) ynothrowv
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
ufopen(const char* filename, const char* mode) ynothrowv
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
ufopen(const char16_t* filename, const char16_t* mode) ynothrowv
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	return ::_wfopen(wcast(filename), wcast(mode));
#else
	YCL_Impl_RetTryCatchAll(std::fopen(MakeMBCS(filename).c_str(),
		MakeMBCS(mode).c_str()))
	return {};
#endif
}
std::FILE*
ufopen(const char* filename, std::ios_base::openmode mode) ynothrowv
{
	if(const auto c_mode = ystdex::openmode_conv(mode))
		return ufopen(filename, c_mode);
	return {};
}
std::FILE*
ufopen(const char16_t* filename, std::ios_base::openmode mode) ynothrowv
{
	YAssertNonnull(filename);
	if(const auto c_mode = ystdex::openmode_conv(mode))
#if YCL_Win32
		YCL_Impl_RetTryCatchAll(
			::_wfopen(wcast(filename), UTF8ToWCS(c_mode).c_str()))
#else
		YCL_Impl_RetTryCatchAll(std::fopen(MakeMBCS(filename).c_str(), c_mode))
#endif
	return {};
}

bool
ufexists(const char* filename) ynothrowv
{
#if YCL_Win32
	return ystdex::call_value_or(ystdex::compose(std::fclose, ystdex::addrof<>()),
		ufopen(filename, "rb"), yimpl(1)) == 0;
#else
	return ystdex::fexists(filename);
#endif
}
bool
ufexists(const char16_t* filename) ynothrowv
{
	return ystdex::call_value_or(ystdex::compose(std::fclose, ystdex::addrof<>()),
		ufopen(filename, u"rb"), yimpl(1)) == 0;
}

int
upclose(std::FILE* fp) ynothrowv
{
	YAssertNonnull(fp);
#if YCL_DS
	errno = ENOSYS;
	return -1;
#elif YCL_Win32
	return ::_pclose(fp);
#else
	return ::pclose(fp);
#endif
}

std::FILE*
upopen(const char* filename, const char* mode) ynothrowv
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_DS
	errno = ENOSYS;
	return {};
#elif YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wpopen(UTF8ToWCS(filename).c_str(),
		UTF8ToWCS(mode).c_str()))
	return {};
#else
	return ::popen(filename, mode);
#endif
}
std::FILE*
upopen(const char16_t* filename, const char16_t* mode) ynothrowv
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_DS
	errno = ENOSYS;
	return {};
#elif YCL_Win32
	return ::_wpopen(wcast(filename), wcast(mode));
#else
	YCL_Impl_RetTryCatchAll(::popen(MakeMBCS(filename).c_str(),
		MakeMBCS(mode).c_str()))
	return {};
#endif
}

char*
ugetcwd(char* buf, size_t size) ynothrowv
{
	YAssertNonnull(buf);
	if(size != 0)
	{
		using namespace std;

#if YCL_Win32
		try
		{
			const auto p(make_unique<wchar_t[]>(size));

			if(const auto cwd = ::_wgetcwd(p.get(), int(size)))
			{
				const auto res(WCSToUTF8(cwd));
				const auto len(res.length());

				if(size < len + 1)
					errno = ERANGE;
				else
					return ystdex::ntctscpy(buf, res.data(), len);
			}
		}
		CatchExpr(std::bad_alloc&, errno = ENOMEM);
#else
		return ucast(::getcwd(wcast(buf), size));
#endif
	}
	else
		errno = EINVAL;
	return {};
}
char16_t*
ugetcwd(char16_t* buf, size_t size) ynothrowv
{
	YAssertNonnull(buf);
	if(size != 0)
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
	else
		errno = EINVAL;
	return {};
}

#define YCL_Impl_FileSystem_ufunc_1(_n) \
bool \
_n(const char* path) ynothrowv \
{ \
	YAssertNonnull(path); \

#if YCL_Win32
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	YCL_Impl_RetTryCatchAll(_wfn(UTF8ToWCS(path).c_str()) == 0) \
	return {}; \
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

YCL_Impl_FileSystem_ufunc_1(uunlink)
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(CallFuncWithAttr(UnlinkWithAttr, path))
	return {};
}
#else
YCL_Impl_FileSystem_ufunc_2(::unlink, )
#endif

YCL_Impl_FileSystem_ufunc_1(uremove)
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(CallFuncWithAttr(
		[](const wchar_t* wstr, FileAttributes attr) YB_NONNULL(1) ynothrow{
			return attr & FileAttributes::Directory ? _wrmdir(wstr) == 0
				: UnlinkWithAttr(wstr, attr);
	}, path))
	return {};
}
#else
YCL_Impl_FileSystem_ufunc_2(std::remove, )
#endif

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

YB_NONNULL(1) size_t
FetchNumberOfLinks(const char* path, bool follow_link) ynothrowv
{
#if YCL_Win32
	return QueryFileLinks(UTF8ToWCS(path).c_str(), follow_link);
#else
	struct ::stat st;

	estat(st, path, follow_link);
	return size_t(st.st_nlink > 0 ? st.st_nlink : 0);
#endif
}
YB_NONNULL(1) size_t
FetchNumberOfLinks(const char16_t* path, bool follow_link) ynothrowv
{
#if YCL_Win32
	return QueryFileLinks(wcast(path), follow_link);
#else
	return FetchNumberOfLinks(MakeMBCS(path).c_str(), follow_link);
#endif
}


UniqueFile
EnsureUniqueFile(const char* dst, mode_t mode, size_t allowed_links,
	bool share)
{
	static yconstexpr const int de_oflag
#if YCL_Win32
		(_O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY);
#else
		(O_WRONLY | O_CREAT | O_TRUNC);
#endif

	mode &= mode_t(Mode::User);
	if(UniqueFile p_file{uopen(dst,
#if YCL_Win32
		de_oflag | _O_EXCL
#else
		de_oflag | O_EXCL
#endif
		, mode)})
		return p_file;
	if(allowed_links != 0 && errno == EEXIST)
	{
		const auto n_links(FetchNumberOfLinks(dst));

		// FIXME: Blocked. TOCTTOU access.
		if(!(allowed_links < n_links)
			&& (n_links == 1 || share || uunlink(dst) || errno == ENOENT))
			if(UniqueFile p_file{uopen(dst, de_oflag, mode)})
				return p_file;
	}
	ThrowFileOperationFailure("Failed creating file '" + string(dst) + "'.");
}

bool
HaveSameContents(const char* path_a, const char* path_b, mode_t mode)
{
	auto pr(HaveSameContents_Impl(path_a, path_b, mode));

	return pr.first ? HaveSameContents(std::move(pr.first),
		std::move(pr.second), path_a, path_b) : false;
}
bool
HaveSameContents(const char16_t* path_a, const char16_t* path_b, mode_t mode)
{
	auto pr(HaveSameContents_Impl(path_a, path_b, mode));

	// TODO: Support names in exception messages.
	return pr.first ? HaveSameContents(std::move(pr.first),
		std::move(pr.second), {}, {}) : false;
}
bool
HaveSameContents(UniqueFile p_a, UniqueFile p_b, const char* name_a,
	const char* name_b)
{
	if(!(bool(Nonnull(p_a)->GetCategory() & NodeCategory::Directory)
		|| bool(Nonnull(p_b)->GetCategory() & NodeCategory::Directory)))
	{
		if(IsNodeShared(p_a.get(), p_b.get()))
			return true;

		filebuf fb_a, fb_b;

		errno = 0;
		// FIXME: Implement for streams without open-by-raw-file extension.
		// TODO: Throw a nested exception with %errno if 'errno != 0'.
		if(!fb_a.open(std::move(p_a),
			std::ios_base::in | std::ios_base::binary))
			ThrowFileOperationFailure(name_a ? ("Failed opening first file '"
				+ string(name_a) + "'.").c_str()
				: "Failed opening first file.");
		// TODO: Throw a nested exception with %errno if 'errno != 0'.
		if(!fb_b.open(std::move(p_b),
			std::ios_base::in | std::ios_base::binary))
			ThrowFileOperationFailure(name_b ? ("Failed opening second file '"
				+ string(name_b) + "'.").c_str()
				: "Failed opening second file.");
		return ystdex::streambuf_equal(fb_a, fb_b);
	}
	return {};
}

bool
IsNodeShared(const char* a, const char* b, bool follow_link) ynothrowv
{
	return a == b || ystdex::ntctscmp(Nonnull(a), Nonnull(b)) == 0
		|| IsNodeShared_Impl(a, b, follow_link);
}
bool
IsNodeShared(const char16_t* a, const char16_t* b, bool follow_link) ynothrowv
{
	return a == b || ystdex::ntctscmp(Nonnull(a), Nonnull(b)) == 0
		|| IsNodeShared_Impl(a, b, follow_link);
}
bool
IsNodeShared(FileDescriptor x, FileDescriptor y) ynothrow
{
	const auto id(x.GetNodeID());

	return id != FileNodeID() && id == y.GetNodeID();
}

} // namespace platform;

