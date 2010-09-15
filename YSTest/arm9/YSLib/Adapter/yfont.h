// YSLib::Adapter::YFontCache by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:02:40;
// UTime = 2010-9-11 0:03;
// Version = 0.6844;


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
class Font;
class FontFamily;
class Typeface;
class FontFile;
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
	GetName() const ythrow(); //取样式名称。
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
class FontFamily// : implements GIEquatable<FontFamily>, implements GILess<FontFamily>, implements GIContainer<Typeface>
{
	friend class Typeface;
	friend class YFontCache;

public:
	typedef std::set<const Typeface*> FTypes; //字型组类型。
	typedef std::map<const FT_String*, const Typeface*, deref_str_comp<FT_String> > FTypesIndex; //字型组索引类型。

	YFontCache& Cache;

private:
	FT_String* family_name;
	FTypes sTypes; //字型组类型。
	FTypesIndex mTypesIndex; //字型组索引类型。

public:
	FontFamily(YFontCache&, const FT_String*);
	~FontFamily() ythrow();

	bool
	operator==(const FontFamily&) const;
	bool
	operator<(const FontFamily&) const;

	DefGetter(const FT_String*, FamilyName, family_name)
	const Typeface*
	GetTypefacePtr(EFontStyle) const; //取指定样式的字型指针，若非 Regular 样式失败则尝试取 Regular 样式的字型指针。
	const Typeface*
	GetTypefacePtr(const FT_String*) const; //取指定样式名称的字型指针。

private:
	void
	operator+=(const Typeface&); //向字型组和字型组索引添加字型对象。
	bool
	operator-=(const Typeface&); //从字型组和字型组索引中移除指定字型对象。
};


//字型标识。
class Typeface// : implements GIEquatable<Typeface>, implements GILess<Typeface>
{
	friend class YFontCache;
	friend FT_Error simpleFaceRequester(FTC_FaceID, FT_Library, FT_Pointer, FT_Face*);

//	static const FT_Matrix MNormal, MOblique;
public:
	YFontCache& Cache;
	const FontFile& File;

private:
	FontFamily* pFontFamily;
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
	Typeface(YFontCache&, const FontFile&, u32 = 0/*, const bool bb = false, const bool bi = false, const bool bu = false*/);
	~Typeface() ythrow();

	bool
	operator==(const Typeface&) const;
	bool
	operator<(const Typeface&) const;

	DefGetter(const FontFamily*, FontFamilyPtr, pFontFamily)
	DefGetter(const FT_String*, FamilyName, pFontFamily ? pFontFamily->GetFamilyName() : NULL)
	DefGetter(const FT_String*, StyleName, style_name)
};


//字体文件。
class FontFile// : implements GIEquatable<FontFile>, implements GILess<FontFile>
{
public:
	static const u16 MaxFontPathLength = YC_MAX_PATH; //最大字体文件路径长度。

private:
	CPATH path;
	FT_Library& library;
	FT_Long nFace;

public:
	FontFile(CPATH, FT_Library&);
	FontFile(CPATH, const char*, FT_Library&);

	bool
	operator==(const FontFile&) const;
	bool
	operator<(const FontFile&) const;

	DefGetter(const char*, Path, path)
	DefGetter(s32, FaceN, nFace)

	void
	ReloadFaces(); //读取字体文件，载入全部字体。
};


//取默认字型。
const Typeface* GetDefaultTypefacePtr();

//取默认字型家族。
inline const FontFamily*
GetDefaultFontFamilyPtr()
{
	const Typeface* p(GetDefaultTypefacePtr());

	YAssert(p != NULL,
		"In function \"inline const FontFamily*\n"
		"GetDefaultFontFamilyPtr()\": \n"
		"The default font face pointer is null.");
	return p->GetFontFamilyPtr();
}
inline const FontFamily&
GetDefaultFontFamily()
{
	const FontFamily* p(GetDefaultFontFamilyPtr());

	YAssert(p != NULL,
		"In function \"inline const FontFamily&\n"
		"GetDefaultFontFamily()\": \n"
		"The default font family pointer is null.");
	return *p;
}


//字体：字模，包含字型和大小。
class Font
{
public:
	typedef u8 SizeType;

	static const SizeType DefSize, MinSize, MaxSize;

private:
	static Font* pDefFont; //默认 Shell 字体。

	const FontFamily* pFontFamily;
	EFontStyle Style;
	SizeType Size;

public:
	explicit
	Font(const FontFamily& = GetDefaultFontFamily(), SizeType = DefSize, EFontStyle = EFontStyle::Regular);

	DefBoolGetter(Bold, Style | EFontStyle::Bold)
	DefBoolGetter(Italic, Style | EFontStyle::Italic)
	DefBoolGetter(Underline, Style | EFontStyle::Underline)
	DefBoolGetter(Strikeout, Style | EFontStyle::Strikeout)

	DefStaticGetter(const Font&, Default, *pDefFont)
	DefGetter(const FontFamily&, FontFamily, *pFontFamily)
	DefGetter(EFontStyle, Style, Style)
	DefGetter(SizeType, Size, Size)
	DefGetter(YFontCache&, Cache, GetFontFamily().Cache)
	DefGetterMember(const FT_String*, FamilyName, GetFontFamily())
	DefGetter(const FT_String*, StyleName, Style.GetName())
	SizeType
	GetHeight() const;

/*	FontFamily&
	SetFontFamily(const FontFamily&);*/
	DefSetter(EFontStyle, Style, Style)
	void
	SetSize(SizeType = DefSize);
	void
	SetFont(const Font&);

