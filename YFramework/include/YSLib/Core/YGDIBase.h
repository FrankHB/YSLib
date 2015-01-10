/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YGDIBase.h
\ingroup Core
\brief 平台无关的基础图形学对象。
\version r1842
\author FrankHB <frankhb1989@gmail.com>
\since build 563
\par 创建时间:
	2011-05-03 07:20:51 +0800
\par 修改时间:
	2014-01-02 09:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YGDIBase
*/


#ifndef YSL_INC_Core_YGDIBase_h_
#define YSL_INC_Core_YGDIBase_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YException
#include <limits>
#include <ystdex/utility.hpp> // for ystdex::cloneable;

namespace YSLib
{

namespace Drawing
{

class Size;
class Rect;


/*!
\brief 屏幕二元组。
\warning 非虚析构。
\since build 242
*/
template<typename _type>
class GBinaryGroup
{
public:
	static const GBinaryGroup Invalid; //!< 无效（不在屏幕坐标系中）对象。

	_type X, Y; //!< 分量。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	\since build 319
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
	\since build 319
	*/
	explicit yconstfn
	GBinaryGroup(const Size&) ynothrow;
	/*!
	\brief 构造：使用 Rect 对象。
	\since build 319
	*/
	explicit yconstfn
	GBinaryGroup(const Rect&) ynothrow;
	/*!
	\brief 构造：使用两个纯量。
	\tparam _tScalar1 第一分量纯量类型。
	\tparam _tScalar2 第二分量纯量类型。
	\warning 模板参数和 _type 符号不同时隐式转换可能改变符号，不保证唯一结果。
	\since build 319
	*/
	template<typename _tScalar1, typename _tScalar2>
	yconstfn
	GBinaryGroup(_tScalar1 x, _tScalar2 y) ynothrow
		: X(_type(x)), Y(_type(y))
	{}
	/*!
	\brief 构造：使用纯量对。
	\note 使用 std::get 取分量。仅取前两个分量。
	\since build 481
	*/
	template<typename _tPair>
	yconstfn
	GBinaryGroup(const _tPair& pr) ynothrow
		: X(std::get<0>(pr)), Y(std::get<1>(pr))
	{}

	//! \since build 554
	DefDeCopyAssignment(GBinaryGroup)

	/*!
	\brief 负运算：取加法逆元。
	\since build 319
	*/
	yconstfn PDefHOp(GBinaryGroup, -, ) const ynothrow
		ImplRet(GBinaryGroup(-X, -Y))

	/*!
	\brief 加法赋值。
	*/
	PDefHOp(GBinaryGroup&, +=, const GBinaryGroup& val) ynothrow
		ImplRet(yunseq(X += val.X, Y += val.Y), *this)
	/*!
	\brief 减法赋值。
	*/
	PDefHOp(GBinaryGroup&, -=, const GBinaryGroup& val) ynothrow
		ImplRet(yunseq(X -= val.X, Y -= val.Y), *this)

	yconstfn DefGetter(const ynothrow, _type, X, X)
	yconstfn DefGetter(const ynothrow, _type, Y, Y)

	DefSetter(_type, X, X)
	DefSetter(_type, Y, Y)

	/*!
	\brief 判断是否是零元素。
	*/
	yconstfn DefPred(const ynothrow, Zero, X == 0 && Y == 0)

