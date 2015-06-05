/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.cpp
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r744
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2012-03-26 13:36:28 +0800
\par 修改时间:
	2015-06-04 21:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::NativeAPI
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_NativeAPI
#if YCL_DS
#	include <nds/disc_io.h> // for ::DISC_INTERFACE, ::sec_t;
#	include <arm9/dldi.h> // for ::_io_dldi_stub;
#	include <stdint.h> // for ::uint32_t;
#	include <sys/iosupport.h> // for ::_reent, ::size_t, ::ssize_t, ::off_t,
//	sturct ::stat, struct ::statvfs, ::DIR_ITER, ::devoptab_t;
#	include YFM_YCLib_Debug // for string, YTraceDe, platform::Descriptions,
//	ystdex::trivially_copy_n;
#	include YFM_YCLib_Reference // for std::malloc, std::free,
//	platform::unique_raw;
#	include <stdlib.h> // for ::free;

//! \since build 602
extern "C" ::DLDI_INTERFACE _io_dldi_stub;
#elif YCL_MinGW
#	include <ctime> // for std::gmtime;
#endif

namespace
{

}

#if YCL_DS
//! \since build 602
//@{
extern "C"
{

struct PARTITION;

PARTITION*
_FAT_partition_constructor(const ::DISC_INTERFACE*, ::uint32_t, ::uint32_t,
	::sec_t);

} // extern "C"

namespace platform_ex
{

using platform::unique_raw;
using namespace platform::Descriptions;
using ::PARTITION;

namespace
{

enum
{
	DIR_ENTRY_DATA_SIZE = 0x20,
	MAX_FILENAME_LENGTH = 768
};

struct FILE_POSITION
{
	std::uint32_t cluster;
	::sec_t sector;
	std::int32_t byte;
};

struct DIR_ENTRY_POSITION
{
	std::uint32_t cluster;
	::sec_t sector;
	std::int32_t offset;
};

struct DIR_ENTRY
{
	std::uint8_t entryData[DIR_ENTRY_DATA_SIZE];
	DIR_ENTRY_POSITION dataStart;
	DIR_ENTRY_POSITION dataEnd;
	char filename[MAX_FILENAME_LENGTH];
};

struct DIR_STATE_STRUCT
{
	PARTITION* partition;
	DIR_ENTRY currentEntry;
	std::uint32_t startCluster;
	bool inUse;
	bool validEntry;
};

struct FILE_STRUCT
{
	std::uint32_t filesize;
	std::uint32_t startCluster;
	std::uint32_t currentPosition;
	FILE_POSITION rwPosition;
	FILE_POSITION appendPosition;
	DIR_ENTRY_POSITION dirEntryStart;
	DIR_ENTRY_POSITION dirEntryEnd;
	PARTITION* partition;
	FILE_STRUCT* prevOpenFile;
	FILE_STRUCT* nextOpenFile;
	bool read;
	bool write;
	bool append;
	bool inUse;
	bool modified;
};

} // unnamed namespace;

} // namespace platform_ex;

extern "C"
{

int
_FAT_open_r(::_reent*, void*, const char*, int, int);

int
_FAT_close_r(::_reent*, int);

::ssize_t
_FAT_write_r(::_reent*, int, const char*, ::size_t);

::ssize_t
_FAT_read_r(::_reent*, int, char*, ::size_t);

::off_t
_FAT_seek_r(::_reent*, int, ::off_t, int);

int
_FAT_fstat_r(::_reent*, int, struct ::stat*);

int
_FAT_stat_r(::_reent*, const char*, struct ::stat*);

int
_FAT_link_r(::_reent*, const char*, const char*);

int
_FAT_unlink_r(::_reent*, const char*);

int
_FAT_chdir_r(::_reent*, const char*);

int
_FAT_rename_r(::_reent*, const char*, const char*);

int
_FAT_mkdir_r(::_reent*, const char*, int);

::DIR_ITER*
_FAT_diropen_r(::_reent*, ::DIR_ITER*, const char*);

int
_FAT_dirreset_r(::_reent*, ::DIR_ITER*);

int
_FAT_dirnext_r(::_reent*, ::DIR_ITER*, char*, struct ::stat*);

int
_FAT_dirclose_r(::_reent*, DIR_ITER*);

int
_FAT_statvfs_r(::_reent*r, const char*, struct ::statvfs*);

int
_FAT_ftruncate_r(::_reent*r, int, ::off_t);

int
_FAT_fsync_r(::_reent*, int);

} // extern "C";


