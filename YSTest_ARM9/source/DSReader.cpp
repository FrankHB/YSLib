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
\version 0.3189;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 14:04:05 +0800;
\par 修改时间:
	2011-07-01 00:48 +0800;
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

BlockedText::BlockedText(TextFile& file)
	: File(file), Blocks(file)
{}


MDualScreenReader::MDualScreenReader(SDst w, SDst h_up, SDst h_down,
	FontCache& fc_)
try	: pText(), fc(fc_),
	rot(RDeg0), itUp(), itDn(),
	AreaUp(Rect(Point::Zero, w, h_up), fc),
	AreaDown(Rect(Point::Zero, w, h_down), fc)
{
	SetFontSize();
	SetColor();
	SetLineGap();
	AreaUp.SetTransparent(true);
	AreaDown.SetTransparent(true);
}
catch(LoggedEvent&)
{
	throw;
}

bool
MDualScreenReader::IsTextTop()
{
	return itUp == pText->Blocks.begin();
}
bool
MDualScreenReader::IsTextBottom()
{
	return itDn == pText->Blocks.end();
}

void
MDualScreenReader::SetColor(Color c)
{
	AreaUp.Color = c;
	AreaDown.Color = c;
}
void
MDualScreenReader::SetLineGap(u8 g)
{
	AreaUp.LineGap = g;
	AreaDown.LineGap = g;
}
void
MDualScreenReader::SetFontSize(Font::SizeType fz)
{
	fc.SetFontSize(fz);
	lnHeight = fc.GetHeight();
}
/*
void MDualScreenReader::SetCurrentTextLineNTo(u8 n)
{
	if(n >= 0)
		AreaUp.SetCurrentTextLineNTo(n);
}
*/

void
MDualScreenReader::Reset()
{
	//清除字符区域缓冲区。
	AreaUp.ClearImage();
	AreaDown.ClearImage();
	//复位缓存区域写入位置。
	AreaUp.ResetPen();
	AreaDown.ResetPen();
}

void
MDualScreenReader::LoadText(TextFile& file)
{
	if(file.IsValid())
	{
		pText = ynew BlockedText(file);
		itUp = pText->Blocks.begin();
		itDn = pText->Blocks.end();
		Update();
	}
	else
		PutString(AreaUp, L"文件打开失败！\n");
}

void
MDualScreenReader::UnloadText()
{
	itUp = Text::TextFileBuffer::HText();
	itDn = Text::TextFileBuffer::HText();
	safe_delete_obj()(pText);
}

void
MDualScreenReader::PrintTextUp(const Graphics& g_up)
{
	BlitTo(g_up, AreaUp, Point::Zero, Point::Zero, rot);
}

void
MDualScreenReader::PrintTextDown(const Graphics& g_down)
{
	BlitTo(g_down, AreaDown, Point::Zero, Point::Zero, rot);
}

void
MDualScreenReader::Update()
{
	Reset();
	//文本填充：输出文本缓冲区字符串，并返回填充字符数。
	itDn = PutString(AreaDown, PutString(AreaUp, itUp));
}

bool
MDualScreenReader::LineUp()
{
	if(IsTextTop())
		return false;

	const u8 h = lnHeight, hx = h + GetLineGapDown();
	const SDst w = AreaUp.GetWidth();
	const u32 t = w * h,
		s = (AreaUp.GetHeight() - FetchResizedMargin(AreaUp,
			AreaUp.GetHeight()) - h) * w,
		d = AreaDown.Margin.Top * w;

	AreaDown.Scroll(hx, FetchResizedBufferHeight(AreaDown,
		AreaDown.GetHeight()));
	std::memcpy(&AreaDown.GetBufferPtr()[d],
		&AreaUp.GetBufferPtr()[s], t * sizeof(PixelType));
	std::memcpy(&AreaDown.GetBufferAlphaPtr()[d],
		&AreaUp.GetBufferAlphaPtr()[s], t * sizeof(u8));
	AreaUp.Scroll(hx, FetchResizedBufferHeight(AreaUp,
		AreaUp.GetHeight()));
	AreaUp.ClearTextLine(0);
	SetCurrentTextLineNTo(AreaUp, 0);

	TextFileBuffer::HText itUpOld(itUp);

	itUp = FetchPreviousLineIterator(AreaUp, itUp,
		pText->Blocks.begin());
	PutLine<TextFileBuffer::HText, uchar_t>(AreaUp, itUp, itUpOld);
	itDn = FetchPreviousLineIterator(AreaUp, itDn,
		pText->Blocks.begin());
	return true;
}
bool
MDualScreenReader::LineDown()
{
	if(IsTextBottom())
		return false;

	const u8 h = lnHeight, hx = h + GetLineGapUp();
	const SDst w = AreaUp.GetWidth();
	const u32 t = w * h,
		s = AreaUp.Margin.Top * w,
		d = (AreaUp.GetHeight() - FetchResizedMargin(AreaUp,
			AreaUp.GetHeight()) - h) * w;

	AreaUp.Scroll(-hx);
	std::memcpy(&AreaUp.GetBufferPtr()[d],
		&AreaDown.GetBufferPtr()[s], t * sizeof(PixelType));
	std::memcpy(&AreaUp.GetBufferAlphaPtr()[d],
		&AreaDown.GetBufferAlphaPtr()[s], t * sizeof(u8));
	AreaDown.Scroll(-hx);
	AreaDown.ClearLnLast();
	AreaDown.SetLnLast();
	itDn = PutLine(AreaDown, itDn);
	itUp = FetchNextLineIterator(AreaUp, itUp, pText->Blocks.end());
	return true;
}

bool
MDualScreenReader::ScreenUp()
{
	if(IsTextTop())
		return false;
	itUp = FetchPreviousLineIterator(AreaUp, itUp, pText->Blocks.begin(),
		AreaUp.GetLnN() + AreaDown.GetLnN());
	Update();
	return true;
}
bool
MDualScreenReader::ScreenDown()
{
	if(IsTextBottom())
		return false;

	int t(AreaUp.GetLnN() + AreaDown.GetLnN());

	while(t-- && itDn != pText->Blocks.end())
	{
		itUp = FetchNextLineIterator(AreaUp, itUp, pText->Blocks.end());
		itDn = FetchNextLineIterator(AreaDown, itDn,
			pText->Blocks.end());
	}
//	itUp = itDn;
	Update();
	return true;
}

/*void
MDualScreenReader::Scroll(Function<void()> pCheck)
{
}*/

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

