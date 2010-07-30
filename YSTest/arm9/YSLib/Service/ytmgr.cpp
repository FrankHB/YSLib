// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 17:48:09;
// UTime = 2010-7-30 21:15;
// Version = 0.3790;


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
		uchar_t cb(len == 0 ? 0 : text[len - 1]), c;
		FILE* const fp(f.GetPtr());
		const CSID cp(f.GetCP());

		while(++i < n && (t = ToUTF(fp, c, cp)) != 0)
		{
			if(c == '\n' && cb == '\r')
				--len;
			else if(cb == '\r' && len != 0)
				text[len - 1] = '\n';
			cb = c;
			text[len++] = c;
			++l;
		}
	}
	return l;
}

MTextBuffer::SizeType
MTextBuffer::LoadN(YTextFile& f, SizeType n)
{
	SizeType l(0);

	if(f.IsValid())
	{
		SizeType i(0), t;
		uchar_t cb(len == 0 ? 0 : text[len - 1]), c;
		FILE* const fp(f.GetPtr());
		const CSID cp(f.GetCP());

		while(i < n && (t = ToUTF(fp, c, cp)) != 0 && (i += t) < n)
		{
			if(c == '\n' && cb == '\r')
				--len;
			else if(cb == '\r' && len != 0)
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


MTextFileBuffer::TextIterator::TextIterator(MTextFileBuffer& b, SizeType n) ythrow()
: pBuf(&b), nPos(n)
{
//	assert(b.GetTextLength() >= 1);
	if(nPos > b.GetTextLength())
		nPos = 0;
}

MTextFileBuffer::TextIterator&
MTextFileBuffer::TextIterator::operator++() ythrow()
{
//	assert(pBuf != NULL);
	if(++nPos == pBuf->GetTextLength())
		nPos = 0;
	return *this;
}

MTextFileBuffer::TextIterator&
MTextFileBuffer::TextIterator::operator--() ythrow()
{
//	assert(pBuf != NULL);
	if(nPos-- == 0)
		nPos = pBuf->GetTextLength() - 1;
	return *this;
}

uchar_t
MTextFileBuffer::TextIterator::operator*() ythrow()
{
	const uchar_t* p(GetTextPtr());

	return p == NULL ? 0 : *p;
}

MTextFileBuffer::TextIterator
MTextFileBuffer::TextIterator::operator+(std::ptrdiff_t o)
{
//	assert(pBuf != NULL);
	return isInIntervalRegular(nPos + o, pBuf->GetTextLength()) ? TextIterator(*pBuf, nPos + o) : TextIterator(*this);
}

MTextFileBuffer::TextIterator&
MTextFileBuffer::TextIterator::operator+=(std::ptrdiff_t o)
{
//	assert(pBuf != NULL);
	if(isInIntervalRegular(nPos + o, pBuf->GetTextLength()))
		nPos += o;
	return *this;
}

const uchar_t*
MTextFileBuffer::TextIterator::GetTextPtr() ythrow()
{
	const uchar_t* p(NULL);

//	assert(pBuf != NULL);
	try
	{
		p = (*pBuf)[nPos / nBlockSize].GetPtr() + nPos % nBlockSize;
	}
	catch(...)
	{
		return NULL;
	}
	return p;
}

MTextFileBuffer::MTextFileBuffer(YTextFile& file)
: File(file), nLen(std::max<u32>(File.GetLength() - File.GetBOMLen(), 1))
{}

MTextBlock&
MTextFileBuffer::operator[](const IndexType& i)
{
	try
	{
		if(i * nBlockSize > File.GetLength())
			throw std::out_of_range("");

		MapType::const_iterator it(Map.find(i));
		MTextBlock& block(*(it == Map.end() ? new MTextBlock(i, nBlockSize) : it->second));

		if(it == Map.end())
		{
			File.fseek(i * nBlockSize + File.GetBOMLen(), SEEK_SET);
			block.LoadN(File, nBlockSize);
			*this += block;
		}
		return block;
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

