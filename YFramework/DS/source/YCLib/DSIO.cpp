/*
	© 2015-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSIO.cpp
\ingroup YCLib
\ingroup DS
\brief DS 底层输入输出接口。
\version r4952
\author FrankHB <frankhb1989@gmail.com>
\since build 604
\par 创建时间:
	2015-06-06 06:25:00 +0800
\par 修改时间:
	2022-11-28 19:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(DS)::DSIO
*/


#include "YCLib/YModules.h"
#include YFM_DS_YCLib_DSIO // for platform::Descriptions, std::runtime_error,
//	ystdex::throw_error, std::system_error, string, string_view,
//	ystdex::exchange, ystdex::retry_on_cond, YAssertNonnull, FetchSeparator,
//	ystdex::id, std::bind, std::placeholder::_1, ystdex::is_parent_or_self,
//	ystdex::invoke, std::ref, YTraceDe, std::exception,
//	ystdex::trivially_copy_n, ptrdiff_t, DISC_INTERFACE, unique_raw,
//	ystdex::aligned_store_cast;
#if YCL_DS
#	include "YSLib/Core/YModules.h"
#	include YFM_YSLib_Core_YException // for YSLib::TryInvoke,
//	YSLib::FilterExceptions;
#	include <functional> // for std::all_of;
#	include <ystdex/invoke.hpp> // for ystdex::nonvoid_result_t,
//	ystdex::invoke_result_t, ystdex::invoke_for_value;
#	include "CHRLib/YModules.h"
#	include YFM_CHRLib_CharacterProcessing // for ystdex::read_uint_le,
//	ystdex::write_uint_le, CHRLib::MakeUCS2LE, ystdex::ntctsicmp,
//	ystdex::ntctsncpy;
#	include <ystdex/string.hpp> // for ystdex::ltrim;
#	include <cerrno> // for ENOMEM, ENOTSUP, EFBIG, EINVAL;
#	include YFM_YCLib_NativeAPI // for OpenMode;
#	include <ystdex/scope_guard.hpp> // for ystdex::make_guard;
#	include <sys/iosupport.h> // for ::_reent, ::size_t, ::ssize_t, ::off_t,
//	sturct ::stat, struct ::statvfs, ::DIR_ITER, ::devoptab_t;
#endif