	/*!
	\brief 选择分量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 319
	*/
	//@{
	PDefH(_type&, GetRef, bool b = true) ynothrow
		ImplRet(b ? X : Y)
	PDefH(const _type&, GetRef, bool b = true) const ynothrow
		ImplRet(b ? X : Y)
	//@}
};

//! \relates GBinaryGroup
//@{
template<typename _type>
const GBinaryGroup<_type> GBinaryGroup<_type>::Invalid{
	std::numeric_limits<_type>::lowest(), std::numeric_limits<_type>::lowest()};

/*!
\brief 比较：屏幕二元组相等关系。
\since build 319
*/
template<typename _type>
yconstfn bool
operator==(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b) ynothrow
{
	return a.X == b.X && a.Y == b.Y;
}

/*!
\brief 比较：屏幕二元组不等关系。
\since build 319
*/
template<typename _type>
yconstfn bool
operator!=(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b) ynothrow
{
	return !(a == b);
}

/*!
\brief 加法：屏幕二元组。
\since build 319
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator+(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b) ynothrow
{
	return GBinaryGroup<_type>(a.X + b.X, a.Y + b.Y);
}

/*!
\brief 减法：屏幕二元组。
\since build 319
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator-(const GBinaryGroup<_type>& a, const GBinaryGroup<_type>& b) ynothrow
{
	return GBinaryGroup<_type>(a.X - b.X, a.Y - b.Y);
}

/*!
\brief 数乘：屏幕二元组。
\since build 319
*/
template<typename _type, typename _tScalar>
yconstfn GBinaryGroup<_type>
operator*(const GBinaryGroup<_type>& val, _tScalar l) ynothrow
{
	return GBinaryGroup<_type>(val.X * l, val.Y * l);
}

/*!
\brief 取分量。
\since build 554
*/
//@{
template<size_t _vIdx, typename _type>
yconstfn _type&
get(GBinaryGroup<_type>& val)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? val.X : val.Y;
}
template<size_t _vIdx, typename _type>
yconstfn const _type&
get(const GBinaryGroup<_type>& val)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? val.X : val.Y;
}
//@}
//@}


/*!
\brief 屏幕二维点（直角坐标表示）。
\since build 242
*/
using Point = GBinaryGroup<SPos>;


/*!
\brief 屏幕二维向量（直角坐标表示）。
\since build 242
*/
using Vec = GBinaryGroup<SPos>;


/*!
\brief 屏幕区域大小。
\warning 非虚析构。
\since build 161
*/
class YF_API Size
{
public:
	/*!
	\brief 无效对象。
	\since build 296
	*/
	static const Size Invalid;

	SDst Width, Height; //!< 宽和高。

	/*!
	\brief 无参数构造。
	\note 零初始化。
	\since build 319
	*/
	yconstfn
	Size() ynothrow
		: Width(0), Height(0)
	{}
	/*!
	\brief 复制构造。
	\since build 319
	*/
	yconstfn
	Size(const Size& s) ynothrow
		: Width(s.Width), Height(s.Height)
	{}
	/*!
	\brief 构造：使用 Rect 对象。
	\since build 319
	*/
	explicit yconstfn
	Size(const Rect&) ynothrow;
	/*!
	\brief 构造：使用屏幕二元组。
	\since build 319
	*/
	template<typename _type>
	explicit yconstfn
	Size(const GBinaryGroup<_type>& val) ynothrow
		: Width(val.X), Height(val.Y)
	{}
	/*!
	\brief 构造：使用两个纯量。
	\since build 319
	*/
	template<typename _tScalar1, typename _tScalar2>
	yconstfn
	Size(_tScalar1 w, _tScalar2 h) ynothrow
		: Width(w), Height(h)
	{}

	//! \since build 554
	DefDeCopyAssignment(Size)

	/*!
	\brief 判断是否为空或非空。
	\since build 320
	*/
	yconstfn DefBoolNeg(yconstfn explicit, Width != 0 || Height != 0)

	/*!
	\brief 求与另一个屏幕区域大小的交。
	\note 结果由分量最小值构造。
	\since build 555
	*/
	PDefHOp(Size&, &=, const Size& s) ynothrow
		ImplRet(yunseq(Width = min(Width, s.Width),
			Height = min(Height, s.Height)), *this)

	/*!
	\brief 求与另一个屏幕标准矩形的并。
	\note 结果由分量最大值构造。
	\since build 555
	*/
	PDefHOp(Size&, |=, const Size& s) ynothrow
		ImplRet(yunseq(Width = max(Width, s.Width),
			Height = max(Height, s.Height)), *this)

	/*!
	\brief 转换：屏幕二维向量。
	\note 以Width 和 Height 分量作为结果的 X 和 Y分量。
	*/
	yconstfn DefCvt(const ynothrow, Vec, Vec(Width, Height))

	/*!
	\brief 判断是否为线段：长或宽中有且一个数值等于 0 。
	\since build 264
	*/
	yconstfn DefPred(const ynothrow, LineSegment,
		!((Width == 0) ^ (Height == 0)))
	/*!
	\brief 判断是否为不严格的空矩形区域：包括空矩形和线段。
	\since build 264
	*/
	yconstfn DefPred(const ynothrow, UnstrictlyEmpty, Width == 0 || Height == 0)

