// YSLib::Adapter::YFontCache by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:02:40;
// UTime = 2010-7-22 9:34;
// Version = 0.6662;


#ifndef INCLUDED_YFONT_H_
#define INCLUDED_YFONT_H_

// YFontCache ：平台无关的字体缓存库。

#include "../Core/yfunc.hpp"
#include "../Core/ystring.h"
#include <set>
#include <map>

#define GLYPH_CACHE_SIZE	(32 * 1024)

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

//默认字体大小（pt）。
const u8 DEFAULT_FONT_SIZE	= 14;

//前向声明。
class MFont;
class MFontFamily;
class MTypeface;
class MFontFile;
class YFontCache;


//字体样式。
struct EFontStyle
{
public:
	typedef enum Styles
	{
		Regular = 0, //常规字体。
		Bold = 1, //粗体。
		Italic = 2, //斜体。
		Underline = 4, //下划线。
		Strikeout = 8, //删除线。
	} Styles;

private:
	Styles style;

public:
	EFontStyle(Styles = Regular);

	DefConverter(const Styles&, style);
	operator Styles&();

	const char*
	GetName() const; //取样式名称。
};

inline
EFontStyle::EFontStyle(Styles s)
: style(s)
{}

inline
EFontStyle::operator Styles&()
{
	return style;
}


//字型家族 (Typeface Family) 标识。
class MFontFamily// : implements GIEquatable<MFontFamily>, implements GILess<MFontFamily>, implements GIContainer<MTypeface>
{
	friend class MTypeface;
	friend class YFontCache;

public:
	typedef std::set<const MTypeface*> FTypes; //字型组类型。
	typedef std::map<const FT_String*, const MTypeface*, deref_str_comp<FT_String> > FTypesIndex; //字型组索引类型。

	YFontCache& Cache;

private:
	FT_String* family_name;
	FTypes sTypes; //字型组类型。
	FTypesIndex mTypesIndex; //字型组索引类型。

public:
	MFontFamily(YFontCache&, const FT_String*);
	~MFontFamily();

	bool
	operator==(const MFontFamily&) const;
	bool
	operator<(const MFontFamily&) const;

	DefGetter(const FT_String*, FamilyName, family_name)
	const MTypeface*
	GetTypefacePtr(const FT_String*) const; //取指定名称字型指针。

private:
	void
	operator+=(const MTypeface&); //向字型组和字型组索引添加字型对象。
	bool
	operator-=(const MTypeface&); //从字型组和字型组索引中移除指定字型对象。
};


//字型标识。
class MTypeface// : implements GIEquatable<MTypeface>, implements GILess<MTypeface>
{
	friend class YFontCache;
	friend FT_Error simpleFaceRequester(FTC_FaceID, FT_Library, FT_Pointer, FT_Face*);

//	static const FT_Matrix MNormal, MOblique;
public:
	YFontCache& Cache;
	const MFontFile& File;

private:
	MFontFamily* pFontFamily;
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
	MTypeface(YFontCache&, const MFontFile&, u32 = 0/*, const bool bb = false, const bool bi = false, const bool bu = false*/);
	~MTypeface();

	bool
	operator==(const MTypeface&) const;
	bool
	operator<(const MTypeface&) const;

	DefGetter(const MFontFamily*, FontFamilyPtr, pFontFamily)
	DefGetter(const FT_String*, FamilyName, pFontFamily ? pFontFamily->GetFamilyName() : NULL)
	DefGetter(const FT_String*, StyleName, style_name)
};


//字体文件。
class MFontFile// : implements GIEquatable<MFontFile>, implements GILess<MFontFile>
{
public:
	static const u16 MaxFontPathLength = YC_MAX_PATH; //最大字体文件路径长度。

private:
	CPATH path;
	FT_Library& library;
	FT_Long nFace;

public:
	MFontFile(CPATH, FT_Library&);
	MFontFile(CPATH, const char*, FT_Library&);

	bool
	operator==(const MFontFile&) const;
	bool
	operator<(const MFontFile&) const;

	DefGetter(const char*, Path, path)
	DefGetter(s32, FaceN, nFace)

	void
	ReloadFaces(); //读取字体文件，载入全部字体。
};


//取默认字型。
const MTypeface* GetDefaultTypefacePtr();

//取默认字型家族。
inline const MFontFamily*
GetDefaultFontFamilyPtr()
{
	return GetDefaultTypefacePtr()->GetFontFamilyPtr();
}


//字体：字模，包含字型和大小。
class MFont
{
public:
	typedef u8 SizeType;

	static const SizeType DefSize, MinSize, MaxSize;

private:
	static MFont* pDefFont; //默认 Shell 字体。

	const MFontFamily* pFontFamily;
	EFontStyle Style;
	SizeType Size;

public:
	explicit
	MFont(const MFontFamily& = *GetDefaultFontFamilyPtr(), SizeType = DefSize, EFontStyle = EFontStyle::Regular);

	DefBoolGetter(Bold, Style | EFontStyle::Bold)
	DefBoolGetter(Italic, Style | EFontStyle::Italic)
	DefBoolGetter(Underline, Style | EFontStyle::Underline)
	DefBoolGetter(Strikeout, Style | EFontStyle::Strikeout)

