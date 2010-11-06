// YSLib::Shell::YText by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 00:06:05 + 08:00;
// UTime = 2010-11-03 19:50 + 08:00;
// Version = 0.6612;


#ifndef INCLUDED_YTEXT_H_
#define INCLUDED_YTEXT_H_

// YText ：基础文本显示。

#include "../Core/ystring.h"
#include "../Core/yftext.h"
#include "ygdi.h"
#include <cwctype>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

//文本状态：笔样式、边框样式、字体缓存和行距。
struct TextState : public PenStyle
{
public:
	typedef PenStyle ParentType;

	Padding Margin; //显示区域到边框的距离。
	SPOS PenX, PenY; //笔坐标。
	u8 LineGap; //行距。

public:
	//********************************
	//名称:		TextState
	//全名:		YSLib::Drawing::TextState::TextState
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	TextState();
	//********************************
	//名称:		TextState
	//全名:		YSLib::Drawing::TextState::TextState
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	Drawing::Font &
	//功能概要:	构造：使用指定字体。
	//备注:		
	//********************************
	explicit
	TextState(Drawing::Font&);
	//********************************
	//名称:		TextState
	//全名:		YSLib::Drawing::TextState::TextState
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	YFontCache &
	//功能概要:	构造：使用指定字体缓存。
	//备注:		
	//********************************
	explicit
	TextState(YFontCache&);

	//********************************
	//名称:		operator=
	//全名:		YSLib::Drawing::TextState::operator=
	//可访问性:	public 
	//返回类型:	TextState&
	//修饰符:	
	//形式参数:	const PenStyle & ps
	//功能概要:	从笔样式中恢复样式。
	//备注:		
	//********************************
	TextState&
	operator=(const PenStyle& ps);
	//********************************
	//名称:		operator=
	//全名:		YSLib::Drawing::TextState::operator=
	//可访问性:	public 
	//返回类型:	TextState&
	//修饰符:	
	//形式参数:	const Padding & ms
	//功能概要:	从边距样式中恢复样式。
	//备注:		
	//********************************
	TextState&
	operator=(const Padding& ms);
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


//********************************
//名称:		GetLnHeightFrom
//全名:		YSLib::Drawing::GetLnHeightFrom
//可访问性:	public 
//返回类型:	YSLib::SDST
//修饰符:	
//形式参数:	const TextState & s
//功能概要:	在指定文本状态中取当前字体设置对应的行高。
//备注:		
//********************************
inline SDST
GetLnHeightFrom(const TextState& s)
{
	return s.GetCache().GetHeight();
}

//********************************
//名称:		GetLnHeightExFrom
//全名:		YSLib::Drawing::GetLnHeightExFrom
//可访问性:	public 
//返回类型:	YSLib::SDST
//修饰符:	
//形式参数:	const TextState & s
//功能概要:	在指定文本状态中取当前字体设置对应的行高与行距之和。
//备注:		
//********************************
inline SDST
GetLnHeightExFrom(const TextState& s)
{
	return s.GetCache().GetHeight() + s.LineGap;
}

//********************************
//名称:		GetLnNNowFrom
//全名:		YSLib::Drawing::GetLnNNowFrom
//可访问性:	public 
//返回类型:	u16
//修饰符:	
//形式参数:	const TextState & s
//功能概要:	取笔所在的当前行数。
//备注:		
//********************************
inline u16
GetLnNNowFrom(const TextState& s)
{
	return (s.PenY - s.Margin.Top) / GetLnHeightExFrom(s);
}

//********************************
//名称:		SetPen
//全名:		YSLib::Drawing::SetPen
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	TextState &
//功能概要:	按字体大小在设置笔的默认位置（区域左上角）。
//备注:		
//********************************
void
SetPensTo(TextState&);
//********************************
//名称:		SetPen
//全名:		YSLib::Drawing::SetPen
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	TextState & s
//形式参数:	SPOS x
//形式参数:	SPOS y
//功能概要:	设置笔位置。
//备注:		
//********************************
inline void
SetPensTo(TextState& s, SPOS x, SPOS y)
{
	s.PenX = x;
	s.PenY = y;
}

//********************************
//名称:		SetMarginsTo
//全名:		YSLib::Drawing::SetMarginsTo
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	TextState & s
//形式参数:	SDST l
//形式参数:	SDST r
//形式参数:	SDST t
//形式参数:	SDST b
//功能概要:	设置边距。
//备注:		4 个 16 位无符号整数形式。
//********************************
inline void
SetMarginsTo(TextState& s, SDST l, SDST r, SDST t, SDST b)
{
	SetAllTo(s.Margin, l, r, t, b);
	SetPensTo(s);
}
//********************************
//名称:		SetMarginsTo
//全名:		YSLib::Drawing::SetMarginsTo
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	TextState & s
//形式参数:	u64 m
//功能概要:	设置边距。
//备注:		64 位无符号整数形式。
//********************************
inline void
SetMarginsTo(TextState& s, u64 m)
{
	SetAllTo(s.Margin, m);
	SetPensTo(s);
}
//********************************
//名称:		SetMarginsTo
//全名:		YSLib::Drawing::SetMarginsTo
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	TextState & s
//形式参数:	SDST h
//形式参数:	SDST v
//功能概要:	设置边距。
//备注:		2 个 16 位无符号整数形式，分别表示水平边距和竖直边距。
//********************************
inline void
SetMarginsTo(TextState& s, SDST h, SDST v)
{
	SetAllTo(s.Margin, h, v);
	SetPensTo(s);
}

//********************************
//名称:		SetLnNNowTo
//全名:		YSLib::Drawing::SetLnNNowTo
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	TextState &
//形式参数:	u16
//功能概要:	设置笔的行位置。
//备注:		
//********************************
void
SetLnNNowTo(TextState&, u16);


//********************************
//名称:		PrintChar
//全名:		YSLib::Drawing::PrintChar
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapBuffer &
//形式参数:	TextState &
//形式参数:	fchar_t
//功能概要:	打印单个字符。
//备注:		
//********************************
void
PrintChar(BitmapBuffer&, TextState&, fchar_t); 

//********************************
//名称:		PrintCharEx
//全名:		YSLib::Drawing::PrintCharEx
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapBufferEx &
//形式参数:	TextState &
//形式参数:	fchar_t
//功能概要:	打印单个字符。
//备注:		
//********************************
void
PrintCharEx(BitmapBufferEx&, TextState&, fchar_t); 


//文本区域。
class TextRegion : public TextState, public BitmapBufferEx
{
public:
	typedef TextState ParentType;

