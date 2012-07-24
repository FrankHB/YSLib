/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YTest YBase Test Library
\ingroup YBase
\brief YFramework 基础测试库。
*/

/*!	\file timing.h
\ingroup YTest
\brief 运行计时测试工具。
\version r1262;
\author FrankHB<frankhb1989@gmail.com>
\since build 308 。
\par 创建时间:
	2012-06-23 20:01:09 +0800;
\par 修改时间:
	2012-07-23 20:24 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YTest::Timing;
*/


#ifndef YB_INC_YTEST_TIMING_HPP_
#define YB_INC_YTEST_TIMING_HPP_

#include "../ydef.h"
#include <ctime>

/*!
\since build 319 。
\brief YFramework 基础测试库命名空间。
*/
namespace ytest
{

/*!
\since build 319 。
\brief 计时测试工具命名空间。
*/
namespace timing
{

/*!
\brief 测试指定函数执行一次的时间。
\note 使用 _fNow 函数指定当前时刻进行计时。
\since build 327 。
*/
template<typename _fNow, typename _fCallable, typename... _tParams>
inline auto
once(_fNow now, _fCallable&& f, _tParams&&... args) -> decltype(now() - now())
{
	const auto cl(now());

	yforward(f)(yforward(args)...);
	return now() - cl;
}

/*!
\brief 测试指定函数执行一次的时间。
\note 使用 std::clock() 计时。
\since build 327 。
*/
template<typename _fCallable, typename... _tParams>
inline double
once_c(_fCallable&& f, _tParams&&... args)
{
	const std::clock_t cl(std::clock());

	yforward(f)(yforward(args)...);
	return double(clock() - cl) / CLOCKS_PER_SEC;
}


/*!
\brief 测试指定函数执行若干次的总时间。
\note 使用 _tClock 时钟类计时。
\pre _tClock 满足 TrivialClock 需求。
\since build 327 。
*/
template<class _fNow, typename _fCallable, typename... _tParams>
inline auto
total(size_t n, _fNow now, _fCallable&& f, _tParams&&... args)
	-> decltype(now() - now())
{
	const auto cl(now());

	for(size_t i(0); i != n; ++i)
		yforward(f)(yforward(args)...);
	return now() - cl;
}

/*!
\brief 测试指定函数执行若干次的总时间。
\note 使用 std::clock() 计时。
\since build 327 。
*/
template<typename _fCallable, typename... _tParams>
inline double
total_c(size_t n, _fCallable&& f, _tParams&&... args)
{
	const std::clock_t cl(std::clock());

	for(size_t i(0); i != n; ++i)
		yforward(f)(yforward(args)...);
	return double(clock() - cl) / CLOCKS_PER_SEC;
}


/*!
\brief 测试指定函数执行若干次的平均时间。
\since build 319 。
*/
template<typename... _tParams>
inline auto
average(size_t n, _tParams&&... args)
	-> decltype(timing::total(n, yforward(args)...) / n)
{
	return timing::total(n, yforward(args)...) / n;
}

} // namespace timing;

} // namespace ytest;

#endif

