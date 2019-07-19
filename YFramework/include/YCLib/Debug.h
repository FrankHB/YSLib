/*
	© 2011-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Debug.h
\ingroup YCLib
\brief YCLib 调试设施。
\version r791
\author FrankHB <frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 14:20:49 +0800
\par 修改时间:
	2019-07-08 19:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Debug
*/


#ifndef YCL_INC_Debug_h_
#define YCL_INC_Debug_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include YFM_YCLib_Container // for string_view, string, std::ostream,
//	platform::sfmt;
#include <ystdex/function.hpp> // for ystdex::function;
#include YFM_YCLib_Mutex // for Concurrency;

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
\def YF_Use_TraceSrc
\brief 在跟踪日志中使用跟踪源码位置。
\since build 724
*/
#ifndef NDEBUG
#	ifndef YF_Use_TraceSrc
#		define YF_Use_TraceSrc 1
#	endif
#endif


namespace platform
{

/*!
\brief 使用标准输出流打印字符串并刷新。
\pre 间接断言：参数的数据指针非空。
\return 打印和刷新是否成功。
\since build 742

使用标准输出流以平台相关的方式打印字符串，然后刷新流。编码视为 UTF-8 。
DS 平台：使用 std::puts 。
Win32 平台：使用控制台接口，失败时使用 std::cout 。
其它平台：使用 std::cout 。
*/
YF_API bool
Echo(string_view) ynoexcept(YF_Platform == YF_Platform_DS);


/*!
\ingroup tracing
\brief 日志记录器。
\since build 498
*/
class YF_API Logger
{
public:
	using Level = Descriptions::RecordLevel;
	using Filter = ystdex::function<bool(Level, Logger&)>;
	//! \note 传递的第三参数非空。
	using Sender = ystdex::function<void(Level, Logger&, const char*)>;

	//! \brief 过滤等级：可用于参照以决定是否过滤的阈值。
#ifdef NDEBUG
	Level FilterLevel = Descriptions::Informative;
#else
	Level FilterLevel = Descriptions::Debug;
#endif

private:
	//! \invariant \c bool(filter) 。
	Filter filter{DefaultFilter};
	//! \invariant \c bool(Sender) 。
	Sender sender{FetchDefaultSender()};
	/*!
	\brief 日志记录互斥量。
	\since build 551

	仅 DoLog 和 DoLogException 在发送日志时使用的锁。
	使用递归锁以允许用户在发送器中间接递归调用 DoLog 和 DoLogException 。
	*/
	Concurrency::recursive_mutex record_mutex{};

public:
	//! \since build 803
	//@{
	/*!
	\brief 无参数构造：使用过滤器和发送器。
	\note 异常中立：同 FetchDefaultSender 。
	\note 由 ystdex::function 的构造模板提供的保证确保无其它异常抛出。
	*/
	DefDeCtor(Logger)
	//! \brief 复制构造：复制过滤等级、过滤器和发送器，使用新创建的互斥量。
	Logger(const Logger&);
	/*!
	\brief 转移构造：转移过滤等级、转移过滤器和发送器，使用新创建的互斥量。
	\post 被转移的日志对象具有默认的过滤器和发送器。
	\see LWG 2062 。
	*/
	Logger(Logger&&) ynothrow;

	DefDeCopyMoveAssignment(Logger)
	//@}

	//! \since build 628
	DefGetter(const ynothrow, const Sender&, Sender, sender)

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

	//! \pre 间接断言：第三参数非空。
	//@{
	/*!
	\brief 默认发送器：使用 std::cerr 输出。
	\note DS 平台：空操作。
	*/
	static YB_NONNULL(3) void
	DefaultSendLog(Level, Logger&, const char*) ynothrowv;

	/*!
	\brief 默认发送器：使用 stderr 输出。
	\since build 626
	*/
	static YB_NONNULL(3) void
	DefaultSendLogToFile(Level, Logger&, const char*) ynothrowv;
	//@}

