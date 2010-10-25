// YSLib::Adaptor::YFontCache by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:02:40 + 08:00;
// UTime = 2010-10-24 17:12 + 08:00;
// Version = 0.7008;


#ifndef INCLUDED_YFONT_H_
#define INCLUDED_YFONT_H_

// YFontCache ：平台无关的字体缓存库。

#include "../Core/yfunc.hpp"
#include "../Core/ystring.h"

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
class FontFamily// : implements GIEquatable<FontFamily>,
	// implements GILess<FontFamily>, implements GIContainer<Typeface>
{
	friend class Typeface;
	friend class YFontCache;

public:
	typedef set<const Typeface*> FTypes; //字型组类型。
	typedef map<const FT_String*, const Typeface*,
		deref_str_comp<FT_String> > FTypesIndex; //字型组索引类型。

	YFontCache& Cache;

private:
	FT_String* family_name;
	FTypes sTypes; //字型组类型。
	FTypesIndex mTypesIndex; //字型组索引类型。

public:
	//********************************
	//名称:		FontFamily
	//全名:		YSLib::Drawing::FontFamily::FontFamily
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	YFontCache &
	//形式参数:	const FT_String *
	//功能概要:	使用字体缓存引用和名称构造字型家族。
	//备注:		
	//********************************
	FontFamily(YFontCache&, const FT_String*);
	//********************************
	//名称:		~FontFamily
	//全名:		YSLib::Drawing::FontFamily::~FontFamily
	//可访问性:	public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构函数。
	//备注:		无异常抛出。
	//********************************
	~FontFamily() ythrow();

	//********************************
	//名称:		operator==
	//全名:		YSLib::Drawing::FontFamily::operator==
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const FontFamily &
	//功能概要:	比较：相等关系。
	//备注:		
	//********************************
	bool
	operator==(const FontFamily&) const;
	//********************************
	//名称:		operator<
	//全名:		YSLib::Drawing::FontFamily::operator<
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const FontFamily &
	//功能概要:	比较：严格递增偏序关系。
	//备注:		
	//********************************
	bool
	operator<(const FontFamily&) const;


	DefGetter(const FT_String*, FamilyName, family_name)
	//********************************
	//名称:		GetTypefacePtr
	//全名:		YSLib::Drawing::FontFamily::GetTypefacePtr
	//可访问性:	public 
	//返回类型:	
	//修饰符:	const
	//形式参数:	const FT_String *
	//形式参数:	FamilyName
	//形式参数:	family_name
	//功能概要:	取指定样式的字型指针。
	//备注:		若非 Regular 样式失败则尝试取 Regular 样式的字型指针。
	//********************************
	const Typeface*
	GetTypefacePtr(EFontStyle) const;
	//********************************
	//名称:		GetTypefacePtr
	//全名:		YSLib::Drawing::FontFamily::GetTypefacePtr
	//可访问性:	public 
	//返回类型:	const Typeface*
	//修饰符:	const
	//形式参数:	const FT_String *
	//功能概要:	取指定样式名称的字型指针。
	//备注:		
	//********************************
	const Typeface*
	GetTypefacePtr(const FT_String*) const;

private:
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Drawing::FontFamily::operator+=
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Typeface &
	//功能概要:	向字型组和字型组索引添加字型对象。
	//备注:		
	//********************************
	void
	operator+=(const Typeface&);
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Drawing::FontFamily::operator-=
	//可访问性:	private 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const Typeface &
	//功能概要:	从字型组和字型组索引中移除指定字型对象。
	//备注:		
	//********************************
	bool
	operator-=(const Typeface&);
};


