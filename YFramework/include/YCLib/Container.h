/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Container.h
\ingroup YCLib
\brief 容器、拟容器和适配器。
\version r817
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2010-10-09 09:25:26 +0800
\par 修改时间:
	2015-10-03 01:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::YContainer
*/


#ifndef YCL_INC_Container_h_
#define YCL_INC_Container_h_ 1

#include "YModules.h"
#include YFM_YCLib_YCommon
//#include <ext/vstring.h>
#include <ystdex/tstring_view.hpp>
#include <ystdex/array.hpp>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>

namespace platform
{

//! \since build 593
inline namespace containers
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

using std::array;
using std::deque;
using std::forward_list;
using std::list;
using std::vector;

using std::map;
using std::multimap;
using std::multiset;
using std::set;

using std::unordered_map;
//! \since build 461
using std::unordered_multimap;
//! \since build 461
using std::unordered_multiset;
using std::unordered_set;

using std::stack;
using std::priority_queue;
using std::queue;

/*!
\brief 满足 ISO C++03 std::basic_string 但不保证满足 ISO C++11 的实现。
\note 假定默认构造不抛出异常。
\since build 597
*/
template<typename _tChar, typename _tTraits = std::char_traits<_tChar>,
	class _tAlloc = std::allocator<_tChar>>
using basic_string = std::basic_string<_tChar, _tTraits, _tAlloc>;
// using versa_string = __gnu_cxx::__versa_string<_tChar>;

using string = basic_string<char>;
//! \since build 608
using u16string = basic_string<char16_t>;
//! \since build 608
using u32string = basic_string<char32_t>;
//! \since build 593
//@{
using wstring = basic_string<wchar_t>;

//! \since build 640
//@{
using ystdex::basic_string_view;
using ystdex::string_view;
using ystdex::u16string_view;
using ystdex::wstring_view;
//@}
//! \since build 641
//@{
using ystdex::basic_tstring_view;
using ystdex::tstring_view;
using ystdex::u16tstring_view;
using ystdex::wtstring_view;
//@}

using ystdex::sfmt;
using ystdex::vsfmt;
//@}

//! \since build 291
using ystdex::arrlen;
//! \since build 308
using std::to_string;
//! \since build 308
using ystdex::to_string;
//! \since build 593
using std::to_wstring;

} // inline namespace containers;

} // namespace platform;

namespace platform_ex
{

//! \since build 593
using namespace platform::containers;

} // namespace platform_ex;

#endif

