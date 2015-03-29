/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YStyle.h
\ingroup UI
\brief 图形用户界面样式。
\version r838
\author FrankHB <frankhb1989@gmail.com>
\since build 561
\par 创建时间:
	2010-06-08 13:21:10 +0800
\par 修改时间:
	2015-03-28 23:01 +0800
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
#include <ystdex/hash.hpp> // for ystdex::combined_hash;

namespace YSLib
{

namespace Drawing
{

//! \pre 间接断言：图形上下文接口有效。
//@{
/*!
\brief 绘制部件边框用空心标准矩形。
\note 右下角顶点坐标
	(pc.Location.X + s.Width - 1, pc.Location.Y + s.Height - 1) 。
\since build 453
*/
YF_API void
DrawRectRoundCorner(const PaintContext& pc, const Size& s, Color);


/*!
\brief 在指定上下文的矩形中描画底边和坐标轴共线的等腰直角三角形箭头。
\pre 断言：指向值有效。
\note 位置指定顶点；大小以高度量； RDeg0 指向右侧。
\since build 453
*/
YF_API void
RectDrawArrow(const PaintContext&, SDst, Rotation = RDeg0,
	Color = ColorSpace::Black);

/*!
\pre 断言：指向值有效。
\since build 522
*/
//@{
/*!
\brief 在指定上下文的矩形中描画底边和坐标轴共线的等腰直角三角形箭头轮廓。
\note 位置指定顶点；大小以高度量； RDeg0 指向右侧。
*/
YF_API void
RectDrawArrowOutline(const PaintContext&, SDst, Rotation = RDeg0,
	Color = ColorSpace::Black);

/*!
\brief 在指定上下文的矩形中描画腰和坐标轴共线的等腰直角三角形箭头。
\note 位置指定顶点；大小以斜边度量； RDeg0 指向右下角。
*/
YF_API void
RectDrawCornerArrow(const PaintContext&, SDst, Rotation = RDeg0,
	Color = ColorSpace::Black);

/*!
\brief 在指定上下文的矩形中描画腰和坐标轴共线的等腰直角三角形箭头轮廓。
\note 位置指定顶点；大小以斜边度量； RDeg0 指向右下角。
*/
YF_API void
RectDrawCornerArrowOutline(const PaintContext&, SDst, Rotation = RDeg0,
	Color = ColorSpace::Black);

/*!
\brief 在指定图形接口上下文上描画底边和坐标轴共线的等腰直角三角形箭头。
\note 第二参数指定边界；第三参数指定矩形外框，图形位于正中。
\note 图形大小以第四参数指定的高度量； RDeg0 指向右侧。
\note 最后的参数决定是否仅描画轮廓。
\since build 530
*/
YF_API void
DrawArrow(const Graphics&, const Rect&, const Rect&, SDst = 4, Rotation = RDeg0,
	Color = ColorSpace::Black, bool = {});

/*!
\brief 在指定图形接口上下文上描画腰和坐标轴共线的等腰直角三角形箭头。
\note 第二参数指定边界；第三参数指定图形左上角。
\note 图形大小以第四参数指定的斜边度量； RDeg0 指向右下角。
\note 最后的参数决定是否仅描画轮廓。
\since build 530
*/
YF_API void
DrawCornerArrow(const Graphics&, const Rect&, const Point&, SDst = 4,
	Rotation = RDeg0, Color = ColorSpace::Black, bool = {});
//@}

/*!
\brief 在指定图形接口上下文中使用指定颜色描画交叉直线段（“×”）。
\note 第二个参数指定边界；绘制的图形顶点为第三个参数指定的矩形的顶点或边的中点。
\note 若不满足长和宽都大于 8 则忽略。
\since build 480
*/
YF_API void
DrawCross(const Graphics&, const Rect&, const Rect&, Color);

/*
\brief 在指定图形接口上下文中使用指定颜色描画对勾（“√”）。
\note 第二个参数指定边界；绘制的图形顶点为第三个参数指定的矩形的顶点或边的中点。
\note 若指定的矩形不满足长和宽都大于 8 则忽略。
\since build 480
*/
//@{
//! \note 使用指定颜色以及底色。
YF_API void
DrawTick(const Graphics&, const Rect&, const Rect&, Color, Color);
//! \note 使用单一颜色。
inline PDefH(void, DrawTick, const Graphics& g, const Rect& bounds,
	const Rect& r, Color c)
	ImplExpr(DrawTick(g, bounds, r, c, c))
//@}
//@}


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
using Hue = ystdex::fixed_point<std::uint16_t, 9>;


/*!
\since build 561
\warning 非虚析构。
*/
//@{
/*!
\brief 色调和饱和度对。
\note 色调取值范围 [0, 360) ，饱和度取值范围 [0, 1] 。
*/
class YF_API HS : private pair<Hue, float>
{
public:
	HS(Hue h = 0, float s = 0) ynothrow
		: pair(h, s)
	{}
	//! \since build 581
	HS(float h, float s = 0) ynothrow
		: HS(Hue(h), s)
	{}

