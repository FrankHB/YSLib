/*
	© 2013-2015, 2018-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file BookmarkUI.cpp
\ingroup YReader
\brief 书签界面。
\version r232
\author FrankHB <frankhb1989@gmail.com>
\since build 391
\par 创建时间:
	2013-03-20 22:10:55 +0800
\par 修改时间:
	2019-07-07 23:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::BookmarkUI
*/


#include "BookmarkUI.h" // for to_std_string, stoul;
#include "ShlReader.h"

namespace YReader
{

namespace
{

//! \since build 835
String
ConvertToUIString(Bookmark::PositionType pos, ShlTextReader& shl)
{
	auto line(shl.GetSlice(pos, 48U));

	if(line.size() > 1)
	{
		const bool b(line[0] == u'\n');
		const auto i(line.find(u"\r\n", b));

		if(i != string::npos)
			line.erase(i);
		if(b)
			// XXX: Use cbegin after libstdc++ bug resolved.
			//	See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60278.
			line.erase(line.begin());
	}
	// TODO: Use %to_u16string or %to_chars?
	return String(to_string(pos)) + u"  " + std::move(line);
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
		bookmarks.push_back(stoul(to_std_string(str.GetMBCS())));
	return bookmarks;
}

} // unnamed namespace;

BookmarkPanel::BookmarkPanel(const BookmarkList& bm, ShlTextReader& shl)
	: DialogPanel(Size(MainScreenWidth, MainScreenHeight)),
	// XXX: Conversion to 'SPos' might be implementation-defined.
	lbPosition({8, 32, 240, 128}),
	btnAdd(Rect(SPos(GetWidth()) - 80, 4, 16, 16), 210),
	btnRemove(Rect(SPos(GetWidth()) - 60, 4, 16, 16), 210), shell(shl),
	bookmarks(bm)
{
	const auto stop_routing_after_direct([](KeyEventArgs&& e) ynothrow{
		if(e.Strategy == RoutedEventArgs::Bubble)
			e.Handled = true;
	});

	AddWidgets(*this, lbPosition, btnAdd, btnRemove),
	yunseq(
	btnAdd.Text = u"+",
	btnRemove.Text = u"-",
	FetchEvent<KeyDown>(lbPosition) += stop_routing_after_direct,
	FetchEvent<KeyHeld>(lbPosition) += stop_routing_after_direct,
	FetchEvent<Click>(btnOK) += [this]{
		bookmarks = ConvertToBookmarkList(lbPosition.GetList());
	},
	FetchEvent<Click>(btnAdd) += [this]{
		auto& lst(lbPosition.GetListRef());
		auto idx(GetSelected());

		if(idx < 0)
			// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
			idx = ptrdiff_t(lst.size());
		lst.insert(lst.begin() + idx,
			String(ConvertToUIString(shell.get().GetReaderPosition(), shell)));
		lbPosition.AdjustViewForContent();
		lbPosition.UpdateView();
	},
	FetchEvent<Click>(btnRemove) += [this]{
		auto& lst(lbPosition.GetListRef());
		const auto idx(GetSelected());

		if(idx >= 0)
		{
			lst.erase(lst.cbegin() + idx);
			lbPosition.AdjustViewForContent();
			lbPosition.UpdateView();
		}
	}
	);
}
ImplDeDtor(BookmarkPanel)

BookmarkList::difference_type
BookmarkPanel::GetSelected() const
{
	return lbPosition.IsSelected()
		? BookmarkList::difference_type(lbPosition.GetSelectedIndex()) : -1;
}

void
BookmarkPanel::LoadBookmarks()
{
	lbPosition.GetListRef() = ConvertToUIString(bookmarks, shell);
}

} // namespace YReader;

