/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfont.h
\ingroup Adaptor
\brief 平台无关的字体缓存库。
\version 0.7092;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 22:02:40 + 08:00;
\par 修改时间:
	2010-12-17 18:56 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YFontCache;
*/


#ifndef INCLUDED_YFONT_H_
#define INCLUDED_YFONT_H_

#include "../Core/yfunc.hpp"
#include "../Core/ystring.h"

#define GLYPH_CACHE_SIZE	(512 << 10)

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

//! \brief 默认字体大小（pt）。
const u8 DEFAULT_FONT_SIZE	= 14;

//前向声明。
class Font;
class FontFamily;
class Typeface;
class FontFile;
class YFontCache;


//! \brief 字体样式。
struct EFontStyle
{
public:
	typedef enum Styles
	{
		Regular = 0, //!< 常规字体。
		Bold = 1, //!< 粗体。
		Italic = 2, //!< 斜体。
		Underline = 4, //!< 下划线。
		Strikeout = 8, //!< 删除线。
	} Styles;

private:
	Styles style;

public:
	EFontStyle(Styles = Regular);

	DefConverter(const Styles&, style);
	operator Styles&();

	/*!
	\brief 取样式名称。
	\note 断言检查。
	\note 无异常抛出。
	*/
	const char*
	GetName() const ythrow();
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


//! \brief 字型家族 (Typeface Family) 标识。
class FontFamily
{
	friend class Typeface;
	friend class YFontCache;

public:
	typedef set<Typeface*> FTypes; //!< 字型组类型。
	typedef map<const FT_String*, Typeface*, deref_str_comp<FT_String> >
		FTypesIndex; //!< 字型组索引类型。

	YFontCache& Cache;

private:
	FT_String* family_name;
	FTypes sTypes; //!< 字型组类型。
	FTypesIndex mTypesIndex; //!< 字型组索引类型。

public:
	/*!
	\brief 使用字体缓存引用和名称构造字型家族。
	*/
	FontFamily(YFontCache&, const FT_String*);
	/*!
	\brief 析构函数。
	\note 无异常抛出。
	*/
	~FontFamily() ythrow();

	/*!
	\brief 比较：相等关系。
	*/
	bool
	operator==(const FontFamily&) const;
	/*!
	\brief 比较：严格递增偏序关系。
	*/
	bool
	operator<(const FontFamily&) const;


	DefGetter(const FT_String*, FamilyName, family_name)
	/*!
	\brief 取指定样式的字型指针。
	\note 若非 Regular 样式失败则尝试取 Regular 样式的字型指针。
	*/
	Typeface*
	GetTypefacePtr(EFontStyle) const;
	/*!
	\brief 取指定样式名称的字型指针。
	*/
	Typeface*
	GetTypefacePtr(const FT_String*) const;

private:
	/*!
	\brief 向字型组和字型组索引添加字型对象。
	*/
	void
	operator+=(Typeface&);
	/*!
	\brief 从字型组和字型组索引中移除指定字型对象。
	*/
	bool
	operator-=(Typeface&);
};


//! \brief 字型标识。
class Typeface
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
	/*!
	\brief 使用字体缓存引用在指定字体文件读取指定索引的字型并构造对象。
	*/
	Typeface(YFontCache&, const FontFile&, u32 = 0
		/*, const bool bb = false,
		const bool bi = false, const bool bu = false*/);
	/*!
	\brief 析构函数。
	\note 无异常抛出。
	*/
	~Typeface() ythrow();

	/*!
	\brief 比较：相等关系。
	*/
	bool
	operator==(const Typeface&) const;
	/*!
	\brief 比较：严格递增偏序关系。
	*/
	bool
	operator<(const Typeface&) const;

	DefGetter(const FontFamily*, FontFamilyPtr, pFontFamily)
	DefGetter(const FT_String*, FamilyName, pFontFamily
		? pFontFamily->GetFamilyName() : NULL)
	DefGetter(const FT_String*, StyleName, style_name)
};


//! \brief 字体文件。
class FontFile
{
public:
	static const u16 MaxFontPathLength = MAX_FILENAME_LENGTH; \
		//!< 最大字体文件路径长度。

private:
	CPATH path;
	FT_Library& library;
	FT_Long nFace;

public:
	/*!
	\brief 使用路径字符串和本地字体库文件构造字体文件对象。
	*/
	FontFile(CPATH, FT_Library&);
	/*!
	\brief 使用路径字符串和本地字体库文件构造字体文件对象。
	\note 两个路径字符串被串接作为字体文件路径。
	*/
	FontFile(CPATH, const char*, FT_Library&);

