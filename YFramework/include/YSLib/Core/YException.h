/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YException.h
\ingroup Core
\brief 异常处理模块。
\version r474
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2010-06-15 20:30:14 +0800
\par 修改时间:
	2015-04-13 03:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YException
*/


#ifndef YSL_INC_Core_yexcept_h_
#define YSL_INC_Core_yexcept_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition
#include <exception>
#include <stdexcept>
#include <string>

namespace YSLib
{

/*!
\ingroup exception_types
\brief 一般运行时异常事件类。
*/
using GeneralEvent = std::runtime_error;


/*!
\ingroup exception_types
\brief 记录日志的异常事件类。
*/
class YF_API LoggedEvent : public GeneralEvent
{
public:
	using LevelType = RecordLevel;

private:
	LevelType level;

public:
	/*!
	\brief 构造：使用异常字符串和异常等级。
	\since build 545
	*/
	LoggedEvent(const std::string& = "", LevelType = Emergent);
	/*!
	\brief 构造：使用一般异常事件对象和异常等级。
	\since build 545
	*/
	LoggedEvent(const GeneralEvent&, LevelType = Emergent);
	//! \since build 586
	DefDeCopyCtor(LoggedEvent)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~LoggedEvent() override;

	DefGetter(const ynothrow, LevelType, Level, level)
};


/*!
\ingroup exception_types
\brief 致命错误。
\since build 497
*/
class YF_API FatalError : public GeneralEvent
{
private:
	std::string content;

public:
	/*!
	\brief 构造：使用标题和内容。
	\since build 549
	*/
	FatalError(const std::string&, const std::string&);
	//! \since build 586
	DefDeCopyCtor(FatalError)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~FatalError() override;

	//! \since build 549
	DefGetter(const ynothrow, const std::string&, Content, content)
	DefGetter(const ynothrow, const char*, Title, what())
};


//! \since build 591
//@{
/*!
\brief 打印带有层次的异常信息的函数类型。
\warning 不保证检查第一个参数非空。
*/
using ExtractedExceptionPrinter
	= std::function<void(const char*, LoggedEvent::LevelType, size_t)>;


/*!
\brief 通过 YCL_TraceRaw 跟踪带空格缩进层次的异常信息的函数类型。
\pre 断言：第一参数非空。
*/
YF_API void
TraceException(const char*, LoggedEvent::LevelType = Err,
	size_t level = 0) ynothrow;

/*!
\brief 通过 YCL_TraceRaw 跟踪记录异常类型。
\todo 处理类型名称。
*/
YF_API void
TraceExceptionType(std::exception&, LoggedEvent::LevelType = Critical) ynothrow;


//! \brief 展开指定层次的异常并使用指定参数记录。
YF_API void
ExtractException(const ExtractedExceptionPrinter&,
	const std::exception&, LoggedEvent::LevelType = Err, size_t = 0) ynothrow;

/*!
\brief 执行并试图记录异常。

对参数指定的函数求值，并跟踪记录异常。
*/
YF_API void
TryExecute(std::function<void()>, const char* = {},
	LoggedEvent::LevelType = Alert);

/*!
\brief 过滤宿主异常。

对参数指定的函数求值，并捕获和跟踪记录所有异常。
*/
template<typename _func>
void
FilterExceptions(_func f, const char* desc = {},
	LoggedEvent::LevelType lv = Alert) ynothrow
{
	TryExpr(TryExecute(f, desc, lv))
	CatchExpr(std::exception& e, TraceExceptionType(e, Emergent))
	CatchExpr(..., YCL_TraceRaw(Emergent,
		"Unknown exception found @ FilterExceptions."))
}
//@}

} // namespace YSLib;

#endif

