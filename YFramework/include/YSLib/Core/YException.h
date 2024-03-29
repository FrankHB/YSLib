﻿/*
	© 2010-2016, 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YException.h
\ingroup Core
\brief 异常处理模块。
\version r708
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2010-06-15 20:30:14 +0800
\par 修改时间:
	2022-02-26 23:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YException
*/


#ifndef YSL_INC_Core_YException_h_
#define YSL_INC_Core_YException_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YFunc // for function;
#include <exception> // for mandated header;
#include <stdexcept> // for mandated header;
#include YFM_YSLib_Adaptor_YTextBase // for string_view;
#include <string> // for std::string;
#include <ystdex/invoke.hpp> // for ystdex::nonvoid_result_t,
//	ystdex::invoke_result_t, ystdex::invoke_nonvoid;

namespace YSLib
{

//! \ingroup exceptions
//@{
//! \brief 一般运行时异常事件类。
using GeneralEvent = std::runtime_error;


//! \brief 记录日志的异常事件类。
class YF_API LoggedEvent : public GeneralEvent
{
private:
	//! \since build 624
	RecordLevel level;

public:
	/*!
	\brief 构造：使用异常字符串和记录等级。
	\since build 643
	*/
	//@{
	//! \pre 间接断言：第一参数非空。
	YB_NONNULL(2)
	LoggedEvent(const char* = "", RecordLevel = Emergent);
	//! \since build 921
	LoggedEvent(const std::string&, RecordLevel = Emergent);
	LoggedEvent(const string_view, RecordLevel = Emergent);
	//@}
	/*!
	\brief 构造：使用一般异常事件对象和记录等级。
	\since build 624
	*/
	LoggedEvent(const GeneralEvent&, RecordLevel = Emergent);
	//! \since build 586
	DefDeCopyCtor(LoggedEvent)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~LoggedEvent() override;

	//! \since build 624
	DefGetter(const ynothrow, RecordLevel, Level, level)
};


/*!
\brief 致命错误。
\note YSLib 库不直接捕获这个类和派生类的异常。
\since build 497
*/
class YF_API FatalError : public GeneralEvent
{
private:
	/*!
	\invariant \c content 。
	\since build 646
	\todo 使用引用计数实现的字符串。
	*/
	shared_ptr<string> content;

public:
	/*!
	\brief 构造：使用标题和内容。
	\pre 间接断言：第一参数和第二参数的数据指针非空。
	\note 复制字符串参数。
	\since build 646
	*/
	YB_NONNULL(2)
	FatalError(const char*, string_view);
	//! \since build 586
	DefDeCopyCtor(FatalError)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~FatalError() override;

	//! \since build 646
	DefGetter(const ynothrow, string_view, Content, Deref(content))
	DefGetter(const ynothrow, const char*, Title, what())
};
//@}


/*!
\brief 打印带有层次信息的函数类型。
\note 约定第一参数非空。
\since build 624
*/
using ExtractedLevelPrinter = function<void(const char*, size_t)>;
template<typename _type>
using GLevelTracer = function<void(_type, RecordLevel)>;
using ExceptionTracer = GLevelTracer<const std::exception&>;


//! \sa YF_TraceRaw
//@{
//! \note 若抛出异常，放弃层次信息，通过 YF_TraceRaw 打印错误消息。
//@{
/*!
\brief 打印带有层次信息的需要保证无异常抛出的错误。
\pre 间接断言：第二参数非空。
\since build 888

调用第一参数打印异常消息。
若打印抛出异常，通过 YF_TraceRaw 打印异常消息。
*/
YF_API YB_NONNULL(2) void
PrintCriticalFor(const ExtractedLevelPrinter&, const char*, RecordLevel = Err,
	size_t = 0) ynothrow;

/*!
\brief 通过 YF_TraceRaw 打印带空格缩进层次的异常信息的函数类型。
\since build 903
\pre 间接断言：第一参数非空。
*/
YF_API YB_NONNULL(1) void
PrintMessage(const char*, RecordLevel = Err, size_t level = 0) ynothrow;
//@}
//@}

/*!
\brief 追踪记录异常类型。
\since build 658
\todo 处理类型名称使之易读。

通过 YF_TraceRaw 打印第一参数的动态类型。
*/
YF_API void
TraceExceptionType(const std::exception&, RecordLevel = Err) ynothrow;

/*!
\brief 使用 PrintMessage 展开和追踪异常类型和信息。
\sa ExtractException
\sa PrintMessage
\sa TraceExceptionType
\since build 658

首先调用 TraceExceptionType ，然后调用 ExtractException 。
在 ExtractException 中调用 PrintMessage 。
*/
YF_API void
ExtractAndTrace(const std::exception&, RecordLevel = Err) ynothrow;

/*!
\brief 展开指定层次的异常并使用指定参数记录。
\sa PrintCriticalFor
\since build 888
*/
YF_API void
ExtractException(const ExtractedLevelPrinter&, const std::exception&,
	size_t = 0) ynothrow;

//! \return 是否发生并捕获异常。
//@{
/*!
\brief 执行并尝试记录异常。
\since build 829

对参数指定的函数求值，并使用最后一个参数追踪记录异常。
*/
template<typename _func>
bool
TryExecute(_func f, const char* desc, RecordLevel lv,
	const ExceptionTracer& trace)
{
	try
	{
		TryExpr(f())
		catch(...)
		{
			if(desc)
				YF_TraceRaw(Notice, "Exception filtered: %s.", desc);
			throw;
		}
		return {};
	}
	CatchExpr(std::exception& e, trace(e, lv))
	return true;
}

/*!
\brief 调用函数并尝试返回。
\since build 702
*/
template<typename _fCallable, typename... _tParams>
ystdex::nonvoid_result_t<ystdex::invoke_result_t<_fCallable, _tParams...>>
TryInvoke(_fCallable&& f, _tParams&&... args) ynothrow
{
	TryRet(ystdex::invoke_nonvoid(yforward(f), yforward(args)...))
	CatchExpr(std::exception& e, ExtractAndTrace(e))
	CatchExpr(..., YF_TraceRaw(Emergent, "Unknown exception found."))
	return {};
}

/*!
\brief 调用函数并过滤宿主异常。
\note 使用 ADL TryExecute 。
\since build 624

对参数指定的函数求值，并捕获和追踪记录所有异常。
若第二参数非空，则在捕获和追踪异常前，
	调用 YF_TraceRaw 以 Notice 记录等级接打印指示过滤的消息。
*/
template<typename _func>
bool
FilterExceptions(_func f, const char* desc = {}, RecordLevel lv = Alert,
	ExceptionTracer trace = ExtractAndTrace) ynothrow
{
	return !TryInvoke([=]{
		return !TryExecute(f, desc, lv, trace);
	});
}
//@}

} // namespace YSLib;

#endif

