
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
\version r1462;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-05-03 07:20:51 +0800;
\par 修改时间:
	2011-09-06 23:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YGDIBase;
*/


#ifndef YSL_INC_CORE_YGDIBASE_H_
#define YSL_INC_CORE_YGDIBASE_H_

#include "yexcept.h"

YSL_BEGIN

// GDI 基本数据类型和宏定义。

YSL_BEGIN_NAMESPACE(Drawing)

//前向声明。

class BinaryGroup;
class Point;
class Vec;
class Size;
class Rect;


//! \brief 屏幕二元组。
class BinaryGroup
{
public:
	SPos X, Y; //!< 分量。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	BinaryGroup();
	/*!
	\brief 复制构造。
	*/
	BinaryGroup(const BinaryGroup&);
	/*!
	\brief 构造：使用 Size 对象。
	*/
	explicit
	BinaryGroup(const Size&);
	/*!
	\brief 构造：使用二维向量。
	*/
	template<typename _tVec>
	inline explicit
	BinaryGroup(const _tVec& v)
	: X(v.X), Y(v.Y)
	{}
	/*!
	\brief 构造：使用两个标量。
	*/
	template<typename _tScalar1, typename _tScalar2>
	inline
	BinaryGroup(_tScalar1 x, _tScalar2 y)
	: X(x), Y(y)
	{}

	/*!
	\brief 负运算：取加法逆元。
	*/
	BinaryGroup
	operator-()
	{
		return BinaryGroup(-X, -Y);
	}

	DefGetter(SPos, X, X)
	DefGetter(SPos, Y, Y)
};

inline
BinaryGroup::BinaryGroup()
	: X(0), Y(0)
{}
inline
BinaryGroup::BinaryGroup(const BinaryGroup& b)
	: X(b.X), Y(b.Y)
{}

/*!
\brief 比较：相等关系。
*/
inline bool
operator==(const BinaryGroup& a, const BinaryGroup& b)
{
	return a.X == b.X && a.Y == b.Y;
}
/*!
\brief 比较：不等关系。
*/
inline bool
operator!=(const BinaryGroup& a, const BinaryGroup& b)
{
	return !(a == b);
}


//! \brief 屏幕二维点（直角坐标表示）。
class Point : public BinaryGroup
{
public:
	static const Point Zero; //!< 无参数构造参数构造的原点对象。
	static const Point FullScreen; \
		//!< 无参数构造参数构造的屏幕右下角边界（不在屏幕坐标系中）对象。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	Point();
	/*!
	\brief 复制构造。
	*/
	Point(const Point&);
	/*!
	\brief 构造：使用 Size 对象。
	*/
	explicit
	Point(const Size&);
	/*!
	\brief 构造：使用二维向量。
	*/
	template<typename _tVec>
	inline explicit
	Point(const _tVec& v)
		: BinaryGroup(v.X, v.Y)
	{}
	/*!
	\brief 构造：使用两个标量。
	*/
	template<typename _tScalar1, typename tScalar2>
	inline
	Point(_tScalar1 x, tScalar2 y)
		: BinaryGroup(x, y)
	{}

	/*!
	\brief 转换：屏幕二维向量。
	\note 结果以原点为起点，该点为终点。
	*/
	operator Vec() const;

	/*!
	\brief 负运算：取与原点对称的元素。
	*/
	Point
	operator-() 
	{
		return Point(-X, -Y);
	}
	/*!
	\brief 加法赋值：按屏幕二维向量平移。
	*/
	Point&
	operator+=(const Vec&);
	/*!
	\brief 减法赋值：按屏幕二维向量的加法逆元平移。
	*/
	Point&
	operator-=(const Vec&);

	/*!
	\biref 判断是否位于原点。
	*/
	DefPredicate(Origin, X == 0 && Y == 0)
};

inline
Point::Point()
	: BinaryGroup()
{}
inline
Point::Point(const Point& p)
	: BinaryGroup(p)
{}


