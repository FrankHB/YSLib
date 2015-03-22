/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file test.cpp
\ingroup Test
\brief YBase 测试。
\version r256
\author FrankHB <frankhb1989@gmail.com>
\since build 519
\par 创建时间:
	2014-07-10 05:09:57 +0800
\par 修改时间:
	2015-03-20 09:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Test::YBase
*/


#include <ytest/test.h>
#include <ystdex/functional.hpp>
#include <vector>
#include <list>
#include <deque>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ystdex/algorithm.hpp>
#include <ystdex/string.hpp>
#include <ystdex/container.hpp>
#include <ystdex/mixin.hpp>
#include <ystdex/bitseg.hpp>

namespace
{

void
show_result(std::ostream& out, const std::string& name, size_t pass_n,
	size_t fail_n)
{
	out << name << ": " << pass_n << '/' << pass_n + fail_n << '.'
		<< std::endl;
}

using namespace std;
using namespace placeholders;
using namespace ystdex;
using namespace ytest;

//! \since build 549
namespace bitseg_test
{

template<size_t _vN, bool _bEndian = false>
static bool
expect(const string& str, vector<byte>&& seq)
{
	using bit = bitseg_iterator<_vN, _bEndian>;

	if(seq.empty())
		return str.empty();

	const bit e(&seq[0] + seq.size());
	ostringstream oss;

	oss << hex;
	for(bit b(&seq[0]); b != e; ++b)
		oss << int(*b);
	return str == oss.str();
}

} // namespace bitseg_test;

} // unnamed namespace;


