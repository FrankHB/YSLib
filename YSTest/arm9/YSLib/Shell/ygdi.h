// YSLib::Shell::YGDI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-14 18:29:46 + 08:00;
// UTime = 2010-10-17 14:59 + 08:00;
// Version = 0.2858;


#ifndef INCLUDED_YGDI_H_
#define INCLUDED_YGDI_H_

// YGDI ：平台无关的图形设备接口实现。

#include "../Core/yobject.h"
#include "../Adaptor/yfont.h"

YSL_BEGIN

// GDI 基本数据类型和宏定义。

YSL_BEGIN_NAMESPACE(Drawing)

//基本函数对象。

//像素填充器。
template<typename _tPixel>
struct FillPixel
{
	_tPixel Color;

	inline explicit
	FillPixel(_tPixel c)
		: Color(c)
	{}

	inline void
	operator()(_tPixel* dst)
	{
		*dst = Color;
	}
};

//序列转换器。
struct transSeq
{
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

//竖直线转换器。
struct transVLine
{
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


//贴图位置计算器。
void
blitScale(const Point& sp, const Point& dp, const Size& ss, const Size& ds, const Size& cs,
		  int& minX, int& minY, int& maxX, int& maxY);


//正则矩形转换器。
struct transRect
{
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
	template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tPixel* dst, const Size& ds, const Rect& rSrc,
		_fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tPixel, _fTransformPixel, _fTransformLine>(dst, ds, rSrc, rSrc, tp, tl);
	}
	template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tPixel* dst, SDST dw, SDST dh, SPOS dx, SPOS dy, SDST sw, SDST sh,
		_fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tPixel, _fTransformPixel, _fTransformLine>(dst, Size(dw, dh), Point(dx, dy), Size(sw, sh), tp, tl);
	}
};


//显示缓存操作：清除/以纯色像素填充。

//清除指定位置的 n 个连续像素。
template<typename _tPixel>
inline void
ClearPixel(_tPixel* dst, std::size_t n)
{
	ClearSequence(dst, n);
}

//使用 n 个指定像素连续填充指定位置。
template<typename _tPixel>
inline void
FillSeq(_tPixel* dst, std::size_t n, _tPixel c)
{
	transSeq()(dst, n, FillPixel<_tPixel>(c));
}

//使用 n 个指定像素竖直填充指定位置。
template<typename _tPixel>
inline void
FillVLine(_tPixel* dst, std::size_t n, SDST dw, _tPixel c)
{
	transVLine()(dst, n, dw, FillPixel<_tPixel>(c));
}

//使用指定像素填充指定的正则矩形区域。
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Point& sp, const Size& ss, _tPixel c)
{
	transRect()(dst, ds, sp, ss, FillPixel<_tPixel>(c), transSeq());
}
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Rect& rSrc, _tPixel c)
{
	transRect()(dst, ds, rSrc, FillPixel<_tPixel>(c), transSeq());
}
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, SDST dw, SDST dh, SPOS sx, SPOS sy, SDST sw, SDST sh, _tPixel c)
{
	transRect()(dst, dw, dh, sx, sy, sw, sh, FillPixel<_tPixel>(c), transSeq());
}


//显示缓存操作：简单复制。

//显示缓存操作：简单贴图。

//复制一块矩形区域的像素。
void
blit(BitmapPtr dst, const Size& ds,
	 ConstBitmapPtr src, const Size& ss,
	 const Point& sp, const Point& dp, const Size& sc);
//水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
void
blitH(BitmapPtr dst, const Size& ds,
	  ConstBitmapPtr src, const Size& ss,
	  const Point& sp, const Point& dp, const Size& sc);
//竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
void
blitV(BitmapPtr dst, const Size& ds,
	  ConstBitmapPtr src, const Size& ss,
	  const Point& sp, const Point& dp, const Size& sc);
//倒置复制一块矩形区域的像素。
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

//复制一块矩形区域的像素（使用第 15 位表示透明性）。
void
blit2(BitmapPtr dst, const Size& ds,
	  ConstBitmapPtr src, const Size& ss,
	  const Point& sp, const Point& dp, const Size& sc);
