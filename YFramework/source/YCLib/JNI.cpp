/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file JNI.cpp
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\brief Java 本机接口包装。
\version r150
\author FrankHB <frankhb1989@gmail.com>
\since build 552
\par 创建时间:
	2014-11-11 03:25:23 +0800
\par 修改时间:
	2016-05-30 17:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::JNI
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_JNI
#if YF_Use_JNI
#include YFM_YCLib_Debug // for platform::Descriptions, platform::Deref,
//	ystdex::replace_cast;
#include YFM_YCLib_POSIXThread
#include <ystdex/exception.h> // for ystdex::unsupported;

namespace platform_ex
{

using namespace platform::Descriptions;
//! \since build 553
using platform::Deref;

namespace JNI
{

#define YCL_RaiseJNIFailure(_str) \
	(YTraceDe(Err, _str), throw platform_ex::JNI::JNIException(_str))

//! \since build 553
namespace
{

//! \since build 554
pair<::JNIEnv&, bool>
FetchJNIEnvRef(::JavaVM& vm, ::jint version)
{
	void* p_env;
	int status(vm.GetEnv(&p_env, version));
	auto& p_jni(ystdex::replace_cast<::JNIEnv*&>(p_env));

	if(status == JNI_EDETACHED)
	{
		YTraceDe(Debug, "JNI thread has not attached.");
		status = vm.AttachCurrentThread(&p_jni, {});
		if(status != JNI_OK)
		{
			YTraceDe(Err, "Status = %d.", status);
			YCL_RaiseJNIFailure("Failed attaching JNI thread.");
		}
		else
			YTraceDe(Informative, "JNI thread attached.");
		TryExpr(JNIBase::EnsureDetachJNIAtThreadExit(vm, Deref(p_jni)))
		CatchIgnore(ystdex::unsupported&)
	}
	else if(status != JNI_OK)
	{
		YTraceDe(Err, "Status = %d.", status);
		YCL_RaiseJNIFailure("Failed getting JNI environment.");
	}
	YAssertNonnull(p_jni);
	return {*p_jni, status == JNI_EDETACHED};
}

} // unnamed namespace;

JNIBase::JNIBase(::JavaVM& vm, ::jint version)
	: JNIBase(vm, FetchJNIEnvRef(vm, version))
{}
JNIBase::~JNIBase()
{
	if(owns)
		vm_ref.get().DetachCurrentThread();
}

void
JNIBase::EnsureDetachJNIAtThreadExit(::JavaVM& vm, ::JNIEnv& env)
{
#	if YF_Multithread == 1
#		if YB_HAS_THREAD_LOCAL
	ythread JNIBase guard(vm, env);
#		elif YF_Use_POSIXThread

	static TLSKey key([](void* p) ynothrow{
		delete static_cast<JNIBase*>(p);
	});

	key.SetValue(new JNIBase(vm, {env, true}));
#		else
	throw ystdex::unsupported("Platform without TLS or POSIX thread "
		" unsupported to automatically detach.");
#		endif
#	endif
}

void
JNIBase::ThrowOnException() ythrow(JNIException)
{
	if(GetEnvRef().ExceptionCheck())
		throw JNIException("A pending exception has occurred.");
}

} // namespace JNI;

} // namespace platform_ex;

#endif

