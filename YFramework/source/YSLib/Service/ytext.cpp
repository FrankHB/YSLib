/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytext.cpp
\ingroup Service
\brief 基础文本显示。
\version r6872;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-13 00:06:05 +0800;
\par 修改时间:
	2011-12-18 12:41 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YText;
*/


#include "YSLib/Service/ytext.h"
#include "YSLib/Service/yblit.h"

using namespace ystdex;

YSL_BEGIN

using namespace Drawing;
using namespace Text;

YSL_BEGIN_NAMESPACE(Drawing)

TextState::TextState()
	: PenStyle(FetchDefaultFontFamily()), Margin(),
	PenX(0), PenY(0), LineGap(0)
{}
TextState::TextState(Drawing::Font& font)
	: PenStyle(font.GetFontFamily()), Margin(),
	PenX(0), PenY(0), LineGap(0)
{}
TextState::TextState(FontCache& fc)
	: PenStyle(*fc.GetDefaultTypefacePtr()->GetFontFamilyPtr()), Margin(),
	PenX(0), PenY(0), LineGap(0)
{}

/*Typeface*
TextState::SetFont(const char* name, CPATH filename)
{
	return pFont = GetCache().SetFont(name, filename);
}*/

void
TextState::PutNewline()
{
	CarriageReturn(*this);
	PenY += GetTextLineHeightExOf(*this);
}

void
TextState::ResetPen()
{
	CarriageReturn(*this);
	//	PenY = Margin.Top + GetTextLineHeightExOf(*this);
	//	PenY = Margin.Top + pCache->GetAscender();
	SetCurrentTextLineNOf(*this, 0);
}

void
TextState::ResetForBounds(const Rect& r, const Size& s, const Padding& m)
{
	Margin = FetchMargin(r + m, s);
	yunseq(PenX = r.X + m.Left,
		PenY = r.Y + GetCache().GetAscender() + m.Top);
}


void
SetCurrentTextLineNOf(TextState& s, u16 n)
{
	s.PenY = s.Margin.Top + s.GetCache().GetAscender()
		+ GetTextLineHeightExOf(s) * n;
}

void
MovePen(TextState& ts, ucs4_t c)
{
	FontCache& cache(ts.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));

	ts.PenX += cache.GetAdvance(c, sbit);
}


namespace
{
	const u8 BLT_TEXT_ALPHA_THRESHOLD(16);
	PixelType char_color;

	template<bool _bPositiveScan>
	struct BlitTextLoop
	{
		void
		operator()(int delta_x, int delta_y,
			pair_iterator<BitmapPtr, u8*> dst_iter, u8* src_iter,
			int dst_inc, int src_inc)
		{
			for(; delta_y > 0; --delta_y)
			{
				for(int x(0); x < delta_x; ++x)
				{
					if(*src_iter >= BLT_TEXT_ALPHA_THRESHOLD)
					{
						*dst_iter.base().second = *src_iter;
						*dst_iter = char_color;
					}
					++src_iter;
					xcrease<_bPositiveScan>(dst_iter);
				}
				src_iter += src_inc;
				delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
			}
		}
	};
}


void
RenderChar(ucs4_t c, TextState& ts, const Graphics& g, const Rect& mask,
	u8* alpha)
{
	//无缓冲区时无法绘图。
	if(g.IsValid())
	{
		using namespace ystdex;

		FontCache& cache(ts.GetCache());
		CharBitmap sbit(cache.GetGlyph(c));
		const int tx(ts.PenX + cache.GetAdvance(c, sbit));

		if(std::iswgraph(c) && sbit.GetBuffer())
		{
			const auto& left(ts.Margin.Left);
			const auto& top(ts.Margin.Top);
			const auto& sbw(sbit.GetWidth());
			const auto& sbh(sbit.GetHeight());
			const auto dx(ts.PenX + sbit.GetLeft()),
				dy(ts.PenY - sbit.GetTop()),
				xmin(max<int>(0, left - dx)),
				ymin(max<int>(0, top - dy));
			const Size& ds(g.GetSize());
			const Size ss(sbw, sbh);
			const Point dp(max<int>(left, dx), max<int>(top, dy));
			Point sp(xmin, ymin);
			const Size sc(max<int>(min<int>(g.GetWidth() - ts.Margin.Right
				- dx, sbw) - xmin, 0), max<int>(min<int>(g.GetHeight()
				- ts.Margin.Bottom - dy, sbh) - ymin, 0));
			const Rect r(Intersect(mask, Rect(dp, sc)));

			sp += r.GetPoint() - dp;
			if(alpha)
			{
				char_color = ts.Color | BITALPHA;
				Blit<BlitTextLoop, false, false>(pair_iterator<BitmapPtr,
					u8*>(g.GetBufferPtr(), alpha), ds, sbit.GetBuffer(), ss,
					r, sp, r);
			}
			else
				Blit<BlitBlendLoop, false, false>(g.GetBufferPtr(), ds,
					MonoIteratorPair(pseudo_iterator<const PixelType>(
					ts.Color | BITALPHA), sbit.GetBuffer()), ss, r, sp, r);
		}
		//移动笔。
		ts.PenX = tx;
	}
}


