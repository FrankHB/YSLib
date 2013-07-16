/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtevt.h
\ingroup UI
\brief 标准部件事件定义。
\version r1453
\author FrankHB <frankhb1989@gmail.com>
\since build 241
\par 创建时间:
	2010-12-17 10:27:50 +0800
\par 修改时间:
	2013-07-16 09:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetEvent
*/


#ifndef YSL_INC_UI_ywgtevt_h_
#define YSL_INC_UI_ywgtevt_h_ 1

#include "YComponent.h"
#include "../Core/yevt.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

/*!
\brief 用户界面绘制优先级。
\since build 294
*/
//@{
yconstexpr EventPriority BackgroundPriority(0xC0);
yconstexpr EventPriority BoundaryPriority(0x60);
yconstexpr EventPriority ForegroundPriority(0x40);
//@}


/*!
\brief 用户界面事件参数基类。
\since build 255
*/
struct YF_API UIEventArgs
{
private:
	IWidget* pSender;

public:
	explicit
	UIEventArgs(IWidget& wgt)
		: pSender(&wgt)
	{}
	/*!
	\brief 复制构造：默认实现。
	\since build 295
	*/
	inline DefDeCopyCtor(UIEventArgs)
	//! \since build 423
	virtual DefDeDtor(UIEventArgs)

	/*!
	\brief 复制赋值：默认实现。
	\since build 295
	*/
	inline DefDeCopyAssignment(UIEventArgs)

	DefGetter(const ynothrow, IWidget&, Sender, *pSender)
	PDefH(void, SetSender, IWidget& wgt)
		ImplExpr(pSender = &wgt)
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
		Bubble = 0, //!< 气泡事件：向上遍历视图树时触发。
		Tunnel = 1, //!< 隧道事件：向下遍历视图树时触发。
		Direct = 2 //!< 直接事件：仅当遍历至目标控件时触发。
	};

	RoutingStrategy Strategy; //!< 事件路由策略。
	bool Handled; //!< 事件已经被处理。

	RoutedEventArgs(IWidget& wgt, RoutingStrategy strategy = Direct)
		: UIEventArgs(wgt),
		Strategy(strategy), Handled(false)
	{}
};


/*!
\brief 屏幕（指针设备）输入事件参数模块类。
\warning 非虚析构。
\since build 160
*/
typedef Drawing::Point MScreenPositionEventArgs;


/*!
\brief 输入事件参数类。
\since 早于 build 132
*/
struct YF_API InputEventArgs : public RoutedEventArgs
{
public:
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

	/*!
	\brief 转换为按键代码类型。
	\since build 298
	*/
	DefCvt(const ynothrow, const KeyInput&, Keys)

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
	typedef KeyInput InputType; //!< 输入类型。

	/*!
	\brief 构造：使用输入类型对象和事件路由策略。
	*/
	KeyEventArgs(IWidget&, const InputType& = {}, RoutingStrategy = Direct);
};


/*!
\brief 指针设备输入事件参数类。
\since build 422
*/
struct YF_API CursorEventArgs : public InputEventArgs,
	public MScreenPositionEventArgs
{
public:
	typedef Drawing::Point InputType; //!< 输入类型。

	/*!
	\brief 构造：使用按键输入类型对象、输入类型对象和事件路由策略。
	\since build 422
	*/
	CursorEventArgs(IWidget&, const KeyInput&, const InputType& = {},
		RoutingStrategy = Direct);
};


/*!
\brief 滚轮度量：以角度计量的转动滚轮的幅度。
\since build 423
*/
typedef ptrdiff_t WheelDelta;


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

	DefGetter(const ynothrow, WheelDelta, Delta, delta)
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
	typedef _type ValueType;

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
};


//事件处理器类型。
DeclDelegate(HUIEvent, UIEventArgs&&)
DeclDelegate(HInputEvent, InputEventArgs&&)
DeclDelegate(HKeyEvent, KeyEventArgs&&)
//! \since build 422
DeclDelegate(HCursorEvent, CursorEventArgs&&)
DeclDelegate(HPaintEvent, PaintEventArgs&&)
/*!
\brief 指针设备滚轮事件。
\since build 425
*/
DeclDelegate(HCursorWheelEvent, CursorWheelEventArgs&&)
//DefDelegate(HPointEvent, Drawing::Point&&)
//DefDelegate(HSizeEvent, Size&&)


#define DefEventTypeMapping(_name, _tEventHandler) \
	template<> \
	struct EventTypeMapping<_name> \
	{ \
		typedef _tEventHandler HandlerType; \
	};


/*!
\brief 标准控件事件空间。
\since build 416
*/
enum VisualEvent
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
	Click, //!< 屏幕点击。
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

	//图形用户界面输出事件。
	Paint, //!< 界面绘制。

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
};


