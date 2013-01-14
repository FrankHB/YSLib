/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextBase.h
\ingroup Service
\brief 基础文本渲染逻辑对象。
\version r2687
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2013-01-14 02:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextBase
*/


#ifndef YSL_INC_SERVICE_TEXTBASE_H_
#define YSL_INC_SERVICE_TEXTBASE_H_ 1

#include "ygdi.h"
#include "../Adaptor/Font.h"
#include "../Core/ystorage.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 默认边距。
\since build 286
*/
yconstexpr Padding DefaultMargin(2, 2, 2, 2);


/*!
\brief 笔样式：字体和笔颜色。
\warning 非虚析构。
\since build 145
*/
class YF_API PenStyle
{
public:
	Drawing::Font Font; //!< 字体。
	Drawing::Color Color; //!< 笔颜色。

	/*!
	\brief 构造：使用指定字体家族、字体大小和颜色。
	\since build 337
	*/
	explicit
	PenStyle(const Drawing::Font& fnt = {},
		Drawing::Color c = Drawing::ColorSpace::White)
		: Font(fnt), Color(c)
	{}

	DefGetterMem(const ynothrow, const FontFamily&, FontFamily, Font)
	DefGetterMem(const ynothrow, FontCache&, Cache, Font)
};


/*!
\brief 文本状态。

包含笔样式、显示区域边界、字体缓存和行距。
文本区域指文本状态描述的平面区域。
文本区域位置坐标是文本区域左上角为原点的屏幕坐标系。
笔位置以文本区域位置坐标表示。
显示区域为文本区域内部实际显示文本光栅化结果的区域。
边距描述显示区域和文本区域的位置关系。
文本状态不包含文本区域和显示区域的大小，应由外部图形接口上下文或缓冲区状态确定。
\since build 145
*/
class YF_API TextState : protected PenStyle
{
public:
	//! \since build 371
	using PenStyle::Font;
	//! \since build 371
	using PenStyle::Color;
	Padding Margin; //!< 边距：文本区域到显示区域的距离。
	/*!
	\brief 笔坐标。
	\since build 371
	*/
	Point Pen;
	u8 LineGap; //!< 行距。

	/*!
	\brief 构造：使用指定字体。
	\since build 337
	*/
	explicit
	TextState(const Drawing::Font& = {});
	/*!
	\brief 构造：使用指定字体缓存。
	*/
	explicit
	TextState(FontCache&);

	/*!
	\brief 赋值：笔样式。
	*/
	TextState&
	operator=(const PenStyle& ps)
	{
		PenStyle::operator=(ps);
		return *this;
	}
	/*!
	\brief 赋值：边距。
	*/
	TextState&
	operator=(const Padding& ms)
	{
		Margin = ms;
		return *this;
	}

	/*!
	\brief 打印换行。
	*/
	void
	PutNewline();

	/*!
	\brief 复位笔：按字体大小设置笔位置为默认位置。
	\note 默认笔位置在由边距约束的显示区域左上角。
	*/
	void
	ResetPen();
	/*!
	\brief 按指定显示区域左上角位置和附加边距重新设置笔位置。
	\since build 372

	通过已有的区域左上角坐标和附加边距的左和上分量约束新和笔位置。
	*/
	void
	ResetPen(const Point&, const Padding& = {});
};


/*!
\brief 取当前指定文本状态的字体设置对应的行高。
\since build 231
*/
inline SDst
GetTextLineHeightOf(const TextState& ts)
{
	return ts.Font.GetHeight();
}

/*!
\brief 取当前指定文本状态的字体设置对应的行高与行距之和。
\since build 231
*/
inline SDst
GetTextLineHeightExOf(const TextState& ts)
{
	return ts.Font.GetHeight() + ts.LineGap;
}

/*!
\brief 取笔所在的当前行数。
\since build 231
*/
inline u16
GetCurrentTextLineNOf(const TextState& ts)
{
	return (ts.Pen.Y - ts.Margin.Top) / GetTextLineHeightExOf(ts);
}

/*!
\brief 取第一行的基线位置（纵坐标）。
\since build 372
*/
inline SPos
GetTextLineBaseOf(const TextState& ts)
{
	return ts.Margin.Top + ts.Font.GetAscender();
}

/*!
\brief 设置笔位置。
\since build 231
*/
inline void
SetPenOf(TextState& ts, SPos x, SPos y)
{
	ts.Pen = Point(x, y);
}

/*!
\brief 设置笔的行位置。
\since build 231
*/
YF_API void
SetCurrentTextLineNOf(TextState&, u16);

/*!
\brief 按指定显示区域和文本区域的高重新设置右边距。
\note 右边距参与行的结尾位置计算。
\note 单独使用时，需要设置笔的起始位置（横坐标），否则只适用于单行显示。
\see TextState::AdjustMarginForBound 。
\see PutChar 。
\since build 372
*/
inline void
AdjustEndOfLine(TextState& ts, const Rect& r, SDst h)
{
//	ts.Margin = FetchMargin(r, s);
	ts.Margin.Right = h - r.Y - r.Width;
}

/*!
\brief 回车。

指定文本状态的笔的水平位置移至左端，竖直位置不变。
\since build 270
*/
inline void
CarriageReturn(TextState& ts)
{
	ts.Pen.X = ts.Margin.Left;
}

/*!
\brief 按字符跨距移动笔。
\since build 196
*/
YF_API void
MovePen(TextState&, ucs4_t);

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

