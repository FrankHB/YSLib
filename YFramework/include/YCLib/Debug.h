/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Debug.h
\ingroup YCLib
\brief YCLib 调试设施。
\version r356
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 14:20:49 +0800
\par 修改时间:
	2014-05-24 18:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Debug
*/


#ifndef YCL_INC_Debug_h_
#define YCL_INC_Debug_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include <libdefect/string.h> // for std::string;
#if YF_Multithread == 1
#	include <mutex>
#endif

/*!	\defgroup diagnostic Diagnostic
\brief 诊断设施。
\since build 378
*/

/*!	\defgroup debugging Debugging
\ingroup diagnostic
\brief 调试设施。
\since build 378

仅在宏 NDEBUG 未被定义时起诊断作用的调试接口和实现。
*/

/*!	\defgroup tracing Tracing
\ingroup diagnostic
\brief 跟踪设施。
\since build 498

独立配置的起诊断作用的调试接口和实现。
*/

/*!
\ingroup tracing
\def YCL_Use_TraceSrc
\brief 在跟踪日志中使用跟踪源码位置。
\since build 498
*/
#ifndef NDEBUG
#	ifndef YCL_Use_TraceSrc
#		define YCL_Use_TraceSrc 1
#	endif
#endif


namespace platform
{

/*!
\ingroup diagnostic
*/
//@{
/*!
\brief 调试模式：设置状态。
\note 当且仅当状态为 true 时，以下除 YDebugGetStatus 外的调试模式函数有效。
\warning 不保证线程安全性。
*/
YF_API void
YDebugSetStatus(bool = true);

/*!
\brief 调试模式：取得状态。
\warning 不保证线程安全性。
*/
YF_API bool
YDebugGetStatus();

/*!
\brief 调试模式：显示控制台。
\warning 控制台显示状态不保证线程安全性。
\since build 312
*/
YF_API void
YDebugBegin();

/*!
\brief 调试模式：按键继续。
\warning 控制台显示状态不保证线程安全性。
*/
YF_API void
YDebug();
/*!
\brief 调试模式：显示控制台字符串，按键继续。
\warning 控制台显示状态不保证线程安全性。
*/
YF_API void
YDebug(const char*);

/*!
\brief 调试模式 printf ：显示控制台格式化输出 ，按键继续。
\warning 控制台显示状态不保证线程安全性。
*/
#if defined _WIN32 && !defined __USE_MINGW_ANSI_STDIO
YB_ATTR(format (ms_printf, 1, 2))
#else
YB_ATTR(format (printf, 1, 2))
#endif
YF_API int
yprintf(const char*, ...);
//@}


/*!
\ingroup tracing
\brief 日志记录器。
\since build 498
*/
class YF_API Logger
{
public:
	using Level = Descriptions::RecordLevel;
	using Filter = std::function<bool(Level, Logger&)>;
	using Sender = std::function<void(Level, Logger&, const char*)>;

	Level FilterLevel = Descriptions::Informative;

private:
	//! \invariant <tt>bool(filter)</tt> 。
	Filter filter{DefaultFilter};
	//! \invariant <tt>bool(Sender)</tt> 。
	Sender sender{FetchDefaultSender()};
#if YF_Multithread == 1
	std::recursive_mutex record_mtx;
#endif

public:
	DefGetter(const ynothrow, Sender, Sender, sender)

	/*!
	\brief 设置过滤器。
	\note 忽略空过滤器。
	*/
	void
	SetFilter(Filter);
	/*!
	\brief 设置发送器。
	\note 忽略空发送器。
	*/
	void
	SetWriter(Sender);

	//! \brief 默认过滤：仅允许等级不大于阈值的日志被记录。
	static bool
	DefaultFilter(Level, Logger&) ynothrow;

	//! \brief 默认发送器：使用 stderr 输出。
	static void
	DefaultSendLog(Level, Logger&, const char*) ynothrowv;

	/*!
	\brief 转发等级和日志至发送器。
	\note 保证串行发送。
	*/
	//@{
	//! \note 忽略空指针参数。
	void
	DoLog(Level, const char*);
	PDefH(void, DoLog, Level lv, const std::string& str)
		ImplRet(DoLog(lv, str.c_str()))
	//@}

