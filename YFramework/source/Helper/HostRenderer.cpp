/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostRenderer.cpp
\ingroup Helper
\brief 宿主渲染器。
\version r272
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2013-07-09 05:37:27 +0800
\par 修改时间:
	2014-06-04 09:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostRenderer
*/


#include "Helper/YModules.h"
#include YFM_Helper_HostRenderer
#include YFM_Helper_Host // for Environment;
//#include YFM_Helper_GUIApplication

namespace YSLib
{

using namespace Drawing;

#if YF_Hosted
namespace Host
{

RenderWindow::RenderWindow(HostRenderer& r, NativeWindowHandle h)
	: Window(h), renderer(r)
{
#	if YCL_Win32
	MessageMap[WM_PAINT] += [this]{
		GSurface<WindowRegionDeviceContext> sf(GetNativeHandle());

		renderer.get().UpdateToSurface(sf);
	};
#	endif
}

void
RenderWindow::Refresh()
{
	auto& rd(GetRenderer());
	auto& wgt(rd.GetWidgetRef());

	if(rd.Validate(wgt, wgt,
		{rd.GetContext(), Point(), rd.GetInvalidatedArea()}))
		rd.Update(rd.GetContext().GetBufferPtr());
}


WindowThread::~WindowThread()
{
	const auto p_wnd_val(GetWindowPtr());

	YAssertNonnull(p_wnd_val);
	try
	{
#	if !YCL_Android
		try
		{
			p_wnd_val->Close();
		}
#		if YCL_Win32
		catch(Win32Exception&)
#		else
		catch(Exception&) // XXX: Use proper platform-dependent type.
#		endif
		{}
#	endif
		// NOTE: If the thread has been already completed there is no effect.
		if(thrd.joinable())
			thrd.join();
	}
	catch(std::system_error& e)
	{
		YTraceDe(Warning, "Caught std::system_error: %s.", e.what());

		yunused(e);
	}
	catch(std::exception& e)
	{
		YTraceDe(Alert, "Caught std::exception[%s]: %s.", typeid(e).name(),
			e.what());

		yunused(e);
	}
	catch(...)
	{
		YTraceDe(Alert, "Caught unknown exception.");
	}
	delete p_wnd_val;
}

void
WindowThread::ThreadLoop(NativeWindowHandle h_wnd)
{
	ThreadLoop(make_unique<Window>(h_wnd));
}
void
WindowThread::ThreadLoop(unique_ptr<Window> p)
{
	YAssert(!p_wnd, "Duplicate window initialization detected.");
	p_wnd = p.release();
	YAssertNonnull(p_wnd);
	WindowLoop(*p_wnd);
}

void
WindowThread::WindowLoop(Window& wnd)
{
#	if YF_Multithread
	auto& env(wnd.GetHost());

	env.EnterWindowThread();
#	endif
#	if !YCL_Android
	wnd.Show();
#	endif
	Environment::HostLoop();
#	if YF_Multithread
	env.LeaveWindowThread();
#	endif
}


void
HostRenderer::SetSize(const Size& s)
{
	BufferedRenderer::SetSize(s),
	rbuf.Resize(s);
}

void
HostRenderer::Update(BitmapPtr buf)
{
	YAssert(GetSizeOf(widget) == rbuf.GetSize(), "Mismatched size found.");

	if(const auto p_wnd = GetWindowPtr())
		try
		{
#	if YCL_Android
			const Rect cbounds(p_wnd->GetSize());
#	else
			const auto& cbounds(p_wnd->GetClientBounds());
#	endif
			auto bounds(cbounds);
			auto& view(widget.get().GetView());
			const auto& loc(view.GetLocation());

			if(!loc.IsZero())
			{
				bounds.GetPointRef() += loc;
				view.SetLocation({});
				rInvalidated = {{}, bounds.GetSize()};
				Validate(widget, widget, {GetContext(), Point(), rInvalidated});
			}
			bounds.GetSizeRef() = view.GetSize();
#	if !YCL_Android
			if(bounds != cbounds)
				p_wnd->SetClientBounds(bounds);
#	endif
			p_wnd->UpdateFrom(buf, rbuf);
		}
#	if YCL_Win32
		catch(Win32Exception&)
#	else
		catch(Exception&) // XXX: Use proper platform-dependent type.
#	endif
		{}
}

} // namespace Host;
#endif

} // namespace YSLib;

