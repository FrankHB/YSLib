/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.h
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version r6334;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2012-02-04 22:10 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWidget;
*/


#ifndef YSL_INC_UI_YWIDGET_H_
#define YSL_INC_UI_YWIDGET_H_

#include "ycomp.h"
#include "yrender.h"
#include "yfocus.h"
#include "ywgtevt.h"
#include "ywgtview.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 部件接口。
\since 早于 build 132 。
*/
DeclI(IWidget)
	/*!
	\brief 取渲染器。
	*/
	DeclIEntry(Renderer& GetRenderer() const)
	/*!
	\brief 取部件视图。
	\warning 注意修改容器指针或焦点指针时，应保持和容器包含部件的状态同步。
	*/
	DeclIEntry(View& GetView() const)
	/*!
	\brief 取控制器。
	*/
	DeclIEntry(AController& GetController() const)
	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	\return 若为保存了子部件中的可见部件的容器则返回指针，否则返回 \c nullptr 。
	\note 使用部件坐标。
	*/
	DeclIEntry(IWidget* GetTopWidgetPtr(const Point&, bool(&)(const IWidget&)))

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	\return 实际被绘制的界面区域。
	\note 边界仅为暗示，允许实现忽略，但可以保证边界内的区域保持最新显示状态。
	\note 若部件的内部状态能够保证显示状态最新，则返回的区域可能比参数 r 更小。
	*/
	DeclIEntry(Rect Refresh(const PaintContext&))
EndDecl


/*!
\brief 判断部件是否可见。
\since build 259 。
*/
inline bool
IsVisible(const IWidget& wgt)
{
	return wgt.GetView().IsVisible();
}

/*!
\brief 判断点是否在部件的可视区域内。
\since build 167 。
*/
bool
Contains(const IWidget&, SPos, SPos);
/*!
\brief 判断点是否在部件的可视区域内。
\since build 167 。
*/
inline bool
Contains(const IWidget& wgt, const Point& pt)
{
	return Contains(wgt, pt.X, pt.Y);
}

/*!
\brief 判断点是否在可见部件的可视区域内。
\since build 173 。
*/
bool
ContainsVisible(const IWidget& wgt, SPos x, SPos y);
/*!
\brief 判断点是否在可见部件的可视区域内。
\since build 173 。
*/
inline bool
ContainsVisible(const IWidget& wgt, const Point& pt)
{
	return ContainsVisible(wgt, pt.X, pt.Y);
}


/*!
\ingroup HelperFunctions
\brief 取部件的容器指针。
\note 使用此函数确保返回值传递的值语义。
\since build 225 。
*/
inline IWidget*
FetchContainerPtr(const IWidget& wgt)
{
	return wgt.GetView().pContainer;
}

/*!
\ingroup HelperFunctions
\brief 取焦点对象指针。
\return 若为保存了子部件中的焦点对象的容器则返回指针，否则返回 \c nullptr 。
\since build 239 。
*/
inline IWidget*
FetchFocusingPtr(IWidget& wgt)
{
	return wgt.GetView().pFocusing;
}

/*!
\ingroup HelperFunctions
\brief 取部件位置。
\return 若为保存了子部件中的焦点对象的容器则返回指针，否则返回 \c nullptr 。
\since build 239 。
*/
inline const Point&
GetLocationOf(const IWidget& wgt)
{
	return wgt.GetView().GetLocation();
}

/*!
\ingroup HelperFunctions
\brief 取部件大小。
\return 若为保存了子部件中的焦点对象的容器则返回指针，否则返回 \c nullptr 。
\since build 259 。
*/
inline const Size&
GetSizeOf(const IWidget& wgt)
{
	return wgt.GetView().GetSize();
}

/*!
\brief 取部件边界。
\since build 190 。
*/
inline Rect
GetBoundsOf(const IWidget& wgt)
{
	return Rect(GetLocationOf(wgt), GetSizeOf(wgt));
}

/*!
\brief 设置部件边界。
\since build 177 。
*/
void
SetBoundsOf(IWidget&, const Rect&);

/*!
\brief 设置部件的容器指针。
\since build 269 。
*/
inline void
SetContainerPtrOf(IWidget& wgt, IWidget* pCon = nullptr)
{
	wgt.GetView().pContainer = pCon;
}

/*!
\brief 设置部件左上角所在位置（相对于容器的偏移坐标）。
\since build 259 。
*/
void
SetLocationOf(IWidget&, const Point&);

/*
\brief 设置部件大小。
\since build 259 。
*/
void
SetSizeOf(IWidget&, const Size&);

/*!
\brief 设置部件可见性。
\since build 259 。
*/
inline void
SetVisibleOf(IWidget& wgt, bool b)
{
	wgt.GetView().SetVisible(b);
}


/*!
\brief 检查指定部件是否满足 Contains(wgt, pt) && f(wgt) 。
\return 若满足则返回部件指针，否则为 nullptr 。
\since build 247 。
*/
IWidget*
CheckWidget(IWidget& wgt, const Point& pt, bool(&f)(const IWidget&));


