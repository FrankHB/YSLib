/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Border.cpp
\ingroup UI
\brief 图形用户界面边框。
\version r205
\author FrankHB <frankhb1989@gmail.com>
\since build 443
\par 创建时间:
	2013-09-06 23:25:42 +0800
\par 修改时间:
	2015-04-29 01:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Border
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Border
#include YFM_YSLib_Service_YDraw
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_UI_YControl

namespace YSLib
{

using namespace Drawing;

namespace UI
{

BorderStyle::BorderStyle()
	: ActiveColor(FetchGUIConfiguration().Colors[Styles::ActiveBorder]),
	InactiveColor(FetchGUIConfiguration().Colors[Styles::InactiveBorder])
{}


void
BorderBrush::operator()(PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	DrawRect(e.Target, e.ClipArea, e.Location, GetSizeOf(sender),
		IsFocused(sender) ? Style.ActiveColor : Style.InactiveColor);
}


BorderResizer::Area
BorderResizer::CheckArea(const Point& pt)
{
	const auto comp([](SPos q, SPos left, SDst left2) ynothrow{
		if(q >= 0)
		{
			// XXX: Conversion to 'SPos' might be implementation-defined.
			const SPos a[]{SPos(), left, SPos(left2)};
			const auto n(SwitchInterval(q, a, 3));

			if(n < 3)
				return BorderArea(n);
		}
		return BorderArea::Center;
	});
	const Size& size(GetSizeOf(widget));

	return {comp(pt.X, Margin.Left, size.Width - GetHorizontalOf(Margin)),
		comp(pt.Y, Margin.Top, size.Height - GetVerticalOf(Margin))};
}

void
BorderResizer::Wrap()
{
	auto& controller(widget.get().GetController());

	yunseq(
	FetchEvent<TouchDown>(controller).Add([this](CursorEventArgs&& e){
		yunseq(orig_loc = FetchGUIState().CursorLocation,
			locked_bounds = GetBoundsOf(widget), focused = CheckArea(e));
	}, 0xE0),
	FetchEvent<TouchHeld>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct
			&& focused != Area(BorderArea::Center, BorderArea::Center))
		{
			auto& st(FetchGUIState());

			if(st.CheckDraggingOffset())
			{
				const auto offset(st.CursorLocation - orig_loc);
				auto bounds(locked_bounds);

				// XXX: Conversion to 'SPos' might be implementation-defined.
				switch(focused.first)
				{
				case BorderArea::Left:
					bounds.Width = SDst(max<SPos>(SPos(MinSize.Width),
						SPos(locked_bounds.Width) - offset.X));
					bounds.X += SPos(locked_bounds.Width) - SPos(bounds.Width);
					break;
				case BorderArea::Right:
					bounds.Width = SDst(max<SPos>(SPos(MinSize.Width),
						SPos(locked_bounds.Width) + offset.X));
					break;
				default:
					;
				}
				switch(focused.second)
				{
				case BorderArea::Up:
					bounds.Height = SDst(max<SPos>(SPos(MinSize.Height),
						SPos(locked_bounds.Height) - offset.Y));
					bounds.Y += SPos(locked_bounds.Height)
						- SPos(bounds.Height);
					break;
				case BorderArea::Down:
					bounds.Height = SDst(max<SPos>(SPos(MinSize.Height),
						SPos(locked_bounds.Height) + offset.Y));
					break;
				default:
					;
				}
				YTraceDe(Notice, "BorderResizer: new bounds = %s.",
					to_string(bounds).c_str());
				InvalidateParent(widget);
				if(RootMode)
				{
					const auto&
						off(bounds.GetPoint() - locked_bounds.GetPoint());

					SetBoundsOf(widget, bounds);

					const auto& nloc(FetchGUIState().CursorLocation - off);

					if(bounds.Width != MinSize.Width)
						orig_loc.X = nloc.X;
					if(bounds.Height != MinSize.Height)
						orig_loc.Y = nloc.Y;
					locked_bounds = GetBoundsOf(widget);
					locked_bounds.GetPointRef() -= off;
				}
				else
					SetBoundsOf(widget, bounds);
			}
			e.Handled = true;
			// XXX: Paint context target invalidated.
		}
	}, 0xE0),
	FetchEvent<Click>(controller).Add([this](CursorEventArgs&& e){
		CallEvent<ClickAcross>(widget, e);
	}, 0xE0),
	FetchEvent<ClickAcross>(controller).Add([this]{
		yunseq(orig_loc = Point::Invalid, locked_bounds = Rect(),
			focused = {BorderArea::Center, BorderArea::Center});
	}, 0xE0)
	);
}

} // namespace UI;

} // namespace YSLib;

