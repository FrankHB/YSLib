// YSLib::Adapter::YFontCache by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:02:40;
// UTime = 2010-7-3 4:59;
// Version = 0.6462;


#ifndef INCLUDED_YFONT_H_
#define INCLUDED_YFONT_H_

// YFontCache ：平台无关的字体缓存库。

#include "../Core/yfunc.hpp"
#include "../Core/ycutil.h"
#include <set>
#include <map>

#define GLYPH_CACHE_SIZE	(32 * 1024)

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

//默认字体大小（pt）。
const u8 DEFAULT_FONT_SIZE	= 14;

//前向声明。
class YFont;
class YFontFamily;
class YTypeface;
class YFontFile;
class YFontCache;


//字体样式。
enum YFontStyle
{
	Regular = 0, //常规字体。
	Bold = 1, //粗体。
	Italic = 2, //斜体。
	Underline = 4, //下划线。
	Strikeout = 8, //删除线。
};


//字型家族 (Typeface Family) 标识。
class YFontFamily// : implements GIEquatable<YFontFamily>, implements GILess<YFontFamily>, implements GIContainer<YTypeface>
{
	friend class YTypeface;
	friend class YFontCache;

public:
	typedef std::map<const FT_String*, YTypeface*, deref_str_comp<FT_String> > FTypes; //字型组类型。

	YFontCache& Cache;

private:
	FT_String* family_name;
	FTypes sTypes;

public:
	YFontFamily(YFontCache&, FT_String*);
	~YFontFamily();

	bool
	operator==(const YFontFamily&) const;
	bool
	operator<(const YFontFamily&) const;

	const FT_String*
	GetFamilyName() const;
	const YTypeface*
	GetTypefacePtr(const FT_String*) const; //取指定名称字型指针。

private:
	void
	operator+=(YTypeface&); //向字型组添加字型对象。
	bool
	operator-=(YTypeface&); //从字型组中移除指定字型对象。
};

inline const FT_String* YFontFamily::GetFamilyName() const
{
	return family_name;
}


//字型标识。
class YTypeface// : implements GIEquatable<YTypeface>, implements GILess<YTypeface>
{
	friend class YFontCache;
	friend FT_Error simpleFaceRequester(FTC_FaceID, FT_Library, FT_Pointer, FT_Face*);

//	static const FT_Matrix MNormal, MOblique;
public:
	YFontCache& Cache;
	const YFontFile& File;

private:
	YFontFamily* pFontFamily;
//	FT_Face face;
//	bool bBold, bOblique, bUnderline;
	FT_String* style_name;

	FT_Long faceIndex;
	FT_Int cmapIndex;
/*	FT_Long nGlyphs;
	FT_UShort uUnitPerEM;
	FT_Int nCharmaps;
	FT_Long lUnderlinePos;
	FT_Matrix matrix;
	std::vector<u8> fixSizes;*/

public:
	YTypeface(YFontCache&, const YFontFile&, u32 = 0/*, const bool bb = false, const bool bi = false, const bool bu = false*/);
	~YTypeface();

	bool
	operator==(const YTypeface&) const;
	bool
	operator<(const YTypeface&) const;

	const YFontFamily*
	GetFontFamilyPtr() const;
	const FT_String*
	GetFamilyName() const;
	const FT_String*
	GetStyleName() const;
};

inline const YFontFamily*
YTypeface::GetFontFamilyPtr() const
{
	return pFontFamily;
}
inline const FT_String*
YTypeface::GetFamilyName() const
{
	return pFontFamily ? pFontFamily->GetFamilyName() : NULL;
}
inline const FT_String*
YTypeface::GetStyleName() const
{
	return style_name;
}


//字体文件。
class YFontFile// : implements GIEquatable<YFontFile>, implements GILess<YFontFile>
{
public:
	static const u16 MaxFontPathLength = YC_MAX_PATH; //最大字体文件路径长度。

private:
	CPATH path;
	FT_Library& library;
	FT_Long nFace;

public:
	YFontFile(const CPATH, FT_Library&);
	YFontFile(const CPATH, const char*, FT_Library&);

	bool
	operator==(const YFontFile&) const;
	bool
	operator<(const YFontFile&) const;

	const char*
	GetPath() const;
	FT_Long
	GetFaceN() const;

