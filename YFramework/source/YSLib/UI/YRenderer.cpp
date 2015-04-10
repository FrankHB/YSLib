/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YRenderer.cpp
\ingroup UI
\brief 样式无关的 GUI 部件渲染器。
\version r677
\author FrankHB <frankhb1989@gmail.com>
\since build 237
\par 创建时间:
	2011-09-03 23:46:22 +0800
\par 修改时间:
	2015-04-04 11:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YRenderer
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YRenderer
#include YFM_YSLib_UI_YControl
#include YFM_YSLib_Service_YGDI

namespace YSLib
{

namespace UI
{

Rect
Renderer::Paint(IWidget& wgt, PaintEventArgs&& e)
{
	YAssert(&e.GetSender().GetRenderer() == this, "Invalid widget found.");
	if(YB_LIKELY(!e.ClipArea.IsUnstrictlyEmpty()))
		CallEvent<UI::Paint>(wgt, e);
	return e.ClipArea;
}


ImplDeDtor(PseudoRenderer)


BufferedRenderer::BufferedRenderer(bool b, unique_ptr<Drawing::IImage> p)
	: Renderer(),
	rInvalidated(), pImageBuffer(p ? std::move(p)
	: make_unique<CompactPixmap>()), IgnoreBackground(b)
{}
BufferedRenderer::BufferedRenderer(const BufferedRenderer& rd)
	: Renderer(rd),
	rInvalidated(rd.rInvalidated), pImageBuffer(ClonePolymorphic(
	rd.pImageBuffer)), IgnoreBackground(rd.IgnoreBackground)
{}

bool
BufferedRenderer::RequiresRefresh() const
{
	return bool(rInvalidated);
}

void
BufferedRenderer::SetImageBuffer(unique_ptr<Drawing::IImage> p)
{
	pImageBuffer = p ? std::move(p) : make_unique<Drawing::CompactPixmap>();
}

void
BufferedRenderer::SetSize(const Size& s)
{
	GetImageBuffer().SetSize(s);
	rInvalidated = {{}, s};
}

Rect
BufferedRenderer::CommitInvalidation(const Rect& r)
{
	return rInvalidated |= r;
}

Rect
BufferedRenderer::Paint(IWidget& wgt, PaintEventArgs&& e)
{
	const Rect& r(Validate(wgt, e.GetSender(), e));

	UpdateTo(e);
	return r;
}

void
BufferedRenderer::UpdateTo(const PaintContext& pc) const
{
	const auto& g(pc.Target);
	const Rect& bounds(pc.ClipArea);

	CopyTo(g.GetBufferPtr(), GetContext(), g.GetSize(), bounds.GetPoint(),
		bounds.GetPoint() - pc.Location, bounds.GetSize());
}

Rect
BufferedRenderer::Validate(IWidget& wgt, IWidget& sender,
	const PaintContext& pc)
{
	YAssert(&sender.GetRenderer() == this, "Invalid widget found.");
	if(RequiresRefresh())
	{
		if(!IgnoreBackground && FetchContainerPtr(sender))
			Invalidate(sender);

		const Rect& clip(pc.ClipArea & (rInvalidated + pc.Location));

		if(!clip.IsUnstrictlyEmpty())
		{
			const auto& g(GetContext());

			if(!IgnoreBackground && FetchContainerPtr(sender))
			{
				const auto dst(g.GetBufferPtr());
				const auto& src(pc.Target);

				if(dst != src.GetBufferPtr())
					CopyTo(g.GetBufferPtr(), src, g.GetSize(), clip.GetPoint()
						- pc.Location, clip.GetPoint(), clip.GetSize());
			}

			PaintEventArgs e(sender,
				{g, Point(), (clip - pc.Location) & Rect(g.GetSize())});

			CallEvent<UI::Paint>(wgt, e);
			// NOTE: To keep %CommitInvalidation result correct, both
			//	components of the size shall be reset.
			rInvalidated.GetSizeRef() = {};
			return e.ClipArea;
		}
	}
	return {};
}

} // namespace UI;

} // namespace YSLib;

