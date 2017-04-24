/*
	© 2013-2015, 2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ImageControl.cpp
\ingroup UI
\brief 图像显示控件。
\version r1192
\author FrankHB <frankhb1989@gmail.com>
\since build 437
\par 创建时间:
	2013-08-13 12:48:27 +0800
\par 修改时间:
	2017-04-24 23:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	ImageBrowser::ImageControl
*/


#include "ImageControl.h"
#include YFM_YSLib_Service_YPixel

namespace ImageBrowser
{

//! \since build 581
enum MenuItem : size_t
{
	Exit = 0,
	OrigSize,
	Flip,
	RotateCW,
	RotateCCW,
	CopyImage
};

//! \since build 583
static yconstexpr float def_scale(1.201F), def_rscale(1 / def_scale);

ImagePanel::ImagePanel(const Rect& r_, const Size& min_size,
	const Size& max_size)
	: Panel(r_),
	min_panel_size(min_size), max_panel_size(max_size), btnClose({{}, 24, 24}),
	lblCenter(Size(96, 32), Font(FetchDefaultTypeface().GetFontFamily(), 16,
	FontStyle::Bold), SolidCompositeBrush({0x00, 0x00, 0x00, 0xA0}),
	ColorSpace::White), hover_state(std::bind(TimedHoverState::LocateForOffset,
	std::placeholders::_1, Point(0, 24))), border(*this, 8, min_size),
	mnuContext({}, make_shared<Menu::ListType, String>(
	{u"退出", u"查看原始大小", u"翻转", u"顺时针旋转", u"逆时针旋转", u"复制"}))
{
	AddWidgets(*this, btnClose, lblCenter),
	SetVisibleOf(lblCenter, {}),
	Host::SetupTopLevelTimedTips(btnClose, hover_state, lblCloseTips, u"关闭"),
	yunseq(
	lblCenter.HorizontalAlignment = TextAlignment::Center,
	Background = SolidBrush({0x00, 0x00, 0x00, 0xC0}),
	btnClose.Background = [this](PaintEventArgs&& e){
		const auto& g(e.Target);
		const auto& pt(e.Location);
		auto& r(e.ClipArea);

		if(hover_updater.IsEntered())
			FillCircle(g, r, pt + Vec(12, 12), 10, {225, 96, 76});
		DrawCross(g, e.ClipArea, {pt + Vec(4, 4), {16, 16}}, ColorSpace::White);
		r = Rect(pt, GetSizeOf(e.GetSender()));
	},
	mnuContext.Confirmed += [this](IndexEventArgs&& e){
		switch(e.Value)
		{
		case MenuItem::Exit:
			YSLib::PostQuitMessage(0);
			break;
		case MenuItem::OrigSize:
			if(session_ptr)
				ResetSize();
			break;
		case MenuItem::Flip:
			Flip();
			break;
		case MenuItem::RotateCW:
			RotateCW();
			break;
		case MenuItem::RotateCCW:
			RotateCCW();
			break;
		case MenuItem::CopyImage:
			CopyToClipboard();
		default:
			break;
		}
	},
	FetchEvent<Resize>(*this) += [this]{
		SetLocationOf(btnClose, CalcCloseButtonLocation());
		if(session_ptr)
		{
			MoveToCenter(lblCenter),
			GetPagesRef().Resize(GetSizeOf(*this)),
			// TODO: Invalidate smaller area?
			Invalidate(*this);
		}
	},
	FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& st(FetchGUIState());

			st.CheckHeldState(e.Keys);

			const auto& k(st.GetCheckedHeldKeys());

			if(k[KeyCodes::Ctrl] && e.Keys['C'])
				CopyToClipboard();
		}
	},
	FetchEvent<KeyHeld>(*this) += OnKeyHeld,
	FetchEvent<Click>(*this) += [this](CursorEventArgs&& e){
		if(session_ptr && e.Keys[KeyCodes::Tertiary])
			ResetSize();
	},
	FetchEvent<CursorWheel>(*this) += [this](CursorWheelEventArgs&& e){
		if(session_ptr)
		{
			auto& pages(GetPagesRef());

			if(pages.ZoomByRatio(e.GetDelta() > 0 ? def_scale : def_rscale, e))
			{
				lblCenter.Text = ystdex::sfmt("%2u%%",
					unsigned(std::round(pages.GetScale() * 100.F)));
				if(!IsVisible(lblCenter))
				{
					MoveToCenter(lblCenter),
					DisplayFor(lblCenter, TimeSpan(1000));
				}
				// TODO: Invalidate smaller area?
				Invalidate(*this),
				UpdateMenuItem();
			}
		}
	},
	FetchEvent<Paint>(*this).Add([this](PaintEventArgs&& e){
		if(session_ptr)
			GetPagesRef().Brush(std::move(e));
	}, BackgroundPriority),
	FetchEvent<Click>(btnClose) += []{
		YSLib::PostQuitMessage(0);
	},
	FetchEvent<Paint>(lblCenter) += BorderBrush(BorderStyle(ColorSpace::Gray))
	);
	SetLocationOf(btnClose, CalcCloseButtonLocation());
}

