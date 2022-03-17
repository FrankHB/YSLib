/*
	© 2010-2016, 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Container.h
\ingroup YCLib
\brief 容器、拟容器和适配器。
\version r1211
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2010-10-09 09:25:26 +0800
\par 修改时间:
	2022-03-11 23:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Container
*/


#ifndef YCL_INC_Container_h_
#define YCL_INC_Container_h_ 1

#include "YModules.h"
#include <ystdex/cassert.h> // for YAssertNonnull;
#include <ystdex/memory_resource.h> // for ystdex::pmr;
#include <ystdex/functor.hpp> // for ystdex::less, ystdex::equal_to;
//#include <ext/vstring.h>
#include <ystdex/tstring_view.hpp>
#include <ystdex/string.hpp> // for ystdex::basic_string,
//	ystdex::basic_string_view, ystdex::make_string_view, std::to_string,
//	std::basic_string;
#include <ystdex/array.hpp>
#include <deque>
#include <forward_list>
#include <ystdex/list.hpp> // for ystdex::list;
#include <vector>
#include <ystdex/map.hpp> // for ystdex::map;
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#if !defined(NDEBUG) && __GLIBCXX__
#include <debug/deque>
#include <debug/forward_list>
#include <debug/vector>
#include <debug/map>
#include <debug/set>
#include <debug/unordered_set>
#include <debug/unordered_map>
#endif
#include <ystdex/allocator.hpp> // for ystdex::propagating_allocator_adaptor,
//	ystdex::make_obj_using_allocator;
#include <iosfwd> // for std::basic_istringstream, std::basic_ostringstream,
//	std::basic_stringstream;
#include <ystdex/hash.hpp> // for std::hash, ystdex::hash_range;

