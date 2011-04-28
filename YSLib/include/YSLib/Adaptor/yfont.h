/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfont.h
\ingroup Adaptor
\brief 平台无关的字体缓存库。
\version 0.7242;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 22:02:40 +0800;
\par 修改时间:
	2011-04-26 16:18 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YFontCache;
*/


#ifndef YSL_INC_ADAPTOR_YFONT_H_
#define YSL_INC_ADAPTOR_YFONT_H_

#include "../Core/yfunc.hpp"
#include "../Core/yobject.h"
#include <string>
#include "../Core/yexcept.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

const u8 DEFAULT_FONT_SIZE(14); //!< 默认字体大小（ pt ）。
const size_t GLYPH_CACHE_SIZE(128 << 10); //!< 字形缓冲区大小（字节）。

//前向声明。
class Font;
class FontFamily;
class Typeface;
class FontFile;
class YFontCache;


//! \brief 字体样式。
struct FontStyle
{
public:
	typedef enum Styles
	{
		Regular = 0, //!< 常规字体。
		Bold = 1, //!< 粗体。
		Italic = 2, //!< 斜体。
		Underline = 4, //!< 下划线。
		Strikeout = 8 //!< 删除线。
	} Styles;

private:
	Styles style;

public:
	FontStyle(Styles = Regular);

	DefConverter(const Styles&, style);
	operator Styles&();

	/*!
	\brief 取样式名称。
	\note 断言检查。
	\note 无异常抛出。
	*/
	const char*
	GetName() const ynothrow;
};

inline
FontStyle::FontStyle(Styles s)
	: style(s)
{}

inline
FontStyle::operator Styles&()
{
	return style;
}


//! \brief 字型家族 (Typeface Family) 标识。
class FontFamily : public NonCopyable
{
	friend class Typeface;
	friend class YFontCache;

public:
	typedef std::string NameType;
	typedef set<Typeface*> FTypes; //!< 字型组类型。
	typedef map<const std::string/*Typeface::NameType*/, Typeface*>
		FTypesIndex; //!< 字型组索引类型。

	YFontCache& Cache;

private:
	NameType family_name;
	FTypes sTypes; //!< 字型组类型。
	FTypesIndex mTypesIndex; //!< 字型组索引类型。

public:
	/*!
	\brief 使用字体缓存引用和名称构造字型家族。
	*/
	FontFamily(YFontCache&, const NameType&);

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


	DefGetter(const NameType&, FamilyName, family_name)
	/*!
	\brief 取指定样式的字型指针。
	\note 若非 Regular 样式失败则尝试取 Regular 样式的字型指针。
	*/
	Typeface*
	GetTypefacePtr(FontStyle) const;
	/*!
	\brief 取指定样式名称的字型指针。
	*/
	Typeface*
	GetTypefacePtr(const std::string/*Typeface::NameType*/&) const;

private:
	/*!
	\brief 向字型组和字型组索引添加字型对象。
	\note 参数为空指针时忽略。
	*/
	void
	operator+=(Typeface*);
	/*!
	\brief 从字型组和字型组索引中移除指定字型对象。
	\note 参数为空指针时返回 false 。
	*/
	bool
	operator-=(Typeface*);
};


//! \brief 字型标识。
class Typeface : public NonCopyable
{
	friend class YFontCache;
	friend FT_Error simpleFaceRequester(FTC_FaceID,
		FT_Library, FT_Pointer, FT_Face*);

//	static const FT_Matrix MNormal, MOblique;
public:
	typedef std::string NameType;

	YFontCache& Cache;
	const FontFile& File;

private:
	FontFamily* pFontFamily;
//	FT_Face face;
//	bool bBold, bOblique, bUnderline;
	NameType style_name;

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
	DefGetter(FontFamily::NameType, FamilyName, pFontFamily
		? pFontFamily->GetFamilyName() : "")
	DefGetter(const NameType&, StyleName, style_name)
};


//! \brief 字体文件。
class FontFile : public NonCopyable
{
public:
	typedef std::string PathType; //!< 路径类型。

private:
	PathType path;
	mutable FT_Long nFace;

public:
	/*!
	\brief 使用路径字符串构造字体文件对象。
	*/
	FontFile(const PathType&);

	/*!
	\brief 比较：相等关系。
	*/
	PDefHOperator(bool, ==, const FontFile& rhs) const
		ImplRet(path == rhs.path);

	/*!
	\brief 比较：严格递增偏序关系。
	*/
	PDefHOperator(bool, <, const FontFile& rhs) const
		ImplRet(path < rhs.path);

	DefGetter(PathType, Path, path)
	DefGetter(s32, FaceN, nFace)

	/*!
	\brief 向指定本地字体库中重新读取字体文件，载入全部字体。
	*/
	void
	ReloadFaces(FT_Library&) const;
};


/*!
\brief 取默认字型引用。
\throw LoggedEvent 记录异常事件。
\note 仅抛出以上异常。
*/
const Typeface&
FetchDefaultTypeface() ythrow(LoggedEvent);

/*!
\brief 取默认字型家族引用。
\throw LoggedEvent 记录异常事件。
\note 仅抛出以上异常。
*/
const FontFamily&
FetchDefaultFontFamily() ythrow(LoggedEvent);


//! \brief 字体：字模，包含字型和大小。
class Font
{
public:
	typedef u8 SizeType;

	static const SizeType DefaultSize, MinimalSize, MaximalSize;

private:
	static Font* pDefFont; //!< 默认 Shell 字体。

	const FontFamily* pFontFamily;
	FontStyle Style;
	SizeType Size;

public:
	/*!
	\brief 构造指定字型家族、大小和样式的字体对象。
	*/
	explicit
	Font(const FontFamily& = FetchDefaultFontFamily(), SizeType = DefaultSize,
		FontStyle = FontStyle::Regular);

