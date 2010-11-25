/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.h
\ingroup Shell
\brief 平台无关的控件实现。
\version 0.4421;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:24 + 08:00;
\par 修改时间:
	2010-11-24 09:50 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YControl;
*/


#ifndef INCLUDED_YCONTROL_H_
#define INCLUDED_YCONTROL_H_

#include "ywidget.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

using namespace Drawing;

//! \brief 屏幕事件参数类。
struct ScreenPositionEventArgs : public EventArgs, public Drawing::Point
{
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
	static InputEventArgs Empty;

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


//! \brief 指针设备输入事件参数类。
struct TouchEventArgs : public ScreenPositionEventArgs,
	public InputEventArgs
{
	typedef Drawing::Point InputType; //!< 输入类型。

	static TouchEventArgs Empty;

	/*!
	\brief 构造：使用输入类型对象。
	*/
	TouchEventArgs(const InputType& = InputType::Zero);
};

inline
TouchEventArgs::TouchEventArgs(const InputType& pt)
	: ScreenPositionEventArgs(pt), InputEventArgs()
{}


//! \brief 键盘输入事件参数类。
struct KeyEventArgs : public EventArgs, public InputEventArgs
{
	typedef Key InputType; //!< 输入类型。

	static KeyEventArgs Empty;

	/*!
	\brief 构造：使用输入类型对象。
	*/
	KeyEventArgs(const InputType& = 0);
};

inline
KeyEventArgs::KeyEventArgs(const InputType& k)
	: InputEventArgs(k)
{}


//! \brief 控件事件参数类型。
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


//! \brief 控件事件类型。
typedef Runtime::GEvent<true, IControl, EventArgs> YControlEvent;


//事件处理器类型。
DefDelegate(InputEventHandler, IVisualControl, InputEventArgs)
DefDelegate(KeyEventHandler, IVisualControl, KeyEventArgs)
DefDelegate(TouchEventHandler, IVisualControl, TouchEventArgs)
DefDelegate(IndexEventHandler, IVisualControl, IndexEventArgs)
DefDelegate(PointEventHandler, IVisualControl, Drawing::Point)
DefDelegate(SizeEventHandler, IVisualControl, Size)


//! \brief 可视控件事件空间。
struct EControl
{
	typedef enum EventSpace
	{
	//	AutoSizeChanged,
	//	BackColorChanged,
	//	ForeColorChanged,
	//	LocationChanged,
	//	MarginChanged,
	//	VisibleChanged,

	//	EnabledChanged,

		GotFocus,
		LostFocus,

	//	TextChanged,
	//	FontChanged,
	//	FontColorChanged,
	//	FontSizeChanged,
	} EventID;
};


//! \brief 控件接口。
DeclInterface(IControl)
	typedef Runtime::GEventMap<EControl, YControlEvent> EventMapType;

	DeclIEntry(EventMapType::Event& operator[](const EventMapType::ID&))

	DeclIEntry(bool IsEnabled() const) //!< 判断是否有效。

	DeclIEntry(void SetEnabled(bool)) //!< 设置有效性。
EndDecl


//! \brief 可视控件接口。
DeclBasedInterface(IVisualControl, virtual IWidget, virtual IControl,
	virtual GIFocusRequester<GMFocusResponser, IVisualControl>)
	DeclIEventEntry(InputEventHandler, Enter) //!< 控件进入。
	DeclIEventEntry(InputEventHandler, Leave) //!< 控件离开。
	DeclIEventEntry(KeyEventHandler, KeyUp) //!< 键接触结束。
	DeclIEventEntry(KeyEventHandler, KeyDown) //!< 键接触开始。
	DeclIEventEntry(KeyEventHandler, KeyHeld) //!< 键接触保持。
	DeclIEventEntry(KeyEventHandler, KeyPress) //!< 键按下。
	DeclIEventEntry(TouchEventHandler, TouchUp) //!< 屏幕接触结束。
	DeclIEventEntry(TouchEventHandler, TouchDown) //!< 屏幕接触开始。
	DeclIEventEntry(TouchEventHandler, TouchHeld) //!< 屏幕接触保持。
	DeclIEventEntry(TouchEventHandler, TouchMove) //!< 屏幕接触移动。
	DeclIEventEntry(TouchEventHandler, Click) //!< 屏幕点击。

	//! \brief 向部件容器请求获得焦点，
	DeclIEntry(void RequestFocus(EventArgs&))

	//! \brief 释放焦点。
	DeclIEntry(void ReleaseFocus(EventArgs&))
EndDecl


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


//! \brief 控件模块类。
class Control// : implements IControl
{
private:
	bool enabled; //!< 控件有效性。

protected:
	IControl::EventMapType EventMap; //!< 事件映射表。

public:
	/*!
	\brief 构造：使用有效性。
	*/
	explicit
	Control(bool = true);

	virtual DefEmptyDtor(Control)

	virtual PDefHOperator(IControl::EventMapType::Event&, [],
		const IControl::EventMapType::ID& id)
		ImplRet(EventMap[id])

