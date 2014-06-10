/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file File.cpp
\ingroup Service
\brief 平台无关的文件抽象。
\version r534
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2014-06-10 21:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::File
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_File
#include YFM_YSLib_Service_FileSystem

namespace YSLib
{

File::File()
	: fp(), fsize(0)
{}
File::File(const char* filename, const char* mode)
	: File()
{
	if(Open(filename, mode))
	{
		Seek(0, SEEK_END);
		fsize = GetPosition();
		Rewind();
	}
}
File::File(const char* filename, std::ios_base::openmode mode)
	: File(filename, ystdex::openmode_conv(mode))
{}
File::File(const String& filename, const ucs2_t* mode)
	: File()
{
	if(Open(filename, mode))
	{
		Seek(0, SEEK_END);
		fsize = GetPosition();
		Rewind();
	}
}
File::File(const String& filename, std::ios_base::openmode mode)
	: File(filename, String(ystdex::openmode_conv(mode)).c_str())
{}

File::~File()
{
	YTraceDe(Informative, "File pointer to be closed in destructor: %p.",
		static_cast<void*>(fp));
	if(*this)
		std::fclose(fp);
}

void
File::CheckSize()
{
	Seek(0, SEEK_END);
	fsize = GetPosition();
	Rewind();
}

void
File::Close()
{
	YTraceDe(Informative, "File pointer to be closed: %p.",
		static_cast<void*>(fp));
	if(*this)
		std::fclose(fp);
	fp = {};
	YTraceDe(Informative, "File closed.");
}

bool
File::Open(const char* filename, const char* mode)
{
	if(*this)
		std::fclose(fp);
	if((fp = ufopen(filename, mode)))
		CheckSize();
	YTraceDe(Informative, "Opened file pointer: %p.", static_cast<void*>(fp));
	return fp;
}
bool
File::Open(const char* filename, std::ios_base::openmode mode)
{
	return Open(filename, ystdex::openmode_conv(mode));
}
bool
File::Open(const String& filename, const ucs2_t* mode)
{
	Close();
	if((fp = ufopen(filename.c_str(), mode)))
		CheckSize();
	return fp;
}
bool
File::Open(const String& filename, std::ios_base::openmode mode)
{
	return Open(filename, String(ystdex::openmode_conv(mode)).c_str());
}

bool
File::Truncate(size_t size) const
{
	return truncate(GetPtr(), size);
}

} // namespace YSLib;

