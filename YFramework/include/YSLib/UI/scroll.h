/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file scroll.h
\ingroup UI
\brief 样式相关的图形用户界面滚动控件。
\version r2510
\author FrankHB<frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2011-03-07 20:10:35 +0800
\par 修改时间:
	2012-09-07 10:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Scroll
*/


#ifndef YSL_INC_UI_SCROLL_H_
#define YSL_INC_UI_SCROLL_H_ 1

#include "button.h"
#include "yuicont.h"
//#include <ystdex/rational.hpp>
//#include "../Core/yres.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 滚动类别。
\since build 261
*/
enum class ScrollCategory
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
};


/*!
\brief 滚动事件参数类。
\since build 193
*/
struct ScrollEventArgs : public UIEventArgs, protected pair<float, float>
{
public:
	typedef float ValueType; //!< 值类型。

	/*!
	\brief 滚动事件类别。
	\since build 271
	*/
	ScrollCategory Category;

	/*!
	\brief 构造：使用指定事件源、滚动事件类型和值。
	\note 值等于旧值。
	*/
	ScrollEventArgs(IWidget& wgt, ScrollCategory t, ValueType val)
		: UIEventArgs(wgt), pair<float, float>(val, float()),
		Category(t)
	{}
	/*!
	\brief 构造：使用指定事件源、滚动事件类型、值和旧值。
	*/
	ScrollEventArgs(IWidget& wgt, ScrollCategory t, ValueType val,
		ValueType old_val)
		: UIEventArgs(wgt), pair<float, float>(val, old_val),
		Category(t)
	{}

	/*!
	\brief 取值。
	\since build 271
	*/
	DefGetter(const ynothrow, ValueType, Value, first)
	/*!
	\brief 取旧值。
	\since build 271
	*/
	DefGetter(const ynothrow, ValueType, OldValue, second)

	/*!
	\brief 设置值。
	\since build 271
	*/
	DefSetter(ValueType, Value, first)
	/*!
	\brief 设置旧值。
	\since build 271
	*/
	DefSetter(ValueType, OldValue, first)
};

DeclDelegate(HScrollEvent, ScrollEventArgs)


/*!
\brief 轨道。
\since build 167
*/
class ATrack : public Control, public GMRange<ScrollEventArgs::ValueType>
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
	\since build 337
	*/
	explicit
	ATrack(const Rect& = {}, SDst = 8);
	inline DefDeMoveCtor(ATrack)

	DefPred(const ynothrow, Horizontal, GetOrientation() == Horizontal)
	DefPred(const ynothrow, Vertical, GetOrientation() == Vertical)

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	\note 仅滑块。
	*/
	IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));
	DefEventGetter(ynothrow, HUIEvent, ThumbDrag, ThumbDrag) //!< 滑块拖动事件。
	DefEventGetter(ynothrow, HScrollEvent, Scroll, Scroll) //!< 滚动事件。
	DefGetter(const ynothrow, SDst, MinThumbLength, min_thumb_length)
	DeclIEntry(Orientation GetOrientation() const) //!< 取轨道方向。
	DefGetter(const ynothrow, SDst, ScrollableLength,
		GetTrackLength() - GetThumbLength()) //!< 取可滚动区域长度。
	DefGetter(const ynothrow, SDst, ThumbLength,
		GetSizeOf(Thumb).GetRef(IsHorizontal())) //!< 取轨道方向上的滑块长度。
	DefGetter(const ynothrow, SDst, ThumbPosition,
		GetLocationOf(Thumb).GetRef(IsHorizontal())) //!< 取滑块位置。
	virtual DefGetter(const ynothrow, SDst, TrackLength,
		GetSizeOf(*this).GetRef(IsHorizontal())) //!< 取轨道方向上的轨道长度。
	DefGetter(const ynothrow, ValueType, LargeDelta, large_delta)

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
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;

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


/*!
\brief 水平轨道。
\since build 205
*/
class HorizontalTrack : public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和最小滑块长。
	\pre 断言：宽大于长。
	\since build 337
	*/
	explicit
	HorizontalTrack(const Rect& = {}, SDst = 8);
	inline DefDeMoveCtor(HorizontalTrack)

	ImplI(ATrack)
	DefGetter(const ynothrow, Orientation, Orientation, Horizontal)
};


