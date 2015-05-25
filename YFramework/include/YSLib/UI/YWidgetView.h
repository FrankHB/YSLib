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
\version r821
\author FrankHB <frankhb1989@gmail.com>
\since build 568
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2015-05-24 23:02 +0800
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
\brief 视图操作消息信号。
\since build 570
*/
class YF_API ViewSignal : public Messaging::MessageSignal
{
public:
	using MessageSignal::MessageSignal;

	//! \since build 586
	DefDeCopyCtor(ViewSignal)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~ViewSignal() override;
};


//! \brief 抽象部件视图。
//@{
class YF_API AView : public ystdex::cloneable
{
public:
	//! \since build 375
	//@{
	mutable IWidget* ContainerPtr{}; //!< 从属的部件容器的指针。
	/*!
	\brief 从属的部件指针。

	逻辑依赖的部件指针，用于提供边界、容器和焦点以外属性的默认值。
	*/
	mutable IWidget* DependencyPtr{};
	mutable IWidget* FocusingPtr{}; //!< 焦点指针。
	//@}

	DefDeCtor(AView)
	AView(const AView&)
		: AView()
	{}
	AView(AView&& v)
		: ContainerPtr(v.ContainerPtr), DependencyPtr(v.DependencyPtr),
		FocusingPtr(v.FocusingPtr)
	{
		yunseq(v.ContainerPtr = {}, v.DependencyPtr = {}, v.FocusingPtr = {});
	}
	//! \brief 虚析构：类定义外默认实现。
	~AView() override;

	DefDeCopyMoveAssignment(AView)

	DeclIEntry(bool IsVisible() const)

	virtual DefGetter(const, Rect, Bounds, {GetLocation(), GetSize()})
	DefGetter(const ynothrow, SDst, Height, GetSize().Height)
	DeclIEntry(Point GetLocation() const)
	DefGetter(const ynothrow, SDst, Width, GetSize().Width)
	DeclIEntry(Size GetSize() const)
	DefGetterMem(const ynothrow, SPos, X, GetLocation())
	DefGetterMem(const ynothrow, SPos, Y, GetLocation())

	virtual PDefH(void, SetBounds, const Rect& r)
		ImplExpr(SetLocation(r.GetPoint()), SetSize(r.GetSize()))
	virtual PDefH(void, SetHeight, SDst h)
		ImplExpr(SetSize(GetSize().Width, h))
	DeclIEntry(void SetLocation(const Point&))
	PDefH(void, SetLocation, SPos x, SPos y)
		ImplExpr(SetLocation({x, y}))
	DeclIEntry(void SetSize(const Size&))
	PDefH(void, SetSize, SDst w, SDst h)
		ImplExpr(SetSize({w, h}))
	virtual PDefH(void, SetWidth, SDst w)
		ImplExpr(SetSize(w, GetSize().Height))
	virtual PDefH(void, SetX, SPos x)
		ImplExpr(SetLocation(x, GetLocation().Y))
	virtual PDefH(void, SetY, SPos y)
		ImplExpr(SetLocation(GetLocation().X, y))

	DeclIEntry(void SetVisible(bool))

	/*!
	\brief 对外源子部件进行命中测试。
	\return 指定位置在非 YSLib 部件边界内。
	\since build 578
	*/
	virtual PDefH(bool, HitChildren, const Point&) const
		ImplRet({})

	DeclIEntry(AView* clone() const ImplI(ystdex::cloneable))

	/*!
	\brief 交换：交换所有数据成员。
	\since build 571
	*/
	void
	swap(AView&) ynothrow;
};

//! \relates AView
//@{
//! \brief 交换指定视图的位置。
YF_API void
SwapLocationOf(AView&, Point&);

//! \brief 交换指定视图的大小。
YF_API void
SwapSizeOf(AView&, Size&);
//@}
//@}


/*!
\brief 部件视图。
\since build 259
*/
class YF_API View : public AView
{
private:
	Visual visual; //!< 当前可视状态。

public:
	/*!
	\brief 构造：使用指定边界、前景色和背景色。
	\since build 337
	*/
	View(const Rect& r = {})
		: AView(),
		visual(r)
	{}
	DefDeCopyMoveCtor(View)

	DefDeCopyMoveAssignment(View)

	bool
	IsVisible() const ynothrow ImplI(AView);

	//! \since build 569
	DefGetter(const ynothrow override, Rect, Bounds, visual.Bounds)
	//! \since build 569
	DefGetter(const ynothrow ImplI(AView), Point, Location,
		View::GetBounds().GetPoint())
	//! \since build 307
	DefGetter(ynothrow, Point&, LocationRef, GetBounds().GetPointRef())
	//! \since build 569
	DefGetterMem(const ynothrow ImplI(AView), Size, Size, View::GetBounds())
	//! \since build 307
	DefGetterMem(ynothrow, Size&, SizeRef, GetBounds())

	DefSetter(override, SDst, Height, GetSizeRef().Height)
	void
	SetVisible(bool) ImplI(AView);
	DefSetter(override, SDst, Width, GetSizeRef().Width)
	DefSetterMem(override, SPos, X, GetLocationRef())
	DefSetterMem(override, SPos, Y, GetLocationRef())
	//! \since build 569
	DefSetter(override, const Rect&, Bounds, visual.Bounds)
	DefSetter(ImplI(AView), const Point&, Location, visual.Bounds.GetPointRef())
	DefSetter(ImplI(AView), const Size&, Size, visual.Bounds.GetSizeRef())

	//! \since build 409
	DefClone(const ImplI(AView), View)
};

} // namespace UI;

} // namespace YSLib;

#endif

