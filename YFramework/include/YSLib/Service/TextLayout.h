/*
	© 2009-2015, 2019-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextLayout.h
\ingroup Service
\brief 文本布局计算。
\version r2899
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2021-05-06 19:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextLayout
*/


#ifndef YSL_INC_Service_TextLayout_h_
#define YSL_INC_Service_TextLayout_h_ 1

#include "YModules.h"
#include YFM_YSLib_Service_TextRenderer
#include <numeric> // for std::accumulate;

namespace YSLib
{

namespace Drawing
{

/*!
\brief 取指定文本状态和文本区域高调整的底边距。
\pre 断言： <tt>GetTextLineHeightExOf(ts) != 0</tt> 。
\post <tt>ts.Margin.Bottom</tt> 不小于原值。
\return 返回调整后的底边距值（由字体大小、行距和高决定）。
\since build 252
*/
YB_ATTR_nodiscard YF_API YB_PURE SDst
FetchResizedBottomMargin(const TextState&, SDst);

/*!
\brief 取指定文本状态和文本区域高所能显示的最大文本行数。
\pre 断言： <tt>GetTextLineHeightExOf(ts) != 0</tt> 。
\return 最大能容纳的文本行数。
\since build 252
*/
YB_ATTR_nodiscard YF_API YB_PURE std::uint16_t
FetchResizedLineN(const TextState& ts, SDst);

/*!
\brief 取指定文本状态在指定高的区域中表示的最底行的基线位置（纵坐标）。
\note 若不足一行则最底行视为首行。
\warning 不检查边距正确性。若顶边距正确，则返回值应小于输入的高。
\since build 190
*/
YB_ATTR_nodiscard YF_API YB_PURE SPos
FetchLastLineBasePosition(const TextState&, SDst);


/*!
\brief 取按字体高度和行距调整文本区域的底边距。
\since build 252
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(SDst, FetchResizedBottomMargin, const TextRegion& tr)
	ImplRet(FetchResizedBottomMargin(tr, tr.GetHeight()))

/*!
\brief 按字体高度和行距调整文本区域的底边距。
\since build 587
*/
inline PDefH(void, AdjustBottomMarginOf, TextRegion& tr)
	// XXX: Conversion to 'SPos' might be implementation-defined.
	ImplExpr(tr.Margin.Bottom = SPos(FetchResizedBottomMargin(tr)))


/*!
\brief 取指定的字符使用指定字体的显示宽度。
\note 无边界限制。
\since build 641
*/
YB_ATTR_nodiscard YF_API YB_PURE SDst
FetchCharWidth(const Font&, char32_t);

/*!
\note 能被显示的（含不完整显示的）字符数和这些字符占用的。
\since build 484
*/
//@{
/*!
\brief 取迭代器指定的单行字符串在指定字体和宽度时的最多能显示的字符数和宽。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
*/
template<typename _tIter,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
YB_ATTR_nodiscard YB_PURE pair<size_t, SDst>
FetchStringOffsets(size_t max_width, const Font& fnt, _tIter s)
{
	size_t r(0);
	SDst w(0);

	for(; *s != char() && w < max_width; yunseq(++s, ++r))
	{
		using ystdex::is_undereferenceable;

		YAssert(!is_undereferenceable(s), "Invalid iterator found.");
		w += FetchCharWidth(fnt, *s);
	}
	return {r, w};
}
//! \since build 641
//@{
/*!
\brief 取迭代器指定的单行字符串在指定字体和宽度时的最多能显示的字符数和宽。
\note 迭代器 s 指向字符串首字符，迭代直至 n 次或指定字符 c 。
*/
template<typename _tIter,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
YB_ATTR_nodiscard YB_PURE pair<size_t, SDst>
FetchStringOffsets(size_t max_width, const Font& fnt, _tIter s, size_t n,
	char32_t c = {})
{
	size_t r(0);
	SDst w(0);

	for(; n-- != 0 && *s != c && w < max_width; yunseq(++s, ++r))
	{
		using ystdex::is_undereferenceable;

		YAssert(!is_undereferenceable(s), "Invalid iterator found.");
		w += FetchCharWidth(fnt, *s);
	}
	return {r, w};
}
/*!
\brief 取迭代器指定的单行字符串在指定字体和宽度时的最多能显示的字符数和宽。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
*/
template<typename _tIter,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
YB_ATTR_nodiscard YB_PURE pair<size_t, SDst>
FetchStringOffsets(size_t max_width, const Font& fnt, _tIter s, _tIter g,
	char32_t c = {})
{
	size_t r(0);
	SDst w(0);

	for(; s != g && *s != c && w < max_width; yunseq(++s, ++r))
	{
		using ystdex::is_undereferenceable;

		YAssert(!is_undereferenceable(s), "Invalid iterator found.");
		w += FetchCharWidth(fnt, *s);
	}
	return {r, w};
}
//@}
//! \brief 取单行字符串在指定字体和宽度时的最多能显示的字符数和宽。
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
YB_ATTR_nodiscard YB_PURE inline pair<size_t, SDst>
FetchStringOffsets(size_t max_width, const Font& fnt, const _tString& str)
{
	return Drawing::FetchStringOffsets(max_width, fnt, str.c_str());
}
/*!
\brief 取单行字符串前不超过 n 个字符在指定字体和宽度时的
	最多能显示的字符数和宽。
*/
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
YB_ATTR_nodiscard YB_PURE inline pair<size_t, SDst>
FetchStringOffsets(size_t max_width, const Font& fnt, const _tString& str,
	size_t n)
{
	return Drawing::FetchStringOffsets(max_width, fnt, str.data(), n);
}
//@}

/*!
\brief 取迭代器指定的单行字符串在字体指定、无边界限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\since build 483
*/
template<typename _tIter,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
YB_ATTR_nodiscard YB_PURE SDst
FetchStringWidth(const Font& fnt, _tIter s)
{
	SDst w(0);

	for(; *s != char(); ++s)
	{
		using ystdex::is_undereferenceable;

		YAssert(!is_undereferenceable(s), "Invalid iterator found.");
		w += FetchCharWidth(fnt, *s);
	}
	return w;
}
//! \since build 641
//@{
/*!
\brief 取迭代器指定的单行字符串在字体指定、无边界限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至 n 次或指定字符 c 。
*/
template<typename _tIter,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
YB_ATTR_nodiscard YB_PURE SDst
FetchStringWidth(const Font& fnt, _tIter s, size_t n, char32_t c = {})
{
	SDst w(0);

	for(; n-- != 0 && *s != c; ++s)
	{
		using ystdex::is_undereferenceable;

		YAssert(!is_undereferenceable(s), "Invalid iterator found.");
		w += FetchCharWidth(fnt, *s);
	}
	return w;
}
/*!
\brief 取迭代器指定的单行字符串在字体指定、无边界限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
*/
template<typename _tIter,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
YB_ATTR_nodiscard YB_PURE SDst
FetchStringWidth(const Font& fnt, _tIter s, _tIter g, char32_t c = {})
{
	SDst w(0);

	for(; s != g && *s != c; ++s)
	{
		using ystdex::is_undereferenceable;

		YAssert(!is_undereferenceable(s), "Invalid iterator found.");
		w += FetchCharWidth(fnt, *s);
	}
	return w;
}
//@}
/*!
\brief 取单行字符串在字体指定、无边界限制时的显示宽度。
\since build 483
*/
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
YB_ATTR_nodiscard YB_PURE inline SDst
FetchStringWidth(const Font& fnt, const _tString& str)
{
	return Drawing::FetchStringWidth(fnt, str.c_str());
}
/*!
\brief 取单行字符串前不超过 n 个字符在字体指定、无边界限制时的显示宽度。
\since build 484
*/
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
YB_ATTR_nodiscard YB_PURE inline SDst
FetchStringWidth(const Font& fnt, const _tString& str, size_t n)
{
	return Drawing::FetchStringWidth(fnt, str.data(), n);
}
//! \note 使用 ADL PrintString 和空文本渲染器渲染字符串。
//@{
/*!
\brief 取迭代器指定的单行字符串在指定文本状态和高度限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\note 字体由文本状态指定。
\since build 483
*/
template<typename _tIter,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
YB_ATTR_nodiscard YB_PURE SDst
FetchStringWidth(TextState& ts, SDst h, _tIter s)
{
	const SPos x(ts.Pen.X);
	EmptyTextRenderer r(ts, h);

	PrintString(r, s);
	return SDst(ts.Pen.X - x);
}
/*!
\brief 取迭代器指定的单行字符串在指定文本状态和高度限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\note 字体由文本状态指定。
\since build 641
*/
template<typename _tIter,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
YB_ATTR_nodiscard YB_PURE SDst
FetchStringWidth(TextState& ts, SDst h, _tIter s, _tIter g, char32_t c = {})
{
	const SPos x(ts.Pen.X);
	EmptyTextRenderer r(ts, h);

	PrintString(r, s, g, c);
	return SDst(ts.Pen.X - x);
}
//@}
/*!
\brief 取单行字符串在指定文本状态和高度限制时的显示宽度。
\note 字体由文本状态指定。
\since build 483
*/
template<class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
YB_ATTR_nodiscard YB_PURE inline SDst
FetchStringWidth(TextState& ts, SDst h, const _tString& str)
{
	return Drawing::FetchStringWidth(ts, h, str.c_str());
}


/*!
\brief 取指定字体显示的迭代器范围中的文本的最大宽度。
\since build 483
*/
template<typename _tIn,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIn>)>
YB_ATTR_nodiscard YB_PURE SDst
FetchMaxTextWidth(const Font& font, _tIn first, _tIn last)
{
	return std::accumulate(first, last, SDst(),
		[&] YB_LAMBDA_ANNOTATE((SDst val, decltype(*first) str), , pure){
		return ystdex::max(val, Drawing::FetchStringWidth(font, str));
	});
}

} // namespace Drawing;

} // namespace YSLib;

#endif

