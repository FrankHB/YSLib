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
\brief 适用于 NDS 的双屏阅读器实现。
\version 0.3064;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 14:04:05 +0800; 
\par 修改时间:
	2011-03-05 17:05 +0800;
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

BlockedText::BlockedText(YTextFile& file)
	: File(file), Blocks(file)
{}


void MDualScreenReader::FillText()
{
	itDn = PutString(*pTextRegionDn, PutString(*pTextRegionUp, itUp));
}

MDualScreenReader::MDualScreenReader(SDST l, SDST w, SDST t_up, SDST h_up,
									 SDST t_down, SDST h_down, YFontCache& fc_)
try	: pText(NULL), fc(fc_),
	left(l), top_up(t_up), top_down(t_down),
	pBgUp(theApp.GetPlatformResource().GetDesktopUp().GetContext().
		GetBufferPtr()),
	pBgDn(theApp.GetPlatformResource().GetDesktopDown().GetContext().
		GetBufferPtr()),
	pTextRegionUp(new TextRegion(fc_)), pTextRegionDn(new TextRegion(fc_)), rot(RDeg0),
	itUp(NULL), itDn(NULL)
{
	//初始化视图。
	pTextRegionUp->SetSize(w, h_up);
	pTextRegionDn->SetSize(w, h_down);
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
	throw LoggedEvent("Error occured"
		" @@ MDualScreenReader::MDualScreenReader();");
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
	pTextRegionDn->Color = c;
}
void
MDualScreenReader::SetLineGap(u8 g)
{
	pTextRegionUp->LineGap = g;
	pTextRegionDn->LineGap = g;
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

void
MDualScreenReader::Reset()
{
	Clear();
	ResetPen();
}

void
MDualScreenReader::LoadText(YTextFile& file)
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
	itUp = NULL;
	itDn = NULL;
	safe_delete_obj()(pText);
}

void
MDualScreenReader::PrintText()
{
	pTextRegionUp->BlitTo(pBgUp, Size::FullScreen, Point::Zero,
		Point::Zero, Size::FullScreen, rot);
	pTextRegionDn->BlitTo(pBgDn, Size::FullScreen, Point::Zero,
		Point::Zero, Size::FullScreen, rot);
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

	const u8 h = lnHeight, hx = h + GetLineGapDn();
	const SDST w = pTextRegionUp->GetWidth();
	const u32 t = w * h,
		s = (pTextRegionUp->GetHeight() - FetchResizedMargin(*pTextRegionUp,
			pTextRegionUp->GetHeight()) - h) * w,
		d = pTextRegionDn->Margin.Top * w;

	pTextRegionDn->Scroll(hx, FetchResizedBufferHeight(*pTextRegionDn,
		pTextRegionDn->GetHeight()));
	std::memcpy(&pTextRegionDn->GetBufferPtr()[d],
		&pTextRegionUp->GetBufferPtr()[s], t * sizeof(PixelType));
	std::memcpy(&pTextRegionDn->GetBufferAlphaPtr()[d],
		&pTextRegionUp->GetBufferAlphaPtr()[s], t * sizeof(u8));
	pTextRegionUp->Scroll(hx, FetchResizedBufferHeight(*pTextRegionUp,
		pTextRegionUp->GetHeight()));
	pTextRegionUp->ClearLn(0);
	SetLnNNowTo(*pTextRegionUp, 0);

	TextFileBuffer::HText itUpOld(itUp);

	itUp = GetPreviousLinePtr(*pTextRegionUp, itUp, pText->Blocks.begin());
	PutLine<TextFileBuffer::HText, uchar_t>(*pTextRegionUp, itUp, itUpOld);
	itDn = GetPreviousLinePtr(*pTextRegionUp, itDn, pText->Blocks.begin());
	return true;
}
bool
MDualScreenReader::LineDown()
{
	if(IsTextBottom())
		return false;

	const u8 h = lnHeight, hx = h + GetLineGapUp();
	const SDST w = pTextRegionUp->GetWidth();
	const u32 t = w * h,
		s = pTextRegionUp->Margin.Top * w,
		d = (pTextRegionUp->GetHeight() - FetchResizedMargin(*pTextRegionUp,
			pTextRegionUp->GetHeight()) - h) * w;

	pTextRegionUp->Scroll(-hx);
	std::memcpy(&pTextRegionUp->GetBufferPtr()[d],
		&pTextRegionDn->GetBufferPtr()[s], t * sizeof(PixelType));
	std::memcpy(&pTextRegionUp->GetBufferAlphaPtr()[d],
		&pTextRegionDn->GetBufferAlphaPtr()[s], t * sizeof(u8));
	pTextRegionDn->Scroll(-hx);
	pTextRegionDn->ClearLnLast();
	pTextRegionDn->SetLnLast();
	itDn = PutLine(*pTextRegionDn, itDn);
	itUp = GetNextLinePtr(*pTextRegionUp, itUp, pText->Blocks.end());
	return true;
}

bool
MDualScreenReader::ScreenUp()
{
	if(IsTextTop())
		return false;
	itUp = GetPreviousLinePtr(*pTextRegionUp, itUp, pText->Blocks.begin(),
		pTextRegionUp->GetLnN() + pTextRegionDn->GetLnN());
	Update();
	return true;
}
bool
MDualScreenReader::ScreenDown()
{
	if(IsTextBottom())
		return false;

	int t(pTextRegionUp->GetLnN() + pTextRegionDn->GetLnN());

	while(t-- && itDn != pText->Blocks.end())
	{
		itUp = GetNextLinePtr(*pTextRegionUp, itUp, pText->Blocks.end());
		itDn = GetNextLinePtr(*pTextRegionDn, itDn, pText->Blocks.end());
	}
//	itUp = itDn;
	Update();
	return true;
}

/*void
MDualScreenReader::Scroll(PFVOID pCheck)
{
}*/

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

