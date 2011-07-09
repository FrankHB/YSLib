/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.h
\ingroup UI
\brief 样式无关的控件。
\version 0.5153;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:24 +0800;
\par 修改时间:
	2011-07-07 21:21 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YComponent;
*/


#ifndef YSL_INC_UI_YCONTROL_H_
#define YSL_INC_UI_YCONTROL_H_

#include "ywidget.h"
#include "../Core/yevt.hpp"
#include "yfocus.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

using namespace Drawing;


//! \brief 路由事件基类。
struct RoutedEventArgs : public EventArgs
{
public:
	//! 事件路由策略枚举。
	typedef enum
	{
		Bubble = 0, //!< 气泡事件：向上遍历视图树时触发。
		Tunnel = 1, //!< 隧道事件：向下遍历视图树时触发。
		Direct = 2 //!< 直接事件：仅当遍历至目标控件时触发。
	} RoutingStrategy;

	RoutingStrategy Strategy; //!< 事件路由策略。
	bool Handled; //!< 事件已经被处理。

	explicit
	RoutedEventArgs(RoutingStrategy = Direct);
};

inline
RoutedEventArgs::RoutedEventArgs(RoutingStrategy strategy)
	: Strategy(strategy), Handled(false)
{}


//! \brief 屏幕（指针设备）输入事件参数模块类。
typedef Drawing::Point MScreenPositionEventArgs;


//! \brief 输入事件参数类。
struct InputEventArgs : public RoutedEventArgs
{
public:
	KeyCode Key;

	/*!
	\brief 构造：使用本机键按下对象和路由事件类型。
	*/
	InputEventArgs(KeyCode = 0, RoutingStrategy = Direct);

	DefConverter(KeyCode, Key)

	DefGetter(KeyCode, KeyCode, Key)
};

inline
InputEventArgs::InputEventArgs(KeyCode k, RoutingStrategy s)
	: RoutedEventArgs(s), Key(k)
{}


//! \brief 按键输入事件参数类。
struct KeyEventArgs : public InputEventArgs
{
public:
	typedef KeyCode InputType; //!< 输入类型。

	/*!
	\brief 构造：使用输入类型对象和路由事件类型。
	*/
	KeyEventArgs(const InputType& = 0, RoutingStrategy = Direct);
};

inline
KeyEventArgs::KeyEventArgs(const InputType& k, RoutingStrategy s)
	: InputEventArgs(k, s)
{}


//! \brief 指针设备输入事件参数类。
struct TouchEventArgs : public InputEventArgs, public MScreenPositionEventArgs
{
public:
	typedef Drawing::Point InputType; //!< 输入类型。

	/*!
	\brief 构造：使用输入类型对象和路由事件类型。
	*/
	TouchEventArgs(const InputType& = InputType::Zero,
		RoutingStrategy = Direct);
};

inline
TouchEventArgs::TouchEventArgs(const InputType& pt, RoutingStrategy s)
	: InputEventArgs(s), MScreenPositionEventArgs(pt)
{}


//! \brief 控件事件参数类。
struct IndexEventArgs : public EventArgs
{
	typedef ssize_t IndexType;

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
	Paint, //!< 界面绘制。

	//图形用户界面输入事件。
	KeyUp, //!< 键接触结束。
	KeyDown, //!< 键接触开始。
	KeyHeld, //!< 键接触保持。
	KeyPress, //!< 键按下。
	TouchUp, //!< 屏幕接触结束。
	TouchDown, //!< 屏幕接触开始。
	TouchHeld, //!< 屏幕接触保持。
	TouchMove, //!< 屏幕接触移动。
	Click, //!< 屏幕点击。
	Enter, //!< 控件进入。
	Leave, //!< 控件离开。

	//焦点事件。
	GotFocus, //!< 焦点获得。
	LostFocus //!< 焦点失去。

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
DefEventTypeMapping(Paint, HVisualEvent)

DefEventTypeMapping(KeyUp, HKeyEvent)
DefEventTypeMapping(KeyDown, HKeyEvent)
DefEventTypeMapping(KeyHeld, HKeyEvent)
DefEventTypeMapping(KeyPress, HKeyEvent)
DefEventTypeMapping(TouchUp, HTouchEvent)
DefEventTypeMapping(TouchDown, HTouchEvent)
DefEventTypeMapping(TouchHeld, HTouchEvent)
DefEventTypeMapping(TouchMove, HTouchEvent)
DefEventTypeMapping(Click, HTouchEvent)
DefEventTypeMapping(Enter, HTouchEvent)
DefEventTypeMapping(Leave, HTouchEvent)

DefEventTypeMapping(GotFocus, HVisualEvent)
DefEventTypeMapping(LostFocus, HVisualEvent)


//! \brief 控件事件映射表类型。
typedef GEventMap<IControl, VisualEvent> VisualEventMapType;


//! \brief 控件接口。
DeclBasedInterface1(IControl, virtual IWidget)
	DeclIEntry(bool IsEnabled() const) //!< 判断是否有效。
	DeclIEntry(bool IsFocused() const) //!< 判断是否取得焦点。

	DeclIEntry(VisualEventMapType& GetEventMap() const) //!< 取事件映射表。

	DeclIEntry(void SetEnabled(bool)) //!< 设置有效性。

	//! \brief 向部件容器请求获得焦点，并指定 GotFocus 事件发送控件。
	DeclIEntry(void RequestFocusFrom(IControl&))

