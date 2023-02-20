/*
	© 2009-2021, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Font.h
\ingroup Adaptor
\brief 平台无关的字体库。
\version r3720
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2009-11-12 22:02:40 +0800
\par 修改时间:
	2023-02-20 06:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Font
*/


#ifndef YSL_INC_Adaptor_Font_h_
#define YSL_INC_Adaptor_Font_h_ 1

#include "../Core/YModules.h"
#include YFM_YSLib_Core_YFunc // for linked_map, ystdex::is_fast_hash,
//	ystdex::false_, list, value_map, default_allocator, byte;
#include YFM_YSLib_Core_YObject
#include <string>
#include YFM_YSLib_Core_YException // for LoggedEvent;
#include YFM_YSLib_Adaptor_YTextBase
#include <ystdex/hash.hpp> // for ystdex::hash_combine_seq;
#include <ystdex/cache.hpp> // for ystdex::recent_used_list,
//	ystdex::used_list_cache;
//#include <ft2build.h>
//#include FT_FREETYPE_H

//! \since build 612
//!@{
struct FT_SizeRec_;
using FT_Size = ::FT_SizeRec_*;
using FT_SizeRec = ::FT_SizeRec_;
struct FT_FaceRec_;
using FT_FaceRec = ::FT_FaceRec_;
struct FT_GlyphSlotRec_;
using FT_GlyphSlot = ::FT_GlyphSlotRec_*;
struct FT_LibraryRec_;
using FT_Library = ::FT_LibraryRec_*;
struct FT_Size_Metrics_;
using FT_Size_Metrics = FT_Size_Metrics_;
//!@}

namespace YSLib
{

namespace Drawing
{

class Font;
class FontCache;
class FontFamily;
class Typeface;


/*!
\brief 字体大小。
\since build 277
*/
using FontSize = std::uint8_t;
/*!
\brief 字体文件路径。
\since build 277
*/
using FontPath = string;
/*!
\brief 字型家族名称。
\since build 277
*/
using FamilyName = string;
/*!
\brief 字型样式名称。
\since build 277
*/
using StyleName = string;


/*!
\brief 字体样式。
\since build 197
*/
enum class FontStyle : std::uint8_t
{
	//! \brief 常规字体。
	Regular = 0,
	//! \brief 粗体。
	Bold = 1,
	//! \brief 斜体。
	Italic = 2,
	//! \brief 下划线。
	Underline = 4,
	//! \brief 删除线。
	Strikeout = 8
};

/*!
\relates FontStyle
\since build 421
*/
DefBitmaskEnum(FontStyle)


/*!
\brief 取样式名称。
\return 非空结果。
\since build 277
*/
YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE yconstfn
	PDefH(const char*, FetchName, FontStyle style) ynothrow
	ImplRet(style == FontStyle::Bold ? "Bold" : (style == FontStyle::Italic
		? "Italic" : (style == FontStyle::Underline ? "Underline" : (style
		== FontStyle::Strikeout ? "Strikeout" : "Regular"))))


/*!
\brief 字体异常。
\since build 241
*/
class YF_API FontException : public LoggedEvent
{
public:
	//! \note 和 \c ::FT_Error 一致。
	using FontError = int;

private:
	FontError err;

public:
	//! \since build 833
	FontException(FontError e, const string_view msg = "")
		: LoggedEvent(msg),
		err(e)
	{}
	//! \since build 586
	DefDeCopyCtor(FontException)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~FontException() override;

	YB_ATTR_nodiscard DefGetter(const ynothrow, FontError, ErrorCode, err)
};


/*!
\brief 本机字体大小。
\since build 419
*/
class YF_API NativeFontSize final : private noncopyable
{
private:
	::FT_Size size;

public:
	//! \since build 420
	NativeFontSize(::FT_FaceRec&, FontSize);
	NativeFontSize(NativeFontSize&&) ynothrow;
	//! \since build 461
	~NativeFontSize();

	//! \since build 968
	PDefHOp(NativeFontSize&, =, NativeFontSize&& ns) ynothrow
		ImplRet(std::swap(size, ns.size), *this)

	YB_ATTR_nodiscard YB_PURE ::FT_SizeRec&
	GetSizeRec() const;

