/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MemoryMapping.cpp
\ingroup YCLib
\brief 内存映射文件。
\version r224
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:59:21 +0800
\par 修改时间:
	2014-12-22 13:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_MemoryMapping
#include YFM_YCLib_FileSystem // for platform::uopen, platform::GetFileSizeOf;
#include YFM_YCLib_NativeAPI
#include <fcntl.h>
#include <stdexcept> // for std::runtime_error;
#if YCL_Win32
#define MAP_FAILED (reinterpret_cast<void*>(-1))
#elif YCL_Linux || YCL_OS_X
#	include <sys/mman.h>
#	include <sys/stat.h>
#endif

namespace platform
{

MappedFile::MappedFile(const char* path)
	: fd(uopen(path, O_RDONLY, S_IRUSR | S_IWUSR)), size([](int fd){
		if(fd == -1)
			throw FileOperationFailure(errno, std::generic_category(),
				"Failed mapping file.");
		return GetFileSizeOf(fd);
	}(fd))
{
#if YCL_DS
	addr = new ystdex::byte[size];

	::read(fd, addr, size);
#else
#	if YCL_Win32
	void* p(MAP_FAILED);

	if(size != 0)
	{
		const auto h(::HANDLE(::_get_osfhandle(fd)));

		if(h != INVALID_HANDLE_VALUE)
			if(::HANDLE fm = ::CreateFileMapping(h, {}, PAGE_READONLY, 0, size,
				{}))
			{
				p = ::MapViewOfFile(fm, FILE_MAP_READ, 0, 0, size);
				::CloseHandle(fm);
			}
	}
#	else
	const auto p(::mmap({}, size, PROT_READ, MAP_PRIVATE, fd, 0));
#	endif

	if(p == MAP_FAILED)
		throw std::runtime_error("Mapping failed.");
	// TODO: Create specific exception type.
	addr = static_cast<ystdex::byte*>(p);
#endif
}
MappedFile::~MappedFile()
{
#if YCL_DS
	delete addr;
#elif YCL_Win32
	::UnmapViewOfFile(addr);
#else
	::munmap(addr, size);
#endif
	::close(fd);
}

} // namespace platform;

