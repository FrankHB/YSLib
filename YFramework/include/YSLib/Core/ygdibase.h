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
\version r1146;
\author FrankHB<frankhb1989@gmail.com>
\since build 206 。
\par 创建时间:
	2011-05-03 07:20:51 +0800;
\par 修改时间:
	2012-08-30 20:10 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YGDIBase;
*/


#ifndef YSL_INC_CORE_YGDIBASE_H_
#define YSL_INC_CORE_YGDIBASE_H_ 1

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
template<typename _type>
class GBinaryGroup
{
public:
	static yconstexpr GBinaryGroup Zero{}; //!< 无参数构造参数构造的原点对象。
	static yconstexpr GBinaryGroup Invalid{std::numeric_limits<_type>::lowest(),
		std::numeric_limits<_type>::lowest()}; //!< 无效（不在屏幕坐标系中）对象。

	_type X, Y; //!< 分量。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	\since build 319 。
	*/
	yconstfn
	GBinaryGroup() ynothrow
		: X(0), Y(0)
	{}
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstfn DefDeCopyCtor(GBinaryGroup)
	/*!
	\brief 构造：使用 Size 对象。
	\since build 319 。
	*/
	explicit yconstfn
	GBinaryGroup(const Size&) ynothrow;
	/*!
	\brief 构造：使用 Rect 对象。
	\since build 319 。
	*/
	explicit yconstfn
	GBinaryGroup(const Rect&) ynothrow;
	/*!
	\brief 构造：使用两个标量。
	\since build 319 。
	*/
	template<typename _tScalar1, typename _tScalar2>
	yconstfn
	GBinaryGroup(_tScalar1 x, _tScalar2 y) ynothrow
		: X(x), Y(y)
	{}

	/*!
	\brief 负运算：取加法逆元。
	\since build 319 。
	*/
	yconstfn GBinaryGroup
	operator-() ynothrow
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
	\since build 319 。
	*/
	_type&
	GetRef(bool b = true) ynothrow
	{
		return b ? X : Y;
	}
	/*!
	\brief 选择分量常量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 319 。
	*/
	const _type&
	GetRef(bool b = true) const ynothrow
	{
		return b ? X : Y;
	}
};

template<typename _type>
yconstexpr GBinaryGroup<_type> GBinaryGroup<_type>::Zero;
template<typename _type>
yconstexpr GBinaryGroup<_type> GBinaryGroup<_type>::Invalid;


//屏幕二元组二元运算。

/*!
\brief 比较：屏幕二元组相等关系。
\since build 319 。
*/
template<typename _type>
yconstfn bool
operator==(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b) ynothrow
{
	return a.X == b.X && a.Y == b.Y;
}

/*!
\brief 比较：屏幕二元组不等关系。
\since build 319 。
*/
template<typename _type>
yconstfn bool
operator!=(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b) ynothrow
{
	return !(a == b);
}

