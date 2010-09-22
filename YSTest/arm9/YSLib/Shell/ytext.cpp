// YSLib::Shell::YText by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 0:06:05;
// UTime = 2010-9-22 4:16;
// Version = 0.6165;


#include "ytext.h"

YSL_BEGIN

using namespace Drawing;
using namespace Text;

YSL_BEGIN_NAMESPACE(Drawing)

#define INVISIBLE_CHAR(c) ((c) == ' ' || (c) == '\t' || (c)== '\r')

static const u8 Alpha_Threshold = 16;

TextState::TextState()
:PenStyle(GetDefaultFontFamily()), Margin(),
penX(0), penY(0), lnGap(0)
{}
TextState::TextState(Drawing::Font& font)
:PenStyle(font.GetFontFamily()), Margin(),
penX(0), penY(0), lnGap(0)
{}
TextState::TextState(YFontCache& fc)
:PenStyle(*fc.GetDefaultTypefacePtr()->GetFontFamilyPtr()), Margin(),
penX(0), penY(0), lnGap(0)
{}

SDST
TextState::GetLnHeight() const
{
	return GetCache().GetHeight();
}
SDST
TextState::GetLnHeightEx() const
{
	return GetCache().GetHeight() + lnGap;
}
u16
TextState::GetLnNNow() const
{
	return (penY - Margin.Top) / GetLnHeightEx();
}

/*Typeface*
TextState::SetFont(const char* name, CPATH filename)
{
	return pFont = GetCache().SetFont(name, filename);
}*/
void
TextState::SetLnNNow(u16 n)
{
	penY = Margin.Top + GetCache().GetAscender() + GetLnHeightEx() * n;
}


void
TextRegion::PrintChar(fchar_t c)
{
	if(img == NULL)
		//无缓冲区时无法绘图。
		return;

	YFontCache& cache(GetCache());
	CharBitmap sbit(cache.GetGlyph(c));
	const int tx(penX + cache.GetAdvance(c, sbit));

	if(!INVISIBLE_CHAR(c) && sbit.GetBuffer() != NULL)
	{
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
		const int dx(penX + sbit.GetLeft()),
			dy(penY - sbit.GetTop()),
			xmin(vmax<int>(0, Margin.Left - dx)),
			ymin(vmax<int>(0, Margin.Top - dy)),
			xmax(vmin<int>(Width - Margin.Right - dx, sbit.GetWidth())),
			ymax(vmin<int>(Height - Margin.Bottom - dy, sbit.GetHeight()));

		if(xmax >= xmin && ymax >= ymin)
		{
			const int sJmp(sbit.GetWidth() - xmax + xmin),
				dJmp(Width - xmax + xmin),
				dOff(vmax<int>(Margin.Top, dy) * Width + vmax<int>(Margin.Left, dx));
			u8* sa(sbit.GetBuffer() + ymin * sbit.GetWidth() + xmin);
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
	}
	//移动笔。
	penX = tx;
}

TextRegion::TextRegion()
: TextState(), MBitmapBufferEx()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	SetPen();
}
TextRegion::TextRegion(Drawing::Font& font)
: TextState(font), MBitmapBufferEx()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	SetPen();
}
TextRegion::TextRegion(YFontCache& fc)
: TextState(fc), MBitmapBufferEx()
{
	Font.SetSize(Font::DefSize);
	Font.UpdateSize();
	SetPen();
}
TextRegion::~TextRegion()
{
}

SDST
TextRegion::GetMarginResized() const
{
	const u8 t(GetLnHeightEx());

	return t ? Margin.Bottom + (Height + lnGap - Margin.Top - Margin.Bottom) % t : 0;
}
SDST
TextRegion::GetBufferHeightResized() const
{
	const u8 t(GetLnHeightEx());

	return t ? Margin.Top + (Height + lnGap - Margin.Top - Margin.Bottom) / t * t : Height;
}
u16
TextRegion::GetLnN() const
{
	return GetBufHeightN() / GetLnHeightEx();
}
u16
TextRegion::GetLnNEx() const
{
	return (GetBufHeightN() + lnGap) / GetLnHeightEx();
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
		SetLnNNow(n - 1);
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
	SDST h(GetLnHeightEx());

	ClearLine(Margin.Top + h * l, h);
}
void
TextRegion::ClearLnLast()
{
	SDST h(GetLnHeightEx());

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
			std::memmove(&img[d], &img[s], t * sizeof(PixelType));
			std::memmove(&imgAlpha[d], &imgAlpha[s], t * sizeof(u8));
		}
	}
}

void
TextRegion::PutNewline()
{
	penX = Margin.Left;
	penY += GetLnHeightEx();
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
	if(penX + GetCache().GetAdvance(c) >= Width - Margin.Right)
	{
		PutNewline();
		return 1;
	}
	PrintChar(c);
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

