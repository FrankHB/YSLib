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
\version 0.4752;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:24 + 08:00;
\par 修改时间:
	2011-02-21 10:00 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YControl;
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
	static const ScreenPositionEventArgs Empty;

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
	static const InputEventArgs Empty;

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

	static const KeyEventArgs Empty;

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

	static const TouchEventArgs Empty;

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

	IVisualControl& Control;
	IndexType Index;

	/*!
	\brief 构造：使用可视控件引用和索引值。
	*/
	IndexEventArgs(IVisualControl& c, IndexType i)
		: EventArgs(),
		Control(c), Index(i)
	{}

	DefConverter(IndexType, Index)
};


//事件处理器类型。
DefDelegate(HVisualEvent, IVisualControl, EventArgs)
DefDelegate(HInputEvent, IVisualControl, InputEventArgs)
DefDelegate(HKeyEvent, IVisualControl, KeyEventArgs)
DefDelegate(HTouchEvent, IVisualControl, TouchEventArgs)
DefDelegate(HIndexEvent, IVisualControl, IndexEventArgs)
//DefDelegate(HPointEvent, IVisualControl, Drawing::Point)
//DefDelegate(HSizeEvent, IVisualControl, Size)


#define DefEventTypeMapping(_name, _tEventHandler) \
	template<> \
	struct EventTypeMapping<_name> \
	{ \
		typedef _tEventHandler HandlerType; \
	};


//! \brief 标准可视控件事件空间。
typedef enum VisualEventSpace
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
DeclInterface(IControl)
	DeclIEntry(bool IsEnabled() const) //!< 判断是否有效。

	DeclIEntry(Runtime::GEventMap<VisualEvent>& GetEventMap() const) \
		//!< 取事件映射表。

	DeclIEntry(void SetEnabled(bool)) //!< 设置有效性。
EndDecl


//! \brief 可视控件接口。
DeclBasedInterface3(IVisualControl, virtual IWidget, virtual IControl,
	virtual GIFocusRequester<GMFocusResponser>)

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
\brief 调用可视控件自身事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent id>
inline void
CallEvent(IVisualControl& c, typename EventTypeMapping<id>
	::HandlerType::EventArgsType& e)
{
	c.GetEventMap().DoEvent<typename EventTypeMapping<id>
		::HandlerType>(id, c, e);
}


/*!
\brief 处理键接触保持事件。
*/
void
OnKeyHeld(IVisualControl&, KeyEventArgs&);

/*!
\brief 处理屏幕接触保持事件。
*/
void
OnTouchHeld(IVisualControl&, TouchEventArgs&);

/*!
\brief 处理屏幕接触移动事件。
*/
void
OnTouchMove(IVisualControl&, TouchEventArgs&);

/*!
\brief 处理屏幕接触移动事件：使用拖放。
*/
void
OnDrag(IVisualControl&, TouchEventArgs&);


//! \brief 控件基实现类。
class Control
{
private:
	bool enabled; //!< 控件有效性。

protected:
	mutable Runtime::GEventMap<VisualEvent> EventMap; //!< 事件映射表。

public:
	/*!
	\brief 构造：使用有效性。
	*/
	explicit
	Control(bool = true);

	virtual DefEmptyDtor(Control)

	virtual DefPredicate(Enabled, enabled)

	DefGetter(Runtime::GEventMap<VisualEvent>&, EventMap, EventMap)

	virtual DefSetter(bool, Enabled, enabled)
};

inline
Control::Control(bool e)
	: enabled(e), EventMap()
{}


//! \brief 控件基类。
class YControl : public YComponent,
	public Control,
	implements IControl
{
public:
	typedef YComponent ParentType;

	virtual DefEmptyDtor(YControl)
};


//! \brief 可视控件基实现类。
class VisualControl : public Widgets::Widget, public Control,
	public AFocusRequester,
	virtual implements IVisualControl
{
public:
	//标准可视控件事件见 VisualEvent 。

	//扩展可视控件事件。
//	DeclEvent(HPointEvent, Move) //!< 可视控件移动。
//	DeclEvent(HSizeEvent, Resize) //!< 可视控件大小调整。

	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	VisualControl(const Rect& = Rect::Empty, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual ~VisualControl() ythrow();

	ImplI1(IVisualControl) DefPredicateBase(Visible, Visual)
	ImplI1(IVisualControl) DefPredicateBase(Transparent, Visual)
	ImplI1(IVisualControl) DefPredicateBase(BgRedrawed, Visual)
	ImplI1(IVisualControl) DefPredicateBase(Enabled, Control)
	ImplI1(AFocusRequester) bool
	IsFocused() const;

	ImplI1(IVisualControl) DefGetterBase(const Point&, Location, Visual)
	ImplI1(IVisualControl) DefGetterBase(const Size&, Size, Visual)
	ImplI1(IVisualControl) DefGetterBase(IUIBox*, ContainerPtr, Widget)
	ImplI1(IVisualControl) DefGetterBase(Runtime::GEventMap<VisualEvent>&,
		EventMap, Control)

	ImplI1(IVisualControl) DefSetterBase(bool, Visible, Visual)
	ImplI1(IVisualControl) DefSetterBase(bool, Transparent, Visual)
	ImplI1(IVisualControl) DefSetterBase(bool, BgRedrawed, Visual)
	ImplI1(IVisualControl) void
	SetLocation(const Point&);
	ImplI1(IVisualControl) void
	SetSize(const Size&);
	ImplI1(IVisualControl) DefSetterBase(bool, Enabled, Control)

	ImplI1(IVisualControl) PDefH0(void, DrawBackground)
		ImplBodyBase0(Widget, DrawBackground)

	ImplI1(IVisualControl) PDefH0(void, DrawForeground)
		ImplBodyBase0(Widget, DrawForeground)

	ImplI1(IVisualControl) PDefH0(void, Refresh)
		ImplBodyBase0(Widget, Refresh)

	/*!
	\brief 向部件容器请求获得焦点。
	\note 若成功则触发 GotFocus 事件。
	*/
	ImplI1(IVisualControl) void
	RequestFocus(EventArgs&);

	/*!
	\brief 释放焦点。
	\note 触发 LostFocus 事件。
	*/
	ImplI1(IVisualControl) void
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


//! \brief 可视控件基类。
class YVisualControl : public YComponent,
	public VisualControl
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	YVisualControl(const Rect& = Rect::Empty, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual DefEmptyDtor(YVisualControl)
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

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

