/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Selector.h
\ingroup UI
\brief 样式相关的图形用户界面选择控件。
\version r681
\author FrankHB <frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-22 07:17:17 +0800
\par 修改时间:
	2016-06-15 09:08 +0800
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

	//! \brief 状态。
	StateType State;

	GMSelector()
		: State()
	{}
	GMSelector(StateType st)
		: State(st)
	{}

	/*!
	\brief 更新状态。
	\return 是否和旧状态相同。
	*/
	bool
	UpdateState(StateType st)
	{
		const auto old(State);

		State = st;
		return st != old;
	}
};


/*!
\ingroup ControlModels
\brief 复选框模块。
\warning 非虚析构。
*/
class YF_API MCheckBox
{
public:
	//! \since build 480
	//@{
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
	//@}

protected:
	MSelector mSelector;

public:
	//! \brief 复选框选中事件。
	GEvent<void(TickedArgs)> Ticked;

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

	//! \since build 478
	using MCheckBox::Ticked;

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

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 311
	*/
	void
	Refresh(PaintEventArgs&&) override;

	/*!
	\brief 设置选中状态并触发复选框选中事件。
	\note 不检查状态改变。
	\since build 478
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
	//! \since build 525
	using MLabel::ForeColor;
	using MLabel::Font;
	using MLabel::Text;

	/*!
	\brief 构造：使用指定边界。
	\since build 337
	*/
	explicit
	CheckButton(const Rect& = {});

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\note 不完全强异常安全：只保证视图设置大小不抛出异常时的一致性。
	*/
	void
	Refresh(PaintEventArgs&&) override;
};


/*!
\ingroup ControlModels
\brief 单选框模块。
\warning 非虚析构。
\since build 493
*/
class YF_API MSharedSelection
{
public:
	//! \since build 480
	//@{
	//! \brief 单选框选中状态类型。
	using StateType = observer_ptr<IWidget>;
	using MSelector = GMSelector<StateType>;
	//! \brief 单选框选中状态参数类型。
	using SelectedArgs = MSelector::SelectedArgs;
	//@}

private:
	/*!
	\brief 共享状态。
	\invariant <tt>bool(p_selector)</tt> 。
	\since build 480
	*/
	shared_ptr<MSelector> p_selector;

public:
	/*!
	\since build 480
	\brief 单选框选中事件。
	*/
	GEvent<void(SelectedArgs)> Selected;

	//! \since build 493
	//@{
	MSharedSelection(StateType st = {})
		: p_selector(make_shared<MSelector>(st))
	{}
	MSharedSelection(shared_ptr<MSelector> p_sel, StateType st = {})
		: p_selector(p_sel ? std::move(p_sel)
		: make_shared<MSelector>(st))
	{}
	//@}

	DefGetter(const ynothrow, StateType, State, p_selector->State)

	DefSetter(, StateType, State, p_selector->State)

	/*!
	\brief 向其它对象覆盖状态。
	\since build 480
	*/
	void
	ShareTo(MSharedSelection&) const;

	PDefH(bool, UpdateState, StateType st)
		ImplRet(p_selector->UpdateState(st))
};


//! \since build 479
//@{
//! \brief 单选框。
class YF_API RadioBox : public Thumb, protected MSharedSelection
{
public:
	//! \brief 视觉样式项目。
	enum StyleItem : Styles::StyleItem
	{
		RadioBoxBackground = Thumb::EndStyle,
		EndStyle
	};
	using MSharedSelection::StateType;
	//! \since build 480
	using MSharedSelection::SelectedArgs;

	//! \since build 480
	using MSharedSelection::Selected;

	//! \brief 构造：使用指定边界。
	explicit
	RadioBox(const Rect& = {});
	DefDeMoveCtor(RadioBox)

	//! \since build 480
	DefPred(const ynothrow, Selected, GetState().get() == this)

	using MSharedSelection::GetState;

	/*!
	\brief 设置选中状态并检查单选框选中事件。
	\note 选中状态发生改变时若旧的选中部件存在则无效化，引起单选框选中事件。
	\since build 480
	*/
	void
	SetSelected();

	//! \brief 刷新：按指定参数绘制界面并更新状态。
	void
	Refresh(PaintEventArgs&&) override;

	/*!
	\brief 设置选中状态并触发单选框选中事件。
	\note 不检查状态改变。
	\since build 480
	*/
	void
	Select();

	/*!
	\brief 向其它对象覆盖状态。
	\since build 480
	*/
	void
	ShareTo(RadioBox&) const;
};


//! \brief 单选按钮。
class YF_API RadioButton : public RadioBox, protected MLabel
{
public:
	//! \since build 525
	using MLabel::ForeColor;
	using MLabel::Font;
	using MLabel::Text;

	//! \brief 构造：使用指定边界。
	explicit
	RadioButton(const Rect& = {});

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\note 不完全强异常安全：只保证视图设置大小不抛出异常时的一致性。
	*/
	void
	Refresh(PaintEventArgs&&) override;
};
//@}

} // namespace UI;

} // namespace YSLib;

#endif

