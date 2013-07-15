/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostRenderer.cpp
\ingroup Helper
\brief 宿主渲染器。
\version r132
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2013-07-09 05:37:27 +0800
\par 修改时间:
	2013-07-12 19:59 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper::HostRenderer
*/


#include "HostRenderer.h"
#include "Host.h" // for Host::Environment;
//#include "Helper/GUIApplication.h"

YSL_BEGIN

using namespace Drawing;

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

void
RenderWindow::Refresh()
{
	auto& rd(GetRenderer());
	auto& wgt(rd.GetWidgetRef());

	if(rd.Validate(wgt, wgt,
		{rd.GetContext(), Point(), rd.GetInvalidatedArea()}))
		rd.Update(rd.GetContext().GetBufferPtr());
}

void
RenderWindow::OnPaint()
{
	GSurface<WindowRegionDeviceContext> sf(GetNativeHandle());

	renderer.get().UpdateToSurface(sf);
}


WindowThread::~WindowThread()
{
	const auto p_wnd_val(GetWindowPtr());

	YAssert(bool(p_wnd_val), "Null pointer found.");

	p_wnd_val->Close();
	// NOTE: If the thread has been already completed there is no effect.
	// TODO: Exception safety: add either assertion or logging when throwing
	//	other exceptions.
	try
	{
		thrd.join();
	}
	catch(std::invalid_argument&)
	{}
	delete p_wnd_val;
}

void
WindowThread::ThreadLoop(NativeWindowHandle h_wnd)
{
	ThreadLoop(ystdex::make_unique<Window>(h_wnd));
}
void
WindowThread::ThreadLoop(unique_ptr<Window> p)
{
	YAssert(!p_wnd, "Repeated window initialization detected.");

	p_wnd = p.release();

	YAssert(p_wnd, "Null pointer found.");

	WindowLoop(*p_wnd);
}

void
WindowThread::WindowLoop(Window& wnd)
{
#	if YCL_MULTITHREAD
	auto& env(wnd.GetHost());

	env.EnterWindowThread();
#	endif
	wnd.Show();
	Environment::HostLoop();
#	if YCL_MULTITHREAD
	env.LeaveWindowThread();
#	endif
}


void
HostRenderer::SetSize(const Size& s)
{
	BufferedRenderer::SetSize(s);
}

void
HostRenderer::Update(BitmapPtr buf)
{
	YAssert(GetSizeOf(widget) == rbuf.GetSize(), "Mismatched size found.");

	rbuf.UpdateFrom(buf);
	if(const auto p_wnd = GetWindowPtr())
		rbuf.UpdateTo(p_wnd->GetNativeHandle());
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

