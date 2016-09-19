/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Video.h
\ingroup YCLib
\brief 平台相关的视频输出接口。
\version r1442
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2011-05-26 19:41:08 +0800
\par 修改时间:
	2015-09-19 18:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Video
*/


#ifndef YCL_INC_Video_h_
#define YCL_INC_Video_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon // for std::is_signed, std::is_unsigned;
#include <ystdex/bitseg.hpp> // for ystdex::integer_width, ystdex::bitseg_trait;

namespace platform
{

#if YCL_Win32
using SPos = long;
using SDst = unsigned long;
#else
using SPos = std::int16_t; //!< 屏幕坐标度量。
using SDst = std::uint16_t; //!< 屏幕坐标距离。
#endif

//! \since build 517
//@{
static_assert(std::is_signed<SPos>(),
	"Only signed integer is supported for screen position measurement.");
static_assert(std::is_unsigned<SDst>(),
	"Only unsigned integer is supported for screen destination measurement.");
static_assert(ystdex::integer_width<SDst>() >= ystdex::integer_width<SPos>(),
	"Max value of screen destination shall be grater than max value of screen"
	" position.");
//@}


//! \since build 728
//@{
//! \brief 索引空间。
inline namespace IndexSpace
{

//! \brief AXYZ 分量索引。
namespace AXYZIndex
{

//! \note 值表示值特征的 AXYZ 索引参数顺序。
enum AXYZ : size_t
{
	A = 0,
	X = 1,
	Y = 2,
	Z = 3
};

} // namespace AXYZIndex;

//! \brief AXYZ 分量索引映射。
namespace AXYZMapping
{

//! \note 值表示 ABGR 分量到 AXZY 分量的映射，和存储无关。
enum ABGR : size_t
{
	A = AXYZIndex::A,
	B = AXYZIndex::X,
	G = AXYZIndex::Y,
	R = AXYZIndex::Z
};

} // namespace AXYZMapping;

//! \brief AZYX 分量索引映射。
namespace AZYXMapping
{

//! \note 值表示 ABGR 分量到 AXZY 分量的映射，和存储无关。
enum ABGR : size_t
{
	A = AXYZIndex::A,
	B = AXYZIndex::Z,
	G = AXYZIndex::Y,
	R = AXYZIndex::X
};

} // namespace AZYXMapping;

} // inline namespace IndexSpace;


/*!
\ingroup type_traits_operations
\note X 、 Y 和 Z 表示色彩分量， A 表示 Alpha 通道分量。
\since build 728
*/
//@{
//! \note 索引参数表示在 AXYZ 分量在位段特征中表示的索引。
template<class _tBitSegTrait, size_t _vIdxA, size_t _vIdxX, size_t _vIdxY,
	size_t _vIdxZ>
struct AXYZValueTrait
	: ystdex::mapped_bitseg_trait<_tBitSegTrait, _vIdxA, _vIdxX, _vIdxY, _vIdxZ>
{
	using Base = ystdex::mapped_bitseg_trait<_tBitSegTrait, _vIdxA, _vIdxX,
		_vIdxY, _vIdxZ>;
	using IntegerType = typename Base::integer;
	using ArrayType = typename Base::array;

	static_assert(Base::bits_n <= 64, "Width larger than 64 unimplemented");

	static yconstexpr const size_t ABitsN
		= Base::template component_width<AXYZIndex::A>::value;
	static yconstexpr const size_t XYZBitsN
		= Base::template component_width<AXYZIndex::X>::value
		+ Base::template component_width<AXYZIndex::Y>::value
		+ Base::template component_width<AXYZIndex::Z>::value;
};


//! \brief AXYZ 特征。
template<size_t _vA, size_t _vX, size_t _vY, size_t _vZ>
using AXYZTrait
	= AXYZValueTrait<ystdex::bitseg_trait<_vA, _vX, _vY, _vZ>, 0, 1, 2, 3>;

//! \brief XYZA 特征。
template<size_t _vX, size_t _vY, size_t _vZ, size_t _vA>
using XYZATrait
	= AXYZValueTrait<ystdex::bitseg_trait<_vX, _vY, _vZ, _vA>, 3, 0, 1, 2>;
//@}
//@}


/*!
\brief BGRA 四元组。
\note 作为 POD 类型，可以用于储存像素。
\warning 用户应检查存储表示的实际大小是否和本类型一致。
\bug 当前仅支持小端整数字节序。
\bug 当前忽略对齐和特定类型的一致性。
\since build 441
*/
template<size_t _vB, size_t _vG, size_t _vR, size_t _vA>
union YB_ATTR(packed) YB_ATTR(
	aligned(yalignof(typename XYZATrait<_vB, _vG, _vR, _vA>::IntegerType))) BGRA
{
	using Trait = XYZATrait<_vB, _vG, _vR, _vA>;
	//! \since build 555
	using ArrayType = typename Trait::ArrayType;
	//! \since build 555
	using IntegerType = typename Trait::IntegerType;
	//! \since build 728
	//@{
	using CMap = AXYZMapping::ABGR;
	using AType = typename Trait::template component_t<CMap::A>;
	using BType = typename Trait::template component_t<CMap::B>;
	using GType = typename Trait::template component_t<CMap::G>;
	using RType = typename Trait::template component_t<CMap::R>;
	//@}

	ArrayType Bytes;
	IntegerType Integer;

//#if !LITTLE_ENDIAN
//#	error Unsupported integer endianness found.
//#endif

	DefDeCtor(BGRA)
	//! \since build 728
	//@{
	yconstfn
	BGRA(IntegerType i)
		: Integer(i)
	{}
	yconstfn
	BGRA(BType b, GType g, RType r, AType a)
		: Integer(Trait::pack(a, b, g, r))
	{}

	yconstfn DefCvt(const ynothrow, IntegerType, Integer)

	yconstfn DefGetter(const ynothrow, AType, A,
		Trait::template extract<CMap::A>(Integer))
	yconstfn DefGetter(const ynothrow, BType, B,
		Trait::template extract<CMap::B>(Integer))
	yconstfn DefGetter(const ynothrow, GType, G,
		Trait::template extract<CMap::G>(Integer))
	yconstfn DefGetter(const ynothrow, RType, R,
		Trait::template extract<CMap::R>(Integer))
	//@}
};


/*!
\brief RGBA 四元组。
\note 作为 POD 类型，可以用于储存像素。
\warning 用户应检查存储表示的实际大小是否和本类型一致。
\bug 当前仅支持小端整数字节序。
\bug 当前忽略对齐和特定类型的一致性。
\since build 441
*/
template<size_t _vR, size_t _vG, size_t _vB, size_t _vA>
union YB_ATTR(packed) YB_ATTR(
	aligned(yalignof(typename XYZATrait<_vR, _vG, _vB, _vA>::IntegerType))) RGBA
{
	using Trait = XYZATrait<_vR, _vG, _vB, _vA>;
	//! \since build 555
	using ArrayType = typename Trait::ArrayType;
	//! \since build 555
	using IntegerType = typename Trait::IntegerType;
	//! \since build 728
	//@{
	using CMap = AZYXMapping::ABGR;
	using AType = typename Trait::template component_t<CMap::A>;
	using BType = typename Trait::template component_t<CMap::B>;
	using GType = typename Trait::template component_t<CMap::G>;
	using RType = typename Trait::template component_t<CMap::R>;
	//@}

	ArrayType Bytes;
	IntegerType Integer;

//#if !LITTLE_ENDIAN
//#	error Unsupported integer endianness found.
//#endif

	DefDeCtor(RGBA)
	//! \since build 728
	//@{
	yconstfn
	RGBA(IntegerType i)
		: Integer(i)
	{}
	yconstfn
	RGBA(RType r, GType g, BType b, AType a)
		: Integer(Trait::pack(a, r, g, b))
	{}

	yconstfn DefCvt(const ynothrow, IntegerType, Integer)

	yconstfn DefGetter(const ynothrow, AType, A,
		Trait::template extract<CMap::A>(Integer))
	yconstfn DefGetter(const ynothrow, BType, B,
		Trait::template extract<CMap::B>(Integer))
	yconstfn DefGetter(const ynothrow, GType, G,
		Trait::template extract<CMap::G>(Integer))
	yconstfn DefGetter(const ynothrow, RType, R,
		Trait::template extract<CMap::R>(Integer))
	//@}
};


/*!
\brief ARGB 四元组。
\note 作为 POD 类型，可以用于储存像素。
\warning 用户应检查存储表示的实际大小是否和本类型一致。
\bug 当前仅支持小端整数字节序。
\bug 当前忽略对齐和特定类型的一致性。
\since build 507
*/
template<size_t _vA, size_t _vR, size_t _vG, size_t _vB>
union YB_ATTR(packed) YB_ATTR(
	aligned(yalignof(typename AXYZTrait<_vA, _vR, _vG, _vB>::IntegerType))) ARGB
{
	using Trait = AXYZTrait<_vA, _vR, _vG, _vB>;
	//! \since build 728
	//@{
	using ArrayType = typename Trait::ArrayType;
	using IntegerType = typename Trait::IntegerType;
	using CMap = AZYXMapping::ABGR;
	using AType = typename Trait::template component_t<CMap::A>;
	using BType = typename Trait::template component_t<CMap::B>;
	using GType = typename Trait::template component_t<CMap::G>;
	using RType = typename Trait::template component_t<CMap::R>;

	ArrayType Bytes;
	IntegerType Integer;
	//@}

//#if !LITTLE_ENDIAN
//#	error Unsupported integer endianness found.
//#endif

	DefDeCtor(ARGB)
	//! \since build 728
	//@{
	yconstfn
	ARGB(IntegerType i)
		: Integer(i)
	{}
	yconstfn
	ARGB(AType a, RType r, GType g, BType b)
		: Integer(Trait::pack(a, r, g, b))
	{}

	yconstfn DefCvt(const ynothrow, IntegerType, Integer)

	yconstfn DefGetter(const ynothrow, AType, A,
		Trait::template extract<CMap::A>(Integer))
	yconstfn DefGetter(const ynothrow, BType, B,
		Trait::template extract<CMap::B>(Integer))
	yconstfn DefGetter(const ynothrow, GType, G,
		Trait::template extract<CMap::G>(Integer))
	yconstfn DefGetter(const ynothrow, RType, R,
		Trait::template extract<CMap::R>(Integer))
	//@}
};


//! \since build 417
//@{
using MonoType = octet;
using AlphaType = octet;
//@}

#if YCL_DS
	/*!
	\brief 标识 XYZ1555 像素格式。
	\note 值表示按整数表示的顺序从高位到低位为 ABGR 。
	\since build 297
	*/
#	define YCL_PIXEL_FORMAT_XYZ555 0xAABBCCDD

/*!
\brief LibNDS 兼容像素。
\relates Pixel
\note 小端序整数表示 ABGR1555 。
*/
using Pixel = RGBA<5, 5, 5, 1>;
/*!
\brief 取像素 Alpha 值。
\relates Pixel
\since build 417
*/
yconstfn PDefH(AlphaType, FetchAlpha, Pixel px) ynothrow
	ImplRet(px.GetA() != 0 ? 0xFF : 0)

/*!
\brief 取不透明像素。
\relates Pixel
\since build 413
*/
yconstfn PDefH(Pixel, FetchOpaque, Pixel px) ynothrow
	ImplRet(px.Integer | 1 << 15)

/*!
\brief 使用 8 位 RGB 构造 std::uint16_t 类型像素。
\since build 441
*/
yconstfn PDefH(std::uint16_t, FetchPixel, MonoType r, MonoType g, MonoType b)
	ynothrow
	ImplRet(r >> 3 | std::uint16_t(g >> 3) << 5 | std::uint16_t(b >> 3) << 10)

#	define DefColorH_(hex, name) name = \
	(FetchPixel(((hex) >> 16) & 0xFF, ((hex) >> 8) & 0xFF, (hex) & 0xFF) \
	| 1 << 15)
#elif YCL_Win32 || YCL_Android || YCL_Linux || YCL_OS_X
// TODO: Real implementation for X11.
#	if YCL_Win32
/*!
\brief 标识 XYZ888 像素格式。
\note 值表示按整数表示的顺序从高位到低位为 ARGB 。
\since build 297
*/
#		define YCL_PIXEL_FORMAT_XYZ888 0xAADDCCBB

/*!
\brief Windows DIB 格式兼容像素。
\note MSDN 注明此处第 4 字节保留为 0 ，但此处使用作为 8 位 Alpha 值使用。
	即小端序整数 ARGB8888 （存储序 BGRA8888 ），兼容 \c ::AlphaBlend 使用的格式。
\note 转换 DIB 在设备上下文绘制时无需转换格式，比 ::COLORREF 更高效。
\warning 仅用于屏幕绘制，不保证无条件兼容所有 DIB 。
\see https://msdn.microsoft.com/en-us/library/windows/desktop/dd183352(v=vs.85).aspx 。
\since build 441
\todo 断言对齐，保证类型兼容。
*/
using Pixel = BGRA<8, 8, 8, 8>;
#	else
/*!
\brief 标识 XYZ888 像素格式。
\note 值表示按整数表示的顺序从高位到低位为 ABGR 。
\since build 506
*/
#		define YCL_PIXEL_FORMAT_XYZ888 0xAABBCCDD
/*!
\brief Android 像素。
\note Android 设备可选多种格式，在此使用 WINDOW_FORMAT_RGBA_8888 。
\since build 506
\todo 断言对齐，保证类型兼容。
*/
using Pixel = RGBA<8, 8, 8, 8>;
#	endif

/*!
\brief 取像素 Alpha 值。
\since build 417
*/
yconstfn PDefH(AlphaType, FetchAlpha, Pixel px) ynothrow
	ImplRet(px.GetA())

/*!
\brief 取不透明像素。
\relates Pixel
\since build 413
*/
yconstfn PDefH(Pixel, FetchOpaque, Pixel px) ynothrow
#	if YCL_Win32
	ImplRet({px.GetB(), px.GetG(), px.GetR(), 0xFF})
#	else
	ImplRet({px.GetB(), px.GetG(), px.GetR(), 0xFF})
#	endif

/*!
\brief 使用 8 位 RGB 构造 std::uint32_t 像素。
\relates Pixel
\since build 417
*/
yconstfn PDefH(std::uint32_t, FetchPixel,
	AlphaType r, AlphaType g, AlphaType b) ynothrow
	ImplRet(std::uint32_t(r) | std::uint32_t(g) << 8 | std::uint32_t(b) << 16)

/*!
\brief 定义 Windows DIB 格式兼容像素。
\note 得到的 32 位整数和 ::RGBQUAD 在布局上兼容。
\note Alpha 值为 0xFF 。
\relates Pixel
\since build 296
*/
#	define DefColorH_(hex, name) \
	name = (FetchPixel(((hex) >> 16) & 0xFF, \
		((hex) >> 8) & 0xFF, (hex) & 0xFF) << 8 | 0xFF)
#else
#	error "Unsupported platform found."
#endif


//! \brief 系统默认颜色空间。
namespace ColorSpace
{
//	#define DefColorA(r, g, b, name) name = ARGB16(1, r, g, b),
#define	HexAdd0x(hex) 0x##hex
#define DefColorH(hex_, name) DefColorH_(HexAdd0x(hex_), name)

/*!
\brief 默认颜色集。
\see http://www.w3schools.com/html/html_colornames.asp 。
\since build 416
*/
enum ColorSet : Pixel::Trait::IntegerType
{
	DefColorH(00FFFF, Aqua),
	DefColorH(000000, Black),
	DefColorH(0000FF, Blue),
	DefColorH(FF00FF, Fuchsia),
	DefColorH(808080, Gray),
	DefColorH(008000, Green),
	DefColorH(00FF00, Lime),
	DefColorH(800000, Maroon),
	DefColorH(000080, Navy),
	DefColorH(808000, Olive),
	DefColorH(800080, Purple),
	DefColorH(FF0000, Red),
	DefColorH(C0C0C0, Silver),
	DefColorH(008080, Teal),
	DefColorH(FFFFFF, White),
	DefColorH(FFFF00, Yellow)
};

#undef DefColorH
#undef DefColorH_
#undef HexAdd0x
} // namespace ColorSpace;


//! \brief 颜色。
class YF_API Color
{
public:
	using ColorSet = ColorSpace::ColorSet;

private:
	/*!
	\brief RGB 分量。
	\since build 276
	*/
	MonoType r = 0, g = 0, b = 0;
	/*!
	\brief Alpha 分量。
	\since build 276
	*/
	AlphaType a = 0;

public:
	/*!
	\brief 无参数构造：所有分量为 0 的默认颜色。
	\since build 319
	*/
	yconstfn
	Color() = default;
	/*!
	\brief 构造：使用本机颜色对象。
	\since build 319
	*/
	yconstfn
	Color(Pixel px) ynothrow
#if YCL_DS
		: r(px.GetR() << 3), g(px.GetG() << 3), b(px.GetB() << 3),
		a(FetchAlpha(px) ? 0xFF : 0x00)
#elif YCL_Win32 || YCL_Linux || YCL_OS_X
		: r(px.GetR()), g(px.GetG()), b(px.GetB()), a(px.GetA())
#endif
	{}
	/*!
	\brief 构造：使用默认颜色。
	\since build 319
	*/
	yconstfn
	Color(ColorSet cs) ynothrow
#if YCL_DS
		: Color(Pixel(cs))
#elif YCL_Win32 || YCL_Android || YCL_Linux || YCL_OS_X
		: r((cs & 0xFF00) >> 8), g((cs & 0xFF0000) >> 16),
		b((cs & 0xFF000000) >> 24), a(0xFF)
#endif
	{}
	/*!
	\brief 构造：使用 RGB 值和 alpha 位。
	\since build 319
	*/
	yconstfn
	Color(MonoType r_, MonoType g_, MonoType b_, AlphaType a_ = 0xFF) ynothrow
		: r(r_), g(g_), b(b_), a(a_)
	{}
	/*!
	\brief 构造：使用相同类型转换为单色的 RGB 值和 alpha位。
	\note 避免列表初始化时 narrowing 转换。
	\since build 360
	*/
	template<typename _tScalar>
	yconstfn
	Color(_tScalar r_, _tScalar g_, _tScalar b_, AlphaType a_ = 0xFF) ynothrow
		: Color(MonoType(r_), MonoType(g_), MonoType(b_), a_)
	{}

	/*!
	\brief 转换：本机颜色对象。
	\since build 319
	*/
	yconstfn
	operator Pixel() const ynothrow
	{
#if YCL_DS
		return int(a != 0) << 15 | FetchPixel(r, g, b);
#elif YCL_Win32
		return {b, g, r, a};
#elif YCL_Android || YCL_Linux || YCL_OS_X
		return {r, g, b, a};
#endif
	}

	/*!
	\brief 取 alpha 分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, A, a)
	/*!
	\brief 取蓝色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, B, b)
	/*!
	\brief 取绿色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, G, g)
	/*!
	\brief 取红色分量。
	\since build 319
	*/
	yconstfn DefGetter(const ynothrow, MonoType, R, r)
};


/*!
\brief 初始化视频输出。
\warning 不保证线程安全性。
*/
YF_API bool
InitVideo();

} // namespace platform;

#endif