//字型标识。
class Typeface// : implements GIEquatable<Typeface>,
	// implements GILess<Typeface>
{
	friend class YFontCache;
	friend FT_Error simpleFaceRequester(FTC_FaceID,
		FT_Library, FT_Pointer, FT_Face*);

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
	vector<u8> fixSizes;*/

public:
	//********************************
	//名称:		Typeface
	//全名:		YSLib::Drawing::Typeface::Typeface
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	YFontCache &
	//形式参数:	const FontFile &
	//形式参数:	u32
	//功能概要:	使用字体缓存引用在指定字体文件读取指定索引的字型并构造对象。
	//备注:		
	//********************************
	Typeface(YFontCache&, const FontFile&, u32 = 0
		/*, const bool bb = false,
		const bool bi = false, const bool bu = false*/);
	//********************************
	//名称:		~Typeface
	//全名:		YSLib::Drawing::Typeface::~Typeface
	//可访问性:	public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构函数。
	//备注:		无异常抛出。
	//********************************
	~Typeface() ythrow();

	//********************************
	//名称:		operator==
	//全名:		YSLib::Drawing::Typeface::operator==
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const Typeface &
	//功能概要:	比较：相等关系。
	//备注:		
	//********************************
	bool
	operator==(const Typeface&) const;
	//********************************
	//名称:		operator<
	//全名:		YSLib::Drawing::Typeface::operator<
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const Typeface &
	//功能概要:	比较：严格递增偏序关系。
	//备注:		
	//********************************
	bool
	operator<(const Typeface&) const;

	DefGetter(const FontFamily*, FontFamilyPtr, pFontFamily)
	DefGetter(const FT_String*, FamilyName, pFontFamily
		? pFontFamily->GetFamilyName() : NULL)
	DefGetter(const FT_String*, StyleName, style_name)
};


//字体文件。
class FontFile// : implements GIEquatable<FontFile>,
	// implements GILess<FontFile>
{
public:
	static const u16 MaxFontPathLength = YC_MAX_PATH; //最大字体文件路径长度。

private:
	CPATH path;
	FT_Library& library;
	FT_Long nFace;

public:
	//********************************
	//名称:		FontFile
	//全名:		YSLib::Drawing::FontFile::FontFile
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	CPATH
	//形式参数:	FT_Library &
	//功能概要:	使用路径字符串和本地字体库文件构造字体文件对象。
	//备注:		
	//********************************
	FontFile(CPATH, FT_Library&);
	//********************************
	//名称:		FontFile
	//全名:		YSLib::Drawing::FontFile::FontFile
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	CPATH
	//形式参数:	FT_Library &
	//功能概要:	使用路径字符串和本地字体库文件构造字体文件对象。
	//备注:		两个路径字符串被串接作为字体文件路径。
	//********************************
	FontFile(CPATH, const char*, FT_Library&);

	//********************************
	//名称:		operator==
	//全名:		YSLib::Drawing::FontFile::operator==
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const FontFile &
	//功能概要:	比较：相等关系。
	//备注:		
	//********************************
	bool
	operator==(const FontFile&) const;
	//********************************
	//名称:		operator<
	//全名:		YSLib::Drawing::FontFile::operator<
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const FontFile &
	//功能概要:	比较：严格递增偏序关系。
	//备注:		
	//********************************
	bool
	operator<(const FontFile&) const;

	DefGetter(const char*, Path, path)
	DefGetter(s32, FaceN, nFace)

	//********************************
	//名称:		ReloadFaces
	//全名:		YSLib::Drawing::FontFile::ReloadFaces
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	重新读取字体文件，载入全部字体。
	//备注:		
	//********************************
	void
	ReloadFaces();
};


//取默认字型。
const Typeface* GetDefaultTypefacePtr();

