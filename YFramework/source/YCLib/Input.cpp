/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Input.cpp
\ingroup YCLib
\brief 平台相关的扩展输入接口。
\version r1076;
\author FrankHB<frankhb1989@gmail.com>
\since build 299 。
\par 创建时间:
	2012-04-07 13:38:36 +0800;
\par 修改时间:
	2012-04-07 15:03 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::Input;
*/


#include "YCLib/Input.h"
#ifdef YCL_MINGW32
#include <mutex>
#endif

namespace platform
{

void
WaitForInput()
{
#ifdef YCL_DS
	while(true)
 	{
		platform_ex::UpdateKeyStates();
		if(platform_ex::FetchKeyDownState().any())
			break;
 		platform_ex::swiWaitForVBlank();
	};
#else
	std::getchar();
#endif
}

} // namespace platform;

namespace platform_ex
{

void
WriteCursor(CursorInfo& tp)
{
#ifdef YCL_DS
	touchRead(&tp);
	//修正触摸位置。
	if(YCL_LIKELY(tp.px != 0 && tp.py != 0))
		yunseq(--tp.px, --tp.py);
	else
		// NOTE: YSL_ Point::Invalid;
		yunseq(tp.px = platform::SDst(-1), tp.py = platform::SDst(-1));
#elif defined(YCL_MINGW32)
// TODO: impl;
#else
#	error Unsupported platform found!
#endif
}


KeyInput KeyState, OldKeyState;
#ifdef YCL_MINGW32
namespace
{
	std::mutex KeyMutex;
}
#endif

void
UpdateKeyStates()
{
#ifdef YCL_MINGW32
	std::lock_guard<std::mutex> lck(KeyMutex);

#endif
	OldKeyState = KeyState;
#ifdef YCL_DS
	KeyState = ::keysCurrent();
#elif defined(YCL_MINGW32)
	for(std::size_t i(0); i < platform::KeyBitsetWidth; ++i)
		KeyState.set(i, ::GetAsyncKeyState(i) & 0x8000);
#endif
}

#ifdef YCL_DS
void
WaitForKey(platform::KeyInput mask)
{
	while(true)
 	{
		UpdateKeyStates();
		if((FetchKeyDownState() & mask).any())
			break;
 		swiWaitForVBlank();
	};
}
#endif

} // namespace platform_ex;

