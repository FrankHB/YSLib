/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Label.h
\ingroup UI
\brief 样式无关的用户界面标签。
\version r1542
\author FrankHB <frankhb1989@gmail.com>
\since build 573
\par 创建时间:
	2011-01-22 08:30:47 +0800
\par 修改时间:
	2015-02-02 10:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Label
*/


#ifndef YSL_INC_UI_Label_h_
#define YSL_INC_UI_Label_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidget
//#include YFM_YSLib_Service_YResource
#include YFM_YSLib_Core_YString
#include YFM_YSLib_Service_TextBase
#include YFM_YSLib_Service_YBrush

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
\ingroup UIBrushes
\ingroup UIModels
\brief 标签模块。
\warning 非虚析构。
*/
class YF_API MLabel
{
public:
	/*!
	\brief 当前使用的剪切文本更新器。
	\note 第三个模板参数表示是否自动换行。
	\note 被 DrawText 调用。
	\since build 525
	*/
	Drawing::GBrushUpdater<Drawing::TextState&, const String&, bool>
		UpdateClippedText{DefaultUpdateClippedText};

	/*!
	\brief 默认前景色。
	\since build 525
	*/
	Color ForeColor{Drawing::ColorSpace::Black};
	Drawing::Font Font; //!< 字体。
	Drawing::Padding Margin{Drawing::DefaultMargin}; //!< 文本和容器的间距。
	/*!
	\brief 文本水平和竖直对齐属性。
	\note 只在停用自动换行且可完整显示时有效。
	\since build 208
	*/
	TextAlignment HorizontalAlignment,
		VerticalAlignment = TextAlignment::Center;
	/*!
	\brief 启用自动换行。
	\since build 309
	*/
	bool AutoWrapLine = {};
//	bool AutoSize; //!< 启用根据字号自动调整大小。
//	bool AutoEllipsis; //!< 启用对超出标签宽度的文本调整大小。
	String Text{}; //!< 标签文本。

	/*!
	\brief 构造：使用指定字体、文本颜色和文本对齐样式。
	\since build 485
	*/
	explicit
	MLabel(const Drawing::Font& = {}, Color = Drawing::ColorSpace::Black,
		TextAlignment = TextAlignment::Left);
	DefDeMoveCtor(MLabel)

	/*!
	\brief 描画：使用发送者的大小并调用 DrawText 绘制文本。
	\since build 525
	*/
	void
	operator()(PaintEventArgs&&) const;

	/*!
	\brief 按参数指定的边界大小和当前状态计算笔的偏移位置。
	\since build 515
	*/
	Point
	GetAlignedPenOffset(const Size&) const;
	//! \since build 526
	DefGetter(const ynothrow, SDst, ItemHeight,
		Font.GetHeight() + GetVerticalOf(Margin))

	/*!
	\brief 绘制文本。
	\sa AlignPen
	\sa DrawClipText
	\since build 525
	*/
	void
	DrawText(const Size&, const PaintContext&) const;

	/*!
	\brief 绘制剪切文本：使用指定的绘制上下文和文本状态。
	\since build 525
	*/
	static void
	DefaultUpdateClippedText(const PaintContext&, Drawing::TextState&,
		const String&, bool);
	//@}
};


//! \brief 标签。
class YF_API Label : public Widget, protected MLabel
{
public:
	//! \since build 525
	using MLabel::ForeColor;
	using MLabel::Font;
	using MLabel::Margin;
	using MLabel::HorizontalAlignment;
	using MLabel::VerticalAlignment;
	//! \since build 308
	using MLabel::AutoWrapLine;
	using MLabel::Text;

	/*!
	\brief 构造：使用指定边界、字体、背景画刷和文字颜色。
	\since build 484
	*/
	explicit
	Label(const Rect& r = {}, const Drawing::Font& fnt = {},
		HBrush b = MakeBlankBrush(), Color c = Drawing::ColorSpace::Black)
		: Widget(r, b), MLabel(fnt, c)
	{}
	DefDeMoveCtor(Label)

	//! \since build 526
	using MLabel::GetAlignedPenOffset;
	//! \since build 526
	using MLabel::GetItemHeight;

	/*!
	\brief 计算指定字符串、初始边界、字体和边距的单行文本需要的标签边界。
	\note 仅当初始边界的大小为 Size::Invalid 时计算新的大小。
	\since build 573
	*/
	static Rect
	CalculateBounds(const String&, Rect, const Drawing::Font&,
		const Drawing::Padding& = {});

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;
};

/*!
\relates Label
\sa Label::CalculateBounds
\since build 573
*/
//@{
//! \brief 创建指定字符串、初始边界、字体和边距的单行文本需要的标签。
YF_API unique_ptr<Label>
MakeLabel(const String&, const Rect& = Rect::Invalid, const Drawing::Font& = {},
	const Drawing::Padding& = Drawing::DefaultMargin * 2);

//! \brief 设置指定字符串、初始边界、字体和边距的单行文本需要的标签。
YF_API void
SetupContentsOf(Label&, const String&, const Rect& = Rect::Invalid, const
	Drawing::Font& = {}, const Drawing::Padding& = Drawing::DefaultMargin * 2);
//@}


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

