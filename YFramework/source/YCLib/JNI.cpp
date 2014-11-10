/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file JNI.cpp
\ingroup YCLib
\brief Java 本机接口包装。
\version r73
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-11-11 03:25:23 +0800
\par 修改时间:
	2014-11-11 03:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::JNI
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_JNI
#include YFM_YCLib_Debug
#include <ystdex/string.hpp>

#if YF_Use_JNI

namespace platform_ex
{

using namespace platform::Descriptions;

#define YCL_RaiseJNIFailure(_str) \
	(YTraceDe(Err, _str), throw std::runtime_error(_str))

::JNIEnv&
FetchJNIEnvRef(::JavaVM& vm)
{
	::JNIEnv* p_jni;

	int status(vm.GetEnv(reinterpret_cast<void**>(&p_jni), JNI_VERSION_1_6));

	if(status == JNI_EDETACHED)
	{
		YTraceDe(Informative, "JNI thread has not attached.");
		status = vm.AttachCurrentThread(&p_jni, {});
		if(status != JNI_OK)
		{
			YTraceDe(Err, "Status = %d.", status);
			YCL_RaiseJNIFailure("Failed attaching JNI thread.");
		}
		else
			YTraceDe(Informative, "JNI thread attached.");
	}
	else if(status != JNI_OK)
	{
		YTraceDe(Err, "Status = %d.", status);
		YCL_RaiseJNIFailure("Failed getting JNI environment.");
	}
	YAssertNonnull(p_jni);
	return *p_jni;
}

} // namespace YSLib;

#endif

