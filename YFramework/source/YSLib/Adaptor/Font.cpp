/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Font.cpp
\ingroup Adaptor
\brief 平台无关的字体库。
\version r3392
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2009-11-12 22:06:13 +0800
\par 修改时间:
	2014-06-15 14:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Font
*/


#include "Helper/YModules.h"
#include YFM_YSLib_Adaptor_Font
#include YFM_YSLib_Core_YApplication
#include YFM_YSLib_Service_FileSystem
#include YFM_Helper_Initialization
#include YFM_YCLib_Debug
#include <algorithm> // for std::for_each;
#include FT_SIZES_H
#include FT_BITMAP_H
//#include FT_GLYPH_H
//#include FT_OUTLINE_H
//#include FT_SYNTHESIS_H
#include <internal/internal.h> // for FreeType internal macros;
#include FT_INTERNAL_OBJECTS_H // for FT_Face_InternalRec_;
#include FT_INTERNAL_TRUETYPE_TYPES_H // for TT_Face, TT_FaceRec_;

using namespace ystdex;
using namespace platform;

namespace YSLib
{

using namespace IO;

namespace Drawing
{

//! \since build 420
namespace
{

//! \brief 替代 ::FT_Set_Pixel_Sizes 。
::FT_Error
N_SetPixelSizes(::FT_FaceRec& face, ::FT_UInt s) ynothrow
{
	::FT_Size_RequestRec req{FT_SIZE_REQUEST_TYPE_NOMINAL, ::FT_Long(s << 6),
		::FT_Long(s << 6), 0, 0};

	return ::FT_Request_Size(&face, &req);
}

/*!
\since build 421
\see ::FT_GlyphSlot_Oblique 实现。
*/
::FT_Matrix italic_matrix{0x10000L, 0x0366AL, 0x0000L, 0x10000L};

} // unnamed namespace;


NativeFontSize::NativeFontSize(::FT_FaceRec& face, FontSize s)
	: size()
{
	if(const auto err = ::FT_New_Size(&face, &size))
		throw FontException(err, "Native font size creation failed.");
	Activate();
	if(const auto err = N_SetPixelSizes(face, s))
		throw FontException(err, "Native font setting size failed.");
}
NativeFontSize::NativeFontSize(NativeFontSize&& ns) ynothrow
	: size(ns.size)
{
	ns.size = {};
}
NativeFontSize::~NativeFontSize()
{
	::FT_Done_Size(size);
}

::FT_SizeRec&
NativeFontSize::GetSizeRec() const
{
	if(YB_UNLIKELY(!size))
		throw LoggedEvent("Invalid native size found.", Critical);
	return *size;
}

void
NativeFontSize::Activate() const
{
	YAssertNonnull(size);

	auto face(size->face);

	YAssertNonnull(face);
	face->size = size;
}


FontFamily::FontFamily(FontCache& cache, const FamilyName& name)
	: Cache(cache), family_name(name), mFaces()
{}

void
FontFamily::operator+=(Typeface& face)
{
	mFaces.emplace(face.GetStyleName(), &face);
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
		throw LoggedEvent("No matching face found.", Critical);
	return *p;
}
Typeface&
FontFamily::GetTypefaceRef(const StyleName& style_name) const
{
	const auto p(GetTypefacePtr(style_name));

	if(YB_UNLIKELY(!p))
		throw LoggedEvent("No matching face found.", Critical);
	return *p;
}


Typeface::SmallBitmapData::SmallBitmapData(::FT_GlyphSlot slot, FontStyle style)
{
	if(slot && slot->format == FT_GLYPH_FORMAT_BITMAP)
	{
		auto& bitmap(slot->bitmap);

		if(bool(style & FontStyle::Bold))
		{
			const auto library(slot->library);
			const auto face(slot->face);
			::FT_Pos xstr(FT_MulFix(face->units_per_EM,
				face->size->metrics.y_scale) / 24 & ~63), ystr(xstr);

			if(xstr == 0)
				xstr = 1 << 6;
			if(::FT_GlyphSlot_Own_Bitmap(slot) == FT_Err_Ok
				&& ::FT_Bitmap_Embolden(library, &bitmap, xstr, ystr)
				== FT_Err_Ok)
			{
				if(slot->advance.x)
					slot->advance.x += xstr;
				if(slot->advance.y)
					slot->advance.y += ystr;
				{
					auto& metrics(slot->metrics);

					yunseq(metrics.width += xstr, metrics.height += ystr,
						metrics.horiAdvance += xstr,
						metrics.vertAdvance += ystr);
				}
				slot->bitmap_top += ::FT_Int(ystr >> 6);
			}
		}

		const ::FT_Pos xadvance((slot->advance.x + 32) >> 6),
			yadvance((slot->advance.y + 32) >> 6);
		::FT_Int temp;

#define SBIT_CHECK_CHAR(d) (temp = ::FT_Char(d), temp == d)
#define SBIT_CHECK_BYTE(d) (temp = ::FT_Byte(d), temp == d)
		if(SBIT_CHECK_BYTE(bitmap.rows) && SBIT_CHECK_BYTE(bitmap.width)
			&& SBIT_CHECK_CHAR(bitmap.pitch)
			&& SBIT_CHECK_CHAR(slot->bitmap_left)
			&& SBIT_CHECK_CHAR(slot->bitmap_top) && SBIT_CHECK_CHAR(xadvance)
			&& SBIT_CHECK_CHAR(yadvance))
		{
			sbit = {::FT_Byte(bitmap.width), ::FT_Byte(bitmap.rows),
				::FT_Char(slot->bitmap_left), ::FT_Char(slot->bitmap_top),
				::FT_Byte(bitmap.pixel_mode), ::FT_Byte(bitmap.num_grays - 1),
				::FT_Char(bitmap.pitch), ::FT_Char(xadvance),
				::FT_Char(yadvance), bitmap.buffer};
			bitmap.buffer = {};
			// XXX: Moving instead of copying should be safe if the library
			//	memory handlers are not customized.
			// NOTE: Be cautious for DLLs. For documented default behavior, see:
			//	http://www.freetype.org/freetype2/docs/design/design-4.html .
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
	// NOTE: See constructor.
	std::free(sbit.buffer);
}


Typeface::Typeface(FontCache& cache, const FontPath& path, u32 i)
	: Path(path), face_index(i), cmap_index(-1), style_name(), ref([&, this]{
		if(YB_UNLIKELY(ystdex::exists(cache.sFaces, this)))
			throw LoggedEvent("Duplicate typeface found.", Critical);

		::FT_Face face;
		auto error(::FT_New_Face(cache.library, Path.c_str(), face_index,
			&face));

		if(YB_LIKELY(!error))
			if(YB_LIKELY(!(error = ::FT_Select_Charmap(face,
				FT_ENCODING_UNICODE)) && face))
				cmap_index = face->charmap
					? ::FT_Get_Charmap_Index(face->charmap) : 0;
		if(YB_UNLIKELY(error))
		{
			platform::yprintf("Face request error: %08x\n", error);
			throw LoggedEvent("Face loading failed.", Critical);
		}

		const FamilyName family_name(face->family_name);
		auto& p_ff(cache.mFamilies[family_name]);

		if(!p_ff)
			p_ff.reset(new FontFamily(cache, family_name));
		return pair<std::reference_wrapper<FontFamily>,
			std::reference_wrapper<::FT_FaceRec_>>(*p_ff.get(), *face);
	}()), bitmap_cache(2047U), glyph_index_cache()
{
	YAssert(::FT_UInt(cmap_index) < ::FT_UInt(ref.second.get().num_charmaps),
		"Invalid CMap index found.");
	style_name = ref.second.get().style_name;
	ref.first.get() += *this;
}
Typeface::~Typeface()
{
	size_cache.clear();
	glyph_index_cache.clear();
	bitmap_cache.clear();
	ref.first.get() -= *this;

	const auto face(&ref.second.get());

	YAssertNonnull(face);
	YAssert(face->internal->refcount == 1,
		"Invalid face reference count found.");
	// XXX: Hack for using %ttmtx.c and %sfobjs.c of FreeType 2.4.11.
	if(FT_IS_SFNT(face))
	{
		const auto ttface(reinterpret_cast<::TT_Face>(face));

		// NOTE: See %Typeface::SmallBitmapData::SmallBitmapData.
		// NOTE: %sfnt_done_face in "sfobjs.c" still releases vertical metrics.
		std::free(ttface->horizontal.long_metrics),
		std::free(ttface->horizontal.short_metrics);
	}
	::FT_Done_Face(face);
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
	return CacheLookup(bitmap_cache, key, [&]{
		LookupSize(key.Size).Activate();
		::FT_Set_Transform(&ref.second.get(),
			bool(key.Style & FontStyle::Italic) ? &italic_matrix : nullptr, {});

		return SmallBitmapData(::FT_Load_Glyph(&ref.second.get(),
			key.GlyphIndex, key.Flags | FT_LOAD_RENDER) == 0
			? ref.second.get().glyph : nullptr, key.Style);
	});
}

::FT_UInt
Typeface::LookupGlyphIndex(ucs4_t c) const
{
	auto i(glyph_index_cache.find(c));

	if(i == glyph_index_cache.end())
	{
		if(cmap_index > 0)
			::FT_Set_Charmap(&ref.second.get(),
				ref.second.get().charmaps[cmap_index]);

		const auto pr(glyph_index_cache.emplace(c, ::FT_Get_Char_Index(
			&ref.second.get(), ::FT_ULong(c))));

		if(YB_UNLIKELY(!pr.second))
			throw LoggedEvent("Glyph index cache insertion failed.", Alert);
		i = pr.first;
	}
	return i->second;
}

NativeFontSize&
Typeface::LookupSize(FontSize s) const
{
	auto i(size_cache.find(s));

	if(i == size_cache.end())
	{
		const auto pr(size_cache.emplace(s, NativeFontSize(ref.second, s)));

		if(YB_UNLIKELY(!pr.second))
			throw LoggedEvent("Bitmap cache insertion failed.", Alert);
		i = pr.first;
	}
	return i->second;
}


const Typeface&
FetchDefaultTypeface()
{
	const Typeface* const pDefaultTypeface(
		FetchDefaultFontCache().GetDefaultTypefacePtr());

	if(YB_UNLIKELY(!pDefaultTypeface))
		throw LoggedEvent("Null default font face pointer found.", Critical);
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
			ystdex::sfmt("Font init failed: %08x\n;", error).c_str(), Alert);
	}
}
FontCache::~FontCache()
{
	ClearContainers();
	::FT_Done_FreeType(library);
}

const FontFamily*
FontCache::GetFontFamilyPtr(const FamilyName& family_name) const
{
	const auto i(mFamilies.find(family_name));

	return (i == mFamilies.cend()) ? nullptr : i->second.get();
}

const Typeface*
FontCache::GetDefaultTypefacePtr() const
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
		return {};
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
		typeface.LookupGlyphIndex(c), font_size, style}).sbit;
}
FontSize
Font::GetHeight() const ynothrow
{
	return GetInternalInfo().height >> 6;
}
::FT_Size_Metrics
Font::GetInternalInfo() const
{
	return GetTypeface().LookupSize(GetSize()).GetSizeRec().metrics;
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
		yunseq(typeface = std::ref(*p), style = fs);
		return true;
	}
	return false;
}

} // namespace Drawing;

} // namespace YSLib;

