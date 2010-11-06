// YSLib::Shell::YGDI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-14 19:55 + 08:00;
// UTime = 2010-11-06 12:35 + 08:00;
// Version = 0.3206;


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

	//********************************
	//名称:		FillPixel
	//全名:		YSLib::Drawing::FillPixel<_tPixel>::FillPixel
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tPixel c
	//功能概要:	构造：使用指定颜色。
	//备注:		
	//********************************
	inline explicit
	FillPixel(_tPixel c)
		: Color(c)
	{}

	//********************************
	//名称:		operator()
	//全名:		YSLib::Drawing::FillPixel<_tPixel>::operator()
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_tPixel * dst
	//功能概要:	像素填充函数。
	//备注:		
	//********************************
	inline void
	operator()(_tPixel* dst)
	{
		*dst = Color;
	}
};

//序列转换器。
struct transSeq
{
	//********************************
	//名称:		operator()
	//全名:		YSLib::Drawing::transSeq<_tPixel, _fTransformPixel>::operator()
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_tPixel * dst
	//形式参数:	std::size_t n
	//形式参数:	_fTransformPixel tp
	//功能概要:	渲染连续像素。
	//备注:		
	//********************************
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
	//********************************
	//名称:		operator()
	//全名:		YSLib::Drawing::transVLine<_tPixel, _fTransformPixel>
	//				::operator()
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_tPixel * dst
	//形式参数:	std::size_t n
	//形式参数:	SDST dw
	//形式参数:	_fTransformPixel tp
	//功能概要:	渲染竖直线上的像素。
	//备注:		
	//********************************
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
blitScale(const Point& sp, const Point& dp,
	const Size& ss, const Size& ds, const Size& cs,
	int& minX, int& minY, int& maxX, int& maxY);


//正则矩形转换器。
struct transRect
{
	//********************************
	//名称:		operator()
	//全名:		YSLib::Drawing::transRect<_tPixel, _fTransformPixel,
	//				_fTransformLine>::operator()
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_tPixel * dst
	//形式参数:	const Size & ds
	//形式参数:	const Point & dp
	//形式参数:	const Size & ss
	//形式参数:	_fTransformPixel tp
	//形式参数:	_fTransformLine tl
	//功能概要:	渲染正则矩形内的像素。
	//备注:		不含右边界和下边界。
	//********************************
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
	//********************************
	//名称:		operator()
	//全名:		YSLib::Drawing::transRect<_tPixel, _fTransformPixel,
	//				_fTransformLine>::operator()
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_tPixel * dst
	//形式参数:	const Size & ds
	//形式参数:	const Rect & rSrc
	//形式参数:	_fTransformPixel tp
	//形式参数:	_fTransformLine tl
	//功能概要:	渲染正则矩形内的像素。
	//备注:		不含右边界和下边界。
	//********************************
	template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
	inline void
	operator()(_tPixel* dst, const Size& ds, const Rect& rSrc,
		_fTransformPixel tp, _fTransformLine tl)
	{
		operator()<_tPixel, _fTransformPixel,
			_fTransformLine>(dst, ds, rSrc, rSrc, tp, tl);
	}
	//********************************
	//名称:		operator()
	//全名:		YSLib::Drawing::transRect<_tPixel, _fTransformPixel,
	//				_fTransformLine>::operator()
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	_tPixel * dst
	//形式参数:	SDST dw
	//形式参数:	SDST dh
	//形式参数:	SPOS dx
	//形式参数:	SPOS dy
	//形式参数:	SDST sw
	//形式参数:	SDST sh
	//形式参数:	_fTransformPixel tp
	//形式参数:	_fTransformLine tl
	//功能概要:	渲染正则矩形内的像素。
	//备注:		不含右边界和下边界。
	//********************************
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

//********************************
//名称:		ClearPixel
//全名:		YSLib::Drawing<_tPixel>::ClearPixel
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_tPixel * dst
//形式参数:	std::size_t n
//功能概要:	清除指定位置的 n 个连续像素。
//备注:		
//********************************
template<typename _tPixel>
inline void
ClearPixel(_tPixel* dst, std::size_t n)
{
	ClearSequence(dst, n);
}

//********************************
//名称:		FillSeq
//全名:		YSLib::Drawing<_tPixel>::FillSeq
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_tPixel * dst
//形式参数:	std::size_t n
//形式参数:	_tPixel c
//功能概要:	使用 n 个指定像素连续填充指定位置。
//备注:		
//********************************
template<typename _tPixel>
inline void
FillSeq(_tPixel* dst, std::size_t n, _tPixel c)
{
	transSeq()(dst, n, FillPixel<_tPixel>(c));
}

//********************************
//名称:		FillVLine
//全名:		YSLib::Drawing<_tPixel>::FillVLine
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_tPixel * dst
//形式参数:	std::size_t n
//形式参数:	SDST dw
//形式参数:	_tPixel c
//功能概要:	使用 n 个指定像素竖直填充指定位置。
//备注:		
//********************************
template<typename _tPixel>
inline void
FillVLine(_tPixel* dst, std::size_t n, SDST dw, _tPixel c)
{
	transVLine()(dst, n, dw, FillPixel<_tPixel>(c));
}

//********************************
//名称:		FillRect
//全名:		YSLib::Drawing<_tPixel>::FillRect
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_tPixel * dst
//形式参数:	const Size & ds
//形式参数:	const Point & sp
//形式参数:	const Size & ss
//形式参数:	_tPixel c
//功能概要:	使用指定像素填充指定的正则矩形区域。
//备注:		
//********************************
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Point& sp, const Size& ss,
	_tPixel c)
{
	transRect()(dst, ds, sp, ss, FillPixel<_tPixel>(c), transSeq());
}
//********************************
//名称:		FillRect
//全名:		YSLib::Drawing<_tPixel>::FillRect
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_tPixel * dst
//形式参数:	const Size & ds
//形式参数:	const Rect & rSrc
//形式参数:	_tPixel c
//功能概要:	使用指定像素填充指定的正则矩形区域。
//备注:		
//********************************
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, const Size& ds, const Rect& rSrc, _tPixel c)
{
	transRect()(dst, ds, rSrc, FillPixel<_tPixel>(c), transSeq());
}
//********************************
//名称:		FillRect
//全名:		YSLib::Drawing<_tPixel>::FillRect
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_tPixel * dst
//形式参数:	SDST dw
//形式参数:	SDST dh
//形式参数:	SPOS sx
//形式参数:	SPOS sy
//形式参数:	SDST sw
//形式参数:	SDST sh
//形式参数:	_tPixel c
//功能概要:	使用指定像素填充指定的正则矩形区域。
//备注:		
//********************************
template<typename _tPixel>
inline void
FillRect(_tPixel* dst, SDST dw, SDST dh, SPOS sx, SPOS sy, SDST sw, SDST sh,
	_tPixel c)
{
	transRect()(dst, dw, dh, sx, sy, sw, sh, FillPixel<_tPixel>(c), transSeq());
}