	/*!
	\brief 激活当前大小。
	\note 替代 \c ::FT_Activate_Size 。
	\since build 420
	*/
	void
	Activate() const;
};


/*!
\brief 字型家族。
\since build 145
*/
class YF_API FontFamily final : private noncopyable
{
public:
	//! \brief 字型组索引类型。
	using FaceMap = linked_map<StyleName, lref<Typeface>>;

private:
	FamilyName family_name;

protected:
	//! \brief 字型组索引类型。
	FaceMap mFaces;

public:
	/*!
	\brief 构造：使用指定名称。
	\since build 661
	*/
	FontFamily(const FamilyName&);

	/*!
	\brief 向字型组和字型组索引添加字型对象。
	\since build 277
	*/
	void
	operator+=(Typeface&);
	/*!
	\brief 从字型组和字型组索引中移除指定字型对象。
	\since build 572
	*/
	bool
	operator-=(Typeface&) ynothrow;

	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const FamilyName&, FamilyName, family_name)
	//! \since build 671
	//!@{
	/*!
	\brief 取指定样式的字型指针。
	\note 若非 Regular 样式失败则尝试取 Regular 样式的字型指针。
	*/
	YB_ATTR_nodiscard YB_PURE observer_ptr<Typeface>
	GetTypefacePtr(FontStyle) const;
	//! \brief 取指定样式名称的字型指针。
	YB_ATTR_nodiscard YB_PURE observer_ptr<Typeface>
	GetTypefacePtr(const StyleName&) const;
	//!@}
	//! \since build 419
	YB_ATTR_nodiscard YB_PURE Typeface&
	GetTypefaceRef(FontStyle) const;
	//! \since build 419
	YB_ATTR_nodiscard YB_PURE Typeface&
	GetTypefaceRef(const StyleName&) const;
};


// XXX: To allow specialization, these types are in the namespace scope.
/*!
\since build 968
\warning 非虚析构。
*/
//!@{
struct yimpl(BitmapKey)
{
	//! \since build 562
	unsigned Flags;
	//! \since build 562
	unsigned GlyphIndex;
	FontSize Size;
	//! \since build 421
	FontStyle Style;

	//! \since build 673
	friend PDefHOp(bool, ==, const BitmapKey& x, const BitmapKey& y)
		ynothrow
		ImplRet(x.Flags == y.Flags && x.GlyphIndex == y.GlyphIndex
			&& x.Size == y.Size && x.Style == y.Style)
};


//! \ingroup hashers
struct yimpl(BitmapKeyHash)
{
	YB_ATTR_nodiscard YB_PURE PDefHOp(size_t, (), const BitmapKey& k)
		const ynothrow
		ImplRet(ystdex::hash_combine_seq(size_t(k.Style), k.Size,
			k.GlyphIndex, k.Flags))
};
//!@}

} // namespace Drawing;

} // namespace YSLib;

namespace ystdex
{

/*!
\relates YSLib::Drawing::BitmapKeyHash
\since build 968
*/
template<>
struct is_fast_hash<YSLib::Drawing::BitmapKeyHash> : false_
{};

} // namespace ystdex;

namespace YSLib
{

namespace Drawing
{

/*!
\brief 字型。
\since build 145
*/
class YF_API Typeface final : private noncopyable, private nonmovable
{
	//! \since build 419
	friend class Font;
	//! \since build 612
	friend class CharBitmap;

private:
	//! \since build 419
	//!@{
	class SmallBitmapData
	{
		friend class CharBitmap;

	private:
		/*!
		\sa ::FTC_SBitRec_
		\since build 612
		*/
		//!@{
		//! \since build 849
		std::uint8_t width = 255, height = 0;
		signed char left = 0, top = 0;
		//! \since build 849
		std::uint8_t format = 0, max_grays = 0;
		short pitch = 0;
		signed char xadvance = 0, yadvance = 0;
		//! \since build 849
		std::uint8_t* buffer = {};
		//!@}

	public:
		//! \since build 421
		SmallBitmapData(::FT_GlyphSlot, FontStyle);
		SmallBitmapData(SmallBitmapData&&) ynothrow;
		~SmallBitmapData();
	};
	//!@}

