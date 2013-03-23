/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file BookmarkUI.cpp
\ingroup YReader
\brief 书签界面。
\version r48
\author FrankHB <frankhb1989@gmail.com>
\since build 391
\par 创建时间:
	2013-03-20 22:10:55 +0800
\par 修改时间:
	2013-03-21 19:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::BookmarkUI
*/


#include "BookmarkUI.h"

YSL_BEGIN_NAMESPACE(YReader)

BookmarkPanel::BookmarkPanel(const BookmarkList& lst)
	: DialogPanel(Rect({}, MainScreenWidth, MainScreenHeight)),
	bookmarks(lst), lbPosition(Rect(32, 32, 192, 128))
{
	*this += lbPosition;
}

BookmarkList::size_type
BookmarkPanel::GetIndex() const
{
	return lbPosition.GetSelectedIndex();
}

YSL_END_NAMESPACE(YReader)

