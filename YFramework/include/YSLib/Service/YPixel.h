/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YPixel.h
\ingroup Service
\brief 体系结构中立的像素操作。
\version r716
\author FrankHB <frankhb1989@gmail.com>
\since build 442
\par 创建时间:
	2013-09-02 00:46:13 +0800
\par 修改时间:
	2014-12-02 18:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YPixel
*/


#ifndef YSL_INC_Service_YPixel_h_
#define YSL_INC_Service_YPixel_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGDIBase
#include <ystdex/rational.hpp>

namespace YSLib
{

namespace Drawing
{

/*!	\defgroup PixelShaders Pixel Shader Operations
\brief 像素着色器操作。
\note 当前支持的类型：目标迭代器×源迭代器→ void 。
\since build 442
*/

/*!
\brief 着色器命名空间。
\since build 442
*/
namespace Shaders
{

/*!
\ingroup PixelShaders
\brief 像素迭代器透明操作。
\warning 不检查迭代器有效性。
\since build 440
*/
struct BlitTransparentPoint
{
	//! \note 使用源迭代器对应像素的第 15 位表示透明性。
	template<typename _tOut, typename _tIn>
	inline void
	operator()(_tOut& dst_iter, _tIn& src_iter)
	{
		if(FetchAlpha(*src_iter))
			*dst_iter = *src_iter;
	}
	//! \note 使用 Alpha 通道表示透明性。
	template<typename _tOut>
	inline void
	operator()(_tOut& dst_iter, IteratorPair& src_iter)
	{
		*dst_iter = *src_iter.base().second & 0x80 ? FetchOpaque(*src_iter)
			: FetchOpaque(PixelType());
	}
};


/*!
\brief 像素组合器。
\tparam _vDstAlphaBits 目标 Alpha 位。
\tparam _vSrcAlphaBits 源 Alpha 位。
\pre 源和目标 Alpha （若存在）为归一化值（可以是浮点数或定点数）。
\note 结果 Alpha 位是源和目标 Alpha 位中的最大值（优先为目标 Alpha 类型）。
\since build 439
\todo 支持推导返回类型。
*/
//@{
template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits>
struct GPixelCompositor
{
	/*!
	\brief Alpha 组合 Alpha 分量。

	a := 1 - (1 - sa) * (1 - da)
		= 1 - (1 - sa - da + sa * da)
		= sa + da - sa * da
		= sa + da * (1 - sa)
	*/
	template<typename _tDstAlpha, typename _tSrcAlpha>
	static yconstfn ystdex::conditional_t<(_vDstAlphaBits < _vSrcAlphaBits),
		_tSrcAlpha, _tDstAlpha>
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha sa)
	{
		static_assert(ystdex::is_normalizable<_tDstAlpha>::value,
			"Non-normalizable destination alpha type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");

		return sa + da * (_tSrcAlpha(1) - sa);
	}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	a != 0
		=> c := (sa * s + (1 - sa) * da * d) / a
		= (sa * s + (a - sa) * d) / a
		= (sa * s + a * d - sa * d) / a
		= (sa * (s - d) + a * d) / a
		= sa * (s - d) / a + d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha a)
	{
		static_assert(ystdex::is_normalizable<_tDst>::value,
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>::value,
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>::value,
			"Non-normalizable alpha type found.");

		return a != _tAlpha(0) ? (s < d ? _tDst(d - sa * (d - s) / a)
			: _tDst(sa * (s - d) / a + d)) : _tDst(0);
	}
};

//! \note 1 位源 Alpha 。
template<size_t _vDstAlphaBits>
struct GPixelCompositor<_vDstAlphaBits, 1>
{
	/*!
	\brief Alpha 组合 Alpha 分量。

	a := sa + da * (1 - sa)
		= sa != 0 ? 1 : da
	*/
	template<typename _tDstAlpha, typename _tSrcAlpha>
	static yconstfn _tDstAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha sa)
	{
		static_assert(ystdex::is_normalizable<_tDstAlpha>::value,
			"Non-normalizable destination alpha type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");

		return sa != 0 ? ystdex::normalized_max<_tDstAlpha>::get() : da;
	}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	a != 0
		=> c := sa * (s - d) / a + d
		= sa != 0 ? (s - d) / a + d : d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha a)
	{
		static_assert(ystdex::is_normalizable<_tDst>::value,
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>::value,
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>::value,
			"Non-normalizable alpha type found.");

		return a != _tAlpha(0) ? (sa != _tSrcAlpha(0) ? (s < d
			? _tDst(d - (d - s) / a) : _tDst((s - d) / a + d)) : d) : _tDst(0);
	}
};