//********************************
//名称:		GetDefaultFontFamilyPtr
//全名:		YSLib::Drawing::GetDefaultFontFamilyPtr
//可访问性:	public 
//返回类型:	const FontFamily*
//修饰符:	
//功能概要:	取默认字型家族指针。
//备注:		
//********************************
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
//********************************
//名称:		GetDefaultFontFamily
//全名:		YSLib::Drawing::GetDefaultFontFamily
//可访问性:	public 
//返回类型:	const FontFamily&
//修饰符:	
//功能概要:	取默认字型家族引用。
//备注:		
//********************************
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
	//********************************
	//名称:		Font
	//全名:		YSLib::Drawing::Font::Font
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const FontFamily &
	//形式参数:	SizeType
	//形式参数:	EFontStyle
	//功能概要:	构造指定字型家族、大小和样式的字体对象。
	//备注:		
	//********************************
	explicit
	Font(const FontFamily& = GetDefaultFontFamily(), SizeType = DefSize,
		EFontStyle = EFontStyle::Regular);

	DefPredicate(Bold, Style | EFontStyle::Bold)
	DefPredicate(Italic, Style | EFontStyle::Italic)
	DefPredicate(Underline, Style | EFontStyle::Underline)
	DefPredicate(Strikeout, Style | EFontStyle::Strikeout)

	DefStaticGetter(const Font&, Default, *pDefFont)
	DefGetter(const FontFamily&, FontFamily, *pFontFamily)
	DefGetter(EFontStyle, Style, Style)
	DefGetter(SizeType, Size, Size)
	DefGetter(YFontCache&, Cache, GetFontFamily().Cache)
	DefGetterMember(const FT_String*, FamilyName, GetFontFamily())
	DefGetter(const FT_String*, StyleName, Style.GetName())
	//********************************
	//名称:		GetHeight
	//全名:		YSLib::Drawing::Font::GetHeight
	//可访问性:	public 
	//返回类型:	YSLib::Drawing::Font::SizeType
	//修饰符:	const
	//功能概要:	取字体对应的字符高度。
	//备注:		
	//********************************
	SizeType
	GetHeight() const;

/*	FontFamily&
	SetFontFamily(const FontFamily&);*/
	DefSetter(EFontStyle, Style, Style)
	//********************************
	//名称:		SetSize
	//全名:		YSLib::Drawing::Font::SetSize
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	SizeType
	//功能概要:	设置字体大小。
	//备注:		
	//********************************
	void
	SetSize(SizeType = DefSize);
	//********************************
	//名称:		SetFont
	//全名:		YSLib::Drawing::Font::SetFont
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Font &
	//功能概要:	设置字体并更新字体缓存中当前处理的字体。
	//备注:		
	//********************************
	void
	SetFont(const Font&);

	//********************************
	//名称:		Update
	//全名:		YSLib::Drawing::Font::Update
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	更新字体缓存中当前处理的字体。
	//备注:		
	//********************************
	bool
	Update();

	//********************************
	//名称:		UpdateSize
	//全名:		YSLib::Drawing::Font::UpdateSize
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	更新字体缓存中当前处理的字体大小。
	//备注:		
	//********************************
	void
	UpdateSize();

	//********************************
	//名称:		InitializeDefault
	//全名:		YSLib::Drawing::Font::InitializeDefault
	//可访问性:	public static 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	初始化默认字体。
	//备注:		
	//********************************
	static bool
	InitializeDefault();

	//********************************
	//名称:		ReleaseDefault
	//全名:		YSLib::Drawing::Font::ReleaseDefault
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	释放默认字体。
	//备注:		
	//********************************
	static void
	ReleaseDefault();
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
	//********************************
	//名称:		CharBitmap
	//全名:		YSLib::Drawing::CharBitmap::CharBitmap
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const NativeType &
	//功能概要:	使用本机类型对象构造字符位图对象。
	//备注:		
	//********************************
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
	typedef set<const FontFile*,
		deref_comp<const FontFile> > FFiles; //字体文件组类型。
	typedef set<const Typeface*,
		deref_comp<const Typeface> > FTypes; //字型组类型。
	typedef set<FontFamily*,
		deref_comp<FontFamily> > FFaces; //字型家族组类型。
	typedef map<const FT_String*, FontFamily*,
		deref_str_comp<FT_String> > FFacesIndex; //字型家族组索引类型。

