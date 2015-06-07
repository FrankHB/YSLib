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
\version r225
\author FrankHB <frankhb1989@gmail.com>
\since build 604
\par 创建时间:
	2015-06-06 06:25:00 +0800
\par 修改时间:
	2015-06-06 07:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(DS)::DSIO
*/


#include "YCLib/YModules.h"
#include YFM_DS_YCLib_DSIO
#include YFM_YCLib_Debug

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
SectorCache::EraseWritePartialSector(const void* p_buf, ::sec_t sec,
	size_t offset, size_t n) ynothrowv
{
	if(!(bytes_per_sector < offset + n))
	{
		const auto key(GetKey(sec));

		if(const auto p_entry = GetPage(key))
		{
			auto& entry(*p_entry);
			const auto sec_bytes((sec - key) * bytes_per_sector);

			ystdex::trivially_fill_n(entry.get() + sec_bytes,
				bytes_per_sector);
			entry.write(sec_bytes + offset, p_buf, n);
			return true;
		}
	}
	return {};
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
	if(!(bytes_per_sector < offset + n))
	{
		const auto key(GetKey(sec));

		if(const auto p_entry = GetPage(key))
		{
			p_entry->read(p_buf, (sec - key) * bytes_per_sector + offset, n);
			return true;
		}
	}
	return {};
}

bool
SectorCache::ReadSectors(::sec_t sec, size_t n, void* p_buf) ynothrowv
{
	auto dst(static_cast<byte*>(p_buf));

	while(0 < n)
	{
		const auto key(GetKey(sec));

		if(const auto p_entry = GetPage(key))
		{
			const auto sec_off(sec - key);
			// TODO: Compare and assert 'count' and 'sec_off'?
			const auto secs_to_process(
				std::min<size_t>(GetBlockCount(key) - sec_off, n));

			p_entry->read(dst, sec_off * bytes_per_sector,
				secs_to_process * bytes_per_sector);
			yunseq(dst += secs_to_process * bytes_per_sector,
				sec += secs_to_process,  n -= secs_to_process);
		}
		else
			return {};
	}
	return true;
}

bool
SectorCache::WritePartialSector(const void* p_buf, ::sec_t sec, size_t offset,
	size_t n) ynothrowv
{
	if(!(bytes_per_sector < offset + n))
	{
		const auto key(GetKey(sec));

		if(const auto p_entry = GetPage(key))
		{
			p_entry->write((sec - key) * bytes_per_sector + offset, p_buf, n);
			return true;
		}
	}
	return {};
}

bool
SectorCache::WriteSectors(::sec_t sec, size_t n, const void* p_buf)
	ynothrow
{
	auto src(static_cast<const byte*>(p_buf));

	while(0 < n)
	{
		const auto key(GetKey(sec));

		if(const auto p_entry = GetPage(key))
		{
			const auto sec_off(sec - key);
			// TODO: Compare and assert 'entry.count' and 'sec'?
			const auto secs_to_process(
				std::min<size_t>(GetBlockCount(key) - sec_off, n));

			p_entry->write(sec_off * bytes_per_sector, src,
				secs_to_process * bytes_per_sector);
			yunseq(src += secs_to_process * bytes_per_sector,
				sec += secs_to_process, n -= secs_to_process);
		}
		else
			return {};
	}
	return true;
}
#endif

} // namespace platform_ex;

