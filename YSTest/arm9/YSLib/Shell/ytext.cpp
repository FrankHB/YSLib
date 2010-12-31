/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytext.cpp
\ingroup Shell
\brief 基础文本显示。
\version 0.6291;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 00:06:05 + 08:00;
\par 修改时间:
	2010-12-31 12:16 + 08:00;
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

#define INVISIBLE_CHAR(c) ((c) == ' ' || (c) == '\t' || (c)== '\r')

static const u8 Alpha_Threshold = 16;

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


void
PrintChar(BitmapBuffer& buf, TextState& ts, fchar_t c)
{
	if(!buf.GetBufferPtr())
		//无缓冲区时无法绘图。
		return;

	YFontCache& cache(ts.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));
	const int tx(ts.PenX + cache.GetAdvance(c, sbit));

	if(!INVISIBLE_CHAR(c) && sbit.GetBuffer())
	{
		PixelType col(ts.Color | BITALPHA);
		const int dx(ts.PenX + sbit.GetLeft()),
			dy(ts.PenY - sbit.GetTop()),
			xmin(vmax<int>(0, ts.Margin.Left - dx)),
			ymin(vmax<int>(0, ts.Margin.Top - dy)),
			xmax(vmin<int>(buf.GetWidth() - ts.Margin.Right - dx,
				sbit.GetWidth())),
			ymax(vmin<int>(buf.GetHeight() - ts.Margin.Bottom - dy,
				sbit.GetHeight()));

		if(xmax >= xmin && ymax >= ymin)
		{
			const int sJmp(sbit.GetWidth() - xmax + xmin),
				dJmp(buf.GetWidth() - xmax + xmin),
				dOff(vmax<int>(ts.Margin.Top, dy) * buf.GetWidth()
					+ vmax<int>(ts.Margin.Left, dx));
			u8* sa(sbit.GetBuffer() + ymin * sbit.GetWidth() + xmin);
			BitmapPtr dc(buf.GetBufferPtr() + dOff);

			for(int y(ymin); y < ymax; ++y)
			{
				for(int x(xmin); x < xmax; ++x)
				{
					if(*sa >= Alpha_Threshold)
						*dc = col;
					++sa;
					++dc;
				}
				sa += sJmp;
				dc += dJmp;
			}
		}
	}
	//移动笔。
	ts.PenX = tx;
}

void
PrintCharEx(BitmapBufferEx& buf, TextState& ts, fchar_t c)
{
	if(!buf.GetBufferPtr())
		//无缓冲区时无法绘图。
		return;

	YFontCache& cache(ts.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));
	const int tx(ts.PenX + cache.GetAdvance(c, sbit));

	if(!INVISIBLE_CHAR(c) && sbit.GetBuffer())
	{
		PixelType col(ts.Color | BITALPHA);
		const int dx(ts.PenX + sbit.GetLeft()),
			dy(ts.PenY - sbit.GetTop()),
			xmin(vmax<int>(0, ts.Margin.Left - dx)),
			ymin(vmax<int>(0, ts.Margin.Top - dy)),
			xmax(vmin<int>(buf.GetWidth() - ts.Margin.Right - dx,
				sbit.GetWidth())),
			ymax(vmin<int>(buf.GetHeight() - ts.Margin.Bottom - dy,
				sbit.GetHeight()));

		if(xmax >= xmin && ymax >= ymin)
		{
			const int sJmp(sbit.GetWidth() - xmax + xmin),
				dJmp(buf.GetWidth() - xmax + xmin),
				dOff(vmax<int>(ts.Margin.Top, dy) * buf.GetWidth()
					+ vmax<int>(ts.Margin.Left, dx));
			u8* sa(sbit.GetBuffer() + ymin * sbit.GetWidth() + xmin);
			BitmapPtr dc(buf.GetBufferPtr() + dOff);
			u8* da(buf.GetBufferAlphaPtr() + dOff);

			for(int y(ymin); y < ymax; ++y)
			{
				for(int x(xmin); x < xmax; ++x)
				{
					if(*sa >= Alpha_Threshold)
					{
						*da = *sa;
						*dc = col;
					}
					++sa;
					++dc;
					++da;
				}
				sa += sJmp;
				dc += dJmp;
				da += dJmp;
			}
		}
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
TextRegion::~TextRegion()
{}

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
	PrintCharEx(*this, *this, c);
	return 0;
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

