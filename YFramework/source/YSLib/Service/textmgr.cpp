/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textmgr.cpp
\ingroup Service
\brief 文本管理服务。
\version r3728
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-01-05 17:48:09 +0800
\par 修改时间:
	2013-04-24 15:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextManager
*/


#include "YSLib/Service/textmgr.h"
#include "CHRLib/MapEx.h"
#include <ystdex/any_iterator.hpp>
#include "CHRLib/Convert.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

namespace
{

//! \since build 400
yconstexpr auto& FetchMapperFunc(FetchMapperPtr<ConversionResult(ucs2_t&,
	ystdex::input_monomorphic_iterator&&, ConversionState&&)>);
//! \since build 400
yconstexpr auto& FetchSkipMapperFunc(FetchMapperPtr<ConversionResult(
	ystdex::input_monomorphic_iterator&&, ConversionState&&)>);

} // unnamed namespace;


TextFileBuffer::Iterator::Iterator(TextFileBuffer* pBuf, size_t b, size_t idx)
	ynothrow
	: pBuffer(pBuf), block(b), index(idx)
{}

TextFileBuffer::Iterator&
TextFileBuffer::Iterator::operator++() ynothrow
{
	YAssert(pBuffer, "Null pointer found.");
	YAssert(block < pBuffer->nBlock, "End iterator found.");

	auto& vec((*pBuffer)[block].first);

	YAssert(index < vec.size(), "Invalid index found.");

	if(YB_UNLIKELY(++index == vec.size()))
		yunseq(++block, index = 0);
	return *this;
}

TextFileBuffer::Iterator&
TextFileBuffer::Iterator::operator--() ynothrow
{
	YAssert(pBuffer, "Null buffer pointer found.");
	YAssert(block != 0 || index != 0, "Begin iterator found."),
	YAssert(block < pBuffer->nBlock || *this == pBuffer->GetEnd(),
		"Invalid iterator found.");

	if(index == 0)
	{
		index = (*pBuffer)[--block].first.size();

		YAssert(index != 0, "Invalid index found.");
	}
	else
		YAssert(index < (*pBuffer)[block].first.size(), "Invalid index found.");
	--index;
	return *this;
}

ucs2_t
TextFileBuffer::Iterator::operator*() const ynothrow
{
	YAssert(pBuffer, "Null pointer found.");

	auto& vec((*pBuffer)[block].first);

	YAssert(!vec.empty(), "Empty block found.");
	YAssert(index < vec.size(), "Invalid index found.");

	return vec[index];
}

bool
operator==(const TextFileBuffer::Iterator& x, const TextFileBuffer::Iterator& y)
	ynothrow
{
	YAssert(x.pBuffer == y.pBuffer, "Iterators to different buffer are not"
		" comparable.");

	return x.block == y.block && x.index == y.index;
}


TextFileBuffer::TextFileBuffer(TextFile& file)
	: File(file), nTextSize(File.GetTextSize()),
	nBlock((nTextSize + BlockSize - 1) / BlockSize), Map(),
	fixed_width(FetchFixedCharWidth(File.Encoding)), max_width(fixed_width
	== 0 ? FetchMaxVariantCharWidth(File.Encoding) : fixed_width)
{
	YAssert(max_width != 0, "Unknown encoding found.");

	if(fixed_width == 0)
		++fixed_width;
	// TODO: Implementation for non-fixed-width char streams.
}

TextFileBuffer::BlockType&
TextFileBuffer::operator[](size_t idx)
{
	YAssert(idx < nBlock, "Invalid index found.");

	auto& b(Map[idx]);
	auto& vec(b.first);

	if(YB_UNLIKELY(vec.empty() && bool(File)))
		if(const auto pfun = FetchMapperFunc(File.Encoding))
		{
			File.Locate(idx * BlockSize);

			size_t len(idx == nBlock - 1 && nTextSize % BlockSize != 0
				? nTextSize % BlockSize : BlockSize);

			vec.reserve(len / fixed_width);

			size_t n_byte(0);
			ucs2_t c;
			ystdex::ifile_iterator i(*File.GetPtr());

			while(n_byte < len)
			{
				ConversionState st;

				if(YB_LIKELY(ConvertCharacter(pfun, c, i, std::move(st))
					== ConversionResult::OK))
					vec.push_back(c);
				n_byte += GetCountOf(st);
			}
			std::ungetc(*i, File.GetPtr()),
			vec.shrink_to_fit();
		}
	return b;
}

TextFileBuffer::Iterator
TextFileBuffer::GetBegin() ynothrow
{
	return TextFileBuffer::Iterator(this);
}
TextFileBuffer::Iterator
TextFileBuffer::GetEnd() ynothrow
{
	return TextFileBuffer::Iterator(this, nBlock);
}
TextFileBuffer::Iterator
TextFileBuffer::GetIterator(size_t pos)
{
	if(pos < nTextSize)
	{
		const size_t idx(pos / BlockSize);

		pos %= BlockSize;
		if(fixed_width == max_width)
			return TextFileBuffer::Iterator(this, idx, pos / max_width);

		YAssert(bool(File), "Invalid file found.");

		if(const auto pfun = FetchSkipMapperFunc(File.Encoding))
		{
			File.Locate(idx * BlockSize);

			size_t n_byte(0), n_char(0);
			ystdex::ifile_iterator i(*File.GetPtr());

			while(n_byte < pos)
			{
				ConversionState st;

				if(YB_LIKELY(ConvertCharacter(pfun, i, std::move(st))
					== ConversionResult::OK))
					++n_char;
				n_byte += GetCountOf(st);
			}
			std::ungetc(*i, File.GetPtr());
			return TextFileBuffer::Iterator(this, idx, n_char);
		}
		return TextFileBuffer::Iterator(this, idx, 0);
	}
	return GetEnd();
}
size_t
TextFileBuffer::GetPosition(TextFileBuffer::Iterator i)
{
	if(i == GetEnd())
		return nTextSize;

	auto idx(i.GetBlockN());
	const auto pos(i.GetIndexN());

	if(fixed_width == max_width)
		return idx * BlockSize + pos * max_width;

	if(const auto pfun = FetchSkipMapperFunc(File.Encoding))
	{
		const auto& vec((*this)[idx].first);

		YAssert(!vec.empty() && bool(File), "Block loading failed.");

		File.Locate(idx *= BlockSize);

		const auto mid(vec.cbegin() + pos);

		YAssert(mid <= vec.cend(), "Wrong iterator found.");

		auto it(vec.begin());

		YAssert(it <= mid, "Wrong iterator found.");

		ystdex::ifile_iterator i(*File.GetPtr());
		size_t n_byte(0);

		while(it != mid)
		{
			ConversionState st;

			if(YB_LIKELY(ConvertCharacter(pfun, i, std::move(st))
				== ConversionResult::OK))
				++it;
			n_byte += GetCountOf(st);
		}
		return idx + n_byte;
	}
	return idx;
}


string
CopySliceFrom(TextFileBuffer& buf, size_t pos, size_t len)
	ythrow(std::out_of_range)
{
	const auto i_end(buf.GetEnd());
	auto i_beg(buf.GetIterator(pos));

	if(i_beg == i_end)
		throw std::out_of_range("Wrong offset found.");
	if(len == 0)
		return {};

	YAssert(pos < pos + len, "Unexpected unsigned integer round up.");

	string str;

	str.reserve(len * 2);
	while(len != 0 && i_beg != i_end)
	{
		str.push_back(*i_beg);
		yunseq(++i_beg, --len);
	}
	return std::move(str);
}

YSL_END_NAMESPACE(Text)

YSL_END

