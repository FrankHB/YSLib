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
\version r298
\author FrankHB <frankhb1989@gmail.com>
\since build 604
\par 创建时间:
	2015-06-06 03:01:27 +0800
\par 修改时间:
	2015-06-29 06:17 +0800
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
#	include YFM_YCLib_FileSystem // for platform::FAT, platform::Deref,
//	platform::Concurrency, platform::FileSystemType, std::system_error;
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

public:
	//! \since build 609
	DefGetter(const ynothrow, Disc, Disc, disc)

private:
	PDefH(size_t, GetKey, ::sec_t sec) const ynothrowv
		ImplRet(sec & ~((1 << sectors_per_page_shift) - 1))
	ystdex::block_buffer*
	GetPage(::sec_t) ynothrow;

public:
	//! \since build 608
	YB_NONNULL(4) bool
	EraseWritePartialSector(::sec_t, size_t, const void*, size_t = 1) ynothrowv;

	//! \since build 608
	bool
	FillPartialSector(::sec_t, size_t, size_t = 1, byte = {}) ynothrowv;

	//! \since build 606
	bool
	FillSectors(::sec_t, size_t = 1, byte = {}) ynothrow;

	bool
	Flush() ynothrow;

private:
	bool
	FlushEntry(UsedListCache::value_type&) ynothrow;

	//! \since build 608
	//@{
	template<typename _func>
	bool
	PerformPartialSectorIO(_func f, ::sec_t sec, size_t offset, size_t n)
	{
		if(!(bytes_per_sector < offset + n))
		{
			const auto key(GetKey(sec));

			if(const auto p_entry = GetPage(key))
			{
				f(*p_entry, (sec - key) * bytes_per_sector);
				return true;
			}
		}
		return {};
	}

	template<typename _func>
	bool
	PerformSectorsIO(_func f, ::sec_t& sec, size_t& n)
	{
		while(0 < n)
		{
			const auto key(GetKey(sec));

			if(const auto p_entry = GetPage(key))
			{
				const auto sec_off(sec - key);
				// TODO: Compare and assert 'count' and 'sec_off'?
				const auto secs_to_process(
					std::min<size_t>(GetBlockCount(key) - sec_off, n));

				f(*p_entry, sec_off, secs_to_process);
				yunseq(sec += secs_to_process,  n -= secs_to_process);
			}
			else
				return {};
		}
		return true;
	}

public:
	YB_NONNULL(2) bool
	ReadPartialSector(void*, ::sec_t, size_t, size_t = 1) ynothrowv;

	YB_NONNULL(2) bool
	ReadSectors(void*, ::sec_t, size_t = 1) ynothrowv;

	YB_NONNULL(4) bool
	WritePartialSector(::sec_t, size_t, const void*, size_t = 1) ynothrowv;

	YB_NONNULL(3) bool
	WriteSectors(::sec_t, const void*, size_t = 1) ynothrow;
	//@}
};
//@}

/*!
\brief 基于文件分配表的文件系统扩展接口。
\since build 610
*/
namespace FAT
{

//! \see Microsoft FAT Specification Section 2 。
static_assert(std::is_same<byte, ystdex::octet>::value,
	"Only 8-bit byte is supported.");

static_assert(!(sizeof(size_t) < sizeof(std::uint32_t)),
	"Invalid type found.");

using namespace platform::FAT;
using platform::Deref;
using namespace platform::Concurrency;
using platform::FileSystemType;

// XXX: Mutex lock probable may throw on platforms other than DS. However,
//	ISO C++ [thread.req.exception] guaranteed only specified error conditions
//	are allowed, and all of them in [thread.mutex.requirements.mutex] are
//	carefully avoided. Thus the exception specifications are not effected even
//	if each creation of lock is not in a try-block.


//! \brief 分配表。
class YF_API AllocationTable final
{
private:
	mutable mutex part_mutex{};
	size_t sectors_per_cluster;
	size_t bytes_per_sector;
	size_t bytes_per_cluster;
	::sec_t table_start;
	size_t table_size;
	size_t total_sectors_num;
	size_t limit;
	size_t root_dir_start;
	size_t root_dir_sectors_num;
	size_t data_sectors_num;
	ClusterIndex first_free = Clusters::First;
	ClusterIndex free_cluster = 0;
	ClusterIndex last_alloc_cluster = 0;
	ClusterIndex last_cluster;
	FileSystemType fs_type;
	ClusterIndex root_dir_cluster;

public:
	mutable SectorCache Cache;

	YB_NONNULL(3)
	AllocationTable(::sec_t, const byte*, size_t, size_t, Disc) ynothrowv;

	DefDeCopyAssignment(AllocationTable)

	PDefH(bool, IsValidCluster, ClusterIndex c) const ynothrow
		// NOTE: This will catch Clusters::Error.
		ImplRet(Clusters::First <= c && c <= last_cluster)

	DefGetter(const ynothrow, size_t, BytesPerCluster, bytes_per_cluster)
	DefGetter(const ynothrow, size_t, BytesPerSector, bytes_per_sector)
	DefGetter(const ynothrow, FileSystemType, FileSystemType, fs_type)
	DefGetter(const ynothrow, size_t, FreeClusters,
		fs_type == FileSystemType::FAT32 ? free_cluster : CountFreeCluster())
	DefGetter(const ynothrow, mutex&, MutexRef, part_mutex)
	DefGetter(const ynothrow, ClusterIndex, RootDirCluster, root_dir_cluster)
	DefGetter(const ynothrow, ClusterIndex, RootDirSectorsNum,
		root_dir_sectors_num)
	DefGetter(const ynothrow, size_t, SectorsPerCluster, sectors_per_cluster)
	DefGetter(const ynothrow, std::uint64_t, TotalSize, std::uint64_t(
		data_sectors_num) * std::uint64_t(bytes_per_sector))
	DefGetter(const ynothrow, size_t, UsedClusters,
		last_cluster - Clusters::First + 1)

	PDefH(::sec_t, ClusterToSector, ClusterIndex c) const ynothrow
		ImplRet((c >= Clusters::First ? (c - Clusters::First) * ::sec_t(
			sectors_per_cluster) + root_dir_sectors_num : 0) + root_dir_start)

	bool
	ClearLinks(ClusterIndex) ynothrow;

	ClusterIndex
	CountFreeCluster() const ynothrow;

	ClusterIndex
	LinkFree(ClusterIndex) ynothrow;

private:
	ClusterIndex
	ProbeForNext(ClusterIndex) const ynothrow;

public:
	ClusterIndex
	QueryNext(ClusterIndex) const ynothrow;

	ClusterIndex
	QueryLast(ClusterIndex) const ynothrow;

	YB_NONNULL(2) void
	ReadClusters(const byte*) ynothrowv;

	ClusterIndex
	TrimChain(ClusterIndex, size_t) ynothrow;

	/*!
	\brief 链接空闲空间。
	\exception std::system_error ENOSPC 空间不足。
	*/
	ClusterIndex
	TryLinkFree(ClusterIndex) ythrow(std::system_error);

	YB_NONNULL(2) void
	WriteClusters(byte*) const ynothrowv;

	bool
	WriteEntry(ClusterIndex, std::uint32_t) const ynothrow;

	YB_NONNULL(2) void
	WriteFSInfo(byte*) ynothrowv;
};

} // namespace FAT;

#endif

} // namespace platform_ex;

#endif

