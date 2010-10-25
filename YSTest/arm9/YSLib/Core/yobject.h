// YSLib::Core::YObject by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-24 19:55 + 08:00;
// Version = 0.2486;


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

	//********************************
	//名称:		Check
	//全名:		YSLib::GStaticCache::Check
	//可访问性:	private static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	检查是否已经初始化，否则构造新对象。
	//备注:		
	//********************************
	static void
	Check()
	{
		if(_ptr == NULL)
			_ptr = new _type();
	}

public:
	DefStaticGetter(_type*, PointerRaw, _ptr)
	//********************************
	//名称:		GetPointer
	//全名:		YSLib::GStaticCache::GetPointer
	//可访问性:	public static 
	//返回类型:	_type*
	//修饰符:	
	//功能概要:	取指针。
	//备注:		
	//********************************
	static _type*
	GetPointer()
	{
		Check();
		return GetPointerRaw();
	}
	//********************************
	//名称:		GetInstance
	//全名:		YSLib::GStaticCache::GetInstance
	//可访问性:	public static 
	//返回类型:	_type&
	//修饰符:	
	//功能概要:	取实例引用。
	//备注:		
	//********************************
	static _type&
	GetInstance()
	{
		Check();
		return *GetPointer();
	}

	//********************************
	//名称:		Release
	//全名:		YSLib::GStaticCache::Release
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	删除对象并置指针为空值。
	//备注:		
	//********************************
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


//屏幕二元组。
class BinaryGroup
{
public:
	SPOS X, Y; //分量。

	//********************************
	//名称:		BinaryGroup
	//全名:		YSLib::Drawing::BinaryGroup::BinaryGroup
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		零初始化。
	//********************************
	BinaryGroup();
	//********************************
	//名称:		BinaryGroup
	//全名:		YSLib::Drawing::BinaryGroup::BinaryGroup
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const BinaryGroup &
	//功能概要:	复制构造。
	//备注:		
	//********************************
	BinaryGroup(const BinaryGroup&);
	//********************************
	//名称:		BinaryGroup
	//全名:		YSLib::Drawing::BinaryGroup::BinaryGroup
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Size &
	//功能概要:	构造：使用 Size 对象。
	//备注:		
	//********************************
	explicit
	BinaryGroup(const Size&);
	//********************************
	//名称:		BinaryGroup
	//全名:		YSLib::Drawing::BinaryGroup::BinaryGroup<_tVec>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const _tVec & v
	//功能概要:	构造：使用二维向量。
	//备注:		
	//********************************
	template<typename _tVec>
	inline explicit
	BinaryGroup(const _tVec& v)
	: X(v.X), Y(v.Y)
	{}
	//********************************
	//名称:		BinaryGroup
	//全名:		YSLib::Drawing::BinaryGroup::BinaryGroup<_tScalar1, _tScalar2>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tScalar1 x
	//形式参数:	_tScalar2 y
	//功能概要:	构造：使用两个标量。
	//备注:		
	//********************************
	template<typename _tScalar1, typename _tScalar2>
	inline
	BinaryGroup(_tScalar1 x, _tScalar2 y)
	: X(x), Y(y)
	{}

	//********************************
	//名称:		operator-
	//全名:		YSLib::Drawing::BinaryGroup::operator-
	//可访问性:	public 
	//返回类型:	YSLib::Drawing::BinaryGroup
	//修饰符:	
	//功能概要:	负运算：取加法逆元。
	//备注:		
	//********************************
	BinaryGroup
	operator-()
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

//********************************
//名称:		operator==
//全名:		YSLib::Drawing::operator==
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const BinaryGroup & a
//形式参数:	const BinaryGroup & b
//功能概要:	比较：相等关系。
//备注:		
//********************************
inline bool
operator==(const BinaryGroup& a, const BinaryGroup& b)
{
	return a.X == b.X && a.Y == b.Y;
}
//********************************
//名称:		operator!=
//全名:		YSLib::Drawing::operator!=
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	const BinaryGroup & a
//形式参数:	const BinaryGroup & b
//功能概要:	比较：不等关系。
//备注:		
//********************************
inline bool
operator!=(const BinaryGroup& a, const BinaryGroup& b)
{
	return !(a == b);
}


//屏幕二维点（直角坐标表示）。
class Point : public BinaryGroup
{
public:
	typedef BinaryGroup ParentType;

