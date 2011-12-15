/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file shlds.h
\ingroup Helper
\ingroup DS
\brief Shell 类库 DS 版本。
\version r2034;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2011-12-13 13:30 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Shell_DS;
*/


#ifndef YSL_INC_HELPER_SHLDS_H_
#define YSL_INC_HELPER_SHLDS_H_

#include "yshelper.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

/*!
\brief 标准命令行界面 Shell 。
\since build 147 。
*/
class ShlCLI : public Shell
{
public:
	/*!
	\brief 无参数构造。
	*/
	DefDeCtor(ShlCLI);

	/*!
	\brief 处理激活消息。
	*/
	virtual int
	OnActivated(const Message&);

	/*!
	\brief 处理停用消息。
	*/
	virtual int
	OnDeactivated(const Message&);

	/*!
	\brief 执行命令行。
	*/
	int
	ExecuteCommand(const ucs2_t*);
	/*!
	\brief 执行命令行。
	*/
	int
	ExecuteCommand(const String&);
};

inline int
ShlCLI::ExecuteCommand(const String& s)
{
	return ExecuteCommand(s.c_str());
}

YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

/*!
\brief 双屏全屏窗口 Shell 。
\since 早于 build 132 。
*/
class ShlDS : public Shells::GUIShell
{
private:
	shared_ptr<Desktop> hDskUp, hDskDown; \
		//正常状态下应总是指向可用的桌面对象。

public:
	/*!
	\brief 无参数构造。
	*/
	ShlDS(const shared_ptr<Desktop>&
		= FetchGlobalInstance().GetDesktopUpHandle(),
		const shared_ptr<Desktop>&
		= FetchGlobalInstance().GetDesktopDownHandle());

	DefGetter(const ynothrow, const shared_ptr<Desktop>&, DesktopUpHandle,
		hDskUp)
	DefGetter(const ynothrow, const shared_ptr<Desktop>&, DesktopDownHandle,
		hDskDown)
	DefGetter(const ynothrow, Desktop&, DesktopUp, *hDskUp)
	DefGetter(const ynothrow, Desktop&, DesktopDown, *hDskDown)

	/*!
	\brief 处理激活消息。
	*/
	virtual int
	OnActivated(const Message&);

	/*!
	\brief 处理停用消息。
	*/
	virtual int
	OnDeactivated(const Message&);

	/*!
	\brief 消息处理函数。
	*/
	virtual int
	OnGotMessage(const Message&);

	/*!
	\brief 更新到屏幕。
	*/
	virtual void
	UpdateToScreen();
};


/*!
\brief 响应输入。
*/
void
ResponseInput(const Message&);


/*!
\brief Shell 快捷操作。
\ingroup HelperFunctions
\since 早于 build 132 。
*/
//@{

/*!
\brief 发送当前 Shell 预注销消息。
\since build 175 。
*/
inline void
NowShellInsertDropMessage(Messaging::Priority p = 0x80)
{
	SendMessage<SM_DROP>(FetchShellHandle(), p, FetchShellHandle());
}

//@}

YSL_BEGIN_NAMESPACE(Components)

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_BEGIN_NAMESPACE(Shells)

/*!
\brief 主 Shell 。
\since 早于 build 132 。
*/
class MainShell : public DS::ShlDS
{
public:
	typedef ShlDS ParentType;

	Components::Label lblTitle, lblStatus, lblDetails;

	/*!
	\brief 无参数构造。
	*/
	MainShell();

	/*!
	\brief 处理激活消息。
	*/
	virtual int
	OnActivated(const Message&);

	/*!
	\brief 处理停用消息。
	*/
	virtual int
	OnDeactivated(const Message&);
};

YSL_END_NAMESPACE(Shells)

YSL_END;

#endif

