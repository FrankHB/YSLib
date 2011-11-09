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
\version r1251;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-09-03 23:46:22 +0800;
\par 修改时间:
	2011-11-05 11:27 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YRenderer;
*/


#include "YSLib/UI/yrender.h"
#include "YSLib/UI/ywgtevt.h"
#include "YSLib/Service/ydraw.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

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
BufferedRenderer::ClearInvalidation()
{
	//只设置一个分量为零可能会使 CommitInvalidation 结果错误。
	static_cast<Size&>(rInvalidated) = Size::Zero;
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

void
BufferedRenderer::UpdateTo(const PaintContext& e) const
{
	const auto& g(e.Target);
	const auto& r(e.ClipArea);

	CopyTo(g.GetBufferPtr(), GetContext(), g.GetSize(), r,
		r.GetPoint() - e.Location, r);
}

YSL_END_NAMESPACE(Components)

YSL_END