	static const Point Zero; //默认构造参数构造的原点对象。
	static const Point FullScreen; \
		//默认构造参数构造的屏幕右下角边界（不在屏幕坐标系中）对象。

	//********************************
	//名称:		Point
	//全名:		YSLib::Drawing::Point::Point
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		零初始化。
	//********************************
	Point();
	//********************************
	//名称:		Point
	//全名:		YSLib::Drawing::Point::Point
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Point &
	//功能概要:	复制构造。
	//备注:		
	//********************************
	Point(const Point&);
	//********************************
	//名称:		Point
	//全名:		YSLib::Drawing::Point::Point
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Size &
	//功能概要:	构造：使用 Size 对象。
	//备注:		
	//********************************
	explicit
	Point(const Size&);
	//********************************
	//名称:		Point
	//全名:		YSLib::Drawing::Point::Point<_tVec>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const _tVec & v
	//功能概要:	构造：使用二维向量。
	//备注:		
	//********************************
	template<typename _tVec>
	inline explicit
	Point(const _tVec& v)
		: BinaryGroup(v.X, v.Y)
	{}
	//********************************
	//名称:		Point
	//全名:		YSLib::Drawing::Point::Point<_tScalar1, tScalar2>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tScalar1 x
	//形式参数:	tScalar2 y
	//功能概要:	构造：使用两个标量。
	//备注:		
	//********************************
	template<typename _tScalar1, typename tScalar2>
	inline
	Point(_tScalar1 x, tScalar2 y)
		: BinaryGroup(x, y)
	{}

	//********************************
	//名称:		operator Vec
	//全名:		YSLib::Drawing::Point::operator Vec
	//可访问性:	public 
	//返回类型:	
	//修饰符:	const
	//功能概要:	转换：屏幕二维向量。
	//备注:		结果以原点为起点，该点为终点。
	//********************************
	operator Vec() const;

	//********************************
	//名称:		operator-
	//全名:		YSLib::Drawing::Point::operator-
	//可访问性:	public 
	//返回类型:	YSLib::Drawing::Point
	//修饰符:	
	//功能概要:	负运算：取与原点对称的元素。
	//备注:		
	//********************************
	Point
	operator-() 
	{
		return Point(-X, -Y);
	}
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Drawing::Point::operator+=
	//可访问性:	public 
	//返回类型:	Point&
	//修饰符:	
	//形式参数:	const Vec &
	//功能概要:	加法赋值：按屏幕二维向量平移。
	//备注:		
	//********************************
	Point&
	operator+=(const Vec&);
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Drawing::Point::operator-=
	//可访问性:	public 
	//返回类型:	Point&
	//修饰符:	
	//形式参数:	const Vec &
	//功能概要:	减法赋值：按屏幕二维向量的加法逆元平移。
	//备注:		
	//********************************
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


//屏幕二维向量（直角坐标表示）。
class Vec : public BinaryGroup
{
public:
	typedef BinaryGroup ParentType;

	static const Vec Zero; //默认构造参数构造的零向量对象。
	static const Vec FullScreen;
		//默认构造参数构造的由屏幕坐标系原点
		//到屏幕右下角边界（不在屏幕坐标系中）对象。

