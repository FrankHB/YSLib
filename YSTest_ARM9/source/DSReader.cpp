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
\version r3422;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 14:04:05 +0800;
\par 修改时间:
	2011-12-22 16:37 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::DSReader;
*/


#include "DSReader.h"
#include <ystdex/algorithm.hpp> // for ystdex::pod_copy_n;

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
		while(b < --s && *s != c)
			;
		return s;
	}

	/*!
	\brief 指定迭代器上确界 b ，在 r 中取文本迭代器 s 的当前行尾的文本迭代器。
	\since build 271 。
	*/
	template<typename _tRan>
	_tRan
	FindLineFeed(const TextRegion& r, _tRan s, _tRan e)
	{
		auto& cache(r.GetCache());
		const SDst wmax(r.GetWidth() - GetHorizontalOf(r.Margin));
		SDst w(0);

		while(s < e && *s != '\n')
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
	template<typename _tRan>
	_tRan
	FindPreviousLineFeed(TextRegion& r, _tRan s, _tRan b)
	{
		if(b < s)
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
	: pText(), fc(fc_),
	rot(RDeg0), iTop(), iBottom(), text_down(),
	AreaUp(Rect(Point::Zero, w, h_up), fc),
	AreaDown(Rect(Point::Zero, w, h_down), fc)
{
	SetFontSize();
	SetColor();
	SetLineGap();
	AreaUp.SetTransparent(true);
	AreaDown.SetTransparent(true);
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
		const auto w(AreaUp.GetWidth());
		const u32 t(w * h);

		if(cmd & Up)
		{
			yunseq(AdjustBottomMarginOf(AreaUp),
				AdjustBottomMarginOf(AreaDown));

			const u32 s((AreaUp.GetHeight() - AreaUp.Margin.Bottom - h) * w),
				d(AreaDown.Margin.Top * w);

			AreaDown.Scroll(hx);
			yunseq(ystdex::pod_copy_n(&AreaUp.GetBufferPtr()[s], t,
				&AreaDown.GetBufferPtr()[d]),
				ystdex::pod_copy_n(&AreaUp.GetBufferAlphaPtr()[s], t,
				&AreaDown.GetBufferAlphaPtr()[d]));
			AreaUp.Scroll(hx);
			AreaUp.ClearTextLine(0);
			SetCurrentTextLineNOf(AreaUp, 0);
			iTop = FindPreviousLineFeed(AreaUp, iTop, pText->cbegin());
			CarriageReturn(AreaUp);
			{
				auto iTopNew(iTop);

				if(*iTopNew == '\n')
					++iTopNew;
				PutLine(AreaUp, iTopNew, pText->cend(), '\n');
			}
			if(!IsTextBottom())
				iBottom = FindPreviousLineFeed(AreaUp, iBottom,
					pText->cbegin());
		}
		else
		{
			const u32 s(AreaUp.Margin.Top * w),
				d((AreaUp.GetHeight() - FetchResizedBottomMargin(AreaUp) - h)
				* w);

			AreaUp.Scroll(-hx);
			yunseq(ystdex::pod_copy_n(&AreaDown.GetBufferPtr()[s], t,
				&AreaUp.GetBufferPtr()[d]),
				ystdex::pod_copy_n(&AreaDown.GetBufferAlphaPtr()[s], t,
				&AreaUp.GetBufferAlphaPtr()[d]));
			AreaDown.Scroll(-hx);
			{
				u16 n(AreaDown.GetTextLineN());

				YAssert(n != 0,
					"No Enough height found @ DualScreenReader::Excute;");

				--n;
				AreaDown.ClearTextLine(n);
				SetCurrentTextLineNOf(AreaDown, n);
			}
			//注意缓冲区不保证以 '\0' 结尾。
			CarriageReturn(AreaDown);
			if(*iBottom == '\n')
				++iBottom;
			iBottom = PutLine(AreaDown, iBottom, pText->cend(), '\n');
			if(*iTop == '\n')
				++iTop;
			iTop = FindLineFeed(AreaUp, iTop, pText->cend());
		}
		Invalidate();
	}
	else
	{
		auto ln(AreaUp.GetTextLineN() + AreaDown.GetTextLineN());

		if(cmd & Up)
		{
			while(ln--)
				iTop = FindPreviousLineFeed(AreaUp, iTop, pText->cbegin());
		}
		else
		{
			while(ln-- && iBottom != pText->end())
			{
				if(*iBottom == '\n')
					++iBottom;
				iBottom = FindLineFeed(AreaDown, iBottom, pText->cend());
				if(*iTop == '\n')
					++iTop;
				iTop = FindLineFeed(AreaUp, iTop, pText->cend());
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
		iTop = pText->cbegin() + pos;
		if(pos != 0)
			iTop = FindPreviousLineFeed(AreaUp, iTop, pText->cbegin());
		UpdateView();
	}
}

void
DualScreenReader::Invalidate()
{
	using YSL_ Components::Invalidate;

	//强制刷新背景。
	Invalidate(AreaUp);
	Invalidate(AreaDown);
	if(ViewChanged)
		ViewChanged();
}

void
DualScreenReader::LoadText(TextFile& file)
{
	if(file.IsValid())
	{
		pText = unique_raw(new Text::TextFileBuffer(file));
		iTop = pText->begin();
		iBottom = pText->end();
		UpdateView();
	}
	else
		PutString(AreaUp, L"文件打开失败！\n");
}

void
DualScreenReader::PrintTextUp(const Graphics& g_up)
{
	BlitTo(g_up, AreaUp, Point::Zero, Point::Zero, rot);
}

void
DualScreenReader::PrintTextDown(const Graphics& g_down)
{
	BlitTo(g_down, AreaDown, Point::Zero, Point::Zero, rot);
}

void
DualScreenReader::Reset()
{
	//清除字符区域缓冲区。
	AreaUp.ClearImage();
	AreaDown.ClearImage();
	//复位缓存区域写入位置。
	AreaUp.ResetPen();
	AreaDown.ResetPen();
}

void
DualScreenReader::UnloadText()
{
	yunseq(iTop = Text::TextFileBuffer::Iterator(),
		iBottom = Text::TextFileBuffer::Iterator(),
		pText = nullptr);
}

void
DualScreenReader::UpdateView()
{
	Reset();
	{
		auto iNew(iTop);

		if(*iNew == '\n')
			++iNew;
		iNew = PutString(AreaUp, iNew);
		iBottom = (text_down = iNew != pText->cend())
			? PutString(AreaDown, iNew) : pText->cend();
	}
	if(iBottom != pText->cend() && *iBottom == '\n')
		--iBottom;
	Invalidate();
}

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

