/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.h
\ingroup UI
\brief 样式无关的控件。
\version r4749
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-02-18 13:44:24 +0800
\par 修改时间:
	2014-03-07 10:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YControl
*/


#ifndef YSL_INC_UI_ycontrol_h_
#define YSL_INC_UI_ycontrol_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidget
#include YFM_YSLib_UI_YWidgetEvent

namespace YSLib
{

namespace UI
{

using namespace Drawing;

/*!
\brief 部件控制器。

保存部件的事件响应策略和状态。
*/
class YF_API Controller : public AController
{
protected:
	mutable EventMapping::MapType EventMap; //!< 事件映射表。

public:
	explicit
	Controller(bool b)
		: AController(b), EventMap()
	{}
	//! \since build 368
	template<typename... _tParams>
	Controller(bool b, _tParams&&... args)
		: AController(b), EventMap(yforward(args)...)
	{}

	ImplI(AController) PDefH(EventMapping::ItemType&, GetItem,
		const VisualEvent& id)
		ImplRet(EventMap.at(id))
	EventMapping::ItemType&
	GetItemRef(const VisualEvent&, EventMapping::MappedType(&)()) override;
	DefGetter(const ynothrow, EventMapping::MapType&, EventMap, EventMap) \
		//!< 取事件映射表。

	//! \since build 409
	ImplI(AController) DefClone(const override, Controller)
};


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
\return 可用性是否改变。
\since build 286
*/
YF_API bool
Enable(IWidget&, bool = true);


/*!
\ingroup helper_functions
\brief 取部件事件。
\tparam _vID 指定事件类型。
\param wgt 指定部件。
\exception BadEvent 异常中立：由控制器抛出。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
\since build 237
*/
template<VisualEvent _vID>
inline EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(IWidget& wgt)
{
	return FetchEvent<_vID>(wgt.GetController());
}

/*!
\brief 调用部件事件，并忽略 BadEvent 异常。
\pre 事件参数需可转换为 EventTypeMapping 的 EventType 。
\note 若控件事件不存在则忽略。
*/
template<VisualEvent _vID, typename _tEventArgs>
inline size_t
CallEvent(IWidget& wgt, _tEventArgs&& e)
{
	using HandlerType = typename EventTypeMapping<_vID>::HandlerType;
	static_assert(std::is_convertible<ystdex::remove_reference_t<_tEventArgs>,
		ystdex::remove_reference_t<typename EventArgsHead<typename
		HandlerType::TupleType>::type>>::value,
		"Invalid event argument type found @ CallEvent;");

	try
	{
		return DoEvent<HandlerType>(wgt.GetController(), _vID, std::move(e));
	}
	catch(BadEvent&)
	{}
	return 0;
}


//公共事件处理器。

/*!
\brief 处理事件：直接转发至发送者。
\tparam _vID 指定转发的目标事件。
\tparam _tEventArgs 事件参数类型。
\warning 不应使用于自身以免无限递归。
\since build 300
*/
template<VisualEvent _vID, class _tEventArgs>
inline void
OnEvent_Call(_tEventArgs&& e)
{
	CallEvent<_vID>(e.GetSender(), e);
}

/*
\brief 处理事件：停止事件路由。
\since build 285
*/
template<class _tEventArgs>
inline void
OnEvent_StopRouting(_tEventArgs&& e)
{
	e.Handled = true;
}

/*!
\brief 处理键接触保持事件。
*/
YF_API void
OnKeyHeld(KeyEventArgs&&);

/*!
\brief 处理屏幕接触开始事件：使用一般接触策略。

屏幕接触时依次进行以下判断：
当事件路由策略非 RoutedEventArgs::Bubble 时请求置于顶端；
当事件路由策略为 RoutedEventArgs::Direct 时清除自身包含的焦点；
当事件路由策略非 RoutedEventArgs::Tunnel 时请求置于顶端。
\since build 422
*/
YF_API void
OnTouchDown_RequestToFrontFocused(CursorEventArgs&&);

/*!
\brief 处理屏幕接触保持事件。
\note 重复触发 TouchDown 事件。
\since build 422
*/
YF_API void
OnTouchHeld(CursorEventArgs&&);

/*!
\brief 处理屏幕接触移动事件：拖放按下部件。
\note 无效化当前区域，但不无效化移动后区域；通过 Move 事件可以调用无效化。
\since build 422
*/
YF_API void
OnTouchHeld_Dragging(CursorEventArgs&&);

/*!
\brief 处理屏幕接触移动事件：拖放指定部件。
\note 无效化当前区域，但不无效化移动后区域；通过 Move 事件可以调用无效化。
\since build 434
*/
YF_API void
OnTouchHeld_DraggingRaw(CursorEventArgs&&, IWidget&);


/*!
\brief 处理按键事件：按键-指针设备接触结束。
\note 仅对 Control 及其派生类有效。
\since build 424

绑定触发 TouchUp 事件。
*/
YF_API void
OnKey_Bound_TouchUp(KeyEventArgs&&);

/*!
\brief 处理按键事件：按键-指针设备接触开始。
\note 仅对 Control 及其派生类有效。
\since build 424

绑定触发 TouchDown 事件。
*/
YF_API void
OnKey_Bound_TouchDown(KeyEventArgs&&);

/*!
\brief 处理按键事件：按键-指针设备按下。

绑定触发 Click 事件。
\note 仅对 Control 及其派生类有效。
*/
YF_API void
OnKey_Bound_Click(KeyEventArgs&&);


/*!
\brief 控件。
\since build 168
*/
class YF_API Control : public Widget
{
protected:
	/*!
	\brief 扩展控件事件表。
	\note 加载 UI::OnTouchDown_RequestToFrontFocused 作为 TouchHeld 事件处理器。
	\since build 240
	*/
	DefExtendEventMap(YF_API ControlEventMap, VisualEventMap)

public:
	//标准控件事件见 VisualEvent 。

