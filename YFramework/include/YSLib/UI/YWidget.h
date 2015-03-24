/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YWidget.h
\ingroup UI
\brief 样式无关的 GUI 部件。
\version r5786
\author FrankHB <frankhb1989@gmail.com>
\since build 569
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2015-03-22 20:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidget
*/


#ifndef YSL_INC_UI_ywidget_h_
#define YSL_INC_UI_ywidget_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YFocus
#include <ystdex/any_iterator.hpp> // for ystdex::any_input_iterator;
#include YFM_YSLib_UI_YWidgetView
#include YFM_YSLib_UI_YRenderer

namespace YSLib
{

namespace UI
{

/*!	\defgroup UIModels User Interface Models
\ingroup UI
\brief 用户界面模型。
*/

/*!
\brief 通用部件迭代器。
\since build 356
*/
using WidgetIterator = ystdex::any_input_iterator<IWidget>;
/*!
\brief 两个部件迭代器组成的左闭右开区间确定的迭代器范围。
\since build 357
*/
using WidgetRange = pair<WidgetIterator, WidgetIterator>;


/*!
\brief 部件接口。
\none 注意具体实现不保证可复制或可转移。
\since 早于 build 132
*/
DeclI(YF_API, IWidget)
	/*!
	\brief 取视图内的子部件。
	\return 包含子部件的迭代器范围，否则返回 WidgetRange() 。
	\note 允许实现可选优化：排除完全在部件边界外的子部件。
	\note 迭代器有效性由派生类约定。
	\since build 357
	*/
	DeclIEntry(WidgetRange GetChildren())
	/*!
	\brief 取控制器。
	\note 引用有效性由派生类约定。
	\since build 243
	*/
	DeclIEntry(AController& GetController() const)
	/*!
	\brief 取渲染器。
	\note 引用有效性由派生类约定。
	\since build 242
	*/
	DeclIEntry(Renderer& GetRenderer() const)
	/*!
	\brief 取部件视图。
	\note 引用有效性由派生类约定。
	\warning 注意修改容器指针或焦点指针时，应保持和容器包含部件的状态同步。
	\since build 569
	*/
	DeclIEntry(AView& GetView() const)
EndDecl

//! \relates IWidget
//@{
/*!
\brief 判断部件是否可见。
\since build 259
*/
inline PDefH(bool, IsVisible, const IWidget& wgt)
	ImplRet(wgt.GetView().IsVisible())

/*!
\brief 判断点是否在部件的可视区域内。
\since build 167
*/
bool
YF_API Contains(const IWidget&, SPos, SPos);
/*!
\brief 判断点是否在部件的可视区域内。
\since build 167
*/
inline PDefH(bool, Contains, const IWidget& wgt, const Point& pt)
	ImplRet(Contains(wgt, pt.X, pt.Y))

/*!
\brief 判断点是否在可见部件的可视区域内。
\since build 173
*/
bool
YF_API ContainsVisible(const IWidget& wgt, SPos x, SPos y);
/*!
\brief 判断点是否在可见部件的可视区域内。
\since build 173
*/
inline PDefH(bool, ContainsVisible, const IWidget& wgt, const Point& pt)
	ImplRet(ContainsVisible(wgt, pt.X, pt.Y))

/*!
\ingroup helper_functions
\brief 取部件的容器指针。
\note 使用此函数确保返回值传递的值语义。
\since build 225
*/
inline PDefH(IWidget*, FetchContainerPtr, const IWidget& wgt)
	ImplRet(wgt.GetView().ContainerPtr)

/*!
\ingroup helper_functions
\brief 取焦点对象指针。
\return 若为保存了子部件中的焦点对象的容器则返回指针，否则返回 \c nullptr 。
\since build 473
*/
inline PDefH(IWidget*, FetchFocusingPtr, const IWidget& wgt)
	ImplRet(wgt.GetView().FocusingPtr)

/*!
\ingroup helper_functions
\brief 取部件位置。
\since build 569
*/
inline PDefH(Point, GetLocationOf, const IWidget& wgt)
	ImplRet(wgt.GetView().GetLocation())

/*!
\ingroup helper_functions
\brief 取部件大小。
\since build 569
*/
inline PDefH(Size, GetSizeOf, const IWidget& wgt)
	ImplRet(wgt.GetView().GetSize())

/*!
\brief 取部件边界。
\since build 190
*/
inline PDefH(Rect, GetBoundsOf, const IWidget& wgt)
	ImplRet(wgt.GetView().GetBounds())

/*!
\brief 设置部件边界。
\note 调用 SetLocationOf 和 SetSizeOf 。用户程序不应预期具有确定的调用顺序。
\since build 177
*/
YF_API void
SetBoundsOf(IWidget&, const Rect&);

/*!
\brief 设置部件的容器指针。
\since build 269
*/
inline PDefH(void, SetContainerPtrOf, IWidget& wgt, IWidget* pCon = {})
	ImplExpr(wgt.GetView().ContainerPtr = pCon)

/*!
\brief 设置部件的无效区域。
\since build 231
*/
YF_API void
SetInvalidationOf(IWidget&);

/*!
\brief 在容器设置部件的无效区域。
\note 若容器不存在则忽略。
\since build 229
*/
YF_API void
SetInvalidationToParent(IWidget&);

/*!
\brief 设置部件左上角所在位置（相对容器的偏移坐标）。
\note 设置视图状态后触发 Move 事件。
\since build 259
*/
YF_API void
SetLocationOf(IWidget&, const Point&);

/*!
\brief 设置部件大小。
\note 依次设置渲染器和视图状态，然后触发 Resize 事件。
\since build 259
*/
YF_API void
SetSizeOf(IWidget&, const Size&);

/*!
\brief 设置部件可见性。
\since build 259
*/
inline PDefH(void, SetVisibleOf, IWidget& wgt, bool b)
	ImplExpr(wgt.GetView().SetVisible(b))

/*!
\brief 关闭部件。
\since build 275

隐藏部件后取消容器（若存在）焦点状态。
*/
YF_API void
Close(IWidget&);

/*!
\brief 隐藏部件。
\since build 229
依次释放部件焦点、设置部件不可见性和无效化。
*/
YF_API void
Hide(IWidget&);

/*!
\brief 无效化：使相对部件的指定区域在直接和间接的窗口缓冲区中无效。
\since build 268
*/
YF_API void
Invalidate(IWidget&, const Rect&);
/*!
\brief 无效化：使部件区域在直接和间接的窗口缓冲区中无效。
\since build 224
*/
inline PDefH(void, Invalidate, IWidget& wgt)
	ImplExpr(Invalidate(wgt, GetSizeOf(wgt)))

//! \since build 473
//@{
//! \brief 无效化：使相对部件及子部件的指定区域在直接和间接的窗口缓冲区中无效。
YF_API void
InvalidateAll(IWidget&, const Rect&);
//! \brief 无效化：使部件及子部件区域在直接和间接的窗口缓冲区中无效。
inline PDefH(void, InvalidateAll, IWidget& wgt)
	ImplExpr(InvalidateAll(wgt, GetSizeOf(wgt)))

//! \brief 无效化：使相对部件的子部件的指定区域在直接和间接的窗口缓冲区中无效。
YF_API void
InvalidateChildren(IWidget&, const Rect&);
//! \brief 无效化：使部件的子部件区域在直接和间接的窗口缓冲区中无效。
inline PDefH(void, InvalidateChildren, IWidget& wgt)
	ImplExpr(InvalidateChildren(wgt, GetSizeOf(wgt)))
//@}

/*!
\brief 无效化部件区域对应位置的父容器区域。
\note 若不存在父容器则忽略。
\sa UI::Invalidate
\since build 433
*/
YF_API void
InvalidateParent(IWidget&);

//! \since build 455
//@{
/*!
\brief 无效化：使相对可见的部件的指定区域在直接和间接的窗口缓冲区中无效。
\sa UI::Invalidate
*/
YF_API void
InvalidateVisible(IWidget&, const Rect&);
/*!
\brief 无效化：使可见的部件区域在直接和间接的窗口缓冲区中无效。
\sa UI::Invalidate
*/
inline PDefH(void, InvalidateVisible, IWidget& wgt)
	ImplExpr(InvalidateVisible(wgt, GetSizeOf(wgt)))

/*!
\brief 无效化可见的部件区域对应位置的父容器区域。
\note 若不存在可见的父容器则忽略。
\sa UI::InvalidateParent
\sa UI::InvalidateVisible
*/
YF_API void
InvalidateVisibleParent(IWidget&);
//@}

/*!
\brief 调用指定子部件的 Paint 事件绘制参数指定的事件发送者。
\sa Renderer::Paint
\since build 263

以 e.Sender() 作为绘制目标，判断其边界是否和区域 e.ClipArea 相交，
若相交区域非空则调用 wgt 的渲染器的 Paint 方法绘制 。
调用中， e.Location 被修改为相对子部件的坐标， e.ClipArea 被覆盖为相交区域。
之后， e 可继续被 e.GetSender() 的渲染器的 Paint 方法修改。
*/
YF_API void
PaintChild(IWidget& wgt, PaintEventArgs&& e);
/*!
\brief 调用指定子部件的 Paint 事件绘制指定子部件。
\return 实际绘制的区域。
\note 使用指定子部件作为事件发送者并复制参数。
\since build 294

以 wgt 作为绘制目标，判断其边界是否和区域 pc.ClipArea 相交，
若相交区域非空则新建 PaintEventArgs ，调用 wgt 的渲染器的 Paint 方法绘制 。
*/
YF_API Rect
PaintChild(IWidget& wgt, const PaintContext& pc);

/*!
\brief 调用 PaintChild 指定子部件并提交参数的重绘区域。
\since build 522
*/
YF_API void
PaintChildAndCommit(IWidget&, PaintEventArgs&);

/*!
\brief 调用可见的 PaintChild 指定子部件并提交参数的重绘区域。
\since build 522
*/
inline void
PaintVisibleChildAndCommit(IWidget& wgt, PaintEventArgs& e)
	ImplExpr(IsVisible(wgt) ? PaintChildAndCommit(wgt, e) : void())

/*!
\brief 请求提升至容器前端。
\note 必要时无效化。
\since build 467
\todo 完全实现提升 IWidget 至容器顶端（目前仅实现父容器为 Panel 的情形）。
*/
YF_API void
RequestToFront(IWidget&);

/*!
\brief 显示部件。

依次设置部件可见性、请求部件焦点和无效化。
\since build 229
*/
YF_API void
Show(IWidget&);
//@}


/*!
\brief 部件。
\since 早于 build 132
*/
class YF_API Widget : implements IWidget
{
private:
	/*!
	\invariant 非空。
	\since build 346
	*/
	//@{
	/*!
	\brief 部件视图指针。
	\since build 569
	*/
	unique_ptr<AView> view_ptr;
	unique_ptr<Renderer> renderer_ptr; //!< 渲染器指针。
	unique_ptr<AController> controller_ptr; //!< 控制器指针。
	//@}

public:
	/*!
	\brief 背景。
	\since build 294
	*/
	mutable HBrush Background;

