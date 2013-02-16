/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file InputManager.h
\ingroup Helper
\brief 输入管理器。
\version r110
\author FrankHB <frankhb1989@gmail.com>
\since build 323
\par 创建时间:
	2012-07-06 11:22:04 +0800
\par 修改时间:
	2013-02-13 13:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::InputManager
*/


#ifndef INC_Helper_InputManager_h_
#define INC_Helper_InputManager_h_ 1

#include "Helper/yglobal.h"
#include "YSLib/UI/ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Devices)

/*!
\brief 输入管理器。
\since build 323
*/
class YF_API InputManager : private noncopyable
{
private:
	/*!
	\brief 指针设备光标位置。
	\note 没有必要把输入内容以消息队列传输，因为总是有且仅有一个实例被同时处理。
	*/
	Drawing::Point cursor_state;
#if YCL_HOSTED
	/*!
	\brief 宿主环境。
	\since build 381
	*/
	std::reference_wrapper<Host::Environment> env;
#endif

public:
	InputManager();

	/*!
	\brief 向指定桌面分发响应输入状态。

	指定平台相关的用户界面输入处理。
	*/
	void
	DispatchInput(Desktop&);
};

YSL_END_NAMESPACE(Devices)

YSL_END

#endif

