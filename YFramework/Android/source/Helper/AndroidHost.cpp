/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file AndroidHost.cpp
\ingroup Helper
\ingroup Android
\brief Android 宿主。
\version r281
\author FrankHB <frankhb1989@gmail.com>
\since build 502
\par 创建时间:
	2014-06-04 23:05:52 +0800
\par 修改时间:
	2014-08-28 17:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(Android)::AndroidHost
*/


#include "Helper/YModules.h"
#include YFM_Android_Helper_AndroidHost
#if YCL_Android
#	include <android/configuration.h>
#	include "AndroidScreen.h"
#endif
#include YFM_YSLib_UI_YDesktop
#include YFM_YSLib_Core_YApplication

#if YCL_Android

namespace YSLib
{

using namespace Drawing;
using namespace Host;

namespace
{

//! \since build 502
Android::NativeHost* NativeAppHostPtr{};

} // unnamed namespace;

namespace Android
{

NativeHost::NativeHost(::ANativeActivity& ac, void* saved_state,
	size_t state_size)
	: activity(ac), p_config(::AConfiguration_new()),
	looper(FetchNativeLooper({})), p_screen(), p_desktop()
{
	NativeAppHostPtr = this;
	YAssert(NativeAppHostPtr, "Duplicate instance found.");
	YAssertNonnull(ac.callbacks);

	auto& cb(*ac.callbacks);

#	define YCL_Android_RegCb_Begin(_n, ...) \
	cb.on##_n = [](__VA_ARGS__){ \
		YAssertNonnull(static_cast<void*>(p_activity)); \
		YTraceDe(Debug, "ANativeActivity::" #_n ": %p.", \
			static_cast<void*>(p_activity));
#define YCL_Android_RegSimpCb(_n, ...) \
YCL_Android_RegCb_Begin(_n, ::ANativeActivity* p_activity) \
	__VA_ARGS__; \
}
#	define YCL_NativeHostPtr static_cast<NativeHost*>(p_activity->instance)
	yunseq
	(
	YCL_Android_RegSimpCb(Destroy, delete YCL_NativeHostPtr),
	YCL_Android_RegSimpCb(Start, void()),
	YCL_Android_RegSimpCb(Resume,
		YCL_NativeHostPtr->ReleaseSavedState()),
	YCL_Android_RegCb_Begin(SaveInstanceState,
		::ANativeActivity* p_activity, ::size_t* p_len)
		return YCL_NativeHostPtr->SaveInstanceState(p_len);
	},
//	YCL_Android_RegSimpCb(Pause, void()),
//	YCL_Android_RegSimpCb(Stop, void()),
	YCL_Android_RegSimpCb(ConfigurationChanged,
		YCL_NativeHostPtr->LoadConfiguration()),
//	YCL_Android_RegSimpCb(LowMemory, void()),
	YCL_Android_RegCb_Begin(WindowFocusChanged,
		::ANativeActivity* p_activity, int focused)
		YTraceDe(Debug, " focused = %d.", focused);
	},
	YCL_Android_RegCb_Begin(NativeWindowResized,
		::ANativeActivity* p_activity, ::ANativeWindow* p_window)
		YTraceDe(Debug, " p_window = %p.", static_cast<void*>(p_window));
		auto& host(*YCL_NativeHostPtr);

