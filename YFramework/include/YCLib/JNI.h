/*
	© 2014-2016, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file JNI.h
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\brief Java 本机接口包装。
\version r225
\author FrankHB <frankhb1989@gmail.com>
\since build 552
\par 创建时间:
	2014-11-11 03:20:32 +0800
\par 修改时间:
	2021-11-03 03:54 +0800
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
#	include YFM_YCLib_Reference // for lref;
#	include <jni.h> // for ::JavaVM, ::JNIEnv;
#	include YFM_YCLib_Container // for pair;
#	include YFM_YBaseMacro // for DefGetter;
#	include <stdexcept>
#	include <utility>

namespace platform_ex
{

//! \since build 553
//@{
namespace JNI
{

//! \brief JNI 异常。
class YF_API JNIException : public std::runtime_error
{
public:
	using runtime_error::runtime_error;
};


//! \brief 当引用为空时按指定初始化并抛出 JNI 异常。
#	define YF_Raise_JNIExceptionOnNull(_ref, ...) \
	{ \
		if(!_ref) \
			throw platform_ex::JNI::JNIException(__VA_ARGS__); \
	}


/*!
\brief 基本Java 本机接口包装类。
\warning 非虚析构。
*/
class YF_API JNIBase
{
private:
	//! \since build 696
	lref<::JavaVM> vm_ref;
	//! \since build 696
	lref<::JNIEnv> env_ref;
	//! \since build 554
	bool owns{};

public:
	/*!
	\brief 构造：使用虚拟机引用和指定的版本。
	\throw JNIException 初始化调用失败。
	\note 在必要时调用 \c AttachCurrentThread 并取得所有权。
	*/
	JNIBase(::JavaVM&, ::jint = YF_Use_JNI);
	//! \brief 构造：使用虚拟机引用和环境引用。
	JNIBase(::JavaVM& vm, ::JNIEnv& env)
		: vm_ref(vm), env_ref(env), owns()
	{}

private:
	/*!
	\brief 构造：使用虚拟机引用、环境引用和所有权状态。
	\since build 593
	*/
	JNIBase(::JavaVM& vm, const pair<::JNIEnv&, bool>& pr)
		: vm_ref(vm), env_ref(pr.first), owns(pr.second)
	{}

public:
	/*!
	\brief 析构：当具有所有权时调用 \c DetachCurrentThread 。
	\note 忽略 \c DetachCurrentThread 返回的错误。
	\see https://bugs.openjdk.java.net/browse/JDK-6616502 。
	*/
	~JNIBase();

	DefGetter(const ynothrow, ::JNIEnv&, EnvRef, env_ref)
	DefGetter(const ynothrow, ::JavaVM&, VMRef, vm_ref)

	/*!
	\brief 检查引用值，若非空则解引用。
	\exception JNIException 引用为空。
	*/
	template<typename _type>
	static inline auto
	Deref(const _type& p) -> decltype(*p)
	{
		return *Nonnull(p);
	}

	/*!
	\brief 保证在线程退出时调用 \c DetachCurrentThread 。
	\throw ystdex::unsupported 不支持没有线程本地存储和 POSIX 线程的平台。
	\since build 563
	*/
	static void
	EnsureDetachJNIAtThreadExit(::JavaVM&, ::JNIEnv&);

	/*!
	\brief 检查引用值，若非空则返回。
	\throw JNIException 引用为空。
	*/
	template<typename _type>
	static inline _type
	Nonnull(const _type& p)
	{
		YF_Raise_JNIExceptionOnNull(p, "Specific object not found.")
		return p;
	}

	/*!
	\brief 调用 \c ExceptionCheck 检查异常，若存在异常则抛出。
	\throw JNIException 存在异常。
	\note 若不存在异常则无作用。
	*/
	void
	ThrowOnException() ythrow(JNIException);
};


/*!
\brief Java 本机接口包装类。
\warning 非虚析构。
*/
class YF_API JNI : private JNIBase
{
public:
	using Boolean = ::jboolean;
	using Byte = ::jbyte;
	using Char = ::jchar;
	using Short = ::jshort;
	using Int = ::jint;
	using Long = ::jlong;
	using Float = ::jfloat;
	using Double = ::jdouble;
	using Object = ystdex::remove_pointer_t<::jobject>;
	using Class = ystdex::remove_pointer_t<::jclass>;
	using String = ystdex::remove_pointer_t<::jstring>;
	using Array = ystdex::remove_pointer_t<::jarray>;
	using ObjectArray = ystdex::remove_pointer_t<::jobjectArray>;
	using BooleanArray = ystdex::remove_pointer_t<::jbooleanArray>;
	using ByteArray = ystdex::remove_pointer_t<::jbyteArray>;
	using CharArray = ystdex::remove_pointer_t<::jcharArray>;
	using ShortArray = ystdex::remove_pointer_t<::jshortArray>;
	using IntArray = ystdex::remove_pointer_t<::jintArray>;
	using LongArray = ystdex::remove_pointer_t<::jlongArray>;
	using FloatArray = ystdex::remove_pointer_t<::jfloatArray>;
	using DoubleArray = ystdex::remove_pointer_t<::jdoubleArray>;
	using Throwable = ystdex::remove_pointer_t<::jthrowable>;
	using Weak = ystdex::remove_pointer_t<::jweak>;
	using FieldID = ystdex::remove_pointer_t<::jfieldID>;
	using MethodID = ystdex::remove_pointer_t<::jmethodID>;
	using Value = ::jvalue;
	using ObjectRefType = ::jobjectRefType;
	using NativeMethod = ::JNINativeMethod;

	using JNIBase::JNIBase;

	//! \since build 662
	//@{
	PDefH(Boolean, IsAssignableFrom, Class& clazz1, Class& clazz2) const
		ynothrow
		ImplRet(GetEnvRef().IsAssignableFrom(&clazz1, &clazz2))
	PDefH(Boolean, IsInstanceOf, Object& obj, Class& clazz) const ynothrow
		ImplRet(GetEnvRef().IsInstanceOf(&obj, &clazz))
	PDefH(Boolean, IsSameObject, Object& ref1, Object& ref2) const ynothrow
		ImplRet(GetEnvRef().IsSameObject(&ref1, &ref2))
	//@}

	using JNIBase::GetEnvRef;
	using JNIBase::GetVMRef;

	using JNIBase::Deref;

	using JNIBase::EnsureDetachJNIAtThreadExit;

	using JNIBase::Nonnull;

	using JNIBase::ThrowOnException;
};

} // namespace JNI;
//@}

} // namespace platform_ex;

#endif

#endif

