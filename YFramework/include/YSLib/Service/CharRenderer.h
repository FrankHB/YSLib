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
\version r2592
\author FrankHB<frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2012-12-11 21:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::CharRenderer
*/


#ifndef YSL_INC_SERVICE_CHARRENDERER_H_
#define YSL_INC_SERVICE_CHARRENDERER_H_ 1

#include "TextBase.h"
#include <cwctype>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 打印单个字符。
\param c 被打印的 UCS4 字符。
\param ts 文本状态。
\param g 输出图形接口上下文。
\param mask 相对于输出图形接口上下文矩形，限定输出边界。
\param alpha 输出设备接收的 8 位 Alpha 缓冲区首个元素的指针，若为 nullptr 则忽略。
\since build 254
*/
YF_API void
RenderChar(ucs4_t c, TextState& ts, const Graphics& g, const Rect& mask,
	u8* alpha);


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

	if(max_w < spaceW)
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

