/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfont.cpp
\ingroup Adaptor
\brief 平台无关的字体缓存库。
\version 0.7047;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 22:06:13 + 08:00;
\par 修改时间:
	2010-12-08 20:19 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YFontCache;
*/


#include "yfont.h"

#include "../Core/yapp.h"
#include "../Core/yexcept.h"
//#include FT_OUTLINE_H
//#include FT_SYNTHESIS_H

using namespace stdex;
using namespace platform;

YSL_BEGIN

using namespace IO;

YSL_BEGIN_NAMESPACE(Drawing)

const char*
EFontStyle::GetName() const ythrow()
{
	const char* ss(NULL);

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
		"EFontStyle::GetName() const\":\n"
		"Font style name string is null.");

	return ss;
}


/*!
\brief 供 FreeType 使用的客户端字体查询函数。
从由 face_id 提供的参数对应的字体文件中读取字体，写入 aface 。
*/
FT_Error
simpleFaceRequester(FTC_FaceID face_id, FT_Library library,
					FT_Pointer request_data, FT_Face* aface)
{
	Typeface* fontFace(static_cast<Typeface*>(face_id));
	FT_Face& face(*aface);
	FT_Error error(FT_New_Face(library, fontFace->File.GetPath(),
		fontFace->faceIndex, aface));

	if(!error)
	{
		error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		if(!error && face)
		{
			fontFace->cmapIndex = face->charmap
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
//	FT_Set_Transform(face, &fontFace->matrix, NULL);
	if(error)
		yprintf("Font fontFace req. error: %08x\n", error);
	return error;
}


FontFamily::FontFamily(YFontCache& cache, const FT_String* name)
	: Cache(cache), family_name(strdup_n(name)), sTypes()
{}
FontFamily::~FontFamily() ythrow()
{
	std::free(family_name);
}

bool
FontFamily::operator==(const FontFamily& rhs) const
{
	return &Cache == &rhs.Cache
		&& (family_name == rhs.family_name
		|| (family_name && rhs.family_name
		&& !strcmp(family_name, rhs.family_name)));
}
bool
FontFamily::operator<(const FontFamily& rhs) const
{
	return &Cache < &rhs.Cache
		|| (ReferenceEquals(Cache, rhs.Cache)
		&& family_name && rhs.family_name
		&& strcmp(family_name, rhs.family_name) < 0);
}

Typeface*
FontFamily::GetTypefacePtr(EFontStyle s) const
{
	Typeface* const p(GetTypefacePtr(s.GetName()));

	return p ? p
		: (s == EFontStyle::Regular ? NULL : GetTypefacePtr("Regular"));
}
Typeface*
FontFamily::GetTypefacePtr(const FT_String* style_name) const
{
	const FTypesIndex::const_iterator i(mTypesIndex.find(style_name));

	return (i == mTypesIndex.end()) ? NULL : i->second;
}

void
FontFamily::operator+=(Typeface& f)
{
	sTypes.insert(&f);
	mTypesIndex.insert(std::make_pair(f.GetStyleName(), &f));
}
bool
FontFamily::operator-=(Typeface& f)
{
	return mTypesIndex.erase(f.GetStyleName()) != 0 && sTypes.erase(&f) != 0;
}


/*const FT_Matrix Typeface::MNormal = {0x10000, 0, 0, 0x10000},
	Typeface::MOblique = {0x10000, 0x5800, 0, 0x10000};*/

Typeface::Typeface(YFontCache& cache, const FontFile& file, u32 i
	/*, const bool bb, const bool bi, const bool bu*/)
	: Cache(cache), File(file), pFontFamily(NULL),
	style_name(NULL), faceIndex(i), cmapIndex(-1)
/*	, bBold(bb), bOblique(bi), bUnderline(bu),
	, matrix(bi ? MOblique : MNormal)*/
{}
Typeface::~Typeface() ythrow()
{
	free(style_name);
}

bool
Typeface::operator==(const Typeface& rhs) const
{
	return &File == &rhs.File && faceIndex == rhs.faceIndex;
}
bool
Typeface::operator<(const Typeface& rhs) const
{
	return &File < &rhs.File
		|| (&File == &rhs.File && faceIndex < rhs.faceIndex);
}


FontFile::FontFile(CPATH p, FT_Library& l)
	: path(strdup_n(p)), library(l), nFace(0)
{
	ReloadFaces();
}
FontFile::FontFile(CPATH p, const char* n, FT_Library& l)
	: path(strcatdup(p, n)), library(l)
{
	ReloadFaces();
}

bool
FontFile::operator==(const FontFile& rhs) const
{
	return &library == &rhs.library
		&& (!(path && rhs.path) || !strncmp(path, rhs.path, MaxFontPathLength));
}
bool
FontFile::operator<(const FontFile& rhs) const
{
	return &library < &rhs.library
		|| (&library == &rhs.library
		&& path && rhs.path
		&& strncmp(path, rhs.path, MaxFontPathLength) < 0);
}

void
FontFile::ReloadFaces()
{
	FT_Face face(NULL);

	if(FT_New_Face(library, path, -1, &face))
	{
		nFace = -1;
		return;
	}
	nFace = face->num_faces;
	FT_Done_Face(face);
}


const Typeface*
GetDefaultTypefacePtr() ythrow()
{
	YAssert(theApp.pFontCache,
		"In function \"const Typeface*\n"
		"GetDefaultTypefacePtr()\": \n"
		"The default font cache pointer is null.");

	return theApp.pFontCache->GetDefaultTypefacePtr();
}

const FontFamily&
GetDefaultFontFamily() ythrow()
{
	const Typeface* pDefaultTypeface(GetDefaultTypefacePtr());

	YAssert(pDefaultTypeface,
		"In function \"inline const FontFamily&\n"
		"GetDefaultFontFamily()\": \n"
		"The default font face pointer is null.");

	const FontFamily* pFontFamily(pDefaultTypeface->GetFontFamilyPtr());

	YAssert(pFontFamily,
		"In function \"inline const FontFamily&\n"
		"GetDefaultFontFamily()\": \n"
		"The default font family pointer is null.");
	return *pFontFamily;
}


const Font::SizeType
Font::DefSize(16), // DEF_SHELL_FONTSIZE,
Font::MinSize(4), // DEF_SHELL_FONTSIZE_MIN,
Font::MaxSize(72); // DEF_SHELL_FONTSIZE_MAX;
Font* Font::pDefFont(NULL);

Font::Font(const FontFamily& family, const SizeType size, EFontStyle style)
	: pFontFamily(&family), Style(style), Size(size)
{}

Font::SizeType Font::GetHeight() const
{
	return GetCache().GetHeight();
}

void
Font::SetSize(Font::SizeType s)
{
	if(s >= MinSize && s <= MaxSize)
		Size = s;
}

bool
Font::Update()
{
	Typeface* t(pFontFamily->GetTypefacePtr(Style));

	if(!t)
		return false;
	if(t != GetCache().GetTypefacePtr())
	{
		if(GetCache().SetTypeface(t))
			UpdateSize();
		else
			return false;
	}
	else if(GetCache().GetFontSize() != Size)
		UpdateSize();
	else
		return false;
	return true;
}

void
Font::UpdateSize()
{
	GetCache().SetFontSize(Size);
}

bool
Font::InitializeDefault()
{
	try
	{
		if(!pDefFont)
			pDefFont = ynew Font();
	}
	catch(...)
	{
		return false;
	}
	return true;
}

void
Font::ReleaseDefault()
{
	safe_delete_obj()(pDefFont);
}


YFontCache::YFontCache(CPATH defFontPath, u32 cacheSize)
	: YObject(),
	pFace(NULL), curSize(0)
{
	FT_Error error(FT_Init_FreeType(&library));

	if(error)
		goto err;
	error = FTC_Manager_New(library, 0, 0, cacheSize,
		&simpleFaceRequester, NULL, &manager);
	if(error)
		goto err;
	error = FTC_SBitCache_New(manager, &sbitCache);
	if(error)
		goto err;
	error = FTC_CMapCache_New(manager, &cmapCache);
	if(error)
		goto err;
	LoadFontFile(defFontPath);
	if(!sTypes.empty())
		SetTypeface(*sTypes.begin());
err:
	if(error)
		yprintf("Font init failed: %08x\n", error);
}
YFontCache::~YFontCache()
{
	scaler.face_id = NULL;
	FTC_Manager_Done(manager);
	FT_Done_FreeType(library);
	ClearContainers();
}

const FontFamily*
YFontCache::GetFontFamilyPtr(const FT_String* family_name) const
{
	const FFacesIndex::const_iterator i(mFacesIndex.find(family_name));

	return (i == mFacesIndex.end()) ? NULL : i->second;
}
/*Typeface* YFontCache::GetTypefacePtr(u16 i) const
{
	return i < sTypes.size() ? sTypes[i] : NULL;
}*/
const Typeface*
YFontCache::GetDefaultTypefacePtr() const
{
	//默认字体缓存的默认字型指针由初始化保证为非空指针。
	return pDefaultFace ? pDefaultFace
		: theApp.pFontCache->GetDefaultTypefacePtr();
}
const Typeface*
YFontCache::GetTypefacePtr(const FT_String* family_name,
	const FT_String* style_name) const
{
	const FontFamily* f(GetFontFamilyPtr(family_name));

	if(!f)
		return NULL;
	return f->GetTypefacePtr(style_name);
}
CharBitmap
YFontCache::GetGlyph(fchar_t c)
{
	if(!scaler.face_id)
		return NULL;

	const u32 flags(FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL);
	const u32 index(FTC_CMapCache_Lookup(cmapCache, scaler.face_id,
		pFace->cmapIndex, c));

	FTC_SBitCache_LookupScaler(sbitCache, &scaler, flags, index, &sbit, NULL);
	return sbit;
}
s8
YFontCache::GetAdvance(fchar_t c, FTC_SBit sbit)
{
	if(c == '\t')
		return GetAdvance(' ') << 2;
	if(!sbit)
		sbit = GetGlyph(c);
	return sbit->xadvance;
}
FT_Face
YFontCache::GetInternalFaceInfo() const
{
	FT_Face face(NULL);

	FTC_Manager_LookupFace(manager, scaler.face_id, &face);
	return face;
}
u8
YFontCache::GetHeight() const
{
	return GetInternalFaceInfo()->size->metrics.height >> 6;
}
s8
YFontCache::GetAscender() const
{
	return GetInternalFaceInfo()->size->metrics.ascender >> 6;
}
s8
YFontCache::GetDescender() const
{
	return GetInternalFaceInfo()->size->metrics.descender >> 6;
}

bool
YFontCache::SetTypeface(Typeface* p)
{
	if(!p || sTypes.find(p) == sTypes.end())
		return false;
	pFace = p;
	scaler.face_id = reinterpret_cast<FTC_FaceID>(p);
	return true;
}
void
YFontCache::SetFontSize(Font::SizeType s)
{
	if(s == 0)
		s = Font::DefSize;
//	if(s != curSize)
//	ClearCache();
	curSize = s;
	scaler.width  = static_cast<FT_UInt>(curSize);
	scaler.height = static_cast<FT_UInt>(curSize);
	scaler.pixel  = 72;
	scaler.x_res  = 0;
	scaler.y_res  = 0;
	GetGlyph(' '); //更新当前字形，否则 GetHeight() 会返回错误的值。
}

void
YFontCache::operator+=(const FontFile& f)
{
	sFiles.insert(&f);
}
bool
YFontCache::operator-=(const FontFile& f)
{
	return sFiles.erase(&f);
}

void
YFontCache::operator+=(Typeface& f)
{
	sTypes.insert(&f);
}
bool
YFontCache::operator-=(Typeface& f)
{
	return sTypes.erase(&f);
}

void
YFontCache::operator+=(FontFamily& f)
{
	sFaces.insert(&f);
	mFacesIndex.insert(std::make_pair(f.family_name, &f));
}
bool
YFontCache::operator-=(FontFamily& f)
{
	return mFacesIndex.erase(f.family_name) != 0 && sFaces.erase(&f) != 0;
}

void
YFontCache::LoadTypefaces()
{
	for(FFiles::iterator i(sFiles.begin()); i != sFiles.end(); ++i)
		LoadTypefaces(**i);
}

void
YFontCache::LoadTypefaces(const FontFile& f)
{
	const FT_Long t(f.GetFaceN());

	for(FT_Long i(0); i < t; ++i)
	{
		try
		{
			Typeface* q(NULL);
			FontFamily* r(NULL);

			try
			{
				try
				{
					if(sTypes.find(q = ynew Typeface(*this, f, i))
						== sTypes.end())
					{
						scaler.face_id = static_cast<FTC_FaceID>(q);

						//读取字型名称并构造名称映射。
						FT_Face face(GetInternalFaceInfo());

						if(!(face && face->family_name))
							throw LoggedEvent("Face loading failed.", 2);

						FFacesIndex::iterator i(mFacesIndex.find(
							face->family_name));

						q->pFontFamily = r = i == mFacesIndex.end()
							? ynew FontFamily(*this, face->family_name)
							: i->second;
						*r += *q;
						if(i == mFacesIndex.end())
							*this += *r;
						q->style_name = strdup_n(face->style_name);
						*this += *q;
					}
					else
						throw LoggedEvent("Repeated face is neglected.", 3);
				}
				catch(std::bad_alloc&)
				{
					throw LoggedEvent("Allocation failed"
						" @@ YFontCache::LoadTypefaces(const FontFile&);", 2);
				}
			}
			catch(...)
			{
				scaler.face_id = NULL;
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
	if(!(pDefaultFace || sTypes.empty()))
		pDefaultFace = *sTypes.begin();
}

void
YFontCache::LoadFontFileDirectory(CPATH path, CPATH ext)
{
	{
		HDirectory dir(path);

		if(dir.IsValid())
			while((++dir).LastError == 0)
				if(std::strcmp(HDirectory::Name, FS_Now) != 0
					&& !HDirectory::IsDirectory()
					&& IsExtendNameOf(ext, HDirectory::Name))
				{
					try
					{
						std::auto_ptr<const FontFile> p(ynew FontFile(path,
							HDirectory::Name, library));
						if(sFiles.find(p.get()) != sFiles.end())
							ydelete(p.release());
						else
						{
							*this += *p;
							p.release();
						}
					}
					catch(std::bad_alloc&)
					{
						throw LoggedEvent("Allocation failed"
							" @@ YFontCache::"
							"LoadFontFileDirectory(CPATH, CPATH);", 2);
					}
				}
	}
	LoadTypefaces();
	if(pDefaultFace)
		SetTypeface(pDefaultFace);
}

void
YFontCache::LoadFontFile(CPATH path) ythrow()
{
	try
	{
		if(GetFileNameFrom(path) && fexists(path))
		{
			std::auto_ptr<const FontFile> p(ynew FontFile(path, library));

			if(sFiles.find(p.get()) != sFiles.end())
				ydelete(p.release());
			else
			{
				LoadTypefaces(*p);
				*this += *p;
				p.release();
				if(pDefaultFace)
					SetTypeface(pDefaultFace);
			}
		}
		else
			LoadFontFileDirectory(path);
	}
	catch(...)
	{}
}

void
YFontCache::ClearFontFiles()
{
	for_each(sFiles.begin(), sFiles.end(), delete_obj());
	sFiles.clear();
}

void
YFontCache::ClearTypefaces()
{
	for_each(sTypes.begin(), sTypes.end(), delete_obj());
	sTypes.clear();
}

void
YFontCache::ClearFontFamilies()
{
	for(FFacesIndex::const_iterator i(mFacesIndex.begin());
			i != mFacesIndex.end(); ++i)
		ydelete(i->second);
	mFacesIndex.clear();
}

void
YFontCache::ClearContainers()
{
	ClearFontFiles();
	ClearTypefaces();
	ClearFontFamilies();
}

void
YFontCache::ClearCache()
{
	ClearContainers();
	FTC_Manager_Reset(manager);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

