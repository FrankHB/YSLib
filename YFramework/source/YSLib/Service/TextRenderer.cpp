/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextRenderer.cpp
\ingroup Service
\brief 文本渲染。
\version r2677
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2013-08-23 12:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextRenderer
*/


#include "YSLib/Service/TextRenderer.h"
#include "YSLib/Service/yblit.h"
#include "YSLib/Service/TextLayout.h" // for FetchLastLineBasePosition;

using namespace ystdex;

namespace YSLib
{

using namespace Drawing;
using namespace Text;

namespace Drawing
{

namespace
{

//! \since build 372
PaintContext
ClipChar(const Graphics& g, const Point& pen, const CharBitmap& cbmp, Rect r)
{
	YAssert(bool(g), "Invalid graphics context found.");

	const auto pt(ClipBounds(r, Rect(pen.X + cbmp.GetLeft(),
		pen.Y - cbmp.GetTop(), cbmp.GetWidth(), cbmp.GetHeight())));

	return {g, pt, r};
}

//! \since build 415
SDst
FetchBMPSrcWidth(const CharBitmap& cbmp)
{
	const SDst abs_pitch(std::abs(cbmp.GetPitch()));

	switch(cbmp.GetFormat())
	{
	case CharBitmap::Mono:
		return abs_pitch * 8;
	case CharBitmap::Gray2:
		return abs_pitch * 4;
	case CharBitmap::Gray4:
		return abs_pitch * 2;
	default:
		break;
	}
	return abs_pitch;
}

//! \since build 368
template<typename _tCharRenderer, _tCharRenderer& _fCharRenderer,
	typename... _tParams>
void
RenderCharFrom(ucs4_t c, const Graphics& g,
	TextState& ts, const Rect& clip, _tParams&&... args)
{
	const auto cbmp(ts.Font.GetGlyph(c));

	if(YB_LIKELY(cbmp))
	{
		// TODO: Show a special glyph when no bitmap found.
		// TODO: Use fast glyph advance fetching for non-graph characters
		//	when possible.
		// TODO: Handle '\t'.
		if(std::iswgraph(c))
			if(const auto cbuf = cbmp.GetBuffer())
			{
				auto&& pc(ClipChar(g, ts.Pen, cbmp, clip));

				// TODO: Test support for bitmaps with negative pitch.
				if(!pc.ClipArea.IsUnstrictlyEmpty())
					_fCharRenderer(std::move(pc), ts.Color, cbmp.GetPitch() < 0,
						cbuf, cbmp.GetFormat(), {FetchBMPSrcWidth(cbmp),
						cbmp.GetHeight()}, yforward(args)...);
			}
		ts.Pen.X += cbmp.GetXAdvance();
	}
}

} // unnamed namespace;

void
TextRenderer::operator()(ucs4_t c)
{
	RenderCharFrom<decltype(RenderChar), RenderChar>(c, GetContext(), State,
		ClipArea);
}

void
TextRenderer::ClearLine(u16 l, SDst n)
{
	const auto& g(GetContext());
	const auto h(g.GetHeight());

	if(YB_LIKELY(bool(g) && l < h))
	{
		if(n == 0 || l + n > h)
			n = h - l;
		ClearPixel(g[l], g.GetWidth() * n);
	}
}


TextRegion::TextRegion()
	: GTextRendererBase<TextRegion>(), TextState(), CompactPixmapEx()
{
	InitializeFont();
}

void
TextRegion::operator()(ucs4_t c)
{
	RenderCharFrom<decltype(RenderCharAlpha), RenderCharAlpha>(c,
		TextRegion::GetContext(), GetTextState(), Rect(GetSize())
		+ GetTextState().Margin, GetBufferAlphaPtr());
}

void
TextRegion::InitializeFont()
{
	Font.SetSize(Font::DefaultSize);
	ResetPen();
}

void
TextRegion::ClearLine(u16 l, SDst n)
{
	const auto& g(GetContext());

	if(YB_UNLIKELY(l > g.GetHeight()))
		return;
	if(!n)
		--n;
	if(YB_LIKELY(bool(g) && pBufferAlpha))
	{
		const u32 t((l + n > g.GetHeight() ? g.GetHeight() - l : n)
			* g.GetWidth());

		yunseq(ClearPixel(g[l], t),
			ClearPixel(&pBufferAlpha[l * g.GetWidth()], t));
	}
}

void
TextRegion::ClearTextLine(u16 l)
{
	auto& ts(GetTextState());
	SDst h(GetTextLineHeightExOf(ts));

	ClearLine(ts.Margin.Top + h * l, h);
}

void
TextRegion::Scroll(ptrdiff_t n)
{
	if(YB_LIKELY(GetHeight() > Margin.Bottom))
		Scroll(n, GetHeight() - Margin.Bottom);
}
void
TextRegion::Scroll(ptrdiff_t n, SDst h)
{
	if(YB_LIKELY(pBuffer && pBufferAlpha))
	{
		const s32 t(((h + Margin.Bottom > GetHeight()
			? GetHeight() - Margin.Bottom : h)
			- Margin.Top - std::abs(n)) * GetWidth());

		if(YB_LIKELY(n && t > 0))
		{
			u32 d(Margin.Top), s(d);

			if(n > 0)
				d += n;
			else
				s -= n;
			yunseq(s *= GetWidth(), d *= GetWidth());
			yunseq(ystdex::pod_move_n(&pBuffer[s], t, &pBuffer[d]),
				ystdex::pod_move_n(&pBufferAlpha[s], t, &pBufferAlpha[d]));
		}
	}
}


void
DrawClippedText(const Graphics& g, const Rect& mask, TextState& ts,
	const String& str, bool line_wrap)
{
	TextRenderer tr(ts, g, mask);

	if(line_wrap)
		PutString(tr, str);
	else
		PutLine(tr, str);
}
void
DrawClippedText(const Graphics& g, const Rect& mask, const Rect& bounds,
	const String& str, const Padding& m, Color c, bool line_wrap,
	const Font& fnt)
{
	TextState ts(fnt);
	const Rect txt_bounds(bounds + m);

	ts.Margin = FetchMargin(txt_bounds, g.GetSize()),
	ts.ResetPen(bounds.GetPoint(), m);
	ts.Color = c;
	DrawClippedText(g, mask & txt_bounds, ts, str, line_wrap);
}

void
DrawText(const Graphics& g, TextState& ts, const String& str, bool line_wrap)
{
	DrawClippedText(g, Rect(g.GetSize()), ts, str, line_wrap);
}
void
DrawText(const Graphics& g, const Rect& bounds, const String& str,
	const Padding& m, Color c, bool line_wrap, const Font& fnt)
{
	DrawClippedText(g, Rect(g.GetSize()), bounds, str, m, c, line_wrap, fnt);
}
void
DrawText(TextRegion& tr, const Graphics& g, const Point& pt, const Size& s,
	const String& str, bool line_wrap)
{
	if(line_wrap)
		PutString(tr, str);
	else
		PutLine(tr, str);
	BlitTo(g.GetBufferPtr(), tr, g.GetSize(), Point(), pt, s);
}

} // namespace Drawing;

} // namespace YSLib;

