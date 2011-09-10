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
\version r5436;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:24 +0800;
\par 修改时间:
	2011-09-10 20:55 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YComponent;
*/


#ifndef YSL_INC_UI_YCONTROL_H_
#define YSL_INC_UI_YCONTROL_H_

#include "ywidget.h"
#include "../Core/yevt.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

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

	IWidget& Widget;
	IndexType Index;

	/*!
	\brief 构造：使用控件引用和索引值。
	*/
	IndexEventArgs(IWidget&, IndexType);
	DefConverter(IndexType, Index)
};

inline
IndexEventArgs::IndexEventArgs(IWidget& wgt, IndexEventArgs::IndexType idx)
	: EventArgs(),
	Widget(wgt), Index(idx)
{}


//! \brief 值类型参数类模块模板。
PDefTH1(_type)
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
DefDelegate(HVisualEvent, IWidget, EventArgs)
DefDelegate(HInputEvent, IWidget, InputEventArgs)
DefDelegate(HKeyEvent, IWidget, KeyEventArgs)
DefDelegate(HTouchEvent, IWidget, TouchEventArgs)
DefDelegate(HIndexEvent, IWidget, IndexEventArgs)
//DefDelegate(HPointEvent, IWidget, Drawing::Point)
//DefDelegate(HSizeEvent, IWidget, Size)


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
typedef GEventMap<IWidget, VisualEvent> VisualEventMap;


/*!
\brief 部件控制器。

保存部件的事件响应策略和状态。
*/
class WidgetController
{
private:
	bool enabled; //!< 控件可用性。

protected:
	mutable VisualEventMap EventMap; //!< 事件映射表。

public:
	explicit
	WidgetController(bool);
	WidgetController(bool, const VisualEventMap&);
	WidgetController(bool, VisualEventMap&&);

	DefPredicate(Enabled, enabled)

	DefGetter(VisualEventMap&, EventMap, EventMap) //!< 取事件映射表。

	DefSetter(bool, Enabled, enabled)
};

inline
WidgetController::WidgetController(bool b)
	: enabled(b), EventMap()
{}
inline
WidgetController::WidgetController(bool b, const VisualEventMap& m)
	: enabled(b), EventMap(m)
{}
inline
WidgetController::WidgetController(bool b, VisualEventMap&& m)
	: enabled(b), EventMap(std::move(m))
{}


/*!
\brief 判断部件是否为可用的控件。
*/
inline bool
IsEnabled(const IWidget& wgt)
{
	try
	{
		return wgt.GetController().IsEnabled();
	}
	catch(BadControl&)
	{}
	return false;
}

/*!
\brief 判断部件是否为可用且可见的控件。
*/
inline bool
IsEnabledAndVisible(const IWidget& wgt)
{
	return IsEnabled(wgt) && wgt.IsVisible();
}

/*!
\brief 设置部件可用性。
*/
inline void
SetEnabledOf(IWidget& wgt, bool b)
{
	try
	{
		wgt.GetController().SetEnabled(b);
	}
	catch(BadControl&)
	{}
}

/*!
\brief 设置部件可用性，且当可用性改变时无效化部件区域。
*/
void
Enable(IWidget&, bool = true);


/*!
\brief 判断部件是否取得焦点。
*/
bool
IsFocused(const IWidget&);

/*!
\brief 向部件容器请求获得焦点，并指定 GotFocus 事件发送控件。
\param dst 事件目标。
\param src 事件源。
\note 若成功则在 dst 上触发 src 发送的 GotFocus 事件。
*/
void
RequestFocusFrom(IWidget& dst, IWidget& src);

/*!
\brief 释放焦点，并指定 LostFocus 事件发送控件。
\param dst 事件目标。
\param src 事件源。
\note 若成功则在 dst 上触发 src 发送的 LostFocus 事件。
*/
void
ReleaseFocusFrom(IWidget& dst, IWidget& src);

/*!
\ingroup HelperFunction
\brief 向部件容器释放获得焦点，成功后向自身发送 GotFocus 事件。
*/
inline void
RequestFocus(IWidget& wgt)
{
	RequestFocusFrom(wgt, wgt);
}

/*!
\ingroup HelperFunction
\brief 释放焦点，成功后向自身发送 LostFocus 事件。
*/
inline void
ReleaseFocus(IWidget& wgt)
{
	ReleaseFocusFrom(wgt, wgt);
}


/*!
\ingroup HelperFunction
\brief 取控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
*/
template<VisualEvent _vID>
inline typename EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(VisualEventMap& m)
{
	return m.GetEvent<typename EventTypeMapping<_vID>::HandlerType>(_vID);
}
/*!
\ingroup HelperFunction
\brief 取部件事件。
\exception BadControl 异常中立：无事件映射表：由 GetController 抛出。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
*/
template<VisualEvent _vID>
inline typename EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(IWidget& wgt)
{
	return FetchEvent<_vID>(wgt.GetController().GetEventMap());
}

