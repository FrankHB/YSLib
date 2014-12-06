/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycounter.hpp
\ingroup Core
\brief 对象计数器。
\version r764
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-02-09 20:43:52 +0800
\par 修改时间:
	2014-12-02 18:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YCounter
*/


#ifndef YSL_INC_Core_ycounter_hpp_
#define YSL_INC_Core_ycounter_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition

namespace YSLib
{

/*!
\brief 模板类实例计数器。
\since 早于 build 132
*/
template<class _type, typename count_t = std::uint32_t>
class GMCounter
{
private:
	static count_t nCount, nTotal; //!< 实例生成计数和实例计数。
	count_t nID; //!< 实例标识序列号。

public:
	/*!
	\brief 构造。
	\note 实例生成计数和实例计数自增。
	\since build 319
	*/
	GMCounter() ynothrow
		: nID(nCount++)
	{
		++nTotal;
	}
	/*!
	\brief 复制构造。
	\note 实例生成计数和实例计数自增。
	\since build 319
	*/
	inline
	GMCounter(const GMCounter&) ynothrow
		: nID(nCount++)
	{
		++nTotal;
	}
	/*!
	\brief 转移构造。
	\brief 转移构造：默认实现。
	*/
	DefDeMoveCtor(GMCounter)

protected:
	/*!
	\brief 析构。
	\note 实例计数自减。
	\since build 461
	*/
	~GMCounter()
	{
		--nTotal;
	}

public:
	//! \warning 不保证线程安全性。
	//@{
	static DefGetter(ynothrow, count_t, Count, nCount)
	static DefGetter(ynothrow, count_t, Total, nTotal)
	//@}
	yconstfn DefGetter(const ynothrow, count_t, ObjectID, nID)

protected:
	/*!
	\brief 复位实例生成计数。
	\since build 319
	*/
	static void
	ResetCount() ynothrow
	{
		nCount = 0;
	}

	/*!
	\brief 复位实例计数。
	\since build 319
	*/
	static void
	ResetTotal() ynothrow
	{
		nTotal = 0;
	}

	/*!
	\brief 复位实例标识序列号。
	\since build 319
	*/
	void
	ResetID() ynothrow
	{
		nID = 0;
	}
};

template<class _type, typename count_t>
count_t GMCounter<_type, count_t>::nCount(0);

template<class _type, typename count_t>
count_t GMCounter<_type, count_t>::nTotal(0);

} // namespace YSLib;

#endif

