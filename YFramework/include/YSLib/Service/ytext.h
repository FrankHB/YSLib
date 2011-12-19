/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytext.h
\ingroup Service
\brief 基础文本显示。
\version r7348;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-13 00:06:05 +0800;
\par 修改时间:
	2011-12-18 12:41 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YText;
*/


#ifndef YSL_INC_SERVICE_YTEXT_H_
#define YSL_INC_SERVICE_YTEXT_H_

#include "ygdi.h"
#include "../Core/ystring.h"
#include <cwctype>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 文本状态。

包含笔样式、显示区域边界、字体缓存和行距。
文本区域指文本状态描述的平面区域。
文本区域位置坐标是文本区域左上角为原点的屏幕坐标系。
笔位置以文本区域位置坐标表示。
显示区域为文本区域内部实际显示文本光栅化结果的区域。
边距描述显示区域和文本区域的位置关系。
文本状态不包含文本区域和显示区域的大小，应由外部图形接口上下文或缓冲区状态确定。
\since build 145 。
*/
class TextState : public PenStyle
{
public:
	Padding Margin; //!< 边距：文本区域到显示区域的距离。
	SPos PenX, PenY; //!< 笔坐标。
	u8 LineGap; //!< 行距。

public:
	/*!
	\brief 无参数构造。
	*/
	TextState();
	/*!
	\brief 构造：使用指定字体。
	*/
	explicit
	TextState(Drawing::Font&);
	/*!
	\brief 构造：使用指定字体缓存。
	*/
	explicit
	TextState(FontCache&);

	/*!
	\brief 赋值：恢复笔样式。
	*/
	TextState&
	operator=(const PenStyle& ps);
	/*!
	\brief 赋值：恢复边距。
	*/
	TextState&
	operator=(const Padding& ms);

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
	\brief 按指定显示区域边界、文本区域大小和附加边距重新设置边距和笔位置。
	\note 通过已有的区域大小和附加边距约束新的边距和笔位置。
	*/
	void
	ResetForBounds(const Rect&, const Size&, const Padding&);
};

inline TextState&
TextState::operator=(const PenStyle& ps)
{
	PenStyle::operator=(ps);
	return *this;
}
inline TextState&
TextState::operator=(const Padding& ms)
{
	Margin = ms;
	return *this;
}


/*!
\brief 取当前指定文本状态的字体设置对应的行高。
\since build 231 。
*/
inline SDst
GetTextLineHeightOf(const TextState& s)
{
	return s.GetCache().GetHeight();
}

/*!
\brief 取当前指定文本状态的字体设置对应的行高与行距之和。
\since build 231 。
*/
inline SDst
GetTextLineHeightExOf(const TextState& s)
{
	return s.GetCache().GetHeight() + s.LineGap;
}

/*!
\brief 取笔所在的当前行数。
\since build 231 。
*/
inline u16
GetCurrentTextLineNOf(const TextState& s)
{
	return (s.PenY - s.Margin.Top) / GetTextLineHeightExOf(s);
}

/*!
\brief 设置笔位置。
\since build 231 。
*/
inline void
SetPenOf(TextState& s, SPos x, SPos y)
{
	s.PenX = x;
	s.PenY = y;
}

/*!
\brief 设置边距。
\note 4 个 \c SDst 形式。
\since build 231 。
*/
inline void
SetMarginsOf(TextState& s, SDst l, SDst r, SDst t, SDst b)
{
	SetAllOf(s.Margin, l, r, t, b);
	s.ResetPen();
}
/*!
\brief 设置边距。
\note 64 位无符号整数形式。
\since build 231 。
*/
inline void
SetMarginsOf(TextState& s, u64 m)
{
	SetAllOf(s.Margin, m);
	s.ResetPen();
}
/*!
\brief 设置边距。
\note 2 个 16 位无符号整数形式，分别表示水平边距和竖直边距。
\since build 231 。
*/
inline void
SetMarginsOf(TextState& s, SDst h, SDst v)
{
	SetAllOf(s.Margin, h, v);
	s.ResetPen();
}

/*!
\brief 设置笔的行位置。
\since build 231 。
*/
void
SetCurrentTextLineNOf(TextState&, u16);

/*!
\brief 回车。

指定文本状态的笔的水平位置移至左端，竖直位置不变。
\since build 270 。
*/
inline void
CarriageReturn(TextState& ts)
{
	ts.PenX = ts.Margin.Left;
}

