/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharRenderer.h
\ingroup Service
\brief 字符渲染。
\version r2636
\author FrankHB<frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2012-12-28 01:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::CharRenderer
*/


#ifndef YSL_INC_SERVICE_CHARRENDERER_H_
#define YSL_INC_SERVICE_CHARRENDERER_H_ 1

#include "TextBase.h"
#include "YSLib/Service/yblit.h"
#include <cwctype>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 字符渲染处理器类型。
\since build 367
*/
typedef void(HCharRenderer)(PaintContext&&, const Padding&, Color,
	const CharBitmap&, u8*);


/*!
\brief 字符块传输。
\tparam _gBlitLoop 循环实现类模板。
\tparam _tOut 输出迭代器类型。
\tparam _tIn 输入迭代器类型。
\param src 源迭代器。
\param ss 源迭代器所在缓冲区大小。
\param pc 指定字符所在区域和渲染目标的绘制上下文。
\see Blit 。
\since build 366
*/
template<template<bool> class _gBlitLoop, typename _tOut, typename _tIn>
inline void
BlitChar(_tOut dst, _tIn src, const Size& ss, const PaintContext& pc)
{
	const auto& g(pc.Target);
	const auto& r(pc.ClipArea);

	Blit<_gBlitLoop, false, false>(dst, g.GetSize(), src, ss,
		r.GetPoint(), pc.Location, r.GetSize());
}


/*!
\brief 渲染单个字符。
\note 忽略 Alpha 缓冲。
\since build 367
*/
YF_API void
RenderChar(PaintContext&&, const Padding&, Color, const CharBitmap&,
	u8* = nullptr);

/*!
\brief 渲染带 Alpha 缓冲的单个字符。
\since build 367
*/
YF_API void
RenderCharAlpha(PaintContext&&, const Padding&, Color, const CharBitmap&, u8*);


/*!
\brief 打印单个可打印字符。
\since build 270
*/
template<class _tRenderer>
void
PrintChar(_tRenderer& r, ucs4_t c)
{
	if(YB_LIKELY(std::iswprint(c)))
		r(c);
}

/*!
\brief 打印单个字符。
\note 处理换行符。
\note 当行内无法容纳完整字符时换行。
\since build 190
*/
template<class _tRenderer>
u8
PutChar(_tRenderer& r, ucs4_t c)
{
	TextState& ts(r.GetTextState());

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
	if(YB_UNLIKELY(ts.PenX + ts.Font.GetAdvance(c)
		> r.GetContext().GetWidth() - ts.Margin.Right))
	{
		ts.PutNewline();
		return 1;
	}
	r(c);
	return 0;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

