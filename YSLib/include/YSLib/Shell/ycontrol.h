/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.h
\ingroup Shell
\brief 平台无关的控件实现。
\version 0.4839;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:24 +0800;
\par 修改时间:
	2011-03-08 09:16 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YComponent;
*/


#ifndef INCLUDED_YCONTROL_H_
#define INCLUDED_YCONTROL_H_

#include "../Core/ysdef.h"
#include "../Core/yobject.h"
#include "../Core/yevt.hpp"
#include "ywidget.h"
#include "yfocus.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

using namespace Drawing;

//! \brief 屏幕事件参数类。
struct ScreenPositionEventArgs : public EventArgs, public Drawing::Point
{
public:
	/*!
	\brief 构造：使用指定点。
	*/
	ScreenPositionEventArgs(const Drawing::Point& = Drawing::Point::Zero);
};

inline
ScreenPositionEventArgs::ScreenPositionEventArgs(const Drawing::Point& pt)
	: EventArgs(), Point(pt)
{}


//! \brief 输入事件参数类。
struct InputEventArgs
{
public:
	typedef Runtime::Key Key;

	Key key;

	/*!
	\brief 构造：使用本机键按下对象。
	*/
	InputEventArgs(const Key& = 0);

	DefConverter(Key, key)

	DefGetter(Key, Key, key)
};

inline
InputEventArgs::InputEventArgs(const Key& k)
	: key(k)
{}


//! \brief 键盘输入事件参数类。
struct KeyEventArgs : public EventArgs, public InputEventArgs
{
	typedef Key InputType; //!< 输入类型。

	/*!
	\brief 构造：使用输入类型对象。
	*/
	KeyEventArgs(const InputType& = 0);
};

inline
KeyEventArgs::KeyEventArgs(const InputType& k)
	: InputEventArgs(k)
{}


//! \brief 指针设备输入事件参数类。
struct TouchEventArgs : public ScreenPositionEventArgs, public InputEventArgs
{
	typedef Drawing::Point InputType; //!< 输入类型。

	/*!
	\brief 构造：使用输入类型对象。
	*/
	TouchEventArgs(const InputType& = InputType::Zero);
};

inline
TouchEventArgs::TouchEventArgs(const InputType& pt)
	: ScreenPositionEventArgs(pt), InputEventArgs()
{}


//! \brief 控件事件参数类。
struct IndexEventArgs : public EventArgs
{
	typedef std::ptrdiff_t IndexType;

	IControl& Control;
	IndexType Index;

	/*!
	\brief 构造：使用控件引用和索引值。
	*/
	IndexEventArgs(IControl& c, IndexType i)
		: EventArgs(),
		Control(c), Index(i)
	{}

	DefConverter(IndexType, Index)
};


//! \brief 值类型参数类模块模板。
template<typename _type>
struct GMValueEventArgs
{
public:
	typedef _type ValueType; //值类型。
	_type Value, OldValue; //值和旧值。

	/*!
	\brief 构造：使用指定值。
	\note 值等于旧值。
	*/
	GMValueEventArgs(ValueType v)
		: Value(v), OldValue(v)
	{}
	/*!
	\brief 构造：使用指定值和旧值。
	*/
	GMValueEventArgs(ValueType v, ValueType old_value)
		: Value(v), OldValue(old_value)
	{}
};


//事件处理器类型。
DefDelegate(HVisualEvent, IControl, EventArgs)
DefDelegate(HInputEvent, IControl, InputEventArgs)
DefDelegate(HKeyEvent, IControl, KeyEventArgs)
DefDelegate(HTouchEvent, IControl, TouchEventArgs)
DefDelegate(HIndexEvent, IControl, IndexEventArgs)
//DefDelegate(HPointEvent, IControl, Drawing::Point)
//DefDelegate(HSizeEvent, IControl, Size)


#define DefEventTypeMapping(_name, _tEventHandler) \
	template<> \
	struct EventTypeMapping<_name> \
	{ \
		typedef _tEventHandler HandlerType; \
	};


//! \brief 标准控件事件空间。
typedef enum
{
//	AutoSizeChanged,
//	BackColorChanged,
//	ForeColorChanged,
//	LocationChanged,
//	MarginChanged,
//	VisibleChanged,

//	EnabledChanged,

	//视图变更事件。
	Move, //!< 移动：位置调整。
	Resize, //!< 大小调整。

	//基本输入事件。
	KeyUp, //!< 键接触结束。
	KeyDown, //!< 键接触开始。
	KeyHeld, //!< 键接触保持。
	KeyPress, //!< 键按下。
	TouchUp, //!< 屏幕接触结束。
	TouchDown, //!< 屏幕接触开始。
	TouchHeld, //!< 屏幕接触保持。
	TouchMove, //!< 屏幕接触移动。
	Click, //!< 屏幕点击。

	//焦点事件。
	GotFocus, //!< 焦点获得。
	LostFocus, //!< 焦点失去。

	//边界事件。
	Enter, //!< 控件进入。
	Leave //!< 控件离开。

//	TextChanged,
//	FontChanged,
//	FontColorChanged,
//	FontSizeChanged,
} VisualEvent;


