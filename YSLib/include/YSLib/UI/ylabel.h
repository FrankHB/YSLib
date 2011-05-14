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
\brief 样式无关的标签模块。
\version 0.2056;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 08:30:47 +0800;
\par 修改时间:
	2011-05-14 20:45 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Label;
*/


#ifndef YSL_INC_SHELL_LABEL_H_
#define YSL_INC_SHELL_LABEL_H_

#include "ywidget.h"
#include "../Adaptor/yfont.h"
//#include "../Core/yres.h"
#include "ytext.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

//标签模块。
class MLabel : public noncopyable
{
public:
	//文本（水平）对齐样式。
	typedef enum
	{
		Left = 0,
		Up = 0,
		Center = 1,
		Right = 2,
		Down = 2
	} TextAlignmentStyle;

	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本和容器的间距。
	TextAlignmentStyle HorizontalAlignment, VerticalAlignment; \
		//!< 文本水平和垂直对齐属性（只在可完整显示时有效）。
//	bool AutoSize; //!< 启用根据字号自动调整大小。
//	bool AutoEllipsis; //!< 启用对超出标签宽度的文本调整大小。
	String Text; //!< 标签文本。

protected:
	/*!
	\brief 构造：使用指定字体。
	*/
	explicit
	MLabel(const Drawing::Font& = Drawing::Font::GetDefault(),
		TextAlignmentStyle = Left);

	/*!
	\brief 绘制文本。
	*/
	void
	PaintText(IWidget&, Color, const Graphics&, const Point&);
};


//标签。
class Label : public Widget, protected MLabel
{
public:
	using MLabel::TextAlignmentStyle;
	using MLabel::Font;
	using MLabel::Margin;
	using MLabel::HorizontalAlignment;
	using MLabel::Text;
/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	/*!
	\brief 构造：使用指定边界和字体。
	*/
	explicit
	Label(const Rect& = Rect::Empty,
		const Drawing::Font& = Drawing::Font::GetDefault());

	/*!
	\brief 绘制界面。
	*/
	virtual void
	Paint();
};


//! \brief 文本列表模块。
class MTextList : public MLabel
{
public:
	typedef String ItemType; //!< 项目类型：字符串。
	typedef vector<ItemType> ListType; //!< 列表类型。
	typedef typename ListType::size_type IndexType; //!< 索引类型。

protected:
	mutable GHandle<ListType> pList; //!< 文本列表指针。

private:
	Drawing::TextState text_state; //!< 文本状态。

protected:
	/*!
	\brief 构造：使用指定文本列表和字体指针。
	*/
	explicit
	MTextList(GHandle<ListType> = nullptr,
		const Drawing::Font& = Drawing::Font::GetDefault());

	//void
	//PaintTextList(Widget&, const Point&);

public:
	/*!
	\brief 取文本列表。
	\note 当文本列表指针为空时新建。
	*/
	ListType&
	GetList() const;
	/*!
	\brief 取指定项目索引的项目指针。
	*/
	ItemType*
	GetItemPtr(IndexType) const;
	/*!
	\brief 取项目行高。
	*/
	SDst
	GetItemHeight() const;

protected:
	/*!
	\brief 取文本状态。
	*/
	Drawing::TextState&
	GetTextState();

public:
	/*!
	\brief 刷新文本状态。
	*/
	void
	RefreshTextState();
};

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

