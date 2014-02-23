/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Selector.h
\ingroup UI
\brief 样式相关的图形用户界面选择控件。
\version r456
\author FrankHB <frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-22 07:17:17 +0800
\par 修改时间:
	2014-02-23 08:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Selector
*/


#ifndef YSL_INC_UI_Selector_h_
#define YSL_INC_UI_Selector_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_Button
#include YFM_YSLib_UI_YStyle

namespace YSLib
{

namespace UI
{

//! \since build 478
//@{
/*!
\ingroup UIModels
\brief 选择器模型模板。
\tparam _type 状态类型。
\warning 非虚析构。
*/
template<typename _type>
class GMSelector
{
public:
	//! \brief 状态类型。
	using StateType = _type;
	//! \brief 选中状态参数类型。
	using SelectedArgs = GValueEventArgs<StateType>;
	//! \brief 选中事件委托模板。
	DeclDelegate(HSelectedEvent, SelectedArgs)

	//! \brief 状态。
	StateType State;

	GMSelector()
		: State()
	{}
	GMSelector(StateType st)
		: State(st)
	{}

	/*
	\brief 更新状态。
	\return 是否和旧状态相同。
	*/
	bool
	UpdateState(StateType st)
	{
		const StateType old(State);

		State = st;
		return st != old;
	}
};


/*!
\brief 选择框模块。
\warning 非虚析构。

用于提供公共类型而非具体控件功能的模块。
*/
class YF_API MSelectorBox
{
public:
	//! \brief 选择框选中状态类型。
	using StateType = enum SelectedState : yimpl(size_t)
	{
		Unchecked = 0,
		Checked = 1,
		Partial = 2
	};
	using MSelector = GMSelector<StateType>;
	/*!
	\brief 选择框选中状态参数类型。
	\note StateType 参数表示选中后的状态。
	*/
	using TickedArgs = MSelector::SelectedArgs;
	//! \brief 选择框选中事件委托类型。
	using HTickedEvent = MSelector::HSelectedEvent;
};


/*!
\brief 复选框模块。
\warning 非虚析构。
*/
class YF_API MCheckBox : protected MSelectorBox
{
protected:
	MSelector mSelector;

public:
	//! \brief 复选框选中事件。
	DeclEvent(HTickedEvent, Ticked)

	MCheckBox(StateType st = Unchecked)
		: mSelector(st)
	{}

	DefGetter(const ynothrow, StateType, State, mSelector.State)
};
//@}


/*!
\brief 复选框。
\since build 205
*/
class YF_API CheckBox : public Thumb, protected MCheckBox
{
public:
	/*!
	\brief 视觉样式项目。
	\since build 468
	*/
	enum StyleItem : Styles::StyleItem
	{
		CheckBoxBackground = Thumb::EndStyle,
		EndStyle
	};
	//! \since build 479
	//@{
	using MCheckBox::StateType;
	using MCheckBox::Checked;
	using MCheckBox::Unchecked;
	using MCheckBox::Partial;
	//@}
	//! \since build 292
	using MCheckBox::TickedArgs;
	//! \since build 292
	using MCheckBox::HTickedEvent;

	//! \since build 478
	using MCheckBox::Ticked;

public:
	/*!
	\brief 构造：使用指定边界。
	\since build 337
	*/
	explicit
	CheckBox(const Rect& = {});
	DefDeMoveCtor(CheckBox)

	DefPred(const ynothrow, Ticked, GetState() == Checked)
	//! \since build 478
	using MCheckBox::GetState;

	/*!
	\brief 设置选中状态并检查复选框选中事件。
	\note 若选中状态发生改变则引起复选框选中事件。
	\since build 488
	*/
	void
	SetTicked(StateType);

protected:
	/*!
	\brief 绘制复选框。
	\since build 453
	*/
	void
	PaintBox(const PaintContext&, const Size&);

public:
	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 311
	*/
	void
	Refresh(PaintEventArgs&&) override;

	/*!
	\brief 设置选中状态并触发复选框选中事件。
	\note 不检查状态改变。
	\since build 488
	*/
	void
	Tick(StateType);
};


/*!
\brief 复选按钮。
\since build 311
*/
class YF_API CheckButton : public CheckBox, protected MLabel
{
public:
	using MLabel::Font;
	using MLabel::Text;

	/*!
	\brief 构造：使用指定边界。
	\since build 337
	*/
	explicit
	CheckButton(const Rect& = {});

	void
	Refresh(PaintEventArgs&&) override;
};

} // namespace UI;

} // namespace YSLib;

#endif

