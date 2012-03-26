/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Helper Helper
\ingroup YSLib
\brief YSLib 助手模块。
*/

/*!	\file yglobal.h
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version r2758;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-22 15:14:57 +0800;
\par 修改时间:
	2012-03-25 15:18 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#ifndef YSL_INC_HELPER_YGLOBAL_H_
#define YSL_INC_HELPER_YGLOBAL_H_

//包含 YCLib 公用部分。
#include <YCLib/ycommon.h>

#include "../Core/ygdibase.h"
#include "../Core/ymsgdef.h"
#include "../Core/ydevice.h"

YSL_BEGIN

/*!	\defgroup CustomGlobalVariables Custom Global Variables
\ingroup GlobalObjects
\brief 平台相关的全局变量。
\since build 173 。
*/
//@{
//@}


//前向声明。
YSL_BEGIN_NAMESPACE(Drawing)
YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Messaging)

/*!
\brief 输入消息内容。
\since build 210 。
*/
class InputContent
{
public:
	KeysInfo Keys;
	Drawing::Point CursorLocation;

	yconstfn DefDeCtor(InputContent)
	explicit yconstfn
	InputContent(const KeysInfo&, const Drawing::Point&);

	bool
	operator==(const InputContent&) const;
};

inline yconstfn
InputContent::InputContent(const KeysInfo& k, const Drawing::Point& pt)
	: Keys(k), CursorLocation(pt)
{}

DefMessageTarget(SM_INPUT, InputContent)

YSL_END_NAMESPACE(Messaging)


//全局函数。

/*!
\brief 以指定前景色和背景色初始化指定屏幕的控制台。
\since build 148 。
*/
bool
InitConsole(Devices::Screen&, Drawing::PixelType, Drawing::PixelType);

YSL_END

#endif

