// YSLib::Core::YEventArgs by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-8 13:21:10;
// UTime = 2010-6-12 14:37;
// Version = 0.1752;


#ifndef INCLUDED_YEVTARG_H_
#define INCLUDED_YEVTARG_H_

// YEventArgs ：事件参数模块。

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
struct YScreenPositionEventArgs : public YEventArgs, public Drawing::SPoint
{
	static const YScreenPositionEventArgs Empty;

	explicit
	YScreenPositionEventArgs(const Drawing::SPoint& pt = Drawing::SPoint::Zero)
	: YEventArgs(), SPoint(pt)
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
struct YTouchEventArgs : public YScreenPositionEventArgs, public MInputEventArgs
{
	typedef Drawing::SPoint InputType;

	static const YTouchEventArgs Empty;

	YTouchEventArgs(const InputType& pt = InputType::Zero)
	: YScreenPositionEventArgs(pt), MInputEventArgs()
	{}
};


//键盘输入事件参数类。
struct YKeyEventArgs : public YEventArgs, public MInputEventArgs
{
	typedef Keys InputType;

	static const YKeyEventArgs Empty;

	YKeyEventArgs(const InputType& k = InputType::Empty)
	: MInputEventArgs(k)
	{}
};

YSL_END_NAMESPACE(Runtime)

YSL_END

#endif