/*!
\brief 加法：屏幕二元组。
\since build 319 。
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator+(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b) ynothrow
{
	return GBinaryGroup<_type>(a.X + b.X, a.Y + b.Y);
}

/*!
\brief 减法：屏幕二元组。
\since build 319 。
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator-(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b) ynothrow
{
	return GBinaryGroup<_type>(a.X - b.X, a.Y - b.Y);
}

/*!
\brief 数乘：屏幕二元组。
\since build 319 。
*/
template<typename _type, typename _tScalar>
yconstfn GBinaryGroup<_type>
operator*(const GBinaryGroup<_type>& val, _tScalar l) ynothrow
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
	static const Size Zero; //!< 无参数构造的零元素对象。
	/*!
	\brief 无效对象。
	\since build 296 。
	*/
	static const Size Invalid;

	SDst Width, Height; //!< 宽和高。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	\since build 319 。
	*/
	yconstfn
	Size() ynothrow
		: Width(0), Height(0)
	{}
	/*!
	\brief 复制构造。
	\since build 319 。
	*/
	yconstfn
	Size(const Size& s) ynothrow
		: Width(s.Width), Height(s.Height)
	{}
	/*!
	\brief 构造：使用 Rect 对象。
	\since build 319 。
	*/
	explicit yconstfn
	Size(const Rect&) ynothrow;
	/*!
	\brief 构造：使用屏幕二元组。
	\since build 319 。
	*/
	template<typename _type>
	yconstfn explicit
	Size(const GBinaryGroup<_type>& val) ynothrow
		: Width(val.X), Height(val.Y)
	{}
	/*!
	\brief 构造：使用两个标量。
	\since build 319 。
	*/
	template<typename _tScalar1, typename _tScalar2>
	yconstfn
	Size(_tScalar1 w, _tScalar2 h) ynothrow
		: Width(w), Height(h)
	{}

	/*!
	\brief 判断是否为空。
	\since build 320 。
	*/
	yconstfn PDefHOp(bool, !) const ynothrow
		ImplRet(Width == 0 && Height == 0)

	/*!
	\brief 判断是否非空。
	\since build 320 。
	*/
	explicit yconstfn DefCvt(const ynothrow, bool, Width != 0 || Height != 0)

	/*!
	\brief 转换：屏幕二维向量。
	\note 以Width 和 Height 分量作为结果的 X 和 Y分量。
	*/
	yconstfn DefCvt(const ynothrow, Vec, Vec(Width, Height))

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
	\since build 319 。
	*/
	SDst&
	GetRef(bool b = true) ynothrow
	{
		return b ? Width : Height;
	}
	/*!
	\brief 选择分量常量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 319 。
	*/
	const SDst&
	GetRef(bool b = true) const ynothrow
	{
		return b ? Width : Height;
	}
};

/*!
\brief 比较：屏幕区域大小相等关系。
\since build 319 。
*/
yconstfn bool
operator==(const Size& a, const Size& b) ynothrow
{
	return a.Width == b.Width && a.Height == b.Height;
}

/*!
\brief 比较：屏幕区域大小不等关系。
\since build 319 。
*/
yconstfn bool
operator!=(const Size& a, const Size& b) ynothrow
{
	return !(a == b);
}


/*!
\brief 加法：使用屏幕二元组和屏幕区域大小分量对应相加构造屏幕二元组。
\since build 319 。
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator+(GBinaryGroup<_type> val, const Size& s) ynothrow
{
	return GBinaryGroup<_type>(val.X + s.Width, val.Y + s.Height);
}


/*!
\brief 二元对象转置。
\since build 319 。
*/
template<class _tBinary>
yconstfn _tBinary
Transpose(_tBinary& obj) ynothrow
{
	return _tBinary(obj.Y, obj.X);
}

/*!
\brief 取面积。
\since build 319 。
*/
yconstfn auto
GetAreaOf(const Size& s) ynothrow -> decltype(s.Width * s.Height)
{
	return s.Width * s.Height;
}


/*!
\brief 屏幕正则矩形：表示屏幕矩形区域。
\note 边平行于水平直线；使用左上点屏幕坐标、宽和高表示。
\warning 非虚析构。
\since build 161 。
*/
class Rect : private Point, private Size
{
public:
	static const Rect Empty; //!< 无参数构造的空矩形对象。
	/*!
	\brief 无效对象。
	\since build 296 。
	*/
	static const Rect Invalid;

	/*!
	\brief 左上角横坐标。
	\see Point::X 。
	\since build 296 。
	*/
	using Point::X;
	/*!
	\brief 左上角纵坐标。
	\see Point::Y 。
	\since build 296 。
	*/
	using Point::Y;
	/*!
	\brief 长。
	\see Size::Width 。
	\since build 296 。
	*/
	using Size::Width;
	/*!
	\brief 宽。
	\see Size::Height 。
	\since build 296 。
	*/
	using Size::Height;


