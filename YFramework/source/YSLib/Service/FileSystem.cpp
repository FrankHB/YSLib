/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup Service
\brief 平台中立的文件系统抽象。
\version r2038
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:36:30 +0800
\par 修改时间:
	2015-09-12 03:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::FileSystem
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_FileSystem
#include <ystdex/cstring.h>
#include <system_error>

namespace YSLib
{

using namespace Text;

namespace IO
{

ImplDeDtor(PathNorm)


String
Path::GetString(ucs2_t delimiter) const
{
	const auto res(ystdex::to_string_d(GetBase(), delimiter));

	YAssert(res.empty() || res.back() == delimiter,
		"Invalid conversion result found.");
	return res;
}

ypath
Path::Parse(const ucs2string& str)
{
	ypath res;
	auto& norm(res.get_norm());

	ystdex::split(str, [&](ucs2_t c){
		return norm.is_delimiter({c});
	}, [&](ucs2string::const_iterator b, ucs2string::const_iterator e){
		res.push_back(ucs2string(b, e));
	});
	if(!res.empty() && !IsAbsolute(res.front()) && IsAbsolute(str.c_str()))
		res.insert(res.cbegin(), {{}});
	return res;
}

String
Path::Verify(ucs2_t delimiter) const
{
	auto res(GetString(delimiter));

	if(!(res.empty() || VerifyDirectory(res)))
		res.pop_back();
	return res;
}


String
FetchCurrentWorkingDirectory(size_t len)
{
	ucs2string str(len, ucs2_t());

	u16getcwd_n(&str[0], len);
	return str;
}

Path
MakeNormalizedAbsolute(const Path& pth, size_t len)
{
	Path res(pth);

	if(IsRelative(res))
		res = Path(FetchCurrentWorkingDirectory(len)) / res;
	res.Normalize();
	YTraceDe(Debug, "Converted path is '%s'.", string(res).c_str());
	YAssert(IsAbsolute(res), "Invalid path converted.");
	return res;
}


bool
VerifyDirectory(const char* path)
{
	try
	{
		DirectorySession ss(path);

		return true;
	}
	CatchIgnore(FileOperationFailure&)
	return {};
}


void
EnsureDirectory(const Path& pth)
{
	string upath;

	for(const auto& name : pth)
	{
		upath += MakeMBCS(name.c_str(), CS_Path) + YCL_PATH_DELIMITER;
		if(!VerifyDirectory(upath) && !umkdir(upath.c_str()) && errno != EEXIST)
		{
			YTraceDe(Err, "Failed making directory path '%s'", upath.c_str());
			throw std::system_error(errno, std::system_category());
		}
	}
}


void
DeleteTree(const Path& pth)
{
	TraverseChildren(pth, [&](NodeCategory c, const string& name){
		const auto child(pth / name);

		if(c == NodeCategory::Directory)
			DeleteTree(child);
		uremove(string(child).c_str());
	});
}

void
ListFiles(const Path& pth, vector<String>& lst)
{
	TryExpr(Traverse(pth, [&](NodeCategory c, const string& name, PathNorm& nm){
		lst.push_back(String(!nm.is_parent(name)
			&& bool(c & NodeCategory::Directory) ? name + YCL_PATH_DELIMITER
			: name, CS_Path));
	}))
	CatchIgnore(FileOperationFailure&)
}


NodeCategory
ClassifyNode(const Path& pth)
{
	if(pth.empty())
		return NodeCategory::Empty;

	const auto& fname(pth.back());

	switch(ystdex::classify_path(fname, pth.get_norm()))
	{
	case ystdex::path_category::empty:
		return NodeCategory::Empty;
	case ystdex::path_category::self:
	case ystdex::path_category::parent:
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

