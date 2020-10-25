/*
	© 2011-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileIO.cpp
\ingroup YCLib
\brief 平台相关的文件访问和输入/输出接口。
\version r3696
\author FrankHB <frankhb1989@gmail.com>
\since build 615
\par 创建时间:
	2015-07-14 18:53:12 +0800
\par 修改时间:
	2020-10-25 06:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileIO
*/


#undef __STRICT_ANSI__ // for ::fileno, ::pclose, ::popen, ::_wfopen;
#include "YCLib/YModules.h"
#include YFM_YCLib_FileIO // for std::is_same, ystdex::underlying_type_t,
//	ystdex::invoke_result_t, ystdex::invoke, RetryOnInterrupted,
//	std::errc::function_not_supported, YCL_CallF_CAPI, std::is_integral,
//	ystdex::invoke, Nonnull, ystdex::temporary_buffer;
#include YFM_YCLib_NativeAPI // for Mode, ::HANDLE, struct ::stat,
//	platform_ex::cstat, platform_ex::estat ::GetConsoleMode, OpenMode,
//	YCL_CallGlobal, ::close, ::fcntl, F_GETFL, ::setmode, ::fchmod, ::_chsize,
//	::ftruncate, ::fsync, ::_wgetcwd, ::getcwd, ::chdir, ::rmdir, ::unlink,
//	!defined(__STRICT_ANSI__) API, ::GetCurrentDirectoryW;
#include YFM_YCLib_FileSystem // for NodeCategory::*, CategorizeNode;
#include <ystdex/functional.hpp> // for ystdex::compose, ystdex::addrof;
#include <ystdex/string.hpp> // for ystdex::write_ntcts;
#include <ystdex/streambuf.hpp> // for ystdex::streambuf_equal;
#if YCL_DS
#	include "CHRLib/YModules.h"
#	include YFM_CHRLib_CharacterProcessing // for CHRLib::MakeMBCS,
//	CHRLib::MakeUCS2LE;

#elif YCL_Win32
#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// At least one headers of <stdlib.h>, <stdio.h>, <Windows.h>, <Windef.h>
//	(and probably more) should have been included to make the MinGW-W64 macro
//	available if it is really being used.
#		undef _fileno
#	endif
#	if __GLIBCXX__
#		include <ystdex/ios.hpp> // for ystdex::rethrow_badstate;
#		include YFM_Win32_YCLib_Consoles // for platform_ex::WConsole;
#		include <ext/stdio_sync_filebuf.h> // for __gnu_cxx::stdio_sync_filebuf;
#	endif
#	include YFM_Win32_YCLib_MinGW32 // for 
//	platform_ex::UnlockFile, platform_ex::LockFile, platform_ex::TryLockFile;
#	include YFM_Win32_YCLib_NLS // for platform_ex::UTF8ToWCS,
//	platform_ex::WCSToUTF8;

//! \since build 540
using platform_ex::UTF8ToWCS;
//! \since build 706
using platform_ex::MakePathStringW;
#elif YCL_API_POSIXFileSystem
#	include "CHRLib/YModules.h"
#	include YFM_CHRLib_CharacterProcessing // for CHRLib::MakeMBCS,
//	CHRLib::MakeUCS2LE;
#	include <sys/file.h> // for ::flock, LOCK_*;

//! \since build 475
using namespace CHRLib;
#else
#	error "Unsupported platform found."
#endif

