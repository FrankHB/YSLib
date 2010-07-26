// YSLib::Core::YObject by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58;
// UTime = 2010-7-26 8:01;
// Version = 0.2144;


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

class SOBG;
class SPoint;
class SVec;
class SSize;
class SRect;


class SOBG //屏幕二元组。
{
public:
	SPOS X, Y; //分量。

	SOBG()
	: X(0), Y(0)
	{}
	explicit
	SOBG(const SSize&);
	template<typename _type>
	explicit
	SOBG(const _type& v)
	: X(v.GetX()), Y(v.GetY())
	{}
	template<typename _type>
	SOBG(_type x, _type y)
	: X(x), Y(y)
	{}

	SOBG
	operator-() //负运算：返回相反元素。
	{
		return SOBG(-X, -Y);
	}

	DefGetter(SPOS, X, X)
	DefGetter(SPOS, Y, Y)
};

inline bool
operator==(const SOBG& a, const SOBG& b)
{
	return a.X == b.X && a.Y == b.Y;
}
inline bool
operator!=(const SOBG& a, const SOBG& b)
{
	return !(a == b);
}


class SPoint : public SOBG //屏幕二维点坐标。
{
public:
	typedef SOBG ParentType;

	static const SPoint Zero; //默认构造参数构造的原点对象。
	static const SPoint FullScreen; //默认构造参数构造的屏幕右下角边界（不在屏幕坐标系中）对象。

	SPoint()
	: SOBG()
	{}
	explicit
	SPoint(const SSize&);
	template<typename _type>
	explicit
	SPoint(const _type& v)
	: SOBG(v.GetX(), v.GetY())
	{}
	template<typename _type>
	SPoint(_type x, _type y)
	: SOBG(x, y)
	{}

	operator SVec() const;

	SPoint
	operator-() //负运算：返回与原点对称的元素。
	{
		return SPoint(-X, -Y);
	}
	SPoint&
	operator+=(const SVec&);
	SPoint&
	operator-=(const SVec&);
};


class SVec : public SOBG //屏幕二维向量。
{
public:
	typedef SOBG ParentType;

	static const SVec Zero; //默认构造参数构造的零向量对象。
	static const SVec FullScreen; //默认构造参数构造的由屏幕坐标系原点到屏幕右下角边界（不在屏幕坐标系中）对象。

	SVec()
	: SOBG() {};
	explicit
	SVec(const SSize&);
	template<typename _type>
	explicit
	SVec(const _type& v)
	: SOBG(v.GetX(), v.GetY())
	{}
	template<typename _type>
	SVec(_type x, _type y)
	: SOBG(x, y)
	{}

	operator SPoint() const
	{
		return SPoint(X, Y);
	}

	SVec
	operator-() //负运算：返回相反向量。
	{
		return SVec(-X, -Y);
	}
	SVec&
	operator+=(const SVec& v)
	{
		X += v.X;
		Y += v.Y;
		return *this;
	}
	SVec&
	operator-=(const SVec& v)
	{
		X -= v.X;
		Y -= v.Y;
		return *this;
	}
};


inline
SPoint::operator SVec() const
{
	return SVec(X, Y);
}

inline SPoint&
SPoint::operator+=(const SVec& v)
{
	X += v.X;
	Y += v.Y;
	return *this;
}
inline SPoint&
SPoint::operator-=(const SVec& v)
{
	X -= v.X;
	Y -= v.Y;
	return *this;
}


//屏幕二维向量二元运算。

inline SVec
operator-(const SPoint& a, const SPoint& b)
{
	return SVec(a.X - b.X, a.Y - b.Y);
}
inline SVec
operator+(const SVec& a, const SVec& b)
{
	return SVec(a.X + b.X, a.Y + b.Y);
}
inline SVec
operator-(const SVec& a, const SVec& b)
{
	return SVec(a.X - b.X, a.Y - b.Y);
}


struct SSize //屏幕区域大小。
{
	SDST Width, Height; //宽和高。

	static const SSize Zero; //默认构造参数构造的零元素对象。
	static const SSize FullScreen; //默认构造参数构造的全屏幕对象。

	SSize()
	{}
	template<typename _type>
	explicit
	SSize(const _type& v)
	: Width(v.GetX()), Height(v.GetY())
	{}
	template<typename _type>
	SSize(_type w, _type h)
	: Width(w), Height(h)
	{}

	operator SVec() const
	{
		return SVec(Width, Height);
	}

	DefGetter(SDST, Width, Width)
	DefGetter(SDST, Height, Height)
	DefGetter(u32, Area, Width * Height) //取面积。
};

