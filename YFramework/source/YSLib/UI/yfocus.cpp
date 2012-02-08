/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.cpp
\ingroup UI
\brief 图形用户界面焦点特性。
\version r1472;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 +0800;
\par 修改时间:
	2012-02-04 22:12 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::YFocus;
*/


#include "YSLib/UI/ycontrol.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

bool
IsFocused(const IWidget& wgt)
{
	const auto p(FetchContainerPtr(wgt));

	return p ? FetchFocusingPtr(*p) == &wgt : false;
}

void
RequestFocusFrom(IWidget& dst, IWidget& src)
{
	if(auto p = FetchContainerPtr(dst))
	{
		auto& pFocusing(p->GetView().pFocusing);

		if(pFocusing != &dst)
		{
			if(pFocusing && IsFocused(*pFocusing))
				ReleaseFocusFrom(*pFocusing, dst);
			pFocusing = &dst;
			CallEvent<GotFocus>(dst, UIEventArgs(src));
		}
	}
}

void
ReleaseFocusFrom(IWidget& dst, IWidget& src)
{
	if(auto p = FetchContainerPtr(dst))
	{
		auto& pFocusing(p->GetView().pFocusing);

		if(pFocusing == &dst)
		{
			pFocusing = nullptr;
			CallEvent<LostFocus>(dst, UIEventArgs(src));
		}
	}
}

void
ClearFocusingOf(IWidget& wgt)
{
	if(const auto p = FetchFocusingPtr(wgt))
	{
		wgt.GetView().pFocusing = nullptr;
		CallEvent<LostFocus>(*p, UIEventArgs(wgt));
	}
}

YSL_END_NAMESPACE(Components)

YSL_END

