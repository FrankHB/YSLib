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
\version r3297;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 14:04:05 +0800;
\par 修改时间:
	2011-12-04 11:11 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::DSReader;
*/


#include "DSReader.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

using namespace Drawing;
using namespace Text;

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

/*
void MDualScreenReader::SetCurrentTextLineNOf(u8 n)
{
	if(n >= 0)
		AreaUp.SetCurrentTextLineNOf(n);
}
*/

void
DualScreenReader::Invalidate()
{
	using YSLib::Components::Invalidate;

	//强制刷新背景。
	Invalidate(AreaUp);
	Invalidate(AreaDown);
}

bool
DualScreenReader::LineUp()
{
	if(IsTextTop())
		return false;

	const u8 h(fc.GetHeight()), hx(h + GetLineGapDown());
	const auto w(AreaUp.GetWidth());

	yunseq(AdjustBottomMarginOf(AreaUp), AdjustBottomMarginOf(AreaDown));

	const u32 t(w * h),
		s((AreaUp.GetHeight() - AreaUp.Margin.Bottom - h) * w),
		d(AreaDown.Margin.Top * w);

	AreaDown.Scroll(hx);
	yunseq(ystdex::pod_copy_n(&AreaUp.GetBufferPtr()[s], t,
		&AreaDown.GetBufferPtr()[d]), ystdex::pod_copy_n(
		&AreaUp.GetBufferAlphaPtr()[s], t, &AreaDown.GetBufferAlphaPtr()[d]));
	AreaUp.Scroll(hx);
	AreaUp.ClearTextLine(0);
	SetCurrentTextLineNOf(AreaUp, 0);

	const auto itUpOld(iTop);

	iTop = FindPrevious(AreaUp, iTop, pText->cbegin());
	PutLine(AreaUp, iTop, itUpOld);
	iBottom = FindPrevious(AreaUp, iBottom, pText->cbegin());
	Invalidate();
	return true;
}

bool
DualScreenReader::LineDown()
{
	if(IsTextBottom())
		return false;

	const u8 h(fc.GetHeight()), hx(h + GetLineGapUp());
	const auto w(AreaUp.GetWidth());
	const u32 t(w * h),
		s(AreaUp.Margin.Top * w),
		d((AreaUp.GetHeight() - FetchResizedBottomMargin(AreaUp) - h) * w);

	AreaUp.Scroll(-hx);
	yunseq(ystdex::pod_copy_n(&AreaDown.GetBufferPtr()[s], t,
		&AreaUp.GetBufferPtr()[d]), ystdex::pod_copy_n(
		&AreaDown.GetBufferAlphaPtr()[s], t, &AreaUp.GetBufferAlphaPtr()[d]));
	AreaDown.Scroll(-hx);
	AreaDown.ClearTextLineLast();
	AreaDown.SetTextLineLast();
	iBottom = PutLine(AreaDown, iBottom);
	iTop = FindNext(AreaUp, iTop, pText->cend());
	Invalidate();
	return true;
}

void
DualScreenReader::LoadText(TextFile& file)
{
	if(file.IsValid())
	{
		pText = ynew Text::TextFileBuffer(file);
		iTop = pText->begin();
		iBottom = pText->end();
		Update();
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

bool
DualScreenReader::ScreenUp()
{
	if(IsTextTop())
		return false;
	iTop = FindPrevious(AreaUp, iTop, pText->cbegin(),
		AreaUp.GetTextLineN() + AreaDown.GetTextLineN());
	Update();
	return true;
}
bool
DualScreenReader::ScreenDown()
{
	if(IsTextBottom())
		return false;

	int t(AreaUp.GetTextLineN() + AreaDown.GetTextLineN());

	while(t-- && iBottom != pText->end())
		yunseq((iTop = FindNext(AreaUp, iTop, pText->cend()),
			iBottom = FindNext(AreaDown, iBottom, pText->cend())));
//	itUp = itDn;
	Update();
	return true;
}

/*void
MDualScreenReader::Scroll(Function<void()> pCheck)
{
}*/

void
DualScreenReader::UnloadText()
{
	iTop = Text::TextFileBuffer::Iterator();
	iBottom = Text::TextFileBuffer::Iterator();
	safe_delete_obj()(pText);
}

void
DualScreenReader::Update()
{
	Reset();
	//文本填充：输出文本缓冲区字符串，并返回填充字符数。
	iBottom = PutString(AreaDown, PutString(AreaUp, iTop));
	Invalidate();
}

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