	/*!
	\ingroup traits
	\since build 968
	*/
	struct UsedListCacheTraits
	{
		using used_list_type
			= ystdex::recent_used_list<BitmapKey, SmallBitmapData,
			list<std::pair<const BitmapKey, SmallBitmapData>>>;
		using map_type = linked_map<BitmapKey, SmallBitmapData, BitmapKeyHash,
			std::equal_to<BitmapKey>, typename used_list_type::allocator_type>;
		using used_cache_type = linked_map<BitmapKey, typename used_list_type
			::iterator, BitmapKeyHash, typename map_type::key_equal,
			ystdex::rebind_alloc_t<typename used_list_type::allocator_type,
			std::pair<const BitmapKey, typename used_list_type::iterator>>>;
	};

	//! \since build 562
	long face_index;
	//! \since build 562
	int cmap_index;
	StyleName style_name;
	//! \since build 554
	pair<lref<FontFamily>, lref<::FT_FaceRec_>> ref;
	//! \since build 968
	//!@{
	mutable ystdex::used_list_cache<BitmapKey, SmallBitmapData, BitmapKeyHash,
		UsedListCacheTraits> bitmap_cache;
	mutable value_map<char32_t, unsigned> glyph_index_cache;
	mutable value_map<FontSize, NativeFontSize> size_cache;
	//!@}

public:
	/*!
	\brief 使用字体缓存引用在指定字体文件路径读取指定索引的字型并构造对象。
	\throw LoggedEvent ：读取的索引不在字型接受的范围内。
	*/
	Typeface(FontCache&, const FontPath&, std::uint32_t = 0);
	//! since build 461
	~Typeface();

	YB_ATTR_nodiscard
		DefGetterMem(const ynothrow, FamilyName, FamilyName, GetFontFamily())
	/*!
	\brief 取字型家族。
	\since build 278
	*/
	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const FontFamily&, FontFamily, ref.first)
	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const StyleName&, StyleName, style_name)
	/*!
	\brief 取字符映射索引号。
	\since build 562
	*/
	YB_ATTR_nodiscard DefGetter(const ynothrow, int, CMapIndex, cmap_index)

private:
	//! \since build 419
	//!@{
	SmallBitmapData&
	LookupBitmap(const BitmapKey&) const;

	//! \since build 641
	YB_ATTR_nodiscard unsigned
	LookupGlyphIndex(char32_t) const;

	//! since build 420
	YB_ATTR_nodiscard NativeFontSize&
	LookupSize(FontSize) const;

public:
	PDefH(void, ClearBitmapCache, )
		ImplExpr(bitmap_cache.clear())

	PDefH(void, ClearGlyphIndexCache, )
		ImplExpr(glyph_index_cache.clear())
	//!@}

	//! since build 420
	PDefH(void, ClearSizeCache, )
		ImplExpr(size_cache.clear())
};


/*!
\brief 取默认字型引用。
\throw LoggedEvent 严重：异常事件。
\since build 425
*/
YB_ATTR_nodiscard YF_API const Typeface&
FetchDefaultTypeface();


/*!
\brief 字符位图。
\warning 若为空时调用成员函数时行为未定义。
\since build 147
*/
class YF_API CharBitmap final
{
public:
	//! \note 和 \c ::FT_Byte* 一致。
	using BufferType = std::uint8_t*;
	/*!
	\note 值兼容于 \c ::FT_Pixel_Mode 。
	\since build 415
	*/
	enum FormatType
	{
		None = 0,
		Mono,
		Gray,
		Gray2,
		Gray4,
		LCD,
		LCD_V
	};
	//! \brief 本机类型对象：指针类型。
	using NativeType = Typeface::SmallBitmapData*;
	/*!
	\note 和 \c ::FT_Short 一致。
	\since build 415
	*/
	using PitchType = short;
	//! \note 和 \c ::FT_Byte 一致。
	using ScaleType = std::uint8_t;
	//! \note 和 \c ::FT_Char 一致。
	using SignedScaleType = signed char;

private:
	NativeType bitmap = {};

public:
	//! \since build 612
	//!@{
	//! \brief 构造：空位图。
	DefDeCtor(CharBitmap)
	//! \brief 构造：使用本机类型对象。
	yconstfn
	CharBitmap(NativeType b)
		: bitmap(b)
	{}
	//!@}

	YB_ATTR_nodiscard yconstfn DefCvt(const ynothrow, NativeType, bitmap)