namespace platform_ex
{

using namespace platform;
using namespace Descriptions;

#if YCL_DS
SectorCache::SectorCache(size_t pages, size_t spps, Disc d, ::sec_t eop,
	size_t bps)
	: disc(d), number_of_pages(std::max<size_t>(pages, 2)),
	sectors_per_page_shift(std::max<size_t>(spps, 3)), end_of_partition(eop),
	bytes_per_sector(bps),
	entries(number_of_pages)
{
	entries.flush = [this](UsedListCache::value_type& pr) ynothrow{
		FlushEntry(pr);
	};
}
SectorCache::~SectorCache()
{
	Flush();
}

size_t
SectorCache::GetBlockCount(::sec_t sec) const ynothrowv
{
	const auto end_sec(std::min<::sec_t>(
		sec + (1 << sectors_per_page_shift), end_of_partition));

	YAssert(sec <= end_sec, "Invalid range found.");
	return end_sec - sec;
}
ystdex::block_buffer*
SectorCache::GetPage(::sec_t key) ynothrowv
{
	YAssert(GetKey(key) == key, "Invalid key found.");

	return YSLib::TryInvoke([this, key]{
		return &ystdex::cache_lookup(entries, key, [this, key]{
			const auto size(bytes_per_sector << sectors_per_page_shift);
			// TODO: Use aligned allocation for cache implementation.
			// NOTE: Intentionally uninitialized as LibFAT.
			ystdex::block_buffer entry(make_unique_default_init<byte[]>(size));
			const auto n(GetBlockCount(key));

			YAssert(n <= size, "Invalid cache creation parameter found.");
			if(YB_UNLIKELY(!disc.ReadSectors(key, n, entry.get())))
				throw std::runtime_error(
					"Failed reading on creating file system cache entry.");
			return entry;
		});
	});
}

bool
SectorCache::EraseWritePartialSector(::sec_t sec, size_t offset,
	const void* p_buf, size_t n) ynothrowv
{
	return PerformPartialSectorIO(
		[this, offset, p_buf, n](ystdex::block_buffer& entry, size_t sec_bytes){
		ystdex::trivially_fill_n(entry.get() + sec_bytes, bytes_per_sector);
		entry.write(sec_bytes + offset, p_buf, n);
	}, sec, offset, n);
}

bool
SectorCache::FillPartialSector(::sec_t sec, size_t offset, size_t n, byte val)
	ynothrowv
{
	return PerformPartialSectorIO(
		[this, offset, n, val](ystdex::block_buffer& entry, size_t sec_bytes){
		entry.fill(sec_bytes + offset, n, val);
	}, sec, offset, n);
}

bool
SectorCache::FillSectors(::sec_t sec, size_t n, byte val) ynothrow
{
	return PerformSectorsIO([&](ystdex::block_buffer& entry, size_t sec_off,
		size_t secs_to_process){
		entry.fill(sec_off * bytes_per_sector,
			secs_to_process * bytes_per_sector, val);
	}, sec, n);
}

bool
SectorCache::Flush() ynothrow
{
	// TODO: Flush all?
	return std::all_of(entries.begin(), entries.end(),
		std::bind(&SectorCache::FlushEntry, this, std::placeholders::_1));
}

bool
SectorCache::FlushEntry(UsedListCache::value_type& pr) ynothrow
{
	if(pr.second.modified())
	{
		if(!disc.WriteSectors(pr.first, GetBlockCount(pr.first),
			pr.second.get()))
			return {};
		pr.second.flush();
	}
	return true;
}

bool
SectorCache::ReadPartialSector(void* p_buf, ::sec_t sec, size_t offset,
	size_t n) ynothrowv
{
	return PerformPartialSectorIO(
		[this, p_buf, offset, n](ystdex::block_buffer& entry, size_t sec_bytes){
		entry.read(p_buf, sec_bytes + offset, n);
	}, sec, offset, n);
}

bool
SectorCache::ReadSectors(void* p_buf, ::sec_t sec, size_t n) ynothrowv
{
	auto dst(static_cast<byte*>(p_buf));

	return PerformSectorsIO([&](ystdex::block_buffer& entry, size_t sec_off,
		size_t secs_to_process){
		entry.read(dst, sec_off * bytes_per_sector,
			secs_to_process * bytes_per_sector);
		dst += secs_to_process * bytes_per_sector;
	}, sec, n);
}

bool
SectorCache::WritePartialSector(::sec_t sec, size_t offset, const void* p_buf,
	size_t n) ynothrowv
{
	return PerformPartialSectorIO(
		[this, offset, p_buf, n](ystdex::block_buffer& entry, size_t sec_bytes){
		entry.write(sec_bytes + offset, p_buf, n);
	}, sec, offset, n);
}

bool
SectorCache::WriteSectors(::sec_t sec, const void* p_buf, size_t n)
	ynothrow
{
	auto src(static_cast<const byte*>(p_buf));

	return PerformSectorsIO([&](ystdex::block_buffer& entry, size_t sec_off,
		size_t secs_to_process){
		entry.write(sec_off * bytes_per_sector, src,
			secs_to_process * bytes_per_sector);
		src += secs_to_process * bytes_per_sector;
	}, sec, n);
}

namespace FAT
{

//! \since build 610
//@{
// TODO: Support big endian implementations.
using ystdex::read_uint_le;
using ystdex::write_uint_le;
using ystdex::throw_error;
//! \since build 611
//@{
using std::system_error;
using std::errc;
// XXX: Coupled string types.
using CHRLib::MakeUCS2LE;
//@}

namespace
{

template<size_t _vWidth>
bool
ReadLEVal(SectorCache& cache, std::uint32_t& value, ::sec_t sec, size_t offset)
	ynothrow
{
	static yconstexpr const size_t
		n(_vWidth / std::numeric_limits<unsigned char>::digits);
	byte buf[n];

	if(cache.ReadPartialSector(buf, sec, offset, n))
	{
		value = ystdex::read_uint_le<_vWidth>(buf);
		return true;
	}
	return {};
}

template<size_t _vWidth>
bool
WriteLEVal(SectorCache& cache, std::uint32_t value, ::sec_t sec, size_t offset)
	ynothrow
{
	static yconstexpr const size_t
		n(_vWidth / std::numeric_limits<unsigned char>::digits);
	byte buf[n]{};

	write_uint_le<_vWidth>(buf, value);
	return cache.WritePartialSector(sec, offset, buf, n);
}


//! \since build 611
//@{
void
CheckThrowEIO(bool b) ythrow(system_error)
{
	if(YB_UNLIKELY(!b))
		throw_error(errc::io_error);
}

//! \since build 656
void
CheckThrowENOSPC(bool b) ythrow(system_error)
{
	if(YB_UNLIKELY(!b))
		throw_error(errc::no_space_on_device);
}

template<class _tPos>
YB_ATTR_nodiscard ::sec_t
PosToSec(const AllocationTable& tbl, const _tPos& pos) ynothrow
{
	return tbl.ClusterToSector(pos.GetCluster()) + pos.GetSector();
}
//@}

//! \since build 713
//@{
PDefH(void, CheckFillPartialSectorOff, const AllocationTable& tbl,
	const FilePosition& pos, size_t offset, size_t n) ythrow(system_error)
	ImplExpr(CheckThrowEIO(
		tbl.Cache.FillPartialSector(PosToSec(tbl, pos), offset, n)))

PDefH(void, CheckFillPartialSector, const AllocationTable& tbl,
	const FilePosition& pos, size_t n) ythrow(system_error)
	ImplExpr(CheckFillPartialSectorOff(tbl, pos, pos.GetByte(), n))

PDefH(void, CheckFillSectors, const AllocationTable& tbl,
	const FilePosition& pos) ythrow(system_error)
	ImplExpr(CheckThrowEIO(tbl.Cache.FillSectors(PosToSec(tbl, pos))))

template<class _tPos>
void
CheckReadPartialSectorOff(const AllocationTable& tbl, const _tPos& pos,
	void* data, size_t offset, size_t n) ythrow(system_error)
{
	CheckThrowEIO(
		tbl.Cache.ReadPartialSector(data, PosToSec(tbl, pos), offset, n));
}

PDefH(void, CheckReadPartialSector, const AllocationTable& tbl,
	const DEntryPosition& pos, void* data) ythrow(system_error)
	ImplExpr(CheckReadPartialSectorOff(tbl, pos, data, pos.GetByte(),
		EntryDataSize))
PDefH(void, CheckReadPartialSector, const AllocationTable& tbl,
	const FilePosition& pos, void* data, size_t n) ythrow(system_error)
	ImplExpr(CheckReadPartialSectorOff(tbl, pos, data, pos.GetByte(), n))

PDefH(void, CheckReadSectors, const AllocationTable& tbl, ClusterIndex c,
	void* buf, size_t n) ythrow(system_error)
	ImplExpr(CheckThrowEIO(
		tbl.Cache.ReadSectors(buf, tbl.ClusterToSector(c), n)))
PDefH(void, CheckReadSectors, const AllocationTable& tbl,
	const FilePosition& pos, void* buf, size_t n) ythrow(system_error)
	ImplExpr(CheckThrowEIO(
		tbl.Cache.ReadSectors(buf, PosToSec(tbl, pos), n)))

template<class _tPos>
void
CheckWritePartialSectorOff(const AllocationTable& tbl, const _tPos& pos,
	const void* data, size_t offset, size_t n) ythrow(system_error)
{
	CheckThrowEIO(
		tbl.Cache.WritePartialSector(PosToSec(tbl, pos), offset, data, n));
}

PDefH(void, CheckWritePartialSector, const AllocationTable& tbl,
	const DEntryPosition& pos, const void* data) ythrow(system_error)
	ImplExpr(CheckWritePartialSectorOff(tbl, pos, data, pos.GetByte(),
		EntryDataSize))
PDefH(void, CheckWritePartialSector, const AllocationTable& tbl, const
	FilePosition& pos, const void* data, size_t n) ythrow(system_error)
	ImplExpr(CheckWritePartialSectorOff(tbl, pos, data, pos.GetByte(), n))

PDefH(void, CheckWriteSectors, const AllocationTable& tbl, ClusterIndex c,
	const void* buf, size_t n) ythrow(system_error) ImplRet(CheckThrowEIO(
	tbl.Cache.WriteSectors(tbl.ClusterToSector(c), buf, n)))
PDefH(void, CheckWriteSectors, const AllocationTable& tbl, const FilePosition&
	pos, const void* buf, size_t n) ythrow(system_error)
	ImplRet(CheckThrowEIO(tbl.Cache.WriteSectors(PosToSec(tbl, pos), buf, n)))
//@}

//! \since build 656
YB_ATTR_nodiscard bool
CompareEntryNames(EntryData& data, string_view comp, const string& name)
	ynothrow
{
	try
	{
		if(comp.length() == name.length()
			&& ystdex::ntctsicmp(MakeUCS2LE(comp).c_str(),
			MakeUCS2LE(name).c_str(), comp.length()) == 0)
			return true;
	}
	CatchIgnore(...)
	try
	{
		if(data.FindAlias(comp))
			return true;
	}
	CatchIgnore(...)
	return {};
}

//! \since build 656
void
AssignValidCluster(Partition& part, FilePosition& pos, ClusterIndex next_start)
	ythrow(system_error)
{
	CheckThrowEIO(part.Table.IsValid(next_start));
	pos = {next_start, 0, pos.GetByte()};
}

} // unnamed namespace;
//@}

AllocationTable::AllocationTable(::sec_t start_sector, const byte* sec_buf,
	size_t pages, size_t sectors_per_page_shift, Disc d)
	ynothrowv
	: sectors_per_cluster(sec_buf[BPB_SecPerClus]),
	bytes_per_sector([sec_buf, this]{
		const auto t(read_uint_le<16>(sec_buf + BPB_BytsPerSec));

		// TODO: Use interval API.
		if(t < MinSectorSize || bytes_per_sector > MaxSectorSize)
			throw std::runtime_error("Unsupported sector size found.");
		return t;
	}()), bytes_per_cluster(bytes_per_sector * sectors_per_cluster),
	table_start(start_sector + read_uint_le<16>(sec_buf + BPB_RsvdSecCnt)),
	table_size([sec_buf]() ynothrowv{
		// NOTE: last_cluster = cluster_count + Clusters::First - 1.
		const auto t(read_uint_le<16>(sec_buf + BPB_FATSz16));

		return t == 0 ? size_t(
			read_uint_le<32>(sec_buf + BPB_FATSz32)) : size_t(t);
	}()), total_sectors_num([sec_buf]() ynothrowv{
		const auto t(read_uint_le<16>(sec_buf + BPB_TotSec16));

		return t == 0 ? read_uint_le<32>(sec_buf + BPB_TotSec32)
			: std::uint32_t(t);
	}()), limit(total_sectors_num / sectors_per_cluster),
	root_dir_start(table_start + sec_buf[BPB_NumFATs] * table_size),
	// NOTE: See Microsoft FAT Specification Section 3.5.
	root_dir_sectors_num((read_uint_le<16>(sec_buf + BPB_RootEntCnt)
	* EntryDataSize + (bytes_per_sector - 1)) / bytes_per_sector),
	data_sectors_num(total_sectors_num - size_t(root_dir_start
	+ root_dir_sectors_num - start_sector)),
	fs_type([start_sector, this]() ynothrowv -> FileSystemType{
		// NOTE: See Microsoft FAT Specification Section 3.5.
		ClusterIndex cluster_count(data_sectors_num / sectors_per_cluster);

		last_cluster = cluster_count + Clusters::First - 1;
		// TODO: Distinguish part type (e.g. FAT16B) with file system type.
		if(cluster_count < Clusters::PerFAT12)
			return FileSystemType::FAT12;
		if(cluster_count < Clusters::PerFAT16)
			return FileSystemType::FAT16;
		return FileSystemType::FAT32;
	}()), root_dir_cluster([sec_buf, this]() ynothrowv -> ClusterIndex{
		if(fs_type != FileSystemType::FAT32)
			return Clusters::FAT16RootDirectory;
		// NOTE: Check if FAT mirroring is enabled.
		if((sec_buf[BPB_ExtFlags] & 0x80) == 0)
			// NOTE: Use the active FAT.
			table_start += table_size * (sec_buf[BPB_ExtFlags] & 0x0F);
		return read_uint_le<32>(sec_buf + BPB_RootClus);
	}()), Cache(pages, sectors_per_page_shift, d,
	start_sector + total_sectors_num, bytes_per_sector)
{}

bool
AllocationTable::ClearLinks(ClusterIndex c) ynothrow
{
	if(IsValid(c))
	{
		if(c < first_free)
			first_free = c;
		while(IsValid(c))
		{
			WriteEntry(ystdex::exchange(c, QueryNext(c)), Clusters::Free);
			if(free_count < limit)
				++free_count;
		}
		return true;
	}
	return {};
}

ClusterCount
AllocationTable::CountFreeCluster() const ynothrow
{
	size_t count(0);

	for(ClusterIndex cur_cluster(Clusters::First); cur_cluster <= last_cluster;
		++cur_cluster)
		if(QueryNext(cur_cluster) == Clusters::Free)
			++count;
	return count;
}

ClusterIndex
AllocationTable::LinkFree(ClusterIndex c) ythrow(system_error)
{
	// TODO: Remove redundant check of Clusters::Error.
	if(c <= last_cluster)
	{
		const auto cur_link(QueryNext(c));

		if(IsValid(cur_link))
		{
			if(cur_link != Clusters::Error)
				return cur_link;
			else
				throw_error(errc::no_space_on_device);
		}

		auto first_valid(std::max<ClusterIndex>(first_free, Clusters::First));
		bool loop_around = {};

		while(QueryNext(first_valid) != Clusters::Free)
			if(last_cluster < ++first_valid)
			{
				if(loop_around)
				{
					first_free = first_valid;
					throw_error(errc::no_space_on_device);
				}
				else
				{
					first_valid = Clusters::First;
					loop_around = true;
				}
			}
		first_free = first_valid;
		if(free_count != 0)
			--free_count;
		last_alloc_cluster = first_valid;
		if(IsValid(c))
			WriteEntry(c, first_valid);
		WriteEntry(first_valid, Clusters::EndOfFile);
		if(IsValid(first_valid) && first_valid != Clusters::Error)
			return first_valid;
	}
	throw_error(errc::no_space_on_device);
}

ClusterIndex
AllocationTable::QueryNext(ClusterIndex c) const ynothrowv
{
	YAssert(IsFreeOrValid(c), "Invalid allocated cluster found.");

	ClusterIndex next_cluster(Clusters::Free);

	if(c != Clusters::Free)
		switch(fs_type)
		{
		case FileSystemType::FAT12:
			{
				const auto c32(c * 3 / 2);
				::sec_t sector(table_start + c32 / bytes_per_sector);
				size_t offset(c32 % bytes_per_sector);

				ReadLEVal<8>(Cache, next_cluster, sector, offset);
				if(++offset == bytes_per_sector)
				{
					offset = 0;
					++sector;
				}

				std::uint32_t next_cluster_h(0);

				ReadLEVal<8>(Cache, next_cluster_h, sector, offset);
				next_cluster |= next_cluster_h << 8;
			}
			if(c & 0x01)
				next_cluster = next_cluster >> 4;
			else
				next_cluster &= 0x0FFF;
			if(Clusters::MaxValid12 < next_cluster)
				next_cluster = Clusters::EndOfFile;
			break;
		case FileSystemType::FAT16:
			ReadLEVal<16>(Cache, next_cluster, table_start + (c << 1)
				/ bytes_per_sector, c % (bytes_per_sector >> 1) << 1);
			if(Clusters::MaxValid16 < next_cluster)
				next_cluster = Clusters::EndOfFile;
			break;
		case FileSystemType::FAT32:
			ReadLEVal<32>(Cache, next_cluster, table_start + (c << 2)
				/ bytes_per_sector, c % (bytes_per_sector >> 2) << 2);
			if(Clusters::MaxValid32 < next_cluster)
				next_cluster = Clusters::EndOfFile;
			break;
		default:
			return Clusters::Error;
		}
	return next_cluster;
}

ClusterIndex
AllocationTable::QueryLast(ClusterIndex c) const ynothrow
{
	return ystdex::retry_on_cond([&, this]() ynothrow{
		return IsValid(c);
	}, [&]() ynothrow{
		return ystdex::exchange(c, QueryNext(c));
	});
}

bool
AllocationTable::SynchronizeClusters(byte* sec_buf) ynothrowv
{
	free_count = read_uint_le<32>(sec_buf + FSI_Free_Count);
	if(free_count == Clusters::UnknownFreeCount)
	{
		UpdateFSInfo(sec_buf);
		return true;
	}
	else
		last_alloc_cluster = read_uint_le<32>(sec_buf + FSI_Nxt_Free);
	return {};
}

ClusterIndex
AllocationTable::TrimChain(ClusterIndex start, size_t len) ythrow(system_error)
{
	auto next(QueryNext(start));

	while(len-- != 0)
	{
		CheckThrowEIO(IsValid(next));
		start = next;
		next = QueryNext(start);
	}
	if(IsValid(next))
		ClearLinks(next);
	WriteEntry(start, Clusters::EndOfFile);
	return start;
}

ClusterIndex
AllocationTable::LinkFreeCleared(ClusterIndex c) ythrow(system_error)
{
	const auto t(LinkFree(c));

	YAssert(t != Clusters::Error, "Invalid cluster found.");
	for(size_t i(0); i < sectors_per_cluster; ++i)
		CheckThrowEIO(Cache.FillSectors(ClusterToSector(t) + i));
	return t;
}

void
AllocationTable::UpdateFSInfo(byte* sec_buf) ynothrowv
{
	YAssertNonnull(sec_buf);
	free_count = CountFreeCluster();
	WriteClusters(sec_buf);
}

void
AllocationTable::WriteClusters(byte* sec_buf) const ynothrowv
{
	write_uint_le<32>(sec_buf + FSI_Free_Count, free_count),
	write_uint_le<32>(sec_buf + FSI_Nxt_Free, last_alloc_cluster);
}

bool
AllocationTable::WriteEntry(ClusterIndex c, std::uint32_t val) const ynothrow
{
	if(IsValid(c))
	{
		switch(fs_type)
		{
		case FileSystemType::FAT12:
			{
				const auto c32(c * 3 / 2);
				::sec_t sector(table_start + (c32 / bytes_per_sector));
				size_t offset(c32 % bytes_per_sector);
				std::uint32_t old_val;

				if((c & 0x01) != 0)
				{
					ReadLEVal<8>(Cache, old_val, sector, offset);
					val = val << 4 | (old_val & 0x0F);
					WriteLEVal<8>(Cache, byte(val), sector, offset);
					if(++offset >= bytes_per_sector)
						yunseq(offset = 0, ++sector);
					WriteLEVal<8>(Cache, byte(val >> 8), sector, offset);
				}
				else
				{
					WriteLEVal<8>(Cache, val, sector, offset);
					if(++offset >= bytes_per_sector)
						yunseq(offset = 0, ++sector);
					ReadLEVal<8>(Cache, old_val, sector, offset);
					val = ((val >> 8) & 0x0F) | (old_val & 0xF0);
					WriteLEVal<8>(Cache, val, sector, offset);
				}
			}
			break;
		case FileSystemType::FAT16:
			WriteLEVal<16>(Cache, val, table_start + (c << 1)
				/ bytes_per_sector, (c % (bytes_per_sector >> 1)) << 1);
			break;
		case FileSystemType::FAT32:
			WriteLEVal<32>(Cache, val, table_start + (c << 2)
				/ bytes_per_sector, (c % (bytes_per_sector >> 2)) << 2);
			break;
		default:
			return {};
		}
		return true;
	}
	return {};
}


void
FilePosition::Decompose(std::uint32_t pos, size_t bytes_per_cluster,
	size_t bytes_per_sector) ynothrowv
{
	YAssert(bytes_per_cluster != 0, "Invalid parameter found."),
	YAssert(bytes_per_sector != 0, "Invalid parameter found.");
	yunseq(sector = pos % bytes_per_cluster / bytes_per_sector,
		byte = pos % bytes_per_sector);
}


//! \since build 900
namespace
{

YB_ATTR_nodiscard ClusterIndex
ReadClusterFromEntry_FAT12OrFAT16(const AllocationTable& table,
	const EntryData& data) ythrow(system_error)
{
	const auto dst(data.data());
	ClusterIndex c(read_uint_le<16>(dst + EntryData::Cluster));

	CheckThrowEIO(table.IsFreeOrValid(c));
	return c;
}

YB_ATTR_nodiscard ClusterIndex
ReadClusterFromEntry_FAT32(const AllocationTable& table,
	const EntryData& data) ythrow(system_error)
{
	const auto dst(data.data());
	// NOTE: High 16 bits of start cluster are only used when they are certainly
	//	correctly defined.
	ClusterIndex c(read_uint_le<16>(dst + EntryData::Cluster)
		| read_uint_le<16>(dst + EntryData::ClusterHigh) << 16);

	CheckThrowEIO(table.IsFreeOrValid(c));
	return c;
}

} // unnamed namespace;


DEntry::DEntry(const Partition& part)
	: NamePos{DEntryPosition{0}, DEntryPosition{0}}, name(".")
{
	// TODO: Optimize?
	Data.SetupRoot(part.GetRootDirCluster());
}
DEntry::DEntry(Partition& part, const NamePosition& name_pos)
	: NamePos(name_pos), name([&]{
		auto pos(name_pos[0]);
		char16_t long_name[LFN::MaxLength];

		ystdex::retry_on_cond([&](bool finished){
			part.ExtendPosition(pos);
			return !finished;
		}, [&]() -> bool{
			EntryData edata;

			CheckReadPartialSector(part.Table, pos, edata.data());
			if(pos == name_pos[1])
				return Data = edata, true;
			else
				edata.CopyLFN(long_name);
			return {};
		});
		return name_pos[0] == name_pos[1] ? Data.GenerateAlias()
			: LFN::ConvertToMBCS(long_name);
	}())
{}
DEntry::DEntry(Partition& part, string_view sv, LeafAction act,
	ClusterIndex& dclus)
{
	if(IsSimpleAbsolute(sv))
	{
		const auto pos(sv.find_first_not_of(FetchSeparator<char>()));

		if(pos == string_view::npos)
			goto found_root;
		sv.remove_prefix(pos),
		dclus = part.GetRootDirCluster();
	}
	else
		dclus = part.GetCWDCluster();
	ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
		const auto next_pos(sv.find(FetchSeparator<char>()));

		if(next_pos == string_view::npos)
		{
			ConsLeafNoCallback(part, sv, act, dclus);
			return {};
		}
		return ConsBranch(part, sv, next_pos, dclus);
	});
	if(IsFAT32Root(part))
	{
found_root:
		ConsReset(part);
		if(act == LeafAction::EnsureDirectory)
			dclus = part.GetRootDirCluster();
	}
}
DEntry::DEntry(Partition& part, string_view sv, LeafAction act,
	ystdex::function<void(DEntry&)> add_entry, ClusterIndex& dclus)
{
	YAssertNonnull(add_entry);
	YAssert(act != LeafAction::EnsureDirectory, "Unsupported action found.");
	if(IsSimpleAbsolute(sv))
	{
		const auto pos(sv.find_first_not_of(FetchSeparator<char>()));

		if(pos == string_view::npos)
			throw_error(errc::invalid_argument);
		sv.remove_prefix(pos),
		dclus = part.GetRootDirCluster();
	}
	else
		dclus = part.GetCWDCluster();
	ystdex::retry_on_cond(ystdex::id<>(), [&]() -> bool{
		const auto next_pos(sv.find(FetchSeparator<char>()));

		if(next_pos == string_view::npos)
		{
			ConsLeafWithCallback(part, sv, act, dclus);
			return {};
		}
		return ConsBranch(part, sv, next_pos, dclus);
	});
	// XXX: For a file system with external link to other file systems, the
	//	order is significant: this check cannot be moved earlier. However, this
	//	is not the case in file systems not supporting symbolic links like FAT.
	//	Just keep it now.
	if(part.IsReadOnly())
		throw_error(errc::read_only_file_system);
#	if false
	ystdex::ltrim(name, ' ');
#	endif
	if(YB_UNLIKELY(LFN::MaxMBCSLength < name.length()))
		throw_error(errc::filename_too_long);
	if(name.empty()
		|| name.find_first_of(LFN::IllegalCharacters) != string::npos)
		throw_error(errc::invalid_argument);
	add_entry(*this);

	const auto name_len(name.length());

	yunused(name_len);
	YAssert(name_len == ystdex::ntctslen(name.c_str())
		&& !(LFN::MaxMBCSLength < name_len) && name_len != 0
		&& name.find_first_of(LFN::IllegalCharacters)
		== string::npos, "Invalid name found.");
	if(YB_UNLIKELY(!LFN::ValidateName(name)))
		throw_error(errc::invalid_argument);
	if(!part.EntryExists(name, dclus))
	{
		EntryDataUnit alias_check_sum(0);
		size_t entry_size;

		std::tie(alias_check_sum, entry_size) = Data.FillNewName(name,
			std::bind(&Partition::EntryExists, &part, std::placeholders::_1,
			std::ref(dclus)));
		FindEntryGap(part, dclus, entry_size);
		YAssert(entry_size != 0, "Invalid entry size found.");

		auto cur_entry_pos(NamePos[0]);
		EntryData long_name_entry;
		const auto& long_name(MakeUCS2LE(name));
		size_t i(entry_size);

		ystdex::retry_on_cond([&](bool cont) ynothrow{
			return part.IncrementPosition(cur_entry_pos)
				== ExtensionResult::Success && cont;
		}, [&]() -> bool{
			if(i > 1)
			{
				long_name_entry[LFN::Ordinal] = (i - 1)
					| (i == entry_size ? LFN::LastLongEntry : 0);
				for(size_t j(0); j < LFN::EntryLength; ++j)
					// NOTE: Padding vs terminating null character.
					write_uint_le<16>(long_name_entry.data()
						+ LFN::OffsetTable[j], long_name[(i - 2)
						* LFN::EntryLength + j] == char16_t() ? (j > 1
						&& long_name[(i - 2) * LFN::EntryLength + j - 1]
						== char16_t() ? 0xFFFF : 0x0000)
						: long_name[(i - 2) * LFN::EntryLength + j]);
				yunseq(
				long_name_entry[LFN::CheckSum] = alias_check_sum,
				long_name_entry[LFN::Flag]
					= EntryDataUnit(Attribute::LongFileName),
				long_name_entry[LFN::Reserved1] = 0
				);
				write_uint_le<16>(
					long_name_entry.data() + LFN::Reserved2, 0);
				CheckWritePartialSector(part.Table, cur_entry_pos,
					long_name_entry.data());
				--i;
				return true;
			}
			else
				CheckWritePartialSector(part.Table, cur_entry_pos,
					Data.data());
			return {};
		});
		return;
	}
	else
		throw_error(errc::file_exists);
	if(IsFAT32Root(part))
		throw_error(errc::invalid_argument);
}

