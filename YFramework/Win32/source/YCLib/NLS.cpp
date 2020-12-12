/*
	© 2014-2016, 2018, 2020 FrankHB.

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
\version r360
\author FrankHB <frankhb1989@gmail.com>
\since build 556
\par 创建时间:
	2013-11-25 17:33:25 +0800
\par 修改时间:
	2020-12-12 10:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::NLS
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_Win32_YCLib_NLS // for map, ystdex::trivially_copy_n,
//	ystdex::try_emplace;
#	include YFM_Win32_YCLib_Registry // for FetchRegistryString;
#	include YFM_YSLib_Core_YCoreUtilities // for YSLib::CheckPositive;
#	include <ystdex/base.h> // for ystdex::noncopyable, ystdex::nonmovable;
#	include YFM_YCLib_MemoryMapping // for platform::MappedFile;
#	include <ystdex/type_pun.hpp> // for ystdex::aligned_cast;

using namespace YSLib;
using namespace Drawing;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

//! \see https://technet.microsoft.com/en-us/library/cc976084.aspx 。
YB_NONNULL(1) wstring
FetchNLSItemFromRegistry(const wchar_t* name)
{
	return FetchRegistryString(HKEY_LOCAL_MACHINE,
		L"System\\CurrentControlSet\\Control\\Nls\\CodePage", name);
}


//! \since build 552
namespace
{

//! \since build 905
//@{
YB_ATTR_nodiscard YB_NONNULL(3) string
MBCSToMBCSImpl(const string::allocator_type& a, int l, const char* str,
	unsigned cp_src, unsigned cp_dst)
{
	if(cp_src != cp_dst)
	{
		const int
			w_len(::MultiByteToWideChar(cp_src, 0, Nonnull(str), l, {}, 0));

		if(w_len != 0)
		{
			wstring wstr(CheckPositive<size_t>(w_len), wchar_t(), a);
			const auto w_str(&wstr[0]);

			::MultiByteToWideChar(cp_src, 0, str, l, w_str, w_len);
			if(l == -1 && !wstr.empty())
				wstr.pop_back();
			return WCSToMBCS({w_str, wstr.length()}, cp_dst);
		}
		return {};
	}
	return str;
}

YB_ATTR_nodiscard YB_NONNULL(3) wstring
MBCSToWCSImpl(const wstring::allocator_type& a, int l, const char* str,
	unsigned cp)
{
	const int
		w_len(::MultiByteToWideChar(cp, 0, Nonnull(str), l, {}, 0));

	if(w_len != 0)
	{
		wstring res(CheckPositive<size_t>(w_len), wchar_t(), a);

		::MultiByteToWideChar(cp, 0, str, l, &res[0], w_len);
		if(l == -1 && !res.empty())
			res.pop_back();
		return res;
	}
	return {};
}

YB_ATTR_nodiscard YB_NONNULL(3) string
WCSToMBCSImpl(const string::allocator_type& a, int l, const wchar_t* str,
	unsigned cp)
{
	const int r_l(::WideCharToMultiByte(cp, 0, Nonnull(str), l, {}, 0, {}, {}));

	if(r_l != 0)
	{
		string res(CheckPositive<size_t>(r_l), char(), a);

		::WideCharToMultiByte(cp, 0, str, l, &res[0], r_l, {}, {});
		if(l == -1 && !res.empty())
			res.pop_back();
		return res;
	}
	return {};
}
//@}


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


class NLSTableEntry final : private noncopyable, private nonmovable
{
private:
	//! \since build 713
	platform::MappedFile mapped;
	CPTABLEINFO table;

public:
	NLSTableEntry(int);
	DefGetter(const ynothrow, const CPTABLEINFO&, Table, table)
};

NLSTableEntry::NLSTableEntry(int cp)
	// FIXME: Some NLS files are missing from SysWOW64 directory in some
	//	versions of 64-bit Windows 10. There should be ways to detect it.
	: mapped(WCSToMBCS(FetchSystemPath() + FetchCPFileNameFromRegistry(cp)))
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
//! \since build 693
map<int, NLSTableEntry> NLSCache;

} // unnamed namespace;

string
MBCSToMBCS(const string::allocator_type& a, const char* str, unsigned cp_src,
	unsigned cp_dst)
{
	return MBCSToMBCSImpl(a, -1, str, cp_src, cp_dst);
}
string
MBCSToMBCS(const string::allocator_type& a, string_view sv, unsigned cp_src,
	unsigned cp_dst)
{
	return sv.length() != 0 ? MBCSToMBCSImpl(a, CheckNonnegative<int>(
		sv.length()), sv.data(), cp_src, cp_dst) : string(a);
}

wstring
MBCSToWCS(const wstring::allocator_type& a, const char* str, unsigned cp)
{
	return MBCSToWCSImpl(a, -1, str, cp);
}
wstring
MBCSToWCS(const wstring::allocator_type& a, string_view sv, unsigned cp)
{
	return sv.length() != 0 ? MBCSToWCSImpl(a,
		CheckNonnegative<int>(sv.length()), sv.data(), cp) : wstring(a);
}

string
WCSToMBCS(const string::allocator_type& a, const wchar_t* str, unsigned cp)
{
	return WCSToMBCSImpl(a, -1, str, cp);
}
string
WCSToMBCS(const string::allocator_type& a, wstring_view sv, unsigned cp)
{
	return sv.length() != 0 ? WCSToMBCSImpl(a,
		CheckNonnegative<int>(sv.length()), sv.data(), cp) : string(a);
}


const unsigned short*
FetchDBCSOffset(int cp) ynothrow
{
	return TryInvoke([=]{
		lock_guard<mutex> lck(NLSCacheMutex);
		// TODO: Enable concurrent initialization using 'call_once', etc?
		auto& tbl(ystdex::try_emplace(NLSCache, cp, cp).first
			->second.GetTable());

		return tbl.DBCSCodePage ? tbl.DBCSOffsets : nullptr;
	});
}

} // inline namespace Windows;

#endif

} // namespace platform_ex;

