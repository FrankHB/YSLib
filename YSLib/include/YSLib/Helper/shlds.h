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
\version 0.1947;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2011-05-17 08:05 +0800;
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

//标准命令行界面 Shell 。
class ShlCLI : public YShell
{
public:
	/*!
	\brief 无参数构造。
	*/
	ShlCLI();

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
	ExecuteCommand(const uchar_t*);
	/*!
	\brief 执行命令行。
	*/
	int
	ExecuteCommand(const String&);
};

inline
ShlCLI::ShlCLI()
	: YShell()
{}

inline int
ShlCLI::ExecuteCommand(const String& s)
{
	return ExecuteCommand(s.c_str());
}

YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

//双屏全屏窗口 Shell 。
class ShlDS : public Shells::YGUIShell
{
private:
	shared_ptr<Desktop> hDskUp, hDskDown; \
		//正常状态下应该总是指向有效的桌面对象。

public:
	/*!
	\brief 无参数构造。
	*/
	ShlDS(const shared_ptr<Desktop>& = GetGlobal().GetDesktopUpHandle(),
		const shared_ptr<Desktop>& = GetGlobal().GetDesktopDownHandle());

	DefGetter(const shared_ptr<Desktop>&, DesktopUpHandle, hDskUp)
	DefGetter(const shared_ptr<Desktop>&, DesktopDownHandle, hDskDown)
	DefGetter(Desktop&, DesktopUp, *hDskUp)
	DefGetter(Desktop&, DesktopDown, *hDskDown)

	/*!
	\brief Shell 处理函数。
	*/
	virtual int
	ShlProc(const Message&);

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
	\brief 发送绘制消息。
	*/
//	void
//	SendDrawingMessage();

	/*!
	\brief 更新到屏幕。
	*/
	virtual void
	UpdateToScreen();
};


//平台相关输入处理。

/*!
\brief 响应输入。
*/
void
ResponseInput(const Message&);


// Shell 快捷操作。

//! \ingroup HelperFunction
//@{

/*!
\brief 发送当前 Shell 预注销消息。
*/
inline void
NowShellInsertDropMessage(Messaging::Priority p = 0x80)
{
	SendMessage(Shells::GetCurrentShellHandle(), SM_DROP, p,
		new Messaging::Content(FetchShellHandle()));
}

//@}

YSL_END_NAMESPACE(DS)

YSL_END;

#endif

