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
\version 0.2367;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 07:59:47 +0800;
\par 修改时间:
	2011-06-16 22:55 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::UIContainer;
*/


#ifndef YSL_INC_UI_YUICONT_H_
#define YSL_INC_UI_YUICONT_H_

#include "ywidget.h"
#include "yfocus.h"
#include <ystdex/cast.hpp>

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
		"The direct window pointer is null.", __LINE__, __FILE__, #comp, #func)

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
	DeclIEntry(void operator+=(IWidget&)) //!< 向部件组添加部件。
	DeclIEntry(void operator+=(IControl&)) //!< 向焦点对象组添加控件。
	DeclIEntry(void operator+=(GMFocusResponser<IControl>&)) \
		//!< 向焦点对象组添加子焦点对象容器。
	DeclIEntry(bool operator-=(IWidget&)) //!< 从部件组移除部件。
	DeclIEntry(bool operator-=(IControl&)) //!< 从焦点对象组移除控件。
	DeclIEntry(bool operator-=(GMFocusResponser<IControl>&)) \
		//!< 从焦点对象组移除子焦点对象容器。
EndDecl


/*!
\brief 取指针指定的部件在视图树中的直接节点指针。
*/
template<class _tNode>
_tNode* FetchWidgetDirectNodePtr(IWidget* pWgt)
{
	_tNode* pNode(nullptr);

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
\brief 取指定部件的直接窗口指针。
\note 加入容器指针判断。
*/
IWindow*
FetchDirectWindowPtr(IWidget&);

/*!
\brief 取指定部件的直接桌面句柄。
\note 加入容器指针判断。
*/
Desktop*
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
LocateOffset(const IWindow*, Point, const IWidget*);

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
	return LocateOffset(FetchWindowPtr(w), pt, &w);
}

/*!
\brief 取指定部件 b 相对于部件 a 的偏移坐标。
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


typedef u8 ZOrderType; //!< Z 顺序类型：覆盖顺序，值越大表示越接近顶层。

const ZOrderType DefaultZOrder(64); //!< 默认 Z 顺序值。
const ZOrderType DefaultWindowZOrder(128); //!< 默认窗口 Z 顺序值。


//! \brief 部件容器模块。
class MUIContainer : protected GMFocusResponser<IControl>
{
public:
	typedef IWidget* ItemType; //!< 部件组项目类型。
	typedef multimap<ZOrderType, ItemType> WidgetMap; \
		//!< 部件组类型：映射 Z 顺序至部件。
	typedef set<GMFocusResponser<IControl>*> FocusContainerSet; \
		//!< 子焦点对象容器组类型。
	typedef WidgetMap::value_type PairType;

protected:
	WidgetMap sWidgets; //!< 部件对象组：存储非空部件指针。
	FocusContainerSet sFocusContainers; //!< 子焦点对象容器组。

public:
	/*!
	\brief 无参数构造：默认实现。
	*/
	MUIContainer() = default;
	virtual DefEmptyDtor(MUIContainer)

protected:
	/*!
	\brief 向部件组添加部件。
	\note 部件已存在时忽略。
	*/
	void
	operator+=(IWidget&);
	/*!
	\brief 向部件组添加控件。

	向焦点对象组添加焦点对象，同时向部件组按默认 Z 顺序值添加部件。
	\note 控件已存在时忽略。
	*/
	PDefHOperator1(void, +=, IControl& ctl)
		ImplRet(Add(ctl))
	/*!
	\brief 向子焦点对象容器组添加子焦点对象容器。
	*/
	PDefHOperator1(void, +=, GMFocusResponser<IControl>& rsp)
		ImplRet(static_cast<void>(sFocusContainers.insert(&rsp)))

	/*!
	\brief 从部件组移除部件。
	*/
	bool
	operator-=(IWidget&);
	/*!
	\brief 从部件组移除控件。

	从部件组移除部件，同时从焦点对象组移除焦点对象。
	*/
	bool
	operator-=(IControl&);
	/*!
	\brief 从子焦点对象容器组移除子焦点对象容器。
	*/
	bool
	operator-=(GMFocusResponser<IControl>&);

public:
	/*!
	\brief 判断是否包含指定部件。
	*/
	bool
	Contains(IWidget&);

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
	\brief 向部件组添加控件。

	向焦点对象组添加焦点对象，同时向部件组按指定 Z 顺序值添加部件。
	\note 控件已存在时忽略。
	*/
	void
	Add(IControl&, ZOrderType = DefaultZOrder);

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
class UIContainer : public Widget, protected MUIContainer,
	implements IUIContainer
{
public:
	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	UIContainer(const Rect& = Rect::Empty);

	ImplI1(IUIContainer) PDefHOperator1(void, +=, IWidget& wgt)
		ImplBodyBase1(MUIContainer, operator+=, wgt)
	ImplI1(IUIContainer) PDefHOperator1(bool, -=, IWidget& wgt)
		ImplBodyBase1(MUIContainer, operator-=, wgt)
	ImplI1(IUIContainer) PDefHOperator1(void, +=, IControl& ctl)
		ImplBodyBase1(MUIContainer, operator+=, ctl)
	ImplI1(IUIContainer) PDefHOperator1(bool, -=, IControl& ctl)
		ImplBodyBase1(MUIContainer, operator-=, ctl)
	ImplI1(IUIContainer) PDefHOperator1(void, +=,
		GMFocusResponser<IControl>& rsp)
		ImplBodyBase1(MUIContainer, operator+=, rsp)
	ImplI1(IUIContainer) PDefHOperator1(bool, -=,
		GMFocusResponser<IControl>& rsp)
		ImplBodyBase1(MUIContainer, operator-=, rsp)

	ImplI1(IUIContainer) DefMutableGetterBase(IControl*, FocusingPtr,
		GMFocusResponser<IControl>)
	ImplI1(IUIContainer) PDefH1(IWidget*, GetTopWidgetPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopWidgetPtr, pt)
	ImplI1(IUIContainer) PDefH1(IControl*, GetTopControlPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopControlPtr, pt)

	ImplI1(IUIContainer) PDefH0(void, ClearFocusingPtr)
		ImplBodyBase0(MUIContainer, ClearFocusingPtr)

	ImplI1(IUIContainer) PDefH1(bool, ResponseFocusRequest,
		AFocusRequester& req)
		ImplBodyBase1(MUIContainer, ResponseFocusRequest, req)

	ImplI1(IUIContainer) PDefH1(bool, ResponseFocusRelease,
		AFocusRequester& req)
		ImplBodyBase1(MUIContainer, ResponseFocusRelease, req)
};

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

