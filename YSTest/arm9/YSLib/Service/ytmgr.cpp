// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 17:48:09;
// UTime = 2010-7-9 9:15;
// Version = 0.3562;


#include "ytmgr.h"
#include "../Core/yapp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

MTextBuffer::MTextBuffer(u32 tlen)
try : mlen(tlen), text(new uchar_t[mlen]), len(0)
{
	ClearText();
}
catch(...)
{
	theApp.log.Error("MTextBuffer::MTextBuffer");
}

u32
MTextBuffer::GetPrevChar(u32 o, uchar_t c)
{
	while(o-- && text[o] != c)
		;
	return ++o;
}
u32
MTextBuffer::GetNextChar(u32 o, uchar_t c)
{
	while(o < mlen && text[o++] != c)
		;
	return o;
}

bool
MTextBuffer::Load(const uchar_t* s, u32 n)
{
	if(n > mlen)
		return false;
	memcpy(text, s, sizeof(uchar_t) * n);
	len = n;
	return true;
}

u32
MTextBuffer::Load(YTextFile& f, u32 n)
{
	u32 l(0);

	if(f.IsValid())
	{
		u32 i(0), t;
		uchar_t cb(0), c;
		FILE* const fp(f.GetFilePtr());
		const CSID cp(f.GetCP());

		while(i < n && (t = ToUTF(fp, c, cp)))
		{
			i += t;
			if(c == '\n' && cb == '\r')
				--len;
			else if(cb == '\r' && len)
				text[len - 1] = '\n';
			cb = c;
			text[len++] = c;
			++l;
		}
	}
	return l;
}

bool
MTextBuffer::Output(uchar_t* d, u32 p, u32 n) const
{
	if(p + n > mlen)
		return false;
	memcpy(d, &text[p], sizeof(uchar_t) * n);
	return true;
}

YSL_END_NAMESPACE(Text)

YSL_END

