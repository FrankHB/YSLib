/*
	© 2010-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file shlds.h
\ingroup Helper
\ingroup DS
\brief DS 平台 Shell 类。
\version r1331
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-13 14:17:14 +0800
\par 修改时间:
	2013-10-23 19:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Shell_DS
*/


#ifndef INC_Helper_shlds_h_
#define INC_Helper_shlds_h_ 1

#include "Helper/GUIShell.h"

namespace YSLib
{

namespace Shells
{

/*!
\brief 标准命令行界面 Shell 。
\since build 147
*/
class YF_API ShlCLI : public Shell
{
public:
	/*!
	\brief 无参数构造。
	*/
	DefDeCtor(ShlCLI)

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

} // namespace Shells;


namespace DS
{

/*!
\brief 以默认屏幕复位两个桌面。
\since build 396
*/
YF_API void
ResetDSDesktops(Desktop&, Desktop&);


/*!
\brief 双屏全屏窗口 Shell 。
\since 早于 build 132
*/
class YF_API ShlDS : public Shells::GUIShell
{
private:
	/*!
	\brief 共享桌面指针：正常状态下应总是指向可用的桌面对象。
	\since build 296
	*/
	shared_ptr<Desktop> main_desktop_ptr, sub_desktop_ptr;
	/*!
	\brief 指针设备响应的桌面指针：总是指向下屏对应的桌面。
	\since build 429
	*/
	shared_ptr<Desktop> cursor_desktop_ptr;

protected:
	/*!
	\brief 指定当前桌面是否需要更新。
	\note 默认可能被 OnGotMessage 和派生类重写的 OnPaint 改变状态。
	\since build 294
	*/
	//@{
	bool bUpdateUp, bUpdateDown;
	//@}

public:
	/*!
	\brief 构造：使用指定上下桌面。
	\note 空参数表示新建上下屏幕初始化对应的桌面。
	\since build 296
	*/
	ShlDS(const shared_ptr<Desktop>& = {}, const shared_ptr<Desktop>& = {});

	//! \since build 429
	//@{
	DefGetter(const ynothrow, const shared_ptr<Desktop>&, CursorDesktopHandle,
		main_desktop_ptr)
	DefGetter(const ynothrow, Desktop&, CursorDesktop, *cursor_desktop_ptr)
	DefGetter(const ynothrow, const shared_ptr<Desktop>&, MainDesktopHandle,
		main_desktop_ptr)
	DefGetter(const ynothrow, Desktop&, MainDesktop, *main_desktop_ptr)
	DefGetter(const ynothrow, const shared_ptr<Desktop>&, SubDesktopHandle,
		sub_desktop_ptr)
	DefGetter(const ynothrow, Desktop&, SubDesktop, *sub_desktop_ptr)
	//@}

	/*!
	\brief 消息处理函数。
	\since build 317

	处理以下消息：
	绘制消息 SM_Paint ：调用 ShlDS::OnInput 。
	输入消息 SM_Input ：分发按键处理后调用 OnInput。
	其它消息传递至 GUIShell::OnGotMessage 。
	*/
	void
	OnGotMessage(const Message&) override;

	/*!
	\brief 处理输入消息：发送绘制消息。
	\sa GUIShell::OnInput
	\since build 289

	默认行为：对桌面后 Validate 操作后调用 OnPaint ，再对桌面 Update ，
		对宿主实现再调用 GUIShell::OnInput 。
	*/
	void
	OnInput() override;

	//! \since build 429
	//@{
	//! \brief 交换桌面。
	void
	SwapDesktops();

	//! \brief 交换屏幕。
	void
	SwapScreens();

	/*!
	\brief 包装指定的部件处理满足指定按键掩码的 KeyDown 事件：交换屏幕。
	\note 忽略 Bubble 路由事件；设置 e.Handled 。
	\note 事件优先级 0xE0 。
	*/
	void
	WrapForSwapScreens(UI::IWidget&, KeyInput&);
	//@}
};


/*!
\brief Shell 快捷操作。
\ingroup helper_functions
\since 早于 build 132
*/
//@{

//@}

namespace UI
{

} // namespace UI;

} // namespace DS;

} // namespace YSLib;

#endif

