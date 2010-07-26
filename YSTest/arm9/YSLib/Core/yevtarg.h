// YSLib::Core::YEventArgs by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-8 13:21:10;
// UTime = 2010-6-26 7:38;
// Version = 0.1792;


#ifndef INCLUDED_YEVTARG_H_
#define INCLUDED_YEVTARG_H_

// MEventArgs ：事件参数模块。

#include "yobject.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

//转换指针设备光标位置为屏幕点。
inline Drawing::SPoint
ToSPoint(const CursorInfo& c)
{
	return Drawing::SPoint(c.GetX(), c.GetY());
}


//屏幕事件参数类。
struct MScreenPositionEventArgs : public MEventArgs, public Drawing::SPoint
{
	static const MScreenPositionEventArgs Empty;

	explicit
	MScreenPositionEventArgs(const Drawing::SPoint& pt = Drawing::SPoint::Zero)
	: MEventArgs(), SPoint(pt)
	{}
};


//输入事件参数模块类。
struct MInputEventArgs : public Keys
{
	static const MInputEventArgs Empty;

	MInputEventArgs(const Keys& k = 0)
	: Keys(k)
	{}
};


//指针设备输入事件参数类。
struct MTouchEventArgs : public MScreenPositionEventArgs, public MInputEventArgs
{
	typedef Drawing::SPoint InputType;

	static const MTouchEventArgs Empty;

	MTouchEventArgs(const InputType& pt = InputType::Zero)
	: MScreenPositionEventArgs(pt), MInputEventArgs()
	{}
};


//键盘输入事件参数类。
struct MKeyEventArgs : public MEventArgs, public MInputEventArgs
{
	typedef Keys InputType;

	static const MKeyEventArgs Empty;

	MKeyEventArgs(const InputType& k = InputType::Empty)
	: MInputEventArgs(k)
	{}
};

YSL_END_NAMESPACE(Runtime)

YSL_END

#endif