	DefPredicate(Bold, Style | FontStyle::Bold)
	DefPredicate(Italic, Style | FontStyle::Italic)
	DefPredicate(Underline, Style | FontStyle::Underline)
	DefPredicate(Strikeout, Style | FontStyle::Strikeout)

	/*!
	\brief 取默认实例。
	*/
	static const Font&
	GetDefault();
	DefGetter(const FontFamily&, FontFamily, *pFontFamily)
	DefGetter(FontStyle, Style, Style)
	DefGetter(SizeType, Size, Size)
	DefGetter(YFontCache&, Cache, GetFontFamily().Cache)
	DefGetterMember(const FontFamily::NameType&, FamilyName, GetFontFamily())
	DefGetter(Typeface::NameType, StyleName, Style.GetName())
	/*!
	\brief 取字体对应的字符高度。
	*/
	SizeType
	GetHeight() const;

/*	FontFamily&
	SetFontFamily(const FontFamily&);*/
	DefSetter(FontStyle, Style, Style)
	/*!
	\brief 设置字体大小。
	*/
	void
	SetSize(SizeType = DefaultSize);
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
};

inline
const Font&
Font::GetDefault()
{
	static Font f;

	return f;
}


//! \brief 字符位图。
class CharBitmap
{
public:
	typedef FTC_SBit NativeType;
	typedef FT_Byte* BufferType;
	typedef FT_Byte ScaleType;
	typedef FT_Char SignedScaleType;

private:
	NativeType bitmap;

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

private:
	FT_Library library; //!< 库实例。
	FTC_Manager manager; //!< 内存管理器实例。
	FTC_ScalerRec scaler;
	FTC_CMapCache cmapCache;
	FTC_SBitCache sbitCache;

public:
	typedef set<const FontFile*, ystdex::deref_comp<const FontFile> >
		FFiles; //!< 字体文件组类型。
	typedef set<Typeface*, ystdex::deref_comp<const Typeface> >
		FTypes; //!< 字型组类型。
	typedef set<FontFamily*, ystdex::deref_comp<FontFamily> >
		FFaces; //!< 字型家族组类型。
	typedef map<FontFamily::NameType, FontFamily*>
		FFacesIndex; //!< 字型家族组索引类型。

private:
	FFiles sFiles; //!< 字体文件组。
	FTypes sTypes; //!< 字型组。
	FFaces sFaces; //!< 字型家族组索引。
	FFacesIndex mFacesIndex; //!< 字型家族组索引。

	Typeface* pDefaultFace; //!< 默认字型指针。

public:
	/*!
	\brief 构造：读取指定路径的字体文件并分配指定大小的缓存空间。
	*/
	explicit
	YFontCache(CPATH, u32 = GLYPH_CACHE_SIZE);
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
	\brief 取指定名称的字型家族指针。
	*/
	const FontFamily*
	GetFontFamilyPtr(const FontFamily::NameType&) const;
	/*!
	\brief 取默认字型指针。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	const Typeface*
	GetDefaultTypefacePtr() const ythrow(LoggedEvent);
	DefGetter(const Typeface*, TypefacePtr,
		static_cast<Typeface*>(scaler.face_id)) //!< 取当前处理的字型指针。
//	Typeface*
//	GetTypefacePtr(u16) const; //!< 取字型组储存的指定索引的字型指针。
	/*!
	\brief 取指定名称的字型指针。
	*/
	const Typeface*
	GetTypefacePtr(const FontFamily::NameType&, const Typeface::NameType&)
		const;
	DefGetter(u8, FontSize, static_cast<u8>(scaler.width)) \
		//!< 取当前处理的字体大小。
	/*!
	\brief 取当前字型和大小渲染的指定字符的字形。
	*/
	CharBitmap
	GetGlyph(fchar_t);
	/*!
	\brief 取跨距。
	*/
	s8
	GetAdvance(fchar_t, FTC_SBit = nullptr);
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
	\note 参数为空指针时忽略。
	*/
	void
	operator+=(const FontFile*);
	/*!
	\brief 从字体文件组中移除指定字体文件对象。
	\note 参数为空指针时返回 false 。
	*/
	bool
	operator-=(const FontFile*);

	/*!
	\brief 向字型组添加字型对象。
	\note 参数为空指针时忽略。
	*/
	void
	operator+=(Typeface*);
	/*!
	\brief 从字型组中移除指定字型对象。
	\note 参数为空指针时忽略。
	*/
	bool
	operator-=(Typeface*);

	/*!
	\brief 向字型家族组添加字型对象。
	\note 参数为空指针时返回 false 。
	*/
	void
	operator+=(FontFamily*);
	/*!
	\brief 从字型家族组中移除指定字型对象。
	\note 参数为空指针时忽略。
	*/
	bool
	operator-=(FontFamily*);

public:
	/*!
	\brief 清除缓存。
	*/
	void
	ClearCache();

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
	\brief 从字体文件组中载入字体信息。
	*/
	void
	LoadTypefaces();

	/*!
	\brief 从指定字体文件中载入字体信息。
	\note 若指定字体文件不在字体文件组中则先按路径添加该文件。
	*/
	void
	LoadTypefaces(const FontFile&);

	/*!
	\brief 按路径添加字体文件并载入字体信息。
	*/
	bool
	LoadFontFile(CPATH);

	/*!
	\brief 初始化默认字型。
	*/
	void
	InitializeDefaultTypeface();

	/*
	!\brief 清除字形缓存。
	*/
	PDefH0(void, ResetGlyphCache)
		ImplRet(FTC_Manager_Reset(manager))
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

