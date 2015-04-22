/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.h
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r858
\author FrankHB <frankhb1989@gmail.com>
\since build 202
\par 创建时间:
	2011-04-13 20:26:21 +0800
\par 修改时间:
	2015-04-19 10:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::NativeAPI
*/


#ifndef YCL_INC_NativeAPI_h_
#define YCL_INC_NativeAPI_h_ 1

//平台定义。
#include "YModules.h"
#include YFM_YCLib_Platform

#include <ydef.h>
#include YFM_YBaseMacro

#ifndef YF_Platform
#	error "Unknown platform found."
#endif

/*!	\defgroup workarounds Workarounds
\brief 替代方案。
\since build 297
*/

/*!	\defgroup name_collision_workarounds Name collision workarounds
\brief 名称冲突替代方案。
\note Windows API 冲突时显式使用带 A 或 W 的全局函数名称。
\since build 381
*/


/*!
\brief 平台替代命名空间。
\since build 298
*/
namespace platform_replace
{} // namespace platform_replace;


#if YCL_API_Has_dirent_h
#	include <dirent.h>
#endif

#if YCL_API_Has_unistd_h
#	include <unistd.h>
#endif


#if YCL_DS

#	include <nds.h>
#	include <fat.h>
//包含 PALib 。
//#	include <PA9.h>

#	ifdef USE_EFS
//包含 EFSLib 。
#		include "efs_lib.h"
#	endif

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
DMAFillWordsAsync(u8 chan, u32 val, void* p_dst, u32 size)
{
	DMA_FILL(chan) = vu32(val);
	DMA_SRC(3) = u32(&DMA_FILL(3));
	DMA_DEST(3) = u32(p_dst);

	DMA_CR(3) = DMA_SRC_FIX | DMA_COPY_WORDS | size >> 2;
}

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
#	include <direct.h> // for ::_mkdir;
#	include <sys/stat.h>

//! \ingroup name_collision_workarounds
//@{
//! \since build 297
#	undef DialogBox
//! \since build 298
#	undef DrawText
//! \since build 592
#	undef ExpandEnvironmentStrings
//! \since build 381
#	undef FindWindow
//! \since build 592
#	undef GetMessage
//! \since build 313
#	undef GetObject
//! \since build 298
#	undef PostMessage
//@}


#	ifndef S_IFIFO
#		define S_IFIFO _S_IFIFO
#	endif

#	ifndef S_IFMT
#		define S_IFMT _S_IFMT
#	endif
#	ifndef S_IFDIR
#		define S_IFDIR _S_IFDIR
#	endif
#	ifndef S_IFCHR
#		define S_IFCHR _S_IFCHR
#	endif
#	ifndef S_IFREG
#		define S_IFREG _S_IFREG
#	endif

#	ifndef S_IREAD
#		define S_IREAD _S_IREAD
#	endif
#	ifndef S_IWRITE
#		define S_IWRITE _S_IWRITE
#	endif
#	ifndef S_IEXEC
#		define S_IEXEC _S_IEXEC
#	endif

#	ifndef S_IRGRP
#		define S_IRGRP	(_S_IREAD >> 3)
#	endif
#	ifndef S_IWGRP
#		define S_IWGRP	(_S_IWRITE >> 3)
#	endif
#	ifndef S_IXGRP
#		define S_IXGRP (_S_IEXEC >> 3)
#	endif
#	ifndef S_IRWXG
#		define	S_IRWXG (S_IRGRP | S_IWGRP | S_IXGRP)
#	endif
#	ifndef S_IROTH
#		define S_IROTH	(S_IRGRP >> 3)
#	endif
#	ifndef S_IWOTH
#		define S_IWOTH	(S_IWGRP >> 3)
#	endif
#	ifndef S_IXOTH
#		define S_IXOTH (S_IXGRP >> 3)
#	endif
#	ifndef S_IRWXO
#		define	S_IRWXO (S_IROTH | S_IWOTH | S_IXOTH)
#	endif


namespace platform_replace
{

#	undef mkdir
/*!
\brief 修正 MinGW 中的 mkdir 参数问题。
\note 忽略第二参数。
\since build 521
*/
inline int
mkdir(char const* dir, ::mode_t = 0777)
{
	return ::_mkdir(dir);
}

} // namespace platform_replace;


extern "C"
{

/*!
\def mkdir
\brief 修正 MinGW 中的 mkdir 参数问题。
\sa platform_replace::makedir
\since build 298
*/
#	define mkdir platform_replace::makedir


#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
//! \since build 465
YF_API struct ::tm* __cdecl __MINGW_NOTHROW
_gmtime32(const ::__time32_t*);
#	endif

} // extern "C";


namespace platform_ex
{

/*!
\brief 判断 WIN32_FIND_DATAA 指定的节点是否为目录。
\since build 298
*/
inline PDefH(bool, IsDirectory, const ::WIN32_FIND_DATAA& d) ynothrow
	ImplRet(d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
/*!
\brief 判断 WIN32_FIND_DATAW 指定的节点是否为目录。
\since build 299
*/
inline PDefH(bool, IsDirectory, const ::WIN32_FIND_DATAW& d) ynothrow
	ImplRet(d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)

} // namespace platform_ex;

#endif

#endif