	/*!
	\brief 比较：相等关系。
	*/
	bool
	operator==(const FontFile&) const;
	/*!
	\brief 比较：严格递增偏序关系。
	*/
	bool
	operator<(const FontFile&) const;

	DefGetter(const char*, Path, path)
	DefGetter(s32, FaceN, nFace)

	/*!
	\brief 重新读取字体文件，载入全部字体。
	*/
	void
	ReloadFaces();
};


/*!
\brief 取默认字型。
\note 断言检查。
\note 无异常抛出。
*/
const Typeface*
GetDefaultTypefacePtr() ythrow();

/*!
\brief 取默认字型家族引用。
*/
const FontFamily&
GetDefaultFontFamily() ythrow();


//! \brief 字体：字模，包含字型和大小。
class Font
{
public:
	typedef u8 SizeType;

	static const SizeType DefSize, MinSize, MaxSize;

private:
	static Font* pDefFont; //!< 默认 Shell 字体。

	const FontFamily* pFontFamily;
	EFontStyle Style;
	SizeType Size;

public:
	/*!
	\brief 构造指定字型家族、大小和样式的字体对象。
	*/
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
	/*!
	\brief 取字体对应的字符高度。
	*/
	SizeType
	GetHeight() const;

/*	FontFamily&
	SetFontFamily(const FontFamily&);*/
	DefSetter(EFontStyle, Style, Style)
	/*!
	\brief 设置字体大小。
	*/
	void
	SetSize(SizeType = DefSize);
	/*!
	\brief 设置字体并更新字体缓存中当前处理的字体。
	*/
	void
	SetFont(const Font&);

	/*!
	\brief 更新字体缓存中当前处理的字体。
	*/
	bool
	Update();

	/*!
	\brief 更新字体缓存中当前处理的字体大小。
	*/
	void
	UpdateSize();

	/*!
	\brief 初始化默认字体。
	*/
	static bool
	InitializeDefault();

	/*!
	\brief 释放默认字体。
	*/
	static void
	ReleaseDefault();
};


//! \brief 字符位图。
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
	/*!
	\brief 使用本机类型对象构造字符位图对象。
	*/
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


//! \brief 字体缓存。
class YFontCache : public YObject
{
	friend class Typeface;

public:
	typedef YObject ParentType;

private:
	FT_Library library; //!< 库实例。
	FTC_Manager manager; //!< 内存管理器实例。
	FTC_ScalerRec scaler;
	FTC_CMapCache cmapCache;
	FTC_SBitCache sbitCache;
	FTC_SBit sbit;

public:
	typedef set<const FontFile*, deref_comp<const FontFile> >
		FFiles; //!< 字体文件组类型。
	typedef set<Typeface*, deref_comp<const Typeface> >
		FTypes; //!< 字型组类型。
	typedef set<FontFamily*, deref_comp<FontFamily> >
		FFaces; //!< 字型家族组类型。
	typedef map<const FT_String*, FontFamily*, deref_str_comp<FT_String> >
		FFacesIndex; //!< 字型家族组索引类型。

private:
	FFiles sFiles; //!< 字体文件组。
	FTypes sTypes; //!< 字型组。
	FFaces sFaces; //!< 字型家族组索引。
	FFacesIndex mFacesIndex; //!< 字型家族组索引。

