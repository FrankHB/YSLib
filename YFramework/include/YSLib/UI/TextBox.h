/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextBox.h
\ingroup UI
\brief 样式相关的用户界面文本框。
\version r209
\author FrankHB <frankhb1989@gmail.com>
\since build 482
\par 创建时间:
	2014-03-02 16:17:46 +0800
\par 修改时间:
	2014-03-23 10:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TextBox
*/


#ifndef YSL_INC_UI_TextBox_h_
#define YSL_INC_UI_TextBox_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YControl
#include YFM_YSLib_UI_Label
#include YFM_YSLib_Service_YTimer
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_UI_Animation

namespace YSLib
{

namespace UI
{

/*!
\brief 插入符光标。
\since build 483
*/
class YF_API Caret final
{
private:
	/*!
	\brief 插入符闪烁动画。
	\todo 使用 TLS 以保证指向动画对象的线程安全性。
	*/
	static GAnimationSession<InvalidationUpdater> caret_animation;

public:
	//! \brief 决定是否显示插入符的计时器。
	Timers::Timer CaretTimer{std::chrono::seconds(1)};
	/*!
	\brief 插入符画刷。
	\build 484
	*/
	HBrush CaretBrush;
	/*!
	\brief 供闪烁动画使用的指定无效化区域的光标位置刷新器。
	\since build 484
	*/
	InvalidationUpdater::Invalidator CursorInvalidator;

	/*!
	\brief 构造：注册插入符光标动画的事件处理器和画刷。
	\note 通过 GotFocus 事件启动动画，通过 OnLostFocus 停止动画。
	\since build 484
	*/
	Caret(IWidget&, HBrush, InvalidationUpdater::Invalidator);
	~Caret();

	//! \brief 检查是否需要对指定部件进行绘制。
	bool
	Check(IWidget&);

	//! \brief 停止插入符光标动画。
	static void
	Stop();
};


/*!
\brief 文本选择区域。
\since build 484
*/
struct YF_API TextSelection final
{
	//! \brief 位置：横坐标和纵坐标。
	using Position = GBinaryGroup<size_t>;
	/*!
	\brief 文本区段：表示起始和结束位置。
	\note 仅表示选择的起始和结束，因此结束位置在文本中可在起始位置之前。
	*/
	using Span = pair<Position, Position>;

	//! \brief 选择的范围。
	Span Range;

	/*!
	\brief 折叠选择范围到结束。
	\since build 485
	*/
	PDefH(void, Collapse, )
		ImplExpr(Range.first = Range.second)
};


/*!
\brief 文本框。
\sa Label
\since build 482
\todo 支持选中删除、退格和非字母符号和方向键光标移动。
\todo 支持多行模式的插入符光标移动、文本选中和插入。
*/
class YF_API TextBox : public Control, protected MLabel, protected MHilightText
{
public:
	using MLabel::Font;
	using MLabel::Margin;
	using MLabel::HorizontalAlignment;
	using MLabel::VerticalAlignment;
	using MLabel::AutoWrapLine;
	using MLabel::Text;
	//! \since build 486
	using MHilightText::HilightBackColor;
	//! \since build 486
	using MHilightText::HilightTextColor;

	/*!
	\brief 选择区域：结束位置指示插入符光标逻辑位置。
	\since build 484
	*/
	TextSelection Selection;
	/*!
	\brief 插入符光标。
	\since build 484
	*/
	Caret CursorCaret;

private:
	/*!
	\brief 文字区域水平基准负偏移：文本内容在光标回退方向超出的未显示部分大小。
	\since build 484
	*/
	SDst h_offset;

public:
	/*!
	\brief 构造：使用指定边界、字体和高亮背景色/文本色对。
	\since build 486
	*/
	explicit
	TextBox(const Rect& = {}, const Drawing::Font& = {}, const
		pair<Drawing::Color, Drawing::Color>& = FetchGUIState().Colors.GetPair(
		Styles::Highlight, Styles::HighlightText));
	DefDeMoveCtor(TextBox)

	/*!
	\brief 取指定点所在的插入符光标逻辑位置。
	\param 相对于部件左上角偏移。
	\since build 484
	\todo AutoWrap 为 true 时判断行数。
	*/
	TextSelection::Position
	GetCaretPosition(const Point&);

	//! \since build 485
	void
	DrawClippedText(const Graphics&, const Rect&, TextState&) override;

	//! \brief 刷新：按指定参数绘制界面并更新状态。
	void
	Refresh(PaintEventArgs&&) override;

	/*!
	\brief 无效化默认插入符。
	\pre 参数对象的动态类型为 TextBox 或其派生类。
	\sa Caret::CursorInvalidator
	\since build 484
	*/
	static bool
	InvalidateDefaultCaret(IWidget&);

	//! \brief 绘制默认插入符。
	void
	PaintDefaultCaret(PaintEventArgs&&);
};

} // namespace UI;

} // namespace YSLib;

#endif

