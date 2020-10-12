/*
	© 2011-2017, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.h
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r1659
\author FrankHB <frankhb1989@gmail.com>
\since build 202
\par 创建时间:
	2011-04-13 20:26:21 +0800
\par 修改时间:
	2019-10-10 18:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::NativeAPI
*/


#ifndef YCL_INC_NativeAPI_h_
#define YCL_INC_NativeAPI_h_ 1

#include "YModules.h"
#include YFM_YCLib_Platform // for yconstfn, YF_API;
#include <ystdex/type_op.hpp> // for ystdex::make_signed_t, std::is_signed;
#include YFM_YBaseMacro // for DefBitmaskEnum, PDefH, ImplRet;

#ifndef YF_Platform
#	error "Unknown platform found."
#endif

/*!	\defgroup workarounds Workarounds
\brief 变通：不便直接实现的替代方案。
\since build 297
*/

/*!	\defgroup name_collision_workarounds Name collision workarounds
\brief 名称冲突变通。
\note Windows API 冲突时显式使用带 A 或 W 的全局函数名称。
\since build 381
*/


//! \since build 709
//@{
/*!
\def YCL_ReservedGlobal
\brief 按实现修饰全局保留名称。
\see ISO C11 7.1.3 和 WG21 N4594 17.6.4.3 。
\see https://msdn.microsoft.com/en-us/library/ttcz0bys.aspx 。
\see https://msdn.microsoft.com/en-us/library/ms235384(v=vs.100).aspx#Anchor_0 。
*/
#if YCL_Win32
#	define YCL_ReservedGlobal(_n) _##_n
#else
#	define YCL_ReservedGlobal(_n) _n
#endif
//! \brief 调用按实现修饰的具有全局保留名称的函数。
#define YCL_CallGlobal(_n, ...) ::YCL_ReservedGlobal(_n)(__VA_ARGS__)
//@}


#include <stdio.h>
#if YCL_API_Has_dirent_h
#	include <dirent.h>
#endif
// TODO: Test whether <fcntl.h> is available.
#include <fcntl.h>
#if YCL_API_Has_semaphore_h
#	include <semaphore.h>
#endif
#if YCL_API_Has_unistd_h
#	include <unistd.h>
//! \since build 625
//@{
namespace platform
{
	using ssize_t = ::ssize_t;
} // namespace platform_ex;
#	else
namespace platform
{
#	if YCL_Win32
	using ssize_t = int;
#	else
	using ssize_t = ystdex::make_signed_t<std::size_t>;
#	endif
} // namespace platform;

static_assert(std::is_signed<platform::ssize_t>(),
	"Invalid signed size type found.");
//@}
#endif


#if YCL_Linux || YCL_OS_X
#	include <sys/mman.h>
#endif


#if YCL_Win32 || YCL_API_POSIXFileSystem
#	include <sys/stat.h> // for struct ::stat;

