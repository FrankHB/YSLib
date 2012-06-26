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
\version r1575;
\author FrankHB<frankhb1989@gmail.com>
\since build 237 。
\par 创建时间:
	2011-09-03 23:46:22 +0800;
\par 修改时间:
	2012-06-23 11:04 +0800;
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
Renderer::Paint(IWidget& wgt, PaintEventArgs&& e)
{
	YAssert(&e.GetSender().GetRenderer() == this, "Invalid widget found.");

	CallEvent<Components::Paint>(wgt, e);
	return e.ClipArea;
}


bool
BufferedRenderer::RequiresRefresh() const
{
	return bool(rInvalidated);
}

void
BufferedRenderer::SetSize(const Size& s)
{
	Buffer.SetSize(s.Width, s.Height);
	rInvalidated.GetSizeRef() = s;
}

Rect
BufferedRenderer::CommitInvalidation(const Rect& r)
{
	return rInvalidated |= r;
}

Rect
BufferedRenderer::Paint(IWidget& wgt, PaintEventArgs&& e)
{
	YAssert(&e.GetSender().GetRenderer() == this, "Invalid widget found.");

	const auto& r(Validate(wgt, e.GetSender(), e));

	UpdateTo(e);
	return r;
}

void
BufferedRenderer::UpdateTo(const PaintContext& pc) const
{
	const auto& g(pc.Target);
	const Rect& r(pc.ClipArea);

	CopyTo(g.GetBufferPtr(), GetContext(), g.GetSize(), r.GetPoint(),
		r.GetPoint() - pc.Location, r.GetSize());
}

Rect
BufferedRenderer::Validate(IWidget& wgt, IWidget& sender,
	const PaintContext& pc)
{
	if(RequiresRefresh())
	{
		const auto& l(GetLocationOf(sender));

		if(!IgnoreBackground && FetchContainerPtr(sender))
			Invalidate(sender);

		const Rect& clip(pc.ClipArea & (rInvalidated + l));

		if(!IgnoreBackground && FetchContainerPtr(sender))
		{
			const auto& g(GetContext());

			CopyTo(g.GetBufferPtr(), pc.Target, g.GetSize(),
				clip.GetPoint() - pc.Location, clip.GetPoint(), clip.GetSize());
		}

		PaintEventArgs e(sender, PaintContext(GetContext(), Point::Zero,
			clip - l));

		CallEvent<Components::Paint>(wgt, e);
		//清除无效区域：只设置一个分量为零可能会使 CommitInvalidation 结果错误。
		rInvalidated.GetSizeRef() = Size::Zero;
		return e.ClipArea;
	}
	return Rect::Empty;
}

YSL_END_NAMESPACE(Components)

YSL_END