	/*!
	\pre 间接断言：位图本机类型对象非空。
	\since build 612
	*/
	//!@{
	YB_ATTR_nodiscard yconstfn DefGetter(const ynothrowv, BufferType, Buffer,
		Deref(bitmap).buffer)
	YB_ATTR_nodiscard yconstfn DefGetter(const ynothrowv, FormatType, Format,
		FormatType(Deref(bitmap).format))
	YB_ATTR_nodiscard yconstfn DefGetter(const ynothrowv, ScaleType, GrayLevel,
		Deref(bitmap).max_grays)
	YB_ATTR_nodiscard yconstfn
		DefGetter(const ynothrowv, ScaleType, Height, Deref(bitmap).height)
	YB_ATTR_nodiscard yconstfn DefGetter(const ynothrowv, SignedScaleType, Left,
		Deref(bitmap).left)
	YB_ATTR_nodiscard yconstfn
		DefGetter(const ynothrowv, PitchType, Pitch, Deref(bitmap).pitch)
	YB_ATTR_nodiscard yconstfn
		DefGetter(const ynothrowv, SignedScaleType, Top, Deref(bitmap).top)
	YB_ATTR_nodiscard yconstfn
		DefGetter(const ynothrowv, ScaleType, Width, Deref(bitmap).width)
	YB_ATTR_nodiscard yconstfn DefGetter(const ynothrowv, SignedScaleType,
		XAdvance, Deref(bitmap).xadvance)
	YB_ATTR_nodiscard yconstfn DefGetter(const ynothrowv, SignedScaleType,
		YAdvance, Deref(bitmap).yadvance)
	//!@}
};


/*!
\brief 字体缓存。
\since build 209
*/
class YF_API FontCache final : private noncopyable,
	private OwnershipTag<Typeface>, private OwnershipTag<FontFamily>
{
	friend class Typeface;
	/*!
	\brief 友元类：访问 scaler 等对象。
	\since build 280
	*/
	friend class Font;

public:
	/*!
	\brief 字型映射类型。
	\invariant 被映射的值非空。
	\since build 661
	*/
	using FaceMap = linked_map<FontPath, unique_ptr<Typeface>>;
	/*!
	\brief 字型家族组索引类型。
	\invariant 被映射的值非空。
	*/
	using FamilyMap = linked_map<FamilyName, unique_ptr<FontFamily>>;

	/*!
	\brief 字形缓冲区大小。
	\note 单位为字节。
	\since build 277
	*/
	static yconstexpr const size_t DefaultGlyphCacheSize = yimpl(128U << 10);

private:
	//! \brief 库实例。
	::FT_Library library;

protected:
	/*!
	\brief 字型映射。
	\since build 661
	*/
	FaceMap mFaces;
	//! \brief 字型家族组索引。
	FamilyMap mFamilies;
	/*!
	\brief 默认字型指针。
	\since build 671
	*/
	observer_ptr<Typeface> pDefaultFace;

public:
	/*!
	\brief 构造：分配指定大小的字形缓存空间。
	\note 当前暂时忽略第一参数。
	\since build 968
	*/
	explicit
	FontCache(size_t = DefaultGlyphCacheSize,
		default_allocator<yimpl(byte)> = {});
	/*!
	\brief 析构：释放空间。
	\since build 461
	*/
	~FontCache();

public:
	/*!
	\brief 取字型映射。
	\since build 661
	*/
	YB_ATTR_nodiscard DefGetter(const ynothrow, const FaceMap&, Faces, mFaces)
	//! \brief 取字型家族组索引。
	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const FamilyMap&, FamilyIndices, mFamilies)
	//! \since build 671
	//!@{
	//! \brief 取指定名称的字型家族指针。
	YB_ATTR_nodiscard YB_PURE observer_ptr<const FontFamily>
	GetFontFamilyPtr(const FamilyName&) const;
	/*!
	\brief 取默认字型指针。
	\exception LoggedEvent 异常中立：初始化时发生的异常。
	*/
	YB_ATTR_nodiscard YB_PURE observer_ptr<const Typeface>
	GetDefaultTypefacePtr() const;
	//! \brief 取指定名称的字型指针。
	YB_ATTR_nodiscard YB_PURE observer_ptr<const Typeface>
	GetTypefacePtr(const FamilyName&, const StyleName&) const;
	//!@}

private:
	/*!
	\brief 向字型组添加字体路径和字型对象的映射。
	\since build 661
	*/
	void
	Add(const FontPath&, unique_ptr<Typeface>);

