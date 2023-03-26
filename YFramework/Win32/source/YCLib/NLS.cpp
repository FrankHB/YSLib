/*
	© 2014-2016, 2018, 2020-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NLS.cpp
\ingroup YCLib
\ingroup Win32
\brief Win32 平台自然语言处理支持扩展接口。
\version r523
\author FrankHB <frankhb1989@gmail.com>
\since build 556
\par 创建时间:
	2013-11-25 17:33:25 +0800
\par 修改时间:
	2023-03-26 12:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::NLS
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include <libdefect/exception.h> // for std::throw_with_nested;
#	include <stdexcept> // for std::invalid_argument;
#	include YFM_Win32_YCLib_NLS // for platform::Nonnull,
//	ystdex::trivially_copy_n, ystdex::try_emplace, ystdex::noncopyable,
//	ystdex::nonmovable, WOW64FileSystemRedirectionGuard;
#	include YFM_YSLib_Core_YCoreUtilities // for YSLib::CheckPositive,
//	YSLib::CheckUpperBound;
#	include YFM_YCLib_Mutex // for platform::Concurrency::mutex,
//	platform::Concurrency::lock_guard;
#	include YFM_Win32_YCLib_Registry // for FetchRegistryString,
//	HKEY_LOCAL_MACHINE;
#	include YFM_YCLib_MemoryMapping // for platform::MappedFile;
#	include <ystdex/type_pun.hpp> // for ystdex::aligned_cast;
#	include <ystdex/unordered_map.hpp> // for ystdex::unordered_map;

//! \since build 937
using YSLib::CheckPositive;
//! \since build 970
using YSLib::CheckUpperBound;
//! \since build 937
using platform::Concurrency::mutex;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

//! \see https://technet.microsoft.com/library/cc976084.aspx 。
YB_NONNULL(1) wstring
FetchNLSItemFromRegistry(const wchar_t* name)
{
	return FetchRegistryString(HKEY_LOCAL_MACHINE,
		L"System\\CurrentControlSet\\Control\\Nls\\CodePage", name);
}


//! \since build 552
namespace
{

//! \since build 970
//!@{
template<class _tWString = wstring>
YB_ATTR_nodiscard YB_NONNULL(3) _tWString
MBCSToWCSImpl(const typename _tWString::allocator_type& a, int l,
	const char* str, unsigned cp)
{
	const int
		w_len(::MultiByteToWideChar(cp, 0, platform::Nonnull(str), l, {}, 0));

	if(w_len != 0)
	{
		_tWString res(CheckPositive<size_t>(w_len), wchar_t(), a);

		::MultiByteToWideChar(cp, 0, str, l, &res[0], w_len);
		if(l == -1 && !res.empty())
			res.pop_back();
		return res;
	}
	return _tWString(a);
}

template<class _tString = string>
YB_ATTR_nodiscard YB_NONNULL(3) _tString
WCSToMBCSImpl(const typename _tString::allocator_type& a, int l,
	const wchar_t* str, unsigned cp)
{
	const int r_l(
		::WideCharToMultiByte(cp, 0, platform::Nonnull(str), l, {}, 0, {}, {}));

	if(r_l != 0)
	{
		_tString res(CheckPositive<size_t>(r_l), char(), a);

		::WideCharToMultiByte(cp, 0, str, l, &res[0], r_l, {}, {});
		if(l == -1 && !res.empty())
			res.pop_back();
		return res;
	}
	return _tString(a);
}

template<class _tString = string, class _tWString = wstring>
YB_ATTR_nodiscard YB_NONNULL(3) _tString
MBCSToMBCSImpl(const typename _tString::allocator_type& a, int l,
	const char* str, unsigned cp_src, unsigned cp_dst)
{
	if(cp_src != cp_dst)
	{
		const int w_len(::MultiByteToWideChar(cp_src, 0, platform::Nonnull(str),
			l, {}, 0));

		if(w_len != 0)
		{
			_tWString wstr(CheckPositive<size_t>(w_len), wchar_t(), a);
			const auto w_str(&wstr[0]);

			::MultiByteToWideChar(cp_src, 0, str, l, w_str, w_len);
			if(l == -1 && !wstr.empty())
				wstr.pop_back();
			return wstr.length() != 0 ? WCSToMBCSImpl<_tString>(a,
				CheckUpperBound<int>(wstr.length()), w_str, cp_dst)
				: _tString(a);
		}
		return _tString(a);
	}
	return str;
}
//!@}


yconstexpr const size_t MAXIMUM_LEADBYTES(12);

struct NLS_FILE_HEADER
{
	unsigned short HeaderSize;
	unsigned short CodePage;
	unsigned short MaximumCharacterSize;
	unsigned short DefaultChar;
	unsigned short UniDefaultChar;
	unsigned short TransDefaultChar;
	unsigned short TransUniDefaultChar;
	byte LeadByte[MAXIMUM_LEADBYTES];
};

struct CPTABLEINFO
{
	unsigned short CodePage;
	unsigned short MaximumCharacterSize;
	unsigned short DefaultChar;
	unsigned short UniDefaultChar;
	unsigned short TransDefaultChar;
	unsigned short TransUniDefaultChar;
	unsigned short DBCSCodePage;
	byte LeadByte[MAXIMUM_LEADBYTES];
	unsigned short* MultiByteTable;
	void* WideCharTable;
	unsigned short* DBCSRanges;
	unsigned short* DBCSOffsets;
};


class NLSTableEntry final
	: private ystdex::noncopyable, private ystdex::nonmovable
{
private:
	//! \since build 713
	platform::MappedFile mapped;
	CPTABLEINFO table;

public:
	NLSTableEntry(int);
	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const CPTABLEINFO&, Table, table)
};

NLSTableEntry::NLSTableEntry(int cp)
	: mapped([](const std::string& path){
		// NOTE: Some NLS files are missing from SysWOW64 directory in some
		//	versions of 64-bit Windows 10.
		WOW64FileSystemRedirectionGuard gd(true);

		TryRet(platform::MappedFile(path))
		// XXX: This would be caught by %YSLib::TryInvoke in the call to
		//	%FetchDBCSOffset.
		CatchExpr(..., std::throw_with_nested(std::invalid_argument(
			ystdex::sfmt("Failed initializing NLS table entry specified from"
			" the path '%s'.", path.c_str()))))
	}(WCSToMBCS(FetchSystemPath() + FetchCPFileNameFromRegistry(cp))))
{
	const auto base(reinterpret_cast<unsigned short*>(mapped.GetPtr()));
	auto& header(*ystdex::aligned_cast<NLS_FILE_HEADER*>(base));

	yunseq(
	table.CodePage = header.CodePage,
	table.MaximumCharacterSize = header.MaximumCharacterSize,
	table.DefaultChar = header.DefaultChar,
	table.UniDefaultChar = header.UniDefaultChar,
	table.TransDefaultChar = header.TransDefaultChar,
	table.TransUniDefaultChar = header.TransUniDefaultChar,
	ystdex::trivially_copy_n(header.LeadByte, 1, table.LeadByte),
	// NOTE: Offset to wide char table is after the header, then the multibyte
	//	table (256 wide characters) .
	table.WideCharTable = base + header.HeaderSize + 1
		+ base[header.HeaderSize],
	table.MultiByteTable = base + header.HeaderSize + 1
	);
	// NOTE: Glyph table (256 wide characters) is probably present.
	table.DBCSRanges = table.MultiByteTable + 256 + 1;
	if(table.MultiByteTable[256])
		table.DBCSRanges += 256;
	table.DBCSOffsets = (table.DBCSCodePage = *table.DBCSRanges)
		? table.DBCSRanges + 1 : nullptr;
}


mutex NLSCacheMutex;
//! \since build 970
ystdex::unordered_map<int, NLSTableEntry> NLSCache;

} // unnamed namespace;

std::string
MBCSToMBCS(const char* str, unsigned cp_src, unsigned cp_dst)
{
	return
		MBCSToMBCSImpl<std::string, std::wstring>({}, -1, str, cp_src, cp_dst);
}
string
MBCSToMBCS(const string::allocator_type& a, const char* str, unsigned cp_src,
	unsigned cp_dst)
{
	return MBCSToMBCSImpl(a, -1, str, cp_src, cp_dst);
}
std::string
MBCSToMBCS(string_view sv, unsigned cp_src, unsigned cp_dst)
{
	return sv.length() != 0 ? MBCSToMBCSImpl<std::string, std::wstring>({},
		CheckUpperBound<int>(sv.length()), sv.data(), cp_src, cp_dst)
		: std::string();
}
string
MBCSToMBCS(const string::allocator_type& a, string_view sv, unsigned cp_src,
	unsigned cp_dst)
{
	return sv.length() != 0 ? MBCSToMBCSImpl(a, CheckUpperBound<int>(
		sv.length()), sv.data(), cp_src, cp_dst) : string(a);
}

std::wstring
MBCSToWCS(const char* str, unsigned cp)
{
	return MBCSToWCSImpl<std::wstring>({}, -1, str, cp);
}
wstring
MBCSToWCS(const wstring::allocator_type& a, const char* str, unsigned cp)
{
	return MBCSToWCSImpl(a, -1, str, cp);
}
std::wstring
MBCSToWCS(string_view sv, unsigned cp)
{
	return sv.length() != 0 ? MBCSToWCSImpl<std::wstring>({},
		CheckUpperBound<int>(sv.length()), sv.data(), cp) : std::wstring();
}
wstring
MBCSToWCS(const wstring::allocator_type& a, string_view sv, unsigned cp)
{
	return sv.length() != 0 ? MBCSToWCSImpl(a,
		CheckUpperBound<int>(sv.length()), sv.data(), cp) : wstring(a);
}

std::string
WCSToMBCS(const wchar_t* str, unsigned cp)
{
	return WCSToMBCSImpl<std::string>({}, -1, str, cp);
}
string
WCSToMBCS(const string::allocator_type& a, const wchar_t* str, unsigned cp)
{
	return WCSToMBCSImpl(a, -1, str, cp);
}
std::string
WCSToMBCS(wstring_view sv, unsigned cp)
{
	return sv.length() != 0 ? WCSToMBCSImpl<std::string>({},
		CheckUpperBound<int>(sv.length()), sv.data(), cp) : std::string();
}
string
WCSToMBCS(const string::allocator_type& a, wstring_view sv, unsigned cp)
{
	return sv.length() != 0 ? WCSToMBCSImpl(a,
		CheckUpperBound<int>(sv.length()), sv.data(), cp) : string(a);
}


const unsigned short*
FetchDBCSOffset(int cp) ynothrow
{
	return YSLib::TryInvoke([=]{
		platform::Concurrency::lock_guard<mutex> lck(NLSCacheMutex);
		// TODO: Enable concurrent initialization using %call_once?
		auto& tbl(ystdex::try_emplace(NLSCache, cp, cp).first
			->second.GetTable());

		return tbl.DBCSCodePage ? tbl.DBCSOffsets : nullptr;
	});
}

} // inline namespace Windows;

#endif

} // namespace platform_ex;

