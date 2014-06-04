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
\version r165
\author FrankHB <frankhb1989@gmail.com>
\since build 502
\par 创建时间:
	2013-06-04 23:05:33 +0800
\par 修改时间:
	2014-06-05 00:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(Android)::AndroidHost
*/


#ifndef INC_Android_Helper_AndroidHost_h_
#define INC_Android_Helper_AndroidHost_h_ 1

#include "YModules.h"
#include YFM_Helper_Host // for Devices::AndroidScreen;
#if YCL_Android
#	include <thread>
#	include <mutex>
#	include <android/native_activity.h>
#	include <android/configuration.h>
#	include YFM_YSLib_Adaptor_YReference // for unique_ptr;
#else
#	error "Currently only Android is supported."
#endif

namespace YSLib
{

#if YCL_Android
namespace Android
{

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
	//! \brief 暂存状态锁。
	mutable std::mutex state_mutex{};
	//! \note Devices::AndroidScreen 是不完整类型，因此不能在类定义内初始化。
	unique_ptr<Devices::AndroidScreen> p_screen;
	unique_ptr<Desktop> p_desktop{};

public:
	NativeHost(::ANativeActivity&, void*, size_t);
	~NativeHost();

	DefGetter(const ynothrow, Desktop&, DesktopRef,
		(YAssertNonnull(p_desktop), *p_desktop))
	DefGetter(const ynothrow, Devices::AndroidScreen&, ScreenRef,
		(YAssertNonnull(p_screen), *p_screen))

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
		std::lock_guard<std::mutex> lck(state_mutex);

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

	void*
	SaveInstanceState(size_t* p_len);
};

YF_API Desktop&
FetchDefaultDesktop() ynothrow;

YF_API NativeHost&
FetchNativeHostInstance() ynothrow;

} // namespace Android;
#endif

} // namespace YSLib;
#endif

