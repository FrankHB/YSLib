/*
	© 2014 FrankHB.

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
\version r207
\author FrankHB <frankhb1989@gmail.com>
\since build 502
\par 创建时间:
	2013-06-04 23:05:33 +0800
\par 修改时间:
	2014-11-13 20:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(Android)::AndroidHost
*/


#ifndef INC_Android_Helper_AndroidHost_h_
#define INC_Android_Helper_AndroidHost_h_ 1

#include "YModules.h"
#include YFM_Helper_Environment // for Devices::AndroidScreen;
#if YCL_Android
#	include <thread>
#	include <android/native_activity.h>
#	include <android/configuration.h>
#	include YFM_YSLib_Adaptor_YReference // for unique_ptr;
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
	PDefHOp(void, (), ::AConfiguration* p)
		ImplRet(::AConfiguration_delete(p))
};


/*!
\brief Android 本机宿主。
\since build 502
*/
class YF_API NativeHost
{
protected:
	/*
	\brief 本机主线程，用于启动入口函数。
	\note 和直接使用 NativeActivity 控制的进程的主线程不同。
	\sa ::y_android_main
	\see https://developer.android.com/guide/components/processes-and-threads.html 。
	*/
	std::thread thrdMain{};
	//! \brief 暂存状态。
	vector<byte> vecSavedState{};
	std::function<void(void*&, size_t&)> fSaveState{};

private:
	std::reference_wrapper<::ANativeActivity> activity;
	unique_ptr<::AConfiguration, ConfigurationDeleter> p_config;
	std::reference_wrapper<::ALooper> looper;
	unique_ptr<InputQueue> p_input_queue{};
	//! \since build 553
	//@{
	mutex msg_mutex{};
	std::pair<UniqueHandle, UniqueHandle> msg_pipe;
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
	//! \note Desktop 是不完整类型，因此不能在类定义内初始化。
	unique_ptr<Desktop> p_desktop;

public:
	NativeHost(::ANativeActivity&, void*, size_t);
	~NativeHost();

	//! \since build 551
	DefPred(const ynothrow, Paused, paused)
	//! \since build 503
	DefGetter(const ynothrow, ::ANativeActivity&, ActivityRef, activity.get())
	DefGetter(const ynothrow, Desktop&, DesktopRef, Deref(p_desktop))
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
	//		_type>>::value, "Invalid state type found.");
		lock_guard<mutex> lck(state_mutex);

		fSaveState = [&](void*& p_saved_state, size_t& saved_size){
			p_saved_state = std::malloc(sizeof(_type));
			if(p_saved_state)
				// TODO: Subclassing.
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

	template<typename _type>
	void
	Restore(_type& state) const
	{
		// TODO: Wait for 'std::is_trivially_copyable' to be implemented.
	//	static_assert(std::is_trivially_copyable<ystdex::remove_reference_t<
	//		_type>>::value, "Invalid state type found.");

		RestoreSavedState(reinterpret_cast<byte*>(&state));
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

YF_API Desktop&
FetchDefaultDesktop() ynothrow;

//! \since build 506
YF_API ::ANativeWindow&
FetchDefaultWindow() ynothrow;

YF_API NativeHost&
FetchNativeHostInstance() ynothrow;

} // namespace Android;
#endif

} // namespace YSLib;
#endif

