/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsgdef.h
\ingroup Core
\brief 标准 Shell 消息列表。
\version r1951
\author FrankHB <frankhb1989@gmail.com>
\since build 217
\par 创建时间:
	2009-12-08 12:05:26 +0800
\par 修改时间:
	2013-08-05 21:06 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YMessageDefinition
*/


#ifndef YSL_INC_CORE_ymsgdef_h_
#define YSL_INC_CORE_ymsgdef_h_ 1

#include "ymsg.h"
#include <functional> // for std::function;

namespace YSLib
{

namespace Messaging
{

using MessageID = enum MessageSpace
{
	Null = 0x0000,
	Set = 0x0003,

	Paint = 0x000F,

	Quit = 0x0012,
	Task = 0x0016,

	Input = 0x00FF

};


#define SM_NULL					Messaging::Null
#define SM_SET					Messaging::Set

#define SM_PAINT				Messaging::Paint
#define SM_QUIT					Messaging::Quit

#define SM_TASK					Messaging::Task

#define SM_INPUT				Messaging::Input


template<MessageID _vID>
struct SMessageMap
{};

#define DefMessageTarget(_id, _type) \
	template<> \
	struct SMessageMap<_id> \
	{ \
		using TargetType = _type; \
	};

DefMessageTarget(SM_NULL, void)
DefMessageTarget(SM_SET, shared_ptr<Shell>)
DefMessageTarget(SM_QUIT, int)
DefMessageTarget(SM_TASK, std::function<void()>)
DefMessageTarget(SM_INPUT, void)


template<MessageID _vID>
inline const typename SMessageMap<_vID>::TargetType&
FetchTarget(const Message& msg)
{
	return msg.GetContent().GetObject<typename SMessageMap<_vID>::TargetType>();
}

} // namespace Messaging;

} // namespace YSLib;

#endif

