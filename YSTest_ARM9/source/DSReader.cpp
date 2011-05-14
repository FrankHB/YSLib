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
\version 0.3119;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 14:04:05 +0800; 
\par 修改时间:
	2011-05-14 20:36 +0800;
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
	pTextRegionUp(new TextRegion(fc_)), pTextRegionDown(new TextRegion(fc_)),
	rot(RDeg0), itUp(), itDn()
{
	//初始化视图。
	pTextRegionUp->SetSize(w, h_up);
	pTextRegionDown->SetSize(w, h_down);
	SetFontSize();
	SetColor();
	SetLineGap();
}
catch(LoggedEvent&)
{
	throw;
}
catch(...)
{
	throw LoggedEvent("Error occured @ MDualScreenReader::MDualScreenReader;");
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
	pTextRegionUp->Color = c;
	pTextRegionDown->Color = c;
}
void
MDualScreenReader::SetLineGap(u8 g)
{
	pTextRegionUp->LineGap = g;
	pTextRegionDown->LineGap = g;
}
void
MDualScreenReader::SetFontSize(Font::SizeType fz)
{
	fc.SetFontSize(fz);	
	lnHeight = fc.GetHeight();
}
/*
void MDualScreenReader::SetLnNNowTo(u8 n)
{
	if(n >= 0)
		pTextRegionUp->SetLnNNowTo(n);
}
*/

void MDualScreenReader::FillText()
{
	itDn = PutString(*pTextRegionDown, PutString(*pTextRegionUp, itUp));
}

void
MDualScreenReader::Reset()
{
	Clear();
	ResetPen();
}

void
MDualScreenReader::LoadText(TextFile& file)
{
//	assert(pText);
	if(file.IsValid())
	{
		pText = ynew BlockedText(file);
		itUp = pText->Blocks.begin();
		itDn = pText->Blocks.end();
		Update();
	}
	else
		PutString(*pTextRegionUp, L"文件打开失败！\n");
}

void
MDualScreenReader::UnloadText()
{
	itUp = Text::TextFileBuffer::HText();
	itDn = Text::TextFileBuffer::HText();
	safe_delete_obj()(pText);
}

void
MDualScreenReader::PrintText(const Graphics& g_up, const Graphics& g_down)
{
	YAssert(pTextRegionUp,
		"Null pointer found @ MDualScreenReader::PrintText;");
	YAssert(pTextRegionDown,
		"Null pointer found @ MDualScreenReader::PrintText;");

	BlitTo(g_up, *pTextRegionUp, Point::Zero, Point::Zero, rot);
	BlitTo(g_down, *pTextRegionDown, Point::Zero, Point::Zero, rot);
}

void
MDualScreenReader::Update()
{
	Reset();
	FillText();
}

bool
MDualScreenReader::LineUp()
{
	if(IsTextTop())
		return false;

	const u8 h = lnHeight, hx = h + GetLineGapDown();
	const SDst w = pTextRegionUp->GetWidth();
	const u32 t = w * h,
		s = (pTextRegionUp->GetHeight() - FetchResizedMargin(*pTextRegionUp,
			pTextRegionUp->GetHeight()) - h) * w,
		d = pTextRegionDown->Margin.Top * w;

	pTextRegionDown->Scroll(hx, FetchResizedBufferHeight(*pTextRegionDown,
		pTextRegionDown->GetHeight()));
	std::memcpy(&pTextRegionDown->GetBufferPtr()[d],
		&pTextRegionUp->GetBufferPtr()[s], t * sizeof(PixelType));
	std::memcpy(&pTextRegionDown->GetBufferAlphaPtr()[d],
		&pTextRegionUp->GetBufferAlphaPtr()[s], t * sizeof(u8));
	pTextRegionUp->Scroll(hx, FetchResizedBufferHeight(*pTextRegionUp,
		pTextRegionUp->GetHeight()));
	pTextRegionUp->ClearLn(0);
	SetLnNNowTo(*pTextRegionUp, 0);

	TextFileBuffer::HText itUpOld(itUp);

	itUp = FetchPreviousLineIterator(*pTextRegionUp, itUp,
		pText->Blocks.begin());
	PutLine<TextFileBuffer::HText, uchar_t>(*pTextRegionUp, itUp, itUpOld);
	itDn = FetchPreviousLineIterator(*pTextRegionUp, itDn,
		pText->Blocks.begin());
	return true;
}
bool
MDualScreenReader::LineDown()
{
	if(IsTextBottom())
		return false;

	const u8 h = lnHeight, hx = h + GetLineGapUp();
	const SDst w = pTextRegionUp->GetWidth();
	const u32 t = w * h,
		s = pTextRegionUp->Margin.Top * w,
		d = (pTextRegionUp->GetHeight() - FetchResizedMargin(*pTextRegionUp,
			pTextRegionUp->GetHeight()) - h) * w;

	pTextRegionUp->Scroll(-hx);
	std::memcpy(&pTextRegionUp->GetBufferPtr()[d],
		&pTextRegionDown->GetBufferPtr()[s], t * sizeof(PixelType));
	std::memcpy(&pTextRegionUp->GetBufferAlphaPtr()[d],
		&pTextRegionDown->GetBufferAlphaPtr()[s], t * sizeof(u8));
	pTextRegionDown->Scroll(-hx);
	pTextRegionDown->ClearLnLast();
	pTextRegionDown->SetLnLast();
	itDn = PutLine(*pTextRegionDown, itDn);
	itUp = FetchNextLineIterator(*pTextRegionUp, itUp, pText->Blocks.end());
	return true;
}

bool
MDualScreenReader::ScreenUp()
{
	if(IsTextTop())
		return false;
	itUp = FetchPreviousLineIterator(*pTextRegionUp, itUp, pText->Blocks.begin(),
		pTextRegionUp->GetLnN() + pTextRegionDown->GetLnN());
	Update();
	return true;
}
bool
MDualScreenReader::ScreenDown()
{
	if(IsTextBottom())
		return false;

	int t(pTextRegionUp->GetLnN() + pTextRegionDown->GetLnN());

	while(t-- && itDn != pText->Blocks.end())
	{
		itUp = FetchNextLineIterator(*pTextRegionUp, itUp, pText->Blocks.end());
		itDn = FetchNextLineIterator(*pTextRegionDown, itDn,
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

