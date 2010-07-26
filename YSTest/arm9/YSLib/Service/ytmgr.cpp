// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 17:48:09;
// UTime = 2010-7-26 6:06;
// Version = 0.3696;


#include "ytmgr.h"
#include "../Core/yapp.h"

YSL_BEGIN

using namespace Exceptions;

YSL_BEGIN_NAMESPACE(Text)

MTextBuffer::MTextBuffer(SizeType tlen)
try : mlen(tlen), text(new uchar_t[mlen]), len(0)
{
	ClearText();
}
catch(...)
{
	throw MLoggedEvent("Error occured @@ MTextBuffer::MTextBuffer();");
}

MTextBuffer::SizeType
MTextBuffer::GetPrevChar(SizeType o, uchar_t c)
{
	while(o-- && text[o] != c)
		;
	return ++o;
}
MTextBuffer::SizeType
MTextBuffer::GetNextChar(SizeType o, uchar_t c)
{
	while(o < mlen && text[o++] != c)
		;
	return o;
}

bool
MTextBuffer::Load(const uchar_t* s, SizeType n)
{
	if(n > mlen)
		return false;
	memcpy(text, s, sizeof(uchar_t) * n);
	len = n;
	return true;
}
MTextBuffer::SizeType
MTextBuffer::Load(YTextFile& f, SizeType n)
{
	SizeType l(0);

	if(f.IsValid())
	{
		SizeType i(0), t;
		uchar_t cb(0), c;
		FILE* const fp(f.GetPtr());
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
MTextBuffer::MTextBuffer::Output(uchar_t* d, SizeType p, SizeType n) const
{
	if(p + n > mlen)
		return false;
	memcpy(d, &text[p], sizeof(uchar_t) * n);
	return true;
}

void
MTextMap::clear()
{
	for(MapType::const_iterator i(Map.begin()); i != Map.end(); ++i)
		delete i->second;
	Map.clear();
}


MTextFileBuffer::MTextFileBuffer(YTextFile& file)
: File(file), nPos(0)
{}

MTextBlock&
MTextFileBuffer::operator[](const IndexType& i)
{
	try
	{
		if(i * 0x2000 > File.GetLength())
			throw std::out_of_range("");

		MapType::const_iterator it(Map.find(i));
		MTextBlock* p(it == Map.end() ? new MTextBlock(i, 0x2000) : it->second);

		if(it == Map.end())
			*this += *p;
		return *p;
	}
	catch(std::out_of_range&)
	{
		throw MLoggedEvent("Wrong range of file @@ MTextFileBuffer::operator[];", 3);
	}
	catch(std::bad_alloc&)
	{
		throw MLoggedEvent("Allocation failed @@ MTextFileBuffer::operator[];", 2);
	}
}

YSL_END_NAMESPACE(Text)

YSL_END

