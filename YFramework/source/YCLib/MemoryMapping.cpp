﻿/*
	© 2012-2016, 2018-2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MemoryMapping.cpp
\ingroup YCLib
\brief 内存映射文件。
\version r527
\author FrankHB <frankhb1989@gmail.com>
\since build 324
\par 创建时间:
	2012-07-11 21:59:21 +0800
\par 修改时间:
	2021-12-26 13:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::MemoryMapping
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_MemoryMapping // for wcast;
#include YFM_YCLib_FileIO // for uopen, YCL_Raise_SysE, YCL_CallF_CAPI,
//	std::throw_with_nested, std::runtime_error, ystdex::throw_error,
//	std::errc::bad_file_descriptor;
#include YFM_YCLib_NativeAPI // for ::UnmapViewOfFile, ::munmap,
//	platform_ex::ToHandle, CreateFileMapping, PAGE_READONLY, PAGE_READWRITE,
//	MapViewOfFile, FILE_MAP_READ, FILE_MAP_ALL_ACCESS, FILE_MAP_COPY, ::mmap,
//	PROT_READ, PROT_WRITE, ::lseek, MAP_PRIVATE, MAP_SHARED, MAP_FAILED;
#include <ystdex/cast.hpp> // for ystdex::narrow;
#if YCL_Win32
#	include YFM_YCLib_Host // for platform_ex::UniqueHandle;
#	include YFM_Win32_YCLib_MinGW32 // for YCL_CallF_Win32;
#endif

namespace platform
{

#if !YCL_DS
void
UnmapDelete::operator()(pointer p) const ynothrowv
{
	YAssertNonnull(p);

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
	len != 0 ? new byte[len] : nullptr
#else
	[this, off, len, opt, key]() -> byte*{
		try
		{
			if(len != 0)
			{
#	if YCL_Win32
				const auto h(platform_ex::ToHandle(*file.get()));

				if(h != INVALID_HANDLE_VALUE)
				{
					platform_ex::UniqueHandle fm(
						YCL_CallF_Win32(CreateFileMappingW, h, {},
						opt != FileMappingOption::ReadWrite ? PAGE_READONLY
						: PAGE_READWRITE, 0, 0, wcast(key)));

					return static_cast<byte*>(YCL_CallF_Win32(MapViewOfFile,
						fm.get(), opt != FileMappingOption::CopyOnWrite
						? (opt == FileMappingOption::ReadOnly ? FILE_MAP_READ
						: FILE_MAP_ALL_ACCESS) : FILE_MAP_COPY,
						static_cast<unsigned long>(off >> 32UL),
						static_cast<unsigned long>(off), len));
				}
				YCL_Raise_SysE(, "::_get_osfhandle", yfsig);
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
				YCL_Raise_SysE(, "::mmap", yfsig);
#	endif
			}
			else
				return {};
		}
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
		if(off != 0)
			YCL_CallF_CAPI(, ::lseek, *file.get(), ::off_t(off), SEEK_SET);
		YCL_CallF_CAPI(, ::read, *file.get(), GetPtr(), len);
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
		if(f)
		{
			const auto s(ystdex::not_widen<size_t>(f->GetSize()));

			return pair<size_t, UniqueFile>(s, std::move(f));
		}
		ystdex::throw_error(std::errc::bad_file_descriptor, yfsig);
	}(), opt, key)
{}
MappedFile::MappedFile(const char* path, FileMappingOption opt,
	FileMappingKey key)
	: MappedFile([&]{
		if(UniqueFile ufile{uopen(path, int((opt == FileMappingOption::ReadOnly
			? OpenMode::Read : OpenMode::ReadWrite) | OpenMode::Binary)
		)})
			return ufile;
		YCL_Raise_SysE(, "Failed opening file", yfsig);
	}(), opt, key)
{}
MappedFile::~MappedFile()
{
	if(*this && GetSize() != 0)
	{
		// NOTE: At least POSIX specifiy nothing about mandontory of flush on
		//	unmapping.
		// NOTE: Windows will only flush when all shared mapping are closed, see
		//	https://msdn.microsoft.com/library/windows/desktop/aa366532(v=vs.85).aspx
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
	YCL_CallF_Win32(::FlushViewOfFile, GetPtr(), GetSize());
	// NOTE: It should be noted %::FlushFileBuffers is required to flush file
	//	buffer, but this will be called in file descriptor flush below in the
	//	call of %FlushFile. This should be also immune to the kernel bug of some
	//	versions of Microsoft Windows 10, see https://randomascii.wordpress.com/2018/02/25/compiler-bug-linker-bug-windows-kernel-bug/.
#else
	// NOTE: It is unspecified that whether data in %MAP_PRIVATE mappings has
	//	any permanent storage locations, see http://pubs.opengroup.org/onlinepubs/9699919799/functions/msync.html.
	// NOTE: To notify the system scheduling at first. See https://jira.mongodb.org/browse/SERVER-12733.
	YCL_CallF_CAPI(, ::msync, GetPtr(), GetSize(), MS_ASYNC);
	YCL_CallF_CAPI(, ::msync, GetPtr(), GetSize(), MS_SYNC);
#endif
}

void
MappedFile::FlushFile()
{
	YAssertNonnull(file);

	// XXX: This can be irrelevant in DS. However, keeping same to make it
	//	simpler.
	if(option != FileMappingOption::ReadOnly)
		// XXX: In POSIX this is perhaps not required, but keeping uniformed
		//	behavior is better. See https://groups.google.com/forum/#!topic/comp.unix.programmer/pIiaQ6CUKjU.
		// TODO: Improve performance on Windows?
		// XXX: This would cause bad performance in Windows until Windows 8 and
		//	Windows 2012, see https://jira.mongodb.org/browse/SERVER-12401.
		file->Flush();
}

} // namespace platform;