SDst
FetchResizedBottomMargin(const TextState& ts, SDst h)
{
	const u8 t(GetTextLineHeightExOf(ts));

	return t ? ts.Margin.Bottom + (h + ts.LineGap - GetVerticalOf(ts.Margin))
		% t : 0;
}

u16
FetchResizedLineN(const TextState& ts, SDst h)
{
	YAssert(GetTextLineHeightExOf(ts) != 0,
		"Zero line height found @ TextFetchResizedLineN");

	return (h - GetVerticalOf(ts.Margin)) / GetTextLineHeightExOf(ts);
}

SPos
FetchLastLineBasePosition(const TextState& ts, SDst h)
{
	return h - ts.Margin.Bottom + ts.GetCache().GetDescender() + 1;
}


SDst
FetchCharWidth(FontCache& cache, ucs4_t c)
{
	CharBitmap sbit(cache.GetGlyph(c));

	return cache.GetAdvance(c, sbit);
}


void
TextRenderer::ClearLine(u16 l, SDst n)
{
	const auto& g(GetContext());
	const auto h(g.GetHeight());

	if(g.IsValid() && l < h)
	{
		if(n == 0 || l + n > h)
			n = h - l;
		ClearPixel(g[l], g.GetWidth() * n);
	}
}


TextRegion::TextRegion()
	: GTextRendererBase<TextRegion>(), TextState(), BitmapBufferEx()
{
	InitializeFont();
}
TextRegion::TextRegion(Drawing::Font& fnt)
	: GTextRendererBase<TextRegion>(), TextState(fnt), BitmapBufferEx()
{
	InitializeFont();
}
TextRegion::TextRegion(FontCache& fc)
	: GTextRendererBase<TextRegion>(), TextState(fc), BitmapBufferEx()
{
	InitializeFont();
}

void
TextRegion::InitializeFont()
{
	Font.SetSize(Font::DefaultSize);
	Font.UpdateSize();
	ResetPen();
}

void
TextRegion::ClearLine(u16 l, SDst n)
{
	const auto& g(GetContext());

	if(l > g.GetHeight())
		return;
	if(!n)
		--n;
	if(g.IsValid() && pBufferAlpha)
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
	if(GetHeight() > Margin.Bottom)
		Scroll(n, GetHeight() - Margin.Bottom);
}
void
TextRegion::Scroll(ptrdiff_t n, SDst h)
{
	if(pBuffer && pBufferAlpha)
	{
		const s32 t(((h + Margin.Bottom > GetHeight()
			? GetHeight() - Margin.Bottom : h)
			- Margin.Top - abs(n)) * GetWidth());

		if(n && t > 0)
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
	const String& str)
{
	TextRenderer tr(ts, g, mask);

	PrintLine(tr, str);
}
void
DrawClippedText(const Graphics& g, const Rect& mask, const Rect& r,
	const String& str, const Padding& m, Color color)
{
	TextState ts;

	(ts.Font = Font()).Update(); //设置默认字体。
	ts.ResetForBounds(r, g.GetSize(), m);
	ts.Color = color;
	DrawClippedText(g, mask, ts, str);
}

void
DrawText(const Graphics& g, TextState& ts, const String& str)
{
	DrawClippedText(g, Rect(Point::Zero, g.GetSize()), ts, str);
}
void
DrawText(const Graphics& g, const Rect& r, const String& str, const Padding& m,
	Color color)
{
	DrawClippedText(g, Rect(Point::Zero, g.GetSize()), r, str, m, color);
}
void
DrawText(TextRegion& tr, const Graphics& g, const Point& pt, const Size& s,
	const String& str)
{
	PutLine(tr, str);
	BlitTo(g.GetBufferPtr(), tr, g.GetSize(), Point::Zero, pt, s);
}

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

YSL_END_NAMESPACE(Text)

YSL_END

