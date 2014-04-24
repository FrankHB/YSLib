/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TabControl.h
\ingroup UI
\brief 样式相关的图形用户界面标签页控件。
\version r178
\author FrankHB <frankhb1989@gmail.com>
\since build 494
\par 创建时间:
	2014-04-19 11:21:43 +0800
\par 修改时间:
	2014-04-24 13:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TabControl
*/


#ifndef YSL_INC_UI_TabControl_h_
#define YSL_INC_UI_TabControl_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YPanel
#include YFM_YSLib_UI_Selector
#include YFM_YSLib_UI_WidgetIteration

namespace YSLib
{

namespace UI
{

/*!
\brief 标签栏控件。
\since build 494
*/
class YF_API TabBar : public Control, protected MLinearUIContainer,
	protected MSharedSelection
{
public:
	using MSharedSelection::HSelectedEvent;
	using MSharedSelection::SelectedArgs;
	using MSharedSelection::StateType;
	using TabPtrType = unique_ptr<Control>;

	using MSharedSelection::Selected;

	//! \brief 首个标签所在的起始偏移位置。
	SPos Offset = 0;
	//! \brief 每个标签最小的位置。
	SDst MinWidth = 12;
	/*!
	\brief 标签背景附加绘制操作。
	\note 空时被忽略。
	*/
	std::function<void(PaintEventArgs&&)> PaintTabBackground{};
	/*!
	\brief 标签前景附加绘制操作。
	\note 空时被忽略。
	*/
	std::function<void(PaintEventArgs&&)>
		PaintTabForeground{DefaultPaintTabBorder};

	explicit
	TabBar(const Rect& = {});

	virtual void
	operator+=(IWidget&);

	virtual bool
	operator-=(IWidget&);

	DefWidgetChildrenGetter

	using MLinearUIContainer::Contains;

	//! \brief 绘制默认标签页边框。
	static void
	DefaultPaintTabBorder(PaintEventArgs&&);

	/*!
	\brief 布局。
	\return 布局结束时的最后标签的结束位置。
	*/
	SPos
	PerformLayout();

	void
	Refresh(PaintEventArgs&&) override;

	/*!
	\brief 切换页面。
	\return 是否切换成功。
	*/
	bool
	SwitchPage(Control&);

	using MSharedSelection::UpdateState;
};


/*!
\brief 标签容器控件。
\since build 494
*/
class YF_API TabControl : public Control
{
public:
	using iterator = ystdex::subscriptive_iterator<TabControl, IWidget>;

private:
	/*!
	\brief 标签栏。
	\invariant <tt>bool(p_bar)</tt> 。
	*/
	unique_ptr<TabBar> p_bar;
	Panel* p_page{};
	vector<Panel*> tab_pages{};

public:
	//! \brief 标签栏初始高。
	SDst BarHeight = 24;

	explicit
	TabControl(const Rect& = {});
	DefDeMoveCtor(TabControl)

	DefWidgetMemberIterationOperations(iterator)

	PDefHOp(IWidget&, [], size_t idx) ynoexcept
		ImplRet(*(std::initializer_list<IWidget*>{p_bar.get(), p_page})
			.begin()[idx])

	DefGetter(const ynothrow, size_t, ChildrenCount, p_page ? 2 : 1)

	//! \brief 取页面边界。
	Rect
	GetPageBounds() const ynothrow;
	DefGetter(ynothrow, TabBar&, TabBarRef,
		(YAssert(bool(p_bar), "Null pointer found."), *p_bar))
	DefGetter(const ynothrow, const vector<Panel*>&, Pages, tab_pages)

	//! \exception std::bad_cast 参数不能 dynamic_cast 为 Panel 。
	void
	operator+=(IWidget&);

	//! \brief 附加：添加 TouchDown 事件处理器。
	void
	Attach(Control&);

	/*!
	\brief 切换页面。
	\return 是否切换成功。
	\note 设置页面选择、页面大小和状态；若参数指定的页面不存在或标签无效则忽略。
	*/
	//@{
	bool
	SwitchPage(size_t);
	//! \pre 断言：控件属于标签。
	bool
	SwitchPage(Control&);
	//@}
};

} // namespace UI;

} // namespace YSLib;

#endif

