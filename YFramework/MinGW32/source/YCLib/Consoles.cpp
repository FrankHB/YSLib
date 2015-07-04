/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Consoles.cpp
\ingroup YCLib
\ingroup MinGW32
\brief 控制台。
\version r258
\author FrankHB <frankhb1989@gmail.com>
\since build 403
\par 创建时间:
	2013-05-09 11:01:35 +0800
\par 修改时间:
	2015-07-04 09:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Consoles
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_MinGW32_YCLib_Consoles
#	include YFM_YSLib_Core_YCoreUtilities // for YSLib::CheckPositiveScalar;

using namespace YSLib;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

WConsole::WConsole(unsigned long dev)
	: WConsole(::GetStdHandle(dev))
{}
WConsole::WConsole(::HANDLE h)
	: h_std(h), saved_attr(GetScreenBufferInfo().wAttributes),
	Attributes(saved_attr)
{
	YAssert(h_std && h_std != INVALID_HANDLE_VALUE, "Invalid handle found.");
}
WConsole::~WConsole()
{
	RestoreAttributes();
}

::CONSOLE_SCREEN_BUFFER_INFO
WConsole::GetScreenBufferInfo() const
{
	::CONSOLE_SCREEN_BUFFER_INFO info;

	YCL_CallWin32(GetConsoleScreenBufferInfo,
		"WConsole::GetScreenBufferInfo", h_std, &info);
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
	Attributes = ComposeAttributes(FetchForeColor(Attributes), bc);
}
void
WConsole::SetCursorPosition(::COORD pos)
{
	// NOTE: %::SetConsoleCursorPosition expects 1-based.
	::SetConsoleCursorPosition(h_std, {short(pos.X + 1), short(pos.Y + 1)});
}
void
WConsole::SetForeColor(std::uint8_t fc)
{
	Attributes = ComposeAttributes(fc, FetchBackColor(Attributes));
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

		// XXX: Conversion to 'short' might be implementation-defined.
		SetCursorPosition({short(pos.Y - short(num_rows)), pos.X});
	}
}

void
WConsole::Erase(wchar_t c)
{
	const auto size(GetScreenBufferInfo().dwSize);

	Fill({short(), short()}, CheckPositiveScalar<unsigned long>(size.X)
		* CheckPositiveScalar<unsigned long>(size.Y), c);
}

void
WConsole::Fill(::COORD coord, unsigned long n, wchar_t c)
{
	::FillConsoleOutputCharacterW(h_std, c, n, coord, {});
	::FillConsoleOutputAttribute(h_std, Attributes, n, coord, {});
	::SetConsoleCursorPosition(h_std, {coord.X, coord.Y});
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
	::SetConsoleTextAttribute(h_std, value);
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

unique_ptr<WConsole>
MakeWConsole(unsigned long h)
{
	unique_ptr<WConsole> p_con;

	TryExpr(p_con.reset(new WConsole(h)))
	CatchIgnore(Win32Exception&)
	return p_con;
}

} // inline namespace Windows;

#endif

} // namespace platform_ex;

