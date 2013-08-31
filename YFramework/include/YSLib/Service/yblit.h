/*
	Copyright by FrankHB 2009-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yblit.h
\ingroup Service
\brief 平台无关的图像块操作。
\version r2567
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:43:24 +0800
\par 修改时间:
	2013-08-31 20:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBlit
*/


#ifndef YSL_INC_Service_yblit_h_
#define YSL_INC_Service_yblit_h_ 1

#include "../Core/ygdibase.h"
#include "../Core/ycutil.h"
#include <ystdex/algorithm.hpp>
#include <ystdex/iterator.hpp>
#include <ystdex/rational.hpp>

namespace YSLib
{

namespace Drawing
{

//基本仿函数。

/*!
\brief 像素填充器。
\since build 182
*/
template<typename _tPixel>
struct PixelFiller
{
	_tPixel Color;

	/*!
	\brief 构造：使用指定颜色。
	*/
	explicit inline
	PixelFiller(_tPixel c)
		: Color(c)
	{}

	/*!
	\brief 像素填充函数。
	\since build 438
	*/
	template<typename _tOut>
	inline void
	operator()(_tOut dst)
	{
		*dst = Color;
	}
};

/*!
\brief 序列转换器。
\since build 182
*/
struct SequenceTransformer
{
	/*!
	\brief 渲染连续像素。
	\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
	\since build 438
	*/
	template<typename _tOut, class _fTransformPixel>
	void
	operator()(_tOut dst, size_t n, _fTransformPixel tp)
	{
		if(YB_LIKELY(dst && n))
		{
			_tOut p(dst + n);

			while(--p >= dst)
				tp(p);
		}
	}
};

/*!
\brief 竖直线转换器。
\since build 182
*/
struct VerticalLineTransfomer
{
	/*!
	\brief 渲染竖直线上的像素。
	\tparam _tOut 输出迭代器类型（需要支持 += 操作，一般应是随机迭代器）。
	\since build 438
	*/
	template<typename _tOut, class _fTransformPixel>
	void
	operator()(_tOut dst, size_t n, SDst dw, _fTransformPixel tp)
	{
		if(YB_LIKELY(dst && n))
			while(n--)
			{
				tp(dst);
				dst += dw;
			}
	}
};


/*!
\brief 贴图边界计算器。
\note 前两个 SDst 参数总是初始化为边界坐上最小值。
	当无合适边界时后两个 SDst 参数不被修改。
\since build 438
*/
YF_API bool
BlitBounds(const Point&, const Point&, const Size&, const Size&, const Size&,
	SDst&, SDst&, SDst&, SDst&);

/*!
\brief 贴图偏移量计算器。
\since build 438
*/
//@{
template<bool _bSwapLR, bool _bSwapUD>
size_t
BlitScale(const Point&, const Size&, SDst, SDst);
template<>
inline size_t
BlitScale<false, false>(const Point& dp, const Size& ds, SDst, SDst)
{
	return max<SPos>(0, dp.Y) * ds.Width + max<SPos>(0, dp.X);
}
template<>
inline size_t
BlitScale<true, false>(const Point& dp, const Size& ds, SDst, SDst delta_y)
{
	return (max<SPos>(0, dp.Y) + delta_y - 1) * ds.Width + max<SPos>(0, dp.X);
}
template<>
inline size_t
BlitScale<false, true>(const Point& dp, const Size& ds, SDst delta_x, SDst)
{
	return max<SPos>(0, dp.Y) * ds.Width + max<SPos>(0, dp.X) + delta_x - 1;
}
template<>
inline size_t
BlitScale<true, true>(const Point& dp, const Size& ds, SDst delta_x,
	SDst delta_y)
{
	return (max<SPos>(0, dp.Y) + delta_y - 1) * ds.Width + max<SPos>(0, dp.X)
		+ delta_x - 1;
}
//@}


/*!
\brief 贴图扫描函数模板。
\tparam _bDec 指定是否翻转水平和竖直方向之一。
\tparam _tScalar 度量大小的纯量类型。
\tparam _tDiff 度量偏移量的差值类型。
\tparam _tOut 输出迭代器类型。
\tparam _tIn 输入迭代器类型。
\tparam _fBlitLoop 循环操作类型。
\param loop 循环操作。
\param d_width 目标缓冲区的宽。
\param s_width 源缓冲区的宽。
\param delta_x 实际操作的宽。
\param delta_y 实际操作的高。
\param dst 目标迭代器。
\param src 源迭代器。
\since build 437

对一块矩形区域调用指定的逐像素扫描操作。
*/
template<bool _bDec, typename _tScalar, typename _tDiff, typename _tOut,
	typename _tIn, typename _fBlitLoop>
void
BlitScan(_fBlitLoop loop, _tOut dst, _tIn src, _tScalar d_width,
	_tScalar s_width, _tDiff delta_x, _tDiff delta_y)
{
	loop(dst, src, delta_x, delta_y, (_bDec ? -1 : 1) * d_width - delta_x,
		s_width - delta_x);
}


/*!	\defgroup BlitLoop Blit Loop Operations
\brief 贴图循环操作。
\since build 438
*/

/*!	\defgroup BlitLineScanner Blit Line Scanner Operations
\brief 贴图扫描线操作。
\since build 438
*/

/*!	\defgroup PixelOperation Pixel Operations
\brief 像素操作。
\since build 440
*/


/*!
\brief 贴图函数模板。
\tparam _bSwapLR 水平翻转镜像（关于水平中轴对称）。
\tparam _bSwapUD 竖直翻转镜像（关于竖直中轴对称）。
\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _tIn 输入迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _fBlitLoop 循环操作类型。
\param loop 循环操作。
\param dst 目标迭代器。
\param ds 目标迭代器所在缓冲区大小。
\param src 源迭代器。
\param ss 源迭代器所在缓冲区大小。
\param dp 目标迭代器起始点所在缓冲区偏移。
\param sp 源迭代器起始点所在缓冲区偏移。
\param sc 源迭代器需要复制的区域大小。
\sa BlitScan
\since build 437

对一块矩形区域逐像素顺序批量操作（如复制或贴图）。
*/
template<bool _bSwapLR, bool _bSwapUD, typename _tOut, typename _tIn,
	typename _fBlitLoop>
void
Blit(_fBlitLoop loop, _tOut dst, _tIn src, const Size& ds, const Size& ss,
	const Point& dp, const Point& sp, const Size& sc)
{
	SDst min_x, min_y, delta_x, delta_y;

	if(BlitBounds(dp, sp, ds, ss, sc, min_x, min_y, delta_x, delta_y))
		BlitScan<_bSwapLR != _bSwapUD>(loop, dst + BlitScale<_bSwapLR,
			_bSwapUD>(dp, ds, delta_x, delta_y), src + (sp.Y + min_y) * ss.Width
			+ sp.X + min_x, ds.Width, ss.Width, delta_x, delta_y);
}


/*!
\ingroup BlitLoop
\brief 贴图扫描线循环操作。
\sa BlitScan
\since build 437
*/
template<bool _bPositiveScan>
struct BlitScannerLoop
{
	//! \since build 440
	template<typename _tOut, typename _tIn, typename _fBlitScanner>
	void
	operator()(_fBlitScanner scanner, _tOut dst_iter, _tIn src_iter,
		SDst delta_x, SDst delta_y, SDst dst_inc, SDst src_inc) const
	{
		while(delta_y-- > 0)
		{
			scanner(dst_iter, src_iter, delta_x);
			src_iter += src_inc;
			ystdex::delta_assign<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
};


/*!
\brief 扫描线贴图函数模板。
\tparam _bSwapLR 水平翻转镜像（关于水平中轴对称）。
\tparam _bSwapUD 竖直翻转镜像（关于竖直中轴对称）。
\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _tIn 输入迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _fBlitScanner 扫描线操作类型。
\param scanner 扫描线操作。
\param dst 目标迭代器。
\param ds 目标迭代器所在缓冲区大小。
\param src 源迭代器。
\param ss 源迭代器所在缓冲区大小。
\param dp 目标迭代器起始点所在缓冲区偏移。
\param sp 源迭代器起始点所在缓冲区偏移。
\param sc 源迭代器需要复制的区域大小。
\sa Blit
\sa BlitScannerLoop
\since build 437

对一块矩形区域逐（水平）扫描线批量操作（如复制或贴图）。
*/
template<bool _bSwapLR, bool _bSwapUD, typename _tOut, typename _tIn,
	typename _fBlitScanner>
void
BlitLines(_fBlitScanner scanner, _tOut dst, _tIn src, const Size& ds,
	const Size& ss, const Point& dp, const Point& sp, const Size& sc)
{
	using namespace std::placeholders;

	Blit<_bSwapLR, _bSwapUD, _tOut, _tIn>(std::bind(BlitScannerLoop<!_bSwapLR>(
		), scanner, _1, _2, _3, _4, _5, _6), dst, src, ds, ss, dp, sp, sc);
}


/*!
\ingroup BlitLineScanner
\brief 贴图扫描点循环操作。
\tparam _bPositiveScan 正向扫描。
\warning 不检查迭代器有效性。
\since build 440
*/
template<bool _bPositiveScan>
struct BlitLineLoop
{
	template<typename _tOut, typename _tIn, typename _fBlit>
	void
	operator()(_fBlit blit, _tOut& dst_iter, _tIn& src_iter, SDst delta_x)
	{
		for(SDst x(0); x < delta_x; ++x)
		{
			blit(dst_iter, src_iter);
			++src_iter;
			ystdex::xcrease<_bPositiveScan>(dst_iter);
		}
	}
};


/*!
\brief 像素贴图函数模板。
\tparam _bSwapLR 水平翻转镜像（关于水平中轴对称）。
\tparam _bSwapUD 竖直翻转镜像（关于竖直中轴对称）。
\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _tIn 输入迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _fBlit 像素操作类型。
\param blit 像素操作。
\param dst 目标迭代器。
\param ds 目标迭代器所在缓冲区大小。
\param src 源迭代器。
\param ss 源迭代器所在缓冲区大小。
\param dp 目标迭代器起始点所在缓冲区偏移。
\param sp 源迭代器起始点所在缓冲区偏移。
\param sc 源迭代器需要复制的区域大小。
\sa Blit
\sa BlitScannerLoop
\sa BlitLineLoop
\since build 440

对一块矩形区域逐（水平）扫描线批量操作（如复制或贴图）。
*/
template<bool _bSwapLR, bool _bSwapUD, typename _tOut, typename _tIn,
	typename _fBlit>
void
BlitPixels(_fBlit blit, _tOut dst, _tIn src, const Size& ds,
	const Size& ss, const Point& dp, const Point& sp, const Size& sc)
{
	BlitLines<_bSwapLR, _bSwapUD, _tOut, _tIn>(
		[blit](_tOut& dst_iter, _tIn& src_iter, SDst delta_x){
			BlitLineLoop<!_bSwapLR>()(blit, dst_iter, src_iter, delta_x);
		}, dst, src, ds, ss, dp, sp, sc);
}


/*!
\brief 标准矩形转换器。
\since build 266
*/
struct RectTransformer
{
	/*!
	\brief 渲染标准矩形内的像素。
	\tparam _tOut 输出迭代器类型（需要支持 += 操作，一般应是随机迭代器）。
	\note 不含右边界和下边界。
	\since build 438
	*/
	//@{
	template<typename _tOut, class _fTransformPixel, class _fTransformLine>
	void
	operator()(_tOut dst, const Size& ds, const Point& dp, const Size& ss,
		_fTransformPixel tp, _fTransformLine tl)
	{
		SDst min_x, min_y, delta_x, delta_y;

		if(BlitBounds(dp, Point(), ds, ss, ss, min_x, min_y, delta_x, delta_y))
		{
			dst += max<SPos>(0, dp.Y) * ds.Width + max<SPos>(0, dp.X);
			while(delta_y-- > 0)
			{
				tl(dst, delta_x, tp);
				dst += ds.Width;
			}
		}
	}
	template<typename _tOut, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tOut dst, const Size& ds, const Rect& rSrc,
		_fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tOut, _fTransformPixel, _fTransformLine>(dst, ds,
			rSrc.GetPoint(), rSrc.GetSize(), tp, tl);
	}
	template<typename _tOut, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tOut dst, SDst dw, SDst dh, SPos dx, SPos dy,
		SDst sw, SDst sh, _fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tOut, _fTransformPixel, _fTransformLine>(
			dst, Size(dw, dh), Point(dx, dy), Size(sw, sh), tp, tl);
	}
	//@}
};


/*
\brief 显示缓存操作：清除/以纯色像素填充。
\tparam _tOut 输出迭代器类型（需要支持 += 操作，一般应是随机迭代器）。
\since build 438
*/
//@{
/*!
\brief 清除指定位置的 n 个连续像素。
\tparam _tOut 输出迭代器类型。
\sa ClearSequence
*/
template<typename _tOut>
inline _tOut
ClearPixel(_tOut dst, size_t n) ynothrow
{
	ClearSequence(dst, n);
	return dst;
}

/*!
\brief 使用 n 个指定像素连续填充指定位置。
*/
template<typename _tPixel, typename _tOut>
inline void
FillPixel(_tOut dst, size_t n, _tPixel c)
{
	SequenceTransformer()(dst, n, PixelFiller<_tPixel>(c));
}

/*!
\brief 使用 n 个指定像素竖直填充指定位置。
*/
template<typename _tPixel, typename _tOut>
inline void
FillVerticalLine(_tOut dst, size_t n, SDst dw, _tPixel c)
{
	VerticalLineTransfomer()(dst, n, dw, PixelFiller<_tPixel>(c));
}

/*!
\brief 使用指定像素填充指定的标准矩形区域。
*/
template<typename _tPixel, typename _tOut>
inline void
FillRect(_tOut dst, const Size& ds, const Point& sp, const Size& ss,
	_tPixel c)
{
	RectTransformer()(dst, ds, sp, ss, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
/*!
\brief 使用指定像素填充指定的标准矩形区域。
*/
template<typename _tPixel, typename _tOut>
inline void
FillRect(_tOut dst, const Size& ds, const Rect& rSrc, _tPixel c)
{
	RectTransformer()(dst, ds, rSrc, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
/*!
\brief 使用指定像素填充指定的标准矩形区域。
*/
template<typename _tPixel, typename _tOut>
inline void
FillRect(_tOut dst, SDst dw, SDst dh, SPos sx, SPos sy, SDst sw, SDst sh,
	_tPixel c)
{
	RectTransformer()(dst, dw, dh, sx, sy, sw, sh, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
//@}


/*!
\ingroup BlitScanner
\brief 扫描线：按指定扫描顺序复制一行像素。
\warning 不检查迭代器有效性。
\since build 438
*/
//@{
template<bool _bPositiveScan>
struct CopyLine
{
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut& dst_iter, _tIn& src_iter, SDst delta_x) const
	{
		std::copy_n(src_iter, delta_x, dst_iter);
		yunseq(src_iter += delta_x, dst_iter += delta_x);
	}
};

//! \todo 增加对不支持前置 -- 操作的迭代器的支持。
template<>
struct CopyLine<false>
{
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut& dst_iter, _tIn& src_iter, SDst delta_x) const
	{
		while(delta_x-- > 0)
			*dst_iter-- = *src_iter++;
	}
};
//@}


/*!
\brief Alpha 光栅化源迭代器对。
\since build 417
*/
using IteratorPair = ystdex::pair_iterator<ConstBitmapPtr, const AlphaType*>;


/*!
\ingroup PixelOperation
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
	static yconstexpr ystdex::conditional_t<(_vDstAlphaBits < _vSrcAlphaBits),
		_tSrcAlpha, _tDstAlpha>
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha sa)
	{
		return sa + da * (1 - sa);
	}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	a != 0
		=> c := (sa * s + (1 - sa) * da * a) / a
		= (sa * s + (a - sa) * d) / a
		= (sa * s + a * d - sa * d) / a
		= (sa * (s - d) + a * d) / a
		= sa * (s - d) / a + d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstexpr _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha a)
	{
		return a != 0 ? (s < d ? _tDst(d - sa * (d - s) / a)
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
	static yconstexpr _tDstAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha sa)
	{
		return sa != 0 ? ystdex::normalized_max<_tDstAlpha>::value : da;
	}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	a != 0
		=> c := sa * (s - d) / a + d
		= sa != 0 ? (s - d) / a + d : d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstexpr _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha a)
	{
		return a != 0 ? (sa != 0 ? (s < d ? _tDst(d - (d - s) / a)
			: _tDst((s - d) / a + d)) : d) : _tDst(0);
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
	template<typename _tDstAlpha>
	static yconstexpr _tDstAlpha
	CompositeAlphaOver(_tDstAlpha da)
	{
		return ystdex::normalized_max<_tDstAlpha>::value;
	}
	template<typename _tDstAlpha, typename _tSrcAlpha>
	static yconstexpr _tDstAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha)
	{
		return ystdex::normalized_max<_tDstAlpha>::value;
	}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	sa := 1
		=> a := 1
		=> c := sa * (s - d) / a + d
		= s - d + d
		= s
	*/
	template<typename _tSrc>
	static yconstexpr _tSrc
	CompositeComponentOver(_tSrc s)
	{
		return s;
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstexpr _tSrc
	CompositeComponentOver(_tDst, _tSrc s, _tSrcAlpha, _tAlpha)
	{
		return s;
	}
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
	static yconstexpr _tSrcAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha sa)
	{
		return da != 0 ? ystdex::normalized_max<_tSrcAlpha>::value : sa;
	}


	/*!
	\brief Alpha 组合非 Alpha 分量。

	a != 0
		=> c := sa * (s - d) / a + d
		= sa * (s - d) + d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstexpr _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha a)
	{
		return a != 0 ? GPixelCompositor<0, _vSrcAlphaBits>
			::CompositeComponentOver(d, s, sa) : _tDst(0);
	}
};

//! \note 不透明目标。
template<size_t _vSrcAlphaBits>
struct GPixelCompositor<0, _vSrcAlphaBits>
{
	/*!
	\brief Alpha 组合 Alpha 分量。

	da = 1
		=> a := sa + da * (1 - sa)
		= 1
	*/
	template<typename _tDstAlpha>
	static yconstexpr _tDstAlpha
	CompositeAlphaOver(_tDstAlpha)
	{
		return ystdex::normalized_max<_tDstAlpha>::value;
	}
	template<typename _tDstAlpha, typename _tSrcAlpha>
	static yconstexpr _tDstAlpha
	CompositeAlphaOver(_tDstAlpha, _tSrcAlpha)
	{
		return ystdex::normalized_max<_tDstAlpha>::value;
	}

	/*!
	\brief Alpha 组合非 Alpha 分量。

	da = 1
		=> a := 1
		=> c := sa * (s - d) / a + d
		= sa * (s - d) + d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha>
	static yconstexpr _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa)
	{
		return s < d ? _tDst(d - sa * (d - s)) : _tDst(sa * (s - d) + d);
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstexpr _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha)
	{
		return CompositeComponentOver(d, s, sa);
	}
};

//! \note 不透明目标和 1 位源 Alpha 。
template<>
struct GPixelCompositor<0, 1> : public GPixelCompositor<0, 2>
{
	using GPixelCompositor<0, 2>::CompositeAlphaOver;

	/*!
	\brief Alpha 组合非 Alpha 分量。

	da = 1
		=> a := 1
		=> c := sa * (s - d) / a + d
		= sa * (s - d) + d
		= sa != 0 ? s : d
	*/
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha>
	static yconstexpr _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa)
	{
		return sa != 0 ? _tDst(s) : _tDst(d);
	}
	template<typename _tDst, typename _tSrc, typename _tSrcAlpha,
		typename _tAlpha>
	static yconstexpr _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha)
	{
		return CompositeComponentOver(d, s, sa);
	}
};

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
	static yconstexpr _tDstAlpha
	CompositeAlphaOver(_tDstAlpha da, _tSrcAlpha sa)
	{
		return sa != 0 || da != 0 ? ystdex::normalized_max<_tDstAlpha>::value
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
	static yconstexpr _tDst
	CompositeComponentOver(_tDst d, _tSrc s, _tSrcAlpha sa, _tAlpha a)
	{
		return a != 0 ? GPixelCompositor<0, 1>::CompositeComponentOver(d, s, sa)
			: _tDst(0);
	}
};

//! \note 不透明源和目标。
template<>
struct GPixelCompositor<0, 0> : public GPixelCompositor<0, 2>,
	public GPixelCompositor<2, 0>
{
	using GPixelCompositor<0, 2>::CompositeAlphaOver;
	using GPixelCompositor<2, 0>::CompositeComponentOver;
};
//@}


/*!
\brief 像素混合器。
\tparam _tPixel 像素类型。
\tparam _vAlphaBits Alpha 宽度。
\since build 439
\todo 支持浮点 Alpha 类型。
*/
//@{
template<typename _tPixel, size_t _vAlphaBits>
struct GPixelBlender
{
	static yconstexpr size_t AlphaBits = _vAlphaBits;

	//! \brief Alpha 分量混合。
	template<typename _tMono, typename _tAlpha>
	static yconstfn _tMono
	BlendComponent(_tMono d, _tMono s, _tAlpha a)
	{
		using namespace ystdex;

		return GPixelCompositor<1, 8>::CompositeComponentOver(d, s,
			fixed_point<u16, 8>(a, raw_tag()),
			normalized_max<fixed_point<u16, 8>>::get());
	//	return d + ((a * (s - d)) >> AlphaBits);
	}

	/*!
	\brief Alpha 混合。

	背景透明，输出 Alpha 饱和。
	*/
	template<typename _tAlpha>
	static _tPixel
	Blend(_tPixel d, _tPixel s, _tAlpha a)
	{
		const Color dc(d), sc(s);

		return Color(BlendComponent(dc.GetR(), sc.GetR(), a),
			BlendComponent(dc.GetG(), sc.GetG(), a),
			BlendComponent(dc.GetB(), sc.GetB(), a), (1 << AlphaBits) - 1);
	}
};

//! \since build 441
template<size_t _vAlphaBits>
struct GPixelBlender<RGBA<5, 5, 5, 1>, _vAlphaBits>
{
	static yconstexpr size_t AlphaBits = _vAlphaBits;

	template<typename _tAlpha>
	static RGBA<5, 5, 5, 1>
	Blend(RGBA<5, 5, 5, 1> d, RGBA<5, 5, 5, 1> s, _tAlpha a)
	{
		return BlendCore(d.Integer, s.Integer, a);
	}

	/*!
	\brief AXYZ1555 格式 PixelType 的 Alpha 混合。
	\since build 417

	设 MaxAlpha := (1 << AlphaBits) - 1 ，
	使用下列公式进行像素的 Alpha 混合（其中 alpha = a / MaxAlpha）：
	输出分量： component := (1 - alpha) * d + alpha * s
	= ((MaxAlpha - a) * d + a * s) >> AlphaBits
	= d + ((a * (s - d)) >> AlphaBits) 。
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
	static u16
	BlendCore(u32 d, u32 s, u8 a)
	{
		u32 dbr((d & 0x1F) | (d << 6 & 0x1F0000)), dg(d & 0x3E0);

		yunseq(dbr += ((((s & 0x1F) | (s << 6 & 0x1F0000)) - dbr) * a)
			>> AlphaBits,
			dg += (((s & 0x3E0) - dg) * a) >> AlphaBits);
		return (dbr & 0x1F) | (dg & 0x3E0) | (dbr >> 6 & 0x7C00) | 1 << 15;
	}
};
//@}


/*!
\ingroup PixelOperation
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
		using Blender = GPixelBlender<PixelType, 8>;

		const AlphaType a(*src_iter.base().second);

		*dst_iter = Blender::Blend(*dst_iter, *src_iter, a);
	}
	template<typename _tOut, typename _tIn>
	inline void
	operator()(_tOut dst_iter, ystdex::pair_iterator<
		ystdex::pseudo_iterator<const PixelType>, _tIn> src_iter)
	{
		static_assert(std::is_convertible<ystdex::remove_reference_t<
			decltype(*dst_iter)>, PixelType>::value, "Wrong type found.");
		using Blender = GPixelBlender<PixelType, 8>;

		const AlphaType a(*src_iter.base().second);

		*dst_iter = Blender::Blend(*dst_iter, *src_iter, a);
	}
};


/*!
\brief 以像素为单位变换标准矩形。
\since build 166
*/
template<class _fTransformPixel>
bool
TransformRect(const Graphics& g, const Point& pt, const Size& s,
	_fTransformPixel tp)
{
	if(YB_LIKELY(g))
	{
		RectTransformer()(g.GetBufferPtr(), g.GetSize(), pt, s, tp,
			SequenceTransformer());
		return true;
	}
	return false;
}
/*!
\brief 以像素为单位变换标准矩形。
\since build 166
*/
template<class _fTransformPixel>
inline bool
TransformRect(const Graphics& g, const Rect& r, _fTransformPixel tp)
{
	return TransformRect<_fTransformPixel>(g, r.GetPoint(), r.GetSize(), tp);
}


/*!
\brief 以第一个参数作为目标，复制第二个参数的缓冲区内容。
\pre 断言：指针非空；大小相等。
\note 缓冲区指针相等时忽略。
\since build 177
*/
YF_API void
CopyBuffer(const Graphics&, const Graphics&);

/*!
\brief 清除图形接口上下文缓冲区。
\since 早于 build 132
*/
YF_API void
ClearImage(const Graphics&);

/*!
\brief 使用指定颜色填充图形接口上下文缓冲区。
\since build 177
*/
YF_API void
Fill(const Graphics&, Color);

} // namespace Drawing;

} // namespace YSLib;

#endif

