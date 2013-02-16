/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReadingList.cpp
\ingroup YReader
\brief 阅读列表。
\version r95
\author FrankHB <frankhb1989@gmail.com>
\since build 328
\par 创建时间:
	2012-07-24 22:14:27 +0800
\par 修改时间:
	2013-02-14 15:12 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReadingList
*/


#include "ReadingList.h"

YSL_BEGIN_NAMESPACE(YReader)

ReadingList::ReadingList()
	: reading_list(), now_reading(reading_list.end())
{}

ReadingList::operator ValueNode() const
{
	auto p(make_unique<ValueNode::Container>());

	for(const auto& bm : reading_list)
		p->insert(MakeNode("bm" + to_string(p->size()),
			ValueNode::Container{MakeNode("Path", bm.Path.GetNativeString()),
			StringifyToNode("Position", bm.Position)}));

	auto&& vn(MakeNode("ReadingList", p.get(), PointerTag()));

	p.release();
	return vn;
}

pair<bool, bool>
ReadingList::CheckBoundary()
{
	return make_pair(now_reading != reading_list.begin(),
		now_reading != reading_list.end());
}

void
ReadingList::DropSubsequent()
{
	reading_list.erase(now_reading, reading_list.end());
	now_reading = reading_list.end();
}

void
ReadingList::Insert(const IO::Path& pth, size_t pos)
{
	reading_list.emplace(now_reading, pth, pos);
}

BookMark
ReadingList::Switch(bool is_prev)
{
	if(is_prev)
	{
		--now_reading;
		--now_reading;
	}

	auto ret(std::move(*now_reading));

	reading_list.erase(now_reading++);
	return ret;
}

YSL_END_NAMESPACE(YReader)

