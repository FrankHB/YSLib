/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydevice.cpp
\ingroup Core
\brief 平台无关的设备抽象层。
\version r2748;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-28 16:39:51 +0800;
\par 修改时间:
	2012-04-24 21:28 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YDevice;
*/


#include "YSLib/Core/ydevice.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Devices)

BitmapPtr
Screen::GetCheckedBufferPtr() const ynothrow
{
	YAssert(GetBufferPtr(), "Null pointer found.");

	return GetBufferPtr();
}

void
Screen::Update(BitmapPtr buf)
{
	mmbcpy(GetCheckedBufferPtr(), buf, GetSizeOfBuffer());
}

YSL_END_NAMESPACE(Devices)

YSL_END

