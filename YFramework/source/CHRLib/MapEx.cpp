/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file MapEx.cpp
\ingroup CHRLib
\brief 附加编码映射。
\version r1041;
\author FrankHB<frankhb1989@gmail.com>
\since build 324 。
\par 创建时间:
	2012-07-09 09:04:43 +0800;
\par 修改时间:
	2012-07-10 16:39 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	CHRLib::MappingEx;
*/


#include "CHRLib/MapEx.h"

CHRLIB_BEGIN

using namespace CharSet;

#if !CHRLIB_NODYNAMIC_MAPPING

byte* cp17;
byte* cp113;
byte* cp2026;

#endif

CHRLIB_END

