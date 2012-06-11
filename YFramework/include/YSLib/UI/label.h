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
\version r2229;
\author FrankHB<frankhb1989@gmail.com>
\since build 188 。
\par 创建时间:
	2011-01-22 08:30:47 +0800;
\par 修改时间:
	2012-06-08 16:28 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Label;
*/


#ifndef YSL_INC_UI_LABEL_H_
#define YSL_INC_UI_LABEL_H_

#include "ywidget.h"
//#include "../Core/yres.h"
#include "../Core/ystring.h"
#include "../Service/TextBase.h"

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


/*!
\brief 标签模块。
\warning 非虚析构。
*/
class MLabel : private noncopyable
{
public:
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本和容器的间距。
	/*!
	\brief 文本水平和竖直对齐属性。
	\note 只在停用自动换行且可完整显示时有效。
	\since build 208 。
	*/
	TextAlignment HorizontalAlignment, VerticalAlignment; 
	/*!
	\brief 启用自动换行。
	\since build 309 。
	*/
	bool AutoWrapLine;
//	bool AutoSize; //!< 启用根据字号自动调整大小。
//	bool AutoEllipsis; //!< 启用对超出标签宽度的文本调整大小。
	String Text; //!< 标签文本。

protected:
	/*!
	\brief 构造：使用指定字体。
	*/
	explicit
	MLabel(const Drawing::Font& = FetchPrototype<Drawing::Font>(),
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
	//! \since build 308 。
	using MLabel::AutoWrapLine;
	using MLabel::Text;
/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	/*!
	\brief 构造：使用指定边界和字体。
	*/
	explicit
	Label(const Rect& r = Rect::Empty,
		const Drawing::Font& fnt = FetchPrototype<Drawing::Font>())
		: Widget(r), MLabel(fnt)
	{}
	inline DefDeMoveCtor(Label)

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294 。
	*/
	void
	Refresh(PaintEventArgs&&) override;
};


/*!
\brief 文本列表模块。
\warning 非虚析构。
*/
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
		const Drawing::Font& = FetchPrototype<Drawing::Font>());
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
	\since build 301 。
	*/
	DefGetter(const ynothrow, SDst, ItemHeight,
		GetTextLineHeightExOf(text_state))

protected:
	/*!
	\brief 取文本状态。
	\since build 301 。
	*/
	DefGetter(ynothrow, Drawing::TextState&, TextState, text_state)

public:
	/*!
	\brief 取文本列表中的最大文本宽度。
	\since build 282 。
	*/
	SDst
	GetMaxTextWidth() const;

	/*!
	\brief 设置文本列表。
	\note 若参数为空则忽略。
	\since build 292 。
	*/
	void
	SetList(const shared_ptr<ListType>& h)
	{
		if(YB_LIKELY(h))
			hList = h;
	}

	/*!
	\brief 查找项。
	\return 若找到则返回对应索引，否则返回 <tt>IndexType(-1)</tt> 。
	\since build 316 。
	*/
	IndexType
	Find(const ItemType&) const;

	/*!
	\brief 刷新文本状态。
	*/
	void
	RefreshTextState();
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

