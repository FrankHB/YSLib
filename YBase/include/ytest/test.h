/*
	© 2014 FrankHB.

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

/*!	\file test.h
\ingroup YTest
\brief 基础测试工具。
\version r109
\author FrankHB <frankhb1989@gmail.com>
\since build 519
\par 创建时间:
	2014-07-17 03:56:17 +0800
\par 修改时间:
	2014-10-17 20:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YTest::Test
*/


#ifndef YB_INC_ytest_test_h_
#define YB_INC_ytest_test_h_ 1

#include "../ydef.h"
#include <string>
#include <functional>

/*!
\since build 319
\brief YFramework 基础测试库命名空间。
*/
namespace ytest
{

using ystdex::size_t;

/*!
\brief 判断可调用对象的应用求值结果是否符合预期值。
\since build 547
*/
template<typename _type, typename _func, typename... _tParams>
bool
expect(const _type& result, _func&& f, _tParams&&... args)
{
	return yforward(f)(yforward(args)...) == result;
}


/*!
\brief 测试组 RAII 辅助对象。
\since build 519
*/
class YB_API group_guard
{
public:
	std::string subject;
	size_t pass_n = 0;
	size_t fail_n = 0;
	std::function<void(group_guard&)> on_end{};

	//! \brief 构造：提供名称。
	group_guard(const std::string&);
	//! \brief 构造：提供名称并注册回调，若起始回调非空时则调用。
	group_guard(const std::string&, std::function<void(group_guard&)>,
		std::function<void(group_guard&)>);
	group_guard(group_guard&&) = default;
	/*!
	\brief 析构：调用 on_end 。
	\note 无异常抛出：捕获所有异常。
	*/
	~group_guard();

	template<typename _fPass, typename _fFail>
	std::function<void(bool)>
	get(_fPass pass, _fFail fail)
	{
		return [=](bool b){
			if(b)
			{
				++pass_n;
				pass();
			}
			else
			{
				++fail_n;
				fail();
			}
		};
	}
};

} // namespace ytest;

#endif

