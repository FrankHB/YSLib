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
\brief 样式无关的图形用户界面容器。
\version 0.2244;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 07:59:47 +0800;
\par 修改时间:
	2011-04-20 11:02 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YUIContainer;
*/


#ifndef YSL_INC_SHELL_YUICONT_H_
#define YSL_INC_SHELL_YUICONT_H_

#include "ywidget.h"
#include "yfocus.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

// GUI 断言。

#undef YWidgetAssert

#ifdef YCL_USE_YASSERT

/*!
\brief 断言：判断所给表达式，如果为假给出指定错误信息。
*/
void
yassert(bool, const char*, int, const char*, const char*, const char*);

#	define YWidgetAssert(ptr, comp, func) \
	Components::yassert((ptr) && FetchDirectWindowPtr( \
		ystdex::general_cast<IWidget&>(*(ptr))), \
		"The direct window handle is null.", __LINE__, __FILE__, #comp, #func)

#else

#	define YWidgetAssert(ptr, comp, func) \
	assert((ptr) && FetchDirectWindowPtr( \
		ystdex::general_cast<IWidget&>(*(ptr))))

#endif

YSL_BEGIN_NAMESPACE(Widgets)

//! \brief 固定部件容器接口。
DeclBasedInterface1(IUIBox, virtual IWidget)
	DeclIEntry(IControl* GetFocusingPtr()) //!< 取焦点对象指针。
	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&)) \
		//!< 取指定的点（屏幕坐标）所在的可见部件的指针。
	DeclIEntry(IControl* GetTopControlPtr(const Point&)) \
		//!< 取指定的点（屏幕坐标）所在的可见控件的指针。

	//! \brief 清除焦点指针。
	DeclIEntry(void ClearFocusingPtr())

	//! \brief 响应焦点请求。
	DeclIEntry(bool ResponseFocusRequest(AFocusRequester&))

	//! \brief 响应焦点释放。
	DeclIEntry(bool ResponseFocusRelease(AFocusRequester&))
EndDecl


//部件容器接口。
DeclBasedInterface1(IUIContainer, IUIBox)
	DeclIEntry(void operator+=(IWidget*)) //!< 向部件组添加部件指针。
	DeclIEntry(void operator+=(IControl*)) //!< 向焦点对象组添加控件指针。
	DeclIEntry(void operator+=(GMFocusResponser<IControl>*)) \
		//!< 向焦点对象组添加子焦点对象容器指针。
	DeclIEntry(bool operator-=(IWidget*)) //!< 从部件组移除部件指针。
	DeclIEntry(bool operator-=(IControl*)) //!< 从焦点对象组移除控件指针。
	DeclIEntry(bool operator-=(GMFocusResponser<IControl>*)) \
		//!< 从焦点对象组移除子焦点对象容器指针。
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
IWindow*
FetchWindowPtr(const IWidget&);

/*!
\brief 取指定部件的直接容器指针。
*/
IUIBox*
FetchDirectContainerPtr(IWidget&);

/*!
\brief 取指定部件的直接窗口句柄。
\note 加入容器指针判断。
*/
IWindow*
FetchDirectWindowPtr(IWidget&);

/*!
\brief 取指定部件的直接桌面句柄。
\note 加入容器指针判断。
*/
YDesktop*
FetchDirectDesktopPtr(IWidget&);

/*!
\brief 取指定部件的直接窗口的图形接口上下文。
\throw GeneralEvent 无法找到有效的直接窗口。
*/
const Graphics&
FetchContext(IWidget&);


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
\brief 取指定的点 pt （相对部件 w 的坐标）相对于 w 的容器的偏移坐标。
*/
inline Point
LocateContainerOffset(const IWidget& w, const Point& pt)
{
	return pt + w.GetLocation();
}