	virtual DefPredicate(Enabled, enabled)

	virtual DefSetter(bool, Enabled, enabled)
};

inline
Control::Control(bool e)
	: enabled(e), EventMap()
{}


//! \brief 可视控件模块类。
class MVisualControl : public Control, public AFocusRequester
{
public:
	//可视控件标准事件。
	DefEvent(InputEventHandler, Enter)
	DefEvent(InputEventHandler, Leave)
	DefEvent(KeyEventHandler, KeyUp)
	DefEvent(KeyEventHandler, KeyDown)
	DefEvent(KeyEventHandler, KeyHeld)
	DefEvent(KeyEventHandler, KeyPress)
	DefEvent(TouchEventHandler, TouchUp)
	DefEvent(TouchEventHandler, TouchDown)
	DefEvent(TouchEventHandler, TouchHeld)
	DefEvent(TouchEventHandler, TouchMove)
	DefEvent(TouchEventHandler, Click)
	//可视控件扩展事件。
	DefEvent(PointEventHandler, Move) //!< 可视控件移动。
	DefEvent(SizeEventHandler, Resize) //!< 可视控件大小调整。

	explicit
	MVisualControl();
	virtual DefEmptyDtor(MVisualControl)
};


//! \brief 控件基类。
class YControl : public YComponent, public Control
{
public:
	typedef YComponent ParentType;

	virtual DefEmptyDtor(YControl)
};


//! \brief 可视控件抽象基类。
class AVisualControl : public Widgets::Widget, public MVisualControl,
	virtual implements IVisualControl
{
public:
	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	AVisualControl(const Rect& = Rect::FullScreen, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	~AVisualControl() ythrow();

	ImplI(IVisualControl)
		PDefHOperator(EventMapType::Event&, [], const EventMapType::ID& id)
		ImplBodyBase(MVisualControl, operator[], id)

	ImplI(IVisualControl) DefPredicateBase(Visible, Visual)
	ImplI(IVisualControl) DefPredicateBase(Transparent, Visual)
	ImplI(IVisualControl) DefPredicateBase(BgRedrawed, Visual)
	ImplI(IVisualControl) DefPredicateBase(Enabled, Control)
	ImplI(IVisualControl) DefPredicateBase(Focused, AFocusRequester)
	ImplI(IVisualControl)
		PDefH(bool, IsFocusOfContainer,
		GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)

	ImplI(IVisualControl)
		PDefH(bool, CheckRemoval, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	ImplI(IVisualControl) DefGetterBase(const Point&, Location, Visual)
	ImplI(IVisualControl) DefGetterBase(const Size&, Size, Visual)
	ImplI(IVisualControl)
		DefGetterBase(IUIBox*, ContainerPtr, Widget)

	ImplI(IVisualControl) DefEventGetter(InputEventHandler, Enter)
	ImplI(IVisualControl) DefEventGetter(InputEventHandler, Leave)
	ImplI(IVisualControl) DefEventGetter(KeyEventHandler, KeyUp)
	ImplI(IVisualControl) DefEventGetter(KeyEventHandler, KeyDown)
	ImplI(IVisualControl) DefEventGetter(KeyEventHandler, KeyHeld)
	ImplI(IVisualControl) DefEventGetter(KeyEventHandler, KeyPress)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, TouchUp)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, TouchDown)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, TouchHeld)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, TouchMove)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, Click)

	ImplI(IVisualControl) DefSetterBase(bool, Visible, Visual)
	ImplI(IVisualControl) DefSetterBase(bool, Transparent, Visual)
	ImplI(IVisualControl) DefSetterBase(bool, BgRedrawed, Visual)
	ImplI(IVisualControl) DefSetterBase(const Point&, Location, Visual)
	ImplI(IVisualControl) DefSetterBase(bool, Enabled, Control)

	ImplI(IVisualControl) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(Widget, DrawBackground)

	ImplI(IVisualControl) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(Widget, DrawForeground)

	ImplI(IVisualControl) PDefH(void, Refresh)
		ImplBodyBaseVoid(Widget, Refresh)

	ImplA(IVisualControl)
	DeclIEntry(void RequestToTop())

	/*!
	\brief 向部件容器请求获得焦点，若成功则引发 GotFocus 事件。
	*/
	ImplI(IVisualControl) void
	RequestFocus(EventArgs&);

	/*!
	\brief 释放焦点，并引发失去焦点事件。
	*/
	ImplI(IVisualControl) void
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

inline void
AVisualControl::OnLostFocus(EventArgs& e)
{
	OnGotFocus(e);
}


//! \brief 可视控件基类。
class YVisualControl : public YComponent, public AVisualControl
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	YVisualControl(const Rect& = Rect::FullScreen, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	~YVisualControl() ythrow();

	/*!
	\brief 请求提升至容器顶端。
	\note 空实现。
	*/
	ImplI(IVisualControl) void
	RequestToTop()
	{}
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