private:
	FFiles sFiles; //字体文件组。
	FTypes sTypes; //字型组。
	FFaces sFaces; //字型家族组索引。
	FFacesIndex mFacesIndex; //字型家族组索引。

	const Typeface* pDefaultFace; //默认字型指针。
	const Typeface* pFace; //当前处理的字型指针。
	Font::SizeType curSize; //当前处理的字体大小。

public:
	//********************************
	//名称:		YFontCache
	//全名:		YSLib::Drawing::YFontCache::YFontCache
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	CPATH
	//形式参数:	u32 cacheSize
	//功能概要:	构造：读取指定路径的字体文件并分配指定大小的缓存空间。
	//备注:		
	//********************************
	explicit
	YFontCache(CPATH, u32 cacheSize = GLYPH_CACHE_SIZE);
	//********************************
	//名称:		~YFontCache
	//全名:		YSLib::Drawing::YFontCache::~YFontCache
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构：释放空间。
	//备注:		
	//********************************
	virtual
	~YFontCache();

private:
	//********************************
	//名称:		GetInternalFaceInfo
	//全名:		YSLib::Drawing::YFontCache::GetInternalFaceInfo
	//可访问性:	private 
	//返回类型:	FT_Face
	//修饰符:	const
	//功能概要:	取当前处理的内部字型结构体指针。
	//备注:		
	//********************************
	FT_Face
	GetInternalFaceInfo() const;

public:
	DefGetter(const FFiles&, Files, sFiles) //取字体文件组。
	DefGetter(const FTypes&, Types, sTypes) //取字型组。
	DefGetter(const FFaces&, Faces, sFaces) //取字型家族组。
	DefGetter(const FFacesIndex&, FacesIndex, mFacesIndex) \
		//取字型家族组索引。
	DefGetter(FFiles::size_type, FilesN, sFiles.size()) \
		//取字体文件组储存的文件数。
	DefGetter(FTypes::size_type, TypesN, sTypes.size()) \
		//取字型组储存的字型数。
	DefGetter(FFaces::size_type, FacesN, sFaces.size()) \
		//取字型家族组储存的字型家族数。
//	Font*
//	GetFontPtr() const;
	//********************************
	//名称:		GetFontFamilyPtr
	//全名:		YSLib::Drawing::YFontCache::GetFontFamilyPtr
	//可访问性:	public 
	//返回类型:	
	//修饰符:	const
	//形式参数:	const FFacesIndex &
	//形式参数:	FacesIndex
	//形式参数:	mFacesIndex
	//功能概要:	取指定名称字型家族指针。
	//备注:		
	//********************************
	const FontFamily*
	GetFontFamilyPtr(const FT_String*) const;
	//********************************
	//名称:		GetDefaultTypefacePtr
	//全名:		YSLib::Drawing::YFontCache::GetDefaultTypefacePtr
	//可访问性:	public 
	//返回类型:	const Typeface*
	//修饰符:	const
	//功能概要:	取默认字型指针。
	//备注:		
	//********************************
	const Typeface*
	GetDefaultTypefacePtr() const;
	DefGetter(const Typeface*, TypefacePtr, pFace) //取当前处理的字型指针。
