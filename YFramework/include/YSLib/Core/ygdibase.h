/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdibase.h
\ingroup Core
\brief 平台无关的基础图形学对象。
\version r1656;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-05-03 07:20:51 +0800;
\par 修改时间:
	2011-11-04 19:22 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YGDIBase;
*/


#ifndef YSL_INC_CORE_YGDIBASE_H_
#define YSL_INC_CORE_YGDIBASE_H_

#include "yexcept.h"
#include <limits>

YSL_BEGIN

// GDI 基本数据类型和宏定义。

YSL_BEGIN_NAMESPACE(Drawing)

//前向声明。

class Size;
class Rect;


//! \brief 屏幕二元组。
PDefTH1(_type)
class GBinaryGroup
{
public:
	static const GBinaryGroup Zero; //!< 无参数构造参数构造的原点对象。
	static const GBinaryGroup Invalid; //!< 无效（不在屏幕坐标系中）对象。

	_type X, Y; //!< 分量。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	yconstexprf
	GBinaryGroup()
		: X(0), Y(0)
	{}
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstexprf DefDeCopyCtor(GBinaryGroup)
	/*!
	\brief 构造：使用 Size 对象。
	*/
	explicit yconstexprf
	GBinaryGroup(const Size&);
	/*!
	\brief 构造：使用两个标量。
	*/
	PDefTH2(_tScalar1, _tScalar2)
	yconstexprf
	GBinaryGroup(_tScalar1 x, _tScalar2 y)
		: X(x), Y(y)
	{}

	/*!
	\brief 负运算：取加法逆元。
	*/
	yconstexprf GBinaryGroup
	operator-()
	{
		return GBinaryGroup(-X, -Y);
	}

	/*!
	\brief 加法赋值。
	*/
	GBinaryGroup&
	operator+=(const GBinaryGroup& val) ynothrow
	{
		yunsequenced(X += val.X, Y += val.Y);
		return *this;
	}
	/*!
	\brief 减法赋值。
	*/
	GBinaryGroup&
	operator-=(const GBinaryGroup& val) ynothrow
	{
		yunsequenced(X -= val.X, Y -= val.Y);
		return *this;
	}

	yconstexprf DefGetter(_type, X, X)
	yconstexprf DefGetter(_type, Y, Y)

	yconstexprf DefSetter(_type, X, X)
	yconstexprf DefSetter(_type, Y, Y)

	/*!
	\biref 判断是否是零元素。
	*/
	yconstexprf DefPredicate(Zero, X == 0 && Y == 0)
};

PDefTH1(_type)
const GBinaryGroup<_type> GBinaryGroup<_type>::Zero = GBinaryGroup<_type>();
PDefTH1(_type)
const GBinaryGroup<_type> GBinaryGroup<_type>::Invalid
	= GBinaryGroup<_type>(std::numeric_limits<_type>::lowest(),
		std::numeric_limits<_type>::lowest());


//屏幕二元组二元运算。

/*!
\brief 比较：相等关系。
*/
PDefTH1(_type)
yconstexprf bool
operator==(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b)
{
	return a.X == b.X && a.Y == b.Y;
}

/*!
\brief 比较：不等关系。
*/
PDefTH1(_type)
yconstexprf bool
operator!=(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b)
{
	return !(a == b);
}

/*!
\brief 加法。
*/
PDefTH1(_type)
yconstexprf GBinaryGroup<_type>
operator+(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b)
{
	return GBinaryGroup<_type>(a.X + b.X, a.Y + b.Y);
}

/*!
\brief 减法。
*/
PDefTH1(_type)
yconstexprf GBinaryGroup<_type>
operator-(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b)
{
	return GBinaryGroup<_type>(a.X - b.X, a.Y - b.Y);
}

/*!
\brief 数乘。
*/
PDefTH2(_type, _tScalar)
yconstexprf GBinaryGroup<_type>
operator*(const GBinaryGroup<_type>& val, _tScalar l)
{
	return GBinaryGroup<_type>(val.X * l, val.Y * l);
}


//! \brief 屏幕二维点（直角坐标表示）。
typedef GBinaryGroup<SPos> Point;


//! \brief 屏幕二维向量（直角坐标表示）。
typedef GBinaryGroup<SPos> Vec;


class Size //!< 屏幕区域大小。
{
public:
	SDst Width, Height; //!< 宽和高。

	static const Size Zero; //!< 无参数构造参数构造的零元素对象。
	static const Size FullScreen; //!< 无参数构造参数构造的全屏幕对象。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	yconstexprf
	Size();
	/*!
	\brief 复制构造。
	*/
	yconstexprf
	Size(const Size&);
	/*!
	\brief 构造：使用屏幕二元组。
	*/
	PDefTH1(_type)
	yconstexprf explicit
	Size(const GBinaryGroup<_type>& val)
		: Width(val.X), Height(val.Y)
	{}
	/*!
	\brief 构造：使用两个标量。
	*/
	PDefTH2(_tScalar1, _tScalar2)
	yconstexprf
	Size(_tScalar1 w, _tScalar2 h)
		: Width(w), Height(h)
	{}

