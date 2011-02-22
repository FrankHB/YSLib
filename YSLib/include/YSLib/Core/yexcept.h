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
\version 0.1223;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-15 20:30:14 + 08:00;
\par 修改时间:
	2011-02-20 13:44 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YException;
*/


#ifndef INCLUDED_YEXCEPT_H_
#define INCLUDED_YEXCEPT_H_

#include "ysdef.h"
#include "../Adaptor/cont.h"
#include <new>
#include <typeinfo>

//#include <string>

YSL_BEGIN

//! \brief YSLib 异常基类。
class Exception : public std::exception
{
public:
	/*!
	\brief 无参数构造。
	*/
	Exception();
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~Exception() throw();
};


//! \brief 一般异常事件类。
class GeneralEvent : public Exception
{
private:
	string str;

public:
	/*!
	\brief 构造：使用异常字符串。
	*/
	GeneralEvent(const string&);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~GeneralEvent() throw();

	/*!
	\brief 取异常字符串。
	\note 无异常抛出。
	*/
	const char*
	what() const throw();
};


//记录异常事件类。
class LoggedEvent : public GeneralEvent
{
private:
	u8 level;

public:
	/*!
	\brief 构造：使用异常字符串和异常等级。
	*/
	LoggedEvent(const string&, u8 = 0);
	/*!
	\brief 构造：使用一般异常事件对象和异常等级。
	*/
	LoggedEvent(const GeneralEvent&, u8 = 0);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~LoggedEvent() throw();

	DefGetter(u8, Level, level);
};

YSL_END

#endif

