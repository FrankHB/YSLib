/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Win32GUI.h
\ingroup YCLib
\ingroup MinGW32
\brief Win32 GUI 接口。
\version r103
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 11:29:04 +0800
\par 修改时间:
	2013-07-10 16:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Win32GUI
*/


#ifndef YCL_MinGW32_INC_Win32GUI_h_
#define YCL_MinGW32_INC_Win32GUI_h_ 1

#include "MinGW32.h"
#include <YSLib/Core/ygdibase.h>

namespace platform_ex
{

//! \since build 389
typedef ::HWND NativeWindowHandle;

namespace Windows
{

/*!
\brief 本机窗口引用。
\note 不具有所有权。
\since build 427
*/
class YF_API WindowReference
{
protected:
	NativeWindowHandle hWindow;

public:
	WindowReference(NativeWindowHandle h = {})
		: hWindow(h)
	{}
	DefDeMoveCtor(WindowReference)

	YSLib::Drawing::Point
	GetLocation() const;
	DefGetter(const ynothrow, NativeWindowHandle, NativeHandle, hWindow)
	YSLib::Drawing::Size
	GetSize() const;

	//! \note 线程安全：跨线程调用时使用基于消息队列的异步设置。
	void
	Close();

	/*!
	\brief 移动窗口。
	\note 线程安全。
	*/
	void
	Move(const YSLib::Drawing::Point&);

	/*!
	\brief 调整窗口大小。
	\note 线程安全。
	*/
	void
	Resize(const YSLib::Drawing::Size&);

	/*!
	\brief 按客户区调整窗口大小。
	\note 线程安全。
	*/
	void
	ResizeClient(const YSLib::Drawing::Size&);

	/*!
	\brief 显示窗口。
	\return 异步操作是否成功。
	*/
	bool
	Show() ynothrow;
};

} // namespace Windows;

} // namespace platform_ex;

#endif

