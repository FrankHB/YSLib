/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YUIContainer.h
\ingroup UI
\brief 样式无关的 GUI 容器。
\version r2131
\author FrankHB <frankhb1989@gmail.com>
\since build 563
\par 创建时间:
	2011-01-22 07:59:47 +0800
\par 修改时间:
	2016-04-12 01:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YUIContainer
*/


#ifndef YSL_INC_UI_YUIContainer_h_
#define YSL_INC_UI_YUIContainer_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidget
#include <ystdex/iterator.hpp>

namespace YSLib
{

namespace UI
{

/*!
\brief 取指定部件的顶层部件。
\return 若无父部件则为参数自身，否则为视图树中的父部件指针为空指针的部件的引用。
\since build 282
*/
YF_API IWidget&
FetchTopLevel(IWidget&);
/*!
\brief 取指定部件的顶层部件，当返回非第一参数时变换坐标。
\return 若无父部件则为第一参数，否则为视图树中的父部件指针为空指针的部件的引用。
\note 原始坐标相对指定部件，会被转换为相对最终节点的坐标。
\since build 282
*/
YF_API IWidget&
FetchTopLevel(IWidget&, Point&);

/*!
\brief 取指定部件向上遍历的轨迹：相对自身和所有父部件的位置。
\return 部件视图树向上遍历的指针和对应相对其指向的部件相对位置序列。
\since build 672
*/
YF_API vector<pair<observer_ptr<const IWidget>, Point>>
FetchTrace(const IWidget&);


/*!
\brief 取相对第三参数指向的部件的点相对第一参数指向的容器的偏移坐标。
\since build 672
*/
YF_API Point
LocateOffset(observer_ptr<const IWidget>, Point, observer_ptr<const IWidget>);

/*!
\brief 取相对部件 wgt 的点 pt 相对 wgt 的容器的偏移坐标。
\since build 167
*/
inline PDefH(Point, LocateContainerOffset, const IWidget& wgt, const Point& pt)
	ImplRet(pt + GetLocationOf(wgt))

/*!
\brief 取顶层部件对应的偏移。
\since build 573

计算第一参数在第二参数指定的部件对应的顶层部件位置与第三参数指定的偏移量之和。
*/
YF_API Point
LocateForTopOffset(const Point&, IWidget&, const Point& = {});

/*!
\brief 取指定部件相对轨迹的偏移坐标。
\since build 672
*/
YF_API Point
LocateForTrace(const vector<pair<observer_ptr<const IWidget>, Point>>&,
	const IWidget&);

/*!
\brief 取部件 wgt 相对部件 base 指定的部件的偏移坐标。
\since build 489
*/
inline PDefH(Point, LocateForWidget, const IWidget& base, const IWidget& wgt)
	ImplRet(LocateForTrace(FetchTrace(base), wgt))

/*!
\brief 取指定部件相对视图树中的直接节点指针的偏移坐标。
\tparam _Node 节点类型。
\tparam _fFetcher 节点访问器类型。
\since build 227
*/
template<class _tWidget, typename _fFetcher>
Point
LocateForWidgetNode(IWidget& wgt, _fFetcher fetch_ptr)
{
	if(YB_LIKELY(dynamic_cast<_tWidget*>(&wgt)))
		return Point();

	_tWidget* const pNode(fetch_ptr(wgt));

	return pNode ? LocateOffset(pNode, Point(), YSLib::make_observer(&wgt))
		: Point::Invalid;
}

/*!
\brief 取指定部件相对容器的父容器的偏移坐标。
\note 若无容器则返回 Point::Invalid 。
\since build 167
*/
YF_API Point
LocateForParentContainer(const IWidget&);


/*!
\brief 移动部件至容器下端。
\pre 间接断言：容器指针非空。
\since build 171
*/
YF_API void
MoveToBottom(IWidget&);

/*!
\brief 移动部件至容器中心。
\pre 间接断言：容器指针非空。
\since build 583
*/
YF_API void
MoveToCenter(IWidget&);

/*!
\brief 移动部件至容器左端。
\pre 断言：容器指针非空。
\since build 171
*/
YF_API void
MoveToLeft(IWidget&);

/*!
\brief 移动部件至容器右端。
\pre 间接断言：容器指针非空。
\since build 171
*/
YF_API void
MoveToRight(IWidget&);

/*!
\brief 移动部件至容器上端。
\pre 断言：容器指针非空。
\since build 171
*/
YF_API void
MoveToTop(IWidget&);


//! \since build 555
//@{
//! \brief Z 顺序类型：覆盖顺序，值越大表示越接近顶层。
using ZOrder = std::uint8_t;
//! \brief 默认 Z 顺序值。
const ZOrder DefaultZOrder(64);
//! \brief 默认窗口 Z 顺序值。
const ZOrder DefaultWindowZOrder(128);
//@}


/*!
\brief 从容器中移除部件。
\return 是否移除成功。
\note 第二个参数指定的部件作为容器检查和尝试移除第一个参数指定的部件。
\note 若移除成功同时移除焦点指针。
\since build 494
*/
YF_API bool
RemoveFrom(IWidget&, IWidget&);


/*!
\brief 线性部件容器模块。
\note 不支持 Z 顺序，但支持随机访问。
\warning 非虚析构。
\since build 494
*/
class YF_API MLinearUIContainer
{
public:
	/*!
	\brief 部件组项目类型。
	\since build 554
	*/
	using ItemType = lref<IWidget>;
	//! \brief 部件组类型。
	using WidgetVector = vector<ItemType>;
	using iterator = WidgetIterator;

protected:
	//! \brief 部件组：存储部件引用。
	WidgetVector vWidgets;

public:
	//! \since build 537
	//@{
	//! \brief 无参数构造：默认实现。
	DefDeCtor(MLinearUIContainer)
	DefDeMoveCtor(MLinearUIContainer)

