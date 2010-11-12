/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ysmsgdef.h
\ingroup Core
\brief 标准 Shell 消息列表。
\version 0.2165;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-08 12:05:26 + 08:00;
\par 修改时间:
	2010-11-12 15:52 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellMessageDefinition;
*/


#ifndef INCLUDED_YSMSGDEF_H_
#define INCLUDED_YSMSGDEF_H_

#define SM_NULL					0x0000
#define SM_CREATE				0x0001
#define SM_DESTROY				0x0002
#define SM_SET					0x0003
#define SM_DROP					0x0004
#define SM_ACTIVATED			0x0005
#define SM_DEACTIVATED			0x0006
#define SM_SETFOCUS				0x0007
#define SM_KILLFOCUS			0x0008

#define SM_ENABLE				0x000A
#define SM_SETREDRAW			0x000B
#define SM_SETTEXT				0x000C
#define SM_GETTEXT				0x000D
#define SM_GETTEXTLENGTH		0x000E
#define SM_PAINT				0x000F
#define SM_CLOSE				0x0010
#define SM_QUIT					0x0012
#define SM_ERASEBKGND			0x0014
#define SM_SYSCOLORCHANGE		0x0015
#define SM_SHOWWINDOW			0x0018

#define SM_DEVMODECHANGE		0x001B
#define SM_ACTIVATEAPP			0x001C
#define SM_FONTCHANGE			0x001D
#define SM_TIMECHANGE			0x001E
#define SM_CANCELMODE			0x001F
#define SM_SETCURSOR			0x0020
#define SM_MOUSEACTIVATE		0x0021
#define SM_CHILDACTIVATE		0x0022
#define SM_QUEUESYNC			0x0023

#define SM_WNDCREATE			0x2001
#define SM_WNDDESTROY			0x2002
#define SM_WNDDROP				0x2004

#define SM_INPUT				0x4001
#define SM_SCRREFRESH			0x4002

#endif