	DefGetter(const ynothrow, Hue, H, first)
	DefGetter(const ynothrow, float, S, second)

	DefGetter(ynothrow, Hue&, HRef, first)
	DefGetter(ynothrow, float&, SRef, second)

	/*
	\brief 色调偏移。
	\pre 断言：输入值在范围内。
	*/
	void
	Roll(Hue) ynothrow;
};


//! \brief HSL 颜色。
class YF_API HSL : private HS
{
private:
	/*!
	\brief 亮度。
	\note 取值范围 [0, 1] 。
	*/
	float lightness;

public:
	HSL(Hue h = 0, float s = 0, float l = 0) ynothrow
		: HS{h, s}, lightness(l)
	{}
	//! \since build 581
	HSL(float h, float s = 0, float l = 0) ynothrow
		: HS{h, s}, lightness(l)
	{}
	//! \brief 构造：使用 RGB 颜色。
	HSL(Color) ynothrow;
	DefDeCopyCtor(HSL)

	/*!
	\brief 转换为 RGB 颜色。
	\pre 断言： <tt>std::isfinite(GetS())</tt> 。
	\note 结果的 Alpha 使用默认初始化值。
	*/
	explicit
	operator Color() const ynothrow;

	using HS::GetH;
	using HS::GetHRef;
	using HS::GetS;
	using HS::GetSRef;
	DefGetter(const ynothrow, float, L, lightness)
	DefGetter(ynothrow, float&, LRef, lightness)

	/*
	\brief 色调偏移。
	\pre 间接断言：输入值在范围内。
	\return 自身引用。
	*/
	PDefH(HSL&, Roll, Hue h) ynothrow
		ImplRet(HS::Roll(h), *this)
};


//! \brief HSV 颜色。
class YF_API HSV : private HS
{
private:
	/*!
	\brief 颜色值。
	\note 取值范围 [0, 1] 。
	*/
	float value;

public:
	HSV(Hue h = 0, float s = 0, float v = 0) ynothrow
		: HS{h, s}, value(v)
	{}
	//! \since build 581
	HSV(float h, float s = 0, float v = 0) ynothrow
		: HS{h, s}, value(v)
	{}
	//! \brief 构造：使用 RGB 颜色。
	HSV(Color) ynothrow;
	DefDeCopyCtor(HSV)

	/*!
	\brief 转换为 RGB 颜色。
	\note 结果的 Alpha 使用默认初始化值。
	*/
	explicit
	operator Color() const ynothrow;

	using HS::GetH;
	using HS::GetHRef;
	using HS::GetS;
	using HS::GetSRef;
	DefGetter(const ynothrow, float, V, value)
	DefGetter(ynothrow, float&, VRef, value)

	/*
	\brief 色调偏移。
	\pre 间接断言：输入值在范围内。
	\return 自身引用。
	*/
	PDefH(HSV&, Roll, Hue h) ynothrow
		ImplRet(HS::Roll(h), *this)
};

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
\warning 非虚析构。
\since build 204
*/
class YF_API Palette
{
public:
	using ItemType = Drawing::Color; //项目类型。
	using ColorListType = vector<ItemType>; //颜色组类型。

private:
	ColorListType colors; //颜色组。

public:
	//! \brief 无参数构造：初始化包含默认界面颜色配置的调色板。
	Palette();

	//! \brief 取颜色引用。
	PDefHOp(Drawing::Color&, [], size_t s) ynothrowv
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
	using std::begin;
	using std::end;

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
		emplace(yforward(args)...);
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

