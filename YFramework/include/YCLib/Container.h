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
\version r786
\author FrankHB <frankhb1989@gmail.com>
\since build 593
\par 创建时间:
	2010-10-09 09:25:26 +0800
\par 修改时间:
	2015-04-24 07:40 +0800
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
#include <ystdex/string.hpp>

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

template<typename _tChar>
struct GSStringTemplate
{
	using basic_string = std::basic_string<_tChar>;
};

using string = yimpl(GSStringTemplate<char>::basic_string);
//! \since build 593
//@{
using wstring = yimpl(GSStringTemplate<wchar_t>::basic_string);

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

