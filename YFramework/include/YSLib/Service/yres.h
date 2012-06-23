/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.h
\ingroup Service
\brief 应用程序资源管理模块。
\version r1534;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-28 16:46:40 +0800;
\par 修改时间:
	2012-06-22 23:13 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Service::YResource;
*/


#ifndef YSL_INC_CORE_YRES_H_
#define YSL_INC_CORE_YRES_H_

#include "../Core/ystorage.hpp"
#include "ygdi.h"

YSL_BEGIN

/*!
\brief 资源接口。
\since build 174 。
*/
DeclI(IResource)
EndDecl


//应用程序资源类型定义。

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 图像资源。
\since build 205 。
*/
class Image : public BitmapBuffer,
	implements IResource
{
public:
	/*!
	\brief 构造：使用指定位图指针、长和宽。
	*/
	explicit
	Image(ConstBitmapPtr = nullptr, SDst = 0, SDst = 0);

	DefGetter(const ynothrow, BitmapPtr, ImagePtr, GetBufferPtr())
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

