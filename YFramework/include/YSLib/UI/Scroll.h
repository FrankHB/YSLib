/*
	© 2011-2016, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Scroll.h
\ingroup UI
\brief 样式相关的图形用户界面滚动控件。
\version r2798
\author FrankHB <frankhb1989@gmail.com>
\since build 586
\par 创建时间:
	2011-03-07 20:10:35 +0800
\par 修改时间:
	2020-01-25 16:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Scroll
*/


#ifndef YSL_INC_UI_Scroll_h_
#define YSL_INC_UI_Scroll_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_Button
#include YFM_YSLib_UI_YUIContainer
#include YFM_YSLib_UI_WidgetIteration
//#include <ystdex/rational.hpp>
//#include YFM_YSLib_Service_YResource
//#include YFM_YSLib_UI_YStyle

namespace YSLib
{

namespace UI
{

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
struct YF_API ScrollEventArgs : public UIEventArgs, protected pair<float, float>
{
public:
	using ValueType = float; //!< 值类型。

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
	//! \since build 586
	DefDeCopyCtor(ScrollEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~ScrollEventArgs() override;

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
	DefSetter(, ValueType, Value, first)
	/*!
	\brief 设置旧值。
	\since build 271
	*/
	DefSetter(, ValueType, OldValue, first)
};


/*!
\brief 轨道。
\since build 167
*/
class YF_API ATrack : public Control, public GMRange<ScrollEventArgs::ValueType>
{
public:
	/*!
	\brief 轨道区域。
	\since build 416
	*/
	enum Area
	{
		None = 0,
		OnThumb = 1,
		OnPrev = 2,
		OnNext = 3
	};
	//注意值类型需要和继承的 GMRange 的 ValueType 一致。
	using ValueType = ScrollEventArgs::ValueType; //!< 值类型。
	//! \since build 460
	using iterator = ystdex::subscriptive_iterator<ATrack, IWidget>;

protected:
	/*!
	\brief 滑块：轨道区域上的滚动框。
	\since build 374
	*/
	Thumb tmbScroll;

private:
	SDst min_thumb_length; //!< 最小滑块长度。
	// MRange 实现滚动事件关联值操作。
	ValueType large_delta; \
		//!< 大距离滚动偏移量：滚动事件关联的滑块位置变化绝对值。
	// MRange::value 实际最大取值为 MRange::max_value - large_delta 。

public:
	//! \brief 滑块拖动事件。
	GEvent<void(UIEventArgs&&)> ThumbDrag;
	//! \brief 滚动事件。
	GEvent<void(ScrollEventArgs&&)> Scroll;

	/*!
	\brief 构造：使用指定边界和大小。
	\since build 337
	*/
	explicit
	ATrack(const Rect& = {}, SDst = 8);

	DefPred(const ynothrow, Horizontal, GetOrientation() == Horizontal)
	DefPred(const ynothrow, Vertical, GetOrientation() == Vertical)

	//! \since build 356
	DefWidgetMemberIteration(tmbScroll)

	DefGetter(const ynothrow, SDst, MinThumbLength, min_thumb_length)
	/*!
	\brief 取轨道方向。
	\since build 527
	*/
	DeclIEntry(Orientation GetOrientation() const ynothrow)
	DefGetter(const ynothrow, SDst, ScrollableLength,
		GetTrackLength() - GetThumbLength()) //!< 取可滚动区域长度。
	DefGetter(const ynothrow, SDst, ThumbLength,
		GetSizeOf(tmbScroll).GetRef(IsHorizontal())) //!< 取轨道方向上的滑块长度。
	/*!
	\brief 取滑块位置。
	\since build 587
	*/
	DefGetter(const ynothrow, SPos, ThumbPosition,
		GetLocationOf(tmbScroll).GetRef(IsHorizontal()))
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
\brief 绘制指定色调的基本按钮背景。
\since build 359
*/
YF_API void
DrawTrackBackground(PaintEventArgs&& e, ATrack&);


/*!
\brief 水平轨道。
\since build 205
*/
class YF_API HorizontalTrack : public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和最小滑块长。
	\pre 断言：宽大于长。
	\since build 337
	*/
	explicit
	HorizontalTrack(const Rect& = {}, SDst = 8);
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~HorizontalTrack() override;

