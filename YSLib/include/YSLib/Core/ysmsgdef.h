/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ysmsgdef.h
\ingroup Core
\brief 标准 Shell 消息列表。
\version 0.2760;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-08 12:05:26 +0800;
\par 修改时间:
	2011-04-13 11:23 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellMessageDefinition;
*/


#ifndef INCLUDED_YSMSGDEF_H_
#define INCLUDED_YSMSGDEF_H_

#include "ysmsg.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Messaging)

#define DefMessageTypeMapping(_name, _tContext) \
	template<> \
	struct MessageTypeMapping<_name> \
	{ \
		typedef _tContext ContextType; \
	};


struct EStandard
{
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
};

template<typename _type>
class GObjectContext : implements IContext
{
public:
	_type Object;

	GObjectContext(_type o)
		: Object(o)
	{}

	ImplI1(IContext) bool
	operator==(const IContext& rhs) const
	{
		if(this == &rhs)
			return true;

		const GObjectContext* const
			pContext(dynamic_cast<const GObjectContext*>(&rhs));

		return pContext ? &this->Object == &pContext->Object : false;
	}
};


template<typename _type>
class GHandleContext : implements IContext
{
public:
	_type Handle;

	GHandleContext(_type h)
		: Handle(h)
	{}

	ImplI1(IContext) bool
	operator==(const IContext& rhs) const
	{
		if(this == &rhs)
			return true;

		const GHandleContext* const
			pContext(dynamic_cast<const GHandleContext*>(&rhs));

		return pContext ? this->Handle == pContext->Handle : false;
	}
};


class InputContext;


template<ID>
struct MessageTypeMapping
{};


DefMessageTypeMapping(EStandard::Null, IContext)
DefMessageTypeMapping(EStandard::Set, GHandleContext<GHandle<YShell> >)
DefMessageTypeMapping(EStandard::Drop, GHandleContext<GHandle<YShell> >)
DefMessageTypeMapping(EStandard::Activated, GHandleContext<GHandle<YShell> >)
DefMessageTypeMapping(EStandard::Deactivated, GHandleContext<GHandle<YShell> >)

DefMessageTypeMapping(EStandard::Paint, GHandleContext<GHandle<YDesktop> >)
DefMessageTypeMapping(EStandard::Quit, GObjectContext<int>)

DefMessageTypeMapping(EStandard::Input, InputContext)

/*!
\ingroup HelperFunction
\brief 使用 dynamic_cast 转换指定类型消息指针。
\note 需要确保 MessageEventTypeMapping 中有对应的 ID ，否则无法匹配此函数模板。
*/
template<ID id>
inline typename MessageTypeMapping<id>::ContextType*
CastMessage(IContext* c)
{
	return dynamic_cast<typename MessageTypeMapping<id>::ContextType*>(c);
}
/*!
\ingroup HelperFunction
\brief 使用 dynamic_cast 转换指定类型消息的消息指针。
\note 需要确保 MessageEventTypeMapping 中有对应的 ID ，否则无法匹配此函数模板。
*/
template<ID id>
typename MessageTypeMapping<id>::ContextType*
CastMessage(const Message& msg)
{
	return CastMessage<id>(GetPointer(msg.GetContextPtr()));
}


#define SM_NULL					Messaging::EStandard::Null
#define SM_SET					Messaging::EStandard::Set
#define SM_DROP					Messaging::EStandard::Drop
#define SM_ACTIVATED			Messaging::EStandard::Activated
#define SM_DEACTIVATED			Messaging::EStandard::Deactivated

#define SM_PAINT				Messaging::EStandard::Paint
#define SM_QUIT					Messaging::EStandard::Quit

#define SM_INPUT				Messaging::EStandard::Input

YSL_END_NAMESPACE(Messaging)

YSL_END

#endif

