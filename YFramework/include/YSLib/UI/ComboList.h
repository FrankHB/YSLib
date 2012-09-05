/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ComboList.h
\ingroup UI
\brief 样式相关的图形用户界面组合列表控件。
\version r2406
\author FrankHB<frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-07 20:30:40 +0800
\par 修改时间:
	2012-09-04 12:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::ComboList
*/


#ifndef YSL_INC_UI_COMBOLIST_H_
#define YSL_INC_UI_COMBOLIST_H_ 1

#include "textlist.h"
#include "scroll.h"
#include "../Core/yfilesys.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 带滚动条的文本列表框。
\since build 205
\todo 实现 Resize 事件调整内容布局。
*/
class ListBox : public ScrollableContainer
{
public:
	typedef TextList::ListType ListType;
	typedef TextList::ViewArgs ViewArgs;
	typedef TextList::HViewEvent HViewEvent;
	//! \since build 316
	typedef TextList::IndexType IndexType;
	//! \since build 316
	typedef TextList::ItemType ItemType;

protected:
	/*
	\brief 文本列表。
	\since build 281
	*/
	TextList lstText;

public:
	explicit
	ListBox(const Rect& = Rect::Empty,
		const shared_ptr<ListType>& = shared_ptr<ListType>());
	inline DefDeMoveCtor(ListBox)

	DefPredMem(const ynothrow, Selected, lstText)
	PDefH(bool, Contains, ListType::size_type i)
		ImplBodyMem(lstText, Contains, i)

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	*/
	IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&)) override;
	DefGetterMem(const ynothrow, ListType::size_type, HeadIndex, lstText)
	DefGetterMem(const ynothrow, ListType::size_type, SelectedIndex,
		lstText)
	DefGetterMem(const ynothrow, ListType&, List, lstText)
	DefEventGetterMem(ynothrow, HViewEvent, ViewChanged, lstText) \
		//!< 视图变更事件。
	DefEventGetterMem(ynothrow, HIndexEvent, Selected, lstText) \
		//!< 项目选择状态变更事件。
	DefEventGetterMem(ynothrow, HIndexEvent, Confirmed, lstText) \
		//!< 项目选中确定事件。

	/*!
	\brief 设置文本列表。
	\since build 292
	*/
	DefSetterMem(const shared_ptr<ListType>&, List, lstText)
	/*!
	\brief 设置选中项。
	\see TextList::SetSelected 。
	\since build 285。
	*/
	PDefH(void, SetSelected, ListType::size_type i)
		ImplBodyMem(lstText, SetSelected, i)

	/*!
	\brief 调整视图长度。
	\see TextList::AdjustViewLength 。
	\since build 285
	*/
	PDefH(void, AdjustViewLength)
		ImplBodyMem(lstText, AdjustViewLength)

	/*!
	\brief 清除选中项。
	\see TextList::ClearSelected 。
	\since build 285
	*/
	PDefH(void, ClearSelected)
		ImplBodyMem(lstText, ClearSelected)

	/*!
	\brief 查找项。
	\see TextList::Find 。
	\since build 316
	*/
	PDefH(IndexType, Find, const ItemType& text)
		ImplBodyMem(lstText, Find, text)

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;

	PDefH(void, ResetView)
		ImplBodyMem(lstText, ResetView)

	/*!
	\brief 按指定大小上限和内容调整大小。
	\param sup 指定调整的大小的上限，当为 Size::Zero 时忽略大小限制。
	\note s 指定大小，分量为 0 时对应分量大小由内容确定（其中宽度含边距）。
	\since build 282
	*/
	void
	ResizeForPreferred(const Size& sup, Size s = Size::Zero);

	PDefH(void, UpdateView)
		ImplBodyMem(lstText, UpdateView)
};


/*!
\brief 文件列表框。
\since build 205
*/
class FileBox : public IO::FileList, public ListBox
{
public:
	explicit
	FileBox(const Rect& = Rect::Empty);
	inline DefDeMoveCtor(FileBox)

	/*!
	\brief 取当前路径。
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
};


/*!
\brief 下拉列表。
\note 顶层容器不可转换至 Panel 时忽略列表。
\since build 282
*/
class DropDownList : public Button
{
public:
	typedef ListBox::ListType ListType;
	/*!
	\brief 视图参数类型。
	\see TextList::ViewArgs 。
	\since build 283
	*/
	typedef ListBox::ViewArgs ViewArgs;
	/*!
	\brief 视图事件委托类型。
	\see TextList::HViewEvent 。
	\since build 283
	*/
	typedef ListBox::HViewEvent HViewEvent;

protected:
	ListBox boxList;

public:
	explicit
	DropDownList(const Rect& = Rect::Empty,
		const shared_ptr<ListType>& = shared_ptr<ListType>());
	inline DefDeMoveCtor(DropDownList)
	/*!
	\brief 析构：分离顶层子部件。
	\since build 283
	*/
	~DropDownList() override;

	DefGetterMem(const ynothrow, ListType&, List, boxList)
	/*!
	\brief 取视图变更事件。
	\since build 283
	*/
	DefEventGetterMem(ynothrow, HViewEvent, ViewChanged, boxList)
	/*!
	\brief 取项目选择状态变更事件。
	\since build 283
	*/
	DefEventGetterMem(ynothrow, HIndexEvent, Selected, boxList)
	/*!
	\brief 取项目选中确定事件。
	\since build 283
	*/
	DefEventGetterMem(ynothrow, HIndexEvent, Confirmed, boxList)

	/*!
	\brief 设置文本列表。
	\since build 292
	*/
	DefSetterMem(const shared_ptr<ListType>&, List, boxList)

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

YSL_END_NAMESPACE(Components)

YSL_END

#endif

