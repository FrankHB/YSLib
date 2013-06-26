/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharRenderer.cpp
\ingroup Service
\brief 字符渲染。
\version r3103
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2013-06-24 22:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::CharRenderer
*/


#include "YSLib/Service/CharRenderer.h"

using namespace ystdex;

YSL_BEGIN

using namespace Drawing;
using namespace Text;

YSL_BEGIN_NAMESPACE(Drawing)

namespace
{

//! \since build 417
const AlphaType BLT_TEXT_ALPHA_THRESHOLD(16);
PixelType char_color;

template<bool _bPositiveScan>
struct BlitTextLoop
{
	/*!
	\bug 依赖于静态对象保存的状态，非线程安全。
	\since build 415
	*/
	template<typename _tOut, typename _tIn>
	void
	operator()(int delta_x, int delta_y, _tOut dst_iter, _tIn src_iter,
		int dst_inc, int src_inc)
	{
		for(; delta_y > 0; --delta_y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				if(*src_iter >= BLT_TEXT_ALPHA_THRESHOLD)
					yunseq(*dst_iter.base().second = *src_iter,
						*dst_iter = char_color);
				yunseq(++src_iter, xcrease<_bPositiveScan>(dst_iter));
			}
			src_iter += src_inc;
			delta_assign<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
};

//! \since build 415
//@{
template<unsigned char _vN>
struct tr_seg
{
	static_assert(_vN < CHAR_BIT, "Specified bits should be within a byte.");

	byte v;

	const byte&
	operator()(const bitseg_iterator<_vN, true>& i) ynothrow
	{
		return v = byte(*i << (CHAR_BIT - _vN) | ((1U << _vN) - 1));
	}
};


typedef pseudo_iterator<const PixelType> PixelIt;

typedef bitseg_iterator<1, true> BIt_1;
typedef bitseg_iterator<2, true> BIt_2;
typedef bitseg_iterator<4, true> BIt_4;

typedef transformed_iterator<BIt_1, tr_seg<1>> TIt_1;
typedef transformed_iterator<BIt_2, tr_seg<2>> TIt_2;
typedef transformed_iterator<BIt_4, tr_seg<4>> TIt_4;

typedef pair_iterator<PixelIt, TIt_1> MonoItPair_1;
typedef pair_iterator<PixelIt, TIt_2> MonoItPair_2;
typedef pair_iterator<PixelIt, TIt_4> MonoItPair_4;

//! \since build 417
typedef pair_iterator<BitmapPtr, AlphaType*> PairIt;


template<unsigned char _vN>
auto
tr_buf(byte* p)
	-> decltype(make_transform(bitseg_iterator<_vN, true>(p), tr_seg<_vN>()))
{
	return make_transform(bitseg_iterator<_vN, true>(p), tr_seg<_vN>());
}
//@}

} // unnamed namespace;

void
RenderChar(PaintContext&& pc, Color c, bool neg_pitch,
	CharBitmap::BufferType cbuf, CharBitmap::FormatType fmt, const Size& ss)
{
	YAssert(cbuf, "Invalid buffer found.");

	switch(fmt)
	{
	case CharBitmap::Mono:
		BlitChar<BlitBlendLoop>(pc.Target.GetBufferPtr(), MonoItPair_1(
			PixelIt(c), tr_buf<1>(cbuf)), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray2:
		BlitChar<BlitBlendLoop>(pc.Target.GetBufferPtr(), MonoItPair_2(
			PixelIt(c), tr_buf<2>(cbuf)), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray4:
		BlitChar<BlitBlendLoop>(pc.Target.GetBufferPtr(), MonoItPair_4(
			PixelIt(c), tr_buf<4>(cbuf)), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray:
		BlitChar<BlitBlendLoop>(pc.Target.GetBufferPtr(), MonoIteratorPair(
			PixelIt(c), cbuf), ss, pc, neg_pitch);
	default:
		break;
	}
}

void
RenderCharAlpha(PaintContext&& pc, Color c, bool neg_pitch,
	CharBitmap::BufferType cbuf, CharBitmap::FormatType fmt, const Size& ss,
	AlphaType* alpha)
{
	YAssert(cbuf, "Invalid buffer found.");

	char_color = c;
	switch(fmt)
	{
	case CharBitmap::Mono:
		BlitChar<BlitTextLoop>(PairIt(pc.Target.GetBufferPtr(), alpha),
			tr_buf<1>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray2:
		BlitChar<BlitTextLoop>(PairIt(pc.Target.GetBufferPtr(), alpha),
			tr_buf<2>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray4:
		BlitChar<BlitTextLoop>(PairIt(pc.Target.GetBufferPtr(), alpha),
			tr_buf<4>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray:
		BlitChar<BlitTextLoop>(PairIt(pc.Target.GetBufferPtr(), alpha), cbuf,
			ss, pc, neg_pitch);
	default:
		break;
	}
}


u8
PutCharBase(TextState& ts, SDst eol, ucs4_t c)
{
	if(c == '\n')
	{
		ts.PutNewline();
		return 0;
	}
	if(YB_UNLIKELY(!std::iswprint(c)))
		return 0;
#if 0
	const int max_w(GetBufferWidthN() - 1),
		space_w(ts.GetCache().GetAdvance(' '));

	if(max_w < space_w)
		return line_breaks_l = 1;
#endif
	if(YB_UNLIKELY(ts.Pen.X + ts.Font.GetAdvance(c) > eol))
	{
		ts.PutNewline();
		return 1;
	}
	return 2;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

