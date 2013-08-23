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
\version r3214
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2013-08-23 09:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::CharRenderer
*/


#include "YSLib/Service/CharRenderer.h"

using namespace ystdex;

namespace YSLib
{

using namespace Drawing;
using namespace Text;

namespace Drawing
{

namespace
{

//! \since build 417
const AlphaType BLT_TEXT_ALPHA_THRESHOLD(16);
PixelType char_color;

/*!
\brief 混合 Alpha 透明度扫描线。
\warning 不检查迭代器有效性。
\since build 438
*/
struct BlitTextLine
{
	//! \bug 依赖于静态对象保存的状态，非线程安全。
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut& dst_iter, _tIn& src_iter, SDst delta_x)
	{
		for(SDst x(0); x < delta_x; ++x)
		{
			if(*src_iter >= BLT_TEXT_ALPHA_THRESHOLD)
				yunseq(*dst_iter.base().second = *src_iter,
					*dst_iter = char_color);
			yunseq(++src_iter, ++dst_iter);
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


using PixelIt = pseudo_iterator<const PixelType>;

using BIt_1 = bitseg_iterator<1, true>;
using BIt_2 = bitseg_iterator<2, true>;
using BIt_4 = bitseg_iterator<4, true>;

using TIt_1 = transformed_iterator<BIt_1, tr_seg<1>> ;
using TIt_2 = transformed_iterator<BIt_2, tr_seg<2>>;
using TIt_4 = transformed_iterator<BIt_4, tr_seg<4>>;

using MonoItPair_1 = pair_iterator<PixelIt, TIt_1>;
using MonoItPair_2 = pair_iterator<PixelIt, TIt_2>;
using MonoItPair_4 = pair_iterator<PixelIt, TIt_4>;

//! \since build 417
using PairIt = pair_iterator<BitmapPtr, AlphaType*>;


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
		BlitGlyphLines(BlitBlendLine<true>(), pc.Target.GetBufferPtr(),
			MonoItPair_1(PixelIt(c), tr_buf<1>(cbuf)), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray2:
		BlitGlyphLines(BlitBlendLine<true>(), pc.Target.GetBufferPtr(),
			MonoItPair_2(PixelIt(c), tr_buf<2>(cbuf)), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray4:
		BlitGlyphLines(BlitBlendLine<true>(), pc.Target.GetBufferPtr(),
			MonoItPair_4(PixelIt(c), tr_buf<4>(cbuf)), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray:
		BlitGlyphLines(BlitBlendLine<true>(), pc.Target.GetBufferPtr(),
			MonoIteratorPair(PixelIt(c), cbuf), ss, pc, neg_pitch);
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
		BlitGlyphLines(BlitTextLine(), PairIt(pc.Target.GetBufferPtr(), alpha),
			tr_buf<1>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray2:
		BlitGlyphLines(BlitTextLine(), PairIt(pc.Target.GetBufferPtr(), alpha),
			tr_buf<2>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray4:
		BlitGlyphLines(BlitTextLine(), PairIt(pc.Target.GetBufferPtr(), alpha),
			tr_buf<4>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray:
		BlitGlyphLines(BlitTextLine(), PairIt(pc.Target.GetBufferPtr(), alpha),
			cbuf, ss, pc, neg_pitch);
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

} // namespace Drawing;

} // namespace YSLib;

