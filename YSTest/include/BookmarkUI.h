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
\version r107
\author FrankHB <frankhb1989@gmail.com>
\since build 391
\par 创建时间:
	2013-03-20 22:11:46 +0800
\par 修改时间:
	2013-11-14 23:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::BookmarkUI
*/


#ifndef INC_YReader_BookmarkUI_h_
#define INC_YReader_BookmarkUI_h_ 1

#include "ReadingList.h"

namespace YReader
{

//! \since build 394
class ShlTextReader;


/*!
\brief 书签管理面板。
\since build 391
*/
class BookmarkPanel : public DialogPanel
{
	//! \since build 394
	friend class ShlTextReader;

protected:
	ListBox lbPosition;
	//! \since build 392
	Button btnAdd, btnRemove;

private:
	//! \since build 554
	lref<ShlTextReader> shell;
	/*!
	\since build 399
	\brief 当前打开文件的书签列表。
	*/
	BookmarkList bookmarks;

public:
	//! \since build 400
	BookmarkPanel(const BookmarkList&, ShlTextReader&);

	//! \since build 392
	BookmarkList::difference_type
	GetSelected() const;

	//! \since build 399
	void
	LoadBookmarks();
};

} // namespace YReader;

#endif

