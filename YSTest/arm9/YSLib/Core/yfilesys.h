// YSLib::Core::YFileSystem by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-28 0:09:28;
// UTime = 2010-7-26 5:54;
// Version = 0.1399;


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

typedef MString YPath; //文件路径类型。

//截取路径末尾的文件名。
const char*
GetFileName(CPATH);
MString
GetFileName(const YPath&);

//截取路径中的目录名并返回字符串。
MString
GetDirectoryName(const YPath&);

//截取路径中的目录名和文件名保存至字符串，并返回最后一个目录分隔符的位置。
YPath::size_type
SplitPath(const YPath&, YPath&, MString&);


//截取文件名开头的主文件名（贪婪匹配）。
MString
GetBaseName(const MString&, const MString&);

//对于两个字符串，判断前者是否是后者的主文件名。
bool
IsBaseName(const char*, const char*);
bool
IsBaseName(const MString&, const MString&);

//判断给定两个文件名的主文件名是否相同（忽略大小写；贪婪匹配）。
bool
SameBaseNames(const char*, const char*);
bool
SameBaseNames(const MString&, const MString&);

//截取文件名末尾的扩展名（非贪婪匹配）。
const char*
GetExtendName(const char*);
MString
GetExtendName(const MString&);

//对于两个字符串，判断前者是否是后者的扩展名。
bool
IsExtendName(const char*, const char*);
bool
IsExtendName(const MString&, const MString&);

//判断给定两个文件名的扩展名是否相同（忽略大小写；非贪婪匹配）。
bool
SameExtendNames(const char*, const char*);
bool
SameExtendNames(const MString&, const MString&);


//切换路径。
inline int
ChDir(CPATH path)
{
	return chdir(path);
}
int
ChDir(const YPath&);

//取当前工作目录。
YPath
GetNowDirectory();


//文件名过滤器。
typedef bool FNFILTER(const MString&);
typedef FNFILTER* PFNFILTER;

struct HFileNameFilter : public GHBase<PFNFILTER>
{
	typedef GHBase<PFNFILTER> ParentType;

	HFileNameFilter(const PFNFILTER pf = NULL)
	: GHBase<PFNFILTER>(pf)
	{}

	bool
	operator()(const MString& name) const
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
	typedef MString ItemType; //项目名称类型。
	typedef std::vector<ItemType> ListType; //项目列表类型。

protected:
	YPath Directory; //目录的完整路径。
	ListType List; //目录中的项目列表。

public:
	MFileList();
	virtual
	~MFileList();

	const YPath&
	GetDirectory() const; //取目录的完整路径。
	const ListType&
	GetList() const; //取项目列表。
	u32
	LoadSubItems(); //在目录中取子项目。

	u32
	ListItems(); //遍历目录中的项目，更新至列表。

	void
	GoToPath(const YPath&); //导航至指定路径对应目录。
	void
	GoToSubDirectory(const MString&); //导航至子目录。
	void
	GoToRoot(); //返回根目录。
	void
	GoToParent(); //返回上一级目录。
};

inline const YPath&
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

