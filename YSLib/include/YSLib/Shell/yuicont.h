/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.h
\ingroup Shell
\brief 平台无关的图形用户界面部件实现。
\version 0.2023;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 07:59:47 + 08:00;
\par 修改时间:
	2011-01-23 07:32 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YUIContainer;
*/


#ifndef INCLUDED_YUICONT_H_
#define INCLUDED_YUICONT_H_

#include "ywidget.h"
#include "yfocus.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

//! \brief 固定部件容器接口。
DeclBasedInterface(IUIBox, virtual IWidget)
	DeclIEntry(IVisualControl* GetFocusingPtr()) //!< 取焦点对象指针。
	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&)) \
		//!< 取指定的点（屏幕坐标）所处的部件的指针。
	DeclIEntry(IVisualControl* GetTopVisualControlPtr(const Point&)) \
		//!< 取指定的点（屏幕坐标）所处的可视控件的指针。

	//! \brief 清除焦点指针。
	DeclIEntry(void ClearFocusingPtr())

	//! \brief 响应焦点请求。
	DeclIEntry(bool ResponseFocusRequest(AFocusRequester&))

	//! \brief 响应焦点释放。
	DeclIEntry(bool ResponseFocusRelease(AFocusRequester&))
EndDecl


//部件容器接口。
DeclBasedInterface(IUIContainer, virtual IUIBox)
	DeclIEntry(void operator+=(IWidget&)) //!< 向部件组添加部件。
	DeclIEntry(bool operator-=(IWidget&)) //!< 从部件组移除部件。
	DeclIEntry(void operator+=(IVisualControl&)) //!< 向焦点对象组添加可视控件。
	DeclIEntry(bool operator-=(IVisualControl&)) //!< 从焦点对象组移除可视控件。
	DeclIEntry(void operator+=(GMFocusResponser<IVisualControl>&)) \
		//!< 向焦点对象组添加子焦点对象容器。
	DeclIEntry(bool operator-=(GMFocusResponser<IVisualControl>&)) \
		//!< 从焦点对象组移除子焦点对象容器。
EndDecl


/*!
\brief 取指针指定的部件在视图树中的直接节点指针。
*/
template<class _tNode>
_tNode* FetchWidgetDirectNodePtr(IWidget* pWgt)
{
	_tNode* pNode(NULL);

	while(pWgt && !(pNode = dynamic_cast<_tNode*>(pWgt)))
		pWgt = pWgt->GetContainerPtr();
	return pNode;
}

/*!
\brief 取指定部件的窗口指针。
*/
inline IWindow*
FetchWindowPtr(const IWidget& w)
{
	return FetchWidgetDirectNodePtr<IWindow>(w.GetContainerPtr());
}

/*!
\brief 取指定部件的直接容器指针。
*/
IUIBox*
FetchDirectContainerPtr(IWidget&);

/*!
\brief 取指定部件的直接窗口句柄。
\note 加入容器指针判断。
*/
inline IWindow*
FetchDirectWindowPtr(IWidget& w)
{
	return FetchWidgetDirectNodePtr<IWindow>(FetchDirectContainerPtr(w));
}

/*!
\brief 取指定部件的直接桌面句柄。
\note 加入容器指针判断。
*/
inline YDesktop*
FetchDirectDesktopPtr(IWidget& w)
{
	return FetchWidgetDirectNodePtr<YDesktop>(FetchDirectContainerPtr(w));
}


/*!
\brief 取指定的点（相对此部件的坐标）相对于指定指针指向的父窗口的偏移坐标。
*/
Point
LocateOffset(const IWidget*, Point, const IWindow*);
/*!
\brief 取指定的点（相对此部件的坐标）相对于指定指针指向的父窗口的偏移坐标。
*/
template<class _tWidget>
Point
LocateOffset(const _tWidget* pCon, const Point& p, const IWindow* pWnd)
{
	const IWidget* pConI(dynamic_cast<const IWidget*>(pCon));

	return pConI ? LocateOffset(pConI, p, pWnd) : p;
}

