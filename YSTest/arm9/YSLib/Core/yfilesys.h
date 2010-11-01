// YSLib::Core::YFileSystem by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-28 00:09:28 + 08:00;
// UTime = 2010-10-31 12:21 + 08:00;
// Version = 0.1972;


#ifndef INCLUDED_YFILESYS_H_
#define INCLUDED_YFILESYS_H_

// YFileSystem ：平台无关的文件处理系统。

#include "ystring.h"
#include "yfunc.hpp"
#include "../Helper/yglobal.h"
#include "../Core/yshell.h" // for HSHL delete procedure;
#include <iterator>
//#include <vector>
//#include <list>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(IO)

//文件系统常量：前缀 FS 表示文件系统 (File System) 。
extern const CPATH FS_Root;
extern const CPATH FS_Seperator;
extern const CPATH FS_Now;
extern const CPATH FS_Parent;


typedef char NativePathCharType; \
	//本机路径字符类型，POSIX 为 char ，Windows 为 wchar_t。
typedef GSStringTemplate<NativePathCharType>::basic_string NativeStringType; \
	//本地字符串类型。

//路径类。
class Path : public ustring
{
public:
	typedef uchar_t ValueType;
	typedef GSStringTemplate<ValueType>::basic_string StringType; \
		//内部字符串类型。
//	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

	static const ValueType Slash;
	static const Path Now;
	static const Path Parent;

public:
	//编码转换。
//	static std::locale imbue(const std::locale&);
//	static const codecvt_type& codecvt();

	//构造函数和析构函数。
	Path();
	Path(const ValueType*);
	Path(const NativePathCharType*);
	Path(const NativeStringType&);
	template<class _tString>
	Path(const _tString&);
	~Path();

	//追加路径。
	Path&
	operator/=(const Path&);

	//查询。
	DefPredicate(Absolute, platform::IsAbsolute(GetNativeString().c_str()))
	DefPredicate(Relative, !IsAbsolute())
	//********************************
	//名称:		HasRootName
	//全名:		YSLib::IO::Path::HasRootName
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否有根名称。
	//备注:		
	//********************************
	bool
	HasRootName() const;
	//********************************
	//名称:		HasRootDirectory
	//全名:		YSLib::IO::Path::HasRootDirectory
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否有根目录。
	//备注:		
	//********************************
	bool
	HasRootDirectory() const;
	//********************************
	//名称:		HasRootPath
	//全名:		YSLib::IO::Path::HasRootPath
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否有根路径。
	//备注:		
	//********************************
	bool
	HasRootPath() const;
	//********************************
	//名称:		HasRelativePath
	//全名:		YSLib::IO::Path::HasRelativePath
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否有相对路径。
	//备注:		
	//********************************
	bool
	HasRelativePath() const;
	//********************************
	//名称:		HasParentPath
	//全名:		YSLib::IO::Path::HasParentPath
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否有父路径。
	//备注:		
	//********************************
	bool
	HasParentPath() const;
	//********************************
	//名称:		HasFilename
	//全名:		YSLib::IO::Path::HasFilename
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否有文件名。
	//备注:		
	//********************************
	bool
	HasFilename() const;
	//********************************
	//名称:		HasStem
	//全名:		YSLib::IO::Path::HasStem
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否有主文件名。
	//备注:		
	//********************************
	bool
	HasStem() const;
	//********************************
	//名称:		HasExtension
	//全名:		YSLib::IO::Path::HasExtension
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否有扩展名。
	//备注:		
	//********************************
	bool
	HasExtension() const;

