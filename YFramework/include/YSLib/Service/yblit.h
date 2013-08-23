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
\version r2078
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:43:24 +0800
\par 修改时间:
	2013-08-23 17:27 +0800
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
	//! \since build 438
	template<typename _fBlitScanner, typename _tOut, typename _tIn>
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

	Blit<_bSwapLR, _bSwapUD, _tOut, _tIn>(std::bind(
		BlitScannerLoop<!_bSwapLR>(), scanner, _1, _2, _3, _4, _5, _6), dst,
		src, ds, ss, dp, sp, sc);
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
\brief Alpha 单色光栅化源迭代器对。
\since build 417
*/
using MonoIteratorPair = ystdex::pair_iterator<
	ystdex::pseudo_iterator<const PixelType>, const AlphaType*>;



/*!
\brief 像素迭代器透明操作。
\warning 不检查迭代器有效性。
\since build 437
*/
//@{
//! \note 使用源迭代器对应像素的第 15 位表示透明性。
template<typename _tOut, typename _tIn>
inline void
BlitTransparentPixel(_tOut& dst_iter, _tIn& src_iter)
{
	if(FetchAlpha(*src_iter))
		*dst_iter = *src_iter;
}
//! \note 使用 Alpha 通道表示透明性。
template<typename _tOut>
inline void
BlitTransparentPixel(_tOut& dst_iter, IteratorPair& src_iter)
{
	*dst_iter = *src_iter.base().second & 0x80 ? FetchOpaque(*src_iter)
		: FetchOpaque(PixelType());
}
//@}

/*!
\ingroup BlitScanner
\brief 扫描线：按指定扫描顺序复制一行透明像素。
\warning 不检查迭代器有效性。
\since build 437
*/
template<bool _bPositiveScan>
struct BlitTransparentLine
{
	//! \since build 438
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut& dst_iter, _tIn& src_iter, SDst delta_x)
	{
		for(SDst x(0); x < delta_x; ++x)
		{
			BlitTransparentPixel(dst_iter, src_iter);
			++src_iter;
			ystdex::xcrease<_bPositiveScan>(dst_iter);
		}
	}
};


/*!
\brief 像素混合器基本实现。
\tparam _tMono 分量类型。
\tparam _tAlpha Alpha 类型。
\since build 438
\todo 支持浮点 Alpha 类型。
*/
template<typename _tPixel, typename _tMono, typename _tAlpha>
struct GPixelBlenderBase
{
	static_assert(std::is_unsigned<_tAlpha>::value, "Invalid type found.");

	static yconstexpr size_t AlphaBits{sizeof(_tAlpha) * CHAR_BIT};
	static yconstexpr _tAlpha MaxAlpha{(1 << AlphaBits) - 1};
	static yconstexpr _tAlpha BlitRound{1 << (AlphaBits - 1)};

	template<class _tBlender>
	static _tPixel
	BlendBase(_tPixel d, _tPixel s, _tAlpha a)
	{
		return _tBlender::BlendCore(d, s, a);
	}

	/*!
	\brief Alpha 分量混合。

	设归一化 Alpha 值 alpha = a / MaxAlpha ，输出结果：
	result := (1 - alpha) * d + alpha * s
	= ((MaxAlpha - a) * d + a * s) >> AlphaBits
	= d + ((a * (s - d) + BlitRound) >> AlphaBits) 。
	*/
	static yconstfn _tMono
	BlendComponent(_tMono d, _tMono s, _tAlpha a)
	{
		return d + ((a * (s - d) + BlitRound) >> AlphaBits);
	}
};
//@}


/*!
\brief 像素混合器。
\sa GPixelBlenderBase
\since build 438
*/
//@{
template<typename _tPixel, typename _tMono, typename _tAlpha>
struct GPixelBlender : public GPixelBlenderBase<_tPixel, _tMono, _tAlpha>
{
	using Base = GPixelBlenderBase<_tPixel, _tMono, _tAlpha>;
	using Base::MaxAlpha;
	using Base::BlendComponent;

	static _tPixel
	Blend(_tPixel d, _tPixel s, _tAlpha a)
	{
		return Base::template BlendBase<GPixelBlender>(d, s, a);
	}