	/*!
	\brief 转换：屏幕二维向量。
	\note 以Width 和 Height 分量作为结果的 X 和 Y分量。
	*/
	yconstexprf DefConverter(Vec, Vec(Width, Height))

	/*!
	\brief 判断是否为空。
	*/
	yconstexprf DefPredicate(Empty, Width == 0 && Height == 0)
	/*!
	\brief 判断是否为空。
	*/
	yconstexprf DefPredicate(EmptyStrict, Width == 0 || Height == 0)
};

yconstexprf
Size::Size()
	: Width(0), Height(0)
{}
yconstexprf
Size::Size(const Size& s)
	: Width(s.Width), Height(s.Height)
{}

yconstexprf bool
operator==(const Size& a, const Size& b)
{
	return a.Width == b.Width && a.Height == b.Height;
}
yconstexprf bool
operator!=(const Size& a, const Size& b)
{
	return !(a == b);
}


PDefTH1(_type)
yconstexprf GBinaryGroup<_type>
operator+(GBinaryGroup<_type> val, const Size& s)
{
	return GBinaryGroup<_type>(val.X + s.Width, val.Y + s.Height);
}


/*!
\brief 选择分量。
\note 第二参数为 true 时选择第一分量，否则选择第二分量。
*/
PDefTH1(_type)
yconstexprf SPos
SelectFrom(const GBinaryGroup<_type>& obj, bool is_1st)
{
	return is_1st ? obj.X : obj.Y;
}
/*!
\brief 选择分量。
\note 第二参数为 true 时选择第一分量，否则选择第二分量。
*/
SDst
SelectFrom(const Size&, bool = true);

/*!
\brief 选择分量引用。
\note 第二参数为 true 时选择第一分量，否则选择第二分量。
*/
PDefTH1(_type)
yconstexprf _type&
SelectRefFrom(GBinaryGroup<_type>& obj, bool is_1st = true)
{
	return is_1st ? obj.X : obj.Y;
}
/*!
\brief 选择分量引用。
\note 第二参数为 true 时选择第一分量，否则选择第二分量。
*/
SDst&
SelectRefFrom(Size&, bool = true);

/*!
\brief 更新：其中的一个分量。
\note 第三参数为 true 时更新第一分量，否则更新第二分量。
*/
PDefTH1(_type)
void
UpdateTo(GBinaryGroup<_type>& obj, SPos val, bool is_1st = true)
{
	if(is_1st)
		obj.X = val;
	else
		obj.Y = val;
}
/*!
\brief 更新：其中的一个分量。
\note 第三参数为 true 时更新第一分量，否则更新第二分量。
*/
void
UpdateTo(Size&, SDst, bool = true);

/*!
\brief 二元对象转置。
*/
template<class _tBinary>
yconstexprf _tBinary
Transpose(_tBinary& obj)
{
	return _tBinary(obj.Y, obj.X);
}

/*!
\brief 取面积。
\todo 确定任意精度的返回值类型。
*/
yconstexprf u32
GetAreaOf(const Size& s)
{
	return s.Width * s.Height;
}


/*!
\brief 屏幕坐标系的正则矩形。
\note 边平行于水平直线；使用左上点屏幕坐标、宽和高表示。
*/
class Rect : public Point, public Size
{
public:
	static const Rect Empty; //!< 无参数构造参数构造的空矩形对象。
	static const Rect FullScreen; //!< 无参数构造参数构造的全屏幕矩形对象。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	yconstexprf
	Rect();
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstexprf DefDeCopyCtor(Rect)
	/*!
	\brief 构造：使用屏幕二维点。
	*/
	explicit yconstexprf
	Rect(const Point&);
	/*!
	\brief 构造：使用 Size 对象。
	*/
	explicit yconstexprf
	Rect(const Size&);
	/*!
	\brief 构造：使用屏幕二维点和 Size 对象。
	*/
	yconstexprf
	Rect(const Point&, const Size&);
	/*!
	\brief 构造：使用屏幕二维点和表示长宽的两个 SDst 值。
	*/
	yconstexprf
	Rect(const Point&, SDst, SDst);
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和 Size 对象。
	*/
	yconstexprf
	Rect(SPos, SPos, const Size&);
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和表示大小的两个 SDst 值。
	*/
	yconstexprf
	Rect(SPos, SPos, SDst, SDst);

	DefDeCopyAssignment(Rect)
	Rect&
	operator=(const Point&);
	Rect&
	operator=(const Size&);

	/*!
	\brief 判断点 (px, py) 是否在矩形内或边上。
	*/
	bool
	Contains(int px, int py) const;
	/*!
	\brief 判断点 pt 是否在矩形内或边上。
	*/
	PDefH(bool, Contains, const Point& pt) const
		ImplRet(Contains(pt.X, pt.Y))
	/*!
	\brief 判断矩形 r 是否在矩形内或边上。
	*/
	bool
	Contains(const Rect& r) const;