//水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素（使用第 15 位表示透明性）。
void
blit2H(BitmapPtr dst, const Size& ds,
	   ConstBitmapPtr src, const Size& ss,
	   const Point& sp, const Point& dp, const Size& sc);
//竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素（使用第 15 位表示透明性）。
void
blit2V(BitmapPtr dst, const Size& ds,
	   ConstBitmapPtr src, const Size& ss,
	   const Point& sp, const Point& dp, const Size& sc);
//倒置复制一块矩形区域的像素（使用第 15 位表示透明性）。
void
blit2U(BitmapPtr dst, const Size& ds,
	   ConstBitmapPtr src, const Size& ss,
	   const Point& sp, const Point& dp, const Size& sc);

//复制一块矩形区域的像素（使用 Alpha 通道表示透明性）。
void
blit2(BitmapPtr dst, const Size& ds,
	  ConstBitmapPtr src, const u8* srcA, const Size& ss,
	  const Point& sp, const Point& dp, const Size& sc);
//水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素（使用 Alpha 通道表示透明性）。
void
blit2H(BitmapPtr dst, const Size& ds,
	   ConstBitmapPtr src, const u8* srcA, const Size& ss,
	   const Point& sp, const Point& dp, const Size& sc);
//竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素（使用 Alpha 通道表示透明性）。
void
blit2V(BitmapPtr dst, const Size& ds,
	   ConstBitmapPtr src, const u8* srcA, const Size& ss,
	   const Point& sp, const Point& dp, const Size& sc);
//倒置复制一块矩形区域的像素（使用 Alpha 通道表示透明性）。
void
blit2U(BitmapPtr dst, const Size& ds,
	   ConstBitmapPtr src, const u8* srcA, const Size& ss,
	   const Point& sp, const Point& dp, const Size& sc);

//复制一块矩形区域的像素（使用 Alpha 通道表示 8 位透明度）。
void
blitAlpha(BitmapPtr dst, const Size& ds,
		  ConstBitmapPtr src, const u8* srcA, const Size& ss,
		  const Point& sp, const Point& dp, const Size& sc);
//水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素（使用 Alpha 通道表示 8 位透明度）。
void
blitAlphaH(BitmapPtr dst, const Size& ds,
		   ConstBitmapPtr src, const u8* srcA, const Size& ss,
		   const Point& sp, const Point& dp, const Size& sc);
//竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素（使用 Alpha 通道表示 8 位透明度）。
void
blitAlphaV(BitmapPtr dst, const Size& ds,
		   ConstBitmapPtr src, const u8* srcA, const Size& ss,
		   const Point& sp, const Point& dp, const Size& sc);
//倒置复制一块矩形区域的像素（使用 Alpha 通道表示 8 位透明度）。
void
blitAlphaU(BitmapPtr dst, const Size& ds,
		   ConstBitmapPtr src, const u8* srcA, const Size& ss,
		   const Point& sp, const Point& dp, const Size& sc);


//图形接口上下文。
class GraphicInterfaceContext
{
private:
	BitmapPtr pBuffer;
	Drawing::Size Size;

public:
	GraphicInterfaceContext(BitmapPtr, const Drawing::Size&);

	DefPredicate(Valid, pBuffer && Size.Width && Size.Height)

	DefGetter(BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const Drawing::Size&, Size, Size)
	DefGetter(SDST, Width, Size.Width)
	DefGetter(SDST, Height, Size.Height)
};

inline
GraphicInterfaceContext::GraphicInterfaceContext(BitmapPtr b, const Drawing::Size& s)
	: pBuffer(b), Size(s)
{}


//图形接口上下文操作：绘图。

//绘制像素：(x, y) 。
inline void
PutPixel(GraphicInterfaceContext& g, SPOS x, SPOS y, Color c)
{
	YAssert(g.IsValid(),
		"In function \"inline void\n"
		"PutPixel(GraphicInterfaceContext& g, SPOS x, SPOS y, Color c)\": \n"
		"The graphics device context is invalid.");
	YAssert(Rect(g.GetSize()).IsInBoundsRegular(x, y),
		"In function \"inline void\n"
		"PutPixel(GraphicInterfaceContext& g, SPOS x, SPOS y, Color c)\": \n"
		"The pixel is not in the device context buffer.");

	g.GetBufferPtr()[y * g.GetWidth() + x] = c;
}

