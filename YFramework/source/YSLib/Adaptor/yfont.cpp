/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfont.cpp
\ingroup Adaptor
\brief 平台无关的字体缓存库。
\version r7551;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:06:13 +0800;
\par 修改时间:
	2012-01-31 06:33 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YFontCache;
*/


#include "YSLib/Adaptor/yfont.h"
#include "YSLib/Core/yapp.h"
#include "YSLib/Core/yexcept.h"
#include "YSLib/Core/yfilesys.h"
#include "YSLib/Helper/yglobal.h"
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
simpleFaceRequester(::FTC_FaceID face_id, ::FT_Library library,
					::FT_Pointer, ::FT_Face* aface)
{
	Typeface* fontFace(static_cast<Typeface*>(face_id));
	::FT_Face& face(*aface);
	::FT_Error error(FT_New_Face(library, fontFace->Path.c_str(),
		fontFace->face_index, aface));

	if(!error)
	{
		error = ::FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		if(!error && face)
		{
			fontFace->cmap_index = face->charmap
				? ::FT_Get_Charmap_Index(face->charmap) : 0;
#if 0
			fontFace->nGlyphs = face->num_glyphs;
			fontFace->uUnitPerEM = face->units_per_EM;
			fontFace->nCharmaps = face->num_charmaps;
			if(FT_IS_SCALABLE(face))
				fontFace->lUnderlinePos = FT_MulFix(face->underline_position,
					face->size->metrics.y_scale) >> 6;
			fontFace->fixSizes.clear();
			if(face->available_sizes)
			{
				::FT_Int t = face->num_fixed_sizes;

				fontFace->fixSizes.reserve(t);
				for(::FT_Int i = 0; i < t; ++i)
					fontFace->fixSizes.push_back(
						face->available_sizes[i].size >> 6);
			}
		
#endif
		}
	}
#if 0
	// NOTE: wrong impl;
	::FT_GlyphSlot_Embolden(face->glyph);
	::FT_GlyphSlot_Oblique(face->glyph);
	::FT_Outline_Embolden(&face->glyph->outline, 64);
	::FT_Set_Transform(face, &fontFace->matrix, nullptr);
#endif
	if(error)
		yprintf("Face request error: %08x\n", error);
	return error;
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
FontFamily::GetTypefacePtr(FontStyle style) const
{
	Typeface* const p(GetTypefacePtr(FetchName(style)));

	return p ? p : (style == FontStyle::Regular ? nullptr
		: GetTypefacePtr("Regular"));
}
Typeface*
FontFamily::GetTypefacePtr(const StyleName& style_name) const
{
	const auto i(mFaces.find(style_name));

	return (i == mFaces.cend()) ? nullptr : i->second;
}


/*const ::FT_Matrix Typeface::MNormal = {0x10000, 0, 0, 0x10000},
	Typeface::MOblique = {0x10000, 0x5800, 0, 0x10000};*/

Typeface::Typeface(FontCache& cache, const FontPath& path, u32 i
	/*, const bool bb, const bool bi, const bool bu*/)
	: Path(path), face_index(i), cmap_index(-1)
/*	, bBold(bb), bOblique(bi), bUnderline(bu),
	, matrix(bi ? MOblique : MNormal)*/
{
	if(cache.sFaces.find(this) != cache.sFaces.end())
		throw LoggedEvent("Duplicate typeface found.", 2);

	::FTC_FaceID new_face_id(this);
	::FT_Face face(nullptr);

	//读取字型名称并构造名称映射。
	if(FTC_Manager_LookupFace(cache.manager, new_face_id, &face) != 0 || !face)
		throw LoggedEvent("Face loading failed.", 2);

	const FamilyName family_name(face->family_name);
	const auto it(cache.mFamilies.find(family_name));
	const bool not_found(it == cache.mFamilies.end());

	yunseq(pFontFamily = not_found ? ynew FontFamily(cache, family_name)
		: it->second, style_name = face->style_name);
	if(not_found)
		cache += *pFontFamily;
	*pFontFamily += *this;
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


const Typeface&
FetchDefaultTypeface() ythrow(LoggedEvent)
{
	const Typeface* const pDefaultTypeface(
		FetchDefaultFontCache().GetDefaultTypefacePtr());

	if(!pDefaultTypeface)
		throw LoggedEvent("Null default font face pointer found"
			" @ FetchDefaultTypeface.");
	return *pDefaultTypeface;
}


FontCache::FontCache(const_path_t default_font_path, size_t cache_size)
{
	::FT_Error error;

	if((error = ::FT_Init_FreeType(&library)) == 0
		&& (error = ::FTC_Manager_New(library, 0, 0, cache_size,
		&simpleFaceRequester, nullptr, &manager)) == 0
		&& (error = ::FTC_SBitCache_New(manager, &sbitCache)) == 0
		&& (error = ::FTC_CMapCache_New(manager, &cmapCache)) == 0)
	{
		if(LoadFontFile(FontPath(default_font_path)))
			LoadTypefaces();
		InitializeDefaultTypeface();
	}
	else
	{
		char strerr[30];

		std::sprintf(strerr, "Font init failed: %08x\n;", error);
		throw LoggedEvent(strerr, 1);
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

	return (i == mFamilies.cend()) ? nullptr : i->second;
}
/*Typeface* FontCache::GetTypefacePtr(u16 i) const
{
	return i < sFaces.size() ? sFaces[i] : nullptr;
}*/
const Typeface*
FontCache::GetDefaultTypefacePtr() const ythrow(LoggedEvent)
{
	//默认字体缓存的默认字型指针由初始化保证为非空指针。
	return pDefaultFace ? pDefaultFace
		: FetchDefaultFontCache().GetDefaultTypefacePtr();
}
const Typeface*
FontCache::GetTypefacePtr(const FamilyName& family_name,
	const StyleName& style_name) const
{
	const FontFamily* f(GetFontFamilyPtr(family_name));

	if(!f)
		return nullptr;
	return f->GetTypefacePtr(style_name);
}
::FT_Face
FontCache::GetNativeFace(Typeface* pFace) const
{
	::FT_Face face(nullptr);

	if(pFace)
		::FTC_Manager_LookupFace(manager, pFace, &face);
	return face;
}

void
FontCache::operator+=(FontFamily& family)
{
	mFamilies.insert(make_pair(family.GetFamilyName(), &family));
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
FontCache::ClearCache()
{
	ClearContainers();
	FTC_Manager_Reset(manager);
}

void
FontCache::ClearContainers()
{
	//清除字体文件组。
	mPaths.clear();
	//清除字型组。
	std::for_each(sFaces.begin(), sFaces.end(), delete_obj());
	sFaces.clear();
	//清除字型家族组。
	std::for_each(mFamilies.begin(), mFamilies.end(), delete_second_mem());
	mFamilies.clear();
}

void
FontCache::LoadTypefaces()
{
	for(auto i(mPaths.begin()); i != mPaths.end(); ++i)
		if(i->second > 0)
			LoadTypefaces(i->first, i->second);
}
void
FontCache::LoadTypefaces(const FontPath& path, size_t n)
{
	if(mPaths.find(path) == mPaths.end() && !LoadFontFile(path))
		return;
	for(size_t i(0); i < n; ++i)
	{
		try
		{
			Typeface* pFace;

			*this += *(pFace = ynew Typeface(*this, path, i));
		}
		catch(...)
		{}
	}
	InitializeDefaultTypeface();
}

bool
FontCache::LoadFontFile(const FontPath& path)
{
	if(GetFileNameOf(path.c_str()) && fexists(path.c_str())
		&& mPaths.find(path) == mPaths.end())
	{
		::FT_Long face_n;
		::FT_Face face(nullptr);

		if(::FT_New_Face(library, path.c_str(), -1, &face))
			face_n = -1;
		else
		{
			face_n = face->num_faces;
			::FT_Done_Face(face);
		}
		mPaths.insert(pair<const FontPath, ::FT_Long>(path, face_n));
		return true;
	}
	return false;
}

void
FontCache::InitializeDefaultTypeface()
{
	if(!(pDefaultFace || sFaces.empty()))
		pDefaultFace = *sFaces.begin();
}


Font::Font(const FontFamily& family, const FontSize size, FontStyle style)
	: Style(style)
{
	yunseq(scaler.face_id = family.GetTypefacePtr(style), scaler.width = size,
		scaler.height = size, scaler.pixel = 1, scaler.x_res = 0,
		scaler.y_res = 0);
	if(!scaler.face_id)
		throw LoggedEvent("Bad font;");
}

s8
Font::GetAdvance(ucs4_t c, FTC_SBit sbit) const
{
	if(c == '\t')
		return GetAdvance(' ') << 2;
	if(!sbit)
		sbit = GetGlyph(c, FT_LOAD_DEFAULT);
	return sbit ? sbit->xadvance : 0;
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
	auto pFace(&GetTypeface());
	auto& cache(GetCache());
	FTC_SBit sbit;

	::FTC_SBitCache_LookupScaler(cache.sbitCache, &scaler, flags,
		::FTC_CMapCache_Lookup(cache.cmapCache, scaler.face_id,
		pFace->GetCMapIndex(), c), &sbit, nullptr);
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

	if(::FTC_Manager_LookupSize(GetCache().manager, &scaler, &size) != 0)
		throw LoggedEvent("Error occured @ Font::GetInternalSize;");
	return *size;
}

void
Font::SetSize(FontSize s)
{
	if(s >= MinimalSize && s <= MaximalSize)
		yunseq(scaler.width = s, scaler.height = s);
}
bool
Font::SetStyle(FontStyle style)
{
	auto pFace(GetFontFamily().GetTypefacePtr(style));

	if(pFace)
	{
		scaler.face_id = pFace;
		return true;
	}
	return false;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