	/*!
	\brief 选择分量引用。
	\note 第二参数为 true 时选择第一分量，否则选择第二分量。
	\since build 319
	*/
	//@{
	PDefH(SDst&, GetRef, bool b = true) ynothrow
		ImplRet(b ? Width : Height)
	PDefH(const SDst&, GetRef, bool b = true) const ynothrow
		ImplRet(b ? Width : Height)
	//@}
};

/*!
\relates Size
\since build 319
*/
//@{
//! \brief 比较：屏幕区域大小相等关系。
yconstfn PDefHOp(bool, ==, const Size& a, const Size& b) ynothrow
	ImplRet(a.Width == b.Width && a.Height == b.Height)

//! \brief 比较：屏幕区域大小不等关系。
yconstfn PDefHOp(bool, !=, const Size& a, const Size& b) ynothrow
	ImplRet(!(a == b))

/*!
\brief 求两个屏幕区域大小的交。
\sa Size::operator&=
\since build 555
*/
yconstfn PDefHOp(Size, &, const Size& a, const Size& b) ynothrow
	ImplRet({a.Width < b.Width ? a.Width : b.Width,
		a.Height < b.Height ? a.Height : b.Height})

/*!
\brief 求两个屏幕区域大小的并。
\sa Size::operator|=
\since build 555
*/
yconstfn PDefHOp(Size, |, const Size& a, const Size& b) ynothrow
	ImplRet({a.Width < b.Width ? b.Width : a.Width,
		a.Height < b.Height ? b.Height : a.Height})

//! \brief 取面积。
yconstfn PDefH(auto, GetAreaOf, const Size& s) ynothrow
	-> decltype(s.Width * s.Height)
	ImplRet(s.Width * s.Height)

/*!
\brief 取分量。
\since build 554
*/
//@{
template<size_t _vIdx>
yconstfn SDst&
get(Size& s)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? s.Width : s.Height;
}
template<size_t _vIdx>
yconstfn const SDst&
get(const Size& s)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? s.Width : s.Height;
}
//@}
//@}


/*!
\brief 加法：使用屏幕二元组和屏幕区域大小分量对应相加构造屏幕二元组。
\since build 319
*/
template<typename _type>
yconstfn GBinaryGroup<_type>
operator+(GBinaryGroup<_type> val, const Size& s) ynothrow
{
	return GBinaryGroup<_type>(val.X + s.Width, val.Y + s.Height);
}


/*!
\brief 二元对象转置。
\since build 319
*/
template<class _tBinary>
yconstfn _tBinary
Transpose(_tBinary& obj) ynothrow
{
	return _tBinary(obj.Y, obj.X);
}


/*!
\brief 计算两个大小和阈值限定的最小缩放值。
\since build 555
*/
template<typename _tScalar = float>
_tScalar
ScaleMin(const Size& x, const Size& y, _tScalar threshold = 1.F)
{
	return YSLib::min({threshold,
		_tScalar(_tScalar(x.Width) / _tScalar(y.Width)),
		_tScalar(_tScalar(x.Height) / _tScalar(y.Width))});
}


/*!
\brief 屏幕标准矩形：表示屏幕矩形区域。
\note 边平行于水平直线；使用左上点屏幕坐标、宽和高表示。
\warning 非虚析构。
\since build 161
*/
class YF_API Rect : private Point, private Size
{
public:
	/*!
	\brief 无效对象。
	\since build 296
	*/
	static const Rect Invalid;

	/*!
	\brief 左上角横坐标。
	\sa Point::X
	\since build 296
	*/
	using Point::X;
	/*!
	\brief 左上角纵坐标。
	\sa Point::Y
	\since build 296
	*/
	using Point::Y;
	/*!
	\brief 长。
	\sa Size::Width
	\since build 296
	*/
	using Size::Width;
	/*!
	\brief 宽。
	\sa Size::Height
	\since build 296
	*/
	using Size::Height;


