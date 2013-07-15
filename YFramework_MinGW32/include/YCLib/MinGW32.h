/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MinGW32.h
\ingroup YCLib
\ingroup MinGW32
\brief YCLib MinGW32 平台扩展公共头文件。
\version r82
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2012-06-08 17:57:49 +0800
\par 修改时间:
	2013-07-14 19:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::MinGW32
*/


#ifndef YCL_MinGW32_INC_MinGW32_h_
#define YCL_MinGW32_INC_MinGW32_h_ 1

#include "YCLib/ycommon.h"
#include <YCLib/NativeAPI.h>
#include "YSLib/Core/yexcept.h" // for YSLib::LoggedEvent;

#if !YCL_MINGW32
#	error "This file is only for MinGW32."
#endif

/*!
\ingroup name_collision_workarounds
\brief 禁止使用 GetObject 宏。
\since build 412
*/
#undef GetObject

namespace platform_ex
{

/*!
\brief 宿主异常。
\since build 426
*/
class YF_API Exception : public YSLib::LoggedEvent
{
public:
	Exception(const std::string& = "unknown host exception", LevelType = 0)
		ynothrow;
};

/*!
\brief Windows 平台扩展接口。
\since build 427
*/
inline namespace Windows
{

/*!
\brief Win32 错误引起的宿主异常。
\since build 426
*/
class YF_API Win32Exception : public Exception
{
private:
	::DWORD err;

public:
	//! \pre 错误码不等于 0 。
	Win32Exception(::DWORD, const std::string& = "Win32 exception",
		LevelType = 0) ynothrow;

	DefGetter(const ynothrow, ::DWORD, ErrorCode, err)
};

/*!
\brief 按 ::GetLastError 的结果和指定参数抛出 MinGW32::Win32Exception 对象。
\since build 426
*/
#	define YF_Raise_Win32Exception(...) \
	throw platform_ex::Windows::Win32Exception(::GetLastError(), __VA_ARGS__)

} // namespace Windows;

} // namespace platform_ex;

#endif

