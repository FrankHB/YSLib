/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.cpp
\ingroup UI
\brief 样式无关的图形用户界面窗口。
\version r4208;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 +0800;
\par 修改时间:
	2011-11-12 11:24 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWindow;
*/


#include "YSLib/UI/ydesktop.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

AWindow::AWindow(const Rect& r, const shared_ptr<Image>& hImg)
	: Control(r), MBackground(hImg)
{
	SetRenderer(unique_raw(new BufferedRenderer()));
}

bool
AWindow::DrawBackgroundImage()
{
	if(hBgImage)
	{
		const auto& g(GetRenderer().GetContext());

		if(g.IsValid())
		{
			Rect r(Point::Zero, g.GetSize());

			GetRenderer().GetInvalidatedArea(r);
			return CopyTo(g.GetBufferPtr(), *hBgImage, g.GetSize(), r, r, r);
		}
	}
	return false;
}

Rect
AWindow::Refresh(const PaintContext& e)
{
	if(!(IsTransparent() || DrawBackgroundImage()))
		GetRenderer().FillInvalidation(BackColor);
	DrawContents();
	return Rect(e.Location, GetSizeOf(*this));
}

void
AWindow::Update()
{
	if(!GetRenderer().RequiresRefresh())
	{
		const auto pCon(FetchContainerPtr(*this));

		if(pCon)
			Components::Update(*this, PaintContext(FetchContext(*pCon),
				GetLocationOf(*this), GetBoundsOf(*this)));
	}
}


AFrame::AFrame(const Rect& r, const shared_ptr<Image>& hImg)
	: AWindow(r, hImg), MUIContainer()
{}

void
AFrame::operator+=(IWidget& wgt)
{
	MUIContainer::operator+=(wgt);
	wgt.GetView().pContainer = this;
}
void
AFrame::operator+=(AWindow& wnd)
{
	MUIContainer::Add(wnd, DefaultWindowZOrder);
	wnd.GetView().pContainer = this;
}

bool
AFrame::operator-=(IWidget& wgt)
{
	if(FetchContainerPtr(wgt) == this)
	{
		wgt.GetView().pContainer = nullptr;
		if(FetchFocusingPtr(*this) == &wgt)
			GetView().pFocusing = nullptr;
		return MUIContainer::operator-=(wgt);
	}
	return false;
}
bool
AFrame::operator-=(AWindow& wnd)
{
	if(FetchContainerPtr(wnd) == this)
	{
		wnd.GetView().pContainer = nullptr;
		if(FetchFocusingPtr(*this) == &wnd)
			GetView().pFocusing = nullptr;
		return MUIContainer::operator-=(wnd);
	}
	return false;
}

void
AFrame::Add(IWidget& wgt, ZOrderType z)
{
	MUIContainer::Add(wgt, z);
	wgt.GetView().pContainer = this;
}

bool
AFrame::MoveToTop(IWidget& wgt)
{
	auto i(std::find_if(sWidgets.begin(), sWidgets.end(),
		[&](const WidgetMap::value_type& val){
		return val.second == &wgt;
	}));

	if(i != sWidgets.end())
	{
		const ZOrderType z(i->first);

		sWidgets.erase(i);
		sWidgets.insert(make_pair(z, static_cast<IWidget*>(&wgt)));
		Invalidate(wgt);
		return true;
	}
	return false;
}


Frame::Frame(const Rect& r, const shared_ptr<Image>& hImg, IWidget* pCon)
	: AFrame(r, hImg)
{}

bool
Frame::DrawContents()
{
	if(!FetchContext(*this).IsValid())
		return false;

	bool result(GetRenderer().RequiresRefresh());

	for(auto i(sWidgets.begin()); !result && i != sWidgets.end(); ++i)
	{
		IWidget* const p(i->second);

		YAssert(p, "Null widget pointer found @ Frame::DrawContents");

		result |= Components::IsVisible(*p);
	}
	if(result)
		for(auto i(sWidgets.begin()); i != sWidgets.end(); ++i)
		{
			IWidget& wgt(*i->second);

			if(Components::IsVisible(wgt))
			{
				//	pt = LocateOffset(this, Point::Zero, &w);
				Point pt(GetLocationOf(wgt));
				Rect r;

				GetRenderer().GetInvalidatedArea(r);
				r = Intersect(Rect(pt, GetSizeOf(wgt)), r);
				if(!r.IsEmptyStrict())
				{
					PaintEventArgs e(wgt, FetchContext(*this), pt, r);

					CallEvent<Paint>(wgt, e);
					GetRenderer().CommitInvalidation(e.ClipArea);
				}
			}
		}
	return result;
}

YSL_END_NAMESPACE(Components)

YSL_END