//显示缓存操作：简单复制。

//显示缓存操作：简单贴图。

//********************************
//名称:		blit
//全名:		YSLib::Drawing::blit
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	复制一块矩形区域的像素。
//备注:		
//********************************
void
blit(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blitH
//全名:		YSLib::Drawing::blitH
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
//备注:		
//********************************
void
blitH(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blitV
//全名:		YSLib::Drawing::blitV
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
//备注:		
//********************************
void
blitV(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);
//********************************
//名称:		blitU
//全名:		YSLib::Drawing::blitU
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	倒置复制一块矩形区域的像素。
//备注:		
//********************************
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

//********************************
//名称:		blit2
//全名:		YSLib::Drawing::blit2
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	复制一块矩形区域的像素。
//备注:		使用第 15 位表示透明性。
//********************************
void
blit2(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blit2H
//全名:		YSLib::Drawing::blit2H
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
//备注:		使用第 15 位表示透明性。
//********************************
void
blit2H(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blit2V
//全名:		YSLib::Drawing::blit2V
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
//备注:		使用第 15 位表示透明性。
//********************************
void
blit2V(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blit2U
//全名:		YSLib::Drawing::blit2U
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	倒置复制一块矩形区域的像素。
//备注:		使用第 15 位表示透明性。
//********************************
void
blit2U(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blit2
//全名:		YSLib::Drawing::blit2
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const u8 * srcA
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	复制一块矩形区域的像素。
//备注:		使用 Alpha 通道表示透明性。
//********************************
void
blit2(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blit2H
//全名:		YSLib::Drawing::blit2H
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const u8 * srcA
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
//备注:		使用 Alpha 通道表示透明性。
//********************************
void
blit2H(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blit2V
//全名:		YSLib::Drawing::blit2V
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const u8 * srcA
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
//备注:		使用 Alpha 通道表示透明性。
//********************************
void
blit2V(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blit2U
//全名:		YSLib::Drawing::blit2U
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const u8 * srcA
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	倒置复制一块矩形区域的像素。
//备注:		使用 Alpha 通道表示透明性。
//********************************
void
blit2U(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blitAlpha
//全名:		YSLib::Drawing::blitAlpha
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const u8 * srcA
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	复制一块矩形区域的像素。
//备注:		使用 Alpha 通道表示 8 位透明度。
//********************************
void
blitAlpha(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blitAlphaH
//全名:		YSLib::Drawing::blitAlphaH
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const u8 * srcA
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	水平翻转镜像（关于水平中轴对称）复制一块矩形区域的像素。
//备注:		使用 Alpha 通道表示 8 位透明度。
//********************************
void
blitAlphaH(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blitAlphaV
//全名:		YSLib::Drawing::blitAlphaV
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const u8 * srcA
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	竖直翻转镜像（关于竖直中轴对称）复制一块矩形区域的像素。
//备注:		使用 Alpha 通道表示 8 位透明度。
//********************************
void
blitAlphaV(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);

//********************************
//名称:		blitAlphaU
//全名:		YSLib::Drawing::blitAlphaU
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr dst
//形式参数:	const Size & ds
//形式参数:	ConstBitmapPtr src
//形式参数:	const u8 * srcA
//形式参数:	const Size & ss
//形式参数:	const Point & sp
//形式参数:	const Point & dp
//形式参数:	const Size & sc
//功能概要:	倒置复制一块矩形区域的像素。
//备注:		使用 Alpha 通道表示 8 位透明度。
//********************************
void
blitAlphaU(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc);


//图形接口上下文。
class Graphics
{
private:
	BitmapPtr pBuffer;
	Drawing::Size Size;

public:
	//********************************
	//名称:		Graphics
	//全名:		YSLib::Drawing::Graphics::Graphics
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	BitmapPtr
	//形式参数:	const Drawing::Size &
	//功能概要:	构造：使用指定位图指针和大小。
	//备注:		
	//********************************
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

//********************************
//名称:		PutPixel
//全名:		YSLib::Drawing::PutPixel
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	SPOS x
//形式参数:	SPOS y
//形式参数:	Color c
//功能概要:	绘制像素：(x, y) 。
//前置条件: 断言 g.IsValid && Rect(g.GetSize()).Contains(x, y) 。
//备注:		
//********************************
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

//********************************
//名称:		DrawPoint
//全名:		YSLib::Drawing::DrawPoint
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	SPOS x
//形式参数:	SPOS y
//形式参数:	Color c
//功能概要:	绘制点：p(x, y) 。
//备注:		
//********************************
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
//********************************
//名称:		DrawPoint
//全名:		YSLib::Drawing::DrawPoint
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	const Point & p
//形式参数:	Color c
//功能概要:	绘制点：p 。
//备注:		
//********************************
inline bool
DrawPoint(const Graphics& g, const Point& p, Color c)
{
	return DrawPoint(g, p.X, p.Y, c);
}

//********************************
//名称:		DrawHLineSeg
//全名:		YSLib::Drawing::DrawHLineSeg
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	SPOS y
//形式参数:	SPOS x1
//形式参数:	SPOS x2
//形式参数:	Color c
//功能概要:	绘制水平线段：指定端点水平坐标 x1 、 x2 - 1，竖直坐标 y 。
//备注:		
//********************************
bool
DrawHLineSeg(const Graphics& g, SPOS y, SPOS x1, SPOS x2, Color c);

//********************************
//名称:		DrawVLineSeg
//全名:		YSLib::Drawing::DrawVLineSeg
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	SPOS x
//形式参数:	SPOS y1
//形式参数:	SPOS y2
//形式参数:	Color c
//功能概要:	绘制竖直线段：指定竖直水平坐标 x ，竖直坐标 y1 - 1 、 y2 。
//备注:		
//********************************
bool
DrawVLineSeg(const Graphics& g, SPOS x, SPOS y1, SPOS y2, Color c);

//********************************
//名称:		DrawLineSeg
//全名:		YSLib::Drawing::DrawLineSeg
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	SPOS x1
//形式参数:	SPOS y1
//形式参数:	SPOS x2
//形式参数:	SPOS y2
//形式参数:	Color c
//功能概要:	绘制一般线段：端点 p1(x1, y1), p2(x2, y2) 。
//备注:		
//********************************
bool
DrawLineSeg(const Graphics& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, Color c);
//********************************
//名称:		DrawLineSeg
//全名:		YSLib::Drawing::DrawLineSeg
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	const Point & p1
//形式参数:	const Point & p2
//形式参数:	Color c
//功能概要:	绘制一般线段：端点 p1, p2 。
//备注:		
//********************************
inline bool
DrawLineSeg(const Graphics& g, const Point& p1, const Point& p2, Color c)
{
	return DrawLineSeg(g, p1.X, p1.Y, p2.X, p2.Y, c);
}

//********************************
//名称:		DrawRect
//全名:		YSLib::Drawing::DrawRect
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	const Point & p
//形式参数:	const Size & s
//形式参数:	Color c
//功能概要:	绘制空心正则矩形。
//备注:		
//********************************
bool
DrawRect(const Graphics& g, const Point& p, const Size& s, Color c);
//********************************
//名称:		DrawRect
//全名:		YSLib::Drawing::DrawRect
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	const Rect & r
//形式参数:	Color c
//功能概要:	绘制空心正则矩形。
//备注:		
//********************************
inline bool
DrawRect(const Graphics& g, const Rect& r, Color c)
{
	return DrawRect(g, r, r, c);
}

//********************************
//名称:		FillRect
//全名:		YSLib::Drawing::FillRect
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	const Point & p
//形式参数:	const Size & s
//形式参数:	Color c
//功能概要:	绘制实心正则矩形。
//备注:		
//********************************
bool
FillRect(const Graphics& g, const Point& p, const Size& s, Color c);
//********************************
//名称:		FillRect
//全名:		YSLib::Drawing::FillRect
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	const Rect & r
//形式参数:	Color c
//功能概要:	绘制实心正则矩形。
//备注:		
//********************************
inline bool
FillRect(const Graphics& g, const Rect& r, Color c)
{
	return FillRect(g, r, r, c);
}

//********************************
//名称:		TransformRect
//全名:		YSLib::Drawing<_fTransformPixel>::TransformRect
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	const Point & p
//形式参数:	const Size & s
//形式参数:	_fTransformPixel tp
//功能概要:	以像素为单位变换正则矩形。
//备注:		
//********************************
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
//********************************
//名称:		TransformRect
//全名:		YSLib::Drawing<_fTransformPixel>::TransformRect
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const Graphics & g
//形式参数:	const Rect & r
//形式参数:	_fTransformPixel tp
//功能概要:	以像素为单位变换正则矩形。
//备注:		
//********************************
template<class _fTransformPixel>
inline bool
TransformRect(const Graphics& g, const Rect& r, _fTransformPixel tp)
{
	return TransformRect<_fTransformPixel>(g, r, r, tp);
}


// GDI 逻辑对象。

class PenStyle //笔样式：字体和笔颜色。
{
public:
	Drawing::Font Font; //字体。
	Drawing::Color Color; //笔颜色。

	//********************************
	//名称:		PenStyle
	//全名:		YSLib::Drawing::PenStyle::PenStyle
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const FontFamily &
	//形式参数:	Font::SizeType
	//形式参数:	Drawing::Color
	//功能概要:	构造：使用指定字体家族、字体大小和颜色。
	//备注:		
	//********************************
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


struct Padding //边距样式。
{
	SDST Left, Right, Top, Bottom; //边距：左、右、上、下。

	//********************************
	//名称:		Padding
	//全名:		YSLib::Drawing::Padding::Padding
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	SDST
	//形式参数:	SDST
	//形式参数:	SDST
	//形式参数:	SDST
	//功能概要:	构造：使用 4 个 16 位无符号整数形式的边距。
	//备注:		
	//********************************
	explicit
	Padding(SDST = 4, SDST = 4, SDST = 4, SDST = 4);

	//********************************
	//名称:		operator+=
	//全名:		YSLib::Drawing::Padding::operator+=
	//可访问性:	public 
	//返回类型:	Padding&
	//修饰符:	
	//形式参数:	const Padding &
	//功能概要:	加法赋值：对应分量调用 operator+= 。
	//备注:		
	//********************************
	Padding&
	operator+=(const Padding&);

};


//********************************
//名称:		operator+
//全名:		YSLib::Drawing::operator+
//可访问性:	public 
//返回类型:	friend Padding
//修饰符:	
//形式参数:	const Padding & a
//形式参数:	const Padding & b
//功能概要:	加法：对应分量调用 operator+ 。
//备注:		
//********************************
Padding
operator+(const Padding& a, const Padding& b);


//********************************
//名称:		GetHorizontalFrom
//全名:		YSLib::Drawing::GetHorizontalFrom
//可访问性:	public 
//返回类型:	YSLib::SDST
//修饰符:	
//形式参数:	const Padding & p
//功能概要:	取水平边距和。
//备注:		
//********************************
inline SDST
GetHorizontalFrom(const Padding& p)
{
	return p.Left + p.Right;
}

//********************************
//名称:		GetVerticalFrom
//全名:		YSLib::Drawing::GetVerticalFrom
//可访问性:	public 
//返回类型:	YSLib::SDST
//修饰符:	
//形式参数:	const Padding & p
//功能概要:	取竖直边距和。
//备注:		
//********************************
inline SDST
GetVerticalFrom(const Padding& p)
{
	return p.Top + p.Bottom;
}

//********************************
//名称:		GetAllFrom
//全名:		YSLib::Drawing::GetAllFrom
//可访问性:	public 
//返回类型:	u64
//修饰符:	
//形式参数:	const Padding &
//功能概要:	取边距。
//备注:		64 位无符号整数形式。
//********************************
u64
GetAllFrom(const Padding&);

//********************************
//名称:		SetAllTo
//全名:		YSLib::Drawing::SetAllTo
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	Padding &
//形式参数:	SDST
//形式参数:	SDST
//形式参数:	SDST
//形式参数:	SDST
//功能概要:	设置边距。
//备注:		4 个 16 位无符号整数形式。
//********************************
void
SetAllTo(Padding&, SDST, SDST, SDST, SDST);
//********************************
//名称:		SetAllTo
//全名:		YSLib::Drawing::SetAllTo
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	Padding & p
//形式参数:	u64 m
//功能概要:	设置边距。
//备注:		64 位无符号整数形式。
//********************************
inline void
SetAllTo(Padding& p, u64 m)
{
	SetAllTo(p, m >> 48, (m >> 32) & 0xFFFF, (m >> 16) & 0xFFFF, m & 0xFFFF);
}
//********************************
//名称:		SetAllTo
//全名:		YSLib::Drawing::SetAllTo
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	Padding & p
//形式参数:	SDST h
//形式参数:	SDST v
//功能概要:	设置边距。
//备注:		2 个 16 位无符号整数形式，分别表示水平边距和竖直边距。
//********************************
inline void
SetAllTo(Padding& p, SDST h, SDST v)
{
	SetAllTo(p, h, h, v, v);
}


//矩形图像缓冲区。
class BitmapBuffer : public Size
{
protected:
	BitmapPtr img; //显示缓冲区指针。

public:
	//********************************
	//名称:		BitmapBuffer
	//全名:		YSLib::Drawing::BitmapBuffer::BitmapBuffer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		零初始化。
	//********************************
	BitmapBuffer();
	//********************************
	//名称:		BitmapBuffer
	//全名:		YSLib::Drawing::BitmapBuffer::BitmapBuffer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	ConstBitmapPtr
	//形式参数:	SDST
	//形式参数:	SDST
	//功能概要:	构造：使用指定位图指针和大小。
	//备注:		
	//********************************
	BitmapBuffer(ConstBitmapPtr, SDST, SDST);
	//********************************
	//名称:		~BitmapBuffer
	//全名:		YSLib::Drawing::BitmapBuffer::~BitmapBuffer
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构：释放资源。
	//备注:		
	//********************************
	virtual
	~BitmapBuffer();

	//********************************
	//名称:		operator==
	//全名:		YSLib::Drawing::operator==
	//可访问性:	public 
	//返回类型:	friend bool
	//修饰符:	
	//形式参数:	const BitmapBuffer &
	//形式参数:	const BitmapBuffer &
	//功能概要:	比较：相等关系。
	//备注:		
	//********************************
	friend bool
	operator==(const BitmapBuffer&, const BitmapBuffer&);

	DefConverter(Graphics, Graphics(img, *this)) //生成图形接口上下文。

	DefGetter(BitmapPtr, BufferPtr, img) //取缓冲区指针。
	DefGetter(std::size_t, SizeOfBuffer, sizeof(PixelType) * GetAreaFrom(*this)) \
		//取缓冲区占用空间。

	//********************************
	//名称:		SetSize
	//全名:		YSLib::Drawing::BitmapBuffer::SetSize
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	SPOS
	//形式参数:	SPOS
	//功能概要:	重新设置缓冲区大小。
	//备注:		
	//********************************
	virtual void
	SetSize(SPOS, SPOS);

	//********************************
	//名称:		SetSizeSwap
	//全名:		YSLib::Drawing::BitmapBuffer::SetSizeSwap
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	交换宽和高；同时交换边距。
	//备注:		
	//********************************
	virtual void
	SetSizeSwap();

	//********************************
	//名称:		ClearImage
	//全名:		YSLib::Drawing::BitmapBuffer::ClearImage
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	const
	//功能概要:	清除缓冲区。
	//备注:		
	//********************************
	virtual void
	ClearImage() const;

	//********************************
	//名称:		BeFilledWith
	//全名:		YSLib::Drawing::BitmapBuffer::BeFilledWith
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	Color
	//功能概要:	以纯色填充显示缓冲区。
	//备注:		
	//********************************
	virtual void
	BeFilledWith(Color) const;

	//********************************
	//名称:		CopyToBuffer
	//全名:		YSLib::Drawing::BitmapBuffer::CopyToBuffer
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	BitmapPtr
	//形式参数:	ROT
	//形式参数:	const Size & ds
	//形式参数:	const Point & sp
	//形式参数:	const Point & dp
	//形式参数:	const Size & sc
	//功能概要:	按指定角度复制缓冲区内容至指定大小和指定点的指定缓存。
	//备注:		
	//********************************
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


//矩形增强图像缓冲区。
class BitmapBufferEx : public BitmapBuffer
{
protected:
	u8* imgAlpha; // Alpha 缓冲区指针。

public:
	//********************************
	//名称:		BitmapBufferEx
	//全名:		YSLib::Drawing::BitmapBufferEx::BitmapBufferEx
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		零初始化。
	//********************************
	BitmapBufferEx();
	//********************************
	//名称:		BitmapBufferEx
	//全名:		YSLib::Drawing::BitmapBufferEx::BitmapBufferEx
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	ConstBitmapPtr
	//形式参数:	SDST
	//形式参数:	SDST
	//功能概要:	构造：使用指定位图指针和大小。
	//备注:		
	//********************************
	BitmapBufferEx(ConstBitmapPtr, SDST, SDST);
	//********************************
	//名称:		~BitmapBufferEx
	//全名:		YSLib::Drawing::BitmapBufferEx::~BitmapBufferEx
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构：释放资源。
	//备注:		
	//********************************
	virtual
	~BitmapBufferEx();

	//********************************
	//名称:		operator==
	//全名:		YSLib::Drawing::operator==
	//可访问性:	public 
	//返回类型:	friend bool
	//修饰符:	
	//形式参数:	const BitmapBufferEx &
	//形式参数:	const BitmapBufferEx &
	//功能概要:	比较：相等关系。
	//备注:		
	//********************************
	friend bool
	operator==(const BitmapBufferEx&, const BitmapBufferEx&);

	DefGetter(u8*, BufferAlphaPtr, imgAlpha) //取 Alpha 缓冲区的指针。
	DefGetter(std::size_t, SizeOfBufferAlpha, sizeof(u8) * GetAreaFrom(*this)) \
		//取 Alpha 缓冲区占用空间。

	//********************************
	//名称:		SetSize
	//全名:		YSLib::Drawing::BitmapBufferEx::SetSize
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	SPOS
	//形式参数:	SPOS
	//功能概要:	重新设置缓冲区大小。
	//备注:		
	//********************************
	virtual void
	SetSize(SPOS, SPOS);
	//********************************
	//名称:		SetSizeSwap
	//全名:		YSLib::Drawing::BitmapBufferEx::SetSizeSwap
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	交换宽和高；同时交换边距。
	//备注:		
	//********************************
	virtual void
	SetSizeSwap();

	//********************************
	//名称:		ClearImage
	//全名:		YSLib::Drawing::BitmapBufferEx::ClearImage
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	const
	//功能概要:	清除缓冲区。
	//备注:		
	//********************************
	virtual void
	ClearImage() const;

	//********************************
	//名称:		CopyToBuffer
	//全名:		YSLib::Drawing::BitmapBufferEx::CopyToBuffer
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	BitmapPtr
	//形式参数:	ROT
	//形式参数:	const Size & ds
	//形式参数:	const Point & sp
	//形式参数:	const Point & dp
	//形式参数:	const Size & sc
	//功能概要:	按指定角度复制缓冲区内容至指定大小和指定点的指定缓存。
	//备注:		
	//********************************
	void
	CopyToBuffer(BitmapPtr, ROT = RDeg0, const Size& ds = Size::FullScreen,
		const Point& sp = Point::Zero, const Point& dp = Point::Zero,
		const Size& sc = Size::FullScreen) const;

	//********************************
	//名称:		BlitToBuffer
	//全名:		YSLib::Drawing::BitmapBufferEx::BlitToBuffer
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	BitmapPtr
	//形式参数:	ROT
	//形式参数:	const Size & ds
	//形式参数:	const Point & sp
	//形式参数:	const Point & dp
	//形式参数:	const Size & sc
	//功能概要:	按指定角度贴图缓冲区内容至指定大小和指定点的指定缓存。
	//备注:		
	//********************************
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

