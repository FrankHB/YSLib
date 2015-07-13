/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NLS.cpp
\ingroup YCLib
\ingroup MinGW32
\brief Win32 平台自然语言处理支持扩展接口。
\version r199
\author FrankHB <frankhb1989@gmail.com>
\since build 556
\par 创建时间:
	2013-11-25 17:33:25 +0800
\par 修改时间:
	2015-07-13 00:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::NLS
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_MinGW32_YCLib_NLS
#	include YFM_YCLib_MemoryMapping
#	include YFM_YCLib_Container // for map;
#	include <ystdex/algorithm.hpp> // for ystdex::trivially_copy_n;

using namespace YSLib;
using namespace Drawing;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

std::wstring
FetchNLSItemFromRegistry(const wchar_t* name)
{
	return FetchRegistryString(HKEY_LOCAL_MACHINE,
		L"System\\CurrentControlSet\\Control\\Nls\\CodePage", name);
}


//! \since build 552
namespace
{

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


class NLSTableEntry
{
private:
	unique_ptr<platform::MappedFile> p_mapped;
	CPTABLEINFO table;

public:
	NLSTableEntry(int);
	DefGetter(const ynothrow, const CPTABLEINFO&, Table, table)
};

NLSTableEntry::NLSTableEntry(int cp)
	: p_mapped(new platform::MappedFile(WCSToMBCS(FetchSystemPath()
	+ FetchCPFileNameFromRegistry(cp))))
{
	const auto base(reinterpret_cast<unsigned short*>(p_mapped->GetPtr()));
	auto& header(*reinterpret_cast<NLS_FILE_HEADER*>(base));

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

map<int, unique_ptr<NLSTableEntry>> NLSCache;

NLSTableEntry&
FetchNLSTableEntry(int cp)
{
	lock_guard<mutex> lck(NLSCacheMutex);

	auto& p(NLSCache[cp]);

	if(YB_UNLIKELY(!p))
		p.reset(new NLSTableEntry(cp));
	return *p;
}

} // unnamed namespace;

const unsigned short*
FetchDBCSOffset(int cp) ynothrow
{
	return TryInvoke([=]{
		auto& tbl(FetchNLSTableEntry(cp).GetTable());

		return tbl.DBCSCodePage ? tbl.DBCSOffsets : nullptr;
	});
	return {};
}

} // inline namespace Windows;

#endif

} // namespace YSLib;

