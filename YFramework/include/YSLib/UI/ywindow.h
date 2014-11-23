/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.h
\ingroup UI
\brief 样式无关的 GUI 窗口。
\version r3844
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-28 16:46:40 +0800
\par 修改时间:
	2014-11-21 09:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWindow
*/


#ifndef YSL_INC_UI_ywindow_h_
#define YSL_INC_UI_ywindow_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YPanel

namespace YSLib
{

namespace UI
{

/*!
\brief 窗口。
\since build 264
*/
class YF_API Window : public Panel
{
public:
	/*!
	\brief 构造：使用指定边界并设置渲染器。
	\note 若渲染器指针为空则忽略设置渲染器。
	\since build 407
	*/
	explicit
	Window(const Rect& = {},
		unique_ptr<Renderer> = make_unique<BufferedRenderer>());
	DefDeMoveCtor(Window)

	void
	operator+=(IWidget&) override;
	virtual void
	operator+=(Window&);
	template<class _type>
	inline void
	operator+=(_type& p)
	{
		return operator+=(ystdex::conditional_t<
			std::is_convertible<_type&, Window&>::value, Window&, IWidget&>(p));
	}

	bool
	operator-=(IWidget&) override;
	virtual bool
	operator-=(Window&);
	template<class _type>
	inline bool
	operator-=(_type& p)
	{
		return operator-=(ystdex::conditional_t<
			std::is_convertible<_type&, Window&>::value, Window&, IWidget&>(p));
	}

	using MUIContainer::Contains;

	//! \since build 555
	void
	Add(IWidget&, ZOrder = DefaultZOrder) override;
};

} // namespace UI;

} // namespace YSLib;

#endif

