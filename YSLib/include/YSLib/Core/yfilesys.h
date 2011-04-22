/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfilesys.h
\ingroup Core
\brief 平台无关的文件系统抽象。
\version 0.2088;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-28 00:09:28 +0800;
\par 修改时间:
	2011-04-22 21:56 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YFileSystem;
*/


#ifndef YSL_INC_CORE_YFILESYS_H_
#define YSL_INC_CORE_YFILESYS_H_

#include "ystring.h"
#include "yfunc.hpp"
#include "yshell.h" // for GHHandle<YShell> delete procedure;
#include <iterator>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(IO)

//文件系统常量：前缀 FS 表示文件系统 (File System) 。
extern const CPATH FS_Root;
extern const CPATH FS_Seperator;
extern const CPATH FS_Now;
extern const CPATH FS_Parent;


typedef char NativePathCharType; \
	//!< 本机路径字符类型，POSIX 为 char ，Windows 为 wchar_t。
typedef GSStringTemplate<NativePathCharType>::basic_string NativeStringType; \
	//!< 本地字符串类型。

//! \brief 路径类。
class Path : public u16string
{
public:
	typedef uchar_t ValueType;
	typedef GSStringTemplate<ValueType>::basic_string StringType; \
		//!< 内部字符串类型。
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
	DefPredicate(Absolute, YSLib::IsAbsolute(GetNativeString().c_str()))
	DefPredicate(Relative, !IsAbsolute())
	/*!
	\brief 判断是否有根名称。
	*/
	bool
	HasRootName() const;
	/*!
	\brief 判断是否有根目录。
	*/
	bool
	HasRootDirectory() const;
	/*!
	\brief 判断是否有根路径。
	*/
	bool
	HasRootPath() const;
	/*!
	\brief 判断是否有相对路径。
	*/
	bool
	HasRelativePath() const;
	/*!
	\brief 判断是否有父路径。
	*/
	bool
	HasParentPath() const;
	/*!
	\brief 判断是否有文件名。
	*/
	bool
	HasFilename() const;
	/*!
	\brief 判断是否有主文件名。
	*/
	bool
	HasStem() const;
	/*!
	\brief 判断是否有扩展名。
	*/
	bool
	HasExtension() const;

	//路径分解。
	/*!
	\brief 取根名称。
	*/
	Path
	GetRootName() const;
	/*!
	\brief 取根目录。
	*/
	Path
	GetRootDirectory() const;
	/*!
	\brief 取根路径。
	*/
	Path
	GetRootPath() const;
	/*!
	\brief 取相对路径。
	*/
	Path
	GetRelativePath() const;
	/*!
	\brief 取父路径。
	*/
	Path
	GetParentPath() const;
	/*!
	\brief 取文件名。
	*/
	Path
	GetFilename() const;
	/*!
	\brief 取主文件名。
	*/
	Path
	GetStemFrom() const;
	/*!
	\brief 取扩展名。
	*/
	Path
	GetExtension() const;
	DefGetter(NativeStringType, NativeString, Text::StringToMBCS(*this)) //!< 取本地格式和编码的字符串。

	//修改函数。

	/*!
	\brief 构造绝对路径。
	*/
	Path&
	MakeAbsolute(const Path&);
	/*!
	\brief 移除文件名。
	*/
	Path&
	RemoveFilename();
	/*!
	\brief 替换扩展名。
	*/
	Path&
	ReplaceExtension(const Path& = Path());

	//迭代器。
	class iterator
		: public std::iterator<std::bidirectional_iterator_tag, Path>
	{
	private:
		const value_type* ptr;
		StringType::size_type n;

		/*!
		\brief 无参数构造。
		\note 空迭代器。仅为兼容标准迭代器需求。
		*/
		iterator();

	public:
		/*!
		\brief 构造：使用值引用。
		*/
		iterator(const value_type&);
		/*!
		\brief 复制构造。
		*/
		iterator(const iterator&);

		/*!

		\brief 迭代：向后遍历。
		*/
		iterator&
		operator++();
		/*!
		\brief 迭代：向后遍历。
		\note 构造新迭代器并返回。
		*/
		iterator
		operator++(int);

		/*!

		\brief 迭代：向前遍历。
		*/
		iterator&
		operator--();
		/*!
		\brief 迭代：向前遍历。
		\note 构造新迭代器并返回。
		*/
		iterator
		operator--(int);

		/*!
		\brief 比较：相等关系。
		*/
		bool
		operator==(const iterator&) const;

		/*!
		\brief 比较：不等关系。
		*/
		bool
		operator!=(const iterator&) const;

		/*!
		\brief 间接访问。
		*/
		value_type
		operator*() const;

		DefGetter(const value_type*, Ptr, ptr)
		DefGetter(StringType::size_type, Position, n)
	};

	typedef iterator const_iterator;

	/*!
	\brief 取起始迭代器。
	*/
	iterator
	begin() const;

	/*!
	\brief 取终止迭代器。
	*/
	iterator
	end() const;
};

inline
Path::Path()
	: u16string()
{}
inline
Path::Path(const Path::ValueType* pathstr)
	: u16string(pathstr)
{}
inline
Path::Path(const NativePathCharType* pathstr)
	: u16string(Text::MBCSToString(pathstr))
{}
inline
Path::Path(const NativeStringType& pathstr)
	: u16string(Text::MBCSToString(pathstr))
{}
template<class _tString>
inline
Path::Path(const _tString& pathstr)
	: u16string(pathstr)
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
	return !GetStemFrom().empty();
}
inline bool
Path::HasExtension() const
{
	return !GetExtension().empty();
}