	DefStaticGetter(const MFont&, Default, *pDefFont)
	DefGetter(const MFontFamily&, FontFamily, *pFontFamily)
	DefGetter(EFontStyle, Style, Style)
	DefGetter(SizeType, Size, Size)
	DefGetter(YFontCache&, Cache, GetFontFamily().Cache)
	DefGetterMember(const FT_String*, FamilyName, GetFontFamily())
	DefGetter(const FT_String*, StyleName, Style.GetName())
	SizeType
	GetHeight() const;

/*	MFontFamily&
	SetFontFamily(const MFontFamily&);*/
	DefSetter(EFontStyle, Style, Style)
	void
	SetSize(SizeType);

	void
	Update(); //更新字体缓存中当前处理的字体。
	void
	UpdateSize(); //更新字体缓存中当前处理的字体大小。

	static bool
	InitializeDefault(); //初始化默认字体。
	static void
	ReleaseDefault(); //释放默认字体。
};

/*
inline const MFontFamily&
MFont::GetFontFamily() const
{
//	return *(pFontFamily ? pFontFamily : GetDefaultTypefacePtr()->pFontFamily);
	// pFontFamily 不可能为 NULL 。
	YAssert(pFontFamily != NULL, 
		"In function \"inline const MFontFamily&\n"
		"MFont::GetFontFamily()\": \n"
		"The default global @MFontFamily pointer is null.");
	return *pFontFamily;
}
*/


//字体缓存。
class YFontCache : public YObject,
	implements GIContainer<const MFontFile>, implements GIContainer<const MTypeface>, implements GIContainer<MFontFamily>
{
	friend class MTypeface;

private:
	FT_Library library; //库实例。
	FTC_Manager manager; //内存管理器实例。
	FTC_ScalerRec scaler;
	FTC_CMapCache cmapCache;
	FTC_SBitCache sbitCache;
	FTC_SBit sbit;

public:
	typedef std::set<const MFontFile*, deref_comp<const MFontFile> > FFiles; //字体文件组类型。
	typedef std::set<const MTypeface*, deref_comp<const MTypeface> > FTypes; //字型组类型。
	typedef std::set<MFontFamily*, deref_comp<MFontFamily> > FFaces; //字型家族组类型。
	typedef std::map<const FT_String*, MFontFamily*, deref_str_comp<FT_String> > FFacesIndex; //字型家族组索引类型。

private:
	FFiles sFiles; //字体文件组。
	FTypes sTypes; //字型组。
	FFaces sFaces; //字型家族组索引。
	FFacesIndex mFacesIndex; //字型家族组索引。

	const MTypeface* pDefaultFace; //默认字型指针。
	const MTypeface* pFace; //当前处理的字型指针。
	MFont::SizeType curSize; //当前处理的字体大小。

public:
	explicit
	YFontCache(CPATH, u32 cacheSize = GLYPH_CACHE_SIZE); //读取指定路径的字体文件并分配指定大小的缓存空间。
	virtual
	~YFontCache();

private:
	FT_Face
	GetInternalFaceInfo() const; //取当前处理的内部字型结构体指针。

public:
	DefGetter(const FFiles&, Files, sFiles) //取字体文件组。
	DefGetter(const FTypes&, Types, sTypes) //取字型组。
	DefGetter(const FFacesIndex&, Faces, mFacesIndex) //取字型家族组。
	DefGetter(FFiles::size_type, FilesN, sFiles.size()) //取字体文件组储存的文件数。
	DefGetter(FTypes::size_type, TypesN, sTypes.size()) //取字型组储存的字型数。
	DefGetter(FFacesIndex::size_type, FacesN, mFacesIndex.size()) //取字型家族组储存的字型家族数。
//	MFont*
//	GetFontPtr() const;
	const MFontFamily*
	GetFontFamilyPtr(const FT_String*) const; //取指定名称字型家族指针。
	const MTypeface*
	GetDefaultTypefacePtr() const; //取默认字型指针。
	DefGetter(const MTypeface*, TypefacePtr, pFace) //取当前处理的字型指针。
//	MTypeface*
//	GetTypefacePtr(u16) const; //取字型组储存的指定索引的字型指针。
	const MTypeface*
	GetTypefacePtr(const FT_String*, const FT_String*) const;//取指定名称字型指针。
	DefGetter(u8, FontSize, curSize) //取当前处理的字体大小。
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

	void
	SetTypeface(const MTypeface*); //设置字型组中指定的字型为当前字型（忽略不属于字型组的字型）。
	void
	SetFontSize(MFont::SizeType); //设置当前处理的字体大小。

public:
	//SetFont;

private:
	void
	operator+=(const MFontFile&); //向字体文件组添加字体文件对象。
	bool
	operator-=(const MFontFile&); //从字体文件组中移除指定字体文件对象。

	void
	operator+=(const MTypeface&); //向字型组添加字型对象。
	bool
	operator-=(const MTypeface&); //从字型组中移除指定字型对象。

	void
	operator+=(MFontFamily&); //向字型家族组添加字型对象。
	bool
	operator-=(MFontFamily&); //从字型家族组中移除指定字型对象。

	void
	LoadTypefaces(); //从字体文件组中载入字体信息。
	void
	LoadTypefaces(const MFontFile&); //从指定字体文件中载入字体信息。

public:
	void
	LoadFontFileDirectory(CPATH, CPATH = "ttf"); //读取字体文件目录并载入目录下指定后缀名的字体文件。
	void
	LoadFontFile(CPATH); //按路径加入字体文件并载入字体信息。

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


//以路径 path 创建字体缓存，指针存储至 p 。
inline void
CreateFontCache(YFontCache*& p, CPATH path)
{
	delete p;
	p = new YFontCache(path);
}

//销毁 p 指向的字体缓存。
inline void
DestroyFontCache(YFontCache*& p)
{
	delete p;
	p = NULL;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

