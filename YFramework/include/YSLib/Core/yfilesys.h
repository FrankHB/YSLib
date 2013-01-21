/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfilesys.h
\ingroup Core
\brief 平台无关的文件系统抽象。
\version r1401
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:09:28 +0800
\par 修改时间:
	2013-01-21 02:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YFileSystem
*/


#ifndef YSL_INC_CORE_YFILESYS_H_
#define YSL_INC_CORE_YFILESYS_H_ 1

#include "ystring.h"
#include <iterator>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(IO)

const auto CP_Path(Text::CS_Default); //!< 路径编码。

/*!
\brief 文件系统常量：前缀 FS 表示文件系统 (File System) 。
\since build 285
*/
//@{
yconstexpr const_path_t FS_Root(YCL_PATH_ROOT);
yconstexpr const_path_t FS_Seperator(YCL_PATH_SEPERATOR);
yconstexpr const_path_t FS_Now(".");
yconstexpr const_path_t FS_Parent("..");
yconstexpr const ucs2_t* FS_Now_X(u".");
yconstexpr const ucs2_t* FS_Parent_X(u"..");
//@}


/*!
\brief 本机字符串。
\since build 285
*/
typedef GSStringTemplate<NativePathCharType>::basic_string NativeString;


/*!
\brief 路径。
\warning 非虚析构。
*/
class YF_API Path : public String
{
public:
	typedef GSStringTemplate<ucs2_t>::basic_string StringType; \
		//!< 内部字符串类型。
//	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

	static yconstexpr ucs2_t Slash = YCL_PATH_DELIMITER;
	static const Path Now;
	static const Path Parent;

public:
	//迭代器。
	class iterator : public std::iterator<std::bidirectional_iterator_tag, Path>
	{
	private:
		const value_type* ptr;
		StringType::size_type n;

		/*!
		\brief 无参数构造。
		\note 空迭代器。仅为兼容标准迭代器需求。
		*/
		iterator()
			: ptr(), n(StringType::npos)
		{}

	public:
		/*!
		\brief 构造：使用值引用。
		*/
		iterator(const value_type& p)
			: ptr(&p), n(StringType::npos)
		{}
		/*!
		\brief 复制构造。
		*/
		iterator(const iterator& i)
			: ptr(i.ptr), n(i.n)
		{}

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
		operator++(int)
		{
			return ++iterator(*this);
		}

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
		operator--(int)
		{
			return --iterator(*this);
		}

		/*!
		\brief 比较：相等关系。
		*/
		bool
		operator==(const iterator& i) const
		{
			return ptr == i.ptr && n == i.n;
		}

		/*!
		\brief 比较：不等关系。
		*/
		bool
		operator!=(const iterator& i) const
		{
			return !(*this == i);
		}

		/*!
		\brief 间接访问。
		*/
		value_type
		operator*() const;

		DefGetter(const ynothrow, const value_type*, Ptr, ptr)
		DefGetter(const ynothrow, StringType::size_type, Position, n)
	};

	typedef iterator const_iterator;

	//编码转换。
//	static std::locale imbue(const std::locale&);
//	static const codecvt_type& codecvt();

	/*!
	\brief 无参数构造：默认实现。
	*/
	inline DefDeCtor(Path)
	/*!
	\brief 复制构造：默认实现。
	*/
	inline DefDeCopyCtor(Path)
	/*!
	\brief 转移构造：默认实现。
	*/
	inline DefDeMoveCtor(Path)
	Path(const ucs2_t* pathstr)
		: String(pathstr)
	{}
	Path(const NativePathCharType* pathstr)
		: String(pathstr, CP_Path)
	{}
	Path(const NativeString& pathstr)
		: String(pathstr, CP_Path)
	{}
	template<class _tString>
	Path(const _tString& pathstr)
		: String(pathstr)
	{}
	inline DefDeDtor(Path)

	/*!
	\brief 复制赋值：默认实现。
	\since build 295
	*/
	inline DefDeCopyAssignment(Path)
	/*!
	\brief 转移赋值：默认实现。
	\since build 295
	*/
	inline DefDeMoveAssignment(Path)

