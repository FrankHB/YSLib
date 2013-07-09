/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostRenderer.h
\ingroup Helper
\brief 宿主渲染器。
\version r174
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2013-07-09 05:37:27 +0800
\par 修改时间:
	2013-07-09 05:59 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::HostRenderer
*/


#ifndef Inc_Helper_HostRenderer_h_
#define Inc_Helper_HostRenderer_h_ 1

#include "Helper/HostWindow.h" // for Host::Window;
#if YCL_MULTITHREAD == 1
#	include <thread>
#endif
#include "YSLib/UI/yrender.h"
#include "YSLib/UI/ywidget.h" // for UI::GetSizeOf;
#include "ScreenBuffer.h" // for ScreenRegionBuffer;

YSL_BEGIN

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

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
	Refresh() override;

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

	//! \since build 409
	YB_NORETURN PDefH(HostRenderer*, clone, ) const override
		ImplExpr(throw LoggedEvent("HostRenderer::clone: Not implemented."));
//	DefClone(const override, HostRenderer)

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

YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif

