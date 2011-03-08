/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytext.cpp
\ingroup Shell
\brief 基础文本显示。
\version 0.6609;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 00:06:05 +0800;
\par 修改时间:
	2011-03-07 19:27 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YText;
*/


#include "ytext.h"

YSL_BEGIN

using namespace Drawing;
using namespace Text;
using ystdex::vmin;
using ystdex::vmax;

YSL_BEGIN_NAMESPACE(Drawing)

TextState::TextState()
	: PenStyle(FetchDefaultFontFamily()), Margin(),
	PenX(0), PenY(0), LineGap(0)
{}
TextState::TextState(Drawing::Font& font)
	: PenStyle(font.GetFontFamily()), Margin(),
	PenX(0), PenY(0), LineGap(0)
{}
TextState::TextState(YFontCache& fc)
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
	PenX = Margin.Left;
	PenY += GetLnHeightExFrom(*this);
}

void
TextState::ResetPen()
{
	PenX = Margin.Left;
	//	PenY = Margin.Top + GetLnHeightExFrom(*this);
	//	PenY = Margin.Top + pCache->GetAscender();
	SetLnNNowTo(*this, 0);
}

void
TextState::ResetForBounds(const Rect& r, const Size& s, const Padding& m)
{
	Margin = FetchMargin(r, s) + m;
	PenX = r.X + m.Left;	
	PenY = r.Y + GetCache().GetAscender() + m.Top;
}


void
SetLnNNowTo(TextState& s, u16 n)
{
	s.PenY = s.Margin.Top + s.GetCache().GetAscender()
		+ GetLnHeightExFrom(s) * n;
}


void
RenderChar(const Graphics& g, TextState& ts, fchar_t c)
{
	if(!g.GetBufferPtr())
		//无缓冲区时无法绘图。
		return;

	YFontCache& cache(ts.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));
	const int tx(ts.PenX + cache.GetAdvance(c, sbit));

	if(std::iswgraph(c) && sbit.GetBuffer())
	{
		const int dx(ts.PenX + sbit.GetLeft()),
			dy(ts.PenY - sbit.GetTop()),
			xmin(vmax<int>(0, ts.Margin.Left - dx)),
			ymin(vmax<int>(0, ts.Margin.Top - dy));

		Blit<BlitBlendLoop, false, false>(g.GetBufferPtr(), g.GetSize(),
			MonoIteratorPair(ystdex::pseudo_iterator<const PixelType>(
			ts.Color | BITALPHA), sbit.GetBuffer()),
			Size(sbit.GetWidth(), sbit.GetHeight()), Point(xmin, ymin),
			Point(vmax<int>(ts.Margin.Left, dx), vmax<int>(ts.Margin.Top, dy)),
			Size(vmax<int>(vmin<int>(g.GetWidth() - ts.Margin.Right - dx,
				sbit.GetWidth()) - xmin, 0), vmax<int>(vmin<int>(g.GetHeight()
				- ts.Margin.Bottom - dy, sbit.GetHeight()) - ymin, 0)));
	}
	//移动笔。
	ts.PenX = tx;
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
			ystdex::pair_iterator<BitmapPtr, u8*> dst_iter, u8* src_iter,
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
					ystdex::xcrease<_bPositiveScan>(dst_iter);
				}
				src_iter += src_inc;
				ystdex::delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
			}
		}
	};
}

void
RenderChar(BitmapBufferEx& buf, TextState& ts, fchar_t c)
{
	if(!buf.GetBufferPtr())
		//无缓冲区时无法绘图。
		return;

	YFontCache& cache(ts.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));
	const int tx(ts.PenX + cache.GetAdvance(c, sbit));

	if(std::iswgraph(c) && sbit.GetBuffer())
	{
		char_color = ts.Color | BITALPHA;

		const int dx(ts.PenX + sbit.GetLeft()),
			dy(ts.PenY - sbit.GetTop()),
			xmin(vmax<int>(0, ts.Margin.Left - dx)),
			ymin(vmax<int>(0, ts.Margin.Top - dy));

		Blit<BlitTextLoop, false, false>(ystdex::pair_iterator<BitmapPtr, u8*>(
			buf.GetBufferPtr(), buf.GetBufferAlphaPtr()),
			buf.GetSize(), sbit.GetBuffer(),
			Size(sbit.GetWidth(), sbit.GetHeight()), Point(xmin, ymin),
			Point(vmax<int>(ts.Margin.Left, dx), vmax<int>(ts.Margin.Top, dy)),
			Size(vmax<int>(vmin<int>(buf.GetWidth() - ts.Margin.Right - dx,
				sbit.GetWidth()) - xmin, 0), vmax<int>(vmin<int>(buf.GetHeight()
				- ts.Margin.Bottom - dy, sbit.GetHeight()) - ymin, 0)));
	}
	//移动笔。
	ts.PenX = tx;
}


