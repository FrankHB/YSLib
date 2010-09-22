// YSLib::Shell::YText by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 0:06:05;
// UTime = 2010-9-22 4:16;
// Version = 0.6326;


#ifndef INCLUDED_YTEXT_H_
#define INCLUDED_YTEXT_H_

// YText ：基础文本显示。

#include "../Core/ystring.h"
#include "../Core/yftext.h"
#include "ygdi.h"
#include <cwctype>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

class TextState : public PenStyle //文本状态：笔样式、边框样式、字体缓存和行距。
{
public:
	typedef PenStyle ParentType;

	Padding Margin; //显示区域到边框的距离。

protected:
	SPOS penX, penY; //笔坐标。
	u8 lnGap; //行距。

public:
	TextState();
	explicit
	TextState(Drawing::Font&);
	explicit
	TextState(YFontCache&);

	TextState&
	operator=(const PenStyle& ps); //从 ts 中恢复样式。
	TextState&
	operator=(const Padding& ms); //从 ms 中恢复样式。

	DefGetter(SPOS, PenX, penX)
	DefGetter(SPOS, PenY, penY)
	DefGetter(u8, LineGap, lnGap) //取当前字体设置对应的行距。
	SDST
	GetLnHeight() const; //取当前字体设置对应的行高。
	SDST
	GetLnHeightEx() const; //取当前字体设置对应的行高与行距之和。
	u16
	GetLnNNow() const; //取笔所在的当前行数。

	void
	SetMargins(u64); //设置边距（64 位无符号整数形式）。
	void
	SetMargins(SDST, SDST); //设置边距（2 个 16 位无符号整数形式，分别表示水平边距和竖直边距）。
	void
	SetMargins(SDST, SDST, SDST, SDST); //设置边距（4 个 16 位无符号整数形式）。
	void
	SetPen(); //按字体大小在设置笔的默认位置（区域左上角）。
	void
	SetPen(SPOS, SPOS); //设置笔位置。
	DefSetter(u8, LineGap, lnGap) //设置行距。
	void
	SetLnNNow(u16 n); //设置笔的行位置。
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

inline void
TextState::SetMargins(u64 m)
{
	Margin.SetAll(m);
	SetPen();
}
inline void
TextState::SetMargins(SDST h, SDST v)
{
	Margin.SetAll(h, v);
	SetPen();
}
inline void
TextState::SetMargins(SDST l, SDST r, SDST t, SDST b)
{
	Margin.SetAll(l, r, t, b);
	SetPen();
}
inline void
TextState::SetPen()
{
	penX = Margin.Left;
	//	penY = Top + GetLnHeightEx();
	//	penY = Top + pCache->GetAscender();
	SetLnNNow(0);
}
inline void
TextState::SetPen(SPOS x, SPOS y)
{
	penX = x;
	penY = y;
}


class TextRegion : public TextState, public MBitmapBufferEx //文本区域。
{
public:
	typedef TextState ParentType;

private:
	void PrintChar(fchar_t); //打印单个字符。

public:
	TextRegion();
	explicit
	TextRegion(Drawing::Font&);
	explicit
	TextRegion(YFontCache&);
	~TextRegion();

	TextRegion& operator=(const TextState& ts); //从 ts 中恢复状态。

	DefGetter(SDST, BufWidthN, Width - Margin.GetHorizontal()) //取缓冲区的文本显示区域的宽。
	DefGetter(SDST, BufHeightN, Height - Margin.GetVertical()) //取缓冲区的文本显示区域的高。
	SDST
	GetMarginResized() const; //根据字体大小、行距和缓冲区的高调整边距，返回调整后的底边距值。
	SDST
	GetBufferHeightResized() const; //取根据字体大小和行距调整后的缓冲区的高（不含底边距）。
	u16
	GetLnN() const; //取按当前行高和行距所能显示的最大行数。
	u16
	GetLnNEx() const; //取按当前行高和行距（行间距数小于行数 1）所能显示的最大行数。
	SPOS
	GetLineLast() const; //取最底行的基线位置。

	void
	SetLnLast(); //设置笔的行位置为最底行。

	void
	ClearLine(u16 l, SDST n); //清除缓冲区第 l 行起始的 n 行像素（n 为 0 时清除之后的所有行）。
	void
	ClearLn(u16 l); //清除缓冲区第 l 个文本行。
	void
	ClearLnLast(); //清除缓冲区最后一个文本行。

