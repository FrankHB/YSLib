/*
	© 2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ctime.h
\ingroup YStandardEx
\brief ISO C 日期和时间接口扩展。
\version r57
\author FrankHB <frankhb1989@gmail.com>
\since build 513
\par 创建时间:
	2016-06-23 09:48:32 +0800
\par 修改时间:
	2015-06-23 09:52 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CTime
*/


#ifndef YB_INC_ystdex_ctime_h_
#define YB_INC_ystdex_ctime_h_ 1

#include "../ydef.h" // for yconstfn;
#include <ctime> // for std::tm;

namespace ystdex
{

//! \since build 704
//@{
//! \brief 判断 std::tm 的日期数据成员范围是否有效。
yconstfn bool
is_date_range_valid(const std::tm& t)
{
	return !(t.tm_mon < 0 || 12 < t.tm_mon || t.tm_mday < 1 || 31 < t.tm_mday);
}

//! \brief 判断 std::tm 的时间数据成员范围是否有效。
yconstfn bool
is_time_no_leap_valid(const std::tm& t)
{
	return !(t.tm_hour < 0 || 23 < t.tm_hour || t.tm_hour < 0 || 59 < t.tm_min
		|| t.tm_sec < 0 || 59 < t.tm_min);
}
//@}

} // namespace ystdex;

#endif

