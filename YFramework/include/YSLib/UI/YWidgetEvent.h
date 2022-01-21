/*
	© 2010-2016, 2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YWidgetEvent.h
\ingroup UI
\brief 标准部件事件定义。
\version r1939
\author FrankHB <frankhb1989@gmail.com>
\since build 241
\par 创建时间:
	2010-12-17 10:27:50 +0800
\par 修改时间:
	2021-12-29 01:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetEvent
*/


#ifndef YSL_INC_UI_YWidgetEvent_h_
#define YSL_INC_UI_YWidgetEvent_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YComponent // for ystdex::enable_if_same_t;
#include YFM_YSLib_Core_YEvent // for module YCore::Event, GHEvent;
#include YFM_YSLib_Core_YString // for String;
#include YFM_YSLib_Core_YDevice // for Devices::KeyInputDevice::Tester,
//	Devices::KeyInputDevice::DefaultTest;
#include <ystdex/function.hpp> // for ystdex::parameter_of;

namespace YSLib
{

namespace UI
{

/*!
\brief 用户界面绘制优先级。
\since build 294
*/
//@{
yconstexpr const EventPriority BackgroundPriority(0xC0);
yconstexpr const EventPriority BoundaryPriority(0x60);
yconstexpr const EventPriority ForegroundPriority(0x40);
//@}


/*!
\brief 用户界面事件参数基类。
\since build 255
*/
struct YF_API UIEventArgs
{
private:
	//! \since build 672
	lref<IWidget> sender;

public:
	explicit
	UIEventArgs(IWidget& wgt)
		: sender(wgt)
	{}
	/*!
	\brief 复制构造：默认实现。
	\since build 295
	*/
	DefDeCopyCtor(UIEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 423
	*/
	virtual
	~UIEventArgs();

	/*!
	\brief 复制赋值：默认实现。
	\since build 295
	*/
	DefDeCopyAssignment(UIEventArgs)

	DefGetter(const ynothrow, IWidget&, Sender, sender)
	PDefH(void, SetSender, IWidget& wgt)
		ImplExpr(sender = wgt)
};


/*!
\brief 路由事件参数基类。
\since build 195
*/
struct YF_API RoutedEventArgs : public UIEventArgs
{
public:
	/*!
	\brief 事件路由策略枚举。
	\since build 416
	*/
	enum RoutingStrategy
	{
		//! \brief 气泡事件：向上遍历视图树时触发。
		Bubble = 0,
		//! \brief 隧道事件：向下遍历视图树时触发。
		Tunnel = 1,
		//! \brief 直接事件：仅当遍历至目标控件时触发。
		Direct = 2
	};

	//! \brief 事件路由策略。
	RoutingStrategy Strategy;
	//! \brief 事件已被处理。
	bool Handled;

	RoutedEventArgs(IWidget& wgt, RoutingStrategy strategy = Direct)
		: UIEventArgs(wgt),
		Strategy(strategy), Handled(false)
	{}
	//! \since build 586
	DefDeCopyCtor(RoutedEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~RoutedEventArgs() override;
};


/*!
\brief 输入事件参数类。
\since 早于 build 132
*/
struct YF_API InputEventArgs : public RoutedEventArgs
{
public:
	/*!
	\brief 按键状态测试器。
	\since build 490
	*/
	Devices::KeyInputDevice::Tester
		Tester{Devices::KeyInputDevice::DefaultTest};
	/*!
	\brief 包含的按键代码。
	\since build 298
	*/
	KeyInput Keys;

	/*!
	\brief 构造：使用本机键按下对象和路由事件类型。
	\since build 298
	*/
	InputEventArgs(IWidget&, const KeyInput& = {}, RoutingStrategy = Direct);
	//! \since build 586
	DefDeCopyCtor(InputEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~InputEventArgs() override;

	/*!
	\brief 转换为按键代码类型。
	\since build 298
	*/
	DefCvt(const ynothrow, const KeyInput&, Keys)

	/*!
	\brief 使用 Tester 和 Keys 判断按键状态。
	\since build 490
	*/
	PDefHOp(bool, [], KeyIndex code) const
		ImplRet(Tester(Keys, code))

	/*!
	\brief 取按键代码。
	\since build 298
	*/
	DefGetter(const ynothrow, const KeyInput&, Keys, Keys)
};


/*!
\brief 按键输入事件参数类。
\since 早于 build 132
*/
struct YF_API KeyEventArgs : public InputEventArgs
{
public:
	//! \brief 输入类型。
	using InputType = KeyInput;

	//! \brief 构造：使用输入类型对象和事件路由策略。
	KeyEventArgs(IWidget&, const InputType& = {}, RoutingStrategy = Direct);

