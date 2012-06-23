/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yexcept.h
\ingroup Core
\brief 异常处理模块。
\version r1320;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-06-15 20:30:14 +0800;
\par 修改时间:
	2012-06-22 09:36 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YException;
*/


#ifndef YSL_INC_CORE_YEXCEPT_H_
#define YSL_INC_CORE_YEXCEPT_H_

#include "ysdef.h"
#include <exception>
#include <stdexcept>
#include <string>

YSL_BEGIN

//! \brief YSLib 异常基类。
typedef std::exception Exception;


//! \brief 一般运行时异常事件类。
typedef std::runtime_error GeneralEvent;


//记录异常事件类。
class LoggedEvent : public GeneralEvent
{
public:
	typedef u8 LevelType;

private:
	LevelType level;

public:
	/*!
	\brief 构造：使用异常字符串和异常等级。
	\since build 319 。
	*/
	LoggedEvent(const std::string&, LevelType = 0) ynothrow;
	/*!
	\brief 构造：使用一般异常事件对象和异常等级。
	\since build 319 。
	*/
	LoggedEvent(const GeneralEvent&, LevelType = 0) ynothrow;
	/*!
	\brief 析构：默认实现。
	*/
	~LoggedEvent() throw() override = default;

	DefGetter(const ynothrow, LevelType, Level, level)
};

YSL_END

#endif

