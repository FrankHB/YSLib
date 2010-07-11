// YSLib::Shell::YText by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 0:06:05;
// UTime = 2010-7-9 10:15;
// Version = 0.5848;


#include "ytext.h"
#include <wctype.h>

YSL_BEGIN

using namespace Text;

YSL_BEGIN_NAMESPACE(Drawing)

#define INVISIBLE_CHAR(c) ((c) == ' ' || (c) == '\t' || (c)== '\r')

static const u8 Alpha_Threshold = 16;

YTextState::YTextState()
:YPenStyle(*GetDefaultFontFamilyPtr()), Margin(), pCache(&GetFontFamily().Cache),
penX(0), penY(0), lnGap(0)
{}
YTextState::YTextState(YFont& font)
:YPenStyle(font.GetFontFamily()), Margin(), pCache(&GetFontFamily().Cache),
penX(0), penY(0), lnGap(0)
{}
YTextState::YTextState(YFontCache* fc)
:YPenStyle(*(fc ? fc : pDefaultFontCache)->GetDefaultTypefacePtr()->GetFontFamilyPtr()), Margin(), pCache(&GetFontFamily().Cache),
penX(0), penY(0), lnGap(0)
{}

YTextState& YTextState::operator=(const YTextState& ts)
{
	YPenStyle::operator=(ts);
	Margin = ts.Margin;
//	if (ts.GetFontPtr() && pFont != ts.GetFontPtr())
//		SetFont(DEF_FONT_NAME, ts.GetFontPtr()->file->path);
	if (GetFontSize() != ts.GetFontSize())
		SetFontSize(ts.GetFontSize());
	return *this;
}

SDST
YTextState::GetLnHeight() const
{
	return pCache->GetHeight();
}
SDST
YTextState::GetLnHeightEx() const
{
	return pCache->GetHeight() + lnGap;
}
u16
YTextState::GetLnNNow() const
{
	return (penY - Margin.Top) / GetLnHeightEx();
}

/*YTypeface*
YTextState::SetFont(const char* name, CPATH filename)
{
	return pFont = pCache->SetFont(name, filename);
}*/
void
YTextState::SetLnNNow(u16 n)
{
	penY = Margin.Top + pCache->GetAscender() + GetLnHeightEx() * n;
}