	/*!
	\brief 判断点 (px, py) 是否在矩形内。
	*/
	bool
	ContainsStrict(int px, int py) const;
	/*!
	\brief 判断点 pt 是否在矩形内。
	*/
	PDefH(bool, ContainsStrict, const Point& pt) const
		ImplRet(ContainsStrict(pt.X, pt.Y))
	/*!
	\brief 判断矩形 r 是否在矩形内或边上。
	*/
	bool
	ContainsStrict(const Rect& r) const;

	/*!
	\brief 取左上角位置。
	*/
	yconstexprf DefGetter(const Point&, Point, static_cast<const Point&>(*this))
	/*!
	\brief 取大小。
	*/
	yconstexprf DefGetter(const Size&, Size, static_cast<const Size&>(*this))
};

yconstexprf
Rect::Rect()
	: Point(), Size()
{}
yconstexprf
Rect::Rect(const Point& pt)
	: Point(pt), Size()
{}
yconstexprf
Rect::Rect(const Size& s)
	: Point(), Size(s)
{}
yconstexprf
Rect::Rect(const Point& pt, const Size& s)
	: Point(pt), Size(s)
{}
yconstexprf
Rect::Rect(const Point& pt, SDst w, SDst h)
	: Point(pt.X, pt.Y), Size(w, h)
{}
yconstexprf
Rect::Rect(SPos x, SPos y, const Size& s)
	: Point(x, y), Size(s.Width, s.Height)
{}
yconstexprf
Rect::Rect(SPos x, SPos y, SDst w, SDst h)
	: Point(x, y), Size(w, h)
{}

inline Rect&
Rect::operator=(const Point& pt)
{
	yunsequenced(X = pt.X, Y = pt.Y);
	return *this;
}
inline Rect&
Rect::operator=(const Size& s)
{
	yunsequenced(Width = s.Width, Height = s.Height);
	return *this;
}

yconstexprf bool
operator==(const Rect& a, const Rect& b)
{
	return a.GetPoint() == b.GetPoint() && a.GetSize() == b.GetSize();
}
yconstexprf bool
operator!=(const Rect& a, const Rect& b)
{
	return !(a == b);
}


/*!
\brief 构造屏幕正则矩形：使用正则矩形 r 和偏移向量 v 。
*/
yconstexprf Rect
operator+(const Rect& r, const Vec& v)
{
	return Rect(r.GetPoint() + v, r.GetSize());
}

/*!
\brief 构造屏幕正则矩形：使用正则矩形 r 和偏移向量的加法逆元 v 。
*/
yconstexprf Rect
operator-(const Rect& r, const Vec& v)
{
	return Rect(r.GetPoint() - v, r.GetSize());
}


/*!
\brief 求两个屏幕正则矩形的交。
\return 若相离为 Rect::Empty ，否则为包含于两个参数中的最大矩形。
*/
Rect
Intersect(const Rect&, const Rect&);

/*!
\brief 求两个屏幕正则矩形的并。
\return 包含两个参数中的最小矩形。
*/
Rect
Unite(const Rect&, const Rect&);


//! \brief 二维图形接口上下文。
class Graphics
{
public:
	static const Graphics Invalid; //!< 无效图形接口上下文。

protected:
	BitmapPtr pBuffer; //!< 显示缓冲区指针。
	Size size; //!< 缓冲区大小。

public:
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	explicit yconstexprf
	Graphics(BitmapPtr = nullptr, const Size& = Size::Zero);
	/*!
	\brief 复制构造：浅复制。
	*/
	yconstexprf
	Graphics(const Graphics&);
	/*!
	\brief 析构：空实现。
	*/
	virtual DefEmptyDtor(Graphics)

	/*!
	\brief 取指定行首元素指针。
	\pre 断言：缓冲区指针非空；参数不越界。
	\note 无异常抛出。
	*/
	BitmapPtr
	operator[](size_t) const ynothrow;

	DefPredicate(Valid, pBuffer && size.Width != 0 && size.Height != 0)

	DefGetter(BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const Size&, Size, size)
	DefGetter(SDst, Width, size.Width)
	DefGetter(SDst, Height, size.Height)
	DefGetter(size_t, SizeOfBuffer,
		sizeof(PixelType) * GetAreaOf(size)) //!< 取缓冲区占用空间。

	/*!
	\brief 取指定行首元素指针。
	\throw GeneralEvent 缓冲区指针为空。
	\throw std::out_of_range 参数越界。
	\note 仅抛出以上异常。
	*/
	BitmapPtr
	at(size_t) const ythrow(GeneralEvent, std::out_of_range);
};

yconstexprf
Graphics::Graphics(BitmapPtr b, const Size& s)
	: pBuffer(b), size(s)
{}
yconstexprf
Graphics::Graphics(const Graphics& g)
	: pBuffer(g.pBuffer), size(g.size)
{}


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

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

