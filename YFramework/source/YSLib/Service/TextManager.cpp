/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextManager.cpp
\ingroup Service
\brief 文本管理服务。
\version r3895
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-01-05 17:48:09 +0800
\par 修改时间:
	2015-07-11 20:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextManager
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_TextManager
#include YFM_CHRLib_MappingEx
#include <ystdex/any_iterator.hpp>
#include YFM_CHRLib_Convert

namespace YSLib
{

namespace Text
{

namespace
{

//! \since build 601
yconstexpr const auto& FetchMapperFunc(FetchMapperPtr<ConversionResult, ucs2_t&,
	ystdex::input_monomorphic_iterator&&, ConversionState&&>);
//! \since build 601
yconstexpr const auto& FetchSkipMapperFunc(FetchMapperPtr<ConversionResult,
	ystdex::pseudo_output&&, ystdex::input_monomorphic_iterator&&,
	ConversionState&&>);


//! \since build 552
template<typename _func, typename _vPFun, typename _tIn, typename... _tParams>
size_t
ConvertChar(_func f, _vPFun pfun, _tIn&& i, _tParams&&... args)
{
	ConversionState st;
	ystdex::pair_iterator<ystdex::decay_t<_tIn>, size_t> it(i);
	const auto
		res(ConvertCharacter(pfun, yforward(args)..., it, std::move(st)));

	switch(ConversionResult(YB_EXPECT(long(res), long(ConversionResult::OK))))
	{
	case ConversionResult::OK:
		f(yforward(args)...);
		break;
//	case ConversionResult::Invalid:
		// TODO: Insert placeholders for %ConversionResult::Invalid.
	default:
		YTraceDe(Warning, "Encoding conversion failed with state = %zu.",
			size_t(res));
	}
	i = get<0>(it.base());
	return get<1>(it.base());
}

} // unnamed namespace;


TextFileBuffer::iterator::iterator(TextFileBuffer* p_buf, size_t b, size_t idx)
	ynothrow
	: p_buffer(p_buf), position(b << BlockShift | idx)
{}

TextFileBuffer::iterator&
TextFileBuffer::iterator::operator++() ynothrow
{
	YAssert(GetBlockN() < Deref(p_buffer).nBlock, "End iterator found.");
	YAssert(GetIndexN() < (*p_buffer)[GetBlockN()].first.size(),
		"Invalid index found.");
	++position;
	return *this;
}

TextFileBuffer::iterator&
TextFileBuffer::iterator::operator--() ynothrow
{
	YAssert(position != 0, "Begin iterator found.");
	YAssert(GetBlockN() < Deref(p_buffer).nBlock || *this == p_buffer->end(),
		"Invalid iterator found.");
	YAssert((GetIndexN() != 0 || GetBlockN() != 0) && GetIndexN()
		< (*p_buffer)[GetBlockN() - (GetIndexN() == 0 ? 1 : 0)].first.size(),
		"Invalid index found.");
	--position;
	return *this;
}

TextFileBuffer::iterator::reference
TextFileBuffer::iterator::operator*() const
{
	auto& vec(Deref(p_buffer)[GetBlockN()].first);

	YAssert(!vec.empty(), "Empty block found.");

	const auto idx(GetIndexN());

	YAssert(idx < vec.size(), "Invalid index found.");
	return vec[idx];
}

bool
operator==(const TextFileBuffer::iterator& x, const TextFileBuffer::iterator& y)
	ynothrow
{
	YAssert(x.p_buffer == y.p_buffer,
		"Iterators of different buffers are not comparable.");

	return x.position == y.position;
}


TextFileBuffer::TextFileBuffer(TextFile& file)
	: File(file), nTextSize(File.GetTextSize()),
	nBlock((nTextSize + (1 << BlockShift) - 1) >> BlockShift),
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

	auto& b(mBuffer[idx]);
	auto& vec(b.first);

	if(YB_UNLIKELY(vec.empty() && bool(File)))
		if(const auto pfun = FetchMapperFunc(File.Encoding))
		{
			File.Locate(idx << BlockShift);

			size_t len(idx == nBlock - 1
				&& (nTextSize & ((1 << BlockShift) - 1)) != 0
				? nTextSize & ((1 << BlockShift) - 1) : 1 << BlockShift);

			vec.reserve(len / fixed_width);

			size_t n_byte(0);
			ucs2_t c;
			auto sentry(File.GetSentry());

			while(n_byte < len)
				n_byte += ConvertChar([&](ucs2_t uc){
					vec.push_back(uc);
				}, pfun, sentry.GetIteratorRef(), c);
			vec.shrink_to_fit();
		}
	return b;
}

TextFileBuffer::iterator
TextFileBuffer::begin() ynothrow
{
	return TextFileBuffer::iterator(this);
}
TextFileBuffer::iterator
TextFileBuffer::end() ynothrow
{
	return TextFileBuffer::iterator(this, nBlock);
}
TextFileBuffer::iterator
TextFileBuffer::GetIterator(size_t pos)
{
	if(pos < nTextSize)
	{
		const size_t idx(pos >> BlockShift);

		pos &= (1 << BlockShift) - 1;
		if(fixed_width == max_width)
			return TextFileBuffer::iterator(this, idx, pos / max_width);

		YAssert(bool(File), "Invalid file found.");

		if(const auto pfun = FetchSkipMapperFunc(File.Encoding))
		{
			File.Locate(idx << BlockShift);

			size_t n_byte(0), n_char(0);
			auto sentry(File.GetSentry());

			while(n_byte < pos)
				n_byte += ConvertChar([&](ystdex::pseudo_output){
					++n_char;
				}, pfun, sentry.GetIteratorRef(), ystdex::pseudo_output());
			return TextFileBuffer::iterator(this, idx, n_char);
		}
		return TextFileBuffer::iterator(this, idx, 0);
	}
	return end();
}
size_t
TextFileBuffer::GetPosition(TextFileBuffer::iterator i)
{
	if(i == end())
		return nTextSize;

	auto idx(i.GetBlockN());
	const auto pos(i.GetIndexN());

	if(fixed_width == max_width)
		return (idx << BlockShift) + pos * max_width;

	if(const auto pfun = FetchSkipMapperFunc(File.Encoding))
	{
		const auto& vec((*this)[idx].first);

		YAssert(!vec.empty() && bool(File), "Block loading failed.");

		File.Locate(idx <<= BlockShift);

		// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
		const auto mid(vec.cbegin() + ptrdiff_t(pos));

		YAssert(mid <= vec.cend(), "Wrong iterator found.");

		auto it(vec.cbegin());

		YAssert(it <= mid, "Wrong iterator found.");

		size_t n_byte(0);
		auto sentry(File.GetSentry());

		while(it != mid)
			n_byte += ConvertChar([&](ystdex::pseudo_output){
				++it;
			}, pfun, sentry.GetIteratorRef(), ystdex::pseudo_output());
		return idx + n_byte;
	}
	return idx;
}


string
CopySliceFrom(TextFileBuffer& buf, size_t pos, size_t len)
{
	const auto i_end(buf.end());
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
	return str;
}

} // namespace Text;

} // namespace YSLib;