namespace platform
{

//! \since build 627
static_assert(std::is_same<mode_t, ystdex::underlying_type_t<Mode>>::value,
	"Mismatched mode types found.");

namespace
{

//! \since build 703
using platform_ex::estat;
//! \since build 719
using platform_ex::cstat;
#if YCL_Win32
//! \since build 704
using platform_ex::ToHandle;
#endif

//! \since build 765
template<int _vErr, typename _fCallable, class _tObj, typename _tByteBuf,
	typename... _tParams>
_tByteBuf
FullReadWrite(_fCallable f, _tObj&& obj, _tByteBuf ptr, size_t nbyte,
	_tParams&&... args)
{
	while(nbyte > 0)
	{
		const auto n(ystdex::invoke(f, obj, ptr, nbyte, yforward(args)...));

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

//! \since build 709
//@{
#if YCL_Win64
using rwsize_t = unsigned;
#else
using rwsize_t = size_t;
#endif

template<typename _func, typename _tBuf>
size_t
SafeReadWrite(_func f, int fd, _tBuf buf, rwsize_t nbyte) ynothrowv
{
	YAssert(fd != -1, "Invalid file descriptor found.");
	return size_t(RetryOnInterrupted([&]{
		return f(fd, Nonnull(buf), nbyte);
	}));
}
//@}

#if !YCL_DS
//! \since build 721
YB_NONNULL(2) void
UnlockFileDescriptor(int fd, const char* sig) ynothrowv
{
#	if YCL_Win32
	const auto res(platform_ex::UnlockFile(ToHandle(fd)));

	if(YB_UNLIKELY(!res))
		YCL_Trace_Win32E(Descriptions::Warning, UnlockFileEx, sig);
	YAssert(res, "Narrow contract violated.");
#	else
	const auto res(YCL_TraceCall_CAPI(::flock, sig, fd, LOCK_UN));

	YAssert(res == 0, "Narrow contract violated.");
	yunused(res);
#	endif
}
#endif

#if YCL_Win32 && __GLIBCXX__
YB_NONNULL(3) bool
StreamPutToFileDescriptor(std::ostream& os, int fd, const char* s)
{
	const auto h(ToHandle(fd));

	if(h != INVALID_HANDLE_VALUE)
	{
		unsigned long mode;

		if(::GetConsoleMode(h, &mode))
		{
			if(os.flush())
			{
				std::ios_base::iostate st(std::ios_base::goodbit);

				if(const auto k{typename std::ostream::sentry(os)})
				{
					if(*s != '\0')
						try
						{
							const auto wstr(platform_ex::UTF8ToWCS(s));
							const auto n(wstr.length());

							if(platform_ex::WConsole(h).WriteString(wstr) != n)
								st |= std::ios_base::badbit;
						}
						CatchExpr(...,
							ystdex::rethrow_badstate(os, std::ios_base::badbit))
				}
				else
					st |= std::ios_base::badbit;
				os.setstate(st);
			}
			return true;
		}
	}
	return {};
}
#endif

//! \since build 701
template<typename _tChar>
YB_NONNULL(1, 2) pair<UniqueFile, UniqueFile>
HaveSameContents_Impl(const _tChar* path_a, const _tChar* path_b, mode_t mode)
{
	if(UniqueFile p_a{uopen(path_a, int(OpenMode::ReadRaw), mode)})
		if(UniqueFile p_b{uopen(path_b, int(OpenMode::ReadRaw), mode)})
			return {std::move(p_a), std::move(p_b)};
	return {};
}

} // unnamed namespace;


string
MakePathString(const char16_t* s)
{
#if YCL_Win32
	return platform_ex::WCSToUTF8(wcast(s));
#else
	return CHRLib::MakeMBCS<string>(s);
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
	if(p && YB_UNLIKELY(YCL_CallGlobal(close, *p) < 0))
		// NOTE: This is not a necessarily error. See $2016-04
		//	@ %Documentation::Workflow.
		YTraceDe(Descriptions::Warning,
			"Falied closing file descriptor, errno = %d.", errno);
}


FileDescriptor::FileDescriptor(std::FILE* fp) ynothrow
	: desc(fp ? YCL_CallGlobal(fileno, fp) : -1)
{}

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
FileDescriptor::GetFlags() const
{
#if YCL_API_POSIXFileSystem
	return YCL_CallF_CAPI(, ::fcntl, desc, F_GETFL);
#else
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#endif
}
mode_t
FileDescriptor::GetMode() const
{
	struct ::stat st;

	cstat(st, desc, yfsig);
	return st.st_mode;
}
std::uint64_t
FileDescriptor::GetSize() const
{
#if YCL_Win32
	return platform_ex::QueryFileSize(ToHandle(desc));
#else
	struct ::stat st;

	YCL_CallF_CAPI(, ::fstat, desc, &st);

	// XXX: No negative file size should be found. See also:
	//	http://stackoverflow.com/questions/12275831/why-is-the-st-size-field-in-struct-stat-signed.
	if(st.st_size >= 0)
		return std::uint64_t(st.st_size);
	throw std::invalid_argument("Negative file size found.");
#endif
}

bool
FileDescriptor::SetBlocking() const
{
#if YCL_API_POSIXFileSystem
	// NOTE: Read-modify-write operation is needed for compatibility.
	//	See http://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html.
	const auto oflags((OpenMode(GetFlags())));

	return bool(oflags & OpenMode::Nonblocking)
		? (SetFlags(int(oflags & ~OpenMode::Nonblocking)), true) : false;
#else
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#endif
}
void
FileDescriptor::SetFlags(int flags) const
{
#if YCL_API_POSIXFileSystem
	YCL_CallF_CAPI(, ::fcntl, desc, F_SETFL, flags);
#else
	// TODO: Try using NT6 %::SetFileInformationByHandle for Win32.
	yunused(flags);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#endif
}
void
FileDescriptor::SetMode(mode_t mode) const
{
#if YCL_API_POSIXFileSystem
	YCL_CallF_CAPI(, ::fchmod, desc, mode);
#else
	yunused(mode);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#endif
}
bool
FileDescriptor::SetNonblocking() const
{
#if YCL_API_POSIXFileSystem
	// NOTE: Read-modify-write operation is need for compatibility.
	//	See http://pubs.opengroup.org/onlinepubs/9699919799/functions/fcntl.html.
	const auto oflags((OpenMode(GetFlags())));

	return !bool(oflags & OpenMode::Nonblocking)
		? (SetFlags(int(oflags | OpenMode::Nonblocking)), true) : false;
#else
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#endif
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
	// TODO: Other platforms.
#if YCL_Win32 || (defined(_NEWLIB_VERSION) && defined(__SCLE))
	return YCL_CallGlobal(setmode, desc, mode);
#else
	// NOTE: No effect.
	yunused(mode);
	return 0;
#endif
}

void
FileDescriptor::Flush()
{
#if YCL_Win32
	// NOTE: This calls %::FlushFileBuffers internally.
	YCL_CallF_CAPI(, ::_commit, desc);
#else
	// XXX: http://austingroupbugs.net/view.php?id=672.
	YCL_CallF_CAPI(, ::fsync, desc);
#endif
}

size_t
FileDescriptor::FullRead(void* buf, size_t nbyte) ynothrowv
{
	using namespace std::placeholders;
	const auto p_buf(static_cast<char*>(buf));

	return size_t(FullReadWrite<0>(&FileDescriptor::Read, this, p_buf, nbyte)
		- p_buf);
}

size_t
FileDescriptor::FullWrite(const void* buf, size_t nbyte) ynothrowv
{
	using namespace std::placeholders;
	const auto p_buf(static_cast<const char*>(buf));

	return size_t(FullReadWrite<ENOSPC>(&FileDescriptor::Write, this, p_buf,
		nbyte) - p_buf);
}

size_t
FileDescriptor::Read(void* buf, size_t nbyte) ynothrowv
{
	return SafeReadWrite(YCL_ReservedGlobal(read), desc, buf, rwsize_t(nbyte));
}

size_t
FileDescriptor::Write(const void* buf, size_t nbyte) ynothrowv
{
	return SafeReadWrite(YCL_ReservedGlobal(write), desc, buf, rwsize_t(nbyte));
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
			YCL_Raise_SysE(, "Failed reading source file '" + to_string(*ifd),
				yfsig);
		if(ofd.FullWrite(buf, len) == size_t(-1))
			YCL_Raise_SysE(,
				"Failed writing destination file '" + to_string(*ofd), yfsig);
		return len != 0;
	}, &FileDescriptor::Read, &ifd, buf, size);
}
void
FileDescriptor::WriteContent(FileDescriptor ofd, FileDescriptor ifd,
	size_t size)
{
	ystdex::temporary_buffer<byte> buf(size);

	WriteContent(ofd, ifd, buf.get().get(), buf.size());
}

void
FileDescriptor::lock()
{
#if YCL_DS
#elif YCL_Win32
	platform_ex::LockFile(ToHandle(desc));
#else
	YCL_CallF_CAPI(, ::flock, desc, LOCK_EX);
#endif
}

void
FileDescriptor::lock_shared()
{
#if YCL_DS
#elif YCL_Win32
	platform_ex::LockFile(ToHandle(desc), 0, std::uint64_t(-1), {});
#else
	YCL_CallF_CAPI(, ::flock, desc, LOCK_SH);
#endif
}

bool
FileDescriptor::try_lock() ynothrowv
{
#if YCL_DS
	return true;
#elif YCL_Win32
	return platform_ex::TryLockFile(ToHandle(desc), 0, std::uint64_t(-1));
#else
	return YCL_TraceCallF_CAPI(::flock, desc, LOCK_EX | LOCK_NB) != -1;
#endif
}

bool
FileDescriptor::try_lock_shared() ynothrowv
{
#if YCL_DS
	return true;
#elif YCL_Win32
	return platform_ex::TryLockFile(ToHandle(desc), 0, std::uint64_t(-1), {});
#else
	return YCL_TraceCallF_CAPI(::flock, desc, LOCK_SH | LOCK_NB) != -1;
#endif
}

void
FileDescriptor::unlock() ynothrowv
{
#if YCL_DS
#else
	UnlockFileDescriptor(desc, yfsig);
#endif
}

void
FileDescriptor::unlock_shared() ynothrowv
{
#if YCL_DS
#else
	UnlockFileDescriptor(desc, yfsig);
#endif
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
	FileDescriptor(Nonnull(stream)).SetTranslationMode(int(OpenMode::Binary));
#else
	yunused(stream);
	// NOTE: No effect.
	YAssertNonnull(stream);
#endif
}


int
omode_conv(std::ios_base::openmode mode) ynothrow
{
	using namespace std;
	auto res(OpenMode::None);

	switch(unsigned((mode &= ~ios_base::ate)
		& ~(ios_base::binary | ios_nocreate | ios_noreplace)))
	{
	case ios_base::out:
		res = OpenMode::Create | OpenMode::Write;
		break;
	case ios_base::out | ios_base::trunc:
		res = OpenMode::Create | OpenMode::WriteTruncate;
		break;
	case ios_base::out | ios_base::app:
	case ios_base::app:
		res = OpenMode::Create | OpenMode::WriteAppend;
		break;
	case ios_base::in:
		res = OpenMode::Read;
		break;
	case ios_base::in | ios_base::out:
		res = OpenMode::ReadWrite;
		break;
	case ios_base::in | ios_base::out | ios_base::trunc:
		res = OpenMode::Create | OpenMode::ReadWriteTruncate;
		break;
	case ios_base::in | ios_base::out | ios_base::app:
	case ios_base::in | ios_base::app:
		res = OpenMode::Create | OpenMode::ReadWriteAppend;
		break;
	default:
		return int(res);
	}
	// XXX: Order is significant.
	if(mode & ios_noreplace)
		res |= OpenMode::CreateExclusive;
	// NOTE: %O_EXCL without %O_CREAT leads to undefined behavior in POSIX.
	if(mode & ios_nocreate)
		res &= ~OpenMode::CreateExclusive;
	return int(res);
}

int
omode_convb(std::ios_base::openmode mode) ynothrow
{
	return omode_conv(mode)
		| int(mode & std::ios_base::binary ? OpenMode::Binary : OpenMode::Text);
}


int
uopen(const char* filename, int oflag, mode_t pmode) ynothrowv
{
	YAssertNonnull(filename);
#if YCL_Win32
	return CallNothrow(-1, [=]{
		return ::_wopen(MakePathStringW(filename).c_str(), oflag, int(pmode));
	});
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
	return CallNothrow(-1, [=]{
		return ::open(MakePathString(filename).c_str(), oflag, pmode);
	});
#endif
}

std::FILE*
ufopen(const char* filename, const char* mode) ynothrowv
{
	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_Win32
	return CallNothrow({}, [=]{
		return ::_wfopen(MakePathStringW(filename).c_str(),
			MakePathStringW(mode).c_str());
	});
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
	return CallNothrow({}, [=]{
		return std::fopen(MakePathString(filename).c_str(),
			MakePathString(mode).c_str());
	});
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
		return CallNothrow({}, [=]{
#if YCL_Win32
			return ::_wfopen(wcast(filename), MakePathStringW(c_mode).c_str());
#else
			return std::fopen(MakePathString(filename).c_str(), c_mode);
#endif
		});
	return {};
}

bool
ufexists(const char* filename) ynothrowv
{
#if YCL_Win32
	return ystdex::call_value_or(ystdex::compose(std::fclose,
		ystdex::addrof<>()), ufopen(filename, "rb"), yimpl(1)) == 0;
#else
	return ystdex::fexists(filename);
#endif
}
bool
ufexists(const char16_t* filename) ynothrowv
{
	return ystdex::call_value_or(ystdex::compose(std::fclose,
		ystdex::addrof<>()), ufopen(filename, u"rb"), yimpl(1)) == 0;
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
	return CallNothrow({}, [=]{
		return ::_wpopen(MakePathStringW(filename).c_str(),
			MakePathStringW(mode).c_str());
	});
#else
	return ::popen(filename, mode);
#endif
}
std::FILE*
upopen(const char16_t* filename, const char16_t* mode) ynothrowv
{
	using namespace platform;

	YAssertNonnull(filename);
	YAssert(Deref(mode) != char(), "Invalid argument found.");
#if YCL_DS
	errno = ENOSYS;
	return {};
#elif YCL_Win32
	return ::_wpopen(wcast(filename), wcast(mode));
#else
	return CallNothrow({}, [=]{
		return ::popen(MakePathString(filename).c_str(),
			MakePathString(mode).c_str());
	});
#endif
}

int
upclose(std::FILE* fp) ynothrowv
{
	YAssertNonnull(fp);
#if YCL_DS
	errno = ENOSYS;
	return -1;
#else
	return YCL_CallGlobal(pclose, fp);
#endif
}


void
StreamPut(std::ostream& os, const char* s)
{
	YAssertNonnull(s);
#if YCL_Win32
	if(const auto p_sb = os.rdbuf())
	{
	// TODO: Implement for other standard library implementations.
#	if __GLIBCXX__
		if(const auto p = dynamic_cast<__gnu_cxx::stdio_filebuf<char>*>(p_sb))
		{
			if(StreamPutToFileDescriptor(os, p->fd(), s))
				return;
		}
		if(const auto p
			= dynamic_cast<__gnu_cxx::stdio_sync_filebuf<char>*>(p_sb))
		{
			if(StreamPutToFileDescriptor(os, ::_fileno(p->file()), s))
				return;
		}
#endif
	}
#endif
	ystdex::write_ntcts(os, s);
}


UniqueFile
EnsureUniqueFile(const char* dst, mode_t mode, size_t allowed_links,
	bool share)
{
	static yconstexpr const auto oflag(OpenMode::Create
		| OpenMode::WriteTruncate | OpenMode::Binary);

	mode &= mode_t(Mode::User);
	if(UniqueFile
		p_file{uopen(dst, int(oflag | OpenMode::Exclusive), mode)})
		return p_file;
	if(allowed_links != 0 && errno == EEXIST)
	{
		const auto n_links(FetchNumberOfLinks(dst, true));

		// FIXME: Blocked. TOCTTOU access.
		if(!(allowed_links < n_links)
			&& (n_links == 1 || share || uunlink(dst) || errno == ENOENT))
			if(UniqueFile p_file{uopen(dst, int(oflag), mode)})
				return p_file;
	}
	YCL_Raise_SysE(, "::open", yfsig);
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
			ystdex::throw_error(std::errc::bad_file_descriptor,
				name_a ? ("Failed opening first file '" + string(name_a) + '\'')
				.c_str() : "Failed opening first file");
		// TODO: Throw a nested exception with %errno if 'errno != 0'.
		if(!fb_b.open(std::move(p_b),
			std::ios_base::in | std::ios_base::binary))
			ystdex::throw_error(std::errc::bad_file_descriptor,
				name_b ? ("Failed opening second file '" + string(name_b)
				+ '\'').c_str() : "Failed opening second file");
		return ystdex::streambuf_equal(fb_a, fb_b);
	}
	return {};
}

} // namespace platform;

namespace platform_ex
{

#if YCL_Win32
wstring
MakePathStringW(const char* s)
{
	return platform_ex::UTF8ToWCS(s);
}
#else
u16string
MakePathStringU(const char* s)
{
	return CHRLib::MakeUCS2LE<u16string>(s);
}
#endif

} // namespace platform_ex;

