// YSLib::Service::YTextManager by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 17:48:09;
// UTime = 2010-9-2 10:29;
// Version = 0.3973;


#include "ytmgr.h"
#include "../Core/yapp.h"

YSL_BEGIN

using namespace Exceptions;

YSL_BEGIN_NAMESPACE(Text)

TextBuffer::TextBuffer(IndexType tlen)
try : mlen(tlen), text(new uchar_t[mlen]), len(0)
{
	ClearText();
}
catch(...)
{
	throw LoggedEvent("Error occured @@ TextBuffer::TextBuffer();");
}

uchar_t&
TextBuffer::operator[](IndexType i) ythrow()
{
	YAssert(i < mlen,
		"In function \"uchar_t\n"
		"TextBuffer::operator[](SizeType i)\":\n"
		"Subscript is not less than the length.")
	return text[i];
}

IndexType
TextBuffer::GetPrevChar(IndexType o, uchar_t c)
{
	while(o-- && text[o] != c)
		;
	return ++o;
}
IndexType
TextBuffer::GetNextChar(IndexType o, uchar_t c)
{
	while(o < mlen && text[o++] != c)
		;
	return o;
}

uchar_t&
TextBuffer::at(IndexType i) ythrow(std::out_of_range)
{
	if(i >= mlen)
		throw std::out_of_range("YSLib::Text::TextBuffer");
	return text[i];
}

bool
TextBuffer::Load(const uchar_t* s, IndexType n)
{
	if(n > mlen)
		return false;
	memcpy(text, s, sizeof(uchar_t) * n);
	len = n;
	return true;
}
IndexType
TextBuffer::Load(YTextFile& f, IndexType n)
{
	IndexType l(0);

	if(f.IsValid())
	{
		IndexType i(0), t;
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

IndexType
TextBuffer::LoadN(YTextFile& f, IndexType n)
{
	IndexType l(0);

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
TextBuffer::TextBuffer::Output(uchar_t* d, IndexType p, IndexType n) const
{
	if(p + n > mlen)
		return false;
	memcpy(d, &text[p], sizeof(uchar_t) * n);
	return true;
}


void
TextMap::clear()
{
	for(MapType::const_iterator i(Map.begin()); i != Map.end(); ++i)
		delete i->second;
	Map.clear();
}


TextFileBuffer::HText::HText(TextFileBuffer& buf, BlockIndexType b, IndexType i) ythrow()
: pBuf(&buf), blk(b), idx(i)
{
//	assert(buf.GetTextSize() >= 1);
}

TextFileBuffer::HText&
TextFileBuffer::HText::operator++() ythrow()
{
//	assert(pBuf != NULL);
	if(blk < pBuf->GetTextSize() / nBlockSize)
	{
		if(idx == GetBlockLength())
		{
			++blk;
			idx = 0;
		}
		else
			++idx;
	}
	else
		*this = pBuf->end();
	return *this;
}

TextFileBuffer::HText&
TextFileBuffer::HText::operator--() ythrow()
{
//	assert(pBuf != NULL);
	if(blk != 0 || idx != 0)
	{
		if(idx == 0)
			idx = GetBlockLength(--blk);
		else
			--idx;
	}
	else
		*this = pBuf->end();
	return *this;
}

uchar_t
TextFileBuffer::HText::operator*() ythrow()
{
	const uchar_t* p(GetTextPtr());

	return p == NULL ? 0 : *p;
}

TextFileBuffer::HText
TextFileBuffer::HText::operator+(std::ptrdiff_t o)
{
//	assert(pBuf != NULL);
	HText i(*this);

	return i += o;
}

bool
operator==(const TextFileBuffer::HText& lhs, const TextFileBuffer::HText& rhs) ythrow()
{
	return lhs.pBuf == rhs.pBuf && lhs.blk == rhs.blk && lhs.idx == rhs.idx;
}

bool
operator<(const TextFileBuffer::HText& lhs, const TextFileBuffer::HText& rhs) ythrow()
{
	if(lhs.pBuf < rhs.pBuf)
		return true;
	if(lhs.pBuf != rhs.pBuf)
		return false;
	if(lhs.blk < rhs.blk)
		return true;
	if(lhs.blk != rhs.blk)
		return false;
	return lhs.idx < rhs.idx;
}

TextFileBuffer::HText&
TextFileBuffer::HText::operator+=(std::ptrdiff_t o)
{
//	assert(pBuf != NULL);
	if(o > 0)
		while(o-- != 0)
			++*this;
	else
		while(o++ != 0)
			--*this;
	return *this;
}

const uchar_t*
TextFileBuffer::HText::GetTextPtr() const ythrow()
{
	const uchar_t* p(NULL);

//	assert(pBuf != NULL);
	try
	{
		p = &(*pBuf)[blk].at(idx);
	}
	catch(...)
	{
		return NULL;
	}
	return p;
}

IndexType
TextFileBuffer::HText::GetBlockLength(BlockIndexType i) const ythrow()
{
	try
	{
		try
		{
			return (*pBuf)[i].GetLength();
		}
		catch(LoggedEvent&)
		{
			// TODO: 日志记录。
			throw;
		}
	}
	catch(...)
	{
		return 0;
	}
}


TextFileBuffer::TextFileBuffer(YTextFile& file)
: File(file), nTextSize(std::max<u32>(File.GetTextSize(), 1)), nBlock(nTextSize / nBlockSize)
{}

TextBlock&
TextFileBuffer::operator[](const BlockIndexType& i)
{
	try
	{
		if(i * nBlockSize > File.GetSize())
			throw std::out_of_range("YSLib::Text::TextBlock");

		MapType::const_iterator it(Map.find(i));
		TextBlock& block(*(it == Map.end() ? new TextBlock(i, nBlockSize) : it->second));

		if(it == Map.end())
		{
			File.fseek(i * nBlockSize + File.GetBOMSize(), SEEK_SET);
			block.LoadN(File, nBlockSize);
			*this += block;
		}
		return block;
	}
	catch(std::out_of_range&)
	{
		throw LoggedEvent("Wrong range of file @@ TextFileBuffer::operator[];", 3);
	}
	catch(std::bad_alloc&)
	{
		throw LoggedEvent("Allocation failed @@ TextFileBuffer::operator[];", 2);
	}
}

YSL_END_NAMESPACE(Text)

YSL_END

