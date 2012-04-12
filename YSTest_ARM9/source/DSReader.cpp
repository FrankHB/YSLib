/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSReader.cpp
\ingroup YReader
\brief 适用于 DS 的双屏阅读器。
\version r3775;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 14:04:05 +0800;
\par 修改时间:
	2011-04-12 21:09 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::DSReader;
*/


#include "DSReader.h"
#include <algorithm> // for std::copy_n;
#include <YSLib/UI/ywindow.h>
#include <YSLib/Service/TextLayout.h>

YSL_BEGIN

using namespace Drawing;
using namespace Text;

namespace
{
	/*!
	\brief 指定迭代器最小值 b ，取文本迭代器 s 前最近出现的字符 c 的对应文本迭代器。
	\since build 270 。
	*/
	template<typename _tBi>
	_tBi
	FindPreviousChar(_tBi s, _tBi b, ucs4_t c = '\0')
	{
	//	while(b < --s && *s != c)
		while(b != --s && *s != c)
			;
		return s;
	}

	/*!
	\brief 指定迭代器上确界 b ，在 r 中取文本迭代器 s 的当前行尾的文本迭代器。
	\since build 271 。
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
			 if(std::iswprint(*s))
			 {
				 w += r.Font.GetAdvance(*s);
				 if(w >= wmax)
					 break;
			 }
			 ++s;
		}
		return s;
	}

	/*!
	\brief 指定迭代器最小值 b ，在 r 中取文本迭代器 s 的前一行首的前一字符的迭代器。
	\since build 270 。
	*/
	template<typename _tBi>
	_tBi
	FindPreviousLineFeed(TextRegion& r, _tBi s, _tBi b)
	{
	//	if(b < s)
		if(b != s)
		{
			const auto e(s);
			auto t(FindPreviousChar(s, b, '\n'));

			do
			{
				s = t;
				if(*t == '\n')
					++t;
				t = FindLineFeed(r, t, e);
			}while(t != e);
		}
		return s;
	}

	/*!
	\brief 迭代器解引用等于指定值时自增。
	\pre i 可解引用。
	\since build 292 。
	*/
	template<typename _type, typename _tIn>
	inline void
	IncreaseIfEqual(_tIn& i, const _type& val)
	{
		if(*i == val)
			++i;
	}

	/*!
	\brief 对可能出现的换行调整迭代器。
	\since build 292 。
	*/
	template<typename _tIn, class _tArea, class _tContainer>
	inline void
	AdjustForNewline(_tArea& area, _tIn& i, _tContainer& c)
	{
		IncreaseIfEqual(i, '\n');
		i = FindLineFeed(area, i, c.GetEnd());
	}

	/*!
	\brief 调整不完整的行首的迭代器。
	\since build 292 。
	*/
	template<typename _tIn, class _tArea, class _tContainer>
	inline void
	AdjustPrevious(_tArea& area, _tIn& i, _tContainer& c)
	{
		i = FindPreviousLineFeed(area, i, c.GetBegin());
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
	\since build 292 。
	*/
	void
	CopyScrollArea(YSL_ Components::BufferedTextArea& src_area,
		size_t src_offset, YSL_ Components::BufferedTextArea& dst_area,
		size_t dst_offset, ptrdiff_t offset, size_t n)
	{
		YAssert(n != 0, "Invalid number of lines found @ CopyScrollArea;");
		YAssert(n <= size_t(std::abs(offset)), "Invalid offset found"
			" @ CopyScrollArea;");

		const SDst w(src_area.GetWidth());

		YAssert(w == dst_area.GetWidth(), "Unequal width found"
			" @ CopyScrollArea;");

		yunseq(src_offset *= w, dst_offset *= w, n *= w);
		dst_area.Scroll(offset);
		yunseq(std::copy_n(src_area.GetBufferPtr() + src_offset, n,
			dst_area.GetBufferPtr() + dst_offset), std::copy_n(
			src_area.GetBufferAlphaPtr() + src_offset, n,
			dst_area.GetBufferAlphaPtr() + dst_offset));
		src_area.Scroll(offset);
	}

	/*!
	\brief 全区域移动上下屏区域像素。
	\note 复制后清除未被覆盖区域。
	\since build 292 。
	*/
	void
	MoveScrollArea(YSL_ Components::BufferedTextArea& area_up,
		YSL_ Components::BufferedTextArea& area_dn, ptrdiff_t offset, size_t n)
	{
		YAssert(area_up.GetHeight() - area_up.Margin.Bottom - n > 0,
			"No enough space of areas found @ MoveScrollArea;");

		SDst src_off(area_dn.Margin.Top),
			dst_off(area_up.GetHeight() - area_up.Margin.Bottom - n);
		auto* p_src(&area_dn);
		auto* p_dst(&area_up);
		SDst clr_off;

		if(offset > 0) //复制区域向下移动，即浏览区域向上滚动。
		{
			std::swap(p_src, p_dst),
			std::swap(src_off, dst_off),
			clr_off = area_up.Margin.Top;
		}
		else
			clr_off = area_dn.GetHeight() - area_dn.Margin.Bottom - n;
		CopyScrollArea(*p_src, src_off, *p_dst, dst_off, offset, n);
		p_src->ClearLine(clr_off, n);
	}
}

YSL_BEGIN_NAMESPACE(DS)

YSL_BEGIN_NAMESPACE(Components)

DualScreenReader::DualScreenReader(SDst w, SDst h_up, SDst h_down,
	FontCache& fc_)
	: pText(), fc(fc_), i_top(), i_btm(), overread_line_n(0), scroll_offset(0),
	Margin(4, 4, 4, 4),
	area_up(Rect(Point::Zero, w, h_up), fc),
	area_dn(Rect(Point::Zero, w, h_down), fc)
{
	SetFontSize(),
	SetColor(),
	SetLineGap(),
	area_up.SetTransparent(true),
	area_dn.SetTransparent(true);
}

void
DualScreenReader::SetLineGap(u8 g)
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

