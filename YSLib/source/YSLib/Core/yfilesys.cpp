/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfilesys.cpp
\ingroup Core
\brief 平台无关的文件系统抽象。
\version 0.2174;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-28 00:36:30 +0800;
\par 修改时间:
	2011-06-28 16:50 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFileSystem;
*/


#include "yfilesys.h"

using namespace ystdex;

YSL_BEGIN

using namespace Text;

YSL_BEGIN_NAMESPACE(IO)

const const_path_t FS_Root(DEF_PATH_ROOT);
const const_path_t FS_Seperator(DEF_PATH_SEPERATOR);
const const_path_t FS_Now(".");
const const_path_t FS_Parent("..");


namespace
{
	const uchar_t FS_Now_X[] = {'.', 0};
	const uchar_t FS_Parent_X[] = {'.', ',', 0};
}

const Path::ValueType Path::Slash(DEF_PATH_DELIMITER);
const Path Path::Now(FS_Now_X);
const Path Path::Parent(FS_Parent_X);

Path&
Path::operator/=(const Path& path)
{
	if(path.IsRelative() && path != Now)
	{
		for(const_iterator i(path.begin()); i != path.end(); ++i)
		{
			if(*i == FS_Parent)
			{
				erase((--end()).GetPosition());
				if(empty())
					*this = GetRootPath();
			}
			else
			{
				*this += *i;
				if(ValidateDirectory(*this))
					*this += Slash;
				else
					break;
			}
		}
	}
	return *this;
}

//路径分解。
Path
Path::GetRootName() const
{
	return Path(StringType(c_str(),
		GetRootNameLength(GetNativeString().c_str())));
}
Path
Path::GetRootDirectory() const
{
	return Path(FS_Seperator);
}
Path
Path::GetRootPath() const
{
	return GetRootName() / GetRootDirectory();
}
Path
Path::GetRelativePath() const
{
	return empty() ? Path() : *begin();
}
Path
Path::GetParentPath() const
{
	return substr(0, (--end()).GetPosition());
}
Path
Path::GetFilename() const
{
	return empty() ? Path() : *--end();
}
Path
Path::GetStemFrom() const
{
	return Path();
}
Path
Path::GetExtension() const
{
	return Path();
}

Path&
Path::MakeAbsolute(const Path&)
{
	// TODO: impl;
	return *this;
}
Path&
Path::RemoveFilename()
{
	// TODO: impl;
	return *this;
}
Path&
Path::ReplaceExtension(const Path& new_extension)
{
//	RemoveExtension();
	*this += new_extension;
	return *this;
}


Path::iterator&
Path::iterator::operator++()
{
	n = n == StringType::npos ? 0
		: ptr->find_first_not_of(Slash, ptr->find(Slash, n));
	return *this;
}

Path::iterator&
Path::iterator::operator--()
{
	n = n == 0 ? StringType::npos
		: ptr->rfind(Slash, ptr->find_last_not_of(Slash, n)) + 1;
	return *this;
}

Path::iterator::value_type
Path::iterator::operator*() const
{
	if(n == StringType::npos)
		return Path(FS_Now);

	StringType::size_type p(ptr->find(Slash, n));

	return ptr->substr(n, p == StringType::npos ? StringType::npos : p - n);
}


const char*
GetFileNameFrom(const_path_t path)
{
	if(!path)
		return nullptr;

	const char* p(strrchr(path, DEF_PATH_DELIMITER));

	return p ? (*++p ? p : nullptr) : path;
}
string
GetFileNameFrom(const string& path)
{
	const string::size_type p(path.rfind(DEF_PATH_DELIMITER));

	return p == string::npos ? string(path) : path.substr(p + 1);
}

string
GetDirectoryNameFrom(const string& path)
{
	const string::size_type p(path.rfind(DEF_PATH_DELIMITER));

	return p == string::npos ? string() : path.substr(0, p + 1);
}

string::size_type
SplitPath(const string& path, string& directory, string& file)
{
	const string::size_type p(path.rfind(DEF_PATH_DELIMITER));

	if(p == string::npos)
	{
		directory = "";
		file = path;
	}
	else
	{
		directory = path.substr(0, p + 1);
		file = path.substr(p + 1);
	}
	return p;
}

string
GetStemFrom(const string& name)
{
	const string::size_type p(name.rfind('.'));

	return p == string::npos ? string(name) : name.substr(0, p);
}

