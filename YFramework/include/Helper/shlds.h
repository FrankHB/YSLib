/*
	Copyright (C) by Franksoft 2010 - 2012.

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
\version r1205;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-13 14:17:14 +0800;
\par 修改时间:
	2012-08-30 20:05 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::Shell_DS;
*/


#ifndef INC_HELPER_SHLDS_H_
#define INC_HELPER_SHLDS_H_ 1

#include <YSLib/Core/yshell.h>
#include <YSLib/Core/ystring.h>
#include "Helper/InputManager.h"

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
	\brief 执行命令行。
	*/
	int
	ExecuteCommand(const ucs2_t*);
	/*!
	\brief 执行命令行。
	*/
	int
	ExecuteCommand(const String& s)
	{
		return ExecuteCommand(s.c_str());
	}
};

YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

/*!
\brief 双屏全屏窗口 Shell 。
\since 早于 build 132 。
*/
class ShlDS : public Shell
{
private:
	/*!
	\brief 输入管理器。
	\since build 323 。
	*/
	Devices::InputManager input_mgr;
	/*!
	\brief 共享桌面指针：正常状态下应总是指向可用的桌面对象。
	\since build 296 。
	*/
	shared_ptr<Desktop> desktop_up_ptr, desktop_down_ptr;

protected:
	/*!
	\brief 指定当前桌面是否需要更新。
	\note 默认可能被 OnGotMessage 和派生类重写的 OnPaint 改变状态。
	\since build 294 。
	*/
	//@{
	bool bUpdateUp, bUpdateDown;
	//@}

public:
	/*!
	\brief 构造：使用上下屏幕初始化对应桌面。
	\note 空参数表示新建。
	\since build 296 。
	*/
	ShlDS(const shared_ptr<Desktop>& = nullptr,
		const shared_ptr<Desktop>& = nullptr);

	DefGetter(const ynothrow, const shared_ptr<Desktop>&, DesktopUpHandle,
		desktop_up_ptr)
	DefGetter(const ynothrow, const shared_ptr<Desktop>&, DesktopDownHandle,
		desktop_down_ptr)
	DefGetter(const ynothrow, Desktop&, DesktopUp, *desktop_up_ptr)
	DefGetter(const ynothrow, Desktop&, DesktopDown, *desktop_down_ptr)

	/*!
	\brief 消息处理函数。
	\since build 317 。

	处理以下消息：
	绘制消息 SM_PAINT ，调用 ShlDS::OnInput 。
	输入消息 SM_INPUT ，分发按键处理后调用 OnInput 。
	*/
	void
	OnGotMessage(const Message&) override;

	/*!
	\brief 处理输入消息：发送绘制消息。
	\note 如需要异步更新可以覆盖本方法。
	\since build 289 。

	默认行为对桌面后 Validate 操作后调用 OnPaint ，再对桌面 Update 。
	*/
	virtual void
	OnInput();

	/*!
	\brief 处理绘制消息：空实现。
	\since build 288 。
	*/
	virtual void
	OnPaint();
};


/*!
\brief Shell 快捷操作。
\ingroup HelperFunctions
\since 早于 build 132 。
*/
//@{

//@}

YSL_BEGIN_NAMESPACE(Components)

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END;

#endif

