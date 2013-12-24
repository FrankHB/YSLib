/*
	© 2012-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Input.h
\ingroup YCLib
\brief 平台相关的扩展输入接口。
\version r411
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 13:37:05 +0800
\par 修改时间:
	2013-12-24 09:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Input
*/


#ifndef YCL_INC_Inupt_h_
#define YCL_INC_Inupt_h_ 1

#include "YModules.h"
#include YFM_YCLib_Keys
#include YFM_YCLib_NativeAPI

namespace platform
{

//! \brief 屏幕指针设备光标信息。
struct YF_API CursorInfo final
#if YCL_DS
	 : public ::touchPosition
#	define YCL_CURSOR_X px
#	define YCL_CURSOR_Y py
#elif YCL_Win32
	 : public ::POINT
#	define YCL_CURSOR_X x
#	define YCL_CURSOR_Y y
#else
#	error "Unsupported platform found."
#endif
{
	/*!
	\brief 转换为二元组。
	\since build 321
	*/
	template<class _tBinary>
	operator _tBinary() const
	{
		return _tBinary(YCL_CURSOR_X, YCL_CURSOR_Y);
	}

	/*!
	\brief 取横坐标。
	\since build 413
	*/
	yconstfn DefGetter(const ynothrow, std::uint16_t, X, YCL_CURSOR_X)
	/*!
	\brief 取纵坐标。
	\since build 413
	*/
	yconstfn DefGetter(const ynothrow, std::uint16_t, Y, YCL_CURSOR_Y)
#undef YCL_CURSOR_Y
#undef YCL_CURSOR_X
};


/*!
\defgroup YCL_KEY_OVERRIDE YCLib Key Overridden Values
\brief 覆盖键值。
\since build 299
*/
//@{
#if !YCL_DS
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
YF_API void
WaitForInput();

} // namespace platform;

namespace platform_ex
{

/*!
\def 指定按键缓冲状态是否以外部对象直接定义。
\note 单线程环境且 KeyBitsetWidth 较小时适合指定直接定义。
\since build 321
*/
#if YF_Multithread // || ...
#	define YCL_KEYSTATE_DIRECT 0
#elif defined(YCL_DS)
#	define YCL_KEYSTATE_DIRECT 1
#endif


/*!
\brief 取按键状态。
\since build 321
*/
YF_API const platform::KeyInput&
FetchKeyState();

/*!
\brief 取上一次更新的按键状态。
\since build 321
*/
YF_API const platform::KeyInput&
FetchOldKeyState();

/*!
\brief 取键按下状态。
\since build 299
*/
YF_API platform::KeyInput
FetchKeyDownState();

/*!
\brief 取键释放状态。
\since build 299
*/
YF_API platform::KeyInput
FetchKeyUpState();

/*!
\brief 清除按键缓冲。
\since build 299
*/
YF_API void
ClearKeyStates();

/*!
\brief 更新按键状态。
\since build 299
*/
YF_API void
UpdateKeyStates();

#if YCL_KEYSTATE_DIRECT
/*!
\brief 按键缓冲。
\since build 299
*/
extern YF_API platform::KeyInput KeyState, OldKeyState;

inline PDefH(const platform::KeyInput&, FetchKeyState, )
	ImplRet(KeyState)

inline PDefH(const platform::KeyInput&, FetchOldKeyState, )
	ImplRet(OldKeyState)

inline PDefH(void, ClearKeyStates, )
	ImplUnseq(KeyState.reset(), OldKeyState.reset())

inline PDefH(platform::KeyInput, FetchKeyDownState, )
	ImplRet(FetchKeyState() &~ FetchOldKeyState())

inline PDefH(platform::KeyInput, FetchKeyUpState, )
	ImplRet((FetchKeyState() ^ FetchOldKeyState()) & ~FetchKeyState())

#endif


/*!
\brief 写入当前指针设备信息。
\since build 272
*/
YF_API void
WriteCursor(platform::CursorInfo&);


#if YCL_DS
/*!
\brief 等待掩码指定的按键。
\since build 298
*/
YF_API void
WaitForKey(platform::KeyInput);

/*!
\brief 等待任意按键（除触摸屏、翻盖外）。
*/
inline PDefH(void, WaitForKeypad, )
	ImplExpr(WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R
		| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
		| KEY_START | KEY_SELECT))

/*!
\brief 等待任意按键（除 L 、 R 和翻盖外）。
*/
inline PDefH(void, WaitForFrontKey, )
	ImplExpr(WaitForKey(KEY_TOUCH | KEY_A | KEY_B | KEY_X | KEY_Y
		| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
		| KEY_START | KEY_SELECT))

/*!
\brief 等待任意按键（除 L 、 R 、触摸屏和翻盖外）。
*/
inline PDefH(void, WaitForFrontKeypad, )
	ImplExpr(WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y
		| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
		|KEY_START | KEY_SELECT))

/*!
\brief 等待方向键。
*/
inline PDefH(void, WaitForArrowKey, )
	ImplExpr(WaitForKey(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN))

/*!
\brief 等待按键 A 、 B 、 X 、 Y 键。
*/
inline PDefH(void, WaitForABXY, )
	ImplExpr(WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y))
#endif

} // namespace platform_ex;

#endif

