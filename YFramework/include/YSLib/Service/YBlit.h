/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBlit.h
\ingroup Service
\brief 平台中立的图像块操作。
\version r3442
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:43:24 +0800
\par 修改时间:
	2015-03-29 12:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBlit
*/


#ifndef YSL_INC_Service_YBlit_h_
#define YSL_INC_Service_YBlit_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGraphics
#include YFM_YSLib_Core_YCoreUtilities
#include <ystdex/iterator.hpp>
#include <ystdex/rational.hpp>

namespace YSLib
{

namespace Drawing
{

/*!
\brief 竖直线转换器。
\since build 182
*/
struct VerticalLineTransfomer
{
	/*!
	\brief 渲染竖直线上的像素。
	\tparam _tOut 输出迭代器类型（需要支持 += 操作，一般应是随机迭代器）。
	\tparam _tIn 输入迭代器类型。
	\pre 断言：对非零参数起始迭代器不能判定为不可解引用。
	\since build 583
	*/
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut dst_iter, size_t n, SDst dw, _tIn src_iter) const
	{
		using ystdex::is_undereferenceable;

		YAssert(n == 0 || !is_undereferenceable(dst_iter),
			"Invalid iterator found.");
		while(n-- != 0)
		{
			*dst_iter = *src_iter;
			yunseq(++src_iter, dst_iter += dw);
		}
	}
};


/*!
\brief 块传输边界计算器。
\return 是否存在合适边界。
\since build 438

按指定的目标区域位置、源区域位置、目标边界大小、源边界大小和块传输区域大小计算和最
终边界相关的值。前两个 SDst 参数总是赋值为源坐标系的边界左上最小值。当无合适边
界时后两个 SDst 参数不被修改，否则赋值为最终块传输区域的宽和高。
*/
YF_API bool
BlitBounds(const Point&, const Point&, const Size&, const Size&, const Size&,
	SDst&, SDst&, SDst&, SDst&);


/*!
\brief 块传输偏移分量计算器。
\since build 577
*/
template<bool _bDec>
yconstfn size_t
BlitScaleComponent(SPos d, SPos s, SDst delta)
{
	return size_t(max<SPos>(0, s < 0 ? d - s : d))
		+ size_t(_bDec ? delta - 1 : 0);
}


/*!
\brief 块传输扫描函数模板。
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
	loop(dst, src, delta_x, delta_y, _tDiff(_bDec ? -1 : 1) * _tDiff(d_width)
		- delta_x, _tDiff(s_width) - delta_x);
}


/*!	\defgroup BlitLoop Blit Loop Operations
\brief 块传输循环操作。
\since build 438
*/

/*!	\defgroup BlitLineScanner Blit Line Scanner Operations
\brief 块传输扫描线操作。
\since build 438
*/


/*!
\brief 块传输函数模板。
\tparam _bSwapLR 水平翻转镜像（关于水平中轴对称）。
\tparam _bSwapUD 竖直翻转镜像（关于竖直中轴对称）。
\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _tIn 输入迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _fBlitLoop 循环操作类型。
\param loop 循环操作。
\param dst 目标迭代器。
\param src 源迭代器。
\param ds 目标迭代器所在缓冲区大小。
\param ss 源迭代器所在缓冲区大小。
\param dp 目标迭代器起始点所在缓冲区偏移。
\param sp 源迭代器起始点所在缓冲区偏移。
\param sc 源迭代器需要复制的区域大小。
\note 允许坐标分量越界。越上界时由 BlitBounds 过滤，越下界时修正为 0 。
\sa BlitBounds
\sa BlitScaleComponent
\sa Drawing::BlitScan
\since build 437

对一块矩形区域逐像素顺序批量操作。
*/
template<bool _bSwapLR, bool _bSwapUD, typename _tOut, typename _tIn,
	typename _fBlitLoop>
void
Blit(_fBlitLoop loop, _tOut dst, _tIn src, const Size& ds, const Size& ss,
	const Point& dp, const Point& sp, const Size& sc)
{
	SDst min_x, min_y, delta_x, delta_y;

	// XXX: Conversion to 'difference_type' might be implementation-defined.
	if(BlitBounds(dp, sp, ds, ss, sc, min_x, min_y, delta_x, delta_y))
		BlitScan<_bSwapLR != _bSwapUD>(loop, dst + typename
			std::iterator_traits<_tOut>::difference_type(BlitScaleComponent<
			_bSwapUD>(dp.Y, sp.Y, delta_y) * ds.Width + BlitScaleComponent<
			_bSwapLR>(dp.X, sp.X, delta_x)), src + typename
			std::iterator_traits<_tIn>::difference_type((_bSwapUD ? ss.Height
			- delta_y - min_y : min_y) * ss.Width + (_bSwapLR ? ss.Width
			- delta_x - min_x : min_x)), ds.Width, ss.Width, delta_x, delta_y);
}


/*!
\ingroup BlitLoop
\brief 块传输扫描线循环操作。
\sa BlitScan
\since build 437
*/
template<bool _bDec>
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
			// NOTE: See $2015-02 @ %Documentation::Workflow::Annual2015.
			if(YB_LIKELY(delta_y != 0))
				yunseq(src_iter += src_inc,
					ystdex::delta_assign<_bDec>(dst_iter, dst_inc));
		}
	}
};


