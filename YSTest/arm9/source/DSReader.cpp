// YReader -> DSReader by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 14:04:05;
// UTime = 2010-8-5 1:43;
// Version = 0.2722;


#include "DSReader.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

using namespace Drawing;
using namespace Exceptions;
using namespace Text;

YSL_BEGIN_NAMESPACE(Components)

u32 MDualScreenReader::TextFill()
{
//	MTextFileBuffer::TextIterator it(Blocks.begin());
	MTextFileBuffer::TextIterator it(Blocks, 0, offUp);
	MTextFileBuffer::TextIterator it2(it);
	std::size_t n(0);

	it = trUp.PutString(it);
	it = trDn.PutString(it);
	while(it2 != it)
	{
		++it2;
		++n;
	}
//	it += n = trUp.PutString(it);
//	it += trDn.PutString(it);
	return n;
}

MDualScreenReader::MDualScreenReader(YTextFile& tf_,
				   u16 l, u16 w, u16 t_up, u16 h_up, u16 t_down, u16 h_down,
				   YFontCache& fc_)
try : tf(tf_), Blocks(tf), fc(fc_),
left(l), top_up(t_up), top_down(t_down),
pBgUp(pDesktopUp->GetBackgroundPtr()), pBgDn(pDesktopDown->GetBackgroundPtr()),
trUp(*new MTextRegion(fc_)), trDn(*new MTextRegion(fc_)), rot(RDeg0),
offUp(0), offDn(0)
{
	trUp.SetSize(w, h_up);
	trDn.SetSize(w, h_down);
	SetFontSize();
	SetColor();
	SetLineGap();
	if(!tf.IsValid())
		trUp.PutString(L"文件打开失败！\n");
	TextInit();
}
catch(MLoggedEvent&)
{
	throw;
}
catch(...)
{
	throw MLoggedEvent("Error occured @@ MDualScreenReader::MDualScreenReader();");
}
MDualScreenReader::~MDualScreenReader()
{
	delete &trUp;
	delete &trDn;
}

bool
MDualScreenReader::IsTextTop()
{
	return !offUp;
}
bool
MDualScreenReader::IsTextBottom()
{
	return false; //Temp;
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
MDualScreenReader::SetFontSize(MFont::SizeType fz)
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
//	trUp.BlitToBuffer(pBgUp, RDeg0);
	trUp.BlitToBuffer(pBgUp, rot);
	trDn.BlitToBuffer(pBgDn, rot);
}
void
MDualScreenReader::Refresh()
{
	InsertMessage(NowShell(), SM_DSR_REFRESH, 0xD2, reinterpret_cast<WPARAM>(this));
}
void
MDualScreenReader::TextInit()
{
	Reset();
	offUp = 0;
	offDn = TextFill();
}
void
MDualScreenReader::Update()
{
	Reset();
	offDn = offUp + TextFill();
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
//	offUp = GetPrevLnOff(trUp, Blocks[0].GetPtr(), offUp) - Blocks[0].GetPtr();
	offUp = GetPreviousLinePtr(trUp, Blocks[0].GetPtr() + offUp, Blocks[0].GetPtr()) - Blocks[0].GetPtr();
	trUp.PutLine(&Blocks[0].GetPtr()[offUp]);
//	offDn = GetPrevLnOff(trDn, Blocks[0].GetPtr(), offDn) - Blocks[0].GetPtr();
	offDn = GetPreviousLinePtr(trDn, Blocks[0].GetPtr() + offDn, Blocks[0].GetPtr()) - Blocks[0].GetPtr();
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
	offDn = trDn.PutLine(&Blocks[0].GetPtr()[offDn]) - Blocks[0].GetPtr();
//	offUp = GetNextLnOff(trUp, Blocks[0].GetPtr(), offUp) - Blocks[0].GetPtr();
	offUp = GetNextLinePtr(trUp, Blocks[0].GetPtr() + offUp, Blocks[0].GetPtr() + Blocks[0].GetLength()) - Blocks[0].GetPtr();
	return true;
}

bool
MDualScreenReader::ScreenUp()
{
	if(IsTextTop())
		return false;
//	offUp = GetPrevLnOff(trUp, Blocks[0].GetPtr(), offUp, trUp.GetLnN() + trDn.GetLnN()) - Blocks[0].GetPtr();
	offUp = GetPreviousLinePtr(trUp, Blocks[0].GetPtr() + offUp, Blocks[0].GetPtr(), trUp.GetLnN() + trDn.GetLnN()) - Blocks[0].GetPtr();
	Update();
	return true;
}
bool
MDualScreenReader::ScreenDown()
{
	if(IsTextBottom())
		return false;
	offUp = offDn;
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

