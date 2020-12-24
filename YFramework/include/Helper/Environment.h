/*
	© 2013-2016, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Environment.h
\ingroup Helper
\brief 环境。
\version r1072
\author FrankHB <frankhb1989@gmail.com>
\since build 521
\par 创建时间:
	2013-02-08 01:28:03 +0800
\par 修改时间:
	2020-12-24 12:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Environment
*/


#ifndef INC_Helper_Environment_h_
#define INC_Helper_Environment_h_ 1

#include "YModules.h"
#include YFM_Helper_YGlobal
#include YFM_YSLib_Core_ValueNode // for ValueNode;

namespace YSLib
{

#if YCL_Android
//! \since build 502
//@{
namespace Devices
{

class AndroidScreen;

} // namespace Devices;

namespace Android
{

class NativeHost;

} // namespace Android;
//@}
#endif


/*!
\brief 应用程序环境。
\since build 378
*/
class YF_API Environment
{
public:
	/*!
	\brief 环境根节点。
	\since build 688
	*/
	ValueNode Root{};

	/*!
	\brief 构造：初始化环境。
	\note Win32 平台：尝试无参数调用 FixConsoleHandler ，若失败则跟踪警告。
	\sa Application::AddExit
	\since build 693
	*/
	Environment(Application&);
	~Environment();

	//! \since build 688
	//@{
	/*!
	\brief 取值类型根节点。
	\pre 断言：已初始化。
	*/
	ValueNode&
	GetRootRef() ynothrowv;
	//@}
};

} // namespace YSLib;

#endif