inline bool
operator==(const SSize& a, const SSize& b)
{
	return a.Width == b.Width && a.Height == b.Height;
}
inline bool
operator!=(const SSize& a, const SSize& b)
{
	return !(a == b);
}


inline
SOBG::SOBG(const SSize& s)
: X(s.Width), Y(s.Height)
{}

inline
SPoint::SPoint(const SSize& s)
: SOBG(s.GetWidth(), s.GetHeight())
{}

inline
SVec::SVec(const SSize& s)
: SOBG(s.GetWidth(), s.GetHeight())
{}


class SRect : public SPoint, public SSize //屏幕坐标系的正则（边平行于水平直线的）矩形：使用左上点屏幕坐标、宽和高表示。
{
public:
	static const SRect Empty; //默认构造参数构造的空矩形对象。
	static const SRect FullScreen; //默认构造参数构造的全屏幕矩形对象。

	SRect();
	explicit
	SRect(const SPoint&);
	explicit
	SRect(const SSize&);
	SRect(const SPoint&, const SSize&);
	SRect(const SPoint&, SDST, SDST);
	SRect(SPOS, SPOS, const SSize&);
	SRect(SPOS, SPOS, SDST, SDST);

	bool
	IsInBounds(const SPoint&) const; //判断点是否在矩形内或边上。
	bool
	IsInBounds(int px, int py) const; //判断点 (px, py) 是否在矩形内或边上。
	bool
	IsInBoundsRegular(const SPoint& p) const; //判断点是否在矩形内或左侧或上侧边上。
	bool
	IsInBoundsRegular(int px, int py) const; //判断点 (px, py) 是否在矩形内或左侧或上侧边上。
	bool
	IsInBoundsStrict(const SPoint& p) const; //判断点是否在矩形内。
	bool
	IsInBoundsStrict(int px, int py) const; //判断点 (px, py) 是否在矩形内。
	SPoint
	GetPoint() const; //取左上角位置。
};

inline
SRect::SRect()
: SPoint(), SSize()
{}
inline
SRect::SRect(const SPoint& p)
: SPoint(p), SSize()
{}
inline
SRect::SRect(const SSize& s)
: SPoint(), SSize(s)
{}
inline
SRect::SRect(const SPoint& p, const SSize& s)
: SPoint(p), SSize(s)
{}
inline
SRect::SRect(const SPoint& p, SDST w, SDST h)
: SPoint(p.GetX(), p.GetY()), SSize(w, h)
{}
inline
SRect::SRect(SPOS x, SPOS y, const SSize& s)
: SPoint(x, y), SSize(s.GetWidth(), s.GetHeight())
{}
inline
SRect::SRect(SPOS x, SPOS y, SDST w, SDST h)
: SPoint(x, y), SSize(w, h)
{}

inline bool
operator==(const SRect& a, const SRect& b)
{
	return static_cast<SPoint>(a) == static_cast<SPoint>(b) && static_cast<SSize>(a) == static_cast<SSize>(b);
}
inline bool
operator!=(const SRect& a, const SRect& b)
{
	return !(a == b);
}

inline bool
SRect::IsInBounds(const SPoint& p) const
{
	return sgnInterval<SPOS>(p.X, X, (X + Width)) >= 0 && sgnInterval<SPOS>(p.Y, Y, (Y + Height)) >= 0;
}
inline bool
SRect::IsInBounds(int px, int py) const
{
	return sgnInterval<SPOS>(px, X, (X + Width)) >= 0 && sgnInterval<SPOS>(py, Y, (Y + Height)) >= 0;
}
inline bool
SRect::IsInBoundsRegular(const SPoint& p) const
{
	return isInIntervalRegular<int>(p.X - X, Width) && isInIntervalRegular<int>(p.Y - Y, Height);
}
inline bool
SRect::IsInBoundsRegular(int px, int py) const
{
	return isInIntervalRegular<int>(px - X, Width) && isInIntervalRegular<int>(py - Y, Height);
}
inline bool
SRect::IsInBoundsStrict(const SPoint& p) const
{
	return isInIntervalRegular<int>(p.X - X, Width) && isInIntervalRegular<int>(p.Y - Y, Height);
}
inline bool
SRect::IsInBoundsStrict(int px, int py) const
{
	return isInIntervalStrict<int>(px - X, Width) && isInIntervalStrict<int>(py - Y, Height);
}
inline SPoint
SRect::GetPoint() const
{
	return SPoint(X, Y);
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

