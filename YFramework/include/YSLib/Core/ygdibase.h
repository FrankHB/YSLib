/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdibase.h
\ingroup Core
\brief 平台无关的基础图形学对象。
\version r1821;
\author FrankHB<frankhb1989@gmail.com>
\since build 206 。
\par 创建时间:
	2011-05-03 07:20:51 +0800;
\par 修改时间:
	2012-03-21 19:11 +0800;
\par 文本编码:
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


/*!
\brief 屏幕二元组。
\since build 242 。
*/
PDefTmplH1(_type)
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
	yconstfn
	GBinaryGroup()
		: X(0), Y(0)
	{}
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstfn DefDeCopyCtor(GBinaryGroup)
	/*!
	\brief 构造：使用 Size 对象。
	*/
	explicit yconstfn
	GBinaryGroup(const Size&);
	/*!
	\brief 构造：使用两个标量。
	*/
	PDefTmplH2(_tScalar1, _tScalar2)
	yconstfn
	GBinaryGroup(_tScalar1 x, _tScalar2 y)
		: X(x), Y(y)
	{}

	/*!
	\brief 负运算：取加法逆元。
	*/
	yconstfn GBinaryGroup
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
		yunseq(X += val.X, Y += val.Y);
		return *this;
	}
	/*!
	\brief 减法赋值。
	*/
	GBinaryGroup&
	operator-=(const GBinaryGroup& val) ynothrow
	{
		yunseq(X -= val.X, Y -= val.Y);
		return *this;
	}

	yconstfn DefGetter(const ynothrow, _type, X, X)
	yconstfn DefGetter(const ynothrow, _type, Y, Y)

	yconstfn DefSetter(_type, X, X)
	yconstfn DefSetter(_type, Y, Y)

	/*!
	\biref 判断是否是零元素。
	*/
	yconstfn DefPred(const ynothrow, Zero, X == 0 && Y == 0)

	/*!
	\brief 选择分量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 268 。
	*/
	_type&
	GetRef(bool b = true)
	{
		return b ? this->X : this->Y;
	}
	/*!
	\brief 选择分量常量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 268 。
	*/
	const _type&
	GetRef(bool b = true) const
	{
		return b ? this->X : this->Y;
	}
};

PDefTmplH1(_type)
const GBinaryGroup<_type> GBinaryGroup<_type>::Zero = GBinaryGroup<_type>();
PDefTmplH1(_type)
const GBinaryGroup<_type> GBinaryGroup<_type>::Invalid
	= GBinaryGroup<_type>(std::numeric_limits<_type>::lowest(),
		std::numeric_limits<_type>::lowest());


//屏幕二元组二元运算。

/*!
\brief 比较：屏幕二元组相等关系。
\since build 242 。
*/
PDefTmplH1(_type)
yconstfn bool
operator==(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b)
{
	return a.X == b.X && a.Y == b.Y;
}

/*!
\brief 比较：屏幕二元组不等关系。
\since build 242 。
*/
PDefTmplH1(_type)
yconstfn bool
operator!=(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b)
{
	return !(a == b);
}

/*!
\brief 加法：屏幕二元组。
\since build 242 。
*/
PDefTmplH1(_type)
yconstfn GBinaryGroup<_type>
operator+(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b)
{
	return GBinaryGroup<_type>(a.X + b.X, a.Y + b.Y);
}

/*!
\brief 减法：屏幕二元组。
\since build 242 。
*/
PDefTmplH1(_type)
yconstfn GBinaryGroup<_type>
operator-(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b)
{
	return GBinaryGroup<_type>(a.X - b.X, a.Y - b.Y);
}

/*!
\brief 数乘：屏幕二元组。
\since build 242 。
*/
PDefTmplH2(_type, _tScalar)
yconstfn GBinaryGroup<_type>
operator*(const GBinaryGroup<_type>& val, _tScalar l)
{
	return GBinaryGroup<_type>(val.X * l, val.Y * l);
}


/*!
\brief 屏幕二维点（直角坐标表示）。
\since build 242 。
*/
typedef GBinaryGroup<SPos> Point;


/*!
\brief 屏幕二维向量（直角坐标表示）。
\since build 242 。
*/
typedef GBinaryGroup<SPos> Vec;


/*!
\brief 屏幕区域大小。
\since build 161 。
*/
class Size
{
public:
	SDst Width, Height; //!< 宽和高。