//! \note 不透明源。
template<size_t _vDstAlphaBits>
struct GPixelCompositor<_vDstAlphaBits, 0>
{
	/*!
	\brief Alpha 组合 Alpha 分量。

	sa = 1
		=> a := sa + da * (1 - sa)
		= 1
	*/
	//@{
	template<typename _tDstAlpha>
	static yconstfn _tDstAlpha
	CompositeAlphaOver(_tDstAlpha)
	{
		static_assert(ystdex::is_normalizable<_tDstAlpha>::value,
			"Non-normalizable destination alpha type found.");

		return ystdex::normalized_max<_tDstAlpha>::get();
	}
	template<typename _tDstAlpha, typename _tSrcAlpha>
	static yconstfn _tDstAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha)
	{
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");

		return CompositeAlphaOver(da);
	}
	//@}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	sa := 1
		=> a := 1
		=> c := sa * (s - d) / a + d
		= s - d + d
		= s
	*/
	//@{
	//! \since build 442
	template<typename _tSrc>
	static yconstfn _tSrc
	CompositeComponentOver(_tSrc s)
	{
		static_assert(ystdex::is_normalizable<_tSrc>::value,
			"Non-normalizable source type found.");

		return s;
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tSrc
	CompositeComponentOver(_tDst, _tSrc s, _tSrcAlpha, _tAlpha)
	{
		static_assert(ystdex::is_normalizable<_tSrc>::value,
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>::value,
			"Non-normalizable alpha type found.");

		return CompositeComponentOver(s);
	}
	//@}
};

//! \note 1 位目标 Alpha 。
template<size_t _vSrcAlphaBits>
struct GPixelCompositor<1, _vSrcAlphaBits>
{
	/*!
	\brief Alpha 组合 Alpha 分量。

	a := sa + da * (1 - sa)
		= da != 0 ? 1 : sa
	*/
	template<typename _tDstAlpha, typename _tSrcAlpha>
	static yconstfn _tSrcAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha sa)
	{
		static_assert(ystdex::is_normalizable<_tDstAlpha>::value,
			"Non-normalizable destination alpha type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");

		return da != 0 ? ystdex::normalized_max<_tSrcAlpha>::get() : sa;
	}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	a != 0
		=> c := sa * (s - d) / a + d
		= sa * (s - d) + d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha a)
	{
		static_assert(ystdex::is_normalizable<_tDst>::value,
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>::value,
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>::value,
			"Non-normalizable alpha type found.");

		return a != _tAlpha(0) ? GPixelCompositor<0, _vSrcAlphaBits>
			::CompositeComponentOver(d, s, sa) : _tDst(0);
	}
};

//! \note 不透明目标。
template<size_t _vSrcAlphaBits>
struct GPixelCompositor<0, _vSrcAlphaBits>
	: private GPixelCompositor<2, 0>
{
	/*!
	\brief Alpha 组合 Alpha 分量。
	\since build 442

	da = 1
		=> a := sa + da * (1 - sa)
		= 1
	*/
	using GPixelCompositor<2, 0>::CompositeAlphaOver;

	/*!
	\brief Alpha 组合非 Alpha 分量。

	da = 1
		=> a := 1
		=> c := sa * (s - d) / a + d
		= sa * (s - d) + d
	*/
	//@{
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa)
	{
		static_assert(ystdex::is_normalizable<_tDst>::value,
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>::value,
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");

		return s < d ? _tDst(d - sa * (d - s)) : _tDst(sa * (s - d) + d);
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha)
	{
		static_assert(ystdex::is_normalizable<_tAlpha>::value,
			"Non-normalizable alpha type found.");

		return CompositeComponentOver(d, s, sa);
	}
	//@}
};