namespace platform
{

//! \since build 626
enum class Mode
#	if YCL_Win32
	: unsigned short
#	else
	: ::mode_t
#	endif
{
#	if YCL_Win32
	FileType = _S_IFMT,
	Directory = _S_IFDIR,
	Character = _S_IFCHR,
	FIFO = _S_IFIFO,
	Regular = _S_IFREG,
	UserRead = _S_IREAD,
	UserWrite = _S_IWRITE,
	UserExecute = _S_IEXEC,
	GroupRead = _S_IREAD >> 3,
	GroupWrite = _S_IWRITE >> 3,
	GroupExecute = _S_IEXEC >> 3,
	OtherRead = _S_IREAD >> 6,
	OtherWrite = _S_IWRITE >> 6,
	OtherExecute = _S_IEXEC >> 6,
#	else
	FileType = S_IFMT,
	Directory = S_IFDIR,
	Character = S_IFCHR,
	Block = S_IFBLK,
	Regular = S_IFREG,
	Link = S_IFLNK,
	Socket = S_IFSOCK,
	FIFO = S_IFIFO,
	UserRead = S_IRUSR,
	UserWrite = S_IWUSR,
	UserExecute = S_IXUSR,
	GroupRead = S_IRGRP,
	GroupWrite = S_IWGRP,
	GroupExecute = S_IXGRP,
	OtherRead = S_IROTH,
	OtherWrite = S_IWOTH,
	OtherExecute = S_IXOTH,
#	endif
	UserReadWrite = UserRead | UserWrite,
	User = UserReadWrite | UserExecute,
	GroupReadWrite = GroupRead | GroupWrite,
	Group = GroupReadWrite | GroupExecute,
	OtherReadWrite = OtherRead | OtherWrite,
	Other = OtherReadWrite | OtherExecute,
	Read = UserRead | GroupRead | OtherRead,
	Write = UserWrite | GroupWrite | OtherWrite,
	Execute = UserExecute | GroupExecute | OtherExecute,
	ReadWrite = Read | Write,
	Access = ReadWrite | Execute,
	//! \since build 627
	//@{
#	if !YCL_Win32
	SetUserID = S_ISUID,
	SetGroupID = S_ISGID,
#	else
	SetUserID = 0,
	SetGroupID = 0,
#	endif
#	if YCL_Linux || _XOPEN_SOURCE
	VTX = S_ISVTX,
#	else
	VTX = 0,
#	endif
	PMode = SetUserID | SetGroupID | VTX | Access,
	All = PMode | FileType
	//@}
};

//! \relates Mode
//@{
//! \since build 626
DefBitmaskEnum(Mode)

//! \since build 627
yconstfn PDefH(bool, HasExtraMode, Mode m)
	ImplRet(bool(m & ~(Mode::Access | Mode::FileType)))
//@}


//! \since build 722
//@{
//! \brief 打开模式。
enum class OpenMode : int
{
#define YCL_Impl_OMode(_n, _nm) _n = YCL_ReservedGlobal(O_##_nm)
#if YCL_Win32
#	define YCL_Impl_OMode_POSIX(_n, _nm) _n = 0
#	define YCL_Impl_OMode_Win32(_n, _nm) YCL_Impl_OMode(_n, _nm)
#else
#	define YCL_Impl_OMode_POSIX(_n, _nm) YCL_Impl_OMode(_n, _nm)
#	define YCL_Impl_OMode_Win32(_n, _nm) _n = 0
#endif
#if O_CLOEXEC
	YCL_Impl_OMode_POSIX(CloseOnExec, CLOEXEC),
#endif
	YCL_Impl_OMode(Create, CREAT),
#if O_DIRECTORY
	YCL_Impl_OMode_POSIX(Directory, DIRECTORY),
#else
	// XXX: Platform %DS does not support this.
	Directory = 0,
#endif
	YCL_Impl_OMode(Exclusive, EXCL),
	CreateExclusive = Create | Exclusive,
	YCL_Impl_OMode_POSIX(NoControllingTerminal, NOCTTY),
#if O_NOFOLLOW
	YCL_Impl_OMode_POSIX(NoFollow, NOFOLLOW),
#else
	// NOTE: Platform %DS does not support links.
	// NOTE: Platform %Win32 does not support links for these APIs.
	NoFollow = 0,
#endif
	YCL_Impl_OMode(Truncate, TRUNC),
#if O_TTY_INIT
	YCL_Impl_OMode_POSIX(TerminalInitialize, TTY_INIT),
#endif
	YCL_Impl_OMode(Append, APPEND),
#if O_DSYNC
	YCL_Impl_OMode_POSIX(DataSynchronized, DSYNC),
#endif
	YCL_Impl_OMode_POSIX(Nonblocking, NONBLOCK),
#if O_RSYNC
	YCL_Impl_OMode_POSIX(ReadSynchronized, RSYNC),
#endif
	YCL_Impl_OMode_POSIX(Synchronized, SYNC),
#if O_EXEC
	YCL_Impl_OMode_POSIX(Execute, EXEC),
#endif
	YCL_Impl_OMode(Read, RDONLY),
	YCL_Impl_OMode(ReadWrite, RDWR),
#if O_SEARCH
	YCL_Impl_OMode_POSIX(Search, SEARCH),
#endif
	YCL_Impl_OMode(Write, WRONLY),
	ReadWriteAppend = ReadWrite | Append,
	ReadWriteTruncate = ReadWrite | Truncate,
	WriteAppend = Write | Append,
	WriteTruncate = Write | Truncate,
	YCL_Impl_OMode_Win32(Text, TEXT),
	YCL_Impl_OMode_Win32(Binary, BINARY),
	Raw = Binary,
	ReadRaw = Read | Raw,
	ReadWriteRaw = ReadWrite | Raw,
	// NOTE: On GNU/Hurd %O_ACCMODE can be zero.
#if O_ACCMODE
	YCL_Impl_OMode_POSIX(AccessMode, ACCMODE),
#else
	AccessMode = Read | Write | ReadWrite,
#endif
	YCL_Impl_OMode_Win32(WText, WTEXT),
	YCL_Impl_OMode_Win32(U16Text, U16TEXT),
	YCL_Impl_OMode_Win32(U8Text, U8TEXT),
	YCL_Impl_OMode_Win32(NoInherit, NOINHERIT),
	YCL_Impl_OMode_Win32(Temporary, TEMPORARY),
	YCL_Impl_OMode_Win32(ShortLived, SHORT_LIVED),
	CreateTemporary = Create | Temporary,
	CreateShortLived = Create | ShortLived,
	YCL_Impl_OMode_Win32(Sequential, SEQUENTIAL),
	YCL_Impl_OMode_Win32(Random, RANDOM),
#if O_NDELAY
	YCL_Impl_OMode(NoDelay, NDELAY),
#endif
	//! \warning 库实现内部使用，需要特定的二进制支持。
	//@{
#if O_LARGEFILE
	//! \note 指定 64 位文件大小。
	YCL_Impl_OMode(LargeFile, LARGEFILE),
#else
	LargeFile = 0,
#endif
#if _O_OBTAIN_DIR
	YCL_Impl_OMode(ObtainDirectory, OBTAIN_DIR),
#else
	//! \note 设置 FILE_FLAG_BACKUP_SEMANTICS 。
	ObtainDirectory = Directory,
#endif
	//@}
	None = 0
#undef YCL_Impl_OMode_POSIX
#undef YCL_Impl_OMode_Win32
#undef YCL_Impl_OMode
};

//! \relates OpenMode
DefBitmaskEnum(OpenMode)
//@}

} // namespace platform;

