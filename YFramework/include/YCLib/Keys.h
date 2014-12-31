/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Keys.h
\ingroup YCLib
\brief 平台相关的基本按键输入定义。
\version r647
\author FrankHB <frankhb1989@gmail.com>
\since build 313
\par 创建时间:
	2012-06-01 14:29:56 +0800
\par 修改时间:
	2014-12-28 23:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Keys
*/


#ifndef YCL_INC_Keys_h_
#define YCL_INC_Keys_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include <bitset>

namespace platform
{

/*!
\brief 按键索引类型：标识特定的按键在按键集合中的位置。
\since build 490
*/
using KeyIndex = std::size_t;

/*!
\brief 按键并行位宽。
\note 不少于实际表示的 KeyPad 按键数。
\since build 490
*/
//@{
#if YCL_DS
yconstexpr KeyIndex KeyBitsetWidth(32);
#elif YCL_Win32 || YCL_Android || YCL_OS_X
yconstexpr KeyIndex KeyBitsetWidth(256);
#elif YCL_Linux
yconstexpr KeyIndex KeyBitsetWidth(0x200);
#else
#	error "Unsupported platform found."
#endif
//@}

/*!
\brief 本机输入类型。
\since build 298

支持并行按键的缓冲区数据类型。
*/
using KeyInput = std::bitset<KeyBitsetWidth>;


/*!
\brief 找到输入缓冲区记录中最小的按键编码。
\return 若存在编码则为最小值，否则为 KeyBitsetWidth 。
\since build 490
\todo 支持 libstdc++ 以外的实现。
*/
YF_API KeyIndex
FindFirstKey(const KeyInput&) ynothrow;

/*!
\brief 找到输入缓冲区记录中大于指定编码的最小的按键编码。
\return 若存在编码则为大于指定编码的最小值，否则为 KeyBitsetWidth 。
\since build 490
\todo 支持 libstdc++ 以外的实现。
*/
YF_API KeyIndex
FindNextKey(const KeyInput&, KeyIndex) ynothrow;


//! \since build 489
//@{
namespace KeyCategory
{

//! \brief 按键类别。
enum Category : std::uint_least32_t
{
	//! \brief 表示未分配或保留的键。
	None = 0,
	//! \brief 表示产生字母字符的键。
	Alphabetic = 1 << 1,
	//! \brief 表示产生数字字符的键。
	Numeric = 1 << 2,
	//! \brief 表示产生字母或数字字符的键。
	Alphanumeric = Alphabetic | Numeric,
	//! \brief 表示产生标点字符的键。
	Punctuation = 1 << 3,
	//! \brief 表示产生字符的键。
	Character = Alphanumeric | Punctuation,
	//! \brief 表示按键顺序相关的组合键。
	Dead = 1 << 4,
	//! \brief 表示锁定键。
	Lock = 2 << 4,
	//! \brief 表示修饰键。
	Modifier = 1 << 6,
	//! \brief 表示功能键。
	Function = 1 << 7,
	//! \brief 表示导航键（如方向键和 PgUp ）、菜单键和 Esc 。
	Navigation = 1 << 8,
	//! \brief 表示编辑键（如回车、空格、退格、插入、删除和制表符）
	Editing = 1 << 9,
	//! \brief 表示系统键（如 SysRq/PrintScr 、 Break/Pause 和电源管理）。
	System = 1 << 10,
	//! \brief 表示输入法模式键（一般用于日文和韩文）。
	IME = 1 << 11,
	//! \brief 表示默认由设备制造商指定。
	OEM = 1 << 12,
	//! \brief 表示菜单键（如 Menu 和 Win ）。
	Menu = 1 << 13,
	//! \brief 表示小键盘键。
	Keypad = 1 << 14,
	//! \brief 表示非键盘键（如鼠标键）。
	NonKeyboard = 2 << 14,
	//! \brief 表示通过多个物理按键组合构成的键。
	Composed = 1 << 16,
	//! \brief 表示没有对应单一物理按键的虚拟键。
	Virtual = 2 << 16
};

//! \relates Category
DefBitmaskEnum(Category)


//! \since build 490
//@{
/*!
\brief 取按键编码对应的按键类别。
\pre 断言：参数小于 KeyBitsetWidth 。
*/
YF_API YB_PURE Category
ClassifyKey(KeyIndex) ynothrow;

/*!
\brief 找到第一个在指定类别的按键编码。
\note 使用位与运算。
*/
YF_API KeyIndex
FindFirstKeyInCategroy(const KeyInput&, KeyIndex) ynothrow;

//! \brief 判断指定按键编码是否通过多个物理按键组合构成。
#if YCL_Win32
inline YB_PURE PDefH(bool, IsComposedKey, KeyIndex code) ynothrow
	ImplRet(ClassifyKey(code))
#else
yconstfn PDefH(bool, IsComposedKey, KeyIndex) ynothrow
	ImplRet({})
#endif
//@}
} // namespace KeyCategory;
//@}


/*!
\brief 映射按键到键入的字符。
\return 若未找到对应按键或不支持为 char() ，否则为对应的字符。
\note 对 Win32 ，返回值不大于 0x80 。
\since build 490
\todo Android 实现。
*/
//@{
#if YCL_Win32
YF_API char
MapKeyChar(KeyIndex) ynothrow;
YF_API char
MapKeyChar(const KeyInput&, KeyIndex) ynothrow;
#else
yconstexpr PDefH(char, MapKeyChar, KeyIndex) ynothrow
	ImplRet({})
//! \since build 489
yconstexpr PDefH(char, MapKeyChar, const KeyInput&, KeyIndex) ynothrow
	ImplRet({})
#endif
//@}


/*!
\brief 本机按键编码。
\note MinGW32 下的 KeyCodes 中仅含每个实现默认需要支持的基本集合。
\since build 298
*/
namespace KeyCodes
{

#if YCL_DS
//! \since build 416
enum NativeSet
{
	//! \warning 不保证名称可移植。
	//@{
	A = 0,
	B = 1,
	Select = 2,
	Start = 3,
	//@}
	Right = 4,
	Left = 5,
	Up = 6,
	Down = 7,
	//! \warning 不保证名称可移植。
	//@{
	R = 8,
	L = 9,
	X = 10,
	Y = 11,
	Touch = 12,
	Lid = 13
	//@}
};

//按键别名。
yconstexpr NativeSet Enter(A), Esc(B), PgUp(L), PgDn(R);
//! \since build 490
yconstexpr NativeSet Home(X), End(Y);
//! \since build 493
yconstexpr NativeSet Primary(Touch);

/*!
\brief 扩展集：作为 DS 可直接被 KeyInput 表示的非物理键按键编码。
\note LibNDS 不使用 14 及以上的移位值。
\since build 490
*/
enum ExtendedSet
{
	Backspace = 14,
	Tab,
	Shift,
	Ctrl,
	Alt,
	Pause,
	CapsLock,
	//! \since build 491
	Space,
	Insert,
	Delete,
	//! \since build 493
	Secondary,
	//! \since build 493
	Tertiary
};
#elif YCL_Win32
/*!
\brief 基本公用按键集合。
\note 值和 WinSDK 的 VK_* 宏替换结果对应。
\since build 416
*/
enum NativeSet
{
	Empty = 0,
	//! \since build 493
	//@{
	//! \note 同 VK_LBUTTON 。
	Primary = 0x01,
	//! \note 同 VK_RBUTTON 。
	Secondary = 0x02,
	//! \note 同 VK_MBUTTON 。
	Tertiary = 0x04,
	//@}
	//! \since build 490
	//@{
	//! \note 同 VK_BACK 。
	Backspace = 0x08,
	//! \note 同 VK_TAB 。
	Tab = 0x09,
	//@}
	//! \note 同 VK_RETURN 。
	Enter = 0x0D,
	//! \since build 490
	//@{
	//! \note 同 VK_SHIFT 。
	Shift = 0x10,
	//! \note 同 VK_CONTROL 。
	Ctrl = 0x11,
	//! \note 同 VK_MENU 。
	Alt = 0x12,
	//! \note 同 VK_PAUSE 。
	Pause = 0x13,
	//! \note 同 VK_CAPITAL 。
	CapsLock = 0x14,
	//@}
	//! \note 同 VK_ESCAPE 。
	Esc = 0x1B,
	/*!
	\note 同 VK_SPACE 。
	\since build 491
	*/
	Space = 0x20,
	//! \note 同 VK_PRIOR 。
	PgUp = 0x21,
	//! \note 同 VK_NEXT 。
	PgDn = 0x22,
	//! \since build 490
	//@{
	//! \note 同 VK_END 。
	End = 0x23,
	//! \note 同 VK_HOME 。
	Home = 0x24,
	//@}
	//! \note 同 VK_LEFT 。
	Left = 0x25,
	//! \note 同 VK_UP 。
	Up = 0x26,
	//! \note 同 VK_RIGHT 。
	Right = 0x27,
	//! \note 同 VK_DOWN 。
	Down = 0x28,
	//! \since build 490
	//@{
	//! \note 同 VK_INSERT 。
	Insert = 0x2D,
	//! \note 同 VK_DELETE 。
	Delete = 0x2E
	//@}
};
#elif YCL_Android
/*!
\brief 基本公用按键集合。
\note 值和 Android SDK 的 android.view.KeyEvent 的 KEYCODE_* 值对应。
\since build 492
*/
enum NativeSet
{
	//! \note 同 KEYCODE_UNKNOWN 。
	Empty = 0,
	//! \note 同 KEYCODE_DPAD_UP 。
	Up = 0x13,
	//! \note 同 KEYCODE_DPAD_DOWN 。
	Down = 0x14,
	//! \note 同 KEYCODE_DPAD_LEFT 。
	Left = 0x15,
	//! \note 同 KEYCODE_DPAD_RIGHT 。
	Right = 0x16,
	//! \note 同 KEYCODE_ALT_LEFT 。
	Alt = 0x39,
	//! \note 同 KEYCODE_SHIFT_LEFT 。
	Shift = 0x3B,
	//! \note 同 KEYCODE_TAB 。
	Tab = 0x3D,
	//! \note 同 KEYCODE_SPACE 。
	Space = 0x3E,
	//! \note 同 KEYCODE_ENTER 。
	Enter = 0x42,
	//! \note 同 KEYCODE_DEL 。
	Backspace = 0x43,
	//! \note 同 KEYCODE_PAGE_UP 。
	PgUp = 0x5C,
	//! \note 同 KEYCODE_PAGE_DOWN 。
	PgDn = 0x5D,
	//! \note 同 KEYCODE_ESCAPE 。
	Esc = 0x6F,
	//! \note 同 KEYCODE_FORWARD_DEL 。
	Delete = 0x70,
	//! \note 同 KEYCODE_CTRL_LEFT 。
	Ctrl = 0x71,
	//! \note 同 KEYCODE_CAPS_LOCK 。
	CapsLock = 0x73,
	//! \note 同 KEYCODE_BREAK 。
	Pause = 0x79,
	//! \note 同 KEYCODE_MOVE_HOME 。
	Home = 0x7A,
	//! \note 同 KEYCODE_MOVE_END 。
	End = 0x7B,
	//! \note 同 KEYCODE_INSERT 。
	Insert = 0x7C
};

//! \since build 493
enum ExtendedSet
{
	Primary = 0xE0,
	Secondary = 0xE1,
	Tertiary = 0xE2
};
#elif YCL_Linux
/*!
\brief 基本公用按键集合。
\note 值和 Linux 头文件 <tt>\<linux/input.h\></tt> 中的宏 KEY_* 替换的值对应。
\since build 562
*/
enum NativeSet
{
	//! \note 同 KEY_RESERVED 。
	Empty = 0,
	//! \note 同 KEY_ESC 。
	Esc = 1,
	//! \note 同 KEY_BACKSPACE 。
	Backspace = 14,
	//! \note 同 KEY_TAB 。
	Tab = 15,
	//! \note 同 KEY_ENTER 。
	Enter = 28,
	//! \note 同 KEY_LEFTCTRL 。
	Ctrl = 29,
	//! \note 同 KEY_LEFTSHIFT 。
	Shift = 42,
	//! \note 同 KEY_LEFTALT 。
	Alt = 56,
	//! \note 同 KEY_SPACE 。
	Space = 57,
	//! \note 同 KEY_CAPSLOCK 。
	CapsLock = 58,
	//! \note 同 KEY_HOME 。
	Home = 102,
	//! \note 同 KEY_UP 。
	Up = 103,
	//! \note 同 KEY_PAGEUP 。
	PgUp = 104,
	//! \note 同 KEY_LEFT 。
	Left = 105,
	//! \note 同 KEY_RIGHT 。
	Right = 106,
	//! \note 同 KEY_END 。
	End = 107,
	//! \note 同 KEY_DOWN 。
	Down = 108,
	//! \note 同 KEY_PAGEDOWN 。
	PgDn = 109,
	//! \note 同 KEY_INSERT 。
	Insert = 110,
	//! \note 同 KEY_DELETE 。
	Delete = 111,
	//! \note 同 KEYCODE_PAUSE 。
	Pause = 119,
	//! \note 同 BTN_LEFT 。
	Primary = 0x110,
	//! \note 同 BTN_RIGHT 。
	Secondary = 0x111,
	//! \note 同 BTN_MIDDLE 。
	Tertiary = 0x112
};
#endif


/*!
\brief 取锁定键状态。
\return 若参数指定可锁定的键则取状态，否则总是 false 。
\since build 490
\todo Android 实现。
*/
YF_API bool
FetchLockState(KeyIndex) ynothrow;

/*!
\brief 切换锁定键状态。
\note 若参数指定可锁定的键则切换状态，否则忽略。
\since build 490
\todo Android 实现。
*/
YF_API void
ToggleLockState(KeyIndex) ynothrow;

} // namespace KeyCodes;

} // namespace platform;

#endif

