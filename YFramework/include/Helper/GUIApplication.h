/*
	© 2012-2016, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIApplication.h
\ingroup Helper
\brief GUI 应用程序。
\version r560
\author FrankHB <frankhb1989@gmail.com>
\since build 398
\par 创建时间:
	2013-04-11 10:02:53 +0800
\par 修改时间:
	2019-11-25 21:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIApplication
*/


#ifndef INC_Helper_GUIApplication_h_
#define INC_Helper_GUIApplication_h_ 1

#include "YModules.h"
#include YFM_Helper_YGlobal // for Environment;
#include YFM_Helper_HostWindow // for Host::Window;
#include YFM_YCLib_NativeAPI
#include YFM_YSLib_Core_YApplication // for Application;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#include YFM_Helper_GUIShell
#include YFM_YCLib_HostedGUI
#include YFM_YCLib_Mutex // for once_flag;
#include <ystdex/utility.hpp> // for ystdex::call_once_init;
#if YCL_Win32
#	include YFM_YSLib_UI_YPanel
#elif YCL_Android
#	include YFM_YSLib_UI_YDesktop
#endif

namespace YSLib
{

/*!
\brief GUI 宿主。
\since build 689
*/
class YF_API GUIHost
{
private:
#if YF_Hosted
	/*!
	\brief 本机窗口对象映射。
	\note 不使用 ::SetWindowLongPtr 等 Windows API 保持跨平台，
		并避免和其它代码冲突。
	\warning 销毁窗口前移除映射，否则可能引起未定义行为。
	\warning 非线程安全，应仅在宿主线程上操作。
	*/
	map<Host::NativeWindowHandle, observer_ptr<Host::Window>> wnd_map;
	//! \brief 窗口对象映射锁。
	mutable mutex wmap_mtx;
	/*!
	\brief 窗口线程计数。
	\sa EnterWindowThrad, LeaveWindowThread
	\since build 725
	*/
	atomic<size_t> wnd_thrd_count{0};

public:
	/*!
	\brief 退出标记。
	\sa LeaveWindowThread
	\since build 725
	*/
	atomic<bool> ExitOnAllWindowThreadCompleted{true};

#	if YCL_Win32
	/*!
	\brief 点映射例程。
	\sa MapCursor
	\since build 872
	*/
	function<pair<observer_ptr<Host::Window>, Drawing::Point>(
		const Drawing::Point&)> MapPoint{};
	//! \brief 宿主环境桌面。
	UI::Panel Desktop{};

private:
	Host::WindowClass window_class;
#	elif YCL_Android
	/*!
	\brief 点映射例程。
	\note 若非空则 MapCursor 调用此实现，否则使用恒等变换。
	*/
	ystdex::id_func_clr_t<Drawing::Point>* MapPoint = {};
	/*!
	\brief 宿主环境桌面。
	\since build 690
	*/
	UI::Desktop Desktop;
#	endif
#endif

public:
	GUIHost();
	~GUIHost();

#if YF_Hosted
	/*!
	\brief 取 GUI 前景窗口。
	\todo 线程安全。
	\todo 非 Win32 宿主平台实现。
	*/
	observer_ptr<Host::Window>
	GetForegroundWindow() const ynothrow;
#endif

#if YF_Hosted
	/*!
	\brief 插入窗口映射项。
	\note 线程安全。
	*/
	void
	AddMappedItem(Host::NativeWindowHandle, observer_ptr<Host::Window>);

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
	*/
	observer_ptr<Host::Window>
	FindWindow(Host::NativeWindowHandle) const ynothrow;

#	if YF_Multithread == 1
	/*!
	\brief 标记结束窗口线程，减少窗口线程计数并在计数为零时执行附加操作。

	减少窗口计数后检查，若为零且退出标记被设置时向 YSLib 消息队列发送退出消息。
	*/
	void
	LeaveWindowThread();
#	endif