	//追加路径。
	Path&
	operator/=(const Path&);

	//查询。
	DefPred(const ynothrow, Absolute,
		YSLib::IsAbsolute(GetNativeString().c_str()))
	DefPred(const ynothrow, Relative, !IsAbsolute())
	/*!
	\brief 判断是否表示目录。
	\note 无视结尾分隔符。
	\since build 298
	*/
	bool
	IsDirectory() const;
	/*!
	\brief 判断是否有根名称。
	*/
	bool
	HasRootName() const
	{
		return !GetRootName().empty();
	}
	/*!
	\brief 判断是否有根目录。
	*/
	bool
	HasRootDirectory() const
	{
		return !GetRootDirectory().empty();
	}
	/*!
	\brief 判断是否有根路径。
	*/
	bool
	HasRootPath() const
	{
		return !GetRootPath().empty();
	}
	/*!
	\brief 判断是否有相对路径。
	*/
	bool
	HasRelativePath() const
	{
		return !GetRelativePath().empty();
	}
	/*!
	\brief 判断是否有父路径。
	*/
	bool
	HasParentPath() const
	{
		return !GetParentPath().empty();
	}
	/*!
	\brief 判断是否有文件名。
	*/
	bool
	HasFilename() const
	{
		return !GetFilename().empty();
	}
	/*!
	\brief 判断是否有主文件名。
	*/
	bool
	HasStem() const
	{
		return !GetStem().empty();
	}
	/*!
	\brief 判断是否有扩展名。
	*/
	bool
	HasExtension() const
	{
		return !GetExtension().empty();
	}

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
	GetStem() const;
	/*!
	\brief 取扩展名。
	*/
	Path
	GetExtension() const;
	DefGetter(const ynothrow, NativeString, NativeString,
		GetMBCS(CP_Path)) //!< 取本地格式和编码的字符串。

	//取迭代器。
	/*!
	\brief 取起始迭代器。
	*/
	iterator
	begin() const
	{
		return ++iterator(*this);
	}

	/*!
	\brief 取终止迭代器。
	*/
	iterator
	end() const
	{
		return iterator(*this);
	}

	//修改函数。

	/*!
	\brief 构造绝对路径。
	*/
	Path&
	MakeAbsolute(const Path&);
	/*!
	\brief 正规化结尾分隔符：根据路径表示的实体修正结尾 Slash 。
	\return 原路径是否改变。
	\note 忽略空路径或仅由一个分隔符组成的路径。
	\since build 298

	当路径表示目录时保证以 Slash 结尾，否则若存在结尾的 Slash 则删除。
	*/
	bool
	NormalizeTrailingSlash();
	/*!
	\brief 移除文件名。
	*/
	Path&
	RemoveFilename();
	/*!
	\brief 替换扩展名。
	*/
	Path&
	ReplaceExtension(const Path& = {});
};

inline bool
operator==(const Path& x, const Path& y)
{
	return x.GetNativeString() == y.GetNativeString();
}
inline bool
operator!=(const Path& x, const Path& y)
{
	return !(x == y);
}
inline bool
operator<(const Path& x, const Path& y)
{
	return x.GetNativeString() < y.GetNativeString();
}
inline bool
operator<=(const Path& x, const Path& y)
{
	return !(y < x);
}
inline bool operator>(const Path& x, const Path& y)
{
	return y < x;
}
inline bool
operator>=(const Path& x, const Path& y)
{
	return !(x < y);
}

inline Path
operator/(const Path& x, const Path& y)
{
	return Path(x) /= y;
}

/*!
\brief 交换。
*/
inline void
swap(Path& x, Path& y)
{
	x.swap(y);
}

#if 0
bool lexicographical_compare(Path::iterator, Path::iterator,
	Path::iterator, Path::iterator);
#endif


/*!
\brief 截取路径末尾的文件名。
*/
YF_API const char*
GetFileNameOf(const_path_t);
/*!
\brief 截取路径末尾的文件名。
*/
YF_API string
GetFileNameOf(const string&);

/*!
\brief 截取路径中的目录名并返回字符串。
*/
YF_API string
GetDirectoryNameOf(const string&);

