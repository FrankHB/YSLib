/*
	© 2010-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Container.h
\ingroup YCLib
\brief 容器、拟容器和适配器。
\version r924
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2010-10-09 09:25:26 +0800
\par 修改时间:
	2018-11-06 11:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Container
*/


#ifndef YCL_INC_Container_h_
#define YCL_INC_Container_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
#include <ystdex/memory_resource.h> // for ystdex::pmr;
#include <ystdex/functor.hpp> // for ystdex::less, ystdex::equal_to;
//#include <ext/vstring.h>
#include <ystdex/tstring_view.hpp>
#include <ystdex/string.hpp>
#include <ystdex/array.hpp>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>
#include <ystdex/map.hpp> // for ystdex::map;
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>

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

using std::array;
//! \since build 843
//@{
template<typename _type, class _tAlloc = pmr::polymorphic_allocator<_type>>
using deque = std::deque<_type, _tAlloc>;

template<typename _type, class _tAlloc = pmr::polymorphic_allocator<_type>>
using forward_list = std::forward_list<_type, _tAlloc>;

template<typename _type, class _tAlloc = pmr::polymorphic_allocator<_type>>
using list = std::list<_type, _tAlloc>;

template<typename _type, class _tAlloc = pmr::polymorphic_allocator<_type>>
using vector = std::vector<_type, _tAlloc>;


// NOTE: Since ISO C++17 node extraction of standard containers is not otherwise
//	supported, solely using of %ystdex::map does not lead to troubles about
//	extracted node handle incompatibilities. This also makes less templates need
//	to be instantiated, in the expense of no debug support in libstdc++, which
//	is an implementation detail.
template<typename _tKey, typename _tMapped, typename _fComp
	= ystdex::less<_tKey>, class _tAlloc
	= pmr::polymorphic_allocator<std::pair<const _tKey, _tMapped>>>
using map = ystdex::map<_tKey, _tMapped, _fComp, _tAlloc>;

template<typename _tKey, typename _tMapped, typename _fComp
	= ystdex::less<_tKey>, class _tAlloc
	= pmr::polymorphic_allocator<std::pair<const _tKey, _tMapped>>>
using multimap = std::multimap<_tKey, _tMapped, _fComp, _tAlloc>;

template<typename _tKey, typename _fComp = ystdex::less<_tKey>,
	class _tAlloc = pmr::polymorphic_allocator<_tKey>>
using multiset = std::multiset<_tKey, _fComp, _tAlloc>;

template<typename _tKey, typename _fComp = ystdex::less<_tKey>,
	class _tAlloc = pmr::polymorphic_allocator<_tKey>>
using set = std::set<_tKey, _fComp, _tAlloc>;


template<typename _tKey, typename _tMapped, typename _fHash = std::hash<_tKey>,
	typename _fEqual = ystdex::equal_to<_tKey>, class _tAlloc
	= pmr::polymorphic_allocator<std::pair<const _tKey, _tMapped>>>
using unordered_map
	= std::unordered_map<_tKey, _tMapped, _fHash, _fEqual, _tAlloc>;

template<typename _tKey, typename _tMapped, typename _fHash = std::hash<_tKey>,
	typename _fEqual = ystdex::equal_to<_tKey>, class _tAlloc
	= pmr::polymorphic_allocator<std::pair<const _tKey, _tMapped>>>
using unordered_multimap
	= std::unordered_multimap<_tKey, _tMapped, _fHash, _fEqual, _tAlloc>;

template<typename _tKey, typename _fHash = std::hash<_tKey>,
	typename _fEqual = ystdex::equal_to<_tKey>,
	class _tAlloc = pmr::polymorphic_allocator<_tKey>>
using unordered_multiset
	= std::unordered_multiset<_tKey, _fHash, _fEqual, _tAlloc>;

template<typename _tKey, typename _fHash = std::hash<_tKey>,
	typename _fEqual = ystdex::equal_to<_tKey>,
	class _tAlloc = pmr::polymorphic_allocator<_tKey>>
using unordered_set = std::unordered_set<_tKey, _fHash, _fEqual, _tAlloc>;

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
	class _tAlloc = std::allocator<_tChar>>
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

//! \since build 833
//@{
static_assert(ystdex::is_implicitly_nothrow_constructible<const std::string&,
	const string&>(), "Invalid string type (which is incompatible to exception"
	" the standard string type in exception types) found.");
static_assert(ystdex::is_implicitly_nothrow_constructible<string_view,
	string>(), "Invalid string view type found.");

//! \since build 593
using ystdex::sfmt;
//! \since build 593
using ystdex::vsfmt;

//! \since build 308
using ystdex::to_string;
using ystdex::to_wstring;
//@}

} // inline namespace strings;

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

#endif

