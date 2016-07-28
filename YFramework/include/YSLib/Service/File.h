/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file File.h
\ingroup Service
\brief 平台中立的文件抽象。
\version r1538
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2009-11-24 23:14:41 +0800
\par 修改时间:
	2016-07-26 22:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::File
*/


#ifndef YSL_INC_Service_File_h_
#define YSL_INC_Service_File_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities

namespace YSLib
{

namespace IO
{

/*!
\brief 打开文件。
\pre 路径参数非空。
\throw FileOperationFailure 打开失败。
\return 非空的文件指针。
\sa uopen
\since build 639
*/
YF_API YB_NONNULL(1) UniqueFile
OpenFile(const char*, int, mode_t = 0);


/*!
\brief 移除文件链接。
\pre 间接断言：参数非空。
\throw FileOperationFailure 文件存在且操作失败。
\since build 713
*/
//@{
template<typename _tChar>
YB_NONNULL(1) void
Remove(const _tChar* path)
{
	if(YB_UNLIKELY(!uremove(path)))
	{
		const int err(errno);

		if(errno != ENOENT)
			ThrowFileOperationFailure("Failed removing destination file '"
				+ IO::MakePathString(path) + "'.", err);
	}
}

template<typename _tChar>
YB_NONNULL(1) void
Unlink(const _tChar* path)
{
	if(YB_UNLIKELY(!uunlink(path)))
	{
		const int err(errno);

		if(errno != ENOENT)
			ThrowFileOperationFailure("Failed unlinking destination file '"
				+ IO::MakePathString(path) + "'.", err);
	}
}
//@}

} // namespace IO;

} // namespace YSLib;

#endif

