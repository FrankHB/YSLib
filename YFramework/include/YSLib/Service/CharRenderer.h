/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharRenderer.h
\ingroup Service
\brief 字符渲染。
\version r2743
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2013-06-24 22:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::CharRenderer
*/


#ifndef YSL_INC_Service_CharRenderer_h_
#define YSL_INC_Service_CharRenderer_h_ 1

#include "TextBase.h"
#include "YSLib/Service/yblit.h"
#include <cwctype>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 字符块传输。
\tparam _gBlitLoop 循环实现类模板。
\tparam _tOut 输出迭代器类型。
\tparam _tIn 输入迭代器类型。
\param src 源迭代器。
\param ss 源迭代器所在缓冲区大小。
\param pc 指定字符所在区域和渲染目标的绘制上下文，其中 Location 为相对于源的坐标。
\param neg_pitch 指定交换行渲染顺序。
\see Blit 。
\since build 415
*/
template<template<bool> class _gBlitLoop, typename _tOut, typename _tIn>
inline void
BlitChar(_tOut dst, _tIn src, const Size& ss, const PaintContext& pc,
	bool neg_pitch)
{
	const auto& g(pc.Target);
	const auto& r(pc.ClipArea);

	(neg_pitch ? Blit<_gBlitLoop, false, true, _tOut, _tIn> : Blit<_gBlitLoop,
		false, false, _tOut, _tIn>)(dst, g.GetSize(), src, ss, r.GetPoint(),
		pc.Location, r.GetSize());
}


/*!
\brief 渲染单个字符。
\param pc 指定字符所在区域和渲染目标的绘制上下文，其中 Location 为相对于源的坐标。
\pre 断言：缓冲区非空。
\note 忽略 Alpha 缓冲。
\since build 415
*/
YF_API void
RenderChar(PaintContext&& pc, Color, bool, CharBitmap::BufferType,
	CharBitmap::FormatType, const Size&);

/*!
\brief 渲染带 Alpha 缓冲的单个字符。
\param pc 指定字符所在区域和渲染目标的绘制上下文，其中 Location 为相对于源的坐标。
\pre 断言：缓冲区非空。
\since build 417
*/
YF_API void
RenderCharAlpha(PaintContext&& pc, Color, bool, CharBitmap::BufferType,
	CharBitmap::FormatType, const Size&, AlphaType*);


/*!
\brief 取文本渲染器的行末位置（横坐标）。
\since build 372
*/
template<class _tRenderer>
inline SDst
GetEndOfLinePositionOf(const _tRenderer& r)
{
	return r.GetTextState().Margin.Right;
}

/*!
\brief 打印单个可打印字符。
\since build 270
\todo 行的结尾位置计算和边距解除耦合。
*/
template<class _tRenderer>
void
PrintChar(_tRenderer& r, ucs4_t c)
{
	if(YB_LIKELY(std::iswprint(c)))
		r(c);
}

/*!
\brief 使用指定的文本状态和行末位置（横坐标）打印并判断是否需要具体渲染单个字符。
\return 遇到行内无法容纳而换行时为 1 ，需要继续渲染为 2 ，否则为 0 。
\since build 372
*/
YF_API u8
PutCharBase(TextState&, SDst, ucs4_t);

/*!
\brief 打印单个字符。
\return 遇到行内无法容纳而换行时返回非零值，否则返回 0 。
\note 处理换行符。
\note 当行内无法容纳完整字符时换行。
\since build 190
*/
template<class _tRenderer>
u8
PutChar(_tRenderer& r, ucs4_t c)
{
	const u8 res(PutCharBase(r.GetTextState(),
		r.GetContext().GetWidth() - GetEndOfLinePositionOf(r), c));

	switch(res)
	{
	case 2:
		r(c);
		return 0;
	default:
		return res;
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

