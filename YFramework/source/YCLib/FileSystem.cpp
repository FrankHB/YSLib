/*
	© 2011-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r5148
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:41:35 +0800
\par 修改时间:
	2023-03-30 15:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_FileSystem // for basic_string, ystdex::allocate_unique,
//	std::is_integral, std::chrono::system_clock, CallNothrow, YAssertNonnull,
//	wstring, make_observer, make_unique, errno, ERANGE, ystdex::ntctscpy,
//	ystdex::retry_on_cond, std::tm, std::is_same, std::min, std::accumulate,
//	ystdex::read_uint_le, std::bind, std::ref, ystdex::write_uint_le;
#include YFM_YCLib_FileIO // for platform_ex::MakePathStringW,
//	platform_Ex::MakePathStringU, MakePathString, Deref, ystdex::throw_error,
//	std::invalid_argument, std::errc::function_not_supported, YCL_CallF_CAPI,
//	CategorizeNode, complete FileDescriptor, ystdex::ntctslen, std::wctob,
//	std::towupper, std::wint_t, ystdex::restrict_length, ystdex::ntctsicmp,
//	std::errc::invalid_argument, std::strchr;
#include YFM_YCLib_NativeAPI // for ::dev_t, ::ino_t, Mode, struct ::stat,
//	::stat, ::GetFileAttributesW, platform_ex::cstat, platform_ex::estat,
//	::futimens, ::linkat, ::symlink, ::lstat, ::readlink,
//	::GetCurrentDirectoryW, ::WideCharToMultiByte;
#include <ystdex/ctime.h> // for std::time_t, ystdex::is_date_range_valid,
//	ystdex::is_time_no_leap_valid;
#include "CHRLib/YModules.h"
#include YFM_CHRLib_CharacterProcessing // for CHRLib::MakeUCS2LE;
#if YCL_Win32
#	include YFM_Win32_YCLib_MinGW32 // for YCL_DeclW32Call, YCL_CallF_Win32,
//	platform_ex::FileAttributes, platform_ex::GetErrnoFromWin32,
//	platform_ex::Invalid, platform_ex::ConvertTime, platform_ex::ToHandle,
//	platform_ex::QueryFileNodeID, platform_ex::QueryFileLinks,
//	platform_ex::QueryFileTime, platform_ex::SetFileTime,
//	platform_ex::ResolveReparsePoint, platform_ex::DirectoryFindData;
#	include <time.h> // for ::localtime_s;

//! \since build 651
namespace
{

namespace YCL_Impl_details
{

// NOTE: See $2023-03 @ %Documentation::Workflow.
YCL_DeclW32Call(CreateSymbolicLinkW, kernel32, unsigned char, const wchar_t*, \
	const wchar_t*, unsigned long)
using platform::wcast;

// NOTE: As %SYMBOLIC_LINK_FLAG_DIRECTORY, but with correct type.
yconstexpr const auto SymbolicLinkFlagDirectory(1UL);

inline PDefH(void, W32_CreateSymbolicLink, const char16_t* dst,
	const char16_t* src, unsigned long flags)
	ImplExpr(
		YCL_CallF_Win32(CreateSymbolicLinkW, wcast(dst), wcast(src), flags))

} // namespace YCL_Impl_details;

} // unnamed namespace;

//! \since build 706
using platform_ex::MakePathStringW;
//! \since build 549
using platform_ex::DirectoryFindData;
#else
#	include <dirent.h>
#	include <ystdex/scope_guard.hpp> // for ystdex::swap_guard;
#	include <time.h> // for ::localtime_r;

//! \since build 706
using platform_ex::MakePathStringU;
#endif

//! \since build 475
using namespace CHRLib;

namespace platform
{

namespace
{

#if !YCL_Win32
//! \since build 710
using errno_guard = ystdex::swap_guard<int, void, decltype(errno)&>;
#endif

//! \since build 708
#if !YCL_DS
template<typename _tChar>
bool
IterateLinkImpl(basic_string<_tChar>& path, size_t& n)
{
	if(!path.empty())
		try
		{
			// TODO: Throw with context information about failed path?
			// TODO: Use preallocted object to improve performance?
			path = ReadLink(path.c_str(), path.get_allocator());
			if(n != 0)
				--n;
			else
				// XXX: Enumerator
				//	%std::errc::too_many_symbolic_link_levels is
				//	commented out in MinGW-w64 configuration of
				//	libstdc++. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71444.
				ystdex::throw_error(ELOOP, "ReadLink @ platform::IteratorLink");
			return true;
		}
		CatchIgnore(std::invalid_argument&)
	return {};
}
#endif

#if YCL_Win32
//! \since build 639
//!@{
using platform_ex::FileAttributes;
using platform_ex::GetErrnoFromWin32;

YB_NONNULL(1) bool
UnlinkWithAttr(const wchar_t* path, FileAttributes attr) ynothrow
{
	return !bool(attr & FileAttributes::ReadOnly) || ::SetFileAttributesW(path,
		static_cast<unsigned long>(attr & ~FileAttributes::ReadOnly))
		? ::_wunlink(path) == 0 : (errno = GetErrnoFromWin32(), false);
}

template<typename _func>
YB_NONNULL(2) bool
CallFuncWithAttr(_func f, const char* path)
{
	const auto& wpath(MakePathStringW(path));
	const auto& wstr(wpath.c_str());
	const auto attr(FileAttributes(::GetFileAttributesW(wstr)));

	return attr != FileAttributes::Invalid ? f(wstr, attr)
		: (errno = GetErrnoFromWin32(), false);
}
//!@}
#endif

#if YCL_Win32
//! \since build 632
using platform_ex::ConvertTime;
//! \since build 704
using platform_ex::ToHandle;
//! \since build 632
using platform_ex::QueryFileNodeID;
//! \since build 639
using platform_ex::QueryFileLinks;

//! \since build 632
using platform_ex::QueryFileTime;
//! \since build 660
void
QueryFileTime(const char* path, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime, bool follow_reparse_point)
{
	QueryFileTime(MakePathStringW(path).c_str(), p_ctime, p_atime, p_mtime,
		follow_reparse_point);
}
//! \since build 632
//!@{
void
QueryFileTime(int fd, ::FILETIME* p_ctime, ::FILETIME* p_atime,
	::FILETIME* p_mtime)
{
	QueryFileTime(ToHandle(fd), p_ctime, p_atime, p_mtime);
}

// TODO: Blocked. Use C++14 generic lambda expressions.
yconstexpr const struct
{
	template<typename _tParam, typename... _tParams>
	FileTime
	operator()(_tParam arg, _tParams... args) const
	{
		::FILETIME atime{};

		QueryFileTime(arg, {}, &atime, {}, args...);
		return ConvertTime(atime);
	}
} get_st_atime{};
yconstexpr const struct
{
	template<typename _tParam, typename... _tParams>
	FileTime
	operator()(_tParam arg, _tParams... args) const
	{
		::FILETIME mtime{};

		QueryFileTime(arg, {}, {}, &mtime, args...);
		return ConvertTime(mtime);
	}
} get_st_mtime{};
yconstexpr const struct
{
	template<typename _tParam, typename... _tParams>
	array<FileTime, 2>
	operator()(_tParam arg, _tParams... args) const
	{
		::FILETIME mtime{}, atime{};

		QueryFileTime(arg, {}, &atime, &mtime, args...);
		return array<FileTime, 2>{ConvertTime(mtime), ConvertTime(atime)};
	}
} get_st_matime{};
//!@}
#else
//! \since build 901
using platform_ex::cstat;
//! \since build 719
YB_NONNULL(2, 4) inline PDefH(void, cstat, struct ::stat& st,
	const char16_t* path, bool follow_link, const char* sig)
	ImplRet(cstat(st, MakePathString(path).c_str(), follow_link, sig))

//! \since build 901
using platform_ex::estat;

//! \since build 638
//!@{
static_assert(std::is_integral<::dev_t>(),
	"Nonconforming '::dev_t' type found.");
static_assert(std::is_unsigned<::ino_t>(),
	"Nonconforming '::ino_t' type found.");

inline PDefH(FileNodeID, get_file_node_id, struct ::stat& st) ynothrow
	ImplRet({std::uint64_t(st.st_dev), std::uint64_t(st.st_ino)})
//!@}

inline PDefH(::timespec, ToTimeSpec, FileTime ft) ynothrow
	ImplRet({std::time_t(ft.count() / 1000000000LL),
		long(ft.count() % 1000000000LL)})

//! \since build 719
void
SetFileTime(int fd, const ::timespec(&times)[2])
{
#if YCL_DS
	// XXX: Hack.
#ifndef UTIME_OMIT
#	define UTIME_OMIT (-1L)
#endif
	yunused(fd), yunused(times);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#else
	// XXX: This should at least work in WSL since Windows 10 1809. See
	//	https://github.com/Microsoft/WSL/issues/1399.
	YCL_CallF_CAPI(, ::futimens, fd, times);
#endif
}

//! \since build 902
FileTime
ToFileTime(std::time_t t) ynothrow
{
	// NOTE: Sicne ISO C++20, std::chrono::system_clock is specified to measure
	//	UNIX time. This is still true before ISO C++20 for supported platforms
	//	other than %YCL_Win32 here, which makes the system time directly usable
	//	as the filesystem time. Nevertheless, this is not relied on, except the
	//	conversion here.
	// TODO: Use ISO C++20 %std::chrono::file_time equivalence.
	return std::chrono::system_clock::from_time_t(t).time_since_epoch();
}

//! \since build 631
//!@{
const auto get_st_atime([](struct ::stat& st){
	return ToFileTime(st.st_atime);
});
const auto get_st_mtime([](struct ::stat& st){
	return ToFileTime(st.st_mtime);
});
const auto get_st_matime([](struct ::stat& st){
	return array<FileTime, 2>{ToFileTime(st.st_mtime), ToFileTime(st.st_atime)};
});
//!@}
#endif

//! \since build 660
bool
IsNodeShared_Impl(const char* a, const char* b, bool follow_link) ynothrow
{
#if YCL_Win32
	return CallNothrow({}, [=]{
		return QueryFileNodeID(MakePathStringW(a).c_str(), follow_link)
			== QueryFileNodeID(MakePathStringW(b).c_str(), follow_link);
	});
#else
	struct ::stat st;

	if(estat(st, a, follow_link) != 0)
		return {};

	const auto id(get_file_node_id(st));

	if(estat(st, b, follow_link) != 0)
		return {};

	return IsNodeShared(id, get_file_node_id(st));
#endif
}
//! \since build 660
bool
IsNodeShared_Impl(const char16_t* a, const char16_t* b, bool follow_link)
	ynothrow
{
#if YCL_Win32
	return CallNothrow({}, [=]{
		return QueryFileNodeID(wcast(a), follow_link)
			== QueryFileNodeID(wcast(b), follow_link);
	});
#else
	return IsNodeShared_Impl(MakePathString(a).c_str(),
		MakePathString(b).c_str(), follow_link);
#endif
}

//! \since build 632
template<typename _func, typename... _tParams>
auto
FetchFileTime(_func f, _tParams... args)
#if YCL_Win32
	-> ystdex::invoke_result_t<_func, _tParams&...>
#else
	-> ystdex::invoke_result_t<_func, struct ::stat&>
#endif
{
#if YCL_Win32
	// NOTE: The %::FILETIME has resolution of 100 nanoseconds.
	// XXX: Error handling for indirect calls.
	return f(args...);
#else
	// TODO: Get more precise time count.
	struct ::stat st;

	cstat(st, args..., yfsig);
	return ystdex::invoke(f, st);
#endif
}

} // unnamed namespace;

bool
IsDirectory(const char* path)
{
#if YCL_Win32
	return IsDirectory(ucast(MakePathStringW(path).c_str()));
#else
	// TODO: Merge? See %platform_ex::estat in %NativeAPI.
	struct ::stat st;

	return ::stat(path, &st) == 0 && bool(Mode(st.st_mode) & Mode::Directory);
#endif
}
bool
IsDirectory(const char16_t* path)
{
#if YCL_Win32
	// TODO: Simplify?
	const auto attr(FileAttributes(::GetFileAttributesW(wcast(path))));

	return attr != FileAttributes::Invalid
		&& bool(FileAttributes(attr) & FileAttributes::Directory);
#else
	return IsDirectory(MakePathString(path).c_str());
#endif
}


size_t
FetchNumberOfLinks(const FileDescriptor& fd)
{
#if YCL_Win32
	return QueryFileLinks(ToHandle(*fd));
#else
	struct ::stat st;
	static_assert(std::is_unsigned<decltype(st.st_nlink)>(),
		"Unsupported 'st_nlink' type found.");

	cstat(st, *fd, yfsig);
	return size_t(st.st_nlink);
#endif
}
size_t
FetchNumberOfLinks(const char* path, bool follow_link)
{
#if YCL_Win32
	return QueryFileLinks(MakePathStringW(path).c_str(), follow_link);
#else
	struct ::stat st;
	static_assert(std::is_unsigned<decltype(st.st_nlink)>(),
		"Unsupported 'st_nlink' type found.");

	cstat(st, path, follow_link, yfsig);
	return size_t(st.st_nlink);
#endif
}
size_t
FetchNumberOfLinks(const char16_t* path, bool follow_link)
{
#if YCL_Win32
	return QueryFileLinks(wcast(path), follow_link);
#else
	return FetchNumberOfLinks(MakePathString(path).c_str(), follow_link);
#endif
}

void
CreateHardLink(const char* dst, const char* src)
{
#if YCL_DS
	YAssertNonnull(dst), YAssertNonnull(src);
	yunused(dst), yunused(src);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#elif YCL_Win32
	CreateHardLink(ucast(MakePathStringW(dst).c_str()),
		ucast(MakePathStringW(src).c_str()));
#else
	// NOTE: POSIX %::link is implementation-defined to following symbolic
	//	link target.
	YCL_CallF_CAPI(, ::linkat, AT_FDCWD, Nonnull(src), AT_FDCWD, Nonnull(dst),
		AT_SYMLINK_FOLLOW);
#endif
}
void
CreateHardLink(const char16_t* dst, const char16_t* src)
{
#if YCL_DS
	YAssertNonnull(dst), YAssertNonnull(src);
	yunused(dst), yunused(src);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#elif YCL_Win32
	YCL_CallF_Win32(CreateHardLinkW, wcast(dst), wcast(src), {});
#else
	// TODO: To make the behavior specific and same as on platform %Win32, use
	//	%::realpath on platform %Linux, etc.
	CreateHardLink(MakePathString(dst).c_str(), MakePathString(src).c_str());
#endif
}

void
CreateSymbolicLink(const char* dst, const char* src, bool is_dir)
{
#if YCL_DS
	yunused(dst), yunused(src), yunused(is_dir);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#elif YCL_Win32
	using namespace platform_ex;

	CreateSymbolicLink(ucast(MakePathStringW(dst).c_str()),
		ucast(MakePathStringW(src).c_str()), is_dir);
#else
	yunused(is_dir);
	YCL_CallF_CAPI(, ::symlink, Nonnull(src), Nonnull(dst));
#endif
}
void
CreateSymbolicLink(const char16_t* dst, const char16_t* src, bool is_dir)
{
#if YCL_DS
	yunused(dst), yunused(src), yunused(is_dir);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#elif YCL_Win32
	YCL_Impl_details::W32_CreateSymbolicLink(dst, src,
		is_dir ? YCL_Impl_details::SymbolicLinkFlagDirectory : 0UL);
#else
	CreateSymbolicLink(MakePathString(dst).c_str(), MakePathString(src).c_str(),
		is_dir);
#endif
}

string
ReadLink(const char* path, string::allocator_type a)
{
#if YCL_DS
	YAssertNonnull(path);
	yunused(path), yunused(a);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#elif YCL_Win32
	// TODO: Simplify?
	return MakePathString(
		ReadLink(ucast(MakePathStringW(path, a).c_str())).c_str(), a);
#else
	struct ::stat st;

	YCL_CallF_CAPI(, ::lstat, Nonnull(path), &st);
	if((Mode(st.st_mode) & Mode::Link) == Mode::Link)
	{
		auto n(st.st_size);

		// NOTE: Some file systems like procfs on Linux are not conforming to
		//	POSIX, thus %st.st_size is not always reliable. In most cases, it is
		//	0. Only 0 is currently supported.
		if(n >= 0)
		{
			// FIXME: Blocked. TOCTTOU access. The race between %::lstat and
			//	%::readlink is not fixable solely by '*at' functions of
			//	POSIX.1-2008.
			// TODO: Evaluate whether platform-specific extensions can be used
			//	here. Linux 2.6.39 introduces %O_PATH for %::open. This is
			//	possible the race with %::fstatfs (since Linux 3.12) and
			//	%::readlinkat by specifying 'O_PATH | O_NOFOLLOW'. See https://man7.org/linux/man-pages/man2/open.2.html.
			if(n == 0)
				// TODO: Use %::pathconf to determine initial length instead of
				//	a magic number.
				n = yimpl(1024);
			return ystdex::retry_for_vector<string>(size_t(n), a,
				[&](string& res, size_t s) -> bool{
				errno_guard gd(errno, 0);
				const auto r(::readlink(path, &res[0], size_t(n)));

				if(r < 0)
				{
					const int err(errno);

					if(err == EINVAL)
						throw std::invalid_argument("Failed reading link:"
							" Specified file is not a link.");
					YCL_RaiseZ_SysE(, err, "::readlink", yfsig);
					throw std::runtime_error(
						"Failed reading link: Unknown error.");
				}
				if(size_t(r) <= s)
				{
					res.resize(size_t(r));
					return {};
				}
				return true;
			});
		}
		throw std::invalid_argument("Invalid link size found.");
	}
	throw std::invalid_argument("Specified file is not a link.");
#endif
}
u16string
ReadLink(const char16_t* path, u16string::allocator_type a)
{
#if YCL_DS
	YAssertNonnull(path);
	yunused(path), yunused(a);
	ystdex::throw_error(std::errc::function_not_supported, yfsig);
#elif YCL_Win32
	using namespace platform_ex;
	const auto sv(ResolveReparsePoint(wcast(path), ReparsePointData().Get()));

	return u16string(sv.cbegin(), sv.cend(), a);
#else
	// XXX: Not using allocator for the temporary string creation for
	//	efficiency.
	return
		MakePathStringU(ReadLink(MakePathString(path).c_str(), a).c_str(), a);
#endif
}

#if !YCL_DS
bool
IterateLink(string& path, size_t& n)
{
	return IterateLinkImpl(path, n);
}
bool
IterateLink(u16string& path, size_t& n)
{
	return IterateLinkImpl(path, n);
}
#endif


#if YCL_Win32
class DirectorySession::Data final : public DirectoryFindData
{
public:
	using DirectoryFindData::DirectoryFindData;
};
#else
//! \since build 680
namespace
{

PDefH(::DIR*, ToDirPtr, DirectorySession::NativeHandle p)
	ImplRet(static_cast<::DIR*>(p))

} // unnamed namespace;
#endif
void
DirectorySession::Deleter::operator()(pointer p) const ynothrowv
{
#if YCL_Win32
	static_cast<const ystdex::allocator_delete<default_allocator<Data>>&>(*this)
		(p);
#else
	const auto res(::closedir(ToDirPtr(p)));

	YAssert(res == 0, "No valid directory found.");
	yunused(res);
#endif
}


DirectorySession::DirectorySession(basic_string<NativeChar>::allocator_type a)
#if YCL_Win32
	// XXX: This is optimal compared use 'u"."' directly by less copying.
	: DirectorySession(L".", a)
#else
	: DirectorySession(".", a)
#endif
{}
DirectorySession::DirectorySession(const char* path, string::allocator_type a)
#if YCL_Win32
	: DirectorySession(MakePathStringW(path, a))
#else
	: sDirPath([&] YB_LAMBDA_ANNOTATE((const char* p), , nonnull(2)){
		const auto res(Deref(p) != char()
			? ystdex::rtrim(string(p, a), FetchSeparator<char>())
			: string(".", a));

		YAssert(res.empty() || EndsWithNonSeperator(res),
			"Invalid directory name state found.");
		return res + FetchSeparator<char>();
	}(path)),
	dir(::opendir(sDirPath.c_str()))
#endif
{
#if !YCL_Win32
	if(!dir)
		ystdex::throw_error(errno, yfsig);
#endif
}
DirectorySession::DirectorySession(const char16_t* path,
	u16string::allocator_type a)
#if YCL_Win32
	// XXX: The path would be copied anyway by the constructor of %Data to
	//	prevent violation of strict aliasing.
	: dir(platform::allocate_unique<Data>(a, u16string_view(path), a))
#else
	: DirectorySession(MakePathString(path, a))
#endif
{}
#if YCL_Win32
DirectorySession::DirectorySession(wstring&& str)
	: dir(platform::allocate_unique<Data>(str.get_allocator(), std::move(str)))
{}
#endif

void
DirectorySession::Rewind() ynothrow
{
	YAssert(dir, "Invalid native handle found.");
#if YCL_Win32
	Deref(dir.get()).Rewind();
#else
	::rewinddir(ToDirPtr(dir.get()));
#endif
}


HDirectory&
HDirectory::operator++()
{
#if YCL_Win32
	auto& find_data(*static_cast<DirectoryFindData*>(GetNativeHandle()));

	if(find_data.Read())
	{
		const wstring_view sv(find_data.GetEntryName());

		dirent_str = u16string(sv.cbegin(), sv.cend());
		YAssert(!dirent_str.empty(), "Invariant violation found.");
	}
	else
		dirent_str.clear();
#else
	YAssert(!p_dirent || bool(GetNativeHandle()), "Invariant violation found.");

	const errno_guard gd(errno, 0);

	if(const auto p = ::readdir(ToDirPtr(GetNativeHandle())))
		p_dirent = make_observer(p);
	else
	{
		const int err(errno);

		if(err == 0)
			p_dirent = {};
		else
			ystdex::throw_error(errno, yfsig);
	}
#endif
	return *this;
}

NodeCategory
HDirectory::GetNodeCategory() const ynothrow
{
	if(*this)
	{
		YAssert(GetNativeHandle(), "Invariant violation found.");

#if YCL_Win32
		const auto res(Deref(static_cast<platform_ex::DirectoryFindData*>(
			GetNativeHandle())).GetNodeCategory());
#else
		using Descriptions::Warning;
		auto res(NodeCategory::Empty);

		try
		{
			auto name(sDirPath + Deref(p_dirent).d_name);
			struct ::stat st;

#	if YCL_DS
#		define YCL_Impl_lstatn ::stat
#	else
#		define YCL_Impl_lstatn ::lstat
#	endif
			// TODO: Simplify.
			// XXX: Value of %errno might be overwritten.
			if(YCL_TraceCallF_CAPI(YCL_Impl_lstatn, name.c_str(), &st) == 0)
				res |= CategorizeNode(st.st_mode);
#	if !YCL_DS
			if(bool(res & NodeCategory::Link)
				&& YCL_TraceCallF_CAPI(::stat, name.c_str(), &st) == 0)
				res |= CategorizeNode(st.st_mode);
#	endif
#	undef YCL_Impl_lstatn
		}
		CatchExpr(std::exception& e, YTraceDe(Warning, "Failed getting node "
			"category (errno = %d) @ %s: %s.", errno, yfsig, e.what()))
		CatchExpr(...,
			YTraceDe(Warning, "Unknown error @ %s.", yfsig))
#endif
		return res != NodeCategory::Empty ? res : NodeCategory::Invalid;
	}
	return NodeCategory::Empty;
}

HDirectory::operator std::string() const
{
	return
#if YCL_Win32
		MakePathString(GetNativeName());
#else
		GetNativeName();
#endif
}

HDirectory::operator std::u16string() const
{
	return
#if YCL_Win32
		GetNativeName();
#else
		MakePathStringU(GetNativeName());
#endif
}

const HDirectory::NativeChar*
HDirectory::GetNativeName() const ynothrow
{
	return *this ?
#if YCL_Win32
		dirent_str.data() : u".";
#else
		p_dirent->d_name : ".";
#endif
}

string
HDirectory::ConvertToMBCS(string::allocator_type a) const
{
	return
#if YCL_Win32
		MakePathString(GetNativeName(), a);
#else
		string(GetNativeName(), a);
#endif
}

u16string
HDirectory::ConvertToUCS2(u16string::allocator_type a) const
{
	return
#if YCL_Win32
		u16string(GetNativeName(), a);
#else
		MakePathStringU(GetNativeName(), a);
#endif
}


int
uaccess(const char* path, int amode) ynothrowv
{
	YAssertNonnull(path);
#if YCL_Win32
	return CallNothrow(-1, [=]{
		return ::_waccess(MakePathStringW(path).c_str(), amode);
	});
#else
	return ::access(path, amode);
#endif
}
int
uaccess(const char16_t* path, int amode) ynothrowv
{
	YAssertNonnull(path);
#if YCL_Win32
	return ::_waccess(wcast(path), amode);
#else
	return CallNothrow(-1, [=]{
		return ::access(MakePathString(path).c_str(), amode);
	});
#endif
}

char*
ugetcwd(char* buf, size_t size) ynothrowv
{
	YAssertNonnull(buf);
	if(size != 0)
	{
		using namespace std;

#if YCL_Win32
		return CallNothrow({}, [=]() -> char*{
			if(YB_LIKELY(size > 0))
			{
				const auto p(make_unique<char16_t[]>(size));
				const auto s(wcast(p.get()));
				// XXX: Truncated.
				const auto
					n(::GetCurrentDirectoryW(unsigned(size), s));

				if(n != 0)
				{
#if true
					const int r_l(::WideCharToMultiByte(CP_UTF8, 0,
						s, -1, {}, 0, {}, {}));

					if(YB_LIKELY(r_l >= 0) && size >= size_t(r_l))
					{
						// XXX: This cannot be false in a sane implemenation.
						if(YB_LIKELY(r_l != 0))
							::WideCharToMultiByte(CP_UTF8, 0, s, -1, buf,
								r_l, {}, {});
						return buf;
					}
					else
						errno = ERANGE;
#else
					// NOTE: Any optimized implemenations shall be equivalent to
					//	this.
					const auto res(MakePathString(p.get()));
					const auto len(res.length());

					if(size < len + 1)
						errno = ERANGE;
					else
						return ystdex::ntctscpy(buf, res.data(), len);
#endif
				}
				else
					errno = GetErrnoFromWin32();
			}
			else
				errno = EINVAL;
			return {};
		});
#else
		// NOTE: POSIX.1 2004 has no guarantee about slashes. POSIX.1 2013
		//	mandates there are no redundant slashes. See http://pubs.opengroup.org/onlinepubs/009695399/functions/getcwd.html.
		//	At least platform %DS (implemented in devkitPro newlib's libsysbase)
		//	may have trailing slashes or not, depending on the last successful
		//	%::chdir call.
		return ::getcwd(buf, size);
#endif
	}
	else
		errno = EINVAL;
	return {};
}
char16_t*
ugetcwd(char16_t* buf, size_t len) ynothrowv
{
	YAssertNonnull(buf);
	if(len != 0)
	{
		using namespace std;

#if YCL_Win32
		// NOTE: Win32 guarantees there will be a separator if and only if when
		//	the result is root directory for ::_wgetcwd, and actually it is
		//	the same in ::%GetCurrentDirectoryW.
		// XXX: Truncated.
		const auto n(::GetCurrentDirectoryW(unsigned(len), wcast(buf)));

		if(n != 0)
			return buf;
		errno = GetErrnoFromWin32();
		return {};
#else
		// XXX: Alias by %char array is safe.
		if(const auto cwd
			= ::getcwd(ystdex::aligned_store_cast<char*>(buf), len))
			return CallNothrow({}, [=]() -> char16_t*{
				const auto res(platform_ex::MakePathStringU(cwd));
				const auto rlen(res.length());

				if(len >= rlen + 1)
					return ystdex::ntctscpy(buf, res.data(), rlen);
				errno = ERANGE;
				return {};
			});
#endif
	}
	else
		errno = EINVAL;
	return {};
}

#define YCL_Impl_FileSystem_ufunc_1(_n) \
bool \
_n(const char* path) ynothrowv \
{ \
	YAssertNonnull(path); \

#if YCL_Win32
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	return CallNothrow({}, [&]{ \
		return _wfn(MakePathStringW(path).c_str()) == 0; \
	}); \
}
#else
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	return _fn(path) == 0; \
}
#endif

#define YCL_Impl_FileSystem_ufunc(_n, _fn, _wfn) \
	YCL_Impl_FileSystem_ufunc_1(_n) \
	YCL_Impl_FileSystem_ufunc_2(_fn, _wfn)

YCL_Impl_FileSystem_ufunc(uchdir, ::chdir, ::_wchdir)

YCL_Impl_FileSystem_ufunc_1(umkdir)
#if YCL_Win32
	YCL_Impl_FileSystem_ufunc_2(_unused_, ::_wmkdir)
#else
	return ::mkdir(path, mode_t(Mode::Access)) == 0;
}
#endif

YCL_Impl_FileSystem_ufunc(urmdir, ::rmdir, ::_wrmdir)

YCL_Impl_FileSystem_ufunc_1(uunlink)
#if YCL_Win32
	return CallNothrow({}, [=]{
		return CallFuncWithAttr(UnlinkWithAttr, path);
	});
}
#else
YCL_Impl_FileSystem_ufunc_2(::unlink, )
#endif

YCL_Impl_FileSystem_ufunc_1(uremove)
#if YCL_Win32
	// NOTE: %::_wremove is same to %::_wunlink on Win32 which cannot delete
	//	empty directories.
	return CallNothrow({}, [=]{
		return CallFuncWithAttr([] YB_LAMBDA_ANNOTATE(
			(const wchar_t* wstr, FileAttributes attr), ynothrow, nonnull(2)){
			return bool(attr & FileAttributes::Directory) ? ::_wrmdir(wstr) == 0
				: UnlinkWithAttr(wstr, attr);
		}, path);
	});
}
#else
YCL_Impl_FileSystem_ufunc_2(std::remove, )
#endif

#undef YCL_Impl_FileSystem_ufunc_1
#undef YCL_Impl_FileSystem_ufunc_2
#undef YCL_Impl_FileSystem_ufunc


#if YCL_Win32
template<>
YF_API string
FetchCurrentWorkingDirectory(size_t init, string::allocator_type a)
{
	return MakePathString(FetchCurrentWorkingDirectory<char16_t>(init,
		a).c_str(), a);
}
template<>
YF_API u16string
FetchCurrentWorkingDirectory(size_t, u16string::allocator_type a)
{
	u16string res(a);
	unsigned long len, rlen(0);

	// NOTE: Retry is necessary to prevent the failure due to modification of
	//	the current working directory from other threads.
	ystdex::retry_on_cond([&]() -> bool{
		if(rlen < len)
		{
			res.pop_back();
			return {};
		}
		if(rlen != 0)
			return true;
		YCL_Raise_Win32E("GetCurrentDirectoryW", yfsig);
	}, [&]{
		if((len = ::GetCurrentDirectoryW(0, {})) != 0)
		{
			res.resize(size_t(len + 1));
			rlen = ::GetCurrentDirectoryW(len, wcast(&res[0]));
		}
	});
	res.resize(rlen);
	return res;
}
#endif


FileNodeID
GetFileNodeIDOf(const FileDescriptor& fd) ynothrow
{
#if YCL_Win32
	return CallNothrow({}, [=]{
		return QueryFileNodeID(ToHandle(*fd));
	});
#else
	struct ::stat st;

	return estat(st, *fd) == 0 ? get_file_node_id(st) : FileNodeID();
#endif
}

bool
IsNodeShared(const char* a, const char* b, bool follow_link) ynothrowv
{
	return a == b || ystdex::ntctscmp(Nonnull(a), Nonnull(b)) == 0
		|| IsNodeShared_Impl(a, b, follow_link);
}
bool
IsNodeShared(const char16_t* a, const char16_t* b, bool follow_link) ynothrowv
{
	return a == b || ystdex::ntctscmp(Nonnull(a), Nonnull(b)) == 0
		|| IsNodeShared_Impl(a, b, follow_link);
}
bool
IsNodeShared(FileDescriptor x, FileDescriptor y) ynothrow
{
	const auto id(GetFileNodeIDOf(x));

	return id != FileNodeID() && id == GetFileNodeIDOf(y);
}


FileTime
GetFileAccessTimeOf(const FileDescriptor& fd)
{
	return FetchFileTime(get_st_atime, *fd);
}
FileTime
GetFileAccessTimeOf(std::FILE* fp)
{
	return GetFileAccessTimeOf(FileDescriptor(fp));
}
FileTime
GetFileAccessTimeOf(const char* filename, bool follow_link)
{
	return FetchFileTime(get_st_atime, filename, follow_link);
}
FileTime
GetFileAccessTimeOf(const char16_t* filename, bool follow_link)
{
	return FetchFileTime(get_st_atime, wcast(filename), follow_link);
}

FileTime
GetFileModificationTimeOf(const FileDescriptor& fd)
{
	return FetchFileTime(get_st_mtime, *fd);
}
FileTime
GetFileModificationTimeOf(std::FILE* fp)
{
	return GetFileModificationTimeOf(FileDescriptor(fp));
}
FileTime
GetFileModificationTimeOf(const char* filename, bool follow_link)
{
	return FetchFileTime(get_st_mtime, filename, follow_link);
}
FileTime
GetFileModificationTimeOf(const char16_t* filename, bool follow_link)
{
	return FetchFileTime(get_st_mtime, wcast(filename), follow_link);
}

array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const FileDescriptor& fd)
{
	return FetchFileTime(get_st_matime, *fd);
}
array<FileTime, 2>
GetFileModificationAndAccessTimeOf(std::FILE* fp)
{
	return GetFileModificationAndAccessTimeOf(FileDescriptor(fp));
}
array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const char* filename, bool follow_link)
{
	return FetchFileTime(get_st_matime, filename, follow_link);
}
array<FileTime, 2>
GetFileModificationAndAccessTimeOf(const char16_t* filename, bool follow_link)
{
	return FetchFileTime(get_st_matime, wcast(filename), follow_link);
}

void
SetFileAccessTimeOf(const FileDescriptor& fd, FileTime ft)
{
#if YCL_Win32
	auto atime(ConvertTime(ft));

	platform_ex::SetFileTime(ToHandle(*fd), {}, &atime, {});
#else
	const ::timespec times[]{ToTimeSpec(ft), {yimpl(0), UTIME_OMIT}};

	SetFileTime(*fd, times);
#endif
}
void
SetFileModificationTimeOf(const FileDescriptor& fd, FileTime ft)
{
#if YCL_Win32
	auto mtime(ConvertTime(ft));

	platform_ex::SetFileTime(ToHandle(*fd), {}, {}, &mtime);
#else
	const ::timespec times[]{{yimpl(0), UTIME_OMIT}, ToTimeSpec(ft)};

	SetFileTime(*fd, times);
#endif
}
void
SetFileModificationAndAccessTimeOf(const FileDescriptor& fd,
	array<FileTime, 2> fts)
{
#if YCL_Win32
	auto atime(ConvertTime(fts[0])), mtime(ConvertTime(fts[1]));

	platform_ex::SetFileTime(ToHandle(*fd), {}, &atime, &mtime);
#else
	const ::timespec times[]{ToTimeSpec(fts[0]), ToTimeSpec(fts[1])};

	SetFileTime(*fd, times);
#endif
}


namespace FAT
{

bool
CheckValidMBR(const byte* sec_buf)
{
	return ((sec_buf[BS_jmpBoot + 0] == byte(0xEB) && sec_buf[BS_jmpBoot + 2]
		== byte(0x90)) || sec_buf[BS_jmpBoot + 0] == byte(0xE9))
		&& (MatchFATSignature(sec_buf + FAT16::BS_FilSysType)
		|| MatchFATSignature(sec_buf + FAT32::BS_FilSysType));
}


std::time_t
ConvertFileTime(Timestamp d, Timestamp t) ynothrow
{
	struct std::tm time_parts;

	yunseq(
	time_parts.tm_hour = t >> 11,
	time_parts.tm_min = (t >> 5) & 0x3F,
	time_parts.tm_sec = (t & 0x1F) << 1,
	time_parts.tm_mday = d & 0x1F,
	time_parts.tm_mon = ((d >> 5) & 0x0F) - 1,
	time_parts.tm_year = (d >> 9) + 80,
	time_parts.tm_isdst = 0
	);
	return std::mktime(&time_parts);
}

pair<Timestamp, Timestamp>
FetchDateTime() ynothrow
{
	struct std::tm tmp;
	std::time_t epoch;

	if(std::time(&epoch) != std::time_t(-1))
	{
#if YCL_Win32
		// NOTE: The return type and parameter order differs than ISO C11
		//	library extension.
		::localtime_s(&tmp, &epoch);
#else
		::localtime_r(&epoch, &tmp);
		// FIXME: For platforms without %::(localtime_r, localtime_s).
#endif
		// NOTE: Microsoft FAT base year is 1980.
		return {ystdex::is_date_range_valid(tmp) ? ((tmp.tm_year - 80) & 0x7F)
			<< 9 | ((tmp.tm_mon + 1) & 0xF) << 5 | (tmp.tm_mday & 0x1F) : 0,
			ystdex::is_time_no_leap_valid(tmp) ? (tmp.tm_hour & 0x1F) << 11
			| (tmp.tm_min & 0x3F) << 5 | ((tmp.tm_sec >> 1) & 0x1F) : 0};
	}
	return {0, 0};
}

namespace LFN
{

tuple<string, string, bool>
ConvertToAlias(const u16string& long_name)
{
	YAssert(ystdex::ntctslen(long_name.c_str()) == long_name.length(),
		"Invalid long filename found.");

	string alias;
	// NOTE: Strip leading periods.
	size_t offset(long_name.find_first_not_of(u'.'));
	// NOTE: Set when the alias had to be modified to be valid.
	bool lossy(offset != 0);
	const auto check_char([&](string& str, char16_t uc){
		int bc(std::wctob(std::towupper(std::wint_t(uc))));

		if(!lossy && std::wctob(std::wint_t(uc)) != bc)
			lossy = true;
		if(bc == ' ')
		{
			// NOTE: Skip spaces in filename.
			lossy = true;
			return;
		}
		// TODO: Optimize.
		if(bc == EOF || string(IllegalAliasCharacters).find(char(bc))
			!= string::npos)
			// NOTE: Replace unconvertible or illegal characters with
			//	underscores. See Microsoft FAT specification Section 7.4.
			yunseq(bc = '_', lossy = true);
		str += char(bc);
	});
	const auto len(long_name.length());

	for(; alias.length() < MaxAliasMainPartLength && long_name[offset] != u'.'
		&& offset != len; ++offset)
		check_char(alias, long_name[offset]);
	if(!lossy && long_name[offset] != u'.' && long_name[offset] != char16_t())
		// NOTE: More than 8 characters in name.
		lossy = true;

	auto ext_pos(long_name.rfind(u'.'));
	string ext;

	if(!lossy && ext_pos != u16string::npos && long_name.rfind(u'.', ext_pos)
		!= u16string::npos)
		// NOTE: More than one period in name.
		lossy = true;
	if(ext_pos != u16string::npos && ext_pos + 1 != len)
	{
		++ext_pos;
		for(size_t ext_len(0); ext_len < LFN::MaxAliasExtensionLength
			&& ext_pos != len; yunseq(++ext_len, ++ext_pos))
			check_char(ext, long_name[ext_pos]);
		if(ext_pos != len)
			// NOTE: More than 3 characters in extension.
			lossy = true;
	}
	return make_tuple(std::move(alias), std::move(ext), lossy);
}

string
ConvertToMBCS(const char16_t* path)
{
	// TODO: Optimize?
	return ystdex::restrict_length(MakePathString(std::u16string(path,
		std::min<size_t>(ystdex::ntctslen(path), MaxLength)).c_str(), {}),
		MaxMBCSLength);
}

EntryDataUnit
GenerateAliasChecksum(const EntryDataUnit* p) ynothrowv
{
	static_assert(std::is_same<EntryDataUnit, byte>(),
		"Only unsigned char as byte is supported by checksum generation.");

	YAssertNonnull(p);
	// NOTE: The operation is an unsigned char rotate right.
	return std::accumulate(p, p + AliasEntryLength, EntryDataUnit(),
		[](EntryDataUnit v, EntryDataUnit b) ynothrow{
			return EntryDataUnit(((octet(v) & 1) != 0 ? 0x80 : 0)
				+ (octet(v) >> 1) + octet(b));
		});
}

bool
ValidateName(string_view name) ynothrowv
{
	YAssertNonnull(name.data());
	return std::all_of(ystdex::begin(name), ystdex::end(name),
		[](char c) ynothrow{
		// TODO: Use interval arithmetic.
		return c >= 0x20 && static_cast<unsigned char>(c) < 0xF0;
	});
}

void
WriteNumericTail(string& alias, size_t k) ynothrowv
{
	YAssert(!(MaxAliasMainPartLength < alias.length()), "Invalid alias found.");

	auto p(&alias[MaxAliasMainPartLength - 1]);

	while(k > 0)
	{
		YAssert(p != &alias[0], "Value is too large to store in the alias.");
		*p-- = '0' + k % 10;
		k /= 10;
	}
	*p = '~';
}

} // namespace LFN;

void
EntryData::CopyLFN(char16_t* str) const ynothrowv
{
	const auto pos(LFN::FetchLongNameOffset((*this)[LFN::Ordinal]));

	YAssertNonnull(str);
	for(size_t i(0); i < LFN::EntryLength; ++i)
		if(pos + i < LFN::MaxLength - 1)
			str[pos + i]
				= ystdex::read_uint_le<16>(data() + LFN::OffsetTable[i]);
}

pair<EntryDataUnit, size_t>
EntryData::FillNewName(string_view name,
	ystdex::function<bool(string_view)> verify)
{
	YAssertNonnull(name.data()),
	YAssertNonnull(verify);

	EntryDataUnit alias_check_sum{};
	size_t entry_size;

	ClearAlias();
	if(name == ".")
	{
		SetDot(0),
		entry_size = 1;
	}
	else if(name == "..")
	{
		SetDot(0),
		SetDot(1),
		entry_size = 1;
	}
	else
	{
		const auto& long_name(CHRLib::MakeUCS2LE<u16string>(name));
		const auto len(long_name.length());

		if(len < LFN::MaxLength)
		{
			auto alias_tp(LFN::ConvertToAlias(long_name));
			auto& pri(get<0>(alias_tp));
			const auto& ext(get<1>(alias_tp));
			auto alias(pri);
			const auto check(std::bind(verify, std::ref(alias)));

			if(!ext.empty())
				alias += '.' + ext;
			if((get<2>(alias_tp) ? alias.length() : 0) == 0)
				entry_size = 1;
			else
			{
				entry_size
					= (len + LFN::EntryLength - 1) / LFN::EntryLength + 1;
				if(ystdex::ntctsicmp(alias.c_str(), name.data(),
					LFN::MaxAliasLength) != 0 || check())
				{
					size_t i(1);

					pri.resize(LFN::MaxAliasMainPartLength, '_');
					alias = pri + '.' + ext;
					ystdex::retry_on_cond(check, [&]{
						if(YB_UNLIKELY(LFN::MaxNumericTail < i))
							ystdex::throw_error(std::errc::invalid_argument,
								yfsig);
						LFN::WriteNumericTail(alias, i++);
					});
				}
			}
			WriteAlias(alias);
		}
		else
			ystdex::throw_error(std::errc::invalid_argument, yfsig);
		alias_check_sum = LFN::GenerateAliasChecksum(data());
	}
	return {alias_check_sum, entry_size};
}

bool
EntryData::FindAlias(string_view name) const
{
	const auto alias(GenerateAlias());

	return ystdex::ntctsicmp(Nonnull(name.data()), alias.c_str(),
		std::min<size_t>(name.length(), alias.length())) == 0;
}

string
EntryData::GenerateAlias() const
{
	if((*this)[0] != EntryDataUnit(Free))
	{
		if((*this)[0] == EntryDataUnit('.'))
			return (*this)[1] == EntryDataUnit('.') ? ".." : ".";

		// NOTE: Copy the base filename.
		bool
			case_info((octet((*this)[CaseInfo]) & LFN::CaseLowerBasename) != 0);
		const auto conv([&](size_t i){
			const auto c((*this)[i]);

			return char(case_info ? EntryDataUnit(std::tolower(int(c))) : c);
		});
		string res;

		res.reserve(LFN::MaxAliasLength - 1);
		for(size_t i(0); i < LFN::MaxAliasMainPartLength
			&& (*this)[Name + i] != EntryDataUnit(' '); ++i)
			res += conv(Name + i);
		if((*this)[Extension] != EntryDataUnit(' '))
		{
			res += '.';
			case_info
				= (octet((*this)[CaseInfo]) & LFN::CaseLowerExtension) != 0;
			for(size_t i(0); i < LFN::MaxAliasExtensionLength
				&& (*this)[Extension + i] != EntryDataUnit(' '); ++i)
				res += conv(Extension + i);
		}
		return res;
	}
	return {};
}

void
EntryData::SetupRoot(ClusterIndex root_cluster) ynothrow
{
	Clear();
	ClearAlias(),
	SetDot(Name),
	SetDirectoryAttribute();
	WriteCluster(root_cluster);
}

void
EntryData::WriteCluster(ClusterIndex c) ynothrow
{
	using ystdex::write_uint_le;

	write_uint_le<16>(data() + Cluster, c),
	write_uint_le<16>(data() + ClusterHigh, c >> 16);
}

void
EntryData::WriteAlias(const string& alias) ynothrow
{
	size_t i(0), j(0);

	for(; j < LFN::MaxAliasMainPartLength && alias[i] != '.'
		&& alias[i] != char(); yunseq(++i, ++j))
		(*this)[j] = EntryDataUnit(alias[i]);
	while(j < LFN::MaxAliasMainPartLength)
	{
		(*this)[j] = EntryDataUnit(' ');
		++j;
	}
	if(alias[i] == '.')
		for(++i; alias[i] != char() && j < LFN::AliasEntryLength;
			yunseq(++i, ++j))
			(*this)[j] = EntryDataUnit(alias[i]);
	for(; j < LFN::AliasEntryLength; ++j)
		(*this)[j] = EntryDataUnit(' ');
}

void
EntryData::WriteCDateTime() ynothrow
{
	using ystdex::write_uint_le;
	const auto& date_time(FetchDateTime());

	write_uint_le<16>(data() + CTime, date_time.second),
	write_uint_le<16>(data() + CDate, date_time.first);
}

void
EntryData::WriteDateTime() ynothrow
{
	using ystdex::write_uint_le;
	using ystdex::unseq_apply;
	const auto date_time(FetchDateTime());
	const auto dst(data());

	unseq_apply([&](size_t offset){
		write_uint_le<16>(dst + offset, date_time.first);
	}, CDate, MDate, ADate),
	unseq_apply([&](size_t offset){
		write_uint_le<16>(dst + offset, date_time.second);
	}, CTime, MTime);
}

const char*
CheckColons(const char* path)
{
	if(const auto p_col = std::strchr(Nonnull(path), ':'))
	{
		path = p_col + 1;
		if(std::strchr(path, ':'))
			ystdex::throw_error(std::errc::invalid_argument, yfsig);
	}
	return path;
}

} // namespace FAT;

} // namespace platform;

namespace platform_ex
{

} // namespace platform_ex;

