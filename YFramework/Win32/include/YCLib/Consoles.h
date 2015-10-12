/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Consoles.h
\ingroup YCLib
\ingroup Win32
\brief 控制台。
\version r285
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2013-05-09 11:01:12 +0800
\par 修改时间:
	2015-10-11 23:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::Consoles
*/


#ifndef YCL_MinGW32_INC_Consoles_h_
#define YCL_MinGW32_INC_Consoles_h_

#include "YCLib/YModules.h"
#include YFM_Win32_YCLib_MinGW32
#include <cstdlib> // for std::system;
#include YFM_YCLib_Container // for array;
#include YFM_YCLib_Video
#include <wincon.h>

namespace platform_ex
{

inline namespace Windows
{

/*!
\build Windows 控制台对象。
\since build 304
*/
class YF_API WConsole
{
public:
	/*!
	\build Windows 控制台文本样式。
	\since build 519
	*/
	enum Style
	{
		//! brief 暗淡文本，暗淡背景。
		Normal = 0,
		//! brief 明亮文本，暗淡背景。
		Bright = 0x08
	};

private:
	//! \since build 559
	::HANDLE h_std;
	//! \since build 519
	::WORD saved_attr;

public:
	//! \since build 519
	::WORD Attributes;

	//! \pre 断言：句柄非空有效。
	//@{
	//! \since build 564
	WConsole(unsigned long h = STD_OUTPUT_HANDLE);
	/*!
	\brief 构造：使用已知控制台句柄。
	\since build 559
	*/
	WConsole(::HANDLE);
	//@}
	//! \note 析构：重置属性。
	~WConsole();

	//! \since build 519
	//@{
	//! \throw Win32Exception 操作失败。
	//@{
	DefGetter(const, ::COORD, CursorPosition,
		GetScreenBufferInfo().dwCursorPosition)
	::CONSOLE_SCREEN_BUFFER_INFO
	GetScreenBufferInfo() const;
	//@}
	DefGetter(const ynothrow, ::WORD, SavedAttributes, saved_attr)
	//@}

	//! \since build 645
	void
	SetBackColor(std::uint8_t) ynothrow;
	//! \since build 519
	void
	SetCursorPosition(::COORD);
	//! \since build 645
	void
	SetForeColor(std::uint8_t) ynothrow;
	static PDefH(void, SetSystemColor, )
		ImplExpr(std::system("COLOR"))
	static void
	SetSystemColor(std::uint8_t);

	static PDefH(void, Clear, )
		ImplExpr(std::system("CLS"))

	//! \since build 645
	static ::WORD
	ComposeAttributes(std::uint8_t, std::uint8_t) ynothrow;

	//! \since build 519
	//@{
	void
	CursorUp(size_t);

	//! \since build 645
	static PDefH(array<std::uint8_t YPP_Comma 2>, DecomposeAttributes,
		::WORD val) ynothrow
		ImplRet({{FetchForeColor(val), FetchBackColor(val)}})

	//! \throw Win32Exception 操作失败。
	//@{
	void
	Erase(wchar_t = L' ');

	//! \since build 645
	static PDefH(std::uint8_t, FetchBackColor, ::WORD val) ynothrow
		ImplRet(std::uint8_t((val >> 4) & 15))

	//! \since build 645
	static PDefH(std::uint8_t, FetchForeColor, ::WORD val) ynothrow
		ImplRet(std::uint8_t(val & 15))

	//! \since build 576
	void
	Fill(::COORD, unsigned long, wchar_t = L' ');

	/*!
	\brief 重置属性。
	\since build 645
	*/
	void
	RestoreAttributes();

	void
	Update();
	void
	Update(::WORD);

	void
	UpdateBackColor(std::uint8_t);

	void
	UpdateForeColor(std::uint8_t);
	//@}

	/*!
	\brief 输出字符串。
	\pre 第一参数的数据指针非空。
	\return 输出的字符数。
	\since build 645
	*/
	//@{
	size_t
	WriteString(string_view);
	//! \note 第二参数为代码页。
	size_t
	WriteString(string_view, unsigned);
	size_t
	WriteString(wstring_view);
	PDefH(size_t, WriteString, u16string_view sv)
		ImplRet(WriteString({platform::wcast(sv.data()), sv.length()}))
	//@}
	//@}
};

/*!
\ingroup helper_functions
\brief 创建控制台对象。
\return 指向新创建的控制台对象的指针，若失败则为空。
\note 不抛出 Win32Exception 。
\relates WConsole
\since build 593
*/
YF_API unique_ptr<WConsole>
MakeWConsole(unsigned long = STD_OUTPUT_HANDLE);

} // inline namespace Windows;

} // namespace platform_ex;

#endif

