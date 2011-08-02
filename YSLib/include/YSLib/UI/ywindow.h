/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.h
\ingroup UI
\brief 样式无关的图形用户界面窗口。
\version 0.4569;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 +0800;
\par 修改时间:
	2011-08-02 11:45 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWindow;
*/


#ifndef YSL_INC_UI_YWINDOW_H_
#define YSL_INC_UI_YWINDOW_H_

#include "ycontrol.h"
#include "../Service/yres.h"
#include "yuicont.h"
#include "../Service/yblit.h"

YSL_BEGIN

#undef YWindowAssert

YSL_BEGIN_NAMESPACE(Components)

#ifdef YCL_USE_YASSERT

#	define YWindowAssert(ptr, comp, func) \
	Components::yassert((ptr) && FetchContext(*(ptr)).IsValid(), \
	"The graphics context is invalid.", __LINE__, __FILE__, #comp, #func)

#else

#	define YWindowAssert(ptr, comp, func) \
	assert((ptr) && FetchContext(*(ptr)).IsValid())

#endif

YSL_BEGIN_NAMESPACE(Forms)

//! \brief 窗口接口。
DeclBasedInterface1(IWindow, virtual IControl)
	DeclIEntry(IWindow* GetWindowPtr() const)
EndDecl


/*!
\brief 显示窗口。
\note 设置可见性和容器（若存在）背景重绘状态并设置自身刷新状态。
*/
void
Show(IWindow&);

/*!
\brief 隐藏窗口。
\note 设置不可见性和容器（若存在）背景重绘状态并取消自身刷新状态。
*/
void
Hide(IWindow&);

/*!
\brief 在父窗口设置无效区域。
\note 若父窗口不存在则忽略。
*/
void
SetInvalidationToParent(IWindow&);


//! \brief 窗口模块。
class MWindow : protected Widgets::MWindowObject
{
protected:
	//基类中的 pWindow 为父窗口对象句柄，若为空则说明无父窗口。
	mutable shared_ptr<Drawing::Image> hBgImage; //!< 背景图像句柄。

public:
	/*!
	\brief 构造：使用指定边界、背景图像和窗口指针。
	*/
	explicit
	MWindow(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()),
		IWindow* = nullptr);

	DefGetter(shared_ptr<Drawing::Image>&, BackgroundImagePtr, hBgImage)
	/*!
	\brief 取位图背景指针。
	*/
	BitmapPtr
	GetBackgroundPtr() const;
};


//! \brief 抽象窗口。
class AWindow : public Controls::Control, protected MWindow,
	implements IWindow
{
public:
	/*!
	\brief 构造：使用指定边界、背景图像和窗口指针。
	*/
	explicit
	AWindow(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()),
		IWindow* = nullptr);

	ImplI1(IWindow) DefGetterBase(IWindow*, WindowPtr, MWindow)
	using MWindow::GetBackgroundImagePtr;
	using MWindow::GetBackgroundPtr;

	/*!
	\brief 设置大小。
	\note 虚 \c public 实现。
	*/
	virtual void
	SetSize(const Size&);

	/*!
	\brief 设置无效区域。
	*/
	void
	SetInvalidation();

protected:
	/*!
	\brief 绘制背景图像。
	*/
	bool
	DrawBackgroundImage();

	DeclIEntry(bool DrawContents())

public:
	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	ImplI1(IWindow) Rect
	Refresh(const Graphics&, const Point&, const Rect&);

	/*!
	\brief 按需更新。
	\note 以父窗口、屏幕优先顺序。
	*/
	virtual void
	Update();
};


//! \brief 抽象框架窗口。
class AFrame : public AWindow, protected Widgets::MUIContainer
{
public:
	explicit
	AFrame(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()),
		IWindow* = nullptr);

	ImplI1(IWindow) void
	operator+=(IWidget&);
	ImplI1(IWindow) void
	operator+=(IControl&);
	ImplI1(IWindow) PDefHOperator1(void, +=, GMFocusResponser<IControl>& rsp)
		ImplBodyBase1(MUIContainer, operator+=, rsp)
	virtual void
	operator+=(IWindow&);
	template<class _type>
	inline void
	operator+=(_type& p)
	{
		return operator+=(static_cast<typename std::conditional<
			std::is_convertible<_type&, IWindow&>::value,
			IWindow&, typename std::conditional<std::is_convertible<_type&,
			IControl&>::value, IControl&, IWidget&>::type>::type>(p));
	}

	ImplI1(IWindow) bool
	operator-=(IWidget&);
	ImplI1(IWindow) bool
	operator-=(IControl&);
	ImplI1(IWindow) PDefHOperator1(bool, -=, GMFocusResponser<IControl>& rsp)
		ImplBodyBase1(MUIContainer, operator-=, rsp)
	virtual bool
	operator-=(IWindow&);
	template<class _type>
	inline bool
	operator-=(_type& p)
	{
		return operator-=(static_cast<typename std::conditional<
			std::is_convertible<_type&, IWindow&>::value,
			IWindow&, typename std::conditional<std::is_convertible<_type&,
			IControl&>::value, IControl&, IWidget&>::type>::type>(p));
	}

	using MUIContainer::Contains;

	ImplI1(IWindow) PDefH0(IControl*, GetFocusingPtr)
		ImplBodyBase0(GMFocusResponser<IControl>, GetFocusingPtr)
	ImplI1(IWindow) PDefH1(IWidget*, GetTopWidgetPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopWidgetPtr, pt)
	ImplI1(IWindow) PDefH1(IControl*, GetTopControlPtr, const Point& pt)
		ImplBodyBase1(MUIContainer, GetTopControlPtr, pt)

	void
	Add(IControl&, Widgets::ZOrderType = Widgets::DefaultZOrder);

	ImplI1(IWindow) void
	ClearFocusingPtr();

	ImplI1(IWindow) PDefH1(bool, ResponseFocusRequest, AFocusRequester& req)
		ImplBodyBase1(MUIContainer, ResponseFocusRequest, req)

	ImplI1(IWindow) PDefH1(bool, ResponseFocusRelease, AFocusRequester& req)
		ImplBodyBase1(MUIContainer, ResponseFocusRelease, req)
};


//! \brief 标准框架窗口。
class Frame : public AFrame
{
public:
	/*!
	\brief 构造：使用指定边界、背景图像和窗口指针。
	*/
	explicit
	Frame(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()),
		IWindow* = nullptr);
	virtual
	~Frame();

protected:
	/*!
	\brief 绘制内容。
	\note 绘制部件。
	*/
	ImplI1(AWindow) bool
	DrawContents();
};

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

