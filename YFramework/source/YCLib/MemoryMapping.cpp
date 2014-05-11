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
\version r187
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:59:21 +0800
\par 修改时间:
	2014-05-07 18:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_MemoryMapping
#include YFM_YCLib_FileSystem // for platform::uopen, platform::GetFileSizeOf;
#include <fcntl.h>
#include <stdexcept> // for std::runtime_error;
#if YCL_DS
#	include <unistd.h>
#elif YCL_Win32
#	include <Windows.h>
#	include <sys/stat.h>

namespace
{

#define MAP_FAILED (reinterpret_cast<void*>(-1))

void*
map_file(size_t len, int fd)
{
	void* p_mapped{};

	if(len != 0)
	{
		const auto h(::HANDLE(::_get_osfhandle(fd)));

		if(h != INVALID_HANDLE_VALUE)
			if(::HANDLE fm = ::CreateFileMapping(h, nullptr, PAGE_READONLY, 0,
				len, nullptr))
			{
				p_mapped = ::MapViewOfFile(fm, FILE_MAP_READ, 0, 0, len);
				::CloseHandle(fm);
			}
	}
	return p_mapped ? p_mapped : MAP_FAILED;
}

} // unnamed namespace;

#elif YCL_Android
#	include <sys/mman.h>
#	include <sys/stat.h>
#else
#	error "Unsupported platform found."
#endif

namespace platform
{

MappedFile::MappedFile(const char* path)
	: fd(uopen(path, O_RDONLY, S_IRUSR | S_IWUSR)), size([](int fd){
		if(fd == -1)
			throw FileOperationFailure("Failed mapping file.");
		return GetFileSizeOf(fd);
	}(fd))
{
#if YCL_DS
	addr = new ystdex::byte[size];

	::read(fd, addr, size);
#else
#	if YCL_Win32
	const auto p(map_file(size, fd));
#	else
	const auto p(::mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0));
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