	static const Size Zero; //!< 无参数构造参数构造的零元素对象。
	static const Size FullScreen; //!< 无参数构造参数构造的全屏幕对象。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	yconstfn
	Size();
	/*!
	\brief 复制构造。
	*/
	yconstfn
	Size(const Size&);
	/*!
	\brief 构造：使用屏幕二元组。
	*/
	PDefTmplH1(_type)
	yconstfn explicit
	Size(const GBinaryGroup<_type>& val)
		: Width(val.X), Height(val.Y)
	{}
	/*!
	\brief 构造：使用两个标量。
	*/
	PDefTmplH2(_tScalar1, _tScalar2)
	yconstfn
	Size(_tScalar1 w, _tScalar2 h)
		: Width(w), Height(h)
	{}

	/*!
	\brief 转换：屏幕二维向量。
	\note 以Width 和 Height 分量作为结果的 X 和 Y分量。
	*/
	yconstfn DefCvt(const ynothrow, Vec, Vec(Width, Height))

	/*!
	\brief 判断是否为空。
	*/
	yconstfn DefPred(const ynothrow, Empty, Width == 0 && Height == 0)
	/*!
	\brief 判断是否为线段：长或宽中有且一个数值等于 0 。
	\since build 264 。
	*/
	yconstfn DefPred(const ynothrow, LineSegment,
		!((Width == 0) ^ (Height == 0)))
	/*!
	\brief 判断是否为不严格的空矩形区域：包括空矩形和线段。
	\since build 264 。
	*/
	yconstfn DefPred(const ynothrow, UnstrictlyEmpty, Width == 0 || Height == 0)

	/*!
	\brief 选择分量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 268 。
	*/
	SDst&
	GetRef(bool b = true)
	{
		return b ? Width : Height;
	}
	/*!
	\brief 选择分量常量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 268 。
	*/
	const SDst&
	GetRef(bool b = true) const
	{
		return b ? Width : Height;
	}
};

yconstfn
Size::Size()
	: Width(0), Height(0)
{}
yconstfn
Size::Size(const Size& s)
	: Width(s.Width), Height(s.Height)
{}

/*!
\brief 比较：屏幕区域大小相等关系。
\since build 161 。
*/
yconstfn bool
operator==(const Size& a, const Size& b)
{
	return a.Width == b.Width && a.Height == b.Height;
}

/*!
\brief 比较：屏幕区域大小不等关系。
\since build 161 。
*/
yconstfn bool
operator!=(const Size& a, const Size& b)
{
	return !(a == b);
}


/*!
\brief 加法：使用屏幕二元组和屏幕区域大小分量对应相加构造屏幕二元组。
\since build 242 。
*/
PDefTmplH1(_type)
yconstfn GBinaryGroup<_type>
operator+(GBinaryGroup<_type> val, const Size& s)
{
	return GBinaryGroup<_type>(val.X + s.Width, val.Y + s.Height);
}


/*!
\brief 二元对象转置。
\since build 242 。
*/
template<class _tBinary>
yconstfn _tBinary
Transpose(_tBinary& obj)
{
	return _tBinary(obj.Y, obj.X);
}

/*!
\brief 取面积。
\since build 231 。
\todo 确定任意精度的返回值类型。
*/
yconstfn u32
GetAreaOf(const Size& s)
{
	return s.Width * s.Height;
}


/*!
\brief 屏幕正则矩形：表示屏幕矩形区域。
\note 边平行于水平直线；使用左上点屏幕坐标、宽和高表示。
\warning 非虚析构。
\since build 161 。
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
	yconstfn
	Rect();
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstfn DefDeCopyCtor(Rect)
	/*!
	\brief 构造：使用屏幕二维点。
	*/
	explicit yconstfn
	Rect(const Point&);
	/*!
	\brief 构造：使用 Size 对象。
	*/
	explicit yconstfn
	Rect(const Size&);
	/*!
	\brief 构造：使用屏幕二维点和 Size 对象。
	*/
	yconstfn
	Rect(const Point&, const Size&);
	/*!
	\brief 构造：使用屏幕二维点和表示长宽的两个 SDst 值。
	*/
	yconstfn
	Rect(const Point&, SDst, SDst);
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和 Size 对象。
	*/
	yconstfn
	Rect(SPos, SPos, const Size&);
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和表示大小的两个 SDst 值。
	*/
	yconstfn
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
	yconstfn DefGetter(const ynothrow, const Point&, Point,
		static_cast<const Point&>(*this))
	/*!
	\brief 取大小。
	*/
	yconstfn DefGetter(const ynothrow, const Size&, Size,
		static_cast<const Size&>(*this))
};

