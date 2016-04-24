/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Environment.h
\ingroup Helper
\brief 环境。
\version r901
\author FrankHB <frankhb1989@gmail.com>
\since build 521
\par 创建时间:
	2013-02-08 01:28:03 +0800
\par 修改时间:
	2016-04-24 20:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Environment
*/


#ifndef INC_Helper_Environment_h_
#define INC_Helper_Environment_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_ValueNode // for ValueNode;
#include YFM_Helper_HostWindow // for Host::Window;
#include <ystdex/any.h> // for ystdex::any;
#include <ystdex/scope_guard.hpp> // for ystdex::unique_guard;
#if YF_Multithread == 1
#	include <atomic>
#endif
#if YCL_Win32
#	include YFM_YSLib_UI_YPanel
#endif

namespace YSLib
{

#if YCL_Android
//! \since build 502
//@{
namespace Devices
{
class AndroidScreen;
} // namespace Devices;

namespace Android
{

class NativeHost;

} // namespace Android;
//@}
#endif

/*!
\brief 应用程序环境。
\since build 378
*/
class YF_API Environment
{
public:
	/*!
	\brief 环境根节点。
	\since build 688
	*/
	ValueNode Root;

private:
	/*!
	\brief 初始化守护。
	\since build 688
	*/
	stack<ystdex::any> app_exit;

#if YF_Hosted
	/*!
	\brief 本机窗口对象映射。
	\note 不使用 ::SetWindowLongPtr 等 Windows API 保持跨平台，
		并避免和其它代码冲突。
	\warning 销毁窗口前移除映射，否则可能引起未定义行为。
	\warning 非线程安全，应仅在宿主线程上操作。
	\since build 670
	*/
	map<Host::NativeWindowHandle, observer_ptr<Host::Window>> wnd_map;
	/*!
	\brief 窗口对象映射锁。
	\since build 551
	*/
	mutable mutex wmap_mtx;
#	if YF_Multithread == 1
	/*!
	\brief 窗口线程计数。
	\sa EnterWindowThrad, LeaveWindowThread
	\since build 399
	*/
	std::atomic<size_t> wnd_thrd_count{0};

public:
	/*!
	\brief 退出标记。
	\sa LeaveWindowThread
	\since build 399
	*/
	std::atomic<bool> ExitOnAllWindowThreadCompleted{true};
#	endif

#	if YCL_Win32
	/*!
	\brief 点映射例程。
	\sa MapCursor
	\since build 670
	*/
	std::function<pair<observer_ptr<Host::Window>, Drawing::Point>(
		const Drawing::Point&)> MapPoint{};
	/*!
	\brief 宿主环境桌面。
	\since build 571
	*/
	UI::Panel Desktop{};
private:
	//! \since build 432
	Host::WindowClass window_class;
#	elif YCL_Android
	/*!
	\brief 点映射例程。
	\note 若非空则 MapCursor 调用此实现，否则使用恒等变换。
	\since build 572
	*/
	ystdex::id_func_clr_t<Drawing::Point>* MapPoint = {};
#	endif
#endif

public:
	/*!
	\brief 构造：初始化环境。
	\note Win32 平台：尝试无参数调用 FixConsoleHandler ，若失败则跟踪警告。
	*/
	Environment();
	~Environment();

#if YF_Hosted
	//! \since build 670
	//@{
	/*!
	\brief 取 GUI 前景窗口。
	\todo 线程安全。
	\todo 非 Win32 宿主平台实现。
	*/
	observer_ptr<Host::Window>
	GetForegroundWindow() const ynothrow;
#endif
	//! \since build 688
	//@{
	/*!
	\brief 取值类型根节点。
	\pre 断言：已初始化。
	\sa InitializeEnvironment
	*/
	ValueNode&
	GetRootRef() ynothrowv;

	//! \pre 参数调用无异常抛出。
	template<typename _func>
	void
	AddExitGuard(_func f)
	{
		static_assert(std::is_nothrow_copy_constructible<_func>(),
			"Invalid guard function found.");

		try
		{
			app_exit.push(ystdex::unique_guard(f));
		}
		catch(...)
		{
			f();
			throw;
		}
	}
	//@}

#if YF_Hosted
	/*!
	\brief 插入窗口映射项。
	\note 线程安全。
	*/
	void
	AddMappedItem(Host::NativeWindowHandle, observer_ptr<Host::Window>);
	//@}

#	if YF_Multithread == 1
	/*!
	\brief 标记开始窗口线程，增加窗口线程计数。
	\note 线程安全。
	\since build 399
	*/
	void
	EnterWindowThread();
#	endif

	/*!
	\brief 取本机句柄对应的窗口指针。
	\note 线程安全。
	\since build 670
	*/
	observer_ptr<Host::Window>
	FindWindow(Host::NativeWindowHandle) const ynothrow;

#	if YF_Multithread == 1
	/*!
	\brief 标记结束窗口线程，减少窗口线程计数并在计数为零时执行附加操作。
	\since build 399

	减少窗口计数后检查，若为零且退出标记被设置时向 YSLib 消息队列发送退出消息。
	*/
	void
	LeaveWindowThread();
#	endif

	/*!
	\brief 映射宿主光标位置到相对顶级窗口输入的光标位置。
	\return 使用的顶级窗口指针（若使用屏幕则为空）和相对顶级窗口或屏幕的位置。
	\since build 670
	\todo 支持 Win32 和 Android 以外的平台。

	首先确定屏幕光标位置，若 MapPoint 非空则调用 MapPoint 确定顶级窗口及变换坐标，
	最后返回结果。
	*/
	pair<observer_ptr<Host::Window>, Drawing::Point>
	MapCursor() const;

#	if YCL_Win32
	/*!
	\brief 映射顶级窗口的点。
	\since build 670

	首先调用使用指定的参数作为屏幕光标位置确定顶级窗口。
	若存在指定的顶级窗口，则调用窗口的 MapCursor 方法确定结果，否则返回无效值。
	*/
	pair<observer_ptr<Host::Window>, Drawing::Point>
	MapTopLevelWindowPoint(const Drawing::Point&) const;
#	endif
	/*!
	\brief 移除窗口映射项。
	\note 线程安全。
	\since build 389
	*/
	void
	RemoveMappedItem(Host::NativeWindowHandle) ynothrow;

	//! \since build 384
	void
	UpdateRenderWindows();
#endif
};

} // namespace YSLib;

#endif

