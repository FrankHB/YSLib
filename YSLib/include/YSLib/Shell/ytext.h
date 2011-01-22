/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytext.h
\ingroup Shell
\brief 基础文本显示。
\version 0.6666;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 00:06:05 + 08:00;
\par 修改时间:
	2010-01-23 06:57 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YText;
*/


#ifndef INCLUDED_YTEXT_H_
#define INCLUDED_YTEXT_H_

#include "../Core/ystring.h"
#include "../Core/yftext.h"
#include "ygdi.h"
#include <cwctype>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

//文本状态：笔样式、边框样式、字体缓存和行距。
class TextState : public PenStyle
{
public:
	typedef PenStyle ParentType;

	Padding Margin; //!< 显示区域到边框的距离。
	SPOS PenX, PenY; //!< 笔坐标。
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
	TextState(YFontCache&);
	/*!
	\brief 析构：空实现。
	\note 无异常抛出。
	*/
	virtual DefEmptyDtor(TextState)

	/*!
	\brief 从笔样式中恢复样式。
	*/
	TextState&
	operator=(const PenStyle& ps);
	/*!
	\brief 从边距样式中恢复样式。
	*/
	TextState&
	operator=(const Padding& ms);

	/*!
	\brief 复位笔：按字体大小设置笔位置为默认位置（区域左上角）。
	*/
	void
	ResetPen();
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
\brief 在指定文本状态中取当前字体设置对应的行高。
*/
inline SDST
GetLnHeightFrom(const TextState& s)
{
	return s.GetCache().GetHeight();
}

/*!
\brief 在指定文本状态中取当前字体设置对应的行高与行距之和。
*/
inline SDST
GetLnHeightExFrom(const TextState& s)
{
	return s.GetCache().GetHeight() + s.LineGap;
}

/*!
\brief 取笔所在的当前行数。
*/
inline u16
GetLnNNowFrom(const TextState& s)
{
	return (s.PenY - s.Margin.Top) / GetLnHeightExFrom(s);
}

/*!
\brief 设置笔位置。
*/
inline void
SetPenTo(TextState& s, SPOS x, SPOS y)
{
	s.PenX = x;
	s.PenY = y;
}

/*!
\brief 设置边距。
\note 4 个 16 位无符号整数形式。
*/
inline void
SetMarginsTo(TextState& s, SDST l, SDST r, SDST t, SDST b)
{
	SetAllTo(s.Margin, l, r, t, b);
	s.ResetPen();
}
/*!
\brief 设置边距。
\note 64 位无符号整数形式。
*/
inline void
SetMarginsTo(TextState& s, u64 m)
{
	SetAllTo(s.Margin, m);
	s.ResetPen();
}
/*!
\brief 设置边距。
\note 2 个 16 位无符号整数形式，分别表示水平边距和竖直边距。
*/
inline void
SetMarginsTo(TextState& s, SDST h, SDST v)
{
	SetAllTo(s.Margin, h, v);
	s.ResetPen();
}

/*!
\brief 设置笔的行位置。
*/
void
SetLnNNowTo(TextState&, u16);


/*!
\brief 打印单个字符。
*/
void
PrintChar(BitmapBuffer&, TextState&, fchar_t); 

/*!
\brief 打印单个字符。
*/
void
PrintCharEx(BitmapBufferEx&, TextState&, fchar_t); 


//文本区域。
class TextRegion : public TextState, public BitmapBufferEx
{
public:
	typedef TextState ParentType;

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
	TextRegion(YFontCache&);
	/*!
	\brief 析构：空实现。
	\note 无异常抛出。
	*/
	virtual DefEmptyDtor(TextRegion)

	/*!
	\brief 从文本状态中恢复状态。
	*/
	TextRegion& operator=(const TextState& ts);

	DefGetter(SDST, BufWidthN, GetWidth() - GetHorizontalFrom(Margin)) \
		//!< 取缓冲区的文本显示区域的宽。
	DefGetter(SDST, BufHeightN, GetHeight() - GetVerticalFrom(Margin)) \
		//!< 取缓冲区的文本显示区域的高。
	/*!
	\brief 根据字体大小、行距和缓冲区的高调整边距，返回调整后的底边距值。
	*/
	SDST
	GetMarginResized() const;
	/*!
	\brief 取根据字体大小和行距调整后的缓冲区的高。
	\note 不含底边距。
	*/
	SDST
	GetBufferHeightResized() const;
	/*!
	\brief 取按当前行高和行距所能显示的最大行数。
	*/
	u16
	GetLnN() const;
	/*!
	\brief 取按当前行高和行距（行间距数小于行数 1）所能显示的最大行数。
	*/
	u16
	GetLnNEx() const;
	/*!
	\brief 取最底行的基线位置。
	*/
	SPOS
	GetLineLast() const;

	/*!
	\brief 设置笔的行位置为最底行。
	*/
	void
	SetLnLast();

	/*!
	\brief 清除缓冲区第 l 行起始的 n 行像素。
	\note n 为 0 时清除之后的所有行。
	*/
	void
	ClearLine(u16 l, SDST n);

	//清除缓冲区第 l 个文本行。
	/*!
	\brief 清除缓冲区中的第 l 个文本行。
	\note l 为 0 表示首行。
	*/
	void
	ClearLn(u16 l);

	//
	/*!
	\brief 清除缓冲区中的最后一个文本行。
	*/
	void
	ClearLnLast();

	/*!
	\brief 缓冲区特效：整体移动 n 像素。
	\note 除上下边界区域；n > 0 时下移， n < 0 时上移。
	*/
	void
	Move(s16 n);
	/*!
	\brief 缓冲区特效：整体移动 n 像素。
	\note 从缓冲区顶端起高 h 的区域内，除上下边界区域；
	//			n > 0 时下移， n < 0 时上移。
	*/
	void
	Move(s16 n, SDST h);

