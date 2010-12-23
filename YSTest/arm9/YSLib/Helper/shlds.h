/*
	Copyright (C) by Franksoft 2010.

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
\version 0.1771;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-13 14:17:14 + 08:00;
\par 修改时间:
	2010-12-21 15:45 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::Shell_DS;
*/


#ifndef INCLUDED_SHLDS_H_
#define INCLUDED_SHLDS_H_

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
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual DefEmptyDtor(ShlCLI)

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


//标准图形用户界面窗口 Shell 。
class ShlGUI : public YGUIShell
{
public:
	ShlGUI();
	virtual
	~ShlGUI() ythrow();

	/*!
	\brief 处理停用消息。
	*/
	virtual int
	OnDeactivated(const Message&);

	/*!
	\brief 发送绘制消息。
	*/
	void
	SendDrawingMessage();

	/*!
	\brief 更新到屏幕。
	*/
	virtual void
	UpdateToScreen();
};

inline
ShlGUI::ShlGUI()
	: YGUIShell()
{}
inline
ShlGUI::~ShlGUI() ythrow()
{}

YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

//双屏全屏窗口 Shell 。
class ShlDS : public Shells::ShlGUI
{
protected:
	HWND hWndUp, hWndDown;

public:
	/*!
	\brief 无参数构造。
	*/
	ShlDS();
	DefEmptyDtor(ShlDS)

	DefGetter(HWND, UpWindowHandle, hWndUp)
	DefGetter(HWND, DownWindowHandle, hWndDown)

	/*!
	\brief Shell 处理函数。
	*/
	virtual int
	ShlProc(const Message&);

	/*!
	\brief 处理停用消息。
	*/
	virtual int
	OnDeactivated(const Message&);
};


//平台相关输入处理。

/*!
\brief 处理屏幕接触结束事件。
*/
inline void
OnTouchUp(const Components::Controls::TouchEventArgs::InputType& pt)
{
	Components::Controls::TouchEventArgs e(pt);

	ResponseTouchUp(*hDesktopDown, e);
}

/*!
\brief 处理屏幕接触开始事件。
*/
inline void
OnTouchDown(const Components::Controls::TouchEventArgs::InputType& pt)
{
	Components::Controls::TouchEventArgs e(pt);

	ResponseTouchDown(*hDesktopDown, e);
}

/*!
\brief 处理屏幕接触保持事件。
*/
inline void
OnTouchHeld(const Components::Controls::TouchEventArgs::InputType& pt)
{
	Components::Controls::TouchEventArgs e(pt);

	ResponseTouchHeld(*hDesktopDown, e);
}

/*!
\brief 处理键接触结束事件。
*/
inline void
OnKeyUp(const Components::Controls::KeyEventArgs::InputType& key)
{
	Components::Controls::KeyEventArgs e(key);

	ResponseKeyUp(*hDesktopDown, e);
}

/*!
\brief 处理键接触开始事件。
*/
inline void
OnKeyDown(const Components::Controls::KeyEventArgs::InputType& key)
{
	Components::Controls::KeyEventArgs e(key);

	ResponseKeyDown(*hDesktopDown, e);
}

/*!
\brief 处理键接触保持事件。
*/
inline void
OnKeyHeld(const Components::Controls::KeyEventArgs::InputType& key)
{
	Components::Controls::KeyEventArgs e(key);

	ResponseKeyHeld(*hDesktopDown, e);
}

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
	SendMessage(NULL, SM_DROP, p,
		new Messaging::GHandleContext<GHHandle<YShell> >(FetchShellHandle()));
}

/*!
\brief 清除屏幕内容。
*/
void
ShlClearBothScreen(GHHandle<YGUIShell> h = FetchGUIShellHandle());

/*!
\brief 当前 Shell 预注销。
\note 发送预注销消息后清除屏幕内容。
*/
inline void
NowShellDrop(Messaging::Priority p = 0x80)
{
	NowShellInsertDropMessage(p);
	ShlClearBothScreen();
}

//@}

YSL_END_NAMESPACE(DS)

YSL_END;

#endif

