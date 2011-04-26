/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Core Core
\ingroup YSLib
\brief YSLib 核心模块。
*/

/*!	\file ysdef.h
\ingroup Core
\brief 宏定义和类型描述。
\version 0.2501;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-24 15:29:11 +0800;
\par 修改时间:
	2011-04-23 13:08 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellDefinition;
*/


#ifndef YSL_INC_CORE_YSDEF_H_
#define YSL_INC_CORE_YSDEF_H_

//适配器模块。
#include "../Adaptor/yadaptor.h"

//间接访问类模块。
#include "../Adaptor/yref.hpp"

YSL_BEGIN

//前向声明和类型定义。

//空结构体类型。
struct EmptyType
{};

class YObject;
typedef EmptyType EventArgs; //!< 事件参数基类。

YSL_BEGIN_NAMESPACE(Device)
class YScreen;
YSL_END_NAMESPACE(Device)

YSL_BEGIN_NAMESPACE(Drawing)
class YFontCache;
YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Messaging)
class YMessageQueue;
YSL_END_NAMESPACE(Messaging)

YSL_BEGIN_NAMESPACE(Shells)
class YShell;
class YMainShell;
YSL_END_NAMESPACE(Shells)

YSL_BEGIN_NAMESPACE(Components)

class YDesktop;

YSL_END_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Text)
typedef size_t SizeType; //!< 字符大小类型。
typedef usize_t IndexType; //!< 字符索引类型。
class String;
YSL_END_NAMESPACE(Text)

class YApplication;
class YLog;

using Components::YDesktop;

using Device::YScreen;

using Drawing::YFontCache;
using Messaging::YMessageQueue;

using Shells::YMainShell;
using Shells::YShell;

using Text::String;


//平台无关的类型定义。
//


/*!	\defgroup GlobalObjects Global Objects
\brief 全局对象。
*/

/*!	\defgroup DefaultGlobalConstants Default Global Constants
\ingroup GlobalObjects
\brief 默认全局常量。
*/
//@{
//@}

/*!	\defgroup DefaultGlobalVariables Default Global Variables
\ingroup GlobalObjects
\brief 默认全局变量。
*/
//@{
//@}

/*!	\defgroup DefaultGlobalObjectMapping Default Global Object Mapping
\ingroup GlobalObjects
\brief 默认全局变量映射。

访问全局程序实例对象。
*/
//@{
//@}

YSL_END

#endif

