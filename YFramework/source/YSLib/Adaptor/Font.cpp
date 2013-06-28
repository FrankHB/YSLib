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
\version r3184
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2009-11-12 22:06:13 +0800
\par 修改时间:
	2013-06-29 06:08 +0800
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
#include FT_SIZES_H
//#include FT_BITMAP_H
//#include FT_GLYPH_H
//#include FT_OUTLINE_H
//#include FT_SYNTHESIS_H

using namespace ystdex;
using namespace platform;

YSL_BEGIN

using namespace IO;

YSL_BEGIN_NAMESPACE(Drawing)

NativeFontSize::NativeFontSize(::FT_Face face, FontSize s)
	: size()
{
	if(const auto err = ::FT_New_Size(face, &size))
		throw FontException(err, "Native font size creation failed.");
	::FT_Activate_Size(size);
	if(const auto err = ::FT_Set_Pixel_Sizes(face, s, s))
		throw FontException(err, "Native font setting size failed.");
}
NativeFontSize::NativeFontSize(NativeFontSize&& ns) ynothrow
	: size(ns.size)
{
	ns.size = {};
}
NativeFontSize::~NativeFontSize() ynothrow
{
	::FT_Done_Size(size);
}

::FT_SizeRec&
NativeFontSize::GetSizeRec() const
{
	if(YB_UNLIKELY(!size))
		throw LoggedEvent("Invalid native size found.");
	return *size;
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
Typeface&
FontFamily::GetTypefaceRef(FontStyle fs) const
{
	const auto p(GetTypefacePtr(fs));

	if(YB_UNLIKELY(!p))
		throw LoggedEvent("No matching face found.");
	return *p;
}
Typeface&
FontFamily::GetTypefaceRef(const StyleName& style_name) const
{
	const auto p(GetTypefacePtr(style_name));

	if(YB_UNLIKELY(!p))
		throw LoggedEvent("No matching face found.");
	return *p;
}


Typeface::SmallBitmapData::SmallBitmapData(::FT_GlyphSlot slot)
{
	if(slot && slot->format == FT_GLYPH_FORMAT_BITMAP)
	{
		const auto& bitmap(slot->bitmap);
		const ::FT_Pos xadvance((slot->advance.x + 32) >> 6),
			yadvance((slot->advance.y + 32) >> 6);
		::FT_Int temp;

#define SBIT_CHECK_CHAR(d) (temp = ::FT_Char(d), temp == d)
#define SBIT_CHECK_BYTE(d) (temp = ::FT_Byte(d), temp == d)
		if(SBIT_CHECK_BYTE(bitmap.rows) &&
			SBIT_CHECK_BYTE(bitmap.width) &&
			SBIT_CHECK_CHAR(bitmap.pitch) &&
			SBIT_CHECK_CHAR(slot->bitmap_left) &&
			SBIT_CHECK_CHAR(slot->bitmap_top) &&
			SBIT_CHECK_CHAR(xadvance) &&
			SBIT_CHECK_CHAR(yadvance))
		{
			const ::FT_ULong size(std::abs(bitmap.pitch) * bitmap.rows);

			sbit = {::FT_Byte(bitmap.width), ::FT_Byte(bitmap.rows),
				::FT_Char(slot->bitmap_left), ::FT_Char(slot->bitmap_top),
				::FT_Byte(bitmap.pixel_mode), ::FT_Byte(bitmap.num_grays - 1),
				::FT_Char(bitmap.pitch), ::FT_Char(xadvance),
				::FT_Char(yadvance), ynew ::FT_Byte[size]};
			std::memcpy(sbit.buffer, bitmap.buffer, size);
			return;
		}
#undef SBIT_CHECK_CHAR
#undef SBIT_CHECK_BYTE
	}
	sbit = {255, 0, 0, 0, 0, 0, 0, 0, 0, nullptr};
}
Typeface::SmallBitmapData::SmallBitmapData(SmallBitmapData&& sbit_dat)
	: sbit(sbit_dat.sbit)
{
	sbit_dat.sbit.buffer = {};
}
Typeface::SmallBitmapData::~SmallBitmapData()
{
	ydelete_array(sbit.buffer);
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

Typeface::SmallBitmapData&
Typeface::LookupBitmap(const Typeface::BitmapKey& key) const
{
	auto i(bitmap_cache.find(key));

	if(i == bitmap_cache.end())
	{
		NativeFontSize native_size(face, key.Size);
		const auto pr(bitmap_cache.emplace(key, SmallBitmapData(::FT_Load_Glyph(
			face, key.GlyphIndex, key.Flags | FT_LOAD_RENDER) == 0
			? face->glyph : nullptr)));

		if(YB_UNLIKELY(!pr.second))
			throw LoggedEvent("Bitmap cache insertion failed.");
		i = pr.first;
	}
	return i->second;
}

::FT_UInt
Typeface::LookupGlyphIndex(ucs4_t c) const
{
	auto i(glyph_index_cache.find(c));

	if(i == glyph_index_cache.end())
	{
		if(cmap_index > 0)
			::FT_Set_Charmap(face, face->charmaps[cmap_index]);

		const auto pr(glyph_index_cache.emplace(c, ::FT_Get_Char_Index(face,
			::FT_ULong(c))));

		if(YB_UNLIKELY(!pr.second))
			throw LoggedEvent("Glyph index cache insertion failed.");
		i = pr.first;
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


FontCache::FontCache(size_t /*cache_size*/)
	: pDefaultFace()
{
	::FT_Error error;

	if(YB_LIKELY((error = ::FT_Init_FreeType(&library)) == 0))
		// TODO: Write log on success.
		;
	else
	{
		// TODO: Format without allocating memory.
		throw LoggedEvent(
			ystdex::sfmt("Font init failed: %08x\n;", error).c_str(), 1);
	}
}
FontCache::~FontCache()
{
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

void
FontCache::operator+=(unique_ptr<FontFamily> p_family)
{
	mFamilies.emplace(p_family->GetFamilyName(), std::move(p_family));
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


Font::Font(const FontFamily& family, const FontSize size, FontStyle fs)
	: typeface(family.GetTypefaceRef(fs)), font_size(size), style(fs)
{}

s8
Font::GetAdvance(ucs4_t c, ::FTC_SBit sbit) const
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
	return GetInternalInfo().ascender >> 6;
}
s8
Font::GetDescender() const
{
	return GetInternalInfo().descender >> 6;
}
CharBitmap
Font::GetGlyph(ucs4_t c, ::FT_UInt flags) const
{
	const auto& typeface(GetTypeface());

	return &typeface.LookupBitmap(Typeface::BitmapKey{flags,
		typeface.LookupGlyphIndex(c), font_size}).sbit;
}
FontSize
Font::GetHeight() const ynothrow
{
	return GetInternalInfo().height >> 6;
}
::FT_Size_Metrics
Font::GetInternalInfo() const
{
	return NativeFontSize(GetTypeface().face, GetSize()).GetSizeRec().metrics;
}

void
Font::SetSize(FontSize s)
{
	if(YB_LIKELY(s >= MinimalSize && s <= MaximalSize))
		font_size = s;
}
bool
Font::SetStyle(FontStyle fs)
{
	auto p(GetFontFamily().GetTypefacePtr(fs));

	if(p)
	{
		typeface = std::ref(*p);
		return true;
	}
	return false;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

