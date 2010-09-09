// YSLib::Core::YFileSystem by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-28 0:09:28;
// UTime = 2010-9-3 23:02;
// Version = 0.1596;


#ifndef INCLUDED_YFILESYS_H_
#define INCLUDED_YFILESYS_H_

// YFileSystem ：平台无关的文件处理系统。

#include "ystring.h"
#include "yfunc.hpp"
#include "../Helper/yglobal.h"
#include "../Core/yshell.h" // for HSHL delete procedure;
#include <vector>
#include <list>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(IO)

//文件系统常量：前缀 FS 表示文件系统 (File System) 。
extern const CPATH FS_Root;
extern const CPATH FS_Seperator;
extern const CPATH FS_Now;
extern const CPATH FS_Parent;


typedef char NativePathCharType; //本地路径字符类型，POSIX 为 char ，Windows 为 wchar_t。


//路径类。
class Path
{
public:
	typedef NativePathCharType value_type;
	typedef std::basic_string<value_type> StringType;
//	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

private:
	StringType pathname;

public:
	//编码转换。
//	static std::locale imbue(const std::locale&);
//	static const codecvt_type& codecvt();

	//构造函数和析构函数。
	Path();
	Path(const Path&);
	template<class _tString>
	Path(const _tString&);
	~Path();

	//赋值。
	Path&
	operator=(const Path&);
	template<class _tString>
	Path&
	operator=(const _tString&);

	//追加路径。
	Path&
	operator/=(const Path&);

	//查询。
	DefBoolGetter(Empth, pathname.empty())
	bool
	IsAbsolute() const;
	bool
	IsRelative() const;
	bool
	HasRootName() const;
	bool
	HasRootDirectory() const;
	bool
	HasRootPath() const;
	bool
	HasRelativePath() const;
	bool
	HasParentPath() const;
	bool
	HasFilename() const;
	bool
	HasStem() const;
	bool HasExtension() const;

	//路径分解。
	Path
	GetRootName() const;
	Path
	GetRootDirectory() const;
	Path
	GetRootPath() const;
	Path
	GetRelativePath() const;
	Path
	GetParentPath() const;
	Path
	GetFilename() const;
	Path
	GetStem() const;
	Path
	GetExtension() const;
	DefGetter(StringType, GeneralString, pathname) //取一般字符串。
	DefGetter(const StringType&, NativeString, pathname) //取本地格式和编码的字符串。

	PDefHead(const value_type*, c_str) //本地格式和编码的 C 风格字符串。
		ImplBodyMember(GetNativeString(), c_str)

	//修改函数。

	PDefHead(void, clear)
		ImplBodyMember(pathname, clear)
	PDefHead(void, swap, Path& rhs) ythrow()
		ImplBodyMember(pathname, swap, rhs.pathname)

	Path&
	MakeAbsolute(const Path&);
	Path&
	RemoveFilename();
	Path&
	ReplaceExtension(const Path& = Path());


//	//迭代器。
//	class iterator
//	{
//	};
//	typedef iterator const_iterator;

//	iterator begin() const;
//	iterator end() const;
};

inline
Path::Path()
: pathname()
{}
inline
Path::Path(const Path& path)
: pathname(path.pathname)
{}
template<class _tString>
inline
Path::Path(const _tString& pathstr)
: pathname(pathstr)
{}
inline
Path::~Path()
{}

inline Path&
Path::operator=(const Path& rhs)
{
	pathname = rhs.pathname;
	return *this;
}
template<class _tString>
inline Path&
Path::operator=(const _tString& rhs)
{
	pathname = rhs;
	return *this;
}