	void
	ReloadFaces(); //读取字体文件，载入全部字体。
};

inline const char*
YFontFile::GetPath() const
{
	return path;
}
inline FT_Long
YFontFile::GetFaceN() const
{
	return nFace;
}


//取默认字型。
const YTypeface* GetDefaultTypefacePtr();

//取默认字型家族。
inline const YFontFamily*
GetDefaultFontFamilyPtr()
{
	return GetDefaultTypefacePtr()->GetFontFamilyPtr();
}


//字体：字模，包含字型和大小。
class YFont
{
public:
	typedef u8 SizeType;

	static const SizeType DefSize, MinSize, MaxSize;

private:
	static YFont* pDefFont; //默认 Shell 字体。

	const YFontFamily* pFontFamily;
	YFontStyle Style;
	SizeType Size;

public:
	explicit
	YFont(const YFontFamily& = *GetDefaultFontFamilyPtr(), const SizeType = DefSize, YFontStyle = Regular);

	bool
	IsBold() const;
	bool
	IsItalic() const;
	bool
	IsUnderline() const;
	bool
	IsStrikeout() const;

	static const YFont&
	GetDefault();
	const YFontFamily&
	GetFontFamily() const;
	YFontStyle
	GetStyle() const;
	SizeType
	GetSize() const;
	YFontCache&
	GetCache() const;
	const FT_String*
	GetFamilyName() const;
	const FT_String*
	GetStyleName() const;
	SizeType
	GetHeight() const;

/*	YFontFamily&
	SetFontFamily(const YFontFamily&);*/
	void
	SetStyle(YFontStyle);
	void
	SetSize(SizeType);

	void
	Update(); //更新字体缓存中当前处理的字体。
	void
	UpdateSize(); //更新字体缓存中当前处理的字体大小。

	static void
	InitialDefault(); //初始化默认字体。
	static void
	ReleaseDefault(); //释放默认字体。
};

inline bool
YFont::IsBold() const
{
	return Style | Bold;
}
inline bool
YFont::IsItalic() const
{
	return Style | Italic;
}
inline bool
YFont::IsUnderline() const
{
	return Style | Underline;
}
inline bool
YFont::IsStrikeout() const
{
	return Style | Strikeout;
}

inline const YFontFamily&
YFont::GetFontFamily() const
{
//	return *(pFontFamily ? pFontFamily : GetDefaultTypefacePtr()->pFontFamily);
	// pFontfamily 不可能为 NULL 。
	return *pFontFamily;
}
inline YFontStyle
YFont::GetStyle() const
{
	return Style;
}
inline YFont::SizeType
YFont::GetSize() const
{
	return Size;
}
inline YFontCache&
YFont::GetCache() const
{
	return GetFontFamily().Cache;
}
inline const FT_String*
YFont::GetFamilyName() const
{
	return GetFontFamily().GetFamilyName();
}
/*inline const FT_String*
YFont::GetStyleName() const
{
	return GetFontFamily().GetStyleName;
}*/

inline void
YFont::SetStyle(YFontStyle s)
{
	Style = s;
}


//字体缓存。
class YFontCache : implements GIContainer<YFontFile>, implements GIContainer<YTypeface>, implements GIContainer<YFontFamily>
{
	friend class YTypeface;

private:
	FT_Library library; //库实例。
	FTC_Manager manager; //内存管理器实例。
	FTC_ScalerRec scaler;
	FTC_CMapCache cmapCache;
	FTC_SBitCache sbitCache;
	FTC_SBit sbit;

public:
	typedef std::set<YFontFile*, deref_comp<YFontFile> > FFiles; //字体文件组类型。
	typedef std::set<YTypeface*, deref_comp<YTypeface> > FTypes; //字型组类型。
	typedef std::map<const FT_String*, YFontFamily*, deref_str_comp<FT_String> > FFaces; //字型家族组类型。

private:
	FFiles sFiles; //字体文件组。
	FTypes sTypes; //字型组。
	FFaces sFaces; //字型家族组。

