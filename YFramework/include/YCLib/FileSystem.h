/*
	© 2011-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.h
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r4234
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:38:37 +0800
\par 修改时间:
	2022-01-13 22:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#ifndef YCL_INC_FileSystem_h_
#define YCL_INC_FileSystem_h_ 1

#include "YModules.h"
#include YFM_YCLib_Debug // for Nonnull, Deref, basic_string_view,
//	ystdex::is_null, string_view_t, ystdex::exclude_self_t, std::wint_t,
//	ystdex::to_array, ystdex::string_traits, ystdex::rtrim, string, u16string,
//	ystdex::retry_for_vector, std::FILE, std::uint8_t, std::uint32_t,
//	std::memcmp, pair, size, tuple;
#include YFM_YCLib_Reference // for unique_ptr_from, tidy_ptr;
#include <system_error> // for std::system_error;
#include <ystdex/base.h> // for ystdex::deref_self;
#include <ystdex/function.hpp> // for ystdex::function;
#include <ystdex/iterator.hpp> // for ystdex::indirect_input_iterator;
#include <ystdex/operators.hpp> // for ystdex::equality_comparable;
#include <chrono> // for std::chrono::nanoseconds;
#include <ystdex/type_pun.hpp> // for ystdex::is_trivially_replaceable;
#include <ctime> // for std::time_t;
#include <ystdex/cstdint.hpp> // for ystdex::read_uint_le;

#if !YCL_Win32 && !YCL_API_POSIXFileSystem
#	error "Unsupported platform found."
#endif

#if !YCL_Win32
//! \since build 680
struct dirent;
#endif

namespace platform
{

//! \since build 654
template<typename _tChar>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
IsColon(_tChar c) ynothrow
{
	return c == _tChar(':');
}

//! \since build 654
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(1) inline const _tChar*
FindColon(const _tChar* p) ynothrowv
{
	return ystdex::ntctschr(Nonnull(p), _tChar(':'));
}

//! \since build 705
//@{
//! \note 取平台首选的路径分隔字符。
//@{
template<typename _tChar>
YB_STATELESS YCL_Tag_constfn _tChar
FetchSeparator_P(IDTag<YF_Platform_Win32>) ynothrow
{
	return '\\';
}
template<typename _tChar>
YB_STATELESS YCL_Tag_constfn _tChar
FetchSeparator_P(IDTagBase) ynothrow
{
	return '/';
}

template<typename _tChar>
YB_STATELESS yconstfn _tChar
FetchSeparator() ynothrow
{
	return FetchSeparator_P<_tChar>(IDTag<YF_Platform>());
}
//@}

//! \note 判断字符是否为平台支持的路径分隔符。
//@{
template<typename _tChar>
YB_STATELESS YCL_Tag_constfn bool
IsSeparator_P(IDTag<YF_Platform_Win32> tag, _tChar c) ynothrow
{
	return c == FetchSeparator_P<_tChar>(tag) || c == _tChar('/');
}
template<typename _tChar>
YB_STATELESS YCL_Tag_constfn bool
IsSeparator_P(IDTagBase tag, _tChar c) ynothrow
{
	return c == FetchSeparator_P<_tChar>(tag);
}

YCL_DefPlatformFwdTmpl(IsSeparator, IsSeparator_P)
//@}
//@}

/*!
\todo 支持非 Win32 文件路径特化。
\sa FetchSeparator_P
*/
//@{
/*!
\pre 间接断言：指针路径参数或路径参数的数据指针非空。
\note 一般字符串类型在此不支持，需另行重载，以避免空字符的语义问题和重载冲突。
\todo 支持非 POSIX 文件路径特化。
*/
//@{
/*!
\brief 判断指定路径字符串是否表示一个绝对路径。
\note 空路径不是绝对路径。
*/
//@{
//! \since build 654
//@{
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline bool
IsAbsolute_P(IDTag<YF_Platform_DS> tag, const _tChar* path) ynothrowv
{
	// XXX: Separator used in root.
	return
		platform::IsSeparator_P(tag, Deref(path)) || platform::FindColon(path);
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE inline bool
IsAbsolute_P(IDTag<YF_Platform_DS> tag, basic_string_view<_tChar> path)
	ynothrowv
{
	// XXX: Separator used in root.
	YAssertNonnull(path.data());
	return !path.empty() && (platform::IsSeparator_P(tag, path.front())
		|| path.find(_tChar(':')) != basic_string_view<_tChar>::npos);
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline bool
IsAbsolute_P(IDTag<YF_Platform_Win32> tag, const _tChar* path) ynothrowv
{
	return platform::IsSeparator_P(tag, Deref(path))
		|| (!ystdex::is_null(*path) && IsColon(*++path));
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE inline bool
IsAbsolute_P(IDTag<YF_Platform_Win32> tag, basic_string_view<_tChar> path)
	ynothrowv
{
	YAssertNonnull(path.data());
	return path.length() > 1
		&& (platform::IsSeparator_P(tag, path.front()) || IsColon(path[1]));
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline bool
IsAbsolute_P(IDTagBase tag, const _tChar* path) ynothrowv
{
	return platform::IsSeparator_P(tag, Deref(path));
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE inline bool
IsAbsolute_P(IDTagBase tag, basic_string_view<_tChar> path) ynothrowv
{
	YAssertNonnull(path.data());
	return !path.empty() && platform::IsSeparator_P(tag, path.front());
}
//@}

//! \since build 652
YCL_DefPlatformFwdTmpl(IsAbsolute, IsAbsolute_P)
//@}

//! \since build 836
//@{
#define YCL_Impl_DefPlatformStringFwdTmpl(_fn) \
	template<class _tTag, class _tString> \
	inline auto \
	_fn(_tTag tag, const _tString& path) ynothrowv \
		-> ystdex::exclude_self_t<string_view_t<_tString>, _tString, \
		decltype(platform::_fn(tag, string_view_t<_tString>(path)))> \
	{ \
		return platform::_fn(tag, string_view_t<_tString>(path)); \
	}

/*!
\brief 取指定路径的文件系统根节点名称的结束位置。
\note 不计入可能存在的紧随在根名称后的一个或多个文件分隔符。
*/
//@{
template<typename _tChar>
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_NONNULL(2) YB_PURE inline
	const _tChar*
FetchRootNameEnd_P(IDTag<YF_Platform_DS>, const _tChar* path) ynothrowv
{
	const auto p(platform::FindColon(path));

	return p ? p : path;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline const _tChar*
FetchRootNameEnd_P(IDTag<YF_Platform_Win32>, const _tChar*) ynothrowv;
template<class _tTag, typename _tChar>
YB_ATTR_nodiscard YB_PURE typename basic_string_view<_tChar>::const_iterator
FetchRootNameEnd_P(_tTag, basic_string_view<_tChar>) ynothrowv;
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline const _tChar*
FetchRootNameEnd_P(IDTagBase, const _tChar*) ynothrowv;
YCL_Impl_DefPlatformStringFwdTmpl(FetchRootNameEnd_P)

YCL_DefPlatformFwdTmpl(FetchRootNameEnd, FetchRootNameEnd_P)
//@}
//@}

/*!
\brief 取指定路径的文件系统根节点名称的长度。
\note 不计入可能存在的紧随在根名称后的一个或多个文件分隔符。
*/
//@{
//! \since build 654
//@{
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline size_t
FetchRootNameLength_P(IDTag<YF_Platform_DS> tag, const _tChar* path) ynothrowv
{
	return size_t(platform::FetchRootNameEnd_P(tag, path) - path);
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE size_t
FetchRootNameLength_P(IDTag<YF_Platform_DS>, basic_string_view<_tChar> path)
	ynothrowv
{
	YAssertNonnull(path.data());

	auto n(path.find_first_of(_tChar(':')));

	return n != basic_string_view<_tChar>::npos ? n : 0;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) size_t
FetchRootNameLength_P(IDTag<YF_Platform_Win32> tag, const _tChar* path)
	ynothrowv
{
	if(!ystdex::is_null(path[0]))
	{
		// XXX: Conversion to 'std::wint_t' might be implementation-defined.
		if(IsColon(path[1]) && std::iswalpha(std::wint_t(path[0])))
			return 2;
		if(platform::IsSeparator_P(tag, path[0]) && !ystdex::is_null(path[1])
			&& !ystdex::is_null(path[2]))
		{
			// NOTE: UNC name with prefix of form '\\?\', '\\.\' or '\??\'.
			if(!ystdex::is_null(path[3]) && platform::IsSeparator_P(tag,
				path[3]) && !platform::IsSeparator_P(tag, path[4])
				&& ((platform::IsSeparator_P(tag, path[1]) && (path[2] == '?'
				|| path[2] == '.')) || (path[1] == '?' && path[2] == '?')))
				return 3;
			// NOTE: UNC name with prefix of form '\\server'.
			if(platform::IsSeparator_P(tag, path[1])
				&& !platform::IsSeparator_P(tag, path[2]))
			{
				auto p(path + 3);

				while(!ystdex::is_null(*p) && !platform::IsSeparator_P(tag, *p))
					++p;
				return p - path;
			}
		}
	}
	return 0;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_PURE size_t
FetchRootNameLength_P(IDTag<YF_Platform_Win32> tag,
	basic_string_view<_tChar> path) ynothrowv
{
	YAssertNonnull(path.data());

	const auto l(path.size());

	if(l >= 2)
	{
		// NOTE: Name with prefix of form 'X:'.
		// TODO: Memory load can be fused.
		// XXX: Conversion to 'std::wint_t' might be implementation-defined.
		if(IsColon(path[1]) && std::iswalpha(std::wint_t(path[0])))
			return 2;
		if(platform::IsSeparator_P(tag, path[0]))
		{
			// NOTE: UNC name with prefix of form '\\?\', '\\.\' or '\??\'.
			if(l >= 4 && platform::IsSeparator_P(tag, path[3])
				&& (l == 4 || !platform::IsSeparator_P(tag, path[4]))
				&& ((platform::IsSeparator_P(tag, path[1]) && (path[2] == '?'
				|| path[2] == '.')) || (path[1] == '?' && path[2] == '?')))
				return 3;
			// NOTE: UNC name with prefix of form '\\server'.
			if(l >= 3 && platform::IsSeparator_P(tag, path[1])
				&& !platform::IsSeparator_P(tag, path[2]))
				// XXX: Use %platform::IsSeparator_P?
				return std::min(path.find_first_of(&ystdex::to_array<_tChar>(
					"/\\")[0], 3), path.length());
		}
	}
	return 0;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline size_t
FetchRootNameLength_P(IDTagBase tag, const _tChar* path) ynothrowv
{
	return platform::IsSeparator_P(tag, Deref(path)) ? 1 : 0;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE inline size_t
FetchRootNameLength_P(IDTagBase tag, basic_string_view<_tChar> path)
	ynothrowv
{
	YAssertNonnull(path.data());
	return !path.empty() && platform::IsSeparator_P(tag, path.front()) ? 1 : 0;
}
//@}
//! \since build 836
YCL_Impl_DefPlatformStringFwdTmpl(FetchRootNameLength_P)

//! \since build 653
YCL_DefPlatformFwdTmpl(FetchRootNameLength, FetchRootNameLength_P)
//@}

//! \since build 836
//@{
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline const _tChar*
FetchRootNameEnd_P(IDTag<YF_Platform_Win32> tag, const _tChar* path) ynothrowv
{
	return path + platform::FetchRootNameLength_P(tag, path);
}
template<class _tTag, typename _tChar>
YB_ATTR_nodiscard YB_PURE typename basic_string_view<_tChar>::const_iterator
FetchRootNameEnd_P(_tTag tag, basic_string_view<_tChar> path) ynothrowv
{
	return path.cbegin() + platform::FetchRootNameLength_P(tag, path);
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE YB_NONNULL(2) inline const _tChar*
FetchRootNameEnd_P(IDTagBase tag, const _tChar* path) ynothrowv
{
	return path + platform::FetchRootNameLength_P(tag, path);
}

/*!
\brief 取指定路径的文件系统根节点路径的长度。
\note 计入可能存在的紧随在根名称后的一个或多个文件分隔符。
*/
//@{
template<typename _tChar>
YB_ATTR_nodiscard YB_NONNULL(2) YB_PURE size_t
FetchRootPathLength_P(IDTag<YF_Platform_DS> tag, const _tChar* path) ynothrowv
{
	auto p(platform::FindColon(path));

	if(!p)
		p = path;
	while(platform::IsSeparator_P(tag, *p))
		++p;
	return size_t(p - path);
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE size_t
FetchRootPathLength_P(IDTag<YF_Platform_DS>, basic_string_view<_tChar> path)
	ynothrowv
{
	YAssertNonnull(path.data());

	auto n(path.find_first_of(_tChar(':')));

	if(n != basic_string_view<_tChar>::npos)
		n = path.find_first_not_of(_tChar('/'), n + 1);
	return n != basic_string_view<_tChar>::npos ? n : 0;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_NONNULL(2) YB_PURE size_t
FetchRootPathLength_P(IDTag<YF_Platform_Win32> tag, const _tChar* path)
	ynothrowv
{
	const auto l(platform::FetchRootNameLength_P(tag, path));

	if(l > 0)
	{
		auto p(path + l);

		// TODO: Extract as %ystdex::tstr_find_first_not_of?
		while(!ystdex::is_null(*p) && !platform::IsSeparator_P(tag, *p))
			++p;
		return p - path;
	}
	return l;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE size_t
FetchRootPathLength_P(IDTag<YF_Platform_Win32> tag,
	basic_string_view<_tChar> path) ynothrowv
{
	const auto l(platform::FetchRootNameLength_P(tag, path));

	if(l > 0)
	{
		// XXX: Use %platform::IsSeparator_P?
		const auto
			n(path.find_first_not_of(&ystdex::to_array<_tChar>("/\\")[0], l));

		return n != basic_string_view<_tChar>::npos ? n : path.length();
	}
	return l;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_NONNULL(2) YB_PURE inline size_t
FetchRootPathLength_P(IDTagBase tag, const _tChar* path) ynothrowv
{
	return platform::IsSeparator_P(tag, Deref(path)) ? 1 : 0;
}
template<typename _tChar>
YB_ATTR_nodiscard YB_PURE inline size_t
FetchRootPathLength_P(IDTagBase tag, basic_string_view<_tChar> path)
	ynothrowv
{
	YAssertNonnull(path.data());
	return !path.empty() && platform::IsSeparator_P(tag, path.front()) ? 1 : 0;
}
YCL_Impl_DefPlatformStringFwdTmpl(FetchRootPathLength_P)

YCL_DefPlatformFwdTmpl(FetchRootPathLength, FetchRootPathLength_P)
//@}
//@}
//@}

#undef YCL_Impl_DefPlatformStringFwdTmpl

/*!
\brief 判断路径字符串是否以非分隔符结束。
\since build 693
\todo 支持字符指针。
*/
//@{
template<class _tString>
YB_ATTR_nodiscard YB_PURE yconstfn bool
EndsWithNonSeperator_P(IDTagBase tag, const _tString& path) ynothrow
{
	return !path.empty() && !platform::IsSeparator_P(tag, path.back());
}

YCL_DefPlatformFwdTmpl(EndsWithNonSeperator, EndsWithNonSeperator_P)
//@}

//! \since build 707
//@{
template<class _tString>
YCL_Tag_constfn _tString&&
TrimTrailingSeperator_P(IDTag<YF_Platform_Win32>, _tString&& path, typename
	ystdex::string_traits<_tString>::const_pointer tail = &ystdex::to_array<
	typename ystdex::string_traits<_tString>::value_type>("/\\")[0]) ynothrow
{
	return ystdex::rtrim(yforward(path), tail);
}

template<class _tString>
YCL_Tag_constfn _tString&&
TrimTrailingSeperator_P(IDTagBase, _tString&& path, typename
	ystdex::string_traits<_tString>::const_pointer tail = &ystdex::to_array<
	typename ystdex::string_traits<_tString>::value_type>("/")[0]) ynothrow
{
	return ystdex::rtrim(yforward(path), tail);
}

YCL_DefPlatformFwdTmpl(TrimTrailingSeperator, TrimTrailingSeperator_P)
//@}
//@}


/*!
\brief 文件节点类别。
\since build 412
*/
enum class NodeCategory : std::uint_least32_t
{
	Empty = 0,
	//! \since build 474
	//@{
	Invalid = 1 << 0,
	Regular = 1 << 1,
	//@}
	Unknown = Invalid | Regular,
	//! \since build 474
	//@{
	Device = 1 << 9,
	Block = Device,
	Character = Device | 1 << 7,
	Communicator = 2 << 9,
	FIFO = Communicator | 1 << 6,
	Socket = Communicator | 2 << 6,
	//@}
	SymbolicLink = 1 << 12,
	MountPoint = 2 << 12,
	Junction = MountPoint,
	//! \since build 474
	//@{
	Link = SymbolicLink | Junction,
	//@}
	Directory = 1 << 15,
	//! \since build 474
	//@{
	Missing = 1 << 16,
	Special = Link | Missing
	//@}
};

/*!
\relates NodeCategory
\since build 543
*/
DefBitmaskEnum(NodeCategory)


/*!
\brief 判断指定路径是否指定目录。
\pre 间接断言：参数非空。
\since build 707
*/
//@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
IsDirectory(const char*);
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
IsDirectory(const char16_t*);
//@}


//! \since build 901
class FileDescriptor;

/*!
\brief 取指定的文件的链接数。
\return 指定的文件在文件系统中共享的实例数。
*/
//@{
//! \since build 901
YB_ATTR_nodiscard YF_API size_t
FetchNumberOfLinks(const FileDescriptor&);
/*!
\note 第二参数表示跟随连接：若文件系统支持，访问链接的文件而不是链接自身。
\since build 846
*/
//@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) size_t
FetchNumberOfLinks(const char*, bool = {});
YB_ATTR_nodiscard YF_API YB_NONNULL(1) size_t
FetchNumberOfLinks(const char16_t*, bool = {});
//@}
//@}

/*!
\pre 断言：路径指针非空。
\exception Win32Exception Win32 平台：本机 API 调用失败。
\exception std::system_error 系统错误。
	\li std::errc::function_not_supported 操作不被文件系统支持。
	\li 可能有其它错误。
*/
//@{
//! \note DS 平台：当前实现不支持替换文件系统，因此始终不支持操作。
//@{
//! \note 前两个参数为目标路径和源路径。
//@{
/*!
\brief 创建硬链接。
\note 源路径指定符号链接时，跟随符号链接目标。
\since build 633
*/
//@{
#if YCL_DS
YB_NORETURN
#endif
YF_API YB_NONNULL(1, 2) void
CreateHardLink(const char*, const char*);
#if YCL_DS
YB_NORETURN
#endif
YF_API YB_NONNULL(1, 2) void
CreateHardLink(const char16_t*, const char16_t*);
//@}

/*!
\brief 创建符号链接。
\note 第三参数指定是否创建目录链接。
\note Win32 平台：成功调用需要操作系统、文件系统和权限（或组策略）支持。
\note 非 Win32 平台：忽略第三参数。
\since build 651
*/
//@{
#if YCL_DS
YB_NORETURN
#endif
YF_API YB_NONNULL(1, 2) void
CreateSymbolicLink(const char*, const char*, bool = {});
#if YCL_DS
YB_NORETURN
#endif
YF_API YB_NONNULL(1, 2) void
CreateSymbolicLink(const char16_t*, const char16_t*, bool = {});
//@}
//@}

/*!
\brief 读取链接指向的路径。
\throw std::invalid_argument 指定的路径存在但不是连接。
\note 支持 Windows 目录链接和符号链接，不特别区分。
\note POSIX 平台：不同于 ::realpath ，分配合适的大小而不依赖 PATH_MAX 。自动重试
	分配足够长的字符串以支持不完全符合 POSIX 的文件系统（如 Linux 的 procfs ）
	导致文件大小为 0 时的情形。
\since build 660
*/
//@{
YB_ATTR_nodiscard
#if YCL_DS
YB_NORETURN
#endif
YF_API YB_NONNULL(1) string
ReadLink(const char*);
YB_ATTR_nodiscard
#if YCL_DS
YB_NORETURN
#endif
YF_API YB_NONNULL(1) u16string
ReadLink(const char16_t*);
//@}
//@}

/*!
\brief 迭代访问链接：替换非空路径并按需减少允许链接计数。
\return 是否成功访问了链接。
\throw std::system_error 系统错误：调用检查失败。
	\li std::errc::too_many_symbolic_link_levels 减少计数后等于 0 。
\note 忽略空路径。对路径类别中立，用户需自行判断是否为绝对路径或相对路径。
\note DS 平台：空实现。
\since build 708
*/
//@{
#if YCL_DS
inline PDefH(bool, IterateLink, string&, size_t&)
	ImplRet({})
inline PDefH(bool, IterateLink, u16string&, size_t&)
	ImplRet({})
#else
YF_API bool
IterateLink(string&, size_t&);
YF_API bool
IterateLink(u16string&, size_t&);
#endif
//@}
//@}


/*!
\brief 目录会话：表示打开的目录。
\note 转移后状态无法通过其它操作取得。
\warning 非虚析构。
\since build 411
*/
class YF_API DirectorySession
{
public:
#if YCL_Win32
	//! \since build 669
	class Data;
#else
	//! \since build 669
	using Data = void;
#endif
	using NativeHandle = Data*;

private:
	//! \since build 669
	class YF_API Deleter
#if YCL_Win32
		: private default_delete<Data>
#endif
	{
	public:
		using pointer = NativeHandle;

		//! \since build 671
		void
		operator()(pointer) const ynothrowv;
	};

#if !YCL_Win32
protected:
	/*!
	\brief 目录路径。
	\invariant <tt>!dir || (ystdex::string_length(sDirPath.c_str()) > 0 && \
		sDirPath.back() == FetchSeparator<char>())</tt> 。
	\since build 593
	*/
	string sDirPath;
#endif

private:
	//! \since build 671
	unique_ptr_from<Deleter> dir;

public:
	/*!
	\brief 构造：打开目录路径。
	\pre 间接断言：参数非空。
	\throw std::system_error 打开失败。
	\note 路径可以一个或多个分隔符结束；结束的分隔符会被视为单一分隔符。
	\note 当路径只包含分隔符或为空字符串时视为当前目录。
	\note 实现中 Win32 使用 UCS2-LE ，其它平台使用 UTF-8 ；否则需要编码转换。
	\note Win32 平台： "/" 可能也被作为分隔符支持。
	\note Win32 平台： 前缀 "\\?\" 关闭非结束的 "/" 分隔符支持，
		且无视 MAX_PATH 限制。
	\since build 699
	*/
	//@{
	//! \note 使用当前目录。
	DirectorySession();
	explicit YB_NONNULL(2)
	DirectorySession(const char*);
	explicit YB_NONNULL(2)
	DirectorySession(const char16_t*);
	//@}
	/*!
	\post \c !GetNativeHandle() 。
	\since build 560
	*/
	DefDeMoveCtor(DirectorySession)
	/*!
	\brief 析构：关闭目录路径。
	\since build 461
	*/
	DefDeDtor(DirectorySession)

	/*!
	\post \c !GetNativeHandle() 。
	\since build 549
	*/
	DefDeMoveAssignment(DirectorySession)

	//! \since build 413
	DefGetter(const ynothrow, NativeHandle, NativeHandle, dir.get())

	//! \brief 复位目录状态。
	void
	Rewind() ynothrow;
};


/*!
\brief 目录句柄：表示打开的目录和内容迭代状态。
\invariant <tt>!*this || bool(GetNativeHandle())</tt> 。
\since build 411
*/
class YF_API HDirectory final
	: private DirectorySession, private ystdex::deref_self<HDirectory>
{
	//! \since build 556
	friend deref_self<HDirectory>;

public:
	//! \since build 648
	using NativeChar =
#if YCL_Win32
		char16_t
#else
		char
#endif
		;
	//! \since build 713
	using iterator = ystdex::indirect_input_iterator<HDirectory*>;

private:
#if YCL_Win32
	//! \since build 705
	u16string dirent_str;
#else
	/*!
	\brief 节点信息。
	\since build 669
	*/
	tidy_ptr<::dirent> p_dirent{};
#endif

public:
	/*!
	\brief 构造：使用目录路径。
	\since build 699
	*/
	using DirectorySession::DirectorySession;
	//! \since build 669
	DefDeMoveCtor(HDirectory)

	//! \since build 549
	DefDeMoveAssignment(HDirectory)

	/*!
	\brief 间接操作：取自身引用。
	\note 使用 ystdex::indirect_input_iterator 和转换函数访问。
	\since build 556
	*/
	using ystdex::deref_self<HDirectory>::operator*;

	/*!
	\brief 迭代：向后遍历。
	\exception std::system_error 读取目录失败。
	*/
	HDirectory&
	operator++();

	/*!
	\brief 判断文件系统节点无效或有效性。
	\since build 561
	*/
#if YCL_Win32
	YB_PURE DefBoolNeg(YB_PURE explicit, !dirent_str.empty())
#else
	YB_PURE DefBoolNeg(YB_PURE explicit, bool(p_dirent))
#endif

	//! \since build 648
	YB_ATTR_nodiscard YB_PURE
		DefCvt(const ynothrow, basic_string_view<NativeChar>, GetNativeName())
	//! \since build 593
	YB_ATTR_nodiscard YB_PURE
	operator string() const;
	//! \since build 648
	YB_ATTR_nodiscard YB_PURE
	operator u16string() const;

	/*!
	\brief 取节点状态信息确定的文件系统节点类别。
	\return 未迭代文件时为 NodeCategory::Empty ，否则为对应的其它节点类别。
	\note 不同系统支持的可能不同。
	\since build 474
	*/
	YB_ATTR_nodiscard NodeCategory
	GetNodeCategory() const ynothrow;
	/*!
	\brief 间接操作：取节点名称。
	\return 非空结果：子节点不可用时为对应类型的 \c "." ，否则为子节点名称。
	\note 返回的结果在析构和下一次迭代前保持有效。
	\since build 648
	*/
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE const NativeChar*
	GetNativeName() const ynothrow;

	//! \brief 复位。
	using DirectorySession::Rewind;

	//! \since build 713
	//@{
	YB_ATTR_nodiscard YB_PURE PDefH(iterator, begin, )
		ImplRet(this)

	YB_ATTR_nodiscard YB_PURE PDefH(iterator, end, )
		ImplRet(iterator())
	//@}
};

/*!
\relates HDirectory
\sa ystdex::is_undereferenceable
\since build 561
*/
inline PDefH(bool, is_undereferenceable, const HDirectory& i) ynothrow
	ImplRet(!i)


/*!
\pre 断言：第一参数非空。
\note 若在发生其它错误前存储分配失败，设置 errno 为 \c ENOMEM 。
*/
//@{
/*!
\brief 测试路径可访问性。
\param path 路径，意义同 POSIX <tt>::open</tt> 。
\param amode 模式，基本语义同 POSIX.1 2004 ，具体行为取决于实现。 。
\note errno 在出错时会被设置，具体值由实现定义。
\since build 669
*/
//@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) int
uaccess(const char* path, int amode) ynothrowv;
YB_ATTR_nodiscard YF_API YB_NONNULL(1) int
uaccess(const char16_t* path, int amode) ynothrowv;
//@}

/*!
\brief 取当前工作目录复制至指定缓冲区中。
\param size 缓冲区长。
\return 若成功为第一参数，否则为空指针。
\note 基本语义同 POSIX.1 2004 的 \c ::getcwd 。
\note 若存储分配失败，设置 errno 为 \c ENOMEM 。
\note Win32 平台：当且仅当结果为根目录时以分隔符结束。
\note 其它平台：未指定结果是否以分隔符结束。
\since build 699
*/
//@{
//! \param buf UTF-8 缓冲区起始指针。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) char*
ugetcwd(char* buf, size_t size) ynothrowv;
//! \param buf UCS-2 缓冲区起始指针。
YB_ATTR_nodiscard YF_API YB_NONNULL(1) char16_t*
ugetcwd(char16_t* buf, size_t size) ynothrowv;
//@}

/*!
\return 操作是否成功。
\note errno 在出错时会被设置，具体值除以上明确的外，由实现定义。
\note 参数表示路径，使用 UTF-8 编码。
\note DS 使用 newlib 实现。MinGW32 使用 MSVCRT 实现。Android 使用 bionic 实现。
	其它 Linux 使用 GLibC 实现。
*/
//@{
/*!
\brief 切换当前工作路径至指定路径。
\note POSIX 平台：除路径和返回值外语义同 \c ::chdir 。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
uchdir(const char*) ynothrowv;

/*!
\brief 按路径以默认权限新建一个目录。
\note 权限由实现定义： DS 使用最大权限； MinGW32 使用 \c ::_wmkdir 指定的默认权限。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
umkdir(const char*) ynothrowv;

/*!
\brief 按路径删除一个空目录。
\note POSIX 平台：除路径和返回值外语义同 \c ::rmdir 。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
urmdir(const char*) ynothrowv;

/*!
\brief 按路径删除一个非目录文件。
\note POSIX 平台：除路径和返回值外语义同 \c ::unlink 。
\note Win32 平台：支持移除只读文件，但删除打开的文件总是失败。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
uunlink(const char*) ynothrowv;

/*!
\brief 按路径移除一个文件。
\note POSIX 平台：除路径和返回值外语义同 \c ::remove 。移除非空目录总是失败。
\note Win32 平台：支持移除空目录和只读文件，但删除打开的文件总是失败。
\see https://msdn.microsoft.com/library/kc07117k.aspx 。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) bool
uremove(const char*) ynothrowv;
//@}
//@}


//! \since build 713
//@{
/*!
\brief 尝试按指定的起始缓冲区大小取当前工作目录的路径。
\pre 间接断言：参数不等于 0 。
\note 未指定结果是否以分隔符结束。
*/
template<typename _tChar>
YB_ATTR_nodiscard basic_string<_tChar>
FetchCurrentWorkingDirectory(size_t init)
{
	return ystdex::retry_for_vector<basic_string<_tChar>>(init,
		[](basic_string<_tChar>& res, size_t) -> bool{
		const auto r(platform::ugetcwd(&res[0], res.length()));

		if(r)
		{
			res = r;
			return {};
		}

		const int err(errno);

		if(err != ERANGE)
			ystdex::throw_error(err, yfsig);
		return true;
	});
}
#if YCL_Win32
//! \note 参数被忽略。
//@{
template<>
YB_ATTR_nodiscard YF_API string
FetchCurrentWorkingDirectory(size_t);
template<>
YB_ATTR_nodiscard YF_API u16string
FetchCurrentWorkingDirectory(size_t);
//@}
#endif
//@}


/*!
\brief 文件节点标识类型。
\warning 非虚析构。
\since build 935
*/
//@{
struct YB_API FileNodeID
#if YCL_Win32
	: pair<std::uint32_t, std::uint64_t>,
#else
	: pair<std::uint64_t, std::uint64_t>,
#endif
	ystdex::equality_comparable<FileNodeID>
{
#if YCL_Win32
	using BaseType = pair<std::uint32_t, std::uint64_t>;
#else
	using BaseType = pair<std::uint64_t, std::uint64_t>;
#endif

	DefDeCtor(FileNodeID)
	using BaseType::BaseType;
	FileNodeID(const BaseType& x)
		: BaseType(x)
	{}

	YB_ATTR_nodiscard YB_PURE yconstfn friend
		PDefHOp(bool, ==, const FileNodeID& x, const FileNodeID& y) ynothrow
		ImplRet(x.first == y.first && x.second == y.second)
};
//@}

/*!
\brief 取文件系统节点标识。
\since build 638
*/
YB_ATTR_nodiscard FileNodeID
GetFileNodeIDOf(const FileDescriptor&) ynothrow;

/*!
\brief 判断参数是否表示共享的文件节点。
\note 可能设置 errno 。
\since build 638
*/
//@{
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefH(bool, IsNodeShared, const FileNodeID& x, const FileNodeID& y) ynothrow
	ImplRet(x != FileNodeID() && x == y)
/*!
\pre 间接断言：字符串参数非空。
\note 最后参数表示跟踪连接。
\since build 660
*/
//@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) bool
IsNodeShared(const char*, const char*, bool = true) ynothrowv;
YB_ATTR_nodiscard YF_API YB_NONNULL(1, 2) bool
IsNodeShared(const char16_t*, const char16_t*, bool = true) ynothrowv;
//@}
/*!
\note 取节点失败视为不共享。
\sa FileDescriptor::GetNodeID
\since build 846
*/
YB_ATTR_nodiscard YF_API bool
IsNodeShared(FileDescriptor, FileDescriptor) ynothrow;
//@}


//! \since build 628
using FileTime = std::chrono::nanoseconds;

/*!
\return 以 POSIX 时间相同历元的时间间隔。
\note 当前 Windows 使用 \c ::GetFileTime 实现，其它只保证最高精确到秒。
\exception std::system_error 参数无效或文件时间查询失败。
*/
//@{
/*!
\brief 取访问时间。
\since build 631
*/
//@{
//! \since build 901
YB_ATTR_nodiscard YF_API FileTime
GetFileAccessTimeOf(const FileDescriptor&);
YB_ATTR_nodiscard YF_API YB_NONNULL(1) FileTime
GetFileAccessTimeOf(std::FILE*);
/*!
\pre 断言：第一参数非空。
\note 最后参数表示跟随连接：若文件系统支持，访问链接的文件而不是链接自身。
*/
//@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) FileTime
GetFileAccessTimeOf(const char*, bool = {});
YB_ATTR_nodiscard YF_API YB_NONNULL(1) FileTime
GetFileAccessTimeOf(const char16_t*, bool = {});
//@}
//@}

/*!
\brief 取文件修改时间。
\since build 628
*/
//@{
//! \since build 901
YB_ATTR_nodiscard YF_API FileTime
GetFileModificationTimeOf(const FileDescriptor&);
YB_ATTR_nodiscard YF_API YB_NONNULL(1) FileTime
GetFileModificationTimeOf(std::FILE*);

/*!
\pre 断言：第一参数非空。
\note 最后参数表示跟随连接：若文件系统支持，访问链接的文件而不是链接自身。
*/
//@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) FileTime
GetFileModificationTimeOf(const char*, bool = {});
YB_ATTR_nodiscard YF_API YB_NONNULL(1) FileTime
GetFileModificationTimeOf(const char16_t*, bool = {});
//@}
//@}

/*!
\brief 取修改和访问时间。
\since build 631
*/
//@{
//! \since build 901
YB_ATTR_nodiscard YF_API array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const FileDescriptor&);
YB_ATTR_nodiscard YF_API YB_NONNULL(1) array<FileTime, 2>
GetFileModificationAndAccessTimeOf(std::FILE*);
/*!
\pre 断言：第一参数非空。
\note 最后参数表示跟随连接：若文件系统支持，访问链接的文件而不是链接自身。
*/
//@{
YB_ATTR_nodiscard YF_API YB_NONNULL(1) array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const char*, bool = {});
YB_ATTR_nodiscard YF_API YB_NONNULL(1) array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const char16_t*, bool = {});
//@}
//@}
//@}

/*!
\throw std::system_error 设置失败。
\note DS 平台：不支持操作。
\since build 901
*/
//@{
/*!
\brief 设置访问时间。
\throw std::system_error 设置失败。
*/
YF_API void
SetFileAccessTimeOf(const FileDescriptor&, FileTime);
//! \note Win32 平台：要求打开的文件具有写权限。
//@{
//! \brief 设置修改时间。
YF_API void
SetFileModificationTimeOf(const FileDescriptor&, FileTime);
//! \brief 设置修改和访问时间。
YF_API void
SetFileModificationAndAccessTimeOf(const FileDescriptor&, array<FileTime, 2>);
//@}
//@}


/*!
\brief 已知文件系统类型。
\since build 607
*/
enum class FileSystemType
{
	Unknown,
	FAT12,
	FAT16,
	FAT32
};


/*!
\brief 文件分配表接口。
\since build 607
*/
namespace FAT
{

using EntryDataUnit = byte;
using ClusterIndex = std::uint32_t;
//! \since build 900
using ClusterCount = std::uint32_t;

//! \since build 844
static_assert(ystdex::is_trivially_replaceable<EntryDataUnit, std::uint8_t>(),
	"Invalid type found.");

//! \since build 608
//@{
//! \see Microsoft FAT Specification Section 3.1 。
//@{
//! \brief BIOS 参数块偏移量。
enum BPB : size_t
{
	BS_jmpBoot = 0,
	BS_OEMName = 3,
	BPB_BytsPerSec = 11,
	BPB_SecPerClus = 13,
	BPB_RsvdSecCnt = 14,
	BPB_NumFATs = 16,
	BPB_RootEntCnt = 17,
	BPB_TotSec16 = 19,
	BPB_Media = 21,
	BPB_FATSz16 = 22,
	BPB_SecPerTrk = 24,
	BPB_NumHeads = 26,
	BPB_HiddSec = 28,
	BPB_TotSec32 = 32
};

/*!
\brief BIOS 参数块偏移量域 BPB_BytsPerSec 的最值。
\since build 609
*/
yconstexpr const size_t MinSectorSize(512), MaxSectorSize(4096);
//@}

//! \brief FAT16 接口（ FAT12 共享实现）。
inline namespace FAT16
{

/*!
\brief FAT12 和 FAT16 扩展 BIOS 参数块偏移量。
\see Microsoft FAT Specification Section 3.2 。
*/
enum BPB : size_t
{
	BS_DrvNum = 36,
	BS_Reserved1 = 37,
	BS_BootSig = 38,
	BS_VolID = 39,
	BS_VolLab = 43,
	BS_FilSysType = 54,
	_reserved_zero_448 = 62,
	Signature_word = 510,
	_reserved_remained = 512
};

} // inline namespace FAT16;

//! \brief FAT32 接口。
inline namespace FAT32
{

/*!
\brief FAT32 扩展 BIOS 参数块偏移量。
\see Microsoft FAT Specification Section 3.3 。
*/
enum BPB : size_t
{
	BPB_FATSz32 = 36,
	BPB_ExtFlags = 40,
	BPB_FSVer = 42,
	BPB_RootClus = 44,
	BPB_FSInfo = 48,
	BPB_BkBootSec = 50,
	BPB_Reserved = 52,
	BS_DrvNum = 64,
	BS_Reserved1 = 65,
	BS_BootSig = 66,
	BS_VolID = 67,
	BS_VolLab = 71,
	BS_FilSysType = 82,
	_reserved_zero_420 = 90
};

} // inline namespace FAT32;

/*!
\see Microsoft FAT Specification Section 3.2 。
\since build 900
*/
//@{
//! \brief FAT 签名：扩展 BPB 中的 BS_FilSysType 公共前缀。
yconstexpr const char FATSignature[]{'F', 'A', 'T'};

//! \brief 判断参数是否匹配 FAT 签名。
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline
PDefH(bool, MatchFATSignature, const void* p)
	ImplRet(std::memcmp(p, FATSignature, sizeof(FATSignature)) == 0)

/*!
\brief 判断参数指定的扇区内容是否可构成合法的主引导记录。
\note 匹配 BS_jmpBoot 和 FAT 签名。
\sa MatchFATSignature
\see Microsoft FAT Specification Section 3.1 。
*/
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE bool
CheckValidMBR(const byte*);
//@}

/*!
\brief 卷标数据类型。
\see Microsoft FAT Specification Section 3.2 。
\since build 610
*/
using VolumeLabel = array<byte, 11>;

//! \see Microsoft FAT Specification Section 5 。
//@{
//! \brief 文件系统信息块偏移量。
enum FSI : size_t
{
	FSI_LeadSig = 0,
	FSI_Reserved1 = 4,
	FSI_StrucSig = 484,
	FSI_Free_Count = 488,
	FSI_Nxt_Free = 492,
	FSI_Reserved2 = 496,
	FSI_TrailSig = 508
};

yconstexpr const std::uint32_t FSI_LeadSig_Value(0x41615252),
	FSI_StrucSig_Value(0x61417272), FSI_TrailSig_Value(0xAA550000);
//@}
//@}

/*!
\brief 文件属性。
\see Microsoft FAT specification Section 6 。
*/
enum class Attribute : octet
{
	ReadOnly = 0x01,
	Hidden = 0x02,
	System = 0x04,
	VolumeID = 0x08,
	Directory = 0x10,
	Archive = 0x20,
	LongFileName = ReadOnly | Hidden | System | VolumeID
};

//! \relates Attribute
DefBitmaskEnum(Attribute)


//! \brief 簇接口。
namespace Clusters
{

// !\see Microsoft FAT Specification Section 3.2 。
//@{
yconstexpr const size_t PerFAT12(4085);
yconstexpr const size_t PerFAT16(65525);
//@}

enum : ClusterIndex
{
	FAT16RootDirectory = 0,
	/*!
	\since build 609
	\see Microsoft FAT Specification Section 4 。
	*/
	//@{
	MaxValid12 = 0xFF6,
	MaxValid16 = 0xFFF6,
	MaxValid32 = 0xFFFFFF6,
	Bad12 = 0xFF7,
	Bad16 = 0xFFF7,
	Bad32 = 0xFFFFFF7,
	EndOfFile12 = 0xFFF,
	EndOfFile16 = 0xFFFF,
	EndOfFile32 = 0xFFFFFFFF,
	//@}
	EndOfFile = 0x0FFFFFFF,
	First = 0x00000002,
	Root = 0x00000000,
	Free = 0x00000000,
	Error = 0xFFFFFFFF
};

//! \since build 642
inline PDefH(bool, IsFreeOrEOF, ClusterIndex c) ynothrow
	ImplRet(c == Clusters::Free || c == Clusters::EndOfFile)

/*!
\brief 未知自由簇数。
\see Microsoft FAT Specification Section 5 。
\since build 900
*/
static yconstexpr const ClusterCount UnknownFreeCount(0xFFFFFFFF);

} // namespace Clusters;


//! \brief 非法别名字符。
const char IllegalAliasCharacters[]{"\\/:;*?\"<>|&+,=[] "};


//! \brief 文件大小：字节数。
using FileSize = std::uint32_t;

/*!
\brief 最大文件大小。
\note 等于 4GiB - 1B 。
*/
static yconstexpr const auto MaxFileSize(FileSize(0xFFFFFFFF));


//! \brief 时间戳：表示日期和时间的整数类型。
using Timestamp = std::uint16_t;

//! \brief 转换日期和时间的时间戳为标准库时间类型。
YB_ATTR_nodiscard YF_API std::time_t
ConvertFileTime(Timestamp, Timestamp) ynothrow;

//! \brief 取以实时时钟的文件日期和时间。
YB_ATTR_nodiscard YF_API pair<Timestamp, Timestamp>
FetchDateTime() ynothrow;


/*!
\brief 长文件名接口。
\note 仅适用于 ASCII 兼容字符集。
\see Microsoft FAT specification Section 7 。
*/
namespace LFN
{

//! \brief 长文件名目录项偏移量。
enum Offsets
{
	//! \brief 长文件名序数。
	Ordinal = 0x00,
	Char0 = 0x01,
	Char1 = 0x03,
	Char2 = 0x05,
	Char3 = 0x07,
	Char4 = 0x09,
	//! \note 值等于 Attribute::LongFilename 。
	Flag = 0x0B,
	//! \note 保留值 0x00 。
	Reserved1 = 0x0C,
	//! \brief 短文件名（别名）校验和。
	CheckSum = 0x0D,
	Char5 = 0x0E,
	Char6 = 0x10,
	Char7 = 0x12,
	Char8 = 0x14,
	Char9 = 0x16,
	Char10 = 0x18,
	//! \note 保留值 0x0000 。
	Reserved2 = 0x1A,
	Char11 = 0x1C,
	Char12 = 0x1E
};

//! \brief 组成长文件名中的 UCS-2 字符在项中的偏移量表。
yconstexpr const size_t OffsetTable[]{0x01, 0x03, 0x05, 0x07, 0x09, 0x0E,
	0x10, 0x12, 0x14, 0x16, 0x18, 0x1C, 0x1E};

enum : size_t
{
	//! \brief UCS-2 项最大长度。
#if YCL_DS
	MaxLength = 255,
#else
	MaxLength = 256,
#endif
	//! \brief UTF-8 项最大长度。
#if YCL_DS
	MaxMBCSLength = 255,
#else
	MaxMBCSLength = MaxLength * 3,
#endif
	EntryLength = size(OffsetTable),
	AliasEntryLength = 11,
	MaxAliasMainPartLength = 8,
	MaxAliasExtensionLength = 3,
	MaxAliasLength = MaxAliasMainPartLength + MaxAliasExtensionLength + 2,
	MaxNumericTail = 999999
};

enum EntryValues : octet
{
	//! \brief WinNT 小写文件名。
	CaseLowerBasename = 0x08,
	//! \brief WinNT 小写扩展名。
	CaseLowerExtension = 0x10,
	//! \brief Ordinal 中标记结束的项。
	LastLongEntry = 0x40
};

//! \brief 非法长文件名字符。
const char IllegalCharacters[]{"\\/:*?\"<>|"};

//! \since build 608
//@{
/*!
\brief 转换长文件名为别名。
\pre 参数指定的字符串不含空字符。
\return 主文件名、扩展名和指定是否为有损转换标识。
\note 返回的文件名长度分别不大于 MaxAliasMainPartLength
	和 MaxAliasExtensionLength 。
*/
YB_ATTR_nodiscard YF_API tuple<string, string, bool>
ConvertToAlias(const u16string&);

//! \brief 按指定序数取长文件名偏移。
YB_ATTR_nodiscard inline PDefH(size_t, FetchLongNameOffset, EntryDataUnit ord)
	ynothrow
	ImplRet((size_t(std::uint8_t(ord) & ~LastLongEntry) - 1U) * EntryLength)
//@}

/*!
\brief 转换 UCS-2 路径字符串为多字节字符串。
\since build 610
*/
YB_ATTR_nodiscard YF_API string
ConvertToMBCS(const char16_t* path);

/*!
\brief 生成别名校验和。
\pre 断言：参数非空。
\see Microsoft FAT specification Section 7.2 。
*/
YB_ATTR_nodiscard YF_API YB_NONNULL(1) YB_PURE EntryDataUnit
GenerateAliasChecksum(const EntryDataUnit*) ynothrowv;

/*!
\brief 检查名称是否为合法的可被 UCS-2 表示的非控制字符组成。
\pre 断言：字符串参数的数据指针非空。
\since build 657
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
ValidateName(string_view) ynothrowv;

/*!
\brief 在指定字符串写前缀字符 '~' 和整数的后缀以作为短文件名。
\pre 字符串长度不小于 MaxAliasMainPartLength 且可保存前缀字符和第二参数转换的数值。
\since build 608
*/
YF_API void
WriteNumericTail(string&, size_t) ynothrowv;

} // namespace LFN;

//! \since build 609
//@{
//! \brief 目录项数据大小。
yconstexpr const size_t EntryDataSize(0x20);

/*!
\brief 目录项数据。
\note 默认构造不初始化。
*/
class YF_API EntryData final : private array<EntryDataUnit, EntryDataSize>
{
public:
	using Base = array<EntryDataUnit, EntryDataSize>;
	/*!
	\brief 目录项偏移量。
	\see Microsoft FAT specification Section 6 。
	*/
	enum Offsets : size_t
	{
		Name = 0x00,
		Extension = 0x08,
		Attributes = 0x0B,
		//! \note 项 DIR_NTRes 保留，指定值为 0 但扩展为表示大小写。
		CaseInfo = 0x0C,
		CTimeTenth = 0x0D,
		CTime = 0x0E,
		CDate = 0x10,
		ADate = 0x12,
		ClusterHigh = 0x14,
		MTime = 0x16,
		MDate = 0x18,
		Cluster = 0x1A,
		FileSize = 0x1C
	};
	//! \see Microsoft FAT specification Section 6.1 。
	enum : octet
	{
		Last = 0x00,
		Free = 0xE5,
	};

	using Base::operator[];
	using Base::data;

	DefPred(const ynothrow, Directory,
		bool(Attribute((*this)[Attributes]) & Attribute::Directory))
	DefPred(const ynothrow, LongFileName,
		Attribute((*this)[Attributes]) == Attribute::LongFileName)
	DefPred(const ynothrow, Volume, bool(Attribute((*this)[Attributes])
		& Attribute::VolumeID))
	DefPred(const ynothrow, Writable,
		!bool(Attribute((*this)[Attributes]) & Attribute::ReadOnly))

	PDefH(void, SetDirectoryAttribute, ) ynothrow
		ImplExpr((*this)[Attributes] = EntryDataUnit(Attribute::Directory))
	PDefH(void, SetDot, size_t n) ynothrowv
		ImplExpr(YAssert(n < EntryDataSize, "Invalid argument found."),
			(*this)[n] = EntryDataUnit('.'))

	PDefH(void, Clear, ) ynothrow
		ImplExpr(ystdex::trivially_fill_n(static_cast<Base*>(this)))

	PDefH(void, ClearAlias, ) ynothrow
		ImplExpr(ystdex::trivially_fill_n(data(), LFN::AliasEntryLength,
			EntryDataUnit(' ')))

	/*!
	\brief 复制长文件名列表项数据到参数指定的缓冲区的对应位置。
	\pre 断言：参数非空。
	*/
	void
	CopyLFN(char16_t*) const ynothrowv;

	PDefH(void, FillLast, ) ynothrow
		ImplExpr(ystdex::trivially_fill_n(static_cast<Base*>(this), 1,
			EntryDataUnit(Last)))

	/*!
	\brief 为添加的项填充名称数据，按需生成短名称后缀。
	\pre 断言：字符串参数的数据指针非空。
	\pre 断言：第二参数非空。
	\return 别名校验值（若不存在别名则为 0 ）和项的大小。
	\note 第二参数是校验别名项存在性的例程，其中字符串的数据指针保证非空。
	\sa LFN::GenerateAliasChecksum
	\sa LFN::WriteNumericTail
	\since build 860
	*/
	pair<EntryDataUnit, size_t>
	FillNewName(string_view, ystdex::function<bool(string_view)>);

	/*!
	\pre 间接断言：参数的数据指针非空。
	\since build 656
	*/
	YB_ATTR_nodiscard bool
	FindAlias(string_view) const;

	YB_ATTR_nodiscard string
	GenerateAlias() const;

	PDefH(std::uint32_t, ReadFileSize, ) ynothrow
		ImplRet(ystdex::read_uint_le<32>(data() + FileSize))

	void
	SetupRoot(ClusterIndex) ynothrow;

	void
	WriteAlias(const string&) ynothrow;

	void
	WriteCDateTime() ynothrow;

	void
	WriteCluster(ClusterIndex) ynothrow;

	void
	WriteDateTime() ynothrow;
};
//@}


/*!
\brief 检查参数指定的 MS-DOS 风格路径冒号。
\pre 间接断言：参数非空。
\exception std::system_error 检查失败。
	\li std::errc::invalid_argument 路径有超过一个冒号。
\since build 611
*/
YF_API YB_NONNULL(1) YB_PURE const char*
CheckColons(const char*);

} // namespace FAT;

} // namespace platform;

namespace platform_ex
{

} // namespace platform_ex;

#endif

