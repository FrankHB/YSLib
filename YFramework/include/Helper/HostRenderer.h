/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostRenderer.h
\ingroup Helper
\brief 宿主渲染器。
\version r527
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2013-07-09 05:37:27 +0800
\par 修改时间:
	2016-02-09 19:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostRenderer
*/


#ifndef INC_Helper_HostRenderer_h_
#define INC_Helper_HostRenderer_h_ 1

#include "YModules.h"
#include YFM_Helper_HostWindow // for Host::Window, ystdex::unimplemented;
#if YF_Multithread == 1
#	include <atomic>
#	include <thread>
#endif
#include YFM_YSLib_UI_YRenderer // for UI::BufferedRenderer;
#include YFM_YSLib_UI_YWidget // for UI::GetSizeOf;
#include YFM_Helper_ScreenBuffer // for ScreenRegionBuffer;

namespace YSLib
{

#if YF_Hosted
namespace Host
{

/*!
\brief 渲染窗口。
\since build 430
*/
class YF_API RenderWindow : public Window
{
private:
	//! \since build 554
	lref<HostRenderer> renderer;

public:
	RenderWindow(HostRenderer&, NativeWindowHandle);
	//! \since build 548
	RenderWindow(HostRenderer& rd, Host::WindowReference wnd_ref)
		: RenderWindow(rd, wnd_ref.GetNativeHandle())
	{}

	DefGetter(const ynothrow, HostRenderer&, Renderer, renderer)

	/*!
	\brief 刷新。
	\sa HostRenderer::RefreshForWidget
	\since build 387
	*/
	void
	Refresh() override;
};


/*!
\brief 宿主窗口线程。
\since build 430

使用合适的参数取特定类型的值表示本机窗口，并在新线程中运行这个窗口的循环。
本机窗口类型可以是 NativeWindowHandle 、 WindowReference 或 unique_ptr<Window> 。
*/
class YF_API WindowThread : private OwnershipTag<Window>
{
public:
	//! \since build 589
	using Guard = ystdex::any;
	//! \since build 589
	using GuardGenerator = std::function<Guard(Window&)>;

private:
	/*!
	\brief 窗口指针。
	\note 不使用 \c unique_ptr 以便于实现线程安全的状态检查。
	\since build 600
	*/
	std::atomic<Window*> p_window{{}};
	std::thread thrd;

public:
	/*!
	\brief 在进入线程时取守护对象。
	\since build 589
	*/
	GuardGenerator GenerateGuard{};

	template<typename... _tParams>
	WindowThread(_tParams&&... args)
		: WindowThread({}, yforward(args)...)
	{}
	//! \since build 589
	template<typename... _tParams>
	WindowThread(GuardGenerator guard_gen, _tParams&&... args)
		: thrd(&WindowThread::ThreadFunc<_tParams...>, this, yforward(args)...),
		GenerateGuard(guard_gen)
	{}
	//! \since build 385
	DefDelMoveCtor(WindowThread)
	/*!
	\brief 析构：关闭窗口。
	\note 忽略实现抛出的异常。
	*/
	~WindowThread();

	/*!
	\note 线程安全。
	\since build 670
	*/
	DefGetter(const ynothrow, observer_ptr<Window>, WindowPtr,
		make_observer(static_cast<Window*>(p_window)))

	/*!
	\brief 默认生成守护对象。
	\since build 589

	生成的守护对象在构造和析构时分别调用 EnterWindowThread 和 LeaveWindowThread 。
	*/
	static Guard
	DefaultGenerateGuard(Window&);

private:
	//! \since build 623
	template<typename _func, typename... _tParams>
	void
	ThreadFunc(_func&& f, _tParams&&... args) ynothrow
	{
		FilterExceptions([&, this]{
			// XXX: Blocked. 'yforward' cause G++ 5.2 crash: segmentation
			//	fault.
			ThreadLoop(ystdex::invoke(std::forward<_func&&>(f),
				std::forward<_tParams&&>(args)...));
		});
	}

