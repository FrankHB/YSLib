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
\version r4104
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-01-05 17:48:09 +0800
\par 修改时间:
	2016-11-26 11:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextManager
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_TextManager
#include YFM_CHRLib_MappingEx
#include <ystdex/any_iterator.hpp> // for ystdex::input_monomorphic_iterator,
//	ystdex::make_transform, std::istreambuf_iterator;
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
yconstexpr const auto& FetchMapperFunc(FetchMapperPtr<ConversionResult,
	char16_t&, GuardPair<MonoIter>&&, ConversionState&&>);
yconstexpr const auto& FetchSkipMapperFunc(FetchMapperPtr<ConversionResult,
	ystdex::pseudo_output&&, GuardPair<MonoIter>&&, ConversionState&&>);


template<typename _func, typename _vPFun, typename _tIn, typename... _tParams>
size_t
ConvertChar(_func f, _vPFun pfun, _tIn&& i, _tParams&&... args)
{
	using InIter = ystdex::decay_t<_tIn>;
	const auto trans([](InIter iter){
		return ystdex::make_transform(iter, [](InIter x){
			return byte(*x);
		});
	});
	using RetainedIter
		= ystdex::pair_iterator<ystdex::decay_t<decltype(trans(i))>, size_t>;
	ConversionState st;
	RetainedIter it(trans(i));
	GuardPair<RetainedIter> gpr(it, RetainedIter(trans(InIter()), 0));
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
	i = get<0>(it.base()).get();
	return get<1>(it.base());
}
//@}

//! \since build 621
class Sentry
{
private:
	//! \since build 744
	tidy_ptr<std::streambuf> fp{};

public:
	//! \since build 744
	std::istreambuf_iterator<char> Iterator{};

	DefDeCtor(Sentry)
	//! \since build 744
	Sentry(std::streambuf& sb)
		: fp(make_observer(&sb)), Iterator(&sb)
	{}
	DefDeMoveCtor(Sentry)
	~Sentry()
	{
		if(fp)
			TryExpr(fp->sungetc())
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
	YAssert(GetBlockN() < Deref(p_buffer).n_block, "End iterator found.");

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
	YAssert(block < Deref(p_buffer).n_block || *this == p_buffer->end(),
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


TextFileBuffer::TextFileBuffer(std::streambuf& file, Encoding enc)
	: File(file), fsize([&, this]() -> size_t{
		const auto
			pos(File.pubseekoff(0, std::ios_base::end, std::ios_base::in));

		if(pos != std::streampos(std::streamoff(-1)))
		{
			const auto
				xpos(File.pubseekoff(0, std::ios_base::cur, std::ios_base::in));

			if(xpos != std::streampos(std::streamoff(-1)))
				return size_t(xpos);
		}
		throw LoggedEvent("Failed getting size of file.");
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
	}()), n_text_size(fsize - bl), n_block((n_text_size + BlockSize - 1)
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
	YAssert(idx < n_block, "Invalid index found.");

	auto& b(buffer[idx]);
	auto& vec(b.first);

	if(YB_UNLIKELY(vec.empty()))
	{
		if(const auto pfun = FetchMapperFunc(encoding))
		{
			// XXX: Conversion to 'std::streamoff' might be
			//	implementation-defined.
			Seek(std::streamoff(idx * BlockSize));

			size_t len(idx == n_block - 1 && n_text_size % BlockSize != 0
				? n_text_size % BlockSize : BlockSize);

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
TextFileBuffer::GetIterator(size_t pos)
{
	if(pos < n_text_size)
	{
		const size_t idx(pos / BlockSize);

		pos %= BlockSize;
		if(fixed_width == max_width)
			return TextFileBuffer::iterator(this, idx, pos / max_width);

		if(const auto pfun = FetchSkipMapperFunc(encoding))
		{
			// XXX: Conversion to 'std::streamoff' might be
			//	implementation-defined.
			Seek(std::streamoff(idx * BlockSize));

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
		return n_text_size;

	auto idx(i.GetBlockN());
	const auto pos(i.GetIndexN());

	if(fixed_width == max_width)
		return idx * BlockSize + pos * max_width;

	if(const auto pfun = FetchSkipMapperFunc(encoding))
	{
		const auto& vec((*this)[idx].first);

		YAssert(!vec.empty(), "Block loading failed.");
		// XXX: Conversion to 'std::streamoff' might be
		//	implementation-defined.
		Seek(std::streamoff(idx *= BlockSize));

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

void
TextFileBuffer::Seek(std::streamoff off)
{
	if(YB_UNLIKELY(File.pubseekpos(bl + off, std::ios_base::in)
		== std::streampos(std::streamoff(-1))))
		throw LoggedEvent("Failed setting reading position.");
}

TextFileBuffer::iterator
TextFileBuffer::begin() ynothrow
{
	return TextFileBuffer::iterator(this);
}

TextFileBuffer::iterator
TextFileBuffer::end() ynothrow
{
	return TextFileBuffer::iterator(this, n_block);
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

