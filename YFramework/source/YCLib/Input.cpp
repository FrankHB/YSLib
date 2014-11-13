/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Input.cpp
\ingroup YCLib
\brief 平台相关的扩展输入接口。
\version r465
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 13:38:36 +0800
\par 修改时间:
	2014-11-12 05:01 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Input
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Input
#include YFM_YCLib_NativeAPI
#include YFM_YCLib_Mutex
#include YFM_YCLib_Debug
#if	YCL_Android
#	include <android/input.h>
#endif

//! \since build 551
using namespace platform::Concurrency;
//! \since build 553
using platform::Deref;

namespace platform
{

void
WaitForInput()
{
	while(true)
	{
		platform_ex::UpdateKeyStates();
		if(platform_ex::FetchKeyDownState().any())
			break;
#if YCL_DS
		::swiWaitForVBlank();
#endif
	}
}

} // namespace platform;

namespace platform_ex
{

//! \since build 551
namespace
{

mutex CompKeyMutex;
mutex KeyMutex;

} // unnamed namespace;

#if YCL_KEYSTATE_DIRECT
platform::KeyInput KeyState, OldKeyState;
#else
namespace
{

//! \since build 321
//@{
platform::KeyInput KeyStateA;
//! \since build 489
platform::KeyInput KeyStateB;
platform::KeyInput *pKeyState(&KeyStateA), *pOldKeyState(&KeyStateB);
//@}
#if YCL_Android
//! \since build 513
platform::KeyInput KeyStateBuffer;
//! \since build 493
mutex CursorMutex;
//! \since build 493
float LastCursorPosX, LastCursorPosY;
#endif

//! \since build 493
inline platform::KeyInput&
FetchKeyStateRef()
{
	return Deref(pKeyState);
}

//! \since build 493
inline platform::KeyInput&
FetchOldKeyStateRef()
{
	return Deref(pOldKeyState);
}

//! \since build 492
//@{
inline platform::KeyInput
FetchKeyDownStateRaw()
{
	return FetchKeyStateRef() & ~FetchOldKeyStateRef();
}

inline platform::KeyInput
FetchKeyUpStateRaw()
{
	return (FetchKeyStateRef() ^ FetchOldKeyStateRef()) & ~FetchKeyStateRef();
}
//@}

} //unnamed namespace;

const platform::KeyInput&
FetchKeyState()
{
	lock_guard<mutex> lck(KeyMutex);

	return FetchKeyStateRef();
}

const platform::KeyInput&
FetchOldKeyState()
{
	lock_guard<mutex> lck(KeyMutex);

	return FetchOldKeyStateRef();
}

platform::KeyInput
FetchKeyDownState()
{
	lock_guard<mutex> comp_lck(CompKeyMutex);

	return FetchKeyDownStateRaw();
}

platform::KeyInput
FetchKeyUpState()
{
	lock_guard<mutex> comp_lck(CompKeyMutex);

	return FetchKeyUpStateRaw();
}

void
ClearKeyStates()
{
	lock_guard<mutex> comp_lck(CompKeyMutex);
	lock_guard<mutex> lck(KeyMutex);

	yunseq(Deref(pKeyState).reset(), Deref(pOldKeyState).reset());
}

#endif

void
UpdateKeyStates()
{
	lock_guard<mutex> comp_lck(CompKeyMutex);
	lock_guard<mutex> lck(KeyMutex);

#if YCL_KEYSTATE_DIRECT
	OldKeyState = KeyState;
#else
	std::swap(pKeyState, pOldKeyState);
#endif
#if YCL_DS
	KeyState = ::keysCurrent();
#elif YCL_Win32
	// NOTE: 0x00 and 0xFF should be invalid.
	for(std::size_t i(1); i < platform::KeyBitsetWidth - 1; ++i)
		pKeyState->set(i, ::GetAsyncKeyState(i) & 0x8000);
#elif YCL_Android
	FetchKeyStateRef() = KeyStateBuffer;
#endif
}


#if YCL_DS
std::pair<std::int16_t, std::int16_t>
FetchCursor()
{
	using pr_type = std::pair<std::int16_t, std::int16_t>;
	::touchPosition tp;

	::touchRead(&tp);
	// NOTE: (-1, -1) is %YSLib::Point::Invalid.
	return YB_LIKELY(tp.px != 0 && tp.py != 0) ? pr_type(tp.px - 1, tp.py - 1)
		: pr_type(-1, -1);
}


void
WaitForKey(platform::KeyInput mask)
{
	while(true)
	{
		UpdateKeyStates();
		if((FetchKeyDownState() & mask).any())
			break;
		swiWaitForVBlank();
	}
}

void
WaitForFrontKey()
{
	return WaitForKey(KEY_TOUCH | KEY_A | KEY_B | KEY_X | KEY_Y
		| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
		| KEY_START | KEY_SELECT);
}

void
WaitForFrontKeypad()
{
	return WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y
		| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
		| KEY_START | KEY_SELECT);
}