	//! \since build 389
	void
	ThreadLoop(NativeWindowHandle);
	//! \since build 548
	PDefH(void, ThreadLoop, WindowReference wnd_ref)
		ImplRet(ThreadLoop(wnd_ref.GetNativeHandle()))
	void
	ThreadLoop(unique_ptr<Window>);

public:
	static void
	WindowLoop(Window&);

private:
	//! \since build 589
	static void
	WindowLoop(Window&, GuardGenerator);
};


/*!
\brief 宿主渲染器：在宿主环境以窗口形式显示的渲染器。
\invariant 部件的位置保持在原点。
\invariant 本机缓冲区大小和 UI::BufferedRenderer 缓冲区大小一致。
\since build 430
*/
class YF_API HostRenderer : public UI::BufferedRenderer
{
private:
	//! \since build 554
	lref<UI::IWidget> widget;
	//! \since build 387
	ScreenRegionBuffer rbuf;
	WindowThread thrd;

public:
	/*!
	\brief 根模式：指定部件为通过外部接口和宿主交互的顶层部件。
	\since build 571
	*/
	bool RootMode = {};

	/*!
	\brief 构造：使用指定部件和窗口线程参数创建 RenderWindow 对象。
	\since build 385
	*/
	template<typename... _tParams>
	HostRenderer(UI::IWidget& wgt, _tParams&&... args)
		: HostRenderer(ystdex::identity<RenderWindow>(), wgt, yforward(args)...)
	{}
	//! \since build 430
	//@{
	//! \brief 构造：使用指定部件和窗口线程参数创建指定类型的渲染窗口。
	template<class _tWindow, typename... _tParams>
	HostRenderer(ystdex::identity<_tWindow>, UI::IWidget& wgt,
		_tParams&&... args)
		: HostRenderer(ystdex::empty_base<WindowThread>(), wgt,
		std::mem_fn(&HostRenderer::MakeRenderWindow<
		_tWindow, ystdex::decay_t<_tParams>...>), this, yforward(args)...)
	{}
	//! \since build 589
	template<class _tWindow, typename... _tParams>
	HostRenderer(ystdex::identity<_tWindow>, UI::IWidget& wgt,
		WindowThread::GuardGenerator guard_gen, _tParams&&... args)
		: HostRenderer(ystdex::empty_base<WindowThread>(), wgt, guard_gen,
		std::mem_fn(&HostRenderer::MakeRenderWindow<_tWindow,
		ystdex::decay_t<_tParams>...>), this, yforward(args)...)
	{}
	//! \brief 构造：使用指定部件和窗口线程参数间接创建 RenderWindow 对象。
	template<typename... _tParams>
	HostRenderer(int, UI::IWidget& wgt, _tParams&&... args)
		: HostRenderer(0, ystdex::identity<RenderWindow>(), wgt,
		yforward(args)...)
	{}
	//! \brief 构造：使用指定部件和窗口线程参数间接创建指定类型的渲染窗口。
	template<class _tWindow, typename... _tParams>
	HostRenderer(int, ystdex::identity<_tWindow>, UI::IWidget& wgt,
		_tParams&&... args)
		: HostRenderer(ystdex::empty_base<WindowThread>(), wgt,
		std::mem_fn(&HostRenderer::MakeRenderWindowEx<
		_tWindow, ystdex::decay_t<_tParams>...>), this, yforward(args)...)
	{}
	//! \since build 589
	template<class _tWindow, typename... _tParams>
	HostRenderer(int, ystdex::identity<_tWindow>, UI::IWidget& wgt,
		WindowThread::GuardGenerator guard_gen, _tParams&&... args)
		: HostRenderer(ystdex::empty_base<WindowThread>(), wgt, guard_gen,
		std::mem_fn(&HostRenderer::MakeRenderWindowEx<_tWindow,
		ystdex::decay_t<_tParams>...>), this, yforward(args)...)
	{}
	//@}
	/*!
	\brief 析构：释放资源。
	\since build 571

	Win32 平台：从宿主环境桌面移除部件；对可转换为 UI::Widget 的部件，
	首先以优先级 0xFF 发送 SM_Task 消息复位视图。
	*/
	~HostRenderer();

private:
	//! \since build 589
	template<typename... _tParams>
	HostRenderer(ystdex::empty_base<WindowThread>, UI::IWidget& wgt,
		_tParams&&... args)
		: BufferedRenderer(),
		widget(wgt), rbuf(GetSizeOf(wgt)), thrd(yforward(args)...)
	{
		InitWidgetView();
	}

public:
	DefDeMoveCtor(HostRenderer)

