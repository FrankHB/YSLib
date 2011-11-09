/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file listbox.h
\ingroup UI
\brief 样式相关的图形用户界面列表框控件。
\version r3220;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:30:40 +0800;
\par 修改时间:
	2011-11-05 10:45 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::ListBox;
*/


#ifndef YSL_INC_UI_LISTBOX_H_
#define YSL_INC_UI_LISTBOX_H_

#include "textlist.h"
#include "scroll.h"
#include "../Core/yfilesys.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 带滚动条的文本列表框。
*/
class ListBox : public ScrollableContainer
{
public:
	typedef TextList::ListType ListType;
	typedef TextList::ViewerType ViewerType;

private:
	TextList TextListBox; //文本列表框。

public:
	explicit
	ListBox(const Rect& = Rect::Empty,
		const shared_ptr<ListType>& = shared_ptr<ListType>());
	inline DefDeMoveCtor(ListBox)

	DefPredicateMember(Selected, TextListBox)
	PDefH(bool, Contains, ViewerType::SizeType i)
		ImplBodyMember(TextListBox, Contains, i)

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	*/
	virtual IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));
	DefGetterMember(ViewerType::SizeType, HeadIndex, TextListBox)
	DefGetterMember(ViewerType::SizeType, SelectedIndex, TextListBox)
	DefGetterMember(ListType&, List, TextListBox)
	DefMutableEventGetterMember(HUIEvent, ViewChanged, TextListBox) \
		//!< 视图变更事件。
	DefMutableEventGetterMember(HIndexEvent, Selected, TextListBox) \
		//!< 项目选择状态变更事件。
	DefMutableEventGetterMember(HIndexEvent, Confirmed, TextListBox) \
		//!< 项目选中确定事件。

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);

	PDefH(void, ResetView)
		ImplBodyMember(TextListBox, ResetView)
	PDefH(void, UpdateView)
		ImplBodyMember(TextListBox, UpdateView)
};


//! \brief 文件列表框。
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
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