	//! \brief 释放焦点，并指定 LostFocus 事件发送控件。
	DeclIEntry(void ReleaseFocusFrom(IControl&))
EndDecl


/*!
\ingroup HelperFunction
\brief 取控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
*/
template<VisualEvent _vID>
inline typename EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(VisualEventMapType& m)
{
	return m.GetEvent<typename EventTypeMapping<_vID>::HandlerType>(_vID);
}
/*!
\ingroup HelperFunction
\brief 取控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
*/
template<VisualEvent _vID>
inline typename EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(IControl& c)
{
	return FetchEvent<_vID>(c.GetEventMap());
}

/*!
\ingroup HelperFunction
\brief 调用控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent _vID>
inline void
CallEvent(VisualEventMapType& m, typename EventTypeMapping<_vID>
	::HandlerType::SenderType& sender, typename EventTypeMapping<_vID>
	::HandlerType::EventArgsType& e)
{
	m.DoEvent<typename EventTypeMapping<_vID>::HandlerType>(_vID, sender,
		std::move(e));
}
/*!
\ingroup HelperFunction
\brief 调用控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent _vID>
inline void
CallEvent(IControl& c, typename EventTypeMapping<_vID>
	::HandlerType::SenderType& sender, typename EventTypeMapping<_vID>
	::HandlerType::EventArgsType& e)
{
	CallEvent<_vID>(c.GetEventMap(), sender, e);
}
/*!
\ingroup HelperFunction
\brief 调用控件自身事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent _vID>
inline void
CallEvent(IControl& c, typename EventTypeMapping<_vID>
	::HandlerType::EventArgsType& e)
{
	CallEvent<_vID>(c.GetEventMap(), c, e);
}


/*!
\brief 处理键接触保持事件。
*/
void
OnKeyHeld(IControl&, KeyEventArgs&&);

/*!
\brief 处理屏幕接触保持事件。

 Control 加载的默认 TouchHeld 事件处理器。
实现记录坐标偏移（用于拖放）或触发 TouchMove 事件。
*/
void
OnTouchHeld(IControl&, TouchEventArgs&&);

/*!
\brief 处理屏幕接触移动事件。
\note 重复触发 TouchDown 事件。
*/
void
OnTouchMove(IControl&, TouchEventArgs&&);

/*!
\brief 处理屏幕接触移动事件。
\note 使用拖放。
*/
void
OnTouchMove_Dragging(IControl&, TouchEventArgs&&);

/*!
\brief 处理按键事件：按键-指针设备接触结束。

绑定触发 TouchUp 和 Leave 事件。
*/
void
OnKey_Bound_TouchUpAndLeave(IControl&, KeyEventArgs&&);

/*!
\brief 处理按键事件：按键-指针设备接触开始。

绑定触发 Enter 和 TouchDown 事件。
*/
void
OnKey_Bound_EnterAndTouchDown(IControl&, KeyEventArgs&&);

/*!
\brief 处理按键事件：按键-指针设备按下。

绑定触发 Click 事件。
*/
void
OnKey_Bound_Click(IControl&, KeyEventArgs&&);


//! \brief 控件。
class Control : public Widgets::Widget, public AFocusRequester,
	virtual implements IControl
{
private:
	bool enabled; //!< 控件有效性。

protected:
	mutable VisualEventMapType EventMap; //!< 事件映射表。

public:
	//标准控件事件见 VisualEvent 。

	//扩展控件事件。
//	DeclEvent(HPointEvent, Move) //!< 控件移动。
//	DeclEvent(HSizeEvent, Resize) //!< 控件大小调整。
	//事件组映射。
	/*!
	\brief 按键-指针设备输入事件组映射。
	\note 默认为 Control::GetBoundControlPtr 。

	转换按键输入事件为指定控件的指针设备输入事件。
	*/
	std::function<IControl*(const KeyCode&)> BoundControlPtr;

	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	Control(const Rect& = Rect::Empty);
	/*!
	\brief 析构。
	*/
	virtual
	~Control();

	ImplI1(IControl) DefPredicate(Enabled, enabled)
	ImplI1(IControl) bool
	IsFocused() const;

	ImplI1(IControl) DefGetter(VisualEventMapType&, EventMap, EventMap)
	/*!
	\brief 取按键-指针设备输入默认事件组映射。
	*/
	virtual PDefH1(IControl*, GetBoundControlPtr, const KeyCode&)
		ImplRet(nullptr)

	ImplI1(IControl) void
	SetLocation(const Point&);
	ImplI1(IControl) void
	SetSize(const Size&);
	ImplI1(IControl) DefSetter(bool, Enabled, enabled)

	/*!
	\brief 绘制控件界面。
	\note 实现 Widget 的界面绘制。
	*/
	virtual void
	DrawControl();

	/*!
	\brief 刷新：绘制界面。
	\note 调用 DrawControl 后调用 Paint 事件。
	*/
	virtual void
	Refresh();

	/*!
	\brief 向部件容器请求获得焦点。
	\note 若成功则触发指定控件发送的 GotFocus 事件。
	*/
	ImplI1(IControl) void
	RequestFocusFrom(IControl&);

	/*!
	\brief 释放焦点。
	\note 触发指定控件发送的 LostFocus 事件。
	*/
	ImplI1(IControl) void
	ReleaseFocusFrom(IControl&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

