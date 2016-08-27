/*
	© 2010-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YException.h
\ingroup Core
\brief 异常处理模块。
\version r615
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2010-06-15 20:30:14 +0800
\par 修改时间:
	2016-08-27 15:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YException
*/


#ifndef YSL_INC_Core_yexcept_h_
#define YSL_INC_Core_yexcept_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition
#include <exception> // for mandated header;
#include <stdexcept> // for mandated header;
#include YFM_YSLib_Adaptor_YTextBase // for string_view;
#include <string> // for std::string;
#include <ystdex/functional.hpp> // for ystdex::non_void_result_t,
//	ystdex::result_of_t, ystdex::invoke_nonvoid;

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


//! \since build 624
//@{
/*!
\brief 打印带有层次信息的函数类型。
\note 约定第一个参数非空。
*/
using ExtractedLevelPrinter
	= std::function<void(const char*, RecordLevel, size_t)>;
template<typename _type>
using GLevelTracer = std::function<void(_type, RecordLevel)>;
using ExceptionTracer = GLevelTracer<const std::exception&>;


/*!
\brief 通过 YCL_TraceRaw 跟踪带空格缩进层次的异常信息的函数类型。
\pre 断言：第一参数非空。
*/
YF_API YB_NONNULL(1) void
TraceException(const char*, RecordLevel = Err, size_t level = 0) ynothrow;

/*!
\brief 通过 YCL_TraceRaw 跟踪记录异常类型。
\since build 658
\todo 处理类型名称。
*/
YF_API void
TraceExceptionType(const std::exception&, RecordLevel = Err) ynothrow;

/*!
\brief 使用 TraceException 展开和跟踪异常类型和信息。
\sa ExtraceException
\sa TraceException
\sa TraceExceptionType
\since build 658
*/
YF_API void
ExtractAndTrace(const std::exception&, RecordLevel = Err) ynothrow;

//! \brief 展开指定层次的异常并使用指定参数记录。
YF_API void
ExtractException(const ExtractedLevelPrinter&,
	const std::exception&, RecordLevel = Err, size_t = 0) ynothrow;
//@}

//! \return 是否发生并捕获异常。
//@{
/*!
\brief 执行并尝试记录异常。
\since build 624

对参数指定的函数求值，并使用最后一个参数跟踪记录异常。
*/
YF_API bool
TryExecute(std::function<void()>, const char* = {}, RecordLevel = Alert,
	ExceptionTracer = ExtractAndTrace);

/*!
\brief 调用函数并尝试返回。
\since build 702
*/
template<typename _fCallable, typename... _tParams>
ystdex::nonvoid_result_t<ystdex::result_of_t<_fCallable&&(_tParams&&...)>>
TryInvoke(_fCallable&& f, _tParams&&... args) ynothrow
{
	TryRet(ystdex::invoke_nonvoid(yforward(f), yforward(args)...))
	CatchExpr(std::exception& e, ExtractAndTrace(e, Emergent))
	CatchExpr(..., YCL_TraceRaw(Emergent, "Unknown exception found."))
	return {};
}

/*!
\brief 调用函数并过滤宿主异常。
\note 使用 ADL \c TryInvoke 和 \c TryExecute 。
\since build 624

对参数指定的函数求值，并捕获和跟踪记录所有异常。
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

