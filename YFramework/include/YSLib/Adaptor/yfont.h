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
\version r7408;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:02:40 +0800;
\par 修改时间:
	2011-12-04 13:02 +0800;
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
#include <ystdex/utility.hpp>

//包含 FreeType2 。

#include <ft2build.h>

#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_BITMAP_H
#include FT_GLYPH_H
//#include FT_OUTLINE_H

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 默认字体大小（ pt ）。
\since build 198 。
*/
const u8 DEFAULT_FONT_SIZE(14);
/*!
\brief 字形缓冲区大小（字节）。
\since build 198 。
*/
const size_t GLYPH_CACHE_SIZE(128 << 10);

//前向声明。
class Font;
class FontFamily;
class Typeface;
class FontFile;
class FontCache;


/*!
\brief 字体样式。
\since build 197 。
*/
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
	yconstfn
	FontStyle(Styles = Regular);

	yconstfn DefCvt(const ynothrow, const Styles&, style)
	DefCvt(ynothrow, Styles&, style)

	/*!
	\brief 取样式名称。
	\post 断言：返回值非空。
	\note 无异常抛出。
	*/
	const char*
	GetName() const ynothrow;
};

yconstfn
FontStyle::FontStyle(Styles s)
	: style(s)
{}


/*!
\brief 字型家族 (Typeface Family) 标识。
\since build 145 。
*/
class FontFamily : public noncopyable
{
	friend class Typeface;
	friend class FontCache;

public:
	typedef std::string NameType;
	typedef set<Typeface*> FaceSet; //!< 字型组类型。
	typedef map<const std::string/*Typeface::NameType*/, Typeface*>
		FaceMap; //!< 字型组索引类型。

	FontCache& Cache;

private:
	NameType family_name;

protected:
	FaceSet sFaces; //!< 字型组类型。
	FaceMap mFaces; //!< 字型组索引类型。

public:
	/*!
	\brief 使用字体缓存引用和名称构造字型家族。
	*/
	FontFamily(FontCache&, const NameType&);

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

public:
	DefGetter(const ynothrow, const NameType&, FamilyName, family_name)
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
};


/*!
\brief 字型标识。
\since build 145 。
*/
class Typeface : public noncopyable
{
	friend class FontCache;
	friend FT_Error
	simpleFaceRequester(FTC_FaceID, FT_Library, FT_Pointer, FT_Face*);

//	static const FT_Matrix MNormal, MOblique;
public:
	typedef std::string NameType;

	FontCache& Cache;
	const FontFile& File;

private:
	FontFamily* pFontFamily;
//	FT_Face face;
//	bool bBold, bOblique, bUnderline;
	NameType style_name;

	FT_Long face_index;
	FT_Int cmap_index;
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
	Typeface(FontCache&, const FontFile&, u32 = 0
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

	DefGetter(const ynothrow, const FontFamily*, FontFamilyPtr, pFontFamily)
	DefGetter(const ynothrow, FontFamily::NameType, FamilyName, pFontFamily
		? pFontFamily->GetFamilyName() : "")
	DefGetter(const ynothrow, const NameType&, StyleName, style_name)
};


/*!
\brief 字体文件。
\since build 145 。
*/
class FontFile : public noncopyable
{
public:
	typedef std::string PathType; //!< 路径类型。

private:
	PathType path;
	mutable FT_Long face_num;

public:
	/*!
	\brief 使用路径字符串构造字体文件对象。
	*/
	FontFile(const PathType&);

	/*!
	\brief 比较：相等关系。
	*/
	PDefHOp(bool, ==, const FontFile& rhs) const
		ImplRet(path == rhs.path);

	/*!
	\brief 比较：严格递增偏序关系。
	*/
	PDefHOp(bool, <, const FontFile& rhs) const
		ImplRet(path < rhs.path);

	DefGetter(const ynothrow, PathType, Path, path)
	DefGetter(const ynothrow, s32, FaceN, face_num)

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
\since build 194 。
*/
const Typeface&
FetchDefaultTypeface() ythrow(LoggedEvent);

/*!
\brief 取默认字型家族引用。
\throw LoggedEvent 记录异常事件。
\note 仅抛出以上异常。
\since build 194 。
*/
const FontFamily&
FetchDefaultFontFamily() ythrow(LoggedEvent);


/*!
\brief 字体：字模，包含字型和大小。
\since build 145 。
*/
class Font
{
public:
	typedef u8 SizeType;

	static yconstexpr SizeType DefaultSize = 12,
		MinimalSize = 4, MaximalSize = 96;

private:
	static Font* pDefFont; //!< 默认 Shell 字体。

	const FontFamily* pFontFamily;
	FontStyle Style;
	SizeType Size;

public:
	/*!
	\brief 构造指定字型家族、大小和样式的字体对象。
	*/
	explicit yconstfn
	Font(const FontFamily& = FetchDefaultFontFamily(), SizeType = DefaultSize,
		FontStyle = FontStyle::Regular);

	yconstfn DefPred(const ynothrow, Bold, Style | FontStyle::Bold)
	yconstfn DefPred(const ynothrow, Italic, Style | FontStyle::Italic)
	yconstfn DefPred(const ynothrow, Underline, Style | FontStyle::Underline)
	yconstfn DefPred(const ynothrow, Strikeout, Style | FontStyle::Strikeout)