	//********************************
	//名称:		TextRegion
	//全名:		YSLib::Drawing::TextRegion::TextRegion
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	TextRegion();
	//********************************
	//名称:		TextRegion
	//全名:		YSLib::Drawing::TextRegion::TextRegion
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	Drawing::Font &
	//功能概要:	构造：使用指定字体。
	//备注:		
	//********************************
	explicit
	TextRegion(Drawing::Font&);
	//********************************
	//名称:		TextRegion
	//全名:		YSLib::Drawing::TextRegion::TextRegion
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	YFontCache &
	//功能概要:	构造：使用指定字体缓存。
	//备注:		
	//********************************
	explicit
	TextRegion(YFontCache&);
	//********************************
	//名称:		~TextRegion
	//全名:		YSLib::Drawing::TextRegion::~TextRegion
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	
	//功能概要:	析构。
	//备注:		
	//********************************
	virtual
	~TextRegion();

	//********************************
	//名称:		operator=
	//全名:		YSLib::Drawing::TextRegion::operator=
	//可访问性:	public 
	//返回类型:	TextRegion&
	//修饰符:	
	//形式参数:	const TextState & ts
	//功能概要:	从文本状态中恢复状态。
	//备注:		
	//********************************
	TextRegion& operator=(const TextState& ts);

