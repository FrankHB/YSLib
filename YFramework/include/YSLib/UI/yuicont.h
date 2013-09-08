/*
	Copyright by FrankHB 2011-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.h
\ingroup UI
\brief 样式无关的 GUI 容器。
\version r1799
\author FrankHB <frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 07:59:47 +0800
\par 修改时间:
	2013-09-03 15:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YUIContainer
*/


#ifndef YSL_INC_UI_yuicont_h_
#define YSL_INC_UI_yuicont_h_ 1

#include "ywidget.h"
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
\note 原始坐标相对于指定部件，会被转换为相对于最终节点的坐标。
\since build 282
*/
YF_API IWidget&
FetchTopLevel(IWidget&, Point&);


/*!
\brief 取相对于第三参数指向的部件的点相对于第一参数指向的容器的偏移坐标。
\since build 229
*/
YF_API Point
LocateOffset(const IWidget*, Point, const IWidget*);

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
YF_API Point
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
		return Point();

	_tWidget* const pNode(fetch_ptr(wgt));

	return pNode ? LocateOffset(pNode, Point(), &wgt) : Point::Invalid;
}

/*!
\brief 取指定部件相对于容器的父容器的偏移坐标。
\note 若无容器则返回 Point::Invalid 。
\since build 167
*/
YF_API Point
LocateForParentContainer(const IWidget&);


/*!
\brief 移动部件 wgt 至容器左端。
\pre 断言： FetchContainerPtr(wgt) 。
\since build 171
*/
YF_API void
MoveToLeft(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器右端。
\pre 断言： FetchContainerPtr(wgt) 。
\since build 171
*/
YF_API void
MoveToRight(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器上端。
\pre 断言： FetchContainerPtr(wgt) 。
\since build 171
*/
YF_API void
MoveToTop(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器下端。
\pre 断言： FetchContainerPtr(wgt) 。
\since build 171
*/
YF_API void
MoveToBottom(IWidget& wgt);


/*
\brief Z 顺序类型：覆盖顺序，值越大表示越接近顶层。
\since build 212
*/
using ZOrderType = u8;
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
class YF_API MUIContainer
{
public:
	using ItemType = IWidget*; //!< 部件组项目类型。
	using WidgetMap = multimap<ZOrderType, ItemType>; \
		//!< 部件映射表类型：映射 Z 顺序至部件。
	using PairType = WidgetMap::value_type;
	//! \since build 357
	using Iterator = WidgetIterator;

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

	//! \since build 357
	Iterator
	GetBegin();
	//! \since build 357
	Iterator
	GetEnd();

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
\since build 303
*/
template<class _tCon, class... _tWidgets>
inline void
AddWidgetsZ(_tCon& con, ZOrderType z, _tWidgets&... wgts)
{
	unseq_apply(std::bind(static_cast<void(_tCon::*)(IWidget&, ZOrderType)
		>(&_tCon::Add), std::ref(con), std::placeholders::_1, z),
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

