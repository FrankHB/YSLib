/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file scroll.h
\ingroup UI
\brief 样式相关的图形用户界面滚动控件。
\version r3335;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:10:35 +0800;
\par 修改时间:
	2011-11-27 21:34 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Scroll;
*/


#ifndef YSL_INC_UI_SCROLL_H_
#define YSL_INC_UI_SCROLL_H_

#include "uicontx.h"
#include "button.h"
//#include <ystdex/rational.hpp>
//#include "../Core/yres.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 滚动类别。
typedef enum class
{
	SmallDecrement = 0, //!< 滚动框小距离减量移动。
	SmallIncrement = 1, //!< 滚动框小距离增量移动。
	LargeDecrement = 2, //!< 滚动框大距离减量移动。
	LargeIncrement = 3, //!< 滚动框大距离增量移动。
	ThumbPosition = 4, //!< 滚动框定位（通过直接设置位置）。
	ThumbTrack = 5, //!< 滚动框当前正在移动。
	First = 6, //!< 滚动框移动至最小位置。
	Last = 7, //!< 滚动框移动至最大位置。
	EndScroll = 8 //!< 滚动框移动停止。
} ScrollCategory;


//! \brief 滚动事件参数类。
struct ScrollEventArgs : public UIEventArgs,
	public GMValueEventArgs<float>
{
public:
	typedef GMValueEventArgs<float> MEventArgs;
	typedef MEventArgs::ValueType ValueType;

	ScrollCategory Type; //滚动事件类型。

	/*!
	\brief 构造：使用指定事件源、滚动事件类型和值。
	\note 值等于旧值。
	*/
	ScrollEventArgs(IWidget&, ScrollCategory, ValueType);
	/*!
	\brief 构造：使用指定事件源、滚动事件类型、值和旧值。
	*/
	ScrollEventArgs(IWidget&, ScrollCategory, ValueType, ValueType);
};

inline
ScrollEventArgs::ScrollEventArgs(IWidget& wgt, ScrollCategory t,
	ScrollEventArgs::ValueType v)
	: UIEventArgs(wgt), MEventArgs(v),
	Type(t)
{}
inline
ScrollEventArgs::ScrollEventArgs(IWidget& wgt, ScrollCategory t,
	ScrollEventArgs::ValueType v, ScrollEventArgs::ValueType old_value)
	: UIEventArgs(wgt), MEventArgs(v, old_value),
	Type(t)
{}


DefDelegate(HScrollEvent, ScrollEventArgs)


//! \brief 轨道。
class ATrack : public AUIBoxControl, public GMRange<ScrollEventArgs::ValueType>
{
public:
	//! \brief 轨道区域。
	typedef enum
	{
		None = 0,
		OnThumb = 1,
		OnPrev = 2,
		OnNext = 3
	} Area;
	//注意值类型需要和继承的 GMRange 的 ValueType 一致。
	typedef ScrollEventArgs::ValueType ValueType; //!< 值类型。

protected:
	Components::Thumb Thumb; //!< 滑块：轨道区域上的滚动框。

private:
	SDst min_thumb_length; //!< 最小滑块长度。
	// MRange 实现滚动事件关联值操作。
	ValueType large_delta; \
		//!< 大距离滚动偏移量：滚动事件关联的滑块位置变化绝对值。
	// MRange::value 实际最大取值为 MRange::max_value - large_delta 。
	DeclEvent(HUIEvent, ThumbDrag) //!< 滑块拖动事件。
	DeclEvent(HScrollEvent, Scroll) //!< 滚动事件。

public:
	/*!
	\brief 构造：使用指定边界和大小。
	*/
	explicit
	ATrack(const Rect& = Rect::Empty, SDst = 8);
	inline DefDeMoveCtor(ATrack)

	DefPredicate(Horizontal, GetOrientation() == Horizontal)
	DefPredicate(Vertical, GetOrientation() == Vertical)

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	\note 仅滑块。
	*/
	ImplI(AUIBoxControl) IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));
	DefMutableEventGetter(HUIEvent, ThumbDrag, ThumbDrag) //!< 滑块拖动事件。
	DefMutableEventGetter(HScrollEvent, Scroll, Scroll) //!< 滚动事件。
	DefGetter(SDst, MinThumbLength, min_thumb_length)
	DeclIEntry(Orientation GetOrientation() const) //!< 取轨道方向。
	DefGetter(SDst, ScrollableLength, GetTrackLength() - GetThumbLength()) \
		//取可滚动区域长度。
	DefGetter(SDst, ThumbLength, SelectFrom(GetSizeOf(Thumb),
		IsHorizontal())) //!< 取轨道方向上的滑块长度。
	DefGetter(SDst, ThumbPosition,
		SelectFrom(GetLocationOf(Thumb), IsHorizontal())) //!< 取滑块位置。
	virtual DefGetter(SDst, TrackLength,
		SelectFrom(GetSizeOf(*this), IsHorizontal())) \
		//!< 取轨道方向上的轨道长度。
	DefGetter(ValueType, LargeDelta, large_delta)

	/*!
	\brief 设置轨道方向上的滑块长度。
	*/
	virtual void
	SetThumbLength(SDst);
	/*!
	\brief 设置滑块位置。
	*/
	void
	SetThumbPosition(SPos);
	/*!
	\brief 设置滚动事件关联值最大取值。
	\note 当指定值非正值时无效。
	\note 约束 large_delta 不大于指定值。
	*/
	void
	SetMaxValue(ValueType);
	/*!
	\brief 设置滚动事件关联值。
	\note 同步按滑块长度和最大取值的比例设置滑块位置。
	*/
	void
	SetValue(ValueType);
	/*!
	\brief 设置大距离滚动偏移量。
	\note 同步按滑块长度和最大取值的比例设置滑块位置。
	*/
	void
	SetLargeDelta(ValueType);

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);

