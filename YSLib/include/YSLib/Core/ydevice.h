/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydevice.h
\ingroup Core
\brief 平台无关的设备抽象层。
\version 0.2848;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:39:39 +0800;
\par 修改时间:
	2011-04-25 12:55 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YDevice;
*/


#ifndef YSL_INC_CORE_YDEVICE_H_
#define YSL_INC_CORE_YDEVICE_H_

#include "yobject.h"

YSL_BEGIN

/*!
\brief 默认屏幕初始化函数。
*/
extern bool
InitAllScreens();

YSL_BEGIN_NAMESPACE(Device)

//图形设备。
class YGraphicDevice : public YObject, public Drawing::Graphics
{
public:
	typedef YObject ParentType;

public:
	/*!
	\brief 构造：指定宽度和高度，从指定缓冲区指针。
	*/
	YGraphicDevice(SDst, SDst, Drawing::BitmapPtr = nullptr);
};

inline
YGraphicDevice::YGraphicDevice(SDst w, SDst h, Drawing::BitmapPtr p)
	: YObject(),
	Graphics(p, Drawing::Size(w, h))
{}


//屏幕。
class YScreen : public YGraphicDevice
{
	friend bool
	YSLib::InitAllScreens();

public:
	typedef YGraphicDevice ParentType;
	typedef int BGType;

private:
	static bool S_InitScr;

	/*!
	\brief 静态初始化。
	*/
	static void InitScreen();
	/*!
	\brief 状态检查。
	*/
	static void CheckInitialization();

	BGType bg;

public:
	/*!
	\brief 构造：指定宽度和高度，从指定缓冲区指针。
	*/
	YScreen(SDst, SDst, Drawing::BitmapPtr = nullptr);

	/*!
	\brief 复位。
	\note 无条件初始化。
	*/
	static void
	Reset();

	/*!
	\brief 取指针。
	\note 无异常抛出。
	\note 进行状态检查。
	*/
	virtual Drawing::BitmapPtr
	GetCheckedBufferPtr() const ynothrow;
	DefGetter(const BGType&, BgID, bg)

	DefSetter(const BGType&, BgID, bg)

	/*!
	\brief 更新。
	\note 复制到屏幕。
	*/
	void
	Update(Drawing::BitmapPtr);
	/*!
	\brief 更新。
	\note 以纯色填充屏幕。
	*/
	void
	Update(Drawing::Color = 0);
};

inline void
YScreen::InitScreen()
{
	S_InitScr = !InitAllScreens();
}

YSL_END_NAMESPACE(Device)

YSL_END

#endif

