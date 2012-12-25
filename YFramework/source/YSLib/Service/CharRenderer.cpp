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
\version r2558
\author FrankHB<frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2012-12-25 19:46 +0800
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

		if(YB_LIKELY(std::iswgraph(c) && sbit.GetBuffer()))
		{
			const Size& ds(g.GetSize());

			if(GetHorizontalOf(ts.Margin) < ds.Width
				&& GetVerticalOf(ts.Margin) < ds.Height)
			{
				const Size ss(sbit.GetWidth(), sbit.GetHeight());
				Rect r(ts.PenX + sbit.GetLeft(), ts.PenY - sbit.GetTop(),
					ss.Width, ss.Height);
				const Point sp(max<SPos>(ts.Margin.Left - r.X, 0),
					max<SPos>(ts.Margin.Top - r.Y, 0));
				const SPos dr(r.X + ss.Width - ds.Width + ts.Margin.Right),
					db(r.Y + ss.Height - ds.Height + ts.Margin.Bottom);

				if(dr > 0)
					r.Width -= dr;
				if(db > 0)
					r.Height -= db;
				r.GetPointRef() += sp;

				const PaintContext pc{g, sp, r & mask};

				if(alpha)
				{
					char_color = Color(ts.Color);
					BlitChar<BlitTextLoop>(pair_iterator<BitmapPtr, u8*>(
						g.GetBufferPtr(), alpha), sbit.GetBuffer(), ss, pc);
				}
				else
					BlitChar<BlitBlendLoop>(g.GetBufferPtr(), MonoIteratorPair(
						pseudo_iterator<const PixelType>(Color(ts.Color)),
						sbit.GetBuffer()), ss, pc);
				}
		}
		//移动笔。
		ts.PenX += sbit.GetXAdvance();
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