	//********************************
	//名称:		Vec
	//全名:		YSLib::Drawing::Vec::Vec
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		零初始化。
	//********************************
	Vec();
	//********************************
	//名称:		Vec
	//全名:		YSLib::Drawing::Vec::Vec
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Vec &
	//功能概要:	复制构造。
	//备注:		
	//********************************
	Vec(const Vec&);
	//********************************
	//名称:		Vec
	//全名:		YSLib::Drawing::Vec::Vec
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Size &
	//功能概要:	构造：使用 Size 对象。
	//备注:		
	//********************************
	explicit
	Vec(const Size&);
	//********************************
	//名称:		Vec
	//全名:		YSLib::Drawing::Vec::Vec<_tVec>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const _tVec & v
	//功能概要:	构造：使用二维向量。
	//备注:		
	//********************************
	template<typename _tVec>
	inline explicit
	Vec(const _tVec& v)
		: BinaryGroup(v.X, v.Y)
	{}
	//********************************
	//名称:		Vec
	//全名:		YSLib::Drawing::Vec::Vec<_tScalar1, _tScalar2>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tScalar1 x
	//形式参数:	_tScalar2 y
	//功能概要:	构造：使用两个标量。
	//备注:		
	//********************************
	template<typename _tScalar1, typename _tScalar2>
	inline
	Vec(_tScalar1 x, _tScalar2 y)
		: BinaryGroup(x, y)
	{}

	//********************************
	//名称:		operator Point
	//全名:		YSLib::Drawing::Vec::operator Point
	//可访问性:	public 
	//返回类型:	
	//修饰符:	const
	//功能概要:	转换：屏幕二维点。
	//备注:		
	//********************************
	operator Point() const
	{
		return Point(X, Y);
	}

