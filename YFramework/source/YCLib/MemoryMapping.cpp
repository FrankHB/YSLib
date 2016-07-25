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
\version r445
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:59:21 +0800
\par 修改时间:
	2016-07-23 20:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_MemoryMapping // for wcast;
#include YFM_YCLib_FileIO // for uopen, ThrowFileOperationFailure;
#include YFM_YCLib_NativeAPI // for ::UnmapViewOfFile, CreateFileMapping,
//	PAGE_READONLY, PAGE_READWRITE, MapViewOfFile, FILE_MAP_READ,
//	FILE_MAP_ALL_ACCESS, FILE_MAP_COPY, ::lseek;
#include <ystdex/exception.h> // for ystdex::throw_error,
//	std::throw_with_nested, std::runtime_error;
#include <cerrno> // for errno;
#include <ystdex/cast.hpp> // for ystdex::narrow;
#if YCL_Win32
#	include YFM_YCLib_Host // for platform_ex::UniqueHandle;
#	include YFM_Win32_YCLib_MinGW32 // for YCL_CallWin32F;
#elif YCL_Linux || YCL_OS_X
#	include <sys/mman.h> // for ::munmap, ::mmap, PROT_READ, PROT_WRITE,
//	MAP_PRIVATE, MAP_SHARED, MAP_FAILED;
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


MappedFile::MappedFile(std::uint64_t off, size_t len, UniqueFile f,
	FileMappingOption opt, FileMappingKey key)
	: option(opt), file(std::move(f)), mapped(
#if YCL_DS
	new byte[len]
#else
	[=](){
		try
		{
			if(len != 0)
			{
#	if YCL_Win32
				const auto h(::HANDLE(::_get_osfhandle(*file.get())));

				if(h != INVALID_HANDLE_VALUE)
				{
					platform_ex::UniqueHandle fm(YCL_CallWin32F(
						CreateFileMapping, h, {},
						opt != FileMappingOption::ReadWrite ? PAGE_READONLY
						: PAGE_READWRITE, 0, 0, wcast(key)));

					return static_cast<byte*>(YCL_CallWin32F(MapViewOfFile,
						fm.get(), opt != FileMappingOption::CopyOnWrite
						? (opt == FileMappingOption::ReadOnly ? FILE_MAP_READ
						: FILE_MAP_ALL_ACCESS) : FILE_MAP_COPY,
						static_cast<unsigned long>(off >> 32UL),
						static_cast<unsigned long>(off), len));
				}
				ystdex::throw_error(errno, "::_get_osfhandle");
#	else
				// NOTE: Since no %MAP_FIXED or extensions can be specified
				//	here, there is no need to convert %ENOMEM to throwing
				//	%std::bad_alloc or derived exceptions.
				const auto
					p(::mmap({}, len, opt != FileMappingOption::ReadWrite
					? PROT_READ : PROT_READ | PROT_WRITE, opt
					== FileMappingOption::CopyOnWrite || key == FileMappingKey()
					? MAP_PRIVATE : MAP_SHARED, *file.get(), ::off_t(off)));

				if(p != MAP_FAILED)
					return static_cast<byte*>(p);
				ystdex::throw_error(errno, "::mmap");
#	endif
			}
			throw std::invalid_argument("Empty file found.");
		}
#if YCL_Win32
		CatchExpr(ystdex::narrowing_error&, throw)
#endif
		// TODO: Create specific exception type?
		CatchExpr(...,
			std::throw_with_nested(std::runtime_error("Mapping failed.")))
	}()
#endif
	// NOTE: For platform %Win32, relying on ::NtQuerySection is not allowed by
	//	current platform implmentation policy.
	, len)
{
#if !YF_Hosted
	yunused(key);
#endif
#if YCL_DS
	yunused(opt);
	try
	{
		if(YB_UNLIKELY(off != 0
			&& ::lseek(*file.get(), ::off_t(off), SEEK_SET) < 0))
			ystdex::throw_error(errno);
		if(YB_UNLIKELY(::read(*file.get(), GetPtr(), len) < 0))
			ystdex::throw_error(errno);
	}
	CatchExpr(...,
		std::throw_with_nested(std::runtime_error("Mapping failed.")))
#endif
}
MappedFile::MappedFile(size_t len, UniqueFile f,
	FileMappingOption opt, FileMappingKey key)
	: MappedFile(std::uint64_t(0), len, std::move(f), opt, key)
{}
MappedFile::MappedFile(UniqueFile f, FileMappingOption opt, FileMappingKey key)
	: MappedFile([&]{
		if(!f)
			ThrowFileOperationFailure(
				"Failed mapping due to invalid file opened.");

		const auto s(ystdex::narrow<size_t>(f->GetSize()));

		return pair<size_t, UniqueFile>(s, std::move(f));
	}(), opt, key)
{}
MappedFile::MappedFile(const char* path, FileMappingOption opt,
	FileMappingKey key)
	: MappedFile(UniqueFile(uopen(path, opt == FileMappingOption::ReadOnly
		? YCL_ReservedGlobal(O_RDONLY) : YCL_ReservedGlobal(O_RDWR))), opt, key)
{}
MappedFile::~MappedFile()
{
	if(*this)
	{
		// NOTE: At least POSIX specifiy nothing about mandontory of flush on
		//	unmapping.
		// NOTE: Windows will only flush when all shared mapping are closed, see
		//	https://msdn.microsoft.com/en-us/library/windows/desktop/aa366532(v=vs.85).aspx
		// TODO: Simplified without exceptions?
		TryExpr(Flush())
		CatchExpr(std::exception& e, YTraceDe(Descriptions::Err,
			"Failed flushing mapped file: %s.", e.what()))
		CatchExpr(..., YTraceDe(Descriptions::Err,
			"Failed flushing mapped file."));
	}
}

void
MappedFile::FlushView()
{
	YAssertNonnull(GetPtr());

#if YCL_DS
	// NOTE: Nothing to do to flush the view.
#elif YCL_Win32
	YCL_CallWin32F(::FlushViewOfFile, GetPtr(), GetSize());
	// NOTE: It should be noted %::FlushFileBuffers is required to flush file
	//	buffer, but this will be called in file descriptor flush below.
#else
	// NOTE: It is unspecified that whether data in %MAP_PRIVATE mappings has
	//	any permanent storage locations, see http://pubs.opengroup.org/onlinepubs/9699919799/functions/msync.html.
	// NOTE: To notify the system scheduling at first. See https://jira.mongodb.org/browse/SERVER-12733.
	if(::msync(GetPtr(), GetSize(), MS_ASYNC) < 0)
		ystdex::throw_error(errno);
	if(::msync(GetPtr(), GetSize(), MS_SYNC) < 0)
		ystdex::throw_error(errno);
#endif
}

void
MappedFile::FlushFile()
{
	YAssertNonnull(file);

	if(option != FileMappingOption::ReadOnly)
		// XXX: In POSIX this is perhapers not required, but keeping uniformed
		//	behavior is better. See https://groups.google.com/forum/#!topic/comp.unix.programmer/pIiaQ6CUKjU
		// XXX: This would cause bad performance for some versions of Windows
		//	supported by this project, see https://jira.mongodb.org/browse/SERVER-12401.
		file->Flush();
}

} // namespace platform;