template<VisualEvent>
struct EventTypeMapping
{
	//定义 HandlerType 的默认值可能会导致运行期 dynamic_cast 失败。
//	typedef HEvent HandlerType;
};

DefEventTypeMapping(Move, HVisualEvent)
DefEventTypeMapping(Resize, HVisualEvent)

DefEventTypeMapping(KeyUp, HKeyEvent)
DefEventTypeMapping(KeyDown, HKeyEvent)
DefEventTypeMapping(KeyHeld, HKeyEvent)
DefEventTypeMapping(KeyPress, HKeyEvent)
DefEventTypeMapping(TouchUp, HTouchEvent)
DefEventTypeMapping(TouchDown, HTouchEvent)
DefEventTypeMapping(TouchHeld, HTouchEvent)
DefEventTypeMapping(TouchMove, HTouchEvent)
DefEventTypeMapping(Click, HTouchEvent)

DefEventTypeMapping(GotFocus, HVisualEvent)
DefEventTypeMapping(LostFocus, HVisualEvent)

DefEventTypeMapping(Enter, HInputEvent)
DefEventTypeMapping(Leave, HInputEvent)


//! \brief 控件接口。
DeclBasedInterface1(IControl, virtual IWidget)
	DeclIEntry(bool IsEnabled() const) //!< 判断是否有效。
	DeclIEntry(bool IsFocused() const) //!< 判断是否取得焦点。

	DeclIEntry(Runtime::GEventMap<VisualEvent>& GetEventMap() const) \
		//!< 取事件映射表。

	DeclIEntry(void SetEnabled(bool)) //!< 设置有效性。

	//! \brief 向部件容器请求获得焦点。
	DeclIEntry(void RequestFocus(EventArgs&))

	//! \brief 释放焦点。
	DeclIEntry(void ReleaseFocus(EventArgs&))
EndDecl


/*!
\ingroup HelperFunction
\brief 取控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
*/
template<VisualEvent id>
inline typename Runtime::GSEvent<typename EventTypeMapping<id>
	::HandlerType>::EventType&
FetchEvent(IControl& c)
{
	return c.GetEventMap().GetEvent<typename EventTypeMapping<id>
		::HandlerType>(id);
}

/*!
\ingroup HelperFunction
\brief 调用控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent id>
inline void
CallEvent(IControl& c, typename EventTypeMapping<id>
	::HandlerType::SenderType& sender, typename EventTypeMapping<id>
	::HandlerType::EventArgsType& e)
{
	c.GetEventMap().DoEvent<typename EventTypeMapping<id>
		::HandlerType>(id, sender, e);
}
/*!
\ingroup HelperFunction
\brief 调用控件自身事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent id>
inline void
CallEvent(IControl& c, typename EventTypeMapping<id>
	::HandlerType::EventArgsType& e)
{
	c.GetEventMap().DoEvent<typename EventTypeMapping<id>
		::HandlerType>(id, c, e);
}


/*!
\brief 处理键接触保持事件。
*/
void
OnKeyHeld(IControl&, KeyEventArgs&);

/*!
\brief 处理屏幕接触保持事件。

 Control 加载的默认 TouchHeld 事件处理器。
实现记录坐标偏移（用于拖放）或触发 TouchMove 事件。
*/
void
OnTouchHeld(IControl&, TouchEventArgs&);

/*!
\brief 处理屏幕接触移动事件。
\note 重复触发 TouchDown 事件。
*/
void
OnTouchMove(IControl&, TouchEventArgs&);

/*!
\brief 处理屏幕接触移动事件。
\note 使用拖放。
*/
void
OnTouchMove_Dragging(IControl&, TouchEventArgs&);