	/*!
	\brief 向部件组添加部件。
	\note 部件已存在时忽略。
	*/
	void
	operator+=(IWidget&);

	/*!
	\brief 从部件组移除部件。
	\return 存在指定部件且移除成功。

	从部件组移除部件。
	*/
	bool
	operator-=(IWidget&);
	//@}

	/*!
	\brief 取指定索引的部件引用。
	\pre 断言：参数小于部件数。
	*/
	//@{
	PDefHOp(IWidget&, [], size_t idx) ynothrowv
		ImplRet(YAssert(idx < GetCount(), "Index is out of range."),
			vWidgets[idx])
	PDefHOp(IWidget&, [], size_t idx) const ynothrowv
		ImplRet(YAssert(idx < GetCount(), "Index is out of range."),
			vWidgets[idx])
	//@}

	/*!
	\brief 判断是否包含指定部件。
	\since build 495
	*/
	bool
	Contains(IWidget&) const;

	/*!
	\brief 取部件数。
	\since build 495
	*/
	DefGetter(const ynothrow, size_t, Count, vWidgets.size())

	/*!
	\brief 查找部件。
	\return 若找到则为部件的索引，否则等于部件数。
	\since build 495
	*/
	size_t
	Find(IWidget&) const;

	/*!
	\brief 绘制可视子部件。
	\since build 537
	*/
	void
	PaintVisibleChildren(PaintEventArgs&);

	/*!
	\brief 取指定索引的部件引用。
	\exception std::out_of_range 异常中立：由 vWidgets.at 抛出。
	\since build 563
	*/
	//@{
	PDefH(IWidget&, at, size_t idx)
		ImplRet(vWidgets.at(idx))
	PDefH(IWidget&, at, size_t idx) const
		ImplRet(vWidgets.at(idx))
	//@}

	iterator
	begin();

	//! \since build 537
	PDefH(void, clear, ) ynothrow
		ImplExpr(vWidgets.clear())

