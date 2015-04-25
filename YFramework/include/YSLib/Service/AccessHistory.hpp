/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file AccessHistory.hpp
\ingroup Service
\brief 访问历史记录。
\version r132
\author FrankHB <frankhb1989@gmail.com>
\since build 403
\par 创建时间:
	2013-05-12 07:42:35 +0800
\par 修改时间:
	2015-04-24 03:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::AccessHistory
*/


#ifndef YSL_INC_Service_AccessHistory_hpp_
#define YSL_INC_Service_AccessHistory_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition // for list;

namespace YSLib
{

/*!
\brief 访问列表。
\warning 非虚析构。
\since build 403
*/
template<typename _type>
class GAccessList
{
public:
	//! \brief 访问项：值类型。
	using ValueType = _type;
	//! \brief 记录列表类型。
	using ListType = list<_type>;

protected:
	//! \brief 列表记录：项的列表。
	ListType lstRecord;
	//! \brief 迭代器：指示当前项在记录中的插入位置。
	typename ListType::iterator itCurrent;

public:
	//! \brief 构造：迭代器指向记录末尾。
	GAccessList()
		: lstRecord(), itCurrent(lstRecord.end())
	{}
	DefDeCopyCtor(GAccessList)
	DefDeMoveCtor(GAccessList)

	DefPred(const, AtBegin, itCurrent == lstRecord.cbegin())
	DefPred(const, AtEnd, itCurrent == lstRecord.cend())

	DefGetter(const ynothrow, const ListType&, List, lstRecord)

	//! \brief 清除所有项。
	PDefH(void, Clear, )
		ImplExpr(lstRecord.clear(), itCurrent = lstRecord.end())

	//! \brief 清除迭代器之后的所有项并使迭代器指向末尾。
	void
	DropSubsequent()
	{
		lstRecord.erase(itCurrent, lstRecord.end());
		itCurrent = lstRecord.end();
	}

	//! \brief 插入项。
	template<typename... _tParams>
	void
	Emplace(_tParams&&... args)
	{
		lstRecord.emplace(itCurrent, yforward(args)...);
	}

	/*!
	\brief 切换项。
	\param is_prev 是否后退。
	\return 被移除的项。
	\warning 不检查是否越界。

	后退或前进，同时移除访问项。
	*/
	_type
	Switch(bool is_prev)
	{
		if(is_prev)
		{
			--itCurrent;
			--itCurrent;
		}

		auto res(std::move(*itCurrent));

		lstRecord.erase(itCurrent++);
		return res;
	}
};

} // namespace YSLib;

#endif