bool
DEntry::IsFAT32Root(const Partition& part) const
{
	// NOTE: On FAT32 an actual cluster should be specified for the root
	//	entry, not cluster 0 as on FAT16.
	return part.GetFileSystemType() == FileSystemType::FAT32
		&& Data.IsDirectory() && ReadClusterFromEntry_FAT32(part.Table, Data)
		== Clusters::Root;
}

bool
DEntry::ConsBranch(Partition& part, string_view& sv, size_t next_pos,
	ClusterIndex& dclus)
{
	YAssertNonnull(sv.data());
	string_view comp(sv.data(), next_pos);

	if(LFN::MaxMBCSLength < comp.length())
		throw_error(errc::filename_too_long);
	if(dclus == part.GetRootDirCluster() && ystdex::is_parent_or_self(comp))
		ConsReset(part);
	else
	{
		name = string(ystdex::rtrim(comp, ' '));
		NamePos = GenerateBeforeFirstNamePos(dclus);

		string comp_name;

		ystdex::retry_on_cond(ystdex::id<>(),
			[&, this]() -> bool{
			if(!QueryNextFrom(part))
				throw_error(errc::no_such_file_or_directory);
			return !(Data.IsDirectory() && CompareEntryNames(Data, comp, name));
		});
	}
	ConsCheckDirectory(part, dclus);
	sv.remove_prefix(next_pos);
	sv.remove_prefix(std::min(
		sv.find_first_not_of(FetchSeparator<char>()), sv.length()));
	return !sv.empty();
}

void
DEntry::ConsCheckDirectory(const Partition& part, ClusterIndex& dclus)
{
	if(Data.IsDirectory())
	{
		dclus = part.ReadClusterFromEntry(part.Table, Data);
		if(dclus == Clusters::Root)
			dclus = part.GetRootDirCluster();
	}
	else
		throw_error(errc::not_a_directory);
}