	/*!
	\brief 输出换行。
	*/
	void
	PutNewline();

	/*!
	\brief 输出单个字符。
	*/
	u8
	PutChar(fchar_t);

	/*!
	\brief 输出迭代器 s 指向字符串，直至行尾或字符串结束。
	\return 输出迭代器。
	*/
	template<typename _tOut>
	_tOut
	PutLine(_tOut s);
	/*!
	\brief 输出迭代器 s 指向字符串，
		直至行尾、遇到指定迭代器 g 或遇到指定字符 f 。
	\return 输出迭代器。
	*/
	template<typename _tOut, typename _tChar>
	_tOut
	PutLine(_tOut s, _tOut g, _tChar f = '\0');
	/*!
	\brief 输出字符串，直至行尾或字符串结束。
	\return 输出字符数。
	*/
	String::size_type
	PutLine(const String&);

	/*!
	\brief 输出迭代器 s 指向字符串，直至区域末尾或字符串结束。
	\return 输出迭代器。
	*/
	template<typename _tOut>
	_tOut
	PutString(_tOut s);
	/*!
	\brief 输出迭代器 s 指向字符串，
		直至区域末尾、遇到指定迭代器 g 或遇到指定字符 f 。
	\return 输出迭代器。
	*/
	template<typename _tOut, typename _tChar>
	_tOut
	PutString(_tOut s, _tOut g , _tChar f = '\0');
	/*!
	\brief 输出字符串，直至区域末尾或字符串结束。
	\return 输出字符数。
	*/
	String::size_type
	PutString(const String&);
};

inline TextRegion&
TextRegion::operator=(const TextState& ts)
{
	TextState::operator=(ts);
	return *this;
}

template<typename _tOut>
_tOut
TextRegion::PutLine(_tOut s)
{
	const SPOS fpy(PenY);
	_tOut t(s);

	while(*t != 0 && fpy == PenY)
		if(!PutChar(*t))
			++t;
	return t;
}
template<typename _tOut, typename _tChar>
_tOut
TextRegion::PutLine(_tOut s, _tOut g, _tChar f)
{
	const SPOS fpy(PenY);

	while(s != g && *s != f && fpy == PenY)
		if(!PutChar(*s))
			++s;
	return s;
}

inline String::size_type
TextRegion::PutLine(const String& s)
{
	return PutLine(s.c_str()) - s.c_str();
}

template<typename _tOut>
_tOut
TextRegion::PutString(_tOut s)
{
	const SPOS mpy(GetLineLast());
	_tOut t(s);

	while(*t != 0 && PenY <= mpy)
		if(!PutChar(*t))
			++t;
	return t;
}
template<typename _tOut, typename _tChar>
_tOut
TextRegion::PutString(_tOut s, _tOut g, _tChar f)
{
	const SPOS mpy(GetLineLast());

	while(s != g && *s != f && PenY <= mpy)
		if(!PutChar(*s))
			++s;
	return s;
}
inline String::size_type
TextRegion::PutString(const String& s)
{
	return PutString(s.c_str()) - s.c_str();
}


/*!
\brief 绘制文本。
*/
void
DrawText(TextRegion&, const Graphics&, const Point&, const Size&,
	const String&);


YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

/*!
\brief 以 cache 为字体缓存，width 为宽度，
	从当前文本迭代器 p 开始逆向查找字符 f 。
\note 不含 p ；满足 p != --g 。
*/
template<typename _tOut, typename _tChar>
_tOut
rfind(YFontCache& cache, SDST width, _tOut p, _tOut g, _tChar f)
{
	if(p != g)
	{
		SDST w(0);
		_tChar c(0);

		while(--p != g && (c = *p, c != f && !(std::iswprint(c) && (w += cache.GetAdvance(c)) > width)))
			;
	}
	return p;
}

/*!
\brief 在 r 中取当前文本迭代器 p 的前 l 行首对应文本迭代器。
\note 满足 p != --g 。
*/
template<typename _tOut>
_tOut
GetPreviousLinePtr(const Drawing::TextRegion& r, _tOut p, _tOut g, u16 l = 1)
{
	while(l-- != 0 && p != g)
	{
		p = rfind<_tOut, uchar_t>(r.GetCache(), r.PenX - r.Margin.Left, p, g, '\n');
		if(p != g)
		{
			p = rfind<_tOut, uchar_t>(r.GetCache(), r.GetBufWidthN(), p, g, '\n');
			if(p != g)
				++p;
		}
	}
	return p;
}

/*!
\brief 在 r 中取当前文本迭代器 p 至后一行首对应文本迭代器。
\note 满足 p != g 。
*/
template<typename _tOut>
_tOut
GetNextLinePtr(const Drawing::TextRegion& r, _tOut p, _tOut g)
{
	if(p == g)
		return p;

	YFontCache& cache(r.GetCache());
	SDST nw(r.GetBufWidthN());
	SDST w(r.PenX - r.Margin.Left);

	while(p != g)
	{
		uchar_t c(*p);
		++p;
		if(c == '\n' || (std::iswprint(c) && (w += cache.GetAdvance(c)) > nw))
			break;
	}
	return p;
}


/*!
\brief 从文本文件 f 中读取 n 字节到 txtbox 中。
\note 无文本缓冲方式；按默认编码转换为 UTF-16LE 。
*/
u32
ReadX(YTextFile& f, Drawing::TextRegion& txtbox, u32 n);

YSL_END_NAMESPACE(Text)

YSL_END

#endif