	//路径分解。
	//********************************
	//名称:		GetRootName
	//全名:		YSLib::IO::Path::GetRootName
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path
	//修饰符:	const
	//功能概要:	取根名称。
	//备注:		
	//********************************
	Path
	GetRootName() const;
	//********************************
	//名称:		GetRootDirectory
	//全名:		YSLib::IO::Path::GetRootDirectory
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path
	//修饰符:	const
	//功能概要:	取根目录。
	//备注:		
	//********************************
	Path
	GetRootDirectory() const;
	//********************************
	//名称:		GetRootPath
	//全名:		YSLib::IO::Path::GetRootPath
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path
	//修饰符:	const
	//功能概要:	取根路径。
	//备注:		
	//********************************
	Path
	GetRootPath() const;
	//********************************
	//名称:		GetRelativePath
	//全名:		YSLib::IO::Path::GetRelativePath
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path
	//修饰符:	const
	//功能概要:	取相对路径。
	//备注:		
	//********************************
	Path
	GetRelativePath() const;
	//********************************
	//名称:		GetParentPath
	//全名:		YSLib::IO::Path::GetParentPath
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path
	//修饰符:	const
	//功能概要:	取父路径。
	//备注:		
	//********************************
	Path
	GetParentPath() const;
	//********************************
	//名称:		GetFilename
	//全名:		YSLib::IO::Path::GetFilename
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path
	//修饰符:	const
	//功能概要:	取文件名。
	//备注:		
	//********************************
	Path
	GetFilename() const;
	//********************************
	//名称:		GetStem
	//全名:		YSLib::IO::Path::GetStem
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path
	//修饰符:	const
	//功能概要:	取主文件名。
	//备注:		
	//********************************
	Path
	GetStem() const;
	//********************************
	//名称:		GetExtension
	//全名:		YSLib::IO::Path::GetExtension
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path
	//修饰符:	const
	//功能概要:	取扩展名。
	//备注:		
	//********************************
	Path
	GetExtension() const;
	DefGetter(NativeStringType, NativeString, Text::StringToMBCS(*this)) //取本地格式和编码的字符串。

	//修改函数。

	//********************************
	//名称:		MakeAbsolute
	//全名:		YSLib::IO::Path::MakeAbsolute
	//可访问性:	public 
	//返回类型:	Path&
	//修饰符:	
	//形式参数:	const Path &
	//功能概要:	构造绝对路径。
	//备注:		
	//********************************
	Path&
	MakeAbsolute(const Path&);
	//********************************
	//名称:		RemoveFilename
	//全名:		YSLib::IO::Path::RemoveFilename
	//可访问性:	public 
	//返回类型:	Path&
	//修饰符:	
	//功能概要:	移除文件名。
	//备注:		
	//********************************
	Path&
	RemoveFilename();
	//********************************
	//名称:		ReplaceExtension
	//全名:		YSLib::IO::Path::ReplaceExtension
	//可访问性:	public 
	//返回类型:	Path&
	//修饰符:	
	//形式参数:	const Path &
	//功能概要:	替换扩展名。
	//备注:		
	//********************************
	Path&
	ReplaceExtension(const Path& = Path());

	//迭代器。
	class iterator
		: public std::iterator<std::bidirectional_iterator_tag, Path>
	{
	private:
		const value_type* ptr;
		StringType::size_type n;

		//********************************
		//名称:		iterator
		//全名:		YSLib::IO::Path::iterator::iterator
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//功能概要:	无参数构造。
		//备注:		空迭代器。仅为兼容标准迭代器需求。
		//********************************
		iterator();

	public:
		//********************************
		//名称:		iterator
		//全名:		YSLib::IO::Path::iterator::iterator
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//形式参数:	const value_type &
		//功能概要:	构造：使用值引用。
		//备注:		
		//********************************
		iterator(const value_type&);
		//********************************
		//名称:		iterator
		//全名:		YSLib::IO::Path::iterator::iterator
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//形式参数:	const iterator &
		//功能概要:	复制构造。
		//备注:		
		//********************************
		iterator(const iterator&);

		//********************************
		//名称:		operator++
		//全名:		YSLib::IO::Path::iterator::operator++
		//可访问性:	public 
		//返回类型:	iterator&
		//修饰符:	
		//功能概要:	迭代：向后遍历。
		//备注:		
		//********************************
		iterator&
		operator++();
		//********************************
		//名称:		operator++
		//全名:		YSLib::IO::Path::iterator::operator++
		//可访问性:	public 
		//返回类型:	YSLib::IO::Path::iterator
		//修饰符:	
		//形式参数:	int
		//功能概要:	迭代：向后遍历。
		//备注:		构造新迭代器并返回。
		//********************************
		iterator
		operator++(int);

		//********************************
		//名称:		operator--
		//全名:		YSLib::IO::Path::iterator::operator--
		//可访问性:	public 
		//返回类型:	iterator&
		//修饰符:	
		//功能概要:	迭代：向前遍历。
		//备注:		
		//********************************
		iterator&
		operator--();
		//********************************
		//名称:		operator--
		//全名:		YSLib::IO::Path::iterator::operator--
		//可访问性:	public 
		//返回类型:	YSLib::IO::Path::iterator
		//修饰符:	
		//形式参数:	int
		//功能概要:	迭代：向前遍历。
		//备注:		构造新迭代器并返回。
		//********************************
		iterator
		operator--(int);

		//********************************
		//名称:		operator==
		//全名:		YSLib::IO::Path::iterator::operator==
		//可访问性:	public 
		//返回类型:	bool
		//修饰符:	const
		//形式参数:	const iterator &
		//功能概要:	比较：相等关系。
		//备注:		
		//********************************
		bool
		operator==(const iterator&) const;

		//********************************
		//名称:		operator!=
		//全名:		YSLib::IO::Path::iterator::operator!=
		//可访问性:	public 
		//返回类型:	bool
		//修饰符:	const
		//形式参数:	const iterator &
		//功能概要:	比较：不等关系。
		//备注:		
		//********************************
		bool
		operator!=(const iterator&) const;

		//********************************
		//名称:		operator*
		//全名:		YSLib::IO::Path::iterator::operator*
		//可访问性:	public 
		//返回类型:	value_type
		//修饰符:	const
		//功能概要:	间接访问。
		//备注:		
		//********************************
		value_type
		operator*() const;

		DefGetter(const value_type*, Ptr, ptr)
		DefGetter(StringType::size_type, Position, n)
	};