	//! \since build 384
	explicit
	Widget(const Rect& = {});
	//! \since build 515
	explicit
	Widget(const Rect&, HBrush);
	/*!
	\brief 构造：使用视图指针、渲染器指针和控制器指针，无背景。
	\param pView_ 视图指针。
	\param pRenderer_ 渲染器指针。
	\param pController_ 控制器指针。
	\pre <tt>bool(pView_) && bool(pRenderer_)</tt> 。
	*/
	template<typename _tView, typename _tRenderer, typename _tController>
	explicit inline
	Widget(_tView&& pView_ = make_unique<View>(),
		_tRenderer&& pRenderer_ = make_unique<Renderer>(),
		_tController&& pController_ = {})
		: view_ptr(yforward(pView_)), renderer_ptr(yforward(pRenderer_)),
		controller_ptr(yforward(pController_)), Background()
	{
		YAssertNonnull(view_ptr), YAssertNonnull(renderer_ptr);
		InitializeEvents();
	}
	/*!
	\brief 复制构造：除容器指针为空外深复制。
	*/
	Widget(const Widget&);
	//! since build 428
	DefDelMoveCtor(Widget)
	/*!
	\brief 析构：虚实现。

	自动释放焦点后释放部件资源。
	\note 由于不完整类型 WidgetController 的依赖性无法使用 inline 实现。
	*/
	virtual
	~Widget();

private:
	/*!
	\brief 初始化事件组。
	\since build 294
	*/
	void
	InitializeEvents();

public:
	//! \since build 357
	DefGetter(ImplI(IWidget), WidgetRange, Children, WidgetRange())
	/*!
	\brief 取空白画刷。
	\return 颜色为 ColorSpace::Black 的 SolidBrush 对象初始化的 HBrush 对象。
	\note 为减少包含头文件，使用非 inline 实现。
	\sa SolidBrush
	\since build 485
	*/
	static HBrush
	MakeBlankBrush();
	DefGetter(const ImplI(IWidget), AController&, Controller,
		Deref(controller_ptr))
	DefGetterMem(const ynothrow, SDst, Height, GetView())
	DefGetter(const ImplI(IWidget), Renderer&, Renderer, Deref(renderer_ptr))
	//! \since build 569
	DefGetter(const ImplI(IWidget), AView&, View, Deref(view_ptr))
	DefGetterMem(const ynothrow, SDst, Width, GetView())
	DefGetterMem(const ynothrow, SPos, X, GetView())
	DefGetterMem(const ynothrow, SPos, Y, GetView())

