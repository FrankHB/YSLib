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
\version r3398;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 14:04:05 +0800;
\par 修改时间:
	2011-12-19 12:22 +0800;
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
	\brief 指定迭代器上确界 b ，在 r 中取当前文本迭代器 s 的后一行首对应文本迭代器。
	\since build 270 。
	*/
	template<typename _tRan>
	_tRan
	FindNextLine(const TextRegion& r, _tRan s, _tRan e)
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
	\brief 指定迭代器最小值 b ，在 r 中取当前文本迭代器 s 的前一行首对应文本迭代器。
	\since build 270 。
	*/
	template<typename _tRan>
	_tRan
	FindPreviousLine(TextRegion& r, _tRan s, _tRan b)
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
				t = FindNextLine(r, t, e);
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
	rot(RDeg0), iTop(), iBottom(),
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
		// NOTE: assume GetLineGapDown() == GetLineGapUp();
		const u8 h(fc.GetHeight()), hx(h + GetLineGapDown());
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
			iTop = FindPreviousLine(AreaUp, iTop, pText->cbegin());
			CarriageReturn(AreaUp);
			{
				auto iTopNew(iTop);

				if(*iTopNew == '\n')
					++iTopNew;
				PutLine(AreaUp, iTopNew, pText->cend(), '\n');
			}
			iBottom = FindPreviousLine(AreaUp, iBottom, pText->cbegin());
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
			iTop = FindNextLine(AreaUp, iTop, pText->cend());
		}
		Invalidate();
	}
	else
	{
		auto ln(AreaUp.GetTextLineN() + AreaDown.GetTextLineN());

		if(cmd & Up)
		{
			while(ln--)
				iTop = FindPreviousLine(AreaUp, iTop, pText->cbegin());
		}
		else
		{
			while(ln-- && iBottom != pText->end())
			{
				if(*iBottom == '\n')
					++iBottom;
				iBottom = FindNextLine(AreaDown, iBottom, pText->cend());
				if(*iTop == '\n')
					++iTop;
				iTop = FindNextLine(AreaUp, iTop, pText->cend());
			}
		}
		UpdateView();
	}
	return true;
}

void
DualScreenReader::Invalidate()
{
	using YSL_ Components::Invalidate;

	//强制刷新背景。
	Invalidate(AreaUp);
	Invalidate(AreaDown);
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
		auto iTopNew(iTop);

		if(*iTopNew == '\n')
			++iTopNew;
		iBottom = PutString(AreaDown, PutString(AreaUp, iTopNew));
	}
	if(*iBottom == '\n')
		--iBottom;
	Invalidate();
}

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