	//********************************
	//名称:		operator-
	//全名:		YSLib::Drawing::Vec::operator-
	//可访问性:	public 
	//返回类型:	YSLib::Drawing::Vec
	//修饰符:	
	//功能概要:	负运算：返回加法逆元。
	//备注:		
	//********************************
	Vec
	operator-()
	{
		return Vec(-X, -Y);
	}
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Drawing::Vec::operator+=
	//可访问性:	public 
	//返回类型:	Vec&
	//修饰符:	
	//形式参数:	const Vec & v
	//功能概要:	加法赋值。
	//备注:		
	//********************************
	Vec&
	operator+=(const Vec& v)
	{
		X += v.X;
		Y += v.Y;
		return *this;
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Drawing::Vec::operator-=
	//可访问性:	public 
	//返回类型:	Vec&
	//修饰符:	
	//形式参数:	const Vec & v
	//功能概要:	减法赋值。
	//备注:		
	//********************************
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

//********************************
//名称:		operator-
//全名:		YSLib::Drawing::operator-
//可访问性:	public 
//返回类型:	YSLib::Drawing::Vec
//修饰符:	
//形式参数:	const Point & a
//形式参数:	const Point & b
//功能概要: 构造屏幕二维向量：使用起点 a 和终点 b 。
//备注:		
//********************************
inline Vec
operator-(const Point& a, const Point& b)
{
	return Vec(a.X - b.X, a.Y - b.Y);
}
//********************************
//名称:		operator+
//全名:		YSLib::Drawing::operator+
//可访问性:	public 
//返回类型:	YSLib::Drawing::Vec
//修饰符:	
//形式参数:	const Vec & a
//形式参数:	const Vec & b
//功能概要:	屏幕二维向量加法。
//备注:		
//********************************
inline Vec
operator+(const Vec& a, const Vec& b)
{
	return Vec(a.X + b.X, a.Y + b.Y);
}
//********************************
//名称:		operator-
//全名:		YSLib::Drawing::operator-
//可访问性:	public 
//返回类型:	YSLib::Drawing::Vec
//修饰符:	
//形式参数:	const Vec & a
//形式参数:	const Vec & b
//功能概要:	屏幕二维向量减法。
//备注:		
//********************************
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

	//********************************
	//名称:		Size
	//全名:		YSLib::Drawing::Size::Size
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		零初始化。
	//********************************
	Size();
	//********************************
	//名称:		Size
	//全名:		YSLib::Drawing::Size::Size
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Size &
	//功能概要:	复制构造。
	//备注:		
	//********************************
	Size(const Size&);
	//********************************
	//名称:		Size
	//全名:		YSLib::Drawing::Size::Size<_tVec>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const _tVec & v
	//功能概要:	构造：使用二维向量。
	//备注:		
	//********************************
	template<typename _tVec>
	inline explicit
	Size(const _tVec& v)
		: Width(v.X), Height(v.Y)
	{}
	//********************************
	//名称:		Size
	//全名:		YSLib::Drawing::Size::Size<_tScalar1, _tScalar2>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tScalar1 w
	//形式参数:	_tScalar2 h
	//功能概要:	构造：使用两个标量。
	//备注:		
	//********************************
	template<typename _tScalar1, typename _tScalar2>
	inline
	Size(_tScalar1 w, _tScalar2 h)
		: Width(w), Height(h)
	{}

	//********************************
	//名称:		operator Vec
	//全名:		YSLib::Drawing::Size::operator Vec
	//可访问性:	public 
	//返回类型:	
	//修饰符:	const
	//功能概要:	转换：屏幕二维向量。
	//备注:		以Width 和 Height 分量作为结果的 X 和 Y分量。
	//********************************
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


//********************************
//名称:		Transpose
//全名:		YSLib::Drawing<_tBinary>::Transpose
//可访问性:	public 
//返回类型:	_tBinary
//修饰符:	
//形式参数:	_tBinary & o
//功能概要:	二元对象转置。
//备注:		
//********************************
template<class _tBinary>
inline _tBinary
Transpose(_tBinary& o)
{
	return _tBinary(o.Y, o.X);
}

//屏幕坐标系的正则（边平行于水平直线的）矩形：使用左上点屏幕坐标、宽和高表示。
class Rect : public Point, public Size
{
public:
	static const Rect Empty; //默认构造参数构造的空矩形对象。
	static const Rect FullScreen; //默认构造参数构造的全屏幕矩形对象。

	//********************************
	//名称:		Rect
	//全名:		YSLib::Drawing::Rect::Rect
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		零初始化。
	//********************************
	Rect();
	//********************************
	//名称:		Rect
	//全名:		YSLib::Drawing::Rect::Rect
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Rect &
	//功能概要:	复制构造。
	//备注:		
	//********************************
	Rect(const Rect&);
	//********************************
	//名称:		Rect
	//全名:		YSLib::Drawing::Rect::Rect
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Point &
	//功能概要:	构造：使用屏幕二维点。
	//备注:		
	//********************************
	explicit
	Rect(const Point&);
	//********************************
	//名称:		Rect
	//全名:		YSLib::Drawing::Rect::Rect
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Size &
	//功能概要:	构造：使用 Size 对象。
	//备注:		
	//********************************
	explicit
	Rect(const Size&);
	//********************************
	//名称:		Rect
	//全名:		YSLib::Drawing::Rect::Rect
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Point &
	//形式参数:	const Size &
	//功能概要:	构造：使用屏幕二维点和 Size 对象。
	//备注:		
	//********************************
	Rect(const Point&, const Size&);
	//********************************
	//名称:		Rect
	//全名:		YSLib::Drawing::Rect::Rect
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Point &
	//形式参数:	SDST
	//形式参数:	SDST
	//功能概要:	构造：使用屏幕二维点和表示长宽的两个 SDST 值。
	//备注:		
	//********************************
	Rect(const Point&, SDST, SDST);
	//********************************
	//名称:		Rect
	//全名:		YSLib::Drawing::Rect::Rect
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	SPOS
	//形式参数:	SPOS
	//形式参数:	const Size &
	//功能概要:	构造：使用表示位置的两个 SPOS 值和 Size 对象。
	//备注:		
	//********************************
	Rect(SPOS, SPOS, const Size&);
	//********************************
	//名称:		Rect
	//全名:		YSLib::Drawing::Rect::Rect
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	SPOS
	//形式参数:	SPOS
	//形式参数:	SDST
	//形式参数:	SDST
	//功能概要:	构造：使用表示位置的两个 SPOS 值和表示大小的两个 SDST 值。
	//备注:		
	//********************************
	Rect(SPOS, SPOS, SDST, SDST);

	//********************************
	//名称:		IsInBounds
	//全名:		YSLib::Drawing::Rect::IsInBounds
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const Point &
	//功能概要:	判断点是否在矩形内或边上。
	//备注:		
	//********************************
	bool
	IsInBounds(const Point&) const;
	//********************************
	//名称:		IsInBounds
	//全名:		YSLib::Drawing::Rect::IsInBounds
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	int px
	//形式参数:	int py
	//功能概要:	判断点 (px, py) 是否在矩形内或边上。
	//备注:		
	//********************************
	bool
	IsInBounds(int px, int py) const;
	//********************************
	//名称:		IsInBoundsRegular
	//全名:		YSLib::Drawing::Rect::IsInBoundsRegular
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const Point & p
	//功能概要:	判断点是否在矩形内或左侧或上侧边上。
	//备注:		
	//********************************
	bool
	IsInBoundsRegular(const Point& p) const;
	//********************************
	//名称:		IsInBoundsRegular
	//全名:		YSLib::Drawing::Rect::IsInBoundsRegular
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	int px
	//形式参数:	int py
	//功能概要:	判断点 (px, py) 是否在矩形内或左侧或上侧边上。
	//备注:		
	//********************************
	bool
	IsInBoundsRegular(int px, int py) const;
	//********************************
	//名称:		IsInBoundsStrict
	//全名:		YSLib::Drawing::Rect::IsInBoundsStrict
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const Point & p
	//功能概要:	判断点是否在矩形内。
	//备注:		
	//********************************
	bool
	IsInBoundsStrict(const Point& p) const;
	//********************************
	//名称:		IsInBoundsStrict
	//全名:		YSLib::Drawing::Rect::IsInBoundsStrict
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	int px
	//形式参数:	int py
	//功能概要:	判断点 (px, py) 是否在矩形内。
	//备注:		
	//********************************
	bool
	IsInBoundsStrict(int px, int py) const;
	//********************************
	//名称:		GetPoint
	//全名:		YSLib::Drawing::Rect::GetPoint
	//可访问性:	public 
	//返回类型:	YSLib::Drawing::Point
	//修饰符:	const
	//功能概要:	
	//备注:		
	//********************************

	DefGetter(Point, Point, Point(X, Y)) //取左上角位置。
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
	return static_cast<Point>(a) == static_cast<Point>(b)
		&& static_cast<Size>(a) == static_cast<Size>(b);
}
inline bool
operator!=(const Rect& a, const Rect& b)
{
	return !(a == b);
}

inline bool
Rect::IsInBounds(const Point& p) const
{
	return sgnInterval<SPOS>(p.X, X, (X + Width)) >= 0
		&& sgnInterval<SPOS>(p.Y, Y, (Y + Height)) >= 0;
}
inline bool
Rect::IsInBounds(int px, int py) const
{
	return sgnInterval<SPOS>(px, X, (X + Width)) >= 0
		&& sgnInterval<SPOS>(py, Y, (Y + Height)) >= 0;
}
inline bool
Rect::IsInBoundsRegular(const Point& p) const
{
	return isInIntervalRegular<int>(p.X - X, Width)
		&& isInIntervalRegular<int>(p.Y - Y, Height);
}
inline bool
Rect::IsInBoundsRegular(int px, int py) const
{
	return isInIntervalRegular<int>(px - X, Width)
		&& isInIntervalRegular<int>(py - Y, Height);
}
inline bool
Rect::IsInBoundsStrict(const Point& p) const
{
	return isInIntervalRegular<int>(p.X - X, Width)
		&& isInIntervalRegular<int>(p.Y - Y, Height);
}
inline bool
Rect::IsInBoundsStrict(int px, int py) const
{
	return isInIntervalStrict<int>(px - X, Width)
		&& isInIntervalStrict<int>(py - Y, Height);
}

YSL_END_NAMESPACE(Drawing)

//基本对象定义。

//基本对象类：所有类名以 Y 作前缀的类的公共基类。
class YObject : private NonCopyable
{
public:
	//********************************
	//名称:		~YObject
	//全名:		YSLib::YObject::~YObject
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构。
	//备注:		空实现。必要的虚函数以构造多态基类。
	//********************************
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
	//********************************
	//名称:		YCountableObject
	//全名:		YSLib::YCountableObject::YCountableObject
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		保护实现。
	//********************************
	YCountableObject();
};

inline
YCountableObject::YCountableObject()
{}

YSL_END

#endif