//绘制点：p(x, y) 。
inline bool
DrawPoint(GraphicInterfaceContext& g, SPOS x, SPOS y, Color c)
{
	if(g.IsValid() && Rect(g.GetSize()).IsInBoundsRegular(x, y))
	{
		PutPixel(g, x, y, c);
		return true;
	}
	return false;
}
inline bool
DrawPoint(GraphicInterfaceContext& g, const Point& p, Color c)
{
	return DrawPoint(g, p.X, p.Y, c);
}

//绘制水平线段：指定水平坐标 x1 、 x2 ，竖直坐标 y 。
bool
DrawHLineSeg(GraphicInterfaceContext& g, SPOS y, SPOS x1, SPOS x2, Color c);

//绘制竖直线段：指定水平坐标 x ，竖直坐标 y1 、 y2 。
bool
DrawVLineSeg(GraphicInterfaceContext& g, SPOS x, SPOS y1, SPOS y2, Color c);

//绘制一般线段：顶点 p1(x1, y1), p2(x2, y2) 。
bool
DrawLineSeg(GraphicInterfaceContext& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, Color c);
inline bool
DrawLineSeg(GraphicInterfaceContext& g, const Point& p1, const Point& p2, Color c)
{
	return DrawLineSeg(g, p1.X, p1.Y, p2.X, p2.Y, c);
}

//绘制空心正则矩形：对角线顶点 p1(x1, y1), p2(x2, y2) 。
bool
DrawRect(GraphicInterfaceContext& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, Color c);
inline bool
DrawRect(GraphicInterfaceContext& g, const Point& p1, const Point& p2, Color c)
{
	return DrawRect(g, p1.X, p1.Y, p2.X, p2.Y, c);
}
inline bool
DrawRect(GraphicInterfaceContext& g, const Point& p, const Size& s, Color c)
{
	return DrawRect(g, p.X, p.Y, p.X + s.Width, p.Y + s.Height, c);
}
inline bool
DrawRect(GraphicInterfaceContext& g, const Rect& r, Color c)
{
	return DrawRect(g, r.X, r.Y, r.X + r.Width, r.Y + r.Height, c);
}


// GDI 逻辑对象。

class PenStyle //笔样式：字体和笔颜色。
{
public:
	Drawing::Font Font; //字体。
	Drawing::Color Color; //笔颜色。

	explicit
	PenStyle(const FontFamily& = GetDefaultFontFamily(), Font::SizeType = Font::DefSize, Drawing::Color = Drawing::Color::White);

	DefGetterMember(const FontFamily&, FontFamily, Font)
	DefGetterMember(YFontCache&, Cache, Font)
};

inline
PenStyle::PenStyle(const FontFamily& family, Font::SizeType size, Drawing::Color c)
	: Font(family, size), Color(c)
{}


struct Padding //边距样式。
{
	SDST Left, Right, Top, Bottom; //边距：左、右、上、下。

	explicit
	Padding(SDST = 4, SDST = 4, SDST = 4, SDST = 4); //使用 4 个 16 位无符号整数形式初始化边距。
	explicit
	Padding(u64); //使用 64 位无符号整数形式初始化边距。

	SDST
	GetHorizontal() const; //取水平边距和。
	SDST
	GetVertical() const; //取竖直边距和。
	u64
	GetAll() const; //取边距（64 位无符号整数形式）。

	void
	SetAll(u64); //设置边距（64 位无符号整数形式）。
	void
	SetAll(SDST, SDST); //设置边距（2 个 16 位无符号整数形式，分别表示水平边距和竖直边距）。
	void
	SetAll(SDST, SDST, SDST, SDST); //设置边距（4 个 16 位无符号整数形式）。

	Padding&
	operator+=(const Padding&);
	friend Padding
	operator+(const Padding& a, const Padding& b);
};

inline SDST
Padding::GetHorizontal() const
{
	return Left + Right;
}
inline SDST
Padding::GetVertical() const
{
	return Top + Bottom;
}

