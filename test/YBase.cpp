/*
	© 2014-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file test.cpp
\ingroup Test
\brief YBase 测试。
\version r685
\author FrankHB <frankhb1989@gmail.com>
\since build 519
\par 创建时间:
	2014-07-10 05:09:57 +0800
\par 修改时间:
	2019-06-12 14:14 +0800
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
#include <ystdex/tstring_view.hpp>
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

using namespace ystdex;
using namespace ytest;
//! \since build 833
using std::vector;

//! \since build 664
struct cwam
{
	// NOTE: Class with array member, used to test member prvalue before CWG616
	//	is resoved, or member xvalue after CWG616 is resolved.
	char foo[42];

	cwam()
		: foo()
	{
		yunseq(foo[0] = 'B', foo[sizeof(foo) - 1] = 'E');
	}
};

//! \since build 660
namespace vbase_test
{

template<class _type>
yconstfn bool
v() ynothrow
{
	return has_nonempty_virtual_base<_type>();
}

template<class _type1, class _type2>
yconstfn bool
cv() ynothrow
{
	return have_common_nonempty_virtual_base<_type1, _type2>();
}

struct eb
{};

struct neb
{
	int i;
};

struct ed : eb
{};

struct ned : neb
{};

struct ved : virtual eb
{};

struct vned : virtual neb
{};

struct vned2 : virtual neb
{};

} // namespace vbase_test;

//! \since build 666
namespace vseq_test
{

using namespace vseq;
static_assert(at_t<integer_sequence<int, 3>, 0>() == 3,
	"Integer sequence check failed.");
static_assert(at_t<make_peano_sequence_t<int, -3, 4>, 2>() == -1,
	"Peano sequence check failed.");

//! \since build 833
using namespace std;
using test_t = tuple<int, int, void, void, int>;
static_assert(is_same<front_t<test_t>, int>(), "");
static_assert(is_same<back_t<test_t>, int>(), "");
static_assert(is_same<pop_back_t<test_t>, tuple<int, int, void, void>>(), "");
static_assert(is_same<pop_front_t<test_t>, tuple<int, void, void, int>>(), "");
static_assert(is_same<deduplicate_t<test_t>, tuple<void, int>>(), "");
static_assert(is_same<unique_t<test_t>, tuple<int, void, int>>(), "");

static_assert(is_same<concat_t<index_sequence<1, 2>,
	index_sequence<2, 3>>, index_sequence<1, 2, 2, 3>>(), "");
static_assert(is_same<unique_t<index_sequence<>>, index_sequence<>>(), "");
static_assert(is_same<unique_t<index_sequence<1>>, index_sequence<1>>(),
	"");
static_assert(at_t<index_sequence<1, 2, 3>, 0>() == 1, "");
static_assert(at_t<index_sequence<1, 2, 3>, 1>() == 2, "");
static_assert(at_t<index_sequence<1, 2, 3>, 2>() == 3, "");
static_assert(at_t<index_sequence<1, 2>, 0>() == 1, "");
static_assert(at_t<index_sequence<1, 2>, 1>() == 2, "");
static_assert(at_t<index_sequence<1>, 0>() == 1, "");
static_assert(is_same<unique_t<index_sequence<1, 1>>, index_sequence<1>>(),
	"");
static_assert(is_same<unique_t<index_sequence<1, 2, 2, 3>>,
	index_sequence<1, 2, 3>>(), "");

static_assert(is_same<vseq::vec_subtract_t<integer_sequence<long, 1, 2, 3>,
	integer_sequence<long, -1, 4, 7>>,
	integer_sequence<long, 2, -2, -4>>(), "");

template<typename... S1, typename... S2, size_t... SD, size_t... SS>
void
copy_tail_impl(tuple<S1...>& d, const tuple<S2...>& s,
	index_sequence<SD...>, index_sequence<SS...>)
{
	yunseq((get<SD>(d) = get<SS>(s))...);
}

template<typename... S1, typename... S2>
void
copy_tail(const tuple<S2...>& s, tuple<S1...>& d)
{
	copy_tail_impl(d, s, index_sequence_for<S1...>(),
		make_peano_sequence_t<size_t, sizeof...(S2) - sizeof...(S1),
		sizeof...(S1)>());
}

using A = tuple<int, double, float>;
using C = tuple<float>;

float
f()
{
	A a{1, 2, 3};
	C c;

	copy_tail(a, c);
	return get<0>(c);
}

} // namespace vseq_test;

//! \since build 649
namespace memory_test
{

struct t1
{
	const t1*
	operator&() const ynothrow
	{
		return {};
	}
};

struct t2
{};

const t2*
operator&(const t2&) ynothrow
{
	return {};
}

static_assert(!ystdex::has_overloaded_addressof<void>(), "Type check failed.");
// NOTE: Use '&x' to suppress Clang++ warning:
//	[-Wunneeded-internal-declaration].
//! \since build 664
const void* volatile unused = &t2();

template<class _type>
bool
t_constfn() ynothrow
{
	static_assert(ystdex::has_overloaded_addressof<_type>(),
		"Type check failed.");
	_type x;

	return ystdex::addressof(x) == std::addressof(x);
}

} // namespace memory_test;

//! \since build 851
namespace function_test
{

struct C
{
	C() = default;
	C(function<C(int)>);
	C(function<C(int, int)>);
	template<class _type>
	C
	operator()(_type&&);
	template<class _type>
	C
	operator()(_type&&, _type&&);
};

} // namespace function_test;

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

	using namespace std;
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
	using std::cout;
	using std::endl;
	using std::make_pair;
	using std::make_tuple;
	using std::numeric_limits;
	using std::deque;
	using std::list;
	const auto make_guard([](const string& subject){
		return group_guard(subject, [](group_guard& printer){
			cout << "CASES: " << printer.subject << ':' << endl;
		}, [](group_guard& printer){
			show_result(cout, printer.subject, printer.pass_n, printer.fail_n);
		});
	});
	size_t pass_n(0), fail_n(0), case_n(0);
	const auto pass([&]{
		yunseq(++pass_n, cout << '#' << ++case_n << ": PASS." << endl);
	});
	const auto fail([&]{
		yunseq(++fail_n, cout << '#' << ++case_n << ": FAIL." << endl);
	});

	// TODO: Check stream error.
	// 30 cases covering: ystdex::begin, ystdex::end, ystdex::cbegin,
	//	ystdex::cend, ystdex::rbegin, ystdex::rend, ystdex::crbegin,
	//	ystdex::crend, ystdex::size, ystdex::data.
	seq_apply(make_guard("YStandard.Range").get(pass, fail),
		// 3 cases covering: ystdex::begin.
		'a' == *begin({'a', 'b', 'c', char(), 'd', 'e'}),
		u'a' == *begin(u"abc\0de"),
		'B' == *begin(cwam().foo),
		// 3 cases covering: ystdex::end.
		'e' == *(end({'a', 'b', 'c', char(), 'd', 'e'}) - 1),
		u'e' == *(end(u"abc\0de") - 2),
		'B' == *(end(cwam().foo) - sizeof(cwam::foo)),
		// 3 cases covering: ystdex::cbegin.
		'a' == *cbegin({'a', 'b', 'c', char(), 'd', 'e'}),
		u'a' == *cbegin(u"abc\0de"),
		'B' == *cbegin(cwam().foo),
		// 3 cases covering: ystdex::cend.
		'e' == *(cend({'a', 'b', 'c', char(), 'd', 'e'}) - 1),
		u'e' == *(cend(u"abc\0de") - 2),
		'B' == *(cend(cwam().foo) - sizeof(cwam::foo)),
		// 3 cases covering: ystdex::rbegin.
		'e' == *rbegin({'a', 'b', 'c', char(), 'd', 'e'}),
		u'\0' == *rbegin(u"abc\0de"),
		'E' == *rbegin(cwam().foo),
		// 3 cases covering: ystdex::rend.
		'a' == *(rend({'a', 'b', 'c', char(), 'd', 'e'}) - 1),
		u'a' == *(rend(u"abc\0de") - 1),
		'E' == *(rend(cwam().foo) - sizeof(cwam::foo)),
		// 3 cases covering: ystdex::crbegin.
		'e' == *crbegin({'a', 'b', 'c', char(), 'd', 'e'}),
		u'e' == *(crbegin(u"abc\0de") + 1),
		'E' == *crbegin(cwam().foo),
		// 3 cases covering: ystdex::crend.
		'a' == *(crend({'a', 'b', 'c', char(), 'd', 'e'}) - 1),
		u'a' == *(crend(u"abc\0de") - 1),
		'E' == *(crend(cwam().foo) - sizeof(cwam::foo)),
		// 3 cases covering: ystdex::size.
		6 == size({'a', 'b', 'c', char(), 'd', 'e'}),
		7 == size(u"abc\0de"),
		sizeof(cwam::foo) == size(cwam().foo),
		// 3 cases covering: ystdex::data.
		'a' == *data({'a', 'b', 'c', char(), 'd', 'e'}),
		u'a' == *data(u"abc\0de"),
		'B' == *data(cwam().foo)
	);
	// 11 cases covering: ystdex::has_nonempty_virtual_base,
	//	ystdex::have_common_nonempty_virtual_base.
	seq_apply(make_guard("YStandard.TypeOperation").get(pass, fail),
		// 6 cases covering: ystdex::has_nonempty_virtual_base.
		!vbase_test::v<vbase_test::eb>(),
		!vbase_test::v<vbase_test::neb>(),
		!vbase_test::v<vbase_test::ed>(),
		!vbase_test::v<vbase_test::ned>(),
		!vbase_test::v<vbase_test::ved>(),
		vbase_test::v<vbase_test::vned>(),
		// 5 cases covering: ystdex::have_common_nonempty_virtual_base.
		!vbase_test::cv<vbase_test::neb, vbase_test::neb>(),
		!vbase_test::cv<vbase_test::ed, vbase_test::ved>(),
		!vbase_test::cv<vbase_test::ved, vbase_test::ved>(),
		!vbase_test::cv<vbase_test::ved, vbase_test::vned>(),
		vbase_test::cv<vbase_test::vned, vbase_test::vned2>()
	);
	// 1 case covering: ystdex::index_sequence, ystdex::make_peano_sequence.
	seq_apply(make_guard("YStandard.IntegerSequence").get(pass, fail),
		std::fabs(vseq_test::f() - 3.F) < numeric_limits<float>::epsilon()
	);
	// 2 cases covering: ystdex::addressof.
	seq_apply(make_guard("YStandard.AddressOf").get(pass, fail),
		memory_test::t_constfn<memory_test::t1>(),
		memory_test::t_constfn<memory_test::t2>()
	);
	// 1 case covering: ystdex::function.
	seq_apply(make_guard("YStandard.Function").get(pass, fail),
		[]{
			// NOTE: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65760.
			const auto c = function_test::C();

			yunused(c);
		}
	);
	// 4 cases covering: ystdex::apply, ystdex::compose, ystdex::make_expanded.
	seq_apply(make_guard("YStandard.Functional").get(pass, fail),
		// 1 case covering: ystdex::apply.
		abs(7.0 - apply([](int i, float f, double d){
			return i + double(f) + d;
		}, make_tuple(1, 2.0F, 4.0))) < numeric_limits<double>::epsilon(),
		// 2 cases covering: ystdex::compose.
		abs(0.5 - compose(static_cast<double(&)(double)>(sin),
			static_cast<double(&)(double)>(asin))(0.5))
			< numeric_limits<double>::epsilon(),
		expect(145, []{
			return compose_n([](int x, int y){
				return x + y + 1;
			}, [](int x){
				return x * 2;
			}, [](int x){
				return x - 1;
			})(42, 32);
		}),
		expect(make_pair(8, 2), []{
			using namespace std;
			using ftype = int(random_access_iterator_tag, double);
			const int i(-1);

			return make_pair((make_expanded<ftype>(
				[](input_iterator_tag, short x){
					return x * 2;
				}))(random_access_iterator_tag(), 4), make_expanded<ftype>(
				[=](forward_iterator_tag){
					return i + 3;
				})(random_access_iterator_tag(), 0));
		})
	);
	// 2 cases covering: ystdex::transposed_iterator, ystdex::make_transposed.
	seq_apply(make_guard("YStandard.Iterator").get(pass, fail),
		expect(vector<int>{0, 4, 1, 5, 2, 6, 3, 7}, []{
			vector<int> v{0, 1, 2, 3, 4, 5, 6, 7}, r;

			copy(make_transposed(v.begin(), 4U, 2U, 0U),
				make_transposed(v.begin(), 4U, 2U, 8U), back_inserter(r));
			return r;
		}),
		expect(deque<int>{0, 1, 2, 3, 4, 5, 6, 7}, []{
			deque<int> d{0, 4, 1, 5, 2, 6, 3, 7}, r;

			copy(make_transposed(d.begin(), 2U, 4U) - ptrdiff_t(d.size()),
				make_transposed(d.begin(), 2U, 4U), back_inserter(r));
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
#if __cpp_constexpr >= 200704
		1 == sizeof(char[ystdex::min(1, 2)]),
#else
		1 == ystdex::min(1, 2),
#endif
		abs(2.F - ystdex::max(1.F, 2.F)) < numeric_limits<float>::epsilon(),
		4U == integral_constant<unsigned, ystdex::min({5U, 6U, 7U, 8U, 4U})>(),
		4 == ystdex::max({4, -5, -6, -7})
	);
	// 32 cases covering: ystdex::string_begin, ystdex::string_end,
	//	ystdex::string_length, ystdex::begins_with, ystdex::ends_with,
	//	ystdex::erase_left, ystdex::erase_right, ystdex::ltrim, ystdex::rtrim,
	//	ystdex::trim.
	seq_apply(make_guard("YStandard.String").get(pass, fail),
		'a' == *string_begin({'a', 'b', 'c', char(), 'd', 'e'}),
		u'a' == *string_begin(u"abc\0de"),
		'e' == *(string_end({'a', 'b', 'c', char(), 'd', 'e'}) - 1),
		u'c' == *(string_end(u"abc\0de") - 1),
		0 == string_length(u8""),
		5 == string_length("abcde"),
		3 == string_length(u"abc\0de"),
		6 == string_length({'a', 'b', 'c', char(), 'd', 'e'}),
		3 == string_length(wstring(L"abc\0de")),
		6 == string_length(u16string{'a', 'b', 'c', 0, 'd', 'e'}),
		true == begins_with("test_string", "test"),
		true == begins_with("test string", string("test")),
		false == begins_with("test_string", "tests"),
		false == begins_with("test_string", "test_string_too_long"),
		true == ends_with("test_string", "string"),
		false == ends_with("test_string", "strnig"),
		"de" == erase_left(3, string("abcde")),
		"cde" == erase_left(string("abcde"), "c"),
		"abcd" == erase_right(3, string("abcde")),
		"ab" == erase_right(string("abcde"), 'b'),
		string("de") == erase_left(3, string("abcde")),
		string("cde") == string(erase_left(string_view("abcde"), "c")),
		string("abcd") == erase_right(3, string("abcde")),
		string("ab") == string(erase_right(string_view("abcde"), 'b')),
		ltrim(string("aabcbaa"), 'a') == "bcbaa",
		string(ltrim(string_view("aabcbaa"), 'a')) == "bcbaa",
		rtrim(string("aabcbaa"), 'a') == "aabcb",
		string(rtrim(string_view("aabcbaa"), 'a')) == "aabcb",
		trim(string("aabcbaa"), 'a') == "bcb",
		trim(string("\v\nabcde\t\n")) == "abcde",
		string(trim(string_view("aabcbaa"), 'a')) == "bcb",
		trim(string_view("\v\nabcde\t\n")) == "abcde"
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
			using std::initializer_list;
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
	// 4 cases covering: ystdex::string_view.
	seq_apply(make_guard("YStandard.StringView").get(pass, fail),
		string_view("????") == string(4, '?'),
		string_view("///a/b/c").find_first_not_of('/') == 3,
		string_view("///a/b/c/").find_last_not_of('/') == 7,
		expect(true, []{
			static_assert(is_same<string_traits<string_view>::string_type,
				string_view>(), "");
			static_assert(is_same<string_traits<string_view>::value_type,
				char>(), "");
			const string str("Hello!");
			const string_view sv(str);

			return sv.find("el") == 1 && sv.find("loo") == string_view::npos
				&& sv.rfind('l') == 3 && sv.find_first_of("abcde") == 1
				&& sv.find_first_not_of("Hel!") == 4;
		})
	);
	// 4 cases covering: ystdex::tstring_view.
	seq_apply(make_guard("YStandard.TStringView").get(pass, fail),
		tstring_view("????") == string(4, '?'),
		tstring_view("///a/b/c").find_first_not_of('/') == 3,
		tstring_view("///a/b/c/").find_last_not_of('/') == 7,
		expect(true, []{
			static_assert(is_same<string_traits<tstring_view>::string_type,
				tstring_view>(), "");
			static_assert(is_same<string_traits<tstring_view>::value_type,
				char>(), "");
			const string str("Hello!");
			const tstring_view sv(str);

			return sv.find("el") == 1 && sv.find("loo") == string_view::npos
				&& sv.rfind('l') == 3 && sv.find_first_of("abcde") == 1
				&& sv.find_first_not_of("Hel!") == 4;
		})
	);
	// 1 case covering: mixin interface.
	seq_apply(make_guard("YStandard.Mixin").get(pass, fail),
		expect(make_pair(string("e"), boxed_value<int>(3)), []{
			using namespace std;
			using ystdex::string;
			using int_exception = wrap_mixin_t<runtime_error, int>;
			using exception_tuple = typename int_exception::tuple_type;
			using type_0 = tuple_element_t<0, exception_tuple>;
			using type_1 = tuple_element_t<1, exception_tuple>;
			static_assert(is_same<type_0, runtime_error>(), "");
			static_assert(is_same<type_1, ystdex::boxed_value<int>>(), "");
			static_assert(is_constructible<type_0, const char*>(), "");
			static_assert(is_constructible<type_1, double>(), "");

			const auto et(int_exception(forward_as_tuple(runtime_error("e"), 3))
				.to_tuple());
			return make_pair(string(get<0>(et).what()), get<1>(et));
		})
	);
	// 3 cases covering: ystdex::bitseg_iterator.
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