	/*!
	\brief 转发等级和日志至发送器。
	\note 忽略字符串参数对应的空数据指针参数。
	\note 保证串行发送。
	*/
	//@{
	void
	DoLog(Level, const char*);
	//! \since build 659
	PDefH(void, DoLog, Level lv, string_view sv)
		ImplRet(DoLog(lv, sv.data()))
	//@}

private:
	/*!
	\brief 转发等级和日志至发送器。
	\pre 间接断言：字符串参数对应的数据指针非空。
	\since build 510
	*/
	//@{
	YB_NONNULL(3) void
	DoLogRaw(Level, const char*);
	//! \since build 659
	PDefH(void, DoLogRaw, Level lv, string_view sv)
		ImplRet(DoLogRaw(lv, sv.data()))
	//@}

public:
	/*!
	\brief 转发等级和异常对象至发送器。

	根据异常对象确定日志字符串并发送。若转发时抛出异常，则记录此异常。
	对并发操作保证串行发送，且整个过程持有锁 record_mutex 以保证连续性。
	*/
	void
	DoLogException(Level level, const std::exception&) ynothrow;

	/*!
	\brief 取新建的平台相关的默认发送：按指定的标签取平台相关实现。
	\pre 断言：参数的数据指针非空。
	\note 非 Android 平台：无异常抛出。
	\since build 658
	*/
	static Sender
	FetchDefaultSender(string_view = "YFramework");

	template<typename _fCaller, typename... _tParams>
	void
	Log(Level level, _fCaller&& f, _tParams&&... args)
	{
		if(filter(level, *this))
			TryExpr(DoLog(level, yforward(f)(yforward(args)...)))
			CatchExpr(std::exception& e, DoLogException(level, e), throw)
	}

	/*!
	\brief 默认发送器：使用第一参数指定的流输出。
	\pre 间接断言：字符串参数非空。
	\since build 626
	*/
	//@{
	static YB_NONNULL(4) void
	SendLog(std::ostream&, Level, Logger&, const char*) ynothrowv;

	//! \pre 断言：流指针非空。
	static YB_NONNULL(1, 4) void
	SendLogToFile(std::FILE*, Level, Logger&, const char*) ynothrowv;
	//@}

	/*!
	\brief 交换：交换所有互斥量以外的数据成员。
	\since build 804
	*/
	YF_API friend void
	swap(Logger&, Logger&) ynothrow;
};


/*!
\brief 取公共日志记录器。
\since build 498
*/
YF_API Logger&
FetchCommonLogger();


/*!
\brief 格式输出日志字符串前追加记录源文件名和行号。
\pre 间接断言：第三参数非空。
\note 允许第一参数为空指针，视为未知。
\note 当失败时调用 ystdex::trace 记录，但只保留参数中的文件名和行号。
\since build 593
*/
YF_API YB_ATTR_gnu_printf(3, 4) YB_NONNULL(1, 3) string
LogWithSource(const char*, int, const char*, ...) ynothrow;


/*!
\brief 使用公共日志记录器记录日志格式字符串。
\note 支持格式同 std::fprintf 。
\note 使用 FetchCommonLogger 保证串行输出。
\since build 498
*/
#define YCL_Log(_lv, ...) \
	platform::FetchCommonLogger().Log( \
		platform::Descriptions::RecordLevel(_lv), __VA_ARGS__)

/*!
\brief YFramework 跟踪。
\note 直接按格式字符串的输出跟踪，不带源代码信息。
\since build 724
*/
#define YF_TraceRaw(_lv, ...) \
	YCL_Log(_lv, [&]() -> platform::string { \
		TryRet(ystdex::sfmt<platform::string>(__VA_ARGS__)) \
		CatchRet(..., {}) \
	})

/*!
\def YF_Trace
\brief YCLib 默认调试跟踪。
\note 无异常抛出。
\since build 724
*/
#if YF_Use_TraceSrc
#	define YF_Trace(_lv, ...) \
	YCL_Log(_lv, [&]{ \
		return platform::LogWithSource(__FILE__, __LINE__, __VA_ARGS__); \
	})
#else
#	define YF_Trace(_lv, ...) YF_TraceRaw(_lv, __VA_ARGS__)
#endif


/*!
\def YTraceDe
\brief YCLib 默认调试跟踪。
\note 使用默认的调试跟踪级别。
\sa YF_Trace
\since build 432
*/
#if YB_Use_YTrace
#	define YTraceDe(_lv, ...) YF_Trace(_lv, __VA_ARGS__)
#else
#	define YTraceDe(...)
#endif

} // namespace platform;