	/*!
	\brief 无参数构造：默认实现。
	\since build 453
	*/
	DefDeCtor(Rect)
	/*!
	\brief 构造：使用屏幕二维点。
	\since build 319
	*/
	explicit yconstfn
	Rect(const Point& pt) ynothrow
		: Point(pt), Size()
	{}
	/*!
	\brief 构造：使用 Size 对象。
	\since build 399
	*/
	yconstfn
	Rect(const Size& s) ynothrow
		: Point(), Size(s)
	{}
	/*!
	\brief 构造：使用屏幕二维点和 Size 对象。
	\since build 319
	*/
	yconstfn
	Rect(const Point& pt, const Size& s) ynothrow
		: Point(pt), Size(s)
	{}
	/*!
	\brief 构造：使用屏幕二维点和表示长宽的两个 SDst 值。
	\since build 319
	*/
	yconstfn
	Rect(const Point& pt, SDst w, SDst h) ynothrow
		: Point(pt.X, pt.Y), Size(w, h)
	{}
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和 Size 对象。
	\since build 319
	*/
	yconstfn
	Rect(SPos x, SPos y, const Size& s) ynothrow
		: Point(x, y), Size(s.Width, s.Height)
	{}
	/*!
	\brief 构造：使用表示位置的两个 SPos 值和表示大小的两个 SDst 值。
	\since build 319
	*/
	yconstfn
	Rect(SPos x, SPos y, SDst w, SDst h) ynothrow
		: Point(x, y), Size(w, h)
	{}
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstfn DefDeCopyCtor(Rect)

	DefDeCopyAssignment(Rect)
	//! \since build 319
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
	\brief 求与另一个屏幕标准矩形的交。
	\note 若相离结果为 Rect() ，否则为包含于两个参数中的最大矩形。
	\since build 320
	*/
	Rect&
	operator&=(const Rect&) ynothrow;

	/*!
	\brief 求与另一个屏幕标准矩形的并。
	\note 结果为包含两个参数中的最小矩形。
	\since build 320
	*/
	Rect&
	operator|=(const Rect&) ynothrow;

	/*!
	\brief 判断是否为空。
	\sa Size::operator!
	\since build 320
	*/
	using Size::operator!;

	/*!
	\brief 判断是否非空。
	\sa Size::bool
	\since build 320
	*/
	using Size::operator bool;

	/*!
	\brief 判断点 (px, py) 是否在矩形内或边上。
	\since build 528
	*/
	bool
	Contains(SPos px, SPos py) const ynothrow;
	/*!
	\brief 判断点 pt 是否在矩形内或边上。
	\since build 319
	*/
	PDefH(bool, Contains, const Point& pt) const ynothrow
		ImplRet(Contains(pt.X, pt.Y))
	/*!
	\brief 判断矩形是否在矩形内或边上。
	\note 空矩形总是不被包含。
	\since build 319
	*/
	bool
	Contains(const Rect&) const ynothrow;

	/*!
	\brief 判断点 (px, py) 是否在矩形内。
	\since build 528
	*/
	bool
	ContainsStrict(SPos px, SPos py) const ynothrow;
	/*!
	\brief 判断点 pt 是否在矩形内。
	\since build 319
	*/
	PDefH(bool, ContainsStrict, const Point& pt) const ynothrow
		ImplRet(ContainsStrict(pt.X, pt.Y))
	/*!
	\brief 判断矩形是否在矩形内或边上。
	\note 空矩形总是不被包含。
	\since build 319
	*/
	bool
	ContainsStrict(const Rect&) const ynothrow;

	/*!
	\brief 判断矩形是否为线段：长和宽中有且一个数值等于 0 。
	\sa Size::IsLineSegment
	\since build 296
	*/
	using Size::IsLineSegment;
	/*!
	\brief 判断矩形是否为不严格的空矩形区域：包括空矩形和线段。
	\sa Size::IsUnstrictlyEmpty
	\since build 296
	*/
	using Size::IsUnstrictlyEmpty;

	/*!
	\brief 取左上角位置。
	*/
	yconstfn DefGetter(const ynothrow, const Point&, Point,
		static_cast<const Point&>(*this))
	/*!
	\brief 取左上角位置引用。
	\since build 296
	*/
	DefGetter(ynothrow, Point&, PointRef, static_cast<Point&>(*this))
	/*!
	\brief 取大小。
	*/
	yconstfn DefGetter(const ynothrow, const Size&, Size,
		static_cast<const Size&>(*this))
	/*!
	\brief 取大小引用。
	\since build 296
	*/
	DefGetter(ynothrow, Size&, SizeRef, static_cast<Size&>(*this))
};