	using YSL_ Components::Invalidate;

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
	// NOTE: margins shall be adjusted before output;
}

void
DualScreenReader::AdjustMargins()
{
	yunseq(area_up.Margin = Margin, area_dn.Margin = Margin);
	yunseq(AdjustBottomMarginOf(area_up), AdjustBottomMarginOf(area_dn));
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
	return scroll_offset != 0 ? ScrollByPixel(GetTextLineHeightExOf(area_up)
		- scroll_offset) : 0;
}

void
DualScreenReader::Attach(YSL_ Components::Window& wnd_up,
	YSL_ Components::Window& wnd_dn)
{
	wnd_up += area_up,
	wnd_dn += area_dn;
}

void
DualScreenReader::Detach()
{
	using YSL_ Components::Window;

	if(auto pCon = dynamic_cast<Window*>(FetchContainerPtr(area_up)))
		*pCon -= area_up;
	if(auto pCon = dynamic_cast<Window*>(FetchContainerPtr(area_dn)))
		*pCon -= area_dn;
}

bool
DualScreenReader::Execute(Command cmd)
{
	if(YCL_UNLIKELY(!pText || pText->GetTextSize() == 0))
		return false;
	if(YCL_UNLIKELY(~cmd & Scroll))
		return false;
	if(AdjustScrollOffset() != 0)
		return false;
	if(cmd & Up)
	{
		if(YCL_UNLIKELY(IsTextTop()))
			return false;
	}
	else if(YCL_UNLIKELY(IsTextBottom()))
		return false;

	YAssert(area_up.LineGap == area_dn.LineGap, "Unequal line gaps found"
		" @ DualScreenReader::Execute;");
	// TODO: assert the fonts are same;

	cmd &= ~Scroll;
	if(cmd & Line)
	{
		const FontSize h(area_up.Font.GetHeight()), hx(h + GetLineGap());

		if(cmd & Up)
		{
			MoveScrollArea(area_up, area_dn, hx, h);
			SetCurrentTextLineNOf(area_up, 0);
			AdjustPrevious(area_up, i_top, *pText);
			CarriageReturn(area_up);
			{
				auto iTopNew(i_top);

				IncreaseIfEqual(iTopNew, '\n');
				PutLine(area_up, iTopNew, pText->GetEnd(), '\n');
			}
			if(overread_line_n > 0)
				--overread_line_n;
			else
				AdjustPrevious(area_up, i_btm, *pText);
		}
		else
		{
			MoveScrollArea(area_up, area_dn, -hx, h);
			{
				u16 n(area_dn.GetTextLineNEx());

				YAssert(n != 0,
					"No Enough height found @ DualScreenReader::Excute;");

				SetCurrentTextLineNOf(area_dn, --n);
			}
			//注意缓冲区不保证以 '\0' 结尾。
			CarriageReturn(area_dn);
			IncreaseIfEqual(i_btm, '\n');
			i_btm = PutLine(area_dn, i_btm, pText->GetEnd(), '\n');
			AdjustForNewline(area_up, i_top, *pText);
		}
		Invalidate();
	}
	else
	{
		auto ln(area_up.GetTextLineNEx() + area_dn.GetTextLineNEx());

		if(cmd & Up)
			while(ln--)
				AdjustPrevious(area_up, i_top, *pText);
		else
			while(ln-- && i_btm != pText->GetEnd())
			{
				AdjustForNewline(area_dn, i_btm, *pText);
				AdjustForNewline(area_up, i_top, *pText);
			}
		UpdateView();
	}
	return true;
}

