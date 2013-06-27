/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Font.cpp
\ingroup Adaptor
\brief 平台无关的字体库。
\version r3032
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2009-11-12 22:06:13 +0800
\par 修改时间:
	2013-06-28 01:12 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Font
*/


#include "YSLib/Adaptor/Font.h"
#include "YSLib/Core/yapp.h"
#include "YSLib/Core/yexcept.h"
#include "YSLib/Core/yfilesys.h"
#include <Helper/GUIApplication.h>
#include "YCLib/Debug.h"
#include <algorithm> // for std::for_each;
//#include FT_BITMAP_H
//#include FT_GLYPH_H
//#include FT_OUTLINE_H
//#include FT_SYNTHESIS_H

using namespace ystdex;
using namespace platform;

YSL_BEGIN

using namespace IO;

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 供 FreeType 使用的客户端字体查询函数。

从由 face_id 提供的参数对应的字体文件中读取字体，写入 aface 。
*/
::FT_Error
simpleFaceRequester(::FTC_FaceID face_id, ::FT_Library,
					::FT_Pointer, ::FT_Face* aface)
{
	if(YB_UNLIKELY(!face_id || !aface))
		return FT_Err_Invalid_Argument;
	*aface = static_cast<Typeface*>(face_id)->face;
	return FT_Err_Ok;
}


FontFamily::FontFamily(FontCache& cache, const FamilyName& name)
	: Cache(cache), family_name(name), mFaces()
{}

void
FontFamily::operator+=(Typeface& face)
{
	mFaces.insert(make_pair(face.GetStyleName(), &face));
}

bool
FontFamily::operator-=(Typeface& face)
{
	return mFaces.erase(face.GetStyleName()) != 0;
}

Typeface*
FontFamily::GetTypefacePtr(FontStyle fs) const
{
	Typeface* const p(GetTypefacePtr(FetchName(fs)));

	return p ? p : (fs == FontStyle::Regular ? nullptr
		: GetTypefacePtr("Regular"));
}
Typeface*
FontFamily::GetTypefacePtr(const StyleName& style_name) const
{
	const auto i(mFaces.find(style_name));

	return (i == mFaces.cend()) ? nullptr : i->second;
}


Typeface::Typeface(FontCache& cache, const FontPath& path, u32 i)
	: Path(path), face_index(i), cmap_index(-1), style_name(), face(),
	family(*[&, this]{
		if(YB_UNLIKELY(cache.sFaces.find(this) != cache.sFaces.end()))
			throw LoggedEvent("Duplicate typeface found.", 2);

		::FT_Error error(::FT_New_Face(cache.library, Path.c_str(),
			face_index, &face));

		if(YB_LIKELY(!error))
			if(YB_LIKELY(!(error = ::FT_Select_Charmap(face,
				FT_ENCODING_UNICODE)) && face))
				cmap_index = face->charmap
					? ::FT_Get_Charmap_Index(face->charmap) : 0;
		if(YB_UNLIKELY(error))
		{
			platform::yprintf("Face request error: %08x\n", error);
			throw LoggedEvent("Face loading failed.", 2);
		}

		const FamilyName family_name(face->family_name);
		auto& p_ff(cache.mFamilies[family_name]);

		if(!p_ff)
			p_ff = make_unique<FontFamily>(cache, family_name);
		return p_ff.get();
	}()), glyph_index_cache()
{
	YAssert(face, "Null pointer found.");
	YAssert(::FT_UInt(cmap_index) < ::FT_UInt(face->num_charmaps),
		"Invalid CMap index found.");

	style_name = face->style_name;
	family.get() += *this;
}

bool
Typeface::operator==(const Typeface& rhs) const
{
	return Path == rhs.Path && face_index == rhs.face_index;
}
bool
Typeface::operator<(const Typeface& rhs) const
{
	return Path < rhs.Path
		|| (Path == rhs.Path && face_index < rhs.face_index);
}

::FT_UInt
Typeface::GetGlyphIndex(ucs4_t c)
{
	auto i(glyph_index_cache.find(c));

	if(i == glyph_index_cache.end())
	{
		if(cmap_index > 0)
			::FT_Set_Charmap(face, face->charmaps[cmap_index]);
		i = glyph_index_cache.insert(make_pair(c, ::FT_Get_Char_Index(face,
			::FT_ULong(c)))).first;
	}
	return i->second;
}


const Typeface&
FetchDefaultTypeface() ythrow(LoggedEvent)
{
	const Typeface* const pDefaultTypeface(
		FetchDefaultFontCache().GetDefaultTypefacePtr());

	if(YB_UNLIKELY(!pDefaultTypeface))
		throw LoggedEvent("Null default font face pointer found.");
	return *pDefaultTypeface;
}


FontCache::FontCache(size_t cache_size)
	: pDefaultFace()
{
	::FT_Error error;

	if(YB_LIKELY((error = ::FT_Init_FreeType(&library)) == 0
		&& (error = ::FTC_Manager_New(library, 0, 0, cache_size,
		&simpleFaceRequester, nullptr, &manager)) == 0
		&& (error = ::FTC_SBitCache_New(manager, &sbitCache)) == 0))
	{
		// TODO: Write log on success.
	}
	else
	{
		// TODO: Format without allocating memory.
		throw LoggedEvent(
			ystdex::sfmt("Font init failed: %08x\n;", error).c_str(), 1);
	}
}
FontCache::~FontCache()
{
	::FTC_Manager_Done(manager);
	::FT_Done_FreeType(library);
	ClearContainers();
}