//! \note 不透明目标和 1 位源 Alpha 。
template<>
struct GPixelCompositor<0, 1> : private GPixelCompositor<2, 0>
{
	using GPixelCompositor<2, 0>::CompositeAlphaOver;

	/*!
	\brief Alpha 组合非 Alpha 分量。

	da = 1
		=> a := 1
		=> c := sa * (s - d) / a + d
		= sa * (s - d) + d
		= sa != 0 ? s : d
	*/
	//@{
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa)
	{
		static_assert(ystdex::is_normalizable<_tDst>::value,
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>::value,
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");

		return sa != 0 ? _tDst(s) : _tDst(d);
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha)
	{
		static_assert(ystdex::is_normalizable<_tAlpha>::value,
			"Non-normalizable alpha type found.");

		return CompositeComponentOver(d, s, sa);
	}
	//@}
};

/*!
\note  1 位目标 Alpha 和不透明源。
\since build 442
*/
template<>
struct GPixelCompositor<1, 0> : public GPixelCompositor<2, 0>
{};

//! \note 1 位源和目标 Alpha 。
template<>
struct GPixelCompositor<1, 1>
{
	/*!
	\brief Alpha 组合 Alpha 分量。

	a := sa + da * (1 - sa)
		= sa != 0 || da != 0
	*/
	template<typename _tDstAlpha, typename _tSrcAlpha>
	static yconstfn _tDstAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha sa)
	{
		static_assert(ystdex::is_normalizable<_tDstAlpha>::value,
			"Non-normalizable destination alpha type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");

		return sa != 0 || da != 0 ? ystdex::normalized_max<_tDstAlpha>::get()
			: _tDstAlpha(0);
	}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	a != 0
		=> c := sa * (s - d) / a + d
		= sa * (s - d) + d
		= sa != 0 ? s : d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha a)
	{
		static_assert(ystdex::is_normalizable<_tDst>::value,
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>::value,
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>::value,
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>::value,
			"Non-normalizable alpha type found.");

		return a != _tAlpha(0) ? GPixelCompositor<0, 1>
			::CompositeComponentOver(d, s, sa) : _tDst(0);
	}
};

//! \note 不透明源和目标。
template<>
struct GPixelCompositor<0, 0> : private GPixelCompositor<2, 0>
{
	using GPixelCompositor<2, 0>::CompositeAlphaOver;

	using GPixelCompositor<2, 0>::CompositeComponentOver;
};
//@}


/*!
\brief 像素分量混合。
\sa GPixelCompositor
\since build 442
\todo 支持浮点数。
*/
template<size_t _vSrcAlphaBits, typename _tDstInt, typename _tSrcInt,
	typename _tSrcAlphaInt>
yconstfn _tDstInt
BlendComponent(_tDstInt d, _tSrcInt s, _tSrcAlphaInt sa)
{
	using namespace ystdex;
	static_assert(integer_width<_tDstInt>::value == _vSrcAlphaBits,
		"Invalid integer destination type found.");
	static_assert(integer_width<_tSrcInt>::value == _vSrcAlphaBits,
		"Invalid integer source type found.");
	static_assert(integer_width<_tSrcAlphaInt>::value == _vSrcAlphaBits,
		"Invalid integer source alpha type found.");
	using pix = fixed_point<typename make_width_int<_vSrcAlphaBits + 1>
		::unsigned_least_type, _vSrcAlphaBits>;

	return GPixelCompositor<1, _vSrcAlphaBits>::CompositeComponentOver(
		pix(d, raw_tag()), pix(s, raw_tag()), pix(sa, raw_tag()),
		normalized_max<pix>::get()).get();
}

/*!
\brief 像素分量组合。
\sa GPixelCompositor
\since build 442
\todo 支持浮点数。
*/
template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits, typename _tDstInt,
	typename _tSrcInt, typename _tSrcAlphaInt, typename _tAlphaInt>