void
WaitForArrowKey()
{
	return WaitForKey(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);
}

void
WaitForABXY()
{
	return WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y);
}
#elif YCL_Android
std::pair<float, float>
FetchCursor()
{
	lock_guard<mutex> lck(CursorMutex);

	return {LastCursorPosX, LastCursorPosY};
}

void
SaveInput(const ::AInputEvent& e)
{
	const auto update_key([](std::int32_t action, const std::uint8_t keycode){
		lock_guard<mutex> lck(KeyMutex);
		// TODO: Track Alt/Shift/Sym key states.
	//	const auto meta(::AKeyEvent_getMetaState(&e));

		switch(action)
		{
		case ::AKEY_EVENT_ACTION_DOWN:
		case ::AKEY_EVENT_ACTION_UP:
			KeyStateBuffer.set(keycode, action == ::AKEY_EVENT_ACTION_DOWN);
			break;
		case ::AKEY_EVENT_ACTION_MULTIPLE:
			// TODO: Record.
			break;
		}
	});
	const auto update_motion_key([](bool down){
		lock_guard<mutex> lck(KeyMutex);

		KeyStateBuffer.set(platform::KeyCodes::Primary, down);
	});

	switch(::AInputEvent_getType(&e))
	{
	case AINPUT_EVENT_TYPE_KEY:
		if(~::AKeyEvent_getFlags(&e) & ::AKEY_EVENT_FLAG_CANCELED)
			update_key(::AKeyEvent_getAction(&e),
				::AKeyEvent_getKeyCode(&e) & 0xFF);
		break;
	case AINPUT_EVENT_TYPE_MOTION:
		// TODO: Detect multiple pointers using 'AMotionEvent_getPointerCount'.
		// TODO: Support multiple pointers handlers.
		// TODO: Detect edges using 'AMotionEvent_getEdgeFlags'.
		// TODO: Record pressure using 'AMotionEvent_getPressure'.
		// TODO: Record touch area size using 'AMotionEvent_getSize'.
		// TODO: Track historical motion using 'AMotionEvent_getHistorical*'.
		if(::AMotionEvent_getFlags(&e) != AMOTION_EVENT_FLAG_WINDOW_IS_OBSCURED)
			switch(::AKeyEvent_getAction(&e) & AMOTION_EVENT_ACTION_MASK)
			{
			case AMOTION_EVENT_ACTION_CANCEL:
				break;
			case AMOTION_EVENT_ACTION_UP:
				update_motion_key({});
				break;
			case AMOTION_EVENT_ACTION_DOWN:
				update_motion_key(true);
			case AMOTION_EVENT_ACTION_MOVE:
			default:
				{
					lock_guard<mutex> lck(CursorMutex);

					yunseq(LastCursorPosX = ::AMotionEvent_getRawX(&e, 0),
						LastCursorPosY = ::AMotionEvent_getRawY(&e, 0));
				}
			}
	}
}
#endif

} // namespace platform_ex;