protected:
	/*!
	\brief 检查轨道方向指定位置所在的区域。
	*/
	Area
	CheckArea(SPos) const;

public:
	/*!
	\brief 定位滑块。
	\note 指定滚动事件关联值设置滑块位置并触发对应事件。
	\note 当滚动类别为 <tt>ScrollCategory::ThumbPosition</tt> 值参数为设置指定值；
		当滚动类别为 <tt>ScrollCategory::SmallDecrement</tt>
		或 <tt>ScrollCategory::SmallIncrement</tt> 时参数为值变化的绝对值；
		其它情况无效。
	*/
	void
	LocateThumb(ValueType, ScrollCategory = ScrollCategory::ThumbPosition);
};


//! \brief 水平轨道。
class HorizontalTrack : public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和最小滑块长。
	\pre 断言：宽大于长的 2 倍。
	*/
	explicit
	HorizontalTrack(const Rect& = Rect::Empty, SDst = 8);
	inline DefDeMoveCtor(HorizontalTrack)

	ImplI(ATrack)
	DefGetter(Orientation, Orientation, Horizontal)
};


//! \brief 竖直轨道。
class VerticalTrack : public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和最小滑块长。
	\pre 断言：长大于宽的 2 倍。
	*/
	explicit
	VerticalTrack(const Rect& = Rect::Empty, SDst = 8);
	inline DefDeMoveCtor(VerticalTrack)

	ImplI(ATrack)
	DefGetter(Orientation, Orientation, Vertical)
};


//! \brief 滚动条。
class AScrollBar : public AUIBoxControl
{
public:
	typedef ATrack::ValueType ValueType; //!< 值类型。

private:
	unique_ptr<ATrack> pTrack; //轨道。

protected:
	Thumb PrevButton, NextButton; //!< 滚动条按钮。

private:
	ValueType small_delta; \
		//!< 小距离滚动偏移量：滚动事件关联的滑块位置变化绝对值。

public:
	/*!
	\brief 构造：使用指定边界、大小和方向。
	*/
	explicit
	AScrollBar(const Rect& = Rect::Empty, SDst = 8, Orientation = Horizontal);
	inline DefDeMoveCtor(AScrollBar)

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	\note 仅滑块和滚动条按钮。
	*/
	ImplI(AUIBoxControl) IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));
	/*!
	\brief 取轨道引用。
	\note 断言检查： is_not_null(pTrack) 。
	*/
	ATrack&
	GetTrack() const ynothrow;
	DefGetterMember(ValueType, MaxValue, GetTrack())
	DefGetterMember(ValueType, Value, GetTrack())
	DefGetterMember(ValueType, LargeDelta, GetTrack())
	DefGetter(ValueType, SmallDelta, small_delta)

	DefSetterMember(ValueType, MaxValue, GetTrack())
	DefSetterMember(ValueType, Value, GetTrack())
	DefSetterMember(ValueType, LargeDelta, GetTrack())
	DefSetter(ValueType, SmallDelta, small_delta)

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);
};

inline ATrack&
AScrollBar::GetTrack() const ynothrow
{
	YAssert(is_not_null(pTrack),
		"Null widget pointer found @ AScrollBar::GetTrack;");

	return *pTrack;
}


//! \brief 水平滚动条。
class HorizontalScrollBar : public AScrollBar
{
public:
	explicit
	HorizontalScrollBar(const Rect& = Rect::Empty, SDst = 8);
	inline DefDeMoveCtor(HorizontalScrollBar)

public:
	ImplI(ATrack)
	DefGetter(Orientation, Orientation, Horizontal)

protected:
	virtual IWidget*
	GetBoundControlPtr(const KeyCode&);
};


//! \brief 竖直滚动条。
class VerticalScrollBar : public AScrollBar
{
public:
	explicit
	VerticalScrollBar(const Rect& = Rect::Empty, SDst = 8);
	inline DefDeMoveCtor(VerticalScrollBar)

public:
	ImplI(ATrack)
	DefGetter(Orientation, Orientation, Vertical)

protected:
	virtual IWidget*
	GetBoundControlPtr(const KeyCode&);
};


//! \brief 带滚动条的容器基实现类。
class ScrollableContainer : public AUIBoxControl
{
protected:
	Components::HorizontalScrollBar HorizontalScrollBar;
	Components::VerticalScrollBar VerticalScrollBar;

public:
	explicit
	ScrollableContainer(const Rect& = Rect::Empty);
	inline DefDeMoveCtor(ScrollableContainer)

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	*/
	ImplI(AUIBoxControl) IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);

protected:
	/*!
	\brief 固定布局。
	\return 调整后的视图大小。
	*/
	Size
	FixLayout(const Size&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

