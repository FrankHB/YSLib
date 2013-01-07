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
\version r2952
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2013-01-07 13:50 +0800
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

const u8 BLT_TEXT_ALPHA_THRESHOLD(16);
PixelType char_color;

template<bool _bPositiveScan>
struct BlitTextLoop
{
	//! \bug 依赖于静态对象保存的状态，非线程安全。
	void
	operator()(int delta_x, int delta_y,
		pair_iterator<BitmapPtr, u8*> dst_iter, u8* src_iter,
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

} // unnamed namespace;

void
RenderChar(PaintContext&& pc, Color c, CharBitmap::BufferType cbuf,
	const Size& ss)
{
	YAssert(cbuf, "Invalid buffer found.");

	BlitChar<BlitBlendLoop>(pc.Target.GetBufferPtr(), MonoIteratorPair(
		pseudo_iterator<const PixelType>(c), cbuf), ss, pc);
}

void
RenderCharAlpha(PaintContext&& pc, Color c, CharBitmap::BufferType cbuf,
	const Size& ss, u8* alpha)
{
	YAssert(cbuf, "Invalid buffer found.");

	char_color = c;
	BlitChar<BlitTextLoop>(pair_iterator<BitmapPtr, u8*>(
		pc.Target.GetBufferPtr(), alpha), cbuf, ss, pc);
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