	DefGetter(const ynothrow ImplI(ATrack), Orientation, Orientation,
		Horizontal)
};


/*!
\brief 竖直轨道。
\since build 205
*/
class YF_API VerticalTrack : public ATrack
{
public:
	/*!
	\brief 构造：使用指定边界和最小滑块长。
	\pre 断言：长大于宽。
	\since build 337
	*/
	explicit
	VerticalTrack(const Rect& = {}, SDst = 8);
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~VerticalTrack() override;

	DefGetter(const ynothrow ImplI(ATrack), Orientation, Orientation, Vertical)
};


/*!
\brief 滚动条。
\since build 527
*/
class YF_API ScrollBar : public Control
{
public:
	using ValueType = ATrack::ValueType; //!< 值类型。
	//! \since build 460
	using iterator = ystdex::subscriptive_iterator<ScrollBar, IWidget>;

	/*!
	\brief 默认前景色。
	\since build 525
	*/
	Color ForeColor{Drawing::ColorSpace::Black};

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
	\since build 527
	*/
	explicit
	ScrollBar(const Rect& = {}, Orientation = Horizontal, SDst = 8);

protected:
	//! \since build 443
	void
	InitializeArrowPainters(Rotation, Rotation);

public:
	//! \since build 356
	DefWidgetMemberIteration(btnPrev, btnNext, *pTrack)

protected:
	//! \since build 671
	observer_ptr<IWidget>
	GetBoundControlPtr(const KeyInput&) override;

public:
	//! \since build 527
	DefGetterMem(const ynothrow, Orientation, Orientation, GetTrackRef())
	/*!
	\brief 取轨道引用。
	\pre 断言： bool(pTrack) 。
	*/
	DefGetter(const ynothrow, ATrack&, TrackRef, Deref(pTrack))
	DefGetterMem(const ynothrow, ValueType, MaxValue, GetTrackRef())
	DefGetterMem(const ynothrow, ValueType, Value, GetTrackRef())
	DefGetterMem(const ynothrow, ValueType, LargeDelta, GetTrackRef())
	DefGetter(const ynothrow, ValueType, SmallDelta, small_delta)

	DefSetterMem(, ValueType, MaxValue, GetTrackRef())
	DefSetterMem(, ValueType, Value, GetTrackRef())
	DefSetterMem(, ValueType, LargeDelta, GetTrackRef())
	DefSetter(, ValueType, SmallDelta, small_delta)

	/*!
	\brief 定位滑块。
	\note 调用 GetTrackRef() 的同名函数指定滚动事件关联值设置滑块位置，
		并触发对应事件。
	*/
	PDefH(void, LocateThumb, ValueType val,
		ScrollCategory t = ScrollCategory::ThumbPosition)
		ImplRet(GetTrackRef().LocateThumb(val, t))
};


/*!
\brief 带滚动条的容器。
\since build 192
*/
class YF_API ScrollableContainer : public Control
{
public:
	//! \since build 460
	using iterator
		= ystdex::subscriptive_iterator<ScrollableContainer, IWidget>;

protected:
	/*!
	\brief 控制水平和竖直可视区域的水平滚动条。
	\since build 527
	*/
	ScrollBar hsbHorizontal, vsbVertical;

public:
	//! \since build 337
	explicit
	ScrollableContainer(const Rect& = {});
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~ScrollableContainer() override;

	//! \since build 356
	DefWidgetMemberIteration(hsbHorizontal, vsbVertical)

protected:
	/*!
	\brief 固定布局。
	\return 调整后的视图大小。
	*/
	Size
	FixLayout(const Size&);
};

} // namespace UI;

} // namespace YSLib;

#endif

