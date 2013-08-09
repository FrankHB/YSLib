/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yexcept.h
\ingroup Core
\brief 异常处理模块。
\version r364
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-06-15 20:30:14 +0800
\par 修改时间:
	2013-08-05 21:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YException
*/


#ifndef YSL_INC_Core_yexcept_h_
#define YSL_INC_Core_yexcept_h_ 1

#include "ysdef.h"
#include <exception>
#include <stdexcept>
#include <string>

namespace YSLib
{

//! \brief YSLib 异常基类。
using Exception = std::exception;


//! \brief 一般运行时异常事件类。
using GeneralEvent = std::runtime_error;


//记录异常事件类。
class YF_API LoggedEvent : public GeneralEvent
{
public:
	using LevelType = RecordLevel;

private:
	LevelType level;

public:
	/*!
	\brief 构造：使用异常字符串和异常等级。
	\since build 432
	*/
	LoggedEvent(const std::string&, LevelType = Emergent) ynothrow;
	/*!
	\brief 构造：使用一般异常事件对象和异常等级。
	\since build 432
	*/
	LoggedEvent(const GeneralEvent&, LevelType = Emergent) ynothrow;
	/*!
	\brief 析构：默认实现。
	*/
	~LoggedEvent() ynothrow override = default;

	DefGetter(const ynothrow, LevelType, Level, level)
};


/*!
\brief 非日志记录的致命错误。
\since build 332
*/
class YF_API FatalError : GeneralEvent
{
private:
	const char* content;

public:
	//! \brief 构造：使用标题和内容。
	FatalError(const char*, const char*) ynothrow;

	DefGetter(const ynothrow, const char*, Content, content)
	DefGetter(const ynothrow, const char*, Title, what())
};

} // namespace YSLib;

#endif