bool
ImagePanel::CopyToClipboard() ynothrow
{
	return TryInvoke([this]() -> bool{
		if(const auto p = Host::GetWindowPtrOf(*this))
		{
			const auto& g(GetPagesRef().Brush.ImagePtr->GetContext());

			YTraceDe(Debug, "Size of image to be copied to clipboard: %s.",
				to_string(g.GetSize()).c_str());
			// TODO: Port to other hosted platforms.
#if YCL_Win32
			platform_ex::Clipboard(p->GetNativeHandle()).Send(g);
#endif
			return true;
		}
		return {};
	});
}

void
ImagePanel::Load(ImagePages&& src)
{
	session_ptr.reset(new Session(forward_as_tuple(std::move(src),
		GAnimationSession<InvalidationUpdater>(), Timers::Timer(),
		vector<std::chrono::milliseconds>())));
	auto& pages(GetPagesRef());

	SetSizeOf(*this, pages.GetViewSize());
	Invalidate(*this);

	// TODO: Check "Loop" metadata.
	const auto& bmps(pages.GetBitmaps());
	const auto n(bmps.size());
	auto& frame_delays(get<3>(*session_ptr));

	YTraceDe(Notice, "Loaded page count = %u.", unsigned(n));
	if(n > 1)
	{
		frame_delays.reserve(n);
		for(const auto& bmp : bmps)
		{
			// TODO: Allow user set minimal frame time.
			TimeSpan d(20);

			TryExpr(d = YSLib::max(GetFrameTimeOf(bmp), d))
			CatchExpr(LoggedEvent& e,
				YTraceDe(e.GetLevel(), "Invalid frame time found."))
			YTraceDe(Informative, "Loaded frame time = %s milliseconds.",
				to_string(d.count()).c_str());
			frame_delays.push_back(d);
		}

		const auto refresh_frame([this]{
			const auto d(get<3>(*session_ptr).at(GetPagesRef().GetIndex()));

			YTraceDe(Informative, "Set frame time = %s ms.",
				to_string(d.count()).c_str());
			get<2>(*session_ptr).Interval = d;
		});

		refresh_frame();
		UI::SetupByTimer(get<1>(*session_ptr), *this, get<2>(*session_ptr), [=]{
			if(GetPagesRef().SwitchPageDiff(1))
			{
				refresh_frame();
				Invalidate(*this);
			}
		});
	}
	UpdateMenuItem();
}

bool
ImagePanel::ResetSize()
{
	YAssertNonnull(session_ptr);

	const auto& s(GetSizeOf(*this));

	if(GetPagesRef().ZoomTo(1.F, Point(s.Width / 2, s.Height / 2)))
	{
		Invalidate(*this);
		UpdateMenuItem({});
		return true;
	}
	return {};
}

void
ImagePanel::SetupContextMenu()
{
	ResizeForContent(mnuContext);
	Host::SetupTopLevelContextMenu(*this, mhMain, mnuContext);
}

void
ImagePanel::Unload()
{
	session_ptr.reset();
}

void
ImagePanel::UpdateBrush()
{
	GetPagesRef().Brush.Update = DispatchRotatedBrush(rot);
	Invalidate(*this);
}

void
ImagePanel::UpdateMenuItem()
{
	UpdateMenuItem(!(std::fabs(GetPagesRef().GetScale() - 1.F)
		< std::numeric_limits<ImageScale>::epsilon()));
}
void
ImagePanel::UpdateMenuItem(bool b)
{
	mnuContext.SetItemEnabled(MenuItem::OrigSize, b);
}

} // namespace ImageBrowser;

