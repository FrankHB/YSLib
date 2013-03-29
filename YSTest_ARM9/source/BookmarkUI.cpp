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
\version r127
\author FrankHB <frankhb1989@gmail.com>
\since build 391
\par 创建时间:
	2013-03-20 22:10:55 +0800
\par 修改时间:
	2013-03-29 12:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::BookmarkUI
*/


#include "BookmarkUI.h"
#include "ShlReader.h"

YSL_BEGIN_NAMESPACE(YReader)

namespace
{

using namespace std;

vector<String>
ConvertToUIString(const BookmarkList& lst)
{
	vector<String> vec;

	vec.reserve(lst.size());
	for(const auto i : lst)
		vec.push_back(String(to_string(i)));
	return vec;
}

BookmarkList
ConvertToBookmarkList(const vector<String>& lst)
{
	BookmarkList bookmarks;

	bookmarks.reserve(lst.size());
	for(const auto& str : lst)
		bookmarks.push_back(stoi(str.GetMBCS()));
	return bookmarks;
}

} // unnamed namespace;

BookmarkPanel::BookmarkPanel(BookmarkList& bookmarks, ShlTextReader& shl)
	: DialogPanel(Rect({}, MainScreenWidth, MainScreenHeight)),
	lbPosition(Rect(32, 32, 192, 128),
	share_raw(new vector<String>(std::move(ConvertToUIString(bookmarks))))),
	btnAdd(Rect(GetWidth() - 80, 4, 16, 16), 210),
	btnRemove(Rect(GetWidth() - 60, 4, 16, 16), 210), shell(shl)
{
	const auto stop_routing_after_direct([](KeyEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Bubble)
			e.Handled = true;
	});

	AddWidgets(*this, lbPosition, btnAdd, btnRemove),
	yunseq(
		btnAdd.Text = u"+",
		btnRemove.Text = u"-",
		FetchEvent<KeyDown>(lbPosition) += stop_routing_after_direct,
		FetchEvent<KeyHeld>(lbPosition) += stop_routing_after_direct,
		FetchEvent<Click>(btnOK) += [&, this](TouchEventArgs&&){
			bookmarks = std::move(ConvertToBookmarkList(lbPosition.GetList()));
		},
		FetchEvent<Click>(btnAdd) += [this](TouchEventArgs&&){
			auto& lst(lbPosition.GetListRef());
			auto idx(GetSelected());

			if(idx < 0)
				idx = lst.size();
			lst.insert(lst.begin() + idx,
				String(to_string(shell.get().GetReaderPosition())));
			lbPosition.AdjustViewForContent();
			lbPosition.UpdateView();
		},
		FetchEvent<Click>(btnRemove) += [this](TouchEventArgs&&){
			auto& lst(lbPosition.GetListRef());
			const auto idx(GetSelected());

			if(idx >= 0)
			{
				lst.erase(lst.begin() + idx);
				lbPosition.AdjustViewForContent();
				lbPosition.UpdateView();
			}
		}
	);
}

BookmarkList::difference_type
BookmarkPanel::GetSelected() const
{
	return lbPosition.IsSelected() ? lbPosition.GetSelectedIndex() : -1;
}

YSL_END_NAMESPACE(YReader)