void
DEntry::ConsLeafNoCallback(Partition& part, string_view comp, LeafAction act,
	ClusterIndex& dclus)
{
	YAssertNonnull(comp.data());

	if(LFN::MaxMBCSLength < comp.length())
		throw_error(errc::filename_too_long);
	if(dclus == part.GetRootDirCluster() && ystdex::is_parent_or_self(comp))
		ConsReset(part);
	else
	{
		name = string(ystdex::rtrim(comp, ' '));
		NamePos = GenerateBeforeFirstNamePos(dclus);

		string comp_name;

		ystdex::retry_on_cond(ystdex::id<>(),
			[&, this]() -> bool{
			if(!QueryNextFrom(part))
				throw_error(errc::no_such_file_or_directory);
			return !CompareEntryNames(Data, comp, name);
		});
	}
	if(act == LeafAction::ThrowExisted)
		throw_error(errc::file_exists);
	if(act != LeafAction::Return)
		ConsCheckDirectory(part, dclus);
}

void
DEntry::ConsLeafWithCallback(Partition& part, string_view comp, LeafAction act,
	ClusterIndex& dclus)
{
	YAssertNonnull(comp.data());
	YAssert(act != LeafAction::EnsureDirectory, "Unsupported action found.");

	if(LFN::MaxMBCSLength < comp.length())
		throw_error(errc::filename_too_long);
	if(dclus == part.GetRootDirCluster() && ystdex::is_parent_or_self(comp))
		throw_error(errc::invalid_argument);
	name = string(ystdex::rtrim(comp, ' '));
	// NOTE: The check is not performed here. Let it go into the callback.
	if(act == LeafAction::ThrowExisted)
	{
		NamePos = GenerateBeforeFirstNamePos(dclus);
		string comp_name(name);

		ystdex::retry_on_cond(ystdex::id<>(),
			[&, this]() -> bool{
			if(!QueryNextFrom(part))
			{
				name = comp_name;
				return {};
			}
			return !CompareEntryNames(Data, comp, name);
		});
	}
}

void
DEntry::FindEntryGap(Partition& part, ClusterIndex dclus, size_t size)
	ythrow(system_error)
{
	// NOTE: Scan for free entry.
	DEntryPosition gap_end(dclus), gap_start(gap_end);
	bool end_of_dir{};
	auto dentry_remain(size);
	EntryData edata;

	if(size != 0)
		while(true)
		{
			CheckReadPartialSector(part.Table, gap_end, edata.data());
			if(edata[0] == EntryData::Last || edata[0] == EntryData::Free)
			{
				if(dentry_remain == size)
					gap_start = gap_end;
				--dentry_remain;
				end_of_dir = edata[0] == EntryData::Last;
			}
			else
				dentry_remain = size;
			if(!end_of_dir && dentry_remain > 0)
				part.ExtendPosition(gap_end);
			else
				break;
		}
	NamePos[0] = gap_start;
	if(end_of_dir)
	{
		edata.FillLast();
		// NOTE: Increase by one to take account of End Of Directory Marker.
		++dentry_remain;
		while(dentry_remain > 0)
		{
			NamePos[1] = gap_end;
			part.ExtendPosition(gap_end);
			// NOTE: Fill the entry with blanks.
			CheckWritePartialSector(part.Table, gap_end, edata.data());
			--dentry_remain;
		}
	}
	else
		NamePos[1] = gap_end;
}

bool
DEntry::QueryNextFrom(Partition& part) ythrow(system_error)
{
	auto estart(NamePos[1]);

	if(estart.IsFAT16RootCluster())
		estart.SetCluster(part.GetRootDirCluster());

	auto eend(estart);
	EntryData edata;
	char16_t long_name[LFN::MaxLength];
	bool has_long_name = {};
	EntryDataUnit chk_sum = 0;

	while(part.IncrementPosition(eend) == ExtensionResult::Success)
	{
		CheckReadPartialSector(part.Table, eend, edata.data());
		if(edata.IsLongFileName())
		{
			const auto ord(edata[LFN::Ordinal]);

			// NOTE: See Microsoft FAT Specification Section 7.1.
			if((ord & 0x80) != 0)
				// NOTE: Corrupted.
				has_long_name = {};
			else if((ord & LFN::LastLongEntry) != 0)
				yunseq(
				estart = eend, has_long_name = true,
				long_name[std::min<size_t>((ord & ~LFN::LastLongEntry)
					* LFN::EntryLength, LFN::MaxLength - 1)] = char16_t(),
				chk_sum = edata[LFN::CheckSum]
				);
			else if(chk_sum != edata[LFN::CheckSum])
				has_long_name = {};
			if(has_long_name)
				edata.CopyLFN(long_name);
		}
		else if(edata.IsVolume())
			;
		else if(edata[0] == EntryData::Last)
			break;
		else if(edata[0] != EntryData::Free && edata[0] > 0x20)
		{
			if(has_long_name
				&& LFN::GenerateAliasChecksum(edata.data()) != chk_sum)
			{
				has_long_name = {};
				name.clear();
			}
			if(has_long_name)
				TryExpr(name = LFN::ConvertToMBCS(long_name))
				CatchRet(..., {})
			else
			{
				estart = eend;
				TryExpr(name = edata.GenerateAlias())
				CatchExpr(..., name.clear())
			}
			yunseq(NamePos[0] = estart, NamePos[1] = eend, Data = edata);
			return true;
		}
	}
	return {};
}


Partition::Partition(Disc d, size_t pages, size_t sectors_per_page_shift,
	::sec_t start_sector)
	// TODO: Use aligned allocation.
	// NOTE: Uninitialized intentionally here to make it behave as LibFAT.
	// TODO: Pool allocation? Use temporary buffer?
	: Partition(make_unique_default_init<byte[]>(MaxSectorSize).get(), d, pages,
	sectors_per_page_shift, start_sector)
{}
Partition::Partition(byte* sec_buf, Disc d, size_t pages,
	size_t sectors_per_page_shift, ::sec_t start_sector)
	: disc(d), Table([&, sec_buf]{
		// NOTE: LibFAT uses uncached reading here.
		// NOTE: LibFAT ignores the error and return sector number as 0 here.
		// NOTE: Different to LibFAT 1.1.2, there is no need to read TWL
		//	partitions in DSi NAND, so unnecessary restrictions of TWL
		//	signatures are avoided. Thus, %FAT::CheckValidMBR is used directly.
		//	This is also conforming to Microsoft FAT specification.
		try
		{
			CheckThrowEIO(ReadSector(start_sector, sec_buf));
			if(sec_buf[Signature_word] != 0x55
				|| sec_buf[Signature_word + 1] != 0xAA)
				throw std::runtime_error("Invalid MBR or boot sector found in"
					" start sector specified for partition.");
			if(start_sector == 0 && !CheckValidMBR(sec_buf))
			{
				start_sector = FindFirstValidPartition(sec_buf);
				CheckThrowEIO(ReadSector(start_sector, sec_buf));
			}
			if(!CheckValidMBR(sec_buf))
				throw std::runtime_error("Failed verifying FAT partition.");
		}
		catch(...)
		{
			std::throw_with_nested(
				std::runtime_error("Failed reading partition."));
		}
		GetMutexRef().lock();
		ystdex::ntctsncpy(label.data(), sec_buf + (MatchFATSignature(sec_buf
			+ FAT16::BS_FilSysType) ? size_t(FAT16::BS_VolLab)
			: size_t(FAT32::BS_VolLab)), label.size());
		return start_sector;
	}(), sec_buf, pages, sectors_per_page_shift, d),
	ReadClusterFromEntry(GetFileSystemType() == FileSystemType::FAT32
	? ReadClusterFromEntry_FAT32 : ReadClusterFromEntry_FAT12OrFAT16),
	fs_info_sector(start_sector + (read_uint_le<16>(sec_buf + BPB_FSInfo)
	? read_uint_le<16>(sec_buf + BPB_FSInfo) : 1)),
	read_only((disc.GetFeatures() & FEATURE_MEDIUM_CANWRITE) == 0),
	cwd_cluster(GetRootDirCluster())
{
	if(GetFileSystemType() == FileSystemType::FAT32)
		ReadFSInfo();
	GetMutexRef().unlock();
}
Partition::~Partition()
{
	YSLib::FilterExceptions([this]{
		lock_guard<mutex> lck(GetMutexRef());

		for(const auto& file : open_files)
			// XXX: Flush after exceptions occurred?
			TryExpr(file.get().SyncToDisc())
			CatchExpr(system_error&, YTraceDe(Err,
				"Synchronization of file failed @ Partition::~Partition."))
		if(GetFileSystemType() == FileSystemType::FAT32)
			WriteFSInfo();
	}, yfsig, Warning);
}

void
Partition::CheckPositionForNextCluster(FilePosition& pos) ythrow(system_error)
{
	const auto s(pos.GetSector());
	const auto spc(GetSectorsPerCluster());

	if(YB_UNLIKELY(spc < s))
		// XXX: Internal error.
		throw_error(errc::invalid_argument);

	const auto c(pos.GetCluster());

	if(YB_UNLIKELY(!Table.IsFreeOrValid(c)))
		// XXX: Internal error.
		throw_error(errc::invalid_argument);
	if(s == spc)
	{
		auto t(Table.QueryNext(c));

		pos = {Clusters::IsFreeOrEOF(t)
			? Table.LinkFree(pos.GetCluster()) : t, 0, pos.GetByte()};
	}
}

void
Partition::ChangeDir(string_view path) ythrow(system_error)
{
	DEntry(*this, path, LeafAction::EnsureDirectory, cwd_cluster);
}

void
Partition::CreateFSInfo()
{
	YAssert(GetFileSystemType() == FileSystemType::FAT32,
		"Only FAT32 is supported.");
	if(!read_only)
	{
		// TODO: Use aligned allocation.
		auto p_sector_buf(make_unique<byte[]>(GetBytesPerSector()));
		const auto sec_buf(p_sector_buf.get());

		write_uint_le<32>(sec_buf + FSI_LeadSig, FSI_LeadSig_Value),
		write_uint_le<32>(sec_buf + FSI_StrucSig, FSI_StrucSig_Value),
		write_uint_le<32>(sec_buf + FSI_TrailSig, FSI_TrailSig_Value),
		Table.UpdateFSInfo(sec_buf);
	}
}

bool
Partition::EntryExists(string_view comp, ClusterIndex dclus)
	ythrow(system_error)
{
	if(comp.length() < LFN::MaxMBCSLength)
	{
		DEntry entry(dclus);

		while(entry.QueryNextFrom(*this))
			if(CompareEntryNames(entry.Data, comp, entry.GetName()))
				return true;
	}
	return {};
}

void
Partition::ExtendPosition(DEntryPosition& entry_pos) ythrow(system_error)
{
	switch(IncrementPosition(entry_pos))
	{
		case ExtensionResult::NoSpace:
			throw_error(errc::no_space_on_device);
		case ExtensionResult::EndOfFile:
			entry_pos.SetCluster(
				Table.LinkFreeCleared(entry_pos.GetCluster()));
		default:
			break;
	}
}