//! \relates Rect
//@{
/*!
\brief 比较：屏幕标准矩形相等关系。
\since build 319
*/
yconstfn PDefHOp(bool, ==, const Rect& x, const Rect& y) ynothrow
	ImplRet(x.GetPoint() == y.GetPoint() && x.GetSize() == y.GetSize())

/*!
\brief 比较：屏幕标准矩形不等关系。
\since build 319
*/
yconstfn PDefHOp(bool, !=, const Rect& x, const Rect& y) ynothrow
	ImplRet(!(x == y))

/*!
\brief 加法：使用标准矩形 r 和偏移向量 v 构造屏幕标准矩形。
\since build 319
*/
yconstfn PDefHOp(Rect, +, const Rect& r, const Vec& v) ynothrow
	ImplRet({r.GetPoint() + v, r.GetSize()})

/*!
\brief 减法：使用标准矩形 r 和偏移向量的加法逆元 v 构造屏幕标准矩形。
\since build 319
*/
yconstfn PDefHOp(Rect, -, const Rect& r, const Vec& v) ynothrow
	ImplRet({r.GetPoint() - v, r.GetSize()})

/*!
\brief 求两个屏幕标准矩形的交。
\sa Rect::operator&=
\since build 555
*/
inline PDefHOp(Rect, &, const Rect& a, const Rect& b) ynothrow
	ImplRet(Rect(a) &= b)

/*!
\brief 求两个屏幕标准矩形的并。
\sa Rect::operator|=
\since build 555
*/
inline PDefHOp(Rect, |, const Rect& a, const Rect& b) ynothrow
	ImplRet(Rect(a) |= b)

/*!
\brief 按指定大小缩小矩形。
\pre 断言：矩形的当前大小足够缩小。
\since build 479
*/
inline PDefH(void, Diminish, Rect& r, SDst off1 = 1, SDst off2 = 2)
	ImplExpr(YAssert(r.Width > off2 && r.Height > off2,
		"Boundary is too small."),
		yunseq(r.X += off1, r.Y += off1, r.Width -= off2, r.Height -= off2))

/*!
\brief 取分量。
\since build 554
*/
//@{
template<size_t _vIdx>
yconstfn ystdex::conditional_t<_vIdx < 2, SPos, SDst>&
get(Rect& r)
{
	return std::get<_vIdx>(std::tuple<SPos&, SPos&, SDst&, SDst&>(r.X, r.Y,
		r.Width, r.Height));
}
template<size_t _vIdx>
yconstfn const ystdex::conditional_t<_vIdx < 2, SPos, SDst>&
get(const Rect& r)
{
	return std::get<_vIdx>(std::tuple<const SPos&, const SPos&, const SDst&,
		const SDst&>(r.X, r.Y, r.Width, r.Height));
}
//@}
//@}


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
\brief 二维图形接口上下文模板。
\warning 非虚析构。
\since build 559
*/
template<typename _tPointer, class _tSize = Size>
class GGraphics
{
	static_assert(std::is_nothrow_copy_constructible<_tPointer>::value,
		"Invalid pointer type found.");
	static_assert(std::is_nothrow_copy_constructible<_tSize>::value,
		"Invalid size type found.");

public:
	using PointerType = _tPointer;
	using SizeType = _tSize;
	using PixelType = ystdex::decay_t<decltype(*PointerType())>;

protected:
	/*!
	\brief 显示缓冲区指针。
	\warning 不应视为具有所有权。
	*/
	PointerType pBuffer{};
	//! \brief 图形区域大小。
	SizeType sGraphics{};

public:
	//! \brief 默认构造：使用空指针和大小。
	DefDeCtor(GGraphics)
	//! \brief 构造：使用指定位图指针和大小。
	explicit yconstfn
	GGraphics(PointerType b, const SizeType& s = {}) ynothrow
		: pBuffer(b), sGraphics(s)
	{}
	//! \brief 构造：使用其它类型的指定位图指针和大小。
	template<typename _tPointer2, class _tSize2>
	explicit yconstfn
	GGraphics(_tPointer2 b, const _tSize2& s = {}) ynothrow
		: GGraphics(static_cast<PointerType>(b), static_cast<SizeType>(s))
	{}
	//! \brief 构造：使用其它类型的指定位图指针和大小类型的二维图形接口上下文。
	template<typename _tPointer2, class _tSize2>
	yconstfn
	GGraphics(const GGraphics<_tPointer2, _tSize2>& g) ynothrow
		: GGraphics(g.GetBufferPtr(), g.GetSize())
	{}
	//! \brief 复制构造：浅复制。
	DefDeCopyCtor(GGraphics)
	DefDeMoveCtor(GGraphics)

