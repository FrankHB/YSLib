/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.h
\ingroup Shell
\brief 平台无关的图形用户界面窗口实现。
\version 0.4022;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 + 08:00;
\par 修改时间:
	2011-01-22 09:52 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWindow;
*/


#ifndef INCLUDED_YWINDOW_H_
#define INCLUDED_YWINDOW_H_

#include "yguicomp.h"

YSL_BEGIN

#undef YWindowAssert

YSL_BEGIN_NAMESPACE(Components)

#ifdef YCL_USE_YASSERT

#	define YWindowAssert(ptr, comp, func) \
	Components::yassert((ptr) && (ptr)->GetContext().IsValid(), \
	"The graphics context is invalid.", __LINE__, __FILE__, #comp, #func)

#else

#	define YWindowAssert(ptr, comp, func) \
	assert((ptr) && (ptr)->GetContext().IsValid())

#endif

YSL_BEGIN_NAMESPACE(Forms)

//! \brief 窗口接口。
DeclBasedInterface(IWindow, virtual IUIContainer, virtual IVisualControl)
	DeclIEntry(bool IsRefreshRequired() const)
	DeclIEntry(bool IsUpdateRequired() const)

	DeclIEntry(const Graphics& GetContext() const) //!< 取图形接口上下文。
	DeclIEntry(HWND GetWindowHandle() const)

	DeclIEntry(void SetRefresh(bool))

	DeclIEntry(void Draw())

	DeclIEntry(void Update()) //!< 按需更新（以父窗口、屏幕优先顺序）。
EndDecl


//! \brief 窗口模块。
class MWindow : protected Widgets::MWindowObject
{
protected:
	//基类中的 hWindow 为父窗口对象句柄，若为空则说明无父窗口。
	GHStrong<YImage> prBackImage; //!< 背景图像指针。
	bool bRefresh; //!< 刷新属性：表示有新的绘制请求。
	bool bUpdate; //!< 更新属性：表示绘制结束，缓冲区准备完毕。

public:
	/*!
	\brief 构造：使用指定背景图像、窗口句柄和 Shell 。
	*/
	explicit
	MWindow(const GHStrong<YImage> = ynew YImage(), HWND = NULL);

	DefPredicate(RefreshRequired, bRefresh)
	DefPredicate(UpdateRequired, bUpdate)

	DefGetter(GHStrong<YImage>, Background, prBackImage)

	DefSetterDe(GHStrong<YImage>, Background, prBackImage, NULL)
};


//! \brief 抽象窗口。
class AWindow : public Controls::AVisualControl, protected MWindow,
	virtual implements IWindow
{
public:
	typedef Controls::AVisualControl ParentType;

	/*!
	\brief 构造：使用指定边界、背景图像、窗口句柄和 Shell 句柄。
	*/
	explicit
	AWindow(const Rect& = Rect::Empty,
		const GHStrong<YImage> = ynew YImage(), HWND = NULL);
	virtual DefEmptyDtor(AWindow)

	ImplI(IWindow) DefPredicateBase(RefreshRequired, MWindow)
	ImplI(IWindow) DefPredicateBase(UpdateRequired, MWindow)

	ImplI(IWindow) DefGetterBase(HWND, WindowHandle, MWindowObject)
	DefGetterBase(GHStrong<YImage>, Background, MWindow)
	/*!
	\brief 取位图背景指针。
	*/
	BitmapPtr
	GetBackgroundPtr() const;

	ImplI(IWindow) DefSetter(bool, Refresh, bRefresh)
	/*!
	\brief 设置大小。
	\note 虚公有实现。
	*/
	virtual void
	SetSize(const Size&);
	ImplI(IWindow) DefSetterBaseDe(GHStrong<YImage>, Background, MWindow,
		NULL)
	DeclIEntry(void SetBufferSize(const Size&)) //!< 设置显示缓冲区大小。

	PDefH(void, ClearBackground) const //!< 清除背景。
		ImplExpr(ClearImage(GetContext()))

	PDefH(void, BeFilledWith, PixelType c) const
		ImplExpr(Drawing::Fill(GetContext(), c)) \
		//!< 以纯色填充显示缓冲区。

protected:
	/*!
	\brief 绘制背景图像。
	*/
	bool
	DrawBackgroundImage();

public:
	/*!
	\brief 绘制背景。
	*/
	ImplI(IWindow) void
	DrawBackground();

protected:
	DeclIEntry(bool DrawWidgets())

public:
	/*!
	\brief 绘图。
	*/
	ImplI(IWindow) void
	Draw();

	/*!
	\brief 刷新至窗口缓冲区。
	*/
	ImplI(IWindow) void
	Refresh();

	/*!
	\brief 按需更新。
	\note 以父窗口、屏幕优先顺序。
	*/
	ImplI(IWindow) void
	Update();

	/*!
	\brief 请求提升至容器顶端。
	*/
	ImplI(IWindow) void
	RequestToTop();

	/*!
	\brief 更新至指定图形设备上下文的指定点。
	\note 以相对于容器的坐标作为相对于图形设备上下文的偏移。
	*/
	void
	UpdateTo(const Graphics&, const Point& = Point::Zero) const;

	/*!
	\brief 更新至桌面。
	*/
	virtual void
	UpdateToDesktop();

	/*!
	\brief 更新至上层窗口缓冲区。
	*/
	virtual void
	UpdateToWindow() const;

	/*!
	\brief 显示窗口。
	\note 自动设置可见性和更新至屏幕。
	*/
	void
	Show();
};


//! \brief 抽象框架窗口。
class AFrameWindow : public AWindow, protected Widgets::MUIContainer
{
public:
	typedef AWindow ParentType;

	explicit
	AFrameWindow(const Rect& = Rect::Empty,
		const GHStrong<YImage> = ynew YImage(), HWND = NULL);
	/*!
	\note 无异常抛出。
	*/
	virtual
	~AFrameWindow() ythrow();

	virtual PDefHOperator(void, +=, IWidget& w)
		ImplExpr(sWgtSet += w)
	virtual PDefHOperator(bool, -=, IWidget& w)
		ImplRet(sWgtSet -= w)
	virtual PDefHOperator(void, +=, IVisualControl& c)
		ImplBodyBaseVoid(MUIContainer, operator+=, c)
	virtual PDefHOperator(bool, -=, IVisualControl& c)
		ImplBodyBase(MUIContainer, operator-=, c)
	virtual PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c)
		ImplBodyBaseVoid(MUIContainer, operator+=, c)
	virtual PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c)
		ImplBodyBase(MUIContainer, operator-=, c)

	ImplI(IWindow) PDefH(IVisualControl*, GetFocusingPtr)
		ImplBodyBase(GMFocusResponser<IVisualControl>, GetFocusingPtr)
	ImplI(IWindow) PDefH(IWidget*, GetTopWidgetPtr, const Point& p)
		ImplBodyBase(MUIContainer, GetTopWidgetPtr, p)
	ImplI(IWindow) PDefH(IVisualControl*, GetTopVisualControlPtr,
		const Point& p)
		ImplBodyBase(MUIContainer, GetTopVisualControlPtr, p)

	ImplI(IWindow) PDefH(void, ClearFocusingPtr)
		ImplBodyBaseVoid(MUIContainer, ClearFocusingPtr)

	ImplI(IWindow) PDefH(bool, ResponseFocusRequest, AFocusRequester& w)
		ImplBodyBase(MUIContainer, ResponseFocusRequest, w)

	ImplI(IWindow) PDefH(bool, ResponseFocusRelease, AFocusRequester& w)
		ImplBodyBase(MUIContainer, ResponseFocusRelease, w)
};


//! \brief 框架窗口。
class YFrameWindow : public GMCounter<YFrameWindow>, public YComponent,
	public AFrameWindow
{
public:
	typedef YComponent ParentType;

protected:
	Drawing::BitmapBuffer Buffer; //!< 显示缓冲区。

public:
	/*!
	\brief 构造：使用指定边界、背景图像、窗口句柄和 Shell 句柄。
	*/
	explicit
	YFrameWindow(const Rect& = Rect::Empty,
		const GHStrong<YImage> = ynew YImage(), HWND = NULL);
	/*!
	\note 无异常抛出。
	*/
	virtual
	~YFrameWindow() ythrow();

	ImplI(AWindow) DefGetter(const Graphics&, Context, Buffer)

	ImplI(AWindow) PDefH(void, SetBufferSize, const Size& s)
		ImplExpr(Buffer.SetSize(s.Width, s.Height)) //!< 设置显示缓冲区大小。

protected:
	/*!
	\brief 绘制部件组。
	*/
	ImplI(AWindow) bool
	DrawWidgets();
};

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

