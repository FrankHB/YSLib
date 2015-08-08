/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSReader.cpp
\ingroup YReader
\brief 适用于 DS 的双屏阅读器。
\version r3216
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-01-05 14:04:05 +0800
\par 修改时间:
	2015-08-05 09:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::DSReader
*/


#include "DSReader.h"
#include <algorithm> // for std::copy_n;
#include YFM_YSLib_UI_YWindow
#include YFM_YSLib_Service_TextLayout

namespace YSLib
{

using namespace Drawing;
using namespace Text;
using ystdex::next_if_eq;

namespace
{

/*!
\brief 指定迭代器最小值 b ，取文本迭代器 s 前最近出现的字符 c 的对应文本迭代器。
\since build 343
*/
template<typename _tBi>
_tBi
FindPreviousChar(_tBi s, _tBi b, ucs4_t c = ucs4_t())
{
//	while(b < --s && *s != c)
	while(b != --s && *s != c)
		;
	return s;
}

/*!
\brief 在 r 中取文本迭代器 s 的当前行尾的文本迭代器。
\since build 271
*/
template<typename _tBi>
_tBi
FindLineFeed(const TextRegion& r, _tBi s, _tBi e)
{
	const SDst wmax(r.GetWidth() - GetHorizontalOf(r.Margin));
	SDst w(0);

//	while(s < e && *s != '\n')
	while(s != e && *s != '\n')
	{
		if(IsPrint(*s))
		{
			w += SDst(r.Font.GetAdvance(*s));
			if(w >= wmax)
				break;
		}
		++s;
	}
	return s;
}

/*!
\brief 指定迭代器最小值 b ，在 r 中取文本迭代器 s 的前一行首的前一字符的迭代器。
\since build 270
*/
template<typename _tBi>
_tBi
FindPreviousLineFeed(TextRegion& r, _tBi s, _tBi b)
{
//	if(b < s)
	if(b != s)
	{
		const auto e(s);

		for(auto t(FindPreviousChar(s, b, '\n')); t != e;
			t = FindLineFeed(r, t, e))
		{
			s = t;
			if(*t == '\n')
				++t;
		}
	}
	return s;
}

/*!
\brief 对可能出现的换行调整迭代器。
\since build 292
*/
template<typename _tIn, class _tArea, class _tCon>
inline void
AdjustForNewline(_tArea& area, _tIn& i, _tCon& c)
{
	i = FindLineFeed(area, next_if_eq(i, '\n'), end(c));
}

/*!
\brief 调整不完整的行首的迭代器。
\since build 292
*/
template<typename _tIn, class _tArea, class _tCon>
inline void
AdjustPrevious(_tArea& area, _tIn& i, _tCon& c)
{
	i = FindPreviousLineFeed(area, i, begin(c));
}

/*!
\brief 按全区域移动复制上下屏区域像素。
\param src_area 源区域。
\param src_offset 源偏移行数。
\param dst_area 目标区域。
\param dst_offset 目标偏移行数。
\param offset 待复制区域的移动偏移（大小等于待复制区域的总行数，向上时 < 0）。
\param n 实际复制行数。
\note 不清理源区域。
\since build 292
*/
void
CopyScrollArea(YSLib::UI::BufferedTextArea& src_area,
	size_t src_offset, YSLib::UI::BufferedTextArea& dst_area,
	size_t dst_offset, ptrdiff_t offset, size_t n)
{
	YAssert(n != 0, "Invalid number of lines found.");
	YAssert(n <= size_t(std::abs(offset)), "Invalid offset found.");

	const SDst w(src_area.GetWidth());

	YAssert(w == dst_area.GetWidth(), "Distinct screen widths found.");
	yunseq(src_offset *= w, dst_offset *= w, n *= w);
	dst_area.Scroll(offset);
	yunseq(std::copy_n(src_area.GetBufferPtr() + src_offset, n,
		dst_area.GetBufferPtr() + dst_offset),
		std::copy_n(src_area.GetBufferAlphaPtr() + src_offset, n,
		dst_area.GetBufferAlphaPtr() + dst_offset));
	src_area.Scroll(offset);
}

/*!
\brief 全区域移动上下屏区域像素。
\note 复制后清除未被覆盖区域。
\since build 588
*/
void
MoveScrollArea(YSLib::UI::BufferedTextArea& area_up,
	YSLib::UI::BufferedTextArea& area_dn, ptrdiff_t offset, SDst n)
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	YAssert(SPos(area_up.GetHeight()) - area_up.Margin.Bottom - SPos(n) > 0,
		"No enough space of areas found.");

	const SDst up_btm(SDst(max<SPos>(area_up.Margin.Bottom, 0)));

	if(YB_UNLIKELY(area_up.GetHeight() <= up_btm + n))
		return;

	auto src_off(SDst(max<SPos>(area_dn.Margin.Top, 0))),
		dst_off(SDst(area_up.GetHeight() - up_btm - n));
	auto* p_src(&area_dn);
	auto* p_dst(&area_up);
	SDst clr_off;