	const YTypeface* pDefaultFace; //默认字型指针。
	const YTypeface* pType; //当前处理的字型指针。
	u8 fontSize; //当前处理的字体大小。

public:
	explicit
	YFontCache(CPATH, u32 cacheSize = GLYPH_CACHE_SIZE); //读取指定路径的字体文件并分配指定大小的缓存空间。
	virtual
	~YFontCache();

private:
	FT_Face GetInternalFaceInfo() const; //取当前处理的内部字型结构体指针。

public:
	const FFiles&
	GetFiles() const; //取字体文件组。
	const FTypes&
	GetTypes() const; //取字型组。
	const FFaces&
	GetFaces() const; //取字型家族组。
	FFiles::size_type
	GetFilesN() const; //取字体文件组储存的文件数。
	FTypes::size_type
	GetTypesN() const; //取字型组储存的字型数。
	FFaces::size_type
	GetFacesN() const; //取字型家族组储存的字型家族数。
//	YFont*
//	GetFontPtr() const;
	const YFontFamily*
	GetFontFamilyPtr(const FT_String*) const; //取指定名称字型家族指针。
	const YTypeface*
	GetDefaultTypefacePtr() const; //取默认字型指针。
	const YTypeface*
	GetTypefacePtr() const; //取当前处理的字型指针。
//	YTypeface*
//	GetTypefacePtr(u16) const; //取字型组储存的指定索引的字型指针。
	const YTypeface*
	GetTypefacePtr(const FT_String*, const FT_String*) const;//取指定名称字型指针。
	u8
	GetFontSize() const; //取当前处理的字体大小。
	FTC_SBit
	GetGlyph(u32); //取当前字型和大小渲染的指定字符的字形。
	s8
	GetAdvance(u32, FTC_SBit sbit = NULL); //取跨距。
	u8
	GetHeight() const; //取行高。
	s8
	GetAscender() const; //取升部。
	s8
	GetDescender() const; //取降部。

	const YTypeface*
	SetTypeface(const YTypeface&); //设置字型组中指定索引的字型为当前字型。
	void
	SetFontSize(YFont::SizeType); //设置当前处理的字体大小。

public:
	//SetFont;

private:
	void
	operator+=(YFontFile&); //向字体文件组添加字体文件对象。
	bool
	operator-=(YFontFile&); //从字体文件组中移除指定字体文件对象。

	void
	operator+=(YTypeface&); //向字型组添加字型对象。
	bool
	operator-=(YTypeface&); //从字型组中移除指定字型对象。

	void
	operator+=(YFontFamily&); //向字型家族组添加字型对象。
	bool
	operator-=(YFontFamily&); //从字型家族组中移除指定字型对象。

	void
	LoadTypefaces(); //从字体文件组中载入字体信息。
	void
	LoadTypefaces(const YFontFile&); //从指定字体文件中载入字体信息。

public:
	void
	LoadFontFileDirectory(const CPATH, const CPATH = "ttf"); //读取字体文件目录并载入目录下指定后缀名的字体文件。
	void
	LoadFontFile(const CPATH); //按路径加入字体文件并载入字体信息。

private:
	void
	ClearFontFiles();
	void
	ClearTypefaces();
	void
	ClearFontFamilies();
	void
	ClearContainers();

public:
	void
	ClearCache(); //清除缓存。
};

inline const YFontCache::FFiles&
YFontCache::GetFiles() const
{
	return sFiles;
}
inline const YFontCache::FTypes&
YFontCache::GetTypes() const
{
	return sTypes;
}
inline const YFontCache::FFaces&
YFontCache::GetFaces() const
{
	return sFaces;
}
inline YFontCache::FFiles::size_type
YFontCache::GetFilesN() const
{
	return sFiles.size();
}
inline YFontCache::FTypes::size_type
YFontCache::GetTypesN() const
{
	return sTypes.size();
}
inline YFontCache::FFaces::size_type
YFontCache::GetFacesN() const
{
	return sFaces.size();
}
inline const YTypeface*
YFontCache::GetTypefacePtr() const
{
	return pType;
}
inline u8
YFontCache::GetFontSize() const
{
	return fontSize;
}


//以路径 p 创建字体缓存 f 。
inline void
CreateFontCache(YFontCache*& f, CPATH p)
{
	delete f;
	f = new YFontCache(p);
}

//销毁字体缓存 f 。
inline void
DestroyFontCache(YFontCache*& f)
{
	delete f;
	f = NULL;
}

YSL_END_NAMESPACE(Drawing)

YSL_END;

#endif

