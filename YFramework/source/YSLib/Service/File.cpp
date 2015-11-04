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
\brief 平台中立的文件抽象。
\version r681
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-24 23:14:51 +0800
\par 修改时间:
	2015-10-25 18:02 +0800
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

UniqueFile
OpenFile(const char* filename, int omode, mode_t pmode)
{
	if(UniqueFile p_ifile{uopen(filename, omode, pmode)})
		return p_ifile;
	else
		ystdex::throw_error<FileOperationFailure>(errno,
			"Failed opening file '" + string(filename) + "'.");
}

} // namespace IO;

} // namespace YSLib;

