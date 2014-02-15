/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystyle.h
\ingroup UI
\brief 图形用户界面样式。
\version r627
\author FrankHB <frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2010-06-08 13:21:10 +0800
\par 修改时间:
	2014-02-14 22:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YStyle
*/


#ifndef YSL_INC_UI_ystyle_h_
#define YSL_INC_UI_ystyle_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YComponent
#include YFM_YSLib_Service_YDraw
#include <ystdex/rational.hpp>
#include <typeindex>

namespace YSLib
{

namespace Drawing
{

/*!
\brief 绘制部件边框用空心标准矩形。
\note 右下角顶点坐标
	(pc.Location.X + s.Width - 1, pc.Location.Y + s.Height - 1) 。
\since build 453
*/
YF_API void
DrawRectRoundCorner(const PaintContext& pc, const Size& s, Color);


/*!
\brief 在指定上下文的矩形中画箭头。
\since build 453
*/
YF_API void
RectDrawArrow(const PaintContext&, SDst, Rotation = RDeg0,
	Color = ColorSpace::Black);

/*!
\brief 在指定图形接口上下文上描画箭头。
\since build 302
*/
YF_API void
DrawArrow(const Graphics&, const Rect&, SDst = 4, Rotation = RDeg0,
	Color = ColorSpace::Black);

/*!
\brief 在指定图形接口上下文中使用指定颜色描画交叉直线段（“×”）。
\note 指定点和大小确定边界。
\note 若不满足长和宽都大于 8 则忽略。
\since build 302
*/
YF_API void
DrawCross(const Graphics&, const Point&, const Size&, Color);


//平台无关色彩系统类型和转换。

//! \since build 464
//@{
//! \brief 按指定分量取灰度色。
yconstfn PDefH(Color, MakeGray, MonoType g)
	ImplRet(Color(g, g, g))

//! \brief 按指定颜色或分量选取彩色或灰度色。
yconstfn PDefH(Color, MakeGrayOrColor, Color c, MonoType g, bool colored)
	ImplRet(colored ? c : Color(g, g, g))
//@}


/*!
\brief 色调。
\note 角度制，单位为 ° ，值 360 对应一个圆周。
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
	\note 取值范围 [0, 360) 。
	\since build 302
	*/
	Hue h;
	/*!
	\brief 饱和度和亮度。
	\note 取值范围 [0, 1] 。
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


/*!
\brief 色调偏移。
\pre 断言：输入值在范围内。
\since build 472
*/
YF_API Color
RollColor(hsl_t, Hue);

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


//! \since build 468
//@{
//! \brief 标识样式渲染项的类型。
using StyleItem = size_t;

//! \brief 标识查找样式的键。
using Key = pair<std::type_index, StyleItem>;

//! \brief 样式处理器。
using Handler = std::function<void(PaintEventArgs&&)>;

//! \brief 样式处理器表。
using HandlerTable = unordered_map<Key, Handler, ystdex::combined_hash<Key>>;

//! \brief 视觉样式。
using VisualStyle = pair<string, HandlerTable>;


/*!
\ingroup helper_functions
\brief 向样式处理器表添加指定类型的处理器。
\note 被添加的值需能表示为 StyleItem 和 Handler 的有序对，
	使用 ADL <tt>get<0U></tt> 和 <tt>get<1U></tt> 实现访问其中的成员。
\since build 469
*/
//@{
template<typename _tIn>
void
AddHandlers(HandlerTable& table, std::type_index idx, _tIn first, _tIn last)
{
	for(; first != last; ++first)
	{
		auto&& ref(*first);

		table.emplace(Key(idx, get<0U>(ref)), get<1U>(ref));
	}
}
template<typename _type, typename _tIn>
inline void
AddHandlers(HandlerTable& table, _tIn first, _tIn last)
{
	Styles::AddHandlers(table, typeid(_type), first, last);
}
inline PDefH(void, AddHandlers, HandlerTable& table, std::type_index idx,
	std::initializer_list<pair<StyleItem, Handler>> il)
	ImplExpr(Styles::AddHandlers(table, idx, il.begin(), il.end()))
template<typename _type>
inline void
AddHandlers(HandlerTable& table,
	std::initializer_list<pair<StyleItem, Handler>> il)
{
	Styles::AddHandlers(table, typeid(_type), il);
}
//! \note 使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定范围迭代器。
//@{
template<typename _tRange>
inline void
AddHandlers(HandlerTable& table, std::type_index idx, _tRange&& c)
{
	// XXX: Consider std::make_move_iterator.
	Styles::AddHandlers(table, idx, begin(yforward(c)), end(yforward(c)));
}
template<typename _type, typename _tRange>
inline void
AddHandlers(HandlerTable& table, _tRange&& c)
{
	Styles::AddHandlers(table, typeid(_type), yforward(c));
}
//@}
//@}


/*!
\brief 带样式的绘制处理函数。
\warning 非虚析构。
*/
class YF_API Painter
{
private:
	Key key;

public:
	Painter(const Key& k)
		: key(k)
	{}
	template<typename _type1, typename _type2>
	Painter(_type1&& arg1, _type2&& arg2)
		: key(yforward(arg1), yforward(arg2))
	{}
	DefDeCopyCtor(Painter)
	DefDeMoveCtor(Painter)

	DefDeCopyAssignment(Painter)
	DefDeMoveAssignment(Painter)

	DefGetter(const ynothrow, StyleItem, Item, key.second)
	DefGetter(const ynothrow, const Key&, Key, key)
	DefGetter(const ynothrow, std::type_index, TypeIndex, key.first)

	void
	operator()(PaintEventArgs&&) const;
};


/*!
\brief 样式映射。
\warning 非虚析构。
*/
class YF_API StyleMap : private noncopyable, private map<string, HandlerTable>
{
public:
	using MapType = map<string, HandlerTable>;
	using MapType::const_iterator;
	using MapType::iterator;

private:
	const_iterator current;

public:
	StyleMap()
		: MapType({{}}), current(cbegin())
	{}
	template<typename... _tParams>
	StyleMap(_tParams&&... args)
		: MapType({{}, yforward(args)...}), current(cbegin())
	{}

	//! \since build 469
	DefGetter(const ynothrow, const_iterator, Current, current)

	template<typename... _tParams>
	void
	Add(_tParams&&... args)
	{
//! \since build 472 as workaround for G++ 4.7.1
#if __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ <= 47100
		insert(value_type(yforward(args)...));
#else
		emplace(yforward(args)...);
#endif
	}

	//! \note 若移除作为默认名称的空串则被忽略。
	void
	Remove(const string&);

	PDefH(void, Paint, const Styles::Painter& painter,
		PaintEventArgs&& e)
		ImplExpr(PaintAsStyle(painter.GetKey(), std::move(e)))

	void
	PaintAsStyle(const Key&, PaintEventArgs&&);

	void
	Switch(const string&);

	using MapType::at;

	using MapType::cbegin;

	using MapType::cend;

	using MapType::crbegin;

	using MapType::crend;

	using MapType::empty;

	using MapType::find;

	using MapType::lower_bound;

	using MapType::size;

	using MapType::upper_bound;
};


/*!
\brief 取默认样式处理器表。
\note 全局共享。
*/
YF_API HandlerTable&
FetchDefault();
//@}

} // namespace Styles;

} // namespace UI;

} // namespace YSLib;

#endif

