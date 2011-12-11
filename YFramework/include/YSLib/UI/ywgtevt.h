/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtevt.h
\ingroup UI
\brief 标准部件事件定义。
\version r2070;
\author FrankHB<frankhb1989@gmail.com>
\since build 241 。
\par 创建时间:
	2010-12-17 10:27:50 +0800;
\par 修改时间:
	2011-12-11 07:25 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWidgetEvent;
*/


#ifndef INCLUDED_YWGTEVT_H_
#define INCLUDED_YWGTEVT_H_

#include "ycomp.h"
#include "../Core/yevt.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 用户界面事件参数基类。
\since build 255 。
*/
struct UIEventArgs
{
private:
	IWidget* pSender;

public:
	explicit
	UIEventArgs(IWidget&);
	DefGetter(const ynothrow, IWidget&, Sender, *pSender)
	PDefH(void, SetSender, IWidget& wgt)
		ImplExpr(pSender = &wgt)
};

inline
UIEventArgs::UIEventArgs(IWidget& wgt)
	: pSender(&wgt)
{}


/*!
\brief 路由事件参数基类。
\since build 195 。
*/
struct RoutedEventArgs : public UIEventArgs
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

	RoutedEventArgs(IWidget&, RoutingStrategy = Direct);
};

inline
RoutedEventArgs::RoutedEventArgs(IWidget& wgt, RoutingStrategy strategy)
	: UIEventArgs(wgt),
	Strategy(strategy), Handled(false)
{}


/*!
\brief 屏幕（指针设备）输入事件参数模块类。
\since build 160 。
*/
typedef Drawing::Point MScreenPositionEventArgs;


/*!
\brief 输入事件参数类。
\since 早于 build 132 。
*/
struct InputEventArgs : public RoutedEventArgs
{
public:
	KeyCode Key;

	/*!
	\brief 构造：使用本机键按下对象和路由事件类型。
	*/
	InputEventArgs(IWidget&, KeyCode = 0, RoutingStrategy = Direct);

	DefCvt(const ynothrow, KeyCode, Key)

	DefGetter(const ynothrow, KeyCode, KeyCode, Key)
};

inline
InputEventArgs::InputEventArgs(IWidget& wgt, KeyCode k, RoutingStrategy s)
	: RoutedEventArgs(wgt, s), Key(k)
{}


/*!
\brief 按键输入事件参数类。
\since 早于 build 132 。
*/
struct KeyEventArgs : public InputEventArgs
{
public:
	typedef KeyCode InputType; //!< 输入类型。

	/*!
	\brief 构造：使用输入类型对象和路由事件类型。
	*/
	KeyEventArgs(IWidget&, const InputType& = 0, RoutingStrategy = Direct);
};

inline
KeyEventArgs::KeyEventArgs(IWidget& wgt, const InputType& k, RoutingStrategy s)
	: InputEventArgs(wgt, k, s)
{}


/*!
\brief 指针设备输入事件参数类。
\since 早于 build 132 。
*/
struct TouchEventArgs : public InputEventArgs, public MScreenPositionEventArgs
{
public:
	typedef Drawing::Point InputType; //!< 输入类型。

	/*!
	\brief 构造：使用输入类型对象和路由事件类型。
	*/
	TouchEventArgs(IWidget&, const InputType& = InputType::Zero,
		RoutingStrategy = Direct);
};

inline
TouchEventArgs::TouchEventArgs(IWidget& wgt, const InputType& pt,
	RoutingStrategy s)
	: InputEventArgs(wgt, 0, s), MScreenPositionEventArgs(pt)
{}


/*!
\brief 简单事件参数类。

保存部件引用和指定类型值的事件参数。
\since build 268 。
*/
PDefTmplH1(_type)
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


/*
\brief 部件绘制上下文。
\since build 255 。
*/
struct PaintContext
{
	Drawing::Graphics Target; //!< 渲染目标：图形接口上下文。
	Drawing::Point Location; //!< 相对渲染目标的偏移坐标，指定部件左上角的位置。
	Drawing::Rect ClipArea; //!< 相对于图形接口上下文的正则矩形，
		//指定需要保证被刷新的边界区域。

	inline DefDeCtor(PaintContext)
	PaintContext(const Drawing::Graphics&, const Drawing::Point&,
		const Drawing::Rect&);
};

inline
PaintContext::PaintContext(const Drawing::Graphics& g,
	const Drawing::Point& pt, const Drawing::Rect& r)
	: Target(g), Location(pt), ClipArea(r)
{}


/*!
\brief 部件绘制参数。
\since build 242 。
*/
struct PaintEventArgs : public UIEventArgs, public PaintContext
{
	PaintEventArgs(IWidget&);
	PaintEventArgs(IWidget&, const PaintContext&);
	PaintEventArgs(IWidget&, const Drawing::Graphics&, const Drawing::Point&,
		const Drawing::Rect&);
};

inline
PaintEventArgs::PaintEventArgs(IWidget& wgt)
	: UIEventArgs(wgt), PaintContext()
{}

inline
PaintEventArgs::PaintEventArgs(IWidget& wgt, const PaintContext& pc)
	: UIEventArgs(wgt), PaintContext(pc)
{}