	//! \since build 536
	DefGetter(ynothrow, ScreenRegionBuffer&, BufferRef, rbuf)
	DefGetter(const ynothrow, UI::IWidget&, WidgetRef, widget.get())
	//! \since build 670
	DefGetterMem(const ynothrow, observer_ptr<Window>, WindowPtr, thrd)

	/*!
	\brief 设置 BufferedRenderer 缓冲区和本机缓冲区大小。
	\post <tt>GetContext().GetSize() == rbuf.Lock()->GetSize()</tt> 。
	\note 可能导致原缓冲区失效。
	\sa BufferedRenderer::SetSize
	*/
	void
	SetSize(const Drawing::Size&) override;

	/*!
	\brief 调整缓冲区大小确保和视图大小一致。
	\post <tt>GetContext().GetSize() == rbuf.Lock()->GetSize()</tt> 。
	\post <tt>UI::GetSizeOf(widget) == GetContext().GetSize()</tt> 。
	\return 是否发现不一致而进行调整。
	\sa SetSize
	\since build 590
	*/
	bool
	AdjustSize();

private:
	/*!
	\brief 初始化部件视图。
	\sa RootMode
	\since build 571

	Win32 平台：对可转换为 UI::Widget 的部件设置视图；添加部件至宿主环境桌面。
	根据初始化的部件视图决定 RootMode 。
	*/
	void
	InitWidgetView();

public:
	/*!
	\brief 创建指定类型的渲染窗口。
	\since build 430
	*/
	//@{
	template<class _tWindow, typename _func, typename... _tParams>
	unique_ptr<Window>
	MakeRenderWindow(_func&& f, _tParams&&... args)
	{
		return make_unique<_tWindow>(*this, yforward(f)(yforward(args)...));
	}

	template<class _tWindow, typename _func, typename... _tParams>
	unique_ptr<Window>
	MakeRenderWindowEx(_func&& f, _tParams&&... args)
	{
		return make_unique<_tWindow>(*this, yforward(f)(), yforward(args)...);
	}
	//@}

	/*!
	\brief 从部件刷新：更新缓冲区，必要时调整缓冲区和视图大小一致。
	\note 间接断言：上下文缓冲区指针非空。
	\sa AdjustSize
	\since build 590
	*/
	void
	RefreshForWidget();

	/*!
	\brief 调整和更新指定缓冲区内指定边界的内容至宿主窗口。
	\pre 断言：指针参数非空。
	\throw LoggedEvent 宿主窗口就绪时本机缓冲区大小和视图大小不一致。
	\note 若宿主窗口未就绪则忽略。
	\sa AdjustSize
	\since build 591

	调整宿主窗口位置，保持部件位置在原点。按内部状态同步宿主窗口大小。
	调用宿主窗口 UpdateFrom 方法更新窗口内容。
	*/
	YB_NONNULL(1) void
	Update(Drawing::ConstBitmapPtr, const Drawing::Rect&);

	//! \since build 387
	template<typename _type>
	void
	UpdateToSurface(_type& sf)
	{
		sf.Update(Deref(rbuf.Lock()));
	}

#if YCL_Win32
	//! \since build 591
	template<typename _type, typename... _tParams>
	void
	UpdateBoundsToSurface(_type& sf, _tParams&&... args)
	{
		sf.UpdateBounds(Deref(rbuf.Lock()), yforward(args)...);
	}
#endif

	/*!
	\brief 等待宿主窗口就绪。
	\since build 471
	*/
	Window&
	Wait();

	//! \since build 409
	YB_NORETURN PDefH(HostRenderer*, clone, ) const override
		ImplThrow(ystdex::unimplemented("HostRenderer::clone unimplemented."))
//	DefClone(const override, HostRenderer)
};

} // namespace Host;
#endif

} // namespace YSLib;

#endif

