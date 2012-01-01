/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSReader.cpp
\ingroup YReader
\brief 适用于 DS 的双屏阅读器。
\version r3487;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 14:04:05 +0800;
\par 修改时间:
	2011-12-31 21:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::DSReader;
*/


#include "DSReader.h"
#include <ystdex/algorithm.hpp> // for ystdex::pod_copy_n;
#include <YSLib/UI/ywindow.h>

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
		auto& cache(r.GetCache());
		const SDst wmax(r.GetWidth() - GetHorizontalOf(r.Margin));
		SDst w(0);

	//	while(s < e && *s != '\n')
		while(s != e && *s != '\n')
		{
			 if(std::iswprint(*s))
			 {
				 w += cache.GetAdvance(*s);
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
}

YSL_BEGIN_NAMESPACE(DS)

YSL_BEGIN_NAMESPACE(Components)

DualScreenReader::DualScreenReader(SDst w, SDst h_up, SDst h_down,
	FontCache& fc_)
	: pText(), fc(fc_), i_top(), i_btm(), overread_line_n(0),
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
	if(~cmd & Scroll)
		return false;
	if(cmd & Up)
	{
		if(IsTextTop())
			return false;
	}
	else if(IsTextBottom())
		return false;

	cmd &= ~Scroll;
	if(cmd & Line)
	{
		// NOTE: assume AreaUp.GetLineGap() == AreaDown.GetLineGap();
		const u8 h(fc.GetHeight()), hx(h + GetLineGap());
		const auto w(area_up.GetWidth());
		const u32 t(w * h);

		if(cmd & Up)
		{
			yunseq(AdjustBottomMarginOf(area_up),
				AdjustBottomMarginOf(area_dn));

			const u32 s((area_up.GetHeight() - area_up.Margin.Bottom - h) * w),
				d(area_dn.Margin.Top * w);

			area_dn.Scroll(hx);
			yunseq(ystdex::pod_copy_n(&area_up.GetBufferPtr()[s], t,
				&area_dn.GetBufferPtr()[d]),
				ystdex::pod_copy_n(&area_up.GetBufferAlphaPtr()[s], t,
				&area_dn.GetBufferAlphaPtr()[d]));
			area_up.Scroll(hx);
			area_up.ClearTextLine(0);
			SetCurrentTextLineNOf(area_up, 0);
			i_top = FindPreviousLineFeed(area_up, i_top, pText->GetBegin());
			CarriageReturn(area_up);
			{
				auto iTopNew(i_top);

				if(*iTopNew == '\n')
					++iTopNew;
				PutLine(area_up, iTopNew, pText->GetEnd(), '\n');
			}
			if(overread_line_n > 0)
				--overread_line_n;
			else
				i_btm = FindPreviousLineFeed(area_up, i_btm,
					pText->GetBegin());
		}
		else
		{
			const u32 s(area_up.Margin.Top * w),
				d((area_up.GetHeight() - FetchResizedBottomMargin(area_up) - h)
				* w);

			area_up.Scroll(-hx);
			yunseq(ystdex::pod_copy_n(&area_dn.GetBufferPtr()[s], t,
				&area_up.GetBufferPtr()[d]),
				ystdex::pod_copy_n(&area_dn.GetBufferAlphaPtr()[s], t,
				&area_up.GetBufferAlphaPtr()[d]));
			area_dn.Scroll(-hx);
			{
				u16 n(area_dn.GetTextLineN());

				YAssert(n != 0,
					"No Enough height found @ DualScreenReader::Excute;");

				--n;
				area_dn.ClearTextLine(n);
				SetCurrentTextLineNOf(area_dn, n);
			}
			//注意缓冲区不保证以 '\0' 结尾。
			CarriageReturn(area_dn);
			if(*i_btm == '\n')
				++i_btm;
			i_btm = PutLine(area_dn, i_btm, pText->GetEnd(), '\n');
			if(*i_top == '\n')
				++i_top;
			i_top = FindLineFeed(area_up, i_top, pText->GetEnd());
		}
		Invalidate();
	}
	else
	{
		auto ln(area_up.GetTextLineN() + area_dn.GetTextLineN());

		if(cmd & Up)
		{
			while(ln--)
				i_top = FindPreviousLineFeed(area_up, i_top, pText->GetBegin());
		}
		else
		{
			while(ln-- && i_btm != pText->GetEnd())
			{
				if(*i_btm == '\n')
					++i_btm;
				i_btm = FindLineFeed(area_dn, i_btm, pText->GetEnd());
				if(*i_top == '\n')
					++i_top;
				i_top = FindLineFeed(area_up, i_top, pText->GetEnd());
			}
		}
		UpdateView();
	}
	return true;
}

void
DualScreenReader::Locate(size_t pos)
{
	if(pos < pText->GetTextSize())
	{
		i_top = pText->GetIterator(pos);
		if(pos != 0)
			i_top = FindPreviousLineFeed(area_up, i_top, pText->GetBegin());
		UpdateView();
	}
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
	if(file.IsValid())
	{
		pText = unique_raw(new Text::TextFileBuffer(file));
		i_top = pText->GetBegin();
		i_btm = pText->GetEnd();
		UpdateView();
	}
	else
		PutString(area_up, L"文件打开失败！\n");
}

void
DualScreenReader::Reset()
{
	//清除字符区域缓冲区。
	area_up.ClearImage();
	area_dn.ClearImage();
	//复位缓存区域写入位置。
	area_up.ResetPen();
	area_dn.ResetPen();
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
	Reset();
	{
		auto i_new(i_top);

		if(*i_new == '\n')
			++i_new;
		i_new = PutString(area_up, i_new, pText->GetEnd());
		if(i_new == pText->GetEnd())
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
	if(i_btm != pText->GetEnd() && *i_btm == '\n')
		--i_btm;
	Invalidate();
}

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