	//! \since build 586
	DefDeCopyCtor(KeyEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~KeyEventArgs() override;
};


/*!
\brief 指针设备输入事件参数类。
\since build 422
*/
struct YF_API CursorEventArgs : public InputEventArgs
{
public:
	//! \brief 输入类型。
	using InputType = Drawing::Point;

	//! \since build 482
	InputType Position;

	/*!
	\brief 构造：使用按键输入类型对象、输入类型对象和事件路由策略。
	\since build 422
	*/
	CursorEventArgs(IWidget&, const KeyInput&, const InputType& = {},
		RoutingStrategy = Direct);
	//! \since build 586
	DefDeCopyCtor(CursorEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~CursorEventArgs() override;
	/*!
	\brief 转换为输入类型。
	\since build 482
	*/
	DefCvt(const ynothrow, const InputType&, Position)
};


/*!
\brief 滚轮度量：以角度计量的转动滚轮的幅度。
\since build 423
*/
using WheelDelta = ptrdiff_t;


/*!
\brief 滚轮事件参数。
\since build 423
*/
class YF_API CursorWheelEventArgs : public CursorEventArgs
{
private:
	WheelDelta delta;

public:
	CursorWheelEventArgs(IWidget&, WheelDelta, const KeyInput&,
		const InputType& = {}, RoutingStrategy = Direct);
	//! \since build 586
	DefDeCopyCtor(CursorWheelEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~CursorWheelEventArgs() override;

	DefGetter(const ynothrow, WheelDelta, Delta, delta)
};


/*!
\brief 文本输入事件参数。
\since build 482
*/
class YF_API TextInputEventArgs : public InputEventArgs
{
public:
	String Text;

	TextInputEventArgs(IWidget&, String, const KeyInput& = {},
		RoutingStrategy = Direct);
	TextInputEventArgs(InputEventArgs, String);
	//! \since build 586
	DefDeCopyCtor(TextInputEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~TextInputEventArgs() override;
};


/*!
\brief 简单 UI 事件参数类。
\since build 268

保存部件引用和指定类型值的事件参数。
*/
template<typename _type>
struct GValueEventArgs : public UIEventArgs
{
	//! \brief 值类型。
	using ValueType = _type;

	ValueType Value;

