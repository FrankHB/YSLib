/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ynew.cpp
\ingroup Adaptor
\brief 存储调试设施。
\version r955
\author FrankHB<frankhb1989@gmail.com>
\since build 173
\par 创建时间:
	2010-12-02 19:49:41 +0800
\par 修改时间:
	2012-09-12 08:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YNew
*/


#include "YSLib/Adaptor/ynew.h"
#include <cstdlib> // for std::atexit, std::malloc & std::free;
#include <cassert> // for assert;
#include <algorithm> // for std::for_each;
#include <functional> // for std::bind;

#ifdef YSL_USE_MEMORY_DEBUG

YSL_BEGIN

YSL_END

/*
using YSLib::GetDebugMemoryList;

void*
operator new(std::size_t s, const char* f, int l) ythrow(std::bad_alloc)
{
	void* p(::operator new(s));

	GetDebugMemoryList().Register(p, s, f, l);
	return p;
}
void*
operator new[](std::size_t s, const char* f, int l) ythrow(std::bad_alloc)
{
	void* p(::operator new[](s));

	GetDebugMemoryList().Register(p, s, f, l);
	return p;
}
void*
operator new(std::size_t s, const std::nothrow_t&, const char* f, int l)
	ynothrow
{
	void* p(::operator new(s, std::nothrow));

	GetDebugMemoryList().Register(p, s, f, l);
	return p;
}
void*
operator new[](std::size_t s, const std::nothrow_t&, const char* f, int l)
	ynothrow
{
	void* p(::operator new[](s, std::nothrow));

	GetDebugMemoryList().Register(p, s, f, l);
	return p;
}

void
operator delete(void* p, const char* f, int l) ynothrow
{
	GetDebugMemoryList().Unregister(p, f, l);
	::operator delete(p);
}
void
operator delete[](void* p, const char* f, int l) ynothrow
{
	GetDebugMemoryList().Unregister(p, f, l);
	::operator delete[](p);
}
void
operator delete(void* p, const std::nothrow_t&, const char* f, int l) ynothrow
{
	GetDebugMemoryList().Unregister(p, f, l);
	::operator delete(p);
}
void
operator delete[](void* p, const std::nothrow_t&, const char* f, int l) ynothrow
{
	GetDebugMemoryList().Unregister(p, f, l);
	::operator delete[](p);
}
*/

YSL_BEGIN

namespace
{

/*!
\brief 调试内存列表。
\warning 不可用于未确定初始化顺序的命名空间作用域对象。
\bug 非线程安全。
\since build 298
*/
static MemoryList DebugMemoryList(0);

} // unnamed namespace;

MemoryList&
GetDebugMemoryList()
{
	return DebugMemoryList;
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
MemoryList::Print(const MapType::value_type& val, std::FILE* stream)
{
	std::fprintf(stream, "@%p, [%u] @ %s: %d;\n", val.first,
		val.second.size, val.second.file.c_str(), val.second.line);
}

void
MemoryList::PrintAll(std::FILE* stream)
{
	std::for_each(Blocks.begin(), Blocks.end(),
		std::bind(MemoryList::Print, std::placeholders::_1, stream));
}

void
MemoryList::PrintAllDuplicate(std::FILE* stream)
{
	std::for_each(DuplicateDeletedBlocks.begin(), DuplicateDeletedBlocks.end(),
		std::bind(MemoryList::Print, std::placeholders::_1, stream));
}

YSL_END

#endif

