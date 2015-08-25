/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MemoryMapping.cpp
\ingroup YCLib
\brief 内存映射文件。
\version r238
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:59:21 +0800
\par 修改时间:
	2015-08-25 00:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_MemoryMapping
#include YFM_YCLib_FileIO // for platform::uopen, platform::GetFileSizeOf;
#include YFM_YCLib_NativeAPI
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
	: fd(uopen(path, O_RDONLY, mode_t(Mode::UserReadWrite))), size([this]{
		if(fd < 0)
			throw FileOperationFailure(errno, std::generic_category(),
				"Failed mapping file.");
		return FileDescriptor(fd).GetSize();
	}())
{
#if YCL_DS
	addr = new byte[size];

	if(YB_UNLIKELY(::read(fd, addr, size) < 0))
		throw std::runtime_error("Mapping failed.");
#else
#	if YCL_Win32
	void* p(MAP_FAILED);

	if(size != 0)
	{
		const auto h(::HANDLE(::_get_osfhandle(fd)));

		if(h != INVALID_HANDLE_VALUE)
			if(::HANDLE fm = ::CreateFileMapping(h, {}, PAGE_READONLY, 0,
				static_cast<unsigned long>(size), {}))
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
	addr = static_cast<byte*>(p);
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