//! \since build 703
namespace platform_ex
{

/*!
\note 第三参数表示是否跟随链接。
\pre 间接断言：指针参数非空。
\note DS 和 Win32 平台：忽略第三参数，始终不跟随链接。
*/
//@{
/*!
\brief 带检查的可跟随链接的 \c stat 调用。
\throw std::system_error 检查失败。
\note 最后一个参数表示调用签名。
\since build 719
*/
//@{
YF_API YB_NONNULL(2, 4) void
cstat(struct ::stat&, const char*, bool, const char*);
YF_API YB_NONNULL(3) void
cstat(struct ::stat&, int, const char*);
//@}

//! \brief 可跟随链接的 \c stat 调用。
YF_API YB_NONNULL(2) int
estat(struct ::stat&, const char*, bool) ynothrowv;
inline PDefH(int, estat, struct ::stat& st, int fd) ynothrow
	ImplRet(::fstat(fd, &st))
//@}

} // namespace platform_ex;
#endif


/*!
\ingroup name_collision_workarounds
\see http://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_01 。
\since build 720
*/
//@{
//! \note 定义在 <stdio.h> 。
//@{
#undef ctermid
#undef dprintf
#undef fdopen
//! \note 已知 DS 平台的 newlib 可能使用宏。
#undef fileno
#undef flockfile
#undef fmemopen
#undef fseeko
#undef ftello
#undef ftrylockfile
#undef funlockfile
#undef getc_unlocked
#undef getchar_unlocked
#undef getdelim
#undef getline
#undef open_memstream
#undef pclose
#undef popen
#undef putc_unlocked
#undef putchar_unlocked
#undef renameat
#undef tempnam
#undef vdprintf
//@}
//! \note 定义在 <dirent.h> 。
//@{
#undef alphasort
#undef closedir
#undef dirfd
#undef fdopendir
#undef opendir
#undef readdir
#undef readdir_r
#undef rewinddir
#undef scandir
#undef seekdir
#undef telldir
//@}
//! \note 定义在 <fcntl.h> 。
//@{
#undef creat
#undef fcntl
#undef open
#undef openat
#undef posix_fadvise
#undef posix_fallocate
//@}
//! \note 定义在 <semaphore.h> 。
#undef sem_close
#undef sem_destroy
#undef sem_getvalue
#undef sem_init
#undef sem_open
#undef sem_post
#undef sem_timedwait
#undef sem_trywait
#undef sem_unlink
#undef sem_wait
//! \note 定义在 <unistd.h> 。
//@{
#undef _exit
#undef access
#undef alarm
#undef chdir
#undef chown
#undef close
#undef confstr
#undef crypt
#undef dup
#undef dup2
#undef encrypt
#undef execl
#undef execle
#undef execlp
#undef execv
#undef execve
#undef execvp
#undef faccessat
#undef fchdir
#undef fchown
#undef fchownat
#undef fdatasync
#undef fexecve
#undef fork
#undef fpathconf
#undef fsync
#undef ftruncate
#undef getcwd
#undef getegid
#undef geteuid
#undef getgid
#undef getgroups
#undef gethostid
#undef gethostname
#undef getlogin
#undef getlogin_r
#undef getopt
#undef getpgid
#undef getpgrp
#undef getpid
#undef getppid
#undef getsid
#undef getuid
#undef isatty
#undef lchown
#undef link
#undef linkat
#undef lockf
#undef lseek
#undef nice
#undef pathconf
#undef pause
#undef pipe
#undef pread
#undef pwrite
#undef read
#undef readlink
#undef readlinkat
#undef rmdir
#undef setegid
#undef seteuid
#undef setgid
#undef setpgid
#undef setpgrp
#undef setregid
#undef setreuid
#undef setsid
#undef setuid
#undef sleep
#undef swab
#undef symlink
#undef symlinkat
#undef sync
#undef sysconf
#undef tcgetpgrp
#undef tcsetpgrp
#undef truncate
#undef ttyname
#undef ttyname_r
#undef unlink
#undef unlinkat
#undef write
//@}
//! \note 定义在 <sys/mman.h> 。
//@{
#undef mlock
#undef mlockall
#undef mmap
#undef mprotect
#undef msync
#undef munlock
#undef munlockall
#undef munmap
#undef posix_madvise
#undef posix_mem_offset
#undef posix_typed_mem_get_info
#undef posix_typed_mem_open
#undef shm_open
#undef shm_unlink
//@}
//! \note 定义在 <sys/stat.h> 。
//@{
#undef chmod
#undef fchmod
#undef fchmodat
#undef fstat
#undef fstatat
#undef futimens
#undef lstat
#undef mkdir
#undef mkdirat
#undef mkfifo
#undef mkfifoat
#undef mknod
#undef mknodat
#undef stat
#undef umask
#undef utimensat
//@}
//@}


