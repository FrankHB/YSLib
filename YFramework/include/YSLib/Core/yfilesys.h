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
\version r1880
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:09:28 +0800
\par 修改时间:
	2013-05-31 21:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YFileSystem
*/


#ifndef YSL_INC_Core_yfilesys_h_
#define YSL_INC_Core_yfilesys_h_ 1

#include "ystring.h"
#include <ystdex/path.hpp>

//! \since build 409
namespace ystdex
{

//! \since build 409
template<>
class file_path_norm<YSLib::String> : public path_norm<YSLib::String>
{
public:
	typedef YSLib::String value_type;

	PDefH(bool, is_delimiter, const value_type& str) override
		ImplRet(YCL_FS_CharIsDelimiter(str[0], u))

	PDefH(bool, is_parent, const value_type& str) ynothrow override
		ImplRet(YCL_FS_StringIsParent(str, u))

	PDefH(bool, is_root, const value_type& str) ynothrow override
		ImplRet(YCL_FS_StringIsRoot(str, u))

	PDefH(bool, is_self, const value_type& str) ynothrow override
		ImplRet(YCL_FS_StringIsCurrent(str, u))

	DefClone(override, file_path_norm)
};

}

YSL_BEGIN

YSL_BEGIN_NAMESPACE(IO)

//! \since build 409
typedef ystdex::path<vector<String>> ypath;

/*!
\brief 文件系统常量：前缀 FS 表示文件系统 (File System) 。
\since build 285
*/
//@{
yconstexpr const_path_t FS_Root(YCL_PATH_ROOT);
//! \since build 402
yconstexpr const_path_t FS_Separator(YCL_PATH_SEPARATOR);
yconstexpr const_path_t FS_Now(".");
yconstexpr const_path_t FS_Parent("..");
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
class YF_API Path : private ypath
{
public:
	using ypath::iterator;
	using ypath::const_iterator;
	//! \since build 409
	using ypath::value_type;

public:
	/*!
	\brief 无参数构造：默认实现。
	*/
	inline DefDeCtor(Path)
	Path(const ucs2_t* str)
		: ypath(Parse(str))
	{}
	//! \since build 402
	//@{
	Path(const ucs2string& str)
		: ypath(Parse(str))
	{}
	Path(ucs2string&& str)
		: ypath(Parse(str))
	{}
	template<typename _type, typename = typename std::enable_if<!std::is_same<
		typename ystdex::remove_rcv<_type>::type, Path>::value, int>::type>
	Path(_type&& arg, Text::Encoding enc = CS_Path)
		: ypath(Parse(String(yforward(arg), enc)))
	{}
	//@}
	/*!
	\brief 复制构造：默认实现。
	*/
	inline DefDeCopyCtor(Path)
	/*!
	\brief 转移构造：默认实现。
	*/
	inline DefDeMoveCtor(Path)
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

	friend bool
	operator==(const Path&, const Path&);

	friend bool
	operator<(const Path&, const Path&);

	//! \since build 409
	operator String() const;

	DefPred(const ynothrow, Absolute, IO::IsAbsolute(GetNativeString().c_str()))
	DefPred(const ynothrow, Relative, !IsAbsolute())
	/*!
	\brief 判断是否表示目录。
	\note 无视结尾分隔符。
	\since build 298
	*/
	bool
	IsDirectory() const;

	/*!
	\brief 取扩展名。
	\note 非贪婪匹配。
	\since build 409
	*/
	String
	GetExtension() const;
	DefGetter(const ynothrow, NativeString, NativeString,
		String(*this).GetMBCS(CS_Path)) //!< 取本地格式和编码的字符串。

	//! \since build 409
	//@{
	static ypath
	Parse(const ucs2string&);

	using ypath::back;

	using ypath::begin;

	using ypath::cbegin;

	using ypath::clear;

	using ypath::cend;

	using ypath::empty;

	using ypath::end;

	using ypath::erase;

	using ypath::front;

	using ypath::insert;

	using ypath::is_absolute;

	using ypath::is_relative;

	using ypath::size;

	using ypath::swap;
	void
	swap(Path& pth)
	{
		static_cast<ypath&>(pth).swap(*this);
	}
	//@}
};

inline bool
operator==(const Path& x, const Path& y)
{
	return static_cast<const ypath&>(x) == static_cast<const ypath&>(y);
}
inline bool
operator!=(const Path& x, const Path& y)
{
	return !(x == y);
}
inline bool
operator<(const Path& x, const Path& y)
{
	return static_cast<const ypath&>(x) < static_cast<const ypath&>(y);
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
ValidatePath(const Path& pth)
{
	return ValidatePath(pth.GetNativeString());
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

