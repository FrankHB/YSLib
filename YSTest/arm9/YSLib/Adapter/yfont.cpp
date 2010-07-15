// YSLib::Adapter::YFontCache by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:06:13;
// UTime = 2010-7-15 20:35;
// Version = 0.6492;


#include "yfont.h"

#include "../Core/yfilesys.h"
//#include "../Helper/yglobal.h"
//#include FT_OUTLINE_H
//#include FT_SYNTHESIS_H

using namespace stdex;
using namespace platform;

YSL_BEGIN

using namespace IO;

YSL_BEGIN_NAMESPACE(Drawing)

//供 FreeType 使用的客户端字体查询函数，从由 face_id 提供的参数对应的字体文件中读取字体，写入 aface 。
FT_Error
simpleFaceRequester(FTC_FaceID face_id, FT_Library library,
					FT_Pointer request_data, FT_Face* aface)
{
	YTypeface* fontFace(static_cast<YTypeface*>(face_id));
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


YFontFamily::YFontFamily(YFontCache& cache, FT_String* name)
: Cache(cache), family_name(name ? name : NULL), sTypes()
{
}
YFontFamily::~YFontFamily()
{
	free(family_name);
}

bool
YFontFamily::operator==(const YFontFamily& rhs) const
{
	return &Cache == &rhs.Cache && (family_name == rhs.family_name || (family_name && rhs.family_name && !strcmp(family_name, rhs.family_name)));
}
bool
YFontFamily::operator<(const YFontFamily& rhs) const
{
	return &Cache < &rhs.Cache || (&Cache == &rhs.Cache && family_name && rhs.family_name && strcmp(family_name, rhs.family_name) < 0);
}

const YTypeface*
YFontFamily::GetTypefacePtr(const FT_String* style_name) const
{
	const FTypes::const_iterator i(sTypes.find(style_name));

	return (i == sTypes.end()) ? NULL : (*i).second;
}

void
YFontFamily::operator+=(YTypeface& f)
{
	sTypes.insert(std::make_pair(f.GetStyleName(), &f));
}
bool
YFontFamily::operator-=(YTypeface& f)
{
	return sTypes.erase(f.GetStyleName());
}


/*const FT_Matrix YTypeface::MNormal = {0x10000, 0, 0, 0x10000},
	YTypeface::MOblique = {0x10000, 0x5800, 0, 0x10000};*/

YTypeface::YTypeface(YFontCache& cache, const YFontFile& file, u32 i/*, const bool bb, const bool bi, const bool bu*/)
: Cache(cache), File(file), pFontFamily(NULL), style_name(NULL), faceIndex(i), cmapIndex(-1)/*, bBold(bb), bOblique(bi), bUnderline(bu), matrix(bi ? MOblique : MNormal)*/
{}
YTypeface::~YTypeface()
{
	free(style_name);
}

bool
YTypeface::operator==(const YTypeface& rhs) const
{
	return &File == &rhs.File && faceIndex == rhs.faceIndex;
}
bool
YTypeface::operator<(const YTypeface& rhs) const
{
	return &File < &rhs.File || (&File == &rhs.File && faceIndex < rhs.faceIndex);
}


YFontFile::YFontFile(const CPATH p, FT_Library& l)
: path(strdup(p)), library(l), nFace(0)
{
	ReloadFaces();
}
YFontFile::YFontFile(const CPATH p, const char* n, FT_Library& l)
: path(strcatdup(p, n)), library(l)
{
	ReloadFaces();
}

bool
YFontFile::operator==(const YFontFile& rhs) const
{
	return &library == &rhs.library && (!(path && rhs.path) || !strncmp(path, rhs.path, MaxFontPathLength));
}
bool
YFontFile::operator<(const YFontFile& rhs) const
{
	return &library < &rhs.library || (&library == &rhs.library && path && rhs.path && strncmp(path, rhs.path, MaxFontPathLength) < 0);
}

void
YFontFile::ReloadFaces()
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


const YTypeface*
GetDefaultTypefacePtr()
{
	return pDefaultFontCache->GetDefaultTypefacePtr();
}


const YFont::SizeType
YFont::DefSize(16), //DEF_SHELL_FONTSIZE,
YFont::MinSize(4), //DEF_SHELL_FONTSIZE_MIN,
YFont::MaxSize(72); //DEF_SHELL_FONTSIZE_MAX;
YFont* YFont::pDefFont(NULL);

YFont::YFont(const YFontFamily& family, const SizeType size, YFontStyle style)
: pFontFamily(&family), Style(style), Size(size)
{
}

YFont::SizeType YFont::GetHeight() const
{
	return GetCache().GetHeight();
}
const FT_String*
YFont::GetStyleName() const
{
	const FT_String* ss(NULL);

	switch(Style)
	{
	case Bold:
		ss = "Bold";
		break;
	case Italic:
		ss = "Italic";
		break;
	case Regular:
	default:
		ss = "Regular";
		break;
	}
	return ss;
}

void
YFont::SetSize(YFont::SizeType s)
{
	if(s >= MinSize && s <= MaxSize)
		Size = s;
}

void
YFont::Update()
{
	const YTypeface* t(pFontFamily->GetTypefacePtr(GetStyleName()));

	if(!(t || Style))
		t = pFontFamily->GetTypefacePtr("Regular");
	if(t)
		GetCache().SetTypeface(*t);
	UpdateSize();
}
void
YFont::UpdateSize()
{
	GetCache().SetFontSize(Size);
}

void
YFont::InitialDefault()
{
	if(!pDefFont)
		pDefFont = new YFont();
}
void
YFont::ReleaseDefault()
{
	if(!pDefaultFontCache)
		delete pDefFont;
	pDefFont = NULL;
}


YFontCache::YFontCache(CPATH defFontPath, u32 cacheSize)
: pType(NULL), fontSize(0)
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
		SetTypeface(**sTypes.begin());
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

const YFontFamily*
YFontCache::GetFontFamilyPtr(const FT_String* family_name) const
{
	const FFaces::const_iterator i(sFaces.find(family_name));

	return (i == sFaces.end()) ? NULL : (*i).second;
}
/*YTypeface* YFontCache::GetTypefacePtr(u16 i) const
{
	return i < sTypes.size() ? sTypes[i] : NULL;
}*/
const YTypeface*
YFontCache::GetDefaultTypefacePtr() const
{
	//默认字体缓存的默认字型指针由初始化保证为非空指针。
	return pDefaultFace ? pDefaultFace : pDefaultFontCache->GetDefaultTypefacePtr();
}
const YTypeface*
YFontCache::GetTypefacePtr(const FT_String* family_name, const FT_String* style_name) const
{
	const YFontFamily* f(GetFontFamilyPtr(family_name));

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
	const u32 index(FTC_CMapCache_Lookup(cmapCache, scaler.face_id, pType->cmapIndex, c));

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

const YTypeface*
YFontCache::SetTypeface(const YTypeface& p)
{
	const YTypeface* const t(pType);
	YTypeface* q(const_cast<YTypeface*>(&p));

	if(sTypes.find(q) != sTypes.end())
		scaler.face_id = (FTC_FaceID)(pType = q);
	return t;
}
void
YFontCache::SetFontSize(YFont::SizeType s)
{
	if(s == 0)
		s = YFont::DefSize;
	if(s != fontSize)
	{
		//ClearCache();
		fontSize = s;
		scaler.width  = (FT_UInt)fontSize;
		scaler.height = (FT_UInt)fontSize;
		scaler.pixel  = 72;
		scaler.x_res  = 0;
		scaler.y_res  = 0;
		GetGlyph(' '); //更新当前字形，否则 GetHeight() 会返回错误的值。
	}
}

void
YFontCache::operator+=(YFontFile& f)
{
	sFiles.insert(&f);
}
bool
YFontCache::operator-=(YFontFile& f)
{
	return sFiles.erase(&f);
}

void
YFontCache::operator+=(YTypeface& f)
{
	sTypes.insert(&f);
}
bool
YFontCache::operator-=(YTypeface& f)
{
	return sTypes.erase(&f);
}

void
YFontCache::operator+=(YFontFamily& f)
{
	sFaces.insert(std::make_pair(f.family_name, &f));
}
bool
YFontCache::operator-=(YFontFamily& f)
{
	return sFaces.erase(f.family_name);
}

void
YFontCache::LoadTypefaces()
{
	for(FFiles::iterator i(sFiles.begin()); i != sFiles.end(); ++i)
		LoadTypefaces(**i);
}
void
YFontCache::LoadTypefaces(const YFontFile& f)
{
	const FT_Long t(f.GetFaceN());
	YTypeface* q;

	for(FT_Long i(0); i < t; ++i)
		if((q = new YTypeface(*this, f, i)) && sTypes.find(q) == sTypes.end())
		{
			scaler.face_id = static_cast<FTC_FaceID>(q);

			//读取字型名称并构造名称映射。
			FT_Face f(GetInternalFaceInfo());
			if(f)
			{
				if(sFaces.find(f->family_name) == sFaces.end())
				{
					YFontFamily* r(new YFontFamily(*this, strdup(f->family_name)));

					if((q->pFontFamily = r) && (q->style_name = strdup(f->style_name)))
					{
						operator+=(*r);
						*r += *q;
					}
					else
					{
						delete r;
						goto test_face_err;
					}
				}
				else
				{
					q->style_name = strdup(f->style_name);
					if(!q->style_name)
						goto test_face_err;
					*(q->pFontFamily = sFaces[f->family_name]) += *q;
				}
				operator+=(*q);
			}
			else
			{
test_face_err:
				delete q;
				scaler.face_id = NULL;
			}
		}
		if(!(pDefaultFace || sTypes.empty()))
			pDefaultFace = *sTypes.begin();
}

void
YFontCache::LoadFontFileDirectory(const CPATH path, const CPATH ext)
{
	DIR_ITER* dir(diropen(path));
	u32 n(0);
	YFontFile* p;

	if(dir)
	{
		FILENAMESTR name;
		struct stat st;

		while(!dirnext(dir, name, &st))
			if(*name != '.' && !(st.st_mode & S_IFDIR) && IsExtendName(ext, name))
				++n;
		//	sFiles.reserve(n);
		dirreset(dir);
		while(!dirnext(dir, name, &st))
			if(*name != '.' && !(st.st_mode & S_IFDIR) && IsExtendName(ext, name) && (p = new YFontFile(path, name, library)))
				operator+=(*p);
		dirclose(dir);
	}
	LoadTypefaces();
	if(pDefaultFace)
		SetTypeface(*pDefaultFace);
}
void
YFontCache::LoadFontFile(const CPATH path)
{
	YFontFile* p;

	if(GetFileName(path) && fexists(path) && (p = new YFontFile(path, library)))
	{
		operator+=(*p);
		LoadTypefaces(*p);
		if(pDefaultFace)
			SetTypeface(*pDefaultFace);
	}
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
	for(FFaces::const_iterator i(sFaces.begin()); i != sFaces.end(); ++i)
		delete (*i).second;
	sFaces.clear();
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