inline bool
operator==(const Path& lhs, const Path& rhs)
{
	return lhs.GetNativeString() == rhs.GetNativeString();
}
inline bool
operator!=(const Path& lhs, const Path& rhs)
{
	return !(lhs == rhs);
}
inline bool
operator<(const Path& lhs, const Path& rhs)
{
	return lhs.GetNativeString() < rhs.GetNativeString();
}
inline bool
operator<=(const Path& lhs, const Path& rhs)
{
	return !(rhs < lhs);
}
inline bool operator>(const Path& lhs, const Path& rhs)
{
	return rhs < lhs;
}
inline bool
operator>=(const Path& lhs, const Path& rhs)
{
	return !(lhs < rhs);
}

inline Path
operator/(const Path& lhs, const Path& rhs)
{
	Path temp(lhs);

	return Path(temp /= rhs);
}

inline void
swap(Path& lhs, Path& rhs)
{
	lhs.swap(rhs);
}
//bool lexicographical_compare(Path::iterator, Path::iterator,
//							 Path::iterator, Path::iterator);


//截取路径末尾的文件名。
const char*
GetFileName(CPATH);
std::string
GetFileName(const std::string&);

//截取路径中的目录名并返回字符串。
std::string
GetDirectoryName(const std::string&);

//截取路径中的目录名和文件名保存至字符串，并返回最后一个目录分隔符的位置。
std::string::size_type
SplitPath(const std::string&, std::string&, std::string&);


//截取文件名开头的主文件名（贪婪匹配）。
std::string
GetStem(const std::string&, const std::string&);

//对于两个字符串，判断前者是否是后者的主文件名。
bool
IsStem(const char*, const char*);
bool
IsStem(const std::string&, const std::string&);

//判断给定两个文件名的主文件名是否相同（忽略大小写；贪婪匹配）。
bool
HaveSameStems(const char*, const char*);
bool
HaveSameStems(const std::string&, const std::string&);

//截取文件名末尾的扩展名（非贪婪匹配）。
const char*
GetExtendName(const char*);
std::string
GetExtendName(const std::string&);

//对于两个字符串，判断前者是否是后者的扩展名。
bool
IsExtendName(const char*, const char*);
bool
IsExtendName(const std::string&, const std::string&);

//判断给定两个文件名的扩展名是否相同（忽略大小写；非贪婪匹配）。
bool
HaveSameExtendNames(const char*, const char*);
bool
HaveSameExtendNames(const std::string&, const std::string&);


//切换路径。
inline int
ChDir(CPATH path)
{
	return chdir(path);
}
int
ChDir(const std::string&);

//取当前工作目录。
std::string
GetNowDirectory();


//文件名过滤器。
typedef bool FNFILTER(const String&);
typedef FNFILTER* PFNFILTER;

struct HFileNameFilter : public GHBase<PFNFILTER>
{
	typedef GHBase<PFNFILTER> ParentType;

	HFileNameFilter(const PFNFILTER pf = NULL)
	: GHBase<PFNFILTER>(pf)
	{}

	bool
	operator()(const String& name) const
	{
		if(_ptr)
			return _ptr(name);
		return -1;
	}
};


//文件列表模块。
class MFileList
{
public:
	typedef String ItemType; //项目名称类型。
	typedef std::vector<ItemType> ListType; //项目列表类型。

protected:
	Path Directory; //目录的完整路径。
	ListType List; //目录中的项目列表。

public:
	MFileList();
	virtual
	~MFileList();

	const Path&
	GetDirectory() const; //取目录的完整路径。
	const ListType&
	GetList() const; //取项目列表。
	u32
	LoadSubItems(); //在目录中取子项目。

	u32
	ListItems(); //遍历目录中的项目，更新至列表。

	void
	GoToPath(const Path&); //导航至指定路径对应目录。
	void
	GoToSubDirectory(const std::string&); //导航至子目录。
	void
	GoToRoot(); //返回根目录。
	void
	GoToParent(); //返回上一级目录。
};

inline const Path&
MFileList::GetDirectory() const
{
	return Directory;
}
inline const MFileList::ListType&
MFileList::GetList() const
{
	return List;
}

inline void
MFileList::GoToRoot()
{
	GoToPath(FS_Root);
}

YSL_END_NAMESPACE(IO)

YSL_END

#endif