	if(offset > 0) //复制区域向下移动，即浏览区域向上滚动。
	{
		std::swap(p_src, p_dst),
		std::swap(src_off, dst_off),
		clr_off = SDst(max<SPos>(area_up.Margin.Top, 0));
	}
	else
		clr_off = area_dn.GetHeight()
			- SDst(max<SPos>(area_dn.Margin.Bottom, 0)) - n;
	CopyScrollArea(*p_src, src_off, *p_dst, dst_off, offset, size_t(n));
	p_src->ClearLine(clr_off, n);
}

//! \since build 375
std::uint16_t
CheckOverRead(TextRegion& r)
{
	const auto b(FetchLastLineBasePosition(r, r.GetHeight()));

	// XXX: Conversion to 'SPos' might be implementation-defined.
	return r.Pen.Y < b ? (b - r.Pen.Y) / SPos(GetTextLineHeightExOf(r)) : 0;
}

} // unnamed namespace;

namespace DS
{

namespace UI
{

DualScreenReader::DualScreenReader(SDst w, SDst h_up, SDst h_down,
	FontCache& fc_)
	: p_text(), fc(fc_), i_top(), i_btm(), overread_line_n(0), scroll_offset(0),
	Margin(4, 4, 4, 4),
	area_up(Rect({}, w, h_up), fc), area_dn(Rect({}, w, h_down), fc)
{
	SetFontSize(),
	SetColor(),
	SetLineGap(),
	area_up.Background = nullptr,
	area_dn.Background = nullptr;
}

void
DualScreenReader::SetLineGap(std::uint8_t g)
{
	if(area_up.LineGap != g)
	{
		yunseq(area_up.LineGap = g, area_dn.LineGap = g);
		UpdateView();
	}
}

void
DualScreenReader::SetVisible(bool b)
{
	SetVisibleOf(area_up, b), SetVisibleOf(area_dn, b);

	using YSLib::UI::Invalidate;

	//强制刷新背景。
	Invalidate(area_up);
	Invalidate(area_dn);
}

void
DualScreenReader::SetFont(const Font& fnt)
{
	yunseq(area_up.Font = fnt, area_dn.Font = fnt);
}
void
DualScreenReader::SetFontSize(FontSize s)
{
	area_up.Font.SetSize(s),
	area_dn.Font.SetSize(s);
	// NOTE: Margins shall be adjusted before output.
}

void
DualScreenReader::AdjustForFirstNewline()
{
	AdjustForNewline(area_up, i_top, *p_text);
}

void
DualScreenReader::AdjustForPrevNewline()
{
	AdjustPrevious(area_up, i_top, *p_text);
}

void
DualScreenReader::AdjustMargins()
{
	yunseq(area_up.Margin = Margin, area_dn.Margin = Margin);
	AdjustBottomMarginOf(area_up), AdjustBottomMarginOf(area_dn);
	{
		const SPos v((area_up.Margin.Bottom - area_up.Margin.Top) / 2);

		yunseq(area_up.Margin.Top += v, area_up.Margin.Bottom -= v);
	}
	{
		const SPos v((area_dn.Margin.Bottom - area_dn.Margin.Top) / 2);

		yunseq(area_dn.Margin.Top += v, area_dn.Margin.Bottom -= v);
	}
}

FontSize
DualScreenReader::AdjustScrollOffset()
{
	return bool(p_text) && scroll_offset != 0
		? ScrollByPixel(GetTextLineHeightExOf(area_up) - scroll_offset) : 0;
}

void
DualScreenReader::Attach(YSLib::UI::Window& wnd_up,
	YSLib::UI::Window& wnd_dn)
{
	wnd_up += area_up,
	wnd_dn += area_dn;
}

void
DualScreenReader::Detach()
{
	using YSLib::UI::Window;

	if(const auto p_con = dynamic_cast<Window*>(FetchContainerPtr(area_up)))
		*p_con -= area_up;
	if(const auto p_con = dynamic_cast<Window*>(FetchContainerPtr(area_dn)))
		*p_con -= area_dn;
}

bool
DualScreenReader::Execute(Command cmd)
{
	if(YB_UNLIKELY(!p_text || p_text->GetTextSize() == 0))
		return {};
	if(YB_UNLIKELY(~cmd & Scroll))
		return {};
	if(AdjustScrollOffset() != 0)
		return {};
	if(cmd & Up)
	{
		if(YB_UNLIKELY(IsTextTop()))
			return {};
	}
	else if(YB_UNLIKELY(IsTextBottom()))
		return {};
	YAssert(area_up.LineGap == area_dn.LineGap, "Distinct line gaps found.");
	// TODO: Assert the fonts are same.
	cmd &= ~Scroll;
	if(cmd & Line)
	{
		const FontSize h(area_up.Font.GetHeight()), hx(h + GetLineGap());

		if(cmd & Up)
		{
			MoveScrollArea(area_up, area_dn, hx, SDst(h));
			SetCurrentTextLineNOf(area_up, 0);
			AdjustForPrevNewline();
			CarriageReturn(area_up);
			PutLine(area_up, next_if_eq(i_top, '\n'), p_text->end(), '\n');
			if(overread_line_n > 0)
				--overread_line_n;
			else
				AdjustPrevious(area_up, i_btm, *p_text);
		}
		else
		{
			MoveUpForLastLine(-hx, h);
			//注意缓冲区不保证以空字符结尾。
			CarriageReturn(area_dn);
			i_btm = PutLastLine();
			AdjustForFirstNewline();
		}
		Invalidate();
	}
	else
	{
		auto ln(area_up.GetTextLineNEx() + area_dn.GetTextLineNEx());

		if(cmd & Up)
			while(ln--)
				AdjustForPrevNewline();
		else
			while(ln-- && !IsTextBottom())
			{
				AdjustForNewline(area_dn, i_btm, *p_text);
				AdjustForFirstNewline();
			}
		UpdateView();
	}
	return true;
}

void
DualScreenReader::Invalidate()
{
	using YSLib::UI::Invalidate;

	//强制刷新背景。
	Invalidate(area_up);
	Invalidate(area_dn);
	if(ViewChanged)
		ViewChanged();
}

void
DualScreenReader::Locate(size_t pos)
{
	const auto s(Deref(p_text).GetTextSize());

	if(s == 0)
	{
		Reset();
		Invalidate();
		return;
	}
	if(pos == 0)
		i_top = p_text->begin();
	else if(pos < s)
	{
		i_top = p_text->GetIterator(pos);
		++i_top;
		AdjustForPrevNewline();
	}
	else
		return;
	UpdateView();
}

void
DualScreenReader::LoadText(ifstream& file, Encoding enc)
{
	if(file.is_open())
	{
		file.unsetf(std::ios_base::skipws);
		p_text.reset(new Text::TextFileBuffer(file, enc));
		yunseq(i_top = p_text->begin(), i_btm = p_text->end());
		UpdateView();
	}
	else
	{
		UnloadText();
		Reset();
		PutString(area_up, u"文件打开失败！");
		Invalidate();
	}
}

void
DualScreenReader::MoveUpForLastLine(ptrdiff_t off, size_t h)
{
	MoveScrollArea(area_up, area_dn, off, SDst(h));

	std::uint16_t n(area_dn.GetTextLineNEx());

	YAssert(n != 0, "No Enough height.");
	SetCurrentTextLineNOf(area_dn, --n);
}

Text::TextFileBuffer::iterator
DualScreenReader::PutLastLine()
{
	return PutLine(area_dn, next_if_eq(i_btm, '\n'), p_text->end(), '\n');
}

void
DualScreenReader::Reset()
{
	//清除字符区域缓冲区。
	area_up.ClearImage();
	area_dn.ClearImage();
	//根据行距调整并均衡边距。
	AdjustMargins();
	//复位缓存区域写入位置。
	area_up.ResetPen();
	area_dn.ResetPen();
}

FontSize
DualScreenReader::ScrollByPixel(Drawing::FontSize h)
{
	const FontSize ln_h_ex(GetTextLineHeightExOf(area_up));

	YAssert(scroll_offset < ln_h_ex, "Invalid scroll offset found.");
	if(YB_UNLIKELY(i_btm == Deref(p_text).end()
		|| scroll_offset + h > ln_h_ex))
		return 0;
	MoveUpForLastLine(-h, h);
	//注意缓冲区不保证以空字符结尾。
	CarriageReturn(area_dn);
	if(YB_LIKELY((scroll_offset += h) < ln_h_ex))
	{
		area_dn.Pen.Y += ln_h_ex - scroll_offset;
		PutLastLine();
	}
	else
	{
		i_btm = PutLastLine();
		AdjustForFirstNewline();
		scroll_offset = 0;
	}
	Invalidate();
	return h;
}

void
DualScreenReader::Stretch(SDst h)
{
	RestrictInClosedInterval<SDst>(h, 0, MainScreenHeight - 40);
	h = MainScreenHeight - h;

	const SDst w(area_dn.GetWidth());

	SetSizeOf(area_dn, Size(w, h)),
	area_dn.SetSize(w, h);
	UpdateView();
}

void
DualScreenReader::UnloadText()
{
	yunseq(i_top = Text::TextFileBuffer::iterator(),
		i_btm = Text::TextFileBuffer::iterator(),
		p_text = nullptr);
}

void
DualScreenReader::UpdateView()
{
	if(YB_UNLIKELY(!p_text || p_text->GetTextSize() == 0))
		return;
	Reset();
	{
		auto i_new(PutString(area_up, next_if_eq(i_top, '\n'),
			p_text->end()));

		if(YB_UNLIKELY(i_new == p_text->end()))
		{
			i_btm = i_new;
			overread_line_n = CheckOverRead(area_up) + area_dn.GetTextLineNEx();
		}
		else
		{
			i_btm = PutString(area_dn, i_new, p_text->end());
			overread_line_n = CheckOverRead(area_dn);
		}
	}
	if(YB_LIKELY(!IsTextBottom() && *i_btm == '\n'))
		--i_btm;
	Invalidate();
}

} // namespace UI;

} // namespace DS;

} // namespace YSLib;

