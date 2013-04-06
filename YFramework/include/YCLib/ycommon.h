/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YCLib YCommonLib
\brief YSLib 基础库。
*/

/*!	\file ycommon.h
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r3416
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-12 22:14:28 +0800
\par 修改时间:
	2013-04-03 22:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YCommon
*/


#ifndef YCL_INC_YCOMMON_H_
#define YCL_INC_YCOMMON_H_ 1

//平台定义。
#include "Platform.h"

//平台无关部分。
#include <ydef.h>
#include <ystdex/cstdio.h>
#include <cstdlib>
#include <string>

//平台相关部分。
//#include <unistd.h>

//! \brief 默认平台命名空间。
namespace platform
{

/*!
\brief 异常终止函数。
\since build 319
*/
YB_NORETURN YF_API void
terminate() ynothrow;


//断言。
#ifdef YB_USE_YASSERT

#undef YAssert

/*!
\brief YCLib 默认断言函数。
\note 当定义 YCL_USE_YASSERT 宏时，宏 YAssert 操作由此函数实现。
*/
YF_API void
yassert(bool, const char*, const char*, int, const char*);

#define YAssert(exp, message) \
	platform::yassert(exp, #exp, message, __LINE__, __FILE__)

#else

#include <cassert>
#	define YAssert(exp, message) assert(exp)

#endif

} // namespace platform;

namespace platform_ex
{

#if YCL_DS

/*!
\brief 设置允许设备进入睡眠的标识状态。
\return 旧状态。
\note 默认状态为 true 。
\warning 非线程安全。
\since build 278
*/
YF_API bool
AllowSleep(bool);

#elif YCL_MINGW32
// TODO: Add more Windows specific APIs.

//! \since build 327
//@{
YF_API std::string
UTF8ToMBCS(const char*, std::size_t, int);
inline std::string
UTF8ToMBCS(const std::string& str, int cp)
{
	return UTF8ToMBCS(str.c_str(), str.length(), cp);
}

YF_API std::string
WCSToMBCS(const wchar_t*, std::size_t, int);
inline std::string
WCSToMBCS(const std::wstring& str, int cp)
{
	return WCSToMBCS(str.c_str(), str.length(), cp);
}
//@}

#else
#	error Unsupport platform found!
#endif

} // namespace platform_ex;

#endif

