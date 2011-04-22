/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Shell Shell
\ingroup YSLib
\brief YSLib Shell 模块。
*/

/*!	\file ycomp.h
\ingroup Shell
\brief 平台无关的 Shell 组件。
\version 0.3079;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-19 20:05:08 +0800;
\par 修改时间:
	2011-04-21 06:38 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YComponent;
*/


#ifndef YSL_INC_SHELL_YCOMP_H_
#define YSL_INC_SHELL_YCOMP_H_

#include "../Core/yobject.h"
#include "../Core/yshell.h"
#include "../Core/yapp.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)
PDeclInterface(IControl)
PDeclInterface(IPanel)
YSL_END_NAMESPACE(Controls)

YSL_BEGIN_NAMESPACE(Forms)
PDeclInterface(IWindow)
class YFrame;
class YForm;
YSL_END_NAMESPACE(Forms)

YSL_BEGIN_NAMESPACE(Widgets)
PDeclInterface(IWidget)
PDeclInterface(IUIBox)
PDeclInterface(IUIContainer)
YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

using Components::Controls::IControl;
using Components::Controls::IPanel;
using Components::Forms::IWindow;
using Components::Widgets::IWidget;
using Components::Widgets::IUIBox;
using Components::Widgets::IUIContainer;

DeclareHandle(IWindow, HWND)

YSL_BEGIN_NAMESPACE(Components)

//! \brief 基本组件接口。
DeclInterface(IComponent)
EndDecl


//! \brief 基本组件。
class YComponent : public GMCounter<YComponent>, public YCountableObject,
	implements IComponent
{
public:
	typedef YCountableObject ParentType;
};

YSL_END_NAMESPACE(Components)

using Components::Forms::YForm;
using Components::Forms::YFrame;

/*!
\ingroup HeplerFunction
\brief 取当前线程空间中运行的 Shell 句柄。
*/
inline GHandle<YShell>
FetchShellHandle()
{
	return GetApp().GetShellHandle();
}

YSL_END

#endif

