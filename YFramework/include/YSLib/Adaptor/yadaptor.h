/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Adaptor Adaptor
\ingroup YSLib
\brief YSLib 适配器模块。
*/

/*!	\file yadaptor.h
\ingroup Adaptor
\brief 外部库关联。
\version r1421
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-02-22 20:16:21 +0800
\par 修改时间:
	2012-09-04 12:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YAdaptor
*/


#ifndef YSL_INC_ADAPTOR_YADAPTOR_H_
#define YSL_INC_ADAPTOR_YADAPTOR_H_ 1

//引入平台设置和存储调试设施。
#include "ynew.h"

//包含 YStandardEx 公用部分。
#include <ystdex/functional.hpp>
#include <ystdex/utility.hpp>

//包含 YCLib 公用部分。
#include <YCLib/ycommon.h>
#include <YCLib/Keys.h>
#include <YCLib/Timer.h>
#include <YCLib/FileSystem.h>
#include <YCLib/Video.h>

//包含 CHRLib 。
#include <CHRLib/chrproc.h>

// 确保包含 C 标准库必要部分。
#include <cstddef>
#include <cstdint>
#include <climits>
#include <cmath>

// 确保包含 C++ 标准库必要部分。
#include <algorithm> // for std::min, std::max;
#include <ystdex/string.hpp> // for std::to_string, ystdex::to_string;

/*
!\brief YSLib 命名空间。
\since 早于 build 132
*/
YSL_BEGIN

//目标平台中立接口。

/*!
\brief 通用数据类型。
\since build 245
*/
//@{
typedef std::uint8_t	u8;
typedef std::uint16_t	u16;
typedef std::uint32_t	u32;
typedef std::uint64_t	u64;
typedef std::int8_t		s8;
typedef std::int16_t	s16;
typedef std::int32_t	s32;
typedef std::int64_t	s64;
typedef volatile u8		vu8;
typedef volatile u16	vu16;
typedef volatile u32	vu32;
typedef volatile u64	vu64;
typedef volatile s8		vs8;
typedef volatile s16	vs16;
typedef volatile s32	vs32;
typedef volatile s64	vs64;
//@}

/*!
\brief 调用分派。
\since build 303
*/
//@{
using ystdex::seq_apply;
using ystdex::unseq_apply;
//@}

/*!
\brief 实用类型。
\since build 209
*/
//@{
using ystdex::noncopyable;
using ystdex::nullptr_t;
//@}

/*!
\brief 数学库函数。
\since build 301
\todo 使用 <tt>std::round</tt> g++ 4.7 后的 libstdc++ ，
	当 !defined _GLIBCXX_USE_C99_MATH_TR1 。
*/
//@{
using ::round;
//@}

/*!
\brief 算法。
\since build 265
*/
//@{
using std::min;
using std::max;
//}

//! \brief 助手功能。
//@{
//! \since build 291
using ystdex::arrlen;
//! \since build 308
using std::to_string;
//! \since build 308
using ystdex::to_string;
//@}


//非目标平台中立接口。

/*!
\brief 平台通用数据类型。
\since build 209
*/
//@{
using ystdex::errno_t;
using ystdex::ptrdiff_t;
using ystdex::size_t;
//! \since build 245
using ystdex::wint_t;
//@}

/*!
\brief 基本实用例程。
\since build 177
*/
//@{
//@}

/*!
\brief 文件系统例程。
\since build 171
*/
//@{
//! \since build 299
using platform::ufopen;
//! \since build 299
using platform::ufexists;
using platform::direxists;
//! \since build 312
using platform::udirexists;
//! \since build 304
using platform::u16getcwd_n;
//! \since build 313
using platform::uchdir;
//@}

//系统处理函数。
using platform::terminate;

//基本图形定义。
using platform::SPos;
using platform::SDst;

//文件系统抽象。
/*!
\brief 本机路径字符类型。
\since build 286
*/
using platform::NativePathCharType;
using platform::const_path_t;
using platform::path_t;

using platform::HFileNode;
using platform::IsAbsolute;
using platform::GetRootNameLength;

//文件系统常量。
using platform::PATHSTR;
using platform::FILENAMESTR;

//基本输出接口。
using platform::InitVideo;

//计时器和时钟。
using platform::GetTicks;
using platform::GetHighResolutionTicks;
using platform::StartTicks;

//输入类型。
namespace KeyCodes = platform::KeyCodes;
using platform::KeyInput;

//! \brief 设计模式。
YSL_BEGIN_NAMESPACE(Pattern)
YSL_END_NAMESPACE(Pattern)

//! \brief 图形处理。
YSL_BEGIN_NAMESPACE(Drawing)

using platform::PixelType;
using platform::BitmapPtr;
using platform::ConstBitmapPtr;
/*!
\since build 297
*/
//@{
using platform::FetchAlpha;
using platform::FetchOpaque;
//@}
using platform::Color;
namespace ColorSpace = platform::ColorSpace;

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

using namespace CHRLib;

YSL_END_NAMESPACE(Text)

using Text::ucs4_t;
using Text::ucs2_t;
using Text::ucsint_t;

YSL_END

#endif

