/*
	© 2011-2015, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Video.h
\ingroup YCLib
\brief 平台相关的视频输出接口。
\version r1789
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2011-05-26 19:41:08 +0800
\par 修改时间:
	2018-09-20 02:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Video
*/


#ifndef YCL_INC_Video_h_
#define YCL_INC_Video_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon // for std::is_signed, std::is_unsigned;
#include <ystdex/bitseg.hpp> // for ystdex::integer_width,
//	ystdex::ordered_bitseg_traits, ystdex::bitseg_traits;

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


//! \since build 729
//@{
//! \ingroup type_traits_operations
//@{
/*!
\breif 逻辑分量值特征。
\note A 表示 Alpha 通道分量， X 、 Y 和 Z 表示颜色分量。
\note 模板参数为位段特征及各个逻辑分量在位段特征中的存储索引。
\sa ystdex::bitseg_traits
\sa ystdex::ordered_bitseg_traits

四个逻辑分量以特定布局以特定的映射关系压缩按整数小端序存储为位段。
逻辑分量值特征允许不同的逻辑分量顺序具有一致的逻辑表示。
逻辑分量存储的顺序、偏移和位宽由位段特征指定。
模板参数列表中，逻辑分量统一为 AXYZ 顺序，对应具有索引值 0~3 ，其值为存储索引。
逻辑分量索引和存储索引之间的映射的接口由有序位段特征适配模板实例的成员提供。
*/
template<class _tBitSegTraits, size_t _vIdxA, size_t _vIdxX, size_t _vIdxY,
	size_t _vIdxZ>
struct AXYZValueTraits : ystdex::ordered_bitseg_traits<_tBitSegTraits, _vIdxA,
	_vIdxX, _vIdxY, _vIdxZ>
{
	using Base = ystdex::ordered_bitseg_traits<_tBitSegTraits, _vIdxA, _vIdxX,
		_vIdxY, _vIdxZ>;

	static yconstexpr const size_t ABitsN
		= Base::template component_width<AXYZIndex::A>::value;
	static yconstexpr const size_t XYZBitsN
		= Base::template component_width<AXYZIndex::X>::value
		+ Base::template component_width<AXYZIndex::Y>::value
		+ Base::template component_width<AXYZIndex::Z>::value;
};


//! \note 调整分量索引参数顺序为逻辑顺序排序的逻辑分量值特征别名。
//@{
//! \brief AXYZ 逻辑分量特征。
template<size_t _vA, size_t _vX, size_t _vY, size_t _vZ>
using AXYZTraits
	= AXYZValueTraits<ystdex::bitseg_traits<_vA, _vX, _vY, _vZ>, 0, 1, 2, 3>;

//! \brief XYZA 逻辑分量特征。
template<size_t _vX, size_t _vY, size_t _vZ, size_t _vA>
using XYZATraits
	= AXYZValueTraits<ystdex::bitseg_traits<_vX, _vY, _vZ, _vA>, 3, 0, 1, 2>;
//@}
//@}
//@}


/*!
\note 作为 POD 类型，可以用于储存像素。
\warning 用户应检查存储表示的实际大小是否和本类型一致。
\bug 当前忽略对齐和特定类型的一致性。
*/
//@{
/*!
\brief 像素四元组。

作为像素的具有四个值分量 ABGR 的联合类型，可被整数或数组表示，以位段实现存储布局控制。
模板参数依次为逻辑分量特征、值分量映射枚举和四个值分量的位宽。
其中表示分量位宽的参数和对应分量在位段中存储的顺序相同。
具体存储布局和存储映射的接口由逻辑分量特征提供。
值分量被值分量映射枚举一一映射为 AXYZ 逻辑分量。
值分量映射枚举应保证映射后的逻辑分量和逻辑分量特征使用的存储索引对应的逻辑分量一致。
不直接依赖数组类型访问存储，因此对字节序中立。
*/
template<class _tTraits, typename _tComponentMap, size_t _v1, size_t _v2,
	size_t _v3, size_t _v4>