/*!
\brief 按字符跨距移动笔。
\since build 196 。
*/
void
MovePen(TextState&, ucs4_t);


/*!
\brief 打印单个字符。
\param c 被打印的 UCS4 字符。
\param ts 文本状态。
\param g 输出图形接口上下文。
\param mask 相对于输出图形接口上下文矩形，限定输出边界。
\param alpha 输出设备接收的 8 位 Alpha 缓冲区首个元素的指针，若为 nullptr 则忽略。
\since build 254 。
*/
void
RenderChar(ucs4_t c, TextState& ts, const Graphics& g, const Rect& mask,
	u8* alpha);


/*!
\brief 取指定文本状态和文本区域高调整的底边距。
\return 返回调整后的底边距值（由字体大小、行距和高决定）。
\since build 252 。
*/
SDst
FetchResizedBottomMargin(const TextState&, SDst);

/*!
\brief 取指定文本状态和文本区域高所能显示的最大文本行数。
\pre 断言： <tt>GetTextLineHeightExOf(ts) != 0</tt> 。
\return 最大能容纳的文本行数。
\since build 252 。
*/
u16
FetchResizedLineN(const TextState& ts, SDst);

/*!
\brief 取指定文本状态表示的最底行的基线位置（纵坐标）。
\since build 190 。
*/
SPos
FetchLastLineBasePosition(const TextState&, SDst);


/*!
\brief 打印单个可打印字符。
\since build 270 。
*/
template<class _tRenderer>
void
PrintChar(_tRenderer& r, ucs4_t c)
{
	if(std::iswprint(c))
		r(c);
}

/*!
\brief 打印单个字符。
\note 处理换行符。
\note 当行内无法容纳完整字符时换行。
\since build 190 。
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
	if(!std::iswprint(c))
		return 0;
/*
	const int maxW(GetBufWidthN() - 1), spaceW(ts.GetCache().GetAdvance(' '));

	if(maxW < spaceW)
		return lineBreaksL = 1;
*/
	if(ts.PenX + ts.GetCache().GetAdvance(c)
		>= r.GetContext().GetWidth() - ts.Margin.Right)
	{
		ts.PutNewline();
		return 1;
	}
	r(c);
	return 0;
}

/*!
\brief 打印迭代器指定的字符串，直至行尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\return 指向结束位置的迭代器。
\since build 190 。
*/
template<typename _tIn, class _tRenderer>
_tIn
PrintLine(_tRenderer& r, _tIn s)
{
	while(*s != 0 && *s != '\n')
		PrintChar(r, *s++);
	return s;
}
/*!
\brief 打印迭代器指定的字符串，直至行尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\return 指向结束位置的迭代器。
\since build 251 。
*/
template<typename _tIn, class _tRenderer>
_tIn
PrintLine(_tRenderer& r, _tIn s, _tIn g, ucs4_t c = '\0')
{
	while(s != g && *s != c && *s != '\n')
		PrintChar(r, *s++);
	return s;
}
/*!
\brief 打印字符串，直至行尾或字符串结束。
\return 打印字符数。
\since build 190 。
*/
template<class _tRenderer>
inline String::size_type
PrintLine(_tRenderer& r, const String& str)
{
	return PrintLine(r, str.c_str()) - str.c_str();
}