	typedef iterator const_iterator;

	//********************************
	//名称:		begin
	//全名:		YSLib::IO::Path::begin
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path::iterator
	//修饰符:	const
	//功能概要:	取起始迭代器。
	//备注:		
	//********************************
	iterator
	begin() const;

	//********************************
	//名称:		end
	//全名:		YSLib::IO::Path::end
	//可访问性:	public 
	//返回类型:	YSLib::IO::Path::iterator
	//修饰符:	const
	//功能概要:	取终止迭代器。
	//备注:		
	//********************************
	iterator
	end() const;
};

inline
Path::Path()
	: ustring()
{}
inline
Path::Path(const Path::ValueType* pathstr)
	: ustring(pathstr)
{}
inline
Path::Path(const NativePathCharType* pathstr)
	: ustring(Text::MBCSToString(pathstr))
{}
inline
Path::Path(const NativeStringType& pathstr)
	: ustring(Text::MBCSToString(pathstr))
{}
template<class _tString>
inline
Path::Path(const _tString& pathstr)
	: ustring(pathstr)
{}
inline
Path::~Path()
{}

inline bool
Path::HasRootName() const
{
	return !GetRootName().empty();
}
inline bool
Path::HasRootDirectory() const
{
	return !GetRootDirectory().empty();
}
inline bool
Path::HasRootPath() const
{
	return !GetRootPath().empty();
}
inline bool
Path::HasRelativePath() const
{
	return !GetRelativePath().empty();
}
inline bool
Path::HasParentPath() const
{
	return !GetParentPath().empty();
}
inline bool
Path::HasFilename() const
{
	return !GetFilename().empty();
}
inline bool
Path::HasStem() const
{
	return !GetStem().empty();
}
inline bool
Path::HasExtension() const
{
	return !GetExtension().empty();
}

inline
Path::iterator::iterator()
{}
inline
Path::iterator::iterator(const value_type& p)
	: ptr(&p), n(StringType::npos)
{}
inline
Path::iterator::iterator(const iterator& i)
	: ptr(i.ptr), n(i.n)
{}

inline Path::iterator
Path::iterator::operator++(int)
{
	return ++iterator(*this);
}

inline Path::iterator
Path::iterator::operator--(int)
{
	return --iterator(*this);
}

inline bool
Path::iterator::operator==(const iterator& rhs) const
{
	return ptr == rhs.ptr && n == rhs.n;
}

inline bool
Path::iterator::operator!=(const iterator& rhs) const
{
	return !(*this == rhs);
}

inline Path::iterator
Path::begin() const
{
	return ++iterator(*this);
}

inline Path::iterator
Path::end() const
{
	return iterator(*this);
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
	return Path(lhs) /= rhs;
}

//********************************
//名称:		swap
//全名:		YSLib::IO::swap
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	Path & lhs
//形式参数:	Path & rhs
//功能概要:	交换。
//备注:		
//********************************
inline void
swap(Path& lhs, Path& rhs)
{
	lhs.swap(rhs);
}
//bool lexicographical_compare(Path::iterator, Path::iterator,
//							 Path::iterator, Path::iterator);


