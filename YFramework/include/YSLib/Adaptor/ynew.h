/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ynew.h
\ingroup Adaptor
\brief 存储调试设施。
\version r2112;
\author FrankHB<frankhb1989@gmail.com>
\since build 173 。
\par 创建时间:
	2010-12-02 19:49:40 +0800;
\par 修改时间:
	2012-06-28 10:18 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YNew;
*/


#ifndef YSL_INC_ADAPTOR_YNEW_H_
#define YSL_INC_ADAPTOR_YNEW_H_

//包含编译配置。
#include "config.h"

//包含 YBase 基础定义。
#include <ydef.h>

//引入 YSLib 命名空间宏。
#include "../Core/ybasemac.h"

#ifdef YSL_USE_MEMORY_DEBUG

#include <new> // for std::nothrow_t;
#include <string>
#include <list>
#include <map>
#include <cstdio>
#include <ext/malloc_allocator.h> // for libg++ malloc allocator;
#include <ystdex/utility.hpp> // for ystdex::noncopyable;


#if 0
//@{
/*	\defgroup YSLMemoryDebugFunctions YSLib Memory Debug Functions
\brief 调试用重载 ::operator new 和 ::operator delete 。
\since build 173 。
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
operator delete[](void*, const std::nothrow_t&, const char*, int) throw();
//@}
#endif


YSL_BEGIN

class MemoryList;

/*!
\brief 取调试用内存块列表。
\since build 203 。
*/
MemoryList&
GetDebugMemoryList();


/*!
\brief 内存块列表。
\warning 非虚析构。
\since build 173 。
*/
class MemoryList : private ystdex::noncopyable
{
public:
	struct BlockInfo final
	{
	public:
		std::size_t size;
		std::string file;
		int line;
	
		explicit
		BlockInfo(std::size_t s, const char* f, int l)
			: size(s), file(f), line(l)
		{}
	};

	/*
	\brief new 表达式分配记录器。
	*/
	class NewRecorder final : private ystdex::noncopyable
	{
	private:
		MemoryList& blocks;
		const char* file;
		const int line;

	public:
		explicit yconstfn
		NewRecorder(const char* f, int l, MemoryList& b = GetDebugMemoryList())
			: blocks(b), file(f), line(l)
		{}

	public:
		/*!
		\brief 接受 new 表达式返回的指针并注册内存分配记录。

		\note 此用法参考 nvwa 0.8.2 ：
		http://sourceforge.net/projects/nvwa/files/nvwa/ 。
		*/
		template<typename T>
		T*
		operator->*(T* p)
		{
			blocks.Register(p, sizeof(T), file, line);
			return p;
		}
	};

	typedef std::map<const void*, BlockInfo, std::less<const void*>,
		__gnu_cxx::malloc_allocator<std::pair<const void* const, BlockInfo>> >
		MapType;
	typedef std::list<std::pair<const void*, BlockInfo>,
		__gnu_cxx::malloc_allocator<std::pair<const void*, BlockInfo>> >
		ListType;

	MapType Blocks;
	ListType DuplicateDeletedBlocks;

	explicit
	MemoryList(void(*)());

	DefGetter(const ynothrow, MapType::size_type, Size, Blocks.size())

	void
	Register(const void*, std::size_t, const char*, int);

	void
	Unregister(const void*, const char*, int);

	//! \since build 317 。
	static void
	Print(const MapType::value_type&, std::FILE*);

	void
	PrintAll(std::FILE*);

	void
	PrintAllDuplicate(std::FILE*);
};

YSL_END

/*
\def ynew
\def ynew_nothrow
\def ydelete
\def ydelete_array
\bug 调试内存列表非线程安全。
*/
#	define ynew YSLib::MemoryList::NewRecorder(__FILE__, __LINE__)->*new
#	define ynew_nothrow new(std::nothrow, __FILE__, __LINE__)
#	define ydelete(p) (GetDebugMemoryList().Unregister(p, __FILE__, \
		__LINE__), delete p)
#	define ydelete_array(p) (GetDebugMemoryList().Unregister(p, __FILE__, \
		__LINE__), \
	delete[] p)

#else

#	define ynew new
#	define ynew_nothrow new(std::nothrow)
#	define ydelete delete
#	define ydelete_array(p) (delete[] p)

#endif

#endif