::sec_t
Partition::FindFirstValidPartition(byte* sec_buf) const
	ythrow(std::system_error)
{
	CheckThrowEIO(ReadSector(0, sec_buf));

	byte part_table[16 * 4];
	byte* ptr(part_table);

	// TODO: Resolve magic numbers.
	ystdex::trivially_copy_n(sec_buf + 0x1BE, sizeof(part_table),
		part_table);
	for(size_t i(0); i < 4; yunseq(++i, ptr += 16))
	{
		const ::sec_t part_lba(read_uint_le<32>(ptr + 0x8));

		// NOTE: For MBR vaildity, see the comments in the constructor above.
		if(CheckValidMBR(sec_buf))
			return part_lba;
		if(ptr[4] == 0)
			continue;
		if(ptr[4] == 0x0F)
		{
			::sec_t part_lba2(part_lba), next_lba2(0);

			// NOTE: Up to 8 logic partitions.
			for(size_t n(0); n < 8; ++n)
			{
				CheckThrowEIO(ReadSector(part_lba + next_lba2, sec_buf));
				// TODO: Resolve magic numbers.
				part_lba2 = part_lba + next_lba2
					+ read_uint_le<32>(sec_buf + 0x1C6);
				next_lba2 = read_uint_le<32>(sec_buf + 0x1D6);
				CheckThrowEIO(ReadSector(part_lba2, sec_buf));
				if(CheckValidMBR(sec_buf))
					return part_lba2;
				if(next_lba2 == 0)
					break;
			}
		}
		else
		{
			CheckThrowEIO(ReadSector(part_lba, sec_buf));
			if(CheckValidMBR(sec_buf))
				return part_lba;
		}
	}
	return 0;
}

ExtensionResult
Partition::IncrementPosition(DEntryPosition& entry_pos) ynothrowv
{
	// TODO: Reevaluate if it is the wrong on writing.
	YAssert(Table.IsFreeOrValid(entry_pos.GetCluster()),
		"Invalid allocated cluster found.");

	auto pos(entry_pos);

	if(pos.IncOffset() == GetBytesPerSector() / EntryDataSize)
	{
		pos.IncSectorAndResetOffset();

		const auto s(pos.GetSector());
		const bool root(pos.IsFAT16RootCluster());

		if(!root && s == GetSectorsPerCluster())
		{
			pos.SetSector(0);

			const auto c(Table.QueryNext(pos.GetCluster()));

			// NOTE: Cover 'c == Clusters::EndOfFile'.
			if(!Table.IsValid(c))
				return ExtensionResult::EndOfFile;
			pos.SetCluster(c);
		}
		else if(root && s == Table.GetRootDirSectorsNum())
			return ExtensionResult::NoSpace;
	}
	entry_pos = pos;
	return ExtensionResult::Success;
}

void
Partition::MakeDir(string_view path) ythrow(system_error)
{
	ClusterIndex parent_clus, dclus;

	DEntry(*this, path, LeafAction::Return,
		[&](DEntry& de){
		de.Data.Clear();
		de.Data.SetDirectoryAttribute();
		de.Data.WriteDateTime();
		dclus = Table.LinkFreeCleared(Clusters::Free);
		de.Data.WriteCluster(dclus);
		// XXX: Only %ENOSPC may be expected.
	}, parent_clus);

	// NOTE: Create the dot entry within the directory.
	EntryData edata;
	auto& cache(GetCacheRef());
	const auto dir_sector(Table.ClusterToSector(dclus));

	edata.SetupRoot(dclus);
	// XXX: POSIX does not require %EIO here.
	CheckThrowENOSPC(cache.EraseWritePartialSector(dir_sector, 0,
		edata.data(), EntryDataSize));
	// NOTE: Create the double dot entry within the directory.
	edata.SetDot(EntryData::Name + 1);
	// NOTE: if parent directory is root then ".." always link to
	//	Cluster 0.
	edata.WriteCluster(parent_clus == GetRootDirCluster()
		? ClusterIndex(Clusters::FAT16RootDirectory) : parent_clus);
	CheckThrowENOSPC(cache.WritePartialSector(dir_sector, EntryDataSize,
		edata.data(), EntryDataSize));
	Flush();
}

void
Partition::ReadFSInfo()
{
	YAssert(GetFileSystemType() == FileSystemType::FAT32,
		"Only FAT32 supported.");

	// TODO: Use aligned allocation.
	auto p_sector_buf(make_unique<byte[]>(GetBytesPerSector()));
	const auto sec_buf(p_sector_buf.get());

	if(ReadFSInfoSector(sec_buf))
	{
		if(read_uint_le<32>(sec_buf + FSI_LeadSig) != FSI_LeadSig_Value
			|| read_uint_le<32>(sec_buf + FSI_StrucSig) != FSI_StrucSig_Value
			|| read_uint_le<32>(sec_buf + FSI_Free_Count) == 0)
			CreateFSInfo();
		else if(Table.SynchronizeClusters(sec_buf))
		{
			if(YB_UNLIKELY(!disc.WriteSectors(fs_info_sector, 1, sec_buf)))
				YTraceDe(Err, "Writing sectors failed on creating file system"
					" information.");
		}
	}
}

void
Partition::Rename(string_view old, string_view new_name) ythrow(system_error)
{
	YAssertNonnull(new_name.data());
	if(!new_name.empty())
	{
		if(FetchPartitionFromPath(new_name.data()) == this)
		{
			// FIXME: %errc::invalid_argument: The old pathname names an
			//	ancestor directory of the new pathname.
			DEntry old_dir_entry(*this, old);

			DEntry(*this, new_name, LeafAction::Return,
				[&](DEntry& de) ynothrow{
				yunseq(de.Data = old_dir_entry.Data,
					de.NamePos = old_dir_entry.NamePos);
				// XXX: Only %ENOSPC may be expected?
			});
			// FIXME: Exception safety.
			RemoveEntry(old_dir_entry.NamePos);
		}
		else
			throw_error(errc::cross_device_link);
	}
	else
		throw_error(errc::no_such_file_or_directory);
}

void
Partition::StatFS(struct ::statvfs& st)
{
	if(GetFileSystemType() == FileSystemType::FAT32)
		ReadFSInfo();

	const auto free_count(Table.GetFreeClusters());

	yunseq(
	// NOTE: FAT clusters = POSIX blocks.
	st.f_bsize = GetBytesPerCluster(),
	st.f_frsize = GetBytesPerCluster(),
	st.f_blocks = Table.GetUsedClusters(),
	st.f_bfree = free_count,
	st.f_bavail = free_count,
	// NOTE: Treat requests for info on inodes as clusters.
	st.f_files = Table.GetUsedClusters(),
	st.f_ffree = free_count,
	st.f_favail = free_count,
	st.f_fsid = disc.GetType(),
	// NOTE: No support for %ST_ISUID and %ST_ISGID.
	st.f_flag = ST_NOSUID | (read_only ? ST_RDONLY : 0),
	st.f_namemax = LFN::MaxMBCSLength
	);
}

void
Partition::RemoveEntry(const DEntry::NamePosition& np) ythrow(system_error)
{
	// NOTE: Create an empty directory entry to overwrite the old ones.
	auto name_pos(np);
	EntryData edata;

	ystdex::retry_on_cond([&]() ynothrow{
		return !(name_pos[0] == name_pos[1]);
	}, [&]{
		CheckReadPartialSector(Table, name_pos[0], edata.data());
		edata[0] = EntryData::Free;
		CheckWritePartialSector(Table, name_pos[0], edata.data());
		CheckThrowEIO(
			IncrementPosition(name_pos[0]) == ExtensionResult::Success);
	});
	Flush();
}

void
Partition::StatFromEntry(const EntryData& data, struct ::stat& st) const
	ythrow(system_error)
{
	const auto dst(data.data());

	st.st_ino = ::ino_t(ReadClusterFromEntry(Table, data));
	yunseq(
	st.st_dev = disc.GetType(),
	// NOTE: The file serial number is the start cluster.
	st.st_mode = mode_t(data.IsDirectory() ? Mode::Directory : Mode::Regular)
		| mode_t(Mode::Read) | (data.IsWritable() ? mode_t(Mode::Write)
		: mode_t()),
	// NOTE: Always one hard link on a FAT file.
	st.st_nlink = 1,
	// NOTE: Faked for FAT.
	st.st_uid = 1, st.st_gid = 2,
	st.st_size = read_uint_le<32>(dst + EntryData::FileSize),
	st.st_atime = ConvertFileTime(read_uint_le<16>(dst + EntryData::ADate), 0),
	st.st_mtime = ConvertFileTime(read_uint_le<16>(
		dst + EntryData::MDate), read_uint_le<16>(dst + EntryData::MTime)),
	st.st_ctime = ConvertFileTime(read_uint_le<16>(
		dst + EntryData::CDate), read_uint_le<16>(dst + EntryData::CTime)),
	// XXX: See https://github.com/devkitPro/newlib/commit/72ff9acad2ab54e80a19ddaec0106065c817e3f6
	//	and https://sourceware.org/legacy-ml/newlib/2019/msg00490.html.
#if __NEWLIB__ * 100 + __NEWLIB_MINOR__ > 301
	// XXX: This is now more accruate than libfat. See also https://github.com/devkitPro/libfat/issues/19.
	st.st_atim.tv_nsec = 0, st.st_mtim.tv_nsec = 0, st.st_ctim.tv_nsec = 0,
#endif
	st.st_blksize = GetBytesPerSector()
	);

	const auto bps(GetBytesPerSector());

	yunseq(st.st_rdev = st.st_dev, st.st_blocks = (st.st_size + bps - 1) / bps);
}

void
Partition::Unlink(string_view path) ythrow(system_error)
{
	if(!read_only)
	{
		DEntry dentry(*this, path);
		const auto c(ReadClusterFromEntry(Table, dentry.Data));

		if(dentry.Data.IsDirectory())
		{
			DEntry dir_contents(c);

			while(dir_contents.QueryNextFrom(*this))
				if(!dir_contents.IsDot())
					// XXX: Currently there is no %::unlinkat implementation
					//	in newlib. See also https://github.com/devkitPro/libfat/issues/10.
					throw_error(errc::operation_not_permitted);
		}
		if(Table.ClearLinks(c))
		{
			// FIXME: Exception safety.
			RemoveEntry(dentry.NamePos);
			return;
		}
		throw_error(errc::io_error);
	}
	throw_error(errc::read_only_file_system);
}

void
Partition::WriteFSInfo() const
{
	YAssert(GetFileSystemType() == FileSystemType::FAT32,
		"Only FAT32 supported.");

	// TODO: Use aligned allocation.
	auto p_sector_buf(make_unique<byte[]>(GetBytesPerSector()));
	const auto sec_buf(p_sector_buf.get());

	if(!ReadFSInfoSector(sec_buf))
		throw std::runtime_error(
			"Failed reading first sector on writing file system information.");
	// TODO: Support big endian implementations.
	if(read_uint_le<32>(sec_buf + FSI_LeadSig) != FSI_LeadSig_Value
		|| read_uint_le<32>(sec_buf + FSI_StrucSig) != FSI_StrucSig_Value)
		throw std::runtime_error("Failed validating FSInfo signature on"
			" writing file system information.");
	Table.WriteClusters(sec_buf);
	if(!WriteSector(fs_info_sector, sec_buf))
		throw std::runtime_error(
			"Failed writing file system information sector.");
}