public:
	/*!
	\brief 从字体文件组中载入字型信息。
	\return 成功载入的字型数。
	\since build 316
	*/
	size_t
	LoadTypefaces(const FontPath&);

	/*!
	\brief 在索引中查找字型家族。
	\note 若索引中不存在，先初始化。
	\since build 660
	*/
	YB_ATTR_nodiscard YB_PURE FontFamily&
	LookupFamily(const FamilyName&);

	/*!
	\brief 初始化默认字型。
	*/
	void
	InitializeDefaultTypeface();
};


/*!
\brief 字体：字模，包含字型、样式和大小。
\since build 145
*/
class YF_API Font final
{
public:
	static yconstexpr const FontSize DefaultSize = 12,
		MinimalSize = 4, MaximalSize = 96;

private:
	//! \since build 554
	lref<Typeface> typeface;
	//! \since build 419
	FontSize font_size;
	/*!
	\brief 字体样式。
	\since build 297
	*/
	FontStyle style;

public:
	/*!
	\brief 无参数构造：使用默认字型家族、大小和样式的字体对象。
	\since build 351
	*/
	Font()
		: Font(FetchDefaultTypeface().GetFontFamily())
	{}
	/*!
	\brief 构造：使用指定字型家族、大小和样式的字体对象。
	\since build 351
	*/
	explicit
	Font(const FontFamily&, FontSize = DefaultSize,
		FontStyle = FontStyle::Regular);

	YB_ATTR_nodiscard
		DefPred(const ynothrow, Bold, bool(style & FontStyle::Bold))
	YB_ATTR_nodiscard
		DefPred(const ynothrow, Italic, bool(style & FontStyle::Italic))
	YB_ATTR_nodiscard
		DefPred(const ynothrow, Underline, bool(style & FontStyle::Underline))
	YB_ATTR_nodiscard
		DefPred(const ynothrow, Strikeout, bool(style & FontStyle::Strikeout))

	/*!
	\brief 取跨距。
	\since build 641
	*/
	YB_ATTR_nodiscard YB_PURE std::int8_t
	GetAdvance(char32_t, CharBitmap = {}) const;
	/*!
	\brief 取升部。
	\since build 280
	*/
	YB_ATTR_nodiscard YB_PURE std::int8_t
	GetAscender() const;
	/*!
	\brief 取降部。
	\since build 280
	*/
	YB_ATTR_nodiscard YB_PURE std::int8_t
	GetDescender() const;
	YB_ATTR_nodiscard DefGetterMem(const ynothrow, const FamilyName&,
		FamilyName, GetFontFamily())
	YB_ATTR_nodiscard DefGetterMem(const ynothrow, const FontFamily&,
		FontFamily, GetTypeface())
	YB_ATTR_nodiscard DefGetter(const ynothrow, FontSize, Size, font_size)
	//! \since build 414
	YB_ATTR_nodiscard DefGetter(const ynothrow, FontStyle, Style, style)
	/*!
	\brief 取当前字型和大小渲染的指定字符的字形。
	\param c 指定需要被渲染的字符。
	\param flags FreeType 渲染标识。
	\note 默认参数为 FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL 。
	\warning 返回的位图在下一次调用 FontCache 方法或底层 FreeType 缓存时不保证有效。
	\warning flags 可能被移除，应仅用于内部实现。
	\since build 641
	*/
	YB_ATTR_nodiscard YB_PURE CharBitmap
	GetGlyph(char32_t c, yimpl(unsigned flags = 4U)) const;
	/*!
	\brief 取字体对应的字符高度。
	\since build 280
	*/
	YB_ATTR_nodiscard YB_PURE FontSize
	GetHeight() const ynothrow;
	//! \since build 628
	YB_ATTR_nodiscard DefGetter(const, StyleName, StyleName, FetchName(style))

private:
	/*!
	\brief 取内部信息。
	\since build 419
	*/
	YB_ATTR_nodiscard YB_PURE ::FT_Size_Metrics
	GetInternalInfo() const;

public:
	/*!
	\brief 取字型引用。
	\since build 280
	*/
	YB_ATTR_nodiscard DefGetter(const ynothrow, Typeface&, Typeface, typeface)

	/*!
	\brief 设置字体大小。
	*/
	void
	SetSize(FontSize = DefaultSize);
	/*!
	\brief 设置样式。
	\note 仅当存在字型时设置样式。
	\since build 280
	*/
	bool
	SetStyle(FontStyle);
};

} // namespace Drawing;

} // namespace YSLib;

#endif

