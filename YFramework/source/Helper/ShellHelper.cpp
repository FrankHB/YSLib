/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShellHelper.cpp
\ingroup Helper
\brief Shell 助手模块。
\version r1291;
\author FrankHB<frankhb1989@gmail.com>
\since build 278 。
\par 创建时间:
	2010-04-04 13:42:15 +0800;
\par 修改时间:
	2012-04-27 11:52 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::ShellHelper;
*/


#include "Helper/ShellHelper.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

YSL_END_NAMESPACE(Drawing)

void
RemoveGlobalTasks(Shell& shl)
{
	auto& app(FetchGlobalInstance());

	app.Queue.Remove(&shl, app.UIResponseLimit);
}

YSL_END;

