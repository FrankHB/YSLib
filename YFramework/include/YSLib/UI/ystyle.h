/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystyle.h
\ingroup UI
\brief 图形用户界面样式。
\version r1326;
\author FrankHB<frankhb1989@gmail.com>
\since build 194 。
\par 创建时间:
	2010-06-08 13:21:10 +0800;
\par 修改时间:
	2012-01-10 17:09 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::YStyle;
*/


#ifndef YSL_INC_UI_YSTYLE_H_
#define YSL_INC_UI_YSTYLE_H_

#include "ycomp.h"
#include "../Service/ydraw.h"
#include <ystdex/rational.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 绘制部件边框用空心正则矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
\since build 215 。
*/
bool
DrawRectRoundCorner(const Graphics&, const Point&, const Size&, Color);


/*!
\brief 在指定图形接口上下文的矩形上画箭头。
\since build 215 。
*/
void
RectDrawArrow(const Graphics&, const Point&, SDst, Rotation = RDeg0,
	Color c = Drawing::ColorSpace::Black);

/*!
\brief 在指定图形接口上下文的窗口上画箭头。
\since build 215 。
*/
void
WndDrawArrow(const Graphics&, const Rect&, SDst, Rotation = RDeg0,
	Color c = Drawing::ColorSpace::Black);


/*!
\brief 矩形区域颜色转换。
\since build 196 。
*/
template<Color::MonoType r, Color::MonoType g, Color::MonoType b>
inline void
transform_pixel_ex(BitmapPtr dst)
{
	*dst ^= Color(r, g, b) & ~BITALPHA;
}


//平台无关色彩系统转换。

/*!
\brief HSL 颜色。
\since build 196 。
*/
typedef struct
{
	/*!
	\brief 色调。
	\since build 276 。
	\note 角度制，值 360 对应一个圆周。
	*/
	ystdex::fixed_point<u16, 9> h;
	/*!
	\brief 饱和度和亮度。
	\note 最大有效值为 1 。
	\since build 276 。
	*/
	float s, l;
} hsl_t;

/*!
\brief RGB 颜色转换为 HSL 颜色。
\since build 276 。
*/
hsl_t
ColorToHSL(Color);

/*!
\brief HSL 颜色转换为 RGB 颜色。
\since build 276 。
*/
Color
HSLToColor(hsl_t);


YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Styles)

/*!
\brief 样式区域类型。
\since b204 。
*/
typedef enum
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
} Area;


/*!
\brief 默认调色板。
\since b204 。
*/
class Palette
{
public:
	typedef Drawing::Color ItemType; //项目类型。
	typedef vector<ItemType> ColorListType; //颜色组类型。

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

YSL_END_NAMESPACE(Styles)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

