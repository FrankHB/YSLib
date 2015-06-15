/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSIO.h
\ingroup YCLib
\ingroup DS
\brief DS 底层输入输出接口。
\version r143
\author FrankHB <frankhb1989@gmail.com>
\since build 604
\par 创建时间:
	2015-06-06 03:01:27 +0800
\par 修改时间:
	2015-06-13 00:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(DS)::DSIO
*/


#ifndef YCL_DS_INC_DSIO_h_
#define YCL_DS_INC_DSIO_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_YCommon
#include YFM_YCLib_Reference // for lref;
#if YCL_DS
#	include <nds/disc_io.h> // for ::DISC_INTERFACE, ::sec_t;
#	include <ystdex/base.h> // for ystdex::noncopyable, ystdex::nonmomable;
#	include <ystdex/cache.hpp> // for ystdex::used_list_cache;
#	include <ystdex/cstdio.h> //for ystdex::block_buffer;
#endif

namespace platform_ex
{

#if YCL_DS
//! \since build 604
//@{
/*
\brief DS 底层外部存储接口封装。
\see LibNDS 头文件 <nds/disc_io.h> 。
\see LibFAT 源码文件 "source/disc.h" 。
*/
class YF_API Disc : private lref<const ::DISC_INTERFACE>
{
public:
	Disc(const ::DISC_INTERFACE& disc) ynothrow
		: lref(disc)
	{}

	DefPred(const ynothrow, Inserted, get().isInserted)
	DefGetter(const ynothrow, std::uint32_t, Type, get().ioType)
	DefGetter(const ynothrow, std::uint32_t, Features, get().features)

	DefFwdTmpl(const ynothrow, bool, ReadSectors,
		get().readSectors(yforward(args)...))

	PDefH(bool, Shutdown, ) const ynothrow
		ImplRet(get().shutdown())

	PDefH(bool, Starup, ) const ynothrow
		ImplRet(get().startup())

	DefFwdTmpl(const ynothrow, bool, WriteSectors,
		get().writeSectors(yforward(args)...))

	using lref::get;
};


//! \brief 扇区缓冲。
class YF_API SectorCache final : private ystdex::noncopyable,
	private ystdex::nonmovable
{
private:
	using UsedListCache
		= ystdex::used_list_cache<::sec_t, ystdex::block_buffer>;
	Disc disc;
	size_t number_of_pages;
	size_t sectors_per_page_shift;
	::sec_t end_of_partition;
	size_t bytes_per_sector;
	UsedListCache entries;

public:
	SectorCache(size_t, size_t, Disc, ::sec_t, size_t);
	~SectorCache();

private:
	size_t
	GetBlockCount(::sec_t) const ynothrow;
	PDefH(size_t, GetKey, ::sec_t sec) const ynothrowv
		ImplRet(sec & ~((1 << sectors_per_page_shift) - 1))
	ystdex::block_buffer*
	GetPage(::sec_t) ynothrow;

public:
	YB_NONNULL(2) bool
	EraseWritePartialSector(const void*, ::sec_t, size_t, size_t) ynothrowv;

	//! \since build 606
	bool
	FillSectors(::sec_t, size_t = 1, byte = {}) ynothrow;

	bool
	Flush() ynothrow;

private:
	bool
	FlushEntry(UsedListCache::value_type&) ynothrow;

public:
	YB_NONNULL(2) bool
	ReadPartialSector(void*, ::sec_t, size_t, size_t) ynothrowv;

	YB_NONNULL(4) bool
	ReadSectors(::sec_t, size_t, void*) ynothrowv;

	YB_NONNULL(2) bool
	WritePartialSector(const void*, ::sec_t, size_t, size_t) ynothrowv;

	YB_NONNULL(4) bool
	WriteSectors(::sec_t, size_t, const void*) ynothrow;
};
//@}
#endif

} // namespace platform_ex;

#endif