inline void
Padding::SetAll(u64 m)
{
	SetAll(m >> 48, (m >> 32) & 0xFFFF, (m >> 16) & 0xFFFF, m & 0xFFFF);
}
inline void
Padding::SetAll(SDST h, SDST v)
{
	SetAll(h, h, v, v);
}


class MBitmapBuffer : public Size //矩形图像缓冲区。
{
protected:
	BitmapPtr img; //显示缓冲区指针。

public:
	MBitmapBuffer();
	MBitmapBuffer(ConstBitmapPtr, SDST, SDST);
	virtual
	~MBitmapBuffer();

	friend bool
	operator==(const MBitmapBuffer&, const MBitmapBuffer&);

	DefConverter(GraphicInterfaceContext, GraphicInterfaceContext(img, *this)) //生成图形接口上下文。

	DefGetter(SDST, Width, Width) //取缓冲区的宽。
	DefGetter(SDST, Height, Height) //取缓冲区的高。
	DefGetter(BitmapPtr, BufferPtr, img) //取缓冲区指针。
	DefGetter(std::size_t, SizeOfBuffer, sizeof(PixelType) * GetArea()) //取缓冲区占用空间。

	//重新设置缓冲区大小。
	virtual void
	SetSize(SPOS, SPOS);
	virtual void
	SetSizeSwap(); //交换宽和高；同时交换边距。

	virtual void
	ClearImage() const; //清除缓冲区。
	virtual void
	Fill(Color) const; //以纯色填充显示缓冲区。

	virtual void
	CopyToBuffer(BitmapPtr, ROT = RDeg0, const Size& ds = Size::FullScreen,
		const Point& sp = Point::Zero, const Point& dp = Point::Zero, const Size& sc = Size::FullScreen) const; //复制至屏幕指定区域。
};

inline MBitmapBuffer::MBitmapBuffer()
	: Size(),
	img(NULL)
{}
inline MBitmapBuffer::~MBitmapBuffer()
{
	delete[] img;
}

inline bool
operator==(const MBitmapBuffer& a, const MBitmapBuffer& b)
{
	return a.img == b.img && a.Width == b.Width && a.Height == b.Height;
}


class MBitmapBufferEx : public MBitmapBuffer //矩形增强图像缓冲区。
{
protected:
	u8* imgAlpha; // Alpha 缓冲区指针。

public:
	MBitmapBufferEx();
	MBitmapBufferEx(ConstBitmapPtr, SDST, SDST);
	virtual
	~MBitmapBufferEx();

	friend bool
	operator==(const MBitmapBufferEx&, const MBitmapBufferEx&);

	DefGetter(u8*, BufferAlphaPtr, imgAlpha) //取 Alpha 缓冲区的指针。
	DefGetter(std::size_t, SizeOfBufferAlpha, sizeof(u8) * GetArea()) //取 Alpha 缓冲区占用空间。

	//重新设置缓冲区大小。
	virtual void
	SetSize(SPOS, SPOS);
	virtual void
	SetSizeSwap(); //交换宽和高；同时交换边距。

	virtual void
	ClearImage() const; //清除缓冲区。

	void
	CopyToBuffer(BitmapPtr, ROT = RDeg0, const Size& ds = Size::FullScreen,
		const Point& sp = Point::Zero, const Point& dp = Point::Zero, const Size& sc = Size::FullScreen) const; //复制至屏幕指定区域。
	void
	BlitToBuffer(BitmapPtr, ROT = RDeg0, const Size& ds = Size::FullScreen,
		const Point& sp = Point::Zero, const Point& dp = Point::Zero, const Size& sc = Size::FullScreen) const; //贴图至屏幕指定区域。
};

inline
MBitmapBufferEx::MBitmapBufferEx() : MBitmapBuffer(), imgAlpha(NULL)
{}
inline
MBitmapBufferEx::~MBitmapBufferEx()
{
	delete[] imgAlpha;
}

inline bool
operator==(const MBitmapBufferEx& a, const MBitmapBufferEx& b)
{
	return static_cast<MBitmapBuffer>(a) == static_cast<MBitmapBuffer>(b);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