/*!
\brief 请求提升至容器顶端。

\todo 完全实现提升 IWidget 至容器顶端（目前仅实现父容器为 Panel 的情形）。
\since build 192 。
*/
void
RequestToTop(IWidget&);


/*!
\brief 显示部件。

设置部件可见性后无效化。
\since build 229 。
*/
void
Show(IWidget&);

/*!
\brief 隐藏部件。

设置部件不可见性后无效化。
\since build 229 。
*/
void
Hide(IWidget&);

/*!
\brief 关闭部件。

隐藏部件后取消容器（若存在）焦点状态。
\since build 275 。
*/
void
Close(IWidget&);


/*!
\brief 部件控制器。
\since build 236 。
*/
class WidgetController : public AController
{
public:
	GEventWrapper<EventT(HPaintEvent), UIEventArgs> Paint;

	/*!
	\brief 构造：使用指定可用性。
	*/
	explicit
	WidgetController(bool = false);

	ImplI(AController) EventMapping::ItemType&
	GetItemRef(const VisualEvent&);

	ImplI(AController) DefClone(WidgetController, Clone)
};


/*!
\brief 部件。
\since 早于 build 132 。
*/
class Widget : implements IWidget
{
private:
	unique_ptr<View> pView; //!< 部件视图指针。
	unique_ptr<Renderer> pRenderer; //!< 渲染器指针。

public:
	unique_ptr<AController> pController; //!< 控制器指针。
	Color BackColor; //!< 默认背景色。
	Color ForeColor; //!< 默认前景色。

	explicit
	Widget(const Rect& = Rect::Empty,
		Color = Drawing::ColorSpace::White, Color = Drawing::ColorSpace::Black);
	/*!
	\brief 构造：使用视图指针、渲染器指针和控制器指针。
	\param pView_ 视图指针。
	\param pRenderer_ 渲染器指针。
	\param pController_ 控制器指针。
	\pre <tt>bool(pView_) && bool(pRenderer_)</tt> 。
	*/
	PDefTmplH3(_tView, _tRenderer, _tController)
	inline Widget(_tView&& pView_ = unique_raw(new View()),
		_tRenderer&& pRenderer_ = unique_raw(new Renderer()),
		_tController pController_ = nullptr)
		: pView(yforward(pView_)), pRenderer(yforward(pRenderer_)),
		pController(yforward(pController_)),
		BackColor(Drawing::ColorSpace::White),
		ForeColor(Drawing::ColorSpace::Black)
	{
		YAssert(bool(pView) && bool(pRenderer),
			"Null pointer(s) found @ Widget::Widget#2;");
	}
	/*!
	\brief 复制构造：除容器指针为空外深复制。
	*/
	Widget(const Widget&);
	DefDeMoveCtor(Widget)
	/*!
	\brief 析构：虚实现。

	自动释放焦点后释放部件资源。
	\note 由于不完整类型 WidgetController 的依赖性无法使用 inline 实现。
	*/
	virtual
	~Widget();

	DefPredMem(const ynothrow, Transparent, GetView())

	DefGetterMem(const ynothrow, SPos, X, GetView())
	DefGetterMem(const ynothrow, SPos, Y, GetView())
	DefGetterMem(const ynothrow, SDst, Width, GetView())
	DefGetterMem(const ynothrow, SDst, Height, GetView())
	ImplI(IWidget) DefGetter(const ynothrow, Renderer&, Renderer, *pRenderer)
	ImplI(IWidget) DefGetter(const ynothrow, View&, View, *pView)
	ImplI(IWidget) AController&
	GetController() const;
	ImplI(IWidget) PDefH(IWidget*, GetTopWidgetPtr, const Point&,
		bool(&)(const IWidget&))
		ImplRet(nullptr)

	DefSetterMem(bool, Transparent, GetView())
	DefSetterMem(SDst, X, GetView())
	DefSetterMem(SDst, Y, GetView())
	DefSetterMem(SDst, Width, GetView())
	DefSetterMem(SDst, Height, GetView())
	/*!
	\brief 设置渲染器为指定指针指向的对象，同时更新渲染器状态。
	\note 若指针为空，则使用以当前部件边界新建的 Renderer 对象。
	\note 取得指定对象的所有权。
	*/
	void
	SetRenderer(unique_ptr<Renderer>&&);
	/*!
	\brief 设置渲染器为指定指针指向的对象，同时更新渲染器状态。
	\note 若指针为空，则使用以当前部件边界新建的 View 对象。
	\note 取得指定对象的所有权。
	*/
	void
	SetView(unique_ptr<View>&&);

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	ImplI(IWidget) Rect
	Refresh(const PaintContext&);
};

inline AController&
Widget::GetController() const
{
	if(!pController)
		throw BadEvent();
	return *pController;
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

