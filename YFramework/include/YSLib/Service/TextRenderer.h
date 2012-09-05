/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextRenderer.h
\ingroup Service
\brief 文本渲染。
\version r2697
\author FrankHB<frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2012-09-04 12:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextRenderer
*/


#ifndef YSL_INC_SERVICE_TEXTRENDERER_H_
#define YSL_INC_SERVICE_TEXTRENDERER_H_ 1

#include "CharRenderer.h"
#include "../Core/ystring.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)


/*!
\brief 打印迭代器指定的字符串，直至行尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\return 指向结束位置的迭代器。
\since build 190
*/
template<typename _tIn, class _tRenderer>
_tIn
PrintLine(_tRenderer& r, _tIn s)
{
	while(*s != 0 && *s != '\n')
	{
		PrintChar(r, *s);
		++s;
	}
	return s;
}
/*!
\brief 打印迭代器指定的字符串，直至行尾或字符迭代终止。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\return 指向结束位置的迭代器。
\since build 251
*/
template<typename _tIn, class _tRenderer>
_tIn
PrintLine(_tRenderer& r, _tIn s, _tIn g, ucs4_t c = '\0')
{
	while(s != g && ucs4_t(*s) != c && *s != '\n')
	{
		PrintChar(r, *s);
		++s;
	}
	return s;
}
/*!
\brief 打印字符串，直至行尾或字符串结束。
\return 打印字符数。
\since build 190
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
\since build 190
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
\since build 251
*/
template<typename _tIn, class _tRenderer>
_tIn
PutLine(_tRenderer& r, _tIn s, _tIn g, ucs4_t c = '\0')
{
	TextState& ts(r.GetTextState());
	const SPos fpy(ts.PenY);

	while(s != g && ucs4_t(*s) != c && fpy == ts.PenY)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印字符串，直至行尾或字符串结束。
\note 当行内无法容纳完整字符时换行。
\return 打印字符数。
\since build 190
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
\since build 190
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
\since build 251
*/
template<typename _tIn, class _tRenderer>
_tIn
PrintString(_tRenderer& r, _tIn s, _tIn g, ucs4_t c = '\0')
{
	while(s != g && ucs4_t(*s) != c && *s != '\n')
		PrintChar(r, *s++);
	return s;
}
/*!
\brief 打印字符串，直至区域末尾或字符串结束。
\return 打印字符数。
\since build 190
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
\since build 190
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
\since build 251
*/
template<typename _tIn, class _tRenderer>
_tIn
PutString(_tRenderer& r, _tIn s, _tIn g, ucs4_t c = '\0')
{
	TextState& ts(r.GetTextState());
	const SPos mpy(FetchLastLineBasePosition(ts, r.GetHeight()));

	while(s != g && ucs4_t(*s) != c && ts.PenY <= mpy)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印字符串，直至区域末尾或字符串结束。
\note 当行内无法容纳完整字符时换行。
\return 打印字符数。
\since build 190
*/
template<class _tRenderer>
inline String::size_type
PutString(_tRenderer& r, const String& str)
{
	return PutString(r, str.c_str()) - str.c_str();
}


/*!	\defgroup TextRenderers Text Renderers
\brief 文本渲染器。
\since build 190
*/

/*!
\ingroup TextRenderers
\brief 空文本渲染器。
\since build 196
*/
class EmptyTextRenderer
{
public:
	TextState& State;
	SDst Height;

	EmptyTextRenderer(TextState& ts, SDst h)
		: State(ts), Height(h)
	{}

	/*!
	\brief 渲染单个字符：仅移动笔，不绘制。
	*/
	void
	operator()(ucs4_t c)
	{
		MovePen(State, c);
	}

	DefGetter(const ynothrow, const TextState&, TextState, State)
	DefGetter(ynothrow, TextState&, TextState, State)
	DefGetter(const ynothrow, SDst, Height, Height)
};


/*!
\brief 文本渲染器静态多态基类模版。
\warning 非虚析构。
\since build 266
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
		CThis->GetContext().GetHeight() + CThis->GetTextState().LineGap))

#undef CThis
#undef This

};


/*!
\ingroup TextRenderers
\brief 文本渲染器。
\warning 非虚析构。
\since build 190

文本渲染器：简单实现。
*/
class TextRenderer : public GTextRendererBase<TextRenderer>
{
public:
	TextState& State;
	const Graphics& Buffer;
	Rect ClipArea;

	TextRenderer(TextState& ts, const Graphics& g)
		: GTextRendererBase<TextRenderer>(),
		State(ts), Buffer(g), ClipArea(Point::Zero, g.GetSize())
	{}
	/*
	\brief 构造：使用文本状态、图形接口上下文和指定区域边界。
	\since build 265
	*/
	TextRenderer(TextState& ts, const Graphics& g, const Rect& mask)
		: GTextRendererBase<TextRenderer>(),
		State(ts), Buffer(g), ClipArea(mask)
	{}

	/*!
	\brief 渲染单个字符。
	*/
	void
	operator()(ucs4_t c)
	{
		RenderChar(c, State, TextRenderer::GetContext(), ClipArea, nullptr);
	}

	ImplS(GTextRendererBase) DefGetter(const ynothrow, const TextState&,
		TextState, State)
	ImplS(GTextRendererBase) DefGetter(ynothrow, TextState&, TextState, State)
	ImplS(GTextRendererBase) DefGetter(const ynothrow, const Graphics&, Context,
		Buffer)
	//! \since build 308
	//@{
	DefGetterMem(const ynothrow, SDst, Height, Buffer)
	DefGetterMem(const ynothrow, SDst, Width, Buffer)
	DefGetterMem(const ynothrow, const Size&, Size, Buffer)
	//@}

	/*!
	\brief 清除缓冲区第 l 行起始的 n 行像素。
	\note 图形接口上下文不可用或 l 越界时忽略。
	\note n 被限制为不越界。
	\note n 为 0 时清除之后的所有行。
	*/
	void
	ClearLine(u16 l, SDst n);
};


/*!
\ingroup TextRenderers
\brief 文本区域。

自带缓冲区的文本渲染器，通过 Alpha 贴图刷新至位图缓冲区显示光栅化文本。
\since 早于 build 132
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
	\brief 复制构造：默认实现。
	\since build 296
	*/
	DefDeCopyCtor(TextRegion)
	/*!
	\brief 转移构造：默认实现。
	\since build 296
	*/
	DefDeMoveCtor(TextRegion)

	/*!
	\brief 从文本状态中恢复状态。
	*/
	TextRegion&
	operator=(const TextState& ts)
	{
		TextState::operator=(ts);
		return *this;
	}
	/*!
	\brief 复制赋值：默认实现。
	\since build 296
	*/
	DefDeCopyAssignment(TextRegion)
	/*!
	\brief 转移赋值：默认实现。
	\since build 296
	*/
	DefDeMoveAssignment(TextRegion)

	/*!
	\brief 渲染单个字符。
	*/
	void
	operator()(ucs4_t c)
	{
		RenderChar(c, *this, *this, Rect(Point::Zero, GetSize()),
			GetBufferAlphaPtr());
	}

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


/*!
\brief 绘制剪切区域的文本。
\param g 输出图形接口上下文。
\param mask 相对于输出图形接口上下文矩形，限定输出边界。
\param ts 输出时使用的文本状态。
\param str 待绘制的字符串。
\param line_wrap 自动换行。
\since build 309
*/
void
DrawClippedText(const Graphics& g, const Rect& mask, TextState& ts,
	const String& str, bool line_wrap);
/*!
\brief 绘制剪切区域的文本。
\param g 输出图形接口上下文。
\param mask 相对于输出图形接口上下文矩形，限定输出边界。
\param bounds 绘制区域的外边界。
\param str 待绘制的字符串。
\param margin 绘制区域边界和外边界之间的边距。
\param line_wrap 自动换行。
\param fnt 输出时使用的字体。
\since build 309
*/
void
DrawClippedText(const Graphics& g, const Rect& mask, const Rect& bounds,
	const String& str, const Padding& margin, Color, bool line_wrap,
	const Font& fnt = Font());

/*!
\brief 绘制文本。
\param g 输出图形接口上下文。
\param ts 输出时使用的文本状态。
\param str 待绘制的字符串。
\param line_wrap 自动换行。
\since build 309
*/
void
DrawText(const Graphics& g, TextState& ts, const String& str, bool line_wrap);
/*!
\brief 绘制文本。
\param g 输出图形接口上下文。
\param bounds 绘制区域的外边界。
\param str 待绘制的字符串。
\param margin 绘制区域边界和外边界之间的边距。
\param line_wrap 自动换行。
\param fnt 输出时使用的字体。
\since build 309
*/
void
DrawText(const Graphics& g, const Rect& bounds, const String& str,
	const Padding& margin, Color, bool line_wrap, const Font& fnt = Font());
/*!
\brief 绘制文本。
\param r 文本区域。
\param g 输出图形接口上下文。
\param pt 绘制区域左上角相对于图形接口上下文的位置偏移。
\param s 绘制区域大小。
\param str 待绘制的字符串。
\param line_wrap 自动换行。
\note 间接绘制并贴图。
\since build 309
*/
void
DrawText(TextRegion& r, const Graphics& g, const Point& pt, const Size& s,
	const String& str, bool line_wrap);

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