int
main()
{
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
	// 4 cases covering: ystdex::apply, ystdex::compose, ystdex::make_expanded.
	seq_apply(make_guard("YStandard.Functional").get(pass, fail),
		// 1 case covering: ystdex::apply.
		7.0 == apply([](int i, float f, double d){
			return i + f + d;
		}, make_tuple(1, 2.0F, 4.0)),
		// 2 cases covering: ystdex::compose.
		0.5 == compose(static_cast<double(&)(double)>(sin),
			static_cast<double(&)(double)>(asin))(0.5),
		expect(145, []{
			return compose([](int x, int y){
				return x + y + 1;
			}, [](int x){
				return x * 2;
			}, [](int x){
				return x - 1;
			})(42, 32);
		}),
		expect(make_pair(8, 2), []{
			using ftype = int(random_access_iterator_tag&, double);
			const int i(-1);

			return make_pair((make_expanded<ftype>(
				[](input_iterator_tag&, short x){
					return x * 2;
				}))(random_access_iterator_tag(), 4), make_expanded<ftype>(
				[=](forward_iterator_tag&){
					return i + 3;
				})(random_access_iterator_tag(), 0));
		})
	);
	// 2 cases covering: ystdex::transposed_iterator, ystdex::make_transposed.
	seq_apply(make_guard("YStandard.Iterator").get(pass, fail),
		expect(vector<int>{0, 4, 1, 5, 2, 6, 3, 7}, []{
			vector<int> v{0, 1, 2, 3, 4, 5, 6, 7}, r;

			copy(make_transposed(v.begin(), 4, 2, 0),
				make_transposed(v.begin(), 4, 2, 8), back_inserter(r));
			return r;
		}),
		expect(deque<int>{0, 1, 2, 3, 4, 5, 6, 7}, []{
			deque<int> d{0, 4, 1, 5, 2, 6, 3, 7}, r;

			copy(make_transposed(d.begin(), 2, 4) - d.size(),
				make_transposed(d.begin(), 2, 4), back_inserter(r));
			return r;
		})
	);
	// 6 cases covering: ystdex::transform_n, ystdex::min, ystdex::max.
	seq_apply(make_guard("YStandard.Algorithm").get(pass, fail),
		expect(vector<int>{-2, 5, 16, 16}, []{
			vector<int> a{1, 2, 3, 4}, b{5, 4, 6, 5}, c{7, 3, 2, 4}, r(4);

			transform_n([](int x, int y, int z)->int{
				return x * y - z;
			}, r.begin(), 4, a.cbegin(), b.cbegin(), c.cbegin());
			return r;
		}),
		expect(list<int>{21, 18, 27, 24, 41}, []{
			list<int> a{1, 2, 3, 4, 5}, b{5, 4, 6, 5, 9}, r(5);

			transform_n([](int x, int y)->int{
				return x + y * 4;
			}, r.begin(), 5, a.cbegin(), b.cbegin());
			return r;
		}),
#if YB_HAS_CONSTEXPR
		1 == sizeof(char[ystdex::min(1, 2)]),
#else
		1 == ystdex::min(1, 2),
#endif
		2.F == ystdex::max(1.F, 2.F),
		4U == integral_constant<unsigned,
			ystdex::min({5U, 6U, 7U, 8U, 4U})>::value,
		4 == ystdex::max({4, -5, -6, -7})
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
	// 4 cases covering: ystdex::range_size.
	seq_apply(make_guard("YStandard.Container").get(pass, fail),
		3 == range_size({1, 2, 3}),
		4 == range_size(vector<int>(4)),
		expect(size_t(5), []{
			int a[5];

			return range_size(a);
		}),
		expect(size_t(6), []{
			struct no_size_function : initializer_list<int>
			{
				no_size_function(initializer_list<int> il)
					: initializer_list<int>(il)
				{}

				using initializer_list::begin;
				using initializer_list::end;
			};

			// XXX: How to detect %std::distance used?
			return range_size(no_size_function({3, 4, 5, 6, 7, 8}));
		})
	);
	// 1 case covering: mixin interface.
	seq_apply(make_guard("YStandard.Mixin").get(pass, fail),
		expect(make_pair(string("e"), boxed_value<int>(3)), []{
			using int_exception = wrap_mixin_t<runtime_error, int>;
			using exception_tuple = typename int_exception::tuple_type;
			using type_0 = tuple_element_t<0, exception_tuple>;
			using type_1 = tuple_element_t<1, exception_tuple>;
			static_assert(is_same<type_0, runtime_error>::value, "");
			static_assert(is_same<type_1, ystdex::boxed_value<int>>::value, "");
			static_assert(is_constructible<type_0, const char*>::value, "");
			static_assert(is_constructible<type_1, double>::value, "");

			const auto et(int_exception(forward_as_tuple(runtime_error("e"), 3))
				.to_tuple());
			return make_pair(string(get<0>(et).what()), get<1>(et));
		})
	);
	// 3 case covering: ystdex::bitseg_iterator.
	seq_apply(make_guard("YStandard.BitSegment").get(pass, fail),
		bitseg_test::expect<1>("1000000001000000110000001010000011101000"
			"000000110000111111111111", {1, 2, 3, 5, 0x17, 0xC0, 0xF0, 0xFF}),
		bitseg_test::expect<1, true>("0000000100000010000000110000010100010111"
			"110000001111000011111111", {1, 2, 3, 5, 0x17, 0xC0, 0xF0, 0xFF}),
		bitseg_test::expect<2>("10002000300011003110000300333333",
			{1, 2, 3, 5, 0x17, 0xC0, 0xF0, 0xFF}),
		bitseg_test::expect<2, true>("00010002000300110113300033003333",
			{1, 2, 3, 5, 0x17, 0xC0, 0xF0, 0xFF}),
		bitseg_test::expect<4>("10203050710c0fff",
			{1, 2, 3, 5, 0x17, 0xC0, 0xF0, 0xFF}),
		bitseg_test::expect<4, true>("0102030517c0f0ff",
			{1, 2, 3, 5, 0x17, 0xC0, 0xF0, 0xFF})
	);
	show_result(cout, "ALL", pass_n, fail_n);
}

