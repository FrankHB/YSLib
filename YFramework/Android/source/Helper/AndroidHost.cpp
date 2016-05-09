/*
	© 2014-2016 FrankHB.

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
\version r442
\author FrankHB <frankhb1989@gmail.com>
\since build 502
\par 创建时间:
	2014-06-04 23:05:52 +0800
\par 修改时间:
	2016-05-05 12:04 +0800
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
#include YFM_Helper_GUIApplication // for YSLib::LockInstance,
//	YSLib::PostMessage;

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
	looper(FetchNativeLooper({})), msg_pipe(platform_ex::MakePipe()),
	p_screen()
{
	NativeAppHostPtr = this;
	YAssert(NativeAppHostPtr, "Duplicate instance found.");

	auto& cb(Deref(ac.callbacks));

#	define YCL_Android_RegCb_Begin(_n, ...) \
	cb.on##_n = [](__VA_ARGS__){ \
		YTraceDe(Debug, "ANativeActivity::" #_n ": %p.", \
			ystdex::pvoid(Nonnull(p_activity)));
#define YCL_Android_RegSimpCb(_n, ...) \
	YCL_Android_RegCb_Begin(_n, ::ANativeActivity* p_activity) \
		__VA_ARGS__; \
	}
#	define YCL_NativeHostPtr static_cast<NativeHost*>(p_activity->instance)
	yunseq
	(
	YCL_Android_RegSimpCb(Destroy, delete YCL_NativeHostPtr),
	YCL_Android_RegSimpCb(Start, void()),
	YCL_Android_RegCb_Begin(Resume, ::ANativeActivity* p_activity)
		YTraceDe(Debug, " Application resumed.");
		YCL_NativeHostPtr->ReleaseSavedState();
		YCL_NativeHostPtr->paused = {};
	},
	YCL_Android_RegCb_Begin(SaveInstanceState,
		::ANativeActivity* p_activity, ::size_t* p_len)
		return YCL_NativeHostPtr->SaveInstanceState(p_len);
	},
	YCL_Android_RegCb_Begin(Pause, ::ANativeActivity* p_activity)
		YTraceDe(Debug, " Application paused.");
		YCL_NativeHostPtr->paused = true;
	},
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
		YTraceDe(Debug, " p_window = %p.", ystdex::pvoid(p_window));
		auto& host(*YCL_NativeHostPtr);

		if(!host.thrdMain.joinable())
		{
			YTraceDe(Debug, "Starting native main thread...");
			host.thrdMain = std::thread([&host, p_activity, p_window]{
				host.p_screen.reset(new Devices::AndroidScreen(*p_window));
				host.MainReturnValue = ::y_android_main();
				YTraceDe(Debug, "Application main routine exited.");
				::ANativeActivity_finish(p_activity);
				host.RunOnUIThread([]{
					YTraceDe(Notice, "Finish method called.");
				});
				YTraceDe(Debug, "Clearing screen pointer...");
				host.p_screen.reset();
				YTraceDe(Informative, "Native main thread finished.");
			});
		}
		else
			YTraceDe(Err, "Native main thread already running.");
	},
	YCL_Android_RegCb_Begin(NativeWindowDestroyed,
		::ANativeActivity* p_activity, ::ANativeWindow* p_window)
		YTraceDe(Debug, " p_window = %p.", ystdex::pvoid(p_window));

		auto& host(*YCL_NativeHostPtr);

		// TODO: Nonull?
		if(const auto p_app = LockInstance())
		{
			// XXX: Use proper exit code.
			PostQuitMessage(0);
			YTraceDe(Debug, "YSLib quit message posted.");
		}
		YTraceDe(Informative,
			"Waiting for native main thread finishing...");
		FilterExceptions(std::bind(&std::thread::join, std::ref(
			host.thrdMain)), "AndroidHost NativeWindowDestroyed handler");
		YTraceDe(Informative, "Waiting for screen being released...");
		YTraceDe(Debug, "Client thread terminated.");
	},
	YCL_Android_RegCb_Begin(InputQueueCreated,
		::ANativeActivity* p_activity, ::AInputQueue* p_queue)
		YTraceDe(Debug, " p_queue = %p.", ystdex::pvoid(p_queue));

		auto& host(*YCL_NativeHostPtr);

		host.p_input_queue.reset(
			new InputQueue(host.looper, Deref(p_queue)));
	},
	YCL_Android_RegCb_Begin(InputQueueDestroyed,
		::ANativeActivity* p_activity, ::AInputQueue* p_queue)
		YTraceDe(Debug, " p_queue = %p.", ystdex::pvoid(p_queue));
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
	YTraceDe(Debug, "Adding file descriptor for UI thread call...");
	::ALooper_addFd(&looper.get(), *msg_pipe.first.get(), ALOOPER_POLL_CALLBACK,
		ALOOPER_EVENT_INPUT, [](int, int, void* p_data){
		auto& host(Deref(static_cast<NativeHost*>(p_data)));
		lock_guard<mutex> lck(host.msg_mutex);
		auto& task(host.msg_task);
		int ret;
		const int res(read(*host.msg_pipe.first.get(), &ret, sizeof(int)));

		if(res < 0)
			YTraceDe(Warning, "Failed reading file descriptor.");
		else if(res != sizeof(int))
			YTraceDe(Warning, "Truncated reading with result %d.", res);
		if(task)
		{
			task();
			task = nullptr;
		}
		return 1;
	}, this);
	YTraceDe(Debug, "Native host launched.");
}
NativeHost::~NativeHost()
{
	::ALooper_removeFd(&looper.get(), *msg_pipe.first.get());
	YTraceDe(Debug, "File descriptor for UI thread call removed.");
	ReleaseSavedState();
	YTraceDe(Debug, "State released.");
	p_input_queue.reset();
	YTraceDe(Debug, "Input queue detached.");
	p_config.reset();
	YTraceDe(Debug, "Native host terminated.");
}

Devices::Screen&
NativeHost::GetHostScreenRef() const ynothrow
{
	// TODO: Expose screen type and use %GetScreenRef instead.
	return Deref(p_screen);
}

void
NativeHost::ClearSavedStateHandler()
{
	lock_guard<mutex> lck(state_mutex);

	fSaveState = nullptr;
}

void
NativeHost::LoadConfiguration()
{
	::AConfiguration_fromAssetManager(Nonnull(p_config).get(),
		activity.get().assetManager);
	TraceConfiguration(*p_config);
	YTraceDe(Debug, "Configuration loaded.");
}

void
NativeHost::ReleaseSavedState() ynothrow
{
	lock_guard<mutex> lck(state_mutex);

	vecSavedState.clear();
}

void
NativeHost::ResizeScreen(const Drawing::Size& s)
{
	Deref(p_screen).SetSize(s);
}

void
NativeHost::RestoreSavedState(byte* p_byte) const
{
	lock_guard<mutex> lck(state_mutex);

	std::copy_n(vecSavedState.cbegin(), vecSavedState.size(), Nonnull(p_byte));
}

void
NativeHost::RunOnUIThread(std::function<void()> f)
{
	YTraceDe(Debug, "RunOnUIThread called.");

	lock_guard<mutex> lck(msg_mutex);

	msg_task = f;

	int res(platform::RetryOnError([this]{
		const int ret(1);

		return ::write(*msg_pipe.second.get(), &ret, sizeof(int));
	}, errno, EINTR));

	if(res < 0)
		YTraceDe(Warning, "Failed writing file descriptor, error = %d.",
			errno);
	else if(size_t(res) != sizeof(int))
		YTraceDe(Warning, "Truncated writing with result %d.", int(res));
}

void*
NativeHost::SaveInstanceState(size_t* p_len)
{
	void* p_saved_state = {};

	{
		lock_guard<mutex> lck(state_mutex);

		if(fSaveState)
			fSaveState(p_saved_state, *p_len);
		vecSavedState.clear();
	}
	return p_saved_state;
}


NativeHost&
FetchNativeHostInstance() ynothrow
{
	return Deref(NativeAppHostPtr);
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

	YTraceDe(Debug, "Creating activity: %p.",
		ystdex::pvoid(Nonnull(p_activity)));
	p_activity->instance = make_unique<NativeHost>(*p_activity, saved_state,
		saved_state_size).release();
}

#endif

