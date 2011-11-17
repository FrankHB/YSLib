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
\version r4682;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 +0800;
\par 修改时间:
	2011-11-12 11:23 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWindow;
*/


#ifndef YSL_INC_UI_YWINDOW_H_
#define YSL_INC_UI_YWINDOW_H_

#include "ycontrol.h"
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


//! \brief 抽象窗口。
class AWindow : public Control, protected MBackground
{
public:
	/*!
	\brief 构造：使用指定边界和背景图像。
	*/
	explicit
	AWindow(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = share_raw(new Drawing::Image()));
	inline DefDeMoveCtor(AWindow)

	using MBackground::GetBackgroundImagePtr;
	using MBackground::GetBackgroundPtr;

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
	Refresh(const PaintContext&);

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
	inline DefDeMoveCtor(AFrame)

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

	virtual PDefH(IWidget*, GetTopWidgetPtr, const Point& pt,
		bool(&f)(const IWidget&))
		ImplBodyBase(MUIContainer, GetTopWidgetPtr, pt, f)

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

protected:
	/*!
	\brief 绘制内容。
	\note 绘制部件。
	*/
	ImplI(AWindow) bool
	DrawContents();
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