	bool
	Update(); //更新字体缓存中当前处理的字体。
	void
	UpdateSize(); //更新字体缓存中当前处理的字体大小。

	static bool
	InitializeDefault(); //初始化默认字体。
	static void
	ReleaseDefault(); //释放默认字体。
};

inline void
Font::SetFont(const Font& f)
{
	*this = f;
	Update();
}


//字符位图。
class CharBitmap
{
public:
	typedef FTC_SBit NativeType;
	typedef FT_Byte* BufferType;
	typedef FT_Byte ScaleType;
	typedef FT_Char SignedScaleType;

private:
	FTC_SBit bitmap;

public:
	CharBitmap(const NativeType&);

	DefConverter(NativeType, bitmap)

	DefGetter(BufferType, Buffer, bitmap->buffer)
	DefGetter(ScaleType, Width, bitmap->width)
	DefGetter(ScaleType, Height, bitmap->height)
	DefGetter(SignedScaleType, Left, bitmap->left)
	DefGetter(SignedScaleType, Top, bitmap->top)
	DefGetter(SignedScaleType, XAdvance, bitmap->xadvance)
	DefGetter(SignedScaleType, YAdvance, bitmap->yadvance)
};

inline
CharBitmap::CharBitmap(const CharBitmap::NativeType& b)
: bitmap(b)
{}


//字体缓存。
class YFontCache : public YObject,
	implements GIContainer<const FontFile>, implements GIContainer<const Typeface>, implements GIContainer<FontFamily>
{
	friend class Typeface;

private:
	FT_Library library; //库实例。
	FTC_Manager manager; //内存管理器实例。
	FTC_ScalerRec scaler;
	FTC_CMapCache cmapCache;
	FTC_SBitCache sbitCache;
	FTC_SBit sbit;

public:
	typedef std::set<const FontFile*, deref_comp<const FontFile> > FFiles; //字体文件组类型。
	typedef std::set<const Typeface*, deref_comp<const Typeface> > FTypes; //字型组类型。
	typedef std::set<FontFamily*, deref_comp<FontFamily> > FFaces; //字型家族组类型。
	typedef std::map<const FT_String*, FontFamily*, deref_str_comp<FT_String> > FFacesIndex; //字型家族组索引类型。

private:
	FFiles sFiles; //字体文件组。
	FTypes sTypes; //字型组。
	FFaces sFaces; //字型家族组索引。
	FFacesIndex mFacesIndex; //字型家族组索引。

	const Typeface* pDefaultFace; //默认字型指针。
	const Typeface* pFace; //当前处理的字型指针。
	Font::SizeType curSize; //当前处理的字体大小。

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
	DefGetter(const FFaces&, Faces, sFaces) //取字型家族组。
	DefGetter(const FFacesIndex&, FacesIndex, mFacesIndex) //取字型家族组索引。
	DefGetter(FFiles::size_type, FilesN, sFiles.size()) //取字体文件组储存的文件数。
	DefGetter(FTypes::size_type, TypesN, sTypes.size()) //取字型组储存的字型数。
	DefGetter(FFaces::size_type, FacesN, sFaces.size()) //取字型家族组储存的字型家族数。
//	Font*
//	GetFontPtr() const;
	const FontFamily*
	GetFontFamilyPtr(const FT_String*) const; //取指定名称字型家族指针。
	const Typeface*
	GetDefaultTypefacePtr() const; //取默认字型指针。
	DefGetter(const Typeface*, TypefacePtr, pFace) //取当前处理的字型指针。
//	Typeface*
//	GetTypefacePtr(u16) const; //取字型组储存的指定索引的字型指针。
	const Typeface*
	GetTypefacePtr(const FT_String*, const FT_String*) const; //取指定名称字型指针。
	DefGetter(u8, FontSize, curSize) //取当前处理的字体大小。
	CharBitmap
	GetGlyph(fchar_t); //取当前字型和大小渲染的指定字符的字形。
	s8
	GetAdvance(fchar_t, FTC_SBit sbit = NULL); //取跨距。
	u8
	GetHeight() const; //取行高。
	s8
	GetAscender() const; //取升部。
	s8
	GetDescender() const; //取降部。

	bool
	SetTypeface(const Typeface*); //设置字型组中指定的字型为当前字型（忽略不属于字型组的字型）。
	void
	SetFontSize(Font::SizeType); //设置当前处理的字体大小（0 表示默认字体大小）。

public:
	//SetFont;

private:
	void
	operator+=(const FontFile&); //向字体文件组添加字体文件对象。
	bool
	operator-=(const FontFile&); //从字体文件组中移除指定字体文件对象。

	void
	operator+=(const Typeface&); //向字型组添加字型对象。
	bool
	operator-=(const Typeface&); //从字型组中移除指定字型对象。

	void
	operator+=(FontFamily&); //向字型家族组添加字型对象。
	bool
	operator-=(FontFamily&); //从字型家族组中移除指定字型对象。

	void
	LoadTypefaces(); //从字体文件组中载入字体信息。
	void
	LoadTypefaces(const FontFile&); //从指定字体文件中载入字体信息。

public:
	void
	LoadFontFileDirectory(CPATH, CPATH = "ttf"); //读取字体文件目录并载入目录下指定后缀名的字体文件。
	void
	LoadFontFile(CPATH) ythrow(); //按路径加入字体文件并载入字体信息。

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

