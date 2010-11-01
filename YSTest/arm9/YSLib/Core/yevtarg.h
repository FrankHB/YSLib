// YSLib::Core::YEventArgs by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-06-08 13:21:10 + 08:00;
// UTime = 2010-11-01 13:54 + 08:00;
// Version = 0.1945;


#ifndef INCLUDED_YEVTARG_H_
#define INCLUDED_YEVTARG_H_

// EventArgs ：事件参数模块。

#include "yobject.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

//********************************
//名称:		ToSPoint
//全名:		YSLib::Runtime::ToSPoint
//可访问性:	public 
//返回类型:	Drawing::Point
//修饰符:	
//形式参数:	const CursorInfo & c
//功能概要:	转换指针设备光标位置为屏幕点。
//备注:		
//********************************
inline Drawing::Point
ToSPoint(const CursorInfo& c)
{
	return Drawing::Point(c.GetX(), c.GetY());
}


//屏幕事件参数类。
struct ScreenPositionEventArgs : public EventArgs, public Drawing::Point
{
	static const ScreenPositionEventArgs Empty;

	//********************************
	//名称:		ScreenPositionEventArgs
	//全名:		YSLib::Runtime::ScreenPositionEventArgs
	//				::ScreenPositionEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Drawing::Point &
	//功能概要:	构造：使用指定点。
	//备注:		
	//********************************
	explicit
	ScreenPositionEventArgs(const Drawing::Point& = Drawing::Point::Zero);
};

inline
ScreenPositionEventArgs::ScreenPositionEventArgs(const Drawing::Point& pt)
	: EventArgs(), Point(pt)
{}


//输入事件参数类。
struct InputEventArgs
{
public:
	static const InputEventArgs Empty;

	typedef platform::Key Key;

	Key k;

	//********************************
	//名称:		InputEventArgs
	//全名:		YSLib::Runtime::InputEventArgs::InputEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Key &
	//功能概要:	构造：使用本机按键对象。
	//备注:		
	//********************************
	InputEventArgs(const Key& = 0);

	DefConverter(Key, k)

	DefGetter(Key, Key, k)
};

inline
InputEventArgs::InputEventArgs(const Key& k)
	: k(k)
{}


//指针设备输入事件参数类。
struct TouchEventArgs : public ScreenPositionEventArgs,
	public InputEventArgs
{
	typedef Drawing::Point InputType; //输入类型。

	static const TouchEventArgs Empty;

	//********************************
	//名称:		TouchEventArgs
	//全名:		YSLib::Runtime::TouchEventArgs::TouchEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const InputType &
	//功能概要:	构造：使用输入类型对象。
	//备注:		
	//********************************
	TouchEventArgs(const InputType& = InputType::Zero);
};

inline
TouchEventArgs::TouchEventArgs(const InputType& pt)
	: ScreenPositionEventArgs(pt), InputEventArgs()
{}


//键盘输入事件参数类。
struct KeyEventArgs : public EventArgs, public InputEventArgs
{
	typedef Key InputType; //输入类型。

	static const KeyEventArgs Empty;

	//********************************
	//名称:		KeyEventArgs
	//全名:		YSLib::Runtime::KeyEventArgs::KeyEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const InputType &
	//功能概要:	构造：使用输入类型对象。
	//备注:		
	//********************************
	KeyEventArgs(const InputType& = 0);
};

inline
KeyEventArgs::KeyEventArgs(const InputType& k)
	: InputEventArgs(k)
{}

YSL_END_NAMESPACE(Runtime)

YSL_END

#endif