yconstfn _tDstInt
CompositeComponent(_tDstInt d, _tSrcInt s, _tSrcAlphaInt sa, _tAlphaInt a)
{
	using namespace ystdex;
	static_assert(integer_width<_tDstInt>::value == _vSrcAlphaBits,
		"Invalid integer destination type found.");
	static_assert(integer_width<_tSrcInt>::value == _vSrcAlphaBits,
		"Invalid integer source type found.");
	static_assert(integer_width<_tSrcAlphaInt>::value == _vSrcAlphaBits,
		"Invalid integer source alpha type found.");
	static_assert(integer_width<_tAlphaInt>::value > _vDstAlphaBits,
		"Invalid integer result alpha type found.");
	using pixd = fixed_point<typename make_width_int<_vDstAlphaBits + 1>
		::unsigned_least_type, _vDstAlphaBits>;
	using pix = fixed_point<typename make_width_int<_vSrcAlphaBits + 1>
		::unsigned_least_type, _vSrcAlphaBits>;

	return GPixelCompositor<_vDstAlphaBits, _vSrcAlphaBits>
		::CompositeComponentOver(pix(d, raw_tag()), pix(s, raw_tag()),
		pix(sa, raw_tag()), pixd(a, raw_tag())).get();
}


/*!
\brief AXYZ1555 格式 PixelType 的 Alpha 混合。
\tparam _vAlphaBits 表示 Alpha 值的有效整数位数。
\since build 442

设 MaxAlpha := (1 << _vAlphaBits) - 1 ，
使用下列公式进行像素的 Alpha 混合（其中 alpha = a / MaxAlpha）：
输出分量： component := (1 - alpha) * d + alpha * s
= ((MaxAlpha - a) * d + a * s) >> _vAlphaBits
= d + ((a * (s - d)) >> _vAlphaBits) 。
背景透明，输出 Alpha 饱和。
像素格式： 16 位 AXYZ1555 。
以 ARGB1555 为例，算法实现示意：
					 arrrrrgggggbbbbb
	0000000000arrrrr gggggbbbbb000000
	0000000000011111 0000000000011111
	00000000000rrrrr 00000000000bbbbb : dbr
	0000000000000000 000000ggggg00000 : dg
分解分量至 32 位寄存器以减少总指令数。
*/
template<size_t _vAlphaBits>
std::uint16_t
BlendCore(std::uint32_t d, std::uint32_t s, std::uint8_t a)
{
	static_assert(_vAlphaBits > 0 && _vAlphaBits < 16 - 5,
		"Wrong number of alpha bits found.");

	std::uint32_t dbr((d & 0x1F) | (d << 6 & 0x1F0000)), dg(d & 0x3E0);

	yunseq(dbr += ((((s & 0x1F) | (s << 6 & 0x1F0000)) - dbr) * a)
		>> _vAlphaBits, dg += (((s & 0x3E0) - dg) * a) >> _vAlphaBits);
	return (dbr & 0x1F) | (dg & 0x3E0) | (dbr >> 6 & 0x7C00) | 1 << 15;
}


/*!
\brief 像素混合：使用指定的源 Alpha 。
\note 背景透明，输出 Alpha 饱和。
\note 使用引用传递像素类型以便优化。
\sa Drawing::BlendComponent
\since build 442
\todo 支持浮点数。
*/
//@{
//! \note 使用 ADL <tt>BlendComponent</tt> 指定混合像素分量。
template<size_t _vSrcAlphaBits, typename _tPixel, typename _tSrcAlphaInt>
yconstfn _tPixel
Blend(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa)
{
	static_assert(std::is_integral<_tSrcAlphaInt>::value,
		"Invalid integer source alpha type found.");

	return Color(BlendComponent<_vSrcAlphaBits>(d.GetR(), s.GetR(), sa),
		BlendComponent<_vSrcAlphaBits>(d.GetG(), s.GetG(), sa), BlendComponent<
		_vSrcAlphaBits>(d.GetB(), s.GetB(), sa), (1 << _vSrcAlphaBits) - 1);
}
//! \note 使用 ADL <tt>BlendCore</tt> 代理混合像素调用。
template<size_t _vSrcAlphaBits, typename _tSrcAlphaInt>
RGBA<5, 5, 5, 1>
Blend(const RGBA<5, 5, 5, 1>& d, const RGBA<5, 5, 5, 1>& s, _tSrcAlphaInt sa)
{
	static_assert(std::is_integral<_tSrcAlphaInt>::value,
		"Invalid integer source alpha type found.");

	return BlendCore<_vSrcAlphaBits>(d, s, sa);
}
//@}

