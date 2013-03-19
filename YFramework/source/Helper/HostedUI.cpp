/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostedUI.cpp
\ingroup Helper
\brief 宿主环境支持的用户界面。
\version r63
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-17 10:22:36 +0800
\par 修改时间:
	2013-03-19 00:50 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::HostedUI
*/


#include "Helper/HostedUI.h"
#include "Host.h"

YSL_BEGIN

using namespace Drawing;
using namespace UI;

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

BufferedRenderer*
GetHostRendererPtrOf(UI::IWidget& wgt)
{
	return dynamic_cast<HostRenderer*>(&wgt.GetRenderer());
}

Window*
GetWindowPtrOf(UI::IWidget& wgt)
{
	if(const auto p_r = dynamic_cast<HostRenderer*>(&wgt.GetRenderer()))
		return p_r->GetWindowPtr();
	return nullptr;
}


unique_ptr<BufferedRenderer>
MakeHostRenderer(IWidget& wgt, std::function<NativeWindowHandle()> f)
{
	return unique_ptr<BufferedRenderer>(new HostRenderer(wgt, std::move(f)));
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

