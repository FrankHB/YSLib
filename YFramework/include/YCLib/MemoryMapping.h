/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MemoryMapping.h
\ingroup YCLib
\brief 内存映射文件。
\version r167
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:48:15 +0800
\par 修改时间:
	2016-07-11 18:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#ifndef YCL_INC_MemoryMapping_h_
#define YCL_INC_MemoryMapping_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include YFM_YCLib_Reference // for default_delete, unique_ptr;
#include YFM_YCLib_FileIO
#include YFM_YBaseMacro

namespace platform
{

#if YCL_DS
//! \since build 669
using UnmapDelete = default_delete<byte[]>;
#else
//! \since build 669
class YF_API UnmapDelete
{
public:
	using pointer = byte*;

#	if !YCL_Win32
private:
	size_t size;
#	endif

public:
#	if YCL_Win32
	DefDeCtor(UnmapDelete)
#	else
	UnmapDelete(size_t s)
		: size(s)
	{}
#	endif

	//! \since build 671
	void
	operator()(pointer) const ynothrow;
};
#endif

/*!
\brief 只读内存映射文件。
\note 对不支持内存映射的实现，使用 POSIX 标准 IO 模拟。
\since build 324
*/
class YF_API MappedFile
{
private:
	//! \since build 669
	UniqueFile file;
	std::uint64_t size;
	//! \since build 669
	unique_ptr<byte[], UnmapDelete> addr;

public:
	/*!
	\throw FileOperationFailure 文件打开失败。
	\throw std::runtime_error 映射失败。
	\exception ystdex::narrowing_error Win32 平台：映射的文件大小不被支持。
	*/
	//@{
	//! \since build 412
	explicit
	MappedFile(const char*);
	//! \since build 326
	template<class _tString>
	explicit
	MappedFile(const _tString& filename)
		: MappedFile(filename.c_str())
	{}
	//@}
	DefDeDtor(MappedFile)

	//! \since build 413
	DefGetter(const ynothrow, byte*, Ptr, addr.get())
	//! \since build 475
	DefGetter(const ynothrow, std::uint64_t, Size, size)
};

} // namespace platform;

#endif

