/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Input.cpp
\ingroup YCLib
\brief 平台相关的扩展输入接口。
\version r190;
\author FrankHB<frankhb1989@gmail.com>
\since build 299 。
\par 创建时间:
	2012-04-07 13:38:36 +0800;
\par 修改时间:
	2012-08-27 17:22 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::Input;
*/


#include "YCLib/Input.h"
#if YCL_MULTITHREAD == 1
#include <mutex>
#	define YCL_DEF_LOCKGUARD(_lck, _mutex) \
	std::lock_guard<std::mutex> _lck(_mutex);
#else
#	define YCL_DEF_LOCKGUARD(...)
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
//! \since build 321 。
//@{
platform::KeyInput KeyStateA, KeyStateAKeyStateB;
platform::KeyInput *pKeyState(&KeyStateA), *pOldKeyState(&KeyStateAKeyStateB);
//@}
#if YCL_MULTITHREAD == 1
//! \since build 321 。
std::mutex CompKeyMutex;
std::mutex KeyMutex;
#endif
} //unnamed namespace;

const platform::KeyInput&
FetchKeyState()
{
	YAssert(pKeyState, "Null pointer found.");

	YCL_DEF_LOCKGUARD(lck, KeyMutex)

	return *pKeyState;
}

const platform::KeyInput&
FetchOldKeyState()
{
	YAssert(pOldKeyState, "Null pointer found.");

	YCL_DEF_LOCKGUARD(lck, KeyMutex)

	return *pOldKeyState;
}

platform::KeyInput
FetchKeyDownState()
{
	YCL_DEF_LOCKGUARD(comp_lck, CompKeyMutex)

	return FetchKeyState() &~ FetchOldKeyState();
}

platform::KeyInput
FetchKeyUpState()
{
	YCL_DEF_LOCKGUARD(comp_lck, CompKeyMutex)

	return (FetchKeyState() ^ FetchOldKeyState()) & ~FetchKeyState();
}

void
ClearKeyStates()
{
	YAssert(pKeyState && pOldKeyState, "Null pointer found.");

	YCL_DEF_LOCKGUARD(comp_lck, CompKeyMutex)
	YCL_DEF_LOCKGUARD(lck, KeyMutex)

	yunseq(pKeyState->reset(), pOldKeyState->reset());
}

#endif

void
UpdateKeyStates()
{
	YCL_DEF_LOCKGUARD(comp_lck, CompKeyMutex)
	YCL_DEF_LOCKGUARD(lck, KeyMutex)

#if YCL_KEYSTATE_DIRECT
	OldKeyState = KeyState;
#else
	std::swap(pKeyState, pOldKeyState);
#endif
#if YCL_DS
	KeyState = ::keysCurrent();
#elif YCL_MINGW32
	// NOTE: 0x00 and 0xFF should be invalid.
	for(std::size_t i(1); i < platform::KeyBitsetWidth - 1; ++i)
		pKeyState->set(i, ::GetAsyncKeyState(i) & 0x8000);
#endif
}


void
WriteCursor(platform::CursorInfo& tp)
{
#if YCL_DS
	::touchRead(&tp);
	//修正触摸位置。
	if(YB_LIKELY(tp.px != 0 && tp.py != 0))
		yunseq(--tp.px, --tp.py);
	else
		// NOTE: %YSL_ Point::Invalid.
		yunseq(tp.px = std::uint16_t(-1), tp.py = std::uint16_t(-1));
#elif YCL_MINGW32
	::GetCursorPos(&tp);
#else
#	error Unsupported platform found!
#endif
}


#if YCL_DS
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
#endif

} // namespace platform_ex;

