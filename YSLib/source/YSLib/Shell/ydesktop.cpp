/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydesktop.cpp
\ingroup Shell
\brief 平台无关的桌面抽象层。
\version 0.2201;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-02 12:00:08 +0800;
\par 修改时间:
	2011-04-13 08:15 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YDesktop;
*/


#include "ydesktop.h"
#include <algorithm>

YSL_BEGIN

using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

using namespace Controls;

YDesktop::YDesktop(YScreen& s, Color c, GHStrong<YImage> i)
	: YFrame(Rect::FullScreen, i),
	Screen(s), sDOs()
{
	BackColor = c;
}

void
YDesktop::operator+=(IControl* p)
{
	if(p && std::find(sDOs.begin(), sDOs.end(), p) == sDOs.end())
	{
		sDOs.push_back(p);
		GMFocusResponser<IControl>::operator+=(*p);
		p->GetContainerPtr() = this;
		bRefresh = true;
	}
}
bool
YDesktop::operator-=(IControl* p)
{
	if(p)
	{
		GMFocusResponser<IControl>::operator-=(*p);
		sDOs.erase(std::remove(sDOs.begin(), sDOs.end(), p), sDOs.end()); 
		p->GetContainerPtr() = NULL;
		bRefresh = true;
		return true;
	}
	return false;
}

IControl*
YDesktop::GetFirstDesktopObjectPtr() const
{
	return sDOs.empty() ? NULL : sDOs.front();
}
IControl*
YDesktop::GetTopDesktopObjectPtr() const
{
	return sDOs.empty() ? NULL : sDOs.back();
}
IControl*
YDesktop::GetTopVisibleDesktopObjectPtr(const Point& pt) const
{
	for(DOs::const_reverse_iterator i(sDOs.rbegin()); i != sDOs.rend(); ++i)
	{
		try
		{
			if((*i)->IsVisible() && Contains(**i, pt))
				return *i;
		}
		catch(std::bad_cast&)
		{}
	}
	return NULL;
}

bool
YDesktop::MoveToTop(IControl& w)
{
	DOs::iterator i(std::find(sDOs.begin(), sDOs.end(), &w));

	if(i != sDOs.end())
	{
		std::swap(*i, sDOs.back());
		bRefresh = true;
		return true;
	}
	return false;
}

void
YDesktop::RemoveTopDesktopObject()
{
	if(!sDOs.empty())
	{
		sDOs.back()->ReleaseFocus(GetStaticRef<EventArgs>());
		sDOs.pop_back();
		bRefresh = true;
	}
}

void
YDesktop::ClearContents()
{
	ClearFocusingPtr();
	sDOs.clear();
	sWgtSet.clear();
	sFOCSet.clear();
	bRefresh = true;
}

bool
YDesktop::DrawContents()
{
	bool result(ParentType::DrawContents());
//	Draw();

	for(DOs::iterator i(sDOs.begin()); i != sDOs.end(); ++i)
	{
		IControl* const p(*i);

		YAssert(p, "Null control pointer found @ YDesktop::DrawContents");

		if(p->IsVisible())
		{
			IWindow* const pWnd(dynamic_cast<IWindow*>(p));

			if(pWnd)
			{
				result = true;
				pWnd->Refresh();
				pWnd->Update();
			}
			else
				p->Draw();
		}
	}
	return result;
}

void
YDesktop::Update()
{
	if(bRefresh)
		bUpdate = false;
	if(bUpdate)
		Screen.Update(GetContext().GetBufferPtr());
}

YSL_END_NAMESPACE(Components)

YSL_END

