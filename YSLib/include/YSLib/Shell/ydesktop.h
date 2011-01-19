/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydesktop.h
\ingroup Shell
\brief 平台无关的桌面抽象层。
\version 0.2185;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-02 12:00:08 + 08:00;
\par 修改时间:
	2010-12-30 16:08 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YDesktop;
*/


#ifndef INCLUDED_YDESKTOP_H_
#define INCLUDED_YDESKTOP_H_

#include "ywindow.h"
//#include <list>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 桌面。
class YDesktop : public YFrameWindow
{
public:
	typedef YComponent ParentType;
	typedef list<IVisualControl*> DOs; //!< 桌面对象组类型。

private:
	YScreen& Screen; //!< 屏幕对象。
	DOs sDOs; //!< 桌面对象组（末尾表示顶端）。

public:
	/*!
	\brief 构造：使用指定屏幕对象引用、背景色和背景图像。
	*/
	explicit
	YDesktop(YScreen&, Color = 0, GHStrong<Drawing::YImage> = NULL);
	virtual DefEmptyDtor(YDesktop)

	DefGetter(const YScreen&, Screen, Screen) //!< 取屏幕对象。
	DefGetter(BitmapPtr, BackgroundPtr, Screen.GetCheckedBufferPtr()) \
		//!< 取屏幕背景指针。

	virtual PDefH(IVisualControl*, GetTopVisualControlPtr, const Point& p)
		ImplBodyBase(YDesktop, GetTopDesktopObjectPtr, p)

	/*!
	\brief 向桌面对象组添加桌面对象。
	*/
	virtual void
	operator+=(IVisualControl&);
	/*!
	\brief 从桌面对象组中移除指定桌面对象。
	*/
	virtual bool
	operator-=(IVisualControl&);
	/*!
	\brief 从桌面对象组中移除所有指定桌面对象。
	\return 移除的对象数。
	*/
	DOs::size_type
	RemoveAll(IVisualControl&);

	/*!
	\brief 请求提升至容器顶端。
	从桌面对象组中查找指定桌面对象并重新插入至顶端。
	*/
	bool
	MoveToTop(IVisualControl&);

	/*!
	\brief 移除桌面对象组中顶端桌面对象。
	移除桌面对象组中首个桌面对象。
	*/
	void
	RemoveTopDesktopObject();

	/*!
	\brief 清除桌面对象组。
	*/
	void
	ClearDesktopObjects();

	/*!
	\brief 取得桌面对象组中首个桌面对象的句柄。
	*/
	IVisualControl*
	GetFirstDesktopObjectPtr() const;
	/*!
	\brief 取得桌面对象组中顶端桌面对象的句柄。
	*/
	IVisualControl*
	GetTopDesktopObjectPtr() const;
	/*!
	\brief 取得桌面对象组中包含指定点的顶端桌面对象的句柄。
	*/
	IVisualControl*
	GetTopDesktopObjectPtr(const Point&) const;

	/*!
	\brief 绘制背景。
	*/
	virtual void
	DrawBackground();

protected:
	/*!
	\brief 绘制桌面对象。
	依次绘制桌面对象组对象。
	*/
	void
	DrawDesktopObjects();

public:
	/*!
	\brief 绘图。
	*/
	virtual void
	Draw();

	/*!
	\brief 刷新至窗口缓冲区。
	*/
	virtual void
	Refresh();

	/*!
	\brief 更新缓冲区至屏幕。
	*/
	virtual void
	Update();
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

