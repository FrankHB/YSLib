/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Video.h
\ingroup YCLib
\brief 平台相关的视频输出接口。
\version r1436;
\author FrankHB<frankhb1989@gmail.com>
\since build 312 。
\par 创建时间:
	2011-05-26 19:41:08 +0800;
\par 修改时间:
	2012-06-01 20:51 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::Video;
*/


#ifndef YCL_INC_VIDEO_H_
#define YCL_INC_VIDEO_H_

#include "ycommon.h"
#include <initializer_list> // for well-formedly using of std::initialize_list;

namespace platform
{

typedef std::int16_t SPos; //!< 屏幕坐标度量。
typedef std::uint16_t SDst; //!< 屏幕坐标距离。

#ifdef YCL_DS
	/*!
	\brief 标识 AXYZ1555 像素格式。
	\since build 297 。
	*/
#	define YCL_PIXEL_FORMAT_AXYZ1555

/*!
\brief LibNDS 兼容像素。
\note ABGR1555 。
*/
typedef std::uint16_t PixelType;
/*!
\brief 取像素 Alpha 值。
\since build 313 。
*/
yconstfn std::uint8_t
FetchAlpha(PixelType px)
{
	return px & 1 << 15 ? 0xFF : 0;
}

/*!
\brief 取不透明像素。
\since build 297 。
*/
yconstfn PixelType
FetchOpaque(PixelType px)
{
	return px | 1 << 15;
}

/*
\brief 使用 8 位 RGB 构造本机类型像素。
\since build 313 。
*/
yconstfn PixelType
FetchPixel(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
	return r >> 3 | std::uint16_t(g >> 3) << 5 | std::uint16_t(b >> 3) << 10;
}

#	define DefColorH_(hex, name) name = \
	(FetchPixel(((hex) >> 16) & 0xFF, ((hex) >> 8) & 0xFF, (hex) & 0xFF) \
	| 1 << 15)
#elif defined(YCL_MINGW32)
/*!
\brief Windows DIB 格式兼容像素。
\note MSDN 注明此处第 4 字节保留为 0 ，但此处使用作为 8 位 Alpha 值使用。
	即 ARGB8888 （考虑字节序为BGRA8888）。
\warning 仅用于屏幕绘制，不保证无条件兼容于所有 DIB 。
\note 转换 DIB 在设备上下文绘制时无需转换格式，比 ::COLORREF 更高效。
\since build 313 。
*/
typedef struct
{
	std::uint8_t rgbBlue;
	std::uint8_t rgbGreen;
	std::uint8_t rgbRed;
	std::uint8_t rgbReserved;
} PixelType;

/*!
\brief 取像素 Alpha 值。
\since build 297 。
*/
yconstfn std::uint8_t
FetchAlpha(PixelType px)
{
	return px.rgbReserved;
}

/*!
\brief 取不透明像素。
\since build 297 。
*/
yconstfn PixelType
FetchOpaque(PixelType px)
{
	return {px.rgbBlue, px.rgbGreen, px.rgbRed, 0xFF};
}

/*
\brief 使用 8 位 RGB 构造 std::uint32_t 像素。
\since build 313 。
*/
yconstfn std::uint32_t
FetchPixel(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
	return r | g << 8 | std::uint32_t(b) << 16;
}

/*!
\brief 定义 Windows DIB 格式兼容像素。
\note 得到的 32 位整数和 ::RGBQUAD 在布局上兼容。
\note Alpha 值为 0xFF 。
\since build 296 。
*/
#	define DefColorH_(hex, name) name = (FetchPixel((((hex) >> 16) & 0xFF), \
	(((hex) >> 8) & 0xFF), ((hex) & 0xFF)) << 8 | 0xFF)
#else
#	error Unsupported platform found!
#endif

typedef PixelType* BitmapPtr;
typedef const PixelType* ConstBitmapPtr;


