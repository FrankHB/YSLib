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
\version 0.2340;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 14:03:47 +0800; 
\par 修改时间:
	2011-04-22 21:55 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::DSReader;
*/


#ifndef INCLUDED_DSREADER_H_
#define INCLUDED_DSREADER_H_

#include <YSLib/Core/yapp.h>
#include <YSLib/Core/yftext.h>
#include <YSLib/Shell/ydesktop.h>
#include <YSLib/Shell/ytext.h>
#include <YSLib/Helper/yglobal.h> 
#include <YSLib/Service/ytmgr.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

YSL_BEGIN_NAMESPACE(Components)

class BlockedText
{
public:
	YTextFile& File; //!< 文本文件对象。
	Text::TextFileBuffer Blocks; //!< 文本缓存映射。

	explicit
	BlockedText(YTextFile&);
};


class MDualScreenReader
{
private:
	BlockedText* pText; //!< 文本资源。
	YFontCache& fc; //!< 字体缓存。
	GHandle<Drawing::TextRegion> pTextRegionUp; //!< 上屏幕对应字符区域。
	GHandle<Drawing::TextRegion> pTextRegionDown; //!< 下屏幕对应字符区域。
	Drawing::Rotation rot; //!< 屏幕指向。
	Text::TextFileBuffer::HText itUp; //!< 字符区域读取文本缓存迭代器。
	Text::TextFileBuffer::HText itDn; //!< 字符区域读取文本缓存迭代器。
	u8 lnHeight; //!< 行高。

public:
	/*!
	\brief 构造。
	\param w 字符区域宽。
	\param h_up 上字符区域高。
	\param h_down 下字符区域高。
	\param fc_ 字体缓存对象。
	*/
	MDualScreenReader(SDst w = Global::MainScreenWidth,
		SDst h_up = Global::MainScreenHeight,
		SDst h_down = Global::MainScreenHeight,
		YFontCache& fc_ = GetApp().GetFontCache());

	bool IsTextTop(); //!< 判断输出位置是否到文本顶端。	
	bool IsTextBottom(); //!< 判断输出位置是否到文本底端。

	DefGetter(u8, FontSize, fc.GetFontSize()) //!< 取字符区域的字体大小。
	DefGetter(Drawing::TextRegion&, Up, *pTextRegionUp) \
		//!< 取上字符区域的引用。
	DefGetter(Drawing::TextRegion&, Dn, *pTextRegionDown) \
		//!< 取下字符区域的引用。
	DefGetter(Color, ColorUp, pTextRegionUp->Color) \
		//!< 取上字符区域的字体颜色。
	DefGetter(Color, ColorDown, pTextRegionDown->Color) \
		//!< 取下字符区域的字体颜色。
	DefGetter(u8, LineGapUp, pTextRegionUp->LineGap) //!< 取上字符区域的行距。
	DefGetter(u8, LineGapDown, pTextRegionDown->LineGap) \
		//!< 取下字符区域的行距。
	DefGetter(Color, Color, GetColorUp()) //!< 取字符区域的字体颜色。
	DefGetter(u8, LineGap, GetLineGapUp()) //!< 取字符区域的行距。

private:
	DefSetterDe(PixelType, ColorUp, pTextRegionUp->Color, 0) \
		//!< 设置上字符区域的字体颜色。
	DefSetterDe(PixelType, ColorDown, pTextRegionDown->Color, 0) \
		//!< 设置下字符区域的字体颜色。
	DefSetterDe(u8, LineGapUp, pTextRegionUp->LineGap, 0) \
		//!< 设置上字符区域的行距。
	DefSetterDe(u8, LineGapDown, pTextRegionDown->LineGap, 0) \
		//!< 设置下字符区域的行距。

public:
	void
	SetFontSize(Drawing::Font::SizeType = Drawing::Font::DefaultSize); \
		//!< 设置字符区域字体大小。
	void
	SetColor(Color = Drawing::ColorSpace::Black); //!< 设置字符颜色。
	void
	SetLineGap(u8 = 0); //!< 设置行距。

	//设置笔的行位置。
	//void
	//SetLnNNowTo(u8);

private:
	//! \brief 清除字符区域缓冲区。
	void Clear()
	{
		pTextRegionUp->ClearImage();
		pTextRegionDown->ClearImage();
	}

	//! \brief 复位缓存区域写入位置。
	void ResetPen()
	{
		pTextRegionUp->ResetPen();
		pTextRegionDown->ResetPen();
	}

	//! \brief 文本填充：输出文本缓冲区字符串，并返回填充字符数。
	void FillText();

public:
	//! \brief 复位输出状态。
	void
	Reset();

	//! \brief 载入文本。
	void
	LoadText(YTextFile&);

	//! \brief 卸载文本。
	void
	UnloadText();

	//! \brief 绘制文本。
	void
	PrintText(const Drawing::Graphics&, const Drawing::Graphics&);

	//! \brief 更新缓冲区文本。
	void
	Update();

	//! \brief 上移一行。
	bool
	LineUp();

	//! \brief 下移一行。
	bool
	LineDown();

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
};

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

#endif

