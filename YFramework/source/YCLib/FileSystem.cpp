/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r4148
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:41:35 +0800
\par 修改时间:
	2016-07-13 14:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_FileSystem // for basic_string, ystdex::pun_storage_t,
//	default_delete, make_observer, std::min, std::accumulate, std::tm,
//	ystdex::read_uint_le, YAssertNonnull, ystdex::write_uint_le, std::bind,
//	std::ref, ystdex::retry_on_cond;
#include YFM_YCLib_FileIO // for platform_ex::MakePathStringW,
//	platform_Ex::MakePathStringU, MakePathString, Deref, ystdex::to_array,
//	ystdex::throw_error, std::errc::function_not_supported,
//	ThrowFileOperationFailure, CategorizeNode, ystdex::ntctslen, std::wctob,
//	std::towupper, ystdex::restrict_length, std::min, ystdex::ntctsicmp,
//	std::errc::invalid_argument, std::strchr;
#include YFM_YCLib_NativeAPI // for Mode, struct ::stat, ::lstat;
#include "CHRLib/YModules.h"
#include YFM_CHRLib_CharacterProcessing // for CHRLib::MakeUCS2LE;
#include <ystdex/ctime.h> // for ystdex::is_date_range_valid,
//	ystdex::is_time_no_leap_valid;
#if YCL_Win32
#	include YFM_Win32_YCLib_MinGW32 // for platform_ex::Invalid,
//	platform_ex::ResolveReparsePoint, platform_ex::DirectoryFindData;
#	include <system_error> // for std::system_error;
#	include <exception> // for std::throw_with_nested;
#	include <time.h> // for ::localtime_s;

//! \since build 651
namespace
{

namespace YCL_Impl_details
{

// NOTE: To avoid hiding of global name, the declarations shall not be under
//	namespace %platform.
YCL_DeclW32Call(CreateSymbolicLinkW, kernel32, unsigned char, const wchar_t*, \
	const wchar_t*, unsigned long)
using platform::wcast;

// NOTE: As %SYMBOLIC_LINK_FLAG_DIRECTORY, but with correct type.
yconstexpr const auto SymbolicLinkFlagDirectory(1UL);

inline PDefH(void, W32_CreateSymbolicLink, const char16_t* dst,
	const char16_t* src, unsigned long flags)
	ImplExpr(YCL_CallWin32F(CreateSymbolicLinkW, wcast(dst), wcast(src), flags))

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
			path = ReadLink(path.c_str());
			if(n != 0)
				--n;
			else
				// XXX: Enumerator
				//	%std::errc::too_many_symbolic_link_levels is
				//	commented out in MinGW-w64 configuration of
				//	libstdc++. See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71444.
				ystdex::throw_error(ELOOP);
			return true;
		}
		CatchIgnore(std::invalid_argument&)
	return {};
}
#endif

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
	const auto attr(::GetFileAttributesW(wcast(path)));

	return attr != platform_ex::Invalid
		&& (platform_ex::FileAttributes(attr) & platform_ex::Directory);
#else
	return IsDirectory(MakePathString(path).c_str());
#endif
}

void
CreateHardLink(const char* dst, const char* src)
{
#if YCL_DS
	YAssertNonnull(dst), YAssertNonnull(src);
	ystdex::throw_error(std::errc::function_not_supported);
#elif YCL_Win32
	CreateHardLink(ucast(MakePathStringW(dst).c_str()),
		ucast(MakePathStringW(src).c_str()));
#else
	// NOTE: POSIX %::link is implementation-defined to following symbolic
	//	link target.
	if(::linkat(AT_FDCWD, Nonnull(src), AT_FDCWD, Nonnull(dst),
		AT_SYMLINK_FOLLOW) != 0)
		ystdex::throw_error(errno, "::linkat");
#endif
}
void
CreateHardLink(const char16_t* dst, const char16_t* src)
{
#if YCL_DS
	YAssertNonnull(dst), YAssertNonnull(src);
	ystdex::throw_error(std::errc::function_not_supported);
#elif YCL_Win32
	YCL_CallWin32F(CreateHardLinkW, wcast(dst), wcast(src), {});
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
	ystdex::throw_error(std::errc::function_not_supported);
#elif YCL_Win32
	using namespace platform_ex;

	CreateSymbolicLink(ucast(MakePathStringW(dst).c_str()),
		ucast(MakePathStringW(src).c_str()), is_dir);
#else
	yunused(is_dir);
	if(::symlink(Nonnull(src), Nonnull(dst)) != 0)
		ystdex::throw_error(errno, "::symlink");
#endif
}
void
CreateSymbolicLink(const char16_t* dst, const char16_t* src, bool is_dir)
{
#if YCL_DS
	yunused(dst), yunused(src), yunused(is_dir);
	ystdex::throw_error(std::errc::function_not_supported);
#elif YCL_Win32
	YCL_Impl_details::W32_CreateSymbolicLink(dst, src,
		is_dir ? YCL_Impl_details::SymbolicLinkFlagDirectory : 0UL);
#else
	CreateSymbolicLink(MakePathString(dst).c_str(), MakePathString(src).c_str(),
		is_dir);
#endif
}

