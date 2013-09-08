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
\version r198
\author FrankHB <frankhb1989@gmail.com>
\since build 391
\par 创建时间:
	2013-03-20 22:10:55 +0800
\par 修改时间:
	2013-09-07 02:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::BookmarkUI
*/


#include "BookmarkUI.h"
#include "ShlReader.h"

namespace YReader
{

namespace
{

using namespace std;

//! \since build 397
string
ConvertToUIString(Bookmark::PositionType pos, ShlTextReader& shl)
{
	auto line(shl.GetSlice(pos, 48U));

	if(line.size() > 1)
	{
		const bool b(line[0] == '\n');
		const auto i(line.find("\r\n", b));

		if(i != string::npos)
			line.erase(i);
		if(b)
			line.erase(line.begin());
	}
	return to_string(pos) + "  " + std::move(line);
}
//! \since build 397
vector<String>
ConvertToUIString(const BookmarkList& lst, ShlTextReader& shl)
{
	vector<String> vec;

	vec.reserve(lst.size());
	for(const auto pos : lst)
		vec.push_back(ConvertToUIString(pos, shl));
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

BookmarkPanel::BookmarkPanel(const BookmarkList& bm, ShlTextReader& shl)
	: DialogPanel(Size(MainScreenWidth, MainScreenHeight)),
	lbPosition({8, 32, 240, 128}),
	btnAdd(Rect(GetWidth() - 80, 4, 16, 16), 210),
	btnRemove(Rect(GetWidth() - 60, 4, 16, 16), 210), shell(shl), bookmarks(bm)
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
	FetchEvent<Click>(btnOK) += [this](CursorEventArgs&&){
		bookmarks = ConvertToBookmarkList(lbPosition.GetList());
	},
	FetchEvent<Click>(btnAdd) += [this](CursorEventArgs&&){
		auto& lst(lbPosition.GetListRef());
		auto idx(GetSelected());

		if(idx < 0)
			idx = lst.size();
		lst.insert(lst.begin() + idx,
			String(ConvertToUIString(shell.get().GetReaderPosition(), shell)));
		lbPosition.AdjustViewForContent();
		lbPosition.UpdateView();
	},
	FetchEvent<Click>(btnRemove) += [this](CursorEventArgs&&){
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

void
BookmarkPanel::LoadBookmarks()
{
	lbPosition.GetListRef() = ConvertToUIString(bookmarks, shell);
}

} // namespace YReader;