void
DualScreenReader::Locate(size_t pos)
{
	if(GetTextSize() == 0)
	{
		Reset();
		Invalidate();
		return;
	}
	if(pos == 0)
		i_top = pText->GetBegin();
	else if(pos < pText->GetTextSize())
	{
		i_top = pText->GetIterator(pos);
		AdjustPrevious(area_up, ++i_top, *pText);
	}
	else
		return;
	UpdateView();
}

void
DualScreenReader::Invalidate()
{
	using YSL_ Components::Invalidate;

	//强制刷新背景。
	Invalidate(area_up);
	Invalidate(area_dn);
	if(ViewChanged)
		ViewChanged();
}

void
DualScreenReader::LoadText(TextFile& file)
{
	if(YCL_LIKELY(file.IsValid()))
	{
		pText = make_unique<Text::TextFileBuffer>(file);
		i_top = pText->GetBegin();
		i_btm = pText->GetEnd();
		UpdateView();
	}
	else
		PutString(area_up, u"文件打开失败！");
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

	YAssert(scroll_offset < ln_h_ex, "Invalid scroll offset found"
		" @ DualScreenReader::AdjustScrollOffset");

	if(YCL_UNLIKELY(i_btm == pText->GetEnd() || scroll_offset + h > ln_h_ex))
		return 0;
	MoveScrollArea(area_up, area_dn, -h, h);
	{
		u16 n(area_dn.GetTextLineNEx());

		YAssert(n != 0,
			"No Enough height found @ DualScreenReader::Excute;");

		SetCurrentTextLineNOf(area_dn, --n);
	}
	//注意缓冲区不保证以 '\0' 结尾。
	CarriageReturn(area_dn);
	if(YCL_LIKELY((scroll_offset += h) < ln_h_ex))
	{
		area_dn.PenY += ln_h_ex - scroll_offset;

		auto i_tmp(i_btm);

		IncreaseIfEqual(i_tmp, '\n');
		PutLine(area_dn, i_tmp, pText->GetEnd(), '\n');
	}
	else
	{
		IncreaseIfEqual(i_btm, '\n');
		i_btm = PutLine(area_dn, i_btm, pText->GetEnd(), '\n');
		AdjustForNewline(area_up, i_top, *pText);
		scroll_offset = 0;
	}
	Invalidate();
	return h;
}

void
DualScreenReader::Stretch(SDst h)
{
	RestrictInClosedInterval(h, 0, MainScreenHeight - 40);
	h = MainScreenHeight - h;

	const SDst w(area_dn.GetWidth());

	SetSizeOf(area_dn, Size(w, h)),
	area_dn.SetSize(w, h);
	UpdateView();
}

void
DualScreenReader::UnloadText()
{
	yunseq(i_top = Text::TextFileBuffer::Iterator(),
		i_btm = Text::TextFileBuffer::Iterator(),
		pText = nullptr);
}

void
DualScreenReader::UpdateView()
{
	if(YCL_UNLIKELY(!pText || pText->GetTextSize() == 0))
		return;
	Reset();
	{
		auto i_new(i_top);

		IncreaseIfEqual(i_new, '\n');
		i_new = PutString(area_up, i_new, pText->GetEnd());
		if(YCL_UNLIKELY(i_new == pText->GetEnd()))
		{
			i_btm = i_new;

			const auto b(FetchLastLineBasePosition(area_up,
				area_up.GetHeight()));

			overread_line_n = area_up.PenY >= b ? 0 : area_dn.GetTextLineNEx()
				+ (b - area_up.PenY) / GetTextLineHeightExOf(area_up);
		}
		else
		{
			i_btm = PutString(area_dn, i_new, pText->GetEnd());

			const auto b(FetchLastLineBasePosition(area_dn,
				area_dn.GetHeight()));

			overread_line_n = area_dn.PenY >= b ? 0 : (b - area_dn.PenY)
				/ GetTextLineHeightExOf(area_dn);
		}
	}
	if(YCL_LIKELY(i_btm != pText->GetEnd() && *i_btm == '\n'))
		--i_btm;
	Invalidate();
}

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

