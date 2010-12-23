/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.h
\ingroup Shell
\brief 平台无关的图形用户界面窗口实现。
\version 0.3878;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 + 08:00;
\par 修改时间:
	2010-12-21 15:45 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWindow;
*/


#ifndef INCLUDED_YWINDOW_H_
#define INCLUDED_YWINDOW_H_

#include "ywidget.h"
#include "yguicomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

//! \brief 窗口接口。
DeclBasedInterface(IWindow, virtual IUIContainer, virtual IVisualControl)
	DeclIEntry(operator Graphics() const) //!< 生成图形接口上下文。

	DeclIEntry(bool IsRefreshRequired() const)
	DeclIEntry(bool IsUpdateRequired() const)

	DeclIEntry(HWND GetWindowHandle() const)

	DeclIEntry(void SetRefresh(bool))
	DeclIEntry(void SetUpdate(bool))

	DeclIEntry(void Draw())

	DeclIEntry(void Update()) //!< 按需更新（以父窗口、屏幕优先顺序）。
EndDecl


//! \brief 桌面对象模块。
class MDesktopObject
{
protected:
	GHHandle<YDesktop> hDesktop; //!< 桌面句柄。

public:
	/*!
	\brief 构造：使用指定桌面句柄。
	*/
	explicit
	MDesktopObject(GHHandle<YDesktop>);

protected:
	DefEmptyDtor(MDesktopObject)

public:
	//判断从属关系。
	PDefH(bool, BelongsTo, GHHandle<YDesktop> hDsk) const
		ImplRet(hDesktop == hDsk)

	DefGetter(GHHandle<YDesktop>, DesktopHandle, hDesktop)
};

inline
MDesktopObject::MDesktopObject(GHHandle<YDesktop> hDsk)
	: hDesktop(hDsk)
{}


//! \brief 窗口模块。
class MWindow : protected Widgets::MWindowObject
{
protected:
	Drawing::BitmapBuffer Buffer; //!< 显示缓冲区。
	//基类中的 hWindow 为父窗口对象句柄，若为空则说明无父窗口。
	GHStrong<YImage> prBackImage; //!< 背景图像指针。
	bool bRefresh; //!< 刷新属性：表示有新的绘制请求。
	bool bUpdate; //!< 更新属性：表示绘制结束，缓冲区准备完毕。

public:
	/*!
	\brief 构造：使用指定背景图像、窗口句柄和 Shell 。
	*/
	explicit
	MWindow(const GHStrong<YImage> = new YImage(), HWND = NULL);

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
		const GHStrong<YImage> = new YImage(), HWND = NULL);
	virtual DefEmptyDtor(AWindow)

	ImplI(IWindow) DefConverterMember(Graphics, Buffer)

	ImplI(IWindow) DefPredicateBase(RefreshRequired, MWindow)
	ImplI(IWindow) DefPredicateBase(UpdateRequired, MWindow)

	ImplI(IWindow) DefGetterBase(HWND, WindowHandle, MWindowObject)
	ImplI(IWindow) DefGetter(const Drawing::BitmapBuffer&, Buffer, Buffer) \
		//!< 取显示缓冲区。
	ImplI(IWindow) DefGetterMember(BitmapPtr, BufferPtr, Buffer) \
		//!< 取缓冲区指针。
	DefGetter(HWND, Handle, HWND(const_cast<AWindow*>(this)))
	DefGetterBase(GHStrong<YImage>, Background, MWindow)
	/*!
	\brief 取位图背景指针。
	*/
	BitmapPtr
	GetBackgroundPtr() const;

	ImplI(IWindow) DefSetter(bool, Refresh, bRefresh)
	ImplI(IWindow) DefSetter(bool, Update, bUpdate)
	/*!
	\brief 设置大小。
	\note 虚公共实现。
	*/
	virtual void
	SetSize(const Size&);
	ImplI(IWindow) DefSetterBaseDe(GHStrong<YImage>, Background, MWindow,
		NULL)

	PDefH(void, ClearBackground) const //!< 清除背景。
		ImplExpr(Buffer.ClearImage())

	PDefH(void, BeFilledWith, PixelType c)
		ImplBodyMemberVoid(Buffer, BeFilledWith, c) //!< 以纯色填充显示缓冲区。

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
	ImplA(IWindow)
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
	\brief 更新至屏幕。
	*/
	virtual void
	UpdateToScreen() const;

	/*!
	\brief 更新至上层窗口缓冲区。
	*/
	virtual void
	UpdateToWindow() const;

	/*!
	\brief 更新至指定桌面所在的屏幕。
	*/
	void
	UpdateToScreen(YDesktop&) const;

	/*!
	\brief 更新至指定窗口缓冲区。
	*/
	void
	UpdateToWindow(IWindow&) const;

	/*!
	\brief 显示窗口。
	\note 自动设置可见性和更新至屏幕。
	*/
	void
	Show();
};


//! \brief 框架窗口。
class YFrameWindow : public GMCounter<YFrameWindow>, public YComponent,
	public AWindow, protected Widgets::MUIContainer
{
public:
	typedef YComponent ParentType;

	/*!
	\brief 构造：使用指定边界、背景图像、窗口句柄和 Shell 句柄。
	*/
	explicit
	YFrameWindow(const Rect& = Rect::Empty,
		const GHStrong<YImage> = new YImage(), HWND = NULL);
	/*!
	\brief 无异常抛出。
	\note 空实现。
	*/
	virtual
	~YFrameWindow() ythrow();

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

