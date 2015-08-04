/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file File.cpp
\ingroup Service
\brief 平台无关的文件抽象。
\version r630
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2015-08-04 12:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::File
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_File
#include YFM_YSLib_Service_FileSystem
#include <ystdex/cast.hpp> // for ystdex::pvoid;

namespace YSLib
{

File::File(const char* filename, std::ios_base::openmode mode)
	: File()
{
	Open(filename, mode);
}
ImplDeDtor(File)

size_t
File::GetPosition()
{
	const auto pos(stream.tellg());

	if(pos != fstream::pos_type(-1))
		return size_t(pos);
	throw LoggedEvent("Failed getting file position.");
}

void
File::CheckSize()
{
	stream.seekg(0, std::ios_base::end);
	fsize = GetPosition();
	GetStream().seekg(0);
}

bool
File::Open(const char* filename, std::ios_base::openmode mode)
{
	stream.open(filename, mode);
	if(*this)
	{
		stream.unsetf(std::ios_base::skipws);
		CheckSize();
		return true;
	}
	return {};
}

} // namespace YSLib;

