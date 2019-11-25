/*
	© 2009-2013, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YMessageDefinition.h
\ingroup Core
\brief 标准 Shell 消息列表。
\version r1983
\author FrankHB <frankhb1989@gmail.com>
\since build 217
\par 创建时间:
	2009-12-08 12:05:26 +0800
\par 修改时间:
	2019-11-25 22:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YMessageDefinition
*/


#ifndef YSL_INC_Core_YMessageDefinition_h_
#define YSL_INC_Core_YMessageDefinition_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YMessage // for shared_ptr, pair, function;

namespace YSLib
{

namespace Messaging
{

using MessageID = enum MessageSpace
{
	Null = 0x0000,
	Set = 0x0003,

	Quit = 0x0012,
	//! \since build 454
	Bound = 0x0014,
	Task = 0x0016,

	Paint = 0x00AF,

	Input = 0x00FF
};


//! \since build 454
//@{
#define SM_Null			YSLib::Messaging::Null
#define SM_Set			YSLib::Messaging::Set

#define SM_Quit			YSLib::Messaging::Quit
#define SM_Bound		YSLib::Messaging::Bound
#define SM_Task			YSLib::Messaging::Task

#define SM_Paint		YSLib::Messaging::Paint
#define SM_Input		YSLib::Messaging::Input
//@}


template<MessageID _vID>
struct SMessageMap
{};

#define DefMessageTarget(_id, _type) \
	template<> \
	struct SMessageMap<_id> \
	{ \
		using TargetType = _type; \
	};

//! \since build 454
//@{
DefMessageTarget(SM_Null, void)
DefMessageTarget(SM_Set, shared_ptr<Shell>)
DefMessageTarget(SM_Quit, int)
DefMessageTarget(SM_Bound, pair<weak_ptr<Shell> YPP_Comma Message>)
DefMessageTarget(SM_Task, function<void()>)
DefMessageTarget(SM_Input, void)
//@}


template<MessageID _vID>
inline const typename SMessageMap<_vID>::TargetType&
FetchTarget(const Message& msg)
{
	return msg.GetContent().GetObject<typename SMessageMap<_vID>::TargetType>();
}

} // namespace Messaging;

} // namespace YSLib;

#endif