//! \brief 系统默认颜色空间。
namespace ColorSpace
{
//	#define DefColorA(r, g, b, name) name = ARGB16(1, r, g, b),
#define	HexAdd0x(hex) 0x##hex
#define DefColorH(hex_, name) DefColorH_(HexAdd0x(hex_), name)

/*!
\brief 默认颜色集。
\see http://www.w3schools.com/html/html_colornames.asp 。
*/
#ifdef YCL_DS
typedef enum : PixelType
#else
typedef enum : std::uint32_t
#endif
{
	DefColorH(00FFFF, Aqua),
	DefColorH(000000, Black),
	DefColorH(0000FF, Blue),
	DefColorH(FF00FF, Fuchsia),
	DefColorH(808080, Gray),
	DefColorH(008000, Green),
	DefColorH(00FF00, Lime),
	DefColorH(800000, Maroon),
	DefColorH(000080, Navy),
	DefColorH(808000, Olive),
	DefColorH(800080, Purple),
	DefColorH(FF0000, Red),
	DefColorH(C0C0C0, Silver),
	DefColorH(008080, Teal),
	DefColorH(FFFFFF, White),
	DefColorH(FFFF00, Yellow)
} ColorSet;

#undef DefColorH
#undef DefColorH_
#undef HexAdd0x
} // namespace ColorSpace;


//! \brief 颜色。
class Color
{
public:
	typedef ColorSpace::ColorSet ColorSet;
	typedef std::uint8_t MonoType;
	typedef std::uint8_t AlphaType;

private:
	/*!
	\brief RGB 分量。
	\since build 276 。
	*/
	MonoType r, g, b;
	/*!
	\brief Alpha 分量。
	\since build 276 。
	*/
	AlphaType a;

public:
	/*!
	\brief 无参数构造：所有分量为 0 的默认颜色。
	\since build 297 。
	*/
	yconstfn
	Color()
		: r(0), g(0), b(0), a(0)
	{}
	/*!
	\brief 构造：使用本机颜色对象。
	\since build 297 。
	*/
	yconstfn
	Color(PixelType px)
#ifdef YCL_DS
		: r(px << 3 & 248), g(px >> 2 & 248), b(px >> 7 & 248),
		a(FetchAlpha(px) ? 0xFF : 0x00)
#elif defined(YCL_MINGW32)
		: r(px.rgbRed), g(px.rgbGreen), b(px.rgbBlue), a(px.rgbReserved)
#else
#	error Unsupport platform found!
#endif
	{}
#ifdef YCL_MINGW32
	/*!
	\brief 构造：使用默认颜色。
	\since build 297 。
	*/
	yconstfn
	Color(ColorSet cs)
		: r((cs & 0xFF00) >> 8), g((cs & 0xFF0000) >> 16),
		b((cs & 0xFF000000) >> 24), a(0xFF)
	{}
#endif
	/*!
	\brief 使用 RGB 值和 alpha 位构造 Color 对象。
	*/
	yconstfn
	Color(MonoType r_, MonoType g_, MonoType b_, AlphaType a_ = 0xFF)
		: r(r_), g(g_), b(b_), a(a_)
	{}

	/*!
	\brief 转换：本机颜色对象。
	*/
	yconstfn
	operator PixelType() const
	{
	#ifdef YCL_DS
		return int(a != 0) << 15 | FetchPixel(r, g, b);
	#elif defined(YCL_MINGW32)
		return {b, g, r, a};
	#else
	#	error Unsupport platform found!
	#endif
	}

	/*!
	\brief 取红色分量。
	*/
	yconstfn MonoType
	GetR() const
	{
		return r;
	}
	/*!
	\brief 取绿色分量。
	*/
	yconstfn MonoType
	GetG() const
	{
		return g;
	}
	/*!
	\brief 取蓝色分量。
	*/
	yconstfn MonoType
	GetB() const
	{
		return b;
	}
	/*!
	\brief 取 alpha 分量。
	*/
	yconstfn AlphaType
	GetA() const
	{
		return a;
	}
};


/*!
\brief 启动控制台。
\note fc 为前景色，bc为背景色。
*/
void
YConsoleInit(std::uint8_t dspIndex,
	Color fc = ColorSpace::White, Color bc = ColorSpace::Black);


/*!
\brief 初始化视频输出。
*/
bool
InitVideo();

} // namespace platform;


namespace platform_ex
{

#ifdef YCL_DS

/*!
\brief 复位屏幕显示模式。
*/
void
ResetVideo();


/*!
\brief 默认上屏初始化函数。
*/
platform::BitmapPtr
InitScrUp(int&);

/*!
\brief 默认下屏初始化函数。
*/
platform::BitmapPtr
InitScrDown(int&);

/*!
\brief 快速刷新缓存映像到显示屏缓冲区。
\note 第一参数为显示屏缓冲区，第二参数为源缓冲区。
\since build 184 。
*/
void
ScreenSynchronize(platform::PixelType*, const platform::PixelType*);
#endif

} // namespace platform_ex;

#endif

