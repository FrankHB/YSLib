/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.h
\ingroup UI
\brief 平台无关的图形设备接口。
\version 0.4018;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-14 18:29:46 +0800;
\par 修改时间:
	2011-06-16 13:58 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YGDI;
*/


#ifndef YSL_INC_SHELL_YGDI_H_
#define YSL_INC_SHELL_YGDI_H_

#include "ycutil.h"
#include "ygdibase.h"
#include "../Adaptor/yfont.h"

YSL_BEGIN

// GDI 基本数据类型和宏定义。

YSL_BEGIN_NAMESPACE(Drawing)

//! \brief 逆时针旋转角度指示输出指向。
typedef enum
{
	RDeg0 = 0,
	RDeg90 = 1,
	RDeg180 = 2,
	RDeg270 = 3
} Rotation;

//! \brief 二元方向。
typedef enum
{
	Horizontal = 0,
	Vertical = 1
} Orientation;


//! \brief Alpha 单色光栅化源迭代器对。
typedef ystdex::pair_iterator<ystdex::pseudo_iterator<const PixelType>,
	const u8*> MonoIteratorPair;

//! \brief Alpha 光栅化源迭代器对。
typedef ystdex::pair_iterator<ConstBitmapPtr, const u8*> IteratorPair;


//基本函数对象。

//! \brief 像素填充器。
template<typename _tPixel>
struct PixelFiller
{
	_tPixel Color;

