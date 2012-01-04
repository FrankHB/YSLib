/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file label.h
\ingroup UI
\brief 样式无关的用户界面标签。
\version r2133;
\author FrankHB<frankhb1989@gmail.com>
\since build 188 。
\par 创建时间:
	2011-01-22 08:30:47 +0800;
\par 修改时间:
	2012-01-04 08:16 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Label;
*/


#ifndef YSL_INC_UI_LABEL_H_
#define YSL_INC_UI_LABEL_H_

#include "ywidget.h"
//#include "../Core/yres.h"
#include "../Service/ytext.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//文本对齐样式。
typedef enum class
{
	Left = 0,
	Up = 0,
	Center = 1,
	Right = 2,
	Down = 2
} TextAlignment;


//! \brief 标签模块。
class MLabel : public noncopyable
{
public:
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本和容器的间距。
	TextAlignment HorizontalAlignment, VerticalAlignment; \
		//!< 文本水平和竖直对齐属性（只在可完整显示时有效）。
//	bool AutoSize; //!< 启用根据字号自动调整大小。
//	bool AutoEllipsis; //!< 启用对超出标签宽度的文本调整大小。
	String Text; //!< 标签文本。

protected:
	/*!
	\brief 构造：使用指定字体。
	*/
	explicit
	MLabel(const Drawing::Font& = Drawing::Font::GetDefault(),
		TextAlignment = TextAlignment::Left);
	inline DefDeMoveCtor(MLabel)

	/*!
	\brief 绘制文本。
	*/
	void
	PaintText(const Size&, Color, const PaintContext&);
};


//! \brief 标签。
class Label : public Widget, protected MLabel
{
public:
	using MLabel::Font;
	using MLabel::Margin;
	using MLabel::HorizontalAlignment;
	using MLabel::VerticalAlignment;
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
	inline DefDeMoveCtor(Label)

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);
};

inline
Label::Label(const Rect& r, const Drawing::Font& fnt)
	: Widget(r), MLabel(fnt)
{}


//! \brief 文本列表模块。
class MTextList : public MLabel
{
public:
	typedef String ItemType; //!< 项目类型：字符串。
	typedef vector<ItemType> ListType; //!< 列表类型。
	typedef typename ListType::size_type IndexType; //!< 索引类型。

protected:
	mutable shared_ptr<ListType> hList; //!< 文本列表句柄。

private:
	Drawing::TextState text_state; //!< 文本状态。

protected:
	/*!
	\brief 构造：使用文本列表句柄和字体指针。
	\note 当文本列表指针为空时新建。
	*/
	explicit
	MTextList(const shared_ptr<ListType>& = shared_ptr<ListType>(),
		const Drawing::Font& = Drawing::Font::GetDefault());
	inline DefDeMoveCtor(MTextList)

	//void
	//PaintTextList(Widget&, const Point&);

public:
	/*!
	\brief 取文本列表。
	*/
	DefGetter(const ynothrow, ListType&, List, *hList)
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

inline SDst
MTextList::GetItemHeight() const
{
	return GetTextLineHeightExOf(text_state);
}
inline Drawing::TextState&
MTextList::GetTextState()
{
	return text_state;
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

