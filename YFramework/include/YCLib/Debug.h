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
\version r518
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 14:20:49 +0800
\par 修改时间:
	2014-12-17 22:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Debug
*/


#ifndef YCL_INC_Debug_h_
#define YCL_INC_Debug_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include YFM_YCLib_Mutex
#include <ystdex/string.hpp> // for std::string, ystdex::sfmt;

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
	//! \note 通过 Logger 默认实现传递的第三参数非空。
	using Sender = std::function<void(Level, Logger&, const char*)>;

	Level FilterLevel = Descriptions::Informative;

private:
	//! \invariant <tt>bool(filter)</tt> 。
	Filter filter{DefaultFilter};
	//! \invariant <tt>bool(Sender)</tt> 。
	Sender sender{FetchDefaultSender()};
	/*!
	\brief 日志记录锁。
	\since build 551

	仅 DoLog 和 DoLogException 在发送日志时使用的锁。
	使用递归锁以允许用户在发送器中间接递归调用 DoLog 和 DoLogException 。
	*/
	Concurrency::recursive_mutex record_mutex;

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
	\since build 519
	*/
	void
	SetSender(Sender);

	/*!
	\brief 访问日志记录执行指定操作。
	\note 线程安全：互斥访问。
	\since build 558
	*/
	template<typename _func>
	auto
	AccessRecord(_func f) -> decltype(f())
	{
		Concurrency::lock_guard<Concurrency::recursive_mutex> lck(record_mutex);

		return f();
	}

	//! \brief 默认过滤：仅允许等级不大于阈值的日志被记录。
	static bool
	DefaultFilter(Level, Logger&) ynothrow;

	/*!
	\brief 默认发送器：使用 stderr 输出。
	\pre 间接断言：第三参数非空。
	*/
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

private:
	/*!
	\brief 转发等级和日志至发送器。
	\since build 510
	*/
	//@{
	//! \pre 间接断言：指针参数非空。
	void
	DoLogRaw(Level, const char*);
	PDefH(void, DoLogRaw, Level lv, const std::string& str)
		ImplRet(DoLogRaw(lv, str.c_str()))
	//@}

public:
	/*!
	\brief 转发等级和异常对象至发送器。

	根据异常对象确定日志字符串并发送。若转发时抛出异常，则记录此异常。
	对并发操作保证串行发送，且整个过程持有锁 record_mutex 以保证连续性。
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
			TryExpr(DoLog(level, yforward(f)(yforward(args)...)))
			CatchExpr(std::exception& e, DoLogException(level, e), throw)
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
\pre 断言：第一参数非空。
\pre 间接断言：第三参数非空。
\since build 498
*/
YF_API YB_ATTR(format (printf, 3, 4)) YB_NONNULL(1, 3) std::string
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

#if YB_Use_YAssert && YF_Multithread == 1
/*!
\brief 日志断言函数。
\note 默认断言 ystdex::yassert 的替代。
\warning 若忽略且继续，则行为未定义。应只用于调试用途。
\sa ystdex::yassert
\since build 553
\todo 允许在 Win32 上禁用消息框。

若断言成功则为空操作，否则：
在 Android 上用此函数包装 NDK 函数替代实现。
在其它平台上：
	在 Win32 上首先使用图形界面（消息框）提示断言失败。
		允许忽略并继续，否则终止程序。当选择中止时候首先发送 \c SIGABRT 信号。
		忽略此过程的所有错误，包括所有被抛出的异常。若捕获异常则继续以下行为。
	锁定公共日志记录器后调用 ystdex::yassert ，最终调用 std::terminate 终止程序。
*/
YF_API void
LogAssert(bool, const char*, const char*, int, const char*) ynothrow;

#	undef YAssert
//! \since build 499
#	define YAssert(_expr, _msg) \
	platform_ex::LogAssert(_expr, #_expr, __FILE__, __LINE__, _msg)
#endif

#if YCL_Android
/*!
\brief 映射 Descriptions::RecordLevel 为 Android 日志 API 使用的日志优先级。
\return 介于 ANDROID_LOG_FATAL 和 ANDROID_LOG_VERBOSE 的日志优先级。
\sa platform::Descriptions::RecordLevel
\since build 498

逆序对应映射 Descriptions::Critical 起的枚举值到 ANDROID_LOG_FATAL 起的枚举值。
对不属于枚举值的区间中间值，使用较大优先级（较小 RecordLevel 值）：
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
	//! \since build 560
	DefDeCopyMoveCtorAssignment(AndroidLogSender)
	~AndroidLogSender();

	//! \pre 间接断言：字符串非空。
	void
	operator()(Level, Logger&, const char*) const;

	DefGetter(const ynothrow, const std::string&, Tag, tag)
};
#endif

} // namespace platform_ex;

namespace platform
{

/*!
\brief 断言并返回非空指针。
\pre 断言：指针非空。
\since build 553
*/
template<typename _type>
inline _type&&
Nonnull(_type&& p)
{
	YAssertNonnull(p);
	return yforward(p);
}

/*!
\brief 断言并解引用非空指针。
\pre 使用 ADL 指定的 Nonnull 调用的表达式语义等价于 platform::Nonnull 。
\pre 间接断言：指针非空。
\since build 553
*/
template<typename _type>
yconstfn auto
Deref(_type&& p) -> decltype(*p)
{
	return *Nonnull(yforward(p));
}

} // namespace platform;

#endif