/*!
\brief 取指定的点 pt （相对部件 w 的坐标）相对于 w 的窗口的偏移坐标。
*/
inline Point
LocateWindowOffset(const IWidget& w, const Point& pt)
{
	return LocateOffset(&w, pt, FetchWindowPtr(w));
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
\brief 以纯色填充部件所在窗口的对应显示缓冲区。
*/
void
Fill(IWidget&, Color);


//! \brief 部件容器模块。
class MUIContainer : protected GMFocusResponser<IControl>
{
public:
	typedef IWidget* ItemType; //!< 部件组项目类型：记录部件指针。
	typedef list<ItemType> WGTs; //!< 部件组类型。
	typedef set<GMFocusResponser<IControl>*> FOCs; \
		//!< 子焦点对象容器组类型。

protected:
	WGTs WidgetsList; //!< 部件对象组模块。
	FOCs sFOCSet; //!< 子焦点对象容器组。

public:
	MUIContainer();
	virtual DefEmptyDtor(MUIContainer)

protected:
	/*!
	\brief 向部件组添加部件。
	*/
	void
	operator+=(IWidget*);
	/*!
	\brief 向部件组添加控件。
	
	向焦点对象组添加焦点对象指针，同时向部件组添加部件指针。
	*/
	void
	operator+=(IControl*);
	/*!
	\brief 向子焦点对象容器组添加子焦点对象容器指针。
	*/
	void
	operator+=(GMFocusResponser<IControl>*);

	/*!
	\brief 从部件组移除部件。
	*/
	bool
	operator-=(IWidget*);
	/*!
	\brief 从部件组移除控件。

	从部件组移除部件指针，同时从焦点对象组移除焦点对象指针。
	*/
	bool
	operator-=(IControl*);
	/*!
	\brief 从子焦点对象容器组移除子焦点对象容器。
	*/
	bool
	operator-=(GMFocusResponser<IControl>*);

public:
	/*!
	\brief 取焦点控件指针。
	*/
	IControl*
	GetFocusingPtr() const;
	/*!
	\brief 取顶端部件指针。
	*/
	IWidget*
	GetTopWidgetPtr(const Point&);
	/*!
	\brief 取顶端控件指针。
	*/
	IControl*
	GetTopControlPtr(const Point&);

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

protected:
	/*!
	\brief 检查部件指针是否满足添加条件。
	\return 若部件指针为空或已在部件组中则返回 false ，否则返回 true 。
	*/
	bool
	CheckWidget(IWidget*);

	/*!
	\brief 从部件组移除部件。
	*/
	bool
	RemoveWidget(IWidget*);
};


//部件容器。
class YUIContainer : public GMCounter<YUIContainer>, public YComponent,
	public Widget, protected MUIContainer,
	implements IUIContainer
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	YUIContainer(const Rect& = Rect::Empty);

	ImplI1(IUIContainer) PDefHOperator1(void, +=, IWidget* p)
		ImplBodyBase1(MUIContainer, operator+=, p)
	ImplI1(IUIContainer) PDefHOperator1(bool, -=, IWidget* p)
		ImplBodyBase1(MUIContainer, operator-=, p)
	ImplI1(IUIContainer) PDefHOperator1(void, +=, IControl* p)
		ImplBodyBase1(MUIContainer, operator+=, p)
	ImplI1(IUIContainer) PDefHOperator1(bool, -=, IControl* p)
		ImplBodyBase1(MUIContainer, operator-=, p)
	ImplI1(IUIContainer) PDefHOperator1(void, +=, GMFocusResponser<IControl>* p)
		ImplBodyBase1(MUIContainer, operator+=, p)
	ImplI1(IUIContainer) PDefHOperator1(bool, -=, GMFocusResponser<IControl>* p)
		ImplBodyBase1(MUIContainer, operator-=, p)

	ImplI1(IUIContainer) DefPredicateBase(Visible, Visual)
	ImplI1(IUIContainer) DefPredicateBase(Transparent, Visual)

	ImplI1(IUIContainer) DefGetterBase(const Point&, Location, Visual)
	ImplI1(IUIContainer) DefGetterBase(const Size&, Size, Visual)
	ImplI1(IUIContainer) DefGetterBase(IUIBox*&, ContainerPtr, Widget)
	ImplI1(IUIContainer) DefMutableGetterBase(IControl*, FocusingPtr,
		GMFocusResponser<IControl>)
	ImplI1(IUIContainer) PDefH1(IWidget*, GetTopWidgetPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopWidgetPtr, pt)
	ImplI1(IUIContainer) PDefH1(IControl*, GetTopControlPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopControlPtr, pt)

	ImplI1(IUIContainer) DefSetterBase(bool, Visible, Visual)
	ImplI1(IUIContainer) DefSetterBase(bool, Transparent, Visual)
	ImplI1(IUIContainer) DefSetterBase(const Point&, Location, Visual)
	ImplI1(IUIContainer) DefSetterBase(const Size&, Size, Visual)

	ImplI1(IUIContainer) PDefH0(void, ClearFocusingPtr)
		ImplBodyBase0(MUIContainer, ClearFocusingPtr)

	ImplI1(IUIContainer) PDefH1(bool, ResponseFocusRequest, AFocusRequester& w)
		ImplBodyBase1(MUIContainer, ResponseFocusRequest, w)

	ImplI1(IUIContainer) PDefH1(bool, ResponseFocusRelease, AFocusRequester& w)
		ImplBodyBase1(MUIContainer, ResponseFocusRelease, w)

	ImplI1(IUIContainer) PDefH0(void, Paint)
		ImplBodyBase0(Widget, Paint)

	ImplI1(IUIContainer) PDefH0(void, Refresh)
		ImplBodyBase0(Widget, Refresh)
};

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