//! \brief 屏幕二维向量（直角坐标表示）。
class Vec : public BinaryGroup
{
public:
	static const Vec Zero; //!< 无参数构造参数构造的零向量对象。
	static const Vec FullScreen;
		//无参数构造参数构造的由屏幕坐标系原点
		//到屏幕右下角边界（不在屏幕坐标系中）对象。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	Vec();
	/*!
	\brief 复制构造。
	*/
	Vec(const Vec&);
	/*!
	\brief 构造：使用 Size 对象。
	*/
	explicit
	Vec(const Size&);
	/*!
	\brief 构造：使用二维向量。
	*/
	template<typename _tVec>
	inline explicit
	Vec(const _tVec& v)
		: BinaryGroup(v.X, v.Y)
	{}
	/*!
	\brief 构造：使用两个标量。
	*/
	template<typename _tScalar1, typename _tScalar2>
	inline
	Vec(_tScalar1 x, _tScalar2 y)
		: BinaryGroup(x, y)
	{}

	/*!
	\brief 转换：屏幕二维点。
	*/
	DefConverter(Point, Point(X, Y))

	/*!
	\brief 负运算：返回加法逆元。
	*/
	Vec
	operator-()
	{
		return Vec(-X, -Y);
	}
	/*!
	\brief 加法赋值。
	*/
	Vec&
	operator+=(const Vec& v)
	{
		X += v.X;
		Y += v.Y;
		return *this;
	}
	/*!
	\brief 减法赋值。
	*/
	Vec&
	operator-=(const Vec& v)
	{
		X -= v.X;
		Y -= v.Y;
		return *this;
	}
};

inline
Vec::Vec()
	: BinaryGroup()
{}

inline
Vec::Vec(const Vec& v)
	: BinaryGroup(v)
{}


inline
Point::operator Vec() const
{
	return Vec(X, Y);
}

inline Point&
Point::operator+=(const Vec& v)
{
	X += v.X;
	Y += v.Y;
	return *this;
}
inline Point&
Point::operator-=(const Vec& v)
{
	X -= v.X;
	Y -= v.Y;
	return *this;
}


//屏幕点和二维向量二元运算。

/*!
\brief 构造屏幕二维向量：使用起点 a 和终点 b 。
*/
inline Vec
operator-(const Point& a, const Point& b)
{
	return Vec(a.X - b.X, a.Y - b.Y);
}

/*!
\brief 构造屏幕点：使用点 p 和偏移向量 d 。
*/
inline Point
operator+(const Point& p, const Vec& d)
{
	return Point(p.X + d.X, p.Y + d.Y);
}

/*!
\brief 构造屏幕点：使用点 p 和偏移向量的加法逆元 v 。
*/
inline Point
operator-(const Point& p, const Vec& v)
{
	return Point(p.X - v.X, v.Y - v.Y);
}

/*!
\brief 屏幕二维向量加法。
*/
inline Vec
operator+(const Vec& a, const Vec& b)
{
	return Vec(a.X + b.X, a.Y + b.Y);
}

/*!
\brief 屏幕二维向量减法。
*/
inline Vec
operator-(const Vec& a, const Vec& b)
{
	return Vec(a.X - b.X, a.Y - b.Y);
}


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
	Size();
	/*!
	\brief 复制构造。
	*/
	Size(const Size&);
	/*!
	\brief 构造：使用屏幕二维向量。
	*/
	inline explicit
	Size(const Vec& v)
		: Width(v.X), Height(v.Y)
	{}
	/*!
	\brief 构造：使用两个标量。
	*/
	template<typename _tScalar1, typename _tScalar2>
	inline
	Size(_tScalar1 w, _tScalar2 h)
		: Width(w), Height(h)
	{}

	/*!
	\brief 转换：屏幕二维向量。
	\note 以Width 和 Height 分量作为结果的 X 和 Y分量。
	*/
	DefConverter(Vec, Vec(Width, Height))

	/*!
	\brief 判断是否为空。
	*/
	DefPredicate(Empty, Width == 0 && Height == 0)
};

inline
Size::Size()
	: Width(0), Height(0)
{}
inline
Size::Size(const Size& s)
	: Width(s.Width), Height(s.Height)
{}

inline bool
operator==(const Size& a, const Size& b)
{
	return a.Width == b.Width && a.Height == b.Height;
}
inline bool
operator!=(const Size& a, const Size& b)
{
	return !(a == b);
}


inline
BinaryGroup::BinaryGroup(const Size& s)
	: X(s.Width), Y(s.Height)
{}