	iterator
	end();
};


/*!
\brief 部件容器模块。
\warning 非虚析构。
\since build 167
*/
class YF_API MUIContainer
{
public:
	/*!
	\brief 部件组项目类型。
	\since build 554
	*/
	using ItemType = lref<IWidget>;
	/*!
	\brief 部件映射表类型：映射 Z 顺序至部件。
	\since build 555
	*/
	using WidgetMap = multimap<ZOrder, ItemType>;
	using PairType = WidgetMap::value_type;
	//! \since build 460
	using iterator = WidgetIterator;

protected:
	/*!
	\brief 部件映射：存储 Z 顺序映射至部件引用。
	\since build 279
	*/
	WidgetMap mWidgets;

public:
	//! \since build 537
	//@{
	/*!
	\brief 无参数构造：默认实现。
	*/
	DefDeCtor(MUIContainer)
	DefDeMoveCtor(MUIContainer)

	/*!
	\brief 向部件组添加部件。
	\note 部件已存在时忽略。

	向部件组按默认 Z 顺序值添加部件。
	*/
	PDefHOp(void, +=, IWidget& wgt)
		ImplRet(Add(wgt))

	/*!
	\brief 从部件组移除部件。
	\return 存在指定部件且移除成功。

	从部件组移除部件。
	*/
	bool
	operator-=(IWidget&);
	//@}

	/*!
	\brief 判断是否包含指定部件。
	*/
	bool
	Contains(IWidget&);

	/*!
	\brief 取部件数。
	\since build 495
	*/
	DefGetter(const ynothrow, size_t, Count, mWidgets.size())

	/*!
	\brief 向部件组添加部件。
	\note 部件已存在时忽略。
	\since build 555

	向焦点对象组添加焦点对象，同时向部件组按指定 Z 顺序值添加部件。
	*/
	void
	Add(IWidget&, ZOrder = DefaultZOrder);

	/*!
	\brief 绘制可视子部件。
	\since build 537
	*/
	void
	PaintVisibleChildren(PaintEventArgs&);

	/*!
	\brief 查询指定部件的 Z 顺序。
	\throw std::out_of_range 不包含指定子部件。
	\since build 555
	*/
	ZOrder
	QueryZ(IWidget&) const;

	//! \since build 460
	iterator
	begin();

	//! \since build 537
	PDefH(void, clear, ) ynothrow
		ImplExpr(mWidgets.clear())

	//! \since build 460
	iterator
	end();
};


/*!
\brief 向部件添加单一子部件。
\since build 496
*/
template<class _tCon>
inline void
AddWidget(_tCon& con, IWidget& wgt)
{
	con += wgt;
}

/*!
\brief 向部件添加指定 Z 顺序的单一子部件。
\since build 555
*/
template<class _tCon>
inline void
AddWidget(_tCon& con, IWidget& wgt, ZOrder z)
{
	con.Add(wgt, z);
}

/*!
\brief 向部件容器添加子部件。
\note 不保证顺序。
\since build 303
*/
template<class _tCon, class... _tWidgets>
inline void
AddWidgets(_tCon& con, _tWidgets&... wgts)
{
	unseq_apply(std::bind(static_cast<void(_tCon::*)(IWidget&)>(
		&_tCon::operator+=), std::ref(con), std::placeholders::_1),
		std::forward<IWidget&>(wgts)...);
}

/*!
\brief 向部件容器添加指定 Z 顺序的子部件。
\note 不保证顺序。
\since build 555
*/
template<class _tCon, class... _tWidgets>
inline void
AddWidgetsZ(_tCon& con, ZOrder z, _tWidgets&... wgts)
{
	unseq_apply(std::bind(static_cast<void(_tCon::*)(IWidget&, ZOrder)>(
		&_tCon::Add), std::ref(con), std::placeholders::_1, z),
		std::forward<IWidget&>(wgts)...);
}

/*!
\brief 从部件容器移除子部件。
\note 不保证顺序。
\since build 303
*/
template<class _tCon, class... _tWidgets>
inline void
RemoveWidgets(_tCon& con, _tWidgets&... wgts)
{
	unseq_apply(std::bind(static_cast<bool(_tCon::*)(IWidget&)>(
		&_tCon::operator-=), std::ref(con), std::placeholders::_1),
		std::forward<IWidget&>(wgts)...);
}

} // namespace UI;

} // namespace YSLib;

#endif