	/*!
	\brief 取默认实例。
	*/
	static const Font&
	GetDefault();
	DefGetter(const ynothrow, const FontFamily&, FontFamily, *pFontFamily)
	yconstfn DefGetter(const ynothrow, FontStyle, Style, Style)
	yconstfn DefGetter(const ynothrow, SizeType, Size, Size)
	DefGetter(const ynothrow, FontCache&, Cache, GetFontFamily().Cache)
	DefGetterMem(const ynothrow, const FontFamily::NameType&, FamilyName,
		GetFontFamily())
	DefGetter(const ynothrow, Typeface::NameType, StyleName, Style.GetName())
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

yconstfn
Font::Font(const FontFamily& family, const SizeType size, FontStyle style)
	: pFontFamily(&family), Style(style), Size(size)
{}

inline
const Font&
Font::GetDefault()
{
	static Font f;

	return f;
}


/*!
\brief 字符位图。
\since build 147 。
*/
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
	yconstfn
	CharBitmap(const NativeType&);

	yconstfn DefCvt(const ynothrow, NativeType, bitmap)

	yconstfn DefGetter(const ynothrow, BufferType, Buffer, bitmap->buffer)
	yconstfn DefGetter(const ynothrow, ScaleType, Width, bitmap->width)
	yconstfn DefGetter(const ynothrow, ScaleType, Height, bitmap->height)
	yconstfn DefGetter(const ynothrow, SignedScaleType, Left, bitmap->left)
	yconstfn DefGetter(const ynothrow, SignedScaleType, Top, bitmap->top)
	yconstfn DefGetter(const ynothrow, SignedScaleType, XAdvance,
		bitmap->xadvance)
	yconstfn DefGetter(const ynothrow, SignedScaleType, YAdvance,
		bitmap->yadvance)
};

yconstfn
CharBitmap::CharBitmap(const NativeType& b)
	: bitmap(b)
{}


/*!
\brief 字体缓存。
\warning 非虚析构。
\since build 209 。
*/
class FontCache : public noncopyable,
	public OwnershipTag<Typeface>, public OwnershipTag<FontFile>
{
	friend class Typeface;

public:
	typedef set<const FontFile*, ystdex::deref_comp<const FontFile>>
		FileSet; //!< 字体文件组类型。
	typedef set<Typeface*, ystdex::deref_comp<const Typeface>>
		FaceSet; //!< 字型组类型。
	typedef set<FontFamily*, ystdex::deref_comp<FontFamily>>
		FamilySet; //!< 字型家族组类型。
	typedef map<FontFamily::NameType, FontFamily*>
		FamilyMap; //!< 字型家族组索引类型。

private:
	FT_Library library; //!< 库实例。
	FTC_Manager manager; //!< 内存管理器实例。
	FTC_ScalerRec scaler;
	FTC_CMapCache cmapCache;
	FTC_SBitCache sbitCache;

protected:
	FileSet sFiles; //!< 字体文件组。
	FaceSet sFaces; //!< 字型组。
	FamilySet sFamilies; //!< 字型家族组索引。
	FamilyMap mFamilies; //!< 字型家族组索引。

	Typeface* pDefaultFace; //!< 默认字型指针。

public:
	/*!
	\brief 构造：读取指定路径的字体文件并分配指定大小的缓存空间。
	*/
	explicit
	FontCache(const_path_t, u32 = GLYPH_CACHE_SIZE);
	/*!
	\brief 析构：释放空间。
	*/
	~FontCache();

private:
	/*!
	\brief 取当前处理的内部字型结构体指针。
	*/
	FT_Face
	GetInternalFaceInfo() const;

public:
	DefGetter(const ynothrow, const FileSet&, Files, sFiles) //!< 取字体文件组。
	DefGetter(const ynothrow, const FaceSet&, Types, sFaces) //!< 取字型组。
	DefGetter(const ynothrow, const FamilySet&, Faces, sFamilies) \
		//!< 取字型家族组。
	DefGetter(const ynothrow, const FamilyMap&, FacesIndex, mFamilies) \
		//!< 取字型家族组索引。
	DefGetter(const ynothrow, FileSet::size_type, FilesN, sFiles.size()) \
		//!< 取字体文件组储存的文件数。
	DefGetter(const ynothrow, FaceSet::size_type, TypesN, sFaces.size()) \
		//!< 取字型组储存的字型数。
	DefGetter(const ynothrow, FamilySet::size_type, FacesN, sFamilies.size()) \
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
	DefGetter(const ynothrow, const Typeface*, TypefacePtr,
		static_cast<Typeface*>(scaler.face_id)) //!< 取当前处理的字型指针。
//	Typeface*
//	GetTypefacePtr(u16) const; //!< 取字型组储存的指定索引的字型指针。
	/*!
	\brief 取指定名称的字型指针。
	*/
	const Typeface*
	GetTypefacePtr(const FontFamily::NameType&, const Typeface::NameType&)
		const;
	DefGetter(const ynothrow, u8, FontSize, u8(scaler.width)) \
		//!< 取当前处理的字体大小。
	/*!
	\brief 取当前字型和大小渲染的指定字符的字形。
	*/
	CharBitmap
	GetGlyph(ucs4_t);
	/*!
	\brief 取跨距。
	*/
	s8
	GetAdvance(ucs4_t, FTC_SBit = nullptr);
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
	\note 若指定字体文件不在字体文件组中则先按路径添加此文件。
	*/
	void
	LoadTypefaces(const FontFile&);

	/*!
	\brief 按路径添加字体文件并载入字体信息。
	*/
	bool
	LoadFontFile(const_path_t);

	/*!
	\brief 初始化默认字型。
	*/
	void
	InitializeDefaultTypeface();

	/*
	!\brief 清除字形缓存。
	*/
	PDefH(void, ResetGlyphCache)
		ImplRet(FTC_Manager_Reset(manager))
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

