/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file BookmarkUI.h
\ingroup YReader
\brief 书签界面。
\version r76
\author FrankHB <frankhb1989@gmail.com>
\since build 391
\par 创建时间:
	2013-03-20 22:11:46 +0800
\par 修改时间:
	2013-03-27 01:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::BookmarkUI
*/


#ifndef INC_YReader_BookmarkUI_h_
#define INC_YReader_BookmarkUI_h_ 1

#include "ReadingList.h"

YSL_BEGIN_NAMESPACE(YReader)

/*!
\brief 书签管理面板。
\since build 391
*/
class BookmarkPanel : public DialogPanel
{
protected:
	ListBox lbPosition;
	//! \since build 392
	Button btnAdd, btnRemove;

private:
	//! \since build 393
	//@{
	std::function<Bookmark::PositionType()> get_reader_position;

public:
	BookmarkPanel(BookmarkList&, std::function<Bookmark::PositionType()>);
	//@}

	//! \since build 392
	BookmarkList::difference_type
	GetSelected() const;
};

YSL_END_NAMESPACE(YReader)

#endif