	/*!
	\brief 映射宿主光标位置到相对顶级窗口输入的光标位置。
	\return 使用的顶级窗口指针（若使用屏幕则为空）和相对顶级窗口或屏幕的位置。
	\todo 支持 Win32 和 Android 以外的平台。

	首先确定屏幕光标位置，若 MapPoint 非空则调用 MapPoint 确定顶级窗口及变换坐标，
	最后返回结果。
	*/
	pair<observer_ptr<Host::Window>, Drawing::Point>
	MapCursor() const;

#	if YCL_Win32
	/*!
	\brief 映射顶级窗口的点。

	首先调用使用指定的参数作为屏幕光标位置确定顶级窗口。
	若存在指定的顶级窗口，则调用窗口的 MapCursor 方法确定结果，否则返回无效值。
	*/
	pair<observer_ptr<Host::Window>, Drawing::Point>
	MapTopLevelWindowPoint(const Drawing::Point&) const;
#	endif
	/*!
	\brief 移除窗口映射项。
	\note 线程安全。
	*/
	void
	RemoveMappedItem(Host::NativeWindowHandle) ynothrow;

	//! \since build 384
	void
	UpdateRenderWindows();
#endif
};


/*!
\brief 平台相关的应用程序类。
\note 含默认接口。
\since build 398
*/
class YF_API GUIApplication : public Application
{
private:
	//! \since build 692
	struct InitBlock final
	{
		//! \brief 环境状态。
		unique_ptr<Environment> p_env;
		//! \brief GUI 宿主状态。
		mutable GUIHost host{};

		//! \since build 693
		InitBlock(Application&);
	};

	//! \since build 692
	ystdex::call_once_init<InitBlock, once_flag> init;

public:
	/*!
	\brief 用户界面输入响应阈值。
	\sa DSApplication::Run

	用于主消息队列的消息循环中控制后台消息生成策略的全局消息优先级。
	*/
	Messaging::Priority UIResponseLimit = 0x40;

	/*!
	\brief 无参数构造。
	\pre 断言：唯一性。
	*/
	GUIApplication();
	/*!
	\brief 析构：释放资源。
	*/
	~GUIApplication() override;

	//! \since build 570
	Environment&
	GetEnvironmentRef() const ynothrow;
	//! \since build 570
	GUIHost&
	GetGUIHostRef() const ynothrow;

	/*!
	\brief 处理当前消息。
	\return 循环条件。
	\note 线程安全：全局消息队列互斥访问。
	\note 优先级小于 UIResponseLimit 的消息时视为后台消息，否则为前台消息。
	\note 消息在响应消息后移除。
	\warning 应保证不移除正在被响应的消息。

	若主消息队列为空，处理空闲消息，否则从主消息队列取出并分发消息。
	当取出的消息的标识为 SM_Quit 时视为终止循环。
	对后台消息，分发前调用后台消息处理程序：分发空闲消息并可进行时序控制。
	*/
	bool
	DealMessage();
};


/*!
\brief 取全局应用程序实例。
\throw GeneralEvent 不存在初始化完成的应用程序实例。
\warning 调用线程安全，但不保证调用结束后实例仍在生存期内。
\since build 550
*/
//@{
YF_API yimpl(GUIApplication&)
FetchGlobalInstance();
template<class _tApp>
inline _tApp&
FetchGlobalInstance()
{
	return ystdex::polymorphic_downcast<_tApp&>(FetchGlobalInstance());
}
//@}

/*!
\brief 锁定实例。
\note 当返回结果为空时实例生存期未开始或已结束。
\since build 551
*/
YF_API locked_ptr<yimpl(GUIApplication), recursive_mutex>
LockInstance();


/*!
\brief 取单一环境对象引用。
\since build 398
*/
inline PDefH(Environment&, FetchEnvironment, )
	ImplRet(FetchGlobalInstance().GetEnvironmentRef())

/*!
\brief 取单一 GUI 宿主对象引用。
\since build 698
*/
inline PDefH(GUIHost&, FetchGUIHost, )
	ImplRet(FetchGlobalInstance().GetGUIHostRef())


/*!
\brief 执行程序主消息循环。
\throw GeneralEvent 激活主 shell 失败。
\note 对宿主实现，设置退出所有窗口时向 YSLib 发送退出消息。
\since build 399
*/
YF_API void
Execute(GUIApplication&, shared_ptr<Shell> = make_shared<Shells::GUIShell>());

} // namespace YSLib;

#endif

