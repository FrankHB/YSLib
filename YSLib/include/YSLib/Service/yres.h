/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.h
\ingroup Service
\brief 应用程序资源管理模块。
\version r1518;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 +0800;
\par 修改时间:
	2011-09-13 23:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YResource;
*/


#ifndef YSL_INC_CORE_YRES_H_
#define YSL_INC_CORE_YRES_H_

#include "../Core/ycounter.hpp"
#include "../Core/ystorage.hpp"
#include "ygdi.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

//! \brief 资源接口。
DeclInterface(IResource)
EndDecl


//应用程序资源类型定义。

YSL_BEGIN_NAMESPACE(Drawing)

//图像资源。
class Image : public BitmapBuffer,
	implements IResource
{
public:
	/*!
	\brief 构造：使用指定位图指针、长和宽。
	*/
	explicit
	Image(ConstBitmapPtr = nullptr, SDst = 0, SDst = 0);

	DefGetter(BitmapPtr, ImagePtr, GetBufferPtr())

	void
	SetImage(ConstBitmapPtr, SDst = MainScreenWidth, SDst = MainScreenHeight);
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