namespace platform_ex
{

#if YF_Multithread == 1
/*!
\brief 日志断言函数。
\note 默认断言 ystdex::yassert 的替代。
\warning Win32 平台：允许忽略且继续，但行为未定义。应只用于调试用途。
\sa ystdex::yassert
\since build 641
\todo 允许在 Win32 上禁用消息框。

在 Android 上用此函数包装 NDK 函数替代实现。
在其它平台上：
	在 Win32 上首先使用图形界面（消息框）提示断言失败。注意不适用于消息循环内部。
		允许忽略并继续，否则终止程序。当选择中止时候首先发送 \c SIGABRT 信号。
		忽略此过程的所有错误，包括所有被抛出的异常。若捕获异常则继续以下行为。
	锁定公共日志记录器后调用 ystdex::yassert ，最终调用 std::terminate 终止程序。
*/
#if !YCL_Win32
YB_NORETURN
#endif
YF_API void
LogAssert(const char*, const char*, int, const char*) ynothrow;

#	if YB_Use_YAssert
#		undef YAssert
//! \since build 499
#		define YAssert(_expr, _msg) \
	((_expr) ? void(0) \
		: platform_ex::LogAssert(#_expr, __FILE__, __LINE__, _msg))
#	endif
#endif

#if YCL_Win32
/*!
\brief 发送字符串至调试器。
\pre 间接断言：字符串参数非空。
\note 当前直接调用 ::OutputDebugStringA 。
\since build 655
*/
YF_API YB_NONNULL(3) void
SendDebugString(platform::Logger::Level, platform::Logger&, const char*)
	ynothrowv;
#elif YCL_Android
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
\warning 非虚析构。
\sa platform::Logger
\since build 298
*/
class YF_API AndroidLogSender
{
public:
	using Logger = platform::Logger;
	using Level = Logger::Level;

private:
	string tag;

public:
	/*!
	\pre 间接断言：参数的数据指针非空。
	\since build 658
	*/
	AndroidLogSender(string_view);
	//! \since build 560
	DefDeCopyMoveCtorAssignment(AndroidLogSender)
	//! \brief 非虚析构：类定义外默认实现。
	~AndroidLogSender();

	//! \pre 间接断言：字符串非空。
	void
	operator()(Level, Logger&, const char*) const;

	DefGetter(const ynothrow, const string&, Tag, tag)
};
#endif

} // namespace platform_ex;

namespace platform
{

/*!
\brief 断言并返回非空参数。
\pre 断言：参数非空。
\since build 702
*/
template<typename _type>
inline _type&&
Nonnull(_type&& p) ynothrowv
{
	YAssertNonnull(p);
	return yforward(p);
}

/*!
\brief 断言并返回可解引用的迭代器。
\pre 断言：迭代器非确定不可解引用。
\since build 702
*/
template<typename _type>
inline _type&&
FwdIter(_type&& i) ynothrowv
{
	using ystdex::is_undereferenceable;

	YAssert(!is_undereferenceable(i), "Invalid iterator found.");
	return yforward(i);
}

/*!
\brief 断言并解引用非空指针。
\pre 使用 ADL 指定的 FwdIter 调用表达式的值等价于调用 platform::FwdIter 。
\pre 间接断言：指针非空。
\since build 553
*/
template<typename _type>
yconstfn auto
Deref(_type&& p) -> decltype(*p)
{
	return *FwdIter(yforward(p));
}

/*!
\ingroup diagnostic
\brief 组合消息和函数签名字符串。
\pre 间接断言：指针参数非空。
\note 使用 ADL to_string 。
\since build 861
*/
//@{
YB_NONNULL(2) inline PDefH(std::string, ComposeMessageWithSignature,
	const std::string& msg, const char* sig)
	ImplRet(msg + " @ " + Nonnull(sig))
YB_NONNULL(1, 2) inline PDefH(std::string, ComposeMessageWithSignature,
	const char* msg, const char* sig)
	ImplRet(std::string(Nonnull(msg)) + " @ " + Nonnull(sig))
template<class _type>
YB_NONNULL(2) inline std::string
ComposeMessageWithSignature(const _type& msg, const char* sig)
{
	using ystdex::to_string;

	return to_string(msg) + " @ " + Nonnull(sig);
}
//@}

} // namespace platform;

#endif

