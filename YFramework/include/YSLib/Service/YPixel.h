/*
	© 2010-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YPixel.h
\ingroup Service
\brief 体系结构中立的像素操作。
\version r1174
\author FrankHB <frankhb1989@gmail.com>
\since build 442
\par 创建时间:
	2013-09-02 00:46:13 +0800
\par 修改时间:
	2016-09-21 15:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YPixel
*/


#ifndef YSL_INC_Service_YPixel_h_
#define YSL_INC_Service_YPixel_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition
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
\ingroup metafunctions
\since build 685
*/
//@{
template<class _tPixel>
using MaskTrait = typename _tPixel::Traits;


template<class _tPixel, class _tMask>
using EnableForMask = ystdex::enable_when<std::is_same<ystdex::detected_or_t<
	void, MaskTrait, _tPixel>, _tMask>::value>;
//@}


/*!
\brief 着色器命名空间。
\since build 442
*/
namespace Shaders
{

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
		static_assert(ystdex::is_normalizable<_tDstAlpha>(),
			"Non-normalizable destination alpha type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
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
		static_assert(ystdex::is_normalizable<_tDst>(),
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>(),
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>(),
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
		static_assert(ystdex::is_normalizable<_tDstAlpha>(),
			"Non-normalizable destination alpha type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");

		return sa != 0 ? _tDstAlpha(1) : da;
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
		static_assert(ystdex::is_normalizable<_tDst>(),
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>(),
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>(),
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
		static_assert(ystdex::is_normalizable<_tDstAlpha>(),
			"Non-normalizable destination alpha type found.");

		return _tDstAlpha(1);
	}
	template<typename _tDstAlpha, typename _tSrcAlpha>
	static yconstfn _tDstAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha)
	{
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
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
		static_assert(ystdex::is_normalizable<_tSrc>(),
			"Non-normalizable source type found.");

		return s;
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tSrc
	CompositeComponentOver(_tDst, _tSrc s, _tSrcAlpha, _tAlpha)
	{
		static_assert(ystdex::is_normalizable<_tSrc>(),
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>(),
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
		static_assert(ystdex::is_normalizable<_tDstAlpha>(),
			"Non-normalizable destination alpha type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");

		return da != 0 ? _tSrcAlpha(1) : sa;
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
		static_assert(ystdex::is_normalizable<_tDst>(),
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>(),
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>(),
			"Non-normalizable alpha type found.");

		return a != _tAlpha(0) ? GPixelCompositor<0, _vSrcAlphaBits>
			::CompositeComponentOver(d, s, sa) : _tDst(0);
	}
};

//! \note 不透明目标。
template<size_t _vSrcAlphaBits>
struct GPixelCompositor<0, _vSrcAlphaBits> : private GPixelCompositor<2, 0>
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
		static_assert(ystdex::is_normalizable<_tDst>(),
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>(),
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");

		return s < d ? _tDst(d - sa * (d - s)) : _tDst(sa * (s - d) + d);
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha)
	{
		static_assert(ystdex::is_normalizable<_tAlpha>(),
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
		static_assert(ystdex::is_normalizable<_tDst>(),
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>(),
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");

		return sa != 0 ? _tDst(s) : _tDst(d);
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstfn _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha)
	{
		static_assert(ystdex::is_normalizable<_tAlpha>(),
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
		static_assert(ystdex::is_normalizable<_tDstAlpha>(),
			"Non-normalizable destination alpha type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");

		return sa != 0 || da != 0 ? _tDstAlpha(1) : _tDstAlpha(0);
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
		static_assert(ystdex::is_normalizable<_tDst>(),
			"Non-normalizable destination type found.");
		static_assert(ystdex::is_normalizable<_tSrc>(),
			"Non-normalizable source type found.");
		static_assert(ystdex::is_normalizable<_tSrcAlpha>(),
			"Non-normalizable source alpha type found.");
		static_assert(ystdex::is_normalizable<_tAlpha>(),
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
	using pix = make_fixed_t<_vSrcAlphaBits>;

	return GPixelCompositor<1, _vSrcAlphaBits>::CompositeComponentOver(
		pix(d, raw_tag()), pix(s, raw_tag()), pix(sa, raw_tag()), pix(1)).get();
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
	using pixd = make_fixed_t<_vDstAlphaBits>;
	using pix = make_fixed_t<_vSrcAlphaBits>;

	return GPixelCompositor<_vDstAlphaBits, _vSrcAlphaBits>
		::CompositeComponentOver(pix(d, raw_tag()), pix(s, raw_tag()),
		pix(sa, raw_tag()), pixd(a, raw_tag())).get();
}


/*!
\brief AXYZ1555 格式 Pixel 的 Alpha 混合。
\tparam _vAlphaBits 表示 Alpha 值的有效整数位数。
\since build 442

设 MaxAlpha := (1 << _vAlphaBits) - 1 ，
使用下列公式进行像素的 Alpha 混合（其中 alpha = a / MaxAlpha）：
输出分量： component := (1 - alpha) * d + alpha * s
= ((MaxAlpha - a) * d + a * s) >> _vAlphaBits
= d + ((a * (s - d)) >> _vAlphaBits) 。
背景透明：不设置 Alpha 。
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
	return (dbr & 0x1F) | (dg & 0x3E0) | (dbr >> 6 & 0x7C00);
}


/*
\note 使用引用传递像素类型以便优化。
\todo 支持浮点数。
\since build 442
*/
//@{
/*!
\brief 分发像素混合操作。
\since build 685
\todo 变通或解决 GCC 偏特化顺序问题。
*/
#if YB_IMPL_GNUCPP
template<typename _tPixel, typename = void>
struct GBlender : GBlender<_tPixel, _t<std::is_same<
	ystdex::detected_or_t<void, MaskTrait, _tPixel>, XYZATraits<5, 5, 5, 1>>>>
{};
#else
template<typename _tPixel, typename = void>
struct GBlender : GBlender<_tPixel, ystdex::when<true>>
{};
#endif

#if YB_IMPL_GNUCPP
template<typename _tPixel>
struct GBlender<_tPixel, ystdex::false_>
#else
template<typename _tPixel, bool _bCond>
struct GBlender<_tPixel, ystdex::when<_bCond>>
#endif
{
	/*!
	\note 使用 ADL BlendComponent 指定混合像素分量。
	\sa Drawing::BlendComponent
	*/
	//@{
	template<size_t _vSrcAlphaBits, typename _tSrcAlphaInt>
	static yconstfn _tPixel
	Blend(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa)
	{
		static_assert(std::is_integral<_tSrcAlphaInt>(),
			"Invalid integer source alpha type found.");

		return Color(BlendComponent<_vSrcAlphaBits>(d.GetR(), s.GetR(), sa),
			BlendComponent<_vSrcAlphaBits>(d.GetG(), s.GetG(), sa),
			BlendComponent<_vSrcAlphaBits>(d.GetB(), s.GetB(), sa),
			(1 << _vSrcAlphaBits) - 1);
	}

	template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits,
		typename _tSrcAlphaInt>
	static yconstfn _tPixel
	BlendAlpha(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa)
	{
		static_assert(std::is_integral<_tSrcAlphaInt>(),
			"Invalid integer source alpha type found.");
		using namespace ystdex;
		using pixd = make_fixed_t<_vDstAlphaBits>;
		using pix = make_fixed_t<_vSrcAlphaBits>;

		return Color(BlendComponent<_vSrcAlphaBits>(d.GetR(), s.GetR(), sa),
			BlendComponent<_vSrcAlphaBits>(d.GetG(), s.GetG(), sa),
			BlendComponent<_vSrcAlphaBits>(d.GetB(), s.GetB(), sa),
			GPixelCompositor<_vDstAlphaBits, _vSrcAlphaBits>
			::CompositeAlphaOver(pixd(d.GetA(), raw_tag()),
			pix(sa, raw_tag())).get());
	}
	//@}

	/*!
	\note 使用 ADL CompositeComponent 指定组合像素分量。
	\sa Drawing::CompositeComponent
	*/
	template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits,
		typename _tSrcAlphaInt, typename _tAlphaInt>
	static yconstfn _tPixel
	Composite(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa,
		_tAlphaInt a)
	{
		static_assert(std::is_integral<_tSrcAlphaInt>(),
			"Invalid integer source alpha type found.");
		static_assert(std::is_integral<_tAlphaInt>(),
			"Invalid integer result alpha type found.");

		return Color(CompositeComponent<_vDstAlphaBits, _vSrcAlphaBits>(
			d.GetR(), s.GetR(), sa, a), CompositeComponent<_vDstAlphaBits,
			_vSrcAlphaBits>(d.GetG(), s.GetG(), sa, a), CompositeComponent<
			_vDstAlphaBits, _vSrcAlphaBits>(d.GetB(), s.GetB(), sa, a), a);
	}
};

/*!
\note 使用 ADL BlendCore 代理混合像素调用。
\sa BlendCore
*/
#if YB_IMPL_GNUCPP
template<typename _tPixel>
struct GBlender<_tPixel, ystdex::true_>
#else
template<typename _tPixel>
struct GBlender<_tPixel, EnableForMask<_tPixel, XYZATraits<5, 5, 5, 1>>>
#endif
{
	template<size_t _vSrcAlphaBits, typename _tSrcAlphaInt>
	static yconstfn _tPixel
	Blend(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa)
	{
		static_assert(std::is_integral<_tSrcAlphaInt>(),
			"Invalid integer source alpha type found.");

		return BlendCore<_vSrcAlphaBits>(d, s, sa) | 1 << 15;
	}

	template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits,
		typename _tSrcAlphaInt>
	static yconstfn _tPixel
	BlendAlpha(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa)
	{
		static_assert(std::is_integral<_tSrcAlphaInt>(),
			"Invalid integer source alpha type found.");
		using namespace ystdex;
		using pixd = make_fixed_t<_vDstAlphaBits>;
		using pix = make_fixed_t<_vSrcAlphaBits>;

		return BlendCore<_vSrcAlphaBits>(d, s, sa) | (GPixelCompositor<
			_vDstAlphaBits, _vSrcAlphaBits>::CompositeAlphaOver(pixd(d.GetA(),
			raw_tag()), pix(sa, raw_tag())).get() != 0 ? 1 << 15 : 0);
	}

	template<size_t, size_t _vSrcAlphaBits, typename _tSrcAlphaInt,
		typename _tAlphaInt>
	static yconstfn _tPixel
	Composite(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa,
		_tAlphaInt a)
	{
		static_assert(std::is_integral<_tSrcAlphaInt>(),
			"Invalid integer source alpha type found.");
		static_assert(std::is_integral<_tAlphaInt>(),
			"Invalid integer alpha type found.");

		return BlendCore<_vSrcAlphaBits>(d, s, sa) | ((a != 0) ? 1 << 15 : 0);
	}
};


/*!
\brief 像素混合：使用指定的源 Alpha 。
\note 输出背景不透明， Alpha 饱和。
\sa GBlender::Blend
*/
template<size_t _vSrcAlphaBits, typename _tPixel, typename _tSrcAlphaInt>
yconstfn _tPixel
Blend(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa)
{
	return GBlender<_tPixel>::template Blend<_vSrcAlphaBits>(d, s, sa);
}


//! \sa Drawing::GPixelCompositor
//@{
/*!
\brief Alpha 像素混合：使用指定的源 Alpha 同时组合透明背景 Alpha 。
\note 忽略源像素中的 Alpha 。
\sa GBlender::BlendAlpha
\since build 561
*/
template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits, typename _tPixel,
	typename _tSrcAlphaInt>
yconstfn _tPixel
BlendAlpha(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa)
{
	return GBlender<_tPixel>::template BlendAlpha<_vDstAlphaBits,
		_vSrcAlphaBits>(d, s, sa);
}


//! \sa GBlender::Composite
//@{
//! \brief 像素组合：使用指定的源 Alpha 和结果 Alpha 。
template<size_t _vDstAlphaBits, size_t _vSrcAlphaBits, typename _tPixel,
	typename _tSrcAlphaInt, typename _tAlphaInt>
yconstfn _tPixel
Composite(const _tPixel& d, const _tPixel& s, _tSrcAlphaInt sa, _tAlphaInt a)
{
	return GBlender<_tPixel>::template Composite<_vDstAlphaBits,
		_vSrcAlphaBits>(d, s, sa, a);
}
//! \note 使用 ADL Composite 代理组合像素调用。
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
	using pixd = make_fixed_t<_vDstAlphaBits>;
	using pix = make_fixed_t<_vSrcAlphaBits>;

	return Composite<_vDstAlphaBits, _vSrcAlphaBits>(d, s, GPixelCompositor<
		_vDstAlphaBits, _vSrcAlphaBits>::CompositeAlphaOver(pixd(d.GetA(),
		raw_tag()), pix(s.GetA(), raw_tag())).get());
}
//@}
//@}
//@}
//@}

} // namespace Shaders;

} // namespace Drawing;

} // namespace YSLib;

#endif

