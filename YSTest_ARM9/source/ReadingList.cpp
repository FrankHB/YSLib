/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReadingList.cpp
\ingroup YReader
\brief 阅读列表。
\version r144
\author FrankHB <frankhb1989@gmail.com>
\since build 328
\par 创建时间:
	2012-07-24 22:14:27 +0800
\par 修改时间:
	2014-01-28 05:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReadingList
*/


#include "ReadingList.h"

namespace YReader
{

Bookmark::operator ValueNode::Container() const
{
	return {MakeNode("Path", string(Path)),
		StringifyToNode("Position", Position)};
}


ReadingList::operator ValueNode::Container() const
{
	ValueNode::Container con;

	for(const auto& bm : GetList())
		con.insert(MakeNode("bm" + to_string(con.size()),
			ValueNode::Container(bm)));
	return con;
}

void
ReadingList::Insert(const IO::Path& pth, size_t pos)
{
	Emplace(pth, pos);
}

} // namespace YReader;