//********************************
//名称:		GetFileName
//全名:		YSLib::IO::GetFileName
//可访问性:	public 
//返回类型:	const char*
//修饰符:	
//形式参数:	CPATH
//功能概要:	截取路径末尾的文件名。
//备注:		
//********************************
const char*
GetFileName(CPATH);
//********************************
//名称:		GetFileName
//全名:		YSLib::IO::GetFileName
//可访问性:	public 
//返回类型:	YSLib::string
//修饰符:	
//形式参数:	const string &
//功能概要:	截取路径末尾的文件名。
//备注:		
//********************************
string
GetFileName(const string&);

//********************************
//名称:		GetDirectoryName
//全名:		YSLib::IO::GetDirectoryName
//可访问性:	public 
//返回类型:	YSLib::string
//修饰符:	
//形式参数:	const string &
//功能概要:	截取路径中的目录名并返回字符串。
//备注:		
//********************************
string
GetDirectoryName(const string&);

//********************************
//名称:		SplitPath
//全名:		YSLib::IO::SplitPath
//可访问性:	public 
//返回类型:	string::size_type
//修饰符:	
//形式参数:	const string &
//形式参数:	string &
//形式参数:	string &
//功能概要:	截取路径中的目录名和文件名保存至字符串，并返回最后一个目录分隔符的位置。
//备注:		
//********************************
string::size_type
SplitPath(const string&, string&, string&);


//********************************
//名称:		GetStem
//全名:		YSLib::IO::GetStem
//可访问性:	public 
//返回类型:	YSLib::string
//修饰符:	
//形式参数:	const string &
//形式参数:	const string &
//功能概要:	截取文件名开头的主文件名。
//备注:		贪婪匹配。
//********************************
string
GetStem(const string&, const string&);

//********************************
//名称:		IsStem
//全名:		YSLib::IO::IsStem
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const char *
//形式参数:	const char *
//功能概要:	对于两个字符串，判断前者是否是后者的主文件名。
//备注:		
//********************************
bool
IsStem(const char*, const char*);
//********************************
//名称:		IsStem
//全名:		YSLib::IO::IsStem
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const string &
//形式参数:	const string &
//功能概要:	对于两个字符串，判断前者是否是后者的主文件名。
//备注:		
//********************************
bool
IsStem(const string&, const string&);

//********************************
//名称:		HaveSameStems
//全名:		YSLib::IO::HaveSameStems
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const char *
//形式参数:	const char *
//功能概要:	判断指定两个文件名的主文件名是否相同。
//备注:		忽略大小写；贪婪匹配。
//********************************
bool
HaveSameStems(const char*, const char*);
//********************************
//名称:		HaveSameStems
//全名:		YSLib::IO::HaveSameStems
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const string &
//形式参数:	const string &
//功能概要:	判断指定两个文件名的主文件名是否相同。
//备注:		忽略大小写；贪婪匹配。
//********************************
bool
HaveSameStems(const string&, const string&);

//********************************
//名称:		GetExtendName
//全名:		YSLib::IO::GetExtendName
//可访问性:	public 
//返回类型:	const char*
//修饰符:	
//形式参数:	const char *
//功能概要:	截取文件名末尾的扩展名。
//备注:		非贪婪匹配。
//********************************
const char*
GetExtendName(const char*);
//********************************
//名称:		GetExtendName
//全名:		YSLib::IO::GetExtendName
//可访问性:	public 
//返回类型:	YSLib::string
//修饰符:	
//形式参数:	const string &
//功能概要:	截取文件名末尾的扩展名。
//备注:		非贪婪匹配。
//********************************
string
GetExtendName(const string&);

//********************************
//名称:		IsExtendName
//全名:		YSLib::IO::IsExtendName
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const char *
//形式参数:	const char *
//功能概要:	对于两个字符串，判断前者是否是后者的扩展名。
//备注:		
//********************************
bool
IsExtendName(const char*, const char*);
//********************************
//名称:		IsExtendName
//全名:		YSLib::IO::IsExtendName
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const string &
//形式参数:	const string &
//功能概要:	对于两个字符串，判断前者是否是后者的扩展名。
//备注:		
//********************************
bool
IsExtendName(const string&, const string&);

//********************************
//名称:		HaveSameExtendNames
//全名:		YSLib::IO::HaveSameExtendNames
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const char *
//形式参数:	const char *
//功能概要:	判断指定两个文件名的扩展名是否相同。
//备注:		忽略大小写；非贪婪匹配。
//********************************
bool
HaveSameExtendNames(const char*, const char*);
//********************************
//名称:		HaveSameExtendNames
//全名:		YSLib::IO::HaveSameExtendNames
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const string &
//形式参数:	const string &
//功能概要:	判断指定两个文件名的扩展名是否相同。
//备注:		忽略大小写；非贪婪匹配。
//********************************
bool
HaveSameExtendNames(const string&, const string&);


