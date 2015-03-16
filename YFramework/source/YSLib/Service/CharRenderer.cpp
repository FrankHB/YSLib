/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharRenderer.cpp
\ingroup Service
\brief 字符渲染。
\version r3308
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2015-03-17 06:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::CharRenderer
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_CharRenderer
#include YFM_YSLib_Service_YBlend // for Drawing::Shaders::BlitAlphaPoint;
#include <ystdex/bitseg.hpp> // for ystdex::bitseg_iterator;

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

/*!
\brief 混合 Alpha 透明度扫描线。
\warning 不检查迭代器有效性。
\since build 440
*/
struct BlitTextPoint
{
	//! \since build 584
	Pixel Color;

	//! \since build 584
	template<typename _tOut, typename _tIn>
	void
	operator()(_tOut dst_iter, _tIn src_iter) const
	{
		if(*src_iter >= BLT_TEXT_ALPHA_THRESHOLD)
			yunseq(*dst_iter.base().second = *src_iter,
				*dst_iter = Color);
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


using PixelIt = pseudo_iterator<const Pixel>;

//! \since build 584
template<size_t _vBit>
using MonoItPairN = pair_iterator<PixelIt,
	transformed_iterator<bitseg_iterator<_vBit, true>, tr_seg<_vBit>>>;

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

	const Shaders::BlitAlphaPoint bp;
	const auto dst(pc.Target.GetBufferPtr());

	switch(fmt)
	{
	case CharBitmap::Mono:
		BlitGlyphPixels(bp, dst, MonoItPairN<1>(PixelIt(c), tr_buf<1>(cbuf)),
			ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray2:
		BlitGlyphPixels(bp, dst, MonoItPairN<2>(PixelIt(c), tr_buf<2>(cbuf)),
			ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray4:
		BlitGlyphPixels(bp, dst, MonoItPairN<4>(PixelIt(c), tr_buf<4>(cbuf)),
			ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray:
		BlitGlyphPixels(bp, dst, pair_iterator<PixelIt, const AlphaType*>(
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

	const BlitTextPoint bp{c};
	const pair_iterator<BitmapPtr, AlphaType*>
		dst(pc.Target.GetBufferPtr(), alpha);

	switch(fmt)
	{
	case CharBitmap::Mono:
		BlitGlyphPixels(bp, dst, tr_buf<1>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray2:
		BlitGlyphPixels(bp, dst, tr_buf<2>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray4:
		BlitGlyphPixels(bp, dst, tr_buf<4>(cbuf), ss, pc, neg_pitch);
		break;
	case CharBitmap::Gray:
		BlitGlyphPixels(bp, dst, cbuf, ss, pc, neg_pitch);
	default:
		break;
	}
}


std::uint8_t
PutCharBase(TextState& ts, SDst eol, ucs4_t c)
{
	if(c == '\n')
	{
		ts.PutNewline();
		return 0;
	}
	if(YB_UNLIKELY(!IsPrint(c)))
		return 0;
#if 0
	const int max_w(GetBufferWidthN() - 1),
		space_w(ts.GetCache().GetAdvance(' '));

	if(max_w < space_w)
		return line_breaks_l = 1;
#endif
	const SPos w_adv(ts.Pen.X + ts.Font.GetAdvance(c));

	if(YB_UNLIKELY(w_adv > 0 && SDst(w_adv) > eol))
	{
		ts.PutNewline();
		return 1;
	}
	return 2;
}

} // namespace Drawing;

} // namespace YSLib;

