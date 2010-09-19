// YSLib::Core::YFileSystem by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2010-3-28 0:36:30;
// UTime = 2010-9-20 6:39;
// Version = 0.1737;


#include "yfilesys.h"

YSL_BEGIN

using namespace Text;

YSL_BEGIN_NAMESPACE(IO)

const CPATH FS_Root(DEF_PATH_ROOT);
const CPATH FS_Seperator(DEF_PATH_SEPERATOR);
const CPATH FS_Now(".");
const CPATH FS_Parent("..");


Path&
Path::operator/=(const Path& path)
{
//	pathname += Slash;
	pathname += path.pathname;
	return *this;
}

bool
Path::IsAbsolute() const
{
	return false;
}
bool
Path::IsRelative() const
{
	return false;
}
bool
Path::HasRootName() const
{
	return false;
}
bool
Path::HasRootDirectory() const
{
	return false;
}
bool
Path::HasRootPath() const
{
	return false;
}
bool
Path::HasRelativePath() const
{
	return false;
}
bool
Path::HasParentPath() const
{
	return false;
}
bool
Path::HasFilename() const
{
	return false;
}
bool
Path::HasStem() const
{
	return false;
}
bool
Path::HasExtension() const
{
	return false;
}

//路径分解。
Path
Path::GetRootName() const
{
	return Path();
}
Path
Path::GetRootDirectory() const
{
	return Path();
}
Path
Path::GetRootPath() const
{
	return Path();
}
Path
Path::GetRelativePath() const
{
	return Path();
}
Path
Path::GetParentPath() const
{
	return Path();
}
Path
Path::GetFilename() const
{
	return Path();
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
	pathname += new_extension.pathname;
	return *this;
}


const char*
GetFileName(CPATH path)
{
	if(!path)
		return NULL;

	const char* p(strrchr(path, DEF_PATH_DELIMITER));

	return p ? (*++p ? p : NULL) : path;
}
std::string
GetFileName(const std::string& path)
{
	const std::string::size_type p(path.rfind(DEF_PATH_DELIMITER));

	return p == std::string::npos ? std::string(path) : path.substr(p + 1);
}

std::string
GetDirectoryName(const std::string& path)
{
	const std::string::size_type p(path.rfind(DEF_PATH_DELIMITER));

	return p == std::string::npos ? std::string() : path.substr(0, p + 1);
}

std::string::size_type
SplitPath(const std::string& path, std::string& directory, std::string& file)
{
	const std::string::size_type p(path.rfind(DEF_PATH_DELIMITER));

	if(p == std::string::npos)
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

std::string
GetStem(const std::string& name)
{
	const std::string::size_type p(name.rfind('.'));

	return p == std::string::npos ? std::string(name) : name.substr(0, p);
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
IsStem(const std::string& str, const std::string& name)
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
HaveSameStems(const std::string& a, const std::string& b)
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
std::string
GetExtendName(const std::string& name)
{
	const std::string::size_type p(name.rfind('.'));

	return p == std::string::npos ? std::string() : name.substr(p + 1);
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
IsExtendName(const std::string& str, const std::string& name)
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
HaveSameExtendNames(const std::string& a, const std::string& b)
{
	std::string ea(GetExtendName(a)), eb(GetExtendName(b));

	return stdex::stricmp_n(ea.c_str(), eb.c_str()) != 0;
//	return ucsicmp(ea.c_str(), eb.c_str());
}

int
ChDir(const std::string& path)
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

std::string
GetNowDirectory()
{
	PATHSTR buf;

	return platform::getcwd_n(buf, MAX_PATH_LENGTH - 1) == NULL ? std::string() : std::string(buf);
}


MFileList::MFileList()
: Directory(GetNowDirectory()), List()
{}
MFileList::~MFileList()
{}

MFileList::ListType::size_type
MFileList::LoadSubItems()
{
	HDirectory dir(FS_Now);
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
				List.push_back(HDirectory::Stat.st_mode & S_IFDIR ?
					MBCSToString(HDirectory::Name) + String(FS_Seperator)
					: MBCSToString(HDirectory::Name));
	}
	return n;
}

MFileList::ListType::size_type
MFileList::ListItems()
{
	return LoadSubItems();
}

void
MFileList::GoToPath(const Path& p)
{
	Directory = GetDirectoryName(p.GetNativeString());
	if(ChDir(Directory.c_str()))
		Directory = GetNowDirectory();
}
void
MFileList::GoToSubDirectory(const std::string& d)
{
	if(ChDir(d.c_str()))
		Directory = GetNowDirectory();
	else
		Directory /= d;
}
void
MFileList::GoToParent()
{
	ChDir(FS_Parent);
	Directory = GetNowDirectory();
}

YSL_END_NAMESPACE(IO)

YSL_END

