/*
	© 2014, 2016-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup Clock
\brief 主界面。
\version r117
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2014-12-16 15:16:03 +0800
\par 修改时间:
	2017-04-24 23:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Clock::Main
*/


#include <YSBuild.h>
#include <ctime>
using namespace std;
using namespace YSLib;

namespace Clock
{

using namespace YSLib;
using namespace Drawing;
using namespace UI;

} // namespace Clock;

int
main()
{
	using namespace Host;
	using namespace Clock;
	static yconstexpr const float pi2(3.14159F * 2);
	static yconstexpr const SDst len(400), rad(len / 2 - 6);
	static yconstexpr const Vec center(len / 2, len / 2);
	static std::time_t old, now;
	GUIApplication app;
	GAnimationSession<InvalidationUpdater> ani;
	Timers::Timer tmr(std::chrono::milliseconds(1000 / 10));
	MLabel lbl(Font(FetchDefaultTypeface().GetFontFamily(), 24,
		FontStyle::Bold), {0xE0, 0x80, 0x20, 0x80}, TextAlignment::Center);
	Panel pnl({400, 300, len, len + 80});

	yunseq(
	lbl.Margin = {0, 0, len, 0},
	pnl.Background = SolidBrush({0x40, 0x40, 0x40, 0x40}),
	FetchEvent<Click>(pnl) += [](CursorEventArgs&& e){
		if(e.GetKeys()[KeyCodes::Secondary])
			YSLib::PostQuitMessage(0);
	},
	FetchEvent<Paint>(pnl) += [&](PaintEventArgs&& e){
		using namespace ColorSpace;
		static const auto calc_pt([](float phi, float r)->Point{
			yunseq(phi *= pi2, r *= rad);
			return {std::sin(phi) * r, -std::cos(phi) * r};
		});
		const auto& g(e.Target);
		const auto& pt(e.Location + center);
		const auto& bounds(e.ClipArea);
		const auto drawl([&](float phi, float r, Color c){
			DrawLineSeg(g, bounds, pt, pt + calc_pt(phi, r), c);
		});

		DrawCircle(g, bounds, pt, rad, Blue);
		FillCircle(g, bounds, pt, rad - 4, {211, 211, 221, 0xC0});
		FillCircle(g, bounds, pt, 6, Navy);
		for(size_t i(0); i < 60; ++i)
			FillCircle(g, bounds, pt + calc_pt(i / 60.F, .95F),
				i % 5 == 0 ? 4 : 2, i % 5 == 0 ? Color(72, 72, 192, 0xD0)
				: Color(72, 160, 72, 0xD0));

		const auto t(std::localtime(&now));

		drawl((t->tm_hour + t->tm_min / 60.F) / 12, .5F, Maroon);
		drawl((t->tm_min + t->tm_sec / 60.F) / 60, .8F, Fuchsia);
		drawl(t->tm_sec / 60.F, .9F, Green);
		lbl.Text = ystdex::sfmt("%d-%02d-%02d %02d:%02d:%02d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
			t->tm_sec);
		lbl(std::move(e));
	}
	);
	UI::SetupByTimer(ani, pnl, tmr, [&]{
		std::time(&now);

		if(old != now)
		{
			YTraceDe(Informative, "Time updated.");
			Invalidate(pnl);
			old = now;
		}
	});
	ShowTopLevelDraggable(pnl);
	// TODO: Port to other hosted platforms.
#if YCL_Win32
	GetWindowPtrOf(pnl)->UseOpacity = true;
#endif
	Execute(app);
}

