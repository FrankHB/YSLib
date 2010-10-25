// YSLib::Core::YEventArgs by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-06-08 13:21:10 + 08:00;
// UTime = 2010-10-24 19:56 + 08:00;
// Version = 0.1929;


#ifndef INCLUDED_YEVTARG_H_
#define INCLUDED_YEVTARG_H_

// MEventArgs ：事件参数模块。

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
struct MScreenPositionEventArgs : public MEventArgs, public Drawing::Point
{
	static const MScreenPositionEventArgs Empty;

	//********************************
	//名称:		MScreenPositionEventArgs
	//全名:		YSLib::Runtime::MScreenPositionEventArgs
	//				::MScreenPositionEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Drawing::Point &
	//功能概要:	构造：使用指定点。
	//备注:		
	//********************************
	explicit
	MScreenPositionEventArgs(const Drawing::Point& = Drawing::Point::Zero);
};

inline
MScreenPositionEventArgs::MScreenPositionEventArgs(const Drawing::Point& pt)
	: MEventArgs(), Point(pt)
{}


//输入事件参数类。
struct MInputEventArgs
{
public:
	static const MInputEventArgs Empty;

	typedef platform::Key Key;

	Key k;

	//********************************
	//名称:		MInputEventArgs
	//全名:		YSLib::Runtime::MInputEventArgs::MInputEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Key &
	//功能概要:	构造：使用本机按键对象。
	//备注:		
	//********************************
	MInputEventArgs(const Key& = 0);

	DefConverter(Key, k)

	DefGetter(Key, Key, k)
};

inline
MInputEventArgs::MInputEventArgs(const Key& k)
	: k(k)
{}


//指针设备输入事件参数类。
struct MTouchEventArgs : public MScreenPositionEventArgs,
	public MInputEventArgs
{
	typedef Drawing::Point InputType; //输入类型。

	static const MTouchEventArgs Empty;

	//********************************
	//名称:		MTouchEventArgs
	//全名:		YSLib::Runtime::MTouchEventArgs::MTouchEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const InputType &
	//功能概要:	构造：使用输入类型对象。
	//备注:		
	//********************************
	MTouchEventArgs(const InputType& = InputType::Zero);
};

inline
MTouchEventArgs::MTouchEventArgs(const InputType& pt)
	: MScreenPositionEventArgs(pt), MInputEventArgs()
{}


//键盘输入事件参数类。
struct MKeyEventArgs : public MEventArgs, public MInputEventArgs
{
	typedef Key InputType; //输入类型。

	static const MKeyEventArgs Empty;

	//********************************
	//名称:		MKeyEventArgs
	//全名:		YSLib::Runtime::MKeyEventArgs::MKeyEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const InputType &
	//功能概要:	构造：使用输入类型对象。
	//备注:		
	//********************************
	MKeyEventArgs(const InputType& = 0);
};

inline
MKeyEventArgs::MKeyEventArgs(const InputType& k)
	: MInputEventArgs(k)
{}

YSL_END_NAMESPACE(Runtime)

YSL_END

#endif