	void
	Move(s16 n); //缓冲区（除上下边界区域）特效：整体移动 n 像素（n > 0 时下移， n < 0 时上移）。
	void
	Move(s16 n, SDST h); //缓冲区（从缓冲区顶端起高 h 的区域内，除上下边界区域）特效：整体移动 n 像素（n > 0 时下移， n < 0 时上移）。

	void
	PutNewline(); //输出换行。
	u8
	PutChar(fchar_t); //输出单个字符。

	template<typename _tOut>
	_tOut
	PutLine(_tOut s); //输出迭代器 s 指向字符串，直至行尾或字符串结束，并返回输出迭代器。
	template<typename _tOut, typename _tChar>
	_tOut
	PutLine(_tOut s, _tOut g, _tChar f = '\0'); //输出迭代器 s 指向字符串，直至行尾、遇到指定迭代器 g 或遇到指定字符 f ，并返回输出迭代器。
	String::size_type
	PutLine(const String&); //输出字符串，直至行尾或字符串结束，并返回输出字符数。

	template<typename _tOut>
	_tOut
	PutString(_tOut s); //输出迭代器 s 指向字符串，直至区域末尾或字符串结束，并返回输出迭代器。
	template<typename _tOut, typename _tChar>
	_tOut
	PutString(_tOut s, _tOut g , _tChar f = '\0'); //输出迭代器 s 指向字符串，直至区域末尾、遇到指定迭代器 g 或遇到指定字符 f ，并返回输出迭代器。
	String::size_type
	PutString(const String&); //输出字符串，直至区域末尾或字符串结束，并返回输出字符数。
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
	const SPOS fpy(penY);
	_tOut t(s);

	while(*t != 0 && fpy == penY)
		if(!PutChar(*t))
			++t;
	return t;
}
template<typename _tOut, typename _tChar>
_tOut
TextRegion::PutLine(_tOut s, _tOut g, _tChar f)
{
	const SPOS fpy(penY);

	while(s != g && *s != f && fpy == penY)
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

	while(*t != 0 && penY <= mpy)
		if(!PutChar(*t))
			++t;
	return t;
}
template<typename _tOut, typename _tChar>
_tOut
TextRegion::PutString(_tOut s, _tOut g, _tChar f)
{
	const SPOS mpy(GetLineLast());

	while(s != g && *s != f && penY <= mpy)
		if(!PutChar(*s))
			++s;
	return s;
}
inline String::size_type
TextRegion::PutString(const String& s)
{
	return PutString(s.c_str()) - s.c_str();
}

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

//以 cache 为字体缓存，width 为宽度，从当前文本迭代器 p （不含）开始逆向查找字符 f （满足 p != --g ）。
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

//在 r 中取当前文本迭代器 p 的前 l 行首对应文本迭代器（满足 p != --g ）。
template<typename _tOut>
_tOut
GetPreviousLinePtr(const Drawing::TextRegion& r, _tOut p, _tOut g, u16 l = 1)
{
	while(l-- != 0 && p != g)
	{
		p = rfind<_tOut, uchar_t>(r.GetCache(), r.GetPenX() - r.Margin.Left, p, g, '\n');
		if(p != g)
		{
			p = rfind<_tOut, uchar_t>(r.GetCache(), r.GetBufWidthN(), p, g, '\n');
			if(p != g)
				++p;
		}
	}
	return p;
}

//在 r 中取当前文本迭代器 p 至后一行首对应文本迭代器（满足 p != g ）。
template<typename _tOut>
_tOut
GetNextLinePtr(const Drawing::TextRegion& r, _tOut p, _tOut g)
{
	if(p == g)
		return p;

	YFontCache& cache(r.GetCache());
	SDST nw(r.GetBufWidthN());
	SDST w(r.GetPenX() - r.Margin.Left);

	while(p != g)
	{
		uchar_t c(*p);
		++p;
		if(c == '\n' || (std::iswprint(c) && (w += cache.GetAdvance(c)) > nw))
			break;
	}
	return p;
}


u32
ReadX(YTextFile& f, Drawing::TextRegion& txtbox, u32 n); //无文本缓冲方式从文本文件 f 中读取 n 字节（按默认编码转化为 UTF-16LE ）到 txtbox 中。

YSL_END_NAMESPACE(Text)

YSL_END

#endif