Partition*
FetchPartitionFromPath(const char* path) ynothrowv
{
	// NOTE: A %::chdir or %::setDefaultDevice call is needed before relative
	//	paths available here.
	if(const auto p_devops = ::GetDeviceOpTab(Nonnull(path)))
		return static_cast<Partition*>(p_devops->deviceData);
	return {};
}


DirState::DirState(Partition& part, string_view path) ythrow(system_error)
	ythrow(system_error)
	: part_ref(part), start_cluster([&]{
		ClusterIndex dclus;

		DEntry(part, path, LeafAction::EnsureDirectory, dclus);
		return dclus;
	}()), current_entry(start_cluster),
	valid_entry(current_entry.QueryNextFrom(GetPartitionRef()))
{}

int
DirState::Iterate(char* filename, struct ::stat* filestat) ythrow(system_error)
{
	if(valid_entry)
	{
		auto& part(GetPartitionRef());

		// TODO: Optimize.
		ystdex::ntctsncpy(Nonnull(filename), current_entry.GetName().c_str(),
			std::min<size_t>(current_entry.GetName().length() + 1, NAME_MAX));
		if(filestat)
			part.StatFromEntry(current_entry.Data, *filestat);
		valid_entry = current_entry.QueryNextFrom(part);
		return 0;
	}
	return -1;
	//	throw_error(errc::no_such_file_or_directory);
	// XXX: Even this throws (with %ENOENT) here, it is still conforming to
	//	POSIX which specifies that reaching end of directory stream shall not
	//	change %errno in the %::readdir call, because the implementation in the
	//	newlib (libsysbase, as of devkitARM r55) would restore the old %errno in
	//	such case. Such manner is relied on to get the end of directory state
	//	with %op_dir_locked.  Returning directly saves a throw/catch in such
	//	case, so it is preferred now. This avoids the dependency on the behavior
	//	of restoring %errno of the %::readdir implemention, also eventually same
	//	to LibFAT at current, see https://github.com/devkitPro/libfat/pull/9.
	//	Note anyway, there is no chance to distinguish the condition with a
	//	broken file system/media.
}

void
DirState::Reset() ythrow(system_error)
{
	current_entry.NamePos = DEntry::GenerateBeforeFirstNamePos(start_cluster);
	valid_entry = current_entry.QueryNextFrom(GetPartitionRef());
}


FileInfo::FileInfo(Partition& part, string_view path, int flags)
{
	const auto oflags((OpenMode(flags)));

	switch(oflags & OpenMode::AccessMode)
	{
	case OpenMode::ReadWrite:
		attr.set(WriteBit);
		YB_ATTR_fallthrough;
	case OpenMode::Read:
		attr.set(ReadBit);
		break;
	case OpenMode::Write:
		attr.set(WriteBit);
		break;
	default:
		throw_error(errc::permission_denied);
	}
	// NOTE: %O_CREAT when file not exist would be check later.
	if((CanWrite() || bool(oflags & OpenMode::Truncate)) && part.IsReadOnly())
		throw_error(errc::read_only_file_system);

	lock_guard<mutex> lck(part.GetMutexRef());
	const auto do_init([&, this](DEntry& de){
		if(de.Data.IsDirectory() && CanWrite())
			throw_error(errc::is_a_directory);
		file_size = de.Data.ReadFileSize();
#	if false
		// NOTE: Allow LARGEFILEs with undefined results. Make sure that the
		//	file size can fit in the available space.
		if(!(flags & OpenMode::LargeFile) && file_size >= (1 << 31))
			throw_error(errc::file_too_large);
#	endif
		// XXX: Extension on file level.
		if(CanWrite() && !de.Data.IsWritable())
			throw_error(errc::read_only_file_system);
		yunseq(part_ptr = &part,
			start_cluster = part.ReadClusterFromEntry(part.Table, de.Data));
		if(bool(oflags & OpenMode::Truncate) && CanWrite()
			&& start_cluster != 0)
		{
			part.Table.ClearLinks(start_cluster);
			yunseq(start_cluster = Clusters::Free, file_size = 0,
				attr.set(ModifiedBit));
		}
		yunseq(name_position = de.NamePos, current_position = 0,
			rw_position = {start_cluster});
		if(bool(oflags & OpenMode::Append))
		{
			attr.set(AppendBit);
			// Set append pointer to the end of the file
			// TODO: Use %FilePosition::Decompose.
			append_position = {part.Table.QueryLast(start_cluster), file_size
				% part.GetBytesPerCluster() / part.GetBytesPerSector(),
				file_size % part.GetBytesPerSector()};
			// NOTE: Check if the end of the file is on the end of a cluster.
			if(file_size > 0 && file_size % part.GetBytesPerCluster() == 0)
				// NOTE: Set flag to allocate a new cluster
				append_position = {append_position.GetCluster(),
					part.GetSectorsPerCluster(), 0};
		}
		else
			// NOTE: Guarantee all members initialized.
			append_position = rw_position;
		Deref(part.LockOpenFiles()).insert(*this);
	});

	const auto act((oflags & OpenMode::CreateExclusive)
		== OpenMode::CreateExclusive ? LeafAction::ThrowExisted
		: LeafAction::Return);

	if(bool(oflags & OpenMode::Create))
	{
		DEntry dentry(part, path, act, [this](DEntry& de) ynothrow{
			attr.set(ModifiedBit);
			de.Data.Clear();
			de.Data.WriteCDateTime();
			// FIXME: Only %ENOSPC may be expected.
		});

		do_init(dentry);
	}
	else
	{
		DEntry dentry(part, path, act);

		do_init(dentry);
	}
}

void
FileInfo::CheckAppend() ynothrow
{
	if(CanAppend())
		append_position = rw_position;
}

void
FileInfo::Extend() ythrow(system_error)
{
	auto& part(GetPartitionRef());
	// NOTE: It is assumed that %start_cluster is always valid. This will be
	//	true when %Extend is called from %write_r.
	// TODO: Use %FilePosition::Decompose?
	const auto bpc(part.GetBytesPerCluster());
	FilePosition file_pos{part.Table.QueryLast(start_cluster),
		file_size % bpc / part.GetBytesPerSector(), file_size % bpc};

	YAssert(file_size < current_position, "Invalid position found.");

	size_t remain(current_position - file_size);

	if(remain > 0 && file_size > 0 && file_pos.GetSector() == 0
		&& file_pos.GetByte() == 0)
		file_pos = {part.Table.LinkFree(file_pos.GetCluster()), 0,
			file_pos.GetByte()};
	if(remain + file_pos.GetByte() < part.GetBytesPerSector())
	{
		// NOTE: Only need to clear to the end of the sector.
		CheckFillPartialSector(part.Table, file_pos, remain);
		file_pos.AddByte(remain);
	}
	else
	{
		if(file_pos.GetByte() > 0)
		{
			const auto delta(part.GetBytesPerSector() - file_pos.GetByte());

			CheckFillPartialSector(part.Table, file_pos, delta);
			remain -= delta;
			file_pos.IncSectorAndResetOffset();
		}
		while(remain >= part.GetBytesPerSector())
		{
			if(file_pos.GetSector() >= part.GetSectorsPerCluster())
			file_pos = {part.Table.LinkFree(file_pos.GetCluster()),
				0, file_pos.GetByte()};
			CheckFillSectors(part.Table, file_pos);
			remain -= part.GetBytesPerSector();
			file_pos.IncSector();
		}
		if(remain != 0)
		{
			part.CheckPositionForNextCluster(file_pos);
			file_pos.SetByte(remain);
			CheckFillPartialSectorOff(part.Table, file_pos, 0, remain);
		}
	}
	rw_position = file_pos;
	file_size = current_position;
}

::ssize_t
FileInfo::Read(char* buf, size_t nbyte) ythrow(system_error)
{
	auto& part(GetPartitionRef());

	if(nbyte != 0 && current_position < file_size
		&& start_cluster != Clusters::Free)
	{
		if(file_size < nbyte + current_position)
			nbyte = file_size - current_position;

		auto remain(nbyte);
		auto pos(rw_position);
		// NOTE: Align to sector.
		auto tmp(std::min<size_t>(part.GetBytesPerSector() - pos.GetByte(),
			remain));

		if(tmp < part.GetBytesPerSector())
		{
			CheckReadPartialSector(part.Table, pos, buf, tmp);
			yunseq(remain -= tmp, buf += tmp),
			pos.AddByte(tmp);
			if(pos.GetByte() >= part.GetBytesPerSector())
				pos.IncSectorAndResetOffset();
		}
		// NPTE: Align to cluster, %tmp is number of sectors to read.
		tmp = remain > (part.GetSectorsPerCluster() - pos.GetSector())
			* part.GetBytesPerSector() ? part.GetSectorsPerCluster()
			- pos.GetSector() : remain / part.GetBytesPerSector();
		if(tmp > 0)
		{
			CheckReadSectors(part.Table, pos, buf, tmp);

			const auto n(tmp * part.GetBytesPerSector());

			yunseq(buf += n, remain -= n),
			pos.AddSector(tmp);
		}
		// NOTE: Move onto next cluster. It should get to here without reading
		//	anything if a cluster is due to be allocated.
		if(pos.GetSector() >= part.GetSectorsPerCluster())
		{
			const auto next_start(part.Table.QueryNext(pos.GetCluster()));

			if(remain == 0 && next_start == Clusters::EndOfFile)
				pos.SetSector(part.GetSectorsPerCluster());
			else
				AssignValidCluster(part, pos, next_start);
		}
		// NOTE: Read in whole clusters, contiguous blocks at a time.
		while(remain >= part.GetBytesPerCluster())
		{
			size_t chunk_size(0);
			ClusterIndex next_start(pos.GetCluster()), chunk_end(
				ystdex::retry_on_cond([&](ClusterIndex end) ynothrow{
					return next_start == end + 1
						&& chunk_size + part.GetBytesPerCluster() <= remain;
				}, [&]() ynothrow{
					chunk_size += part.GetBytesPerCluster();
					return ystdex::exchange(next_start,
						part.Table.QueryNext(next_start));
				}));

			CheckReadSectors(part.Table, pos.GetCluster(), buf,
				chunk_size / part.GetBytesPerSector());
			yunseq(buf += chunk_size, remain -= chunk_size);
			if(remain == 0 && next_start == Clusters::EndOfFile)
				pos = {chunk_end, part.GetSectorsPerCluster(), pos.GetByte()};
			else
				AssignValidCluster(part, pos, next_start);
		}
		// NOTE: %tmp is number of sectors left.
		tmp = remain / part.GetBytesPerSector();
		if(tmp > 0)
		{
			CheckReadSectors(part.Table, pos.GetCluster(), buf, tmp);

			const auto n(tmp * part.GetBytesPerSector());

			yunseq(buf += n, remain -= n),
			pos.AddSector(tmp);
		}
		if(remain > 0)
		{
			CheckReadPartialSectorOff(part.Table, pos, buf, 0, remain);
			pos.AddByte(remain);
			remain = 0;
		}
		nbyte -= remain;
		yunseq(rw_position = pos, current_position += nbyte);
		return nbyte;
	}
	return 0;
}

