﻿/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharRenderer.h
\ingroup Service
\brief 字符渲染。
\version r2830
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2014-09-03 13:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::CharRenderer
*/


#ifndef YSL_INC_Service_CharRenderer_h_
#define YSL_INC_Service_CharRenderer_h_ 1

#include "YModules.h"
#include YFM_YSLib_Service_TextBase
#include YFM_YSLib_Service_YBlit
#include <cwctype>

namespace YSLib
{

namespace Drawing
{

/*!
\brief 字符按扫描线块传输。
\tparam _tOut 输出迭代器类型。
\tparam _tIn 输入迭代器类型。
\tparam _fBlitScanner 扫描线操作类型。
\param scanner 扫描线操作。
\param src 源迭代器。
\param ss 源迭代器所在缓冲区大小。
\param pc 指定字符所在区域和渲染目标的绘制上下文，
	其中 Location 为相对源的坐标。
\param neg_pitch 指定交换行渲染顺序。
\sa Blit
\sa BlitLines
\since build 438
*/
template<typename _tOut, typename _tIn, typename _fBlitScanner>
void
BlitGlyphLines(_fBlitScanner scanner, _tOut dst, _tIn src, const Size& ss,
	const PaintContext& pc, bool neg_pitch)
{
	const auto& ds(pc.Target.GetSize());
	const Rect& bounds(pc.ClipArea);

	if(neg_pitch)
		BlitLines<false, true>(scanner, dst, src, ds, ss, bounds.GetPoint(),
			pc.Location, bounds.GetSize());
	else
		BlitLines<false, false>(scanner, dst, src, ds, ss, bounds.GetPoint(),
			pc.Location, bounds.GetSize());
}


/*!
\brief 字符按像素块传输。
\tparam _tOut 输出迭代器类型。
\tparam _tIn 输入迭代器类型。
\tparam _fPixelShader 像素着色器类型。
\param blit 像素操作。
\param src 源迭代器。
\param ss 源迭代器所在缓冲区大小。
\param pc 指定字符所在区域和渲染目标的绘制上下文，
	其中 Location 为相对源的坐标。
\param neg_pitch 指定交换行渲染顺序。
\sa Blit
\sa BlitPixels
\since build 440
*/
template<typename _tOut, typename _tIn, typename _fPixelShader>
void
BlitGlyphPixels(_fPixelShader blit, _tOut dst, _tIn src, const Size& ss,
	const PaintContext& pc, bool neg_pitch)
{
	const auto& ds(pc.Target.GetSize());
	const Rect& bounds(pc.ClipArea);

	if(neg_pitch)
		BlitPixels<false, true>(blit, dst, src, ds, ss, bounds.GetPoint(),
			pc.Location, bounds.GetSize());
	else
		BlitPixels<false, false>(blit, dst, src, ds, ss, bounds.GetPoint(),
			pc.Location, bounds.GetSize());
}


/*!
\brief 渲染单个字符。
\param pc 指定字符所在区域和渲染目标的绘制上下文，
	其中 Location 为相对源的坐标。
\pre 断言：缓冲区非空。
\note 忽略 Alpha 缓冲。
\since build 415
*/
YF_API void
RenderChar(PaintContext&& pc, Color, bool, CharBitmap::BufferType,
	CharBitmap::FormatType, const Size&);

/*!
\brief 渲染带 Alpha 缓冲的单个字符。
\param pc 指定字符所在区域和渲染目标的绘制上下文，
	其中 Location 为相对源的坐标。
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
	if(YB_LIKELY(Text::IsPrint(c)))
		r(c);
}

/*!
\brief 使用指定的文本状态和行末位置（横坐标）按需打印换行并判断是否需要渲染单个字符。
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

} // namespace Drawing;

} // namespace YSLib;

#endif