		if(!host.thrdMain.joinable())
		{
			YTraceDe(Debug, "Starting main thread...");
			host.thrdMain = std::thread([&host, p_activity, p_window]{
				host.p_screen.reset(new Devices::AndroidScreen(*p_window));
				host.p_desktop.reset(
					new Desktop(FetchNativeHostInstance().GetScreenRef()));
				::y_android_main();
				::ANativeActivity_finish(p_activity);
				host.p_desktop.reset();
				host.p_screen.reset();
			});
		}
		else
			YTraceDe(Err, "Native host thread already running.");
	},
	YCL_Android_RegCb_Begin(NativeWindowDestroyed,
		::ANativeActivity* p_activity, ::ANativeWindow* p_window)
		YTraceDe(Debug, " p_window = %p.", static_cast<void*>(p_window));
		// XXX: Use proper exit code.
		PostQuitMessage(0);
		YTraceDe(Debug, "YSLib quit message posted.");

		auto& host(*YCL_NativeHostPtr);

		// NOTE: If the thread has been already completed there is no effect.
		if(host.thrdMain.joinable())
		{
			YTraceDe(Informative, "Waiting for native thread finishing...");
			host.thrdMain.join();
		}
		YTraceDe(Informative, "Waiting for screen being released...");
		// FIXME: Thread safety.
		while(host.p_screen)
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		YTraceDe(Debug, "Client thread terminated.");
	},
	YCL_Android_RegCb_Begin(InputQueueCreated,
		::ANativeActivity* p_activity, ::AInputQueue* p_queue)
		YTraceDe(Debug, " p_queue = %p.", static_cast<void*>(p_queue));

		auto& host(*YCL_NativeHostPtr);

		YAssertNonnull(p_queue);
		host.p_input_queue.reset(new InputQueue(host.looper, *p_queue));
	},
	YCL_Android_RegCb_Begin(InputQueueDestroyed,
		::ANativeActivity* p_activity, ::AInputQueue* p_queue)
		YTraceDe(Debug, " p_queue = %p.", static_cast<void*>(p_queue));
		YCL_NativeHostPtr->p_input_queue.reset();
	}
	);
#	undef YCL_NativeHostPtr
#	undef YCL_Android_RegSimpCb
#	undef YCL_Android_RegCb_Begin
	if(saved_state && state_size != 0)
	{
		const auto p_src(static_cast<const byte*>(saved_state));

		YTraceDe(Debug, "Saved state needed to be stored.");
		vecSavedState = vector<byte>(p_src, p_src + state_size);
		YTraceDe(Debug, "Saved state cloned.");
	}
	YTraceDe(Debug, "Loading configuration...");
	LoadConfiguration();
	YTraceDe(Debug, "Native host launched.");
}
NativeHost::~NativeHost()
{
	ReleaseSavedState();
	YTraceDe(Debug, "State released.");
	p_input_queue.reset();
	YTraceDe(Debug, "Input queue detached.");
	p_config.reset();
	YTraceDe(Debug, "Native host terminated.");
}

void
NativeHost::ClearSavedStateHandler()
{
	std::lock_guard<std::mutex> lck(state_mutex);

	fSaveState = {};
}

void
NativeHost::LoadConfiguration()
{
	YAssertNonnull(p_config);
	::AConfiguration_fromAssetManager(p_config.get(),
		activity.get().assetManager);
	TraceConfiguration(*p_config);
	YTraceDe(Debug, "Configuration loaded.");
}

void
NativeHost::ReleaseSavedState() ynothrow
{
	std::lock_guard<std::mutex> lck(state_mutex);

	vecSavedState.clear();
}

void
NativeHost::RestoreSavedState(byte* p_byte) const
{
	YAssertNonnull(p_byte);
	std::lock_guard<std::mutex> lck(state_mutex);

	std::copy_n(vecSavedState.cbegin(), vecSavedState.size(), p_byte);
}

void*
NativeHost::SaveInstanceState(size_t* p_len)
{
	void* p_saved_state = {};

	{
		std::lock_guard<std::mutex> lck(state_mutex);

		if(fSaveState)
			fSaveState(p_saved_state, *p_len);
		vecSavedState.clear();
	}
	return p_saved_state;
}


NativeHost&
FetchNativeHostInstance() ynothrow
{
	YAssertNonnull(NativeAppHostPtr);
	return *NativeAppHostPtr;
}

Desktop&
FetchDefaultDesktop() ynothrow
{
	return FetchNativeHostInstance().GetDesktopRef();
}

::ANativeWindow&
FetchDefaultWindow() ynothrow
{
	return FetchNativeHostInstance().GetScreenRef().GetWindowRef();
}

} // namespace Android;

} // namespace YSLib;

extern "C" void
ANativeActivity_onCreate(::ANativeActivity* p_activity,
	void* saved_state, ::size_t saved_state_size)
{
	using namespace YSLib;
	using namespace YSLib::Android;

	YAssertNonnull(p_activity);

#ifndef NDEBUG
	platform::FetchCommonLogger().FilterLevel = platform::Descriptions::Debug;
#endif
	YTraceDe(Debug, "Creating activity: %p.", static_cast<void*>(p_activity));
	p_activity->instance = ystdex::make_unique<NativeHost>(*p_activity,
		saved_state, saved_state_size).release();
}

#endif

