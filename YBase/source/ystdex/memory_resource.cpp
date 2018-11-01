/*
	© 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory_resource.cpp
\ingroup YStandardEx
\brief 存储资源。
\version r156
\author FrankHB <frankhb1989@gmail.com>
\since build 842
\par 创建时间:
	2018-10-27 19:30:12 +0800
\par 修改时间:
	2018-11-02 03:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::MemoryResource
*/


#include "ystdex/memory_resource.h" // for __cpp_aligned_new, std::align_val_t,
//	__cpp_sized_deallocation, std::bad_alloc, ::operator new, ::operator delete;
#if YB_Has_memory_resource != 1
#	include <atomic> // for std::atomic;
#endif

namespace ystdex
{

namespace pmr
{

inline namespace cpp2017
{

#if YB_Has_memory_resource != 1
memory_resource::~memory_resource() = default;


memory_resource*
new_delete_resource() ynothrow
{
	static struct nres final : public memory_resource
	{
#if __cpp_aligned_new >= 201606L
#	define YB_Impl_aligned_new true
#endif
		YB_ALLOCATOR void*
		do_allocate(size_t bytes, size_t alignment) override
		{
#if YB_Impl_aligned_new
			if(alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
				return ::operator new(bytes, std::align_val_t(alignment));
#else
			yunused(alignment);
#endif
			return ::operator new(bytes);
		}

		void
		do_deallocate(void* p, size_t bytes, size_t alignment) ynothrow override
		{
#if YB_Impl_aligned_new
			if(alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
				::operator delete(p, bytes, alignment);
			else
				::operator delete(p, bytes, std::align_val_t(alignment));
#elif __cpp_sized_deallocation >= 201309L
			yunused(alignment);
			::operator delete(p, bytes);
#else
			yunused(bytes), yunused(alignment);
			::operator delete(p);
#endif

		}
#undef YB_Impl_aligned_new

		bool
		do_is_equal(const memory_resource& r) const ynothrow override
		{
			return &r == this;
		}
	} r;

	return &r;
}

memory_resource*
null_memory_resource() ynothrow
{
	static struct nres final : public memory_resource
	{
		YB_NORETURN void*
		do_allocate(size_t, size_t) override
		{
			throw std::bad_alloc();
		}

		void
		do_deallocate(void*, size_t, size_t) ynothrow override
		{}

		bool
		do_is_equal(const memory_resource& r) const ynothrow override
		{
			// NOTE: This is required even it can be singleton. See ISO C++
			//	[mem.res.global]/2.
			return &r == this;
		}
	} r;

	return &r;
}

namespace
{

std::atomic<memory_resource*>&
ref_default_resource()
{
	static std::atomic<memory_resource*> res(new_delete_resource());

	return res;
}

} // unnamed namespace;


memory_resource*
set_default_resource(memory_resource* r) ynothrow
{
	if(!r)
		r = new_delete_resource();
	return ref_default_resource().exchange(r);
}

memory_resource*
get_default_resource() ynothrow
{
	return ref_default_resource().load();
}
#endif

} // namespace cpp2017;

} // namespace pmr;

} // namespace ystdex;

