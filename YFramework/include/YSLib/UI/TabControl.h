/*
	© 2014-2016, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TabControl.h
\ingroup UI
\brief 样式相关的图形用户界面标签页控件。
\version r265
\author FrankHB <frankhb1989@gmail.com>
\since build 494
\par 创建时间:
	2014-04-19 11:21:43 +0800
\par 修改时间:
	2019-11-25 21:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TabControl
*/


#ifndef YSL_INC_UI_TabControl_h_
#define YSL_INC_UI_TabControl_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YPanel // for function;
#include YFM_YSLib_UI_WidgetIteration

namespace YSLib
{

namespace UI
{

/*!
\brief 标签栏控件。
\since build 494
*/
class YF_API TabBar : public Control, protected MLinearUIContainer
{
public:
	using TabPtrType = unique_ptr<Control>;

	//! \brief 首个标签所在的起始偏移位置。
	SPos Offset = 0;
	//! \brief 每个标签最小的位置。
	SDst MinWidth = 12;
	/*!
	\note 空时被忽略。
	\since build 872
	*/
	//@{
	//! \brief 标签背景附加绘制操作。
	function<void(PaintEventArgs&&)> PaintTabBackground{};
	//! \brief 标签前景附加绘制操作。
	function<void(PaintEventArgs&&)> PaintTabForeground{DefaultPaintTabBorder};
	//@}

	explicit
	TabBar(const Rect& = {});

	virtual void
	operator+=(IWidget&);

	virtual bool
	operator-=(IWidget&);

	DefWidgetChildrenGetter

	using MLinearUIContainer::Contains;

	//! \since build 495
	using MLinearUIContainer::GetCount;

	//! \brief 绘制默认标签页边框。
	static void
	DefaultPaintTabBorder(PaintEventArgs&&);

	//! \since build 495
	using MLinearUIContainer::Find;

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
};


/*!
\brief 标签容器控件。
\since build 494
*/
class YF_API TabControl : public Control
{
public:
	using iterator = ystdex::subscriptive_iterator<TabControl, IWidget>;

	//! \brief 标签栏初始高。
	SDst BarHeight = 24;

private:
	/*!
	\brief 标签栏。
	\invariant <tt>bool(p_bar)</tt> 。
	*/
	unique_ptr<TabBar> p_bar;
	//! \since build 672
	observer_ptr<Panel> p_page{};
	//! \since build 672
	vector<observer_ptr<Panel>> tab_pages{};

public:
	explicit
	TabControl(const Rect& = {});
	DefDeMoveCtor(TabControl)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~TabControl() override;

	DefWidgetMemberIterationOperations(iterator)

	PDefHOp(IWidget&, [], size_t idx) ynoexcept
		ImplRet(YAssertNonnull(idx == 0 || p_page),
			*ystdex::forward_as_array<observer_ptr<IWidget>>(
			make_observer(p_bar.get()), p_page).begin()[idx])

	/*!
	\brief 添加部件并隐藏。
	\pre 参数对应的部件的动态类型是 Panel 或其派生类。
	\exception std::bad_cast 参数不能 dynamic_cast 为 Panel& 。
	*/
	void
	operator+=(IWidget&);

	DefGetter(const ynothrow, size_t, ChildrenCount, p_page ? 2 : 1)
	//! \brief 取页面边界。
	Rect
	GetPageBounds() const ynothrow;
	//! \since build 495
	DefGetter(const ynothrow, TabBar&, TabBarRef, Deref(p_bar))
	/*!
	\brief 取标签数。
	\since build 495
	*/
	DefGetter(const ynothrow, size_t, TabCount, GetTabBarRef().GetCount())
	//! \since build 672
	DefGetter(const ynothrow, const vector<observer_ptr<Panel>>&, Pages, tab_pages)


	//! \brief 附加：添加 TouchDown 事件处理器。
	void
	Attach(Control&);

	/*!
	\brief 查找页面部件。
	\return 若找到则为页面部件的索引，否则等于页面数。
	\since build 495
	*/
	size_t
	Find(IWidget&) const;

	/*!
	\brief 按标签切换页面。
	\since build 495
	*/
	bool
	SwitchTab(Control&);

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

	/*!
	\brief 更新标签页状态。
	\sa UpdatePageLayout
	\since build 495

	调整标签栏布局；选中第一个存在对应页面的标签；
	若存在选中页面则设置可视状态并调整大小；最后无效化。
	*/
	void
	UpdateTabPages();
};

} // namespace UI;

} // namespace YSLib;

#endif