	/*!
	\brief 无参数构造。
	\note 零初始化。
	\since build 319 。
	*/
	yconstfn
	Rect() ynothrow
		: Point(), Size()
	{}
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstfn DefDeCopyCtor(Rect)
	/*!
	\brief 构造：使用屏幕二维点。
	\since build 319 。
	*/
	explicit yconstfn
	Rect(const Point& pt) ynothrow
		: Point(pt), Size()
	{}
	/*!
	\brief 构造：使用 Size 对象。
	\since build 319 。
	*/
	explicit yconstfn
	Rect(const Size& s) ynothrow
		: Point(), Size(s)
	{}
	/*!
	\brief 构造：使用屏幕二维点和 Size 对象。
	\since build 319 。
	*/
	yconstfn
	Rect(const Point& pt, const Size& s) ynothrow
		: Point(pt), Size(s)
	{}
	/*!
	\brief 构造：使用屏幕二维点和表示长宽的两个 SDst 值。
	\since build 319 。
	*/
	yconstfn
	Rect(const Point& pt, SDst w, SDst h) ynothrow
		: Point(pt.X, pt.Y), Size(w, h)
	{}
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和 Size 对象。
	\since build 319 。
	*/
	yconstfn
	Rect(SPos x, SPos y, const Size& s) ynothrow
		: Point(x, y), Size(s.Width, s.Height)
	{}
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和表示大小的两个 SDst 值。
	\since build 319 。
	*/
	yconstfn
	Rect(SPos x, SPos y, SDst w, SDst h) ynothrow
		: Point(x, y), Size(w, h)
	{}

	DefDeCopyAssignment(Rect)
	//! \since build 319 。
	//@{
	Rect&
	operator=(const Point& pt) ynothrow
	{
		yunseq(X = pt.X, Y = pt.Y);
		return *this;
	}
	Rect&
	operator=(const Size& s) ynothrow
	{
		yunseq(Width = s.Width, Height = s.Height);
		return *this;
	}
	//@}

	/*!
	\brief 求与另一个屏幕正则矩形的交。
	\note 若相离结果为 Rect::Empty ，否则为包含于两个参数中的最大矩形。
	\since build 320 。
	*/
	Rect&
	operator&=(const Rect&) ynothrow;

	/*!
	\brief 求与另一个屏幕正则矩形的并。
	\note 结果为包含两个参数中的最小矩形。
	\since build 320 。
	*/
	Rect&
	operator|=(const Rect&) ynothrow;

	/*!
	\brief 判断是否为空。
	\see Size::operator! 。
	\since build 320 。
	*/
	using Size::operator!;

	/*!
	\brief 判断是否非空。
	\see Size::bool 。
	\since build 320 。
	*/
	using Size::operator bool;

	/*!
	\brief 判断点 (px, py) 是否在矩形内或边上。
	\since build 319 。
	*/
	bool
	Contains(int px, int py) const ynothrow;
	/*!
	\brief 判断点 pt 是否在矩形内或边上。
	\since build 319 。
	*/
	PDefH(bool, Contains, const Point& pt) const ynothrow
		ImplRet(Contains(pt.X, pt.Y))
	/*!
	\brief 判断矩形 r 是否在矩形内或边上。
	\since build 319 。
	*/
	bool
	Contains(const Rect& r) const ynothrow;

	/*!
	\brief 判断点 (px, py) 是否在矩形内。
	\since build 319 。
	*/
	bool
	ContainsStrict(int px, int py) const ynothrow;
	/*!
	\brief 判断点 pt 是否在矩形内。
	\since build 319 。
	*/
	PDefH(bool, ContainsStrict, const Point& pt) const ynothrow
		ImplRet(ContainsStrict(pt.X, pt.Y))
	/*!
	\brief 判断矩形 r 是否在矩形内或边上。
	\since build 319 。
	*/
	bool
	ContainsStrict(const Rect& r) const ynothrow;