//	Typeface*
//	GetTypefacePtr(u16) const; //取字型组储存的指定索引的字型指针。
	//********************************
	//名称:		GetTypefacePtr
	//全名:		YSLib::Drawing::YFontCache::GetTypefacePtr
	//可访问性:	public 
	//返回类型:	
	//修饰符:	const
	//形式参数:	const Typeface *
	//形式参数:	TypefacePtr
	//形式参数:	pFace
	//功能概要:	取指定名称字型指针。
	//备注:		
	//********************************
	const Typeface*
	GetTypefacePtr(const FT_String*, const FT_String*) const;
	DefGetter(u8, FontSize, curSize) //取当前处理的字体大小。
	//********************************
	//名称:		GetGlyph
	//全名:		YSLib::Drawing::YFontCache::GetGlyph
	//可访问性:	public 
	//返回类型:	YSLib::Drawing::CharBitmap
	//修饰符:	
	//形式参数:	fchar_t
	//功能概要:	取当前字型和大小渲染的指定字符的字形。
	//备注:		
	//********************************
	CharBitmap
	GetGlyph(fchar_t);
	//********************************
	//名称:		GetAdvance
	//全名:		YSLib::Drawing::YFontCache::GetAdvance
	//可访问性:	public 
	//返回类型:	s8
	//修饰符:	
	//形式参数:	fchar_t
	//形式参数:	FTC_SBit sbit
	//功能概要:	取跨距。
	//备注:		
	//********************************
	s8
	GetAdvance(fchar_t, FTC_SBit sbit = NULL);
	//********************************
	//名称:		GetHeight
	//全名:		YSLib::Drawing::YFontCache::GetHeight
	//可访问性:	public 
	//返回类型:	u8
	//修饰符:	const
	//功能概要:	取行高。
	//备注:		
	//********************************
	u8
	GetHeight() const;
	//********************************
	//名称:		GetAscender
	//全名:		YSLib::Drawing::YFontCache::GetAscender
	//可访问性:	public 
	//返回类型:	s8
	//修饰符:	const
	//功能概要:	取升部。
	//备注:		
	//********************************
	s8
	GetAscender() const;
	//********************************
	//名称:		GetDescender
	//全名:		YSLib::Drawing::YFontCache::GetDescender
	//可访问性:	public 
	//返回类型:	s8
	//修饰符:	const
	//功能概要:	取降部。
	//备注:		
	//********************************
	s8
	GetDescender() const;

	//********************************
	//名称:		SetTypeface
	//全名:		YSLib::Drawing::YFontCache::SetTypeface
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const Typeface *
	//功能概要:	设置字型组中指定的字型为当前字型。
	//备注:		忽略不属于字型组的字型。
	//********************************
	bool
	SetTypeface(const Typeface*);
	//********************************
	//名称:		SetFontSize
	//全名:		YSLib::Drawing::YFontCache::SetFontSize
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Font::SizeType
	//功能概要:	设置当前处理的字体大小。
	//备注:		0 表示默认字体大小。
	//********************************
	void
	SetFontSize(Font::SizeType);

public:
	//SetFont;

private:
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Drawing::YFontCache::operator+=
	//可访问性:	ImplI(GIContainer<const FontFile>) private 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const FontFile &
	//功能概要:	向字体文件组添加字体文件对象。
	//备注:		
	//********************************
	ImplI(GIContainer<const FontFile>) void
	operator+=(const FontFile&);
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Drawing::YFontCache::operator-=
	//可访问性:	ImplI(GIContainer<const FontFile>) private 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const FontFile &
	//功能概要:	从字体文件组中移除指定字体文件对象。
	//备注:		
	//********************************
	ImplI(GIContainer<const FontFile>) bool
	operator-=(const FontFile&);

	//********************************
	//名称:		operator+=
	//全名:		YSLib::Drawing::YFontCache::operator+=
	//可访问性:	ImplI(GIContainer<const Typeface>) private 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Typeface &
	//功能概要:	向字型组添加字型对象。
	//备注:		
	//********************************
	ImplI(GIContainer<const Typeface>) void
	operator+=(const Typeface&);
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Drawing::YFontCache::operator-=
	//可访问性:	ImplI(GIContainer<const Typeface>) private 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const Typeface &
	//功能概要:	从字型组中移除指定字型对象。
	//备注:		
	//********************************
	ImplI(GIContainer<const Typeface>) bool
	operator-=(const Typeface&);

	//********************************
	//名称:		operator+=
	//全名:		YSLib::Drawing::YFontCache::operator+=
	//可访问性:	ImplI(GIContainer<const FontFamily>) private 
	//返回类型:	void
	//修饰符:	
	//形式参数:	FontFamily &
	//功能概要:	向字型家族组添加字型对象。
	//备注:		
	//********************************
	ImplI(GIContainer<const FontFamily>) void
	operator+=(FontFamily&);
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Drawing::YFontCache::operator-=
	//可访问性:	ImplI(GIContainer<const FontFamily>) private 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	FontFamily &
	//功能概要:	从字型家族组中移除指定字型对象。
	//备注:		
	//********************************
	ImplI(GIContainer<const FontFamily>) bool
	operator-=(FontFamily&);

	//********************************
	//名称:		LoadTypefaces
	//全名:		YSLib::Drawing::YFontCache::LoadTypefaces
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//功能概要:	从字体文件组中载入字体信息。
	//备注:		
	//********************************
	void
	LoadTypefaces();

	//********************************
	//名称:		LoadTypefaces
	//全名:		YSLib::Drawing::YFontCache::LoadTypefaces
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const FontFile &
	//功能概要:	从指定字体文件中载入字体信息。
	//备注:		
	//********************************
	void
	LoadTypefaces(const FontFile&);

