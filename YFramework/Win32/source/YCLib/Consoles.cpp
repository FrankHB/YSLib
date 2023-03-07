/*
	© 2013-2016, 2020, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Consoles.cpp
\ingroup YCLib
\ingroup Win32
\brief 控制台。
\version r373
\author FrankHB <frankhb1989@gmail.com>
\since build 403
\par 创建时间:
	2013-05-09 11:01:35 +0800
\par 修改时间:
	2023-03-04 16:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::Consoles
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_Win32_YCLib_Consoles // for UTF8ToWCS;
#	include YFM_YSLib_Core_YCoreUtilities // for YSLib::CheckPositive;
#	include YFM_Win32_YCLib_NLS // for platform_ex::MBCSToWCS;

using namespace YSLib;
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

//! \since build 712
namespace
{

//! \since build 565
YB_PURE int WINAPI
ConsoleHandler(unsigned long ctrl)
{
	switch(ctrl)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		std::_Exit(int(STATUS_CONTROL_C_EXIT));
	}
	return 0;
}

} // unnamed namespace;

void
FixConsoleHandler(int(WINAPI* handler)(unsigned long), bool add)
{
	YCL_CallF_Win32(SetConsoleCtrlHandler, handler ? handler : ConsoleHandler,
		add);
}


WConsole::WConsole(unsigned long dev)
	: WConsole(::GetStdHandle(dev))
{}
WConsole::WConsole(::HANDLE h)
	: h_out(h), saved_attr(GetScreenBufferInfo().wAttributes),
	Attributes(saved_attr)
{
	YAssert(h_out && h_out != INVALID_HANDLE_VALUE, "Invalid handle found.");
}
WConsole::~WConsole()
{
	FilterExceptions(std::bind(&WConsole::RestoreAttributes, this), yfsig);
}

::CONSOLE_SCREEN_BUFFER_INFO
WConsole::GetScreenBufferInfo() const
{
	::CONSOLE_SCREEN_BUFFER_INFO info;

	YCL_CallF_Win32(GetConsoleScreenBufferInfo, h_out, &info);
	return info;
}

void
WConsole::SetBackColor(std::uint8_t bc) ynothrow
{
	Attributes = ComposeAttributes(Attributes, FetchForeColor(Attributes), bc);
}
void
WConsole::SetCursorPosition(::COORD pos)
{
	// NOTE: %::SetConsoleCursorPosition expects 1-based.
	YCL_CallF_Win32(SetConsoleCursorPosition, h_out,
		{short(pos.X + 1), short(pos.Y + 1)});
}
void
WConsole::SetForeColor(std::uint8_t fc) ynothrow
{
	Attributes = ComposeAttributes(Attributes, fc, FetchBackColor(Attributes));
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

	Fill({short(), short()}, CheckPositive<unsigned long>(size.X)
		* CheckPositive<unsigned long>(size.Y), c);
}

void
WConsole::Fill(::COORD coord, unsigned long n, wchar_t c)
{
	unsigned long w;

	YCL_CallF_Win32(FillConsoleOutputCharacterW, h_out, c, n, coord, &w);
	YCL_CallF_Win32(FillConsoleOutputAttribute, h_out, Attributes, n, coord,
		&w);
	YCL_CallF_Win32(SetConsoleCursorPosition, h_out, {coord.X, coord.Y});
}

void
WConsole::RestoreAttributes()
{
	Attributes = saved_attr;
	Update();
}

void
WConsole::Update()
{
	Update(Attributes);
}
void
WConsole::Update(::WORD attr)
{
	YCL_CallF_Win32(SetConsoleTextAttribute, h_out, attr);
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

size_t
WConsole::WriteString(string_view sv)
{
	YAssertNonnull(sv.data());
	return WriteString(UTF8ToWCS(sv));
}
size_t
WConsole::WriteString(string_view sv, unsigned cp)
{
	YAssertNonnull(sv.data());
	return WriteString(MBCSToWCS(sv, cp));
}
size_t
WConsole::WriteString(wstring_view sv)
{
	YAssertNonnull(sv.data());

	unsigned long n;

	YCL_CallF_Win32(WriteConsoleW, h_out, sv.data(),
		static_cast<unsigned long>(sv.length()), &n, yimpl({}));
	return size_t(n);
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

