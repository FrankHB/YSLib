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
\version 0.6379;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 00:06:05 + 08:00;
\par 修改时间:
	2011-01-29 14:59 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YText;
*/


#include "ytext.h"

YSL_BEGIN

using namespace Drawing;
using namespace Text;

YSL_BEGIN_NAMESPACE(Drawing)

TextState::TextState()
	: PenStyle(GetDefaultFontFamily()), Margin(),
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
TextState::ResetPen()
{
	PenX = Margin.Left;
	//	PenY = Margin.Top + GetLnHeightExFrom(*this);
	//	PenY = Margin.Top + pCache->GetAscender();
	SetLnNNowTo(*this, 0);
}


void
SetLnNNowTo(TextState& s, u16 n)
{
	s.PenY = s.Margin.Top + s.GetCache().GetAscender()
		+ GetLnHeightExFrom(s) * n;
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
PrintChar(BitmapBufferEx& buf, TextState& ts, fchar_t c)
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
			buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), buf.GetSize(),
			sbit.GetBuffer() + ymin * sbit.GetWidth() + xmin,
			Size(sbit.GetWidth(), sbit.GetHeight()), Point::Zero,
			Point(vmax<int>(ts.Margin.Left, dx), vmax<int>(ts.Margin.Top, dy)),
			Size(vmin<int>(buf.GetWidth() - ts.Margin.Right - dx,
				sbit.GetWidth()) - xmin, vmin<int>(buf.GetHeight()
				- ts.Margin.Bottom - dy, sbit.GetHeight()) - ymin));
	}
	//移动笔。
	ts.PenX = tx;
}


TextRegion::TextRegion()
	: TextState(), BitmapBufferEx()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	this->ResetPen();
}
TextRegion::TextRegion(Drawing::Font& font)
	: TextState(font), BitmapBufferEx()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	this->ResetPen();
}
TextRegion::TextRegion(YFontCache& fc)
	: TextState(fc), BitmapBufferEx()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	this->ResetPen();
}

SDST
TextRegion::GetMarginResized() const
{
	const u8 t(GetLnHeightExFrom(*this));

	return t ? Margin.Bottom
		+ (GetHeight() + LineGap - Margin.Top - Margin.Bottom) % t : 0;
}
SDST
TextRegion::GetBufferHeightResized() const
{
	const u8 t(GetLnHeightExFrom(*this));

	return t ? Margin.Top + (GetHeight() + LineGap - Margin.Top - Margin.Bottom)
		/ t * t : GetHeight();
}
u16
TextRegion::GetLnN() const
{
	return GetBufHeightN() / GetLnHeightExFrom(*this);
}
u16
TextRegion::GetLnNEx() const
{
	return (GetBufHeightN() + LineGap) / GetLnHeightExFrom(*this);
}
SPOS
TextRegion::GetLineLast() const
{
	return GetHeight() - Margin.Bottom + GetCache().GetDescender();
}

void
TextRegion::SetLnLast()
{
	const u16 n(GetLnN());

	if(n)
		SetLnNNowTo(*this, n - 1);
}

void
TextRegion::ClearLine(u16 l, SDST n)
{
	if(l > GetHeight())
		return;
	if(!n)
		--n;
	if(pBuffer && pBufferAlpha)
	{
		const u32 t((l + n > GetHeight() ? GetHeight() - l : n) * GetWidth()),
			s(l * GetWidth());

		mmbset(&pBuffer[s], 0, t * sizeof(PixelType));
		mmbset(&pBufferAlpha[s], 0, t * sizeof(u8));
	}
}

void
TextRegion::ClearLn(u16 l)
{
	SDST h(GetLnHeightExFrom(*this));

	ClearLine(Margin.Top + h * l, h);
}

void
TextRegion::ClearLnLast()
{
	SDST h(GetLnHeightExFrom(*this));

	ClearLine(GetHeight() - Margin.Bottom - h, h);
}

void
TextRegion::Move(s16 n)
{
	if(GetHeight() > Margin.Bottom)
		Move(n, GetHeight() - Margin.Bottom);
}
void
TextRegion::Move(s16 n, SDST h)
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
TextRegion::PutNewline()
{
	PenX = Margin.Left;
	PenY += GetLnHeightExFrom(*this);
}

u8
TextRegion::PutChar(fchar_t c)
{
	if(c == '\n')
	{
		PutNewline();
		return 0;
	}
	if(!std::iswprint(c))
		return 0;
/*
	const int maxW = GetBufWidthN() - 1,
		spaceW = pCache->GetAdvance(' ');

	if(maxW < spaceW)
		return lineBreaksL = 1;
*/
	if(PenX + GetCache().GetAdvance(c) >= GetWidth() - Margin.Right)
	{
		PutNewline();
		return 1;
	}
	PrintChar(*this, *this, c);
	return 0;
}


void
DrawText(TextRegion& tr, const Graphics& g, const Point& pt, const Size& s,
	const String& str)
{
	tr.PutLine(str);
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
			tr.PutChar(c);
			++l;
		}
	}
	return l;
}

YSL_END_NAMESPACE(Text)

YSL_END

