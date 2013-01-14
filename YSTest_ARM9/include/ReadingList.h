/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReadingList.h
\ingroup YReader
\brief 阅读列表。
\version r156
\author FrankHB <frankhb1989@gmail.com>
\since build 328
\par 创建时间:
	2012-07-24 22:13:46 +0800
\par 修改时间:
	2013-01-15 00:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReadingList
*/


#ifndef INC_YREADER_READINGLIST_H_
#define INC_YREADER_READINGLIST_H_ 1

#include "Shells.h"

YSL_BEGIN_NAMESPACE(YReader)

/*!
\brief 书签。
\warning 非虚析构。
\since build 286
*/
class BookMark
{
public:
	IO::Path Path;
	size_t Position;

	BookMark(const IO::Path& path, size_t pos)
		: Path(path), Position(pos)
	{}
	DefDeCopyCtor(BookMark)
	DefDeMoveCtor(BookMark)
};


/*!
\brief 判断书签是否相等。
\since build 286
*/
inline bool
operator==(const BookMark& x, const BookMark& y)
{
	return x.Path == y.Path && x.Position == y.Position;
}


/*!
\brief 阅读记录。
\warning 非虚析构。
\since build 286
*/
class ReadingList
{
public:
	/*!
	\brief 阅读记录列表类型。
	*/
	typedef list<BookMark> ListType;

private:
	/*!
	\brief 阅读记录：书签项列表。
	*/
	ListType reading_list;
	/*!
	\brief 阅读迭代器：当前浏览项在阅读记录中的插入位置。
	*/
	ListType::iterator now_reading;

public:
	/*!
	\brief 构造：阅读迭代器指向阅读记录末尾。
	*/
	ReadingList();
	DefDelCopyCtor(ReadingList)
	DefDelMoveCtor(ReadingList)

	//! \since build 373
	explicit
	operator ValueNode() const;

	/*!
	\brief 检查操作可用性。
	\return 后退和向前操作是否可用。
	*/
	pair<bool, bool>
	CheckBoundary();

	/*!
	\brief 清除所有记录。
	*/
	PDefH(void, Clear, )
		ImplExpr(reading_list.clear(), now_reading = reading_list.end())

	/*!
	\brief 清除当前迭代器之后的所有记录并使迭代器指向末尾。
	\since build 288
	*/
	void
	DropSubsequent();

	/*!
	\brief 插入阅读记录。
	*/
	void
	Insert(const IO::Path&, size_t);

	/*!
	\brief 切换阅读记录。
	\param 是否后退。
	\return 被移除的记录。
	\warning 不检查是否越界。

	后退或前进，同时移除该记录。
	*/
	BookMark
	Switch(bool);
};

YSL_END_NAMESPACE(YReader)

#endif