#if YCL_DS
#	include <nds.h>
#	include <ystdex/base.h> // for ystdex::nonmovable;

namespace platform_ex
{

/*!
\brief DMA 异步填充字。
\param chan 使用的通道编号（取值范围 0 ~ 3 ）。
\param val 填充的 32 位字。
\param p_dst 填充目标。
\param size 填充的字节数。
\note 当前仅适用于 ARM9 ；填充的字节数会被按字（ 4 字节）截断。
\since build 405
*/
inline void
DMAFillWordsAsync(std::uint8_t chan, std::uint32_t val, void* p_dst,
	std::uint32_t size)
{
	DMA_FILL(chan) = std::uint32_t(val);
	DMA_SRC(3) = std::uint32_t(&DMA_FILL(3));
	DMA_DEST(3) = std::uint32_t(p_dst);

	DMA_CR(3) = DMA_SRC_FIX | DMA_COPY_WORDS | size >> 2;
}


/*!
\brief 文件系统。
\since build 690
*/
class YF_API FileSystem final
	: private ystdex::noncopyable, private ystdex::nonmovable
{
public:
	enum class RootKind
	{
		Null,
		SD,
		FAT
	};

private:
	RootKind root;
	const char* init_dev;

public:
	/*!
	\brief 构造：使用指定缓冲页数初始化文件系统，成功后切换当前目录。
	\post <tt>root != RootKind::Null && init_dev</tt> 。
	\throw std::runtime_error 初始化失败。
	\note 参数及默认值对应 LibFAT 源码。
	\see LibFAT 源码文件 "common.h" 的宏 DEFAULT_CACHE_PAGES 。
	*/
	FileSystem(size_t = 16);
	/*!
	\brief 析构：反初始化文件系统。
	\pre 间接断言： \c init_dev 。
	*/
	~FileSystem();
};

} // namespace platform_ex;


