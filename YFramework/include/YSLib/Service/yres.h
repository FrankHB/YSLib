/*
	Copyright by FrankHB 2009-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.h
\ingroup Service
\brief 应用程序资源管理模块。
\version r562
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-28 16:46:40 +0800
\par 修改时间:
	2013-09-03 15:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YResource
*/


#ifndef YSL_INC_Service_yres_h_
#define YSL_INC_Service_yres_h_ 1

#include "../Core/ystorage.hpp"
#include "ygdi.h"

namespace YSLib
{

/*!
\brief 资源接口。
\since build 174
*/
DeclI(YF_API, IResource)
EndDecl


//应用程序资源类型定义。

namespace Drawing
{

/*!
\brief 图像资源。
\since build 205
*/
class YF_API Image : public CompactPixmap,
	implements IResource
{
public:
	/*!
	\brief 构造：使用指定位图指针、长和宽。
	*/
	explicit
	Image(ConstBitmapPtr = {}, SDst = 0, SDst = 0);
	//! \since build 433
	Image(const CompactPixmap&);
	//! \since build 433
	Image(CompactPixmap&&);

	DefGetter(const ynothrow, BitmapPtr, ImagePtr, GetBufferPtr())
};

} // namespace Drawing;

} // namespace YSLib;

#endif

