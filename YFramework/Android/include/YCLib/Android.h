/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Android.h
\ingroup YCLib
\ingroup Android
\brief YCLib Android 平台公共扩展。
\version r649
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 18:30:24 +0800
\par 修改时间:
	2014-12-16 05:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Android)::Android
*/


#ifndef YCL_Android_INC_Android_h_
#define YCL_Android_INC_Android_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Host
#if !YCL_Android
#	error "This file is only for Android."
#endif

//! \since build 499
struct AConfiguration;
//! \since build 502
struct AInputQueue;
//! \since build 499
struct ALooper;

namespace platform_ex
{

/*!
\brief Android 平台扩展接口。
\since build 492
*/
inline namespace Android
{

/*!
\brief 输入队列。
\note 按键的处理后转交给系统继续处理。
\warning 非虚析构。
\since build 502
\todo 添加定制处理后是否转交的接口。
*/
class YF_API InputQueue
{
private:
	std::reference_wrapper<::AInputQueue> queue_ref;

public:
	InputQueue(::ALooper&, ::AInputQueue&);
	~InputQueue();
};


/*!
\brief 取当前线程的本机消息循环。
\note 参数指定是否允许非回调。
\throw Exception 调用失败。
\since build 499
*/
YF_API ::ALooper&
FetchNativeLooper(bool) ythrow(Exception);

YF_API void
TraceConfiguration(::AConfiguration&,
	platform::Logger::Level = platform::Descriptions::Notice);

} // inline namespace Android;

} // namespace platform_ex;

#endif