#elif YCL_Win32

#	ifndef UNICODE
#		define UNICODE 1
#	endif

#	ifndef WINVER
//! \since build 448
#		define WINVER 0x0501
#	endif

#	ifndef WIN32_LEAN_AND_MEAN
//! \since build 448
#		define WIN32_LEAN_AND_MEAN 1
#	endif

#	ifndef NOMINMAX
//! \since build 521
#		define NOMINMAX 1
#	endif

#	include <Windows.h>
#	if __has_include(<specstrings_undef.h>)
#		include <specstrings_undef.h>
#	else
// NOTE: Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97362.
#		undef __deref
#	endif
#	include <direct.h>
#	include <io.h> // for ::_get_osfhandle;

//! \ingroup name_collision_workarounds
//@{
//! \since build 637
#	undef CopyFile
//! \since build 633
#	undef CreateHardLink
//! \since build 651
#	undef CreateSymbolicLink
//! \since build 297
#	undef DialogBox
//! \since build 298
#	undef DrawText
//! \since build 592
#	undef ExpandEnvironmentStrings
//! \since build 381
#	undef FindWindow
//! \since build 664
#	undef FormatMessage
//! \since build 592
#	undef GetMessage
//! \since build 313
#	undef GetObject
//! \since build 298
#	undef PostMessage
//! \since build 762
#	undef SetEnvironmentVariable
//@}

extern "C"
{

#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
//! \since build 465
YF_API struct ::tm* __cdecl __MINGW_NOTHROW
_gmtime32(const ::__time32_t*);
#	endif

} // extern "C";

namespace platform_ex
{

/*!
\brief 取文件描述符对应的句柄。
\since build 704
*/
inline PDefH(::HANDLE, ToHandle, int fd) ynothrow
	ImplRet(::HANDLE(::_get_osfhandle(fd)))

} // namespace platform_ex;

#elif YCL_Android
/*!
\see https://android.googlesource.com/platform/bionic/+/840a114eb12773c5af39c0c97675b27aa6dee78c/libc/include/sys/stat.h 。
\since build 651
*/
extern "C"
{

/*!
\see http://pubs.opengroup.org/onlinepubs/9699919799/functions/link.html 。
\since build 660
*/
#	if !(__ANDROID_API__ >= 21)
int
linkat(int, const char*, int, const char*, int) yimpl(ynothrow);
#	endif

#	ifndef UTIME_NOW
#		define UTIME_NOW ((1L << 30) - 1L)
#	endif
#	ifndef UTIME_OMIT
#		define UTIME_OMIT ((1L << 30) - 2L)
#	endif

//! \see http://pubs.opengroup.org/onlinepubs/9699919799/functions/utimensat.html 。
//@{
#	if !(__ANDROID_API__ >= 19)
int
futimens(int, const ::timespec times[2]) yimpl(ynothrow);
#	endif

#	if !(__ANDROID_API__ >= 12)
int
utimensat(int, const char*, const ::timespec[2], int) yimpl(ynothrow);
#	endif
//@}

} // extern "C";
#endif

#endif