//! \brief 控件基实现类。
class Control : public Widgets::Widget, public AFocusRequester,
	virtual implements IControl
{
private:
	bool enabled; //!< 控件有效性。

protected:
	mutable Runtime::GEventMap<VisualEvent> EventMap; //!< 事件映射表。

public:
	//标准控件事件见 VisualEvent 。

	//扩展控件事件。
//	DeclEvent(HPointEvent, Move) //!< 控件移动。
//	DeclEvent(HSizeEvent, Resize) //!< 控件大小调整。

	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	Control(const Rect& = Rect::Empty, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~Control() ythrow();

	ImplI1(IControl) DefPredicateBase(Visible, Visual)
	ImplI1(IControl) DefPredicateBase(Transparent, Visual)
	ImplI1(IControl) DefPredicateBase(BgRedrawed, Visual)
	ImplI1(IControl) DefPredicate(Enabled, enabled)
	ImplI1(IControl) bool
	IsFocused() const;

	ImplI1(IControl) DefGetterBase(const Point&, Location, Visual)
	ImplI1(IControl) DefGetterBase(const Size&, Size, Visual)
	ImplI1(IControl) DefGetterBase(IUIBox*, ContainerPtr, Widget)
	ImplI1(IControl) DefGetter(Runtime::GEventMap<VisualEvent>&, EventMap,
		EventMap)

	ImplI1(IControl) DefSetterBase(bool, Visible, Visual)
	ImplI1(IControl) DefSetterBase(bool, Transparent, Visual)
	ImplI1(IControl) DefSetterBase(bool, BgRedrawed, Visual)
	ImplI1(IControl) void
	SetLocation(const Point&);
	ImplI1(IControl) void
	SetSize(const Size&);
	ImplI1(IControl) DefSetter(bool, Enabled, enabled)

	ImplI1(IControl) PDefH0(void, DrawBackground)
		ImplBodyBase0(Widget, DrawBackground)

	ImplI1(IControl) PDefH0(void, DrawForeground)
		ImplBodyBase0(Widget, DrawForeground)

	ImplI1(IControl) PDefH0(void, Refresh)
		ImplBodyBase0(Widget, Refresh)

	/*!
	\brief 向部件容器请求获得焦点。
	\note 若成功则触发 GotFocus 事件。
	*/
	ImplI1(IControl) void
	RequestFocus(EventArgs&);

	/*!
	\brief 释放焦点。
	\note 触发 LostFocus 事件。
	*/
	ImplI1(IControl) void
	ReleaseFocus(EventArgs&);

private:
	/*!
	\brief 处理获得焦点事件。
	*/
	void
	OnGotFocus(EventArgs&);

	/*!
	\brief 处理失去焦点事件。
	*/
	void
	OnLostFocus(EventArgs&);

	/*!
	\brief 处理屏幕接触开始事件。
	*/
	void
	OnTouchDown(TouchEventArgs&);
};


//! \brief 控件基类。
class YControl : public YComponent,
	public Control
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	YControl(const Rect& = Rect::Empty, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual DefEmptyDtor(YControl)
};


//! \brief 按钮模块。
class MButton
{
protected:
	bool bPressed; //!< 键按下状态：是否处于按下状态。

	/*!
	\brief 构造：使用键按下状态。
	*/
	explicit
	MButton(bool = false);

public:
	DefPredicate(Pressed, bPressed)
};

inline
MButton::MButton(bool b)
	: bPressed(b)
{}


//! \brief 滚动事件类型空间。
namespace ScrollEventSpace
{
	//! \brief 滚动事件类型。
	typedef enum
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
	} ScrollEventType;
};


//! \brief 滚动事件参数类。
struct ScrollEventArgs : public EventArgs,
	public GMValueEventArgs<SDST>
{
public:
	typedef GMValueEventArgs<SDST> MEventArgs;
	typedef MEventArgs::ValueType ValueType;

	ScrollEventSpace::ScrollEventType Type; //滚动事件类型。

	/*!
	\brief 构造：使用指定滚动事件类型和值。
	\note 值等于旧值。
	*/
	ScrollEventArgs(ScrollEventSpace::ScrollEventType, ValueType);
	/*!
	\brief 构造：使用指定滚动事件类型、值和旧值。
	*/
	ScrollEventArgs(ScrollEventSpace::ScrollEventType, ValueType, ValueType);
};

inline
ScrollEventArgs::ScrollEventArgs(ScrollEventSpace::ScrollEventType t,
	ScrollEventArgs::ValueType v)
	: GMValueEventArgs<SDST>(v), Type(t)
{}
inline
ScrollEventArgs::ScrollEventArgs(ScrollEventSpace::ScrollEventType t,
	ScrollEventArgs::ValueType v, ScrollEventArgs::ValueType old_value)
	: GMValueEventArgs<SDST>(v, old_value), Type(t)
{}


DefDelegate(HScrollEvent, IControl, ScrollEventArgs)


//! \brief 范围模块类。
template<typename _type>
class GMRange
{
public:
	typedef _type ValueType;

protected:
	ValueType max_value; //!< 最大取值。
	ValueType value; //!< 值。

	/*!
	\brief 构造：使用指定最大取值和值。
	*/
	GMRange(ValueType m, ValueType v)
		: max_value(m), value(v)
	{}

public:
	DefGetter(ValueType, MaxValue, max_value)
	DefGetter(ValueType, Value, value)
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

