/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharacterProcessing.cpp
\ingroup CHRLib
\brief 字符编码处理。
\version r1504
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:53:21 +0800
\par 修改时间:
	2015-10-03 14:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterProcessing
*/


#include "CHRLib/YModules.h"
#include YFM_CHRLib_CharacterProcessing
#include YFM_CHRLib_MappingEx
#include <cctype>
#include <cstdlib>
#include <cwchar>
#include <ystdex/cstdio.h>
#include <ystdex/memory.hpp> // for ystdex::make_unique;
#include YFM_CHRLib_Convert
#include <ystdex/algorithm.hpp> // for ystdex::copy_when,
//	ystdex::transform_when;

namespace CHRLib
{

using std::malloc;
using std::size_t;
using std::tolower;
using ystdex::is_null;
using ystdex::ntctslen;
//! \since build 476
using ystdex::make_unique;

ConversionResult
MBCToUC(char16_t& uc, const char*& c, Encoding enc, ConversionState&& st)
{
	if(const auto pfun = FetchMapperPtr<ConversionResult, char16_t&, const char*&,
		ConversionState&&>(enc))
		return pfun(uc, c, std::move(st));
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(char16_t& uc, const char*& c, const char* e, Encoding enc,
	ConversionState&& st)
{
	yconstraint(c <= e);
	if(const auto pfun = FetchMapperPtr<ConversionResult, char16_t&,
		GuardPair<const char*>&&, ConversionState&&>(enc))
		return pfun(uc, {c, e}, std::move(st));
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(char16_t& uc, std::FILE* fp, Encoding enc, ConversionState&& st)
{
	yconstraint(fp);
	if(const auto pfun = FetchMapperPtr<ConversionResult, char16_t&,
		ystdex::ifile_iterator&, ConversionState&&>(enc))
	{
		ystdex::ifile_iterator i(fp);
		const auto r(pfun(uc, i, std::move(st)));

		i.sungetc(fp);
		return r;
	}
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(const char*& c, Encoding enc, ConversionState&& st)
{
	if(const auto pfun = FetchMapperPtr<ConversionResult,
		ystdex::pseudo_output&&, const char*&, ConversionState&&>(enc))
		return pfun(ystdex::pseudo_output(), c, std::move(st));
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(const char*& c, const char* e, Encoding enc, ConversionState&& st)
{
	yconstraint(c <= e);
	if(const auto pfun = FetchMapperPtr<ConversionResult,
		ystdex::pseudo_output&&, GuardPair<const char*>&&,
		ConversionState&&>(enc))
		return pfun(ystdex::pseudo_output(), {c, e}, std::move(st));
	return ConversionResult::Unhandled;
}
ConversionResult
MBCToUC(std::FILE* fp, Encoding enc, ConversionState&& st)
{
	yconstraint(fp);
	if(const auto pfun = FetchMapperPtr<ConversionResult,
		ystdex::pseudo_output&&, ystdex::ifile_iterator&,
		ConversionState&&>(enc))
	{
		ystdex::ifile_iterator i(fp);
		const auto r(pfun(ystdex::pseudo_output(), i, std::move(st)));

		i.sungetc(fp);
		return r;
	}
	return ConversionResult::Unhandled;
}

size_t
UCToMBC(char* d, const char16_t& s, Encoding enc)
{
	yconstraint(d);

	size_t l(0);

	if(const auto pfun = FetchMapperPtr<size_t, char*, char32_t>(enc))
		l = pfun(d, s);
	return l;
}


size_t
MBCSToUCS2(char16_t* d, const char* s, Encoding enc)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	if(const auto pfun = FetchMapperPtr<ConversionResult, char16_t&,
		const char*&, ConversionState&&>(enc))
		while(!is_null(*s) && pfun(*d, s, ConversionState())
			== ConversionResult::OK)
			++d;
	*d = char16_t();
	return size_t(d - p);
}
size_t
MBCSToUCS2(char16_t* d, const char* s, const char* e, Encoding enc)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(e),
	yconstraint(s <= e);

	const auto p(d);

	if(const auto pfun = FetchMapperPtr<ConversionResult, char16_t&,
		GuardPair<const char*>&&, ConversionState&&>(enc))
		while(!is_null(*s) && pfun(*d, {s, e}, ConversionState())
			== ConversionResult::OK)
			++d;
	return size_t(d - p);
}

size_t
MBCSToUCS4(char32_t* d, const char* s, Encoding enc)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	// TODO: Use UCS-4 internal conversion directly?
	if(const auto pfun = FetchMapperPtr<ConversionResult, char16_t&,
		const char*&, ConversionState&&>(enc))
		while(!is_null(*s))
		{
			// TODO: Necessary initialization?
			char16_t c;

			if(pfun(c, s, ConversionState()) == ConversionResult::OK)
			{
				*d = c;
				++d;
			}
			else
				break;
		}
	*d = char32_t();
	return size_t(d - p);
}
size_t
MBCSToUCS4(char32_t* d, const char* s, const char* e, Encoding enc)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(e),
	yconstraint(s <= e);

	const auto p(d);

	// TODO: Use UCS-4 internal conversion directly?
	if(const auto pfun = FetchMapperPtr<ConversionResult, char16_t&,
		GuardPair<const char*>&&, ConversionState&&>(enc))
		while(!is_null(*s))
		{
			// TODO: Necessary initialization?
			char16_t c;

			if(pfun(c, {s, e}, ConversionState()) == ConversionResult::OK)
			{
				*d = c;
				++d;
			}
			else
				break;
		}
	return size_t(d - p);
}

size_t
UCS2ToMBCS(char* d, const char16_t* s, Encoding enc)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	if(const auto pfun = FetchMapperPtr<size_t, char*, char32_t>(enc))
		while(!is_null(*s))
			d += pfun(d, *s++);
	*d = char();
	return size_t(d - p);
}
size_t
UCS2ToMBCS(char* d, const char16_t* s, const char16_t* e, Encoding enc)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(s <= e);

	const auto p(d);

	// TODO: Deferred. Use guard for encoding.
	if(const auto pfun = FetchMapperPtr<size_t, char*, char32_t>(enc))
		while(s < e)
			d += pfun(d, *s++);
	return size_t(d - p);
}


