/*
	Copyright (C) by Franksoft 2009 - 2010.

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
\version 0.2471;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-24 15:29:11 + 08:00;
\par 修改时间:
	2010-12-24 23:48 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShellDefinition;
*/


#ifndef INCLUDED_YSDEF_H_
#define INCLUDED_YSDEF_H_

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
typedef std::size_t SizeType; //!< 字符大小类型。
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


#define DeclareHandle(type, handle) typedef GHHandle<type> handle;


//类型定义。
typedef s16 SPOS; //!< 屏幕坐标度量。
typedef u16 SDST; //!< 屏幕坐标距离。


/*!	\defgroup GlobalObjects Global Objects
\brief 全局对象。
*/

/*!	\defgroup DefaultGlobalConstants Default Global Constants
\ingroup GlobalObjects
\brief 默认全局常量。
*/
//@{
//! \brief 屏幕大小。
extern const SDST SCRW, SCRH;
//@}

/*!	\defgroup DefaultGlobalVariables Default Global Variables
\ingroup GlobalObjects
\brief 默认全局变量。
*/
//@{
//! \brief 全局日志。
extern YLog DefaultLog;
//@}

/*!	\defgroup DefaultGlobalObjectMapping Default Global Object Mapping
\ingroup GlobalObjects
\brief 默认全局变量映射。

访问全局程序实例对象。
*/
//@{
//! \brief 全局应用程序对象。
extern YApplication& theApp;
//@}

YSL_END

#endif