template<VisualEvent>
struct EventTypeMapping
{
	//定义 HandlerType 的默认值可能会导致运行期 dynamic_cast 失败。
//	typedef HEvent HandlerType;
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
//! \since build 423
DefEventTypeMapping(CursorWheel, HCursorWheelEvent)

DefEventTypeMapping(Paint, HPaintEvent)

DefEventTypeMapping(GotFocus, HUIEvent)
DefEventTypeMapping(LostFocus, HUIEvent)

DefEventTypeMapping(Enter, HCursorEvent)
DefEventTypeMapping(Leave, HCursorEvent)


/*!
\brief 事件映射命名空间。
\since build 242
*/
YSL_BEGIN_NAMESPACE(EventMapping)

typedef GEventPointerWrapper<UIEventArgs&&> MappedType; //!< 映射项类型。
typedef GIHEvent<UIEventArgs&&> ItemType;
typedef pair<VisualEvent, MappedType> PairType;
typedef map<VisualEvent, MappedType> MapType; //!< 映射表类型。
typedef pair<typename MapType::iterator, bool> SearchResult; \
	//!< 搜索表结果类型。

YSL_END_NAMESPACE(EventMapping)

typedef EventMapping::MapType VisualEventMap;


/*!
\brief 错误或不存在的部件事件异常。
\since build 241
*/
class YF_API BadEvent : public LoggedEvent
{
public:
	//! \since build 411
	BadEvent(const std::string& msg = "")
		: LoggedEvent(msg)
	{}
};


/*!
\brief 控制器抽象类。
\since build 243
*/
class YF_API AController
{
private:
	bool enabled; //!< 控件可用性。

public:
	/*!
	\brief 构造：使用指定可用性。
	*/
	AController(bool b = true)
		: enabled(b)
	{}
	/*!
	\brief 析构：空实现。
	\since build 295
	*/
	virtual DefDeDtor(AController)

	DefPred(const ynothrow, Enabled, enabled)

	/*!
	\brief 取事件项。
	\since build 293
	*/
	DeclIEntry(EventMapping::ItemType& GetItem(const VisualEvent&))
	/*!
	\brief 取事件项，若不存在则用指定函数指针添加。
	\note 派生类的实现可能抛出异常并忽略加入任何事件项。
	*/
	virtual EventMapping::ItemType&
	GetItemRef(const VisualEvent& id, EventMapping::MappedType(&)())
	{
		return GetItem(id);
	}

	DefSetter(bool, Enabled, enabled)

	/*
	\brief 复制实例。
	\since build 409
	*/
	DeclIEntry(AController* clone() const)
};


//! \since build 413
template<class _tEventHandler>
size_t
DoEvent(AController& controller, const VisualEvent& id,
	typename EventArgsHead<typename _tEventHandler::TupleType>::type&& e)
{
	try
	{
		return dynamic_cast<EventT(typename _tEventHandler)&>(
			controller.GetItem(id))(std::move(e));
	}
	catch(std::out_of_range&)
	{}
	catch(std::bad_cast&)
	{}
	return 0;
}


/*!
\brief 构造指针指向的 VisualEvent 指定的事件对象。
*/
template<VisualEvent _vID>
EventMapping::MappedType
NewEvent()
{
	return EventMapping::MappedType(new GEventWrapper<EventT(typename
		EventTypeMapping<_vID>::HandlerType), UIEventArgs&&>());
}

/*!
\brief 在事件映射表中取指定 id 对应的事件。
*/
YF_API EventMapping::ItemType&
GetEvent(EventMapping::MapType&, const VisualEvent&,
	EventMapping::MappedType(&)());

/*!
\ingroup helper_functions
\brief 取控件事件。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
*/
template<VisualEvent _vID>
EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(VisualEventMap& m)
{
	return dynamic_cast<EventT(typename EventTypeMapping<_vID>::HandlerType)&>(
		GetEvent(m, _vID, NewEvent<_vID>));
}
/*!
\ingroup helper_functions
\brief 取部件事件。
\tparam _vID 指定事件类型。
\param controller 指定部件的控制器。
\exception BadEvent 异常中立：由控制器抛出。
\note 需要确保 EventTypeMapping 中有对应的 EventType ，否则无法匹配此函数模板。
\note 若控件事件不存在则自动添加空事件。
\since build 195
*/
template<VisualEvent _vID>
EventT(typename EventTypeMapping<_vID>::HandlerType)&
FetchEvent(AController& controller)
{
	return dynamic_cast<EventT(typename EventTypeMapping<_vID>::HandlerType)&>(
		controller.GetItemRef(_vID, NewEvent<_vID>));
}


/*!
\brief 部件控制器。
\since build 236
*/
class YF_API WidgetController : public AController
{
public:
	//! \since build 331
	GEventWrapper<EventT(HPaintEvent), UIEventArgs&&> Paint;

	/*!
	\brief 构造：使用指定可用性。
	*/
	explicit
	WidgetController(bool = {});

	ImplI(AController) EventMapping::ItemType&
	GetItem(const VisualEvent&);

	//! \since build 409
	ImplI(AController) DefClone(const override, WidgetController)
};

YSL_END_NAMESPACE(UI)

YSL_END

#endif