	/*!
	\brief 判断矩形是否为线段：长和宽中有且一个数值等于 0 。
	\see Size::IsLineSegment 。
	\since build 296 。
	*/
	using Size::IsLineSegment;
	/*!
	\brief 判断矩形是否为不严格的空矩形区域：包括空矩形和线段。
	\see Size::IsUnstrictlyEmpty 。
	\since build 296 。
	*/
	using Size::IsUnstrictlyEmpty;

	/*!
	\brief 取左上角位置。
	*/
	yconstfn DefGetter(const ynothrow, const Point&, Point,
		static_cast<const Point&>(*this))
	/*!
	\brief 取左上角位置引用。
	\since build 296 。
	*/
	DefGetter(ynothrow, Point&, PointRef, static_cast<Point&>(*this))
	/*!
	\brief 取大小。
	*/
	yconstfn DefGetter(const ynothrow, const Size&, Size,
		static_cast<const Size&>(*this))
	/*!
	\brief 取大小引用。
	\since build 296 。
	*/
	DefGetter(ynothrow, Size&, SizeRef, static_cast<Size&>(*this))
};

/*!
\brief 比较：屏幕正则矩形相等关系。
\since build 319 。
*/
yconstfn bool
operator==(const Rect& x, const Rect& y) ynothrow
{
	return x.GetPoint() == y.GetPoint() && x.GetSize() == y.GetSize();
}

/*!
\brief 比较：屏幕正则矩形不等关系。
\since build 319 。
*/
yconstfn bool
operator!=(const Rect& x, const Rect& y) ynothrow
{
	return !(x == y);
}


/*!
\brief 加法：使用正则矩形 r 和偏移向量 v 构造屏幕正则矩形。
\since build 319 。
*/
yconstfn Rect
operator+(const Rect& r, const Vec& v) ynothrow
{
	return Rect(r.GetPoint() + v, r.GetSize());
}

/*!
\brief 减法：使用正则矩形 r 和偏移向量的加法逆元 v 构造屏幕正则矩形。
\since build 319 。
*/
yconstfn Rect
operator-(const Rect& r, const Vec& v) ynothrow
{
	return Rect(r.GetPoint() - v, r.GetSize());
}

/*!
\brief 求两个屏幕正则矩形的交。
\see Rect::operator&= 。
\since build 320 。
*/
Rect
operator&(const Rect&, const Rect&) ynothrow;

/*!
\brief 求两个屏幕正则矩形的并。
\see Rect::operator|= 。
\since build 320 。
*/
Rect
operator|(const Rect&, const Rect&) ynothrow;


template<typename _type>
yconstfn
GBinaryGroup<_type>::GBinaryGroup(const Rect& r) ynothrow
	: X(r.X), Y(r.Y)
{}

yconstfn
Size::Size(const Rect& r) ynothrow
	: Width(r.Width), Height(r.Height)
{}


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
	\since build 319 。
	*/
	explicit yconstfn
	Graphics(BitmapPtr b = nullptr, const Size& s = Size::Zero) ynothrow
		: pBuffer(b), size(s)
	{}
	/*!
	\brief 复制构造：浅复制。
	\since build 319 。
	*/
	yconstfn
	Graphics(const Graphics& g) ynothrow
		: pBuffer(g.pBuffer), size(g.size)
	{}
	/*!
	\brief 析构：空实现。
	*/
	DefEmptyDtor(Graphics)

	/*!
	\brief 判断无效性。
	\since build 319 。
	*/
	PDefHOp(bool, !) const ynothrow
		ImplRet(!bool(*this))

	/*!
	\brief 取指定行首元素指针。
	\pre 断言：缓冲区指针非空；参数不越界。
	*/
	BitmapPtr
	operator[](size_t) const ynothrow;

	/*!
	\brief 判断有效性。
	\since build 319 。
	*/
	explicit DefCvt(const ynothrow, bool,
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

