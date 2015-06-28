/*
	© 2015 FrankHB.

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
\version r668
\author FrankHB <frankhb1989@gmail.com>
\since build 604
\par 创建时间:
	2015-06-06 06:25:00 +0800
\par 修改时间:
	2015-06-29 06:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(DS)::DSIO
*/


#include "YCLib/YModules.h"
#include YFM_DS_YCLib_DSIO // for std::exception, etc;
#if YCL_DS
#	include <ystdex/cstdint.hpp> // for ystdex::read_uint_le,
//	ystdex::write_uint_le;
#	include <ystdex/exception.h> // for ystdex::throw_system_error;
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
	entries.flush = [this](UsedListCache::value_type& pr){
		FlushEntry(pr);
	};
}
SectorCache::~SectorCache()
{
	Flush();
}

size_t
SectorCache::GetBlockCount(::sec_t sec) const ynothrow
{
	const auto end_sec(std::min<::sec_t>(
		sec + (1 << sectors_per_page_shift), end_of_partition));

	YAssert(sec <= end_sec, "Invalid range found.");
	return end_sec - sec;
}
ystdex::block_buffer*
SectorCache::GetPage(::sec_t key) ynothrow
{
	YAssert(GetKey(key) == key, "Invalid key found.");

	TryRet(&ystdex::cache_lookup(entries, key, [key, this]{
			const auto size(bytes_per_sector << sectors_per_page_shift);
			// TODO: Use aligned allocation for cache implementation.
			// NOTE: Intentionally uninitialized as libfat.
			ystdex::block_buffer entry(make_unique_default_init<byte[]>(size));
			const auto n(GetBlockCount(key));

			YAssert(n <= size, "Invalid cache creation parameter found.");
			if(YB_UNLIKELY(!disc.ReadSectors(key, n, entry.get())))
				throw std::runtime_error(
					"Failed reading on creating file system cache entry.");
			return entry;
		}))
	CatchExpr(std::exception& e, YCL_TraceRaw(Warning,
		"FATMount failure[%s]: %s", typeid(e).name(), e.what()))
	CatchExpr(..., YCL_TraceRaw(Emergent,
		"Unknown exception found when getting file system cache page."))
	return {};
}

bool
SectorCache::EraseWritePartialSector(::sec_t sec, size_t offset,
	const void* p_buf, size_t n) ynothrowv
{
	return PerformPartialSectorIO([=](ystdex::block_buffer& entry,
		size_t sec_bytes){
		ystdex::trivially_fill_n(entry.get() + sec_bytes, bytes_per_sector);
		entry.write(sec_bytes + offset, p_buf, n);
	}, sec, offset, n);
}