/*!
\brief 取指定的点 p （相对部件 w 的坐标）相对于 w 的容器的偏移坐标。
*/
inline Point
LocateContainerOffset(const IWidget& w, const Point& p)
{
	return p + w.GetLocation();
}

/*!
\brief 取指定的点 p （相对部件 w 的坐标）相对于 w 的窗口的偏移坐标。
*/
inline Point
LocateWindowOffset(const IWidget& w, const Point& p)
{
	return LocateOffset(&w, p, GetPointer(FetchWindowPtr(w)));
}

/*!
\brief 取指定部件 a 相对于部件 b 的偏移坐标。
*/
Point
LocateForWidget(IWidget& a, IWidget& b);

/*!
\brief 取指定部件相对于直接窗口的偏移坐标。
\note 若自身是窗口则返回 Zero ；若无容器或窗口则返回 FullScreen 。
*/
Point
LocateForWindow(IWidget&);

/*!
\brief 取指定部件相对于桌面的偏移坐标。
\note 若自身是桌面则返回 Zero ；若无窗口或窗口不在桌面上则返回 FullScreen 。
*/
Point
LocateForDesktop(IWidget&);

/*!
\brief 取指定部件相对于容器的父容器的偏移坐标。
\note 若无容器则返回 FullScreen 。
*/
Point
LocateForParentContainer(const IWidget&);

/*!
\brief 取指定部件相对于窗口的父窗口的偏移坐标。
\note 若无容器则返回 FullScreen 。
*/
Point
LocateForParentWindow(const IWidget&);


/*!
\brief 移动部件至容器左端。
\pre 断言：w.GetContainerPtr()。
*/
void
MoveToLeft(IWidget&);

/*!
\brief 移动部件至容器右端。
\pre 断言：w.GetContainerPtr()。
*/
void
MoveToRight(IWidget&);

/*!
\brief 移动部件至容器上端。
\pre 断言：w.GetContainerPtr()。
*/
void
MoveToTop(IWidget&);

/*!
\brief 移动部件至容器下端。
\pre 断言：w.GetContainerPtr()。
*/
void
MoveToBottom(IWidget&);


/*!
\brief 取指定部件的直接窗口的图形接口上下文。
\exception std::runtime_error 无法找到有效的直接窗口。
*/
const Graphics&
FetchContext(IWidget&);

/*!
\brief 以纯色填充部件所在窗口的对应显示缓冲区。
*/
void
Fill(IWidget&, Color);


//! \brief 部件容器模块。
class MUIContainer : protected GMFocusResponser<IVisualControl>
{
public:
	typedef GContainer<IWidget> WidgetSet;
	typedef WidgetSet::ContainerType WGTs; //!< 部件组类型。
	typedef GContainer<GMFocusResponser<IVisualControl> > FOCs; \
		//!< 子焦点对象容器组类型。

protected:
	WidgetSet sWgtSet; //!< 部件对象组模块。
	FOCs sFOCSet; //!< 子焦点对象容器组。

public:
	MUIContainer();
	virtual DefEmptyDtor(MUIContainer)

protected:
	virtual PDefHOperator(void, +=,
		IVisualControl& r) //!< 向焦点对象组添加焦点对象。
		ImplBodyBaseVoid(GMFocusResponser<IVisualControl>, operator+=, r)
	virtual PDefHOperator(bool, -=,
		IVisualControl& r) //!< 从焦点对象组移除焦点对象。
		ImplBodyBase(GMFocusResponser<IVisualControl>, operator-=, r)
	PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c) \
		//!< 向子焦点对象容器组添加子焦点对象容器。
		ImplBodyMemberVoid(sFOCSet, insert, &c)
	PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c) \
		//!< 从子焦点对象容器组移除子焦点对象容器。
		ImplBodyMember(sFOCSet, erase, &c)