/*!
\brief 竖直轨道。
\since build 205
*/
class VerticalTrack : public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和最小滑块长。
	\pre 断言：长大于宽。
	\since build 337
	*/
	explicit
	VerticalTrack(const Rect& = {}, SDst = 8);
	inline DefDeMoveCtor(VerticalTrack)

	ImplI(ATrack)
	DefGetter(const ynothrow, Orientation, Orientation, Vertical)
};


/*!
\brief 滚动条。
\since build 162
*/
class AScrollBar : public Control
{
public:
	typedef ATrack::ValueType ValueType; //!< 值类型。

private:
	unique_ptr<ATrack> pTrack; //轨道。

protected:
	/*!
	\brief 前滚动条按钮。
	\note 可响应按键持续。
	\since build 276
	*/
	Thumb btnPrev;
	/*!
	\brief 后滚动条按钮。
	\note 可响应按键持续。
	\since build 276
	*/
	Thumb btnNext;

private:
	ValueType small_delta; \
		//!< 小距离滚动偏移量：滚动事件关联的滑块位置变化绝对值。

public:
	/*!
	\brief 构造：使用指定边界、大小和方向。
	\since build 337
	*/
	explicit
	AScrollBar(const Rect& = {}, SDst = 8, Orientation = Horizontal);
	inline DefDeMoveCtor(AScrollBar)

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	\note 仅滑块和滚动条按钮。
	*/
	IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));
	/*!
	\brief 取轨道引用。
	\note 断言检查： bool(pTrack) 。
	*/
	ATrack&
	GetTrack() const ynothrow;
	DefGetterMem(const ynothrow, ValueType, MaxValue, GetTrack())
	DefGetterMem(const ynothrow, ValueType, Value, GetTrack())
	DefGetterMem(const ynothrow, ValueType, LargeDelta, GetTrack())
	DefGetter(const ynothrow, ValueType, SmallDelta, small_delta)

	DefSetterMem(ValueType, MaxValue, GetTrack())
	DefSetterMem(ValueType, Value, GetTrack())
	DefSetterMem(ValueType, LargeDelta, GetTrack())
	DefSetter(ValueType, SmallDelta, small_delta)

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;

	/*!
	\brief 定位滑块。
	\note 调用 GetTrack() 的同名函数指定滚动事件关联值设置滑块位置并触发对应事件。
	*/
	void
	LocateThumb(ValueType, ScrollCategory = ScrollCategory::ThumbPosition);
};

inline ATrack&
AScrollBar::GetTrack() const ynothrow
{
	YAssert(bool(pTrack), "Null pointer found.");

	return *pTrack;
}

inline void
AScrollBar::LocateThumb(ValueType val, ScrollCategory t)
{
	return GetTrack().LocateThumb(val, t);
}


/*!
\brief 水平滚动条。
\since build 205
*/
class HorizontalScrollBar : public AScrollBar
{
public:
	//! \since build 337
	explicit
	HorizontalScrollBar(const Rect& = {}, SDst = 8);
	inline DefDeMoveCtor(HorizontalScrollBar)

public:
	ImplI(ATrack) DefGetter(const ynothrow, Orientation, Orientation,
		Horizontal)

protected:
	IWidget*
	GetBoundControlPtr(const KeyInput&) override;
};


/*!
\brief 竖直滚动条。
\since build 205
*/
class VerticalScrollBar : public AScrollBar
{
public:
	//! \since build 337
	explicit
	VerticalScrollBar(const Rect& = {}, SDst = 8);
	inline DefDeMoveCtor(VerticalScrollBar)

public:
	ImplI(ATrack) DefGetter(const ynothrow, Orientation, Orientation, Vertical)

protected:
	IWidget*
	GetBoundControlPtr(const KeyInput&) override;
};


/*!
\brief 带滚动条的容器。
\since build 192
*/
class ScrollableContainer : public Control
{
protected:
	/*!
	\brief 控制水平可视区域的水平滚动条。
	\since build 315
	*/
	HorizontalScrollBar hsbHorizontal;
	/*!
	\brief 控制竖直可视区域的竖直滚动条。
	\since build 315
	*/
	VerticalScrollBar vsbVertical;

public:
	//! \since build 337
	explicit
	ScrollableContainer(const Rect& = {});
	inline DefDeMoveCtor(ScrollableContainer)

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	*/
	IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;

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

