/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfont.cpp
\ingroup Adaptor
\brief 平台无关的字体缓存库。
\version r7289;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 22:06:13 +0800;
\par 修改时间:
	2011-10-25 12:53 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YFontCache;
*/


#include "yfont.h"
#include "../Core/yapp.h"
#include "../Core/yexcept.h"
#include "../Core/yfilesys.h"
#include "../Helper/yglobal.h"
#include <algorithm> // for std::for_each;
//#include FT_OUTLINE_H
//#include FT_SYNTHESIS_H

using namespace ystdex;
using namespace platform;

YSL_BEGIN

using namespace IO;

YSL_BEGIN_NAMESPACE(Drawing)

const char*
FontStyle::GetName() const ynothrow
{
	const char* ss(nullptr);

	switch(style)
	{
	case Bold:
		ss = "Bold";
		break;
	case Italic:
		ss = "Italic";
		break;
	case Underline:
		ss = "Underline";
		break;
	case Strikeout:
		ss = "Strikeout";
		break;
	case Regular:
	default:
		ss = "Regular";
		break;
	}

	YAssert(ss,
		"In function \"const char*\n"
		"FontStyle::GetName() const\":\n"
		"Font style name string is null.");

	return ss;
}


/*!
\brief 供 FreeType 使用的客户端字体查询函数。

从由 face_id 提供的参数对应的字体文件中读取字体，写入 aface 。
*/
FT_Error
simpleFaceRequester(FTC_FaceID face_id, FT_Library library,
					FT_Pointer, FT_Face* aface)
{
	Typeface* fontFace(static_cast<Typeface*>(face_id));
	FT_Face& face(*aface);
	FT_Error error(FT_New_Face(library, fontFace->File.GetPath().c_str(),
		fontFace->face_index, aface));

	if(!error)
	{
		error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		if(!error && face)
		{
			fontFace->cmap_index = face->charmap
				? FT_Get_Charmap_Index(face->charmap) : 0;
		/*	fontFace->nGlyphs = face->num_glyphs;
			fontFace->uUnitPerEM = face->units_per_EM;
			fontFace->nCharmaps = face->num_charmaps;*/
		//	if(FT_IS_SCALABLE(face))
		//		fontFace->lUnderlinePos = FT_MulFix(face->underline_position,
		//			face->size->metrics.y_scale) >> 6;
		/*	fontFace->fixSizes.clear();
			if(face->available_sizes)
			{
				FT_Int t = face->num_fixed_sizes;

				fontFace->fixSizes.reserve(t);
				for(FT_Int i = 0; i < t; ++i)
					fontFace->fixSizes.push_back(
						face->available_sizes[i].size >> 6);
			}
		*/
		}
	}
//	FT_GlyphSlot_Embolden(face->glyph);
//	FT_GlyphSlot_Oblique(face->glyph);
//	FT_Outline_Embolden(&face->glyph->outline, 64);
//	FT_Set_Transform(face, &fontFace->matrix, nullptr);
	if(error)
		yprintf("Face request error: %08x\n", error);
	return error;
}


FontFamily::FontFamily(FontCache& cache, const NameType& name)
	: Cache(cache), family_name(name), sFaces()
{}

bool
FontFamily::operator==(const FontFamily& rhs) const
{
	return &Cache == &rhs.Cache && family_name == rhs.family_name;
}
bool
FontFamily::operator<(const FontFamily& rhs) const
{
	return &Cache < &rhs.Cache || (&Cache == &rhs.Cache
		&& family_name < rhs.family_name);
}

void
FontFamily::operator+=(Typeface* face_pointer)
{
	if(face_pointer)
	{
		sFaces.insert(face_pointer);
		mFaces.insert(make_pair(face_pointer->GetStyleName(),
			face_pointer));
	}
}

bool
FontFamily::operator-=(Typeface* face_pointer)
{
	return face_pointer ? mFaces.erase(face_pointer->GetStyleName()) != 0
		&& sFaces.erase(face_pointer) != 0 : false;
}

Typeface*
FontFamily::GetTypefacePtr(FontStyle s) const
{
	Typeface* const p(GetTypefacePtr(s.GetName()));

	return p ? p
		: (s == FontStyle::Regular ? nullptr : GetTypefacePtr("Regular"));
}
Typeface*
FontFamily::GetTypefacePtr(const Typeface::NameType& style_name) const
{
	const auto i(mFaces.find(style_name));

	return (i == mFaces.cend()) ? nullptr : i->second;
}


/*const FT_Matrix Typeface::MNormal = {0x10000, 0, 0, 0x10000},
	Typeface::MOblique = {0x10000, 0x5800, 0, 0x10000};*/

Typeface::Typeface(FontCache& cache, const FontFile& file, u32 i
	/*, const bool bb, const bool bi, const bool bu*/)
	: Cache(cache), File(file), pFontFamily(),
	style_name(), face_index(i), cmap_index(-1)
