/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MemoryMapping.h
\ingroup YCLib
\brief 内存映射文件。
\version r122
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:48:15 +0800
\par 修改时间:
	2014-09-03 13:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#ifndef YCL_INC_MemoryMapping_h_
#define YCL_INC_MemoryMapping_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon

namespace platform
{

/*!
\brief 只读内存映射文件。
\note 对不支持内存映射的实现，使用 POSIX 标准 IO 模拟。
\since build 324
*/
class YF_API MappedFile
{
private:
	int fd;
	std::uint64_t size;
	ystdex::byte* addr;

public:
	//! \throw FileOperationFailure 映射失败。
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
	~MappedFile();

	//! \since build 413
	DefGetter(const ynothrow, ystdex::byte*, Ptr, addr)
	//! \since build 475
	DefGetter(const ynothrow, std::uint64_t, Size, size)
};

} // namespace platform;

#endif

