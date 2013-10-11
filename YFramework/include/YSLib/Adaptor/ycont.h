/*
	© 2010-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycont.h
\ingroup Adaptor
\brief 容器、拟容器和适配器。
\version r675
\author FrankHB <frankhb1989@gmail.com>
\since build 161
\par 创建时间:
	2010-10-09 09:25:26 +0800
\par 修改时间:
	2013-10-07 00:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YContainer
*/


#ifndef YSL_INC_Adaptor_ycont_h_
#define YSL_INC_Adaptor_ycont_h_ 1

#include "yadaptor.h"
#include <utility>
#include <ystdex/tuple.hpp>
#include "CHRLib/chrdef.h"


//包含 flex_string 。
#ifdef YSL_USE_FLEX_STRING
#	include <loki/flex/flex_string_shell.h>

namespace YSLib
{

/*
#	ifdef YSL_USE_YASLI_VECTOR
#		include "yaslivsp.hpp"
#	else
#		include <loki/flex/vectorstringstorage.h>
#	endif
*/
#	include <loki/flex/allocatorstringstorage.h>
#	include <loki/flex/smallstringopt.h>
#	ifdef YSL_USE_COPY_ON_WRITE
#		include <loki/flex/cowstringopt.h>
#	endif

} // namespace YSLib;

#else
//#	include <string>
//#	include <ext/vstring.h>
#endif
#include <ystdex/string.hpp>

//包含标准库容器和容器适配器或代用品。
#include <array>
#include <deque>
#include <forward_list>
#include <list>
//包含 YASLI 。
#ifdef YSL_USE_YASLI_VECTOR
#	include <loki/yasli/yasli_vector.h>
#else
#	include <vector>
#endif

#include <map>
#include <set>

#include <unordered_set>
#include <unordered_map>

#include <queue>
#include <stack>

namespace YSLib
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


using std::array;
using std::deque;
using std::forward_list;
using std::list;
#ifdef YSL_USE_YASLI_VECTOR
	using yasli::vector;
#else
	using std::vector;
#endif

using std::map;
using std::multimap;
using std::multiset;
using std::set;

using std::unordered_map;
using std::unordered_set;

using std::stack;
using std::priority_queue;
using std::queue;

#ifdef YSL_USE_FLEX_STRING

template<typename _tChar,
	class _tCharTrait = std::char_traits<_tChar>,
	class _tAlloc = std::allocator<_tChar>,
	class _tStorage =
#	if YSL_OPT_SMALL_STRING_LENGTH > 0
		SmallStringOpt<
#	endif
#	ifdef YSL_USE_COPY_ON_WRITE
		CowStringOpt<
#	endif
			AllocatorStringStorage<_tChar, _tAlloc>
#	ifdef YSL_USE_COPY_ON_WRITE
		>
#	endif
#	if YSL_OPT_SMALL_STRING_LENGTH > 0
		, YSL_OPT_SMALL_STRING_LENGTH
	>
#endif
>
struct GSStringTemplate
{
	using basic_string = flex_string<_tChar, _tCharTrait, _tAlloc, _tStorage>;
};

#else

template<typename _tChar>
struct GSStringTemplate
{
	using basic_string = std::basic_string<_tChar>;
};

#endif

using string = GSStringTemplate<char>::basic_string;

// YSLib 基本字符串（使用 UCS-2LE / UCS-4LE ）。
using ucs2string = GSStringTemplate<CHRLib::ucs2_t>::basic_string;
using ucs4string = GSStringTemplate<CHRLib::ucs4_t>::basic_string;

} // namespace YSLib;

#endif

