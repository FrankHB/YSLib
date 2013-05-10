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
\version r1502
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:36:30 +0800
\par 修改时间:
	2013-05-10 23:09 +0800
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

const Path Path::Now(u".");
const Path Path::Parent(u"..");


Path::iterator&
Path::iterator::operator++()
{
	n = n == StringType::npos ? 0 : ptr->path_string.find_first_not_of(Slash,
		ptr->path_string.find(Slash, n));
	return *this;
}

Path::iterator&
Path::iterator::operator--()
{
	n = n == 0 ? StringType::npos : ptr->path_string.rfind(Slash,
		ptr->path_string.find_last_not_of(Slash, n)) + 1;
	return *this;
}

Path::iterator::value_type
Path::iterator::operator*() const
{
	if(n == StringType::npos)
		return Path(FS_Now);

	StringType::size_type p(ptr->path_string.find(Slash, n));

	return ptr->path_string.substr(n,
		p == StringType::npos ? StringType::npos : p - n);
}


Path&
Path::operator/=(const Path& path)
{
	if(path.IsRelative() && path != Now)
		for(const_iterator i(path.begin()); i != path.end(); ++i)
		{
			if(*i == FS_Parent)
			{
				path_string.erase((--end()).GetPosition());
				if(path_string.empty())
					*this = GetRootPath();
			}
			else
			{
				path_string += (*i).path_string;
				if(IsDirectory())
					path_string += Slash;
				else
					break;
			}
		}
	return *this;
}

bool
Path::IsDirectory() const
{
	return udirexists(GetNativeString().c_str());
}

//路径分解。
Path
Path::GetRootName() const
{
	return Path(StringType(path_string.c_str(),
		GetRootNameLength(GetNativeString().c_str())));
}
Path
Path::GetRootDirectory() const
{
	return Path(FS_Separator);
}
Path
Path::GetRootPath() const
{
	return GetRootName() / GetRootDirectory();
}
Path
Path::GetRelativePath() const
{
	return path_string.empty() ? Path() : *begin();
}
Path
Path::GetParentPath() const
{
	return path_string.substr(0, (--end()).GetPosition());
}
Path
Path::GetFilename() const
{
	return path_string.empty() ? Path() : *--end();
}
Path
Path::GetStem() const
{
	const auto pos(path_string.rfind('.'));

	return pos == String::npos
		? Path(path_string) : Path(path_string.substr(0, pos));
}
Path
Path::GetExtension() const
{
	const auto pos(path_string.rfind('.'));

	return pos == String::npos ? Path() : Path(path_string.substr(pos + 1));
}

Path&
Path::MakeAbsolute(const Path&)
{
	// TODO: Implementation.
	return *this;
}

bool
Path::NormalizeTrailingSlash()
{
	if(YB_UNLIKELY(path_string.empty()))
		return false;

	const bool has_trailing_slash(path_string.back() == Slash);

	if(has_trailing_slash && path_string.size() == 1)
		return false;
	if(IsDirectory())
	{
		if(has_trailing_slash)
			return false;
		path_string.push_back(Slash);
	}
	else
	{
		if(!has_trailing_slash)
			return false;
		path_string.pop_back();
	}
	return true;
}

Path&
Path::RemoveFilename()
{
	// TODO: Implementation;
	return *this;
}

Path&
Path::ReplaceExtension(const Path& new_extension)
{
//	RemoveExtension();
	path_string += new_extension.path_string;
	return *this;
}


string
GetDirectoryNameOf(const string& path)
{
	const string::size_type p(path.rfind(YCL_PATH_DELIMITER));

	return p == string::npos ? string() : path.substr(0, p + 1);
}


int
ChangeDirectory(const string& path)
{
	if(YB_UNLIKELY(path.length() > YCL_MAX_PATH_LENGTH))
		return -2;

	return ChangeDirectory(path.c_str());
}

String
GetNowDirectory()
{
	ucs2_t buf[YCL_MAX_PATH_LENGTH];

	return u16getcwd_n(buf, YCL_MAX_PATH_LENGTH - 1)
		? String(buf) : String();
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
		(Directory = d).NormalizeTrailingSlash();
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