inline
Path::iterator::iterator()
	: ptr(NULL), n(StringType::npos)
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

/*!
\brief 交换。
*/
inline void
swap(Path& lhs, Path& rhs)
{
	lhs.swap(rhs);
}
//bool lexicographical_compare(Path::iterator, Path::iterator,
//							 Path::iterator, Path::iterator);


/*!
\brief 截取路径末尾的文件名。
*/
const char*
GetFileNameFrom(CPATH);
/*!
\brief 截取路径末尾的文件名。
*/
string
GetFileNameFrom(const string&);

/*!
\brief 截取路径中的目录名并返回字符串。
*/
string
GetDirectoryNameFrom(const string&);

/*!
\brief 截取路径中的目录名和文件名保存至字符串，并返回最后一个目录分隔符的位置。
*/
string::size_type
SplitPath(const string&, string&, string&);


/*!
\brief 截取文件名开头的主文件名。
\note 贪婪匹配。
*/
string
GetStemFrom(const string&, const string&);

/*!
\brief 对于两个字符串，判断前者是否是后者的主文件名。
*/
bool
IsStemOf(const char*, const char*);
/*!
\brief 对于两个字符串，判断前者是否是后者的主文件名。
*/
bool
IsStemOf(const string&, const string&);

/*!
\brief 判断指定两个文件名的主文件名是否相同。
\note 忽略大小写；贪婪匹配。
*/
bool
HaveSameStems(const char*, const char*);
/*!
\brief 判断指定两个文件名的主文件名是否相同。
\note 忽略大小写；贪婪匹配。
*/
bool
HaveSameStems(const string&, const string&);

/*!
\brief 截取文件名末尾的扩展名。
\note 非贪婪匹配。
*/
const char*
GetExtendNameFrom(const char*);
/*!
\brief 截取文件名末尾的扩展名。
\note 非贪婪匹配。
*/
string
GetExtendNameFrom(const string&);

/*!
\brief 对于两个字符串，判断前者是否是后者的扩展名。
*/
bool
IsExtendNameOf(const char*, const char*);
/*!
\brief 对于两个字符串，判断前者是否是后者的扩展名。
*/
bool
IsExtendNameOf(const string&, const string&);

/*!
\brief 判断指定两个文件名的扩展名是否相同。
\note 忽略大小写；非贪婪匹配。
*/
bool
HaveSameExtendNames(const char*, const char*);
/*!
\brief 判断指定两个文件名的扩展名是否相同。
\note 忽略大小写；非贪婪匹配。
*/
bool
HaveSameExtendNames(const string&, const string&);


/*!
\brief 切换路径。
*/
inline int
ChangeDirectory(CPATH path)
{
	return chdir(path);
}
/*!
\brief 切换路径。
*/
int
ChangeDirectory(const string&);

/*!
\brief 取当前工作目录。
*/
string
GetNowDirectory();

/*!
\brief 验证绝对路径有效性。
*/
bool
ValidateDirectory(const string&);
/*!
\brief 验证绝对路径有效性。
*/
inline bool
ValidateDirectory(const Path& path)
{
	return ValidateDirectory(path.GetNativeString());
}


//! \brief 文件名过滤器。
typedef bool FNFILTER(const String&);
typedef FNFILTER* PFNFILTER;

struct HFileNameFilter : public GHBase<PFNFILTER>
{
	typedef GHBase<PFNFILTER> ParentType;

	/*!
	\brief 构造：使用函数指针。
	*/
	HFileNameFilter(const PFNFILTER pf = NULL)
	: GHBase<PFNFILTER>(pf)
	{}

	/*!
	\brief 调用函数。
	*/
	bool
	operator()(const String& name) const
	{
		if(GetPtr())
			return GetPtr()(name);
		return -1;
	}
};


//! \brief 文件列表模块。
class FileList
{
public:
	typedef String ItemType; //!< 项目名称类型。
	typedef vector<ItemType> ListType; //!< 项目列表类型。

protected:
	Path Directory; //!< 目录的完整路径。
	GHandle<ListType> spList; //!< 目录中的项目列表的强指针。

public:
	/*!
	\brief 构造：使用指定路径。
	\note 参数为空或空字符串时为根目录。
	*/
	FileList(CPATH = NULL);
	/*!
	\brief 构造：使用窄字符串。
	\note 参数为空字符串时为根目录。
	*/
	FileList(const string&);
	/*!
	\brief 构造：使用指定项目。
	\note 参数为空字符串时为根目录。
	*/
	FileList(const ItemType&);
	virtual DefEmptyDtor(FileList)

	/*!
	\brief 导航至相对路径。
	*/
	bool
	operator/=(const string&);
	/*!
	\brief 导航至相对路径。
	*/
	bool
	operator/=(const String&);

	DefGetter(const Path&, Directory, Directory) //!< 取目录的完整路径。
	DefGetter(GHWeak<ListType>, ListWeakPtr, spList); //!< 取项目列表的弱指针。
//	DefGetter(const ListType&, List, List) //!< 取项目列表。

	/*!
	\brief 在目录中取子项目。
	*/
	ListType::size_type
	LoadSubItems();

	/*!
	\brief 遍历目录中的项目，更新至列表。
	*/
	ListType::size_type
	ListItems();
};

inline bool
FileList::operator/=(const String& s)
{
	return *this /= StringToMBCS(s);
}

YSL_END_NAMESPACE(IO)

YSL_END

#endif

