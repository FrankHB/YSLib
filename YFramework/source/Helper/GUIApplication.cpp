/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIApplication.cpp
\ingroup Helper
\brief GUI 应用程序。
\version r578
\author FrankHB <frankhb1989@gmail.com>
\since build 396
\par 创建时间:
	2013-04-06 22:42:54 +0800
\par 修改时间:
	2016-08-27 15:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIApplication
*/


#include "Helper/YModules.h"
#include YFM_Helper_GUIApplication
#include YFM_Helper_Environment
#include YFM_YCLib_Input // for platform_ex::FetchCursor;
#if YCL_Win32
#	include YFM_Win32_Helper_Win32Control // for Windows::UI::ControlView;
#elif YCL_Android
#	include YFM_Android_Helper_AndroidHost // for Android::NativeHost;
#endif

namespace YSLib
{

using namespace Drawing;
#if YF_Hosted
using namespace Host;
#endif

namespace
{

//! \since build 550
recursive_mutex ApplicationMutex;

//! \since build 499
GUIApplication* ApplicationPtr;

/*!
\brief 取空闲消息。
\since build 320
*/
inline PDefH(Message, FetchIdleMessage, )
	ImplRet(Message(SM_Input))

/*!
\brief 后台消息处理程序。
\since build 320
*/
inline void
Idle(Messaging::Priority prior)
{
	// NOTE: Wait for GUI input of any shells. Post message for specific shell
	//	would cause low performance when there are many candidate messages
	//	of distinct shells.
	PostMessage(FetchIdleMessage(), prior);
}

} // unnamed namespace;


GUIHost::GUIHost()
#if YF_Hosted
	: wnd_map(), wmap_mtx()
#	if YF_Multithread == 1
#		if YCL_Win32
	, window_class(WindowClassName)
#		endif
#	endif
#	if YCL_Android
	, Desktop(Android::FetchNativeHostInstance().GetHostScreenRef())
#	endif
#endif
{
#if YCL_Win32
	Desktop.SetView(
		make_unique<Windows::UI::ControlView>(::GetDesktopWindow()));
	Desktop.SetRenderer(make_unique<UI::PseudoRenderer>());
#endif
	YF_Trace(Debug, "GUI host lifetime began.");
}
GUIHost::~GUIHost()
{
	YF_Trace(Debug, "GUI host lifetime ended.");

#	if YF_Hosted && !YCL_Android
	using ystdex::get_value;

	std::for_each(wnd_map.cbegin() | get_value, wnd_map.cend() | get_value,
		[](observer_ptr<Window> p) ynothrow{
		FilterExceptions([&]{
			p->Close();
		});
	});
#	endif
}

#if YF_Hosted
observer_ptr<Window>
GUIHost::GetForegroundWindow() const ynothrow
{
#	ifdef YCL_Win32
	return FindWindow(::GetForegroundWindow());
#	else
	return {};
#	endif
}

void
GUIHost::AddMappedItem(NativeWindowHandle h, observer_ptr<Window> p)
{
	lock_guard<mutex> lck(wmap_mtx);

	wnd_map.emplace(h, p);
}

void
GUIHost::EnterWindowThread()
{
	++wnd_thrd_count;
}

observer_ptr<Window>
GUIHost::FindWindow(NativeWindowHandle h) const ynothrow
{
	lock_guard<mutex> lck(wmap_mtx);
	const auto i(wnd_map.find(h));

	return i == wnd_map.end() ? nullptr : i->second;
}

#	if YF_Multithread == 1
void
GUIHost::LeaveWindowThread()
{
	if(--wnd_thrd_count == 0 && ExitOnAllWindowThreadCompleted)
		YSLib::PostQuitMessage(0);
	YTraceDe(Debug, "New window thread count = %zu.", wnd_thrd_count.load());
}
#	endif

pair<observer_ptr<Window>, Point>
GUIHost::MapCursor() const
{
#	if YCL_Win32
	::POINT cursor;

	if(YB_UNLIKELY(!::GetCursorPos(&cursor)))
		return {{}, Point::Invalid};

	const Point pt{cursor.x, cursor.y};

	return MapPoint ? MapPoint(pt) : pair<observer_ptr<Window>, Point>({}, pt);
#	elif YCL_Android
	// TODO: Support floating point coordinates.
	const auto& cursor(platform_ex::FetchCursor());
	const Point pt(cursor.first, cursor.second);

	return {{}, MapPoint ? MapPoint(pt) : pt};
#	else
	return {{}, Point::Invalid};
#	endif
}

#	if YCL_Win32
pair<observer_ptr<Window>, Point>
GUIHost::MapTopLevelWindowPoint(const Point& pt) const
{
	::POINT cursor{pt.X, pt.Y};

	if(const auto h = ::ChildWindowFromPointEx(::GetDesktopWindow(),
		cursor, CWP_SKIPINVISIBLE))
	{
		auto p_wnd = FindWindow(h);

		if(!p_wnd)
			p_wnd = GetForegroundWindow();
		if(p_wnd)
		{
			YCL_CallF_Win32(ScreenToClient, p_wnd->GetNativeHandle(), &cursor);
			return {p_wnd, p_wnd->MapPoint({cursor.x, cursor.y})};
		}
	}
	return {{}, Point::Invalid};
}
#	endif

void
GUIHost::RemoveMappedItem(NativeWindowHandle h) ynothrow
{
	lock_guard<mutex> lck(wmap_mtx);
	const auto i(wnd_map.find(h));

	if(i != wnd_map.end())
		wnd_map.erase(i);
}

void
GUIHost::UpdateRenderWindows()
{
	lock_guard<mutex> lck(wmap_mtx);

	for(const auto& pr : wnd_map)
		if(pr.second)
			pr.second->Refresh();
}
#endif


GUIApplication::InitBlock::InitBlock(Application& app)
	: p_env(new Environment(app))
{}

GUIApplication::GUIApplication()
	: Application(), init(*this)
{
	lock_guard<recursive_mutex> lck(ApplicationMutex);

	YAssert(!ApplicationPtr, "Duplicate instance found.");
	ApplicationPtr = this;
}

GUIApplication::~GUIApplication()
{
	lock_guard<recursive_mutex> lck(ApplicationMutex);

	ApplicationPtr = {};
}

Environment&
GUIApplication::GetEnvironmentRef() const ynothrow
{
	return Deref(init.get().p_env);
}
GUIHost&
GUIApplication::GetGUIHostRef() const ynothrow
{
	return init.get().host;
}

bool
GUIApplication::DealMessage()
{
	if(AccessQueue([](MessageQueue& mq) ynothrow{
		return mq.empty();
	}))
	//	Idle(UIResponseLimit);
		OnGotMessage(FetchIdleMessage());
	else
	{
		const auto i(AccessQueue([](MessageQueue& mq) ynothrow{
			return mq.cbegin();
		}));

		if(YB_UNLIKELY(i->second.GetMessageID() == SM_Quit))
			return {};
		if(i->first < UIResponseLimit)
			Idle(UIResponseLimit);
		OnGotMessage(i->second);
		AccessQueue([i](MessageQueue& mq) ynothrowv{
			mq.erase(i);
		});
	}
	return true;
}


GUIApplication&
FetchGlobalInstance()
{
	lock_guard<recursive_mutex> lck(ApplicationMutex);

	if(ApplicationPtr)
		return *ApplicationPtr;
	throw GeneralEvent("Application instance is not ready.");
}

/* extern */Application&
FetchAppInstance()
{
	return FetchGlobalInstance();
}

locked_ptr<GUIApplication, recursive_mutex>
LockInstance()
{
	return {ApplicationPtr, ApplicationMutex};
}


void
Execute(GUIApplication& app, shared_ptr<Shell> p_shl)
{
#if YF_Hosted
	FetchGUIHost().ExitOnAllWindowThreadCompleted = true;
#endif
	if(YB_UNLIKELY(!Activate(p_shl)))
		throw GeneralEvent("Failed activating the main shell.");
	while(app.DealMessage())
		;
}

} // namespace YSLib;