/*!
\brief 打印迭代器指定的字符串，直至行尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\note 当行内无法容纳完整字符时换行。
\return 指向结束位置的迭代器。
\since build 190 。
*/
template<typename _tIn, class _tRenderer>
_tIn
PutLine(_tRenderer& r, _tIn s)
{
	TextState& ts(r.GetTextState());
	const SPos fpy(ts.PenY);

	while(*s != 0 && fpy == ts.PenY)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印迭代器指定的字符串，直至行尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\note 当行内无法容纳完整字符时换行。
\return 指向结束位置的迭代器。
\since build 251 。
*/
template<typename _tIn, class _tRenderer>
_tIn
PutLine(_tRenderer& r, _tIn s, _tIn g, ucs4_t c = '\0')
{
	TextState& ts(r.GetTextState());
	const SPos fpy(ts.PenY);

	while(s != g && *s != c && fpy == ts.PenY)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印字符串，直至行尾或字符串结束。
\note 当行内无法容纳完整字符时换行。
\return 打印字符数。
\since build 190 。
*/
template<class _tRenderer>
inline String::size_type
PutLine(_tRenderer& r, const String& str)
{
	return PutLine(r, str.c_str()) - str.c_str();
}

/*!
\brief 打印迭代器指定的字符串，直至区域末尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\return 指向结束位置的迭代器。
\since build 190 。
*/
template<typename _tIn, class _tRenderer>
_tIn
PrintString(_tRenderer& r, _tIn s)
{
	while(*s != 0 && *s != '\n')
		PrintChar(r, *s++);
	return s;
}
/*!
\brief 打印迭代器指定的字符串，直至区域末尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\return 指向结束位置的迭代器。
\since build 251 。
*/
template<typename _tIn, class _tRenderer>
_tIn
PrintString(_tRenderer& r, _tIn s, _tIn g, ucs4_t c = '\0')
{
	while(s != g && *s != c && *s != '\n')
		PrintChar(r, *s++);
	return s;
}
/*!
\brief 打印字符串，直至区域末尾或字符串结束。
\return 打印字符数。
\since build 190 。
*/
template<class _tRenderer>
inline String::size_type
PrintString(_tRenderer& r, const String& str)
{
	return PrintString(r, str.c_str()) - str.c_str();
}

/*!
\brief 打印迭代器指定的字符串，直至区域末尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\note 当行内无法容纳完整字符时换行。
\return 指向结束位置的迭代器。
\since build 190 。
*/
template<typename _tIn, class _tRenderer>
_tIn
PutString(_tRenderer& r, _tIn s)
{
	TextState& ts(r.GetTextState());
	const SPos mpy(FetchLastLineBasePosition(ts, r.GetHeight()));

	while(*s != 0 && ts.PenY <= mpy)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印迭代器指定的字符串，直至区域末尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\note 当行内无法容纳完整字符时换行。
\return 指向结束位置的迭代器。
\since build 251 。
*/
template<typename _tIn, class _tRenderer>
_tIn
PutString(_tRenderer& r, _tIn s, _tIn g, ucs4_t c = '\0')
{
	TextState& ts(r.GetTextState());
	const SPos mpy(FetchLastLineBasePosition(ts, r.GetHeight()));

	while(s != g && *s != c && ts.PenY <= mpy)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印字符串，直至区域末尾或字符串结束。
\note 当行内无法容纳完整字符时换行。
\return 打印字符数。
\since build 190 。
*/
template<class _tRenderer>
inline String::size_type
PutString(_tRenderer& r, const String& str)
{
	return PutString(r, str.c_str()) - str.c_str();
}


/*!
\brief 取指定的字符使用指定字体缓存的当前字体的显示宽度。
\note 无边界限制。
\since build 270 。
*/
SDst
FetchCharWidth(FontCache&, ucs4_t);
/*!
\brief 取指定的字符使用指定字体的显示宽度。
\note 无边界限制。
\since build 214 。
*/
inline SDst
FetchCharWidth(const Font& fnt, ucs4_t c)
{
	return FetchCharWidth(fnt.GetCache(), c);
}


/*!	\defgroup TextRenderers Text Renderers
\brief 文本渲染器。
\since build 190 。
*/

/*!
\ingroup TextRenderers
\brief 空文本渲染器。
\since build 196 。
*/
class EmptyTextRenderer
{
public:
	TextState& State;
	SDst Height;

	EmptyTextRenderer(TextState&, SDst);

	/*!
	\brief 渲染单个字符：仅移动笔，不绘制。
	*/
	void
	operator()(ucs4_t);

	DefGetter(const ynothrow, const TextState&, TextState, State)
	DefGetter(ynothrow, TextState&, TextState, State)
	DefGetter(const ynothrow, SDst, Height, Height)
};

inline
EmptyTextRenderer::EmptyTextRenderer(TextState& ts, SDst h)
	: State(ts), Height(h)
{}

inline void
EmptyTextRenderer::operator()(ucs4_t c)
{
	MovePen(State, c);
}


/*!
\brief 文本渲染器静态多态模版基类。
\since build 266 。
*/
template<class _type>
class GTextRendererBase
{
public:
	DeclSEntry(const TextState& GetTextState() const) //!< 取文本状态。
	DeclSEntry(TextState& GetTextState()) //!< 取文本状态。
	DeclSEntry(const Graphics& GetContext() const) //!< 取图形接口上下文。

#define This static_cast<_type*>(this)
#define CThis static_cast<const _type*>(this)

	/*!
	\brief 取按当前行高和行距所能显示的最大行数。
	*/
	DefGetter(const, u16, TextLineN, FetchResizedLineN(CThis->GetTextState(),
		CThis->GetContext().GetHeight()))
	/*!
	\brief 取按当前行高和行距（行间距数小于行数 1 ）所能显示的最大行数。
	*/
	DefGetter(const, u16, TextLineNEx, FetchResizedLineN(CThis->GetTextState(),
		CThis->GetContext().GetHeight()) + CThis->GetTextState().LineGap)

#undef CThis
#undef This

};


/*!
\ingroup TextRenderers
\brief 文本渲染器。

简单实现。
\since build 190 。
*/
class TextRenderer : public GTextRendererBase<TextRenderer>
{
public:
	TextState& State;
	const Graphics& Buffer;
	Rect ClipArea;

	TextRenderer(TextState&, const Graphics&);
	/*
	\brief 构造：使用文本状态、图形接口上下文和指定区域边界。
	\since build 265 。
	*/
	TextRenderer(TextState&, const Graphics&, const Rect&);

	/*!
	\brief 渲染单个字符。
	*/
	void
	operator()(ucs4_t);

	ImplS(GTextRendererBase) DefGetter(const ynothrow, const TextState&,
		TextState, State)
	ImplS(GTextRendererBase) DefGetter(ynothrow, TextState&, TextState, State)
	ImplS(GTextRendererBase) DefGetter(const ynothrow, const Graphics&, Context,
		Buffer)

	/*!
	\brief 清除缓冲区第 l 行起始的 n 行像素。
	\note 图形接口上下文不可用或 l 越界时忽略。
	\note n 被限制为不越界。
	\note n 为 0 时清除之后的所有行。
	*/
	void
	ClearLine(u16 l, SDst n);
};

inline
TextRenderer::TextRenderer(TextState& ts, const Graphics& g)
	: GTextRendererBase<TextRenderer>(),
	State(ts), Buffer(g), ClipArea(Point::Zero, g.GetSize())
{}
inline
TextRenderer::TextRenderer(TextState& ts, const Graphics& g, const Rect& mask)
	: GTextRendererBase<TextRenderer>(),
	State(ts), Buffer(g), ClipArea(mask)
{}

inline void
TextRenderer::operator()(ucs4_t c)
{
	RenderChar(c, State, TextRenderer::GetContext(), ClipArea, nullptr);
}


/*!
\ingroup TextRenderers
\brief 文本区域。

自带缓冲区的文本渲染器，通过 Alpha 贴图刷新至位图缓冲区显示光栅化文本。
\since 早于 build 132 。
*/
class TextRegion : public GTextRendererBase<TextRegion>,
	public TextState, public BitmapBufferEx
{
public:
	/*!
	\brief 无参数构造。
	*/
	TextRegion();
	/*!
	\brief 构造：使用指定字体。
	*/
	explicit
	TextRegion(Drawing::Font&);
	/*!
	\brief 构造：使用指定字体缓存。
	*/
	explicit
	TextRegion(FontCache&);

	/*!
	\brief 从文本状态中恢复状态。
	*/
	TextRegion&
	operator=(const TextState&);

	/*!
	\brief 渲染单个字符。
	*/
	void
	operator()(ucs4_t);

	ImplS(GTextRendererBase) DefGetter(const ynothrow, const TextState&,
		TextState, *this)
	ImplS(GTextRendererBase) DefGetter(ynothrow, TextState&, TextState, *this)
	ImplS(GTextRendererBase) DefGetter(const ynothrow, const Graphics&, Context,
		*this)

protected:
	/*!
	\brief 初始化字体。
	*/
	void
	InitializeFont();

public:
	/*!
	\brief 清除缓冲区第 l 行起始的 n 行像素。
	\note n 为 0 时清除之后的所有行。
	*/
	void
	ClearLine(u16 l, SDst n);

	/*!
	\brief 清除缓冲区中的指定行号的文本行。
	\note 参数为 0 表示首行。
	*/
	void
	ClearTextLine(u16);

	/*!
	\brief 缓冲区特效：整体移动 n 像素。
	\note 除上下边界区域。
	\note n > 0 时下移， n < 0 时上移。
	*/
	void
	Scroll(ptrdiff_t n);
	/*!
	\brief 缓冲区特效：整体移动 n 像素。
	\note 从缓冲区顶端起高 h 的区域内，除上下边界区域。
	\note n > 0 时下移， n < 0 时上移。
	*/
	void
	Scroll(ptrdiff_t n, SDst h);
};

inline TextRegion&
TextRegion::operator=(const TextState& ts)
{
	TextState::operator=(ts);
	return *this;
}

inline void
TextRegion::operator()(ucs4_t c)
{
	RenderChar(c, *this, *this, Rect(Point::Zero, GetSize()),
		GetBufferAlphaPtr());
}

/*!
\brief 取按字体高度和行距调整文本区域的底边距。
\since build 252 。
*/
inline SDst
FetchResizedBottomMargin(const TextRegion& tr)
{
	return FetchResizedBottomMargin(tr, tr.GetHeight());
}

/*!
\brief 按字体高度和行距调整文本区域的底边距。
\since build 252 。
*/
inline SDst
AdjustBottomMarginOf(TextRegion& tr)
{
	return tr.Margin.Bottom = FetchResizedBottomMargin(tr);
}


/*!
\brief 取迭代器指定的字符串在字体指定、无边界限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\since build 214 。
*/
template<typename _tIn>
SDst
FetchStringWidth(const Font& fnt, _tIn s)
{
	SDst w(0);

	while(*s != '\0')
		w += FetchCharWidth(fnt, *s++);
	return w;
}
/*!
\brief 取迭代器指定的单行字符串在字体指定、无边界限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\since build 251 。
*/
template<typename _tIn>
SDst
FetchStringWidth(const Font& fnt, _tIn s, _tIn g, ucs4_t c = '\0')
{
	SDst w(0);

	while(s != g && *s != c)
		w += FetchCharWidth(fnt, *s++);
	return w;
}
/*!
\brief 取单行字符串在字体指定、无边界限制时的显示宽度。
\since build 214 。
*/
inline SDst
FetchStringWidth(const Font& fnt, const String& str)
{
	return FetchStringWidth(fnt, str.c_str());
}
/*!
\brief 取迭代器指定的单行字符串在指定文本状态和高度限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\note 字体由文本状态指定。
\since build 197 。
*/
template<typename _tIn>
SDst
FetchStringWidth(TextState& ts, SDst h, _tIn s)
{
	const SPos x(ts.PenX);
	EmptyTextRenderer r(ts, h);

	PrintString(r, s);
	return ts.PenX - x;
}
/*!
\brief 取迭代器指定的单行字符串在指定文本状态和高度限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\note 字体由文本状态指定。
\since build 251 。
*/
template<typename _tIn>
SDst
FetchStringWidth(TextState& ts, SDst h, _tIn s, _tIn g, ucs4_t c = '\0')
{
	const SPos x(ts.PenX);
	EmptyTextRenderer r(ts, h);

	PrintString(r, s, g, c);
	return ts.PenX - x;
}
/*!
\brief 取单行字符串在指定文本状态和高度限制时的显示宽度。
\note 字体由文本状态指定。
\since build 197 。
*/
inline SDst
FetchStringWidth(TextState& ts, SDst h, const String& str)
{
	return FetchStringWidth(ts, h, str.c_str());
}


/*!
\brief 绘制文本。
\since build 265 。
*/
void
DrawClippedText(const Graphics&, const Rect& mask, TextState&, const String&);
/*!
\brief 绘制文本。
\since build 265 。
*/
void
DrawClippedText(const Graphics&, const Rect& mask, const Rect& bound,
	const String&, const Padding&, Color);

/*!
\brief 绘制文本。
\since build 190 。
*/
void
DrawText(const Graphics&, TextState&, const String&);
/*!
\brief 绘制文本。
\since build 190 。
*/
void
DrawText(const Graphics&, const Rect&, const String&, const Padding&, Color);
/*!
\brief 绘制文本。
\note 间接绘制并贴图。
\since build 188 。
*/
void
DrawText(TextRegion&, const Graphics&, const Point&, const Size&,
	const String&);

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)



YSL_END_NAMESPACE(Text)

YSL_END

#endif