namespace platform
{

//! \since build 843
namespace pmr = ystdex::pmr;

//! \since build 843
inline namespace basic_utilities
{

using std::forward_as_tuple;
using std::get;
using std::ignore;
using std::make_pair;
using std::make_tuple;
using std::pair;
using std::tie;
using std::tuple;
using std::tuple_cat;

//! \since build 546
using std::begin;
//! \since build 546
using std::end;

//! \since build 664
using ystdex::size;

} // inline namespace utilities;

//! \since build 593
inline namespace containers
{

#if !defined(NDEBUG) && __GLIBCXX__
namespace yimpl(stdd) = __gnu_debug;
#else
namespace yimpl(stdd) = std;
#endif

/*!
\brief 默认使用的分配器模板。
\since build 941
*/
#if false
// XXX: For exposition only. This shall be well-formed unless a memory resource
//	not compatible to propagatation is used. However, it is still not efficient
//	otherwise.
template<typename _type>
using default_allocator
	= ystdex::propagating_allocator_adaptor<pmr::polymorphic_allocator<_type>>;
#else
template<typename _type>
using default_allocator = pmr::polymorphic_allocator<_type>;
#endif

// XXX: The class template %__gnu_debug::array has been removed since GCC 11.
//	See https://gcc.gnu.org/git/?p=gcc.git;h=6db082477ad839438c4b54fc61083276c68d47ec.
using std::array;
//! \since build 843
//@{
template<typename _type, class _tAlloc = default_allocator<_type>>
using deque = stdd::deque<_type, _tAlloc>;

template<typename _type, class _tAlloc = default_allocator<_type>>
using forward_list = stdd::forward_list<_type, _tAlloc>;

template<typename _type, class _tAlloc = default_allocator<_type>>
using list = ystdex::list<_type, _tAlloc>;

template<typename _type, class _tAlloc = default_allocator<_type>>
using vector = stdd::vector<_type, _tAlloc>;


// NOTE: Since ISO C++17 node extraction of standard containers is not otherwise
//	supported, solely using of %ystdex::map does not lead to troubles about
//	extracted node handle incompatibilities. This also makes less templates need
//	to be instantiated, in the expense of no debug support in libstdc++, which
//	is an implementation detail.
template<typename _tKey, typename _tMapped, typename _fComp
	= ystdex::less<_tKey>, class _tAlloc
	= default_allocator<std::pair<const _tKey, _tMapped>>>
using map = ystdex::map<_tKey, _tMapped, _fComp, _tAlloc>;

template<typename _tKey, typename _tMapped, typename _fComp
	= ystdex::less<_tKey>, class _tAlloc
	= default_allocator<std::pair<const _tKey, _tMapped>>>
using multimap = stdd::multimap<_tKey, _tMapped, _fComp, _tAlloc>;

template<typename _tKey, typename _fComp = ystdex::less<_tKey>,
	class _tAlloc = default_allocator<_tKey>>
using multiset = stdd::multiset<_tKey, _fComp, _tAlloc>;

template<typename _tKey, typename _fComp = ystdex::less<_tKey>,
	class _tAlloc = default_allocator<_tKey>>
using set = stdd::set<_tKey, _fComp, _tAlloc>;


template<typename _tKey, typename _tMapped, typename _fHash = std::hash<_tKey>,
	typename _fEqual = ystdex::equal_to<_tKey>, class _tAlloc
	= default_allocator<std::pair<const _tKey, _tMapped>>>
using unordered_map
	= stdd::unordered_map<_tKey, _tMapped, _fHash, _fEqual, _tAlloc>;

template<typename _tKey, typename _tMapped, typename _fHash = std::hash<_tKey>,
	typename _fEqual = ystdex::equal_to<_tKey>, class _tAlloc
	= default_allocator<std::pair<const _tKey, _tMapped>>>
using unordered_multimap
	= stdd::unordered_multimap<_tKey, _tMapped, _fHash, _fEqual, _tAlloc>;

template<typename _tKey, typename _fHash = std::hash<_tKey>,
	typename _fEqual = ystdex::equal_to<_tKey>,
	class _tAlloc = default_allocator<_tKey>>
using unordered_multiset
	= stdd::unordered_multiset<_tKey, _fHash, _fEqual, _tAlloc>;

template<typename _tKey, typename _fHash = std::hash<_tKey>,
	typename _fEqual = ystdex::equal_to<_tKey>,
	class _tAlloc = default_allocator<_tKey>>
using unordered_set = stdd::unordered_set<_tKey, _fHash, _fEqual, _tAlloc>;

template<typename _type, class _tSeqCon = deque<_type>>
using stack = std::stack<_type, _tSeqCon>;

template<typename _tKey, class _tSeqCon = vector<_tKey>,
	class _fComp = ystdex::less<_tKey>>
using priority_queue = std::priority_queue<_tKey, _tSeqCon, _fComp>;

template<typename _type, class _tSeqCon = deque<_type>>
using queue = std::queue<_type, _tSeqCon>;
//@}

} // inline namespace containers;

inline namespace strings
{

/*!
\brief 满足 ISO C++03 std::basic_string 但不保证满足 ISO C++11 的实现。
\note 假定默认构造不抛出异常。
\since build 597
*/
template<typename _tChar, typename _tTraits = std::char_traits<_tChar>,
	class _tAlloc = default_allocator<_tChar>>
using basic_string = ystdex::basic_string<_tChar, _tTraits, _tAlloc>;
//	using versa_string = __gnu_cxx::__versa_string<_tChar>;

using string = basic_string<char>;
//! \since build 608
using u16string = basic_string<char16_t>;
//! \since build 608
using u32string = basic_string<char32_t>;
//! \since build 593
using wstring = basic_string<wchar_t>;

//! \since build 640
//@{
using ystdex::basic_string_view;
using ystdex::string_view;
using ystdex::u16string_view;
using ystdex::wstring_view;
//@}
//! \since build 836
using ystdex::string_view_t;
//! \since build 641
//@{
using ystdex::basic_tstring_view;
using ystdex::tstring_view;
using ystdex::u16tstring_view;
using ystdex::wtstring_view;
//@}

//! \since build 861
using ystdex::make_string_view;

//! \since build 861
static_assert(ystdex::is_implicitly_nothrow_constructible<const std::string&,
	const basic_string<char, std::char_traits<char>, std::allocator<char>>&>(),
	"Invalid basic string type (which is incompatible to exception"
	" the standard string type in exception types) found.");
//! \since build 833
static_assert(ystdex::is_implicitly_nothrow_constructible<string_view,
	string>(), "Invalid string view type found.");

//! \since build 593
using ystdex::sfmt;
//! \since build 593
using ystdex::vsfmt;

// XXX: This set of overloads is sometimes useful in conversions between values
//	of %basic_string and %std::basic_string instances.
//! \since build 896
//@{
template<typename _tChar, class _tString = basic_string<_tChar>>
inline _tString
to_pmr_string(basic_string_view<_tChar> sv)
{
	YAssertNonnull(sv.data());
	return _tString(sv.data(), sv.size());
}
//! \since build 905
template<typename _tChar, class _tString = basic_string<_tChar>,
	class _tAlloc = typename _tString::allocator_type>
inline auto
to_pmr_string(basic_string_view<_tChar> sv, const _tAlloc& a)
	-> decltype(ystdex::make_obj_using_allocator<_tString>(a, sv.data(),
	sv.size()))
{
	YAssertNonnull(sv.data());
	return ystdex::make_obj_using_allocator<_tString>(a, sv.data(), sv.size());
}
template<typename _tChar, class _tString = basic_string<_tChar>,
	class _tTraits, class _tAlloc,
	yimpl(typename = ystdex::enable_if_inconvertible_t<
	const std::basic_string<_tChar, _tTraits, _tAlloc>&, _tString>)>
inline _tString
to_pmr_string(const std::basic_string<_tChar, _tTraits, _tAlloc>& str)
{
	return _tString(str.data(), str.size());
}
//! \since build 905
template<typename _tChar, class _tString = basic_string<_tChar>,
	class _tTraits, class _tAlloc>
inline auto
to_pmr_string(const std::basic_string<_tChar, _tTraits, _tAlloc>& str,
	const _tAlloc& a)
	-> decltype(ystdex::make_obj_using_allocator<_tString>(a, str.data(),
	str.size()))
{
	return
		ystdex::make_obj_using_allocator<_tString>(a, str.data(), str.size());
}
template<class _tString = string, class _tParam = _tString,
	yimpl(typename = ystdex::enable_if_convertible_t<_tParam, _tString>)>
inline _tString
to_pmr_string(_tParam&& str)
{
	return yforward(str);
}
//! \since build 905
template<class _tString = string, class _tParam = _tString,
	class _tAlloc = typename _tString::allocator_type>
inline auto
to_pmr_string(_tParam&& str, const _tAlloc& a)
	-> decltype(ystdex::make_obj_using_allocator<_tString>(a, yforward(str)))
{
	return ystdex::make_obj_using_allocator<_tString>(a, yforward(str));
}
//@}

// XXX: The overloads %to_std_string is provided to get %std::string values, in
//	particular for arguments to %std::exception. No %std::wstring and other
//	types are required like this.
//! \since build 896
//@{
//! \since build 861
template<typename _tChar, class _tString = std::basic_string<_tChar>>
inline _tString
to_std_string(basic_string_view<_tChar> sv)
{
	YAssertNonnull(sv.data());
	return _tString(sv.data(), sv.size());
}
//! \since build 905
template<typename _tChar, class _tString = std::basic_string<_tChar>,
	class _tAlloc = typename _tString::allocator_type>
inline auto
to_std_string(basic_string_view<_tChar> sv, const _tAlloc& a)
	-> decltype(ystdex::make_obj_using_allocator<_tString>(a, sv.data(),
	sv.size()))
{
	YAssertNonnull(sv.data());
	return ystdex::make_obj_using_allocator<_tString>(a, sv.data(), sv.size());
}
template<typename _tChar, class _tString = std::basic_string<_tChar>,
	class _tTraits, class _tAlloc,
	yimpl(typename = ystdex::enable_if_inconvertible_t<
	const basic_string<_tChar, _tTraits, _tAlloc>&, _tString>)>
inline _tString
to_std_string(const basic_string<_tChar, _tTraits, _tAlloc>& str)
{
	return _tString(str.data(), str.size());
}
//! \since build 905
template<typename _tChar, class _tString = std::basic_string<_tChar>,
	class _tTraits, class _tAlloc>
inline auto
to_std_string(const basic_string<_tChar, _tTraits, _tAlloc>& str,
	const _tAlloc& a)
	-> decltype(ystdex::make_obj_using_allocator<_tString>(a, str.data(),
	str.size()))
{
	return
		ystdex::make_obj_using_allocator<_tString>(a, str.data(), str.size());
}
template<class _tString = std::string, class _tParam = _tString,
	yimpl(typename = ystdex::enable_if_convertible_t<_tParam, _tString>)>
inline _tString
to_std_string(_tParam&& str)
{
	return yforward(str);
}
//! \since build 905
template<class _tString = std::string, class _tParam = _tString,
	class _tAlloc = typename _tString::allocator_type>
inline auto
to_std_string(_tParam&& str, const _tAlloc& a)
	-> decltype(ystdex::make_obj_using_allocator<_tString>(a, yforward(str)))
{
	return ystdex::make_obj_using_allocator<_tString>(a, yforward(str));
}
//@}

//! \since build 308
using ystdex::to_string;
//! \since build 833
using ystdex::to_wstring;

} // inline namespace strings;

//! \since build 861
//@{
inline namespace ios
{

template<typename _tChar,
	class _tTraits = typename basic_string<_tChar>::traits_type,
	class _tAlloc = typename basic_string<_tChar>::allocator_type>
using basic_stringbuf = std::basic_stringbuf<_tChar, _tTraits, _tAlloc>;
template<class _tString>
using stringbuf_for = basic_stringbuf<typename _tString::value_type,
	typename _tString::traits_type, typename _tString::allocator_type>;
using stringbuf = stringbuf_for<string>;
using wstringbuf = stringbuf_for<wstring>;

template<typename _tChar,
	class _tTraits = typename basic_string<_tChar>::traits_type,
	class _tAlloc = typename basic_string<_tChar>::allocator_type>
using basic_istringstream = std::basic_istringstream<_tChar, _tTraits, _tAlloc>;
template<class _tString>
using istringstream_for = basic_istringstream<typename _tString::value_type,
	typename _tString::traits_type, typename _tString::allocator_type>;
using istringstream = istringstream_for<string>;
using wistringstream = istringstream_for<wstring>;

template<typename _tChar,
	class _tTraits = typename basic_string<_tChar>::traits_type,
	class _tAlloc = typename basic_string<_tChar>::allocator_type>
using basic_ostringstream = std::basic_ostringstream<_tChar, _tTraits, _tAlloc>;
template<class _tString>
using ostringstream_for = basic_ostringstream<typename _tString::value_type,
	typename _tString::traits_type, typename _tString::allocator_type>;
using ostringstream = ostringstream_for<string>;
using wostringstream = ostringstream_for<wstring>;

template<typename _tChar,
	class _tTraits = typename basic_string<_tChar>::traits_type,
	class _tAlloc = typename basic_string<_tChar>::allocator_type>
using basic_stringstream = std::basic_stringstream<_tChar, _tTraits, _tAlloc>;
template<class _tString>
using stringstream_for = basic_stringstream<typename _tString::value_type,
	typename _tString::traits_type, typename _tString::allocator_type>;
using stringstream = stringstream_for<string>;
using wstringstream = stringstream_for<wstring>;

} // inline namespace ios;
//@}

} // namespace platform;

namespace platform_ex
{

//! \since build 843
using namespace platform::basic_utilities;
//! \since build 593
using namespace platform::containers;
//! \since build 843
using namespace platform::strings;

} // namespace platform_ex;

// XXX: Currently %ystdex::polymorphic_allocator is always separatedly defined.
#if !YB_Impl_String_has_P0254R2
//! \since build 833
namespace std
{

/*!
\brief 使用多态分配器的 ystdex::basic_string 散列支持。
\since build 861
\see LWG 2978 。
\since build 941
\todo 在合适的 YBase.YStandardEx 头文件中扩展并使用 WG21 P1406R1 的解决方案。
*/
template<typename _tChar, class _tTraits>
struct hash<platform::strings::basic_string<_tChar, _tTraits>>
{
	size_t
	operator()(const platform::strings::basic_string<_tChar, _tTraits>& k) const
		ynothrow
	{
		// NOTE: This is similar to %ystdex::basic_string_view.
		return ystdex::hash_range(k.data(), k.data() + k.size());
	}
};

} // namespace std;
#endif

#endif