::off_t
FileInfo::Seek(::off_t offset, int whence) ythrow(system_error)
{
	::off_t new_pos(offset);

	switch(whence)
	{
	case SEEK_SET:
		break;
	case SEEK_CUR:
		new_pos += ::off_t(current_position);
		break;
	case SEEK_END:
		new_pos += ::off_t(file_size);
		break;
	default:
		throw_error(errc::invalid_argument);
	}

	auto& part(GetPartitionRef());

	if(!(0 < offset && new_pos < 0))
	{
		// NOTE: %new_pos can only be larger than the %MaxFileSize on platforms
		//	where ::off_t is larger than 32 bits.
		if(!(new_pos < 0
			|| (sizeof(new_pos) > 4 && new_pos > ::off_t(MaxFileSize))))
		{
			auto pos(static_cast<std::uint32_t>(new_pos));

			if(pos <= file_size && start_cluster != Clusters::Free)
			{
				ptrdiff_t clus_count(pos / part.GetBytesPerCluster());
				ClusterIndex cluster(start_cluster);

				if(pos >= current_position)
				{
					ptrdiff_t
						cur_count(current_position / part.GetBytesPerCluster());
					if(rw_position.GetSector() == part.GetSectorsPerCluster())
						--cur_count;
					yunseq(clus_count -= cur_count,
						cluster = rw_position.GetCluster());
				}
				rw_position.Decompose(pos, part.GetBytesPerCluster(),
					part.GetBytesPerSector());
				for(auto next_cluster(part.Table.QueryNext(cluster));
					clus_count > 0 && part.Table.IsValid(next_cluster);
					next_cluster = part.Table.QueryNext(cluster))
				{
					--clus_count;
					cluster = next_cluster;
				}
				if(clus_count > 0)
				{
					if(clus_count == 1 && file_size == pos
						&& rw_position.GetSector() == 0)
						rw_position = {rw_position.GetCluster(),
							part.GetSectorsPerCluster(), 0};
					else
						throw_error(errc::invalid_argument);
				}
				rw_position.SetCluster(cluster);
			}
			current_position = pos;
			return pos;
		}
		throw_error(errc::invalid_argument);
	}
	throw_error(errc::value_too_large);
}

void
FileInfo::Stat(struct ::stat& st) const
{
	auto& part(GetPartitionRef());
	EntryData edata;

	CheckReadPartialSector(part.Table, name_position[1], edata.data());
	part.StatFromEntry(edata, st);
	// NOTE: Update fields that have changed since the file was opened.
	yunseq(st.st_ino = ::ino_t(start_cluster), st.st_size = file_size);
}

void
FileInfo::SyncToDisc() ythrow(system_error)
{
	if(CanWrite() && IsModified())
	{
		auto& part(GetPartitionRef());
		EntryData dir_entry_data;
		const auto data(dir_entry_data.data());
		const auto& dir_entry_end(GetNamePosition()[1]);
		const auto c(GetStartCluster());
		const auto date_time(FetchDateTime());

		CheckReadPartialSector(part.Table, dir_entry_end, data);
		write_uint_le<32>(data + EntryData::FileSize, GetSize()),
		dir_entry_data.WriteCluster(c),
		write_uint_le<16>(data + EntryData::MTime, date_time.second),
		write_uint_le<16>(data + EntryData::MDate, date_time.first),
		write_uint_le<16>(data + EntryData::ADate, date_time.first);
		data[EntryData::Attributes] |= EntryDataUnit(Attribute::Archive);
		CheckWritePartialSector(part.Table, dir_entry_end, data);
		part.GetCacheRef().Flush();
	}
	attr.set(ModifiedBit, {});
}

void
FileInfo::Truncate(FileSize new_size) ythrow(system_error)
{
	auto& part(GetPartitionRef());

	if(new_size > GetSize())
	{
		if(start_cluster == Clusters::Free)
		{
			ClusterIndex t(part.Table.LinkFree(Clusters::Free));

			yunseq(start_cluster = t, rw_position = {t});
		}

		const FilePosition saved_position(GetRWPosition());
		const FileSize saved_offset(GetCurrentPosition());

		SetCurrentPosition(new_size);
		Extend();
		CheckAppend();
		SetRWPosition(saved_position),
		SetCurrentPosition(saved_offset);
	}
	else if(new_size < GetSize())
	{
		if(new_size == 0)
		{
			part.Table.ClearLinks(GetStartCluster());
			yunseq(start_cluster = Clusters::Free,
				append_position = {Clusters::Free});
		}
		else
		{
			// NOTE: Trim the file down to the required size. Drop the unneeded
			//	end of the cluster chain. If the end falls on a cluster
			//	boundary, drop that cluster too, then set a flag to allocate
			//	a cluster as needed.
			const auto last_cluster(part.Table.TrimChain(GetStartCluster(),
				(new_size - 1) / part.GetBytesPerCluster()));

			if(CanAppend())
			{
				const auto re(new_size % part.GetBytesPerCluster());

				SetAppendPosition({last_cluster, re == 0
					? part.GetSectorsPerCluster()
					: re / part.GetBytesPerSector(),
					new_size % part.GetBytesPerSector()});
			}
		}
	}
	UpdateSize(new_size);
}

::ssize_t
FileInfo::Write(const char* buf, size_t nbyte) ythrow(system_error)
{
	// FIXME: Only write up to the maximum file size, taking wrap-around of
	//	integers into account. Overflow?
	if(nbyte + file_size > MaxFileSize || nbyte + file_size < file_size)
		nbyte = MaxFileSize - file_size;
	if(nbyte != 0)
	{
		auto& part(GetPartitionRef());
		size_t remain(nbyte);

		if(start_cluster == Clusters::Free)
		{
			start_cluster = part.Table.LinkFree(Clusters::Free);
			yunseq(append_position = {start_cluster},
				rw_position = {start_cluster});
		}

		bool flag_appending(CanAppend());
		auto pos(flag_appending ? append_position : rw_position);

		if(!flag_appending)
		{
			if(current_position > file_size)
				Extend();
			if(nbyte + current_position > file_size)
				flag_appending = true;
		}
		if(remain != 0)
			part.CheckPositionForNextCluster(pos);

		// NOTE: Align to sector.
		size_t tmp(std::min<size_t>(part.GetBytesPerSector() - pos.GetByte(),
			remain));

		if(tmp < part.GetBytesPerSector())
		{
			CheckWritePartialSector(part.Table, pos, buf, tmp);
			yunseq(remain -= tmp, buf += tmp);
			pos.AddByte(tmp);
			if(pos.GetByte() >= part.GetBytesPerSector())
				pos.IncSectorAndResetOffset();
		}
		// NOTE: Align to sector. %tmp is number of sectors to write.
		tmp = remain > (part.GetSectorsPerCluster() - pos.GetSector())
			* part.GetBytesPerSector() ? part.GetSectorsPerCluster()
			- pos.GetSector() : remain / part.GetBytesPerSector();
		if(tmp > 0 && tmp < part.GetSectorsPerCluster())
		{
			CheckWriteSectors(part.Table, pos, buf, tmp);

			const auto n(tmp * part.GetBytesPerSector());

			yunseq(buf += n, remain -= n),
			pos.AddSector(tmp);
		}
		while(remain >= part.GetBytesPerCluster())
		{
			if(remain != 0)
				part.CheckPositionForNextCluster(pos);

			auto chunk_end(pos.GetCluster()), next_start(chunk_end);
			size_t chunk_size(part.GetBytesPerCluster());
			FilePosition next_position(pos);

			// NOTE: Group consecutive clusters.
			while(chunk_size + part.GetBytesPerCluster() < remain)
			{
				// NOTE: Pretend to use up all sectors in %next_position.
				next_position.SetSector(part.GetSectorsPerCluster());
				// TODO: != ?
				if(remain > chunk_size)
					part.CheckPositionForNextCluster(next_position);
				next_start = next_position.GetCluster();
				if(next_start != chunk_end + 1)
					break;
				yunseq(chunk_end = next_start,
					chunk_size += part.GetBytesPerCluster());
			}
			CheckWriteSectors(part.Table, pos.GetCluster(), buf,
				chunk_size / part.GetBytesPerSector());
			yunseq(buf += chunk_size, remain -= chunk_size);
			if(chunk_end != next_start && part.Table.IsValid(next_start))
				// NOTE: New cluster is already allocated (because it was not
				//	consecutive).
				pos = {next_start, 0, pos.GetByte()};
			else
				pos = {chunk_end, part.GetSectorsPerCluster(), pos.GetByte()};
		}
		if(remain != 0)
			part.CheckPositionForNextCluster(pos);
		// NOTE: %tmp is number of sectors left.
		tmp = remain / part.GetBytesPerSector();
		if(tmp > 0)
		{
			CheckWriteSectors(part.Table, pos.GetCluster(), buf, tmp);

			const auto n(tmp * part.GetBytesPerSector());

			yunseq(buf += n, remain -= n),
			pos.AddSector(tmp);
		}
		if(remain > 0)
		{
			if(flag_appending)
				CheckThrowEIO(part.GetCacheRef().EraseWritePartialSector(
					PosToSec(part.Table, pos), 0, buf, remain));
			else
				CheckWritePartialSectorOff(part.Table, pos, buf, 0, remain);
			pos.AddByte(remain);
			remain = 0;
		}
		yunseq(nbyte -= remain, attr.set(ModifiedBit));
		if(CanAppend())
		{
			append_position = pos;
			file_size += nbyte;
		}
		else
		{
			rw_position = pos;
			current_position += nbyte;
			if(file_size < current_position)
				file_size = current_position;
		}
	}
	return nbyte;
}