	/*!
	\brief 转发等级和异常对象至发送器。
	\note 根据异常对象确定日志字符串。
	*/
	void
	DoLogException(Level level, const std::exception&) ynothrow;

	//! \brief 取新建的平台相关的默认发送：按指定的标签取平台相关实现。
	static Sender
	FetchDefaultSender(const std::string& = "YFramework");

	template<typename _fCaller, typename... _tParams>
	void
	Log(Level level, _fCaller&& f, _tParams&&... args)
	{
		if(filter(level, *this))
			try
			{
				DoLog(level, yforward(f)(yforward(args)...));
			}
			catch(std::exception& e)
			{
				DoLogException(level, e);
				throw;
			}
	}
};


/*!
\brief 取公共日志记录器。
\since build 498
*/
YF_API Logger&
FetchCommonLogger();


/*!
\brief 格式输出日志字符串前追加记录源文件行号和文件名。
\pre 断言：指针参数非空。
\since build 498
*/
YF_API YB_ATTR(format (printf, 3, 4)) std::string
LogWithSource(const char*, int, const char*, ...);


/*!
\brief 使用公共日志记录器记录日志格式字符串。
\note 支持格式同 std::fprintf 。
\note 使用 FetchCommonLogger 保证串行输出。
\since build 498
*/
#define YCL_Log(_lv, ...) platform::FetchCommonLogger().Log(_lv, __VA_ARGS__)


/*!
\def YCL_Trace
\brief YCLib 默认调试跟踪。
\since build 498
*/
#if YCL_Use_TraceSrc
#	define YCL_Trace(_lv, ...) \
	YCL_Log(_lv, [&]{ \
		return platform::LogWithSource(__FILE__, __LINE__, __VA_ARGS__); \
	})
#else
#	define YCL_Trace(_lv, ...) \
	YCL_Log(_lv, [&]{return ystdex::sfmt(__VA_ARGS__);})
#endif


/*!
\def YTraceDe
\brief YCLib 默认调试跟踪。
\note 使用默认的调试跟踪级别。
\sa YCL_Trace
\since build 432
*/
#if YB_Use_YTrace
#	define YTraceDe(_lv, ...) YCL_Trace(_lv, __VA_ARGS__)
#else
#	define YTraceDe(...)
#endif

} // namespace platform;

namespace platform_ex
{

#if YCL_Android
/*!
\brief 日志断言函数。
\note 默认断言和 ystdex::yassert 无法使用调试日志输出时的替代。
\note 在 Android 平台上用此函数包装 NDK 函数实现。
\sa ystdex::yassert
\since build 499
*/
YF_API void
LogAssert(bool, const char*, const char*, int, const char*);

#	ifdef YAssert
#		undef YAssert
//! \since build 499
#		define YAssert(_expr, _msg) \
			platform_ex::LogAssert(_expr, #_expr, __FILE__, __LINE__, _msg)
#	endif


/*!
\brief 映射 Descriptions::RecordLevel 为 Android 日志 API 使用的日志优先级。
\post 返回值介于 ANDROID_LOG_FATAL 和 ANDROID_LOG_VERBOSE 。
\sa platform::Descriptions::RecordLevel
\since build 498

逆序对应映射 Descriptions::Critical 起的枚举值到 ANDROID_LOG_FATAL 起的枚举值。
对于不属于枚举值的区间中间值，使用较大优先级（较小 RecordLevel 值）：
例如 0x80 和 0x81 都映射为 ANDROID_LOG_WARNING 。
同时小于 Descriptions::Critical 的值都映射为 ANDROID_LOG_FATAL 。
*/
YF_API int
MapAndroidLogLevel(platform::Descriptions::RecordLevel);


/*!
\brief 使用 Android 日志 API 实现 YCLib 日志发送。
\sa platform::Logger
\since build 298
*/
class YF_API AndroidLogSender
{
public:
	using Logger = platform::Logger;
	using Level = Logger::Level;

private:
	std::string tag;

public:
	AndroidLogSender(const std::string&);
	DefDeCopyCtor(AndroidLogSender)
	DefDeMoveCtor(AndroidLogSender)
	~AndroidLogSender();

	//! \pre 间接断言：字符串非空。
	void
	operator()(Level, Logger&, const char*) const;

	DefGetter(const ynothrow, const std::string&, Tag, tag)
};
#endif

} // namespace platform_ex;

#endif

