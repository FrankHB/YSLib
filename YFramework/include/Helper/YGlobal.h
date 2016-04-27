/*
	© 2009-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YGlobal.h
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version r1945
\author FrankHB <frankhb1989@gmail.com>
\since build 569
\par 创建时间:
	2009-12-22 15:14:57 +0800
\par 修改时间:
	2016-04-27 14:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::YGlobal
*/


#ifndef INC_Helper_YGlobal_h_
#define INC_Helper_YGlobal_h_ 1

#include "YModules.h"
#include "../YCLib/YModules.h"
#include YFM_YCLib_YCommon
#if YCL_Win32
#	include YFM_Win32_YCLib_MinGW32
#elif YCL_Android
#	include YFM_Android_YCLib_Android
#elif YF_Hosted
#	include YFM_YCLib_Host
#endif
#include YFM_YSLib_Core_YGDIBase
#include YFM_YSLib_Core_YMessageDefinition
#include YFM_YSLib_Core_YDevice

namespace YSLib
{

/*!	\defgroup CustomGlobalVariables Custom Global Variables
\ingroup GlobalObjects
\brief 平台相关的全局变量。
\since build 173
*/
//@{
//@}


namespace Drawing
{
} // namespace Drawing;

namespace Messaging
{
} // namespace Messaging;

//! \since build 388
namespace UI
{

class Desktop;

} // namespace UI;

//! \since build 502
class Environment;
//! \since build 689
class GUIHost;
#if YF_Hosted
/*!
\brief 宿主环境支持。
\since build 378
*/
namespace Host
{

//! \since build 381
class Window;
class RenderWindow;
class WindowThread;
class HostRenderer;

/*!
\brief 宿主异常：平台异常。
\since build 427
*/
using platform_ex::Exception;

} // namespace Host;
#endif

//! \brief 运行时平台。
#if YCL_DS
namespace DS
{

using namespace platform_ex;

} // namespace DS;
#elif YCL_Win32
//! \since build 569
namespace Windows
{

using namespace platform_ex;

} // namespace Windows;
#	if YCL_MinGW
//! \since build 569
namespace MinGW
{

using namespace YSLib::Windows;

} // namespace MinGW;
#	endif
#elif YCL_Android
//! \since build 492
namespace Android
{

using namespace platform_ex;

} // namespace Android;
#endif

//! \since build 388
using UI::Desktop;

} // namespace YSLib;

#if YCL_Android
/*!
\brief 用户提供定义的入口函数。
\since build 498
*/
extern "C" void
y_android_main();
#endif

#endif

