// YSLib::Core::YObject by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-21 10:12 + 08:00;
// Version = 0.2370;


#ifndef INCLUDED_YOBJECT_H_
#define INCLUDED_YOBJECT_H_

// YObject ：平台无关的基础对象实现。

#include "ycounter.hpp"
#include "ycutil.h"

YSL_BEGIN

//全局静态单例存储器。
template<typename _type>
class GStaticCache
{
private:
	static _type* _ptr;

	GStaticCache();

	static void
	Check()
	{
		if(!_ptr)
			_ptr = new _type();
	}

public:
	DefStaticGetter(_type*, PointerRaw, _ptr)
	static _type*
	GetPointer()
	{
		Check();
		return GetPointerRaw();
	}
	static _type&
	GetInstance()
	{
		Check();
		return *GetPointer();
	}

	inline void
	Release()
	{
		delete _ptr;
		_ptr = NULL;
	}
};

template<typename _type>
_type* GStaticCache<_type>::_ptr(NULL);


YSL_BEGIN_NAMESPACE(Drawing)

//前向声明。

class BinaryGroup;
class Point;
class Vec;
class Size;
class Rect;


class BinaryGroup //屏幕二元组。
{
public:
	SPOS X, Y; //分量。

	BinaryGroup();
	BinaryGroup(const BinaryGroup&);
	explicit
	BinaryGroup(const Size&);
	template<typename _tVec>
	inline explicit
	BinaryGroup(const _tVec& v)
	: X(v.X), Y(v.Y)
	{}
	template<typename _tScalar1, typename _tScalar2>
	inline
	BinaryGroup(_tScalar1 x, _tScalar2 y)
	: X(x), Y(y)
	{}

	BinaryGroup
	operator-() //负运算：返回相反元素。
	{
		return BinaryGroup(-X, -Y);
	}

	DefGetter(SPOS, X, X)
	DefGetter(SPOS, Y, Y)
};

inline
BinaryGroup::BinaryGroup()
	: X(0), Y(0)
{}
inline
BinaryGroup::BinaryGroup(const BinaryGroup& b)
	: X(b.X), Y(b.Y)
{}

inline bool
operator==(const BinaryGroup& a, const BinaryGroup& b)
{
	return a.X == b.X && a.Y == b.Y;
}
inline bool
operator!=(const BinaryGroup& a, const BinaryGroup& b)
{
	return !(a == b);
}


class Point : public BinaryGroup //屏幕二维点坐标。
{
public:
	typedef BinaryGroup ParentType;

	static const Point Zero; //默认构造参数构造的原点对象。
	static const Point FullScreen; //默认构造参数构造的屏幕右下角边界（不在屏幕坐标系中）对象。

	Point();
	Point(const Point&);
	explicit
	Point(const Size&);
	template<typename _tVec>
	inline explicit
	Point(const _tVec& v)
		: BinaryGroup(v.X, v.Y)
	{}
	template<typename _tScalar1, typename tScalar2>
	inline
	Point(_tScalar1 x, tScalar2 y)
		: BinaryGroup(x, y)
	{}

	operator Vec() const;

	Point
	operator-() //负运算：返回与原点对称的元素。
	{
		return Point(-X, -Y);
	}
	Point&
	operator+=(const Vec&);
	Point&
	operator-=(const Vec&);
};

inline
Point::Point()
	: BinaryGroup()
{}
inline
Point::Point(const Point& p)
	: BinaryGroup(p)
{}


class Vec : public BinaryGroup //屏幕二维向量。
{
public:
	typedef BinaryGroup ParentType;

	static const Vec Zero; //默认构造参数构造的零向量对象。
	static const Vec FullScreen; //默认构造参数构造的由屏幕坐标系原点到屏幕右下角边界（不在屏幕坐标系中）对象。

	Vec();
	Vec(const Vec&);
	explicit
	Vec(const Size&);
	template<typename _tVec>
	inline explicit
	Vec(const _tVec& v)
		: BinaryGroup(v.X, v.Y)
	{}
	template<typename _tScalar1, typename _tScalar2>
	inline
	Vec(_tScalar1 x, _tScalar2 y)
		: BinaryGroup(x, y)
	{}

	operator Point() const
	{
		return Point(X, Y);
	}

	Vec
	operator-() //负运算：返回相反向量。
	{
		return Vec(-X, -Y);
	}
	Vec&
	operator+=(const Vec& v)
	{
		X += v.X;
		Y += v.Y;
		return *this;
	}
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


//屏幕二维向量二元运算。

inline Vec
operator-(const Point& a, const Point& b)
{
	return Vec(a.X - b.X, a.Y - b.Y);
}
inline Vec
operator+(const Vec& a, const Vec& b)
{
	return Vec(a.X + b.X, a.Y + b.Y);
}
inline Vec
operator-(const Vec& a, const Vec& b)
{
	return Vec(a.X - b.X, a.Y - b.Y);
}


struct Size //屏幕区域大小。
{
	SDST Width, Height; //宽和高。

