/*
	© 2010-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.cpp
\ingroup UI
\brief 图形用户界面焦点特性。
\version r568
\author FrankHB <frankhb1989@gmail.com>
\since build 258
\par 创建时间:
	2010-05-01 13:52:56 +0800
\par 修改时间:
	2013-12-23 23:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YFocus
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YControl

namespace YSLib
{

namespace UI
{

bool
IsFocused(const IWidget& wgt)
{
	const auto p(FetchContainerPtr(wgt));

	return p ? FetchFocusingPtr(*p) == &wgt : false;
}

bool
DoRequestFocus(IWidget& wgt, bool release_event)
{
	if(const auto p = FetchContainerPtr(wgt))
	{
		auto& pFocusing(p->GetView().FocusingPtr);

		if(pFocusing != &wgt)
		{
			if(pFocusing && IsFocused(*pFocusing))
			{
				if(release_event)
					ReleaseFocusFrom(*pFocusing, wgt);
				else
					DoReleaseFocus(*pFocusing);
			}
			pFocusing = &wgt;
			return true;
		}
	}
	return false;
}

bool
DoReleaseFocus(IWidget& wgt)
{
	if(const auto p = FetchContainerPtr(wgt))
	{
		auto& pFocusing(p->GetView().FocusingPtr);

		if(pFocusing == &wgt)
		{
			pFocusing = {};
			return true;
		}
	}
	return false;
}

void
RequestFocusFrom(IWidget& dst, IWidget& src)
{
	if(DoRequestFocus(dst, true))
		CallEvent<GotFocus>(dst, UIEventArgs(src));
}

void
ReleaseFocusFrom(IWidget& dst, IWidget& src)
{
	if(DoReleaseFocus(dst))
		CallEvent<LostFocus>(dst, UIEventArgs(src));
}

void
ClearFocusingOf(IWidget& wgt)
{
	if(const auto p = FetchFocusingPtr(wgt))
	{
		wgt.GetView().FocusingPtr = {};
		CallEvent<LostFocus>(*p, UIEventArgs(wgt));
	}
}

void
RequestFocusCascade(IWidget& wgt)
{
	auto p(&wgt);

	do
	{
		RequestFocus(*p);
	}while((p = FetchContainerPtr(*p)));
}

void
ReleaseFocusCascade(IWidget& wgt)
{
	auto p(&wgt);

	do
	{
		ReleaseFocus(*p);
	}while((p = FetchContainerPtr(*p)));
}

} // namespace UI;

} // namespace YSLib;