//********************************
//名称:		ChDir
//全名:		YSLib::IO::ChDir
//可访问性:	public 
//返回类型:	int
//修饰符:	
//形式参数:	CPATH path
//功能概要:	切换路径。
//备注:		
//********************************
inline int
ChDir(CPATH path)
{
	return platform::chdir(path);
}
//********************************
//名称:		ChDir
//全名:		YSLib::IO::ChDir
//可访问性:	public 
//返回类型:	int
//修饰符:	
//形式参数:	const string &
//功能概要:	切换路径。
//备注:		
//********************************
int
ChDir(const string&);

//********************************
//名称:		GetNowDirectory
//全名:		YSLib::IO::GetNowDirectory
//可访问性:	public 
//返回类型:	YSLib::string
//修饰符:	
//功能概要:	取当前工作目录。
//备注:		
//********************************
string
GetNowDirectory();

//********************************
//名称:		ValidateDirectory
//全名:		YSLib::IO::ValidateDirectory
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const string &
//功能概要:	验证绝对路径有效性。
//备注:		
//********************************
bool
ValidateDirectory(const string&);
//********************************
//名称:		ValidateDirectory
//全名:		YSLib::IO::ValidateDirectory
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Path & path
//功能概要:	验证绝对路径有效性。
//备注:		
//********************************
inline bool
ValidateDirectory(const Path& path)
{
	return ValidateDirectory(path.GetNativeString());
}


//文件名过滤器。
typedef bool FNFILTER(const String&);
typedef FNFILTER* PFNFILTER;

struct HFileNameFilter : public GHBase<PFNFILTER>
{
	typedef GHBase<PFNFILTER> ParentType;

	//********************************
	//名称:		HFileNameFilter
	//全名:		YSLib::IO::HFileNameFilter::HFileNameFilter
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const PFNFILTER pf
	//功能概要:	构造：使用函数指针。
	//备注:		
	//********************************
	HFileNameFilter(const PFNFILTER pf = NULL)
	: GHBase<PFNFILTER>(pf)
	{}

	//********************************
	//名称:		operator()
	//全名:		YSLib::IO::HFileNameFilter::operator()
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const String & name
	//功能概要:	调用函数。
	//备注:		
	//********************************
	bool
	operator()(const String& name) const
	{
		if(GetPtr())
			return GetPtr()(name);
		return -1;
	}
};


//文件列表模块。
class MFileList
{
public:
	typedef String ItemType; //项目名称类型。
	typedef vector<ItemType> ListType; //项目列表类型。

protected:
	Path Directory; //目录的完整路径。
	ListType List; //目录中的项目列表。

public:
	//********************************
	//名称:		MFileList
	//全名:		YSLib::IO::MFileList::MFileList
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	构造：使用指定路径。
	//备注:		参数为空时为根目录。
	//********************************
	MFileList(CPATH = NULL);
	virtual DefEmptyDtor(MFileList)

	//********************************
	//名称:		operator/=
	//全名:		YSLib::IO::MFileList::operator/=
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const string &
	//功能概要:	导航至相对路径。
	//备注:		
	//********************************
	bool
	operator/=(const string&);
	//********************************
	//名称:		operator/=
	//全名:		YSLib::IO::MFileList::operator/=
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const String &
	//功能概要:	导航至相对路径。
	//备注:		
	//********************************
	bool
	operator/=(const String&);

	DefGetter(const Path&, Directory, Directory) //取目录的完整路径。
	DefGetter(const ListType&, List, List) //取项目列表。

	//********************************
	//名称:		LoadSubItems
	//全名:		YSLib::IO::MFileList::LoadSubItems
	//可访问性:	public 
	//返回类型:	ListType::size_type
	//修饰符:	
	//功能概要:	在目录中取子项目。
	//备注:		
	//********************************
	ListType::size_type
	LoadSubItems();

	//********************************
	//名称:		LoadSubItems
	//全名:		YSLib::IO::MFileList::ListItems
	//可访问性:	public 
	//返回类型:	ListType::size_type
	//修饰符:	
	//功能概要:	遍历目录中的项目，更新至列表。
	//备注:		
	//********************************
	ListType::size_type
	ListItems();
};

inline bool
MFileList::operator/=(const String& s)
{
	return *this /= StringToMBCS(s);
}

YSL_END_NAMESPACE(IO)

YSL_END

#endif

