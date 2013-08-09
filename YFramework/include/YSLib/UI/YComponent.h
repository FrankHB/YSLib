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

/*!	\file YComponent.h
\ingroup UI
\brief 平台中立的 UI 组件。
\version r2373
\author FrankHB <frankhb1989@gmail.com>
\since build 426
\par 创建时间:
	2010-03-19 20:05:08 +0800
\par 修改时间:
	2013-08-05 21:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YComponent
*/


#ifndef YSL_INC_UI_YComponent_h_
#define YSL_INC_UI_YComponent_h_ 1

#include "../Core/ygdibase.h"
#include "../Core/yfunc.hpp"
#include "../Core/ymsgdef.h"
#include "../Adaptor/ycont.h"

namespace YSLib
{

namespace Drawing
{

//! \since build 319
class Image;

/*!
\brief 转换为字符串。
\note 使用 ADL 。
\since build 308
*/
//@{
template<typename _type>
string
to_string(const GBinaryGroup<_type>& val)
{
	using YSLib::to_string;

	return "(" + to_string(val.X) + ", " + to_string(val.Y) + ')';
}
YF_API string
to_string(const Size&);
YF_API string
to_string(const Rect&);
//@}

} // namespace Drawing;

namespace UI
{

class AController;
//! \since build 422
struct CursorEventArgs;
//! \since build 422
class GUIState;
FwdDeclI(IWidget)
//! \since build 294
class ImageBrush;
//! \since build 293
//@{
struct InputEventArgs;
struct KeyEventArgs;
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
using HBrush = std::function<void(PaintEventArgs&&)>;


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

} // namespace UI;

//! \since build 388
namespace Messaging
{

DefMessageTarget(SM_PAINT, shared_ptr<UI::IWidget>)

} // namespace Messaging;

} // namespace YSLib;

#endif