/*!
\brief 扫描线块传输函数模板。
\tparam _bSwapLR 水平翻转镜像（关于水平中轴对称）。
\tparam _bSwapUD 竖直翻转镜像（关于竖直中轴对称）。
\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _tIn 输入迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _fBlitScanner 扫描线操作类型。
\param scanner 扫描线操作。
\param dst 目标迭代器。
\param src 源迭代器。
\param ds 目标迭代器所在缓冲区大小。
\param ss 源迭代器所在缓冲区大小。
\param dp 目标迭代器起始点所在缓冲区偏移。
\param sp 源迭代器起始点所在缓冲区偏移。
\param sc 源迭代器需要复制的区域大小。
\sa Drawing::Blit
\sa BlitScannerLoop
\since build 437

对一块矩形区域逐（水平）扫描线批量操作。
*/
template<bool _bSwapLR, bool _bSwapUD, typename _tOut, typename _tIn,
	typename _fBlitScanner>
inline void
BlitLines(_fBlitScanner scanner, _tOut dst, _tIn src, const Size& ds,
	const Size& ss, const Point& dp, const Point& sp, const Size& sc)
{
	using namespace std::placeholders;

	Blit<_bSwapLR, _bSwapUD, _tOut, _tIn>(std::bind(BlitScannerLoop<!_bSwapLR>(
		), scanner, _1, _2, _3, _4, _5, _6), dst, src, ds, ss, dp, sp, sc);
}


/*!
\ingroup BlitLineScanner
\brief 块传输扫描点循环操作。
\tparam _bPositiveScan 正向扫描。
\warning 不检查迭代器有效性。
\since build 440
*/
template<bool _bDec>
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
			ystdex::xcrease<_bDec>(dst_iter);
		}
	}
};


/*!
\brief 像素块传输函数模板。
\tparam _bSwapLR 水平翻转镜像（关于水平中轴对称）。
\tparam _bSwapUD 竖直翻转镜像（关于竖直中轴对称）。
\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _tIn 输入迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _fPixelShader 像素着色器类型。
\param shader 像素着色器。
\param dst 目标迭代器。
\param src 源迭代器。
\param ds 目标迭代器所在缓冲区大小。
\param ss 源迭代器所在缓冲区大小。
\param dp 目标迭代器起始点所在缓冲区偏移。
\param sp 源迭代器起始点所在缓冲区偏移。
\param sc 源迭代器需要复制的区域大小。
\sa Drawing::Blit
\sa BlitScannerLoop
\sa BlitLineLoop
\since build 440

对一块矩形区域逐（水平）扫描线批量操作。
*/
template<bool _bSwapLR, bool _bSwapUD, typename _tOut, typename _tIn,
	typename _fPixelShader>
inline void
BlitPixels(_fPixelShader shader, _tOut dst, _tIn src, const Size& ds,
	const Size& ss, const Point& dp, const Point& sp, const Size& sc)
{
	BlitLines<_bSwapLR, _bSwapUD, _tOut, _tIn>(
		[shader](_tOut& dst_iter, _tIn& src_iter, SDst delta_x){
			BlitLineLoop<!_bSwapLR>()(shader, dst_iter, src_iter, delta_x);
		}, dst, src, ds, ss, dp, sp, sc);
}


