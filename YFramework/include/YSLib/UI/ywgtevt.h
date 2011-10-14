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
\version r1910;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-12-17 10:27:50 +0800;
\par 修改时间:
	2011-09-18 01:26 +0800;
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


struct PaintEventArgs : public EventArgs
{
	Drawing::Graphics Target; //!< 渲染目标：图形接口上下文。
	Drawing::Point Location; //!< 相对渲染目标的偏移坐标，指定部件左上角的位置。
	Drawing::Rect ClipArea; //!< 相对于图形接口上下文的正则矩形，
		//指定需要保证被刷新的边界区域。

	inline DefDeCtor(PaintEventArgs)
	PaintEventArgs(const Drawing::Graphics&, const Drawing::Point&,
		const Drawing::Rect&);
};

inline
PaintEventArgs::PaintEventArgs(const Drawing::Graphics& g,
	const Drawing::Point& pt, const Drawing::Rect& r)
	: Target(g), Location(pt), ClipArea(r)
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
DefDelegate(HPaintEvent, IWidget, PaintEventArgs)
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

DefEventTypeMapping(Paint, HPaintEvent)

DefEventTypeMapping(GotFocus, HVisualEvent)
DefEventTypeMapping(LostFocus, HVisualEvent)

DefEventTypeMapping(Enter, HTouchEvent)
DefEventTypeMapping(Leave, HTouchEvent)


//! \brief 事件映射命名空间。
YSL_BEGIN_NAMESPACE(EventMapping)

typedef GIHEvent<IWidget, EventArgs> ItemType;
typedef GEventPointerWrapper<IWidget, EventArgs> MappedType; //!< 映射项类型。
typedef pair<VisualEvent, MappedType> PairType;
typedef map<VisualEvent, MappedType> MapType; //!< 映射表类型。
typedef pair<typename MapType::iterator, bool> SearchResult; \
	//!< 搜索表结果类型。

YSL_END_NAMESPACE(EventMapping)

typedef EventMapping::MapType VisualEventMap;


//! \brief 错误或不存在的部件事件异常。
struct BadEvent
{};


//! \brief 控制器抽象类。
class AController
{
private:
	bool enabled; //!< 控件可用性。

public:
	AController(bool);

	DefPredicate(Enabled, enabled)

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
	typename _tEventHandler::SenderType& sender,
	typename _tEventHandler::EventArgsType&& e)
{
	try
	{
		return dynamic_cast<typename GSEvent<_tEventHandler>::EventType&>(
			controller.GetItemRef(id))(sender, std::move(e));
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
	typename _tEventHandler::SenderType& sender,
	typename _tEventHandler::EventArgsType& e)
{
	return DoEvent<_tEventHandler>(controller, id, sender, std::move(e));
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

