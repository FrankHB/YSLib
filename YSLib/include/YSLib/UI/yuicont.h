/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.h
\ingroup UI
\brief 样式无关的图形用户界面容器。
\version r2561;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 07:59:47 +0800;
\par 修改时间:
	2011-09-11 22:09 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YUIContainer;
*/


#ifndef YSL_INC_UI_YUICONT_H_
#define YSL_INC_UI_YUICONT_H_

#include "ywidget.h"
#include <ystdex/cast.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 取指针指定的部件在视图树中的节点指针。
\tparam _Node 节点类型。
*/
template<class _tNode>
_tNode* FetchWidgetNodePtr(IWidget* pWgt)
{
	_tNode* pNode(nullptr);

	while(pWgt && !(pNode = dynamic_cast<_tNode*>(pWgt)))
		pWgt = FetchContainerPtr(*pWgt);
	return pNode;
}
/*!
\brief 取指针指定的部件在视图树中的直接节点指针，同时j变换偏移坐标。
\tparam _Node 节点类型。
\note 原始坐标相对于指定的部件，会被转换为相对于最终节点的坐标。
*/
template<class _tNode>
_tNode* FetchWidgetNodePtr(IWidget* pWgt, Point& pt)
{
	_tNode* pNode(nullptr);

	while(pWgt && !(pNode = dynamic_cast<_tNode*>(pWgt)))
	{
		pt += pWgt->GetLocation();
		pWgt = FetchContainerPtr(*pWgt);
	}
	return pNode;
}

/*!
\brief 取指定部件的桌面指针。
\note 加入容器指针判断；包括部件自身。
*/
Desktop*
FetchDesktopPtr(IWidget&);


/*!
\brief 取相对于 pWgt 指向的部件的点 pt 相对于 pEnd 指向的容器的偏移坐标。
*/
Point
LocateOffset(const IWidget* pEnd, Point, const IWidget* pWgt);

/*!
\brief 取相对部件 w 的点 pt 相对于 w 的容器的偏移坐标。
*/
inline Point
LocateContainerOffset(const IWidget& w, const Point& pt)
{
	return pt + w.GetLocation();
}

/*!
\brief 取指定部件 b 相对于部件 a 的偏移坐标。
*/
Point
LocateForWidget(IWidget& a, IWidget& b);

/*!
\brief 取指定部件相对于视图树中的直接节点指针的偏移坐标。
\tparam _Node 节点类型。
\tparam _fFetcher 节点访问器类型。
*/
template<class _tWidget, typename _fFetcher>
Point
LocateForWidgetNode(IWidget& wgt, _fFetcher fetch_ptr)
{
	if(dynamic_cast<_tWidget*>(&wgt))
		return Point::Zero;

	_tWidget* const pNode(fetch_ptr(wgt));

	return pNode ? LocateOffset(pNode, Point::Zero, &wgt) : Point::FullScreen;
}

/*!
\brief 取指定部件相对于容器的父容器的偏移坐标。
\note 若无容器则返回 FullScreen 。
*/
Point
LocateForParentContainer(const IWidget&);


/*!
\brief 移动部件 wgt 至容器左端。
\pre 断言： FetchContainerPtr(wgt) 。
*/
void
MoveToLeft(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器右端。
\pre 断言： FetchContainerPtr(wgt) 。
*/
void
MoveToRight(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器上端。
\pre 断言： FetchContainerPtr(wgt) 。
*/
void
MoveToTop(IWidget& wgt);

/*!
\brief 移动部件 wgt 至容器下端。
\pre 断言： FetchContainerPtr(wgt) 。
*/
void
MoveToBottom(IWidget& wgt);


typedef u8 ZOrderType; //!< Z 顺序类型：覆盖顺序，值越大表示越接近顶层。

const ZOrderType DefaultZOrder(64); //!< 默认 Z 顺序值。
const ZOrderType DefaultWindowZOrder(128); //!< 默认窗口 Z 顺序值。


//! \brief 部件容器模块。
class MUIContainer
{
public:
	typedef IWidget* ItemType; //!< 部件组项目类型。
	typedef multimap<ZOrderType, ItemType> WidgetMap; \
		//!< 部件组类型：映射 Z 顺序至部件。
	typedef WidgetMap::value_type PairType;

protected:
	WidgetMap sWidgets; //!< 部件对象组：存储非空部件指针。

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
	PDefHOperator1(void, +=, IWidget& wgt)
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
	\brief 向部件组添加控件。

	向焦点对象组添加焦点对象，同时向部件组按指定 Z 顺序值添加部件。
	\note 控件已存在时忽略。
	*/
	virtual void
	Add(IWidget&, ZOrderType = DefaultZOrder);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