size_t
UCS2ToUCS4(char32_t* d, const char16_t* s)
{
	const auto p(ystdex::copy_when(s, d, [](char16_t c) ynothrow{
		return !is_null(c);
	}));

	*p = char32_t();
	return size_t(p - d);
}
size_t
UCS2ToUCS4(char32_t* d, const char16_t* s, const char16_t* e)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(s <= e);

	return size_t(std::copy(s, e, d) - d);
}

size_t
UCS4ToMBCS(char* d, const char32_t* s, Encoding enc)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	if(const auto pfun = FetchMapperPtr<size_t, char*, char32_t>(enc))
		while(!is_null(*s))
			d += pfun(d, char16_t(*s++));
	*d = char();
	return size_t(d - p);
}
size_t
UCS4ToMBCS(char* d, const char32_t* s, const char32_t* e, Encoding enc)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(s <= e);

	const auto p(d);

	// TODO: Deferred. Use guard for encoding.
	if(const auto pfun = FetchMapperPtr<size_t, char*, char32_t>(enc))
		while(s < e)
			d += pfun(d, char16_t(*s++));
	return size_t(d - p);
}

size_t
UCS4ToUCS2(char16_t* d, const char32_t* s)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(ystdex::transform_when(s, d, [](char32_t c) ynothrow{
		return !is_null(c);
	}, [](char32_t c) ynothrow{
		return char16_t(c);
	}));

	*p = char16_t();
	return size_t(p - d);
}
size_t
UCS4ToUCS2(char16_t* d, const char32_t* s, const char32_t* e)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(s <= e);

	return size_t(std::transform(s, e, d, [](char32_t c) ynothrow{
		return char16_t(c);
	}) - d);
}

} // namespace CHRLib;

