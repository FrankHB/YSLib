/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.cpp
\ingroup UI
\brief 图形用户界面焦点特性。
\version r597
\author FrankHB <frankhb1989@gmail.com>
\since build 258
\par 创建时间:
	2010-05-01 13:52:56 +0800
\par 修改时间:
	2014-05-23 09:52 +0800
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
				{
					IWidget* p_sub(pFocusing);

					for(auto p_foc = p_sub; p_foc;
						p_foc = FetchFocusingPtr(*p_sub))
						p_sub = p_foc;
					for(; p_sub != pFocusing; p_sub = FetchContainerPtr(*p_sub))
					{
						YAssert(p_sub, "Wrong child focus state found.");
						if(DoReleaseFocus(*p_sub))
							CallEvent<LostFocus>(*p_sub, RoutedEventArgs(wgt));
					}
					ReleaseFocusFrom(*pFocusing, wgt);
				}
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

bool
IsFocusedCascade(const IWidget& wgt, const IWidget* p_top)
{
	auto p_wgt(&wgt);

	while(auto p_con = FetchContainerPtr(*p_wgt))
	{
		if(p_con == p_top)
			break;
		if(FetchFocusingPtr(*p_con) != p_wgt)
			return false;
		p_wgt = p_con;
	}
	return true;
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

