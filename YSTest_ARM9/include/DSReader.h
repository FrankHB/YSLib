/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSReader.h
\ingroup YReader
\brief 适用于 DS 的双屏阅读器。
\version r2581;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 14:03:47 +0800;
\par 修改时间:
	2011-12-22 16:36 +0800;
\par 字符集:
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
	FontCache& fc; //!< 字体缓存。
	Drawing::Rotation rot; //!< 屏幕指向。
	/*!
	\brief 字符区域输入迭代器。
	
	字符区域的起点和终点在文本缓冲区的迭代器，构成一个左闭右开区间。
	\note 若因为读入换行符而换行，则迭代器指向的字符此换行符。
	*/
	//@{
	Text::TextFileBuffer::Iterator iTop;
	Text::TextFileBuffer::Iterator iBottom;
	//@}
	/*!
	\brief 下屏文字指示标记。

	最近一次 UpdateView 操作时确认下屏存在需要输出的字符。
	*/
	bool text_down;

public:
	YSL_ Components::BufferedTextArea AreaUp; //!< 上屏幕对应字符区域。
	YSL_ Components::BufferedTextArea AreaDown; //!< 下屏幕对应字符区域。
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
		FontCache& fc_ = FetchGlobalInstance().GetFontCache());

	DefPred(const ynothrow, TextTop, iTop == pText->cbegin()) \
		//!< 判断输出位置是否到文本顶端。
	DefPred(const ynothrow, TextBottom, iBottom == pText->cend()) \
		//!< 判断输出位置是否到文本底端。

	DefGetter(const ynothrow, u8, FontSize, fc.GetFontSize()) \
		//!< 取字符区域的字体大小。
	DefGetter(const ynothrow, Color, ColorUp, AreaUp.Color) \
		//!< 取上字符区域的字体颜色。
	DefGetter(const ynothrow, Color, ColorDown, AreaDown.Color) \
		//!< 取下字符区域的字体颜色。
	DefGetter(const ynothrow, u8, LineGap, AreaUp.LineGap) \
		//!< 取字符区域的行距。
	DefGetter(const ynothrow, Text::Encoding, Encoding, pText
		? pText->GetEncoding() : Text::CharSet::Null) //!< 取编码。
	/*!
	\brief 取文本大小。
	\note 单位为字节。
	\since build 270 。
	*/
	DefGetter(const ynothrow, size_t, TextSize, pText ? pText->size() : 0)
	/*!
	\brief 取阅读位置。

	取字符区域起始位置的输入迭代器相对于文本缓冲区起始迭代器的偏移。
	\note 单位为字节。
	\since build 271 。
	*/
	DefGetter(const ynothrow, size_t, Position,
		pText ? iTop - pText->cbegin() : 0)

	PDefH(void, SetColor, Color c = Drawing::ColorSpace::Black)
		ImplUnseq(AreaUp.Color = c, AreaDown.Color = c) //!< 设置字符颜色。
	PDefH(void, SetFontSize, Drawing::Font::SizeType s
		= Drawing::Font::DefaultSize)
		ImplExpr(fc.SetFontSize(s)) //!< 设置字符区域字体大小。
	PDefH(void, SetLineGap, u8 g = 0)
		ImplUnseq(AreaUp.LineGap = g, AreaDown.LineGap = g) //!< 设置行距。

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

	//! \brief 复位输出状态。
	void
	Reset();

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