	/*!
	\brief 构造：使用指定颜色。
	*/
	inline explicit
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

//! \brief 序列转换器。
struct SequenceTransformer
{
	/*!
	\brief 渲染连续像素。
	*/
	template<typename _tPixel, class _fTransformPixel>
	void
	operator()(_tPixel* dst, size_t n, _fTransformPixel tp)
	{
		if(dst && n)
		{
			_tPixel* p(dst + n);

			while(--p >= dst)
				tp(p);
		}
	}
};

//! \brief 竖直线转换器。
struct VerticalLineTransfomer
{
	/*!
	\brief 渲染竖直线上的像素。
	*/
	template<typename _tPixel, class _fTransformPixel>
	void
	operator()(_tPixel* dst, size_t n, SDst dw, _fTransformPixel tp)
	{
		if(dst && n)
			while(n--)
			{
				tp(dst);
				dst += dw;
			}
	}
};


//! \brief 贴图边界计算器。
bool
BlitBounds(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& cs,
	int& min_x, int& min_y, int& max_x, int& max_y);

//! \brief 贴图偏移量计算器。
template<bool _bSwapLR, bool _bSwapUD>
int
BlitScale(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& sc, int delta_x, int delta_y);
template<>
int
BlitScale<false, false>(const Point&, const Point&,
	const Size&, const Size&, const Size&, int, int);
template<>
int
BlitScale<true, false>(const Point&, const Point&,
	const Size&, const Size&, const Size&, int, int);
template<>
int
BlitScale<false, true>(const Point&, const Point&,
	const Size&, const Size&, const Size&, int, int);
template<>
int
BlitScale<true, true>(const Point&, const Point&,
	const Size&, const Size&, const Size&, int, int);


/*!
\brief 贴图函数模板。

对一块矩形区域的逐像素顺序批量操作（如复制或贴图）。
\tparam _gBlitLoop 循环实现模板类。
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
*/
template<template<bool> class _gBlitLoop, bool _bSwapLR, bool _bSwapUD,
	typename _tOut, typename _tIn>
void Blit(_tOut dst, const Size& ds,
	_tIn src, const Size& ss,
	const Point& dp, const Point& sp, const Size& sc)
{
	int min_x, min_y, max_x, max_y;

	if(BlitBounds(dp, sp, ds, ss, sc, min_x, min_y, max_x, max_y))
	{
		const int delta_x(max_x - min_x), delta_y(max_y - min_y),
			src_off(min_y * ss.Width + min_x),
			dst_off(BlitScale<_bSwapLR, _bSwapUD>(dp, sp, ds, ss, sc,
				delta_x, delta_y));

		_gBlitLoop<!_bSwapLR>()(delta_x, delta_y, dst + dst_off, src + src_off,
			(_bSwapLR ^ _bSwapUD ? -1 : 1) * ds.Width - delta_x,
			ss.Width - delta_x);
	}
}


//! \brief 正则矩形转换器。
struct RectTransfomer
{
	/*!
	\brief 渲染正则矩形内的像素。
	\note 不含右边界和下边界。
	*/
	template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
	void
	operator()(_tPixel* dst, const Size& ds, const Point& dp, const Size& ss,
		_fTransformPixel tp, _fTransformLine tl)
	{
		int min_x, min_y, max_x, max_y;

		BlitBounds(dp, Point::Zero, ds, ss, ss,
			min_x, min_y, max_x, max_y);

		const int delta_x(max_x - min_x);
		int delta_y(max_y - min_y);

		dst += ystdex::vmax<SPos>(0, dp.Y) * ds.Width
			+ ystdex::vmax<SPos>(0, dp.X);
		for(; delta_y > 0; --delta_y)
		{
			tl(dst, delta_x, tp);
			dst += ds.Width;
		}
	}
	/*!
	\brief 渲染正则矩形内的像素。
	\note 不含右边界和下边界。
	*/
	template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tPixel* dst, const Size& ds, const Rect& rSrc,
		_fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tPixel, _fTransformPixel,
			_fTransformLine>(dst, ds, rSrc, rSrc, tp, tl);
	}
	/*!
	\brief 渲染正则矩形内的像素。
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
*/
template<typename _tPixel>
inline void
ClearPixel(_tPixel* dst, size_t n)
{
	ClearSequence(dst, n);
}

/*!
\brief 使用 n 个指定像素连续填充指定位置。
*/
template<typename _tPixel>
inline void
FillPixel(_tPixel* dst, size_t n, _tPixel c)
{
	SequenceTransformer()(dst, n, PixelFiller<_tPixel>(c));
}

/*!
\brief 使用 n 个指定像素竖直填充指定位置。
*/
template<typename _tPixel>
inline void
FillVerticalLine(_tPixel* dst, size_t n, SDst dw, _tPixel c)
{
	VerticalLineTransfomer()(dst, n, dw, PixelFiller<_tPixel>(c));
}

/*!
\brief 使用指定像素填充指定的正则矩形区域。
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Point& sp, const Size& ss,
	_tPixel c)
{
	RectTransfomer()(dst, ds, sp, ss, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
/*!
\brief 使用指定像素填充指定的正则矩形区域。
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Rect& rSrc, _tPixel c)
{
	RectTransfomer()(dst, ds, rSrc, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}
/*!
\brief 使用指定像素填充指定的正则矩形区域。
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, SDst dw, SDst dh, SPos sx, SPos sy, SDst sw, SDst sh,
	_tPixel c)
{
	RectTransfomer()(dst, dw, dh, sx, sy, sw, sh, PixelFiller<_tPixel>(c),
		SequenceTransformer());
}


//显示缓存操作：复制和贴图。

/*!
\brief 循环：按指定扫描顺序复制一行像素。
\note 不检查迭代器有效性。
*/
template<bool _bPositiveScan>
void
BlitLine(BitmapPtr& dst_iter, ConstBitmapPtr& src_iter, int delta_x)
{
	if(delta_x > 0)
	{
		mmbcpy(dst_iter, src_iter, delta_x * sizeof(PixelType));
		src_iter += delta_x;
		dst_iter += delta_x;
	}
}
template<>
inline void
BlitLine<false>(BitmapPtr& dst_iter, ConstBitmapPtr& src_iter, int delta_x)
{
	for(; delta_x > 0; --delta_x)
		*dst_iter-- = *src_iter++;
}

/*!
\brief 循环：按指定扫描顺序复制一块矩形区域的像素。
\note 不检查迭代器有效性。
*/
template<bool _bPositiveScan>
struct BlitLoop
{
	void
	operator()(int delta_x, int delta_y,
		BitmapPtr dst_iter, ConstBitmapPtr src_iter,
		int dst_inc, int src_inc)
	{
		for(; delta_y > 0; --delta_y)
		{
			BlitLine<_bPositiveScan>(dst_iter, src_iter, delta_x);
			src_iter += src_inc;
			ystdex::delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
};

/*!
\brief 循环：按指定扫描顺序复制一块矩形区域的像素。
\note 不检查迭代器有效性。透明性复制。
*/
template<bool _bPositiveScan>
struct BlitTransparentLoop
{
	//使用源迭代器对应像素的第 15 位表示透明性。
	void
	operator()(int delta_x, int delta_y,
		BitmapPtr dst_iter, ConstBitmapPtr src_iter,
		int dst_inc, int src_inc)
	{
		for(; delta_y > 0; --delta_y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				if(*src_iter & BITALPHA)
				*dst_iter = *src_iter;
				++src_iter;
				ystdex::xcrease<_bPositiveScan>(dst_iter);
			}
			src_iter += src_inc;
			ystdex::delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
		}
	}

	//使用 Alpha 通道表示透明性。
	void
	operator()(int delta_x, int delta_y,
		BitmapPtr dst_iter, IteratorPair src_iter,
		int dst_inc, int src_inc)
	{
		for(; delta_y > 0; --delta_y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				*dst_iter = ((*src_iter.base().second & 0x80) ? *src_iter : 0)
					| BITALPHA;
				++src_iter;
				ystdex::xcrease<_bPositiveScan>(dst_iter);
			}
			src_iter += src_inc;
			ystdex::delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
};

const u8 BLT_ALPHA_BITS(8);
const u32 BLT_MAX_ALPHA((1 << BLT_ALPHA_BITS) - 1);
const u32 BLT_ROUND(1 << (BLT_ALPHA_BITS - 1));
const u8 BLT_THRESHOLD(8);
const u8 BLT_THRESHOLD2(128);

//#define YSL_FAST_BLIT

#ifdef YSL_FAST_BLIT

//测试用，不使用 Alpha 混合的快速算法。
inline void
biltAlphaPoint(BitmapPtr dst_iter, IteratorPair src_iter)
{
	if(*src_iter.base().second >= BLT_THRESHOLD2)
		*dst_iter = *src_iter | BITALPHA;
}

#else

// TODO: 消除具体像素格式依赖。
inline u16
blitAlphaBlend(u32 d, u32 s, u8 a)
{
	/*
	格式： 16 位 ARGB1555 。
	算法示意：
						arrrrrgggggbbbbb
		0000000000arrrrrgggggbbbbb000000
		00000000000111110000000000000000
		00000000000rrrrr0000000000000000
		00000000000rrrrr00000000000bbbbb : dbr
		0000000000000000000000ggggg00000 : dg
	分解红色和蓝色分量至 32 位寄存器以减少乘法次数。
	使用下列 Alpha 混合公式（其中 alpha = a / BLT_MAX_ALPHA）：
	*dst_iter = (1 - alpha) * d + alpha * s
	= ((BLT_MAX_ALPHA - a) * d + a * s) >> BLT_ALPHA_BITS
	= ((d << BLT_ALPHA_BITS) + BLT_ROUND + a * (s - d))
		>> BLT_ALPHA_BITS;
	可进一步近似为 d + ((a * (s - d)) >> BLT_ALPHA_BITS)，但有额外损失。
	*/
	if(d & BITALPHA && a <= BLT_MAX_ALPHA - BLT_THRESHOLD)
	{
		register u32 dbr((d & 0x1F) | (d << 6 & 0x1F0000)), dg(d & 0x3E0);

		dbr = (dbr + (((((s & 0x1F) | (s << 6 & 0x1F0000)) - dbr)
			* a + BLT_ROUND) >> BLT_ALPHA_BITS));
		dg  = (dg  + ((((s & 0x3E0) - dg) * a + BLT_ROUND)
			>> BLT_ALPHA_BITS));
		return (dbr & 0x1F) | (dg & 0x3E0)
			| (dbr >> 6 & 0x7C00) | BITALPHA;
	}
	if(a >= BLT_THRESHOLD2)
		return s | BITALPHA;
	return d;
}

template<typename _tOut, typename _tIn>
void
biltAlphaPoint(_tOut dst_iter, _tIn src_iter);
template<>
inline void
biltAlphaPoint(PixelType* dst_iter, MonoIteratorPair src_iter)
{
	register u8 a(*src_iter.base().second);

	if(a >= BLT_THRESHOLD)
		*dst_iter = blitAlphaBlend(*dst_iter, *src_iter, a);
}
template<>
inline void
biltAlphaPoint(PixelType* dst_iter, IteratorPair src_iter)
{
	register u8 a(*src_iter.base().second);

	if(a >= BLT_THRESHOLD)
		*dst_iter = blitAlphaBlend(*dst_iter, *src_iter, a);
}

#endif

/*!
\brief 循环：按指定扫描顺序复制一块矩形区域的像素。
\note 不检查迭代器有效性。透明度 Alpha 混合。
*/
template<bool _bPositiveScan>
struct BlitBlendLoop
{
	//使用 Alpha 通道表示 8 位透明度。
	template<typename _tIn>
	void
	operator()(int delta_x, int delta_y,
		BitmapPtr dst_iter, _tIn src_iter,
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
			ystdex::delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
};


//图形接口上下文操作：绘图。

/*!
\brief 绘制像素：(x, y) 。
\pre 断言 g.IsValid && Rect(g.GetSize()).Contains(x, y) 。
*/
inline void
PutPixel(const Graphics& g, SPos x, SPos y, Color c)
{
	YAssert(g.IsValid(),
		"In function \"inline void\n"
		"PutPixel(const Graphics& g, SPos x, SPos y, Color c)\": \n"
		"The graphics device context is invalid.");
	YAssert(Rect(g.GetSize()).Contains(x, y),
		"In function \"inline void\n"
		"PutPixel(const Graphics& g, SPos x, SPos y, Color c)\": \n"
		"The pixel is not in the device context buffer.");

	g.GetBufferPtr()[y * g.GetWidth() + x] = c;
}

/*!
\brief 绘制点：(x, y) 。
*/
inline bool
DrawPoint(const Graphics& g, SPos x, SPos y, Color c)
{
	if(g.IsValid() && Rect(g.GetSize()).Contains(x, y))
	{
		PutPixel(g, x, y, c);
		return true;
	}
	return false;
}
/*!
\brief 绘制点：pt 。
*/
inline bool
DrawPoint(const Graphics& g, const Point& pt, Color c)
{
	return DrawPoint(g, pt.X, pt.Y, c);
}

/*!
\brief 绘制水平线段：指定端点水平坐标 x1 、 x2 - 1 ，竖直坐标 y 。
\pre 断言：g.IsValid() 。
*/
bool
DrawHLineSeg(const Graphics& g, SPos y, SPos x1, SPos x2, Color c);

/*!
\brief 绘制竖直线段：指定竖直水平坐标 x ，竖直坐标 y1 - 1 、 y2 。
\pre 断言：g.IsValid() 。
*/
bool
DrawVLineSeg(const Graphics& g, SPos x, SPos y1, SPos y2, Color c);

/*!
\brief 绘制一般线段：端点 p1(x1, y1) 和 p2(x2, y2) 。
*/
bool
DrawLineSeg(const Graphics& g, SPos x1, SPos y1, SPos x2, SPos y2, Color c);
/*!
\brief 绘制一般线段：端点 p1, p2 。
*/
inline bool
DrawLineSeg(const Graphics& g, const Point& p1, const Point& p2, Color c)
{
	return DrawLineSeg(g, p1.X, p1.Y, p2.X, p2.Y, c);
}

/*!
\brief 绘制空心正则矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
*/
bool
DrawRect(const Graphics& g, const Point& pt, const Size& s, Color c);
/*!
\brief 绘制空心正则矩形。
\note 右下角顶点坐标 (r.X + r.Width - 1, r.Y + r.Height - 1) 。
*/
inline bool
DrawRect(const Graphics& g, const Rect& r, Color c)
{
	return DrawRect(g, r, r, c);
}

/*!
\brief 绘制实心正则矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
*/
bool
FillRect(const Graphics& g, const Point& pt, const Size& s, Color c);
/*!
\brief 绘制实心正则矩形。
\note 右下角顶点坐标 (r.X + r.Width - 1, r.Y + r.Height - 1) 。
*/
inline bool
FillRect(const Graphics& g, const Rect& r, Color c)
{
	return FillRect(g, r, r, c);
}

/*!
\brief 以像素为单位变换正则矩形。
*/
template<class _fTransformPixel>
bool
TransformRect(const Graphics& g, const Point& pt, const Size& s,
	_fTransformPixel tp)
{
	if(g.IsValid())
	{
		RectTransfomer()(g.GetBufferPtr(), g.GetSize(), pt, s, tp,
			SequenceTransformer());
		return true;
	}
	return false;
}
/*!
\brief 以像素为单位变换正则矩形。
*/
template<class _fTransformPixel>
inline bool
TransformRect(const Graphics& g, const Rect& r, _fTransformPixel tp)
{
	return TransformRect<_fTransformPixel>(g, r, r, tp);
}

/*!
\brief 以第一个参数作为目标，复制第二个参数的缓冲区内容。
\pre 断言：指针非空；大小相等。
\note 缓冲区指针相等时忽略。
*/
void
CopyBuffer(const Graphics&, const Graphics&);

/*!
\brief 清除图形接口上下文缓冲区。
*/
void
ClearImage(const Graphics&);

/*!
\brief 使用指定颜色填充图形接口上下文缓冲区。
*/
void
Fill(const Graphics&, Color);


// GDI 逻辑对象。

//! \brief 笔样式：字体和笔颜色。
class PenStyle
{
public:
	Drawing::Font Font; //!< 字体。
	Drawing::Color Color; //!< 笔颜色。

	/*!
	\brief 构造：使用指定字体家族、字体大小和颜色。
	*/
	explicit
	PenStyle(const FontFamily& = FetchDefaultFontFamily(),
		Font::SizeType = Font::DefaultSize,
		Drawing::Color = Drawing::ColorSpace::White);
	/*!
	\brief 析构：空实现。
	*/
	virtual DefEmptyDtor(PenStyle)

	DefGetterMember(const FontFamily&, FontFamily, Font)
	DefGetterMember(FontCache&, Cache, Font)
};

inline
PenStyle::PenStyle(const FontFamily& family, Font::SizeType size,
	Drawing::Color c)
	: Font(family, size), Color(c)
{}


//! \brief 边距样式。
struct Padding
{
	SDst Left, Right, Top, Bottom; //!< 边距：左、右、上、下。

	/*!
	\brief 构造：使用 4 个 16 位无符号整数形式的边距。
	*/
	explicit
	Padding(SDst = 4, SDst = 4, SDst = 4, SDst = 4);

	/*!
	\brief 加法赋值：对应分量调用 operator+= 。
	*/
	Padding&
	operator+=(const Padding&);

};


/*!
\brief 加法：对应分量调用 operator+ 。
*/
Padding
operator+(const Padding& a, const Padding& b);


/*!
\brief 取水平边距和。
*/
inline SDst
GetHorizontalFrom(const Padding& m)
{
	return m.Left + m.Right;
}

/*!
\brief 取竖直边距和。
*/
inline SDst
GetVerticalFrom(const Padding& m)
{
	return m.Top + m.Bottom;
}

/*!
\brief 取边距。
\note 64 位无符号整数形式。
*/
u64
GetAllFrom(const Padding&);

/*!
\brief 设置边距。
\note 4 个 16 位无符号整数形式。
*/
void
SetAllTo(Padding&, SDst, SDst, SDst, SDst);
/*!
\brief 设置边距。
\note 64 位无符号整数形式。
*/
inline void
SetAllTo(Padding& m, u64 u)
{
	SetAllTo(m, u >> 48, (u >> 32) & 0xFFFF, (u >> 16) & 0xFFFF, u & 0xFFFF);
}
/*!
\brief 设置边距。
\note 2 个 16 位无符号整数形式，分别表示水平边距和竖直边距。
*/
inline void
SetAllTo(Padding& m, SDst h, SDst v)
{
	SetAllTo(m, h, h, v, v);
}


/*!
\brief 取内边界相对于外边界的边距。
*/
Padding
FetchMargin(const Rect&, const Size&);


//! \brief 正则矩形位图缓冲区。
class BitmapBuffer : public noncopyable, public Graphics
{
public:
	/*!
	\brief 无参数构造：默认实现。
	\note 零初始化。
	*/
	BitmapBuffer() = default;
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	BitmapBuffer(ConstBitmapPtr, SDst, SDst);
	/*!
	\brief 析构：释放资源。
	*/
	virtual
	~BitmapBuffer();

	/*!
	\brief 重新设置缓冲区大小。
	\note 若有一边为零则删除缓冲区；若大于缓冲区空间则重新分配；
		设置后清除缓冲区。
	*/
	virtual void
	SetSize(SDst, SDst);

	/*!
	\brief 交换宽和高；同时清除缓冲区。
	*/
	void
	SetSizeSwap();

	/*!
	\brief 清除缓冲区。
	*/
	virtual void
	ClearImage() const;

	/*!
	\brief 以纯色填充显示缓冲区。
	*/
	virtual void
	BeFilledWith(Color) const;
};

inline BitmapBuffer::~BitmapBuffer()
{
	ydelete_array(pBuffer);
}


//! \brief 扩展的正则矩形位图缓冲区。
class BitmapBufferEx : public BitmapBuffer
{
protected:
	u8* pBufferAlpha; //!<  Alpha 缓冲区指针。

public:
	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	BitmapBufferEx();
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	BitmapBufferEx(ConstBitmapPtr, SDst, SDst);
	/*!
	\brief 析构：释放资源。
	*/
	virtual
	~BitmapBufferEx();

	DefGetter(u8*, BufferAlphaPtr, pBufferAlpha) //!< 取 Alpha 缓冲区的指针。
	DefGetter(size_t, SizeOfBufferAlpha,
		sizeof(u8) * GetAreaFrom(GetSize())) \
		//!< 取 Alpha 缓冲区占用空间。

	/*!
	\brief 重新设置缓冲区大小。
	\note 若有一边为零则删除缓冲区；若大于缓冲区空间则重新分配；
		设置后清除缓冲区。
	*/
	virtual void
	SetSize(SDst, SDst);

	/*!
	\brief 清除缓冲区。
	*/
	virtual void
	ClearImage() const;
};

inline
BitmapBufferEx::BitmapBufferEx() : BitmapBuffer(), pBufferAlpha()
{}
inline
BitmapBufferEx::~BitmapBufferEx()
{
	ydelete_array(pBufferAlpha);
}


/*!
\brief 图形接口上下文向指针指定的缓冲区复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

以指定图形接口上下文作为源，向指定大小和点（相对左上角）的
	指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
bool
CopyTo(BitmapPtr, const Graphics&, const Size& = Size::FullScreen,
	const Point& = Point::Zero, const Point& = Point::Zero,
	const Size& = Size::FullScreen, Rotation = RDeg0);
/*!
\brief 刷新：位图缓冲区向指针指定的缓冲区复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
bool
CopyTo(BitmapPtr, const BitmapBufferEx&, const Size& = Size::FullScreen,
	const Point& = Point::Zero, const Point& = Point::Zero,
	const Size& = Size::FullScreen, Rotation = RDeg0);
/*!
\brief 图形接口上下文复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

以第一个参数作为目标，以指定输出指向复制第二个参数的缓冲区内容
	至相对于目标缓冲区的点。
*/
inline bool
CopyTo(const Graphics& dst, const Graphics& src,
	const Point& dp = Point::Zero, const Point& sp = Point::Zero,
	Rotation rot = RDeg0)
{
	return CopyTo(dst.GetBufferPtr(), src, dst.GetSize(),
		dp, sp, src.GetSize(), rot);
}
/*!
\brief 刷新：位图缓冲区向图形接口上下文复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
inline bool
CopyTo(const Graphics& dst, const BitmapBufferEx& src,
	const Point& dp = Point::Zero, const Point& sp = Point::Zero,
	Rotation rot = RDeg0)
{
	return CopyTo(dst.GetBufferPtr(), src, dst.GetSize(),
		dp, sp, src.GetSize(), rot);
}

/*!
\brief 贴图：位图缓冲区向指针指定的缓冲区以贴图算法复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向以缓冲区内容贴图。
*/
bool
BlitTo(BitmapPtr, const BitmapBufferEx&, const Size& = Size::FullScreen,
	const Point& = Point::Zero, const Point& = Point::Zero,
	const Size& = Size::FullScreen, Rotation = RDeg0);
/*!
\brief 贴图：位图缓冲区向指针指定的缓冲区以贴图算法复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向以缓冲区内容贴图。
*/
inline bool
BlitTo(const Graphics& dst, const BitmapBufferEx& src,
	const Point& dp = Point::Zero, const Point& sp = Point::Zero,
	Rotation rot = RDeg0)
{
	return BlitTo(dst.GetBufferPtr(), src, dst.GetSize(),
		dp, sp, src.GetSize(), rot);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

