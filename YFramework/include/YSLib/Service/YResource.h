/*
	© 2009-2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YResource.h
\ingroup Service
\brief 应用程序资源管理模块。
\version r582
\author FrankHB <frankhb1989@gmail.com>
\since build 578
\par 创建时间:
	2009-12-28 16:46:40 +0800
\par 修改时间:
	2015-02-22 08:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YResource
*/


#ifndef YSL_INC_Service_YResource_h_
#define YSL_INC_Service_YResource_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YStorage
#include YFM_YSLib_Service_YGDI

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


/*!
\brief 按指定图像大小旋转左上角坐标。
\since build 578
*/
//@{
yconstfn PDefH(Point, RotateCenter, const Size& s)
	ImplRet({(SPos(s.Width) - SPos(s.Height)) / 2,
		(SPos(s.Height) - SPos(s.Width)) / 2})
inline PDefH(Point, RotateCenter, const Image& img)
	ImplRet(RotateCenter(img.GetSize()))
//@}

} // namespace Drawing;

} // namespace YSLib;

#endif

