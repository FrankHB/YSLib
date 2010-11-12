/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.h
\ingroup Shell
\brief 平台无关的图形设备接口实现。
\version 0.3226;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-14 18:29:46 + 08:00;
\par 修改时间:
	2010-11-12 18:55 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YGDI;
*/


#ifndef INCLUDED_YGDI_H_
#define INCLUDED_YGDI_H_

#include "../Core/yobject.h"
#include "../Adaptor/yfont.h"

YSL_BEGIN

// GDI 基本数据类型和宏定义。

YSL_BEGIN_NAMESPACE(Drawing)

//基本函数对象。

//! \brief 像素填充器。
template<typename _tPixel>
struct FillPixel
{
	_tPixel Color;

	/*!
	\brief 构造：使用指定颜色。
	*/
	inline explicit
	FillPixel(_tPixel c)
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
struct transSeq
{
	/*!
	\brief 渲染连续像素。
	*/
	template<typename _tPixel, class _fTransformPixel>
	void
	operator()(_tPixel* dst, std::size_t n, _fTransformPixel tp)
	{
		if(dst && n)
		{
			_tPixel* p = dst + n;

			while(--p >= dst)
				tp(p);
		}
	}
};

//! \brief 竖直线转换器。
struct transVLine
{
	/*!
	\brief 渲染竖直线上的像素。
	*/
	template<typename _tPixel, class _fTransformPixel>
	void
	operator()(_tPixel* dst, std::size_t n, SDST dw, _fTransformPixel tp)
	{
		if(dst && n)
			while(n--)
			{
				tp(dst);
				dst += dw;
			}
	}
};


//! \brief 贴图位置计算器。
void
blitScale(const Point& sp, const Point& dp,
	const Size& ss, const Size& ds, const Size& cs,
	int& minX, int& minY, int& maxX, int& maxY);


//! \brief 正则矩形转换器。
struct transRect
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
		int minX, minY, maxX, maxY;

		blitScale(Point::Zero, dp, ss, ds, ss,
			minX, minY, maxX, maxY);

		const int deltaX(maxX - minX),
			deltaY(maxY - minY);