/*!
\brief 截取路径中的目录名和文件名保存至字符串，并返回最后一个目录分隔符的位置。
*/
YF_API string::size_type
SplitPath(const string&, string&, string&);


/*!
\brief 截取文件名开头的主文件名。
\note 贪婪匹配。
*/
YF_API string
GetStemOf(const string&);

/*!
\brief 对于两个字符串，判断前者是否是后者的主文件名。
*/
YF_API bool
IsStemOf(const char*, const char*);
/*!
\brief 对于两个字符串，判断前者是否是后者的主文件名。
*/
YF_API bool
IsStemOf(const string&, const string&);

/*!
\brief 判断指定两个文件名的主文件名是否相同。
\note 忽略大小写；贪婪匹配。
*/
YF_API bool
HaveSameStems(const char*, const char*);
/*!
\brief 判断指定两个文件名的主文件名是否相同。
\note 忽略大小写；贪婪匹配。
*/
YF_API bool
HaveSameStems(const string&, const string&);

/*!
\brief 截取文件名末尾的扩展名。
\note 非贪婪匹配。
*/
YF_API const char*
GetExtensionOf(const char*);
/*!
\brief 截取文件名末尾的扩展名。
\note 非贪婪匹配。
*/
YF_API string
GetExtensionOf(const string&);

/*!
\brief 对于两个字符串，判断前者是否是后者的扩展名。
*/
YF_API bool
IsExtensionOf(const char*, const char*);
/*!
\brief 对于两个字符串，判断前者是否是后者的扩展名。
*/
YF_API bool
IsExtensionOf(const string&, const string&);

/*!
\brief 判断指定两个文件名的扩展名是否相同。
\note 忽略大小写；非贪婪匹配。
*/
YF_API bool
HaveSameExtensions(const char*, const char*);
/*!
\brief 判断指定两个文件名的扩展名是否相同。
\note 忽略大小写；非贪婪匹配。
*/
YF_API bool
HaveSameExtensions(const string&, const string&);


/*!
\brief 切换路径。
*/
inline int
ChangeDirectory(const_path_t path)
{
	return uchdir(path);
}
/*!
\brief 切换路径。
*/
YF_API int
ChangeDirectory(const string&);

/*!
\brief 取当前工作目录。
\note 不含结尾分隔符。
\since build 304
*/
YF_API String
GetNowDirectory();

/*!
\brief 验证绝对路径有效性。
\since build 298
*/
YF_API bool
ValidatePath(const string&);
/*!
\brief 验证绝对路径有效性。
\since build 298
*/
inline bool
ValidatePath(const Path& path)
{
	return ValidatePath(path.GetNativeString());
}


// \brief 文件名过滤器。
// TODO: Definition & impl.


//! \brief 文件列表模块。
class YF_API FileList
{
public:
	typedef String ItemType; //!< 项目名称类型。
	typedef vector<ItemType> ListType; //!< 项目列表类型。

protected:
	Path Directory; //!< 目录的完整路径。
	shared_ptr<ListType> hList; //!< 目录中的项目列表句柄。

public:
	/*!
	\brief 构造：使用指定路径。
	\note 参数为空或空字符串时为根目录。
	*/
	FileList(const_path_t = {});
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
	\brief 导航至绝对路径。
	\note 若成功同时读取列表。
	\since build 298
	*/
	bool
	operator=(const Path&);
	/*!
	\brief 导航至相对路径。
	\note 若成功同时读取列表。
	\since build 298
	*/
	bool
	operator/=(const Path&);

	DefGetter(const ynothrow, const Path&, Directory, Directory) \
		//!< 取目录的完整路径。
	DefGetter(const ynothrow, shared_ptr<ListType>, ListPtr, hList) \
		//!< 取项目列表句柄。
//	DefGetter(const ynothrow, const ListType&, List, List) //!< 取项目列表。

	/*!
	\brief 遍历目录中的项目，更新至列表。
	*/
	ListType::size_type
	ListItems();
};

YSL_END_NAMESPACE(IO)

YSL_END

#endif

