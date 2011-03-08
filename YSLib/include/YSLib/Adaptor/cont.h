/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cont.h
\ingroup Adaptor
\brief 容器适配器。
\version 0.1522;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-09 09:25:26 +0800;
\par 修改时间:
	2011-03-05 17:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::Container;
*/


#ifndef INCLUDED_CONT_H_
#define INCLUDED_CONT_H_

#include "yadaptor.h"

//包含 YASLI 。
#ifdef YSL_USE_YASLI_VECTOR
#	include <loki/yasli/yasli_vector.h>
#else
#	include <vector>
#endif

//包含 flex_string 。
#ifdef YSL_USE_FLEX_STRING
#	include <loki/flex/flex_string_shell.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Design)

YSL_BEGIN_NAMESPACE(Policies)

YSL_BEGIN_NAMESPACE(Storage)

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

YSL_END_NAMESPACE(Storage)

YSL_END_NAMESPACE(Policies)

YSL_END_NAMESPACE(Design)

YSL_END

#else
#	include <string>
#endif

//包含标准库容器和容器适配器。
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>

YSL_BEGIN

#ifdef YSL_USE_YASLI_VECTOR
	using yasli::vector;
#else
	using std::vector;
#endif

using std::list;
using std::map;
using std::set;
using std::stack;
using std::queue;
using std::priority_queue;

#ifdef YSL_USE_FLEX_STRING

template<typename _tChar,
	class _tCharTrait = std::char_traits<_tChar>,
	class _tAlloc = std::allocator<_tChar>,
	class _tStorage = 
#	if defined(YSL_OPT_SMALL_STRING_LENGTH) && YSL_OPT_SMALL_STRING_LENGTH > 0
		Design::Policies::Storage::SmallStringOpt<
#	endif
#	ifdef YSL_USE_COPY_ON_WRITE
		Design::Policies::Storage::CowStringOpt<
#	endif
			Design::Policies::Storage::AllocatorStringStorage<_tChar, _tAlloc>
#	ifdef YSL_USE_COPY_ON_WRITE
		>
#	endif
#	if defined(YSL_OPT_SMALL_STRING_LENGTH) && YSL_OPT_SMALL_STRING_LENGTH > 0
		, YSL_OPT_SMALL_STRING_LENGTH
	>
#endif
>
struct GSStringTemplate
{
	typedef flex_string<_tChar, _tCharTrait, _tAlloc, _tStorage> basic_string;
};

#else

template<typename _tChar>
struct GSStringTemplate
{
	typedef std::basic_string<_tChar> basic_string;
};

#endif

typedef GSStringTemplate<char>::basic_string string;

// YSLib 基本字符串（使用 UTF-16LE ）。
typedef GSStringTemplate<uchar_t>::basic_string ustring;

YSL_END

#endif

