// YSLib::Shell::YGDI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-14 18:29:46;
// UTime = 2010-7-20 16:41;
// Version = 0.2580;


#ifndef INCLUDED_YGDI_H_
#define INCLUDED_YGDI_H_

// YGDI：平台无关的图形设备接口实现。

#include "../Core/yobject.h"
#include "../Adapter/yfont.h"

YSL_BEGIN

// GDI 基本数据类型和宏定义。

YSL_BEGIN_NAMESPACE(Drawing)

//基本函数对象。

//像素填充器。
template<typename _pixelType>
struct FillPixel
{
	_pixelType Color;

	inline explicit
	FillPixel(_pixelType c)
	: Color(c)
	{}

	inline void
	operator()(_pixelType* dst)
	{
		*dst = Color;
	}
};

//序列转换器。
struct transSeq
{
	template<typename _pixelType, class _transpType>
	void
	operator()(_pixelType* dst, size_t n, _transpType tp)
	{
		if(dst && n)
		{
			_pixelType* p = dst + n;

			while(--p >= dst)
				tp(p);
		}
	}
};

//竖直线转换器。
struct transVLine
{
	template<typename _pixelType, class _transpType>
	void
	operator()(_pixelType* dst, size_t n, SDST dw, _transpType tp)
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
blitScale(const SPoint& sp, const SPoint& dp, const SSize& ss, const SSize& ds, const SSize& cs,
		  int& minX, int& minY, int& maxX, int& maxY);


//正则矩形转换器。
struct transRect
{
	template<typename _pixelType, class _transpType, class _translType>
	void
	operator()(_pixelType* dst, const SSize& ds,
		const SPoint& dp, const SSize& ss, _transpType tp, _translType tl)
	{
		int minX, minY, maxX, maxY;

		blitScale(SPoint::Zero, dp, ss, ds, ss,
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
	template<typename _pixelType, class _transpType, class _translType>
	inline void
	operator()(_pixelType* dst, const SSize& ds, const SRect& rSrc, _transpType tp, _translType tl)
	{
		operator()<_pixelType, _transpType, _translType>(dst, ds, rSrc, rSrc, tp, tl);
	}
};


//显示缓存操作：清除/以纯色像素填充。

//清除指定位置的 n 个连续像素。
template<typename _pixelType>
inline void
ClearPixel(_pixelType* dst, size_t n)
{
	ClearSequence(dst, n);
}

//使用 n 个指定像素连续填充指定位置。
template<typename _pixelType>
inline void
FillSeq(_pixelType* dst, size_t n, _pixelType c)
{
	transSeq()(dst, n, FillPixel<_pixelType>(c));
}

//使用 n 个指定像素竖直填充指定位置。
template<typename _pixelType>
inline void
FillVLine(_pixelType* dst, size_t n, SDST dw, _pixelType c)
{
	transVLine()(dst, n, dw, FillPixel<_pixelType>(c));
}

//使用指定像素填充指定的正则矩形区域。
template<typename _pixelType>
inline void
FillRect(_pixelType* dst, SDST dw, SDST dh, SPOS sx, SPOS sy, SDST sw, SDST sh, _pixelType c)
{
	transRect()(dst, dw, dh, sx, sy, sw, sh, FillPixel<_pixelType>(c), transSeq());
}
template<typename _pixelType>
inline void
FillRect(_pixelType* dst, const SSize& sDst, const SRect& rSrc, _pixelType c)
{
	transRect()(dst, sDst, rSrc, FillPixel<_pixelType>(c), transSeq());
}


//显示缓存操作：简单复制。

//显示缓存操作：简单贴图。

//复制一块矩形区域的像素。
void
blit(BitmapPtr dst, const SSize& ds,
	 ConstBitmapPtr src, const SSize& ss,
	 const SPoint& sp, const SPoint& dp, const SSize& sc);
//水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
void
blitH(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc);
//竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
void
blitV(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc);
//倒置复制一块矩形区域的像素。
void
blitU(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc);

/*
void
blit(u8* dst, SDST dw, SDST dh,
	 const u8* src, SDST sw, SDST sh,
	 const SPoint& sp, const SPoint& dp, const SSize& sc);
*/

//复制一块矩形区域的像素（使用第 15 位表示透明性）。
void
blit2(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc);
//水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素（使用第 15 位表示透明性）。
void
blit2H(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc);
//竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素（使用第 15 位表示透明性）。
void
blit2V(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc);
//倒置复制一块矩形区域的像素（使用第 15 位表示透明性）。
void
blit2U(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc);

//复制一块矩形区域的像素（使用 Alpha 通道表示透明性）。
void
blit2(BitmapPtr dst, const SSize& ds,
	  ConstBitmapPtr src, const u8* srcA, const SSize& ss,
	  const SPoint& sp, const SPoint& dp, const SSize& sc);
//水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素（使用 Alpha 通道表示透明性）。
void
blit2H(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc);
//竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素（使用 Alpha 通道表示透明性）。
void
blit2V(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc);
//倒置复制一块矩形区域的像素（使用 Alpha 通道表示透明性）。
void
blit2U(BitmapPtr dst, const SSize& ds,
	   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
	   const SPoint& sp, const SPoint& dp, const SSize& sc);

//复制一块矩形区域的像素（使用 Alpha 通道表示 8 位透明度）。
void
blitAlpha(BitmapPtr dst, const SSize& ds,
		  ConstBitmapPtr src, const u8* srcA, const SSize& ss,
		  const SPoint& sp, const SPoint& dp, const SSize& sc);
//水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素（使用 Alpha 通道表示 8 位透明度）。
void
blitAlphaH(BitmapPtr dst, const SSize& ds,
		   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
		   const SPoint& sp, const SPoint& dp, const SSize& sc);
//竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素（使用 Alpha 通道表示 8 位透明度）。
void
blitAlphaV(BitmapPtr dst, const SSize& ds,
		   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
		   const SPoint& sp, const SPoint& dp, const SSize& sc);
//倒置复制一块矩形区域的像素（使用 Alpha 通道表示 8 位透明度）。
void
blitAlphaU(BitmapPtr dst, const SSize& ds,
		   ConstBitmapPtr src, const u8* srcA, const SSize& ss,
		   const SPoint& sp, const SPoint& dp, const SSize& sc);


//图形接口上下文。
class YGIC : public YObject
{
private:
	BitmapPtr pBuffer;
	SSize Size;

public:
	YGIC(BitmapPtr, const SSize&);

	DefBoolGetter(Valid, pBuffer && Size.Width && Size.Height)

	DefGetter(BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const SSize&, Size, Size)
	DefGetter(SDST, Width, Size.Width)
	DefGetter(SDST, Height, Size.Height)
};

inline
YGIC::YGIC(BitmapPtr b, const SSize& s)
: pBuffer(b), Size(s)
{}


//图形接口上下文操作：绘图。

//绘制像素：(x, y) 。
inline void
PutPixel(YGIC& g, SPOS x, SPOS y, PixelType c)
{
	YAssert(g.IsValid(),
		"In function \"inline void\n"
		"PutPixel(YGIC& g, SPOS x, SPOS y, PixelType c)\": \n"
		"The graphic device context is invalid.");
	YAssert(SRect(g.GetSize()).IsInBoundsRegular(x, y),
		"In function \"inline void\n"
		"PutPixel(YGIC& g, SPOS x, SPOS y, PixelType c)\": \n"
		"The pixel is not in the device context buffer.");

	g.GetBufferPtr()[y * g.GetWidth() + x] = c;
}

//绘制点：p(x, y) 。
inline bool
DrawPoint(YGIC& g, SPOS x, SPOS y, PixelType c)
{
	if(g.IsValid() && SRect(g.GetSize()).IsInBoundsRegular(x, y))
	{
		PutPixel(g, x, y, c);
		return true;
	}
	return false;
}
inline bool
DrawPoint(YGIC& g, const SPoint& p, PixelType c)
{
	return DrawPoint(g, p.X, p.Y, c);
}

//绘制水平线段：指定水平坐标 x1 、 x2 ，竖直坐标 y 。
bool
DrawHLineSeg(YGIC& g, SPOS y, SPOS x1, SPOS x2, PixelType c);

//绘制竖直线段：指定水平坐标 x ，竖直坐标 y1 、 y2 。
bool
DrawVLineSeg(YGIC& g, SPOS x, SPOS y1, SPOS y2, PixelType c);

//绘制一般线段：顶点 p1(x1, y1), p2(x2, y2) 。
bool
DrawLineSeg(YGIC& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, PixelType c);
inline bool
DrawLineSeg(YGIC& g, const SPoint& p1, const SPoint& p2, PixelType c)
{
	return DrawLineSeg(g, p1.X, p1.Y, p2.X, p2.Y, c);
}

//绘制空心正则矩形：对角线顶点 p1(x1, y1), p2(x2, y2) 。
bool
DrawRect(YGIC& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, PixelType c);
inline bool
DrawRect(YGIC& g, const SPoint& p1, const SPoint& p2, PixelType c)
{
	return DrawRect(g, p1.X, p1.Y, p2.X, p2.Y, c);
}
inline bool
DrawRect(YGIC& g, const SPoint& p, const SSize& s, PixelType c)
{
	return DrawRect(g, p.X, p.Y, p.X + s.Width, p.Y + s.Height, c);
}
inline bool
DrawRect(YGIC& g, const SRect& r, PixelType c)
{
	return DrawRect(g, r.X, r.Y, r.X + r.Width, r.Y + r.Height, c);
}


// GDI 逻辑对象。

class YPenStyle : YObject //笔样式：字体和笔颜色。
{
public:
	typedef YObject ParentType;

protected:
	MFont Font; //字体。
	PixelType Color; //笔颜色。

public:
	explicit
	YPenStyle(const MFontFamily& = *GetDefaultFontFamilyPtr(), MFont::SizeType = MFont::DefSize, PixelType = RGB15(31, 31, 31) | BITALPHA);

	MFont&
	GetFont();
	DefGetter(const MFont&, Font, Font)
	DefGetterMember(const MFontFamily&, FontFamily, Font)
	DefGetter(MFont::SizeType, FontSize, Font.GetSize())
	DefGetter(PixelType, Color, Color)

	void
	SetFont(const MFont&); //设置字体。
	void
	SetFontStyle(EFontStyle); //设置字体样式。
	void
	SetFontSize(MFont::SizeType); //设置字体大小。
	DefSetterDef(PixelType, Color, Color, ~0) //设置颜色。

	void
	UpdateFont(); //更新字体缓存中当前处理的字体。
	void
	UpdateFontSize(); //更新字体缓存中当前处理的字体大小。
};

inline
YPenStyle::YPenStyle(const MFontFamily& family, MFont::SizeType size, PixelType c)
: Font(family, size), Color(c)
{}

inline MFont&
YPenStyle::GetFont()
{
	return Font;
}

inline void
YPenStyle::SetFont(const MFont& f)
{
	Font = f;
	UpdateFont();
}
inline void
YPenStyle::SetFontStyle(EFontStyle s)
{
	Font.SetStyle(s);
	UpdateFont();
}
inline void
YPenStyle::SetFontSize(MFont::SizeType s)
{
	Font.SetSize(s);
	UpdateFont();
}

inline void
YPenStyle::UpdateFont()
{
	Font.Update();
}
inline void
YPenStyle::UpdateFontSize()
{
	Font.UpdateSize();
}


struct MPadding //边距样式。
{
	SDST Left, Right, Top, Bottom; //边距：左、右、上、下。

	explicit
	MPadding(SDST = 4, SDST = 4, SDST = 4, SDST = 4); //使用 4 个 16 位无符号整数形式初始化边距。
	explicit
	MPadding(u64); //使用 64 位无符号整数形式初始化边距。

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

	MPadding&
	operator+=(const MPadding&);
	friend MPadding
	operator+(const MPadding& a, const MPadding& b);
};

inline SDST
MPadding::GetHorizontal() const
{
	return Left + Right;
}
inline SDST
MPadding::GetVertical() const
{
	return Top + Bottom;
}

inline void
MPadding::SetAll(u64 m)
{
	SetAll(m >> 48, (m >> 32) & 0xFFFF, (m >> 16) & 0xFFFF, m & 0xFFFF);
}
inline void
MPadding::SetAll(SDST h, SDST v)
{
	SetAll(h, h, v, v);
}


class MBitmapBuffer : public SSize //矩形图像缓冲区。
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

	DefConverter(YGIC, YGIC(img, *this)) //生成图形接口上下文。

	DefGetter(SDST, Width, Width) //取缓冲区的宽。
	DefGetter(SDST, Height, Height) //取缓冲区的高。
	DefGetter(BitmapPtr, BufferPtr, img) //取缓冲区指针。
	DefGetter(size_t, SizeOfBuffer, sizeof(PixelType) * GetArea()) //取缓冲区占用空间。

	//重新设置缓冲区大小。
	virtual void
	SetSize(SPOS, SPOS);
	virtual void
	SetSizeSwap(); //交换宽和高；同时交换边距。

	virtual void
	ClearImage() const; //清除缓冲区。
	virtual void
	Fill(PixelType) const; //以纯色填充显示缓冲区。

	virtual void
	CopyToBuffer(BitmapPtr, ROT = RDeg0, const SSize& ds = SSize::FullScreen,
		const SPoint& sp = SPoint::Zero, const SPoint& dp = SPoint::Zero, const SSize& sc = SSize::FullScreen) const; //复制至屏幕指定区域。
};

inline MBitmapBuffer::MBitmapBuffer()
: SSize(), img(NULL)
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
	DefGetter(size_t, SizeOfBufferAlpha, sizeof(u8) * GetArea()) //取 Alpha 缓冲区占用空间。

	//重新设置缓冲区大小。
	virtual void
	SetSize(SPOS, SPOS);
	virtual void
	SetSizeSwap(); //交换宽和高；同时交换边距。

	virtual void
	ClearImage() const; //清除缓冲区。

	void
	CopyToBuffer(BitmapPtr, ROT = RDeg0, const SSize& ds = SSize::FullScreen,
		const SPoint& sp = SPoint::Zero, const SPoint& dp = SPoint::Zero, const SSize& sc = SSize::FullScreen) const; //复制至屏幕指定区域。
	void
	BlitToBuffer(BitmapPtr, ROT = RDeg0, const SSize& ds = SSize::FullScreen,
		const SPoint& sp = SPoint::Zero, const SPoint& dp = SPoint::Zero, const SSize& sc = SSize::FullScreen) const; //贴图至屏幕指定区域。
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

