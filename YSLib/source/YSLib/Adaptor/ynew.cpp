/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ynew.cpp
\ingroup Adaptor
\brief 存储调试设施。
\version 0.1886;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-12-02 19:49:41 +0800;
\par 修改时间:
	2011-04-21 15:49 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YNew;
*/


#include "ynew.h"
#include <cstdlib> //for std::atexit & std::malloc & std::free;
#include <cassert> //for assert;

#ifdef YSL_USE_MEMORY_DEBUG

YSL_BEGIN

YSL_END

/*
using YSLib::GetDebugMemoryList;

void*
operator new(std::size_t s, const char* f, int l) throw (std::bad_alloc)
{
	void* p(::operator new(s));

	GetDebugMemoryList().Register(p, s, f, l);
	return p;
}
void*
operator new[](std::size_t s, const char* f, int l) throw (std::bad_alloc)
{
	void* p(::operator new[](s));

	GetDebugMemoryList().Register(p, s, f, l);
	return p;
}
void*
operator new(std::size_t s, const std::nothrow_t& unused,
	const char* f, int l) throw()
{
	void* p(::operator new(s, std::nothrow));

	GetDebugMemoryList().Register(p, s, f, l);
	return p;
}
void*
operator new[](std::size_t s, const std::nothrow_t& unused,
	const char* f, int l)
	throw()
{
	void* p(::operator new[](s, std::nothrow));

	GetDebugMemoryList().Register(p, s, f, l);
	return p;
}

void
operator delete(void* p, const char* f, int l) throw()
{
	GetDebugMemoryList().Unregister(p, f, l);
	::operator delete(p);
}
void
operator delete[](void* p, const char* f, int l) throw()
{
	GetDebugMemoryList().Unregister(p, f, l);
	::operator delete[](p);
}
void
operator delete(void* p, const std::nothrow_t& unused,
	const char* f, int l) throw()
{
	GetDebugMemoryList().Unregister(p, f, l);
	::operator delete(p);
}
void
operator delete[](void* p, const std::nothrow_t& unused,
	const char* f, int l) throw()
{
	GetDebugMemoryList().Unregister(p, f, l);
	::operator delete[](p);
}
*/

YSL_BEGIN

MemoryList&
GetDebugMemoryList()
{
	static MemoryList debug_memory_list(NULL);

	return debug_memory_list;
}


MemoryList::MemoryList(void(*p)())
	: Blocks(), DuplicateDeletedBlocks()
{
	if(p)
		std::atexit(p);
}

void
MemoryList::Register(const void* p, std::size_t s, const char* f, int l)
{
	if(p)
		Blocks.insert(std::make_pair(p, MemoryList::BlockInfo(s, f, l)));
}

void
MemoryList::Unregister(const void* p, const char* f, int l)
{
	if(p)
	{
		std::size_t n(Blocks.erase(p));

		if(n != 1)
			DuplicateDeletedBlocks.push_back(std::make_pair(p,
				MemoryList::BlockInfo(0, f, l)));
	}
}

void
MemoryList::Print(MapType::const_iterator i, std::FILE* stream)
{
	std::fprintf(stream, "@%p, [%u] @ %s: %d;\n", i->first,
		i->second.size, i->second.file.c_str(), i->second.line);
}
void
MemoryList::Print(ListType::const_iterator i, std::FILE* stream)
{
	std::fprintf(stream, "@%p, [%u] @ %s: %d;\n", i->first,
		i->second.size, i->second.file.c_str(), i->second.line);
}

void
MemoryList::PrintAll(std::FILE* stream)
{
	for(MapType::const_iterator i(Blocks.begin()); i != Blocks.end(); ++i)
		Print(i, stream);
}

void
MemoryList::PrintAllDuplicate(std::FILE* stream)
{
	for(ListType::const_iterator i(DuplicateDeletedBlocks.begin());
		i != DuplicateDeletedBlocks.end(); ++i)
		Print(i, stream);
}

YSL_END

#endif

