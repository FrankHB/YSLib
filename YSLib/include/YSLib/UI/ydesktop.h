/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydesktop.h
\ingroup UI
\brief 平台无关的桌面抽象层。
\version r2335;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-02 12:00:08 +0800;
\par 修改时间:
	2011-09-03 14:53 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YDesktop;
*/


#ifndef YSL_INC_UI_YDESKTOP_H_
#define YSL_INC_UI_YDESKTOP_H_

#include "ywindow.h"
#include "../Adaptor/cont.h"
#include "../Core/ydevice.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 桌面。
class Desktop : public Frame
{
private:
	Devices::Screen& screen; //!< 屏幕对象。

public:
	/*!
	\brief 构造：使用指定屏幕对象引用、背景色和背景图像。
	*/
	explicit
	Desktop(Devices::Screen&, Color = Drawing::ColorSpace::Black,
		const shared_ptr<Drawing::Image>& = shared_ptr<Drawing::Image>());

	DefGetter(const Devices::Screen&, Screen, screen) //!< 取屏幕对象。
	DefGetter(BitmapPtr, BackgroundPtr, screen.GetCheckedBufferPtr()) \
		//!< 取屏幕背景指针。

	/*!
	\brief 清除桌面内容。
	\note 直接清除桌面对象组、部件组和焦点对象组中的指针，不进行焦点或其它操作。
	*/
	void
	ClearContents();

public:
	/*!
	\brief 更新缓冲区至屏幕。
	*/
	virtual void
	Update();
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

