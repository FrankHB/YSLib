/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydesktop.h
\ingroup UI
\brief 平台无关的桌面抽象层。
\version r1402
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-05-02 12:00:08 +0800
\par 修改时间:
	2013-01-28 19:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YDesktop
*/


#ifndef YSL_INC_UI_YDESKTOP_H_
#define YSL_INC_UI_YDESKTOP_H_ 1

#include "ywindow.h"
#include "../Core/ydevice.h"
#include "yrender.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 桌面。
\since 早于 build 132
*/
class YF_API Desktop : public Window
{
private:
	Devices::Screen& screen; //!< 屏幕对象。

public:
	/*!
	\brief 构造：使用指定屏幕对象引用、背景色和背景图像。
	*/
	explicit
	Desktop(Devices::Screen&, Color = Drawing::ColorSpace::Black,
		const shared_ptr<Drawing::Image>& = {});
	inline DefDeMoveCtor(Desktop)

	DefGetter(const ynothrow, const Devices::Screen&, Screen, screen) \
		//!< 取屏幕对象。
	DefGetter(const ynothrow, BitmapPtr, BackgroundPtr,
		screen.GetCheckedBufferPtr()) //!< 取屏幕背景指针。

	DefGetter(ynothrow, Rect, InvalidatedArea,
		GetRenderer().CommitInvalidation(Rect()))

	/*!
	\brief 更新缓冲区至屏幕。
	*/
	void
	Update();

	/*!
	\brief 验证：绘制缓冲区使之有效。
	\since build 267
	\todo 渲染器类型安全。
	*/
	Rect
	Validate();
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

