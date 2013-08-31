/*
	Copyright by FrankHB 2010-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystyle.h
\ingroup UI
\brief 图形用户界面样式。
\version r427
\author FrankHB <frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2010-06-08 13:21:10 +0800
\par 修改时间:
	2013-08-31 14:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YStyle
*/


#ifndef YSL_INC_UI_ystyle_h_
#define YSL_INC_UI_ystyle_h_ 1

#include "YComponent.h"
#include "../Service/ydraw.h"
#include <ystdex/rational.hpp>

namespace YSLib
{

namespace Drawing
{

/*!
\brief 绘制部件边框用空心标准矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
\since build 215
*/
YF_API bool
DrawRectRoundCorner(const Graphics&, const Point&, const Size&, Color);


/*!
\brief 在指定图形接口上下文的矩形上画箭头。
\since build 215
*/
YF_API void
RectDrawArrow(const Graphics&, const Point&, SDst, Rotation = RDeg0,
	Color c = ColorSpace::Black);

/*!
\brief 在指定图形接口上下文上画箭头。
\since build 302
*/
YF_API void
DrawArrow(const Graphics&, const Rect&, SDst = 4, Rotation = RDeg0,
	Color = ColorSpace::Black);

/*!
\brief 在指定图形接口上下文中的指定点为边框左上角使用指定颜色绘制交叉直线段（“×”）。
\note 若不满足长和宽都大于 8 则忽略。
\since build 302
*/
YF_API void
DrawCross(const Graphics&, const Point&, const Size&, Color);


/*!
\brief 矩形区域颜色转换。
\since build 417
*/
template<MonoType r, MonoType g, MonoType b>
inline void
transform_pixel_ex(BitmapPtr dst)
{
	const Color d(*dst);

	*dst = Color(d.GetR() ^ r, d.GetG() ^ g, d.GetB() ^ b);
}


//平台无关色彩系统类型和转换。

/*!
\brief 色调。
\note 角度制，值 360 对应一个圆周。
\since build 302
*/
using Hue = ystdex::fixed_point<u16, 9>;


/*!
\brief HSL 颜色。
\since build 433
*/
struct hsl_t
{
	/*!
	\brief 色调。
	\since build 302
	*/
	Hue h;
	/*!
	\brief 饱和度和亮度。
	\note 最大有效值为 1 。
	\since build 276
	*/
	float s, l;
};

/*!
\brief RGB 颜色转换为 HSL 颜色。
\relates hsl_t
\since build 276
*/
YF_API hsl_t
ColorToHSL(Color);

/*!
\brief HSL 颜色转换为 RGB 颜色。
\relates hsl_t
\since build 276
*/
YF_API Color
HSLToColor(hsl_t);


} // namespace Drawing;

namespace UI
{

namespace Styles
{

/*!
\brief 样式区域类型。
\since build 416
*/
enum Area
{
	//背景和填充区域。
	Null = 0, //!< 空（屏幕背景）。
	Desktop = 1, //!< 桌面背景。
	Window = 2, //!< 窗口背景。
	Panel = 3, //!< 面板背景。
	Track = 4, //!< 滚动条背景。
	Workspace = 5, //!< 应用程序工作区背景。
	Shadow = 6, //!< 阴影背景。
	DockShadow = 7, //!< 容器阴影背景。
	Light = 8, //!< 明亮背景。
	Frame = 9, //!< 框架背景。
	Highlight = 10, //!< 高亮背景。
	BorderFill = 11, //!< 边框填充。
	ActiveBorder = 12, //!< 活动边框背景。
	InactiveBorder = 13, //!< 不活动边框背景。
	ActiveTitle = 14, //!< 活动标题背景。
	InactiveTitle = 15, //!< 不活动标题背景。

	//前景和文本。
	HighlightText = 16,
	WindowText = 17,
	PanelText = 18,
	GrayText = 19,
	TitleText = 20,
	InactiveTitleText = 21,
	HotTracking = 22,

	EndArea = 24
};


/*!
\brief 默认调色板。
\since build 204
*/
class YF_API Palette
{
public:
	using ItemType = Drawing::Color; //项目类型。
	using ColorListType = vector<ItemType>; //颜色组类型。

private:
	ColorListType colors; //颜色组。
//	Drawing::Color colors[EndArea];

public:
	/*!
	\brief 无参数构造。
	\note 得到包含默认界面颜色配置的调色板。
	*/
	Palette();

	/*!
	\brief 取颜色引用。
	*/
	PDefHOp(Drawing::Color&, [], size_t s)
		ImplRet(colors[s])

	DefGetter(const ynothrow, const ColorListType&, List, colors)
	DefGetter(ynothrow, ColorListType&, List, colors)
	/*!
	\brief 取指定下标的颜色对。
	*/
	pair<Drawing::Color, Drawing::Color>
	GetPair(ColorListType::size_type, ColorListType::size_type) const;
};

} // namespace Styles;

} // namespace UI;

} // namespace YSLib;

#endif

