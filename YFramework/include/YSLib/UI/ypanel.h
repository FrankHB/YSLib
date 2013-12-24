/*
	© 2012-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ypanel.h
\ingroup UI
\brief 样式无关的 GUI 面板。
\version r426
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2011-04-13 20:40:51 +0800
\par 修改时间:
	2013-12-23 22:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YPanel
*/


#ifndef YSL_INC_UI_ypanel_h_
#define YSL_INC_UI_ypanel_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YControl
#include YFM_YSLib_UI_YUIContainer

namespace YSLib
{

namespace UI
{

/*!
\brief 面板。
\since build 201
*/
class YF_API Panel : public Control, protected MUIContainer
{
public:
	/*!
	\brief 构造：使用指定边界。
	\since build 337
	*/
	explicit
	Panel(const Rect& = {});
	inline DefDeMoveCtor(Panel)

	virtual void
	operator+=(IWidget&);

	virtual bool
	operator-=(IWidget&);

	using MUIContainer::Contains;

	/*!
	\since build 357
	\note 不使用 DefWidgetChildrenGetter 以避免对非必要文件的依赖。
	*/
	DefGetter(override, WidgetRange, Children, WidgetRange(begin(), end()))

	/*!
	\brief 按指定 Z 顺序向部件组添加部件，并设置指针。
	\see MUIContainer::Add 。
	\since build 279
	*/
	virtual void
	Add(IWidget&, ZOrderType = DefaultZOrder);

	/*!
	\brief 清除内容。

	清除焦点指针和部件组并无效化。
	*/
	void
	ClearContents();

	/*!
	\brief 提升部件至 Z 顺序相等的同组部件的顶端。
	\since build 279

	子部件组中查找指定部件并重新插入至顶端。
	*/
	bool
	MoveToTop(IWidget&);

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;

	//! \since build 460
	//@{
	using MUIContainer::begin;

	using MUIContainer::end;
	//@}
};

} // namespace UI;

} // namespace YSLib;

#endif

