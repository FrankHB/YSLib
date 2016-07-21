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
\version r276
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:48:15 +0800
\par 修改时间:
	2016-07-21 09:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#ifndef YCL_INC_MemoryMapping_h_
#define YCL_INC_MemoryMapping_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include YFM_YCLib_Reference // for ystdex::pseudo_output, unique_ptr,
//	default_delete;
#include YFM_YCLib_FileIO
#include YFM_YBaseMacro

namespace platform
{

//! \since build 711
//@{
//! \brief 文件映射选项。
enum class FileMappingOption
{
	ReadOnly,
	ReadWrite,
	CopyOnWrite
};


/*!
\brief 文件映射键类型。
\note 用于指定映射的标识。
\note 保证默认构造对象可比较，若与之相等时为匿名映射，否则可为共享映射。
\note 为可移植性，可使用 const char16_t* 类型固定值（如字符串字面量转换的指针）。
*/
//@{
#if !YF_Hosted
using FileMappingKey = ystdex::pseudo_output;
#elif YCL_Win32
using FileMappingKey = const char16_t*;
#else
using FileMappingKey = bool;
#endif
//@}
//@}


//! \since build 669
class YF_API UnmapDelete
#if YCL_DS
	: public default_delete<byte[]>
#endif
{
public:
	using pointer = byte*;

private:
	size_t size;

public:
	UnmapDelete(size_t s)
		: size(s)
	{}

#if YCL_DS
	//! \since build 711
	using default_delete<byte[]>::operator();
#else
	//! \since build 671
	void
	operator()(pointer) const ynothrow;
#endif

	//! \since build 711
	DefGetter(const ynothrow, size_t, Size, size)
};


/*!
\brief 只读内存映射文件。
\note 可移植实现仅保证进程内生存期，但进程间可能共享。
\note 对不支持内存映射的实现，使用 POSIX 文件 IO 模拟。
\since build 324
*/
class YF_API MappedFile
{
private:
	//! \since build 669
	UniqueFile file;
	//! \since build 711
	unique_ptr<byte[], UnmapDelete> mapped;

public:
	/*!
	\brief 构造：创建映射文件。
	\exception ystdex::narrowing_error 映射的文件大小不被支持。
	\throw FileOperationFailure 文件打开失败。
	\throw std::runtime_error 嵌套异常：映射失败。
	\note 非宿主平台：忽略第三参数。
	\note DS 平台：使用文件 IO 模拟。
	\since build 711

	创建映射到已存在的文件上的映射。
	若键非空且不使用写时复制选项，创建全局可见的共享映射；
	否则创建修改仅在进程内可见的映射。
	*/
	//@{
	//! \pre 文件可读；使用可写映射时文件可写；文件长度非零。
	//@{
	//! \pre 指定的长度非零；文件包含指定的区域。
	//@{
	/*!
	\brief 以指定的区域偏移、区域长度、文件、选项和键创建映射。
	\note DS 和 POSIX 平台： 偏移截断为 \c ::off_t 。
	\pre 偏移位置按页对齐。
	*/
	explicit
	MappedFile(std::uint64_t, size_t, UniqueFile,
		FileMappingOption = FileMappingOption::ReadOnly, FileMappingKey = {});
	//! \brief 以指定的区域长度、文件、选项和键创建映射。
	//@{
private:
	explicit
	MappedFile(pair<size_t, UniqueFile> pr, FileMappingOption opt,
		FileMappingKey key)
		: MappedFile(pr.first, std::move(pr.second), opt, key)
	{}

public:
	explicit
	MappedFile(size_t, UniqueFile,
		FileMappingOption = FileMappingOption::ReadOnly, FileMappingKey = {});
	//@}
	//@}
	//! \brief 以指定的文件、选项和键创建映射。
	explicit
	MappedFile(UniqueFile,
		FileMappingOption = FileMappingOption::ReadOnly, FileMappingKey = {});
	//@}
	//! \pre 间接断言：第一参数非空。
	explicit YB_NONNULL(2)
	MappedFile(const char*, FileMappingOption = FileMappingOption::ReadOnly,
		FileMappingKey = {});
	template<class _tString, typename... _tParams>
	explicit
	MappedFile(const _tString& filename, _tParams&&... args)
		: MappedFile(filename.c_str(), yforward(args)...)
	{}
	//@}
	/*!
	\brief 析构：刷新并捕获所有错误，然后释放资源。
	\since build 711
	*/
	~MappedFile();

	//! \since build 413
	DefGetter(const ynothrow, byte*, Ptr, mapped.get())
	//! \since build 711
	DefGetterMem(const ynothrow, size_t, Size, mapped.get_deleter())

	/*!
	\brief 刷新：刷新映射视图。
	\note DS 平台：空操作。
	\note POSIX 平台：未指定只读视图关联的永久存储。
	\since build 711
	*/
	void
	FlushView();
};

} // namespace platform;

#endif