inline
PaintEventArgs::PaintEventArgs(IWidget& wgt, const Drawing::Graphics& g,
	const Drawing::Point& pt, const Drawing::Rect& r)
	: UIEventArgs(wgt), PaintContext(g, pt, r)
{}


/*!
\brief 保存旧值的值类型参数类模块模板。
\since build 268 。
*/
PDefTmplH1(_type)
struct GMDoubleValueEventArgs
{
public:
	typedef _type ValueType; //值类型。
	_type Value, OldValue; //值和旧值。

	/*!
	\brief 构造：使用指定值。
	\note 值等于旧值。
	*/
	GMDoubleValueEventArgs(ValueType v)
		: Value(v), OldValue(v)
	{}
	/*!
	\brief 构造：使用指定值和旧值。
	*/
	GMDoubleValueEventArgs(ValueType v, ValueType old_value)
		: Value(v), OldValue(old_value)
	{}
};


//事件处理器类型。
DeclDelegate(HUIEvent, UIEventArgs)
DeclDelegate(HInputEvent, InputEventArgs)
DeclDelegate(HKeyEvent, KeyEventArgs)
DeclDelegate(HTouchEvent, TouchEventArgs)
DeclDelegate(HPaintEvent, PaintEventArgs)
//DefDelegate(HPointEvent, Drawing::Point)
//DefDelegate(HSizeEvent, Size)


#define DefEventTypeMapping(_name, _tEventHandler) \
	template<> \
	struct EventTypeMapping<_name> \
	{ \
		typedef _tEventHandler HandlerType; \
	};


/*!
\brief 标准控件事件空间。
\since build 192 。
*/
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
} VisualEvent;


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
DefEventTypeMapping(TouchUp, HTouchEvent)
DefEventTypeMapping(TouchDown, HTouchEvent)
DefEventTypeMapping(TouchHeld, HTouchEvent)
DefEventTypeMapping(TouchMove, HTouchEvent)
DefEventTypeMapping(Click, HTouchEvent)

DefEventTypeMapping(Paint, HPaintEvent)

DefEventTypeMapping(GotFocus, HUIEvent)
DefEventTypeMapping(LostFocus, HUIEvent)

DefEventTypeMapping(Enter, HTouchEvent)
DefEventTypeMapping(Leave, HTouchEvent)


/*!
\brief 事件映射命名空间。
\since build 242 。
*/
YSL_BEGIN_NAMESPACE(EventMapping)

typedef GEventPointerWrapper<UIEventArgs> MappedType; //!< 映射项类型。
typedef GIHEvent<UIEventArgs> ItemType;
typedef pair<VisualEvent, MappedType> PairType;
typedef map<VisualEvent, MappedType> MapType; //!< 映射表类型。
typedef pair<typename MapType::iterator, bool> SearchResult; \
	//!< 搜索表结果类型。

YSL_END_NAMESPACE(EventMapping)

typedef EventMapping::MapType VisualEventMap;


//! \brief 错误或不存在的部件事件异常。
struct BadEvent
{};


/*!
\brief 控制器抽象类。
\since build 243 。
*/
class AController
{
private:
	bool enabled; //!< 控件可用性。

public:
	/*!
	\brief 构造：使用指定可用性。
	*/
	AController(bool = true);

	DefPred(const ynothrow, Enabled, enabled)

	/*!
	\brief 取事件项。
	*/
	DeclIEntry(EventMapping::ItemType& GetItemRef(const VisualEvent&))

	/*!
	\brief 取事件项，若不存在则用指定函数指针添加。
	\throw std::out_of_range 拒绝加入任何事件项。
	*/
	virtual EventMapping::ItemType&
	GetItemRef(const VisualEvent&, EventMapping::MappedType(&)());

	DefSetter(bool, Enabled, enabled)

	/*
	\brief 复制实例。
	\note 应为抽象方法，但无法使用协变返回类型，所以使用非抽象实现。
	\warning 断言：禁止直接使用。
	*/
	virtual AController*
	Clone();
};

inline
AController::AController(bool b)
	: enabled(b)
{}

inline AController*
AController::Clone()
{
	YAssert(false, "Invalid call @ AController::Clone;");

	return nullptr;
}

inline EventMapping::ItemType&
AController::GetItemRef(const VisualEvent&, EventMapping::MappedType(&)())
{
	throw std::out_of_range("AController::GetItemRef;");
}


template<class _tEventHandler>
size_t
DoEvent(AController& controller, const VisualEvent& id,
	typename _tEventHandler::EventArgsType&& e)
{
	try
	{
		return dynamic_cast<EventT(typename _tEventHandler)&>(
			controller.GetItemRef(id))(std::move(e));
	}
	catch(std::out_of_range&)
	{}
	catch(std::bad_cast&)
	{}
	return 0;
}
template<class _tEventHandler>
inline size_t
DoEvent(AController& controller, const VisualEvent& id,
	typename _tEventHandler::EventArgsType& e)
{
	return DoEvent<_tEventHandler>(controller, id, std::move(e));
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

