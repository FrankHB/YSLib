/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ComboList.h
\ingroup UI
\brief 样式相关的图形用户界面组合列表控件。
\version r2585
\author FrankHB <frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-07 20:30:40 +0800
\par 修改时间:
	2013-12-23 22:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::ComboList
*/


#ifndef YSL_INC_UI_ComboList_h_
#define YSL_INC_UI_ComboList_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_TextList
#include YFM_YSLib_UI_Scroll
#include YFM_YSLib_Core_YFileSystem
//#include YFM_YSLib_UI_YStyle

namespace YSLib
{

namespace UI
{

/*!
\brief 带滚动条的文本列表框。
\since build 205
\todo 实现 Resize 事件调整内容布局。
*/
class YF_API ListBox : public ScrollableContainer
{
public:
	using ListType = TextList::ListType;
	using ViewArgs = TextList::ViewArgs;
	using HViewEvent = TextList::HViewEvent;
	//! \since build 316
	using IndexType = TextList::IndexType;
	//! \since build 316
	using ItemType = TextList::ItemType;
	//! \since build 460
	using iterator = ystdex::subscriptive_iterator<ListBox, IWidget>;

protected:
	/*
	\brief 文本列表。
	\since build 391
	*/
	TextList tlContent;

public:
	//! \since build 337
	explicit
	ListBox(const Rect& = {}, const shared_ptr<ListType>& = {});
	inline DefDeMoveCtor(ListBox)

	//! \since build 357
	DefWidgetMemberIterationBase(ScrollableContainer, tlContent)

	DefPredMem(const ynothrow, Selected, tlContent)
	PDefH(bool, Contains, ListType::size_type i)
		ImplBodyMem(tlContent, Contains, i)

	DefGetterMem(const ynothrow, ListType::size_type, HeadIndex, tlContent)
	DefGetterMem(const ynothrow, ListType::size_type, SelectedIndex,
		tlContent)
	//! \since build 392
	DefGetterMem(const ynothrow, const ListType&, List, tlContent)
	//! \since build 392
	DefGetterMem(ynothrow, ListType&, ListRef, tlContent)
	DefEventGetterMem(ynothrow, HViewEvent, ViewChanged, tlContent) \
		//!< 视图变更事件。
	DefEventGetterMem(ynothrow, HIndexEvent, Selected, tlContent) \
		//!< 项目选择状态变更事件。
	DefEventGetterMem(ynothrow, HIndexEvent, Confirmed, tlContent) \
		//!< 项目选中确定事件。

	/*!
	\brief 设置文本列表。
	\since build 292
	*/
	DefSetterMem(const shared_ptr<ListType>&, List, tlContent)
	/*!
	\brief 设置选中项。
	\sa TextList::SetSelected
	\since build 285。
	*/
	PDefH(void, SetSelected, ListType::size_type i)
		ImplBodyMem(tlContent, SetSelected, i)

	/*!
	\brief 按内容大小依次调整列表视图中选中和首个项目的索引。
	\warning 若视图大小变化后不调用此方法调整视图，可能导致选择项越界而行为未定义。
	\sa TextList::AdjustViewLength
	\since build 392
	*/
	PDefH(void, AdjustViewForContent, )
		ImplBodyMem(tlContent, AdjustViewForContent, )

	/*!
	\brief 调整视图长度。
	\sa TextList::AdjustViewLength
	\since build 285
	*/
	PDefH(void, AdjustViewLength, )
		ImplBodyMem(tlContent, AdjustViewLength, )

	/*!
	\brief 清除选中项。
	\sa TextList::ClearSelected
	\since build 285
	*/
	PDefH(void, ClearSelected, )
		ImplBodyMem(tlContent, ClearSelected, )

	/*!
	\brief 查找项。
	\sa TextList::Find
	\since build 316
	*/
	PDefH(IndexType, Find, const ItemType& text)
		ImplBodyMem(tlContent, Find, text)

	PDefH(void, ResetView, )
		ImplBodyMem(tlContent, ResetView, )

