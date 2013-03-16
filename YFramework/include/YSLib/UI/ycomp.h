/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup UI UI
\ingroup YSLib
\brief YSLib UI 模块。
*/

/*!	\file ycomp.h
\ingroup UI
\brief 平台无关的 Shell 组件。
\version r2332
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-19 20:05:08 +0800
\par 修改时间:
	2013-03-13 15:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YComponent
*/


#ifndef YSL_INC_UI_ycomp_h_
#define YSL_INC_UI_ycomp_h_ 1

#include "../Core/ygdibase.h"
#include "../Core/yfunc.hpp"
#include "../Core/ymsgdef.h"
#include "../Adaptor/ycont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

//! \since build 319
class Image;

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(UI)

class AController;
FwdDeclI(IWidget)
//! \since build 294
class ImageBrush;
//! \since build 293
//@{
struct InputEventArgs;
struct KeyEventArgs;
struct TouchEventArgs;
struct PaintEventArgs;
class Renderer;
struct RoutedEventArgs;
//! \since build 294
class SolidBrush;
struct UIEventArgs;
class WidgetController;
//@}
class Window;


//类型别名。
/*!
\brief 画刷回调函数。
\since build 293
*/
typedef std::function<void(PaintEventArgs&&)> HBrush;


//名称引用。
using Drawing::PixelType;
using Drawing::BitmapPtr;
using Drawing::ConstBitmapPtr;
using Drawing::Color;

using Drawing::Point;
using Drawing::Vec;
using Drawing::Size;
using Drawing::Rect;

using Drawing::Graphics;
//! \since build 366
using Drawing::PaintContext;

YSL_END_NAMESPACE(UI)

//! \since build 388
YSL_BEGIN_NAMESPACE(Messaging)

DefMessageTarget(SM_PAINT, shared_ptr<UI::IWidget>)

YSL_END_NAMESPACE(Messaging)

YSL_END

#endif

