/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Task.h
\ingroup Core
\brief 任务处理。
\version r104
\author FrankHB <frankhb1989@gmail.com>
\since build 449
\par 创建时间:
	2013-10-06 22:08:26 +0800
\par 修改时间:
	2013-12-24 09:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::Task
*/


#ifndef YSL_INC_Core_Task_h_
#define YSL_INC_Core_Task_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YApplication

namespace YSLib
{

/*!
\brief 通过消息队列部署后任务。
\since build 448
*/
template<typename _fCallable>
inline void
PostTask(_fCallable&& f, Messaging::Priority prior = Messaging::NormalPriority)
{
	PostMessage<SM_Task>(prior, yforward(f));
}

namespace Messaging
{

/*!
\brief 按更新条件和优先级通过消息队列部署任务。
\since build 455
*/
template<typename _fCallable>
void
Renew(_fCallable update, Priority prior = Messaging::NormalPriority)
{
	PostTask([=]{
		if(update())
			Messaging::Renew(update, prior);
	}, prior);
}

/*!
\brief 可执行更新操作的任务。
\since build 449
*/
template<typename _fCallable>
class GAutoTask
{
	static_assert(std::is_object<_fCallable>::value,
		"Callable object type is needed.");
public:
	//! \since build 450
	using Callable = _fCallable;

	_fCallable Update;
	Priority TaskPriority;

	//! \since build 451
	template<typename _fUpdater>
	GAutoTask(_fUpdater f, Priority prior = NormalPriority)
		: Update(f), TaskPriority(prior)
	{}

	virtual DefDeDtor(GAutoTask)

	//! \since build 450
	DefCvt(ynothrow, Callable&, *this)
	//! \since build 450
	DefCvt(const ynothrow, const Callable&, *this)
};

} // namespace Messaging;

} // namespace YSLib;

#endif

