/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ynew.cpp
\ingroup Service
\brief 存储调试设施。
\version 0.1683;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-12-02 19:49:41 + 08:00;
\par 修改时间:
	2010-12-04 23:28 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YNew;
*/


#include "ynew.h"
#include <cstdlib> //for std::atexit;
#include <cassert> //for assert;

#ifdef YSL_USE_MEMORY_DEBUG

YSL_BEGIN

YSL_END

using YSLib::DebugMemory;

void*
operator new(std::size_t s, const char* f, int l) throw (std::bad_alloc)
{
	void* p(::operator new(s));

	DebugMemory.Register(p, s, f, l);
	return p;
}
void*
operator new[](std::size_t s, const char* f, int l) throw (std::bad_alloc)
{
	void* p(::operator new(s));

	DebugMemory.Register(p, s, f, l);
	return p;
}
void*
operator new(std::size_t s, const std::nothrow_t&, const char* f, int l) throw()
{
	void* p(::operator new(s, std::nothrow));

	DebugMemory.Register(p, s, f, l);
	return p;
}
void*
operator new[](std::size_t s, const std::nothrow_t&, const char* f, int l)
	throw()
{
	void* p(::operator new(s, std::nothrow));

	DebugMemory.Register(p, s, f, l);
	return p;
}

void
operator delete(void* p, const char* f, int l) throw()
{
	DebugMemory.Unregister(p, f, l);
	::operator delete(p);
}
void
operator delete[](void* p, const char* f, int l) throw()
{
	DebugMemory.Unregister(p, f, l);
	::operator delete(p);
}
void
operator delete(void* p, const std::nothrow_t&, const char* f, int l) throw()
{
	DebugMemory.Unregister(p, f, l);
	::operator delete(p);
}
void
operator delete[](void* p, const std::nothrow_t&, const char* f, int l) throw()
{
	DebugMemory.Unregister(p, f, l);
	::operator delete(p);
}

YSL_BEGIN

MemoryList::MemoryList(void(*p)())
	: m_map()
{
	if(p)
		std::atexit(p);
}

void
MemoryList::Register(const void* p, std::size_t s, const char* f, int l)
{
	if(p)
		m_map.insert(std::make_pair(p, MemoryList::BlockInfo(s, f, l)));
}

void
MemoryList::Unregister(const void* p, const char*, int)
{
	if(p)
	{
		std::size_t n(m_map.erase(p));

		assert(n == 1);
	}
}

void
MemoryList::Print(MapType::const_iterator i, std::FILE* stream)
{
	std::fprintf(stream, "@%p, [%u] @ %s: %d;\n", i->first,
		i->second.size, i->second.file.c_str(), i->second.line);
}

void
MemoryList::PrintAll(std::FILE* stream)
{
	for(MapType::const_iterator i(m_map.begin()); i != m_map.end(); ++i)
		Print(i, stream);
}

YSL_END

#endif