inline
Point::Point(const Size& s)
	: BinaryGroup(s.Width, s.Height)
{}

inline
Vec::Vec(const Size& s)
	: BinaryGroup(s.Width, s.Height)
{}

/*!
\brief 选择分量。
\note 第二参数为 true 时选择第一分量，否则选择第二分量。
*/
SPos
SelectFrom(const BinaryGroup&, bool = true);
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
SPos&
SelectRefFrom(BinaryGroup&, bool = true);
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
void
UpdateTo(BinaryGroup&, SPos, bool = true);
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
inline _tBinary
Transpose(_tBinary& o)
{
	return _tBinary(o.Y, o.X);
}

/*!
\brief 取面积。
*/
inline u32
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
	Rect();
	/*!
	\brief 复制构造。
	*/
	Rect(const Rect&);
	/*!
	\brief 构造：使用屏幕二维点。
	*/
	explicit
	Rect(const Point&);
	/*!
	\brief 构造：使用 Size 对象。
	*/
	explicit
	Rect(const Size&);
	/*!
	\brief 构造：使用屏幕二维点和 Size 对象。
	*/
	Rect(const Point&, const Size&);
	/*!
	\brief 构造：使用屏幕二维点和表示长宽的两个 SDst 值。
	*/
	Rect(const Point&, SDst, SDst);
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和 Size 对象。
	*/
	Rect(SPos, SPos, const Size&);
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和表示大小的两个 SDst 值。
	*/
	Rect(SPos, SPos, SDst, SDst);

	inline Rect&
	operator=(const Rect&) = default;
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
	PDefH1(bool, Contains, const Point& pt) const
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
	PDefH1(bool, ContainsStrict, const Point& pt) const
		ImplRet(ContainsStrict(pt.X, pt.Y))
	/*!
	\brief 判断矩形 r 是否在矩形内或边上。
	*/
	bool
	ContainsStrict(const Rect& r) const;

	/*!
	\brief 取左上角位置。
	*/
	DefGetter(Point, Point, Point(X, Y))
	/*!
	\brief 取大小。
	*/
	DefGetter(Size, Size, Size(Width, Height))
};

inline
Rect::Rect()
	: Point(), Size()
{}
inline
Rect::Rect(const Rect& r)
	: Point(r), Size(r)
{}
inline
Rect::Rect(const Point& p)
	: Point(p), Size()
{}
inline
Rect::Rect(const Size& s)
	: Point(), Size(s)
{}
inline
Rect::Rect(const Point& p, const Size& s)
	: Point(p), Size(s)
{}
inline
Rect::Rect(const Point& p, SDst w, SDst h)
	: Point(p.X, p.Y), Size(w, h)
{}
inline
Rect::Rect(SPos x, SPos y, const Size& s)
	: Point(x, y), Size(s.Width, s.Height)
{}
inline
Rect::Rect(SPos x, SPos y, SDst w, SDst h)
	: Point(x, y), Size(w, h)
{}

inline Rect&
Rect::operator=(const Point& pt)
{
	X = pt.X;
	Y = pt.Y;
	return *this;
}
inline Rect&
Rect::operator=(const Size& s)
{
	Width = s.Width;
	Height = s.Height;
	return *this;
}

inline bool
operator==(const Rect& a, const Rect& b)
{
	return static_cast<Point>(a) == static_cast<Point>(b)
		&& static_cast<Size>(a) == static_cast<Size>(b);
}
inline bool
operator!=(const Rect& a, const Rect& b)
{
	return !(a == b);
}


/*!
\brief 构造屏幕正则矩形：使用正则矩形 r 和偏移向量 v 。
*/
inline Rect
operator+(const Rect& r, const Vec& v)
{
	return Rect(r.GetPoint() + v, r.GetSize());
}

/*!
\brief 构造屏幕正则矩形：使用正则矩形 r 和偏移向量的加法逆元 v 。
*/
inline Rect
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
	explicit
	Graphics(BitmapPtr = nullptr, const Size& = Size::Zero);
	/*!
	\brief 复制构造：浅复制。
	*/
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

inline
Graphics::Graphics(BitmapPtr b, const Size& s)
	: pBuffer(b), size(s)
{}
inline
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

