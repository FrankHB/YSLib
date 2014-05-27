/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yblit.h
\ingroup Service
\brief 平台中立的图像块操作。
\version r3072
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:43:24 +0800
\par 修改时间:
	2014-05-23 09:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBlit
*/


#ifndef YSL_INC_Service_yblit_h_
#define YSL_INC_Service_yblit_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGDIBase
#include YFM_YSLib_Core_YCoreUtilities
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
	\warning 无访问检查。
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
	\pre 断言：对于非零参数起始迭代器不能判定为不可解引用。
	\since build 453
	*/
	template<typename _tOut, class _fTransformPixel>
	void
	operator()(_tOut dst, size_t n, _fTransformPixel tp) const
	{
		using ystdex::is_undereferenceable;

		YAssert(n == 0 || !is_undereferenceable(dst),
			"Invalid iterator found.");
		for(const auto e(dst + n); dst != e; ++dst)
			tp(dst);
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
	\pre 断言：对于非零参数起始迭代器不能判定为不可解引用。
	\since build 453
	*/
	template<typename _tOut, class _fTransformPixel>
	void
	operator()(_tOut dst, size_t n, SDst dw, _fTransformPixel tp) const
	{
		using ystdex::is_undereferenceable;

		YAssert(n == 0 || !is_undereferenceable(dst),
			"Invalid iterator found.");

		while(n-- != 0)
		{
			tp(dst);
			dst += dw;
		}
	}
};


/*!
\brief 贴图边界计算器。
\return 是否存在合适边界。
\since build 438

按指定的目标区域位置、源区域位置、目标边界大小、源边界大小和贴图区域大小计算和最
终边界相关的值。前两个 SDst 参数总是赋值为源坐标系的边界左上最小值。当无合适边
界时后两个 SDst 参数不被修改，否则赋值为最终贴图区域的宽和高。
*/
YF_API bool
BlitBounds(const Point&, const Point&, const Size&, const Size&, const Size&,
	SDst&, SDst&, SDst&, SDst&);

/*!
\brief 贴图偏移分量计算器。
\since build 445
*/
//@{
template<bool>
yconstfn size_t
BlitScaleComponent(SPos d, SDst)
{
	return d < 0 ? 0 : d;
}
template<>
yconstfn size_t
BlitScaleComponent<true>(SPos d, SDst delta)
{
	return d < 0 ? 0 : d + delta - 1;
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
\warning loop 接受的最后两个参数应保证不是无符号整数类型。
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
\note 允许坐标分量越界。越上界时由 BlitBounds 过滤，越下界时修正为 0 。
\sa BlitBounds
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
		BlitScan<_bSwapLR != _bSwapUD>(loop, dst + BlitScaleComponent<_bSwapUD>(
			dp.Y - (sp.Y < 0 ? sp.Y : 0), delta_y) * ds.Width
			+ BlitScaleComponent<_bSwapLR>(dp.X - (sp.X < 0 ? sp.X : 0),
			delta_x), src + min_y * ss.Width + min_x, ds.Width, ss.Width,
			delta_x, delta_y);
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
	//! \since build 445
	template<typename _tOut, typename _tIn, typename _fBlitScanner>
	void
	operator()(_fBlitScanner scanner, _tOut dst_iter, _tIn src_iter,
		SDst delta_x, SDst delta_y, SPos dst_inc, SPos src_inc) const
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
	template<typename _tOut, typename _tIn, typename _fPixelShader>
	void
	operator()(_fPixelShader shader, _tOut& dst_iter, _tIn& src_iter,
		SDst delta_x)
	{
		for(SDst x(0); x < delta_x; ++x)
		{
			shader(dst_iter, src_iter);
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
\tparam _fPixelShader 像素着色器类型。
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
	typename _fPixelShader>
void
BlitPixels(_fPixelShader shader, _tOut dst, _tIn src, const Size& ds,
	const Size& ss, const Point& dp, const Point& sp, const Size& sc)
{
	BlitLines<_bSwapLR, _bSwapUD, _tOut, _tIn>(
		[shader](_tOut& dst_iter, _tIn& src_iter, SDst delta_x){
			BlitLineLoop<!_bSwapLR>()(shader, dst_iter, src_iter, delta_x);
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
	operator()(_tOut dst, const Size& ds, const Point& dp, const Size& sc,
		_fTransformPixel tp, _fTransformLine tl)
	{
		Blit<false, false>([&](_tOut dst_iter, _tOut, SDst delta_x,
			SDst delta_y, SPos dst_inc, SPos){
			while(delta_y-- > 0)
			{
				tl(dst_iter, delta_x, tp);
				dst_iter += dst_inc + delta_x;
			}
		}, dst, dst, ds, ds, dp, dp, sc);
	}
	template<typename _tOut, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tOut dst, const Size& ds, const Rect& r, _fTransformPixel tp,
		_fTransformLine tl)
	{
		operator()<_tOut, _fTransformPixel, _fTransformLine>(dst, ds,
			r.GetPoint(), r.GetSize(), tp, tl);
	}
	template<typename _tOut, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tOut dst, SDst dw, SDst dh, SPos x, SPos y,
		SDst w, SDst h, _fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tOut, _fTransformPixel, _fTransformLine>(dst, {dw, dh},
			{x, y}, {w, h}, tp, tl);
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
//@}

/*!
\brief 使用指定像素填充指定的标准矩形区域。
\since build 452
*/
//@{
template<typename _tPixel, typename _tOut>
inline void
FillRectRaw(_tOut dst, const Size& ds, const Point& sp, const Size& sc,
	_tPixel c)
{
	RectTransformer()(dst, ds, sp, sc, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
template<typename _tPixel, typename _tOut>
inline void
FillRectRaw(_tOut dst, const Size& ds, const Rect& r, _tPixel c)
{
	RectTransformer()(dst, ds, r, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
template<typename _tPixel, typename _tOut>
inline void
FillRectRaw(_tOut dst, SDst dw, SDst dh, SPos x, SPos y, SDst w, SDst h,
	_tPixel c)
{
	RectTransformer()(dst, dw, dh, x, y, w, h, PixelFiller<_tPixel>(c),
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

