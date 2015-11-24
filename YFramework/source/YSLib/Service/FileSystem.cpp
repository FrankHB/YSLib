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
\version r2167
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:36:30 +0800
\par 修改时间:
	2015-11-23 23:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::FileSystem
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_FileSystem
#include <ystdex/cstring.h>
#include <ystdex/exception.h> // for ystdex::throw_error, system_error;
#include YFM_YSLib_Service_File // for OpenFile;

namespace YSLib
{

using namespace Text;

namespace IO
{

String
Path::GetString(char16_t delimiter) const
{
	const auto res(ystdex::to_string_d(GetBase(), delimiter));

	YAssert(res.empty() || res.back() == delimiter,
		"Invalid conversion result found.");
	return res;
}

ypath
Path::Parse(const u16string& str)
{
	ypath res;

	ystdex::split(str, [&](char16_t c){
		return PathTraits::IsDelimiter(c);
	}, [&](u16string::const_iterator b, u16string::const_iterator e){
		if(b != e)
			res.push_back(u16string(b, e));
	});
	// NOTE: Fix first component for absolute path beginning with delimiter.
	if(!res.empty() && IsRelative(res.front()) && IsAbsolute(str.c_str()))
		res.insert(res.cbegin(), {{}});
	return res;
}

String
Path::Verify(char16_t delimiter) const
{
	auto res(GetString(delimiter));

	if(!(res.empty() || VerifyDirectory(res)))
		res.pop_back();
	return res;
}


String
FetchCurrentWorkingDirectory(size_t len)
{
	u16string str(len, char16_t());

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
	YTraceDe(Debug, "Converted path is '%s'.", res.VerifyAsMBCS().c_str());
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
		upath += MakeMBCS(name.c_str()) + YCL_PATH_DELIMITER;
		if(!VerifyDirectory(upath) && !umkdir(upath.c_str()) && errno != EEXIST)
		{
			YTraceDe(Err, "Failed making directory path '%s'", upath.c_str());
			ystdex::throw_error(errno);
		}
	}
}


//! \since build 639
namespace
{

//! \since build 649
YB_NONNULL(1) UniqueFile
OpenFileForCopy(const char* src)
{
	return
		OpenFile(src, omode_convb(std::ios_base::in | std::ios_base::binary));
}

} // unnamed namespace;

void
CopyFile(UniqueFile p_dst, FileDescriptor src_fd)
{
	FileDescriptor::WriteContent(Nonnull(p_dst.get()), Nonnull(src_fd));
}
void
CopyFile(UniqueFile p_dst, const char* src)
{
	CopyFile(std::move(p_dst), OpenFileForCopy(src).get());
}
void
CopyFile(const char* dst, FileDescriptor src_fd, mode_t dst_mode,
	size_t allowed_links, bool share)
{
	FileDescriptor::WriteContent(EnsureUniqueFile(dst, dst_mode, allowed_links,
		share).get(), Nonnull(src_fd));
}
void
CopyFile(const char* dst, const char* src, mode_t dst_mode,
	size_t allowed_links, bool share)
{
	CopyFile(dst, OpenFileForCopy(src).get(), dst_mode, allowed_links, share);
}
void
CopyFile(const char* dst, FileDescriptor src_fd, CopyFileHandler f,
	mode_t dst_mode, size_t allowed_links, bool share)
{
	const auto p_dst(EnsureUniqueFile(dst, dst_mode, allowed_links,
		share));

	FileDescriptor::WriteContent(p_dst.get(), Nonnull(src_fd));
	f(p_dst.get(), src_fd);
}
void
CopyFile(const char* dst, const char* src, CopyFileHandler f, mode_t dst_mode,
	size_t allowed_links, bool share)
{
	CopyFile(dst, OpenFileForCopy(src).get(), f, dst_mode, allowed_links, share);
}


void
ClearTree(const Path& pth)
{
	TraverseChildren(pth.VerifyAsMBCS(),
		[&](NodeCategory c, NativePathView npv){
		const auto child(pth / String(npv));

		if(c == NodeCategory::Directory)
			DeleteTree(child);
		TryRemove(child.VerifyAsMBCS().c_str());
	});
}

void
ListFiles(const Path& pth, vector<String>& lst)
{
	Traverse(pth, [&](NodeCategory c, NativePathView npv){
		lst.push_back(!PathTraits::is_parent(npv)
			&& bool(c & NodeCategory::Directory) ? String(npv)
			+ char16_t(YCL_PATH_DELIMITER) : String(npv));
	});
}


NodeCategory
ClassifyNode(const Path& pth)
{
	if(pth.empty())
		return NodeCategory::Empty;

	const auto& fname(pth.back());

	switch(ystdex::classify_path<String, PathTraits>(fname))
	{
	case ystdex::path_category::empty:
		return NodeCategory::Empty;
	case ystdex::path_category::self:
	case ystdex::path_category::parent:
		break;
	default:
		return VerifyDirectory(pth) ? NodeCategory::Directory
			: (ufexists(String(pth).c_str()) ? NodeCategory::Regular
			: NodeCategory::Missing);
	// TODO: Implementation for other categories.
	}
	return NodeCategory::Unknown;
}

} // namespace IO;

} // namespace YSLib;

