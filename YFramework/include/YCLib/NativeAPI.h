/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.h
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r1283
\author FrankHB <frankhb1989@gmail.com>
\since build 202
\par 创建时间:
	2011-04-13 20:26:21 +0800
\par 修改时间:
	2016-06-23 09:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::NativeAPI
*/


#ifndef YCL_INC_NativeAPI_h_
#define YCL_INC_NativeAPI_h_ 1

#include "YModules.h"
#include YFM_YCLib_Platform
#include <ystdex/type_op.hpp> // for ystdex::make_signed_t, std::is_signed;
#include YFM_YBaseMacro

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


#if YCL_API_Has_dirent_h
#	include <dirent.h>
#endif

// TODO: Test whether <fcntl.h> is available.
#include <fcntl.h>

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

} // namespace platform;

//! \since build 703
namespace platform_ex
{

/*!
\note 第三参数表示是否跟随链接。
\note DS 和 Win32 平台：忽略第三参数，始终不跟随链接。
*/
YF_API YB_NONNULL(2) int
estat(struct ::stat&, const char*, bool) ynothrowv;
inline PDefH(int, estat, struct ::stat& st, int fd) ynothrow
	ImplRet(::fstat(fd, &st))

} // namespace platform_ex;
#endif


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
int
linkat(int, const char*, int, const char*, int) ynothrow;

#	ifndef UTIME_NOW
#		define UTIME_NOW ((1L << 30) - 1L)
#	endif
#	ifndef UTIME_OMIT
#		define UTIME_OMIT ((1L << 30) - 2L)
#	endif

//! \see http://pubs.opengroup.org/onlinepubs/9699919799/functions/utimensat.html 。
//@{
int
futimens(int, const ::timespec times[2]) ynothrow;

int
utimensat(int, const char*, const ::timespec[2], int) ynothrow;
//@}

} // extern "C";
#endif

#endif

