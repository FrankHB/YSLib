/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textmgr.cpp
\ingroup Service
\brief 文本管理服务。
\version r4135;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 17:48:09 +0800;
\par 修改时间:
	2011-09-22 15:47 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextManager;
*/


#include "textmgr.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

TextBuffer::TextBuffer(SizeType tlen)
try : capacity(tlen), text(ynew ucs2_t[capacity]), len(0)
{
	ClearText();
}
catch(...)
{
	throw LoggedEvent("Error occured @ TextBuffer::TextBuffer;");
}

ucs2_t&
TextBuffer::operator[](SizeType idx) ynothrow
{
	YAssert(idx < capacity,
		"In function \"ucs2_t\n"
		"TextBuffer::operator[](SizeType)\":\n"
		"Subscript is not less than the length.");

	return text[idx];
}

SizeType
TextBuffer::GetPrevChar(SizeType o, ucs2_t c)
{
	while(o-- && text[o] != c)
		;
	return ++o;
}
SizeType
TextBuffer::GetNextChar(SizeType o, ucs2_t c)
{
	while(o < capacity && text[o++] != c)
		;
	return o;
}

ucs2_t&
TextBuffer::at(SizeType idx) ythrow(std::out_of_range)
{
	if(idx >= capacity)
		throw std::out_of_range("YSLib::Text::TextBuffer");
	return text[idx];
}

bool
TextBuffer::Load(const ucs2_t* s, SizeType n)
{
	if(n > capacity)
		return false;
	mmbcpy(text, s, sizeof(ucs2_t) * n);
	len = n;
	return true;
}
SizeType
TextBuffer::Load(TextFile& f, SizeType n)
{
	if(n > capacity)
		return 0;

	SizeType l(0);

	if(f.IsValid())
	{
		SizeType idx(0), t;
		ucs2_t cb(len == 0 ? 0 : text[len - 1]), c;
		std::FILE* const fp(f.GetPtr());
		const Encoding cp(f.GetCP());

		while(++idx < n && (t = MBCToUC(c, fp, cp)) != 0)
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

SizeType
TextBuffer::LoadN(TextFile& f, SizeType n)
{
	SizeType l(0);

	if(f.IsValid())
	{
		SizeType idx(0), t;
		ucs2_t cb(len == 0 ? 0 : text[len - 1]), c;
		std::FILE* const fp(f.GetPtr());
		const Encoding cp(f.GetCP());

		while(idx < n && (t = MBCToUC(c, fp, cp)) != 0 && (idx += t) < n)
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
TextBuffer::TextBuffer::Output(ucs2_t* d, SizeType p, SizeType n) const
{
	if(p + n > capacity)
		return false;
	mmbcpy(d, &text[p], sizeof(ucs2_t) * n);
	return true;
}


void
TextMap::Clear()
{
	for(auto i(Map.cbegin()); i != Map.cend(); ++i)
		ydelete(i->second);
	Map.clear();
}


TextFileBuffer::Iterator::Iterator(TextFileBuffer* pBuf, BlockSizeType b,
	SizeType idx) ynothrow
	: pBuffer(pBuf), block(b), index(idx)
{
//	assert(buf.GetTextSize() >= 1);
}

TextFileBuffer::Iterator&
TextFileBuffer::Iterator::operator++() ynothrow
{
	if(pBuffer)
	{
	//	assert(GetBlockLength() >= 1);
		if(block <= (pBuffer->GetTextSize() - 1) / nBlockSize)
		{
			if(++index == GetBlockLength())
			{
				++block;
				index = 0;
			}
		}
		else
			*this = pBuffer->end();
	}
	return *this;
}

TextFileBuffer::Iterator&
TextFileBuffer::Iterator::operator--() ynothrow
{
	if(pBuffer)
	{
	//	assert(GetBlockLength() >= 1);
		if(block != 0 || index != 0)
		{
			if(index-- == 0)
				index = GetBlockLength(--block);
		}
		else
			*this = pBuffer->end();
	}
	return *this;
}

ucs2_t
TextFileBuffer::Iterator::operator*() ynothrow
{
	const ucs2_t* p(GetTextPtr());

	return p ? *p : 0;
}

TextFileBuffer::Iterator
TextFileBuffer::Iterator::operator+(ptrdiff_t o)
{
	if(!pBuffer)
		return Iterator();

	Iterator i(*this);

	return i += o;
}

bool
operator==(const TextFileBuffer::Iterator& x, const TextFileBuffer::Iterator& y)
	ynothrow
{
	return x.pBuffer == y.pBuffer && x.block == y.block && x.index == y.index;
}

bool
operator<(const TextFileBuffer::Iterator& x, const TextFileBuffer::Iterator& y)
	ynothrow
{
	if(x.pBuffer < y.pBuffer)
		return true;
	if(x.pBuffer != y.pBuffer)
		return false;
	if(x.block < y.block)
		return true;
	if(x.block != y.block)
		return false;
	return x.index < y.index;
}

TextFileBuffer::Iterator&
TextFileBuffer::Iterator::operator+=(ptrdiff_t o)
{
	if(pBuffer)
	{
		if(o > 0)
			while(o-- != 0)
				++*this;
		else
			while(o++ != 0)
				--*this;
	}
	return *this;
}

const ucs2_t*
TextFileBuffer::Iterator::GetTextPtr() const ynothrow
{
	const ucs2_t* p(nullptr);

	if(pBuffer)
	{
		try
		{
			p = &(*pBuffer)[block].at(index);
		}
		catch(...)
		{
			return nullptr;
		}
	}
	return p;
}

SizeType
TextFileBuffer::Iterator::GetBlockLength(BlockSizeType idx) const ynothrow
{
	if(!pBuffer)
		return 0;
	try
	{
		try
		{
			return (*pBuffer)[idx].GetLength();
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


TextFileBuffer::TextFileBuffer(TextFile& file)
	: TextMap(),
	File(file), nTextSize(std::max<u32>(File.GetTextSize(), 1)),
	nBlock((nTextSize + nBlockSize - 1) / nBlockSize)
{}

TextBlock&
TextFileBuffer::operator[](const BlockSizeType& idx)
{
	try
	{
		if(idx * nBlockSize > File.GetSize())
			throw std::out_of_range("YSLib::Text::TextBlock");

		auto i(Map.find(idx));
		TextBlock& block(*(i == Map.cend()
			? ynew TextBlock(idx, nBlockSize) : i->second));

		if(i == Map.end())
		{
			File.SetPosition(idx * nBlockSize + File.GetBOMSize(), SEEK_SET);
			block.LoadN(File, nBlockSize);
			*this += block;
		}
		return block;
	}
	catch(std::out_of_range&)
	{
		throw LoggedEvent("Wrong range of file @ TextFileBuffer::operator[];",
			3);
	}
	catch(std::bad_alloc&)
	{
		throw LoggedEvent("Allocation failed @ TextFileBuffer::operator[];", 2);
	}
}

TextFileBuffer::Iterator
TextFileBuffer::begin() ynothrow
{
	return TextFileBuffer::Iterator(this);
}

TextFileBuffer::Iterator
TextFileBuffer::end() ynothrow
{
	return TextFileBuffer::Iterator(this,
		(nTextSize + nBlockSize - 1) / nBlockSize);
}

YSL_END_NAMESPACE(Text)

YSL_END