		dst += (vmax<SPOS>(0, dp.Y) * ds.Width) + vmax<SPOS>(0, dp.X);
		for(int y(0); y < deltaY; ++y)
		{
			tl(dst, deltaX, tp);
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
	operator()(_tPixel* dst, SDST dw, SDST dh, SPOS dx, SPOS dy,
		SDST sw, SDST sh, _fTransformPixel tp, _fTransformLine tl)
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
ClearPixel(_tPixel* dst, std::size_t n)
{
	ClearSequence(dst, n);
}

/*!
\brief 使用 n 个指定像素连续填充指定位置。
*/
template<typename _tPixel>
inline void
FillSeq(_tPixel* dst, std::size_t n, _tPixel c)
{
	transSeq()(dst, n, FillPixel<_tPixel>(c));
}

/*!
\brief 使用 n 个指定像素竖直填充指定位置。
*/
template<typename _tPixel>
inline void
FillVLine(_tPixel* dst, std::size_t n, SDST dw, _tPixel c)
{
	transVLine()(dst, n, dw, FillPixel<_tPixel>(c));
}

/*!
\brief 使用指定像素填充指定的正则矩形区域。
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Point& sp, const Size& ss,
	_tPixel c)
{
	transRect()(dst, ds, sp, ss, FillPixel<_tPixel>(c), transSeq());
}
/*!
\brief 使用指定像素填充指定的正则矩形区域。
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Rect& rSrc, _tPixel c)
{
	transRect()(dst, ds, rSrc, FillPixel<_tPixel>(c), transSeq());
}
/*!
\brief 使用指定像素填充指定的正则矩形区域。
*/
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, SDST dw, SDST dh, SPOS sx, SPOS sy, SDST sw, SDST sh,
	_tPixel c)
{
	transRect()(dst, dw, dh, sx, sy, sw, sh, FillPixel<_tPixel>(c), transSeq());
}


//显示缓存操作：复制和贴图。

/*!
\brief 复制一块矩形区域的像素。
*/
void
blit(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
*/
void
blitH(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
*/
void
blitV(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);
/*!
\brief 倒置复制一块矩形区域的像素。
*/
void
blitU(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*
void
blit(u8* dst, SDST dw, SDST dh,
	const u8* src, SDST sw, SDST sh,
	const Point& sp, const Point& dp, const Size& sc);
*/

/*!
\brief 复制一块矩形区域的像素。
\note 使用第 15 位表示透明性。
*/
void
blit2(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
\note 使用第 15 位表示透明性。
*/
void
blit2H(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
\note 使用第 15 位表示透明性。
*/
void
blit2V(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 倒置复制一块矩形区域的像素。
\note 使用第 15 位表示透明性。
*/
void
blit2U(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 复制一块矩形区域的像素。
\note 使用 Alpha 通道表示透明性。
*/
void
blit2(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
\note 使用 Alpha 通道表示透明性。
*/
void
blit2H(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
\note 使用 Alpha 通道表示透明性。
*/
void
blit2V(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 倒置复制一块矩形区域的像素。
\note 使用 Alpha 通道表示透明性。
*/
void
blit2U(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 复制一块矩形区域的像素。
\note 使用 Alpha 通道表示 8 位透明度。
*/
void
blitAlpha(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
\note 使用 Alpha 通道表示 8 位透明度。
*/
void
blitAlphaH(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
\note 使用 Alpha 通道表示 8 位透明度。
*/
void
blitAlphaV(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

/*!
\brief 倒置复制一块矩形区域的像素。
\note 使用 Alpha 通道表示 8 位透明度。
*/
void
blitAlphaU(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);


//! \brief 图形接口上下文。
class Graphics
{
private:
	BitmapPtr pBuffer;
	Drawing::Size Size;

public:
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	Graphics(BitmapPtr, const Drawing::Size&);

	DefPredicate(Valid, pBuffer && Size.Width != 0 && Size.Height != 0)

	DefGetter(BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const Drawing::Size&, Size, Size)
	DefGetter(SDST, Width, Size.Width)
	DefGetter(SDST, Height, Size.Height)
};

inline
Graphics::Graphics(BitmapPtr b, const Drawing::Size& s)
	: pBuffer(b), Size(s)
{}


//图形接口上下文操作：绘图。

/*!
\brief 绘制像素：(x, y) 。
\pre 断言 g.IsValid && Rect(g.GetSize()).Contains(x, y) 。
*/
inline void
PutPixel(const Graphics& g, SPOS x, SPOS y, Color c)
{
	YAssert(g.IsValid(),
		"In function \"inline void\n"
		"PutPixel(const Graphics& g, SPOS x, SPOS y, Color c)\": \n"
		"The graphics device context is invalid.");
	YAssert(Rect(g.GetSize()).Contains(x, y),
		"In function \"inline void\n"
		"PutPixel(const Graphics& g, SPOS x, SPOS y, Color c)\": \n"
		"The pixel is not in the device context buffer.");

	g.GetBufferPtr()[y * g.GetWidth() + x] = c;
}

/*!
\brief 绘制点：p(x, y) 。
*/
inline bool
DrawPoint(const Graphics& g, SPOS x, SPOS y, Color c)
{
	if(g.IsValid() && Rect(g.GetSize()).Contains(x, y))
	{
		PutPixel(g, x, y, c);
		return true;
	}
	return false;
}
/*!
\brief 绘制点：p 。
*/
inline bool
DrawPoint(const Graphics& g, const Point& p, Color c)
{
	return DrawPoint(g, p.X, p.Y, c);
}

/*!
\brief 绘制水平线段：指定端点水平坐标 x1 、 x2 - 1，竖直坐标 y 。
*/
bool
DrawHLineSeg(const Graphics& g, SPOS y, SPOS x1, SPOS x2, Color c);

/*!
\brief 绘制竖直线段：指定竖直水平坐标 x ，竖直坐标 y1 - 1 、 y2 。
*/
bool
DrawVLineSeg(const Graphics& g, SPOS x, SPOS y1, SPOS y2, Color c);

/*!
\brief 绘制一般线段：端点 p1(x1, y1), p2(x2, y2) 。
*/
bool
DrawLineSeg(const Graphics& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, Color c);
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
*/
bool
DrawRect(const Graphics& g, const Point& p, const Size& s, Color c);
/*!
\brief 绘制空心正则矩形。
*/
inline bool
DrawRect(const Graphics& g, const Rect& r, Color c)
{
	return DrawRect(g, r, r, c);
}

/*!
\brief 绘制实心正则矩形。
*/
bool
FillRect(const Graphics& g, const Point& p, const Size& s, Color c);
/*!
\brief 绘制实心正则矩形。
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
TransformRect(const Graphics& g, const Point& p, const Size& s,
	_fTransformPixel tp)
{
	if(g.IsValid())
	{
		transRect()(g.GetBufferPtr(), g.GetSize(), p, s, tp, transSeq());
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
	PenStyle(const FontFamily& = GetDefaultFontFamily(),
		Font::SizeType = Font::DefSize,
		Drawing::Color = Drawing::ColorSpace::White);

	DefGetterMember(const FontFamily&, FontFamily, Font)
	DefGetterMember(YFontCache&, Cache, Font)
};

inline
PenStyle::PenStyle(const FontFamily& family, Font::SizeType size,
	Drawing::Color c)
	: Font(family, size), Color(c)
{}


//! \brief 边距样式。
struct Padding
{
	SDST Left, Right, Top, Bottom; //!< 边距：左、右、上、下。

	/*!
	\brief 构造：使用 4 个 16 位无符号整数形式的边距。
	*/
	explicit
	Padding(SDST = 4, SDST = 4, SDST = 4, SDST = 4);

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
inline SDST
GetHorizontalFrom(const Padding& p)
{
	return p.Left + p.Right;
}

/*!
\brief 取竖直边距和。
*/
inline SDST
GetVerticalFrom(const Padding& p)
{
	return p.Top + p.Bottom;
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
SetAllTo(Padding&, SDST, SDST, SDST, SDST);
/*!
\brief 设置边距。
\note 64 位无符号整数形式。
*/
inline void
SetAllTo(Padding& p, u64 m)
{
	SetAllTo(p, m >> 48, (m >> 32) & 0xFFFF, (m >> 16) & 0xFFFF, m & 0xFFFF);
}
/*!
\brief 设置边距。
\note 2 个 16 位无符号整数形式，分别表示水平边距和竖直边距。
*/
inline void
SetAllTo(Padding& p, SDST h, SDST v)
{
	SetAllTo(p, h, h, v, v);
}


//! \brief 矩形图像缓冲区。
class BitmapBuffer : public Size
{
protected:
	BitmapPtr img; //!< 显示缓冲区指针。

public:
	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	BitmapBuffer();
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	BitmapBuffer(ConstBitmapPtr, SDST, SDST);
	/*!
	\brief 析构：释放资源。
	*/
	virtual
	~BitmapBuffer();

	/*!
	\brief 比较：相等关系。
	*/
	friend bool
	operator==(const BitmapBuffer&, const BitmapBuffer&);

	DefConverter(Graphics, Graphics(img, *this)) //!< 生成图形接口上下文。

	DefGetter(BitmapPtr, BufferPtr, img) //!< 取缓冲区指针。
	DefGetter(std::size_t, SizeOfBuffer, sizeof(PixelType) * GetAreaFrom(*this)) \
		//!< 取缓冲区占用空间。

	/*!
	\brief 重新设置缓冲区大小。
	*/
	virtual void
	SetSize(SPOS, SPOS);

	/*!
	\brief 交换宽和高；同时交换边距。
	*/
	virtual void
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

	/*!
	\brief 按指定角度复制缓冲区内容至指定大小和指定点的指定缓存。
	*/
	virtual void
	CopyToBuffer(BitmapPtr, ROT = RDeg0, const Size& ds = Size::FullScreen,
		const Point& sp = Point::Zero, const Point& dp = Point::Zero,
		const Size& sc = Size::FullScreen) const;
};

inline BitmapBuffer::BitmapBuffer()
	: Size(),
	img(NULL)
{}
inline BitmapBuffer::~BitmapBuffer()
{
	delete[] img;
}

inline bool
operator==(const BitmapBuffer& a, const BitmapBuffer& b)
{
	return a.img == b.img && a.Width == b.Width && a.Height == b.Height;
}


//! \brief 矩形增强图像缓冲区。
class BitmapBufferEx : public BitmapBuffer
{
protected:
	u8* imgAlpha; //!<  Alpha 缓冲区指针。

public:
	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	BitmapBufferEx();
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	BitmapBufferEx(ConstBitmapPtr, SDST, SDST);
	/*!
	\brief 析构：释放资源。
	*/
	virtual
	~BitmapBufferEx();

	/*!
	\brief 比较：相等关系。
	*/
	friend bool
	operator==(const BitmapBufferEx&, const BitmapBufferEx&);

	DefGetter(u8*, BufferAlphaPtr, imgAlpha) //!< 取 Alpha 缓冲区的指针。
	DefGetter(std::size_t, SizeOfBufferAlpha, sizeof(u8) * GetAreaFrom(*this)) \
		//!< 取 Alpha 缓冲区占用空间。

	/*!
	\brief 重新设置缓冲区大小。
	*/
	virtual void
	SetSize(SPOS, SPOS);
	/*!
	\brief 交换宽和高；同时交换边距。
	*/
	virtual void
	SetSizeSwap();

	/*!
	\brief 清除缓冲区。
	*/
	virtual void
	ClearImage() const;

	/*!
	\brief 按指定角度复制缓冲区内容至指定大小和指定点的指定缓存。
	*/
	void
	CopyToBuffer(BitmapPtr, ROT = RDeg0, const Size& ds = Size::FullScreen,
		const Point& sp = Point::Zero, const Point& dp = Point::Zero,
		const Size& sc = Size::FullScreen) const;

	/*!
	\brief 按指定角度贴图缓冲区内容至指定大小和指定点的指定缓存。
	*/
	void
	BlitToBuffer(BitmapPtr, ROT = RDeg0, const Size& ds = Size::FullScreen,
		const Point& sp = Point::Zero, const Point& dp = Point::Zero,
		const Size& sc = Size::FullScreen) const;
};

inline
BitmapBufferEx::BitmapBufferEx() : BitmapBuffer(), imgAlpha(NULL)
{}
inline
BitmapBufferEx::~BitmapBufferEx()
{
	delete[] imgAlpha;
}

inline bool
operator==(const BitmapBufferEx& a, const BitmapBufferEx& b)
{
	return static_cast<BitmapBuffer>(a) == static_cast<BitmapBuffer>(b);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

