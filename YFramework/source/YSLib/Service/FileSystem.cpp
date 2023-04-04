/*
	© 2010-2016, 2019-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup Service
\brief 平台中立的文件系统抽象。
\version r2397
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:36:30 +0800
\par 修改时间:
	2023-04-04 00:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::FileSystem
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_FileSystem // for FetchCurrentWorkingDirectory,
//	IsAbsolute, DirectorySession, umkdir, MakeMBCS, FetchSeparator,
//	use_openmode_t;
#include <ios> // for std::ios_base::in, std::ios_base::binary;
#include <cerrno> // for errno, EEXIST;
#include <ystdex/exception.h> // for std::system_error, ystdex::throw_error;
#include YFM_YSLib_Service_File // for OpenFile;

namespace YSLib
{

using namespace Text;

namespace IO
{

String
Path::GetString(char16_t delimiter) const
{
	auto res(to_string_d(GetBase(), get_allocator(), delimiter));

	YAssert(res.empty() || res.back() == delimiter,
		"Invalid conversion result found.");
	return res;
}

String
Path::Verify(char16_t delimiter) const
{
	return VerifyDirectoryPathTail(GetString(delimiter));
}


Path
MakeNormalizedAbsolute(Path&& res, size_t init_size)
{
	if(IsRelative(res))
#if true
	{
		// XXX: This is a bit more efficient, at least with x86_64-pc-linux G++
		//	12.1.
		Path pa(FetchCurrentWorkingDirectory<char16_t>(init_size,
			res.get_allocator()));

		res = std::move(pa) / std::move(res);
	}
#else
		// NOTE: Any optimized implemenations shall be equivalent to this,
		res = Path(FetchCurrentWorkingDirectory<char16_t>(init_size,
			res.get_allocator())) / std::move(res);
#endif
	res.Normalize();
	YTraceDe(Debug, "Converted path is '%s'.", res.VerifyAsMBCS().c_str());
	YAssert(IsAbsolute(res), "Invalid path converted.");
	return std::move(res);
}


//! \since build 699
namespace
{

//! \since build 970
YB_NONNULL(2, 3) void
CheckCreateDiretoryError(int err, const char* path, const char* sig)
{
	if(err != EEXIST)
	{
		YTraceDe(Err, "Failed making directory path '%s'", path);
		ystdex::throw_error(err, sig);
	}
}

} // unnamed namespace;

bool
CreateDirectory(const char* path)
{
	const bool success(umkdir(path));

	if(YB_UNLIKELY(!success))
		CheckCreateDiretoryError(errno, path, yfsig);
	return success;
}

bool
EnsureDirectory(const Path& pth)
{
	// NOTE: Similar to %ystdex::to_string_d for %ystdex::path.
	if(!pth.empty())
	{
		bool success(true);
		auto i(pth.begin());
		auto res(i->GetMBCS());
		const auto sep(FetchSeparator<char>());
		const auto ensure([&]{
			if(!VerifyDirectory(res) && !umkdir(res.c_str()))
			{
				CheckCreateDiretoryError(errno, res.c_str(), yfsig);
				success = {};
			}
		});

		if(!PathTraits::has_root_path(res))
		{
			res += sep;
			ensure();
		}
		while(++i != pth.end())
		{
			res += i->GetMBCS();
			res += sep;
			ensure();
		}
		return success;
	}
	return {};
}


//! \since build 639
namespace
{

//! \since build 649
YB_NONNULL(1) UniqueFile
OpenFileForCopy(const char* src)
{
	return OpenFile(src, use_openmode_t(),
		std::ios_base::in | std::ios_base::binary, mode_t());
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
	// NOTE: Guards are not used since the post operations should only be
	//	performed after success copying.
	f(p_dst.get(), src_fd);
}
void
CopyFile(const char* dst, const char* src, CopyFileHandler f, mode_t dst_mode,
	size_t allowed_links, bool share)
{
	CopyFile(dst, OpenFileForCopy(src).get(), f, dst_mode, allowed_links,
		share);
}


void
ClearTree(const Path& pth)
{
	TraverseChildren(pth, [&](NodeCategory c, NativePathView npv){
		const auto child(pth / String(npv));

		if(c == NodeCategory::Directory)
			DeleteTree(child);
		Remove(string(child).c_str());
	});
}

void
ListFiles(const Path& pth, vector<String>& lst)
{
	Traverse(pth, [&](NodeCategory c, NativePathView npv){
		lst.push_back(!PathTraits::is_parent(npv)
			&& bool(c & NodeCategory::Directory) ? String(npv)
			+ FetchSeparator<char16_t>() : String(npv));
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

namespace Deployment
{

using namespace IO;

void
InstallFile(const char* dst, const char* src)
{
	// FIXME: Blocked. TOCTTOU access.
	try
	{
		if(HaveSameContents(dst, src))
			return;
	}
	CatchExpr(std::system_error& e, YTraceDe(Err, "Failed accessing file for"
		" comparison before installing file."), ExtractAndTrace(e, Err))
	CopyFile(dst, src, PreserveModificationTime);
}

void
InstallDirectory(const string& dst, const string& src)
{
	TraverseTree([](const Path& dname, const Path& sname){
		InstallFile(string(dname).c_str(), string(sname).c_str());
	}, Path(dst, dst.get_allocator()), Path(src, src.get_allocator()));
}

void
InstallHardLink(const char* dst, const char* src)
{
	if(VerifyDirectory(src))
		throw std::invalid_argument("Source is a directory.");
	Remove(dst);
	TryExpr(CreateHardLink(dst, src))
	CatchExpr(..., InstallFile(dst, src))
}

void
InstallSymbolicLink(const char* dst, const char* src)
{
	Remove(dst);
	TryExpr(CreateSymbolicLink(dst, src))
	CatchExpr(..., InstallFile(dst, src))
}

void
InstallExecutable(const char* dst, const char* src)
{
	InstallFile(dst, src);
}

} // namespace Deployment;

} // namespace YSLib;