	DefDeCopyAssignment(GGraphics)
	DefDeMoveAssignment(GGraphics)

	/*!
	\brief 判断无效或有效性。
	\since build 319
	*/
	DefBoolNeg(explicit,
		bool(pBuffer) && sGraphics.Width != 0 && sGraphics.Height != 0)

	/*!
	\brief 取指定行首元素指针。
	\pre 断言：参数不越界。
	\pre 间接断言：缓冲区指针非空。
	*/
	PointerType
	operator[](size_t r) const ynothrow
	{
		YAssert(r < sGraphics.Height, "Access out of range.");
		return Nonnull(pBuffer) + r * sGraphics.Width;
	}

	DefGetter(const ynothrow, PointerType, BufferPtr, pBuffer)
	DefGetter(const ynothrow, const SizeType&, Size, sGraphics)
	//! \since build 196
	DefGetter(const ynothrow, SDst, Width, sGraphics.Width)
	//! \since build 196
	DefGetter(const ynothrow, SDst, Height, sGraphics.Height)
	//! \since build 177
	DefGetter(const ynothrow, size_t, SizeOfBuffer,
		sizeof(PixelType) * GetAreaOf(sGraphics)) //!< 取缓冲区占用空间。

	/*!
	\brief 取指定行首元素指针。
	\throw GeneralEvent 缓冲区指针为空。
	\throw std::out_of_range 参数越界。
	\since build 563
	*/
	PointerType
	at(size_t r) const
	{
		if(YB_UNLIKELY(!pBuffer))
			throw GeneralEvent("Null pointer found.");
		if(YB_UNLIKELY(r >= sGraphics.Height))
			throw std::out_of_range("Access out of range.");

		return pBuffer + r * sGraphics.Width;
	}
};


using BitmapPtr = Pixel*;
using ConstBitmapPtr = const Pixel*;

//! \since build 559
using ConstGraphics = GGraphics<ConstBitmapPtr>;

//! \since build 559
using Graphics = GGraphics<BitmapPtr>;


/*!
\brief 图像接口。
\since build 405
*/
DeclDerivedI(YF_API, IImage, ystdex::cloneable)
	//! \since build 406
	DeclIEntry(const Graphics& GetContext() const ynothrow)
	DeclIEntry(void SetSize(const Size&))

	//! \since build 409
	DeclIEntry(IImage* clone() const ImplI(ystdex::cloneable))
EndDecl


/*
\brief 绘制上下文。
\warning 非虚析构。
\since build 255
*/
struct YF_API PaintContext
{
	Graphics Target; //!< 渲染目标：图形接口上下文。
	/*!
	\brief 参考位置。

	指定渲染目标关联的参考点的位置的偏移坐标。
	除非另行约定，选取渲染目标左上角为原点的屏幕坐标系。
	*/
	Point Location;
	/*!
	\brief 剪切区域。

	相对图形接口上下文的标准矩形，指定需要保证被刷新的边界区域。
	除非另行约定，剪切区域的位置坐标选取渲染目标左上角为原点的屏幕坐标系。
	*/
	Rect ClipArea;
};

/*!
\brief 根据目标的边界更新剪切区域。
\relates PaintContext
\since build 453
*/
//@{
inline PDefH(void, UpdateClipArea, PaintContext& pc, const Rect& r)
	ImplExpr(pc.ClipArea = r & Rect(pc.Target.GetSize()))

inline PDefH(void, UpdateClipSize, PaintContext& pc, const Size& s)
	ImplExpr(UpdateClipArea(pc, {pc.Location, s}))
//@}


/*!
\brief 逆时针旋转角度指示输出指向。
\note 保证底层为无符号整数类型。
\since build 416
*/
enum Rotation : yimpl(size_t)
{
	RDeg0 = 0,
	RDeg90 = 1,
	RDeg180 = 2,
	RDeg270 = 3
};

/*!
\brief 二元方向。
\since build 416
*/
enum Orientation
{
	Horizontal = 0,
	Vertical = 1
};

} // namespace Drawing;

} // namespace YSLib;

#endif