	/*!
	\brief 按指定大小上限和内容调整大小。
	\param sup 指定调整的大小的上限，当为 Size() 时忽略大小限制。
	\param s 指定大小，分量为 0 时对应分量大小由内容确定（其中宽度含边距）。
	\since build 337
	*/
	void
	ResizeForPreferred(const Size& sup, Size s = {});

	PDefH(void, UpdateView, )
		ImplExpr(UI::UpdateView(tlContent))
};


/*!
\brief 文件列表框。
\since build 205
*/
class YF_API FileBox : public ListBox
{
protected:
	/*!
	\brief 目录的完整路径。
	\since build 414
	*/
	IO::Path pthDirectory;

public:
	//! \since build 337
	explicit
	FileBox(const Rect& = {});
	inline DefDeMoveCtor(FileBox)

	/*!
	\brief 导航至绝对路径。
	\since build 414
	*/
	bool
	operator=(const IO::Path&);

	/*!
	\brief 导航至子目录。
	\since build 414
	*/
	bool
	operator/=(const String&);
	/*!
	\brief 导航至相对路径。
	\since build 414
	*/
	bool
	operator/=(const IO::Path&);

	/*!
	\brief 取目录的完整路径。
	\since build 414
	*/
	DefGetter(const ynothrow, const IO::Path&, Directory, pthDirectory)
	/*!
	\brief 取当前选中项的路径。
	*/
	IO::Path
	GetPath() const;

	/*!
	\brief 设置路径，若成功则刷新列表。
	\return 是否成功。
	\since build 298
	*/
	bool
	SetPath(const IO::Path&);

	/*!
	\brief 遍历目录中的项目，更新至列表。
	\since build 414
	*/
	ListType
	ListItems() const;
};


/*!
\brief 下拉列表。
\note 顶层容器不可转换至 Panel 时忽略列表。
\since build 282
*/
class YF_API DropDownList : public Button
{
public:
	using ListType = ListBox::ListType;
	/*!
	\brief 视图参数类型。
	\sa TextList::ViewArgs
	\since build 283
	*/
	using ViewArgs = ListBox::ViewArgs;
	/*!
	\brief 视图事件委托类型。
	\sa TextList::HViewEvent
	\since build 283
	*/
	using HViewEvent = ListBox::HViewEvent;
	//! \since build 460
	using iterator = ystdex::subscriptive_iterator<DropDownList, IWidget>;

protected:
	//! \since build 391
	ListBox lbContent;

public:
	//! \since build 337
	explicit
	DropDownList(const Rect& = {}, const shared_ptr<ListType>& = {});
	inline DefDeMoveCtor(DropDownList)
	/*!
	\brief 析构：分离顶层子部件。
	\since build 283
	*/
	~DropDownList() override;

	//! \since build 356
	DefWidgetMemberIteration(lbContent)

	//! \since build 392
	DefGetterMem(const ynothrow, const ListType&, List, lbContent)
	//! \since build 392
	DefGetterMem(ynothrow, ListType&, ListRef, lbContent)
	/*!
	\brief 取视图变更事件。
	\since build 283
	*/
	DefEventGetterMem(ynothrow, HViewEvent, ViewChanged, lbContent)
	/*!
	\brief 取项目选择状态变更事件。
	\since build 283
	*/
	DefEventGetterMem(ynothrow, HIndexEvent, Selected, lbContent)
	/*!
	\brief 取项目选中确定事件。
	\since build 283
	*/
	DefEventGetterMem(ynothrow, HIndexEvent, Confirmed, lbContent)

	/*!
	\brief 设置文本列表。
	\since build 292
	*/
	DefSetterMem(const shared_ptr<ListType>&, List, lbContent)

	/*!
	\brief 按内容大小依次调整列表视图中选中和首个项目的索引。
	\warning 若视图大小变化后不调用此方法调整视图，可能导致选择项越界而行为未定义。
	\sa TextList::AdjustViewLength
	\since build 392
	*/
	PDefH(void, AdjustViewForContent, )
		ImplBodyMem(lbContent, AdjustViewForContent, )

private:
	/*!
	\brief 分离顶层子部件。
	\since build 282
	*/
	void
	DetachTopWidget();

public:
	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;
};

} // namespace UI;

} // namespace YSLib;

#endif

