/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file AndroidHost.h
\ingroup Helper
\ingroup Android
\brief Android 宿主。
\version r246
\author FrankHB <frankhb1989@gmail.com>
\since build 502
\par 创建时间:
	2013-06-04 23:05:33 +0800
\par 修改时间:
	2016-05-05 12:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(Android)::AndroidHost
*/


#ifndef INC_Android_Helper_AndroidHost_h_
#define INC_Android_Helper_AndroidHost_h_ 1

#include "YModules.h"
#include YFM_Helper_Environment // for unique_ptr, std::atomic,
//	Devices::AndroidScreen, ystdex::aligned_store_cast;
#if YCL_Android
#	include <thread>
#	include <android/native_activity.h>
#	include <android/configuration.h>
#	include YFM_YCLib_JNI
#	include YFM_YCLib_Host
#else
//#	error "Currently only Android is supported."
#endif

namespace YSLib
{

#if YCL_Android
namespace Android
{

//! \since build 553
namespace JNI = platform_ex::JNI;
//! \since build 553
using platform_ex::UniqueHandle;

/*!
\brief 配置对象删除器。
\since build 502
*/
struct YF_API ConfigurationDeleter
{
	//! \since build 594
	using pointer = ::AConfiguration*;

	//! \since build 594
	PDefHOp(void, (), pointer p) const ynothrow
		ImplRet(::AConfiguration_delete(p))
};


/*!
\brief Android 本机宿主。
\warning 非虚析构。
\since build 502
*/
class YF_API NativeHost
{
protected:
	/*!
	\brief 本机主线程，用于启动入口函数。
	\note 和直接使用 NativeActivity 控制的进程的主线程不同。
	\sa ::y_android_main
	\see https://developer.android.com/guide/components/processes-and-threads.html 。
	*/
	std::thread thrdMain{};
	//! \brief 暂存状态。
	vector<byte> vecSavedState{};
	std::function<void(void*&, size_t&)> fSaveState{};

public:
	/*!
	\brief 调用用户入口的返回值。
	\sa ::y_android_main
	\since build 691
	*/
	int MainReturnValue = EXIT_SUCCESS;

private:
	//! \since build 559
	lref<::ANativeActivity> activity;
	unique_ptr<::AConfiguration, ConfigurationDeleter> p_config;
	//! \since build 559
	lref<::ALooper> looper;
	unique_ptr<InputQueue> p_input_queue{};
	//! \since build 553
	//@{
	mutex msg_mutex{};
	//! \since build 593
	pair<UniqueHandle, UniqueHandle> msg_pipe;
	std::function<void()> msg_task{};
	//@}
	/*!
	\brief 暂存状态锁。
	\since build 551
	*/
	mutable mutex state_mutex{};
	/*!
	\brief 暂停状态。
	\since build 551
	*/
	std::atomic<bool> paused{{}};
	//! \note Devices::AndroidScreen 是不完整类型，因此不能在类定义内初始化。
	unique_ptr<Devices::AndroidScreen> p_screen;

public:
	NativeHost(::ANativeActivity&, void*, size_t);
	~NativeHost();

	//! \since build 551
	DefPred(const ynothrow, Paused, paused)
	//! \since build 503
	DefGetter(const ynothrow, ::ANativeActivity&, ActivityRef, activity.get())
	//! \since build 690
	Devices::Screen&
	GetHostScreenRef() const ynothrow;
	DefGetter(const ynothrow, Devices::AndroidScreen&, ScreenRef,
		Deref(p_screen))

	/*!
	\brief 绑定指定类型的状态设置。
	\warning 状态类型需在整个程序中保持兼容。
	*/
	template<typename _type>
	void
	BindSavedState(const _type& state)
	{
		// TODO: Wait for 'std::is_trivially_copyable' to be implemented.
	//	static_assert(std::is_trivially_copyable<ystdex::remove_reference_t<
	//		_type>>(), "Invalid state type found.");
		lock_guard<mutex> lck(state_mutex);

		fSaveState = [&](void*& p_saved_state, size_t& saved_size){
			p_saved_state = std::malloc(sizeof(_type));
			if(p_saved_state)
				// TODO: Use inherited class of exception.
				throw std::bad_alloc();
			new(p_saved_state) _type(state);
			saved_size = sizeof(_type);
		};
	}

	void
	ClearSavedStateHandler();

	void
	LoadConfiguration();

	void
	ReleaseSavedState() ynothrow;

	//! \since build 563
	void
	ResizeScreen(const Drawing::Size&);

	template<typename _type>
	void
	Restore(_type& state) const
	{
		// TODO: Wait for 'std::is_trivially_copyable' to be implemented.
	//	static_assert(std::is_trivially_copyable<ystdex::remove_reference_t<
	//		_type>>(), "Invalid state type found.");

		RestoreSavedState(ystdex::aligned_store_cast<byte*>(&state));
	}

	/*!
	\brief 恢复暂存状态。
	\pre 断言： <tt>p_byte</tt> 。
	\warning 不检查空间可用。
	*/
	void
	RestoreSavedState(byte* p_byte) const;

	//! \since build 553
	void
	RunOnUIThread(std::function<void()>);

	void*
	SaveInstanceState(size_t* p_len);
};

//! \since build 506
YF_API ::ANativeWindow&
FetchDefaultWindow() ynothrow;

YF_API NativeHost&
FetchNativeHostInstance() ynothrow;

} // namespace Android;
#endif

} // namespace YSLib;
#endif