bool
SectorCache::FillPartialSector(::sec_t sec, size_t offset, size_t n, byte val)
	ynothrowv
{
	return PerformPartialSectorIO([=](ystdex::block_buffer& entry,
		size_t sec_bytes){
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
	for(auto& pr : entries)
		if(!FlushEntry(pr))
			return {};
	return true;
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
	return PerformPartialSectorIO([=](ystdex::block_buffer& entry,
		size_t sec_bytes){
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
	return PerformPartialSectorIO([=](ystdex::block_buffer& entry,
		size_t sec_bytes){
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
using ystdex::read_uint_le;
using ystdex::write_uint_le;
using ystdex::throw_system_error;

namespace
{

template<size_t _vWidth>
bool
ReadLEVal(SectorCache& cache, std::uint32_t& value, ::sec_t sec, size_t offset)
	ynothrow
{
	static yconstexpr const size_t
		n(_vWidth / std::numeric_limits<byte>::digits);
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
		n(_vWidth / std::numeric_limits<byte>::digits);
	byte buf[n]{};

	write_uint_le<_vWidth>(buf, value);
	return cache.WritePartialSector(sec, offset, buf, n);
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
	table_size([sec_buf]{
		// NOTE: last_cluster = cluster_count + Clusters::First - 1.
		const auto t(read_uint_le<16>(sec_buf + BPB_FATSz16));

		return t == 0 ? size_t(
			read_uint_le<32>(sec_buf + BPB_FATSz32)) : size_t(t);
	}()), total_sectors_num([sec_buf]{
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
	fs_type([start_sector, this]() -> FileSystemType{
		// NOTE: See Microsoft FAT Specification Section 3.5.
		ClusterIndex cluster_count(data_sectors_num / sectors_per_cluster);

		last_cluster = cluster_count + Clusters::First - 1;
		// TODO: Distinguish part type (e.g. FAT16B) with file system type. 
		if(cluster_count < Clusters::PerFAT12)
			return FileSystemType::FAT12;
		if(cluster_count < Clusters::PerFAT16)
			return FileSystemType::FAT16;
		return FileSystemType::FAT32;
	}()), root_dir_cluster([sec_buf, this]() -> ClusterIndex{
		if(fs_type != FileSystemType::FAT32)
			return Clusters::FAT16RootDirectory;

		const auto t(read_uint_le<32>(sec_buf + BPB_RootClus));
		// NOTE: Check if FAT mirroring is enabled.
		if((sec_buf[BPB_ExtFlags] & 0x80) == 0)
			// NOTE: Use the active FAT.
			table_start += table_size * (sec_buf[BPB_ExtFlags] & 0x0F);
		return t;
	}()), Cache(pages, sectors_per_page_shift, d,
	start_sector + total_sectors_num, bytes_per_sector)
{}

bool
AllocationTable::ClearLinks(ClusterIndex c) ynothrow
{
	if(IsValidCluster(c))
	{
		if(c < first_free)
			first_free = c;
		while(!Clusters::IsFreeOrEOF(c) && c != Clusters::Error)
		{
			const auto next(QueryNext(c));

			WriteEntry(c, Clusters::Free);
			if(free_cluster < limit)
				++free_cluster;
			c = next;
		}
		return true;
	}
	return {};
}

ClusterIndex
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
AllocationTable::LinkFree(ClusterIndex c) ynothrow
{
	if(last_cluster < c)
		return Clusters::Error;

	const auto cur_link(QueryNext(c));

	if(IsValidCluster(cur_link))
		return cur_link;

	auto first_valid(std::max<ClusterIndex>(first_free, Clusters::First));
	bool loop_around = {};

	while(QueryNext(first_valid) != Clusters::Free)
		if(last_cluster < ++first_valid)
		{
			if(loop_around)
			{
				first_free = first_valid;
				return Clusters::Error;
			}
			else
			{
				first_valid = Clusters::First;
				loop_around = true;
			}
		}
	first_free = first_valid;
	if(free_cluster != 0)
		--free_cluster;
	last_alloc_cluster = first_valid;
	if(IsValidCluster(c))
		WriteEntry(c, first_valid);
	WriteEntry(first_valid, Clusters::EndOfFile);
	return first_valid;
}

ClusterIndex
AllocationTable::ProbeForNext(ClusterIndex c) const ynothrow
{
	// FIXME: Too large malformed parameter?
	YAssert(c != Clusters::Free, "Invalid parameter found.");

	ClusterIndex next_cluster(Clusters::Free);

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
		ReadLEVal<16>(Cache, next_cluster, table_start
			+ (c << 1) / bytes_per_sector, c % (bytes_per_sector >> 1) << 1);
		if(Clusters::MaxValid16 < next_cluster)
			next_cluster = Clusters::EndOfFile;
		break;
	case FileSystemType::FAT32:
		ReadLEVal<32>(Cache, next_cluster, table_start
			+ (c << 2) / bytes_per_sector, c % (bytes_per_sector >> 2) << 2);
		if(Clusters::MaxValid32 < next_cluster)
			next_cluster = Clusters::EndOfFile;
		break;
	default:
		return Clusters::Error;
	}
	return next_cluster;
}

ClusterIndex
AllocationTable::QueryNext(ClusterIndex c) const ynothrow
{
	// FIXME: c == Clusters::EndOfFile?
	return c == Clusters::Free ? ClusterIndex(Clusters::Free) : ProbeForNext(c);
}

ClusterIndex
AllocationTable::QueryLast(ClusterIndex c) const ynothrow
{
	// TODO: Assert !Clusters::IsFreeOrEOF(c)?
	ClusterIndex t;

	do 
	{
		t = c;
		c = QueryNext(t);
	}while(!Clusters::IsFreeOrEOF(c));
	return t;
}

void
AllocationTable::ReadClusters(const byte* sec_buf) ynothrowv
{
	yunseq(
	free_cluster = read_uint_le<32>(sec_buf + FSI_Free_Count),
	last_alloc_cluster = read_uint_le<32>(sec_buf + FSI_Nxt_Free)
	);
}

ClusterIndex
AllocationTable::TrimChain(ClusterIndex start, size_t len)
	ynothrow
{
	if(len == 0)
	{
		ClearLinks(start);
		return Clusters::Free;
	}
	--len;

	auto next(QueryNext(start));

	while(len > 0 && next != Clusters::Free && next != Clusters::EndOfFile)
	{
		--len;
		start = next;
		next = QueryNext(start);
	}
	if(next != Clusters::Free && next != Clusters::EndOfFile)
		ClearLinks(next);
	WriteEntry(start, Clusters::EndOfFile);
	return start;
}

ClusterIndex
AllocationTable::TryLinkFree(ClusterIndex c) ythrow(std::system_error)
{
	const auto t(LinkFree(c));

	if(IsValidCluster(t))
		return t;
	throw_system_error(ENOSPC);
}

void
AllocationTable::WriteClusters(byte* sec_buf) const ynothrowv
{
	write_uint_le<32>(sec_buf + FSI_Free_Count, free_cluster),
	write_uint_le<32>(sec_buf + FSI_Nxt_Free, last_alloc_cluster);
}

bool
AllocationTable::WriteEntry(ClusterIndex c, std::uint32_t val) const
	ynothrow
{
	if(IsValidCluster(c))
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
AllocationTable::WriteFSInfo(byte* sec_buf) ynothrowv
{
	YAssertNonnull(sec_buf);
	// FIXME: Big endian implementations.
	write_uint_le<32>(sec_buf + FSI_LeadSig, FSI_LeadSig_Value),
	write_uint_le<32>(sec_buf + FSI_StrucSig, FSI_StrucSig_Value);
	free_cluster = CountFreeCluster();
	WriteClusters(sec_buf);
	write_uint_le<32>(sec_buf + FSI_TrailSig, FSI_TrailSig_Value);
}

}
#endif

} // namespace platform_ex;

