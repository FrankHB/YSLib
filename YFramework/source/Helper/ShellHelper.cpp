﻿/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShellHelper.cpp
\ingroup Helper
\brief Shell 助手模块。
\version r1373;
\author FrankHB<frankhb1989@gmail.com>
\since build 278 。
\par 创建时间:
	2010-04-04 13:42:15 +0800;
\par 修改时间:
	2012-05-15 08:56 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::ShellHelper;
*/


#include "Helper/ShellHelper.h"
#include <cstdio> // for sprintf;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

string
to_string(const Size& s)
{
	using YSL_ to_string;

	return "(" + to_string(s.Width) + ", " + to_string(s.Height) + ')';
}
string
to_string(const Rect& r)
{
	using YSL_ to_string;

	return "(" + to_string(r.X) + ", " + to_string(r.Y) + ", "
		+ to_string(r.Width) + ", " + to_string(r.Height) + ')';
}

YSL_END_NAMESPACE(Drawing)

void
RemoveGlobalTasks(Shell& shl)
{
	auto& app(FetchGlobalInstance());

	app.Queue.Remove(&shl, app.UIResponseLimit);
}


namespace
{

//! \since build 264 。
inline void
snftime(char* buf, size_t n, const std::tm& tm,
	const char* format = DefaultTimeFormat)
{
	// FIXME: correct behavior for time with BC date(i.e. tm_year < -1900);
	// FIXME: snprintf shall be a member of namespace std;
	///*std*/::snprintf(buf, n, format, tm.tm_year + 1900,
	//	tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	// FIXME: use std::snprintf;
	std::sprintf(buf, format, tm.tm_year + 1900,
		tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

} // unnamed namespace;


const char*
TranslateTime(const std::tm& tm, const char* format)
{
	static char str[80];

	// NOTE: 'std::strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", &tm)'
	//	is correct but make the object file too large;
	snftime(str, 80, tm, format);
	return str;
}
const char*
TranslateTime(const std::time_t& t, const char* format) ythrow(GeneralEvent)
{
	auto p(std::localtime(&t));

	if(YCL_UNLIKELY(!p))
		throw GeneralEvent("Get broken-down time object failed"
			" @ TranslateTime#2;");
	return TranslateTime(*p, format);
}


shared_ptr<Components::TextList::ListType>
FetchFontFamilyNames()
{
	const auto& mFamilies(FetchGlobalInstance().GetFontCache()
		.GetFamilyIndices());

	// TODO: use g++ 4.7 later;
//	return make_shared<TextList::ListType>(mFamilies.cbegin()
//		| ystdex::get_key, mFamilies.cend() | ystdex::get_key);
	return share_raw(new Components::TextList::ListType(mFamilies.cbegin()
		| ystdex::get_key, mFamilies.cend() | ystdex::get_key));
}


FPSCounter::FPSCounter(u64 s)
	: last_tick(GetHighResolutionTicks()), now_tick(), refresh_count(1),
	MinimalInterval(s)
{}

u32
FPSCounter::Refresh()
{
	const u64 tmp_tick(GetHighResolutionTicks());

	if(YCL_UNLIKELY(last_tick + MinimalInterval < tmp_tick))
	{
		last_tick = now_tick;

		const u32 r(1000000000000ULL * refresh_count
			/ ((now_tick = tmp_tick) - last_tick));

		refresh_count = 1;
		return r;
	}
	else
		++refresh_count;
	return 0;
}

YSL_BEGIN_NAMESPACE(Components)

void
SwitchVisible(IWidget& wgt)
{
	SetVisibleOf(wgt, !IsVisible(wgt));
	Invalidate(wgt);
}

YSL_END_NAMESPACE(Components)

YSL_END;

