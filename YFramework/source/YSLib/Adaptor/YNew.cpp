/*
	© 2009-2016, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YNew.cpp
\ingroup Adaptor
\brief 存储调试设施。
\version r1013
\author FrankHB <frankhb1989@gmail.com>
\since build 173
\par 创建时间:
	2010-12-02 19:49:41 +0800
\par 修改时间:
	2022-11-28 18:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YNew
*/


#include "YSLib/Adaptor/YModules.h"
#include YFM_YSLib_Adaptor_YAdaptor
#include <cstdlib> // for std::atexit;
#include <cassert> // for assert;
#include <algorithm> // for std::for_each;
#include <ystdex/addressof.hpp> // for ystdex::pvoid;
#include <ystdex/bind.hpp> // for ystdex::bind1;

#ifdef YSL_USE_MEMORY_DEBUG

namespace YSLib
{

} // namespace YSLib;

#if 0
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
#endif

namespace YSLib
{

namespace
{

/*!
\brief 调试内存列表。
\warning 不可用于未确定初始化顺序的命名空间作用域对象。
\since build 298
*/
MemoryList DebugMemoryList({});

} // unnamed namespace;

MemoryList&
GetDebugMemoryList()
{
	static recursive_mutex mtx;
	lock_guard<recursive_mutex> lck(mtx);

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
		Blocks.emplace(p, MemoryList::BlockInfo(s, f, l));
}

void
MemoryList::Unregister(const void* p, const char* f, int l)
{
	if(p)
	{
		std::size_t n(Blocks.erase(p));

		if(n != 1)
			DuplicateDeletedBlocks.emplace_back(p,
				MemoryList::BlockInfo(0, f, l));
	}
}

void
MemoryList::Print(const MapType::value_type& val, std::FILE* stream)
{
	// XXX: Error from 'std::fprintf' is ignored.
	std::fprintf(stream, "@%p, [%zu] @ %s: %d;\n", ystdex::pvoid(val.first),
		val.second.size, val.second.file.c_str(), val.second.line);
}

void
MemoryList::PrintAll(std::FILE* stream)
{
	std::for_each(Blocks.cbegin(), Blocks.cend(),
		ystdex::bind1(MemoryList::Print, stream));
}

void
MemoryList::PrintAllDuplicate(std::FILE* stream)
{
	std::for_each(DuplicateDeletedBlocks.cbegin(),
		DuplicateDeletedBlocks.cend(),
		ystdex::bind1(MemoryList::Print, stream));
}

} // namespace YSLib;

#endif

