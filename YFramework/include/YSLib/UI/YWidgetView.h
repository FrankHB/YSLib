/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YWidgetView.h
\ingroup UI
\brief 样式无关的 GUI 部件。
\version r664
\author FrankHB <frankhb1989@gmail.com>
\since build 568
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2015-01-22 18:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetView
*/


#ifndef YSL_INC_UI_YWidgetView_h_
#define YSL_INC_UI_YWidgetView_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YComponent

namespace YSLib
{

namespace UI
{

/*!
\brief 方向模块。
\since build 170
*/
class YF_API MOriented
{
protected:
	Drawing::Orientation Orientation;

	explicit
	MOriented(Drawing::Orientation o)
		: Orientation(o)
	{}

	DefGetter(const ynothrow, Drawing::Orientation, Orientation, Orientation)
};


/*!
\brief 可视状态。
\note 成员使用非虚 \c public 实现。
\warning 非虚析构。
\since build 168
*/
class YF_API Visual
{
public:
	/*
	\brief 可见性。
	\since build 568
	*/
	bool Visible{true};
	/*!
	\brief 部件边界：表示左上角所在位置（相对容器的偏移坐标）和大小。
	\since build 568
	*/
	Rect Bounds{};

	//! \since build 568
	DefDeCtor(Visual)
	/*!
	\brief 构造：使用指定边界。
	\since build 337
	*/
	Visual(const Rect& r)
		: Bounds(r)
	{}
};


/*!
\brief 部件视图。
\since build 259
*/
class YF_API View
{
private:
	Visual visual; //!< 当前可视状态。

public:
	//! \since build 375
	//@{
	mutable IWidget* ContainerPtr; //!< 从属的部件容器的指针。
	/*!
	\brief 从属的部件指针。

	逻辑依赖的部件指针，用于提供边界、容器和焦点以外属性的默认值。
	*/
	mutable IWidget* DependencyPtr;
	mutable IWidget* FocusingPtr; //!< 焦点指针。
	//@}

	/*!
	\brief 构造：使用指定边界、前景色和背景色。
	\since build 337
	*/
	View(const Rect& r = {})
		: visual(r), ContainerPtr(), DependencyPtr(), FocusingPtr()
	{}
	View(const View& v)
		: visual(v.visual), ContainerPtr(), DependencyPtr(), FocusingPtr()
	{}
	View(View&& v)
		: visual(v.visual), ContainerPtr(v.ContainerPtr),
		DependencyPtr(v.DependencyPtr), FocusingPtr(v.FocusingPtr)
	{
		yunseq(v.ContainerPtr = {}, v.DependencyPtr = {}, v.FocusingPtr = {});
	}
	virtual DefDeDtor(View)

	/*!
	\brief 复制赋值：仅可视状态。
	\since build 295
	*/
	PDefHOp(View&, =, const View& v)
		ImplRet(visual = v.visual, *this)
	/*!
	\brief 转移赋值：仅可视状态。
	\since build 295
	*/
	PDefHOp(View&, =, View&& v)
		ImplRet(visual = std::move(v.visual), *this)

	bool
	IsVisible() const ynothrow;

	DefGetter(const ynothrow, SDst, Height, GetSize().Height)
	DefGetter(const ynothrow, const Point&, Location, visual.Bounds.GetPoint())
	//! \since build 307
	DefGetter(ynothrow, Point&, LocationRef, visual.Bounds.GetPointRef())
	DefGetterMem(const ynothrow, const Size&, Size, visual.Bounds)
	//! \since build 307
	DefGetterMem(ynothrow, Size&, SizeRef, visual.Bounds)
	DefGetter(const ynothrow, SDst, Width, GetSize().Width)
	DefGetterMem(const ynothrow, SPos, X, GetLocation())
	DefGetterMem(const ynothrow, SPos, Y, GetLocation())

	DefSetter(SDst, Height, GetSizeRef().Height)
	void
	SetVisible(bool);
	DefSetter(SDst, Width, GetSizeRef().Width)
	DefSetterMem(SPos, X, GetLocationRef())
	DefSetterMem(SPos, Y, GetLocationRef())
	virtual DefSetter(const Point&, Location, visual.Bounds.GetPointRef())
	PDefH(void, SetLocation, SPos x, SPos y)
		ImplExpr(SetLocation({x, y}))
	virtual DefSetter(const Size&, Size, visual.Bounds.GetSizeRef())
	PDefH(void, SetSize, SDst w, SDst h)
		ImplExpr(SetSize({w, h}))

	//! \since build 409
	virtual DefClone(const, View)
};

} // namespace UI;

} // namespace YSLib;

#endif

