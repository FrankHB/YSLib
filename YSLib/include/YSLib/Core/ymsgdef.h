/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ymsgdef.h
\ingroup Core
\brief 标准 Shell 消息列表。
\version r2902;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-08 12:05:26 +0800;
\par 修改时间:
	2011-09-16 02:56 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YMessageDefinition;
*/


#ifndef YSL_INC_CORE_YSMSGDEF_H_
#define YSL_INC_CORE_YSMSGDEF_H_

#include "ymsg.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

typedef enum MessageSpace
{
	Null = 0x0000,

	Set = 0x0003,
	Drop = 0x0004,
	Activated = 0x0005,
	Deactivated = 0x0006,

	Paint = 0x000F,
	Quit = 0x0012,

	Input = 0x4001

} MessageID;


#define SM_NULL					Messaging::Null
#define SM_SET					Messaging::Set
#define SM_DROP					Messaging::Drop
#define SM_ACTIVATED			Messaging::Activated
#define SM_DEACTIVATED			Messaging::Deactivated

#define SM_PAINT				Messaging::Paint
#define SM_QUIT					Messaging::Quit

#define SM_INPUT				Messaging::Input


template<MessageID _vID>
struct SMessageMap
{};

#define DefMessageTarget(_id, _type) \
	template<> \
	struct SMessageMap<_id> \
	{ \
		typedef _type TargetType; \
	};

DefMessageTarget(SM_NULL, void)
DefMessageTarget(SM_SET, shared_ptr<Shell>)
DefMessageTarget(SM_DROP, shared_ptr<Shell>)
DefMessageTarget(SM_ACTIVATED, shared_ptr<Shell>)
DefMessageTarget(SM_DEACTIVATED, shared_ptr<Shell>)
DefMessageTarget(SM_PAINT, shared_ptr<Desktop>)
DefMessageTarget(SM_QUIT, int)


template<MessageID _vID>
inline const typename SMessageMap<_vID>::TargetType&
FetchTarget(const Message& msg)
{
	return msg.GetContent().GetObject<typename SMessageMap<_vID>::TargetType>();
}

YSL_END_NAMESPACE(Messaging)

YSL_END

#endif