	/*
	\brief Alpha 混合。

	使用下列公式进行像素的 Alpha 混合（其中 alpha = a / MaxAlpha）：
	背景透明， 输出 Alpha 饱和。
	*/
	static _tPixel
	BlendCore(_tPixel d, _tPixel s, _tAlpha a)
	{
		const Color dc(d), sc(s);

		return Color(Base::BlendComponent(dc.GetR(), sc.GetR(), a),
			Base::BlendComponent(dc.GetG(), sc.GetG(), a),
			Base::BlendComponent(dc.GetB(), sc.GetB(), a), MaxAlpha);
	}
};

template<typename _tMono, typename _tAlpha>
struct GPixelBlender<u16, _tMono, _tAlpha>
	: public GPixelBlenderBase<u16, _tMono, _tAlpha>
{
	using Base = GPixelBlenderBase<u16, _tMono, _tAlpha>;
	using Base::AlphaBits;
	using Base::MaxAlpha;
	using Base::BlitRound;
	static yconstexpr u32 BlitRound_XZ{BlitRound | BlitRound << 16};

	static u16
	Blend(u16 d, u16 s, _tAlpha a)
	{
		return Base::template BlendBase<GPixelBlender>(d, s, a);
	}

	/*
	\brief AXYZ1555 格式 PixelType 的 Alpha 混合。
	\since build 417

	使用下列公式进行像素的 Alpha 混合（其中 alpha = a / MaxAlpha）：
	输出分量： component := (1 - alpha) * d + alpha * s
	= ((MaxAlpha - a) * d + a * s) >> AlphaBits
	= d + ((a * (s - d) + BlitRound) >> AlphaBits) 。
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
	BlendCore(u32 d, u32 s, _tAlpha a)
	{
		u32 dbr((d & 0x1F) | (d << 6 & 0x1F0000)), dg(d & 0x3E0);

		yunseq(dbr += ((((s & 0x1F) | (s << 6 & 0x1F0000)) - dbr) * a
			+ BlitRound_XZ) >> AlphaBits,
			dg += (((s & 0x3E0) - dg) * a + BlitRound) >> AlphaBits);
		return (dbr & 0x1F) | (dg & 0x3E0) | (dbr >> 6 & 0x7C00) | 1 << 15;
	}
};
//@}


/*!
\brief 像素计算：Alpha 混合。
\since build 438
*/
//@{
template<typename _tOut>
inline void
BiltAlphaPoint(_tOut dst_iter, IteratorPair src_iter)
{
	static_assert(std::is_convertible<typename std::remove_reference<
		decltype(*dst_iter)>::type, PixelType>::value, "Wrong type found.");
	using Blender = GPixelBlender<PixelType, MonoType, AlphaType>;

	const AlphaType a(*src_iter.base().second);

	*dst_iter = Blender::Blend(*dst_iter, *src_iter, a);
}
template<typename _tOut, typename _tIn>
inline void
BiltAlphaPoint(_tOut dst_iter, ystdex::pair_iterator<
	ystdex::pseudo_iterator<const PixelType>, _tIn> src_iter)
{
	static_assert(std::is_convertible<typename std::remove_reference<
		decltype(*dst_iter)>::type, PixelType>::value, "Wrong type found.");
	using Blender = GPixelBlender<PixelType, MonoType, AlphaType>;

	const AlphaType a(*src_iter.base().second);

	*dst_iter = Blender::Blend(*dst_iter, *src_iter, a);
}
//@}


/*!
\ingroup BlitScanner
\brief 扫描线：按指定扫描顺序复制一行透明像素。
\note 使用 Alpha 通道表示 8 位透明度。
\warning 不检查迭代器有效性。
\since build 437
*/
template<bool _bPositiveScan>
struct BlitBlendLine
{
	//! \since build 438
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut& dst_iter, _tIn& src_iter, SDst delta_x)
	{
		for(SDst x(0); x < delta_x; ++x)
		{
			BiltAlphaPoint(dst_iter, src_iter);
			++src_iter;
			ystdex::xcrease<_bPositiveScan>(dst_iter);
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

