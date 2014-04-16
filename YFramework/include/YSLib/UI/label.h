/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file label.h
\ingroup UI
\brief 样式无关的用户界面标签。
\version r1360
\author FrankHB <frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 08:30:47 +0800
\par 修改时间:
	2014-04-16 21:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Label
*/


#ifndef YSL_INC_UI_label_h_
#define YSL_INC_UI_label_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidget
//#include YFM_YSLib_Service_YResource
#include YFM_YSLib_Core_YString
#include YFM_YSLib_Service_TextBase

namespace YSLib
{

namespace UI
{

//文本对齐样式。
enum class TextAlignment
{
	Left = 0,
	Up = 0,
	Center = 1,
	Right = 2,
	Down = 2
};


/*!
\ingroup UIModels
\brief 标签模块。
*/
class YF_API MLabel : private noncopyable
{
public:
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin; //!< 文本和容器的间距。
	/*!
	\brief 文本水平和竖直对齐属性。
	\note 只在停用自动换行且可完整显示时有效。
	\since build 208
	*/
	TextAlignment HorizontalAlignment, VerticalAlignment;
	/*!
	\brief 启用自动换行。
	\since build 309
	*/
	bool AutoWrapLine;
//	bool AutoSize; //!< 启用根据字号自动调整大小。
//	bool AutoEllipsis; //!< 启用对超出标签宽度的文本调整大小。
	String Text; //!< 标签文本。

	//! \since build 485
	//@{
	//! \brief 构造：使用指定字体。
	explicit
	MLabel(const Drawing::Font& = {}, TextAlignment = TextAlignment::Left);
	DefDeMoveCtor(MLabel)
	virtual DefDeDtor(MLabel)

	/*!
	\brief 绘制文本。
	\sa DrawClipText
	*/
	void
	DrawText(const Size&, Color, const PaintContext&);

	/*!
	\brief 绘制剪切文本：使用指定的图形接口上下文、相对于部件的边界和文本状态。
	\note 被 DrawText 调用。
	*/
	virtual void
	DrawClippedText(const Graphics&, const Rect&, Drawing::TextState&);
	//@}
};


//! \brief 标签。
class YF_API Label : public Widget, protected MLabel
{
public:
	using MLabel::Font;
	using MLabel::Margin;
	using MLabel::HorizontalAlignment;
	using MLabel::VerticalAlignment;
	//! \since build 308
	using MLabel::AutoWrapLine;
	using MLabel::Text;
/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	/*!
	\brief 构造：使用指定边界、字体、背景画刷和文字颜色。
	\since build 484
	*/
	explicit
	Label(const Rect& r = {}, const Drawing::Font& fnt = {},
		HBrush b = MakeBlankBrush(), Color c = Drawing::ColorSpace::Black)
		: Widget(r, b, c), MLabel(fnt)
	{}
	DefDeMoveCtor(Label)

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;
};


/*!
\ingroup UIModels
\brief 文本列表模块。
\warning 非虚析构。
*/
class YF_API MTextList : public MLabel
{
public:
	using ItemType = String; //!< 项目类型：字符串。
	using ListType = vector<ItemType>; //!< 列表类型。
	using IndexType = typename ListType::size_type; //!< 索引类型。

protected:
	mutable shared_ptr<ListType> hList; //!< 文本列表句柄。
	/*!
	\brief 列表文本状态。
	\since build 346
	*/
	Drawing::TextState tsList;

protected:
	/*!
	\brief 构造：使用文本列表句柄和字体指针。
	\note 当文本列表指针为空时新建。
	\since build 337
	*/
	explicit
	MTextList(const shared_ptr<ListType>& = {}, const Drawing::Font& = {});
	DefDeMoveCtor(MTextList)

	//void
	//PaintTextList(Widget&, const Point&);

public:
	/*!
	\brief 取文本列表。
	\since build 392
	*/
	DefGetter(const ynothrow, const ListType&, List, *hList)
	/*!
	\brief 取文本列表引用。
	\since build 392
	*/
	DefGetter(ynothrow, ListType&, ListRef, *hList)
	/*!
	\brief 取指定项目索引的项目指针。
	\since build 392
	*/
	ItemType*
	GetItemPtr(const IndexType&);
	/*!
	\brief 取指定项目索引的项目 const 指针。
	\since build 392
	*/
	const ItemType*
	GetItemPtr(const IndexType&) const;
	/*!
	\brief 取项目行高。
	\since build 301
	*/
	DefGetter(const ynothrow, SDst, ItemHeight,
		GetTextLineHeightExOf(tsList))

	/*!
	\brief 取文本列表中的最大文本宽度。
	\since build 282
	*/
	SDst
	GetMaxTextWidth() const;

	/*!
	\brief 设置文本列表。
	\note 若参数为空则忽略。
	\since build 292
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
	\since build 316
	*/
	IndexType
	Find(const ItemType&) const;

	/*!
	\brief 刷新文本状态。
	*/
	void
	RefreshTextState();
};


/*!
\ingroup UIModels
\brief 高亮文本模块。
\warning 非虚析构。
\since build 486
*/
class YF_API MHilightText
{
public:
	//! \brief 高亮背景色。
	Color HilightBackColor;
	//! \brief 高亮文本色。
	Color HilightTextColor;

	MHilightText(const Color& bc, const Color& fc)
		: HilightBackColor(bc), HilightTextColor(fc)
	{}
	template<class _type>
	MHilightText(const _type& pr)
		: MHilightText(get<0>(pr), get<1>(pr))
	{}
};

} // namespace UI;

} // namespace YSLib;

#endif