	/*!
	\brief 构造：使用部件引用和值。
	*/
	template<typename... _tParams>
	yconstfn
	GValueEventArgs(IWidget& wgt, _tParams&&... args)
		: UIEventArgs(wgt),
		Value(yforward(args)...)
	{}
	DefCvt(const ynothrow, ValueType, Value)
};


/*!
\brief 部件绘制参数。
\since build 242
*/
struct YF_API PaintEventArgs : public UIEventArgs, public PaintContext
{
	PaintEventArgs(IWidget&);
	PaintEventArgs(IWidget&, const PaintContext&);
	PaintEventArgs(IWidget&, const Drawing::Graphics&, const Drawing::Point&,
		const Drawing::Rect&);
	//! \since build 586
	DefDeCopyCtor(PaintEventArgs)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~PaintEventArgs() override;
};


//事件处理器类型。
using HUIEvent = GHEvent<void(UIEventArgs&&)>;
using HInputEvent = GHEvent<void(InputEventArgs&&)>;
using HKeyEvent = GHEvent<void(KeyEventArgs&&)>;
//! \since build 422
using HCursorEvent = GHEvent<void(CursorEventArgs&&)>;
using HPaintEvent = GHEvent<void(PaintEventArgs&&)>;
/*!
\brief 指针设备滚轮事件。
\since build 425
*/
using HCursorWheelEvent = GHEvent<void(CursorWheelEventArgs&&)>;
/*!
\brief 文本输入事件。
\since build 482
*/
using HTextInputEvent = GHEvent<void(TextInputEventArgs&&)>;


#define DefEventTypeMapping(_name, _tEventHandler) \
	template<> \
	struct EventTypeMapping<_name> \
	{ \
		using HandlerType = _tEventHandler; \
	};


/*!
\brief 标准控件事件空间。
\since build 416
*/
enum VisualEvent : size_t
{
	//\note 视图变更事件。
	//@{
	//! \brief 移动：位置调整。
	Move,
	//! \brief 大小调整。
	Resize,
	//@}
	//! \note GUI 输入事件。
	//@{
	//! \brief 键接触结束。
	KeyUp,
	//! \brief 键接触开始。
	KeyDown,
	//! \brief 键接触保持。
	KeyHeld,
	//! \brief 键按下。
	KeyPress,
	//! \brief 屏幕接触结束。
	TouchUp,
	//! \brief 屏幕接触开始。
	TouchDown,
	//! \brief 屏幕接触保持。
	TouchHeld,
	//! \brief 屏幕点击。
	Click,
	/*!
	\brief 屏幕点击在不同部件上。
	\since build 433
	*/
	ClickAcross,
	/*!
	\brief 指针设备光标悬停。
	\since build 422
	*/
	CursorOver,
	/*!
	\brief 指针设备滚轮输入。
	\since build 423
	*/
	CursorWheel,
	/*!
	\brief 文本输入。
	\since build 482
	*/
	TextInput,
	//@}
	//! \note GUI 输出事件。
	//@{
	//! \brief 界面绘制。
	Paint,
	//@}
	//! \note 焦点事件。
	//@{
	//! \brief 焦点获得。
	GotFocus,
	//! \brief 焦点失去。
	LostFocus,
	//@}
	//边界事件。
	//@{
	//! \brief 控件进入。
	Enter,
	//! \brief 控件离开。
	Leave,
	//@}
	/*!
	\brief 事件边界。
	\since build 580

	仅用于标识枚举中的最大项，不作为实际的事件使用。
	*/
	MaxEvent
};


//! \note 定义 HandlerType 的默认值可能会导致运行时转换失败而抛出异常。
template<VisualEvent>
struct EventTypeMapping
{
//	using HandlerType = HEvent;
};

DefEventTypeMapping(Move, HUIEvent)
DefEventTypeMapping(Resize, HUIEvent)

DefEventTypeMapping(KeyUp, HKeyEvent)
DefEventTypeMapping(KeyDown, HKeyEvent)
DefEventTypeMapping(KeyHeld, HKeyEvent)
DefEventTypeMapping(KeyPress, HKeyEvent)
//! \since build 422
DefEventTypeMapping(CursorOver, HCursorEvent)
DefEventTypeMapping(TouchUp, HCursorEvent)
DefEventTypeMapping(TouchDown, HCursorEvent)
DefEventTypeMapping(TouchHeld, HCursorEvent)
DefEventTypeMapping(Click, HCursorEvent)
//! \since build 433
DefEventTypeMapping(ClickAcross, HCursorEvent)
//! \since build 423
DefEventTypeMapping(CursorWheel, HCursorWheelEvent)
//! \since build 482
DefEventTypeMapping(TextInput, HTextInputEvent)

DefEventTypeMapping(Paint, HPaintEvent)

DefEventTypeMapping(GotFocus, HUIEvent)
DefEventTypeMapping(LostFocus, HUIEvent)

DefEventTypeMapping(Enter, HCursorEvent)
DefEventTypeMapping(Leave, HCursorEvent)


//! \since build 850
//@{
//! \brief 取索引指定的事件处理器类型。
template<VisualEvent _vID>
using HandlerOf = typename EventTypeMapping<_vID>::HandlerType;

//! \brief 取索引指定的事件类型。
template<VisualEvent _vID>
using EventOf = GEvent<typename HandlerOf<_vID>::FuncType>;
//@}


/*!
\brief 事件映射项类型。
\since build 850
*/
using EventItem = ValueObject;

/*!
\brief 事件引用。
\since build 850
*/
class YF_API EventRef final
{
private:
	ValueObject content;

public:
	template<typename _fSig>
	EventRef(GEvent<_fSig>& e)
		: content(e, OwnershipTag<>())
	{}
	//! \since build 851
	template<class _tParam,
		yimpl(typename = ystdex::enable_if_same_t<EventItem, _tParam>)>
	EventRef(const _tParam& e)
		: content(e.MakeIndirect())
	{}
	DefDeCopyMoveCtorAssignment(EventRef)

	//! \throw std::bad_cast 转换失败。
	//@{
	//! \brief 转换映射项为签名指定的事件类型的对象引用。
	template<typename _fSig>
	GEvent<_fSig>&
	Cast()
	{
		return content.Access<GEvent<_fSig>&>();
	}

	//! \brief 转换映射项为索引指定的事件类型的对象引用。
	template<VisualEvent _vID>
	EventOf<_vID>&
	CastWith()
	{
		return content.Access<EventOf<_vID>&>();
	}
	//@}
};

//! \brief 映射表类型。
using VisualEventMap = map<VisualEvent, EventItem>;


/*!
\ingroup exceptions
\brief 错误或不存在的部件事件异常。
\since build 241
*/
class YF_API BadEvent : public LoggedEvent
{
public:
	//! \since build 538
	using LoggedEvent::LoggedEvent;

