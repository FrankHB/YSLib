/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Host.h
\ingroup Helper
\brief 宿主环境。
\version r483
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:28:03 +0800
\par 修改时间:
	2013-04-07 12:22 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::Host
*/


#ifndef Inc_Helper_Host_h_
#define Inc_Helper_Host_h_ 1

#include "Helper/HostWindow.h" // for Devices::DSScreen, Host::Window;
#if YCL_MULTITHREAD == 1
#	include <thread>
#	include <mutex>
#endif
#include "YSLib/UI/yrender.h"
#include "YSLib/UI/ywidget.h" // for UI::GetSizeOf;
#include "DSScreen.h" // for ScreenBuffer;

YSL_BEGIN

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

//! \since build 382
yconstexpr wchar_t WindowClassName[]{L"YFramework Window"};


/*!
\brief 渲染窗口。
\since build 384
*/
class RenderWindow : public Window
{
private:
	std::reference_wrapper<HostRenderer> renderer;

public:
	RenderWindow(NativeWindowHandle h, HostRenderer& r)
		: Window(h), renderer(r)
	{}

	DefGetter(const ynothrow, HostRenderer&, Renderer, renderer)

	//! \since build 387
	void
	OnPaint() override;
};


/*!
\brief 宿主窗口线程。
\since build 384
*/
class WindowThread
{
private:
	unique_ptr<Window> p_wnd;
	std::thread thrd;

public:
	template<typename... _tParams>
	WindowThread(_tParams&&... args)
		: p_wnd(), thrd(std::mem_fn(&WindowThread::ThreadFunc<typename
		ystdex::qualified_decay<_tParams>::type...>), this,
		ystdex::decay_forward(yforward(args))...)
	{}
	//! \since build 385
	DefDelMoveCtor(WindowThread)
	~WindowThread();

	DefGetter(const ynothrow, Window*, WindowPtr, p_wnd.get())

private:
	template<typename _fCallable, typename... _tParams>
	void
	ThreadFunc(_fCallable&& f, _tParams&&... args)
	{
		ThreadLoop(yforward(f)(yforward(args)...));
	}

	//! \since build 389
	void
	ThreadLoop(NativeWindowHandle);
	void
	ThreadLoop(unique_ptr<Window>);

public:
	static void
	WindowLoop(Window&);
};


/*!
\brief 宿主渲染器：在宿主环境以窗口形式显示的渲染器。
\since build 384
*/
class HostRenderer : public UI::BufferedRenderer
{
private:
	std::reference_wrapper<UI::IWidget> widget;
	//! \since build 387
	ScreenRegionBuffer rbuf;
	WindowThread thrd;

public:
	//! \since build 385
	template<typename... _tParams>
	HostRenderer(UI::IWidget& wgt, _tParams&&... args)
		: BufferedRenderer(),
		widget(wgt), rbuf(GetSizeOf(wgt)),
		thrd(std::mem_fn(&HostRenderer::MakeRenderWindow<typename
			ystdex::qualified_decay<_tParams>::type...>), this,
			ystdex::decay_forward(yforward(args))...)
	{}
	DefDeMoveCtor(HostRenderer)

	DefGetter(const ynothrow, UI::IWidget&, WidgetRef, widget.get())
	DefGetterMem(const ynothrow, Window*, WindowPtr, thrd)

	void
	SetSize(const Drawing::Size&) override;

	YB_NORETURN HostRenderer*
	Clone() const override
	{
		throw LoggedEvent("HostRenderer::Clone: Not implemented.");
	}
//	DefClone(const override, HostRenderer, Clone)

	//! \since build 386
	template<typename _fCallable, typename... _tParams>
	unique_ptr<Window>
	MakeRenderWindow(_fCallable&& f, _tParams&&... args)
	{
		return unique_ptr<Window>(new
			RenderWindow(yforward(f)(yforward(args)...), *this));
	}

	void
	Update(Drawing::BitmapPtr);

	//! \since build 387
	template<typename _type>
	void
	UpdateToSurface(_type& sf)
	{
		sf.Update(rbuf);
	}
};


/*!
\brief 宿主环境。
\since build 378
*/
class Environment
{
private:
	/*!
	\brief 本机窗口对象映射。
	\note 不使用 ::SetWindowLongPtr 等 Windows API 保持跨平台及避免和其它代码冲突。
	\warning 销毁窗口前移除映射，否则可能导致未定义行为。
	\warning 非线程安全，应仅在宿主线程上操作。
	\since build 389
	*/
	map<NativeWindowHandle, Window*> wnd_map;
	/*!
	\brief 窗口对象映射锁。
	\since build 381
	*/
	mutable std::mutex wmap_mtx;
#	if YCL_MULTITHREAD == 1
#		if YCL_MINGW32
	//! \since build 397
	::HINSTANCE h_instance;
#		endif
	/*!
	\brief 宿主窗口线程。
	\since build 397
	*/
	unique_ptr<WindowThread> p_wnd_thrd;
#	endif

public:
	Environment();
	~Environment();

	/*!
	\brief 取 GUI 前景窗口。
	\since build 381
	\todo 线程安全。
	*/
	Window*
	GetForegroundWindow() const ynothrow;

	//! \since build 387
	Window&
	GetMainWindow() const ynothrow;

	/*!
	\brief 插入窗口映射项。
	\note 线程安全。
	\since build 389
	*/
	void
	AddMappedItem(NativeWindowHandle, Window*);

	/*!
	\brief 取本机句柄对应的窗口指针。
	\note 线程安全。
	\since build 389
	*/
	Window*
	FindWindow(NativeWindowHandle) const ynothrow;

	/*!
	\brief 宿主消息循环。
	\since build 379
	*/
	static void
	HostLoop();

#	if YCL_MULTITHREAD == 1
	/*!
	\brief 初始化宿主资源和本机消息循环线程并等待主窗口初始化完毕。
	\since build 397
	*/
	void
	InitHostTask();
#	endif

	/*!
	\brief 移除窗口映射项。
	\note 线程安全。
	\since build 389
	*/
	void
	RemoveMappedItem(NativeWindowHandle) ynothrow;

	//! \since build 384
	void
	UpdateRenderWindows();
};

YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif

