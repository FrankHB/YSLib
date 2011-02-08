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
\brief 适用于 DS 的双屏阅读器实现。
\version 0.2278;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 14:03:47 + 08:00; 
\par 修改时间:
	2011-01-31 19:16 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::DSReader;
*/


#ifndef INCLUDED_DSREADER_H_
#define INCLUDED_DSREADER_H_

#include <YSLib/Helper/shlds.h>

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
	SDST left; //!< 上下字符区域距离屏幕左边距离。
	SDST top_up; //!< 上字符区域距离上屏顶端距离。
	SDST top_down; //!< 下字符区域距离下屏顶端距离。
	PixelType* pBgUp; //!< 上屏幕背景层显存地址。
	PixelType* pBgDn; //!< 下屏幕背景层显存地址。
	GHStrong<TextRegion> pTextRegionUp; //!< 上屏幕对应字符区域。
	GHStrong<TextRegion> pTextRegionDn; //!< 下屏幕对应字符区域。
	Rotation rot; //!< 屏幕指向。
	Text::TextFileBuffer::HText itUp; //!< 字符区域读取文本缓存迭代器。
	Text::TextFileBuffer::HText itDn; //!< 字符区域读取文本缓存迭代器。
	u8 lnHeight; //!< 行高。

	DefGetter(Color, ColorUp, pTextRegionUp->Color) //!< 取上字符区域的字体颜色。
	DefGetter(Color, ColorDn, pTextRegionDn->Color) //!< 取下字符区域的字体颜色。
	DefGetter(u8, LineGapUp, pTextRegionUp->LineGap) //!< 取上字符区域的行距。
	DefGetter(u8, LineGapDn, pTextRegionDn->LineGap) //!< 取下字符区域的行距。

	DefSetterDe(PixelType, ColorUp, pTextRegionUp->Color, 0) \
		//!< 设置上字符区域的字体颜色。
	DefSetterDe(PixelType, ColorDn, pTextRegionDn->Color, 0) \
		//!< 设置下字符区域的字体颜色。
	DefSetterDe(u8, LineGapUp, pTextRegionUp->LineGap, 0) //!< 设置上字符区域的行距。
	DefSetterDe(u8, LineGapDn, pTextRegionDn->LineGap, 0) //!< 设置下字符区域的行距。

	//! \brief 清除字符区域缓冲区。
	void Clear()
	{
		pTextRegionUp->ClearImage();
		pTextRegionDn->ClearImage();
	}

	//! \brief 复位缓存区域写入位置。
	void ResetPen()
	{
		pTextRegionUp->ResetPen();
		pTextRegionDn->ResetPen();
	}

	//! \brief 文本填充：输出文本缓冲区字符串，并返回填充字符数。
	void FillText();

public:
	/*!
	\brief 构造函数。
	\param l 距离屏幕左边距离。
	\param w 字符区域宽。
	\param t_up 上字符区域距离上屏顶端距离。
	\param h_up 上字符区域高。
	\param t_down 下字符区域距离下屏顶端距离。
	\param h_down 下字符区域高。
	\param fc_ 字体缓存对象引用。
	*/
	MDualScreenReader(SDST l = 0, SDST w = SCRW,
		SDST t_up = 0, SDST h_up = SCRH, SDST t_down = 0, SDST h_down = SCRH,
		YFontCache& fc_ = *theApp.pFontCache);

	bool IsTextTop(); //!< 判断输出位置是否到文本顶端。	
	bool IsTextBottom(); //!< 判断输出位置是否到文本底端。

	DefGetter(u8, FontSize, fc.GetFontSize()) //!< 取字符区域的字体大小。
	DefGetter(TextRegion&, Up, *pTextRegionUp) //!< 取上字符区域的引用。
	DefGetter(TextRegion&, Dn, *pTextRegionDn) //!< 取下字符区域的引用。
	DefGetter(Color, Color, GetColorUp()) //!< 取字符区域的字体颜色。
	DefGetter(u8, LineGap, GetLineGapUp()) //!< 取字符区域的行距。

	DefSetterDe(SDST, Left, left, 0) //!< 设置字符区域距离屏幕左边距离。
	void
	SetFontSize(Font::SizeType = Font::DefSize); //!< 设置字符区域字体大小。
	void
	SetColor(Color = Drawing::ColorSpace::Black); //!< 设置字符颜色。
	void
	SetLineGap(u8 = 0); //!< 设置行距。

	//设置笔的行位置。
	//void
	//SetLnNNowTo(u8);

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
	PrintText();

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
	//Scroll(PFVOID pCheck);
};

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END

#endif

