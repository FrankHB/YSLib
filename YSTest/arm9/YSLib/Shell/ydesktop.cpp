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
\version 0.2092;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-02 12:00:08 + 08:00;
\par 修改时间:
	2011-01-14 06:50 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YDesktop;
*/


#include "ydesktop.h"

YSL_BEGIN

using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

using namespace Controls;

YDesktop::YDesktop(YScreen& s, Color c, GHStrong<YImage> i)
	: YFrameWindow(Rect::FullScreen, i),
	Screen(s), sDOs()
{
	BackColor = c;
}

void
YDesktop::operator+=(IVisualControl& w)
{
	if(std::find(sDOs.begin(), sDOs.end(), &w) == sDOs.end())
	{
		sDOs.push_back(&w);
		GMFocusResponser<IVisualControl>::operator+=(w);
		bRefresh = true;
	}
}
bool
YDesktop::operator-=(IVisualControl& w)
{
	DOs::iterator i(std::find(sDOs.begin(), sDOs.end(), &w));

	if(i == sDOs.end())
		return false;
	w.CheckRemoval(*this);
	sDOs.erase(i);
	bRefresh = true;
	return true;
}
YDesktop::DOs::size_type
YDesktop::RemoveAll(IVisualControl& w)
{
	DOs::size_type n(0);
	DOs::iterator i;

	while((i = std::find(sDOs.begin(), sDOs.end(), &w)) != sDOs.end())
	{
		w.CheckRemoval(*this);
		sDOs.erase(i);
		++n;
	}
	bRefresh = true;
	return n;
}

bool
YDesktop::MoveToTop(IVisualControl& w)
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
		sDOs.back()->CheckRemoval(*this);
		sDOs.pop_back();
		bRefresh = true;
	}
}

void
YDesktop::ClearDesktopObjects()
{
	ClearFocusingPtr();
	sDOs.clear();
	bRefresh = true;
}

IVisualControl*
YDesktop::GetFirstDesktopObjectPtr() const
{
	return sDOs.empty() ? NULL : sDOs.front();
}
IVisualControl*
YDesktop::GetTopDesktopObjectPtr() const
{
	return sDOs.empty() ? NULL : sDOs.back();
}
IVisualControl*
YDesktop::GetTopDesktopObjectPtr(const Point& pt) const
{
	for(DOs::const_reverse_iterator i(sDOs.rbegin()); i != sDOs.rend(); ++i)
	{
		try
		{
			if(Contains(**i, pt))
				return *i;
		}
		catch(std::bad_cast&)
		{}
	}
	return NULL;
}

void
YDesktop::DrawBackground()
{
	YWindowAssert(this, YDesktop, DrawBackground);

	if(prBackImage)
	{
		const BitmapPtr buf(GetContext().GetBufferPtr());

		if(buf)
		{
			const BitmapPtr imgBg(prBackImage->GetImagePtr());

			if(imgBg)
			{
				mmbcpy(buf, imgBg, GetContext().GetSizeOfBuffer());
				return;
			}
		}
	}
	BeFilledWith(BackColor);
}

void
YDesktop::DrawDesktopObjects()
{
	YWindowAssert(this, YDesktop, DrawDesktopObjects);

	for(DOs::iterator i(sDOs.begin()); i != sDOs.end(); ++i)
	{
		try
		{
			IWindow& w(dynamic_cast<IWindow&>(**i));
			w.Refresh();
			w.Update();
		}
		catch(std::bad_cast&)
		{}
	}
}

void
YDesktop::Draw()
{
	DrawBackground();
	DrawDesktopObjects();
	bUpdate = true;
}

void
YDesktop::Refresh()
{
	if(bRefresh)
	{
		Draw();
		bRefresh = false;
	}
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

