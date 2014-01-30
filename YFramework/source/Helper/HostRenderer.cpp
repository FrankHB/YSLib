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
\version r198
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2013-07-09 05:37:27 +0800
\par 修改时间:
	2014-01-25 12:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostRenderer
*/


#include "Helper/YModules.h"
#include YFM_Helper_HostRenderer
#include YFM_Helper_Host // for Host::Environment;
//#include YFM_Helper_GUIApplication

namespace YSLib
{

using namespace Drawing;

#if YF_Hosted
namespace Host
{

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

	try
	{
		p_wnd_val->Close();
	}
	catch(Win32Exception&)
	{}
	// NOTE: If the thread has been already completed there is no effect.
	// FIXME: Exception safety: add either assertion or logging when throwing
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
	ThreadLoop(make_unique<Window>(h_wnd));
}
void
WindowThread::ThreadLoop(unique_ptr<Window> p)
{
	YAssert(!p_wnd, "Duplicate window initialization detected.");

	p_wnd = p.release();

	YAssert(p_wnd, "Null pointer found.");

	WindowLoop(*p_wnd);
}

void
WindowThread::WindowLoop(Window& wnd)
{
#	if YF_Multithread
	auto& env(wnd.GetHost());

	env.EnterWindowThread();
#	endif
	wnd.Show();
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
			const auto& cbounds(p_wnd->GetClientBounds());
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
			if(bounds != cbounds)
				p_wnd->SetClientBounds(bounds);
			p_wnd->UpdateFrom(buf, rbuf);
		}
		catch(Win32Exception&)
		{}
}

} // namespace Host;
#endif

} // namespace YSLib;

