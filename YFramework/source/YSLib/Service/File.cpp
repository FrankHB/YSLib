/*
	© 2009-2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file File.cpp
\ingroup Service
\brief 平台中立的文件抽象。
\version r729
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2021-08-03 19:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::File
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_File

namespace YSLib
{

namespace IO
{

SharedInputMappedFileStream::SharedInputMappedFileStream(const char* path)
	: MappedFile(path), SharedIndirectLockGuard<const UniqueFile>(
 	GetUniqueFile()), ystdex::membuf(ystdex::replace_cast<const char*>(
 	GetPtr()), GetSize()), std::istream(this)
{}

ImplDeDtor(SharedInputMappedFileStream)


UniqueLockedOutputFileStream::UniqueLockedOutputFileStream(int fd,
	std::ios_base::openmode mode)
	: ofstream(UniqueFile(fd), mode), desc(fd), lock()
{
	if(*this)
		lock = unique_lock<FileDescriptor>(desc);
}
UniqueLockedOutputFileStream::UniqueLockedOutputFileStream(UniqueFile ufile,
	std::ios_base::openmode mode)
	: UniqueLockedOutputFileStream(*ufile.get(), mode)
{
	ufile.release();
}

ImplDeDtor(UniqueLockedOutputFileStream)

} // namespace IO;

} // namespace YSLib;

