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
\version r5520;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:24 +0800;
\par 修改时间:
	2011-10-01 13:19 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YComponent;
*/


#ifndef YSL_INC_UI_YCONTROL_H_
#define YSL_INC_UI_YCONTROL_H_

#include "ywidget.h"
#include "ywgtevt.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

using namespace Drawing;

/*!
\brief 部件控制器。

保存部件的事件响应策略和状态。
*/
class Controller : public AController
{
protected:
	mutable EventMapping::MapType EventMap; //!< 事件映射表。

public:
	explicit
	Controller(bool);
	Controller(bool, const EventMapping::MapType&);
	Controller(bool, EventMapping::MapType&&);

	ImplI1(AController) PDefH1(EventMapping::ItemType&, GetItemRef,
		const VisualEvent& id)
		ImplRet(EventMap.at(id))
	virtual EventMapping::ItemType&
	GetItemRef(const VisualEvent&, EventMapping::MappedType(&)());
	DefGetter(EventMapping::MapType&, EventMap, EventMap) //!< 取事件映射表。

	ImplI1(AController) DefClone(Controller, Clone)
};

inline
Controller::Controller(bool b)
	: AController(b), EventMap()
{}
inline
Controller::Controller(bool b, const EventMapping::MapType& m)
	: AController(b), EventMap(m)
{}
inline
Controller::Controller(bool b, EventMapping::MapType&& m)
	: AController(b), EventMap(std::move(m))
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
	catch(BadEvent&)
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
	catch(BadEvent&)
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
\brief 构造指针指向的 VisualEvent 指定的事件对象。
*/
template<VisualEvent _vID>
EventMapping::MappedType
NewEvent()
{
	return EventMapping::MappedType(new GEventWrapper<typename
		GSEvent<typename EventTypeMapping<_vID>::HandlerType>::EventType>());
}

/*!
\brief 在事件映射表中取指定 id 对应的事件。
*/
EventMapping::ItemType&
GetEvent(EventMapping::MapType&, const VisualEvent&,
	EventMapping::MappedType(&)());

/*!
\ingroup HelperFunction
\brief 取控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
*/
template<VisualEvent _vID>
typename EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(VisualEventMap& m)
{
	return dynamic_cast<typename GSEvent<typename EventTypeMapping<_vID>
		::HandlerType>::EventType&>(GetEvent(m, _vID, NewEvent<_vID>));
}
/*!
\ingroup HelperFunction
\brief 取部件事件。
\exception BadControl 异常中立：无事件映射表：由 GetController 抛出。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
*/
template<VisualEvent _vID>
typename EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(AController& controller)
{
	return dynamic_cast<typename GSEvent<typename EventTypeMapping<_vID>
		::HandlerType>::EventType&>(controller.GetItemRef(_vID,
		NewEvent<_vID>));
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
	return FetchEvent<_vID>(wgt.GetController());
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
	static_assert(std::is_same<typename std::remove_reference<_tEventArgs>
		::type, typename EventTypeMapping<_vID>::HandlerType::EventArgsType>
		::value, "Invalid event argument type found @ CallEvent #1;");

	try
	{
		return DoEvent<typename EventTypeMapping<_vID>::HandlerType>(
			wgt.GetController(), _vID, sender, yforward(e));
	}
	catch(BadEvent&)
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
	DefDeMoveCtor(Control)

	/*!
	\brief 取按键-指针设备输入默认事件组映射。
	*/
	virtual PDefH1(IWidget*, GetBoundControlPtr, const KeyCode&)
		ImplRet(nullptr)

	virtual void
	SetLocation(const Point&);
	virtual void
	SetSize(const Size&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

