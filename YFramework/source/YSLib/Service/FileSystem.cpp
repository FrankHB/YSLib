/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup Service
\brief 平台中立的文件系统抽象。
\version r1895
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:36:30 +0800
\par 修改时间:
	2014-02-15 16:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::FileSystem
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_FileSystem
#include <ystdex/cstring.h>

namespace YSLib
{

using namespace Text;

namespace IO
{

Path&
Path::operator/=(const String& fname)
{
	auto& norm(get_norm());

	if(norm.is_parent(fname))
	{
		if((is_absolute() ? 1 : 0) < size())
			pop_back();
	}
	else if(!norm.is_self(fname))
		push_back(fname);
	return *this;
}
Path&
Path::operator/=(const Path& pth)
{
	for(const auto& fname : pth)
		*this /= fname;
	return *this;
}

Path::operator String() const
{
	auto res(GetString());

	if(!(res.empty() || VerifyDirectory(res)))
		res.pop_back();
	return res;
}

String
Path::GetString() const
{
	auto res(ystdex::to_string_d(static_cast<const ypath&>(*this),
		YCL_PATH_DELIMITER));

	YAssert(res.empty() || res.back() == YCL_PATH_DELIMITER,
		"Invalid conversion result found.");

	return res;
}

ypath
Path::Parse(const ucs2string& str)
{
	ypath res;
	auto& norm(res.get_norm());

	ystdex::split(str, [&](ucs2_t c){
		return norm.is_delimiter(ucs2string{c});
	}, [&](ucs2string::const_iterator b, ucs2string::const_iterator e){
		res.push_back(ucs2string(b, e));
	});
	return res;
}


String
GetExtensionOf(const String& fname)
{
	if(!fname.empty())
	{
		const auto pos(fname.rfind('.'));

		if(pos != String::npos)
			return fname.substr(pos + 1);
	}
	return {};
}


String
FetchCurrentWorkingDirectory(size_t len)
{
	ucs2string str(len, ucs2_t());

	u16getcwd_n(&str[0], len);
	return str;
}


bool
VerifyDirectory(const char* path)
{
	try
	{
		DirectorySession ss(path);

		return true;
	}
	catch(FileOperationFailure&)
	{}
	return false;
}


void
ListFiles(const Path& pth, vector<String>& lst)
{
	try
	{
		HDirectory dir(string(pth).c_str());
		PathNorm nm;

		std::for_each(FileIterator(&dir), FileIterator(),
			[&](const std::string& name){
			if(YB_LIKELY(!nm.is_self(name)))
				lst.push_back(String(!nm.is_parent(name)
					&& dir.GetNodeCategory() == NodeCategory::Directory
					? name + YCL_PATH_DELIMITER : name, CS_Path));
		});
	}
	catch(FileOperationFailure&)
	{}
}


PathCategory
ClassifyPath(const String& fname, ypath::norm&& norm)
{
	if(YB_UNLIKELY(fname.empty()))
		return PathCategory::Empty;
	if(norm.is_self(fname))
		return PathCategory::Self;
	if(norm.is_parent(fname))
		return PathCategory::Parent;
	return PathCategory::Node;
}

NodeCategory
ClassifyNode(const Path& pth)
{
	if(pth.empty())
		return NodeCategory::Empty;

	const auto& fname(pth.back());

	switch(ClassifyPath(fname, std::move(pth.get_norm())))
	{
	case PathCategory::Empty:
		return NodeCategory::Empty;
	case PathCategory::Self:
	case PathCategory::Parent:
		break;
	default:
		if(ufexists(string(pth)))
			return VerifyDirectory(pth)
				? NodeCategory::Directory : NodeCategory::Regular;
		else
			return NodeCategory::Missing;
	// TODO: Implementation for other categories.
	}
	return NodeCategory::Unknown;
}

} // namespace IO;

} // namespace YSLib;

