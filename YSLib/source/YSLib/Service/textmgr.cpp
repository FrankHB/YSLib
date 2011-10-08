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
\version r4214;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 17:48:09 +0800;
\par 修改时间:
	2011-10-08 16:25 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextManager;
*/


#include "textmgr.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

TextBlock::TextBlock(BlockSizeType idx, SizeType tlen)
	: vector<ucs2_t>(), Index(idx)
{
	reserve(tlen);
}


SizeType
LoadText(TextBlock& b, TextFile& f, SizeType n)
{
	if(n > b.capacity())
		return 0;
	if(f.IsValid())
	{
		auto l(b.size());

		b.resize(l + n);

		const auto l_old(l);
		const auto fp(f.GetPtr());
		const auto cp(f.GetEncoding());
		SizeType idx(0), t;
		ucs2_t c;
		ConversionState st;

		while(idx < n && (t = MBCToUC(c, fp, cp, st)) != 0
			&& GetCountOf(st) >= 0 && (idx += t) < n)
			b[l++] = c;
		return l - l_old;
	}
	return 0;
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
		try
		{
			p = &pBuffer->at(block).at(index);
		}
		catch(...)
		{}
	return p;
}

SizeType
TextFileBuffer::Iterator::GetBlockLength(BlockSizeType idx) const ynothrow
{
	if(pBuffer)
		try
		{
			return pBuffer->at(idx).size();
		}
		catch(...)
		{}
	return 0;
}


TextFileBuffer::TextFileBuffer(TextFile& file)
	: TextMap(),
	File(file), nTextSize(std::max<u32>(File.GetTextSize(), 1)),
	nBlock((nTextSize + nBlockSize - 1) / nBlockSize)
{}

TextBlock&
TextFileBuffer::at(const BlockSizeType& idx)
{
	if(idx * nBlockSize > File.GetSize())
		throw std::out_of_range("YSLib::Text::TextBlock");

	auto& p((*this)[idx]);

	if(is_null(p))
	{
		p = unique_raw(new TextBlock(idx, nBlockSize));
		File.SetPosition(idx * nBlockSize + File.GetBOMSize(), SEEK_SET);
		LoadText(*p, File, nBlockSize);
	}
	return *p;
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

