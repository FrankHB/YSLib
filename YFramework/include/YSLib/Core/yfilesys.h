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
\brief 平台中立的文件系统抽象。
\version r2055
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-28 00:09:28 +0800
\par 修改时间:
	2013-06-13 14:32 +0800
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

//! \since build 411
typedef ystdex::file_path_norm<String> PathNorm;


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

	//! \brief 追加路径。
	//@{
	//! \since build 410
	Path&
	operator/=(const String&);
	Path&
	operator/=(const Path&);
	//@}

	friend bool
	operator==(const Path&, const Path&);

	friend bool
	operator<(const Path&, const Path&);

	/*!
	\brief 转换为字符串。
	\note 使用 VerifyDirectory 验证，当且仅当确认为可打开的目录时结果以分隔符结尾。
	\since build 409
	*/
	operator String() const;
	/*!
	\brief 转换为窄字符串。
	\since build 411
	*/
	DefCvt(const, string, GetMBCS(CS_Path))

	/*!
	\brief 取指定编码的多字节字符串。
	\since build 411
	*/
	PDefH(string, GetMBCS, Text::Encoding enc = CS_Path) const
		ImplRet(String(*this).GetMBCS(enc))
	/*!
	\brief 取字符串表示。
	\post 断言：结果为空或以分隔符结尾。
	\since build 411
	*/
	String
	GetString() const;

	/*!
	\brief 正规化：去除自指和父节点的路径成员。
	\since build 410
	*/
	PDefH(void, Normalize,)
		ImplRet(ystdex::normalize(*this))

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

	//! \since build 410
	using ypath::filter_self;

	using ypath::front;

	//! \since build 410
	using ypath::get_norm;

	using ypath::insert;

	using ypath::is_absolute;

	using ypath::is_relative;

	//! \since build 410
	using ypath::merge_parents;

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
	return std::move(Path(x) /= y);
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
\brief 取扩展名。
\note 非贪婪匹配。
\since build 410
*/
//@{
YF_API String
GetExtensionOf(const String&);
inline String
GetExtensionOf(const Path& pth)
{
	return pth.empty() ? String() : GetExtensionOf(pth.back());
}
//@}


/*!
\brief 取当前工作目录。
\note 不含结尾分隔符。
\since build 304
*/
YF_API String
GetNowDirectory();


//! \since build 410
//@{
//! \brief 验证路径表示的目录是否存在。
//@{
inline PDefH(bool, IsAbsolute, const string& path)
	ImplRet(IsAbsolute(path.c_str()))
inline PDefH(bool, IsAbsolute, const String& path)
	ImplRet(IsAbsolute(path.GetMBCS(CS_Path)))
inline PDefH(bool, IsAbsolute, const Path& pth)
	ImplRet(!pth.empty() && IsAbsolute(pth.GetString()))
//@}

//! \brief 验证路径表示的目录是否存在。
//@{
inline PDefH(bool, IsRelative, const char* path)
	ImplRet(!IsAbsolute(path))
inline PDefH(bool, IsRelative, const string& path)
	ImplRet(!IsAbsolute(path))
inline PDefH(bool, IsRelative, const String& path)
	ImplRet(!IsAbsolute(path))
inline PDefH(bool, IsRelative, const Path& pth)
	ImplRet(!IsAbsolute(pth))
//@}


//! \brief 验证路径表示的目录是否存在。
//@{
YF_API bool
VerifyDirectory(const char*);
inline PDefH(bool, VerifyDirectory, const string& path)
	ImplRet(VerifyDirectory(path.c_str()))
inline PDefH(bool, VerifyDirectory, const String& path)
	ImplRet(VerifyDirectory(path.GetMBCS(CS_Path)))
inline PDefH(bool, VerifyDirectory, const Path& pth)
	ImplRet(!pth.empty() && VerifyDirectory(pth.GetString()))
//@}


//! 路径类别。
enum class PathCategory : u32
{
	Empty,
	Self,
	Parent,
	Node
};

//! 文件系统节点类别。
enum class NodeCategory : u32
{
	Empty = u32(PathCategory::Empty),
	Unknown = u32(PathCategory::Node),
	Directory,
	HardLink,
	SymbolicLink,
	Pipe,
	Socket,
	Normal
};

/*!
\brief 按路径类别对路径成员分类。
\since build 411
*/
YF_API PathCategory
ClassifyPath(const String&, ypath::norm&& = PathNorm());

//! \brief 按文件系统节点类别对路径分类。
YF_API NodeCategory
ClassifyNode(const Path&);
//@}


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
	\since build 412
	*/
	FileList(const char* = {});
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
	virtual DefDeDtor(FileList)

	/*!
	\brief 导航至绝对路径。
	\note 若成功同时读取列表。
	\since build 298
	*/
	bool
	operator=(const Path&);

	/*!
	\brief 导航至子目录。
	\note 若成功同时读取列表。
	\since build 410
	*/
	bool
	operator/=(const String&);
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
	\pre 断言：列表指针非空。
	\since build 411
	*/
	void
	ListItems();
};

YSL_END_NAMESPACE(IO)

YSL_END

#endif

