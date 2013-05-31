/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfilesys.cpp
\ingroup Core
\brief 平台无关的文件系统抽象。
\version r1668
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:36:30 +0800
\par 修改时间:
	2013-05-31 21:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YFileSystem
*/


#include "YSLib/Core/yfilesys.h"
#include <ystdex/cstring.h>

using namespace ystdex;

YSL_BEGIN

using namespace Text;

YSL_BEGIN_NAMESPACE(IO)

namespace
{

//! \since build 409
String
ConvertPath(const ypath& pth)
{
	return ystdex::to_string_d(pth, YCL_PATH_DELIMITER);
}

} // unnamed namespace;

Path&
Path::operator/=(const Path& path)
{
	auto& rnorm(path.get_norm());

	if(!path.empty() && path.is_relative())
		for(const auto& s : path)
			if(rnorm.is_parent(s))
			{
				if(empty())
					*this = {get_root()};
				else
					pop_back();
			}
			else if(!rnorm.is_self(s))
				push_back(s);
	return *this;
}

Path::operator String() const
{
	auto res(ConvertPath(*this));

	if(!res.empty() && res.back() == YCL_PATH_DELIMITER && !IsDirectory())
		res.pop_back();
	return std::move(res);
}

bool
Path::IsDirectory() const
{
	return !empty() && udirexists(ConvertPath(*this).GetMBCS(CS_Path).c_str());
}

String
Path::GetExtension() const
{
	if(!empty())
	{
		const auto& fname(back());
		const auto pos(fname.rfind('.'));

		if(pos != String::npos)
			return fname.substr(pos + 1);
	}
	return {};
}

ypath
Path::Parse(const ucs2string& str)
{
	ypath res;
	auto& norm(res.get_norm());

	ystdex::split(str, [&](ucs2_t c){
		return norm.is_delimiter(ucs2string{c});
	}, [&](ucs2string::const_iterator b,
		ucs2string::const_iterator e){
		res.push_back(ucs2string(b, e));
	});
	return std::move(res);
}


String
GetNowDirectory()
{
	ucs2_t buf[YCL_MAX_PATH_LENGTH];

	return u16getcwd_n(buf, YCL_MAX_PATH_LENGTH - 1) ? String(buf) : String();
}

bool
ValidatePath(const string& pathstr)
{
	return bool(HFileNode(pathstr.c_str()));
}


FileList::FileList(const_path_t path)
	: Directory((path && *path) ? path : FS_Root), hList(new ListType())
{}
FileList::FileList(const string& path)
	: Directory(path.empty() ? FS_Root : path.c_str()), hList(new ListType())
{}
FileList::FileList(const FileList::ItemType& path)
	: Directory(path.empty() ? FS_Root : path.GetMBCS(CS_Path).c_str()),
	hList(new ListType())
{}

bool
FileList::operator=(const Path& d)
{
	if(d.IsDirectory())
	{
		Directory = d;
		ListItems();
		return true;
	}
	return false;
}
bool
FileList::operator/=(const Path& d)
{
	return *this = (Directory / d);
}


FileList::ListType::size_type
FileList::ListItems()
{
	HFileNode dir(Directory.GetNativeString().c_str());
	u32 n(0);

	if(dir)
	{
		YAssert(bool(hList), "Null handle found.");

		hList->clear();
		while((++dir).LastError == 0)
			if(YB_LIKELY(std::strcmp(dir.GetName(), FS_Now) != 0))
				++n;
		hList->reserve(n);
		dir.Reset();
		while((++dir).LastError == 0)
			if(YB_LIKELY(std::strcmp(dir.GetName(), FS_Now) != 0))
				hList->push_back(std::strcmp(dir.GetName(), FS_Parent)
					&& dir.IsDirectory() ? String(
					string(dir.GetName()) + FS_Separator, CS_Path)
					: String(dir.GetName(), CS_Path));
		// TODO: Platform-dependent name converting.
	}
	return n;
}

YSL_END_NAMESPACE(IO)

YSL_END

