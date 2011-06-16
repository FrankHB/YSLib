/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Helper Helper
\ingroup YSLib
\brief YSLib 助手模块。
*/

/*!	\file yglobal.h
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version 0.2362;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:14:57 +0800;
\par 修改时间:
	2011-06-16 14:10 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#ifndef YSL_INC_HELPER_YGLOBAL_H_
#define YSL_INC_HELPER_YGLOBAL_H_

#include "../Core/ygdibase.h"
#include "../Core/yapp.h"
#include "../Core/ydevice.h"

YSL_BEGIN

/*!	\defgroup CustomGlobalConstants Custom Global Constants
\ingroup GlobalObjects
\brief 平台相关的全局常量。
*/
//@{
//! \brief 屏幕大小。
const SDst MainScreenWidth(SCREEN_WIDTH), MainScreenHeight(SCREEN_HEIGHT);
//@}

/*!	\defgroup CustomGlobalVariables Custom Global Variables
\ingroup GlobalObjects
\brief 平台相关的全局变量。
*/
//@{
//@}


YSL_BEGIN_NAMESPACE(Devices)

// DS 屏幕。
class DSScreen : public Screen
{
public:
	typedef int BGType;

private:
	BGType bg;

public:
	/*!
	\brief 构造：指定宽度和高度，从指定缓冲区指针。
	*/
	DSScreen(SDst, SDst, Drawing::BitmapPtr = nullptr);

	/*!
	\brief 复位。
	\note 无条件初始化。
	*/
	static void
	Reset();

	/*!
	\brief 取指针。
	\note 无异常抛出。
	\note 进行状态检查。
	*/
	virtual Drawing::BitmapPtr
	GetCheckedBufferPtr() const ynothrow;
	DefGetter(const BGType&, BgID, bg)

	/*!
	\brief 更新。
	\note 复制到屏幕。
	*/
	void
	Update(Drawing::BitmapPtr);
	/*!
	\brief 更新。
	\note 以纯色填充屏幕。
	*/
	void
	Update(Drawing::Color = 0);
};

YSL_END_NAMESPACE(Devices)


using Devices::DSScreen;


/*!
\brief 平台相关的应用程序类。
\note 含默认接口。
*/
class YDSApplication : public YApplication
{
	friend YDSApplication&
	FetchGlobalInstance() ynothrow;

private:
	shared_ptr<DSScreen> hScreenUp; //<! DS 上屏幕句柄。
	shared_ptr<DSScreen> hScreenDown; //<! DS 上屏幕句柄。
	shared_ptr<Desktop> hDesktopUp; //<! DS 下屏幕默认桌面句柄。
	shared_ptr<Desktop> hDesktopDown; //<! DS 下屏幕默认桌面句柄。

	/*!
	\brief \c private 构造函数：非内联。

	\note 通过友元单例实现进程唯一性语义
	*/
	YDSApplication();

public:
	DefGetter(const shared_ptr<DSScreen>&, ScreenUpHandle, hScreenUp)
	DefGetter(const shared_ptr<DSScreen>&, ScreenDownHandle, hScreenDown)
	DefGetter(const shared_ptr<Desktop>&, DesktopUpHandle, hDesktopUp)
	DefGetter(const shared_ptr<Desktop>&, DesktopDownHandle, hDesktopDown)
	/*!
	\brief 取上屏幕。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	DSScreen&
	GetScreenUp() const ynothrow;
	/*!
	\brief 取下屏幕。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	DSScreen&
	GetScreenDown() const ynothrow;
	/*!
	\brief 取上桌面。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	Desktop&
	GetDesktopUp() const ynothrow;
	/*!
	\brief 取下桌面。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	Desktop&
	GetDesktopDown() const ynothrow;
	/*!
	\brief 取默认屏幕。
	\note 无异常抛出。
	*/
	PDefH0(DSScreen&, GetDefaultScreen)
		ImplRet(GetScreenUp())
	/*!
	\brief 取默认桌面。
	\note 默认接口：可能会被 YSLib 本体使用。
	\note 无异常抛出。
	*/
	PDefH0(Desktop&, GetDefaultDesktop)
		ImplRet(GetDesktopUp())
	/*!
	\brief 取触摸屏所在桌面。
	\note 默认接口：可能会被 YSLib 本体使用。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	PDefH0(Desktop&, GetTouchableDesktop)
		ImplRet(GetDesktopDown())

	/*!
	\brief 初始化设备。
	\note 无异常抛出。
	*/
	void
	InitializeDevices() ynothrow;

	/*!
	\brief 释放设备。
	\note 无异常抛出。
	*/
	void
	ReleaseDevices() ynothrow;
};

inline DSScreen&
YDSApplication::GetScreenUp() const ynothrow
{
	YAssert(is_valid(hScreenUp), "Fatal error:"
		" invalid screen handle found @ YDSApplication::GetScreenUp;");

	return *hScreenUp;
}
inline DSScreen&
YDSApplication::GetScreenDown() const ynothrow
{
	YAssert(is_valid(hScreenDown), "Fatal error:"
		" invalid screen handle found @ YDSApplication::GetScreenDown;");

	return *hScreenDown;
}
inline Desktop&
YDSApplication::GetDesktopUp() const ynothrow
{
	YAssert(is_valid(hDesktopUp), "Fatal error:"
		" invalid desktop handle found @ YDSApplication::GetDesktopUp;");

	return *hDesktopUp;
}
inline Desktop&
YDSApplication::GetDesktopDown() const ynothrow
{
	YAssert(is_valid(hDesktopDown), "Fatal error:"
		" invalid desktop handle found @ YDSApplication::GetDesktopDown;");

	return *hDesktopDown;
}


/*!
\brief 取平台相关的全局资源。
\note 无异常抛出。
*/
YDSApplication&
FetchGlobalInstance() ynothrow;

YSL_BEGIN_NAMESPACE(Messaging)

//! \brief 输入消息内容。
class InputContent
{
public:
	Runtime::KeysInfo Key;
	Drawing::Point CursorLocation;

	explicit
	InputContent(Runtime::KeysInfo, const Drawing::Point&);

	bool
	operator==(const InputContent&) const;
};

inline
InputContent::InputContent(Runtime::KeysInfo k, const Drawing::Point& pt)
	: Key(k), CursorLocation(pt)
{}

DefMessageTarget(SM_INPUT, shared_ptr<InputContent>)

YSL_END_NAMESPACE(Messaging)


/*!
\brief 默认消息发生函数。
*/
void
Idle();

/*!
\brief 以指定前景色和背景色初始化指定屏幕的控制台。
*/
bool
InitConsole(Devices::Screen&, Drawing::PixelType, Drawing::PixelType);

//全局函数。

void
ShowFatalError(const char*);

/*!	\defgroup HelperFunction Helper Function
\brief 助手功能/函数。

仅帮助简化编码形式，并不包含编译期之后逻辑功能实现的代码设施。
*/

YSL_END

#endif

