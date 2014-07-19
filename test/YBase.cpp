/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file test.cpp
\ingroup Test
\brief YBase 测试。
\version r102
\author FrankHB <frankhb1989@gmail.com>
\since build 519
\par 创建时间:
	2014-07-10 05:09:57 +0800
\par 修改时间:
	2014-07-17 06:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Test::Test
*/


#include <ytest/test.h>
#include <ystdex/functional.hpp>
#include <vector>
#include <list>
#include <iostream>
#include <ystdex/algorithm.hpp>
#include <ystdex/string.hpp>

void
show_result(std::ostream& out, const std::string& name, size_t pass_n,
	size_t fail_n)
{
	out << name << ": " << pass_n << '/' << pass_n + fail_n << '.'
		<< std::endl;
}

int
main()
{
	using namespace std;
	using namespace placeholders;
	using namespace ystdex;
	using namespace ytest;
	const auto make_guard([](const string& subject){
		return group_guard(subject, [](group_guard& printer){
			cout << "CASES: " << printer.subject << ':' << endl;
		}, [](group_guard& printer){
			show_result(cout, printer.subject, printer.pass_n, printer.fail_n);
		});
	});
	size_t pass_n(0), fail_n(0);
	const auto pass([&]{
		yunseq(++pass_n, cout << "PASS." << endl);
	});
	const auto fail([&]{
		yunseq(++fail_n, cout << "FAIL." << endl);
	});

	// TODO: Check stream error.
	// 2 cases covering: ystdex::transform_n.
	seq_apply(make_guard("YStandard.Algorithm").get(pass, fail),
		expect(vector<int>{-2, 5, 16, 16}, []{
			std::vector<int> a{1,2,3,4}, b{5,4,6,5}, c{7,3,2,4}, r(4);

			transform_n([](int a, int b, int c)->int{
				return a * b - c;
			}, r.begin(), 4, a.cbegin(), b.cbegin(), c.cbegin());
			return r;
		}),
		expect(list<int>{21, 18, 27, 24, 41}, []{
			std::list<int> a{1,2,3,4,5}, b{5,4,6,5,9}, r(5);

			transform_n([](int a, int b)->int{
				return a + b * 4;
			}, r.begin(), 5, a.cbegin(), b.cbegin());
			return r;
		})
	);
	// 11 cases covering: ystdex::string_length, ystdex::begins_with,
	//	ystdex::ends_with.
	seq_apply(make_guard("YStandard.String").get(pass, fail),
		0 == string_length(u8""),
		5 == string_length("abcde"),
		6 == string_length(u"abc\0de"),
		3 == string_length(wstring(L"abc\0de")),
		6 == string_length(u16string{'a', 'b', 'c', 0, 'd', 'e'}),
		true == begins_with("test_string", "test"),
		true == begins_with("test string", string("test")),
		false == begins_with("test_string", "tests"),
		false == begins_with("test_string", "test_string_too_long"),
		true == ends_with("test_string", "string"),
		false == ends_with("test_string", "strnig")
	);
	show_result(cout, "ALL", pass_n, fail_n);
}

