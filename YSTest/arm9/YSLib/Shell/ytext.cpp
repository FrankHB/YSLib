// YSLib::Shell::YText by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 00:06:05 + 08:00;
// UTime = 2010-11-03 19:55 + 08:00;
// Version = 0.6238;


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
SetPensTo(TextState& s)
{
	s.PenX = s.Margin.Left;
	//	s.PenY = s.Margin.Top + GetLnHeightExFrom();
	//	s.PenY = s.Margin.Top + pCache->GetAscender();
	SetLnNNowTo(s, 0);
}

void
SetLnNNowTo(TextState& s, u16 n)
{
	s.PenY = s.Margin.Top + s.GetCache().GetAscender() + GetLnHeightExFrom(s) * n;
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
			xmax(vmin<int>(buf.Width - ts.Margin.Right - dx,
				sbit.GetWidth())),
			ymax(vmin<int>(buf.Height - ts.Margin.Bottom - dy,
				sbit.GetHeight()));

		if(xmax >= xmin && ymax >= ymin)
		{
			const int sJmp(sbit.GetWidth() - xmax + xmin),
				dJmp(buf.Width - xmax + xmin),
				dOff(vmax<int>(ts.Margin.Top, dy) * buf.Width
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
			xmax(vmin<int>(buf.Width - ts.Margin.Right - dx,
				sbit.GetWidth())),
			ymax(vmin<int>(buf.Height - ts.Margin.Bottom - dy,
				sbit.GetHeight()));

		if(xmax >= xmin && ymax >= ymin)
		{
			const int sJmp(sbit.GetWidth() - xmax + xmin),
				dJmp(buf.Width - xmax + xmin),
				dOff(vmax<int>(ts.Margin.Top, dy) * buf.Width
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
	SetPensTo(*this);
}
TextRegion::TextRegion(Drawing::Font& font)
	: TextState(font), BitmapBufferEx()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	SetPensTo(*this);
}
TextRegion::TextRegion(YFontCache& fc)
	: TextState(fc), BitmapBufferEx()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	SetPensTo(*this);
}
TextRegion::~TextRegion()
{}

SDST
TextRegion::GetMarginResized() const
{
	const u8 t(GetLnHeightExFrom(*this));

	return t ? Margin.Bottom
		+ (Height + LineGap - Margin.Top - Margin.Bottom) % t : 0;
}
SDST
TextRegion::GetBufferHeightResized() const
{
	const u8 t(GetLnHeightExFrom(*this));

	return t ? Margin.Top + (Height + LineGap - Margin.Top - Margin.Bottom)
		/ t * t : Height;
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
	return Height - Margin.Bottom + GetCache().GetDescender();
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
	if(l > Height)
		return;
	if(!n)
		--n;
	if(img && imgAlpha)
	{
		const u32 t((l + n > Height ? Height - l : n) * Width),
			s(l * Width);

		memset(&img[s], 0, t * sizeof(PixelType));
		memset(&imgAlpha[s], 0, t * sizeof(u8));
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

	ClearLine(Height - Margin.Bottom - h, h);
}

void
TextRegion::Move(s16 n)
{
	if(Height > Margin.Bottom)
		Move(n, Height - Margin.Bottom);
}
void
TextRegion::Move(s16 n, SDST h)
{
	if(img && imgAlpha)
	{
		const s32 t(((h + Margin.Bottom > Height ? Height - Margin.Bottom : h)
			- Margin.Top - abs(n)) * Width);

		if(n && t > 0)
		{
			u32 d(Margin.Top), s(d);

			if(n > 0)
				d += n;
			else
				s -= n;
			s *= Width;
			d *= Width;
			std::memmove(&img[d], &img[s], t * sizeof(PixelType));
			std::memmove(&imgAlpha[d], &imgAlpha[s], t * sizeof(u8));
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
	if(PenX + GetCache().GetAdvance(c) >= Width - Margin.Right)
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

