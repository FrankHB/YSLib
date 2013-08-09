/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yblit.h
\ingroup Service
\brief 平台无关的图像块操作。
\version r1544
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:43:24 +0800
\par 修改时间:
	2013-08-05 21:02 +0800
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

namespace YSLib
{

namespace Drawing
{

/*!
\brief Alpha 光栅化源迭代器对。
\since build 417
*/
using IteratorPair = ystdex::pair_iterator<ConstBitmapPtr, const AlphaType*>;

/*!
\brief Alpha 单色光栅化源迭代器对。
\since build 417
*/
using MonoIteratorPair = ystdex::pair_iterator<
	ystdex::pseudo_iterator<const PixelType>, const AlphaType*>;


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
	*/
	inline void
	operator()(_tPixel* dst)
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
	*/
	template<typename _tPixel, class _fTransformPixel>
	void
	operator()(_tPixel* dst, size_t n, _fTransformPixel tp)
	{
		if(YB_LIKELY(dst && n))
		{
			_tPixel* p(dst + n);

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
	*/
	template<typename _tPixel, class _fTransformPixel>
	void
	operator()(_tPixel* dst, size_t n, SDst dw, _fTransformPixel tp)
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
\since build 189
*/
YF_API bool
BlitBounds(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& cs,
	int& min_x, int& min_y, int& delta_x, int& delta_y);

/*!
\brief 贴图偏移量计算器。
\since build 315
*/
//@{
template<bool _bSwapLR, bool _bSwapUD>
int
BlitScale(const Point& dp, const Size& ds, int delta_x, int delta_y);
template<>
inline int
BlitScale<false, false>(const Point& dp, const Size& ds, int, int)
{
	return max<int>(0, dp.Y) * ds.Width + max<int>(0, dp.X);
}
template<>
inline int
BlitScale<true, false>(const Point& dp, const Size& ds, int, int delta_y)
{
	return (max<int>(0, dp.Y) + delta_y - 1) * ds.Width + max<int>(0, dp.X);
}
template<>
inline int
BlitScale<false, true>(const Point& dp, const Size& ds, int delta_x, int)
{
	return max<int>(0, dp.Y) * ds.Width + max<int>(0, dp.X) + delta_x - 1;
}
template<>
inline int
BlitScale<true, true>(const Point& dp, const Size& ds, int delta_x, int delta_y)
{
	return (max<int>(0, dp.Y) + delta_y - 1) * ds.Width
		+ max<int>(0, dp.X) + delta_x - 1;
}
//@}


/*!
\brief 贴图扫描函数模板。
\tparam _bDec 指定是否翻转水平和竖直方向之一。
\tparam _fCallable 调用操作类型。
\tparam _tScalar 度量大小的纯量类型。
\tparam _tDiff 度量偏移量的差值类型。
\tparam _tOut 输出迭代器类型。
\tparam _tIn 输入迭代器类型。
\param f 操作对象。
\param d_width 目标缓冲区的宽。
\param s_width 源缓冲区的宽。
\param delta_x 实际操作的宽。
\param delta_y 实际操作的高。
\param dst 目标迭代器。
\param src 源迭代器。
\since build 370

对一块矩形区域调用指定的逐像素扫描操作。
*/
template<bool _bDec, typename _fCallable, typename _tScalar, typename _tDiff,
	typename _tOut, typename _tIn>
void
BlitScan(_fCallable&& f, _tScalar d_width, _tScalar s_width,
	_tDiff delta_x, _tDiff delta_y, _tOut dst, _tIn src)
{
	yforward(f)(delta_x, delta_y, dst, src,
		(_bDec ? -1 : 1) * d_width - delta_x, s_width - delta_x);
}

/*!
\brief 贴图函数模板。
\tparam _gBlitLoop 循环实现类模板。
\tparam _bSwapLR 水平翻转镜像（关于水平中轴对称）。
\tparam _bSwapUD 竖直翻转镜像（关于竖直中轴对称）。
\tparam _tOut 输出迭代器类型。
\tparam _tIn 输入迭代器类型。
\param dst 目标迭代器。
\param ds 目标迭代器所在缓冲区大小。
\param src 源迭代器。
\param ss 源迭代器所在缓冲区大小。
\param dp 目标迭代器起始点所在缓冲区偏移。
\param sp 源迭代器起始点所在缓冲区偏移。
\param sc 源迭代器需要复制的区域大小。
\since build 182

对一块矩形区域逐像素顺序批量操作（如复制或贴图）。
*/
template<template<bool> class _gBlitLoop, bool _bSwapLR, bool _bSwapUD,
	typename _tOut, typename _tIn>
void
Blit(_tOut dst, const Size& ds, _tIn src, const Size& ss,
	const Point& dp, const Point& sp, const Size& sc)
{
	int min_x, min_y, delta_x, delta_y;

	if(YB_LIKELY(BlitBounds(dp, sp, ds, ss, sc, min_x, min_y,
		delta_x, delta_y)))
		BlitScan<_bSwapLR != _bSwapUD>(_gBlitLoop<!_bSwapLR>(), ds.Width,
			ss.Width, delta_x, delta_y, dst + BlitScale<_bSwapLR, _bSwapUD>(dp,
			ds, delta_x, delta_y), src + (sp.Y + min_y) * ss.Width
			+ sp.X + min_x);
}


/*!
\brief 标准矩形转换器。
\since build 266
*/
struct RectTransformer
{
	/*!
	\brief 渲染标准矩形内的像素。
	\note 不含右边界和下边界。
	*/
	template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
	void
	operator()(_tPixel* dst, const Size& ds, const Point& dp, const Size& ss,
		_fTransformPixel tp, _fTransformLine tl)
	{
		int min_x, min_y, delta_x, delta_y;

		BlitBounds(dp, Point(), ds, ss, ss, min_x, min_y, delta_x, delta_y);
		dst += max<SPos>(0, dp.Y) * ds.Width + max<SPos>(0, dp.X);
		for(; delta_y > 0; --delta_y)
		{
			tl(dst, delta_x, tp);
			dst += ds.Width;
		}
	}
	/*!
	\brief 渲染标准矩形内的像素。
	\note 不含右边界和下边界。
	*/
	template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tPixel* dst, const Size& ds, const Rect& rSrc,
		_fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tPixel, _fTransformPixel,
			_fTransformLine>(dst, ds, rSrc.GetPoint(), rSrc.GetSize(), tp, tl);
	}
	/*!
	\brief 渲染标准矩形内的像素。
	\note 不含右边界和下边界。
	*/
	template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tPixel* dst, SDst dw, SDst dh, SPos dx, SPos dy,
		SDst sw, SDst sh, _fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tPixel, _fTransformPixel, _fTransformLine>(
			dst, Size(dw, dh), Point(dx, dy), Size(sw, sh), tp, tl);
	}
};


//显示缓存操作：清除/以纯色像素填充。

/*!
\brief 清除指定位置的 n 个连续像素。
\since build 322
*/
template<typename _tPixel>
inline _tPixel*
ClearPixel(_tPixel* dst, size_t n) ynothrow
{
	ClearSequence(dst, n);
	return dst;
}

/*!
\brief 使用 n 个指定像素连续填充指定位置。
\since 早于 build 132
*/
template<typename _tPixel>
inline void
FillPixel(_tPixel* dst, size_t n, _tPixel c)
{
	SequenceTransformer()(dst, n, PixelFiller<_tPixel>(c));
}

/*!
\brief 使用 n 个指定像素竖直填充指定位置。
\since build 182
*/
template<typename _tPixel>
inline void
FillVerticalLine(_tPixel* dst, size_t n, SDst dw, _tPixel c)
{
	VerticalLineTransfomer()(dst, n, dw, PixelFiller<_tPixel>(c));
}

/*!
\brief 使用指定像素填充指定的标准矩形区域。
\since build 160
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Point& sp, const Size& ss,
	_tPixel c)
{
	RectTransformer()(dst, ds, sp, ss, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
/*!
\brief 使用指定像素填充指定的标准矩形区域。
\since build 151
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Rect& rSrc, _tPixel c)
{
	RectTransformer()(dst, ds, rSrc, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
/*!
\brief 使用指定像素填充指定的标准矩形区域。
\since build 160
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, SDst dw, SDst dh, SPos sx, SPos sy, SDst sw, SDst sh,
	_tPixel c)
{
	RectTransformer()(dst, dw, dh, sx, sy, sw, sh, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}


//显示缓存操作：复制和贴图。

/*!
\brief 循环：按指定扫描顺序复制一行像素。
\note 不检查迭代器有效性。
\since build 189
*/
//@{
template<bool _bPositiveScan>
void
BlitLine(BitmapPtr& dst_iter, ConstBitmapPtr& src_iter, int delta_x)
{
	if(delta_x > 0)
	{
		std::copy_n(src_iter, delta_x, dst_iter);
		yunseq(src_iter += delta_x, dst_iter += delta_x);
	}
}
template<>
inline void
BlitLine<false>(BitmapPtr& dst_iter, ConstBitmapPtr& src_iter, int delta_x)
{
	for(; delta_x > 0; --delta_x)
		*dst_iter-- = *src_iter++;
}
//@}

/*!
\brief 循环：按指定扫描顺序复制一块矩形区域的像素。
\warning 不检查迭代器有效性。
\since build 189
*/
template<bool _bPositiveScan>
struct BlitLoop
{
	void
	operator()(int delta_x, int delta_y, BitmapPtr dst_iter,
		ConstBitmapPtr src_iter, int dst_inc, int src_inc)
	{
		for(; delta_y > 0; --delta_y)
		{
			BlitLine<_bPositiveScan>(dst_iter, src_iter, delta_x);
			src_iter += src_inc;
			ystdex::delta_assign<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
};

/*!
\brief 循环：按指定扫描顺序复制一块矩形区域的像素。
\note 复制透明性。
\warning 不检查迭代器有效性。
\since build 189
*/
template<bool _bPositiveScan>
struct BlitTransparentLoop
{
	//使用源迭代器对应像素的第 15 位表示透明性。
	void
	operator()(int delta_x, int delta_y, BitmapPtr dst_iter,
		ConstBitmapPtr src_iter, int dst_inc, int src_inc)
	{
		for(; delta_y > 0; --delta_y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				if(FetchAlpha(*src_iter))
					*dst_iter = *src_iter;
				++src_iter;
				ystdex::xcrease<_bPositiveScan>(dst_iter);
			}
			src_iter += src_inc;
			ystdex::delta_assign<_bPositiveScan>(dst_iter, dst_inc);
		}
	}

	//使用 Alpha 通道表示透明性。
	void
	operator()(int delta_x, int delta_y, BitmapPtr dst_iter,
		IteratorPair src_iter, int dst_inc, int src_inc)
	{
		for(; delta_y > 0; --delta_y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				*dst_iter = *src_iter.base().second & 0x80
					? FetchOpaque(*src_iter) : FetchOpaque(PixelType());
				++src_iter;
				ystdex::xcrease<_bPositiveScan>(dst_iter);
			}
			src_iter += src_inc;
			ystdex::delta_assign<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
};


//#define YSL_FAST_BLIT

#ifdef YSL_FAST_BLIT

//测试用，不使用 Alpha 混合的快速算法。

template<typename _tOut, typename _tIn>
void
biltAlphaPoint(_tOut dst_iter, _tIn src_iter);
template<>
inline void
biltAlphaPoint(PixelType* dst_iter, IteratorPair src_iter)
{
	if(*src_iter.base().second >= BLT_THRESHOLD2)
		*dst_iter = FetchOpaque(*src_iter);
}
template<>
inline void
biltAlphaPoint(PixelType* dst_iter, MonoIteratorPair src_iter)
{
	if(*src_iter.base().second >= BLT_THRESHOLD2)
		*dst_iter = FetchOpaque(*src_iter);
}

#else

//! \since build 417
yconstexpr AlphaType BLT_ALPHA_BITS(8);
yconstexpr u32 BLT_MAX_ALPHA((1 << BLT_ALPHA_BITS) - 1);
yconstexpr u32 BLT_ROUND(1 << (BLT_ALPHA_BITS - 1));
//! \since build 417
yconstexpr AlphaType BLT_THRESHOLD(8);
//! \since build 417
yconstexpr AlphaType BLT_THRESHOLD2(128);
yconstexpr u32 BLT_ROUND_BR(BLT_ROUND | BLT_ROUND << 16);

#	if YCL_PIXEL_FORMAT_XYZ555 & 0xAA000000

/*
\brief AXYZ1555 格式 PixelType 的 Alpha 混合。
\since build 417

使用下列公式进行像素的 Alpha 混合（其中 alpha = a / BLT_MAX_ALPHA）：
输出分量： dst := (1 - alpha) * d + alpha * s
= ((BLT_MAX_ALPHA - a) * d + a * s) >> BLT_ALPHA_BITS
= d + ((a * (s - d) + BLT_ROUND) >> BLT_ALPHA_BITS) 。
背景透明， 输出 Alpha 饱和。
*/
inline u16
blitAlphaBlend(u32 d, u32 s, AlphaType a)
{
	/*
	格式： 16 位 AXYZ1555 ，以 ARGB1555 为例。
	算法示意：
						 arrrrrgggggbbbbb
		0000000000arrrrr gggggbbbbb000000
		0000000000011111 0000000000011111
		00000000000rrrrr 00000000000bbbbb : dbr
		0000000000000000 000000ggggg00000 : dg
	分解分量至 32 位寄存器以减少总指令数。
	*/
	if(FetchAlpha(d) && a <= BLT_MAX_ALPHA - BLT_THRESHOLD)
	{
		u32 dbr((d & 0x1F) | (d << 6 & 0x1F0000)), dg(d & 0x3E0);

		dbr += ((((s & 0x1F) | (s << 6 & 0x1F0000)) - dbr) * a + BLT_ROUND_BR)
			>> BLT_ALPHA_BITS,
		dg  += (((s & 0x3E0) - dg) * a + BLT_ROUND) >> BLT_ALPHA_BITS;
		return FetchOpaque((dbr & 0x1F) | (dg & 0x3E0) | (dbr >> 6 & 0x7C00));
	}
	return a < BLT_THRESHOLD2 ? u16(d) : u16(FetchOpaque(s));
}

#	else

/*
\brief Alpha 分量混合。
\since build 415

输出分量： dst := (1 - alpha) * d + alpha * s
= ((BLT_MAX_ALPHA - a) * d + a * s) >> BLT_ALPHA_BITS
= d + ((a * (s - d) + BLT_ROUND) >> BLT_ALPHA_BITS) 。
*/
template<typename _tAlpha>
inline _tAlpha
component_blend(_tAlpha d, _tAlpha s, _tAlpha a)
{
	return d + ((a * (s - d) + BLT_ROUND) >> BLT_ALPHA_BITS);
}

/*
\brief Alpha 混合。
\since build 417

使用下列公式进行像素的 Alpha 混合（其中 alpha = a / BLT_MAX_ALPHA）：
背景透明， 输出 Alpha 饱和。
*/
inline PixelType
blitAlphaBlend(PixelType d, PixelType s, AlphaType a)
{
	if(FetchAlpha(d) && a <= BLT_MAX_ALPHA - BLT_THRESHOLD)
	{
		const Color dc(d), sc(s);

		return Color(component_blend(dc.GetR(), sc.GetR(), a),
			component_blend(dc.GetG(), sc.GetG(), a),
			component_blend(dc.GetB(), sc.GetB(), a), 0xFF);
	}
	return a < BLT_THRESHOLD2 ? d : FetchOpaque(s);
}

#endif

template<typename _tOut, typename _tIn>
void
biltAlphaPoint(_tOut dst_iter, _tIn src_iter);
template<>
inline void
biltAlphaPoint(PixelType* dst_iter, IteratorPair src_iter)
{
	const AlphaType a(*src_iter.base().second);

	if(a >= BLT_THRESHOLD)
		*dst_iter = blitAlphaBlend(*dst_iter, *src_iter, a);
}
//! \since build 414
template<typename _tIn>
inline void
biltAlphaPoint(PixelType* dst_iter, ystdex::pair_iterator<
	ystdex::pseudo_iterator<const PixelType>, _tIn> src_iter)
{
	const AlphaType a(*src_iter.base().second);

	if(a >= BLT_THRESHOLD)
		*dst_iter = blitAlphaBlend(*dst_iter, *src_iter, a);
}

#endif

/*!
\brief 循环：按指定扫描顺序复制一块矩形区域的像素。
\note 混合 Alpha 透明度。
\warning 不检查迭代器有效性。
\since build 189
*/
template<bool _bPositiveScan>
struct BlitBlendLoop
{
	//使用 Alpha 通道表示 8 位透明度。
	template<typename _tIn>
	void
	operator()(int delta_x, int delta_y, BitmapPtr dst_iter, _tIn src_iter,
		int dst_inc, int src_inc)
	{
		for(; delta_y > 0; --delta_y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				biltAlphaPoint(dst_iter, src_iter);
				++src_iter;
				ystdex::xcrease<_bPositiveScan>(dst_iter);
			}
			src_iter += src_inc;
			ystdex::delta_assign<_bPositiveScan>(dst_iter, dst_inc);
		}
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

