/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file JNI.h
\ingroup YCLib
\brief Java 本机接口包装。
\version r47
\author FrankHB <frankhb1989@gmail.com>
\since build 551
\par 创建时间:
	2014-11-11 03:20:32 +0800
\par 修改时间:
	2014-11-11 03:52 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::JNI
*/


#ifndef YCL_INC_JNI_h_
#define YCL_INC_JNI_h_ 1

#include "YModules.h"
#include YFM_YCLib_Platform
#if YF_Use_JNI
#	include <jni.h>

namespace platform_ex
{

//! \since build 552
YF_API ::JNIEnv&
FetchJNIEnvRef(::JavaVM&);

} // namespace platform_ex;

#endif

#endif