	DefGetter(SDST, BufWidthN, Width - GetHorizontalFrom(Margin)) \
		//取缓冲区的文本显示区域的宽。
	DefGetter(SDST, BufHeightN, Height - GetVerticalFrom(Margin)) \
		//取缓冲区的文本显示区域的高。
	//********************************
	//名称:		GetMarginResized
	//全名:		YSLib::Drawing::TextRegion::GetMarginResized
	//可访问性:	public 
	//返回类型:	YSLib::SDST
	//修饰符:	const
	//功能概要:	根据字体大小、行距和缓冲区的高调整边距，返回调整后的底边距值。
	//备注:		
	//********************************
	SDST
	GetMarginResized() const;
	//********************************
	//名称:		GetBufferHeightResized
	//全名:		YSLib::Drawing::TextRegion::GetBufferHeightResized
	//可访问性:	public 
	//返回类型:	YSLib::SDST
	//修饰符:	const
	//功能概要:	取根据字体大小和行距调整后的缓冲区的高。
	//备注:		不含底边距。
	//********************************
	SDST
	GetBufferHeightResized() const;
	//********************************
	//名称:		GetLnN
	//全名:		YSLib::Drawing::TextRegion::GetLnN
	//可访问性:	public 
	//返回类型:	u16
	//修饰符:	const
	//功能概要:	取按当前行高和行距所能显示的最大行数。
	//备注:		
	//********************************
	u16
	GetLnN() const;
	//********************************
	//名称:		GetLnNEx
	//全名:		YSLib::Drawing::TextRegion::GetLnNEx
	//可访问性:	public 
	//返回类型:	u16
	//修饰符:	const
	//功能概要:	取按当前行高和行距（行间距数小于行数 1）所能显示的最大行数。
	//备注:		
	//********************************
	u16
	GetLnNEx() const;
	//********************************
	//名称:		GetLineLast
	//全名:		YSLib::Drawing::TextRegion::GetLineLast
	//可访问性:	public 
	//返回类型:	YSLib::SPOS
	//修饰符:	const
	//功能概要:	取最底行的基线位置。
	//备注:		
	//********************************
	SPOS
	GetLineLast() const;

	//********************************
	//名称:		SetLnLast
	//全名:		YSLib::Drawing::TextRegion::SetLnLast
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	设置笔的行位置为最底行。
	//备注:		
	//********************************
	void
	SetLnLast();

	//********************************
	//名称:		ClearLine
	//全名:		YSLib::Drawing::TextRegion::ClearLine
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	u16 l
	//形式参数:	SDST n
	//功能概要:	清除缓冲区第 l 行起始的 n 行像素。
	//备注:		n 为 0 时清除之后的所有行。
	//********************************
	void
	ClearLine(u16 l, SDST n);

	//清除缓冲区第 l 个文本行。
	//********************************
	//名称:		ClearLn
	//全名:		YSLib::Drawing::TextRegion::ClearLn
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	u16 l
	//功能概要:	清除缓冲区中的第 l 个文本行。
	//备注:		l 为 0 表示首行。
	//********************************
	void
	ClearLn(u16 l);

	//
	//********************************
	//名称:		ClearLnLast
	//全名:		YSLib::Drawing::TextRegion::ClearLnLast
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除缓冲区中的最后一个文本行。
	//备注:		
	//********************************
	void
	ClearLnLast();

	//********************************
	//名称:		Move
	//全名:		YSLib::Drawing::TextRegion::Move
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	s16 n
	//功能概要:	缓冲区特效：整体移动 n 像素。
	//备注:		除上下边界区域；n > 0 时下移， n < 0 时上移。
	//********************************
	void
	Move(s16 n);
	//********************************
	//名称:		Move
	//全名:		YSLib::Drawing::TextRegion::Move
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	s16 n
	//形式参数:	SDST h
	//功能概要:	缓冲区特效：整体移动 n 像素。
	//备注:		从缓冲区顶端起高 h 的区域内，除上下边界区域；
	//			n > 0 时下移， n < 0 时上移。
	//********************************
	void
	Move(s16 n, SDST h);

	//********************************
	//名称:		PutNewline
	//全名:		YSLib::Drawing::TextRegion::PutNewline
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	输出换行。
	//备注:		
	//********************************
	void
	PutNewline();

	//********************************
	//名称:		PutChar
	//全名:		YSLib::Drawing::TextRegion::PutChar
	//可访问性:	public 
	//返回类型:	u8
	//修饰符:	
	//形式参数:	fchar_t
	//功能概要:	输出单个字符。
	//备注:		
	//********************************
	u8
	PutChar(fchar_t);

	//********************************
	//名称:		PutLine
	//全名:		YSLib::Drawing::TextRegion::PutLine<_tOut>
	//可访问性:	public 
	//返回类型:	_tOut
	//修饰符:	
	//形式参数:	_tOut s
	//功能概要:	输出迭代器 s 指向字符串，直至行尾或字符串结束，
	//			并返回输出迭代器。
	//备注:		
	//********************************
	template<typename _tOut>
	_tOut
	PutLine(_tOut s);
	//********************************
	//名称:		PutLine
	//全名:		YSLib::Drawing::TextRegion::PutLine<_tOut, _tChar>
	//可访问性:	public 
	//返回类型:	_tOut
	//修饰符:	
	//形式参数:	_tOut s
	//形式参数:	_tOut g
	//形式参数:	_tChar f
	//功能概要:	输出迭代器 s 指向字符串，
	//			直至行尾、遇到指定迭代器 g 或遇到指定字符 f ，
	//			并返回输出迭代器。
	//备注:		
	//********************************
	template<typename _tOut, typename _tChar>
	_tOut
	PutLine(_tOut s, _tOut g, _tChar f = '\0');
	//********************************
	//名称:		PutLine
	//全名:		YSLib::Drawing::TextRegion::PutLine
	//可访问性:	public 
	//返回类型:	String::size_type
	//修饰符:	
	//形式参数:	const String &
	//功能概要:	输出字符串，直至行尾或字符串结束，并返回输出字符数。
	//备注:		
	//********************************
	String::size_type
	PutLine(const String&);

