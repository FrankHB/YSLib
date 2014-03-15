/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextRenderer.h
\ingroup Service
\brief 文本渲染。
\version r2935
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2014-03-15 10:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextRenderer
*/


#ifndef YSL_INC_Service_TextRenderer_h_
#define YSL_INC_Service_TextRenderer_h_ 1

#include "YModules.h"
#include YFM_YSLib_Service_CharRenderer
#include YFM_YSLib_Core_YString

namespace YSLib
{

namespace Drawing
{

/*!
\brief 打印迭代器指定的起始字符的字符串，直至行尾或字符迭代终止。
\param r 使用的字符渲染器。
\param s 指向字符串起始字符的输入迭代器。
\return 指向结束位置的迭代器。
\note 迭代直至字符串结束符。
\since build 483
*/
template<typename _tIter, class _tRenderer,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
_tIter
PrintLine(_tRenderer& r, _tIter s)
{
	while(*s != 0 && *s != '\n')
	{
		PrintChar(r, *s);
		++s;
	}
	return s;
}
/*!
\brief 打印迭代器指定的起始字符的字符串，直至行尾或字符迭代终止。
\param r 使用的字符渲染器。
\param s 指向字符串起始字符的输入迭代器。
\param g 指示迭代终止位置的输入迭代器。
\param c 指向迭代终止的字符。
\return 指向结束位置的迭代器。
\note 迭代直至 g 指定的位置或指定位置的字符为 c 。
\since build 483
*/
template<typename _tIter, class _tRenderer,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
_tIter
PrintLine(_tRenderer& r, _tIter s, _tIter g, ucs4_t c = {})
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
\param r 使用的字符渲染器。
\param str 被输出的字符串。
\return 打印字符数。
\since build 483
*/
template<class _tRenderer, class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline String::size_type
PrintLine(_tRenderer& r, const _tString& str)
{
	return PrintLine(r, &str[0]) - &str[0];
}

/*!
\brief 打印迭代器指定的起始字符的字符串，直至行尾或字符迭代终止。
\param r 使用的字符渲染器。
\param s 指向字符串起始字符的输入迭代器。
\return 指向结束位置的迭代器。
\note 迭代直至字符串结束符。
\note 当行内无法容纳完整字符时换行。
\since build 483
*/
template<typename _tIter, class _tRenderer,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
_tIter
PutLine(_tRenderer& r, _tIter s)
{
	TextState& ts(r.GetTextState());
	const SPos fpy(ts.Pen.Y);

	while(*s != 0 && fpy == ts.Pen.Y)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印迭代器指定的起始字符的字符串，直至行尾或字符迭代终止。
\param r 使用的字符渲染器。
\param s 指向字符串起始字符的输入迭代器。
\param g 指示迭代终止位置的输入迭代器。
\param c 指向迭代终止的字符。
\return 指向结束位置的迭代器。
\note 迭代直至 g 指定的位置或指定位置的字符为 c 。
\note 当行内无法容纳完整字符时换行。
\since build 483
*/
template<typename _tIter, class _tRenderer,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
_tIter
PutLine(_tRenderer& r, _tIter s, _tIter g, ucs4_t c = {})
{
	TextState& ts(r.GetTextState());
	const SPos fpy(ts.Pen.Y);

	while(s != g && ucs4_t(*s) != c && fpy == ts.Pen.Y)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印字符串，直至行尾或字符串结束。
\param r 使用的字符渲染器。
\param str 被输出的字符串。
\return 打印字符数。
\note 当行内无法容纳完整字符时换行。
\since build 483
*/
template<class _tRenderer, class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline String::size_type
PutLine(_tRenderer& r, const _tString& str)
{
	return PutLine(r, &str[0]) - &str[0];
}

/*!
\brief 打印迭代器指定的起始字符的字符串，直至区域末尾或字符迭代终止。
\param r 使用的字符渲染器。
\param s 指向字符串起始字符的输入迭代器。
\return 指向结束位置的迭代器。
\note 迭代直至字符串结束符。
\since build 483
*/
template<typename _tIter, class _tRenderer,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
_tIter
PrintString(_tRenderer& r, _tIter s)
{
	while(*s != 0 && *s != '\n')
		PrintChar(r, *s++);
	return s;
}
/*!
\brief 打印迭代器指定的起始字符的字符串，直至区域末尾或字符迭代终止。
\param r 使用的字符渲染器。
\param s 指向字符串起始字符的输入迭代器。
\param g 指示迭代终止位置的输入迭代器。
\param c 指向迭代终止的字符。
\return 指向结束位置的迭代器。
\note 迭代直至 g 指定的位置或指定位置的字符为 c 。
\since build 483
*/
template<typename _tIter, class _tRenderer,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
_tIter
PrintString(_tRenderer& r, _tIter s, _tIter g, ucs4_t c = {})
{
	while(s != g && ucs4_t(*s) != c && *s != '\n')
		PrintChar(r, *s++);
	return s;
}
/*!
\brief 打印字符串，直至区域末尾或字符串结束。
\param r 使用的字符渲染器。
\param str 被输出的字符串。
\return 打印字符数。
\since build 483
*/
template<class _tRenderer, class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline String::size_type
PrintString(_tRenderer& r, const _tString& str)
{
	return PrintString(r, &str[0]) - &str[0];
}

/*!
\brief 打印迭代器指定的起始字符的字符串，直至区域末尾或字符迭代终止。
\param r 使用的字符渲染器。
\param s 指向字符串起始字符的输入迭代器。
\return 指向结束位置的迭代器。
\note 迭代直至字符串结束符。
\note 当行内无法容纳完整字符时换行。
\since build 483
*/
template<typename _tIter, class _tRenderer,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
_tIter
PutString(_tRenderer& r, _tIter s)
{
	TextState& ts(r.GetTextState());
	const SPos mpy(FetchLastLineBasePosition(ts, r.GetHeight()));

	while(*s != 0 && ts.Pen.Y <= mpy)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印迭代器指定的起始字符的字符串，直至区域末尾或字符迭代终止。
\param r 使用的字符渲染器。
\param s 指向字符串起始字符的输入迭代器。
\param g 指示迭代终止位置的输入迭代器。
\param c 指向迭代终止的字符。
\return 指向结束位置的迭代器。
\note 迭代直至 g 指定的位置或指定位置的字符为 c 。
\note 当行内无法容纳完整字符时换行。
\since build 483
*/
template<typename _tIter, class _tRenderer,
	yimpl(typename = ystdex::enable_for_iterator_t<_tIter>)>
_tIter
PutString(_tRenderer& r, _tIter s, _tIter g, ucs4_t c = {})
{
	TextState& ts(r.GetTextState());
	const SPos mpy(FetchLastLineBasePosition(ts, r.GetHeight()));

	while(s != g && ucs4_t(*s) != c && ts.Pen.Y <= mpy)
		if(!PutChar(r, *s))
			++s;
	return s;
}
/*!
\brief 打印字符串，直至区域末尾或字符串结束。
\param r 使用的字符渲染器。
\param str 被输出的字符串。
\return 打印字符数。
\note 当行内无法容纳完整字符时换行。
\since build 483
*/
template<class _tRenderer, class _tString,
	yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
inline String::size_type
PutString(_tRenderer& r, const _tString& str)
{
	return PutString(r, &str[0]) - &str[0];
}

/*!
\brief 打印文本。
\param multi 指定是否可输出多行。
\param args 传递给 PutString 或 PutLine 的参数。
\since build 485
*/
template<typename... _tParams>
void
PutText(bool multi, _tParams&&... args)
{
	if(multi)
		PutString(yforward(args)...);
	else
		PutLine(yforward(args)...);
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
class YF_API EmptyTextRenderer
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
	PDefHOp(void, (), ucs4_t c)
		ImplExpr(MovePen(State, c))

	DefGetter(const ynothrow, const TextState&, TextState, State)
	DefGetter(ynothrow, TextState&, TextState, State)
	DefGetter(const ynothrow, SDst, Height, Height)
};


/*!
\brief 文本渲染器静态多态基类模板。
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
\brief 文本渲染器：简单实现。
\warning 非虚析构。
\since build 190
*/
class YF_API TextRenderer : public GTextRendererBase<TextRenderer>
{
public:
	TextState& State;
	const Graphics& Buffer;
	Rect ClipArea;

	TextRenderer(TextState& ts, const Graphics& g)
		: GTextRendererBase<TextRenderer>(),
		State(ts), Buffer(g), ClipArea(g.GetSize())
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
	operator()(ucs4_t);

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
\brief 定制文本渲染器：使用自定义的渲染函数替代的 TextRenderer 。
\warning 非虚析构。
\since build 485
*/
class YF_API CustomTextRenderer : public TextRenderer,
	public GTextRendererBase<CustomTextRenderer>
{
public:
	using RenderFunctionType = std::function<void(TextRenderer&, ucs4_t)>;

	RenderFunctionType RenderFunction;

	CustomTextRenderer(RenderFunctionType f, TextState& ts, const Graphics& g)
		: TextRenderer(ts, g), RenderFunction(f)
	{}
	CustomTextRenderer(RenderFunctionType f, TextState& ts, const Graphics& g,
		const Rect& mask)
		: TextRenderer(ts, g, mask), RenderFunction(f)
	{}

	/*!
	\brief 渲染单个字符。
	\note 使用 RenderFunction 。
	*/
	PDefHOp(void, (), ucs4_t c)
		ImplExpr(RenderFunction(*this, c))
};


/*!
\ingroup TextRenderers
\brief 文本区域。
\warning 非虚析构。
\since 早于 build 132

自带缓冲区的文本渲染器，通过 Alpha 贴图刷新至位图缓冲区显示光栅化文本。
*/
class YF_API TextRegion : public GTextRendererBase<TextRegion>,
	public TextState, public CompactPixmapEx
{
public:
	/*!
	\brief 无参数构造。
	*/
	TextRegion();
	/*!
	\brief 构造：使用指定参数确定文本状态。
	\since build 368
	*/
	template<typename... _tParams>
	explicit
	TextRegion(_tParams&&... args)
		: GTextRendererBase<TextRegion>(),
		TextState(yforward(args)...), CompactPixmapEx()
	{
		InitializeFont();
	}
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


/*!
\brief 绘制剪切区域的文本。
\param g 输出图形接口上下文。
\param mask 相对于输出图形接口上下文矩形，限定输出边界。
\param ts 输出时使用的文本状态。
\param str 待绘制的字符串。
\param line_wrap 自动换行。
\since build 309
*/
YF_API void
DrawClippedText(const Graphics& g, const Rect& mask, TextState& ts,
	const String& str, bool line_wrap);
/*!
\brief 绘制剪切区域的文本。
\param g 输出图形接口上下文。
\param mask 相对于输出图形接口上下文矩形，限定输出边界。
\param bounds 绘制区域的外边界。
\param str 待绘制的字符串。
\param m 绘制区域边界和外边界之间的边距。
\param line_wrap 自动换行。
\param fnt 输出时使用的字体。
\since build 309
*/
YF_API void
DrawClippedText(const Graphics& g, const Rect& mask, const Rect& bounds,
	const String& str, const Padding& m, Color, bool line_wrap,
	const Font& fnt = {});

/*!
\brief 绘制文本。
\param g 输出图形接口上下文。
\param ts 输出时使用的文本状态。
\param str 待绘制的字符串。
\param line_wrap 自动换行。
\since build 309
*/
YF_API void
DrawText(const Graphics& g, TextState& ts, const String& str, bool line_wrap);
/*!
\brief 绘制文本。
\param g 输出图形接口上下文。
\param bounds 绘制区域的外边界。
\param str 待绘制的字符串。
\param m 绘制区域边界和外边界之间的边距。
\param line_wrap 自动换行。
\param fnt 输出时使用的字体。
\since build 309
*/
YF_API void
DrawText(const Graphics& g, const Rect& bounds, const String& str,
	const Padding& m, Color, bool line_wrap, const Font& fnt = {});
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
YF_API void
DrawText(TextRegion& r, const Graphics& g, const Point& pt, const Size& s,
	const String& str, bool line_wrap);

} // namespace Drawing;

} // namespace YSLib;

#endif

