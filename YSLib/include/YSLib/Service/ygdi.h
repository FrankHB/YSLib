﻿/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.h
\ingroup Service
\brief 平台无关的图形设备接口。
\version r4063;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-14 18:29:46 +0800;
\par 修改时间:
	2011-09-11 21:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YGDI;
*/


#ifndef YSL_INC_SERVICE_YGDI_H_
#define YSL_INC_SERVICE_YGDI_H_

#include "../Core/ygdibase.h"
#include "../Adaptor/yfont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

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
operator+(const Padding&, const Padding&);
/*!
\brief 加法：缩小屏幕正则矩形，相对位置由指定边距决定。
\note 若边距过大，则矩形的宽或高可能为 0 。
*/
Rect
operator+(const Rect&, const Padding&);


/*!
\brief 取水平边距和。
*/
inline SDst
GetHorizontalOf(const Padding& m)
{
	return m.Left + m.Right;
}

/*!
\brief 取竖直边距和。
*/
inline SDst
GetVerticalOf(const Padding& m)
{
	return m.Top + m.Bottom;
}

/*!
\brief 取边距。
\note 64 位无符号整数形式。
*/
u64
GetAllOf(const Padding&);

/*!
\brief 设置边距。
\note 4 个 16 位无符号整数形式。
*/
void
SetAllOf(Padding&, SDst, SDst, SDst, SDst);
/*!
\brief 设置边距。
\note 64 位无符号整数形式。
*/
inline void
SetAllOf(Padding& m, u64 u)
{
	SetAllOf(m, u >> 48, (u >> 32) & 0xFFFF, (u >> 16) & 0xFFFF, u & 0xFFFF);
}
/*!
\brief 设置边距。
\note 2 个 16 位无符号整数形式，分别表示水平边距和竖直边距。
*/
inline void
SetAllOf(Padding& m, SDst h, SDst v)
{
	SetAllOf(m, h, h, v, v);
}


/*!
\brief 取内边界相对于外边界的边距。
*/
Padding
FetchMargin(const Rect&, const Size&);


//! \brief 正则矩形位图缓冲区。
class BitmapBuffer : public Graphics
{
public:
	/*!
	\brief 无参数构造：默认实现。
	\note 零初始化。
	*/
	DefDeCtor(BitmapBuffer)
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	BitmapBuffer(ConstBitmapPtr, SDst, SDst);
	/*!
	\brief 析构：释放资源。
	*/
	BitmapBuffer(const BitmapBuffer&);
	DefDeMoveCtor(BitmapBuffer)
	virtual DefClone(BitmapBuffer, Clone)
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
	BitmapBufferEx(const BitmapBufferEx&);
	DefDeMoveCtor(BitmapBufferEx)
	virtual DefClone(BitmapBufferEx, Clone)
	virtual
	~BitmapBufferEx();

	DefGetter(u8*, BufferAlphaPtr, pBufferAlpha) //!< 取 Alpha 缓冲区的指针。
	DefGetter(size_t, SizeOfBufferAlpha, sizeof(u8) * GetAreaOf(GetSize())) \
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
