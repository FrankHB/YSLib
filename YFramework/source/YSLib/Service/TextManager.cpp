/*
	© 2010-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextManager.cpp
\ingroup Service
\brief 文本管理服务。
\version r4032
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-01-05 17:48:09 +0800
\par 修改时间:
	2016-02-07 19:45 +0800
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

//! \since build 615
//@{
using MonoIter = ystdex::input_monomorphic_iterator;
//! \since build 641
yconstexpr const auto& FetchMapperFunc(FetchMapperPtr<ConversionResult, char16_t&,
	GuardPair<MonoIter>&&, ConversionState&&>);
yconstexpr const auto& FetchSkipMapperFunc(FetchMapperPtr<ConversionResult,
	ystdex::pseudo_output&&, GuardPair<MonoIter>&&, ConversionState&&>);


template<typename _func, typename _vPFun, typename _tIn, typename... _tParams>
size_t
ConvertChar(_func f, _vPFun pfun, _tIn&& i, _tParams&&... args)
{
	using InIter = ystdex::decay_t<_tIn>;
	using RetainedIter = ystdex::pair_iterator<InIter, size_t>;
	ConversionState st;
	RetainedIter it(i);
	GuardPair<RetainedIter> gpr(it, RetainedIter(InIter(), 0));
	const auto
		res(ConvertCharacter(pfun, yforward(args)..., gpr, std::move(st)));

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
//@}

//! \since build 621
class Sentry
{
private:
	//! \since build 670
	tidy_ptr<std::istream> fp{};

public:
	//! \since build 622
	std::istream_iterator<char> Iterator{};

	DefDeCtor(Sentry)
	Sentry(std::istream& is)
		: fp(make_observer(&is)), Iterator(is)
	{}
	DefDeMoveCtor(Sentry)
	~Sentry()
	{
		if(fp)
			TryExpr(fp->clear(), fp->unget(), fp->clear())
			CatchIgnore(std::ios_base::failure&)
			CatchExpr(..., yassume(false))
	}

	DefDeMoveAssignment(Sentry)
};

} // unnamed namespace;


TextFileBuffer::iterator::iterator(TextFileBuffer* p_buf, size_t b, size_t idx)
	ynothrow
	: p_buffer(p_buf), block(b), index(idx)
{}

TextFileBuffer::iterator&
TextFileBuffer::iterator::operator++() ynothrow
{
	YAssert(GetBlockN() < Deref(p_buffer).nBlock, "End iterator found.");

	auto& vec((*p_buffer)[GetBlockN()].first);

	YAssert(GetIndexN() < vec.size(), "Invalid index found.");
	if(YB_UNLIKELY(++index == vec.size()))
		yunseq(++block, index = 0);
	return *this;
}

TextFileBuffer::iterator&
TextFileBuffer::iterator::operator--() ynothrow
{
	YAssert(block != 0 || index != 0, "Begin iterator found.");
	YAssert(block < Deref(p_buffer).nBlock || *this == p_buffer->end(),
		"Invalid iterator found.");
	if(index == 0)
	{
		index = (*p_buffer)[--block].first.size();

		YAssert(index != 0, "Invalid index found.");
	}
	else
		YAssert(index < (*p_buffer)[block].first.size(),
			"Invalid index found.");
	--index;
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

	return x.block == y.block && x.index == y.index;
}


TextFileBuffer::TextFileBuffer(std::istream& file, Encoding enc)
	: File(file), fsize([&, this]() -> size_t{
		File.seekg(0, std::ios_base::end);
		if(!File)
			throw LoggedEvent("Failed getting size of file.");
		return size_t(File.tellg());
	}()), encoding(enc), bl([this]() -> size_t{
		if(encoding == CharSet::Null)
		{
			size_t blen;

			tie(encoding, blen) = DetectBOM(File, fsize);
			// TODO: More accurate encoding checking for text stream without
			//	BOM.
			if(encoding == CharSet::Null)
				encoding = CharSet::GBK;
			return blen;
		}
		return 0;
	}()), nTextSize(fsize - bl), nBlock((nTextSize + BlockSize - 1)
	/ BlockSize), fixed_width(FetchFixedCharWidth(encoding)), max_width(
	fixed_width == 0 ? FetchMaxVariantCharWidth(encoding) : fixed_width)
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

	if(YB_UNLIKELY(vec.empty()))
	{
		YAssert(bool(File), "Invalid file found.");
		if(const auto pfun = FetchMapperFunc(encoding))
		{
			// XXX: Conversion to 'fstream::off_type' might be
			//	implementation-defined.
			File.seekg(fstream::off_type(bl + idx * BlockSize));

			size_t len(idx == nBlock - 1 && nTextSize % BlockSize != 0
				? nTextSize % BlockSize : BlockSize);

			vec.reserve(len / fixed_width);

			size_t n_byte(0);
			char16_t c;
			Sentry sentry(File);

			while(n_byte < len)
				n_byte += ConvertChar([&](char16_t uc){
					vec.push_back(uc);
				}, pfun, sentry.Iterator, c);
			vec.shrink_to_fit();
		}
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
		const size_t idx(pos / BlockSize);

		pos %= BlockSize;
		if(fixed_width == max_width)
			return TextFileBuffer::iterator(this, idx, pos / max_width);

		YAssert(bool(File), "Invalid file found.");

		if(const auto pfun = FetchSkipMapperFunc(encoding))
		{
			// XXX: Conversion to 'fstream::off_type' might be
			//	implementation-defined.
			File.seekg(fstream::off_type(bl + idx * BlockSize));

			size_t n_byte(0), n_char(0);
			Sentry sentry(File);

			while(n_byte < pos)
				n_byte += ConvertChar([&](ystdex::pseudo_output){
					++n_char;
				}, pfun, sentry.Iterator, ystdex::pseudo_output());
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
		return idx * BlockSize + pos * max_width;

	if(const auto pfun = FetchSkipMapperFunc(encoding))
	{
		const auto& vec((*this)[idx].first);

		YAssert(!vec.empty() && bool(File), "Block loading failed.");
		// XXX: Conversion to 'fstream::off_type' might be
		//	implementation-defined.
		File.seekg(fstream::off_type(bl + (idx *= BlockSize)));

		// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
		const auto mid(vec.cbegin() + ptrdiff_t(pos));

		YAssert(mid <= vec.cend(), "Wrong iterator found.");

		auto it(vec.cbegin());

		YAssert(it <= mid, "Wrong iterator found.");

		size_t n_byte(0);
		Sentry sentry(File);

		while(it != mid)
			n_byte += ConvertChar([&](ystdex::pseudo_output){
				++it;
			}, pfun, sentry.Iterator, ystdex::pseudo_output());
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