union YB_ATTR(packed) YB_ATTR(aligned(yalignof(typename _tTraits::integer)))
	PixelQuadruple
{
	using Traits = _tTraits;
	using StorageTraits = typename _tTraits::base;
	using ArrayType = typename StorageTraits::array;
	using IntegerType = typename Traits::integer;
	//! \brief 存储分量类型：值分量按参数列表出现顺序的索引的类型。
	template<size_t _vIdx>
	using StoredComponentType
		= typename StorageTraits::template component_t<_vIdx>;
	//! \brief 分量映射枚举：值分量 ABGR 分别映射为逻辑分量 AXYZ 之一。
	using CMap = _tComponentMap;
	using AType = typename Traits::template component_t<CMap::A>;
	using BType = typename Traits::template component_t<CMap::B>;
	using GType = typename Traits::template component_t<CMap::G>;
	using RType = typename Traits::template component_t<CMap::R>;

	ArrayType Bytes;
	IntegerType Integer;

	DefDeCtor(PixelQuadruple)
	yconstfn
	PixelQuadruple(IntegerType i) ynothrow
		: Integer(i)
	{}
	yconstfn
	PixelQuadruple(StoredComponentType<0> v0, StoredComponentType<1> v1,
		StoredComponentType<2> v2, StoredComponentType<3> v3) ynothrow
		: Integer(Traits::base::pack(v0, v1, v2, v3))
	{}

	yconstfn DefCvt(const ynothrow, IntegerType, Integer)

	yconstfn DefGetter(const ynothrow, AType, A,
		Traits::template extract<CMap::A>(Integer))
	yconstfn DefGetter(const ynothrow, BType, B,
		Traits::template extract<CMap::B>(Integer))
	yconstfn DefGetter(const ynothrow, GType, G,
		Traits::template extract<CMap::G>(Integer))
	yconstfn DefGetter(const ynothrow, RType, R,
		Traits::template extract<CMap::R>(Integer))
	//@}
};


template<size_t _vB, size_t _vG, size_t _vR, size_t _vA>
using BGRA = PixelQuadruple<XYZATraits<_vB, _vG, _vR, _vA>, AXYZMapping::ABGR,
	_vB, _vG, _vR, _vA>;


template<size_t _vR, size_t _vG, size_t _vB, size_t _vA>
using RGBA = PixelQuadruple<XYZATraits<_vR, _vG, _vB, _vA>, AZYXMapping::ABGR,
	_vR, _vG, _vB, _vA>;


template<size_t _vA, size_t _vR, size_t _vG, size_t _vB>
using ARGB = PixelQuadruple<AXYZTraits<_vA, _vR, _vG, _vB>, AZYXMapping::ABGR,
	_vA, _vR, _vG, _vB>;


template<size_t _vA, size_t _vB, size_t _vG, size_t _vR>
using ABGR = PixelQuadruple<AXYZTraits<_vA, _vB, _vG, _vR>, AXYZMapping::ABGR,
	_vA, _vB, _vG, _vR>;
//@}
//@}


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
	即整数 ARGB8888 （小端序存储序 BGRA8888 ），兼容 \c ::AlphaBlend 使用的格式。
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
\note 值表示按整数表示的顺序从高位到低位为 ABGR8888 （小端序存储序 RGBA8888 ）。
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
	ImplRet({px.GetR(), px.GetG(), px.GetB(), 0xFF})
#	endif

/*!
\brief 使用 8 位 RGB 构造 std::uint32_t 像素。
\relates Pixel
\since build 417
*/
yconstfn PDefH(std::uint32_t, FetchPixel, MonoType r, MonoType g, MonoType b)
	ynothrow
	ImplRet(std::uint32_t(r) | std::uint32_t(g) << 8 | std::uint32_t(b) << 16)
#else
#	error "Unsupported platform found."
#endif


/*!
\brief 像素分量转换。
\since build 839
*/
//@{
yconstfn AlphaType
PixelToAlpha(Pixel px) ynothrow
{
#if YCL_DS
	return FetchAlpha(px) ? 0xFF : 0x00;
#elif YCL_Win32 || YCL_Linux || YCL_OS_X
	return px.GetA();
#endif
}

#if YCL_DS
#	define YCL_Impl_RGB_Shift << 3
#elif YCL_Win32 || YCL_Linux || YCL_OS_X
#	define YCL_Impl_RGB_Shift
#endif
yconstfn MonoType
PixelToBlue(Pixel px) ynothrow
{
	return px.GetB() YCL_Impl_RGB_Shift;
}

yconstfn MonoType
PixelToGreen(Pixel px) ynothrow
{
	return px.GetG() YCL_Impl_RGB_Shift;
}

yconstfn MonoType
PixelToRed(Pixel px) ynothrow
{
	return px.GetR() YCL_Impl_RGB_Shift;
}
#undef YCL_Impl_RGB_Shift

yconstfn Pixel
ColorComponentsToPixel(MonoType r, MonoType g, MonoType b, AlphaType a) ynothrow
{
#if YCL_DS
	return int(a != 0) << 15 | FetchPixel(r, g, b);
#elif YCL_Win32
	return {b, g, r, a};
#elif YCL_Android || YCL_Linux || YCL_OS_X
	return {r, g, b, a};
#endif
}
//@}

/*!
\brief 初始化视频输出。
\warning 不保证线程安全性。
*/
YF_API bool
InitVideo();

} // namespace platform;

#endif