public:
	/*!
	\brief 取焦点控件指针。
	*/
	IVisualControl*
	GetFocusingPtr() const;
	/*!
	\brief 取顶端部件指针。
	*/
	IWidget*
	GetTopWidgetPtr(const Point&);
	/*!
	\brief 取顶端可视部件指针。
	*/
	IVisualControl*
	GetTopVisualControlPtr(const Point&);

	/*!
	\brief 响应焦点请求。
	*/
	bool
	ResponseFocusRequest(AFocusRequester&);

	/*!
	\brief 响应焦点释放。
	*/
	bool
	ResponseFocusRelease(AFocusRequester&);
};


//部件容器。
class YUIContainer : public GMCounter<YUIContainer>, public YComponent,
	public Widget, protected MUIContainer,
	implements IUIContainer
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	YUIContainer(const Rect& = Rect::Empty, IUIBox* = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~YUIContainer() ythrow();

	ImplI(IUIContainer) PDefHOperator(void, +=, IWidget& w)
		ImplExpr(sWgtSet += w)
	ImplI(IUIContainer) PDefHOperator(bool, -=, IWidget& w)
		ImplRet(sWgtSet -= w)
	ImplI(IUIContainer) PDefHOperator(void, +=, IVisualControl& c)
		ImplBodyBaseVoid(MUIContainer, operator+=, c)
	ImplI(IUIContainer) PDefHOperator(bool, -=, IVisualControl& c)
		ImplBodyBase(MUIContainer, operator-=, c)
	ImplI(IUIContainer) PDefHOperator(void, +=,
		GMFocusResponser<IVisualControl>& c)
		ImplBodyBaseVoid(MUIContainer, operator+=, c)
	ImplI(IUIContainer) PDefHOperator(bool, -=,
		GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MUIContainer, operator-=, c)

	ImplI(IUIContainer) DefPredicateBase(Visible, Visual)
	ImplI(IUIContainer) DefPredicateBase(Transparent, Visual)
	ImplI(IUIContainer) DefPredicateBase(BgRedrawed, Visual)

	ImplI(IUIContainer) DefGetterBase(const Point&, Location, Visual)
	ImplI(IUIContainer) DefGetterBase(const Size&, Size, Visual)
	ImplI(IUIContainer) DefGetterBase(IUIBox*, ContainerPtr,
		Widget)
	ImplI(IUIContainer) DefMutableGetterBase(IVisualControl*, FocusingPtr,
		GMFocusResponser<IVisualControl>)
	ImplI(IUIContainer) PDefH(IWidget*, GetTopWidgetPtr, const Point& p)
		ImplBodyBase(MUIContainer, GetTopWidgetPtr, p)
	ImplI(IUIContainer) PDefH(IVisualControl*, GetTopVisualControlPtr,
		const Point& p)
		ImplBodyBase(MUIContainer, GetTopVisualControlPtr, p)

	ImplI(IUIContainer) DefSetterBase(bool, Visible, Visual)
	ImplI(IUIContainer) DefSetterBase(bool, Transparent, Visual)
	ImplI(IUIContainer) DefSetterBase(bool, BgRedrawed, Visual)
	ImplI(IUIContainer) DefSetterBase(const Point&, Location, Visual)
	ImplI(IUIContainer) DefSetterBase(const Size&, Size, Visual)

	ImplI(IUIContainer) PDefH(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MUIContainer, ClearFocusingPtr)

	ImplI(IUIContainer) PDefH(bool, ResponseFocusRequest, AFocusRequester& w)
		ImplBodyBase(MUIContainer, ResponseFocusRequest, w)

	ImplI(IUIContainer) PDefH(bool, ResponseFocusRelease, AFocusRequester& w)
		ImplBodyBase(MUIContainer, ResponseFocusRelease, w)

	ImplI(IUIContainer) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(Widget, DrawBackground)

	ImplI(IUIContainer) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(Widget, DrawForeground)

	ImplI(IUIContainer) PDefH(void, Refresh)
		ImplBodyBaseVoid(Widget, Refresh)

	/*!
	\brief 请求提升至容器顶端。
	\note 空实现。
	*/
	ImplI(IUIContainer) void
	RequestToTop()
	{}
};

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

