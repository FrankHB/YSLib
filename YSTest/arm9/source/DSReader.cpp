// YReader -> DSReader by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-01-05 14:04:05 + 08:00;
// UTime = 2010-10-28 13:43 + 08:00;
// Version = 0.2952;


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
	itDn = pTrDn->PutString(pTrUp->PutString(itUp));
}

MDualScreenReader::MDualScreenReader(u16 l, u16 w, u16 t_up, u16 h_up,
									 u16 t_down, u16 h_down, YFontCache& fc_)
try	: pText(NULL), fc(fc_),
	left(l), top_up(t_up), top_down(t_down),
	pBgUp(pDesktopUp->GetBufferPtr()), pBgDn(pDesktopDown->GetBufferPtr()),
	pTrUp(new TextRegion(fc_)), pTrDn(new TextRegion(fc_)), rot(RDeg0),
	itUp(NULL), itDn(NULL)
{
	//初始化视图。
	pTrUp->SetSize(w, h_up);
	pTrDn->SetSize(w, h_down);
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
	pTrUp->Color = c;
	pTrDn->Color = c;
}
void
MDualScreenReader::SetLineGap(u8 g)
{
	pTrUp->LineGap = g;
	pTrDn->LineGap = g;
}
void
MDualScreenReader::SetFontSize(Font::SizeType fz)
{
	fc.SetFontSize(fz);	
	lnHeight = fc.GetHeight();
}
/*
void MDualScreenReader::SetLnNNow(u8 n)
{
	if(n >= 0)
		pTrUp->SetLnNNow(n);
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
		pText = new BlockedText(file);
		itUp = pText->Blocks.begin();
		itDn = pText->Blocks.end();
		Update();
	}
	else
		pTrUp->PutString(L"文件打开失败！\n");
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
//	pTrUp->BlitToBuffer(pBgUp, RDeg0, SCRW, SCRH, 0, 0, 0, 0,
//		pTrUp->Width, pTrUp->GetBufferHeightResized());
	pTrUp->BlitToBuffer(pBgUp, rot);
	pTrDn->BlitToBuffer(pBgDn, rot);
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

	const u8 h = lnHeight, hx = h + GetLnGapDn();
	const u16 w = pTrUp->Width;
	const u32 t = w * h,
		s = (pTrUp->Height - pTrUp->GetMarginResized() - h) * w,
		d = pTrDn->Margin.Top * w;

	pTrDn->Move(hx, pTrDn->GetBufferHeightResized());
	std::memcpy(&pTrDn->GetBufferPtr()[d], &pTrUp->GetBufferPtr()[s],
		t * sizeof(PixelType));
	std::memcpy(&pTrDn->GetBufferAlphaPtr()[d], &pTrUp->GetBufferAlphaPtr()[s],
		t * sizeof(u8));
	pTrUp->Move(hx, pTrUp->GetBufferHeightResized());
	pTrUp->ClearLn(0);
	SetLnNNow(*pTrUp, 0);

	TextFileBuffer::HText itUpOld(itUp);

	itUp = GetPreviousLinePtr(*pTrUp, itUp, pText->Blocks.begin());
	pTrUp->PutLine<TextFileBuffer::HText, uchar_t>(itUp, itUpOld);
	itDn = GetPreviousLinePtr(*pTrUp, itDn, pText->Blocks.begin());
	return true;
}
bool
MDualScreenReader::LineDown()
{
	if(IsTextBottom())
		return false;

	const u8 h = lnHeight, hx = h + GetLnGapUp();
	const u16 w = pTrUp->Width;
	const u32 t = w * h,
		s = pTrUp->Margin.Top * w,
		d = (pTrUp->Height - pTrUp->GetMarginResized() - h) * w;

	pTrUp->Move(-hx);
	std::memcpy(&pTrUp->GetBufferPtr()[d], &pTrDn->GetBufferPtr()[s],
		t * sizeof(PixelType));
	std::memcpy(&pTrUp->GetBufferAlphaPtr()[d], &pTrDn->GetBufferAlphaPtr()[s],
		t * sizeof(u8));
	pTrDn->Move(-hx);
	pTrDn->ClearLnLast();
	pTrDn->SetLnLast();
	itDn = pTrDn->PutLine(itDn);
	itUp = GetNextLinePtr(*pTrUp, itUp, pText->Blocks.end());
	return true;
}

bool
MDualScreenReader::ScreenUp()
{
	if(IsTextTop())
		return false;
	itUp = GetPreviousLinePtr(*pTrUp, itUp, pText->Blocks.begin(),
		pTrUp->GetLnN() + pTrDn->GetLnN());
	Update();
	return true;
}
bool
MDualScreenReader::ScreenDown()
{
	if(IsTextBottom())
		return false;

	int t(pTrUp->GetLnN() + pTrDn->GetLnN());

	while(t-- && itDn != pText->Blocks.end())
	{
		itUp = GetNextLinePtr(*pTrUp, itUp, pText->Blocks.end());
		itDn = GetNextLinePtr(*pTrDn, itDn, pText->Blocks.end());
	}
//	itUp = itDn;
	Update();
	return true;
}

void
MDualScreenReader::Scroll(PFVOID pCheck)
{
}

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

