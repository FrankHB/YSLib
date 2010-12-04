/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.h
\ingroup Core
\brief 资源管理模块。
\version 0.1411;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 + 08:00;
\par 修改时间:
	2010-11-27 23:39 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YResource;
*/


#ifndef INCLUDED_YRES_H_
#define INCLUDED_YRES_H_

#include "yobject.h"
#include "../Shell/ygdi.h"

YSL_BEGIN

/*!
\brief 全局默认共享资源生成函数。
\note 线程空间内共享；全局资源初始化之后可调用。
*/
template<class T>
GHStrong<T>&
GetGlobalResource()
{
	static GHStrong<T> p(new T);

	return p;
}

//资源定义。

YSL_BEGIN_NAMESPACE(Drawing)

//图像资源。
class YImage
	: public GMCounter<YImage>, public YCountableObject, public BitmapBuffer
{
public:
	typedef YCountableObject ParentType;

	/*!
	\brief 构造：使用指定位图指针、长和宽。
	*/
	explicit
	YImage(ConstBitmapPtr = NULL, SDST = 0, SDST = 0);

	DefGetter(BitmapPtr, ImagePtr, GetBufferPtr())

	void
	SetImage(ConstBitmapPtr, SDST = SCRW, SDST = SCRH);
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