bool
IsStemOf(const char* str, const char* name)
{
	size_t t(strlen_n(str));

	if(t > strlen_n(name))
		return false;
	return !strncmp(str, name, strlen_n(str));
}
bool
IsStemOf(const string& str, const string& name)
{
	if(str.length() > name.length())
		return false;
	return !name.compare(0, str.length(), str);
}

bool
HaveSameStems(const char* a, const char* b)
{
	const char *pea(GetExtendNameFrom(a)), *peb(GetExtendNameFrom(b));

	if(pea - a != peb - b)
		return false;
	while(a < pea)
	{
		if(*a != *b)
			return false;
		++a;
		++b;
	}
	return true;
}
bool
HaveSameStems(const string& a, const string& b)
{
	return GetStemFrom(a) == GetStemFrom(b);
}

const char*
GetExtendNameFrom(const char* name)
{
	if(!name)
		return nullptr;

	const char* p(strrchr(name, '.'));

	return p && *++p ? p : nullptr;
}
string
GetExtendNameFrom(const string& name)
{
	const string::size_type p(name.rfind('.'));

	return p == string::npos ? string() : name.substr(p + 1);
}

bool
IsExtendNameOf(const char* str, const char* name)
{
	const char* p(GetExtendNameFrom(name));

	if(!p)
		return false;
	return !stricmp(str, p);
}
bool
IsExtendNameOf(const string& str, const string& name)
{
	if(str.length() > name.length())
		return false;
	return GetExtendNameFrom(name) == str;
}

bool
HaveSameExtendNames(const char* a, const char* b)
{
	if(!(a && b))
		return false;

	const char *pa(GetExtendNameFrom(a)), *pb(GetExtendNameFrom(b));

	if(!(pa && pb))
		return false;
	return stricmp_n(pa, pb) != 0;
}
bool
HaveSameExtendNames(const string& a, const string& b)
{
	string ea(GetExtendNameFrom(a)), eb(GetExtendNameFrom(b));

	return stricmp_n(ea.c_str(), eb.c_str()) != 0;
//	return ucsicmp(ea.c_str(), eb.c_str());
}

int
ChangeDirectory(const string& path)
{
	if(path.length() > YCL_MAX_PATH_LENGTH)
		return -2;

	return ChangeDirectory(path.c_str());
}
/*int //for String;
ChangeDirectory(const Path& path)
{
	if(path.length() > YCL_MAX_PATH_LENGTH)
		return -2;

	PATHSTR p;

	UTF16LEToMBCS(p, path.c_str());
	return ChangeDirectory(p);
}*/

string
GetNowDirectory()
{
	PATHSTR buf;

	return getcwd_n(buf, YCL_MAX_PATH_LENGTH - 1)
		? string(buf) : string();
}

bool
ValidateDirectory(const string& pathstr)
{
	return HDirectory(pathstr.c_str()).IsValid();
}


FileList::FileList(const_path_t path)
	: Directory((path && *path) ? path : FS_Root), hList(new ListType())
{}
FileList::FileList(const string& path)
	: Directory(path.empty() ? FS_Root : path.c_str()), hList(new ListType())
{}
FileList::FileList(const FileList::ItemType& path)
	: Directory(path.empty() ? FS_Root : StringToMBCS(path).c_str()),
	hList(new ListType())
{}

bool
FileList::operator/=(const string& d)
{
	Path t(Directory / d);

	if(t == Directory || !ValidateDirectory(t))
		return false;
	Directory.swap(t);
	return true;
}

FileList::ListType::size_type
FileList::LoadSubItems()
{
	HDirectory dir(Directory.GetNativeString().c_str());
	u32 n(0);

	if(dir.IsValid())
	{
		YAssert(is_null(hList),
			"Null handle found @ FileList::LoadSubItems;");

		hList->clear();

		while((++dir).LastError == 0)
			if(std::strcmp(HDirectory::Name, FS_Now) != 0)
				++n;
		hList->reserve(n);
		dir.Reset();
		while((++dir).LastError == 0)
			if(std::strcmp(HDirectory::Name, FS_Now) != 0)
				hList->push_back(std::strcmp(HDirectory::Name, FS_Parent)
					&& HDirectory::IsDirectory()
					? MBCSToString(string(HDirectory::Name) + FS_Seperator)
					: MBCSToString(HDirectory::Name));
	}
	return n;
}

FileList::ListType::size_type
FileList::ListItems()
{
	return LoadSubItems();
}

YSL_END_NAMESPACE(IO)

YSL_END

