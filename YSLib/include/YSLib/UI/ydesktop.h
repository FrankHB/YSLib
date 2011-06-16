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
\version 0.2320;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-02 12:00:08 +0800;
\par 修改时间:
	2011-06-15 21:26 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YDesktop;
*/


#ifndef YSL_INC_SHELL_YDESKTOP_H_
#define YSL_INC_SHELL_YDESKTOP_H_

#include "ywindow.h"
#include "../Adaptor/cont.h"
#include "../Core/ydevice.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 桌面。
class Desktop : public Forms::Frame
{
private:
	Devices::Screen& Screen; //!< 屏幕对象。

public:
	/*!
	\brief 构造：使用指定屏幕对象引用、背景色和背景图像。
	*/
	explicit
	Desktop(Devices::Screen&, Color = 0,
		const shared_ptr<Drawing::Image>& = shared_ptr<Drawing::Image>());

	DefGetter(const Devices::Screen&, Screen, Screen) //!< 取屏幕对象。
	DefGetter(BitmapPtr, BackgroundPtr, Screen.GetCheckedBufferPtr()) \
		//!< 取屏幕背景指针。

	virtual PDefH1(IControl*, GetTopControlPtr, const Point& p)
		ImplBodyBase1(Desktop, GetTopVisibleDesktopObjectPtr, p)

	/*!
	\brief 取桌面对象组中包含指定点的可见的顶端桌面对象指针。
	*/
	IControl*
	GetTopVisibleDesktopObjectPtr(const Point&) const;

	/*!
	\brief 清除桌面内容。
	\note 直接清除桌面对象组、部件组和焦点对象组中的指针，不进行焦点或其它操作。
	*/
	void
	ClearContents();

	/*!
	\brief 请求提升至容器顶端。

	从桌面对象组中查找指定桌面对象并重新插入至顶端。
	*/
	bool
	MoveToTop(IControl&);

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

