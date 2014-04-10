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
\version r699
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:32:37 +0800
\par 修改时间:
	2014-04-10 12:34 +0800
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

KeyIndex
FindFirstKey(const KeyInput& keys) ynothrow
{
#if _GLIBCXX_BITSET
	return keys._Find_first();
#else
#	error "Only libstdc++ is currently supported."
#endif
}

KeyIndex
FindNextKey(const KeyInput& keys, KeyIndex key) ynothrow
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
	NonKeyboard, // KEY_LID is a key of lid.
	Editing | Virtual, // Backapce is an editing key.
	Editing | Virtual, // Tab is an editing key.
	Modifier | Virtual, // Shift is an modifier key.
	Modifier | Virtual, // Ctrl is an modifier key.
	Modifier | Menu | Virtual, // Alt is an modifier menu key.
	System | Virtual, // Pause is a system key.
	Lock | Virtual, // CapsLock is an lock key.
	Editing | Virtual, // Insert is an editing key.
	Editing | Virtual // Delete is an editing key.
};
#elif YCL_Win32
// See http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx .
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
	Punctuation | Keypad, // VK_ADD.
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
#elif YCL_Android
// See http://developer.android.com/reference/android/view/KeyEvent.html .
yconstexpr const Category KeyCategoryTable[KeyBitsetWidth]{
	None, // KEYCODE_UNKNOWN.
	Function, // KEYCODE_SOFT_LEFT.
	Function, // KEYCODE_SOFT_RIGHT .
	System, // KEYCODE_HOME is handled by the framework.
	Navigation, // KEYCODE_BACK.
	Function, // KEYCODE_CALL.
	Function, // KEYCODE_ENDCALL.
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
	Punctuation, // KEYCODE_STAR.
	Punctuation, // KEYCODE_POUND.
	Navigation | Keypad, // KEYCODE_DPAD_UP.
	Navigation | Keypad, // KEYCODE_DPAD_DOWN.
	Navigation | Keypad, // KEYCODE_DPAD_LEFT.
	Navigation | Keypad, // KEYCODE_DPAD_RIGHT.
	Navigation | Keypad, // KEYCODE_DPAD_CENTER.
	Function, // KEYCODE_VOLUME_UP.
	Function, // KEYCODE_VOLUME_DOWN.
	System, // KEYCODE_POWER.
	Function, // KEYCODE_CAMERA.
	Editing, // KEYCODE_CLEAR.
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
	Punctuation, // KEYCODE_COMMA.
	Punctuation, // KEYCODE_PERIOD.
	Modifier | Menu, // KEYCODE_ALT_LEFT.
	Modifier | Menu, // KEYCODE_ALT_RIGHT.
	Modifier, // KEYCODE_SHIFT_LEFT.
	Modifier, // KEYCODE_SHIFT_RIGHT.
	Editing, // KEYCODE_TAB.
	Editing, // KEYCODE_SPACE.
	Modifier, // KEYCODE_SYM.
	Function, // KEYCODE_EXPLORER.
	Function, // KEYCODE_ENVELOPE.
	Editing, // KEYCODE_ENTER.
	Editing, // KEYCODE_DEL.
	Punctuation, // KEYCODE_GRAVE.
	Punctuation, // KEYCODE_MINUS.
	Punctuation, // KEYCODE_EQUALS.
	Punctuation, // KEYCODE_LEFT_BRACKET.
	Punctuation, // KEYCODE_RIGHT_BRACKET.
	Punctuation, // KEYCODE_BACKSLASH.
	Punctuation, // KEYCODE_SEMICOLON.
	Punctuation, // KEYCODE_APOSTROPHE.
	Punctuation, // KEYCODE_SLASH.
	Punctuation, // KEYCODE_AT.
	Modifier, // KEYCODE_NUM.
	NonKeyboard, // KEYCODE_HEADSETHOOK.
	Function, // KEYCODE_FOCUS.
	Punctuation, // KEYCODE_PLUS.
	Function | Menu, // KEYCODE_MENU.
	Function, // KEYCODE_NOTIFICATION.
	Function, // KEYCODE_SEARCH.
	Function, // KEYCODE_MEDIA_PLAY_PAUSE.
	Function, // KEYCODE_MEDIA_STOP.
	Function, // KEYCODE_MEDIA_NEXT.
	Function, // KEYCODE_MEDIA_PREVIOUS.
	Function, // KEYCODE_MEDIA_REWIND.
	Function, // KEYCODE_MEDIA_FAST_FORWARD.
	Function, // KEYCODE_MUTE.
	Navigation, // KEYCODE_PAGE_UP.
	Navigation, // KEYCODE_PAGE_DOWN.
	Modifier, // KEYCODE_PICTSYMBOLS.
	IME | Modifier, // KEYCODE_SWITCH_CHARSET.
	Function | Keypad, // KEYCODE_BUTTON_A.
	Function | Keypad, // KEYCODE_BUTTON_B.
	Function | Keypad, // KEYCODE_BUTTON_C.
	Function | Keypad, // KEYCODE_BUTTON_X.
	Function | Keypad, // KEYCODE_BUTTON_Y.
	Function | Keypad, // KEYCODE_BUTTON_Z.
	Function | Keypad, // KEYCODE_BUTTON_L1.
	Function | Keypad, // KEYCODE_BUTTON_R1.
	Function | Keypad, // KEYCODE_BUTTON_L2.
	Function | Keypad, // KEYCODE_BUTTON_R2.
	Function | Keypad, // KEYCODE_BUTTON_THUMBL.
	Function | Keypad, // KEYCODE_BUTTON_THUMBR.
	Function | Keypad, // KEYCODE_BUTTON_START.
	Function | Keypad, // KEYCODE_BUTTON_SELECT.
	Function | Keypad, // KEYCODE_BUTTON_MODE.
	Navigation, // KEYCODE_ESCAPE.
	Editing, // KEYCODE_FORWARD_DEL.
	Modifier, // KEYCODE_CTRL_LEFT.
	Modifier, // KEYCODE_CTRL_RIGHT.
	Lock, // KEYCODE_CAPS_LOCK.
	Lock, // KEYCODE_SCROLL_LOCK.
	Modifier, // KEYCODE_META_LEFT.
	Modifier, // KEYCODE_META_RIGHT.
	Function | Modifier, // KEYCODE_FUNCTION.
	System, // KEYCODE_SYSRQ.
	System, // KEYCODE_BREAK.
	Navigation, // KEYCODE_MOVE_HOME.
	Navigation, // KEYCODE_MOVE_END.
	Editing, // Editing.
	Navigation, // KEYCODE_FORWARD.
	Function, // KEYCODE_MEDIA_PLAY.
	Function, // KEYCODE_MEDIA_PAUSE.
	Function, // KEYCODE_MEDIA_CLOSE.
	Function, // KEYCODE_MEDIA_EJECT.
	Function, // KEYCODE_MEDIA_RECORD.
	Function, // KEYCODE_F1.
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
	Function, // KEYCODE_F12.
	Lock, // KEYCODE_NUM_LOCK.
	Numeric | Keypad, // KEYCODE_NUMPAD_0.
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad,
	Numeric | Keypad, // KEYCODE_NUMPAD_9.
	Punctuation | Keypad, // KEYCODE_NUMPAD_DIVIDE.
	Punctuation | Keypad, // KEYCODE_NUMPAD_MULTIPLY.
	Punctuation | Keypad, // KEYCODE_NUMPAD_SUBTRACT.
	Punctuation | Keypad, // KEYCODE_NUMPAD_ADD.
	Punctuation | Keypad, // KEYCODE_NUMPAD_DOT.
	Punctuation | Keypad, // KEYCODE_NUMPAD_COMMA.
	Editing | Keypad, // KEYCODE_NUMPAD_ENTER.
	Punctuation | Keypad, // KEYCODE_NUMPAD_EQUALS.
	Punctuation | Keypad, // KEYCODE_NUMPAD_LEFT_PAREN.
	Punctuation | Keypad, // KEYCODE_NUMPAD_RIGHT_PAREN.
	Function, // KEYCODE_VOLUME_MUTE.
	Function, // KEYCODE_INFO.
	Function, // KEYCODE_CHANNEL_UP.
	Function, // KEYCODE_CHANNEL_DOWN.
	Function, // KEYCODE_ZOOM_IN.
	Function, // KEYCODE_ZOOM_OUT.
	Function, // KEYCODE_TV.
	Function, // KEYCODE_WINDOW.
	Function, // KEYCODE_GUIDE.
	Function, // KEYCODE_DVR.
	Function, // KEYCODE_BOOKMARK.
	Function, // KEYCODE_CAPTIONS.
	Function, // KEYCODE_SETTINGS.
	System, // KEYCODE_TV_POWER.
	Function, // KEYCODE_TV_INPUT.
	System, // KEYCODE_STB_POWER.
	Function, // KEYCODE_STB_INPUT.
	System, // KEYCODE_AVR_POWER.
	Function, // KEYCODE_AVR_INPUT.
	Function, // KEYCODE_PROG_RED.
	Function, // KEYCODE_PROG_GREEN.
	Function, // KEYCODE_PROG_YELLOW.
	Function, // KEYCODE_PROG_BLUE.
	Function, // KEYCODE_APP_SWITCH.
	Function | Keypad, // KEYCODE_BUTTON_1.
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad,
	Function | Keypad, // KEYCODE_BUTTON_16.
	IME, // KEYCODE_LANGUAGE_SWITCH.
	Function, // KEYCODE_MANNER_MODE.
	Function, // KEYCODE_3D_MODE.
	Function, // KEYCODE_CONTACTS.
	Function, // KEYCODE_CALENDAR.
	Function, // KEYCODE_MUSIC.
	Function, // KEYCODE_CALCULATOR.
	IME, // KEYCODE_ZENKAKU_HANKAKU.
	IME, // KEYCODE_EISU.
	IME, // KEYCODE_MUHENKAN.
	IME, // KEYCODE_HENKAN.
	IME, // KEYCODE_KATAKANA_HIRAGANA.
	Punctuation, // KEYCODE_YEN.
	Punctuation, // KEYCODE_RO.
	IME, // KEYCODE_KANA.
	System, // KEYCODE_ASSIST is not delivered to applications.
	Function, // KEYCODE_BRIGHTNESS_DOWN.
	Function, // KEYCODE_BRIGHTNESS_UP.
	Function // KEYCODE_MEDIA_AUDIO_TRACK.
};
#else
#	error "Unsupported platform found."
#endif

} // unnamed namespace;

