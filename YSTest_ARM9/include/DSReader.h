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
\version r2485;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 14:03:47 +0800;
\par 修改时间:
	2011-10-14 14:32 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::DSReader;
*/


#ifndef INCLUDED_DSREADER_H_
#define INCLUDED_DSREADER_H_

#include <YSLib/Core/yapp.h>
#include <YSLib/Service/yftext.h>
#include <YSLib/UI/ydesktop.h>
#include <YSLib/Service/ytext.h>
#include <YSLib/Helper/yglobal.h>
#include <YSLib/UI/textarea.h>
#include <YSLib/Service/textmgr.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

YSL_BEGIN_NAMESPACE(Components)

using Drawing::Color;
using Drawing::PixelType;

class DualScreenReader
{
private:
	Text::TextFileBuffer* pText; //!< 文本资源。
	FontCache& fc; //!< 字体缓存。
	Drawing::Rotation rot; //!< 屏幕指向。
	Text::TextFileBuffer::Iterator iTop; //!< 字符区域顶端文本缓存输入迭代器。
	Text::TextFileBuffer::Iterator iBottom; //!< 字符区域底端文本缓存输入迭代器。

public:
	YSLib::Components::TextArea AreaUp; //!< 上屏幕对应字符区域。
	YSLib::Components::TextArea AreaDown; //!< 下屏幕对应字符区域。

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

	DefPredicate(TextTop, iTop == pText->cbegin()) \
		//!< 判断输出位置是否到文本顶端。
	DefPredicate(TextBottom, iBottom == pText->cend()) \
		//!< 判断输出位置是否到文本底端。

	DefGetter(u8, FontSize, fc.GetFontSize()) //!< 取字符区域的字体大小。
	DefGetter(Color, ColorUp, AreaUp.Color) \
		//!< 取上字符区域的字体颜色。
	DefGetter(Color, ColorDown, AreaDown.Color) \
		//!< 取下字符区域的字体颜色。
	DefGetter(u8, LineGapUp, AreaUp.LineGap) //!< 取上字符区域的行距。
	DefGetter(u8, LineGapDown, AreaDown.LineGap) \
		//!< 取下字符区域的行距。
	DefGetter(Color, Color, GetColorUp()) //!< 取字符区域的字体颜色。
	DefGetter(u8, LineGap, GetLineGapUp()) //!< 取字符区域的行距。
	DefGetter(Text::Encoding, Encoding, pText ? pText->GetEncoding()
		: Text::CharSet::Null) //!< 取编码。

	PDefH1(void, SetColor, Color c = Drawing::ColorSpace::Black)
		ImplRet(static_cast<void>(yunsequenced(AreaUp.Color = c,
		AreaDown.Color = c))) //!< 设置字符颜色。
	PDefH1(void, SetFontSize, Drawing::Font::SizeType s
		= Drawing::Font::DefaultSize)
		ImplRet(static_cast<void>(fc.SetFontSize(s))) \
		//!< 设置字符区域字体大小。
	PDefH1(void, SetLineGap, u8 g = 0)
		ImplRet(static_cast<void>(yunsequenced(AreaUp.LineGap = g,
		AreaDown.LineGap = g))) //!< 设置行距。

	//设置笔的行位置。
	//void
	//SetCurrentTextLineNTo(u8);

	//! \brief 无效化文本区域。
	void
	Invalidate();

	//! \brief 上移一行。
	bool
	LineUp();

	//! \brief 下移一行。
	bool
	LineDown();

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

	//! \brief 上移一屏。
	bool
	ScreenUp();

	//! \brief 下移一屏。
	bool
	ScreenDown();

	/*//!
	\brief 自动滚屏。
	\param pCheck 输入检测函数指针。
	*/
	//void
	//Scroll(Function<void()> pCheck);

	//! \brief 卸载文本。
	void
	UnloadText();

	//! \brief 更新缓冲区文本。
	void
	Update();
};

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

#endif

