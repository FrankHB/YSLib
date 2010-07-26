// YSLib::Adapter::YFontCache by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:06:13;
// UTime = 2010-7-26 6:06;
// Version = 0.6756;


#include "yfont.h"

#include "../Core/yfilesys.h"
//#include "../Helper/yglobal.h"
//#include FT_OUTLINE_H
//#include FT_SYNTHESIS_H

using namespace stdex;
using namespace platform;

YSL_BEGIN

using namespace IO;
using namespace Exceptions;

YSL_BEGIN_NAMESPACE(Drawing)

const char*
EFontStyle::GetName() const
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
	YAssert(ss != NULL,
		"In function \"const char*\n"
		"EFontStyle::GetName() const\":\n"
		"Font style name string is null.");
	return ss;
}


//供 FreeType 使用的客户端字体查询函数，从由 face_id 提供的参数对应的字体文件中读取字体，写入 aface 。
FT_Error
simpleFaceRequester(FTC_FaceID face_id, FT_Library library,
					FT_Pointer request_data, FT_Face* aface)
{
	MTypeface* fontFace(static_cast<MTypeface*>(face_id));
	FT_Face& face(*aface);
	FT_Error error(FT_New_Face(library, fontFace->File.GetPath(), fontFace->faceIndex, aface));

	if(!error)
	{
		error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
		if(!error && face)
		{
			fontFace->cmapIndex = face->charmap ? FT_Get_Charmap_Index(face->charmap) : 0;
		/*	fontFace->nGlyphs = face->num_glyphs;
			fontFace->uUnitPerEM = face->units_per_EM;
			fontFace->nCharmaps = face->num_charmaps;*/
		//	if(FT_IS_SCALABLE(face))
		//		fontFace->lUnderlinePos = FT_MulFix(face->underline_position, face->size->metrics.y_scale) >> 6;
		/*	fontFace->fixSizes.clear();
			if(face->available_sizes)
			{
				FT_Int t = face->num_fixed_sizes;

				fontFace->fixSizes.reserve(t);
				for(FT_Int i = 0; i < t; ++i)
					fontFace->fixSizes.push_back(face->available_sizes[i].size >> 6);
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


MFontFamily::MFontFamily(YFontCache& cache, const FT_String* name)
: Cache(cache), family_name(strdup_n(name)), sTypes()
{
}
MFontFamily::~MFontFamily()
{
	free(family_name);
}

bool
MFontFamily::operator==(const MFontFamily& rhs) const
{
	return &Cache == &rhs.Cache && (family_name == rhs.family_name || (family_name && rhs.family_name && !strcmp(family_name, rhs.family_name)));
}
bool
MFontFamily::operator<(const MFontFamily& rhs) const
{
	return &Cache < &rhs.Cache || (ReferenceEquals(Cache, rhs.Cache) && family_name && rhs.family_name && strcmp(family_name, rhs.family_name) < 0);
}

const MTypeface*
MFontFamily::GetTypefacePtr(EFontStyle s) const
{
	const MTypeface* p(GetTypefacePtr(s.GetName()));

	return p ? p : (s == EFontStyle::Regular ? NULL : GetTypefacePtr("Regular"));
}
const MTypeface*
MFontFamily::GetTypefacePtr(const FT_String* style_name) const
{
	const FTypesIndex::const_iterator i(mTypesIndex.find(style_name));

	return (i == mTypesIndex.end()) ? NULL : i->second;
}

void
MFontFamily::operator+=(const MTypeface& f)
{
	sTypes.insert(&f);
	mTypesIndex.insert(std::make_pair(f.GetStyleName(), &f));
}
bool
MFontFamily::operator-=(const MTypeface& f)
{
	return mTypesIndex.erase(f.GetStyleName()) != 0 && sTypes.erase(&f) != 0;
}


/*const FT_Matrix MTypeface::MNormal = {0x10000, 0, 0, 0x10000},
	MTypeface::MOblique = {0x10000, 0x5800, 0, 0x10000};*/

MTypeface::MTypeface(YFontCache& cache, const MFontFile& file, u32 i/*, const bool bb, const bool bi, const bool bu*/)
: Cache(cache), File(file), pFontFamily(NULL), style_name(NULL), faceIndex(i), cmapIndex(-1)/*, bBold(bb), bOblique(bi), bUnderline(bu), matrix(bi ? MOblique : MNormal)*/
{}
MTypeface::~MTypeface()
{
	free(style_name);
}

bool
MTypeface::operator==(const MTypeface& rhs) const
{
	return &File == &rhs.File && faceIndex == rhs.faceIndex;
}
bool
MTypeface::operator<(const MTypeface& rhs) const
{
	return &File < &rhs.File || (&File == &rhs.File && faceIndex < rhs.faceIndex);
}


MFontFile::MFontFile(CPATH p, FT_Library& l)
: path(strdup_n(p)), library(l), nFace(0)
{
	ReloadFaces();
}
MFontFile::MFontFile(CPATH p, const char* n, FT_Library& l)
: path(strcatdup(p, n)), library(l)
{
	ReloadFaces();
}

bool
MFontFile::operator==(const MFontFile& rhs) const
{
	return &library == &rhs.library && (!(path && rhs.path) || !strncmp(path, rhs.path, MaxFontPathLength));
}
bool
MFontFile::operator<(const MFontFile& rhs) const
{
	return &library < &rhs.library || (&library == &rhs.library && path && rhs.path && strncmp(path, rhs.path, MaxFontPathLength) < 0);
}

void
MFontFile::ReloadFaces()
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


const MTypeface*
GetDefaultTypefacePtr()
{
	YAssert(pDefaultFontCache != NULL,
		"In function \"const MTypeface*\n"
		"GetDefaultTypefacePtr()\": \n"
		"The default font cache pointer is null.");
	return pDefaultFontCache->GetDefaultTypefacePtr();
}


const MFont::SizeType
MFont::DefSize(16), //DEF_SHELL_FONTSIZE,
MFont::MinSize(4), //DEF_SHELL_FONTSIZE_MIN,
MFont::MaxSize(72); //DEF_SHELL_FONTSIZE_MAX;
MFont* MFont::pDefFont(NULL);

MFont::MFont(const MFontFamily& family, const SizeType size, EFontStyle style)
: pFontFamily(&family), Style(style), Size(size)
{
}

MFont::SizeType MFont::GetHeight() const
{
	return GetCache().GetHeight();
}

void
MFont::SetSize(MFont::SizeType s)
{
	if(s >= MinSize && s <= MaxSize)
		Size = s;
}

bool
MFont::Update()
{
	const MTypeface* t(pFontFamily->GetTypefacePtr(Style));
	bool b(false);

	if(t != NULL)
		b = GetCache().SetTypeface(t);
	return b || UpdateSize();
}
bool
MFont::UpdateSize()
{
	return GetCache().SetFontSize(Size);
}

bool
MFont::InitializeDefault()
{
	try
	{
		if(pDefFont == NULL)
			pDefFont = new MFont();
	}
	catch(...)
	{
		return false;
	}
	return pDefFont != NULL;
}
void
MFont::ReleaseDefault()
{
	if(!pDefaultFontCache)
		delete pDefFont;
	pDefFont = NULL;
}


YFontCache::YFontCache(CPATH defFontPath, u32 cacheSize)
: pFace(NULL), curSize(0)
{
	FT_Error error(FT_Init_FreeType(&library));

	if(error)
		goto err;
	error = FTC_Manager_New(library, 0, 0, cacheSize, &simpleFaceRequester, NULL, &manager);
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

const MFontFamily*
YFontCache::GetFontFamilyPtr(const FT_String* family_name) const
{
	const FFacesIndex::const_iterator i(mFacesIndex.find(family_name));

	return (i == mFacesIndex.end()) ? NULL : i->second;
}
/*MTypeface* YFontCache::GetTypefacePtr(u16 i) const
{
	return i < sTypes.size() ? sTypes[i] : NULL;
}*/
const MTypeface*
YFontCache::GetDefaultTypefacePtr() const
{
	//默认字体缓存的默认字型指针由初始化保证为非空指针。
	return pDefaultFace ? pDefaultFace : pDefaultFontCache->GetDefaultTypefacePtr();
}
const MTypeface*
YFontCache::GetTypefacePtr(const FT_String* family_name, const FT_String* style_name) const
{
	const MFontFamily* f(GetFontFamilyPtr(family_name));

	if(!f)
		return NULL;
	return f->GetTypefacePtr(style_name);
}
FTC_SBit
YFontCache::GetGlyph(u32 c)
{
	if(!scaler.face_id)
		return NULL;

	const u32 flags(FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL);
	const u32 index(FTC_CMapCache_Lookup(cmapCache, scaler.face_id, pFace->cmapIndex, c));

	FTC_SBitCache_LookupScaler(sbitCache, &scaler, flags, index, &sbit, NULL);
	return sbit;
}
s8
YFontCache::GetAdvance(u32 c, FTC_SBit sbit)
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
YFontCache::SetTypeface(const MTypeface* p)
{
	if(p == NULL || sTypes.find(p) == sTypes.end())
		return false;
	pFace = p;
	scaler.face_id = reinterpret_cast<FTC_FaceID>(const_cast<MTypeface*>(p));
	return true;
}
bool
YFontCache::SetFontSize(MFont::SizeType s)
{
	if(s == 0)
		s = MFont::DefSize;
	if(s != curSize)
	{
		//ClearCache();
		curSize = s;
		scaler.width  = static_cast<FT_UInt>(curSize);
		scaler.height = static_cast<FT_UInt>(curSize);
		scaler.pixel  = 72;
		scaler.x_res  = 0;
		scaler.y_res  = 0;
		GetGlyph(' '); //更新当前字形，否则 GetHeight() 会返回错误的值。
		return true;
	}
	else
		return false;
}

void
YFontCache::operator+=(const MFontFile& f)
{
	sFiles.insert(&f);
}
bool
YFontCache::operator-=(const MFontFile& f)
{
	return sFiles.erase(&f);
}

void
YFontCache::operator+=(const MTypeface& f)
{
	sTypes.insert(&f);
}
bool
YFontCache::operator-=(const MTypeface& f)
{
	return sTypes.erase(&f);
}

void
YFontCache::operator+=(MFontFamily& f)
{
	sFaces.insert(&f);
	mFacesIndex.insert(std::make_pair(f.family_name, &f));
}
bool
YFontCache::operator-=(MFontFamily& f)
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
YFontCache::LoadTypefaces(const MFontFile& f)
{
	const FT_Long t(f.GetFaceN());

	for(FT_Long i(0); i < t; ++i)
	{
		try
		{
			MTypeface* q(NULL);
			MFontFamily* r(NULL);

			try
			{
				try
				{
					if(sTypes.find(q = new MTypeface(*this, f, i)) == sTypes.end())
					{
						scaler.face_id = static_cast<FTC_FaceID>(q);

						//读取字型名称并构造名称映射。
						FT_Face face(GetInternalFaceInfo());

						if(face == NULL || face->family_name == NULL)
							throw MLoggedEvent("Face loading failed.", 2);

						FFacesIndex::iterator i(mFacesIndex.find(face->family_name));

						q->pFontFamily = r = i == mFacesIndex.end() ? new MFontFamily(*this, face->family_name) : i->second;
						*r += *q;
						if(i == mFacesIndex.end())
							*this += *r;
						q->style_name = strdup_n(face->style_name);
						*this += *q;
					}
					else
						throw MLoggedEvent("Repeated face is neglected.", 3);
				}
				catch(std::bad_alloc&)
				{
					throw MLoggedEvent("Allocation failed @@ YFontCache::LoadTypefaces(const MFontFile&);", 2);
				}
			}
			catch(...)
			{
				scaler.face_id = NULL;
				delete r;
				delete q;
				throw;
			}
		}
		catch(MLoggedEvent& e)
		{
			if(e.GetLevel() < 2)
				throw;
		}
	}
	if(pDefaultFace == NULL && !sTypes.empty())
		pDefaultFace = *sTypes.begin();
}

void
YFontCache::LoadFontFileDirectory(CPATH path, CPATH ext)
{
	DIR_ITER* dir(diropen(path));
	u32 n(0);

	if(dir)
	{
		FILENAMESTR name;
		struct stat st;

		while(!dirnext(dir, name, &st))
			if(name[0] != '.' && !(st.st_mode & S_IFDIR) && IsExtendName(ext, name))
				++n;
		//	sFiles.reserve(n);
		dirreset(dir);
		while(!dirnext(dir, name, &st))
			if(name[0] != '.' && !(st.st_mode & S_IFDIR) && IsExtendName(ext, name))
			{
				try
				{
					*this += *new MFontFile(path, name, library);
				}
				catch(std::bad_alloc&)
				{
					throw MLoggedEvent("Allocation failed @@ YFontCache::LoadFontFileDirectory(CPATH, CPATH);", 2);
				}
			}
		dirclose(dir);
	}
	LoadTypefaces();
	if(pDefaultFace)
		SetTypeface(pDefaultFace);
}
void
YFontCache::LoadFontFile(CPATH path)
{
	if(GetFileName(path) != NULL && fexists(path))
		try
		{
			MFontFile* p(new MFontFile(path, library));

			*this += *p;
			LoadTypefaces(*p);
			if(pDefaultFace)
				SetTypeface(pDefaultFace);
		}
		catch(...)
		{}
	else
		LoadFontFileDirectory(path);
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
	for(FFacesIndex::const_iterator i(mFacesIndex.begin()); i != mFacesIndex.end(); ++i)
		delete i->second;
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