	static const Size Zero; //默认构造参数构造的零元素对象。
	static const Size FullScreen; //默认构造参数构造的全屏幕对象。

	Size();
	Size(const Size&);
	template<typename _tVec>
	inline explicit
	Size(const _tVec& v)
		: Width(v.X), Height(v.Y)
	{}
	template<typename _tScalar1, typename _tScalar2>
	inline
	Size(_tScalar1 w, _tScalar2 h)
		: Width(w), Height(h)
	{}

	operator Vec() const
	{
		return Vec(Width, Height);
	}

	DefGetter(SDST, Width, Width)
	DefGetter(SDST, Height, Height)
	DefGetter(u32, Area, Width * Height) //取面积。
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


//二元对象转置。
template<class _tBinary>
inline _tBinary
Transpose(_tBinary& o)
{
	return _tBinary(o.Y, o.X);
}


class Rect : public Point, public Size //屏幕坐标系的正则（边平行于水平直线的）矩形：使用左上点屏幕坐标、宽和高表示。
{
public:
	static const Rect Empty; //默认构造参数构造的空矩形对象。
	static const Rect FullScreen; //默认构造参数构造的全屏幕矩形对象。

	Rect();
	Rect(const Rect&);
	explicit
	Rect(const Point&);
	explicit
	Rect(const Size&);
	Rect(const Point&, const Size&);
	Rect(const Point&, SDST, SDST);
	Rect(SPOS, SPOS, const Size&);
	Rect(SPOS, SPOS, SDST, SDST);

	bool
	IsInBounds(const Point&) const; //判断点是否在矩形内或边上。
	bool
	IsInBounds(int px, int py) const; //判断点 (px, py) 是否在矩形内或边上。
	bool
	IsInBoundsRegular(const Point& p) const; //判断点是否在矩形内或左侧或上侧边上。
	bool
	IsInBoundsRegular(int px, int py) const; //判断点 (px, py) 是否在矩形内或左侧或上侧边上。
	bool
	IsInBoundsStrict(const Point& p) const; //判断点是否在矩形内。
	bool
	IsInBoundsStrict(int px, int py) const; //判断点 (px, py) 是否在矩形内。
	Point
	GetPoint() const; //取左上角位置。
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
Rect::Rect(const Point& p, SDST w, SDST h)
	: Point(p.X, p.Y), Size(w, h)
{}
inline
Rect::Rect(SPOS x, SPOS y, const Size& s)
	: Point(x, y), Size(s.Width, s.Height)
{}
inline
Rect::Rect(SPOS x, SPOS y, SDST w, SDST h)
	: Point(x, y), Size(w, h)
{}

inline bool
operator==(const Rect& a, const Rect& b)
{
	return static_cast<Point>(a) == static_cast<Point>(b) && static_cast<Size>(a) == static_cast<Size>(b);
}
inline bool
operator!=(const Rect& a, const Rect& b)
{
	return !(a == b);
}

inline bool
Rect::IsInBounds(const Point& p) const
{
	return sgnInterval<SPOS>(p.X, X, (X + Width)) >= 0 && sgnInterval<SPOS>(p.Y, Y, (Y + Height)) >= 0;
}
inline bool
Rect::IsInBounds(int px, int py) const
{
	return sgnInterval<SPOS>(px, X, (X + Width)) >= 0 && sgnInterval<SPOS>(py, Y, (Y + Height)) >= 0;
}
inline bool
Rect::IsInBoundsRegular(const Point& p) const
{
	return isInIntervalRegular<int>(p.X - X, Width) && isInIntervalRegular<int>(p.Y - Y, Height);
}
inline bool
Rect::IsInBoundsRegular(int px, int py) const
{
	return isInIntervalRegular<int>(px - X, Width) && isInIntervalRegular<int>(py - Y, Height);
}
inline bool
Rect::IsInBoundsStrict(const Point& p) const
{
	return isInIntervalRegular<int>(p.X - X, Width) && isInIntervalRegular<int>(p.Y - Y, Height);
}
inline bool
Rect::IsInBoundsStrict(int px, int py) const
{
	return isInIntervalStrict<int>(px - X, Width) && isInIntervalStrict<int>(py - Y, Height);
}
inline Point
Rect::GetPoint() const
{
	return Point(X, Y);
}

YSL_END_NAMESPACE(Drawing)

//基本对象定义。

//基本对象类：所有类名以 Y 作前缀类（除模板类、非直接实例化类和模块类）的公共基类。
class YObject : private NonCopyable
{
public:
	virtual
	~YObject()
	{}
};


//基本可数对象类：所有可数的基本对象类的公共基类。
class YCountableObject : public GMCounter<YCountableObject>, public YObject
{
public:
	typedef YObject ParentType;

protected:
	YCountableObject();
};

inline
YCountableObject::YCountableObject()
{}

YSL_END

#endif