	Typeface* pDefaultFace; //!< 默认字型指针。
	Typeface* pFace; //!< 当前处理的字型指针。
	Font::SizeType curSize; //!< 当前处理的字体大小。

public:
	/*!
	\brief 构造：读取指定路径的字体文件并分配指定大小的缓存空间。
	*/
	explicit
	YFontCache(CPATH, u32 cacheSize = GLYPH_CACHE_SIZE);
	/*!
	\brief 析构：释放空间。
	*/
	virtual
	~YFontCache();

private:
	/*!
	\brief 取当前处理的内部字型结构体指针。
	*/
	FT_Face
	GetInternalFaceInfo() const;

public:
	DefGetter(const FFiles&, Files, sFiles) //!< 取字体文件组。
	DefGetter(const FTypes&, Types, sTypes) //!< 取字型组。
	DefGetter(const FFaces&, Faces, sFaces) //!< 取字型家族组。
	DefGetter(const FFacesIndex&, FacesIndex, mFacesIndex) \
		//!< 取字型家族组索引。
	DefGetter(FFiles::size_type, FilesN, sFiles.size()) \
		//!< 取字体文件组储存的文件数。
	DefGetter(FTypes::size_type, TypesN, sTypes.size()) \
		//!< 取字型组储存的字型数。
	DefGetter(FFaces::size_type, FacesN, sFaces.size()) \
		//!< 取字型家族组储存的字型家族数。
//	Font*
//	GetFontPtr() const;
	/*!
	\brief 取指定名称字型家族指针。
	*/
	const FontFamily*
	GetFontFamilyPtr(const FT_String*) const;
	/*!
	\brief 取默认字型指针。
	*/
	const Typeface*
	GetDefaultTypefacePtr() const;
	DefGetter(const Typeface*, TypefacePtr, pFace) //!< 取当前处理的字型指针。
//	Typeface*
//	GetTypefacePtr(u16) const; //!< 取字型组储存的指定索引的字型指针。
	/*!
	\brief 取指定名称字型指针。
	*/
	const Typeface*
	GetTypefacePtr(const FT_String*, const FT_String*) const;
	DefGetter(u8, FontSize, curSize) //!< 取当前处理的字体大小。
	/*!
	\brief 取当前字型和大小渲染的指定字符的字形。
	*/
	CharBitmap
	GetGlyph(fchar_t);
	/*!
	\brief 取跨距。
	*/
	s8
	GetAdvance(fchar_t, FTC_SBit sbit = NULL);
	/*!
	\brief 取行高。
	*/
	u8
	GetHeight() const;
	/*!
	\brief 取升部。
	*/
	s8
	GetAscender() const;
	/*!
	\brief 取降部。
	*/
	s8
	GetDescender() const;

	/*!
	\brief 设置字型组中指定的字型为当前字型。
	\note 忽略不属于字型组的字型。
	*/
	bool
	SetTypeface(Typeface*);
	/*!
	\brief 设置当前处理的字体大小。
	\note 0 表示默认字体大小。
	*/
	void
	SetFontSize(Font::SizeType);

public:
	//SetFont;

private:
	/*!
	\brief 向字体文件组添加字体文件对象。
	*/
	virtual void
	operator+=(const FontFile&);
	/*!
	\brief 从字体文件组中移除指定字体文件对象。
	*/
	virtual bool
	operator-=(const FontFile&);

	/*!
	\brief 向字型组添加字型对象。
	*/
	virtual void
	operator+=(Typeface&);
	/*!
	\brief 从字型组中移除指定字型对象。
	*/
	virtual bool
	operator-=(Typeface&);

	/*!
	\brief 向字型家族组添加字型对象。
	*/
	virtual void
	operator+=(FontFamily&);
	/*!
	\brief 从字型家族组中移除指定字型对象。
	*/
	virtual bool
	operator-=(FontFamily&);

	/*!
	\brief 从字体文件组中载入字体信息。
	*/
	void
	LoadTypefaces();

	/*!
	\brief 从指定字体文件中载入字体信息。
	*/
	void
	LoadTypefaces(const FontFile&);

public:
	/*!
	\brief 读取字体文件目录并载入目录下指定后缀名的字体文件。
	*/
	void
	LoadFontFileDirectory(CPATH, CPATH = "ttf");

	/*!
	\brief 按路径添加字体文件并载入字体信息。
	\note 无异常抛出。
	*/
	void
	LoadFontFile(CPATH) ythrow();

private:
	/*!
	\brief 清除字体文件组。
	*/
	void
	ClearFontFiles();

	/*!
	\brief 清除字型组。
	*/
	void
	ClearTypefaces();

	/*!
	\brief 清除字型家族组。
	*/
	void
	ClearFontFamilies();

	/*!
	\brief 清除容器。
	*/
	void
	ClearContainers();

public:
	/*!
	\brief 清除缓存。
	*/
	void
	ClearCache();
};


/*!
\ingroup HelperFunction
\brief 以指定路径 path 创建字体缓存。
\note 指针存储至指定指针。
*/
void
CreateFontCache(YFontCache*&, CPATH);

/*!
\ingroup HelperFunction
\brief 销毁指定指针指向的字体缓存。
*/
void
DestroyFontCache(YFontCache*&);

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

