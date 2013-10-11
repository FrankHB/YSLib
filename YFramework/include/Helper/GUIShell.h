/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIShell.h
\ingroup Helper
\brief GUI Shell 类。
\version r106
\author FrankHB <frankhb1989@gmail.com>
\since build 399
\par 创建时间:
	2013-04-14 05:42:29 +0800
\par 修改时间:
	2013-10-11 01:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIShell
*/


#ifndef INC_Helper_GUIShell_h_
#define INC_Helper_GUIShell_h_ 1

#include "YSLib/Core/yshell.h"
#include "YSLib/Core/ystring.h"
#include "InputManager.h"

namespace YSLib
{

namespace Shells
{

/*!
\brief GUI Shell 基类。
\since build 398
*/
class YF_API GUIShell : public Shell
{
protected:
	/*!
	\brief 输入管理器。
	*/
	Devices::InputManager imMain;

public:
	/*!
	\brief 挂起延时时间。
	\since build 400
	*/
	std::chrono::nanoseconds IdleSleep{1000000};

	/*!
	\brief 无参数构造：复位 GUI 状态。
	*/
	GUIShell();

	/*!
	\brief 消息处理函数。

	处理以下消息：
	绘制消息 SM_PAINT ：调用 GUIShell::OnInput 。
	输入消息 SM_INPUT ：分发按键处理后调用 OnInput。
	其它消息传递至 Shell::OnGotMessage 。
	*/
	void
	OnGotMessage(const Message&) override;

	/*!
	\brief 处理输入消息：发送绘制消息。
	\note 如需要异步更新可以覆盖本方法。

	默认行为：调用 OnPaint ；若为宿主实现，更新宿主环境 GUI ，
		之后线程内延时 IdleSleep 。
	*/
	virtual void
	OnInput();

	/*!
	\brief 处理绘制消息：默认为空实现。
	*/
	virtual void
	OnPaint();
};

} // namespace Shells;

} // namespace YSLib;

#endif