/*!
\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\tparam _tIn 输入迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
\since build 584
*/
//@{
/*!
\brief 矩形块传输函数模板。
\tparam _func 处理函数对象类型。
\param f 处理函数。
\param dst 目标迭代器。
\param src 源迭代器。

对一块矩形区域按指定处理函数对象进行操作。
*/
//! \param ds 目标迭代器所在缓冲区大小。
//@{
//@{
/*!
\param dp 目标迭代器起始点所在缓冲区偏移。
\param sc 源迭代器需要复制的区域大小。
*/
//@{
/*!
\param ss 源迭代器所在缓冲区大小。
\param sp 源迭代器起始点所在缓冲区偏移。
*/
template<typename _func, typename _tOut, typename _tIn>
inline void
BlitRect(_func f, _tOut dst, _tIn src, const Size& ds, const Size& ss,
	const Point& dp, const Point& sp, const Size& sc)
{
	f(dst, src, ds, ss, dp, sp, sc);
}
template<typename _func, typename _tOut, typename _tIn>
inline void
BlitRect(_func f, _tOut dst, _tIn src, const Size& ds, const Point& sp,
	const Size& sc)
{
	Drawing::BlitRect(f, dst, src, ds, ds, sp, sp, sc);
}
//@}
//! \param r 源迭代器所在缓冲区边界。
template<typename _func, typename _tOut, typename _tIn>
inline void
BlitRect(_func f, _tOut dst, _tIn src, const Size& ds, const Rect& r)
{
	Drawing::BlitRect(f, dst, src, ds, r.GetPoint(), r.GetSize());
}
//@}
/*!
\param dw 目标迭代器所在缓冲区的宽。
\param dh 目标迭代器所在缓冲区的高。
\param x 源迭代器起始点所在缓冲区水平偏移。
\param y 源迭代器起始点所在缓冲区竖直偏移。
\param w 源迭代器需要复制的区域的宽。
\param h 源迭代器需要复制的区域的高。
*/
template<typename _func, typename _tOut, typename _tIn>
inline void
BlitRect(_func f, _tOut dst, _tIn src, SDst dw, SDst dh, SPos x, SPos y, SDst w,
	SDst h)
{
	Drawing::BlitRect(f, dst, src, {dw, dh}, {x, y}, {w, h});
}
//@}


/*!
\brief 根据指定批量操作（块传输扫描器或像素着色器）函数对象对矩形像素块进行批量操作。
\note 使用 ADL 调用 BlitRect 进行操作。
\sa BlitRect
*/
//@{
//! \sa Drawing::BlitLines
template<typename _tOut, typename _tIn, typename _fBlitScanner,
	typename... _tParams>
inline void
BlitRectLines(_fBlitScanner scanner, _tOut dst, _tIn src, _tParams&&... args)
{
	using namespace std::placeholders;

	BlitRect(std::bind(Drawing::BlitLines<false, false, _tOut, _tIn,
		_fBlitScanner>, scanner, _1, _2, _3, _4, _5, _6, _7), dst, src,
		yforward(args)...);
}

//! \sa Drawing::BlitPixels
template<typename _tOut, typename _tIn, typename _fPixelShader,
	typename... _tParams>
inline void
BlitRectPixels(_fPixelShader shader, _tOut dst, _tIn src, _tParams&&... args)
{
	using namespace std::placeholders;

	BlitRect(std::bind(Drawing::BlitPixels<false, false, _tOut, _tIn,
		_fPixelShader>, shader, _1, _2, _3, _4, _5, _6, _7), dst, src,
		yforward(args)...);
}
//@}
//@}


/*!
\brief 分派用于着色器更新的可能转置的图像。
\since build 579

第一个参数指定是否需要转置（原始图像旋转一个直角）。
第二个参数指定更新器，接受可能转置的原迭代器、可能转置的源图像大小和转置偏移位置。
未转置时，转置偏移位置为原点；否则使用 RotateCenter 指定。
*/
//@{
template<typename _func>
inline void
DispatchTranspose(std::false_type, _func updater, const ConstGraphics& src)
{
	updater(src.GetBufferPtr(), src.GetSize(), Point());
}
template<typename _func>
inline void
DispatchTranspose(std::true_type, _func updater, const ConstGraphics& src)
{
	updater(ystdex::make_transposed(src.GetBufferPtr(), src.GetWidth(),
		src.GetHeight(), 0U), Transpose(src.GetSize()),
		RotateCenter(src.GetSize()));
}
//@}


