/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.h
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r700
\author FrankHB <frankhb1989@gmail.com>
\since build 202
\par 创建时间:
	2011-04-13 20:26:21 +0800
\par 修改时间:
	2013-12-24 09:34 +0800
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

#define YCL_YCOMMON_H <ycommon.h>
#define YSL_YSBUILD_H <ysbuild.h>

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


#ifdef YCL_API_USE_UNISTD
#include <unistd.h>
#endif

#ifdef YCL_API_USE_SYS_DIR
#include <sys/dir.h>
#endif


#if YCL_DS

#include <nds.h>
#include <fat.h>
//包含 PALib 。
//#include <PA9.h>

#ifdef USE_EFS
//包含 EFSLib 。
#include "efs_lib.h"
#endif

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

#ifndef UNICODE
#	define UNICODE 1
#endif

#ifndef WINVER
//! \since build 448
#	define WINVER 0x0501
#endif

#ifndef WIN32_LEAN_AND_MEAN
//! \since build 448
#	define WIN32_LEAN_AND_MEAN 1
#endif

#include <Windows.h>
#include <direct.h> // for ::_mkdir;
#include <sys/stat.h>

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 DialogBox 宏。
\since build 297
*/
#undef DialogBox

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 DrawText 宏。
\since build 298
*/
#undef DrawText

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 FindWindow 宏。
\since build 381
*/
#undef FindWindow

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 GetObject 宏。
\since build 313
*/
#undef GetObject

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 PostMessage 宏。
\since build 298
*/
#undef PostMessage


#define NAME_MAX 256

#define S_IFIFO		_S_IFIFO

#define	S_IRWXG		(S_IRGRP | S_IWGRP | S_IXGRP)
#define		S_IRGRP	0
#define		S_IWGRP	0
#define		S_IXGRP 0
#define	S_IRWXO		(S_IROTH | S_IWOTH | S_IXOTH)
#define		S_IROTH	0
#define		S_IWOTH	0
#define		S_IXOTH 0


namespace platform_replace
{

/*!
\brief 修正 MinGW 中的 mkdir 参数问题。

忽略第二参数。
*/
inline int
makedir(char const* dir, mode_t)
{
	return ::_mkdir(dir);
}

} // namespace platform_replace;


extern "C"
{

/*!
\def mkdir
\brief 修正 MinGW 中的 mkdir 参数问题。
\see platform_replace::makedir
\since build 298
*/
#define mkdir platform_replace::makedir

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

