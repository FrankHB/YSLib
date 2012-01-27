/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSReader.h
\ingroup YReader
\brief 适用于 DS 的双屏阅读器。
\version r2669;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 14:03:47 +0800;
\par 修改时间:
	2012-01-27 06:06 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::DSReader;
*/


#ifndef INCLUDED_DSREADER_H_
#define INCLUDED_DSREADER_H_

#include <YSLib/UI/textarea.h>
#include <YSLib/Service/yftext.h>
#include <YSLib/Service/textmgr.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

YSL_BEGIN_NAMESPACE(Components)

using Drawing::Color;
using Drawing::PixelType;


/*!
\brief 双屏阅读器。
\warning 非虚析构。
\since build 251 。
*/
class DualScreenReader
{
public:
	/*!
	\brief 命令类型。
	\since build 270 。
	*/
	typedef enum : u16
	{
		Null = 0,
		Scroll = 1,
		Up = 2,
		Down = 0,
		Line = 4,
		Screen = 0,

		LineUp = Line | Up,
		LineDown = Line | Down,
		ScreenUp = Screen | Up,
		ScreenDown = Screen | Down,

		LineUpScroll = LineUp | Scroll,
		LineDownScroll = LineDown | Scroll,
		ScreenUpScroll = ScreenUp | Scroll,
		ScreenDownScroll = ScreenDown | Scroll
	} Command;

private:
	unique_ptr<Text::TextFileBuffer> pText; //!< 文本资源。
	Drawing::FontCache& fc; //!< 字体缓存。
	/*!
	\brief 字符区域输入迭代器。
	
	字符区域的起点和终点在文本缓冲区的迭代器，构成一个左闭右开区间。
	\note 若因为读入换行符而换行，则迭代器指向的字符此换行符。
	\since build 273 。
	*/
	//@{
	Text::TextFileBuffer::Iterator i_top;
	Text::TextFileBuffer::Iterator i_btm;
	//@}
	/*!
	\brief 读入文件结束后的空行数。

	最近一次 UpdateView 操作时确认文件结束后的空行数。
	\since build 272 。
	*/
	u16 overread_line_n;

public:
	/*!
	\brief 公用边距。

	限制文本区域的文本边界的基准边距。
	\since build 274 。
	*/
	Drawing::Padding Margin;

private:
	/*!
	\brief 上下屏幕对应字符区域。
	\since build 273 。
	*/
	//@{
	YSL_ Components::BufferedTextArea area_up;
	YSL_ Components::BufferedTextArea area_dn;
	//@}

public:
	/*!
	\brief 视图变更回调函数。
	\since build 271 。
	*/
	std::function<void()> ViewChanged;

	/*!
	\brief 构造。
	\param w 字符区域宽。
	\param h_up 上字符区域高。
	\param h_down 下字符区域高。
	\param fc_ 字体缓存对象。
	*/
	DualScreenReader(SDst w = MainScreenWidth,
		SDst h_up = MainScreenHeight, SDst h_down = MainScreenHeight,
		Drawing::FontCache& fc_ = FetchGlobalInstance().GetFontCache());

	DefPred(const ynothrow, TextTop, i_top == pText->GetBegin()) \
		//!< 判断输出位置是否到文本顶端。
	DefPred(const ynothrow, TextBottom, i_btm == pText->GetEnd()) \
		//!< 判断输出位置是否到文本底端。

	DefGetter(const ynothrow, u8, FontSize, area_up.Font.GetSize()) \
		//!< 取字符区域的字体大小。
	/*!
	\brief 取字符区域的字体颜色。
	\note 上下字符区域的字体颜色应该保持一致，否则滚屏时字体颜色错误。
	\since build 279 。
	*/
	DefGetter(const ynothrow, Color, Color, area_up.Color) \
	DefGetter(const ynothrow, u8, LineGap, area_up.LineGap) \
		//!< 取字符区域的行距。
	DefGetter(const ynothrow, Text::Encoding, Encoding, pText
		? pText->GetEncoding() : Text::CharSet::Null) //!< 取编码。
	/*!
	\brief 取文本大小。
	\note 单位为字节。
	\since build 270 。
	*/
	DefGetter(const ynothrow, size_t, TextSize,
		pText ? pText->GetTextSize() : 0)
	/*!
	\brief 取阅读位置。

	取字符区域起始位置的输入迭代器相对于文本缓冲区迭代器的偏移。
	\note 单位为字节。
	\since build 272 。
	*/
	//@{
	DefGetter(const ynothrow, size_t, TopPosition,
		pText ? pText->GetPosition(i_top) : 0)
	DefGetter(const ynothrow, size_t, BottomPosition,
		pText ? pText->GetPosition(i_btm) : 0)
	//@}

	PDefH(void, SetColor, Color c = Drawing::ColorSpace::Black)
		ImplUnseq(area_up.Color = c, area_dn.Color = c) //!< 设置字符颜色。
	void
	SetFontSize(Drawing::FontSize = Drawing::Font::DefaultSize); \
		//!< 设置字符区域字体大小后调整边距。
	void
	SetLineGap(u8 = 0); //!< 设置行距。
	/*!
	\brief 设置文本区域可见性。
	\since build 275 。
	*/
	void
	SetVisible(bool = true);

	/*!
	\brief 调整边距：使用公用边距更新各文本显示区域的边距。
	\note 保持顶端和底端边距均衡。
	\since build 274 。
	*/
	void
	AdjustMargins();

	/*!
	\brief 附加到窗口。
	\since build 273 。
	*/
	void
	Attach(YSL_ Components::Window&, YSL_ Components::Window&);

	/*!
	\brief 从窗口分离。
	\since build 273 。
	*/
	void
	Detach();

	/*!
	\brief 执行阅读器命令。
	\since build 270 。
	*/
	bool
	Execute(Command);

	/*!
	\brief 无效化文本区域，并调用 ViewChanged （仅当非空）。
	\since build 233 。
	*/
	void
	Invalidate();

	/*!
	\brief 文本定位。

	以指定偏移量定位起始迭代器，若越界则忽略。
	\note 自动转至最近行首。
	\since build 271 。
	*/
	void
	Locate(size_t);

	//! \brief 载入文本。
	void
	LoadText(TextFile&);

	//! \brief 绘制上屏文本。
	void
	PrintTextUp(const Drawing::Graphics&);

	//! \brief 绘制下屏文本。
	void
	PrintTextDown(const Drawing::Graphics&);

	//! \brief 复位输出显示状态。
	void
	Reset();

	/*!
	\brief 伸缩：从最大值起向上调整（减少）下文字区域的高后更新视图。
	\note 高的最大值为 MainScreenHeight 。
	\note 调整后的高限制为 40 到 MainScreenHeight 的闭区间。
	\note 顶端文本迭代器保持不变，底端文本迭代器可能改变。
	*/
	void
	Stretch(SDst);

	/*//!
	\brief 自动滚屏。
	\param pCheck 输入检测函数指针。
	*/
	//void
	//Scroll(Function<void()> pCheck);

	//! \brief 卸载文本。
	void
	UnloadText();

	/*!
	\brief 更新视图。

	根据文本起点迭代器和当前视图状态重新填充缓冲区文本并无效化文本区域。
	\since build 270 。
	*/
	void
	UpdateView();
};

DefBitmaskOperations(DualScreenReader::Command, u16)

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

#endif