//! \since build 611
namespace
{

template<typename _func, typename... _tParams>
using FilterRes = ystdex::nonvoid_result_t<
	ystdex::invoke_result_t<_func, _tParams...>, int>;

inline PDefH(int, seterr, int& re, int e) ynothrow
	ImplRet(re = e, -1)
YB_NONNULL(1) inline PDefH(int, seterr, ::_reent* r, int e) ynothrow
	// NOTE: The newlib reentrant environment pointer shall not be null in a
	//	%::devoptab_t operation.
	ImplRet(Deref(r)._errno = e, -1)

template<typename _func>
auto
FilterDevOps(::_reent* r, _func f) ynothrowv -> FilterRes<_func>
{
	using fres_t = decltype(f());
	using res_t = ystdex::nonvoid_result_t<fres_t, int>;
	static yconstexpr const auto
		de_val(std::is_pointer<fres_t>::value ? res_t() : res_t(-1));

	TryRet(ystdex::invoke_for_value(0, f))
	CatchExpr(std::system_error& e, seterr(r, e.code().value()))
	CatchExpr(std::bad_alloc&, seterr(r, ENOMEM))
	CatchExpr(..., YAssert(false, "Invalid exception found."))
	return de_val;
}

//! \since build 702
template<typename _fCallable>
YB_NONNULL(1) auto
op_path(::_reent* r, const char*& path, _fCallable f) ynothrowv
	-> FilterRes<_fCallable, Partition&>
{
	return FilterDevOps(r, [=, &path]{
		if(Deref(path) != char())
		{
			if(const auto p_part = FetchPartitionFromPath(path))
			{
				// XXX: %EINVAL is not a POSIX error for path.
				path = CheckColons(path);
				return ystdex::invoke(f, *p_part);
			}
			// XXX: This is not a POSIX error.
			throw_error(errc::no_such_device);
		}
		throw_error(errc::no_such_file_or_directory);
	});
}

//! \since build 764
//@{
template<class _tHolder, typename _fCallable, class _type, typename... _tParams>
inline auto
locked_invoke(_tHolder& holder, _fCallable&& f, _type& x, _tParams&&... args)
	-> decltype(ystdex::invoke(yforward(f), x, yforward(args)...))
{
	lock_guard<mutex> lck(holder.GetMutexRef());

	return ystdex::invoke(yforward(f), x, yforward(args)...);
}

template<typename _fCallable, typename... _tParams>
YB_NONNULL(1) auto
op_path_locked(::_reent* r, const char*& path, _fCallable f, _tParams&&... args)
	ynothrowv -> FilterRes<_fCallable, Partition&, _tParams&&...>
{
	return op_path(r, path, [&, f](Partition& part){
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		return locked_invoke(part, f, part, std::forward<_tParams>(args)...);
	});
}

template<typename _fCallable, typename... _tParams>
YB_NONNULL(1) auto
op_dir_locked(::_reent* r, ::DIR_ITER* dir_state, _fCallable f,
	_tParams&&... args) ynothrowv
	-> FilterRes<_fCallable, DirState&, _tParams&&...>
{
	auto& state(Deref(static_cast<DirState*>(Deref(dir_state).dirStruct)));

	return FilterDevOps(r, [&, f]{
		// XXX: Extended partition mutex to lock directory states.
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		return locked_invoke(state.GetPartitionRef(), f, state,
			std::forward<_tParams>(args)...);
	});
}
//@}

//! \since build 826
template<typename _fCheck, typename _fCallable, typename... _tParams>
YB_NONNULL(1) auto
op_file_checked(::_reent* r, void* fh, _fCheck check, _fCallable f,
	_tParams&&... args) ynothrowv
	-> FilterRes<_fCallable, FileInfo&, _tParams&&...>
{
	auto& file(Deref(static_cast<FileInfo*>(fh)));

	return FilterDevOps(r, [&, check, f]{
		// NOTE: Check of %file is similar to %::close_r.
		if(ystdex::invoke(check, file))
			// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
			//	error: Segmentation fault.
			return locked_invoke(file, f, file,
				std::forward<_tParams>(args)...);
		throw_error(errc::bad_file_descriptor);
	});
}

//! \since build 826
template<typename _fCallable, typename... _tParams>
YB_NONNULL(1) auto
op_file_locked(::_reent* r, void* fh, _fCallable f, _tParams&&... args)
	ynothrowv -> FilterRes<_fCallable, FileInfo&, _tParams&&...>
{
	auto& file(Deref(static_cast<FileInfo*>(fh)));

	return FilterDevOps(r, [&, f]{
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		return locked_invoke(file, f, file, std::forward<_tParams>(args)...);
	});
}

// NOTE: %REENTRANT_SYSCALLS_PROVIDED is configured for libgloss for
//	arm-*-*-eabi targets in devkitPro ports. See source
//	'newlib/libgloss/configure'.

const ::devoptab_t dotab_fat{
	"fat", sizeof(FileInfo), [] YB_LAMBDA_ANNOTATE((::_reent* r,
		void* file_struct, const char* path, int flags, int), ynothrowv,
		nonnull(2, 3)){
		// NOTE: Before the call of %::devoptab_t::open_r, the parameter
		//	%file_struct is set as a pointer after handling in implementation
		//	of devkitPro port of newlib (libsysbase) %::_open_r, which is
		//	assumed to be not null.
		return op_path(r, path, [=, &path](Partition& part){
			::new(Nonnull(file_struct)) FileInfo(part, path, flags);

			return int(file_struct);
		});
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, void* fh) , ynothrowv, nonnull(2)){
		// NOTE: The parameter %fd is actually cast from the file structure
		//	pointer stored by %devoptab_t::open_r. This function is called
		//	when the reference count in the handle decreased to zero. Since this
		//	pointer would not be null for non-predefined streams unless
		//	corrupted, check for null (yield %EBADF) is not performed, as same
		//	as LibFAT does. Also note the name is %fd, not POSIX %filedes.
		return FilterDevOps(r, [=]{
			auto& file(Deref(static_cast<FileInfo*>(fh)));
			auto& part(file.GetPartitionRef());
			const auto gd(ystdex::make_guard([&]() ynothrowv{
				Deref(part.LockOpenFiles()).erase(file);
				file.~FileInfo();
			}));
			lock_guard<mutex> lck(part.GetMutexRef());

			file.SyncToDisc();
		});
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, void* fh, const char* buf,
		size_t nbyte), ynothrowv, nonnull(2, 4)){
		return op_file_checked(r, fh, &FileInfo::CanWrite, &FileInfo::Write,
			buf, nbyte);
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, void* fh, char* buf, size_t nbyte),
		ynothrowv, nonnull(2, 4)){
		return op_file_checked(r, fh, &FileInfo::CanRead, &FileInfo::Read, buf,
			nbyte);
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, void* fh, ::off_t offset,
		int whence), ynothrowv, nonnull(2)){
		return op_file_locked(r, fh, &FileInfo::Seek, offset, whence);
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, void* fh, struct ::stat* buf),
		ynothrowv, nonnull(2, 4)){
		return op_file_locked(r, fh, &FileInfo::Stat, Deref(buf));
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, const char* path,
		struct ::stat* buf), ynothrowv, nonnull(2, 3, 4)){
		return op_path_locked(r, path, &Partition::Stat, Deref(buf), path);
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, const char*, const char*), ynothrowv,
		nonnull(2, 3, 4)){
		return seterr(r, ENOTSUP);
	}, [] YB_LAMBDA_ANNOTATE(
		(::_reent* r, const char* path), ynothrowv, nonnull(2, 3)){
		return op_path_locked(r, path, &Partition::Unlink, path);
	}, [] YB_LAMBDA_ANNOTATE(
		(::_reent* r, const char* path), ynothrowv, nonnull(2, 3)){
		return op_path_locked(r, path, &Partition::ChangeDir, path);
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, const char* old,
		const char* new_name), ynothrowv, nonnull(2, 3, 4)){
		return op_path_locked(r, old, &Partition::Rename, old, new_name);
	}, [] YB_LAMBDA_ANNOTATE(
		(::_reent* r, const char* path, int), ynothrowv, nonnull(2, 3)){
		return op_path_locked(r, path, &Partition::MakeDir, path);
	}, sizeof(DirState), [] YB_LAMBDA_ANNOTATE((::_reent* r,
		::DIR_ITER* dir_state, const char* path), ynothrowv, nonnull(2, 3, 4)){
		return
			op_path_locked(r, path, [=, &path](Partition& part) -> ::DIR_ITER*{
			const auto p(dir_state->dirStruct);

			::new(Nonnull(p)) DirState(part, path);
			return static_cast<::DIR_ITER*>(p);
		});
	}, [] YB_LAMBDA_ANNOTATE(
		(::_reent* r, ::DIR_ITER* dir_state), ynothrowv, nonnull(2, 3)){
		return op_dir_locked(r, dir_state, &DirState::Reset);
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, ::DIR_ITER* dir_state,
		char* filename, struct ::stat* filestat), ynothrowv, nonnull(2, 3, 4)){
		// NOTE: The filename is of %NAME_MAX characters in newlib DS port.
		return
			op_dir_locked(r, dir_state, &DirState::Iterate, filename, filestat);
	}, [] YB_LAMBDA_ANNOTATE(
		(::_reent* r, ::DIR_ITER* dir_state), ynothrowv, nonnull(2, 3)){
		return op_dir_locked(r, dir_state, [](DirState& state) ynothrow{
			state.~DirState();
		});
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, const char* path,
		struct ::statvfs* buf), ynothrowv, nonnull(2, 3, 4)){
		return op_path_locked(r, path, &Partition::StatFS, Deref(buf));
	}, [] YB_LAMBDA_ANNOTATE(
		(::_reent* r, void* fh, ::off_t length), ynothrowv, nonnull(2)) -> int{
		return length >= 0 ? (sizeof(length) <= 4 || length <= ::off_t(
			MaxFileSize) ? op_file_checked(r, fh, &FileInfo::CanWrite,
			&FileInfo::Truncate, std::uint32_t(length)) : EFBIG) : EINVAL;
	}, [] YB_LAMBDA_ANNOTATE((::_reent* r, void* fh), ynothrowv, nonnull(2))
		-> int{
		return op_file_locked(r, fh, &FileInfo::SyncToDisc);
	}, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};

} // unnamed namespace;

bool
Mount(string_view name, const ::DISC_INTERFACE& intf, ::sec_t start_sector,
	size_t pages, size_t sectors_per_page_shift)
{
	YAssertNonnull(name.data());
	if(intf.startup() && intf.isInserted())
	{
		const auto devname(string(name) + ':');

		if(::FindDevice(devname.c_str()) >= 0)
			return true;
		if(auto p = unique_raw(static_cast<::devoptab_t*>(::operator new(
			sizeof(::devoptab_t) + name.length() + 1)),
			static_cast<void(&)(void*)>(::operator delete)))
			if(auto p_part = make_unique<Partition>(intf, pages,
				sectors_per_page_shift, start_sector))
			{
				const auto
					p_name(ystdex::aligned_store_cast<char*>(p.get() + 1));

				yunseq(ystdex::trivially_copy_n(&dotab_fat, 1, p.get()),
					ystdex::ntctscpy(p_name, name.data(), name.length()),
					p->name = p_name, p->deviceData = p_part.get());
				// NOTE: LibFAT has no error handling here.
				if(YB_UNLIKELY(::AddDevice(p.get())) == -1)
					throw std::runtime_error("Adding device failed.");
				p_part.release();
				p.release();
				return true;
			}
		YTraceDe(Alert, "Memory allocation failure @ FATMount.");
	}
	return {};
}

bool
Unmount(const char* name) ynothrow
{
	if(const auto p_devops = ::GetDeviceOpTab(Nonnull(name)))
		if(p_devops->open_r == dotab_fat.open_r && ::RemoveDevice(name) != -1)
		{
			delete static_cast<Partition*>(p_devops->deviceData);
			::operator delete(const_cast<::devoptab_t*>(p_devops));
			return true;
		}
	return {};
}

} // namespace FAT;
#endif

} // namespace platform_ex;