SDST
FetchResizedMargin(const TextState& ts, SDST h)
{
	const u8 t(GetLnHeightExFrom(ts));

	return t ? ts.Margin.Bottom + (h + ts.LineGap - GetVerticalFrom(ts.Margin))
		% t : 0;
}

SDST
FetchResizedBufferHeight(const TextState& ts, SDST h)
{
	const u8 t(GetLnHeightExFrom(ts));

	return t ? ts.Margin.Top + (h + ts.LineGap - GetVerticalFrom(ts.Margin))
		/ t * t : h;
}

SPOS
FetchLastLineBasePosition(const TextState& ts, SDST h)
{
	return h - ts.Margin.Bottom + ts.GetCache().GetDescender();
}


u16
ATextRenderer::GetLnN() const
{
	const TextState& ts(GetTextState());

	return (GetContext().GetHeight() - GetVerticalFrom(ts.Margin))
		/ GetLnHeightExFrom(ts);
}
u16
ATextRenderer::GetLnNEx() const
{
	const TextState& ts(GetTextState());

	return (GetContext().GetHeight() - GetVerticalFrom(ts.Margin)
		+ ts.LineGap) / GetLnHeightExFrom(ts);
}

void
ATextRenderer::SetLnLast()
{
	const u16 n(GetLnN());

	if(n)
		SetLnNNowTo(GetTextState(), n - 1);
}

void
ATextRenderer::ClearLine(u16 l, SDST n)
{
	const Graphics& g(GetContext());

	if(l > g.GetHeight())
		return;
	if(!n)
		--n;
	if(g.IsValid())
		mmbset(g[l], 0, ((l + n > g.GetHeight() ? g.GetHeight() - l : n)
			* g.GetWidth()) * sizeof(PixelType));
}

void
ATextRenderer::ClearLn(u16 l)
{
	SDST h(GetLnHeightExFrom(GetTextState()));

	ClearLine(GetTextState().Margin.Top + h * l, h);
}

void
ATextRenderer::ClearLnLast()
{
	TextState ts(GetTextState());
	SDST h(GetLnHeightExFrom(ts));

	ClearLine(GetContext().GetHeight() - ts.Margin.Bottom - h, h);
}


TextRegion::TextRegion()
	: ATextRenderer(), TextState(), BitmapBufferEx()
{
	InitializeFont();
}
TextRegion::TextRegion(Drawing::Font& font)
	: ATextRenderer(), TextState(font), BitmapBufferEx()
{
	InitializeFont();
}
TextRegion::TextRegion(YFontCache& fc)
	: ATextRenderer(), TextState(fc), BitmapBufferEx()
{
	InitializeFont();
}

void
TextRegion::InitializeFont()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	ResetPen();
}

void
TextRegion::ClearLine(u16 l, SDST n)
{
	const Graphics& g(GetContext());

	if(l > g.GetHeight())
		return;
	if(!n)
		--n;
	if(g.IsValid() && pBufferAlpha)
	{
		const u32 t((l + n > g.GetHeight() ? g.GetHeight() - l : n)
			* g.GetWidth());

		mmbset(g[l], 0, t * sizeof(PixelType));
		mmbset(&pBufferAlpha[l * g.GetWidth()], 0, t * sizeof(u8));
	}
}

void
TextRegion::Scroll(std::ptrdiff_t n)
{
	if(GetHeight() > Margin.Bottom)
		Scroll(n, GetHeight() - Margin.Bottom);
}
void
TextRegion::Scroll(std::ptrdiff_t n, SDST h)
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
			s *= GetWidth();
			d *= GetWidth();
			std::memmove(&pBuffer[d], &pBuffer[s], t * sizeof(PixelType));
			std::memmove(&pBufferAlpha[d], &pBufferAlpha[s], t * sizeof(u8));
		}
	}
}


void
DrawText(const Graphics& g, TextState& ts, const String& str)
{
	TextRenderer tr(ts, g);

	PrintLine(tr, str);
}
void
DrawText(const Graphics& g, const Rect& r, const String& str, const Padding& m,
	Color color)
{
	TextState ts;

	ts.Font.SetFont(Font()); //设置默认字体。
	ts.ResetForBounds(r, g.GetSize(), m);
	ts.Color = color;
	DrawText(g, ts, str);
}
void
DrawText(TextRegion& tr, const Graphics& g, const Point& pt, const Size& s,
	const String& str)
{
	PutLine(tr, str);
	tr.BlitTo(g.GetBufferPtr(), g.GetSize(), Point::Zero,
		pt, s);
}

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

u32
ReadX(YTextFile& f, TextRegion& tr, u32 n)
{
	u32 l(0);

	if(f.IsValid())
	{
		u32 i(0), t;
		FILE* const fp(f.GetPtr());
		const CSID cp(f.GetCP());
		uchar_t c;

		while(i < n && (t = ToUTF(fp, c, cp)))
		{
			i += t;
			PutChar(tr, c);
			++l;
		}
	}
	return l;
}

YSL_END_NAMESPACE(Text)

YSL_END