/*!
\note 使用引用传递像素类型以便优化。
\sa Drawing::CompositeComponent
\since build 442
\todo 支持浮点数。
*/
//@{
/*
\brief 像素组合：使用指定的源 Alpha 和结果 Alpha 。
\note 使用 ADL <tt>CompositeComponent</tt> 指定组合像素分量。
*/
template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits, typename _tPixel,
	typename _tSrcAlphaInt, typename _tAlphaInt>
yconstfn _tPixel
Composite(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa, _tAlphaInt a)
{
	static_assert(std::is_integral<_tSrcAlphaInt>::value,
		"Invalid integer source alpha type found.");
	static_assert(std::is_integral<_tAlphaInt>::value,
		"Invalid integer result alpha type found.");

	return Color(CompositeComponent<_vDstAlphaBits, _vSrcAlphaBits>(d.GetR(),
		s.GetR(), sa, a), CompositeComponent<_vDstAlphaBits, _vSrcAlphaBits>(
		d.GetG(), s.GetG(), sa, a), CompositeComponent<_vDstAlphaBits,
		_vSrcAlphaBits>(d.GetB(), s.GetB(), sa, a), a);
}
//! \note 使用 ADL <tt>Composite</tt> 代理组合像素调用。
//@{
//! \brief 像素组合：使用指定的结果 Alpha 。
template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits, typename _tPixel,
	typename _tAlphaInt>
yconstfn _tPixel
Composite(const _tPixel& d, const _tPixel& s, _tAlphaInt a)
{
	return Composite<_vDstAlphaBits, _vSrcAlphaBits>(d, s, s.GetA(), a);
}
//! \brief 像素组合。
template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits, typename _tPixel>
yconstfn _tPixel
Composite(const _tPixel& d, const _tPixel& s)
{
	using namespace ystdex;
	using pixd = fixed_point<typename make_width_int<_vDstAlphaBits + 1>
		::unsigned_least_type, _vDstAlphaBits>;
	using pix = fixed_point<typename make_width_int<_vSrcAlphaBits + 1>
		::unsigned_least_type, _vSrcAlphaBits>;

	return Composite<_vDstAlphaBits, _vSrcAlphaBits>(d, s, GPixelCompositor<
		_vDstAlphaBits, _vSrcAlphaBits>::CompositeAlphaOver(pixd(d.GetA(),
		raw_tag()), pix(s.GetA(), raw_tag())).get());
}
//@}
//@}


/*!
\ingroup PixelShaders
\brief 像素计算：Alpha 混合。
\since build 440
*/
struct BlitAlphaPoint
{
	template<typename _tOut>
	inline void
	operator()(_tOut dst_iter, IteratorPair src_iter)
	{
		static_assert(std::is_convertible<ystdex::remove_reference_t<
			decltype(*dst_iter)>, PixelType>::value, "Wrong type found.");

		const AlphaType a(*src_iter.base().second);

		*dst_iter = Blend<8>(*dst_iter, *src_iter, a);
	}
	template<typename _tOut, typename _tIn>
	inline void
	operator()(_tOut dst_iter, ystdex::pair_iterator<
		ystdex::pseudo_iterator<const PixelType>, _tIn> src_iter)
	{
		static_assert(std::is_convertible<ystdex::remove_reference_t<
			decltype(*dst_iter)>, PixelType>::value, "Wrong type found.");

		const AlphaType a(*src_iter.base().second);

		*dst_iter = Blend<8>(*dst_iter, *src_iter, a);
	}
	//! \since build 448
	template<typename _tOut, typename _tIn>
	inline void
	operator()(_tOut dst_iter, _tIn src_iter)
	{
		*dst_iter = Shaders::Composite<ystdex::remove_reference_t<decltype(
			*dst_iter)>::Trait::ABitsN, 8>(*dst_iter, *src_iter);
	}
};

} // namespace Shaders;

} // namespace Drawing;

} // namespace YSLib;

#endif

