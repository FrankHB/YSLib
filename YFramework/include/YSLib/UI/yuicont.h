/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.h
\ingroup UI
\brief 样式无关的图形用户界面容器。
\version r1748
\author FrankHB<frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 07:59:47 +0800
\par 修改时间:
	2012-09-04 12:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YUIContainer
*/


#ifndef YSL_INC_UI_YUICONT_H_
#define YSL_INC_UI_YUICONT_H_ 1

#include "ywidget.h"
#include <ystdex/iterator.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 取指定部件的顶层部件。
\return 若无父部件则为参数自身，否则为视图树中的父部件指针为空指针的部件的引用。
\since build 282
*/
IWidget&
FetchTopLevel(IWidget&);
/*!
\brief 取指定部件的顶层部件，当返回非第一参数时变换坐标。
\return 若无父部件则为第一参数，否则为视图树中的父部件指针为空指针的部件的引用。
\note 原始坐标相对于指定部件，会被转换为相对于最终节点的坐标。
\since build 282
*/
IWidget&
FetchTopLevel(IWidget&, Point&);


/*!
\brief 取相对于 pWgt 指向的部件的点 pt 相对于 pEnd 指向的容器的偏移坐标。
\since build 229
*/
Point
LocateOffset(const IWidget* pEnd, Point, const IWidget* pWgt);

/*!
\brief 取相对部件 wgt 的点 pt 相对于 wgt 的容器的偏移坐标。
\since build 167
*/
inline Point
LocateContainerOffset(const IWidget& wgt, const Point& pt)
{
	return pt + GetLocationOf(wgt);
}

/*!
\brief 取指定部件 b 相对于部件 a 的偏移坐标。
\since build 169
*/
Point
LocateForWidget(IWidget& a, IWidget& b);

/*!
\brief 取指定部件相对于视图树中的直接节点指针的偏移坐标。
\tparam _Node 节点类型。
\tparam _fFetcher 节点访问器类型。
\since build 227
*/
template<class _tWidget, typename _fFetcher>
Point
LocateForWidgetNode(IWidget& wgt, _fFetcher fetch_ptr)
{
	if(YB_LIKELY(dynamic_cast<_tWidget*>(&wgt)))
		return Point::Zero;

	_tWidget* const pNode(fetch_ptr(wgt));

	return pNode ? LocateOffset(pNode, Point::Zero, &wgt) : Point::Invalid;
}

/*!
\brief 取指定部件相对于容器的父容器的偏移坐标。
\note 若无容器则返回 Point::Invalid 。
\since build 167
*/
Point
LocateForParentContainer(const IWidget&);


/*!
\brief 移动部件 wgt 至容器左端。
\pre 断言： FetchContainerPtr(wgt) 。
\since build 171
*/
void
MoveToLeft(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器右端。
\pre 断言： FetchContainerPtr(wgt) 。
\since build 171
*/
void
MoveToRight(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器上端。
\pre 断言： FetchContainerPtr(wgt) 。
\since build 171
*/
void
MoveToTop(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器下端。
\pre 断言： FetchContainerPtr(wgt) 。
\since build 171
*/
void
MoveToBottom(IWidget& wgt);


/*
\brief Z 顺序类型：覆盖顺序，值越大表示越接近顶层。
\since build 212
*/
typedef u8 ZOrderType;
/*!
\brief 默认 Z 顺序值。
\since build 212
*/
const ZOrderType DefaultZOrder(64);
/*!
\brief 默认窗口 Z 顺序值。
\since build 212
*/
const ZOrderType DefaultWindowZOrder(128);


/*!
\brief 部件容器模块。
\warning 非虚析构。
\since build 167
*/
class MUIContainer
{
public:
	typedef IWidget* ItemType; //!< 部件组项目类型。
	typedef multimap<ZOrderType, ItemType> WidgetMap; \
		//!< 部件映射表类型：映射 Z 顺序至部件。
	typedef WidgetMap::value_type PairType;

protected:
	/*
	\brief 部件映射：存储 Z 顺序映射至非空部件指针。
	\since build 279
	*/
	WidgetMap mWidgets;

	/*!
	\brief 无参数构造：默认实现。
	*/
	DefDeCtor(MUIContainer)
	inline DefDeMoveCtor(MUIContainer)

	/*!
	\brief 向部件组添加部件。

	向焦点对象组添加焦点对象，同时向部件组按默认 Z 顺序值添加部件。
	\note 部件已存在时忽略。
	*/
	PDefHOp(void, +=, IWidget& wgt)
		ImplRet(Add(wgt))

	/*!
	\brief 从部件组移除部件。

	从部件组移除部件，同时从焦点对象组移除焦点对象。
	\return 存在指定部件且移除成功。
	*/
	bool
	operator-=(IWidget&);

public:
	/*!
	\brief 判断是否包含指定部件。
	*/
	bool
	Contains(IWidget&);

	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	*/
	IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

	/*!
	\brief 向部件组添加部件。

	向焦点对象组添加焦点对象，同时向部件组按指定 Z 顺序值添加部件。
	\note 部件已存在时忽略。
	*/
	void
	Add(IWidget&, ZOrderType = DefaultZOrder);

protected:
	/*!
	\brief 绘制可视子部件。
	\since build 295
	*/
	void
	PaintVisibleChildren(PaintEventArgs&);
};


/*!
\brief 向部件容器添加子部件。
\note 不保证顺序。
\since build 303
*/
template<class _tContainer, class... _tWidgets>
inline void
AddWidgets(_tContainer& con, _tWidgets&... wgts)
{
	unseq_apply(std::bind(static_cast<void(_tContainer::*)(IWidget&)>(
		&_tContainer::operator+=), std::ref(con), std::placeholders::_1),
		std::forward<IWidget&>(wgts)...);
}

/*!
\brief 向部件容器添加指定 Z 顺序的子部件。
\note 不保证顺序。
\since build 303
*/
template<class _tContainer, class... _tWidgets>
inline void
AddWidgetsZ(_tContainer& con, ZOrderType z, _tWidgets&... wgts)
{
	unseq_apply(std::bind(static_cast<void(_tContainer::*)(IWidget&, ZOrderType)
		>(&_tContainer::Add), std::ref(con), std::placeholders::_1, z),
		std::forward<IWidget&>(wgts)...);
}

/*!
\brief 从部件容器移除子部件。
\note 不保证顺序。
\since build 303
*/
template<class _tContainer, class... _tWidgets>
inline void
RemoveWidgets(_tContainer& con, _tWidgets&... wgts)
{
	unseq_apply(std::bind(static_cast<bool(_tContainer::*)(IWidget&)>(
		&_tContainer::operator-=), std::ref(con), std::placeholders::_1),
		std::forward<IWidget&>(wgts)...);
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

