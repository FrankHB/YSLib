/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydevice.cpp
\ingroup Core
\brief 平台无关的设备抽象层。
\version r1769
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-28 16:39:51 +0800
\par 修改时间:
	2014-04-25 09:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YDevice
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YDevice

namespace YSLib
{

using namespace Drawing;

namespace Devices
{

BitmapPtr
Screen::GetCheckedBufferPtr() const ynothrow
{
	YAssertNonnull(GetBufferPtr());

	return GetBufferPtr();
}

void
Screen::Update(BitmapPtr buf) ynothrow
{
	std::copy_n(buf, GetAreaOf(GetSize()), GetCheckedBufferPtr());
}

} // namespace Devices;

} // namespace YSLib;

