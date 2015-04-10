/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Keys.cpp
\ingroup YCLib
\brief 平台相关的基本按键输入定义。
\version r1237
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:32:37 +0800
\par 修改时间:
	2015-04-06 03:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Keys
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Keys
#include YFM_YCLib_Debug
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
	Editing | Virtual, // Delete is an editing key.
	Editing | NonKeyboard, // Extended secondary key.
	Editing | NonKeyboard // Extended tertiary key.
};
#elif YCL_Win32
// See http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx .
yconstexpr Category KeyCategoryTable[KeyBitsetWidth]{
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
yconstexpr Category KeyCategoryTable[KeyBitsetWidth]{
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
	Modifier | IME, // KEYCODE_SWITCH_CHARSET.
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
	Function, // KEYCODE_MEDIA_AUDIO_TRACK.
	Editing | NonKeyboard, // Extended primary key.
	Editing | NonKeyboard, // Extended secondary key.
	Editing | NonKeyboard // Extended tertiary key.
};
#elif YCL_Linux
yconstexpr Category KeyCategoryTable[]{
	None, // KEY_RESERVED.
	Navigation, // KEY_ESC.
	Numeric, // KEY_1.
	Numeric, // KEY_2.
	Numeric, // KEY_3.
	Numeric, // KEY_4.
	Numeric, // KEY_5.
	Numeric, // KEY_6.
	Numeric, // KEY_7.
	Numeric, // KEY_8.
	Numeric, // KEY_9.
	Numeric, // KEY_0.
	Punctuation, // KEY_MINUS.
	Punctuation, // KEY_EQUAL.
	Editing, // KEY_BACKSPACE.
	Editing, // KEY_TAB.
	Alphabetic, // KEY_Q.
	Alphabetic, // KEY_W.
	Alphabetic, // KEY_E.
	Alphabetic, // KEY_R.
	Alphabetic, // KEY_T.
	Alphabetic, // KEY_Y.
	Alphabetic, // KEY_U.
	Alphabetic, // KEY_I.
	Alphabetic, // KEY_O.
	Alphabetic, // KEY_P.
	Punctuation, // KEY_LEFTBRACE.
	Punctuation, // KEY_RIGHTBRACE.
	Editing, // KEY_ENTER.
	Modifier, // KEY_LEFTCTRL.
	Alphabetic, // KEY_A.
	Alphabetic, // KEY_S.
	Alphabetic, // KEY_D.
	Alphabetic, // KEY_F.
	Alphabetic, // KEY_G.
	Alphabetic, // KEY_H.
	Alphabetic, // KEY_J.
	Alphabetic, // KEY_K.
	Alphabetic, // KEY_L.
	Punctuation, // KEY_SEMICOLON.
	Punctuation, // KEY_APOSTROPHE.
	Punctuation, // KEY_GRAVE.
	Modifier, // KEY_LEFTSHIFT.
	Punctuation, // KEY_BACKSLASH.
	Alphabetic, // KEY_Z.
	Alphabetic, // KEY_X.
	Alphabetic, // KEY_C.
	Alphabetic, // KEY_V.
	Alphabetic, // KEY_B.
	Alphabetic, // KEY_N.
	Alphabetic, // KEY_M.
	Punctuation, // KEY_COMMA.
	Punctuation, // KEY_DOT.
	Punctuation, // KEY_SLASH.
	Modifier, // KEY_RIGHTSHIFT.
	Punctuation | Keypad, // KEY_KPASTERISK.
	Modifier, // KEY_LEFTALT.
	Editing, // KEY_SPACE.
	Lock, // KEY_CAPSLOCK.
	Function, // KEY_F1.
	Function, // KEY_F2.
	Function, // KEY_F3.
	Function, // KEY_F4.
	Function, // KEY_F5.
	Function, // KEY_F6.
	Function, // KEY_F7.
	Function, // KEY_F8.
	Function, // KEY_F9.
	Function, // KEY_F10.
	Lock, // KEY_NUMLOCK.
	Lock, // KEY_SCROLLLOCK.
	Numeric | Keypad, // KEY_KP7.
	Numeric | Keypad, // KEY_KP8.
	Numeric | Keypad, // KEY_KP9.
	Punctuation | Keypad, // KEY_KPMINUS.
	Numeric | Keypad, // KEY_KP4.
	Numeric | Keypad, // KEY_KP5.
	Numeric | Keypad, // KEY_KP6.
	Punctuation | Keypad, // KEY_KPPLUS.
	Numeric | Keypad, // KEY_KP1.
	Numeric | Keypad, // KEY_KP2.
	Numeric | Keypad, // KEY_KP3.
	Numeric | Keypad, // KEY_KP0.
	Punctuation | Keypad, // KEY_KPDOT.
	None,
	IME, // KEY_ZENKAKUHANKAKU.
	Punctuation, // KEY_102ND.
	Function, // KEY_F11.
	Function, // KEY_F12.
	IME, // KEY_RO.
	IME, // KEY_KATAKANA.
	IME, // KEY_HIRAGANA.
	IME, // KEY_HENKAN.
	IME, // KEY_KATAKANAHIRAGANA.
	IME, // KEY_MUHENKAN.
	Punctuation | Keypad, // KEY_KPJPCOMMA.
	Editing | Keypad, // KEY_KPENTER.
	Modifier, // KEY_RIGHTCTRL.
	Punctuation | Keypad, // KEY_KPSLASH.
	System, // KEY_SYSRQ.
	Modifier, // KEY_RIGHTALT.
	Editing, // KEY_LINEFEED.
	Navigation, // KEY_HOME.
	Navigation, // KEY_UP.
	Navigation, // KEY_PAGEUP.
	Navigation, // KEY_LEFT.
	Navigation, // KEY_RIGHT.
	Navigation, // KEY_END.
	Navigation, // KEY_DOWN.
	Navigation, // KEY_PAGEDOWN.
	Editing, // KEY_INSERT.
	Editing, // KEY_DELETE.
	Punctuation | OEM, // KEY_MACRO.
	Function, // KEY_MUTE.
	Function, // KEY_VOLUMEDOWN.
	Function, // KEY_VOLUMEUP.
	System, // KEY_POWER.
	Punctuation | Keypad, // KEY_KPEQUAL.
	Punctuation | Keypad, // KEY_KPPLUSMINUS.
	System, // KEY_PAUSE.
	None,
	Punctuation | Keypad, // KEY_KPCOMMA.
	IME, // KEY_HANGUEL.
	IME, // KEY_HANJA.
	Punctuation, // KEY_YEN.
	Modifier, // KEY_LEFTMETA.
	Modifier, // KEY_RIGHTMETA.
	Modifier, // KEY_COMPOSE.
	Function, // KEY_STOP.
	Function, // KEY_AGAIN.
	Function, // KEY_PROPS.
	Editing, // KEY_UNDO.
	Function, // KEY_FRONT.
	Editing, // KEY_COPY.
	Function, // KEY_OPEN.
	Editing, // KEY_PASTE.
	Function, // KEY_FIND.
	Editing, // KEY_CUT.
	Function, // KEY_HELP.
	Function, // KEY_MENU.
	Function, // KEY_CALC.
	Function, // KEY_SETUP.
	System, // KEY_SLEEP.
	System, // KEY_WAKEUP.
	Function, // KEY_FILE.
	Function, // KEY_SENDFILE.
	Function, // KEY_DELETEFILE.
	Function, // KEY_XFER.
	OEM, // KEY_PROG1.
	OEM, // KEY_PROG2.
	Function, // KEY_WWW.
	Function, // KEY_MSDOS.
	Function, // KEY_COFFEE.
	Function, // KEY_DIRECTION.
	Function, // KEY_CYCLEWINDOWS.
	Function, // KEY_MAIL.
	Function, // KEY_BOOKMARKS.
	Function, // KEY_COMPUTER.
	Function, // KEY_BACK.
	Function, // KEY_FORWARD.
	Function, // KEY_CLOSECD.
	Function, // KEY_EJECTCD.
	Function, // KEY_EJECTCLOSECD.
	Function, // KEY_NEXTSONG.
	Function, // KEY_PLAYPAUSE.
	Function, // KEY_PREVIOUSSONG.
	Function, // KEY_STOPCD.
	Function, // KEY_RECORD.
	Function, // KEY_REWIND.
	Function, // KEY_PHONE.
	IME, // KEY_ISO.
	Function, // KEY_CONFIG.
	Function, // KEY_HOMEPAGE.
	Function, // KEY_REFRESH.
	Function, // KEY_EXIT.
	Function, // KEY_MOVE.
	Function, // KEY_EDIT.
	Navigation, // KEY_SCROLLUP.
	Navigation, // KEY_SCROLLDOWN.
	Punctuation | Keypad, // KEY_KPLEFTPAREN.
	Punctuation | Keypad, // KEY_KPRIGHTPAREN.
	None,
	None,
	Function, // KEY_F13.
	Function, // KEY_F14.
	Function, // KEY_F15.
	Function, // KEY_F16.
	Function, // KEY_F17.
	Function, // KEY_F18.
	Function, // KEY_F19.
	Function, // KEY_F20.
	Function, // KEY_F21.
	Function, // KEY_F22.
	Function, // KEY_F23.
	Function, // KEY_F24.
	None,
	None,
	None,
	None,
	None,
	Function, // KEY_PLAYCD.
	Function, // KEY_PAUSECD.
	OEM, // KEY_PROG3.
	OEM, // KEY_PROG4.
	None,
	System, // KEY_SUSPEND.
	Function, // KEY_CLOSE.
	Function, // KEY_PLAY.
	Function, // KEY_FASTFORWARD.
	Function, // KEY_BASSBOOST.
	Function, // KEY_PRINT.
	Function, // KEY_HP.
	Function, // KEY_CAMERA.
	Function, // KEY_SOUND.
	Function, // KEY_QUESTION.
	Function, // KEY_EMAIL.
	Function, // KEY_CHAT.
	Function, // KEY_SEARCH.
	Function, // KEY_CONNECT.
	Function, // KEY_FINANCE.
	Function, // KEY_SPORT.
	Function, // KEY_SHOP.
	Function, // KEY_ALTERASE.
	Navigation, // KEY_CANCEL.
	Function, // KEY_BRIGHTNESSDOWN.
	Function, // KEY_BRIGHTNESSUP.
	Function, // KEY_MEDIA.
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None, // KEY_UNKNOWN.
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	NonKeyboard, // BTN_MISC/BTN_0.
	NonKeyboard, // BTN_1.
	NonKeyboard, // BTN_2.
	NonKeyboard, // BTN_3.
	NonKeyboard, // BTN_4.
	NonKeyboard, // BTN_5.
	NonKeyboard, // BTN_6.
	NonKeyboard, // BTN_7.
	NonKeyboard, // BTN_8.
	NonKeyboard, // BTN_9.
	None,
	None,
	None,
	None,
	None,
	None,
	Editing | NonKeyboard, // BTN_MOUSE/BTN_LEFT.
	Editing | NonKeyboard, // BTN_RIGHT.
	Editing | NonKeyboard, // BTN_MIDDLE.
	Editing | NonKeyboard, // BTN_SIDE.
	OEM | NonKeyboard, // BTN_EXTRA.
	Navigation | NonKeyboard, // BTN_FORWARD.
	Navigation | NonKeyboard, // BTN_BACK.
	NonKeyboard, // BTN_TASK.
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	NonKeyboard, // BTN_JOYSTICK/BTN_TRIGGER.
	NonKeyboard, // BTN_THUMB.
	NonKeyboard, // BTN_THUMB2.
	NonKeyboard, // BTN_TOP.
	NonKeyboard, // BTN_TOP2.
	NonKeyboard, // BTN_PINKIE.
	NonKeyboard, // BTN_BASE.
	NonKeyboard, // BTN_BASE2.
	NonKeyboard, // BTN_BASE3.
	NonKeyboard, // BTN_BASE4.
	NonKeyboard, // BTN_BASE5.
	NonKeyboard, // BTN_BASE6.
	NonKeyboard, // BTN_DEAD.
	None,
	None,
	None,
	NonKeyboard, // BTN_GAMEPAD/BTN_A.
	NonKeyboard, // BTN_B.
	NonKeyboard, // BTN_C.
	NonKeyboard, // BTN_X.
	NonKeyboard, // BTN_Y.
	NonKeyboard, // BTN_Z.
	NonKeyboard, // BTN_TL.
	NonKeyboard, // BTN_TR.
	NonKeyboard, // BTN_TL2.
	NonKeyboard, // BTN_TR2.
	NonKeyboard, // BTN_SELECT.
	Editing | NonKeyboard, // BTN_START.
	Function | NonKeyboard, // BTN_MODE.
	Editing | NonKeyboard, // BTN_THUMBL.
	Editing | NonKeyboard, // BTN_THUMBR.
	None,
	Editing | NonKeyboard, // BTN_DIGI/BTN_TOOL_PEN.
	Editing | NonKeyboard, // BTN_TOOL_RUBBER.
	Editing | NonKeyboard, // BTN_TOOL_BRUSH.
	Editing | NonKeyboard, // BTN_TOOL_PENCIL.
	Editing | NonKeyboard, // BTN_TOOL_AIRBRUSH.
	Editing | NonKeyboard, // BTN_TOOL_FINGER.
	Editing | NonKeyboard, // BTN_TOOL_MOUSE.
	Editing | NonKeyboard, // BTN_TOOL_LENS.
	None,
	None,
	NonKeyboard, // BTN_TOUCH.
	NonKeyboard, // BTN_STYLUS.
	NonKeyboard, // BTN_STYLUS2.
	Editing | NonKeyboard | Virtual, // BTN_TOOL_DOUBLETAP.
	Editing | NonKeyboard | Virtual, // BTN_TOOL_TRIPLETAP.
	None,
	Navigation | NonKeyboard, // BTN_WHEEL/BTN_GEAR_DOWN.
	Navigation | NonKeyboard, // BTN_GEAR_UP.
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	Editing, // KEY_OK.
	Function, // KEY_SELECT.
	Function, // KEY_GOTO.
	Function, // KEY_CLEAR.
	System, // KEY_POWER2.
	Function, // KEY_OPTION.
	Function, // KEY_INFO.
	Function, // KEY_TIME.
	OEM, // KEY_VENDOR.
	Function, // KEY_ARCHIVE.
	Function, // KEY_PROGRAM.
	Function, // KEY_CHANNEL.
	Function, // KEY_FAVORITES.
	Function, // KEY_EPG.
	Function, // KEY_PVR.
	Function, // KEY_MHP.
	Function, // KEY_LANGUAGE.
	Function, // KEY_TITLE.
	Function, // KEY_SUBTITLE.
	Function, // KEY_ANGLE.
	Function, // KEY_ZOOM.
	Function, // KEY_MODE.
	Function, // KEY_KEYBOARD.
	Function, // KEY_SCREEN.
	Function, // KEY_PC.
	Function, // KEY_TV.
	Function, // KEY_TV2.
	Function, // KEY_VCR.
	Function, // KEY_VCR2.
	Function, // KEY_SAT.
	Function, // KEY_SAT2.
	Function, // KEY_CD.
	Function, // KEY_TAPE.
	Function, // KEY_RADIO.
	Function, // KEY_TUNER.
	Function, // KEY_PLAYER.
	Function, // KEY_TEXT.
	Function, // KEY_DVD.
	Function, // KEY_AUX.
	Function, // KEY_MP3.
	Function, // KEY_AUDIO.
	Function, // KEY_VIDEO.
	Function, // KEY_DIRECTORY.
	Function, // KEY_LIST.
	Function, // KEY_MEMO.
	Function, // KEY_CALENDAR.
	Function, // KEY_RED.
	Function, // KEY_GREEN.
	Function, // KEY_YELLOW.
	Function, // KEY_BLUE.
	Function, // KEY_CHANNELUP.
	Function, // KEY_CHANNELDOWN.
	Navigation, // KEY_FIRST.
	Navigation, // KEY_LAST.
	Function, // KEY_AB.
	Navigation, // KEY_NEXT.
	Function, // KEY_RESTART.
	Function, // KEY_SLOW.
	Function, // KEY_SHUFFLE.
	Function, // KEY_BREAK.
	Function, // KEY_PREVIOUS.
	Function, // KEY_DIGITS.
	Function, // KEY_TEEN.
	Function, // KEY_TWEN.
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	Editing, // KEY_DEL_EOL.
	Editing, // KEY_DEL_EOS.
	Editing, // KEY_INS_LINE.
	Editing, // KEY_DEL_LINE.
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	Function, // KEY_FN.
	Editing | Function, // KEY_FN_ESC.
	Function, // KEY_FN_F1.
	Function, // KEY_FN_F2.
	Function, // KEY_FN_F3.
	Function, // KEY_FN_F4.
	Function, // KEY_FN_F5.
	Function, // KEY_FN_F6.
	Function, // KEY_FN_F7.
	Function, // KEY_FN_F8.
	Function, // KEY_FN_F9.
	Function, // KEY_FN_F10.
	Function, // KEY_FN_F11.
	Function, // KEY_FN_F12.
	Function, // KEY_FN_1.
	Function, // KEY_FN_2.
	Function, // KEY_FN_D.
	Function, // KEY_FN_E.
	Function, // KEY_FN_F.
	Function, // KEY_FN_S.
	Function, // KEY_FN_B.
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None,
	None // KEY_MAX.
};

//! \since build 562
static_assert(ystdex::arrlen(KeyCategoryTable) == KeyBitsetWidth,
	"Key table is invalid.");
#else
#	error "Unsupported platform found."
#endif

} // unnamed namespace;

Category
ClassifyKey(KeyIndex code) ynothrow
{
	YAssert(code < KeyBitsetWidth, "Invalid argument found.");
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

#if YCL_Win32
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
	return ::ToAscii(unsigned(code), ::MapVirtualKeyW(unsigned(code),
		MAPVK_VK_TO_VSC), state, &s, 0) == 1 ? switch_key(char(s) & 0x7F)
		: char();
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
	return (::GetKeyState(int(code)) & 0x0001) != 0;
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