const FontFamily*
FontCache::GetFontFamilyPtr(const FamilyName& family_name) const
{
	const auto i(mFamilies.find(family_name));

	return (i == mFamilies.cend()) ? nullptr : i->second.get();
}

const Typeface*
FontCache::GetDefaultTypefacePtr() const ythrow(LoggedEvent)
{
	// NOTE: Guaranteed to be non-null for default typeface in default cache.
	return pDefaultFace ? pDefaultFace
		: FetchDefaultFontCache().GetDefaultTypefacePtr();
}
const Typeface*
FontCache::GetTypefacePtr(const FamilyName& family_name,
	const StyleName& style_name) const
{
	const FontFamily* f(GetFontFamilyPtr(family_name));

	if(YB_UNLIKELY(!f))
		return nullptr;
	return f->GetTypefacePtr(style_name);
}
::FT_Face
FontCache::GetNativeFace(Typeface* pFace) const
{
	::FT_Face face(nullptr);

	if(YB_LIKELY(pFace))
		::FTC_Manager_LookupFace(manager, pFace, &face);
	return face;
}

void
FontCache::operator+=(unique_ptr<FontFamily> p_family)
{
	mFamilies.emplace(make_pair(p_family->GetFamilyName(),
		std::move(p_family)));
}
void
FontCache::operator+=(Typeface& face)
{
	sFaces.insert(&face);
}

bool
FontCache::operator-=(FontFamily& family)
{
	return mFamilies.erase(family.GetFamilyName()) != 0;
}
bool
FontCache::operator-=(Typeface& face)
{
	return &face != pDefaultFace && sFaces.erase(&face) != 0;
}

void
FontCache::ClearContainers()
{
	std::for_each(sFaces.begin(), sFaces.end(), delete_obj());
	sFaces.clear();
	mFamilies.clear();
}

size_t
FontCache::LoadTypefaces(const FontPath& path)
{
	if(ufexists(path.c_str()) && !IO::VerifyDirectory(path))
	{
		::FT_Face face(nullptr);

		if(::FT_New_Face(library, path.c_str(), -1, &face) != 0)
			return 0;

		const auto face_num(face->num_faces);

		::FT_Done_Face(face);
		if(face_num < 0)
			return 0;

		const size_t face_n(face_num);

		for(size_t i(0); i < face_n; ++i)
			try
			{
				*this += *(ynew Typeface(*this, path, i));
			}
			catch(...)
			{}
		return face_n;
	}
	return 0;
}

void
FontCache::InitializeDefaultTypeface()
{
	if(YB_LIKELY(!(pDefaultFace || sFaces.empty())))
		pDefaultFace = *sFaces.begin();
}

void
FontCache::Reset()
{
	ClearContainers();
	::FTC_Manager_Reset(manager);
}


Font::Font(const FontFamily& family, const FontSize size, FontStyle fs)
	: scaler{family.GetTypefacePtr(fs), size, size, 1, 0, 0}, style(fs)
{
	if(YB_UNLIKELY(!scaler.face_id))
		throw LoggedEvent("Bad font found.");
}

s8
Font::GetAdvance(ucs4_t c, FTC_SBit sbit) const
{
	if(YB_UNLIKELY(c == '\t'))
		return GetAdvance(' ') << 2;
	if(!sbit)
		sbit = GetGlyph(c, FT_LOAD_DEFAULT);
	if(YB_LIKELY(sbit))
		return sbit->xadvance;
	return 0;
}
s8
Font::GetAscender() const
{
	return GetInternalInfo().metrics.ascender >> 6;
}
s8
Font::GetDescender() const
{
	return GetInternalInfo().metrics.descender >> 6;
}
CharBitmap
Font::GetGlyph(ucs4_t c, ::FT_UInt flags) const
{
	auto& cache(GetCache());
	::FTC_SBit sbit;

	::FTC_SBitCache_LookupScaler(cache.sbitCache, &scaler, flags,
		GetTypeface().GetGlyphIndex(c), &sbit, nullptr);
	return sbit;
}
FontSize
Font::GetHeight() const ynothrow
{
	return GetInternalInfo().metrics.height >> 6;
}
FT_SizeRec&
Font::GetInternalInfo() const
{
	::FT_Size size(nullptr);

	if(YB_UNLIKELY(::FTC_Manager_LookupSize(GetCache().manager, &scaler, &size)
		!= 0))
		throw LoggedEvent("Size lookup failed.");
	return *size;
}

void
Font::SetSize(FontSize s)
{
	if(YB_LIKELY(s >= MinimalSize && s <= MaximalSize))
		yunseq(scaler.width = s, scaler.height = s);
}
bool
Font::SetStyle(FontStyle fs)
{
	auto pFace(GetFontFamily().GetTypefacePtr(fs));

	if(pFace)
	{
		scaler.face_id = pFace;
		return true;
	}
	return false;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