/*!
\ingroup HelperFunction
\brief 调用事件映射表中的控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent _vID, typename _tEventArgs>
inline size_t
CallEvent(VisualEventMap& m, typename EventTypeMapping<_vID>
	::HandlerType::SenderType& sender, _tEventArgs&& e)
{
	static_assert(std::is_same<typename std::remove_reference<_tEventArgs>
		::type, typename EventTypeMapping<_vID>::HandlerType::EventArgsType>
		::value, "Invalid event argument type found @ CallEvent #1;");

	return m.DoEvent<typename EventTypeMapping<_vID>::HandlerType>(_vID, sender,
		yforward(e));
}
/*!
\brief 调用部件事件，并忽略 BadControl 异常。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent _vID, typename _tEventArgs>
inline size_t
CallEvent(IWidget& wgt, typename EventTypeMapping<_vID>
	::HandlerType::SenderType& sender, _tEventArgs&& e)
{
	try
	{
		return CallEvent<_vID>(wgt.GetController().GetEventMap(), sender,
			yforward(e));		
	}
	catch(BadControl&)
	{}
	return 0;
}
/*!
\ingroup HelperFunction
\brief 调用控件自身事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent _vID, typename _tEventArgs>
inline size_t
CallEvent(IWidget& wgt, _tEventArgs&& e)
{
	return CallEvent<_vID>(wgt, wgt, yforward(e));
}


//公共事件处理器。

/*!
\brief 处理键接触保持事件。
*/
void
OnKeyHeld(IWidget&, KeyEventArgs&&);

/*!
\brief 处理屏幕接触保持事件。

 Control 加载的默认 TouchHeld 事件处理器。
实现记录坐标偏移（用于拖放）或触发 TouchMove 事件。
*/
void
OnTouchHeld(IWidget&, TouchEventArgs&&);

/*!
\brief 处理屏幕接触移动事件。
\note 重复触发 TouchDown 事件。
*/
void
OnTouchMove(IWidget&, TouchEventArgs&&);

/*!
\brief 处理屏幕接触移动事件。
\note 使用拖放。
*/
void
OnTouchMove_Dragging(IWidget&, TouchEventArgs&&);

/*!
\brief 处理部件事件：引起无效化。
*/
PDefTH1(_tEventArgs)
void
OnWidget_Invalidate(IWidget& wgt, _tEventArgs&&)
{
	Invalidate(wgt);
}


// Control 事件处理器。

/*!
\brief 处理按键事件：按键-指针设备接触结束。

绑定触发 TouchUp 和 Leave 事件。
\note 仅对 Control 及其派生类有效。
*/
void
OnKey_Bound_TouchUpAndLeave(IWidget&, KeyEventArgs&&);

/*!
\brief 处理按键事件：按键-指针设备接触开始。

绑定触发 Enter 和 TouchDown 事件。
\note 仅对 Control 及其派生类有效。
*/
void
OnKey_Bound_EnterAndTouchDown(IWidget&, KeyEventArgs&&);

/*!
\brief 处理按键事件：按键-指针设备按下。

绑定触发 Click 事件。
\note 仅对 Control 及其派生类有效。
*/
void
OnKey_Bound_Click(IWidget&, KeyEventArgs&&);


//! \brief 控件。
class Control : public Widget
{
protected:
	DefExtendEventMap(ControlEventMap, VisualEventMap)

public:
	//标准控件事件见 VisualEvent 。

	//扩展控件事件示例。
//	DeclEvent(H1Event, Ext1) //!< 扩展事件 1 。
//	DeclEvent(H2Event, Ext2) //!< 扩展事件 2 。

	//事件组映射。
	/*!
	\brief 按键-指针设备输入事件组映射。
	\note 默认为 Control::GetBoundControlPtr 。

	转换按键输入事件为指定部件的指针设备输入事件。
	*/
	std::function<IWidget*(const KeyCode&)> BoundControlPtr;

	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	Control(const Rect& = Rect::Empty);
	/*!
	\brief 复制构造：除容器为空外深复制。
	*/
	Control(const Control&);
	Control(Control&&) = default;

	/*!
	\brief 取按键-指针设备输入默认事件组映射。
	*/
	virtual PDefH1(IWidget*, GetBoundControlPtr, const KeyCode&)
		ImplRet(nullptr)

	virtual void
	SetLocation(const Point&);
	virtual void
	SetSize(const Size&);

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const Graphics&, const Point&, const Rect&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