namespace platform_ex
{

namespace
{

const ::devoptab_t dotab_fat{
	"fat", sizeof(FILE_STRUCT), ::_FAT_open_r, ::_FAT_close_r, ::_FAT_write_r,
	::_FAT_read_r, ::_FAT_seek_r, ::_FAT_fstat_r, ::_FAT_stat_r, ::_FAT_link_r,
	::_FAT_unlink_r, ::_FAT_chdir_r, ::_FAT_rename_r, ::_FAT_mkdir_r,
	sizeof(DIR_STATE_STRUCT), ::_FAT_diropen_r, ::_FAT_dirreset_r,
	::_FAT_dirnext_r, ::_FAT_dirclose_r, ::_FAT_statvfs_r, ::_FAT_ftruncate_r,
	::_FAT_fsync_r, nullptr, nullptr, nullptr, nullptr
};

inline void*
DevopTabAlloc(size_t size)
{
	return std::malloc(size);
}

inline void
DevopTabFree(void* mem)
{
	return std::free(mem);
}

bool
FATMount(const string& name, const ::DISC_INTERFACE& intf, ::sec_t start_sector,
	std::uint32_t cache_size, std::uint32_t sectors_per_page)
{
	if(intf.startup() && intf.isInserted())
	{
		const auto devname(string(name) + ':');

		if(::FindDevice(devname.c_str()) >= 0)
			return true;
		if(auto p = unique_raw(static_cast<::devoptab_t*>(DevopTabAlloc(
			sizeof(::devoptab_t) + name.length() + 1)), DevopTabFree))
		{
			if(auto p_part = unique_raw(::_FAT_partition_constructor(&intf,
				cache_size, sectors_per_page, start_sector), ::free))
			{
				const auto p_name(reinterpret_cast<char*>(p.get() + 1));

				yunseq(ystdex::trivially_copy_n(&dotab_fat, 1, p.get()),
					ystdex::trivially_copy_n(name.c_str(), name.length() + 1,
					p_name), p->name = p_name, p->deviceData = p_part.get());
				if(YB_UNLIKELY(::AddDevice(p.get())) == -1)
					throw std::runtime_error("Adding device failed.");
				p_part.release();
				p.release();
				return true;
			}
		}
		YTraceDe(Alert, "Memory allocation failure @ FATMount.");
	}
	return {};
}
//@}

enum class DSFATRoot
{
	Null,
	SD,
	FAT
};

} // unnamed namespace;

bool
InitializeFileSystem() ynothrow
{
	// NOTE: %DEFAULT_CACHE_PAGES is 16 in "common.h" in libfat source.
	const auto res([](size_t cache_size) ynothrow -> DSFATRoot{
		const auto init([=](const char* name, const ::DISC_INTERFACE& disc_io)
			ynothrow -> bool{
			// NOTE: %DEFAULT_SECTORS_PAGE is 8 in "common.h" in libfat source.
			TryRet(FATMount(name, disc_io, 0, std::uint32_t(cache_size), 8))
			CatchExpr(std::exception& e, YCL_TraceRaw(Warning,
				"FATMount failure[%s]: %s", typeid(e).name(), e.what()))
			CatchExpr(..., YCL_TraceRaw(Emergent,
				"Unknown exception found @ InitializeFileSystem."))
			return {};
		});

		if(init("sd", ::__io_dsisd))
			return DSFATRoot::SD;
		// NOTE: As ::dldiGetInternal.
		if(_io_dldi_stub.ioInterface.features
			& (FEATURE_SLOT_GBA | FEATURE_SLOT_NDS))
			//NOTE: As ::sysSetCardOwner(BUS_OWNER_ARM9).
			REG_EXMEMCNT = REG_EXMEMCNT & ~ARM7_OWNS_CARD;
		if(init("fat", _io_dldi_stub.ioInterface))
			return DSFATRoot::FAT;
		return DSFATRoot::Null;
	}(16));

	if(res != DSFATRoot::Null)
	{
		// NOTE: No %ARGV_MAGIC here as libnds does.
		::chdir(res == DSFATRoot::FAT ? "fat:/" : "sd:/");
		return true;
	}
	return {};
}

} // namespace platform_ex;
#elif YCL_MinGW
extern "C"
{

#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// See FileSystem.cpp, line 40.
//! \since build 465
struct ::tm* __cdecl __MINGW_NOTHROW
_gmtime32(const ::__time32_t* p)
{
	return std::gmtime(p);
}
#	endif

} // extern "C";

#endif

namespace platform
{
}