string
ReadLink(const char* path)
{
#if YCL_DS
	YAssertNonnull(path);
	ystdex::throw_error(std::errc::function_not_supported);
#elif YCL_Win32
	// TODO: Simplify?
	return
		MakePathString(ReadLink(ucast(MakePathStringW(path).c_str())).c_str());
#else
	struct ::stat st;

	if(::lstat(Nonnull(path), &st) == 0)
	{
		if((Mode(st.st_mode) & Mode::Link) == Mode::Link)
		{
			auto n(st.st_size);

			// NOTE: Some file systems like procfs on Linux are not
			//	conforming to POSIX, thus %st.st_size is not always reliable. In
			//	most cases, it is 0. Only 0 is currently supported.
			if(n >= 0)
			{
				// FIXME: Blocked. TOCTTOU access.
				if(n == 0)
					// TODO: Use %::pathconf to determine initial length instead
					//	of magic number.
					n = yimpl(1024);
				return ystdex::retry_for_vector<string>(n,
					[&](string& res, size_t s) -> bool{
					errno_guard gd(errno, 0);
					const auto r(::readlink(path, &res[0], size_t(n)));

					if(r < 0)
					{
						const int err(errno);

						switch(err)
						{
						case EINVAL:
							throw std::invalid_argument("Failed reading link:"
								" Specified file is not a link.");
						case 0:
							throw std::runtime_error(
								"Unknown error @ ::readlink.");
						default:
							ystdex::throw_error(err, "::readlink");
						}
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
	}
	ystdex::throw_error(errno, "::lstat");
#endif
}
u16string
ReadLink(const char16_t* path)
{
#if YCL_DS
	YAssertNonnull(path);
	ystdex::throw_error(std::errc::function_not_supported);
#elif YCL_Win32
	using namespace platform_ex;
	const auto sv(ResolveReparsePoint(wcast(path), ReparsePointData().Get()));

	return {sv.cbegin(), sv.cend()};
#else
	return MakePathStringU(ReadLink(MakePathString(path).c_str()));
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
	default_delete<Data>::operator()(p);
#else
	const auto res(::closedir(ToDirPtr(p)));

	YAssert(res == 0, "No valid directory found.");
	yunused(res);
#endif
}


DirectorySession::DirectorySession()
#if YCL_Win32
	: DirectorySession(u".")
#else
	: DirectorySession(".")
#endif
{}
DirectorySession::DirectorySession(const char* path)
#if YCL_Win32
	: dir(new Data(MakePathStringW(path)))
#else
	: sDirPath([](const char* p) YB_NONNULL(1){
		const auto res(Deref(p) != char()
			? ystdex::rtrim(string(p), FetchSeparator<char>()) : ".");

		YAssert(res.empty() || EndsWithNonSeperator(res),
			"Invalid directory name state found.");
		return res + FetchSeparator<char>();
	}(path)),
	dir(::opendir(sDirPath.c_str()))
#endif
{
#if !YCL_Win32
	if(!dir)
		ThrowFileOperationFailure("Opening directory failed.");
#endif
}
DirectorySession::DirectorySession(const char16_t* path)
#if YCL_Win32
	: dir(new Data(path))
#else
	: DirectorySession(MakePathString(path).c_str())
#endif
{}

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

	try
	{
		if(find_data.Read())
		{
			const wstring_view sv(find_data.GetEntryName());

			dirent_str = u16string(sv.cbegin(), sv.cend());
			YAssert(!dirent_str.empty(), "Invariant violation found.");
		}
		else
			dirent_str.clear();
	}
	CatchExpr(std::system_error& e, std::throw_with_nested(
		FileOperationFailure(e.code(), "Failed iterating directory.")))
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
			ThrowFileOperationFailure("Failed iterating directory.", err);
	}
#endif
	return *this;
}

NodeCategory
HDirectory::GetNodeCategory() const ynothrow
{
	if(*this)
	{
		YAssert(bool(GetNativeHandle()), "Invariant violation found.");
#if YCL_Win32
		const auto res(Deref(static_cast<platform_ex::DirectoryFindData*>(
			GetNativeHandle())).GetNodeCategory());
#else
		auto res(NodeCategory::Empty);

		try
		{
			auto name(sDirPath + Deref(p_dirent).d_name);
			struct ::stat st;

#	if YCL_DS
			if(::stat(name.c_str(), &st) == 0)
#	else
			// XXX: Value of %errno might be overwrite.
			if(::lstat(name.c_str(), &st) == 0)
				res |= CategorizeNode(st.st_mode);
			if(bool(res & NodeCategory::Link) && ::stat(name.c_str(), &st) == 0)
#	endif
				res |= CategorizeNode(st.st_mode);
		}
		// TODO: Log on failure.
		CatchIgnore(...)
#endif
		return res != NodeCategory::Empty ? res : NodeCategory::Invalid;
	}
	return NodeCategory::Empty;
}

HDirectory::operator string() const
{
#if YCL_Win32
	return MakePathString(GetNativeName());
#else
	return GetNativeName();
#endif
}
HDirectory::operator u16string() const
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


namespace FAT
{

namespace LFN
{

tuple<string, string, bool>
ConvertToAlias(const u16string& long_name)
{
	YAssert(ystdex::ntctslen(long_name.c_str()) == long_name.length(),
		"Invalid long file name found.");

	string alias;
	// NOTE: Strip leading periods.
	size_t offset(long_name.find_first_not_of(u'.'));
	// NOTE: Set when the alias had to be modified to be valid.
	bool lossy(offset != 0);
	const auto check_char([&](string& str, char16_t uc){
		int bc(std::wctob(std::towupper(wint_t(uc))));

		if(!lossy && std::wctob(wint_t(uc)) != bc)
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
	ImplRet(ystdex::restrict_length(MakePathString({path,
		std::min<size_t>(ystdex::ntctslen(path), MaxLength)}), MaxMBCSLength))
}

EntryDataUnit
GenerateAliasChecksum(const EntryDataUnit* p) ynothrowv
{
	static_assert(std::is_same<EntryDataUnit, unsigned char>::value,
		"Only unsigned char as byte is supported by checksum generation.");

	YAssertNonnull(p);
	// NOTE: The operation is an unsigned char rotate right.
	return std::accumulate(p, p + AliasEntryLength, 0,
		[](EntryDataUnit v, EntryDataUnit b){
			return ((v & 1) != 0 ? 0x80 : 0) + (v >> 1) + b;
		});
}

bool
ValidateName(string_view name) ynothrowv
{
	YAssertNonnull(name.data());
	return std::all_of(begin(name), end(name), [](char c) ynothrow{
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
	std::function<bool(string_view)> verify)
{
	YAssertNonnull(name.data()),
	YAssertNonnull(verify);

	EntryDataUnit alias_check_sum(0);
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
		const auto& long_name(CHRLib::MakeUCS2LE(name));
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
							ystdex::throw_error(std::errc::invalid_argument);
						LFN::WriteNumericTail(alias, i++);
					});
				}
			}
			WriteAlias(alias);
		}
		else
			ystdex::throw_error(std::errc::invalid_argument);
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
	if((*this)[0] != Free)
	{
		if((*this)[0] == '.')
			return (*this)[1] == '.' ? ".." : ".";

		// NOTE: Copy the base filename.
		bool case_info(((*this)[CaseInfo] & LFN::CaseLowerBasename) != 0);
		const auto conv([&](size_t i){
			const auto c((*this)[i]);

			return char(case_info ? std::tolower(int(c)) : c);
		});
		string res;

		res.reserve(LFN::MaxAliasLength - 1);
		for(size_t i(0); i < LFN::MaxAliasMainPartLength
			&& (*this)[Name + i] != ' '; ++i)
			res += conv(Name + i);
		if((*this)[Extension] != ' ')
		{
			res += '.';
			case_info = ((*this)[CaseInfo] & LFN::CaseLowerExtension) != 0;
			for(size_t i(0); i < LFN::MaxAliasExtensionLength
				&& (*this)[Extension + i] != ' '; ++i)
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
		(*this)[j] = ' ';
		++j;
	}
	if(alias[i] == '.')
		for(++i; alias[i] != char() && j < LFN::AliasEntryLength;
			yunseq(++i, ++j))
			(*this)[j] = EntryDataUnit(alias[i]);
	for(; j < LFN::AliasEntryLength; ++j)
		(*this)[j] = ' ';
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
			ystdex::throw_error(std::errc::invalid_argument);
	}
	return path;
}

} // namespace FAT;

} // namespace platform;

namespace platform_ex
{

} // namespace platform_ex;