	//其它扩展控件事件示例。
//	DeclEvent(H1Event, Ext1) //!< 扩展事件 1 。
//	DeclEvent(H2Event, Ext2) //!< 扩展事件 2 。

	//事件组映射。
	/*!
	\brief 按键-指针设备输入事件组映射。
	\note 默认为 Control::GetBoundControlPtr 。

	转换按键输入事件为指定部件的指针设备输入事件。
	*/
	std::function<IWidget*(const KeyInput&)> BoundControlPtr;

	/*!
	\brief 构造：使用指定边界，默认背景（同 Widget 默认构造）。
	\see 其它重载构造函数。
	\since build 337
	*/
	explicit
	Control(const Rect& = {});
	/*!
	\brief 构造：使用指定边界，无背景。
	\note 使用扩展控件事件表加载的事件处理器。
	\note 加载事件处理器： Move 、 Resize 、 GotFocus 和 LostFocus 时自动无效化。
	\note 加载事件处理器： OnTouchDown_RequestToFrontFocused 。
	\since build 350
	*/
	Control(const Rect&, NoBackgroundTag);
	/*!
	\brief 复制构造：除容器为空外深复制。
	*/
	Control(const Control&);
	DefDeMoveCtor(Control)

	/*!
	\brief 取按键-指针设备输入默认事件组映射。
	*/
	virtual PDefH(IWidget*, GetBoundControlPtr, const KeyInput&)
		ImplRet(nullptr)

	/*!
	\brief 处理屏幕事件：关闭显示。
	\since build 422

	调用 UI::Close 。
	*/
	void
	OnTouch_Close(CursorEventArgs&&);
};


/*!
\brief 指针设备光标相对于控件的状态。
\since build 463
*/
enum class CursorState
{
	Outside,
	Over,
	Pressed
};

} // namespace UI;

} // namespace YSLib;

#endif

