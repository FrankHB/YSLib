/*
	© 2010-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.h
\ingroup Service
\brief 平台中立的文件系统抽象。
\version r3443
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2010-03-28 00:09:28 +0800
\par 修改时间:
	2018-08-29 19:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::FileSystem
*/


#ifndef YSL_INC_Service_FileSystem_h_
#define YSL_INC_Service_FileSystem_h_ 1

#include "YModules.h"
#include YFM_YSLib_Service_File // for YSLib::CheckNonnegative, IO::Remove;
#include YFM_YSLib_Core_YString
#include <ystdex/path.hpp> // for ystdex::path;

namespace YSLib
{

namespace IO
{

//! \since build 648
using NativePathView = basic_string_view<HDirectory::NativeChar>;

/*!
\brief 默认目录路径长度。
\note 适合一般情形分配空间大小。
\since build 542
*/
yconstexpr const size_t MaxPathLength(yimpl(1 << 10));


/*!
\brief 判断路径表示绝对路径。
\since build 410
*/
//@{
inline PDefH(bool, IsAbsolute, const string& path)
	ImplRet(IsAbsolute(path.c_str()))
inline PDefH(bool, IsAbsolute, const String& path)
	ImplRet(IsAbsolute(path.GetMBCS()))
//@}

/*!
\brief 判断路径表示相对路径（包括空路径）。
\note 使用 ADL IsAbsolute 。
\since build 652
*/
template<typename _type>
inline bool
IsRelative(const _type& arg)
{
	return !IsAbsolute(arg);
}


/*!
\brief 文件路径特征。
\since build 647

在文件系统中使用的路径的特征，扩展抽象路径特征 ystdex::path_traits 。
ystdex::path_traits 中的操作名称仍然保留，部分被在本特征中覆盖实现；
	在本特征添加的特征操作名称使用框架命名风格，并附带接口兼容要求。
本特征约定：
分隔符由 IO::IsSeparator 决定。
第一个元素是空串时为绝对路径，其含义和根目录为一个分隔符的情形一致。
*/
struct PathTraits : ystdex::path_traits<void>
{
	using value_type = YSLib::String;

	//! \note 要求无异常抛出。
	template<typename _tChar>
	static yconstfn bool
	IsDelimiter(_tChar c) ynothrow
	{
		return IO::IsSeparator(c);
	}

	//! \since build 836
	//@{
	template<class _tString>
	static yconstfn bool
	is_absolute(const _tString& str) ynothrow
	{
		return has_root_path(str) && (ystdex::string_empty(str)
			|| IO::IsSeparator(str[ystdex::string_length(str) - 1]));
	}

	template<class _tString>
	static yconstfn bool
	has_root_path(const _tString& str) ynothrow
	{
		return ystdex::string_length(str) == IO::FetchRootPathLength(str);
	}