public:
	//********************************
	//名称:		LoadFontFileDirectory
	//全名:		YSLib::Drawing::YFontCache::LoadFontFileDirectory
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	CPATH
	//形式参数:	CPATH
	//功能概要:	读取字体文件目录并载入目录下指定后缀名的字体文件。
	//备注:		
	//********************************
	void
	LoadFontFileDirectory(CPATH, CPATH = "ttf");

	//********************************
	//名称:		LoadFontFile
	//全名:		YSLib::Drawing::YFontCache::LoadFontFile
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	ythrow()
	//形式参数:	CPATH
	//功能概要:	按路径添加字体文件并载入字体信息。
	//备注:		无异常抛出。
	//********************************
	void
	LoadFontFile(CPATH) ythrow();

private:
	//********************************
	//名称:		ClearFontFiles
	//全名:		YSLib::Drawing::YFontCache::ClearFontFiles
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除字体文件组。
	//备注:		
	//********************************
	void
	ClearFontFiles();

	//********************************
	//名称:		ClearTypefaces
	//全名:		YSLib::Drawing::YFontCache::ClearTypefaces
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除字型组。
	//备注:		
	//********************************
	void
	ClearTypefaces();

	//********************************
	//名称:		ClearFontFamilies
	//全名:		YSLib::Drawing::YFontCache::ClearFontFamilies
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除字型家族组。
	//备注:		
	//********************************
	void
	ClearFontFamilies();

	//********************************
	//名称:		ClearContainers
	//全名:		YSLib::Drawing::YFontCache::ClearContainers
	//可访问性:	private 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除容器。
	//备注:		
	//********************************
	void
	ClearContainers();

public:
	//********************************
	//名称:		ClearCache
	//全名:		YSLib::Drawing::YFontCache::ClearCache
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除缓存。
	//备注:		
	//********************************
	void
	ClearCache();
};


//********************************
//名称:		CreateFontCache
//全名:		YSLib::Drawing::CreateFontCache
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	YFontCache * & p
//形式参数:	CPATH path
//功能概要:	以路径 path 创建字体缓存。
//备注:		指针存储至 p 。
//********************************
inline void
CreateFontCache(YFontCache*& p, CPATH path)
{
	delete p;
	p = new YFontCache(path);
}

//********************************
//名称:		DestroyFontCache
//全名:		YSLib::Drawing::DestroyFontCache
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	YFontCache * & p
//功能概要:	销毁 p 指向的字体缓存。
//备注:		
//********************************
inline void
DestroyFontCache(YFontCache*& p)
{
	delete p;
	p = NULL;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