/*!
\ingroup BlitScanner
\brief 扫描线：按指定扫描顺序复制一行像素。
\warning 不检查迭代器有效性。
\since build 438
*/
//@{
template<bool _bDec>
struct CopyLine
{
	/*!
	\brief 复制迭代器指定的一行像素。
	\tparam _tOut 输出迭代器类型（需要支持 + 操作，一般应是随机迭代器）。
	\tparam _tIn 输入迭代器类型。
	\pre 断言：对非零参数起始迭代器不能判定为不可解引用。
	*/
	//@{
	//! \since build 453
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut& dst_iter, _tIn& src_iter, SDst delta_x) const
	{
		using ystdex::is_undereferenceable;

		YAssert(delta_x == 0 || !is_undereferenceable(dst_iter),
			"Invalid output iterator found."),
		YAssert(delta_x == 0 || !is_undereferenceable(src_iter),
			"Invalid input iterator found.");
		std::copy_n(src_iter, delta_x, dst_iter);
		// NOTE: Possible undefined behavior. See $2015-02
		//	@ %Documentation::Workflow::Annual2015.
		yunseq(src_iter += delta_x, dst_iter += delta_x);
	}
	//! \since build 583
	template<typename _tOut, typename _tPixel>
	void
	operator()(_tOut& dst_iter, ystdex::pseudo_iterator<_tPixel> src_iter,
		SDst delta_x) const
	{
		using ystdex::is_undereferenceable;

		YAssert(delta_x == 0 || !is_undereferenceable(dst_iter),
			"Invalid output iterator found."),
		std::fill_n(dst_iter, delta_x, Deref(src_iter));
		// NOTE: Possible undefined behavior. See $2015-02
		//	@ %Documentation::Workflow::Annual2015.
		dst_iter += delta_x;
	}
	//@}
};

template<>
struct CopyLine<false>
{
	/*!
	\todo 增加对不支持前置 -- 操作的迭代器的支持。
	\todo 平凡复制类型优化。
	*/
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut& dst_iter, _tIn& src_iter, SDst delta_x) const
	{
		while(delta_x-- > 0)
			*dst_iter-- = *src_iter++;
	}
};
//@}


/*
\brief 显示缓存操作：清除/以纯色像素填充。
\tparam _tOut 输出迭代器类型（需要支持 += 操作，一般应是随机迭代器）。
\since build 438
*/
//@{
/*!
\brief 清除指定位置的 n 个连续像素。
\tparam _tOut 输出迭代器类型。
\sa YSLib::ClearSequence
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
FillPixel(_tOut dst_iter, size_t n, _tPixel c)
{
	CopyLine<true>()(dst_iter, ystdex::pseudo_iterator<_tPixel>(c), SDst(n));
}

/*!
\brief 使用 n 个指定像素竖直填充指定位置。
*/
template<typename _tPixel, typename _tOut>
inline void
FillVerticalLine(_tOut dst, size_t n, SDst dw, _tPixel c)
{
	VerticalLineTransfomer()(dst, n, dw, ystdex::pseudo_iterator<_tPixel>(c));
}
//@}

/*!
\brief 使用指定像素填充指定的标准矩形区域。
\since build 584
*/
template<typename _tPixel, typename _tOut, typename... _tParams>
inline void
FillRectRaw(_tOut dst, _tPixel c, _tParams&&... args)
{
	Drawing::BlitRectLines(CopyLine<true>(), dst,
		ystdex::pseudo_iterator<_tPixel>(c), yforward(args)...);
}


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
		Drawing::BlitPixels<false, false>(
			[tp](BitmapPtr dst_iter, ConstBitmapPtr){
				tp(dst_iter);
			}, g.GetBufferPtr(), g.GetBufferPtr(), g.GetSize(), g.GetSize(), pt,
			pt, s);
		return true;
	}
	return {};
}
/*!
\brief 以像素为单位变换标准矩形。
\since build 166
*/
template<class _fTransformPixel>
inline bool
TransformRect(const Graphics& g, const Rect& r, _fTransformPixel tp)
{
	return Drawing::TransformRect<_fTransformPixel>(g, r.GetPoint(),
		r.GetSize(), tp);
}


/*!
\brief 以第一个参数作为目标，复制第二个参数的缓冲区内容。
\pre 断言：大小相等。
\pre 间接断言：指针非空。
\note 缓冲区指针相等时忽略。
\since build 559
*/
YF_API void
CopyBuffer(const Graphics&, const ConstGraphics&);

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

