/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Environment.h
\ingroup Helper
\brief 环境。
\version r779
\author FrankHB <frankhb1989@gmail.com>
\since build 521
\par 创建时间:
	2013-02-08 01:28:03 +0800
\par 修改时间:
	2014-11-04 17:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Environment
*/


#ifndef INC_Helper_Environment_h_
#define INC_Helper_Environment_h_ 1

#include "YModules.h"
#include YFM_Helper_HostWindow // for Host::Window;
#if YF_Multithread == 1
#	include <atomic>
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
private:
#if YF_Hosted
	/*!
	\brief 本机窗口对象映射。
	\note 不使用 ::SetWindowLongPtr 等 Windows API 保持跨平台，
		并避免和其它代码冲突。
	\warning 销毁窗口前移除映射，否则可能引起未定义行为。
	\warning 非线程安全，应仅在宿主线程上操作。
	\since build 389
	*/
	map<Host::NativeWindowHandle, Host::Window*> wnd_map;
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

private:
#	if YCL_Win32
	//! \since build 432
	Host::WindowClass window_class;
#	endif
#endif

public:
#	if YCL_Android
	/*!
	\brief 点映射例程。
	\note 若非空则 MapCursor 调用此实现，否则使用恒等变换。
	\since build 521
	*/
	Drawing::Point(*MapPoint)(const Drawing::Point&) = {};
#	endif

	Environment();
	~Environment();

#if YF_Hosted
	/*!
	\brief 取 GUI 前景窗口。
	\since build 381
	\todo 线程安全。
	*/
	Host::Window*
	GetForegroundWindow() const ynothrow;

	/*!
	\brief 插入窗口映射项。
	\note 线程安全。
	\since build 389
	*/
	void
	AddMappedItem(Host::NativeWindowHandle, Host::Window*);

#	if YF_Multithread == 1
	/*!
	\brief 标记开始窗口线程，增加窗口线程计数。
	\note 线程安全。
	\since build 399
	*/
	PDefH(void, EnterWindowThread, )
		ImplExpr(++wnd_thrd_count)
#	endif

	/*!
	\brief 取本机句柄对应的窗口指针。
	\note 线程安全。
	\since build 389
	*/
	Host::Window*
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
	\brief 映射宿主光标位置到相对顶层部件输入的光标位置。
	\since build 509
	*/
	Drawing::Point
	MapCursor() const;

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