/*	, bBold(bb), bOblique(bi), bUnderline(bu),
	, matrix(bi ? MOblique : MNormal)*/
{}

bool
Typeface::operator==(const Typeface& rhs) const
{
	return &File == &rhs.File && face_index == rhs.face_index;
}
bool
Typeface::operator<(const Typeface& rhs) const
{
	return &File < &rhs.File
		|| (&File == &rhs.File && face_index < rhs.face_index);
}


FontFile::FontFile(const PathType& p)
	: path(p), face_num(0)
{}

void
FontFile::ReloadFaces(FT_Library& library) const
{
	FT_Face face(nullptr);

	if(FT_New_Face(library, path.c_str(), -1, &face))
	{
		face_num = -1;
		return;
	}
	face_num = face->num_faces;
	FT_Done_Face(face);
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

const FontFamily&
FetchDefaultFontFamily() ythrow(LoggedEvent)
{
	const FontFamily* const pFontFamily(
		FetchDefaultTypeface().GetFontFamilyPtr());

	if(!pFontFamily)
		throw LoggedEvent("Null default font family pointer found"
			" @ FetchDefaultFontFamily.");
	return *pFontFamily;
}


Font::SizeType
Font::GetHeight() const
{
	return GetCache().GetHeight();
}

void
Font::SetSize(SizeType s)
{
	if(s >= MinimalSize && s <= MaximalSize)
		Size = s;
}

bool
Font::Update()
{
	Typeface* t(pFontFamily->GetTypefacePtr(Style));

	if(t)
	{
		if(t != GetCache().GetTypefacePtr())
			if(!GetCache().SetTypeface(t))
				return false;
		if(GetCache().GetFontSize() != Size)
			UpdateSize();
		return true;
	}
	return false;
}

void
Font::UpdateSize()
{
	GetCache().SetFontSize(Size);
}


FontCache::FontCache(const_path_t default_font_path, u32 cache_size)
{
	scaler.face_id = nullptr;
	scaler.width = 0;
	scaler.height = 0;
	scaler.pixel = 1;
	scaler.x_res = 0;
	scaler.y_res = 0;

	FT_Error error;

	if((error = FT_Init_FreeType(&library)) == 0
		&& (error = FTC_Manager_New(library, 0, 0, cache_size,
		&simpleFaceRequester, nullptr, &manager)) == 0
		&& (error = FTC_SBitCache_New(manager, &sbitCache)) == 0
		&& (error = FTC_CMapCache_New(manager, &cmapCache)) == 0)
	{
		if(LoadFontFile(default_font_path))
			LoadTypefaces();
		InitializeDefaultTypeface();
		if(pDefaultFace)
			SetTypeface(pDefaultFace);
	}
	else
	{
		char strerr[30];

		std::sprintf(strerr, "Font init failed: %08x\n", error);
		throw LoggedEvent(strerr, 1);
	}
}
FontCache::~FontCache()
{
	scaler.face_id = nullptr;
	FTC_Manager_Done(manager);
	FT_Done_FreeType(library);
	ClearContainers();
}

const FontFamily*
FontCache::GetFontFamilyPtr(const FontFamily::NameType& family_name) const
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
FontCache::GetTypefacePtr(const FontFamily::NameType& family_name,
	const Typeface::NameType& style_name) const
{
	const FontFamily* f(GetFontFamilyPtr(family_name));

	if(!f)
		return nullptr;
	return f->GetTypefacePtr(style_name);
}
CharBitmap
FontCache::GetGlyph(ucs4_t c)
{
	if(!scaler.face_id)
		return FTC_SBit();

	const u32 flags(FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL);
	const u32 index(FTC_CMapCache_Lookup(cmapCache, scaler.face_id,
		GetTypefacePtr()->cmap_index, c));
	FTC_SBit sbit;

	FTC_SBitCache_LookupScaler(sbitCache, &scaler, flags, index, &sbit,
		nullptr);
	return sbit;
}
s8
FontCache::GetAdvance(ucs4_t c, FTC_SBit sbit)
{
	if(c == '\t')
		return GetAdvance(' ') << 2;
	if(!sbit)
		sbit = GetGlyph(c);
	return sbit->xadvance;
}
FT_Face
FontCache::GetInternalFaceInfo() const
{
	FT_Face face(nullptr);

	FTC_Manager_LookupFace(manager, scaler.face_id, &face);
	return face;
}
u8
FontCache::GetHeight() const
{
	return GetInternalFaceInfo()->size->metrics.height >> 6;
}
s8
FontCache::GetAscender() const
{
	return GetInternalFaceInfo()->size->metrics.ascender >> 6;
}
s8
FontCache::GetDescender() const
{
	return GetInternalFaceInfo()->size->metrics.descender >> 6;
}

bool
FontCache::SetTypeface(Typeface* p)
{
	if(!p || sFaces.find(p) == sFaces.end())
		return false;
	scaler.face_id = static_cast<FTC_FaceID>(p);
	return true;
}
void
FontCache::SetFontSize(Font::SizeType s)
{
	if(s == 0)
		s = Font::DefaultSize;
//	if(s != GetFontSize())
//	ClearCache();
	scaler.width  = static_cast<FT_UInt>(s);
	scaler.height = static_cast<FT_UInt>(s);
	GetGlyph(' '); //更新当前字形，否则 GetHeight() 会返回错误的值。
}

void
FontCache::operator+=(const FontFile* fontfile_pointer)
{
	if(fontfile_pointer)
		sFiles.insert(fontfile_pointer);
}
bool
FontCache::operator-=(const FontFile* fontfile_pointer)
{
	return sFiles.erase(fontfile_pointer) != 0;
}

void
FontCache::operator+=(Typeface* face_pointer)
{
	if(face_pointer)
		sFaces.insert(face_pointer);
}
bool
FontCache::operator-=(Typeface* face_pointer)
{
	if(face_pointer == GetTypefacePtr() || face_pointer == pDefaultFace)
		return false;
	return sFaces.erase(face_pointer) != 0;
}

void
FontCache::operator+=(FontFamily* family_pointer)
{
	if(family_pointer)
	{
		sFamilies.insert(family_pointer);
		mFamilies.insert(make_pair(family_pointer->family_name,
			family_pointer));
	}
}
bool
FontCache::operator-=(FontFamily* family_pointer)
{
	return family_pointer ? mFamilies.erase(family_pointer->family_name) != 0
		&& sFamilies.erase(family_pointer) != 0 : false;
}

void
FontCache::ClearCache()
{
	ClearContainers();
	FTC_Manager_Reset(manager);
}

void
FontCache::ClearFontFiles()
{
	std::for_each(sFiles.begin(), sFiles.end(), delete_obj());
	sFiles.clear();
}

void
FontCache::ClearTypefaces()
{
	std::for_each(sFaces.begin(), sFaces.end(), delete_obj());
	sFaces.clear();
}

void
FontCache::ClearFontFamilies()
{
	for(auto i(mFamilies.cbegin()); i != mFamilies.cend(); ++i)
		ydelete(i->second);
	mFamilies.clear();
}

void
FontCache::ClearContainers()
{
	ClearFontFiles();
	ClearTypefaces();
	ClearFontFamilies();
}

void
FontCache::LoadTypefaces()
{
	for(auto i(sFiles.begin()); i != sFiles.end(); ++i)
		LoadTypefaces(**i);
}

void
FontCache::LoadTypefaces(const FontFile& f)
{
	if(sFiles.find(&f) == sFiles.end() && !LoadFontFile(f.GetPath().c_str()))
		return;

	const FT_Long t(f.GetFaceN());

	for(FT_Long i(0); i < t; ++i)
	{
		try
		{
			Typeface* q(nullptr);
			FontFamily* r(nullptr);

			try
			{
				try
				{
					if(sFaces.find(q = ynew Typeface(*this, f, i))
						== sFaces.end())
					{
						FTC_FaceID new_face_id(static_cast<FTC_FaceID>(q));

						//读取字型名称并构造名称映射。
						FT_Face face(nullptr);

						FTC_Manager_LookupFace(manager, new_face_id, &face);
						if(!face)
							throw LoggedEvent("Face loading failed.", 2);
						if(!scaler.face_id)
							scaler.face_id = new_face_id;

						auto i(mFamilies.find(face->family_name));

						q->pFontFamily = r = i == mFamilies.end()
							? ynew FontFamily(*this, face->family_name)
							: i->second;
						q->style_name = face->style_name;
						*r += q;
						if(i == mFamilies.end())
							*this += r;
						*this += q;
					}
					else
						throw LoggedEvent("Repeated face is neglected.", 3);
				}
				catch(std::bad_alloc&)
				{
					throw LoggedEvent("Allocation failed"
						" @ FontCache::LoadTypefaces;", 2);
				}
			}
			catch(...)
			{
				scaler.face_id = nullptr;
				ydelete(r);
				ydelete(q);
				throw;
			}
		}
		catch(LoggedEvent& e)
		{
			if(e.GetLevel() < 2)
				throw;
		}
	}
	InitializeDefaultTypeface();
}

bool
FontCache::LoadFontFile(const_path_t path)
{
	try
	{
		if(GetFileNameOf(path) && fexists(path))
		{
			unique_ptr<const FontFile> p(ynew FontFile(path));

			p->ReloadFaces(library);
			if(sFiles.find(raw(p)) != sFiles.end())
			{
				ydelete(p.release());
				return false;
			}
			else
				*this += p.release();
			return true;
		}
	}
	catch(std::bad_alloc&)
	{
		throw LoggedEvent("Allocation failed"
			" @ FontCache::LoadFontFileDirectory;", 2);
	}
	return false;
}

void
FontCache::InitializeDefaultTypeface()
{
	if(!(pDefaultFace || sFaces.empty()))
		pDefaultFace = *sFaces.begin();
}

YSL_END_NAMESPACE(Drawing)

YSL_END

