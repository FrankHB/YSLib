// YReader -> DSReader by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 14:04:05;
// UTime = 2010-9-23 15:31;
// Version = 0.2796;


#include "DSReader.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

using namespace Drawing;
using namespace Text;

YSL_BEGIN_NAMESPACE(Components)

void MDualScreenReader::FillText()
{
	itDn = trDn.PutString(trUp.PutString(itUp));
}

MDualScreenReader::MDualScreenReader(YTextFile& tf_,
				   u16 l, u16 w, u16 t_up, u16 h_up, u16 t_down, u16 h_down,
				   YFontCache& fc_)
try : tf(tf_), Blocks(tf), fc(fc_),
left(l), top_up(t_up), top_down(t_down),
pBgUp(pDesktopUp->GetBufferPtr()), pBgDn(pDesktopDown->GetBufferPtr()),
trUp(*new TextRegion(fc_)), trDn(*new TextRegion(fc_)), rot(RDeg0),
itUp(Blocks), itDn(Blocks)
{
	trUp.SetSize(w, h_up);
	trDn.SetSize(w, h_down);
	SetFontSize();
	SetColor();
	SetLineGap();
	if(!tf.IsValid())
		trUp.PutString(L"文件打开失败！\n");
	InitText();
}
catch(LoggedEvent&)
{
	throw;
}
catch(...)
{
	throw LoggedEvent("Error occured @@ MDualScreenReader::MDualScreenReader();");
}
MDualScreenReader::~MDualScreenReader()
{
	delete &trUp;
	delete &trDn;
}

bool
MDualScreenReader::IsTextTop()
{
	return itUp == Blocks.begin();
}
bool
MDualScreenReader::IsTextBottom()
{
	return itUp == Blocks.end();
}

void
MDualScreenReader::SetColor(PixelType c)
{
	trUp.Color = c;
	trDn.Color = c;
}
void
MDualScreenReader::SetLineGap(u8 g)
{
	trUp.SetLineGap(g);
	trDn.SetLineGap(g);
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
		trUp.SetLnNNow(n);
}
*/

void
MDualScreenReader::Reset()
{
	Clear();
	ResetPen();
}
void
MDualScreenReader::PrintText()
{
//	trUp.BlitToBuffer(pBgUp, RDeg0, SCRW, SCRH, 0, 0, 0, 0, trUp.GetWidth(), trUp.GetBufferHeightResized());
	trUp.BlitToBuffer(pBgUp, rot);
	trDn.BlitToBuffer(pBgDn, rot);
}
void
MDualScreenReader::InitText()
{
	itUp = Blocks.begin();
	Update();
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
	const u16 w = trUp.GetWidth();
	const u32 t = w * h, s = (trUp.GetHeight() - trUp.GetMarginResized() - h) * w, d = trDn.Margin.Top * w;

	trDn.Move(hx, trDn.GetBufferHeightResized());
	std::memcpy(&trDn.GetBufferPtr()[d], &trUp.GetBufferPtr()[s], t * sizeof(PixelType));
	std::memcpy(&trDn.GetBufferAlphaPtr()[d], &trUp.GetBufferAlphaPtr()[s], t * sizeof(u8));
	trUp.Move(hx, trUp.GetBufferHeightResized());
	trUp.ClearLn(0);
	trUp.SetLnNNow(0);

	TextFileBuffer::HText itUpOld(itUp);

	itUp = GetPreviousLinePtr(trUp, itUp, Blocks.begin());
	trUp.PutLine<TextFileBuffer::HText, uchar_t>(itUp, itUpOld);
	itDn = GetPreviousLinePtr(trDn, itDn, Blocks.begin());
	return true;
}
bool
MDualScreenReader::LineDown()
{
	if(IsTextBottom())
		return false;

	const u8 h = lnHeight, hx = h + GetLnGapUp();
	const u16 w = trUp.GetWidth();
	const u32 t = w * h, s = trUp.Margin.Top * w, d = (trUp.GetHeight() - trUp.GetMarginResized() - h) * w;

	trUp.Move(-hx);
	std::memcpy(&trUp.GetBufferPtr()[d], &trDn.GetBufferPtr()[s], t * sizeof(PixelType));
	std::memcpy(&trUp.GetBufferAlphaPtr()[d], &trDn.GetBufferAlphaPtr()[s], t * sizeof(u8));
	trDn.Move(-hx);
	trDn.ClearLnLast();
	trDn.SetLnLast();
	itDn = trDn.PutLine(itDn);
	itUp = GetNextLinePtr(trUp, itUp, Blocks.end());
	return true;
}

bool
MDualScreenReader::ScreenUp()
{
	if(IsTextTop())
		return false;
	itUp = GetPreviousLinePtr(trUp, itUp, Blocks.begin(), trUp.GetLnN() + trDn.GetLnN());
	Update();
	return true;
}
bool
MDualScreenReader::ScreenDown()
{
	if(IsTextBottom())
		return false;
	itUp = itDn;
	Update();
	return true;
}

void
MDualScreenReader::Scroll(PFVOID pCheck)
{

}


void
YWndDSReader::DrawForeground()
{
}

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

