/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Consoles.h
\ingroup NBuilder
\brief 控制台。
\version r212
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2013-05-09 11:01:12 +0800
\par 修改时间:
	2014-10-06 06:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Consoles
*/


#ifndef YCL_MinGW32_INC_Consoles_h_
#define YCL_MinGW32_INC_Consoles_h_

#include "YCLib/YModules.h"
#include YFM_MinGW32_YCLib_MinGW32
#include <cstdlib> // for std::system;
#include <array> // for std::array;
#include YFM_YCLib_Video
#include <wincon.h>

namespace platform_ex
{

inline namespace Windows
{

/*
\build Windows 控制台对象。
\since build 304
*/
class YF_API WConsole
{
public:
	/*
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
	//! \since build 519
	::DWORD std_handle;
	//! \since build 519
	::HANDLE h_console;
	//! \since build 519
	::WORD saved_attr;

public:
	//! \since build 519
	::WORD Attributes;

	//! \since build 519
	WConsole(::DWORD h = STD_OUTPUT_HANDLE);
	//! \note 重置属性。
	~WConsole();

	//! \since build 519
	//@{
	DefGetter(const, ::COORD, CursorPosition,
		GetScreenBufferInfo().dwCursorPosition)
	::CONSOLE_SCREEN_BUFFER_INFO
	GetScreenBufferInfo() const;
	DefGetter(const ynothrow, ::WORD, SavedAttributes, saved_attr)
	DefGetter(const ynothrow, ::DWORD, StandardHandle, std_handle)
	//@}

	//! \since build 519
	void
	SetBackColor(std::uint8_t);
	//! \since build 519
	//@{
	void
	SetCursorPosition(::COORD);
	void
	SetForeColor(std::uint8_t);
	//@}
	static PDefH(void, SetSystemColor, )
		ImplExpr(std::system("COLOR"))
	static void
	SetSystemColor(std::uint8_t);

	static PDefH(void, Clear, )
		ImplExpr(std::system("CLS"))

	//! \since build 519
	//@{
	//! \since build 540
	static ::WORD
	ComposeAttributes(std::uint8_t, std::uint8_t);

	void
	CursorUp(size_t);

	//! \since build 540
	static std::array<std::uint8_t, 2>
	DecomposeAttributes(::WORD);

	void
	Erase(wchar_t = L' ');

	void
	Fill(::COORD, size_t, wchar_t = L' ');

	//! \brief 重置属性。
	void
	RestoreAttributes() ynothrow;

	void
	Update();
	void
	Update(::WORD);

	void
	UpdateBackColor(std::uint8_t);

	void
	UpdateForeColor(std::uint8_t);
	//@}
};

/*!
\ingroup helper_functions
\brief 创建控制台对象。
\return 指向新创建的控制台对象的指针，若失败则为空。
\note 不抛出 Win32Exception 。
\relates WConsole
\since build 542
*/
YF_API std::unique_ptr<WConsole>
MakeWConsole(::DWORD = STD_OUTPUT_HANDLE);

} // inline namespace Windows;

} // namespace platform_ex;

#endif

