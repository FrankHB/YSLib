// YSLib::Shell::YText by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 0:06:05;
// UTime = 2010-7-20 15:50;
// Version = 0.5945;


#ifndef INCLUDED_YTEXT_H_
#define INCLUDED_YTEXT_H_

// YText ：基础文本显示。

#include "../Core/ystring.h"
#include "../Core/yftext.h"
#include "ygdi.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

class YTextState : public YPenStyle //文本状态：笔样式、边框样式、字体缓存和行距。
{
public:
	typedef YPenStyle ParentType;

	MPadding Margin; //显示区域到边框的距离。

protected:
	YFontCache* pCache; //字体缓存。
	SPOS penX, penY; //笔坐标。
	u8 lnGap; //行距。

public:
	YTextState();
	explicit
	YTextState(MFont&);
	explicit
	YTextState(YFontCache*);

	YTextState&
	operator=(const YPenStyle& ps); //从 ts 中恢复样式。
	YTextState&
	operator=(const MPadding& ms); //从 ms 中恢复样式。
	YTextState&
	operator=(const YTextState& ms); //从 ts 中恢复状态。

	SPOS
	GetPenX() const;
	SPOS
	GetPenY() const;
	u8
	GetLineGap() const; //取当前字体设置对应的行距。
	SDST
	GetLnHeight() const; //取当前字体设置对应的行高。
	SDST
	GetLnHeightEx() const; //取当前字体设置对应的行高与行距之和。
	u16
	GetLnNNow() const; //取笔所在的当前行数。

//	MTypeface*
//	SetFont(const char*, CPATH); //设置字体。
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
	void
	SetLineGap(u8);
	void
	SetLnNNow(u16 n); //设置笔的行位置。
};

inline YTextState&
YTextState::operator=(const YPenStyle& ps)
{
	YPenStyle::operator=(ps);
	pCache = &GetFontFamily().Cache;
	return *this;
}
inline YTextState&
YTextState::operator=(const MPadding& ms)
{
	Margin = ms;
	return *this;
}

inline SPOS
YTextState::GetPenX() const
{
	return penX;
}
inline SPOS
YTextState::GetPenY() const
{
	return penY;
}
inline u8
YTextState::GetLineGap() const
{
	return lnGap;
}

inline void
YTextState::SetMargins(u64 m)
{
	Margin.SetAll(m);
	SetPen();
}
inline void
YTextState::SetMargins(SDST h, SDST v)
{
	Margin.SetAll(h, v);
	SetPen();
}
inline void
YTextState::SetMargins(SDST l, SDST r, SDST t, SDST b)
{
	Margin.SetAll(l, r, t, b);
	SetPen();
}
inline void
YTextState::SetPen()
{
	penX = Margin.Left;
	//	penY = Top + GetLnHeightEx();
	//	penY = Top + pCache->GetAscender();
	SetLnNNow(0);
}
inline void
YTextState::SetPen(SPOS x, SPOS y)
{
	penX = x;
	penY = y;
}
inline void
YTextState::SetLineGap(u8 g)
{
	lnGap = g;
}


class YTextRegion : public YTextState, public MBitmapBufferEx //文本区域。
{
public:
	typedef YTextState ParentType;

private:
	void PrintChar(u32); //打印单个字符。

public:
	YTextRegion();
	explicit
	YTextRegion(MFont&);
	explicit
	YTextRegion(YFontCache*);
	~YTextRegion();

	YTextRegion& operator=(const YTextState& ts); //从 ts 中恢复状态。

	SDST
	GetBufWidthN() const; //取缓冲区的文本显示区域的宽。
	SDST
	GetBufHeightN() const; //取缓冲区的文本显示区域的高。
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
	u32
	GetPrevLnOff(const uchar_t* s, u32 n) const; //取前一行首对应文本指针至当前文本指针 &s[n] 的字符数。
	u32
	GetPrevLnOff(const uchar_t* s, u32 n, u16 l) const; //取前 l 行首对应文本指针至当前文本指针 &s[n] 的字符数。
	u32
	GetNextLnOff(const uchar_t* s, u32 n) const; //取当前文本指针 &s[n] 至后一行首对应文本指针的字符数。
//	u32
//	GetNextLnOff(const uchar_t* s, u32 n, u16 l) const; //取当前文本指针 &s[n] 至后 l 行首对应文本指针的字符数。

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
	PutChar(u32); //输出单个字符。

	//输出字符串，直至行尾或字符串结束，并返回输出字符数。
	template<class _CharT>
	u32
	PutLine(const _CharT*);
	//u32
	//PutLine(const uchar_t*);
	u32
	PutLine(const YString& s);

	//输出字符串，直至区域末尾或字符串结束，并返回输出字符数。
	template<class _CharT>
	u32
	PutString(const _CharT*);
	//u32
	//PutString(const uchar_t*);
};

inline YTextRegion&
YTextRegion::operator=(const YTextState& ts)
{
	YTextState::operator=(ts);
	return *this;
}

inline SDST
YTextRegion::GetBufWidthN() const
{
	return Width - Margin.GetHorizontal();
}
inline SDST
YTextRegion::GetBufHeightN() const
{
	return Height - Margin.GetVertical();
}

template<class _CharT>
u32
YTextRegion::PutLine(const _CharT* s)
{
	SPOS fpy(penY);
	const _CharT* t(s);

	while(*t && fpy == penY)
		if(!PutChar(*t))
			++t;
	return t - s;
}
inline u32
YTextRegion::PutLine(const YString& s)
{
	return PutLine(s.c_str());
}

template<class _CharT>
u32
YTextRegion::PutString(const _CharT* s)
{
	SPOS mpy(GetLineLast());
	const _CharT* t(s);

	while(*t && penY <= mpy)
		if(!PutChar(*t))
			++t;
	return t - s;
}

u32
ReadX(YTextFile& f, YTextRegion& txtbox, u32 n); //无文本缓冲方式从文本文件 f 中读取 n 字节（按默认编码转化为 UTF-16LE）到 txtbox 中。

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

