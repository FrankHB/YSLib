/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file InputManager.h
\ingroup Helper
\brief 输入管理器。
\version r1085;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2012-07-06 11:22:04 +0800;
\par 修改时间:
	2012-07-07 16:49 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::InputManager;
*/


#ifndef YSL_INC_HELPER_INPUGMANAGER_H_
#define YSL_INC_HELPER_INPUGMANAGER_H_

#include <YSLib/UI/ycomp.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Devices)

/*!
\brief 输入管理器。
\since build 323 。
*/
class InputManager : private noncopyable
{
private:
	/*!
	\brief 指针设备光标位置。
	\note 没有必要把输入内容以消息队列传输，因为总是有且仅有一个实例被同时处理。
	*/
	Drawing::Point cursor_state;

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

