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
\version r59
\author FrankHB <frankhb1989@gmail.com>
\since build 391
\par 创建时间:
	2013-03-20 22:11:46 +0800
\par 修改时间:
	2013-03-21 19:38 +0800
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
private:
	std::reference_wrapper<const BookmarkList> bookmarks;

protected:
	ListBox lbPosition;

public:
	BookmarkPanel(const BookmarkList&);

	BookmarkList::size_type
	GetIndex() const;
};

YSL_END_NAMESPACE(YReader)

#endif

