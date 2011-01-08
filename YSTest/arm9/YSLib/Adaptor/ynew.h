/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ynew.h
\ingroup Adaptor
\brief 存储调试设施。
\version 0.1898;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-12-02 19:49:40 + 08:00;
\par 修改时间:
	2011-01-08 18:21 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YNew;
*/


#ifndef INCLUDED_YDEBUG_H_
#define INCLUDED_YDEBUG_H_

//包含编译配置。
#include "config.h"

//包含平台文件。
#include <platform.h>

//引入 YSLib 命名空间宏。
#include "base.h"

#ifdef YSL_USE_MEMORY_DEBUG

#include <new> // for std::nothrow_t;
#include <string>
#include <list>
#include <map>
#include <cstdio>
#include <ext/malloc_allocator.h> // for libg++ malloc allocator;


//@{
/*!	\defgroup YSLMemoryDebugFunctions YSLib Memory Debug Functions
\brief 调试用重载 operator new 和 operator delete 。
*/
void*
operator new(std::size_t, const char*, int) throw (std::bad_alloc);
void*
operator new[](std::size_t, const char*, int) throw (std::bad_alloc);
void*
operator new(std::size_t, const std::nothrow_t&, const char*, int) throw();
void*
operator new[](std::size_t, const std::nothrow_t&, const char*, int) throw();
void
operator delete(void*, const char*, int) throw();
void
operator delete[](void*, const char*, int) throw();
void
operator delete(void*, const std::nothrow_t&, const char*, int) throw();
void
operator delete[](void*, const std::nothrow_t&) throw();
//}@


YSL_BEGIN

//内存块列表。
class MemoryList
{
public:
	struct BlockInfo
	{
	public:
		std::size_t size;
		std::string file;
		int line;
	
		explicit
		BlockInfo(std::size_t, const char*, int);
	};

	typedef std::map<const void*, BlockInfo, std::less<const void*>,
		__gnu_cxx::malloc_allocator<std::pair<const void* const, BlockInfo> > >
		MapType;
	typedef std::list<std::pair<const void*, BlockInfo>,
		__gnu_cxx::malloc_allocator<std::pair<const void*, BlockInfo> > >
		ListType;

	MapType Blocks;
	ListType DuplicateDeletedBlocks;

	explicit
	MemoryList(void(*)());

	DefGetter(MapType::size_type, Size, Blocks.size())

	void
	Register(const void*, std::size_t, const char*, int);

	void
	Unregister(const void*, const char*, int);

	static void
	Print(MapType::const_iterator, std::FILE*);
	static void
	Print(ListType::const_iterator, std::FILE*);

	void
	PrintAll(std::FILE*);

	void
	PrintAllDuplicate(std::FILE*);
};

inline
MemoryList::BlockInfo::BlockInfo(std::size_t s, const char* f, int l)
	: size(s), file(f), line(l)
{}


//! \brief 调试用内存块列表。
extern YSLib::MemoryList DebugMemory;

YSL_END

#	define ynew new(__FILE__, __LINE__)
#	define ynew_nothrow new(std::nothrow, __FILE__, __LINE__)
#	define ydelete(p) (DebugMemory.Unregister(p, __FILE__, __LINE__), delete p)
#	define ydelete_array(p) (DebugMemory.Unregister(p, __FILE__, __LINE__), \
	delete[] p)

#else

#	define ynew new
#	define ynew_nothrow new(std::nothrow)
#	define ydelete delete
#	define ydelete_array(p) (delete[] p)

#endif

#endif

