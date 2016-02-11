/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MemoryMapping.cpp
\ingroup YCLib
\brief 内存映射文件。
\version r283
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:59:21 +0800
\par 修改时间:
	2016-02-11 01:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_MemoryMapping
#include YFM_YCLib_FileIO // for uopen, ThrowFileOperationFailure;
#include YFM_YCLib_NativeAPI
#include <stdexcept> // for std::runtime_error;
#if YCL_Linux || YCL_OS_X
#	include <sys/mman.h>
#	include <sys/stat.h>
#endif

namespace platform
{

#if !YCL_DS
void
UnmapDelete::operator()(pointer p) const ynothrow
{
	// XXX: Error ignored.
#if YCL_Win32
	::UnmapViewOfFile(p);
#else
	::munmap(p, size);
#endif
}
#endif


MappedFile::MappedFile(const char* path)
	: file(uopen(path, O_RDONLY, mode_t(Mode::UserReadWrite))), size([this]{
		if(!file)
			ThrowFileOperationFailure("Failed mapping file.");
		return file->GetSize();
	}()), addr(
#if YCL_DS
	new byte[size]
#else
	[this]{
#	if YCL_Win32
	auto p(reinterpret_cast<void*>(-1));

	if(size != 0)
	{
		const auto h(::HANDLE(::_get_osfhandle(*file.get())));

		if(h != INVALID_HANDLE_VALUE)
			if(::HANDLE fm = ::CreateFileMapping(h, {}, PAGE_READONLY, 0,
				static_cast<unsigned long>(size), {}))
			{
				p = ::MapViewOfFile(fm, FILE_MAP_READ, 0, 0, size);
				::CloseHandle(fm);
			}
	}
#	else
	const auto p(::mmap({}, size, PROT_READ, MAP_PRIVATE, *file.get(), 0));
#	endif

	if(p == reinterpret_cast<void*>(-1))
		throw std::runtime_error("Mapping failed.");
	// TODO: Create specific exception type.
	return static_cast<byte*>(p);
	}()
#endif
#if !(YCL_DS || YCL_Win32)
	, size
#endif
	)
{
#if YCL_DS
	if(YB_UNLIKELY(::read(*file.get(), GetPtr(), size) < 0))
		throw std::runtime_error("Mapping failed.");
#endif
}

} // namespace platform;

