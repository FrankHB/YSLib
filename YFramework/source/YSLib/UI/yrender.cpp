/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yrender.cpp
\ingroup UI
\brief 样式无关的图形用户界面部件渲染器。
\version r1472;
\author FrankHB<frankhb1989@gmail.com>
\since build 237 。
\par 创建时间:
	2011-09-03 23:46:22 +0800;
\par 修改时间:
	2011-11-26 15:38 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YRenderer;
*/


#include "YSLib/UI/yrender.h"
#include "YSLib/UI/ycontrol.h"
#include "YSLib/Service/ydraw.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

Rect
Renderer::Refresh(IWidget& wgt, PaintContext&& pc)
{
	YAssert(&wgt.GetRenderer() == this,
		"Invalid widget found @ Render::Refresh;");

	return wgt.Refresh(pc);
}


bool
BufferedRenderer::RequiresRefresh() const
{
	return !rInvalidated.IsEmpty();
}

void
BufferedRenderer::GetInvalidatedArea(Rect& r) const
{
	r = rInvalidated;
}

void
BufferedRenderer::SetSize(const Size& s)
{
	Buffer.SetSize(s.Width, s.Height);
	static_cast<Size&>(rInvalidated) = s;
}

void
BufferedRenderer::CommitInvalidation(const Rect& r)
{
	rInvalidated = Unite(rInvalidated, r);
}

void
BufferedRenderer::FillInvalidation(Color c)
{
	FillRect(Buffer, rInvalidated, c);
}

Rect
BufferedRenderer::Refresh(IWidget& wgt, PaintContext&& pc)
{
	YAssert(&wgt.GetRenderer() == this,
		"Invalid widget found @ BufferedRenderer::Refresh;");

	auto r(Validate(wgt,
		Rect(pc.ClipArea.GetPoint() - GetLocationOf(wgt), pc.ClipArea)));

	Update(wgt, pc);
	return r;
}

void
BufferedRenderer::UpdateTo(const PaintContext& pc) const
{
	const auto& g(pc.Target);
	const auto& r(pc.ClipArea);

	CopyTo(g.GetBufferPtr(), GetContext(), g.GetSize(), r,
		r.GetPoint() - pc.Location, r);
}

Rect
BufferedRenderer::Validate(IWidget& wgt, const Rect& r)
{
	if(RequiresRefresh())
	{
		auto result(wgt.Refresh(PaintContext(GetContext(),
			Point::Zero, r)));

		//清除无效区域：只设置一个分量为零可能会使 CommitInvalidation 结果错误。
		static_cast<Size&>(rInvalidated) = Size::Zero;
		return result;
	}
	return Rect::Empty;
}


void
SetInvalidationOf(IWidget& wgt)
{
	wgt.GetRenderer().CommitInvalidation(Rect(Point::Zero, GetSizeOf(wgt)));
}

void
SetInvalidationToParent(IWidget& wgt)
{
	if(const auto pCon = FetchContainerPtr(wgt))
		pCon->GetRenderer().CommitInvalidation(GetBoundsOf(wgt));
}


void
Invalidate(IWidget& wgt)
{
	InvalidateCascade(wgt, Rect(Point::Zero, GetSizeOf(wgt)));
}

void
InvalidateCascade(IWidget& wgt, const Rect& bounds)
{
	auto pWgt(&wgt);
	Rect r(bounds);

	do
	{
		pWgt->GetRenderer().CommitInvalidation(r);
		pWgt->GetRenderer().GetInvalidatedArea(r);
		r += GetLocationOf(*pWgt);
	}while((pWgt = FetchContainerPtr(*pWgt)));
}

bool
PaintIntersection(IWidget& wgt, PaintEventArgs&& e)
{
	e.ClipArea = Intersect(Rect(e.Location, GetSizeOf(e.GetSender())),
		e.ClipArea);
	if(!e.ClipArea.IsEmptyStrict())
	{
		CallEvent<Paint>(wgt, e);
		return true;
	}
	return false;
}

bool
PaintChild(IWidget& wgt, PaintEventArgs&& e)
{
	e.Location += GetLocationOf(e.GetSender());
	return PaintIntersection(wgt, std::move(e));
}
bool
PaintChild(IWidget& wgt, const PaintContext& pc)
{
	return PaintChild(wgt, PaintEventArgs(wgt, pc));
}

void
Render(IWidget& wgt, PaintContext&& pc)
{
	pc.ClipArea = wgt.GetRenderer().Refresh(wgt, std::move(pc));
}
void
Render(PaintEventArgs&& e)
{
	return Render(e.GetSender(), std::move(static_cast<PaintContext&>(e)));
}

void
Update(const IWidget& wgt, const PaintContext& e)
{
	if(IsVisible(wgt))
		wgt.GetRenderer().UpdateTo(e);
}

Rect
Validate(IWidget& wgt)
{
	return wgt.GetRenderer().Validate(wgt, GetBoundsOf(wgt));
}

YSL_END_NAMESPACE(Components)

YSL_END

