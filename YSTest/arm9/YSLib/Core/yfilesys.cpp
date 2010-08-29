// YSLib::Core::YFileSystem by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2010-3-28 0:36:30;
// UTime = 2010-8-29 6:29;
// Version = 0.1633;


#include "yfilesys.h"
#include "../Shell/ywindow.h" // for HWND delete procedure;

YSL_BEGIN

using namespace Text;

YSL_BEGIN_NAMESPACE(IO)

const CPATH FS_Root(DEF_PATH_ROOT);
const CPATH FS_Seperator(DEF_PATH_SEPERATOR);
const CPATH FS_Now(".");
const CPATH FS_Parent("..");


const char*
GetFileName(CPATH path)
{
	if(!path)
		return NULL;

	const char* p(strrchr(path, DEF_PATH_DELIMITER));

	return p ? (*++p ? p : NULL) : path;
}
std::string
GetFileName(const MPath& path)
{
	const MPath::size_type p(path.rfind(DEF_PATH_DELIMITER));

	return p == MPath::npos ? MPath(path) : path.substr(p + 1);
}

std::string
GetDirectoryName(const MPath& path)
{
	const MPath::size_type p(path.rfind(DEF_PATH_DELIMITER));

	return p == MPath::npos ? MPath() : path.substr(0, p + 1);
}

MPath::size_type
SplitPath(const MPath& path, MPath& directory, std::string& file)
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
GetBaseName(const std::string& name)
{
	const std::string::size_type p(name.rfind('.'));

	return p == std::string::npos ? std::string(name) : name.substr(0, p);
}

bool
IsBaseName(const char* str, const char* name)
{
	using stdex::strlen_n;

	std::size_t t(strlen_n(str));

	if(t > strlen_n(name))
		return false;
	return !strncmp(str, name, strlen_n(str));
}
bool
IsBaseName(const std::string& str, const std::string& name)
{
	if(str.length() > name.length())
		return false;
	return !name.compare(0, str.length(), str);
}

bool
HaveSameBaseNames(const char* a, const char* b)
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
HaveSameBaseNames(const std::string& a, const std::string& b)
{
	return GetBaseName(a) == GetBaseName(b);
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
ChDir(const MPath& path)
{
	if(path.length() > MAX_PATH_LENGTH)
		return -2;

	return ChDir(path.c_str());
}
/*int //for MString;
ChDir(const MPath& path)
{
	if(path.length() > MAX_PATH_LENGTH)
		return -2;

	PATHSTR p;

	UTF16LEToMBCS(p, path.c_str());
	return ChDir(p);
}*/

MPath
GetNowDirectory()
{
	PATHSTR buf;

	return getcwd(buf, MAX_PATH_LENGTH - 1) != NULL ? std::string() : std::string(buf);
}


MFileList::MFileList()
: Directory(GetNowDirectory()), List()
{}
MFileList::~MFileList()
{}

u32
MFileList::LoadSubItems()
{
	DirIter dir(FS_Now);
	u32 n(0);

	if(dir.IsValid())
	{
		List.clear();

		while((++dir).LastError == 0)
			if(std::strcmp(DirIter::Name, FS_Now) != 0)
				++n;
		List.reserve(n);
		dir.Reset();
		while((++dir).LastError == 0)
			if(std::strcmp(DirIter::Name, FS_Now) != 0)
				List.push_back(DirIter::Stat.st_mode & S_IFDIR ?
					MBCSToMString(DirIter::Name) + MString(FS_Seperator)
					: MBCSToMString(DirIter::Name));
	}
	return n;
}

u32
MFileList::ListItems()
{
	return LoadSubItems();
}

void
MFileList::GoToPath(const MPath& p)
{
	Directory = GetDirectoryName(p);
	if(ChDir(Directory.c_str()))
		Directory = GetNowDirectory();
}
void
MFileList::GoToSubDirectory(const std::string& d)
{
	if(ChDir(d.c_str()))
		Directory = GetNowDirectory();
	else
		Directory += d;
}
void
MFileList::GoToParent()
{
	ChDir(FS_Parent);
	Directory = GetNowDirectory();
}

YSL_END_NAMESPACE(IO)

YSL_END

