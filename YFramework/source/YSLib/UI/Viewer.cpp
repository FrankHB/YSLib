/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Viewer.cpp
\ingroup UI
\brief 样式无关的视图。
\version r157
\author FrankHB <frankhb1989@gmail.com>
\since build 525
\par 创建时间:
	2014-08-08 14:39:59 +0800
\par 修改时间:
	2014-08-08 14:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Viewer
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Viewer

namespace YSLib
{

namespace UI
{

bool
SequenceViewer::Contains(size_t i) const
{
	return Length != 0 && IsInInterval<size_t>(i - GetHeadIndex(), Length);
}

size_t
SequenceViewer::GetValid(size_t total) const ynothrow
{
	return min<size_t>(total - GetHeadIndex(), Length);
}

bool
SequenceViewer::SetHeadIndex(size_t t, size_t total)
{
	if(t < total && t != head)
	{
		if(t == 0)
			MoveViewerToBegin();
		else if(total < Length + t)
			MoveViewerToEnd(total);
		else
			head = t;
		return true;
	}
	return {};
}
bool
SequenceViewer::SetSelectedIndex(size_t t, size_t total)
{
	if(t < total && !(t == selected && is_selected))
	{
		selected = t;
		RestrictView();
		is_selected = true;
		return true;
	}
	return {};
}

bool
SequenceViewer::AdjustForContent(size_t total)
{
	if(total != 0)
	{
		if(!(selected < total))
			selected = total - 1;
		return RestrictView();
	}
	else
		Reset();
	return true;
}

SequenceViewer&
SequenceViewer::IncreaseHead(ptrdiff_t d, size_t total)
{
	ptrdiff_t t(head + d);

	RestrictInInterval(t, 0, ptrdiff_t(total));
	SetHeadIndex(size_t(t), total);
	return *this;
}

SequenceViewer&
SequenceViewer::IncreaseSelected(ptrdiff_t d, size_t total)
{
	ptrdiff_t t(selected + d);

	RestrictInInterval(t, 0, ptrdiff_t(total));
	SetSelectedIndex(size_t(t), total);
	return *this;
}

bool
SequenceViewer::MoveViewerToBegin()
{
	if(head)
	{
		head = 0;
		return true;
	}
	return {};
}

bool
SequenceViewer::MoveViewerToEnd(size_t total)
{
	if(total < Length)
		return {};
	head = total - Length;
	return true;
}

bool
SequenceViewer::RestrictSelected()
{
	if(selected < head)
		selected = head;
	else if(selected < head + Length)
		return {};
	else
		selected = head + Length - 1;
	return true;
}

bool
SequenceViewer::RestrictView()
{
	if(selected < head)
		head = selected;
	else if(selected < head + Length)
		return {};
	else
		head = selected + 1 - Length;
	return true;
}

} // namespace UI;

} // namespace YSLib;

