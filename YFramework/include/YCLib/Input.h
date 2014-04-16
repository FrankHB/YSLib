/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Input.h
\ingroup YCLib
\brief 平台相关的扩展输入接口。
\version r491
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 13:37:05 +0800
\par 修改时间:
	2014-04-14 13:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Input
*/


#ifndef YCL_INC_Inupt_h_
#define YCL_INC_Inupt_h_ 1

#include "YModules.h"
#include YFM_YCLib_Keys
#include YFM_YCLib_Video // for SPos;

namespace platform
{

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

#if YCL_Android
//! \since build 493
struct AInputEvent;
#endif

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


#if YCL_DS
/*!
\brief 取当前指针设备光标位置。
\since build 481
*/
YF_API std::pair<std::int16_t, std::int16_t>
FetchCursor();


/*!
\brief 等待掩码指定的按键。
\since build 298
*/
YF_API void
WaitForKey(platform::KeyInput);

/*!
\brief 等待任意按键（除触摸屏、翻盖外）。
*/
YF_API void
WaitForKeypad();

/*!
\brief 等待任意按键（除 L 、 R 和翻盖外）。
*/
YF_API void
WaitForFrontKey();

/*!
\brief 等待任意按键（除 L 、 R 、触摸屏和翻盖外）。
*/
YF_API void
WaitForFrontKeypad();

/*!
\brief 等待方向键。
*/
YF_API void
WaitForArrowKey();

/*!
\brief 等待按键 A 、 B 、 X 、 Y 键。
*/
YF_API void
WaitForABXY();
#elif YCL_Android
/*!
\brief 取当前指针设备光标位置。
\since build 493
*/
YF_API std::pair<float, float>
FetchCursor();

/*!
\brief 记录输入事件。
\sa UpdateKeyStates
\since build 493
*/
void
SaveInput(const ::AInputEvent&);
#endif

} // namespace platform_ex;

#endif