Category
ClassifyKey(KeyIndex code) ynothrow
{
	yconstraint(code < KeyBitsetWidth);

	return KeyCategoryTable[code];
}

KeyIndex
FindFirstKeyInCategroy(const KeyInput& keys, KeyIndex cat) ynothrow
{
	for(auto code(FindFirstKey(keys)); code != KeyBitsetWidth;
		code = FindNextKey(keys, code))
		if(ClassifyKey(code) & cat)
			return code;
	return KeyBitsetWidth;
}

} // namespace KeyCategory;


//! \since build 490
namespace
{

#if !YCL_DS
inline char
switch_key(char s) ynothrow
{
	using namespace KeyCodes;

	return std::isalpha(s) && FetchLockState(CapsLock) ? s ^ 0x20 : s;
}
#endif

} // unnamed namespace;

#if YCL_Win32
char
MapKeyChar(KeyIndex code) ynothrow
{
	return ::MapVirtualKeyW(unsigned(code), MAPVK_VK_TO_CHAR) & 0x7F;
}
char
MapKeyChar(const KeyInput& keys, KeyIndex code) ynothrow
{
	ystdex::byte state[256];
	unsigned short s;

	// NOTE: See implementation of %UpdateKeyStates.
	for(std::size_t i(1); i < platform::KeyBitsetWidth - 1; ++i)
		state[i] = keys[i] ? 0x80 : 0;
	return ::ToAscii(code, ::MapVirtualKeyW(code, MAPVK_VK_TO_VSC), state, &s,
		0) == 1 ? switch_key(char(s) & 0x7F) : char();
}

namespace KeyCodes
{

//! \since build 490
namespace
{

#if YCL_DS
bool caps_lock;
#endif

} // unnamed namespace;

bool
FetchLockState(KeyIndex code) ynothrow
{
#if YCL_DS || YCL_Android
	return code == CapsLock ? caps_lock : false;
#elif YCL_Win32
	return (::GetKeyState(code) & 0x0001) != 0;
#endif
}

void
ToggleLockState(KeyIndex code) ynothrow
{
#if YCL_DS || YCL_Android
	if(code == CapsLock)
		caps_lock = !caps_lock;
#elif YCL_Win32
	if(KeyCategory::ClassifyKey(code) & KeyCategory::Lock)
	{
		::INPUT input[2]{{INPUT_KEYBOARD, {}}, {INPUT_KEYBOARD, {}}};

		yunseq(
		input[0].ki.wVk = code, input[1].ki.wVk = code,
		input[1].ki.dwFlags = KEYEVENTF_KEYUP
		);
		::SendInput(2, input, sizeof(::INPUT));
	}
#endif
}

}

#endif

}

