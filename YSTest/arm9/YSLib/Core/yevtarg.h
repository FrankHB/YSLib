// YSLib::Core::YEventArgs by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-06-08 13:21:10 + 08:00;
// UTime = 2010-10-21 11:25 + 08:00;
// Version = 0.1868;


#ifndef INCLUDED_YEVTARG_H_
#define INCLUDED_YEVTARG_H_

// MEventArgs ：事件参数模块。

#include "yobject.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

//转换指针设备光标位置为屏幕点。
inline Drawing::Point
ToSPoint(const CursorInfo& c)
{
	return Drawing::Point(c.GetX(), c.GetY());
}


//屏幕事件参数类。
struct MScreenPositionEventArgs : public MEventArgs, public Drawing::Point
{
	static const MScreenPositionEventArgs Empty;

	explicit
	MScreenPositionEventArgs(const Drawing::Point& = Drawing::Point::Zero);
};

inline
MScreenPositionEventArgs::MScreenPositionEventArgs(const Drawing::Point& pt)
	: MEventArgs(), Point(pt)
{}


//输入事件参数模块类。
struct MInputEventArgs
{
public:
	static const MInputEventArgs Empty;

	typedef platform::Key Key;

	Key k;

	MInputEventArgs(const Key& = 0);

	DefConverter(Key, k)

	DefGetter(Key, Key, k)
};

inline
MInputEventArgs::MInputEventArgs(const Key& k)
	: k(k)
{}


//指针设备输入事件参数类。
struct MTouchEventArgs : public MScreenPositionEventArgs, public MInputEventArgs
{
	typedef Drawing::Point InputType;

	static const MTouchEventArgs Empty;

	MTouchEventArgs(const InputType& = InputType::Zero);
};

inline
MTouchEventArgs::MTouchEventArgs(const InputType& pt)
	: MScreenPositionEventArgs(pt), MInputEventArgs()
{}


//键盘输入事件参数类。
struct MKeyEventArgs : public MEventArgs, public MInputEventArgs
{
	typedef Key InputType;

	static const MKeyEventArgs Empty;

	MKeyEventArgs(const InputType& = 0);
};

inline
MKeyEventArgs::MKeyEventArgs(const InputType& k)
	: MInputEventArgs(k)
{}

YSL_END_NAMESPACE(Runtime)

YSL_END

#endif

