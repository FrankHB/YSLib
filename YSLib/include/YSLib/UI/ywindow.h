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
\version r4661;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 +0800;
\par 修改时间:
	2011-09-08 02:24 +0800;
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


//! \brief 窗口模块。
class MWindow : public noncopyable
{
protected:
	mutable shared_ptr<Drawing::Image> hBgImage; //!< 背景图像句柄。

public:
	/*!
	\brief 构造：使用指定边界和背景图像。
	*/
	explicit
	MWindow(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()));

	DefGetter(shared_ptr<Drawing::Image>&, BackgroundImagePtr, hBgImage)
	/*!
	\brief 取位图背景指针。
	*/
	BitmapPtr
	GetBackgroundPtr() const;
};


//! \brief 抽象窗口。
class AWindow : public Control, protected MWindow
{
public:
	/*!
	\brief 构造：使用指定边界和背景图像。
	*/
	explicit
	AWindow(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()));

	using MWindow::GetBackgroundImagePtr;
	using MWindow::GetBackgroundPtr;

	/*!
	\brief 设置大小。
	\note 虚 \c public 实现。
	*/
	virtual void
	SetSize(const Size&);

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
	virtual Rect
	Refresh(const Graphics&, const Point&, const Rect&);

	/*!
	\brief 按需更新。
	\note 以父窗口、屏幕优先顺序。
	*/
	virtual void
	Update();
};


//! \brief 抽象框架窗口。
class AFrame : public AWindow, protected MUIContainer
{
public:
	explicit
	AFrame(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()));

	virtual void
	operator+=(IWidget&);
	virtual void
	operator+=(AWindow&);
	template<class _type>
	inline void
	operator+=(_type& p)
	{
		return operator+=(static_cast<typename std::conditional<
			std::is_convertible<_type&, AWindow&>::value,
			AWindow&, IWidget&>::type>(p));
	}

	virtual bool
	operator-=(IWidget&);
	virtual bool
	operator-=(AWindow&);
	template<class _type>
	inline bool
	operator-=(_type& p)
	{
		return operator-=(static_cast<typename std::conditional<
			std::is_convertible<_type&, AWindow&>::value,
			AWindow&, IWidget&>::type>(p));
	}

	using MUIContainer::Contains;

	virtual PDefH2(IWidget*, GetTopWidgetPtr, const Point& pt,
		bool(&f)(const IWidget&))
		ImplBodyBase2(MUIContainer, GetTopWidgetPtr, pt, f)

	virtual void
	Add(IWidget&, ZOrderType = DefaultZOrder);

	/*!
	\brief 提升部件至顶端。

	子部件组中查找指定部件并重新插入至顶端。
	*/
	bool
	MoveToTop(IWidget&);
};


//! \brief 标准框架窗口。
class Frame : public AFrame
{
public:
	/*!
	\brief 构造：使用指定边界、背景图像和容器指针。
	*/
	explicit
	Frame(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()),
		IWidget* = nullptr);
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

YSL_END_NAMESPACE(Components)

YSL_END

#endif

