/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycounter.hpp
\ingroup Core
\brief 对象计数器。
\version 0.1698;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-09 20:43:52 +0800;
\par 修改时间:
	2011-06-24 20:44 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YCounter;
*/


#ifndef INCLUDED_CORE_YCOUNTER_H_
#define INCLUDED_CORE_YCOUNTER_H_

#include "ysdef.h"

YSL_BEGIN

//! \brief 模板类实例计数器。
template<class T, typename count_t = u32>
class GMCounter
{
private:
	static count_t nCount, nTotal; //!< 实例生成计数和实例计数。
	count_t nID; //!< 实例标识序列号。

public:
	/*!
	\brief 构造。
	\note 实例生成计数和实例计数自增。
	*/
	GMCounter()
		: nID(nCount++)
	{
		++nTotal;
	}
	/*!
	\brief 复制构造。
	\note 实例生成计数和实例计数自增。
	*/
	inline
	GMCounter(const GMCounter&)
		: nID(nCount++)
	{
		++nTotal;
	}
	/*!
	\brief 移动构造。
	\brief 移动构造：默认实现。
	*/
	inline
	GMCounter(GMCounter&&) = default;

protected:
	/*!
	\brief 析构。
	\note 实例计数自减。
	*/
	~GMCounter()
	{
		--nTotal;
	}

public:
	static DefMutableGetter(count_t, Count, nCount)
	static DefMutableGetter(count_t, Total, nTotal)
	DefGetter(count_t, ObjectID, nID)

protected:
	/*!
	\brief 复位实例生成计数。
	*/
	static void
	ResetCount()
	{
		nCount = 0;
	}

	/*!
	\brief 复位实例计数。
	*/
	static void
	ResetTotal()
	{
		nTotal = 0;
	}

	/*!
	\brief 复位实例标识序列号。
	*/
	void
	ResetID()
	{
		nID = 0;
	}
};

template<class T, typename count_t>
count_t GMCounter<T, count_t>::nCount(0);

template<class T, typename count_t>
count_t GMCounter<T, count_t>::nTotal(0);

YSL_END

#endif

