// YSLib::Core::YFileSystem by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2010-03-28 00:36:30 + 08:00;
// UTime = 2010-10-24 14:52 + 08:00;
// Version = 0.1944;


#include "yfilesys.h"

YSL_BEGIN

using namespace Text;

YSL_BEGIN_NAMESPACE(IO)

const CPATH FS_Root(DEF_PATH_ROOT);
const CPATH FS_Seperator(DEF_PATH_SEPERATOR);
const CPATH FS_Now(".");
const CPATH FS_Parent("..");


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
		platform::GetRootNameLength(GetNativeString().c_str())));
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
Path::GetStem() const
{
	return Path();
}
Path
Path::GetExtension() const
{
	return Path();
}

Path&
Path::MakeAbsolute(const Path& base)
{
	return *this;
}
Path&
Path::RemoveFilename()
{
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
GetFileName(CPATH path)
{
	if(!path)
		return NULL;

	const char* p(strrchr(path, DEF_PATH_DELIMITER));

	return p ? (*++p ? p : NULL) : path;
}
string
GetFileName(const string& path)
{
	const string::size_type p(path.rfind(DEF_PATH_DELIMITER));

	return p == string::npos ? string(path) : path.substr(p + 1);
}

string
GetDirectoryName(const string& path)
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
GetStem(const string& name)
{
	const string::size_type p(name.rfind('.'));

	return p == string::npos ? string(name) : name.substr(0, p);
}

bool
IsStem(const char* str, const char* name)
{
	using stdex::strlen_n;

	std::size_t t(strlen_n(str));

	if(t > strlen_n(name))
		return false;
	return !strncmp(str, name, strlen_n(str));
}
bool
IsStem(const string& str, const string& name)
{
	if(str.length() > name.length())
		return false;
	return !name.compare(0, str.length(), str);
}

bool
HaveSameStems(const char* a, const char* b)
{
	const char *pea(GetExtendName(a)), *peb(GetExtendName(b));

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
	return GetStem(a) == GetStem(b);
}

const char*
GetExtendName(const char* name)
{
	if(!name)
		return NULL;

	const char* p(strrchr(name, '.'));

	return p && *++p ? p : NULL;
}
string
GetExtendName(const string& name)
{
	const string::size_type p(name.rfind('.'));

	return p == string::npos ? string() : name.substr(p + 1);
}

bool
IsExtendName(const char* str, const char* name)
{
	const char* p(GetExtendName(name));

	if(!p)
		return false;
	return !stricmp(str, p);
}
bool
IsExtendName(const string& str, const string& name)
{
	if(str.length() > name.length())
		return false;
	return GetExtendName(name) == str;
}

bool
HaveSameExtendNames(const char* a, const char* b)
{
	if(!(a && b))
		return false;

	const char *pa(GetExtendName(a)), *pb(GetExtendName(b));

	if(!(pa && pb))
		return false;
	return stdex::stricmp_n(pa, pb) != 0;
}
bool
HaveSameExtendNames(const string& a, const string& b)
{
	string ea(GetExtendName(a)), eb(GetExtendName(b));

	return stdex::stricmp_n(ea.c_str(), eb.c_str()) != 0;
//	return ucsicmp(ea.c_str(), eb.c_str());
}

int
ChDir(const string& path)
{
	if(path.length() > MAX_PATH_LENGTH)
		return -2;

	return ChDir(path.c_str());
}
/*int //for String;
ChDir(const Path& path)
{
	if(path.length() > MAX_PATH_LENGTH)
		return -2;

	PATHSTR p;

	UTF16LEToMBCS(p, path.c_str());
	return ChDir(p);
}*/

string
GetNowDirectory()
{
	PATHSTR buf;

	return platform::getcwd_n(buf, MAX_PATH_LENGTH - 1) == NULL
		? string() : string(buf);
}

bool
ValidateDirectory(const string& pathstr)
{
	return HDirectory(pathstr.c_str()).IsValid();
}


MFileList::MFileList(CPATH pathstr)
	: Directory(pathstr == NULL ? FS_Root : pathstr), List()
{
//	if(!ValidateDirectory(Directory))
//		throw;
}

bool
MFileList::operator/=(const string& d)
{
	Path t(Directory / d);

	if(t == Directory || !ValidateDirectory(t))
		return false;
	Directory.swap(t);
	return true;
}

MFileList::ListType::size_type
MFileList::LoadSubItems()
{
	HDirectory dir(Directory.GetNativeString().c_str());
	u32 n(0);

	if(dir.IsValid())
	{
		List.clear();

		while((++dir).LastError == 0)
			if(std::strcmp(HDirectory::Name, FS_Now) != 0)
				++n;
		List.reserve(n);
		dir.Reset();
		while((++dir).LastError == 0)
			if(std::strcmp(HDirectory::Name, FS_Now) != 0)
				List.push_back(std::strcmp(HDirectory::Name, FS_Parent)
					&& HDirectory::IsDirectory()
					? MBCSToString(string(HDirectory::Name) + FS_Seperator)
					: MBCSToString(HDirectory::Name));
	}
	return n;
}

MFileList::ListType::size_type
MFileList::ListItems()
{
	return LoadSubItems();
}

YSL_END_NAMESPACE(IO)

YSL_END