	//! \since build 586
	//@{
	DefDeCtor(BadEvent)
	DefDeCopyCtor(BadEvent)
	//! \brief 虚析构：类定义外默认实现。
	~BadEvent() override;
	//@}
};


/*!
\ingroup exceptions
\brief 用户事件信号：表示携带用户事件的消息处理中断的异常。
\since build 444
*/
class YF_API UIEventSignal : public Messaging::MessageSignal
{
public:
	//! \since build 586
	//@{
	using MessageSignal::MessageSignal;

	DefDeCopyCtor(UIEventSignal)
	//! \brief 虚析构：类定义外默认实现。
	virtual
	~UIEventSignal();
	//@}
};


/*!
\brief 抽象控制器。
\since build 243
*/
class YF_API AController : public ystdex::cloneable
{
private:
	//! \brief 控件可用性。
	bool enabled;

public:
	/*!
	\brief 构造：使用指定可用性。
	*/
	AController(bool b = true)
		: enabled(b)
	{}
	//! \since build 586
	DefDeCopyCtor(AController)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 295
	*/
	~AController() override;

	DefPred(const ynothrow, Enabled, enabled)
	/*!
	\brief 判断指定事件是否启用。
	\note 默认实现：仅启用 Paint 事件。
	\since build 581
	*/
	virtual PDefH(bool, IsEventEnabled, VisualEvent id) const
		ImplRet(id == Paint)

	/*!
	\brief 取事件项。
	\since build 850
	*/
	DeclIEntry(EventRef GetEvent(VisualEvent) const)
	/*!
	\brief 取事件项，若不存在则使用指定参数添加。
	\note 派生类的实现可能抛出异常并忽略加入任何事件项。
	\since build 850
	*/
	virtual PDefH(EventRef, GetEventRef, VisualEvent id, EventItem(&)()) const
		ImplRet(GetEvent(id))

	DefSetter(, bool, Enabled, enabled)
	/*!
	\brief 设置指定事件是否启用。
	\throw ystdex::unsupported 不支持设置事件启用操作。
	\note 默认实现：总是抛出异常。
	\since build 581
	*/
	virtual PDefH(void, SetEventEnabled, VisualEvent, bool)
		ImplThrow(ystdex::unsupported("AController::SetEventEnabled"))

	/*!
	\brief 复制实例。
	\since build 409
	*/
	DeclIEntry(AController* clone() const ImplI(ystdex::cloneable))
};


/*!
\brief 调用指定控制器和索引的事件。
\note std::out_of_range 和 std::bad_cast 异常。
\since build 850
*/
template<class _tEventHandler>
size_t
DoEvent(AController& controller, VisualEvent id,
	ystdex::parameter_of_t<0, typename _tEventHandler::FuncType>&& e)
{
	TryRet(controller.GetEvent(id).template
		Cast<typename _tEventHandler::FuncType>()(std::move(e)))
	CatchIgnore(std::out_of_range&)
	CatchIgnore(std::bad_cast&)
	return 0;
}


/*!
\brief 构造指针指向的 VisualEvent 指定的事件对象。
\since build 850
*/
template<VisualEvent _vID>
EventItem
NewEvent()
{
	return EventItem(EventOf<_vID>());
}

/*!
\brief 在事件映射表中取指定索引对应的事件。
\since build 850
*/
YB_ATTR_nodiscard YF_API YB_PURE EventRef
GetMappedEvent(VisualEventMap&, VisualEvent, EventItem(&)());

/*!
\ingroup helper_functions
\brief 取控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
\since build 572
*/
template<VisualEvent _vID>
YB_ATTR_nodiscard EventOf<_vID>&
FetchEvent(VisualEventMap& m)
{
	return
		UI::GetMappedEvent(m, _vID, NewEvent<_vID>).template CastWith<_vID>();
}
/*!
\ingroup helper_functions
\brief 取部件事件。
\tparam _vID 指定事件类型。
\param controller 指定部件的控制器。
\exception BadEvent 异常中立：由控制器抛出。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
\since build 572
*/
template<VisualEvent _vID>
YB_ATTR_nodiscard EventOf<_vID>&
FetchEvent(AController& controller)
{
	return
		controller.GetEventRef(_vID, NewEvent<_vID>).template CastWith<_vID>();
}


/*!
\brief 部件控制器。
\since build 236
*/
class YF_API WidgetController : public AController
{
public:
	//! \since build 850
	mutable GEvent<void(PaintEventArgs&&)> Paint;

	/*!
	\brief 构造：使用指定可用性。
	*/
	explicit
	WidgetController(bool = {});

	//! \since build 850
	EventRef
	GetEvent(VisualEvent) const ImplI(AController);

	//! \since build 409
	DefClone(const ImplI(AController), WidgetController)
};

} // namespace UI;

} // namespace YSLib;

#endif