	template<class _tString>
	static yconstfn bool
	has_root_name(const _tString& str) ynothrow
	{
		return !ystdex::string_empty(str) && IO::FetchRootNameLength(str) != 0;
	}
	//@}
};


/*!
\brief 目录路径结束正规化：保证以一个分隔符结束。
\note 兼容根目录。
\since build 540
*/
template<class _tString>
inline ystdex::decay_t<_tString>
NormalizeDirectoryPathTail(_tString&& str, typename
	ystdex::string_traits<_tString>::const_pointer tail = &ystdex::to_array<
	typename ystdex::string_traits<_tString>::value_type>("/\\")[0])
{
	using value_type = typename ystdex::string_traits<_tString>::value_type;

	TrimTrailingSeperator(yforward(str), tail);
	YAssert(str.empty() || !IsSeparator(str.back()), "Invalid path converted.");
	str += FetchSeparator<value_type>();
	return yforward(str);
}


/*!
\brief 解析字符串为路径。
\note 使用 ADL cbegin 和 cend 。
\note 忽略空路径组件。
\note 检查根路径。若存在根路径且提供的 Path 参数为空则覆盖。
\since build 708
*/
//@{
/*!
\pre 断言：路径参数的数据指针非空。
\since build 836
*/
template<class _tPath, typename _func,
	class _tStringView = string_view_t<typename _tPath::value_type>,
	class _tTraits = typename _tPath::traits_type>
_tPath
ParsePathWith(_tStringView sv, _func f, _tPath res = {})
{
	using ystdex::cbegin;
	using ystdex::cend;

	YAssertNonnull(sv.data());

	const auto l(IO::FetchRootPathLength(sv));

	if(l != 0 && res.empty())
		res.push_back(typename _tPath::value_type(sv.substr(0, l)));
	// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
	ystdex::split(cbegin(sv) + YSLib::CheckNonnegative<ptrdiff_t>(l), cend(sv),
		[](decltype(*cbegin(sv)) c) ynothrow{
		return _tTraits::IsDelimiter(c);
	}, [&](decltype(cbegin(sv)) b, decltype(cend(sv)) e){
		if(b != e)
			f(res, b, e);
	});
	return res;
}

template<class _tPath,
	class _tStringView = string_view_t<typename _tPath::value_type>,
	class _tTraits = typename _tPath::traits_type>
_tPath
ParsePath(_tStringView sv, _tPath res = {})
{
	using ystdex::cbegin;
	using ystdex::cend;

	return IO::ParsePathWith<_tPath>(sv,
		[&](_tPath& pth, decltype(cbegin(sv)) b, decltype(cend(sv)) e){
		pth.push_back(typename _tPath::value_type(b, e));
	}, std::move(res));
}
//@}


//! \since build 409
using ypath = ystdex::path<vector<String>, PathTraits>;


/*!
\brief 路径。
\note 基于 ystdex::path 。
\note 不直接支持非单一分隔符的外部表示（如 OpenVMS 文件），需另行预解析转换。
\warning 非虚析构。
\sa ypath
*/
class YF_API Path : private ypath, private ystdex::totally_ordered<Path>,
	private ystdex::dividable<Path, String>, private ystdex::dividable<Path>
{
public:
	using ypath::iterator;
	using ypath::const_iterator;
	//! \since build 409
	using ypath::value_type;
	//! \since build 708
	using ypath::traits_type;

public:
	/*!
	\brief 无参数构造：默认实现。
	*/
	DefDeCtor(Path)
	//! \since build 635
	explicit
	Path(ypath pth) ynothrow
		: ypath(std::move(pth))
	{}
	/*!
	\pre 间接断言：初始化的视图的路径参数的数据指针非空。
	\since build 837
	*/
	template<typename _type, yimpl(typename = ystdex::enable_if_t<
		ystdex::or_<std::is_constructible<u16string_view, _type&&>,
		std::is_constructible<String, _type&&>>::value>,
		typename = ystdex::exclude_self_t<Path, _type>)>
	explicit
	Path(_type&& arg)
		: ypath(ParsePath<ypath>(u16string_view(AsStringArg(yforward(arg)))))
	{}
	//! \since build 730
	template<typename _type>
	explicit
	Path(_type&& arg, Text::Encoding enc)
		: ypath(ParsePath<ypath>(u16string_view(String(yforward(arg), enc))))
	{}
	//! \since build 599
	template<typename _tIn>
	Path(_tIn first, _tIn last)
		: ypath(first, last)
	{}
	//! \since build 599
	template<typename _tElem>
	Path(std::initializer_list<_tElem> il)
		: ypath(il)
	{}
	/*!
	\brief 复制构造：默认实现。
	*/
	DefDeCopyCtor(Path)
	/*!
	\brief 转移构造：默认实现。
	*/
	DefDeMoveCtor(Path)
	DefDeDtor(Path)

	/*!
	\brief 复制赋值：默认实现。
	\since build 295
	*/
	DefDeCopyAssignment(Path)
	/*!
	\brief 转移赋值：默认实现。
	\since build 295
	*/
	DefDeMoveAssignment(Path)

	//! \brief 追加路径。
	//@{
	//! \since build 410
	PDefHOp(Path&, /=, const String& fname)
		ImplRet(GetBaseRef() /= fname, *this)
	PDefHOp(Path&, /=, const Path& pth)
		ImplRet(GetBaseRef() /= pth, *this)
	//@}

	friend PDefHOp(bool, ==, const Path& x, const Path& y)
		ImplRet(x.GetBase() == y.GetBase())

	friend PDefHOp(bool, <, const Path& x, const Path& y)
		ImplRet(x.GetBase() < y.GetBase())

	/*!
	\brief 转换为字符串。
	\sa Verify
	\since build 409
	*/
	explicit
	DefCvt(const, String, GetLeafString())
	/*!
	\brief 转换为窄字符串。
	\since build 411
	*/
	explicit
	DefCvt(const, string, String(*this).GetMBCS())

	//! \since build 600
	DefGetter(const ynothrow, const ypath&, Base, *this)
	//! \since build 600
	DefGetter(ynothrow, ypath&, BaseRef, *this)
	//! \since build 641
	//@{
	//! \brief 取不带分隔符结束的字符串。
	PDefH(String, GetLeafString,
		char16_t delimiter = FetchSeparator<char16_t>()) const
		ImplRet(ystdex::to_string(GetBase(), {delimiter}))
	/*!
	\brief 取指定分隔符的字符串表示。
	\post 断言：结果为空或以分隔符结束。
	*/
	String
	GetString(char16_t = FetchSeparator<char16_t>()) const;
	//@}

private:
	//! \since build 837
	//@{
	template<typename _tParam>
	auto
	AsStringArg(_tParam&& arg) -> yimpl(ystdex::enable_if_t)<
		std::is_constructible<u16string_view, _tParam&&>::value, u16string_view>
	{
		return u16string_view(yforward(arg));
	}
	template<typename _tParam, yimpl(typename = ystdex::enable_if_t<
		ystdex::and_<ystdex::not_<std::is_constructible<u16string_view,
		_tParam&&>>, std::is_constructible<String, _tParam&&>>::value
	>)>
	String
	AsStringArg(_tParam&& arg)
	{
		return String(yforward(arg));
	}
	//@}

public:
	/*!
	\brief 正规化：去除自指和父节点的路径成员。
	\since build 410
	*/
	PDefH(void, Normalize, )
		ImplExpr(ystdex::normalize(*this))

	/*!
	\brief 验证：转换为指定分隔符表示的字符串并检查分隔符。
	\note 使用 VerifyDirectory 验证，当且仅当确认为可打开的目录时结果以分隔符结束。
	\sa GetString
	\sa VerifyDirectory
	\since build 599
	*/
	String
	Verify(char16_t = FetchSeparator<char16_t>()) const;
	/*!
	\brief 验证指定分隔符和编码的多字节字符串。
	\since build 651
	*/
	PDefH(string, VerifyAsMBCS,
		char16_t delimiter = char16_t(FetchSeparator<char16_t>()),
		Text::Encoding enc = Text::CS_Default) const
		ImplRet(Verify(delimiter).GetMBCS(enc))

	//! \since build 409
	//@{
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

	//! \since build 837
	using ypath::has_leaf;

	//! \since build 837
	using ypath::has_leaf_nonempty;

	using ypath::insert;

	using ypath::is_absolute;

	using ypath::is_relative;

	//! \since build 599
	using ypath::max_size;

	//! \since build 410
	using ypath::merge_parents;

	//! \since build 473
	using ypath::pop_back;

	//! \since build 473
	using ypath::push_back;

	//! \since build 837
	using ypath::redirect;

	//! \since build 837
	using ypath::remove_leaf;

	//! \since build 599
	using ypath::size;

	//! \since build 710
	//@{
	friend PDefH(void, swap, Path& x, ypath& y)
		ImplExpr(y.swap(x));
	friend PDefH(void, swap, ypath& x, Path& y)
		ImplExpr(x.swap(y));
	friend PDefH(void, swap, Path& x, Path& y)
		ImplExpr(y.GetBaseRef().swap(x))
	//@}

	//! \since build 475
	friend PDefH(String, to_string, const Path& pth)
		ImplRet(to_string(pth.GetBase(), {FetchSeparator<char16_t>()}))
	//@}
};


/*!
\brief 取扩展名。
\note 非贪婪匹配。
\since build 410
*/
//@{
//! \since build 599
template<class _tString>
_tString
GetExtensionOf(const _tString& fname)
{
	return ystdex::find_suffix(fname, typename _tString::value_type('.'));
}
//! \relates Path
inline PDefH(String, GetExtensionOf, const Path& pth)
	ImplRet(pth.empty() ? String() : GetExtensionOf(pth.back()))
//@}


//! \relates Path
//@{
/*!
\brief 判断路径表示绝对路径。
\since build 410
*/
inline PDefH(bool, IsAbsolute, const Path& pth)
	ImplRet(!pth.empty() && IsAbsolute(pth.GetString()))

/*!
\brief 根据当前工作目录和指定路径取绝对路径。
\post 断言：结果是绝对路径。
\return 若指定路径为相对路径则为正规化当前工作目录追加此路径，否则为正规化指定路径。
\note 第二参数指定取当前工作目录指定的起始缓冲区大小。
\sa IsRelative
\sa Path::Normalize
\sa FetchCurrentWorkingDirectory
\since build 542
*/
YF_API Path
MakeNormalizedAbsolute(const Path&, size_t = MaxPathLength);
//@}


/*!
\brief 验证路径表示的目录是否存在且可打开。
\note 受权限限制。
\since build 410
*/
//@{
YF_API YB_NONNULL(1) bool
VerifyDirectory(const char*);
//! \since build 699
YF_API YB_NONNULL(1) bool
VerifyDirectory(const char16_t*);
inline PDefH(bool, VerifyDirectory, const string& path)
	ImplRet(VerifyDirectory(path.c_str()))
//! \since build 699
inline PDefH(bool, VerifyDirectory, const u16string& path)
	ImplRet(VerifyDirectory(path.c_str()))
inline PDefH(bool, VerifyDirectory, const String& path)
	ImplRet(VerifyDirectory(path.c_str()))
inline PDefH(bool, VerifyDirectory, const Path& pth)
	ImplRet(!pth.empty() && VerifyDirectory(pth.GetString()))
//@}

/*!
\brief 验证目录路径结尾：确保当且仅当验证成功时保留结尾字符。
\note 使用 ADL VerfiyDirectory 。
\since build 710
*/
template<class _tString>
inline ystdex::decay_t<_tString>
VerifyDirectoryPathTail(_tString&& str)
{
	if(!(str.empty() || VerifyDirectory(str)))
		str.pop_back();
	return yforward(str);
}


/*!
\brief 验证路径表示的目录是否存在，若不存在则逐级创建。
\throw std::system_error 失败时根据 errno 抛出的异常。
\note 使用 umkdir 实现。
\since build 563
*/
//@{
YF_API void
EnsureDirectory(const Path&);
inline PDefH(void, EnsureDirectory, const char* path)
	ImplExpr(EnsureDirectory(Path(path)))
inline PDefH(void, EnsureDirectory, const string& path)
	ImplExpr(EnsureDirectory(path.c_str()))
inline PDefH(void, EnsureDirectory, const String& path)
	ImplExpr(EnsureDirectory(path.GetMBCS()))
//@}


/*!
\brief 解析路径：取跟踪链接的绝对路径。
\return 解析得到的绝对路径。
\exception std::system_error std::errc::too_many_symbolic_link_levels
	超过链接限制。
\note 忽略空路径组件。
\note 使用 ADL cbegin 和 cend 。
\since build 708
*/
//@{
/*!
\note 第二参数指定起始路径。
\note 第三参数指定解析链接的次数上限。
\since build 836
*/
template<class _tPath = Path,
	class _tStringView = string_view_t<typename _tPath::value_type>,
	class _tTraits = typename _tPath::traits_type>
_tPath
ResolvePathWithBase(_tStringView sv, _tPath base,
	size_t n = FetchLimit(SystemOption::MaxSymlinkLoop))
{
	using ystdex::cbegin;
	using ystdex::cend;

	if(IsAbsolute(sv.data()))
		base.clear();
	return IO::ParsePathWith<_tPath>(sv,
		[&](_tPath& pth, decltype(cbegin(sv)) b, decltype(cend(sv)) e){
		// NOTE: Since the underlying call would resolve the path of the link,
		//	there is no need to resolve it using %operator/= here.
		pth.push_back(typename _tPath::value_type(b, e));

		// XXX: This may be not efficient because underlying calls will resolve
		//	the path prefix repeatedly.
		auto str(to_string(pth));

		// FIXME: Blocked. TOCTTOU access.
		if(IO::IterateLink(str, n))
			pth.redirect(IO::ParsePath<_tPath>(std::move(str)));
	}, std::move(base));
}

/*!
\note 第二参数指定解析链接的次数上限。
\note 第三参数指定起始路径使用的初始缓冲区大小。
*/
template<class _tPath = Path, class _tString = typename _tPath::value_type,
	class _tTraits = typename _tPath::traits_type>
_tPath
ResolvePath(const _tString& sv,
	size_t n = FetchLimit(SystemOption::MaxSymlinkLoop),
	size_t init_len = MaxPathLength)
{
	return IO::ResolvePathWithBase(sv, IsAbsolute(sv.data()) ? _tPath()
		: IO::ParsePath<_tPath>(FetchCurrentWorkingDirectory<typename
		_tString::value_type>(init_len)), n);
}
//@}


/*!
\brief 目录遍历操作。
\since build 538
*/
//@{
template<typename _func>
void
Traverse(HDirectory& dir, _func f)
{
	for(NativePathView npv : dir)
	{
		YAssert(!npv.empty(), "Empty name found.");
		// TODO: Allow user code to specify filtering on self node?
		if(!PathTraits::is_self(npv))
			f(dir.GetNodeCategory(), npv);
	}
}
//! \exception std::system_error 目录打开失败。
//@{
//! \note 允许目录路径以分隔符结束。
//@{
//! \pre 间接断言：指针参数非空。
template<typename _func>
YB_NONNULL(1) inline void
Traverse(const char* path, _func f)
{
	// NOTE: Separators at end of path is allowed by %HDirectory::HDirectory.
	HDirectory dir(path);

	IO::Traverse(dir, f);
}
//! \since build 593
template<typename _func>
inline void
Traverse(const string& path, _func f)
{
	IO::Traverse(path.c_str(), f);
}
//@}
template<typename _func>
inline void
Traverse(const Path& pth, _func f)
{
	IO::Traverse(string(pth), f);
}
//@}

//! \note 允许目录路径以分隔符结束。
//@{
/*!
\pre 间接断言：指针参数非空。
\since build 654
*/
template<typename _func>
YB_NONNULL(1) void
TraverseChildren(const char* path, _func f)
{
	IO::Traverse(path, [f](NodeCategory c, NativePathView npv)
		ynoexcept_spec(f(c, npv)){
		if(!PathTraits::is_parent(npv))
			f(c, npv);
	});
}
//! \since build 593
template<typename _func>
void
TraverseChildren(const string& path, _func f)
{
	IO::TraverseChildren(path.c_str(), f);
}
//@}
//! \since build 654
template<typename _func>
inline void
TraverseChildren(const Path& pth, _func f)
{
	IO::TraverseChildren(string(pth), f);
}
//@}

/*!
\brief 递归遍历目录树。
\warning 不检查无限递归调用。
\note 使用 ADL TraverseChildren 遍历子目录。
\note 函数传入的路径参数可以被修改。
\since build 657
*/
template<typename _func, typename... _tParams>
auto
TraverseTree(_func f, const Path& dst, const Path& src, _tParams&&... args)
	-> decltype(f(std::declval<Path&>(), std::declval<Path&>(),
	std::forward<_tParams>(args)...))
{
	EnsureDirectory(dst);
	TraverseChildren(src, [&](NodeCategory c, NativePathView npv){
		const String name(npv);
		// NOTE: Allowed to be modified.
		auto dname(dst / name), sname(src / name);

		// XXX: Blocked. 'yforward' cause G++ 5.2 crash: internal compiler
		//	error: Aborted (program cc1plus) or crash silently.
		return bool(c & NodeCategory::Directory)
			? IO::TraverseTree(f, dname, sname, std::forward<_tParams>(args)...)
			: f(dname, sname, std::forward<_tParams>(args)...);
	});
}


//! \since build 651
//@{
/*!
\brief 复制文件处理器：通知文件复制事件。
\note 函数参数分别对应目标和源。
\since build 651
*/
using CopyFileHandler = std::function<void(FileDescriptor, FileDescriptor)>;

//! \sa CopyFileHandler
//@{
//! \brief 保持修改时间。
const auto PreserveModificationTime(
	ystdex::bind_forward(&FileDescriptor::SetModificationTime,
	&FileDescriptor::GetModificationTime));
//! \brief 保持修改和访问时间。
const auto PreserveModificationAndAccessTime(
	ystdex::bind_forward(&FileDescriptor::SetModificationAndAccessTime,
	&FileDescriptor::GetModificationAndAccessTime));
//@}
//@}

/*!
\brief 复制文件。
\pre 间接断言：表示目标和源的参数非空。
\note 第一参数表示目标，第二参数表示源。
\note mode_t 参数依次表示打开目标和源的权限模式。
\note 不复制元数据。
\see $2015-09 @ %Documentation::Workflow::Annual2015.
\since build 648
*/
//@{
/*!
\note 不清空目标。
\since build 637
*/
YF_API void
CopyFile(UniqueFile, FileDescriptor);
//! \exception std::system_error 打开文件失败。
//@{
//! \note 不清空目标。
YF_API YB_NONNULL(2) void
CopyFile(UniqueFile, const char*);
//! \sa EnsureUniqueFile
//@{
//! \note 除第二参数外含义和 EnsureUniqueFile 的参数依次相同。
//@{
//! \since build 639
YF_API YB_NONNULL(1) void
CopyFile(const char*, FileDescriptor, mode_t = DefaultPMode(),
	size_t = 1, bool = {});
YF_API YB_NONNULL(1, 2) void
CopyFile(const char*, const char*, mode_t = DefaultPMode(), size_t = 1,
	bool = {});
//@}
/*!
\exception std::bad_function_call 第三参数为空。
\note 第三参数指定成功复制内容后关闭目标文件前的操作。
\note 除第二和第三参数外含义和 EnsureUniqueFile 的参数依次相同。
\since build 651
*/
//@{
YF_API YB_NONNULL(1) void
CopyFile(const char*, FileDescriptor, CopyFileHandler, mode_t = DefaultPMode(),
	size_t = 1, bool = {});
YF_API YB_NONNULL(1, 2) void
CopyFile(const char*, const char*, CopyFileHandler, mode_t = DefaultPMode(),
	size_t = 1, bool = {});
//@}
//@}
//@}
//@}

//! \note 失败时立刻终止操作。
//@{
/*!
\brief 复制目录树。
\warning 不检查无限递归调用。
\note 使用 ADL TraverseChildren 递归遍历。
\since build 648
*/
template<typename... _tParams>
void
CopyTree(const Path& dst, const Path& src, _tParams&&... args)
{
	// XXX: Blocked. 'yforward' may cause G++ 5.2 silent crash.
	IO::TraverseTree([](const Path& dname, const Path& sname,
		_tParams&&... fargs){
		IO::CopyFile(string(dname).c_str(), string(sname).c_str(),
			std::forward<_tParams>(fargs)...);
	}, dst, src, std::forward<_tParams>(args)...);
}

//! \exception std::system_error 路径指向的不是一个目录或删除失败。
//@{
/*!
\brief 清空参数指定路径的目录树内容。
\since build 639
*/
//@{
YF_API void
ClearTree(const Path&);
inline PDefH(void, ClearTree, const string& pth)
	ImplRet(ClearTree(Path(pth)))
//@}

//! \brief 删除参数指定路径的目录树。
//@{
inline PDefH(void, DeleteTree, const Path& pth)
	ImplExpr(ClearTree(pth), IO::Remove(string(pth).c_str()))
//! \since build 593
inline PDefH(void, DeleteTree, const string& pth)
	ImplExpr(DeleteTree(Path(pth)))
//@}
//@}

/*!
\brief 遍历目录中的项目，更新至列表。
\exception std::system_error 操作失败。
\since build 538
*/
YF_API void
ListFiles(const Path&, vector<String>&);
//@}


/*!
\brief 按文件系统节点类别对路径分类。
\since build 410
\todo 支持目录和常规文件外的其它类别。
*/
YF_API NodeCategory
ClassifyNode(const Path&);

} // namespace IO;

} // namespace YSLib;

#endif

