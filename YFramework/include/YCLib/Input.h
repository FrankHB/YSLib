/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Input.h
\ingroup YCLib
\brief 平台相关的扩展输入接口。
\version r1177;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 299 。
\par 创建时间:
	2012-04-07 13:37:05 +0800;
\par 修改时间:
	2012-06-04 17:24 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::Input;
*/


#ifndef YCL_INC_INPUT_H_
#define YCL_INC_INPUT_H_

#include "Keys.h"
#include "NativeAPI.h"

namespace platform
{

#if YCL_DS
//! \brief 屏幕指针设备光标信息。
typedef struct CursorInfo : public ::touchPosition
{
	/*!
	\brief 取横坐标。
	\since build 313 。
	*/
	std::uint16_t
	GetX() const
	{
		return px;
	}
	/*!
	\brief 取纵坐标。
	\since build 313 。
	*/
	std::uint16_t
	GetY() const
	{
		return py;
	}
} CursorInfo;

#elif YCL_MINGW32
/*!
\brief 屏幕指针设备光标信息。
\since build 296 。
*/
typedef ::CURSORINFO CursorInfo;
#else
#	error Unsupport platform found!
#endif


/*!
\defgroup YCL_KEY_OVERRIDE YCLib Key Overridden Values
\brief 覆盖键值。
\since build 299 。
*/
//@{
#ifndef YCL_DS
#	define YCL_KEY(X) (#X [0])
#	define YCL_KEY_Start 'P'
#else
#	define YCL_KEY(X) KeyCodes::X
#	define YCL_KEY_Start KeyCodes::Start
#endif
//@}

/*!
\brief 等待任意按键。
*/
void
WaitForInput();

} // namespace platform;

namespace platform_ex
{

/*!
\brief 写入当前指针设备信息。
\since build 272 。
*/
void
WriteCursor(platform::CursorInfo&);


/*!
\brief 按键缓冲。
\since build 299 。
*/
extern platform::KeyInput KeyState, OldKeyState;

/*!
\brief 清除按键缓冲。
\since build 299 。
*/
inline void
ClearKeyStates()
{
	yunseq(KeyState = 0, OldKeyState = 0);
}

/*!
\brief 取键按下状态。
\since build 299 。
*/
inline platform::KeyInput
FetchKeyDownState()
{
	return KeyState &~ OldKeyState;
}

/*!
\brief 取键释放状态。
\since build 299 。
*/
inline platform::KeyInput
FetchKeyUpState()
{
	return (KeyState ^ OldKeyState) & ~KeyState;
}

/*!
\brief 更新按键状态。
\since build 299 。
*/
void
UpdateKeyStates();


#if YCL_DS
/*!
\brief 等待掩码指定的按键。
\since build 298 。
*/
void
WaitForKey(platform::KeyInput);

/*!
\brief 等待任意按键（除触摸屏、翻盖外）。
*/
inline void
WaitForKeypad()
{
	WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R
		| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
		| KEY_START | KEY_SELECT);
}

/*!
\brief 等待任意按键（除 L 、 R 和翻盖外）。
*/
inline void
WaitForFrontKey()
{
	WaitForKey(KEY_TOUCH | KEY_A | KEY_B | KEY_X | KEY_Y
		| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
		| KEY_START | KEY_SELECT);
}

/*!
\brief 等待任意按键（除 L 、 R 、触摸屏和翻盖外）。
*/
inline void
WaitForFrontKeypad()
{
	WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y
		| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
		|KEY_START | KEY_SELECT);
}

/*!
\brief 等待方向键。
*/
inline void
WaitForArrowKey()
{
	WaitForKey(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);
}

/*!
\brief 等待按键 A 、 B 、 X 、 Y 键。
*/
inline void
WaitForABXY()
{
	WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y);
}
#endif

} // namespace platform_ex;

#endif