	//********************************
	//名称:		PutString
	//全名:		YSLib::Drawing::TextRegion::PutString<_tOut>
	//可访问性:	public 
	//返回类型:	_tOut
	//修饰符:	
	//形式参数:	_tOut s
	//功能概要:	输出迭代器 s 指向字符串，直至区域末尾或字符串结束，
	//			并返回输出迭代器。
	//备注:		
	//********************************
	template<typename _tOut>
	_tOut
	PutString(_tOut s);
	//********************************
	//名称:		PutString
	//全名:		YSLib::Drawing::TextRegion::PutString<_tOut, _tChar>
	//可访问性:	public 
	//返回类型:	_tOut
	//修饰符:	
	//形式参数:	_tOut s
	//形式参数:	_tOut g
	//形式参数:	_tChar f
	//功能概要:	输出迭代器 s 指向字符串，
	//			直至区域末尾、遇到指定迭代器 g 或遇到指定字符 f ，
	//			并返回输出迭代器。
	//备注:		
	//********************************
	template<typename _tOut, typename _tChar>
	_tOut
	PutString(_tOut s, _tOut g , _tChar f = '\0');
	//********************************
	//名称:		PutString
	//全名:		YSLib::Drawing::TextRegion::PutString
	//可访问性:	public 
	//返回类型:	String::size_type
	//修饰符:	
	//形式参数:	const String &
	//功能概要:	输出字符串，直至区域末尾或字符串结束，并返回输出字符数。
	//备注:		
	//********************************
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

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

//********************************
//名称:		rfind
//全名:		YSLib::Text::rfind<_tOut, _tChar>
//可访问性:	public 
//返回类型:	_tOut
//修饰符:	
//形式参数:	YFontCache & cache
//形式参数:	SDST width
//形式参数:	_tOut p
//形式参数:	_tOut g
//形式参数:	_tChar f
//功能概要:	以 cache 为字体缓存，width 为宽度，
//			从当前文本迭代器 p 开始逆向查找字符 f 。
//备注:		不含 p ；满足 p != --g 。
//********************************
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

//********************************
//名称:		GetPreviousLinePtr
//全名:		YSLib::Text::GetPreviousLinePtr<_tOut>
//可访问性:	public 
//返回类型:	_tOut
//修饰符:	
//形式参数:	const Drawing::TextRegion & r
//形式参数:	_tOut p
//形式参数:	_tOut g
//形式参数:	u16 l
//功能概要:	在 r 中取当前文本迭代器 p 的前 l 行首对应文本迭代器。
//备注:		满足 p != --g 。
//********************************
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

//********************************
//名称:		GetNextLinePtr
//全名:		YSLib::Text::GetNextLinePtr<_tOut>
//可访问性:	public 
//返回类型:	_tOut
//修饰符:	
//形式参数:	const Drawing::TextRegion & r
//形式参数:	_tOut p
//形式参数:	_tOut g
//功能概要:	在 r 中取当前文本迭代器 p 至后一行首对应文本迭代器。
//备注:		满足 p != g 。
//********************************
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


//********************************
//名称:		ReadX
//全名:		YSLib::Text::ReadX
//可访问性:	public 
//返回类型:	u32
//修饰符:	
//形式参数:	YTextFile & f
//形式参数:	Drawing::TextRegion & txtbox
//形式参数:	u32 n
//功能概要:	从文本文件 f 中读取 n 字节到 txtbox 中。
//备注:		无文本缓冲方式；按默认编码转换为 UTF-16LE 。
//********************************
u32
ReadX(YTextFile& f, Drawing::TextRegion& txtbox, u32 n);

YSL_END_NAMESPACE(Text)

YSL_END

#endif

