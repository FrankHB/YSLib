/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostWindow.cpp
\ingroup Helper
\brief 宿主环境窗口。
\version r551
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:18:46 +0800
\par 修改时间:
	2015-10-02 19:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#include "Helper/YModules.h"
#include YFM_Helper_Environment // for Host::Window, ::RAWINPUT, WM_*,
//	ystdex::pun_storage_t, ystdex::replace_cast;
#include YFM_Helper_GUIApplication // for FetchEnvironent;
#include YFM_YCLib_Input // for platform::ClearKeyStates;
#if YCL_Win32
#	include YFM_YSLib_UI_YUIContainer // for UI::FetchTopLevel;
#endif

namespace YSLib
{

using namespace Drawing;
using namespace UI;

#if YF_Hosted
namespace Host
{

Window::Window(NativeWindowHandle h)
	: Window(h, FetchEnvironment())
{}
Window::Window(NativeWindowHandle h, Environment& e)
	: HostWindow(h), env(e)
#	if YCL_Win32
	, has_hosted_caret(::CreateCaret(h, {}, 1, 1))
#	endif
{
#	if YCL_Win32
	e.AddMappedItem(h, this);
	yunseq(
	MessageMap[WM_MOVE] += [this]{
		UpdateCandidateWindowLocation();
	},
	MessageMap[WM_KILLFOCUS] += []{
		platform_ex::ClearKeyStates();
	},
	MessageMap[WM_INPUT] += [this](::WPARAM, ::LPARAM l_param) ynothrow{
		ystdex::pun_storage_t<::RAWINPUT> buf;
		unsigned size(sizeof(buf));

		// TODO: Use '{}' to simplify after resolving CWG 1368. See $2015-09
		//	@ %Documentation::Workflow::Annual2015.
		ystdex::trivially_fill_n(&buf);
		if(YB_LIKELY(::GetRawInputData(::HRAWINPUT(l_param), RID_INPUT, &buf,
			&size, sizeof(::RAWINPUTHEADER)) != unsigned(-1)))
		{
			const auto p_raw(ystdex::replace_cast<::RAWINPUT*>(&buf));

			if(YB_LIKELY(p_raw->header.dwType == RIM_TYPEMOUSE))
			{
				if(p_raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
					// NOTE: This value is safe to cast because it is
					//	specified as a signed value, see https://msdn.microsoft.com/en-us/library/windows/desktop/ms645578(v=vs.85).aspx .
					RawMouseButton = short(p_raw->data.mouse.usButtonData);
			}
		}
	},
	MessageMap[WM_CHAR] += [this](::WPARAM w_param, ::LPARAM l_param){
		lock_guard<recursive_mutex> lck(input_mutex);

		for(size_t n(l_param & 0x7FFF); n-- != 0;)
			comp_str += char16_t(w_param);
	},
	MessageMap[WM_IME_COMPOSITION] += [this]{
		UpdateCandidateWindowLocation();
	}
	);
	ystdex::unseq_apply([this](int msg){
		BindDefaultWindowProc(GetNativeHandle(), MessageMap, unsigned(msg));
	}, WM_MOVE, WM_IME_COMPOSITION);
#	endif
}
Window::~Window()
{
	env.get().RemoveMappedItem(GetNativeHandle());
#	if YCL_Win32
	if(has_hosted_caret)
		::DestroyCaret();
#	endif
}

#	if YCL_Win32
void
Window::UpdateCandidateWindowLocation()
{
	lock_guard<recursive_mutex> lck(input_mutex);

	UpdateCandidateWindowLocationUnlocked();
}
void
Window::UpdateCandidateWindowLocation(const Point& pt)
{
	if(pt != Point::Invalid)
	{
		lock_guard<recursive_mutex> lck(input_mutex);

		caret_location = pt;
		UpdateCandidateWindowLocationUnlocked();
	}
}
void
Window::UpdateCandidateWindowLocationUnlocked()
{
	if(YB_LIKELY(caret_location != Point::Invalid))
	{
		YTraceDe(Informative, "Update composition form position: %s.",
			to_string(caret_location).c_str());

		const auto h_wnd(Nonnull(GetNativeHandle()));

		if(const auto h_imc = ::ImmGetContext(h_wnd))
		{
			const auto client_pt(caret_location + GetClientLocation()
				- GetClientLocation());

			::CANDIDATEFORM cand_form{0, CFS_CANDIDATEPOS,
				{client_pt.X, client_pt.Y}, {0, 0, 0, 0}};

			// TODO: Error handling.
			::ImmSetCandidateWindow(h_imc, &cand_form);
			::ImmReleaseContext(h_wnd, h_imc);
		}
		// FIXME: Correct implementation for non-Chinese IME.
		// NOTE: See comment on %IMM32Manager::MoveImeWindow in
		//	https://src.chromium.org/viewvc/chrome/trunk/src/ui/base/ime/win/imm32_manager.cc .
		::SetCaretPos(caret_location.X, caret_location.Y);
	}
}

void
Window::UpdateTextInputFocus(IWidget& wgt, const Point& pt)
{
	auto loc(pt);
	auto& top_level(FetchTopLevel(wgt, loc));

	yunused(top_level);
	UpdateCandidateWindowLocation(loc);
}
#	endif

void
Window::UpdateFrom(Drawing::ConstBitmapPtr p, ScreenBuffer& buf)
{
	const auto h_wnd(GetNativeHandle());

#	if YCL_Win32
	if(UseOpacity)
	{
		buf.Premultiply(p);
		buf.UpdatePremultipliedTo(h_wnd, Opacity);
	}
	else
#	endif
	{
		buf.UpdateFrom(p);
		buf.UpdateTo(h_wnd);
	}
}

#	if YCL_Win32
void
Window::UpdateFromBounds(Drawing::ConstBitmapPtr p, ScreenBuffer& buf,
	const Rect& r, const Point& sp)
{
	const auto h_wnd(GetNativeHandle());

	if(UseOpacity)
	{
		buf.Premultiply(p);
		buf.UpdatePremultipliedTo(h_wnd, Opacity);
	}
	else
	{
		buf.UpdateFromBounds(p, r);
		buf.UpdateToBounds(h_wnd, r, sp);
	}
}
#	endif

} // namespace Host;
#endif

} // namespace YSLib;

