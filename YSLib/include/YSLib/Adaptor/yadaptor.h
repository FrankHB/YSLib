/*
	Copyright (C) by Franksoft 2010 - 2011.

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
\version r2244;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-22 20:16:21 +0800;
\par 修改时间:
	2011-10-03 01:14 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YAdaptor;
*/


#ifndef YSL_INC_ADAPTOR_YADAPTOR_H_
#define YSL_INC_ADAPTOR_YADAPTOR_H_

//引入平台设置和存储调试设施。
#include "ynew.h"

//包含 YCLib 公用部分。
#include <ycommon.h>
#include <ystdex/cstdio.h>
#include <ystdex/cstring.h>
#include <ystdex/utility.hpp>


#include <CHRLib/chrproc.h> //包含 CHRLib 。

// !\brief YSLib 命名空间。
YSL_BEGIN

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

	using ystdex::errno_t;
	using ystdex::nullptr_t;
	using ystdex::ptrdiff_t;
	using ystdex::size_t;
	using ystdex::ssize_t;
	using ystdex::wint_t;

	using ystdex::noncopyable;

	using platform::const_path_t;
	using platform::path_t;

	//文件系统常量。
	using platform::DEF_PATH_DELIMITER;
	using platform::DEF_PATH_SEPERATOR;
	using platform::PATHSTR;
	using platform::FILENAMESTR;

	using platform::mmbset;
	using platform::mmbcpy;
	using platform::direxists;
	using platform::chdir;
	using platform::getcwd_n;

	using platform::terminate;

	using platform::SPos;
	using platform::SDst;

	using platform::HDirectory;
	using platform::IsAbsolute;
	using platform::GetRootNameLength;

	using platform::ScreenSynchronize;

	using platform::WaitForInput;

	using platform::GetRTC;
	using platform::ResetRTC;

	using platform::InitVideo;

	namespace KeySpace = platform::KeySpace;
	using platform::KeyCode;
	using platform::KeysInfo;
	using platform::CursorInfo;

//! \brief 设计模式。
YSL_BEGIN_NAMESPACE(Pattern)
YSL_END_NAMESPACE(Pattern)

//! \brief 图形处理。
YSL_BEGIN_NAMESPACE(Drawing)

	using platform::PixelType;
	using platform::BitmapPtr;
	using platform::ConstBitmapPtr;
	using platform::ScreenBufferType;
	using platform::Color;
	namespace ColorSpace = platform::ColorSpace;

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

	using namespace CHRLib;

YSL_END_NAMESPACE(Text)

	using Text::ucs4_t;
	using Text::ucs2_t;
	using Text::ucsint_t;

	//! \brief 运行时平台。
	namespace DS
	{
		using namespace platform_ex;
	}

YSL_END

#endif

