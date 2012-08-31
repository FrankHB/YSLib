/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharRenderer.cpp
\ingroup Service
\brief 字符渲染。
\version r2464;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2009-11-13 00:06:05 +0800;
\par 修改时间:
	2012-08-30 19:14 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Service::CharRenderer;
*/


#include "YSLib/Service/CharRenderer.h"
#include "YSLib/Service/yblit.h"

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
					{
						*dst_iter.base().second = *src_iter;
						*dst_iter = char_color;
					}
					++src_iter;
					xcrease<_bPositiveScan>(dst_iter);
				}
				src_iter += src_inc;
				delta_assign<_bPositiveScan>(dst_iter, dst_inc);
			}
		}
	};
}


void
RenderChar(ucs4_t c, TextState& ts, const Graphics& g, const Rect& mask,
	u8* alpha)
{
	//无缓冲区时无法绘图。
	if(YB_LIKELY(g))
	{
		using namespace ystdex;

		CharBitmap sbit(ts.Font.GetGlyph(c));

		//无法获取位图时无法绘图。
		if(YB_UNLIKELY(!sbit))
			return;
		// TODO: Show a special glyph when no bitmap found.

		const int tx(ts.PenX + ts.Font.GetAdvance(c, sbit));

		if(YB_LIKELY(std::iswgraph(c) && sbit.GetBuffer()))
		{
			const auto& left(ts.Margin.Left);
			const auto& top(ts.Margin.Top);
			const auto& sbw(sbit.GetWidth());
			const auto& sbh(sbit.GetHeight());
			const auto dx(ts.PenX + sbit.GetLeft()),
				dy(ts.PenY - sbit.GetTop()),
				xmin(max<int>(0, left - dx)),
				ymin(max<int>(0, top - dy));
			const Size& ds(g.GetSize());
			const Size ss(sbw, sbh);
			const Point dp(max<int>(left, dx), max<int>(top, dy));
			Point sp(xmin, ymin);
			const Size sc(max<int>(min<int>(g.GetWidth() - ts.Margin.Right
				- dx, sbw) - xmin, 0), max<int>(min<int>(g.GetHeight()
				- ts.Margin.Bottom - dy, sbh) - ymin, 0));
			const Rect& r(mask & Rect(dp, sc));

			sp += r.GetPoint() - dp;
			if(alpha)
			{
				char_color = Color(ts.Color);
				Blit<BlitTextLoop, false, false>(pair_iterator<BitmapPtr,
					u8*>(g.GetBufferPtr(), alpha), ds, sbit.GetBuffer(), ss,
					r.GetPoint(), sp, r.GetSize());
			}
			else
				Blit<BlitBlendLoop, false, false>(g.GetBufferPtr(), ds,
					MonoIteratorPair(pseudo_iterator<const PixelType>(
					Color(ts.Color)), sbit.GetBuffer()), ss,
					r.GetPoint(), sp, r.GetSize());
		}
		//移动笔。
		ts.PenX = tx;
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

