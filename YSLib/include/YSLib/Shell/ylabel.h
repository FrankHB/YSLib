/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ylabel.h
\ingroup Shell
\brief 平台无关的标签模块实现。
\version 0.1900;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 08:30:47 + 08:00;
\par 修改时间:
	2011-02-20 14:27 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YLabel;
*/


#ifndef INCLUDED_YLABEL_H_
#define INCLUDED_YLABEL_H_

#include "../Core/ysdef.h"
#include "../Core/ycutil.h"
#include "../Core/ycounter.hpp"
#include "../Adaptor/yfont.h"
#include "ywidget.h"
//#include "../Core/yres.h"
#include "ytext.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

//标签模块。
class MLabel : public NonCopyable
{
public:
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本和容器的间距。
	bool AutoSize; //!< 启用根据字号自动调整大小。
	bool AutoEllipsis; //!< 启用对超出标签宽度的文本调整大小。
	String Text; //!< 标签文本。

protected:
	/*!
	\brief 构造：使用指定字体。
	*/
	explicit
	MLabel(const Drawing::Font& = Drawing::Font::GetDefault());

	/*!
	\brief 绘制文本。
	*/
	void
	PaintText(Widget&, const Graphics&, const Point&);
};


//标签。
class YLabel : public GMCounter<YLabel>, public YWidget,
	public MLabel
{
public:
	typedef YWidget ParentType;

/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	/*!
	\brief 构造：使用指定边界、部件容器指针和字体文本区域。
	*/
	explicit
	YLabel(const Rect& = Rect::Empty, IUIBox* = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault());

	virtual DefEmptyDtor(YLabel)

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();
};


//! \brief 文本列表模块。
class MTextList : private MLabel
{
protected:
	/*!
	\brief 构造：使用指定字体指针。
	*/
	explicit
	MTextList(const Drawing::Font& = Drawing::Font::GetDefault());

	//void
	//PaintTextList(Widget&, const Point&);
};

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

