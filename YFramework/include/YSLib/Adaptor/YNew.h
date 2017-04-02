/*
	© 2010-2015, 2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YNew.h
\ingroup Adaptor
\brief 存储调试设施。
\version r1197
\author FrankHB <frankhb1989@gmail.com>
\since build 561
\par 创建时间:
	2010-12-02 19:49:40 +0800
\par 修改时间:
	2017-03-30 14:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YNew
*/


#ifndef YSL_INC_Adaptor_YNew_h_
#define YSL_INC_Adaptor_YNew_h_ 1

#include "YModules.h"

//包含编译配置。
#include YFM_YSLib_Adaptor_Configuration

//包含 YBase 基础定义。
#include <ydef.h>

//引入 YSLib 基本宏。
#include YFM_YBaseMacro

#ifdef YSL_USE_MEMORY_DEBUG

#include <new> // for std::nothrow_t;
#include <string>
#include <list>
#include <map>
#include <cstdio>
#if defined(__GLIBCXX__)
#	include <ext/malloc_allocator.h> // for libstdc++ malloc allocator;
#endif
#include <ystdex/base.h> // for ystdex::noncopyable;

#if false
//@{
/*	\defgroup YSLMemoryDebugFunctions YSLib Memory Debug Functions
\brief 调试用重载 ::operator new 和 ::operator delete 。
\since build 173
*/
YF_API YB_ALLOCATOR void*
operator new(std::size_t, const char*, int) ythrow(std::bad_alloc);
YF_API YB_ALLOCATOR void*
operator new[](std::size_t, const char*, int) ythrow(std::bad_alloc);
YF_API YB_ALLOCATOR void*
operator new(std::size_t, const std::nothrow_t&, const char*, int) ynothrow;
YF_API YB_ALLOCATOR void*
operator new[](std::size_t, const std::nothrow_t&, const char*, int) ynothrow;
YF_API void
operator delete(void*, const char*, int) ynothrow;
YF_API void
operator delete[](void*, const char*, int) ynothrow;
YF_API void
operator delete(void*, const std::nothrow_t&, const char*, int) ynothrow;
YF_API void
operator delete[](void*, const std::nothrow_t&, const char*, int) ynothrow;
//@}
#endif


namespace YSLib
{

class MemoryList;

/*!
\brief 取调试用内存块列表。
\since build 203
*/
YF_API MemoryList&
GetDebugMemoryList();


//! \since build 454
template<typename _type>
using MemoryListAllocator
#if defined(__GLIBCXX__)
	= __gnu_cxx::malloc_allocator<_type>;
#else
	= std::allocator<_type>;
#endif


/*!
\brief 内存块列表。
\warning 非虚析构。
\since build 173
*/
class YF_API MemoryList : private ystdex::noncopyable
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
		template<typename _type>
		_type*
		operator->*(_type* p)
		{
			blocks.Register(p, sizeof(_type), file, line);
			return p;
		}
	};

	using MapType = std::map<const void*, BlockInfo, std::less<const void*>,
		MemoryListAllocator<std::pair<const void* const, BlockInfo>>>;
	using ListType = std::list<std::pair<const void*, BlockInfo>,
		MemoryListAllocator<std::pair<const void*, BlockInfo>>>;

	MapType Blocks;
	ListType DuplicateDeletedBlocks;

	explicit
	MemoryList(void(*)());

	DefGetter(const ynothrow, MapType::size_type, Size, Blocks.size())

	void
	Register(const void*, std::size_t, const char*, int);

	void
	Unregister(const void*, const char*, int);

	//! \since build 317
	static void
	Print(const MapType::value_type&, std::FILE*);

	void
	PrintAll(std::FILE*);

	void
	PrintAllDuplicate(std::FILE*);
};

} // namespace YSLib;

/*
\def ynew
\def ynew_nothrow
\def ydelete
\def ydelete_array
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

