/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystyle.h
\ingroup Shell
\brief 图形用户界面样式。
\version 0.1253;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-08 13:21:10 +0800;
\par 修改时间:
	2011-06-04 17:04 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YStyle;
*/


#ifndef YSL_INC_SHELL_YSTYLE_H_
#define YSL_INC_SHELL_YSTYLE_H_

#include "ycomp.h"
#include "../Core/ygdi.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*
\brief 绘制部件边框用空心正则矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
*/
bool
DrawRectRoundCorner(const Graphics&, const Point&, const Size&, Color);


/*!
\brief 绘制窗口边框。
*/
void
DrawWindowBounds(IWindow*, Color);

/*!
\brief 绘制部件边框。
\note 限无缓冲区的部件。
*/
void
DrawWidgetBounds(IWidget&, Color);

/*!
\brief 绘制部件边框。
\note 和 DrawWidgetBounds 类似，但对于有缓冲区的部件，
	在此部件所在窗口的图形上下文绘制。
*/
void
DrawWidgetOutline(IWidget&, Color);


void
WndDrawFocus(IWindow*, const Size&);


void
RectDrawArrow(const Graphics&, const Point&, SDst, Rotation = RDeg0,
	Color c = Drawing::ColorSpace::Black);

void
WndDrawArrow(const Graphics&, const Rect&, SDst, Rotation = RDeg0,
	Color c = Drawing::ColorSpace::Black);


template<Color::MonoType r, Color::MonoType g, Color::MonoType b>
inline void
transform_pixel_ex(BitmapPtr dst)
{
	*dst ^= Color(r, g, b) & ~BITALPHA;
}


//平台无关色彩系统转换。

typedef struct
{
	u8 r, g, b;
} rgb_t;

typedef struct
{
	u16 h; //单位为 1/64 度。
	u16 s;
	u16 l;
} hsl_t;

inline
Color
rgb2Color(const rgb_t& c)
{
	return Color(c.r, c.g, c.b);
}

inline
rgb_t
Color2rgb(const Color& c)
{
	const rgb_t r = {c.GetR(), c.GetG(), c.GetB()};

	return r;
}

hsl_t
rgb2hsl(rgb_t c);

rgb_t
hsl2rgb(hsl_t c);


YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Styles)

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
	PDefHOperator1(Drawing::Color&, [], size_t s)
		ImplRet(colors[s])

	DefGetter(const ColorListType&, List, colors)
	DefMutableGetter(ColorListType&, List, colors)
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

