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
\version r271
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 13:38:36 +0800
\par 修改时间:
	2014-04-06 13:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Input
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Input
#include YFM_YCLib_NativeAPI
#if YF_Multithread == 1
#	include <mutex>
//! \since build 489
#	define YCL_Def_LockGuard(_lck, _mutex) \
	std::lock_guard<std::mutex> _lck(_mutex);
#else
#	define YCL_Def_LockGuard(...)
#endif

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
#if YF_Multithread == 1
//! \since build 321
std::mutex CompKeyMutex;
std::mutex KeyMutex;
#endif
} //unnamed namespace;

const platform::KeyInput&
FetchKeyState()
{
	YAssert(pKeyState, "Null pointer found.");

	YCL_Def_LockGuard(lck, KeyMutex)

	return *pKeyState;
}

const platform::KeyInput&
FetchOldKeyState()
{
	YAssert(pOldKeyState, "Null pointer found.");

	YCL_Def_LockGuard(lck, KeyMutex)

	return *pOldKeyState;
}

platform::KeyInput
FetchKeyDownState()
{
	YCL_Def_LockGuard(comp_lck, CompKeyMutex)

	return FetchKeyState() & ~FetchOldKeyState();
}

platform::KeyInput
FetchKeyUpState()
{
	YCL_Def_LockGuard(comp_lck, CompKeyMutex)

	return (FetchKeyState() ^ FetchOldKeyState()) & ~FetchKeyState();
}

void
ClearKeyStates()
{
	YAssert(pKeyState && pOldKeyState, "Null pointer found.");

	YCL_Def_LockGuard(comp_lck, CompKeyMutex)
	YCL_Def_LockGuard(lck, KeyMutex)

	yunseq(pKeyState->reset(), pOldKeyState->reset());
}

#endif

void
UpdateKeyStates()
{
	YCL_Def_LockGuard(comp_lck, CompKeyMutex)
	YCL_Def_LockGuard(lck, KeyMutex)

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
		|KEY_START | KEY_SELECT);
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
#endif

} // namespace platform_ex;

