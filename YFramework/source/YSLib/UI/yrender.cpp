/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yrender.cpp
\ingroup UI
\brief 样式无关的图形用户界面部件渲染器。
\version r1539;
\author FrankHB<frankhb1989@gmail.com>
\since build 237 。
\par 创建时间:
	2011-09-03 23:46:22 +0800;
\par 修改时间:
	2012-02-04 07:28 +0800;
\par 文本编码:
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
BufferedRenderer::SetSize(const Size& s)
{
	Buffer.SetSize(s.Width, s.Height);
	static_cast<Size&>(rInvalidated) = s;
}

Rect
BufferedRenderer::CommitInvalidation(const Rect& r)
{
	return rInvalidated = Unite(rInvalidated, r);
}

Rect
BufferedRenderer::Refresh(IWidget& wgt, PaintContext&& pc)
{
	YAssert(&wgt.GetRenderer() == this,
		"Invalid widget found @ BufferedRenderer::Refresh;");

	Rect r(Validate(wgt, pc));

	UpdateTo(pc);
	return r;
}

void
BufferedRenderer::UpdateTo(const PaintContext& pc) const
{
	const auto& g(pc.Target);
	const Rect& r(pc.ClipArea);

	CopyTo(g.GetBufferPtr(), GetContext(), g.GetSize(), r,
		r.GetPoint() - pc.Location, r);
}

Rect
BufferedRenderer::Validate(IWidget& wgt, const PaintContext& pc)
{
	if(RequiresRefresh())
	{
		const auto& l(GetLocationOf(wgt));
		const Rect& clip(Intersect(pc.ClipArea, rInvalidated + l));

		if(!IgnoreBackground && FetchContainerPtr(wgt))
		{
			const auto& g(GetContext());

			CopyTo(g.GetBufferPtr(), pc.Target, g.GetSize(),
				clip.GetPoint() - pc.Location, clip, clip);
		}

		Rect result(wgt.Refresh(PaintContext(GetContext(), Point::Zero,
			clip - l)));

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
	Invalidate(wgt, Rect(Point::Zero, GetSizeOf(wgt)));
}
void
Invalidate(IWidget& wgt, const Rect& bounds)
{
	auto pWgt(&wgt);
	Rect r(bounds);

	do
	{
		r = pWgt->GetRenderer().CommitInvalidation(r);
		r += GetLocationOf(*pWgt);
	}while((pWgt = FetchContainerPtr(*pWgt)));
}

void
PaintChild(IWidget& wgt, PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	e.Location += GetLocationOf(sender);
	e.ClipArea = Intersect(e.ClipArea, Rect(e.Location, GetSizeOf(sender)));
	if(!e.ClipArea.IsUnstrictlyEmpty())
		CallEvent<Paint>(wgt, e);
}
void
PaintChild(IWidget& wgt, const PaintContext& pc)
{
	PaintChild(wgt, PaintEventArgs(wgt, pc));
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

YSL_END_NAMESPACE(Components)

YSL_END

