/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Consoles.cpp
\ingroup NBuilder
\brief 控制台。
\version r223
\author FrankHB <frankhb1989@gmail.com>
\since build 403
\par 创建时间:
	2013-05-09 11:01:35 +0800
\par 修改时间:
	2014-10-21 12:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Consoles
*/


#include "YCLib/YModules.h"
#include YFM_MinGW32_YCLib_Consoles

namespace platform_ex
{

inline namespace Windows
{

WConsole::WConsole(::DWORD h)
	: std_handle(h), h_console(::GetStdHandle(h)),
	saved_attr(GetScreenBufferInfo().wAttributes), Attributes(saved_attr)
{
	YAssert(h_console && h_console != INVALID_HANDLE_VALUE,
		"Wrong handle found;");
}
WConsole::~WConsole()
{
	RestoreAttributes();
}

::CONSOLE_SCREEN_BUFFER_INFO
WConsole::GetScreenBufferInfo() const
{
	::CONSOLE_SCREEN_BUFFER_INFO info;

	if(!::GetConsoleScreenBufferInfo(h_console, &info))
		YF_Raise_Win32Exception("GetConsoleScreenBufferInfo");
	return info;
}

void
WConsole::SetSystemColor(std::uint8_t color)
{
	char cmd[9];

	std::sprintf(cmd, "COLOR %02x", int(color));
	std::system(cmd);
}

void
WConsole::SetBackColor(std::uint8_t bc)
{
	const auto arr(DecomposeAttributes(Attributes));

	Attributes = ComposeAttributes(arr[0], bc);
}
void
WConsole::SetCursorPosition(::COORD pos)
{
	// NOTE: ::SetConsoleCursorPosition expects 1-based.
	::SetConsoleCursorPosition(h_console,
		{short(pos.X + 1), short(pos.Y + 1)});
}
void
WConsole::SetForeColor(std::uint8_t fc)
{
	const auto arr(DecomposeAttributes(Attributes));

	Attributes = ComposeAttributes(fc, arr[1]);
}

::WORD
WConsole::ComposeAttributes(std::uint8_t fore, std::uint8_t back)
{
	return (fore & 15) | ((back & 15) << 4);
}

void
WConsole::CursorUp(size_t num_rows)
{
	if(num_rows != 0)
	{
		const auto pos(GetCursorPosition());

		SetCursorPosition({short(pos.Y - num_rows), pos.X});
	}
}

std::array<std::uint8_t, 2>
WConsole::DecomposeAttributes(::WORD value)
{
	return {{std::uint8_t(value & 15), std::uint8_t((value >> 4) & 15)}};
}

void
WConsole::Erase(wchar_t c)
{
	const auto size(GetScreenBufferInfo().dwSize);

	Fill({short(0), short(0)}, size.X * size.Y, c);
}

void
WConsole::Fill(::COORD coord, size_t n, wchar_t c)
{
	::FillConsoleOutputCharacterW(h_console, c, n, coord, {});
	::FillConsoleOutputAttribute(h_console, Attributes, n, coord, {});
	::SetConsoleCursorPosition(h_console, {coord.X, coord.Y});
}

void
WConsole::RestoreAttributes() ynothrow
{
//	SetColor();
	Update(GetSavedAttributes());
}

void
WConsole::Update()
{
	Update(Attributes);
}
void
WConsole::Update(::WORD value)
{
	::SetConsoleTextAttribute(h_console, value);
}

void
WConsole::UpdateBackColor(std::uint8_t fc)
{
	SetBackColor(fc);
	Update();
}

void
WConsole::UpdateForeColor(std::uint8_t fc)
{
	SetForeColor(fc);
	Update();
}

std::unique_ptr<WConsole>
MakeWConsole(::DWORD h)
{
	std::unique_ptr<WConsole> p_con;

	TryExpr(p_con.reset(new WConsole(h)))
	CatchIgnore(Win32Exception&)
	return p_con;
}

} // inline namespace Windows;

} // namespace platform_ex;