	DefSetterMem(SDst, Height, GetView())
	DefSetterMem(SDst, Width, GetView())
	//! \since build 587
	DefSetterMem(SPos, X, GetView())
	//! \since build 587
	DefSetterMem(SPos, Y, GetView())
	/*!
	\brief 设置渲染器为指定指针指向的对象，同时更新渲染器状态。
	\note 若指针为空，则使用以当前部件边界新建的 Renderer 对象。
	\since build 406
	*/
	void
	SetRenderer(unique_ptr<Renderer>);
	/*!
	\brief 设置视图为指定指针指向的对象。
	\note 若指针为空，则使用以当前部件边界新建的 View 对象。
	\note 确保视图后在设置之前首先交换抽象视图的成员。
	\since build 570
	*/
	void
	SetView(unique_ptr<AView>);

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\note 默认按 GetChildren() 得到的迭代器范围绘制可见子部件。
	\sa PaintContext
	\since build 294

	由参数指定的信息绘制事件发送者。参数的 ClipArea 成员指定边界。
	边界仅为暗示，允许实现忽略，但应保证调用后边界内的区域保持最新显示状态。
	绘制结束后更新边界，表示实际被绘制的区域。
	若部件的内部状态能够保证显示状态最新，则返回时边界区域可能更小。
	*/
	virtual void
	Refresh(PaintEventArgs&&);
};

/*!
\brief 包装渲染器：设置渲染器并返回被设置的渲染器引用。
\pre 间接断言：指针参数非空。
\relates Widget
\since build 548
*/
//@{
template<class _tRenderer>
inline _tRenderer&
WrapRenderer(Widget& wgt, unique_ptr<_tRenderer> p_renderer)
{
	auto& res(Deref(p_renderer));

	wgt.SetRenderer(std::move(p_renderer));
	return res;
}
template<class _tRenderer, typename... _tParams>
inline _tRenderer&
WrapRenderer(Widget& wgt, _tParams&&... args)
{
	return UI::WrapRenderer(wgt, make_unique<_tRenderer>(yforward(args)...));
}
//@}

} // namespace UI;

} // namespace YSLib;

#endif

