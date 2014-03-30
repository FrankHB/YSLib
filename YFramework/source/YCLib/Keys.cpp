/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Keys.cpp
\ingroup YCLib
\brief 平台相关的基本按键输入定义。
\version r399
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:32:37 +0800
\par 修改时间:
	2014-03-30 18:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Keys
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Keys
#include YFM_YCLib_NativeAPI

namespace platform
{

size_t
FindFirstKey(const KeyInput& keys)
{
#if _GLIBCXX_BITSET
	return keys._Find_first();
#else
#	error "Only libstdc++ is currently supported."
#endif
}

size_t
FindNextKey(const KeyInput& keys, size_t key)
{
#if _GLIBCXX_BITSET
	return keys._Find_next(key);
#else
#	error "Only libstdc++ is currently supported."
#endif
}


namespace KeyCategory
{

//! \since build 489
namespace
{

#if YCL_DS
yconstexpr Category KeyCategoryTable[KeyBitsetWidth]{
	Editing, // KEY_A as a synonym of 'Enter'.
	Navigation, // KEY_B as a synonym of 'Esc'.
	Menu | System, // KEY_SELECT as a synonym of 'Menu'.
	Editing | Menu | System, // KEY_START as a synonym of 'Enter'.
	Navigation, // KEY_RIGHT is an arrow key.
	Navigation, // KEY_LEFT is an arrow key.
	Navigation, // KEY_UP is an arrow key.
	Navigation, // KEY_DOWN is an arrow key.
	Navigation, // KEY_R is an extended navigation key.
	Navigation, // KEY_L is an extended navigation key.
	Navigation, // KEY_X is an extended navigation key.
	Navigation, // KEY_Y is an extended navigation key.
	NonKeyboard, // KEY_TOUCH is a key of cursor device.
	NonKeyboard // KEY_LID is a key of lid.
};
#elif YCL_Win32
yconstexpr const Category KeyCategoryTable[KeyBitsetWidth]{
	None, // 0 is not assigned.
	Editing | NonKeyboard, // VK_LBUTTON is a mouse key.
	Editing | NonKeyboard, // VK_RBUTTON is a mouse key.
	System | Composed, // VK_CANCEL.
	Editing | NonKeyboard, // VK_MBUTTON is a mouse key.
	NonKeyboard, // VK_XBUTTON1 is a mouse key.
	NonKeyboard, // VK_XBUTTON2 is a mouse key.
	None, // Undefined.
	Editing, // VK_BACK.
	Editing, // VK_TAB.
	None, // Reserved.
	None, // Reserved.
	Editing, // VK_CLEAR.
	Editing, // VK_RETURN.
	None, // Undefined.
	None, // Undefined.
	Modifier | Composed, // VK_SHIFT.
	Modifier | Composed, // VK_CONTROL.
	Modifier | Menu | Composed, // VK_MENU is a composed menu modifier key.
	System, // VK_PAUSE.
	Lock, // VK_CAPITAL.
	IME, // VK_KANA, VK_HANGUEL, VK_HANGUL.
	None, // Undefined.
	IME, // VK_JUNJA.
	IME, // VK_FINAL.
	IME, // VK_HANJA, VK_KANJI.
	None, // Undefined.
	Navigation, // VK_ESCAPE.
	IME, // VK_CONVERT.
	IME, // VK_NONCONVERT.
	IME, // VK_ACCEPT.
	IME, // VK_MODECHANGE.
	Editing, // VK_SPACE.
	Navigation, // VK_PRIOR.
	Navigation, // VK_NEXT.
	Navigation, // VK_END.
	Navigation, // VK_HOME.
	Navigation, // VK_LEFT.
	Navigation, // VK_UP.
	Navigation, // VK_RIGHT.
	Navigation, // VK_DOWN.
	System, // VK_SELECT.
	System, // VK_PRINT.
	System, // VK_EXECUTE.
	System, // VK_SNAPSHOT.
	Editing, // VK_INSERT.
	Editing, // VK_DELETE.
	Function, // VK_HELP.
	Numeric, // 0 key.
	Numeric,
	Numeric,
	Numeric,
	Numeric,
	Numeric,
	Numeric,
	Numeric,
	Numeric,
	Numeric, // 9 key.
	None, // Undefined.
	None, // Undefined.
	None, // Undefined.
	None, // Undefined.
	None, // Undefined.
	None, // Undefined.
	None, // Undefined.
	Alphabetic, // A key.
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic,
	Alphabetic, // Z key.
	Modifier | Menu, // VK_LWIN.
	Modifier | Menu, // VK_RWIN.
	Function | Menu, // VK_APPS.
	None, // Reserved.
	System, // VK_SLEEP.
	Numeric | Keypad, // VK_NUMPAD0.
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad, // VK_NUMPAD9.
	Punctuation | Keypad, // VK_MULTIPLY.
	Punctuation | Keypad, // VK_SEPARATOR.
	Punctuation | Keypad, // VK_SUBTRACT.
	Punctuation | Keypad, // VK_DECIMAL.
	Punctuation | Keypad, // VK_DIVIDE.
	Function, // VK_F1.
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function, // VK_F12.
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function,
	Function, // VK_F24.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	Lock, // VK_NUMLOCK.
	Lock, // VK_SCROLL.
	OEM,
	OEM,
	OEM,
	OEM,
	OEM,
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	Modifier, // VK_LSHIFT.
	Modifier, // VK_RSHIFT.
	Modifier, // VK_LCONTROL.
	Modifier, // VK_RCONTROL.
	Modifier | Menu, // VK_LMENU.
	Modifier | Menu, // VK_RMENU.
	Function, // VK_BROWSER_BACK.
	Function, // VK_BROWSER_FORWARD.
	Function, // VK_BROWSER_REFRESH.
	Function, // VK_BROWSER_STOP.
	Function, // VK_BROWSER_SEARCH.
	Function, // VK_BROWSER_FAVORITES.
	Function, // VK_BROWSER_HOME.
	Function, // VK_VOLUME_MUTE.
	Function, // VK_VOLUME_DOWN.
	Function, // VK_VOLUME_UP.
	Function, // VK_MEDIA_NEXT_TRACK.
	Function, // VK_MEDIA_PREV_TRACK.
	Function, // VK_MEDIA_STOP.
	Function, // VK_MEDIA_PLAY_PAUSE.
	Function, // VK_LAUNCH_MAIL.
	Function, // VK_LAUNCH_MEDIA_SELECT.
	Function, // VK_LAUNCH_APP1.
	Function, // VK_LAUNCH_APP2.
	None, // Reserved.
	None, // Reserved.
	Punctuation | OEM, // VK_OEM_1.
	Punctuation | OEM, // VK_OEM_PLUS.
	Punctuation | OEM, // VK_OEM_COMMA.
	Punctuation | OEM, // VK_OEM_MINUS.
	Punctuation | OEM, // VK_OEM_PERIOD.
	Punctuation | OEM, // VK_OEM_2.
	Punctuation | OEM, // VK_OEM_3.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Reserved.
	None, // Unassigned.
	None, // Unassigned.
	None, // Unassigned.
	Punctuation | OEM, // VK_OEM_4.
	Punctuation | OEM, // VK_OEM_5.
	Punctuation | OEM, // VK_OEM_6.
	Punctuation | OEM, // VK_OEM_7.
	OEM, // VK_OEM_8.
	None, // Reserved.
	OEM, // OEM specific.
	Punctuation | OEM, // VK_OEM_102.
	OEM, // OEM specific.
	OEM, // OEM specific.
	IME, // VK_PROCESSKEY.
	OEM, // OEM specific.
	Virtual, // VK_PACKET.
	None, // Unassigned.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	OEM, // OEM specific.
	Function, // VK_ATTN.
	Function, // VK_CRSEL.
	Function, // VK_EXSEL.
	Function, // VK_EREOF.
	Function, // VK_PLAY.
	Function, // VK_ZOOM.
	None, // VK_NONAME.
	None, // VK_PA1.
	OEM | Function // VK_OEM_CLEAR.
};
#else
#	error "Unsupported platform found."
#endif

} // unnamed namespace;

Category
ClassifyKey(size_t code)
{
	yconstraint(code < KeyBitsetWidth);

	return KeyCategoryTable[code];
}

size_t
FindFirstKeyInCategroy(const KeyInput& keys, size_t cat)
{
	for(auto code(FindFirstKey(keys)); code != KeyBitsetWidth;
		code = FindNextKey(keys, code))
		if(ClassifyKey(code) & cat)
			return code;
	return KeyBitsetWidth;
}

} // namespace KeyCategory;


#if YCL_Win32
char
MapKeyChar(size_t code)
{
	return ::MapVirtualKeyW(unsigned(code), MAPVK_VK_TO_CHAR) & 0x7F;
}
char
MapKeyChar(const KeyInput& keys, size_t code)
{
	ystdex::byte state[256];
	unsigned short s;

	// NOTE: See implementation of %UpdateKeyStates.
	for(std::size_t i(1); i < platform::KeyBitsetWidth - 1; ++i)
		state[i] = keys[i] ? 0x80 : 0;
	return ::ToAscii(code, ::MapVirtualKeyW(code, MAPVK_VK_TO_VSC), state, &s,
		0) == 1 ? char(s) & 0x7F : char();
}
#endif

}