void
YTextRegion::PrintChar(u32 c)
{
	if(!img)
		//无缓冲区时无法绘图。
		return;

	FTC_SBit sbit(pCache->GetGlyph(c));
	const int tx(penX + pCache->GetAdvance(c, sbit));

	if(INVISIBLE_CHAR(c) || !sbit->buffer)
	{
		penX = tx;
		return;
	}

	/*
	u8* bitmap = sbit->buf;
	if(!bitmap)
	{
		penX = tx;
		return;
	}

	int maxX = Width - Right;
	int maxY = Height - Bottom;

	PixelType col(color | BITALPHA);
	int t(0);
	for(int y(0); y < sbit->height; ++y)
	{
		for(int x(0); x < sbit->width; ++x)
		{
			if(bitmap[t] >= Alpha_Threshold)
			{
				int sy = penY + y - sbit->top;
				int sx = penX + x + sbit->left;

				if(sx >= Left && sy >= Top && sx < maxX && sy < maxY)
				{
					int s = sy * Width + sx;
					imgAlpha[s] = bitmap[t];
					img[s] = col;
				}
			}
			++t;
		}
	}
	*/

	PixelType col(Color | BITALPHA);
	const int dx(penX + sbit->left),
		dy(penY - sbit->top),
		xmin(vmax<int>(0, Margin.Left - dx)),
		ymin(vmax<int>(0, Margin.Top - dy)),
		xmax(vmin<int>(Width - Margin.Right - dx, sbit->width)),
		ymax(vmin<int>(Height - Margin.Bottom - dy, sbit->height));

	if(xmax >= xmin && ymax >= ymin)
	{
		const int sJmp(sbit->width - xmax + xmin),
			dJmp(Width - xmax + xmin),
			dOff(vmax<int>(Margin.Top, dy) * Width + vmax<int>(Margin.Left, dx));
		u8* sa(sbit->buffer + ymin * sbit->width + xmin);
		BitmapPtr dc(img + dOff);
		u8* da(imgAlpha + dOff);
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

	//移动笔。
	penX = tx;
}

YTextRegion::YTextRegion() : YTextState(), MBitmapBufferEx()
{
	SetFontSize(YFont::DefSize);
	SetPen();
}
YTextRegion::YTextRegion(YFont& font) : YTextState(font), MBitmapBufferEx()
{
	SetFontSize(YFont::DefSize);
	SetPen();
}
YTextRegion::YTextRegion(YFontCache* fc) : YTextState(fc), MBitmapBufferEx()
{
	SetFontSize(YFont::DefSize);
	SetPen();
}
YTextRegion::~YTextRegion()
{
}

SDST
YTextRegion::GetMarginResized() const
{
	const u8 t(GetLnHeightEx());

	return t ? Margin.Bottom + (Height + lnGap - Margin.Top - Margin.Bottom) % t : 0;
}
SDST
YTextRegion::GetBufferHeightResized() const
{
	const u8 t(GetLnHeightEx());

	return t ? Margin.Top + (Height + lnGap - Margin.Top - Margin.Bottom) / t * t : Height;
}
u16
YTextRegion::GetLnN() const
{
	return GetBufHeightN() / GetLnHeightEx();
}
u16
YTextRegion::GetLnNEx() const
{
	return (GetBufHeightN() + lnGap) / GetLnHeightEx();
}
SPOS
YTextRegion::GetLineLast() const
{
	return Height - Margin.Bottom + pCache->GetDescender();
}
u32
YTextRegion::GetPrevLnOff(const uchar_t* s, u32 n) const
{
	SDST nw(GetBufWidthN());
	const uchar_t* p(s + n);
	SDST w(0);
	uchar_t c(*--p);

	if(c == '\n')
		--p;
	while(p >= s && (c = *p) != '\n' && !(iswprint(c) && (w += pCache->GetAdvance(c)) > nw))
		--p;
	return p + (w <= nw) - s;
}
u32
YTextRegion::GetPrevLnOff(const uchar_t* s, u32 n, SDST l) const
{
	SDST nw(GetBufWidthN());
	const uchar_t* p(s + n);
	SDST w(0);
	uchar_t c(*--p);

	while(p >= s && (w = 0, l--))
	{
		if(c == '\n')
			--p;
		while(p >= s && (c = *p) != '\n' && !(iswprint(c) && (w += pCache->GetAdvance(c)) > nw))
			--p;
	}
	return p + (w <= nw) - s;
}
u32
YTextRegion::GetNextLnOff(const uchar_t* s, u32 n) const
{
	SDST nw(GetBufWidthN());
	const uchar_t* p(s + n);
	SDST w(0);
	uchar_t c;

	while(c = *p++, c != '\n' && !(iswprint(c) && (w += pCache->GetAdvance(c)) > nw))
		;
	return p - s;
}
/*
u32
YTextRegion::GetNextLnOff(const uchar_t* s, u32 n, u16 l) const
{
	return 0;
}
*/

void
YTextRegion::SetLnLast()
{
	const u16 n(GetLnN());

	if(n)
		SetLnNNow(n - 1);
}

void
YTextRegion::ClearLine(u16 l, SDST n)
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
YTextRegion::ClearLn(u16 l)
{
	SDST h(GetLnHeightEx());

	ClearLine(Margin.Top + h * l, h);
}
void
YTextRegion::ClearLnLast()
{
	SDST h(GetLnHeightEx());

	ClearLine(Height - Margin.Bottom - h, h);
}

void
YTextRegion::Move(s16 n)
{
	if(Height > Margin.Bottom)
		Move(n, Height - Margin.Bottom);
}
void
YTextRegion::Move(s16 n, SDST h)
{
	if(img && imgAlpha)
	{
		const s32 t(((h + Margin.Bottom > Height ? Height - Margin.Bottom : h) - Margin.Top - abs(n)) * Width);

		if(n && t > 0)
		{
			u32 d(Margin.Top), s(d);

			if(n > 0)
				d += n;
			else
				s -= n;
			s *= Width;
			d *= Width;
			memmove(&img[d], &img[s], t * sizeof(PixelType));
			memmove(&imgAlpha[d], &imgAlpha[s], t * sizeof(u8));
		}
	}
}

void
YTextRegion::PutNewline()
{
	UpdateFontSize();
	penX = Margin.Left;
	penY += GetLnHeightEx();
}

u8
YTextRegion::PutChar(u32 c)
{
	if(c == '\n')
	{
		PutNewline();
		return 0;
	}
	if(!iswprint(c))
		return 0;
/*
	const int maxW = GetBufWidthN() - 1,
		spaceW = pCache->GetAdvance(' ');

	if(maxW < spaceW)
		return lineBreaksL = 1;
*/
	if(penX + pCache->GetAdvance(c) >= Width - Margin.Right)
	{
		PutNewline();
		return 1;
	}
	UpdateFont();
	PrintChar(c);
	return 0;
}

/*
u32
YTextRegion::PutLine(const uchar_t* s)
{
	SPOS fpy(penY);
	const uchar_t* t(s);

	while(*t && fpy == penY)
		if(!PutChar(*t))
			++t;
	return t - s;
}

u32
YTextRegion::PutString(const uchar_t* s)
{
	SPOS mpy(GetLineLast());
	const uchar_t* t(s);

	while(*t && penY <= mpy)
		if(!PutChar(*t))
			++t;
	return t - s;
}
*/


u32
ReadX(YTextFile& f, YTextRegion& tr, u32 n)
{
	u32 l(0);

	if(f.IsValid())
	{
		u32 i(0), t;
		FILE* const fp(f.GetFilePtr());
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

YSL_END_NAMESPACE(Drawing)

YSL_END