yconstfn
Rect::Rect()
	: Point(), Size()
{}
yconstfn
Rect::Rect(const Point& pt)
	: Point(pt), Size()
{}
yconstfn
Rect::Rect(const Size& s)
	: Point(), Size(s)
{}
yconstfn
Rect::Rect(const Point& pt, const Size& s)
	: Point(pt), Size(s)
{}
yconstfn
Rect::Rect(const Point& pt, SDst w, SDst h)
	: Point(pt.X, pt.Y), Size(w, h)
{}
yconstfn
Rect::Rect(SPos x, SPos y, const Size& s)
	: Point(x, y), Size(s.Width, s.Height)
{}
yconstfn
Rect::Rect(SPos x, SPos y, SDst w, SDst h)
	: Point(x, y), Size(w, h)
{}

inline Rect&
Rect::operator=(const Point& pt)
{
	yunseq(X = pt.X, Y = pt.Y);
	return *this;
}
inline Rect&
Rect::operator=(const Size& s)
{
	yunseq(Width = s.Width, Height = s.Height);
	return *this;
}

/*!
\brief 比较：屏幕正则矩形相等关系。
\since build 161 。
*/
yconstfn bool
operator==(const Rect& a, const Rect& b)
{
	return a.GetPoint() == b.GetPoint() && a.GetSize() == b.GetSize();
}

/*!
\brief 比较：屏幕正则矩形不等关系。
\since build 161 。
*/

yconstfn bool
operator!=(const Rect& a, const Rect& b)
{
	return !(a == b);
}


/*!
\brief 加法：使用正则矩形 r 和偏移向量 v 构造屏幕正则矩形。
\since build 161 。
*/
yconstfn Rect
operator+(const Rect& r, const Vec& v)
{
	return Rect(r.GetPoint() + v, r.GetSize());
}

/*!
\brief 减法：使用正则矩形 r 和偏移向量的加法逆元 v 构造屏幕正则矩形。
\since build 161 。
*/
yconstfn Rect
operator-(const Rect& r, const Vec& v)
{
	return Rect(r.GetPoint() - v, r.GetSize());
}


/*!
\brief 求两个屏幕正则矩形的交。
\return 若相离为 Rect::Empty ，否则为包含于两个参数中的最大矩形。
\since build 225 。
*/
Rect
Intersect(const Rect&, const Rect&);

/*!
\brief 求两个屏幕正则矩形的并。
\return 包含两个参数中的最小矩形。
\since build 225 。
*/
Rect
Unite(const Rect&, const Rect&);


/*!
\brief 二维图形接口上下文。
\since build 164 。
*/
class Graphics
{
public:
	static const Graphics Invalid; //!< 无效图形接口上下文。

protected:
	/*!
	\brief 显示缓冲区指针。
	\warning 不应视为具有所有权。
	*/
	BitmapPtr pBuffer;
	Size size; //!< 缓冲区大小。

public:
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	explicit yconstfn
	Graphics(BitmapPtr = nullptr, const Size& = Size::Zero);
	/*!
	\brief 复制构造：浅复制。
	*/
	yconstfn
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

	DefPred(const ynothrow, Valid,
		pBuffer && size.Width != 0 && size.Height != 0)

	DefGetter(const ynothrow, BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const ynothrow, const Size&, Size, size)
	DefGetter(const ynothrow, SDst, Width, size.Width)
	DefGetter(const ynothrow, SDst, Height, size.Height)
	DefGetter(const ynothrow, size_t, SizeOfBuffer,
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

yconstfn
Graphics::Graphics(BitmapPtr b, const Size& s)
	: pBuffer(b), size(s)
{}
yconstfn
Graphics::Graphics(const Graphics& g)
	: pBuffer(g.pBuffer), size(g.size)
{}


/*!
\brief 逆时针旋转角度指示输出指向。
\since build 178 。
*/
typedef enum
{
	RDeg0 = 0,
	RDeg90 = 1,
	RDeg180 = 2,
	RDeg270 = 3
} Rotation;

/*!
\brief 二元方向。
\since build 170 。
*/
typedef enum
{
	Horizontal = 0,
	Vertical = 1
} Orientation;

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

