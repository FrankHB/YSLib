/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostRenderer.h
\ingroup Helper
\brief 宿主渲染器。
\version r317
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2013-07-09 05:37:27 +0800
\par 修改时间:
	2014-11-14 22:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostRenderer
*/


#ifndef INC_Helper_HostRenderer_h_
#define INC_Helper_HostRenderer_h_ 1

#include "YModules.h"
#include YFM_Helper_HostWindow // for Host::Window;
#if YF_Multithread == 1
#	include <atomic>
#	include <thread>
#endif
#include YFM_YSLib_UI_YRenderer
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
	RenderWindow(HostRenderer& renderer, platform_ex::WindowReference wnd_ref)
		: RenderWindow(renderer, wnd_ref.GetNativeHandle())
	{}

	DefGetter(const ynothrow, HostRenderer&, Renderer, renderer)

	//! \since build 387
	void
	Refresh() override;
};


/*!
\brief 宿主窗口线程。
\since build 430
*/
class YF_API WindowThread : private OwnershipTag<Window>
{
private:
	/*!
	\brief 窗口指针。
	\note 不使用 \c unique_ptr 以便于实现线程安全的状态检查。
	\since build 428
	*/
	std::atomic<Window*> p_wnd{{}};
	std::thread thrd;

public:
	template<typename... _tParams>
	WindowThread(_tParams&&... args)
		: thrd(&WindowThread::ThreadFunc<ystdex::decay_t<_tParams>...>, this,
		ystdex::decay_copy(args)...)
	{}
	//! \since build 385
	DefDelMoveCtor(WindowThread)
	/*!
	\brief 析构：关闭窗口。
	\note 忽略实现抛出的异常。
	*/
	~WindowThread();

	//! \note 线程安全。
	DefGetter(const ynothrow, Window*, WindowPtr, p_wnd)

	/*!
	\brief 宿主消息循环。
	\since build 379
	*/
	static void
	HostLoop();

private:
	//! \todo 使用 \c INVOKE 调用。
	template<typename _func, typename... _tParams>
	void
	ThreadFunc(_func&& f, _tParams&&... args)
	{
		ThreadLoop(yforward(f)(yforward(args)...));
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
};


/*!
\brief 宿主渲染器：在宿主环境以窗口形式显示的渲染器。
\invariant 部件的位置保持在原点。
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
	//! \since build 385
	template<typename... _tParams>
	HostRenderer(UI::IWidget& wgt, _tParams&&... args)
		: HostRenderer(ystdex::identity<RenderWindow>(), wgt, yforward(args)...)
	{}
	//! \since build 430
	//@{
	template<class _tWindow, typename... _tParams>
	HostRenderer(ystdex::identity<_tWindow>, UI::IWidget& wgt,
		_tParams&&... args)
		: BufferedRenderer(),
		widget(wgt), rbuf(GetSizeOf(wgt)),
		thrd(std::mem_fn(&HostRenderer::MakeRenderWindow<_tWindow,
		ystdex::decay_t<_tParams>...>), this, ystdex::decay_copy(args)...)
	{}
	template<typename... _tParams>
	HostRenderer(int, UI::IWidget& wgt, _tParams&&... args)
		: HostRenderer(0, ystdex::identity<RenderWindow>(), wgt,
		yforward(args)...)
	{}
	template<class _tWindow, typename... _tParams>
	HostRenderer(int, ystdex::identity<_tWindow>, UI::IWidget& wgt,
		_tParams&&... args)
		: BufferedRenderer(),
		widget(wgt), rbuf(GetSizeOf(wgt)),
		thrd(std::mem_fn(&HostRenderer::MakeRenderWindowEx<_tWindow,
		ystdex::decay_t<_tParams>...>), this, ystdex::decay_copy(args)...)
	{}
	//@}
	DefDeMoveCtor(HostRenderer)

	DefGetter(const ynothrow, UI::IWidget&, WidgetRef, widget.get())
	DefGetterMem(const ynothrow, Window*, WindowPtr, thrd)

	void
	SetSize(const Drawing::Size&) override;

	//! \since build 409
	YB_NORETURN PDefH(HostRenderer*, clone, ) const override
		ImplExpr(
			throw ystdex::unimplemented("HostRenderer::clone unimplemented."));
//	DefClone(const override, HostRenderer)

	//! \since build 430
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
	\brief 调整和更新指定缓冲区内容至宿主窗口。
	\pre 断言：部件和内部缓冲区大小一致。
	\note 若宿主窗口未就绪则忽略。

	调整宿主窗口位置，保持部件位置在原点。按内部状态同步宿主窗口大小。
	调用宿主窗口 UpdateFrom 方法更新窗口内容。
	*/
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

} // namespace Host;
#endif

} // namespace YSLib;

#endif

