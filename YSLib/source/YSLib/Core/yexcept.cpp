/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yexcept.cpp
\ingroup Core
\brief 异常处理模块。
\version 0.1164;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-15 20:30:14 +0800;
\par 修改时间:
	2011-03-07 13:10 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YException;
*/


#include "yexcept.h"

YSL_BEGIN

Exception::Exception()
	: std::exception()
{}
Exception::~Exception() throw()
{}


GeneralEvent::GeneralEvent(const string& s)
	: Exception(),
	str(s)
{}
GeneralEvent::~GeneralEvent() throw()
{}

const char*
GeneralEvent::what() const throw()
{
	return str.c_str();
}


LoggedEvent::LoggedEvent(const string& s, u8 l)
	: GeneralEvent(s),
	level(l)
{}
LoggedEvent::LoggedEvent(const GeneralEvent& e, u8 l)
	: GeneralEvent(e),
	level(l)
{}
LoggedEvent::~LoggedEvent() throw()
{}

YSL_END

