/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YStyle.cpp
\ingroup UI
\brief 图形用户界面样式。
\version r1098
\author FrankHB <frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2010-05-01 13:52:56 +0800
\par 修改时间:
	2015-03-24 16:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YStyle
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YStyle
#include YFM_YSLib_UI_YWindow
#include YFM_YSLib_UI_YGUI

using namespace ystdex;

namespace YSLib
{

namespace Drawing
{

void
DrawRectRoundCorner(const PaintContext& pc, const Size& s, Color c)
{
	const auto& g(pc.Target);
	const auto& pt(pc.Location);
	const Rect& bounds(pc.ClipArea);
	// XXX: Conversion to 'SPos' might be implementation-defined.
	const SPos x1(pt.X + 1), y1(pt.Y + 1), x2(pt.X + SPos(s.Width) - 1),
		y2(pt.Y + SPos(s.Height) - 1);

	if(YB_LIKELY(x1 <= x2 && y1 <= y2))
	{
		DrawVLineSeg(g, bounds, x1 - 1, y1, y2, c),
		DrawHLineSeg(g, bounds, y2, x1, x2, c),
		DrawVLineSeg(g, bounds, x2, y1, y2, c),
		DrawHLineSeg(g, bounds, y1 - 1, x1, x2, c);
		if(YB_LIKELY(s.Width > 4 && s.Height > 4))
		{
			DrawPoint(g, bounds, x1, y1, c);
			DrawPoint(g, bounds, x1, y2 - 1, c);
			DrawPoint(g, bounds, x2 - 1, y2 - 1, c);
			DrawPoint(g, bounds, x2 - 1, y1, c);
		}
	}
}


void
RectDrawArrow(const PaintContext& pc, SDst half_size, Rotation rot, Color c)
{
	const auto& g(pc.Target);
	const auto& pt(pc.Location);
	const Rect& bounds(pc.ClipArea);
	SPos x(pt.X), y(pt.Y);

	switch(rot)
	{
	case RDeg0:
	case RDeg180:
		{
			SPos t(pt.Y);

			for(SDst i(0); i < half_size; ++i)
				DrawVLineSeg(g, bounds, rot == 0 ? x-- : x++, y--, t++, c);
		}
		break;
	case RDeg90:
	case RDeg270:
		{
			SPos t(pt.X);

			for(SDst i(0); i < half_size; ++i)
				DrawHLineSeg(g, bounds, rot == RDeg90 ? y++ : y--, x--, t++, c);
		}
		break;
	default:
		YAssert(false, "Invalid rotation found.");
	}
}

void
RectDrawArrowOutline(const PaintContext& pc, SDst half_size, Rotation rot,
	Color c)
{
	if(half_size < 1U)
		return;

	const auto& g(pc.Target);
	const auto& pt(pc.Location);
	const Rect& bounds(pc.ClipArea);
	SPos x(pt.X), y(pt.Y);

	switch(rot)
	{
	case RDeg0:
	case RDeg180:
		{
			SPos t(pt.Y);

			for(SDst i(0); i < half_size - 1; ++i)
			{
				DrawPoint(g, bounds, {x, y--}, c),
				DrawPoint(g, bounds, {x, t++}, c);
				rot == RDeg0 ? --x : ++x;
			}
			DrawVLineSeg(g, bounds, x, y, ++t, c);
		}
		break;
	case RDeg90:
	case RDeg270:
		{
			SPos t(pt.X);

			for(SDst i(0); i < half_size - 1; ++i)
			{
				DrawPoint(g, bounds, {x--, y}, c),
				DrawPoint(g, bounds, {t++, y}, c);
				rot == RDeg90 ? ++y : --y;
			}
			DrawHLineSeg(g, bounds, y, x, ++t, c);
		}
		break;
	default:
		YAssert(false, "Invalid rotation found.");
	}
}

void
RectDrawCornerArrow(const PaintContext& pc, SDst size, Rotation rot,
	Color c)
{
	const auto& g(pc.Target);
	const auto& pt(pc.Location);
	const Rect& bounds(pc.ClipArea);
	const SPos x(pt.X);
	SPos y(pt.Y);

	switch(rot)
	{
	case RDeg0:
	case RDeg270:
		{
			SPos t(x);

			y -= size;
			for(SDst i(0); i < size; ++i)
				DrawHLineSeg(g, bounds, y++, rot == RDeg0 ? t-- : t++, x, c);
		}
		break;
	case RDeg90:
	case RDeg180:
		{
			// XXX: Conversion to 'SPos' might be implementation-defined.
			SPos t(rot == RDeg90 ? x - SPos(size) : x + SPos(size));

			for(SDst i(0); i < size; ++i)
				DrawHLineSeg(g, bounds, y++, rot == RDeg90 ? t++ : t--, x, c);
		}
		break;
	default:
		YAssert(false, "Invalid rotation found.");
	}
}

void
RectDrawCornerArrowOutline(const PaintContext& pc, SDst size, Rotation rot,
	Color c)
{
	const auto& g(pc.Target);
	const auto& pt(pc.Location);
	const Rect& bounds(pc.ClipArea);
	const SPos x(pt.X), y(pt.Y),
		inv_x(rot < RDeg180 ? x - SPos(size) : x + SPos(size)),
		inv_y(rot == RDeg0 || rot == RDeg270 ? y - SPos(size) : y + SPos(size));

	DrawVLineSeg(g, bounds, x, inv_y, y, c);
	DrawHLineSeg(g, bounds, y, inv_x, x, c);
	DrawLineSeg(g, bounds, x, inv_y, inv_x, y, c);
}

void
DrawArrow(const Graphics& g, const Rect& bounds, const Rect& r, SDst half_size,
	Rotation rot, Color c, bool outline)
{
	SPos x(r.X), y(r.Y);

	switch(rot)
	{
	case RDeg0:
	case RDeg180:
		x += (rot == RDeg180
			? (r.Width - half_size) : (r.Width + half_size)) / 2;
		y += (r.Height + 1) / 2;
		break;
	case RDeg90:
	case RDeg270:
		y += (rot == RDeg90
			? (r.Height - half_size) : (r.Height + half_size)) / 2;
		x += (r.Width + 1) / 2;
	default:
		break;
	}
	(outline ? RectDrawArrowOutline : RectDrawArrow)({g, {x, y}, bounds},
		half_size, rot, c);
}

void
DrawCornerArrow(const Graphics& g, const Rect& bounds, const Point& pt,
	SDst size, Rotation rot, Color c, bool outline)
{
	SPos x(pt.X), y(pt.Y);

	if(rot < RDeg180)
		x += size;
	if(rot == RDeg0 || rot == RDeg270)
		y += size;
	(outline ? RectDrawCornerArrowOutline : RectDrawCornerArrow)(
		{g, {x, y}, bounds}, size, rot, c);
}

void
DrawCross(const Graphics& g, const Rect& bounds, const Rect& r, Color c)
{
	if(YB_LIKELY(r.Width > 8 && r.Height > 8))
	{
		// XXX: Conversion to 'SPos' might be implementation-defined.
		const SPos xmin(r.X + 4), xmax(xmin + SPos(r.Width) - 8),
			ymin(r.Y + 4), ymax(ymin + SPos(r.Height) - 8);

		DrawLineSeg(g, bounds, xmin, ymin, xmax, ymax, c),
		DrawLineSeg(g, bounds, xmax - 1, ymin, xmin - 1, ymax, c);
	}
}

void
DrawTick(const Graphics& g, const Rect& bounds, const Rect& r, Color c1,
	Color c2)
{
	if(YB_LIKELY(r.Width > 8 && r.Height > 8))
	{
		// XXX: Conversion to 'SPos' might be implementation-defined.
		const Point p1(r.X + 2, r.Y + SPos(r.Height) / 2),
			p3(r.X + SPos(r.Width) - 2, r.Y + 1);
		Point p2(r.X + SPos(r.Width) / 2 - 1, r.Y + SPos(r.Height) - 3);

		--p2.Y;
		DrawLineSeg(g, bounds, p1 + Vec(1, 0), p2, c2);
		DrawLineSeg(g, bounds, p2, p3 + Vec(-1, 0), c2);
		p2.Y += 2;
		DrawLineSeg(g, bounds, p1 + Vec(0, 1), p2, c2);
		DrawLineSeg(g, bounds, p2, p3 + Vec(0, 1), c2);
		--p2.Y;
		DrawLineSeg(g, bounds, p1, p2, c1);
		DrawLineSeg(g, bounds, p2, p3, c1);
	}
}


void
HS::Roll(Hue delta) ynothrow
{
	auto& hue(GetHRef());

	YAssert(IsInInterval<Hue>(hue, 0, 360), "Invalid hue found.");
	delta += hue;
	hue = delta < Hue(360) ? delta : delta - Hue(360);
}


HSL::HSL(Color c) ynothrow
	: HS(yimpl(0), 0)
{
	auto& saturation(GetSRef());
	const std::uint8_t r(c.GetR()), g(c.GetG()), b(c.GetB()),
		min_color(min(min(r, g), b)), max_color(max(max(r, g), b));
	float h(0); // 此处 h 的值每 0x6 对应一个圆周。

	if(min_color == max_color)
		lightness = decltype(lightness)(min_color) / 0x100;
	else
	{
		const std::uint16_t p(max_color + min_color);
		const float q(max_color - min_color);

		lightness = decltype(lightness)(p) / 0x200;
	/*
		l = 0.2126 * r + 0.7152 * g + 0.0722 * b; // Rec. 601 luma;
		l = 0.299 * r + 0.588 * g + 0.114 * b; // Rec. 709 luma;
	*/
		saturation = q / (p < 0x100 ? p : 0x200 - p);
		if(r == max_color)
			h = (g - b) / q;
		else if(g == max_color)
			h = (b - r) / q + 0x2;
		else if(b == max_color)
			h = (r - g) / q + 0x4;
		if(h < 0)
			h += 0x6;
	}
	GetHRef() = Hue(h * 60);
}

HSL::operator Color() const ynothrow
{
	YAssert(IsInInterval<Hue>(GetH(), 0, 360), "Invalid hue found."),
	YAssert(IsInClosedInterval(GetS(), 0.F, 1.F),
		"Invalid saturation found."),
	YAssert(IsInClosedInterval(lightness, 0.F, 1.F), "Invalid light found.");
	YAssert(std::isfinite(GetS()), "Abnormal saturation found.");
	if(!std::isnormal(GetS()))
		return MakeGray(lightness > 255.F / 0x100 ? 0xFF : lightness * 0x100);

	float t2((lightness < 0.5F ? lightness * (1 + GetS())
		: (lightness + GetS() - lightness * GetS())) * 0x100),
		t1((lightness * 0x200) - t2);
	// 每个元素对应一个 RGB 分量，值 360 对应一个圆周。
	float tmp[3]{GetH() + Hue(120), GetH(), GetH() - Hue(120)};
	float dc[3]; //对应 RGB 分量。

	for(size_t i(0); i < 3; ++i)
	{
		if(tmp[i] < 0)
			tmp[i] += 360;
		else if(tmp[i] > 360)
			tmp[i] -= 360;
		if(tmp[i] < 60)
			dc[i] = t1 + (t2 - t1) * tmp[i] / 60;
		else if(tmp[i] < 180)
			dc[i] = t2;
		else if(tmp[i] < 240)
			dc[i] = t1 + (t2 - t1) * (240 - tmp[i]) / 60;
		else
			dc[i] = t1;
		if(dc[i] > 0xFF)
			dc[i] = 0xFF;
	}
	return Color(dc[0], dc[1], dc[2]);
}


HSV::HSV(Color c) ynothrow
{
	using int_type = ystdex::make_widen_int<MonoType, true>::type;
	static_assert(std::is_signed<int_type>::value, "Invalid type found.");
	// TODO: Check range by constexpr 'std::numeric_limits' members.
	const int_type r(c.GetR()), g(c.GetG()), b(c.GetB()),
		max_color(max(r, max(g, b))),
		delta(max_color - min(r, min(g, b)));

	yunseq(GetSRef() = max_color == 0 ? 0.F : delta / float(max_color),
		value = max_color / 255.F);
	GetHRef() = Hue((max_color > 0 ? [=]{
		int_type res(0);

		if(r == max_color)
			res = g - b;
		else if(g == max_color)
			res = delta * 2 + b - r;
		else if(b == max_color)
			res = delta * 4 + r - g;
		if(res < 0)
			res += delta * 6;
		return res;
	}() * 60.F / delta : -360.F));
}

HSV::operator Color() const ynothrow
{
	static_assert(std::numeric_limits<MonoType>::max() == 255,
		"Invalid mono type found.");
	const auto v(value * 255);
	const auto s(GetS());

	YAssert(IsInInterval<float>(GetH(), 0, 360), "Invalid hue found."),
	YAssert(IsInClosedInterval(s, 0.F, 1.F), "Invalid saturation found."),
	YAssert(IsInClosedInterval(v, 0.F, 256.F), "Invalid value found.");

	if(s > 0.F)
	{
		float h(GetH() / 60.F);
		const unsigned k(std::floor(h));
		const float f(GetH() / 60.F - k), aa(v * (1.F - s)),
			bb(v * (1.F - s * f)), cc(v * (1.F - (s * (1.F - f))));

		switch(k)
		{
		case 0U:
			return {v, cc, aa};
		case 1U:
			return {bb, v, aa};
		case 2U:
			return {aa, v, cc};
		case 3U:
			return {aa, bb, v};
		case 4U:
			return {cc, aa, v};
		case 5U:
			return {v, aa, bb};
		default:
			break;
		}
		YAssert(false, "Invalid state found.");
	}
	return MakeGray(v);
}

} // namespace Drawing;

namespace UI
{

namespace Styles
{

Palette::Palette()
	: colors{
		{0, 0, 0}, // Null;
		{10, 59, 118}, // Desktop;
		{255, 255, 255}, // Window;
		{240, 240, 240}, // Panel;
		{237, 237, 237}, // Track;
		{171, 171, 171}, // Workspace;
		{160, 160, 160}, // Shadow;
		{105, 105, 105}, // DockShadow;
		{227, 227, 227}, // Light;
		{100, 100, 100}, // Frame;
		{51, 153, 255}, // Highlight;
		{158, 62, 255}, // BorderFill;
		{86, 157, 229}, // ActiveBorder;
		{180, 180, 180}, // InactiveBorder;
		{153, 180, 209}, // ActiveTitle;
		{191, 205, 219}, // InactiveTitle;

		{255, 255, 255}, // HighlightText;
		{0, 0, 0}, // WindowText;
		{0, 0, 0}, // PanelText;
		{109, 109, 109}, // GrayText;
		{0, 0, 0}, // TitleText;
		{67, 78, 84}, // InactiveTitleText;
		{0, 102, 204} // HotTracking;
	}
{
	//	colors[ActiveBorder] = Color(180, 180, 180),
	//	colors[InactiveBorder] = Color(244, 247, 252);
	//"GradientActiveTitle"="185 209 234"
	//"GradientInactiveTitle"="215 228 242"
}

pair<Drawing::Color, Drawing::Color>
Palette::GetPair(Palette::ColorListType::size_type n1,
	Palette::ColorListType::size_type n2) const
{
	return {colors[n1], colors[n2]};
}


void
Painter::operator()(PaintEventArgs&& e) const
{
	FetchGUIConfiguration().Styles.PaintAsStyle(key, std::move(e));
}


void
StyleMap::PaintAsStyle(const Key& key, PaintEventArgs&& e)
{
	using ystdex::is_undereferenceable;

	YAssert(!empty(), "No style found.");
	YAssert(!is_undereferenceable(current), "Invalidate style state found.");

	const auto& table(current->second);
	auto i(table.find(key));

	if(i == table.cend())
	{
		const auto& de_table(cbegin()->second);

		i = de_table.find(key);
		if(i == de_table.end())
			return;
	}
	i->second(std::move(e));
}

void
StyleMap::Remove(const string& name)
{
	if(!name.empty())
	{
		if(current->first == name)
			current = cbegin();
		erase(name);
	}
}

void
StyleMap::Switch(const string& name)
{
	YTraceDe(Notice, "Style '%s' requested to be switched.", name.c_str());

	const auto i(find(name));
	const bool exist(i != cend());

	YTraceDe(Notice, "Style '%s'%s found in style map.", name.c_str(),
		exist ? "" : " not");
	current = exist ? i : cbegin();
}


HandlerTable&
FetchDefault()
{
	return FetchGUIConfiguration().Styles.at({});
}

} // namespace Styles;

} // namespace UI;

} // namespace YSLib;

