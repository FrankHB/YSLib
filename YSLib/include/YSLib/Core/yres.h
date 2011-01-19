/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yres.h
\ingroup Core
\brief 资源管理模块。
\version 0.1453;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-28 16:46:40 + 08:00;
\par 修改时间:
	2011-01-01 18:55 + 08:00;
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

//! \brief 资源接口。
DeclInterface(IResource)
EndDecl


/*!
\ingroup HelperFunction
\brief 全局默认共享资源生成函数。
\note 线程空间内共享；全局资源初始化之后可调用。
*/
template<class _type>
inline GHWeak<_type>
GetGlobalResource()
{
	return GStaticCache<_type, GHStrong<_type> >::GetPointer();
}

/*!
\ingroup HelperFunction
\brief 全局默认共享资源释放函数。
*/
template<class _type>
inline void
ReleaseGlobalResource() ythrow()
{
	return GStaticCache<_type, GHStrong<_type> >::Release();
}

//资源定义。

YSL_BEGIN_NAMESPACE(Drawing)

//图像资源。
class YImage : public GMCounter<YImage>, public YCountableObject,
	public BitmapBuffer,
	implements IResource
{
public:
	typedef YCountableObject ParentType;

	/*!
	\brief 构造：使用指定位图指针、长和宽。
	*/
	explicit
	YImage(ConstBitmapPtr = NULL, SDST = 0, SDST = 0);
	/*!
	\brief 析构：空实现。
	\note 无异常抛出。
	*/
	virtual DefEmptyDtor(YImage)

	DefGetter(BitmapPtr, ImagePtr, GetBufferPtr())

	void
	SetImage(ConstBitmapPtr, SDST = SCRW, SDST = SCRH);
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

