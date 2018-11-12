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
\version r657
\author FrankHB <frankhb1989@gmail.com>
\since build 842
\par 创建时间:
	2018-10-27 19:30:12 +0800
\par 修改时间:
	2018-11-09 12:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::MemoryResource
*/


#include "ystdex/memory_resource.h" // for __cpp_aligned_new, std::align_val_t,
//	std::align, __cpp_sized_deallocation, std::bad_alloc, ::operator new,
//	::operator delete, make_observer, is_trivial, is_decayed, yassume,
//	yconstraint, PTRDIFF_MAX, ceiling_lb;
#if YB_Has_memory_resource != 1
#	include <atomic> // for std::atomic;
#	include "ystdex/map.hpp" // for map, greater;
#	include "ystdex/pointer.hpp" // for tidy_ptr;
#	include <algorithm> // for std::max, std::min, std::lower_bound;
#	include "ystdex/functional.hpp" // for ystdex::logical_not, retry_on_cond;
#	include "ystdex/scope_guard.hpp" // for make_guard;
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
			return alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__
				? ::operator new(bytes, std::align_val_t(alignment))
				: ::operator new(bytes);
#else
			if(alignment > 1)
			{
				// TODO: Record sizeof debugging?
				// TODO: Extract as %::operator new with extended alignment?
				using hdr_t = void*;
				auto space(sizeof(hdr_t) + bytes + alignment);
				const auto ptr(::operator new(space));
				void* p(static_cast<byte*>(ptr) + sizeof(hdr_t));

				if(std::align(alignment, bytes, p, space))
				{
					yassume(p);
					::new(static_cast<byte*>(p) - sizeof(hdr_t)) hdr_t(ptr);
					return p;
				}
				throw std::bad_alloc();
			}
			return ::operator new(bytes);
#endif
		}

		void
		do_deallocate(void* p, size_t bytes, size_t alignment) ynothrow override
		{
#if YB_Impl_aligned_new
			if(alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
				::operator delete(p, bytes, std::align_val_t(alignment));
#	if __cpp_sized_deallocation >= 201309L
			::operator delete(p, bytes);
#	else
			yunused(bytes);
			::operator delete(p);
#	endif
#else
			// XXX: Size is always ignored if ::operator new with extended
			//	alignment is unavailable.
			yunused(bytes);

			if(alignment > 1)
			{
				using hdr_t = void*;
				const auto p_ptr(reinterpret_cast<hdr_t*>(static_cast<byte*>(p)
					- sizeof(hdr_t)));

				yassume(p_ptr);
				::operator delete(*p_ptr);
				p_ptr->~hdr_t();
			}
			else
				::operator delete(p);
#endif
		}
#undef YB_Impl_aligned_new

		bool
		do_is_equal(const memory_resource& other) const ynothrow override
		{
			return this == &other;
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
		do_is_equal(const memory_resource& other) const ynothrow override
		{
			// NOTE: This is required even it can be singleton. See ISO C++
			//	[mem.res.global]/2.
			return this == &other;
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

//! \since build 843
namespace
{

struct slink
{
	tidy_ptr<slink> next;
};

template<typename _type>
struct intrusive_stack
{
	using link_type = slink;
	using link_ptr = tidy_ptr<link_type>;
	using item_ptr = tidy_ptr<_type>;
	link_ptr head{};

	intrusive_stack() = default;
	intrusive_stack(intrusive_stack&&) = default;

	intrusive_stack&
	operator=(intrusive_stack&&) = default;

	static yconstfn link_ptr
	as_link(item_ptr p) ynothrow
	{
		static_assert(is_base_of<link_type, _type>::value,
			"Invalid target type found.");

		return link_ptr(make_observer(static_cast<link_type*>(p.get().get())));
	}

	static yconstfn item_ptr
	as_item(link_ptr p) ynothrow
	{
		static_assert(is_base_of<link_type, _type>::value,
			"Invalid target type found.");

		return item_ptr(make_observer(static_cast<_type*>(p.get().get())));
	}

	YB_ATTR_nodiscard yconstfn bool
	empty() const ynothrow
	{
		return !head;
	}

	yconstfn item_ptr
	top() const ynothrow
	{
		return as_item(head);
	}

	yconstfn_relaxed void
	push(item_ptr p_item) ynothrow
	{
		const auto p_link(as_link(p_item));

		p_link->next = head;
		head = p_link;
	}

	//! \pre \c head 。
	yconstfn_relaxed void
	pop() ynothrow
	{
		const auto p_link(head);

		head = head->next;
		as_item(p_link)->~_type();
	}
};


} // unnamed namespace;

//! \brief 池类型。
struct pool_resource::pool_t final
{
private:
	//! \brief 区块类型。
	class chunk_t final
	{
	private:
		lref<pool_t> pool;
		intrusive_stack<slink> free_blocks{};
		size_t free_count;
		size_t capacity;
		byte* base;
		size_t next_available = 0;

	public:
		chunk_t(pool_t&, void*, size_t) ynothrow;
		chunk_t(chunk_t&&);
		~chunk_t();

		chunk_t&
		operator=(chunk_t&&) ynothrow;

		YB_ALLOCATOR void*
		allocate_block_free();

		YB_ALLOCATOR void*
		allocate_block_seq(size_t);

		void
		add_free(void* p) ynothrow
		{
			free_blocks.push(make_observer(::new(p) slink)),
			++free_count;
		}

		bool
		is_empty() const ynothrow
		{
			return free_count == capacity;
		}

		bool
		is_full() const ynothrow
		{
			return free_count == 0;
		}
	};
	enum : size_t
	{
		default_next_capacity = yimpl(4)
	};
	static_assert(default_next_capacity > 1,
		"Invalid default value found.");
	//! \brief 区块标识类型。
	using id_t = size_t;
	static_assert(is_decayed<id_t>(), "Invalid type found.");
	//! \brief 保存在块末尾的元数据类型。
	struct block_meta_t
	{
		// The types shall be decayed to avoid need of call to %std::launder.
		id_t id;
		//! \invariant \c p_chunk 。
		chunk_t* p_chunk;
	};
	static_assert(is_trivial<block_meta_t>(), "Invalid type found.");
	//! \brief 区块存储对类型。
	using chunk_pr_t = std::pair<const id_t, chunk_t>;
	//! \brief 池资源引用。
	lref<pool_resource> pool_rsrc_ref;
	//! \brief 被池所有的区块。
	map<id_t, chunk_t, greater<>, polymorphic_allocator<chunk_pr_t>> chunks{};
	//! \brief 用于减少复杂度的待分配贮藏指针。
	tidy_ptr<chunk_pr_t> p_stashed{};
	//! \brief 用于减少复杂度的待分配贮藏指针。
	tidy_ptr<chunk_pr_t> p_empty{};
	//! \brief 下一可用的区块中分配块的容量。
	size_t next_capacity = default_next_capacity;
	//! \brief 分配块大小。
	size_t block_size;
	//! \invariant <tt>block_size == 1 << lb_size</tt> 。
	size_t lb_size;

public:
	pool_t(pool_resource&, size_t) ynothrowv;
	pool_t(pool_t&&) ynothrow;
	pool_t&
	operator=(pool_t&&) ynothrow;
	~pool_t();

	//! \sa p_stashed
	YB_ALLOCATOR void*
	allocate();

	static size_t
	adjust_size(size_t bytes, size_t alignment) ynothrow
	{
		return std::max(bytes + sizeof(block_meta_t), alignment);
	}

	void
	deallocate(void*) ynothrowv;

private:
	YB_NONNULL(2) void
	deallocate_chunk(void*, size_t) ynothrow;

	tidy_ptr<chunk_pr_t>
	find_chunk_pr_ptr(id_t id) ynothrowv
	{
		const auto i(chunks.find(id));

		yassume(i != chunks.end());
		return make_observer(&*i);
	}

public:
	//! \pre 参数指向池中块的首字节。
	block_meta_t&
	access_meta(void* p) const ynothrow
	{
		yconstraint(p);

		const auto p_meta(reinterpret_cast<block_meta_t*>(
			static_cast<byte*>(p) + block_size - sizeof(block_meta_t)));

		yassume(is_aligned_ptr(p_meta));
		return *p_meta;
	}

	size_t
	get_lb_size() const ynothrow
	{
		return lb_size;
	}

	size_t
	size_for_capacity(size_t c) const ynothrow
	{
		return c << lb_size;
	}

	memory_resource&
	upstream() ynothrowv
	{
		return pool_rsrc_ref.get().upstream();
	}
};

pool_resource::pool_t::chunk_t::chunk_t(pool_t& pl, void* b, size_t c) ynothrow
	: pool(pl), free_count(c), capacity(c), base(static_cast<byte*>(b))
{}
pool_resource::pool_t::chunk_t::chunk_t(chunk_t&& cnk)
	: pool(cnk.pool), free_blocks(std::move(cnk.free_blocks)),
	free_count(cnk.free_count), capacity(cnk.capacity), base(cnk.base),
	next_available(cnk.next_available)
{
	cnk.base = {};
}
pool_resource::pool_t::chunk_t::~chunk_t()
{
	if(base)
		pool.get().deallocate_chunk(base, capacity);
}

pool_resource::pool_t::chunk_t&
pool_resource::pool_t::chunk_t::operator=(chunk_t&& cnk) ynothrow
{
	std::swap(base, cnk.base),
	yunseq(
	pool = cnk.pool,
	free_blocks = std::move(cnk.free_blocks),
	free_count = cnk.free_count,
	capacity = cnk.capacity,
	next_available = cnk.next_available
	);
	return *this;
}

void*
pool_resource::pool_t::chunk_t::allocate_block_free()
{
	yconstraint(base);
	if(!free_blocks.empty())
	{
		yassume(free_count != 0);

		const auto blk(free_blocks.top().get().get());

		yassume(blk);
		--free_count, free_blocks.pop();
		return blk;
	}
	return {};
}

void*
pool_resource::pool_t::chunk_t::allocate_block_seq(size_t lb_size_v)
{
	yconstraint(base);
	if(next_available < capacity)
	{
		const auto blk(base + (next_available << lb_size_v));

		yunseq(--free_count, ++next_available);
		return blk;
	}
	return {};
}

pool_resource::pool_t::pool_t(pool_resource& pl_rsrc, size_t lb_size_v)
	ynothrowv
	: pool_rsrc_ref(pl_rsrc), block_size(size_t(1) << lb_size_v),
	lb_size(lb_size_v)
{
	yconstraint(block_size > sizeof(block_meta_t));
}
pool_resource::pool_t::pool_t(pool_t&& pool) ynothrow
	: pool_rsrc_ref(pool.pool_rsrc_ref), chunks(std::move(pool.chunks)),
	p_stashed(std::move(pool.p_stashed)), p_empty(std::move(pool.p_empty)),
	next_capacity(ystdex::exchange(pool.next_capacity, default_next_capacity)),
	block_size(pool.block_size), lb_size(pool.lb_size)
{}
// XXX: Order of chunk destruction is unspecified.
pool_resource::pool_t::~pool_t() = yimpl(default);

pool_resource::pool_t&
pool_resource::pool_t::operator=(pool_t&& pool) ynothrow
{
	yunseq(
	pool_rsrc_ref = pool.pool_rsrc_ref,
	chunks = std::move(pool.chunks),
	p_stashed = std::move(pool.p_stashed),
	p_empty = std::move(pool.p_empty),
	next_capacity = ystdex::exchange(pool.next_capacity, default_next_capacity),
	block_size = pool.block_size,
	lb_size = pool.lb_size
	);
	return *this;
}

void
pool_resource::pool_t::deallocate_chunk(void* base, size_t capacity) ynothrow
{
	yconstraint(base);
	upstream().deallocate(base, size_for_capacity(capacity), block_size);
}

pool_resource::pool_resource(
	const pool_options& opts, memory_resource* p_up) ynothrow
	: saved_options(opts), pools((yconstraint(p_up), p_up))
{
	yconstexpr const auto max_blocks_per_chunk_limit(size_t(PTRDIFF_MAX));
	// NOTE: Larger chunks are assumed being better handled by the upstream
	//	resource by default.
	yconstexpr const auto largest_required_pool_block_limit(
		size_t(PTRDIFF_MAX >> yimpl(8)) + 1);
	static_assert(is_power_of_2(largest_required_pool_block_limit),
		"Invalid limit value found.");

	// TODO: Use interval arithmetic libraries.
	if(saved_options.max_blocks_per_chunk - 1 >= max_blocks_per_chunk_limit)
		saved_options.max_blocks_per_chunk = max_blocks_per_chunk_limit;
	if(saved_options.largest_required_pool_block - 1
		>= largest_required_pool_block_limit)
		saved_options.largest_required_pool_block
		= largest_required_pool_block_limit;
	// NOTE: The minimum chunk size should not be smaller than a machine word in
	//	typical implementations.
	else if(saved_options.largest_required_pool_block - 1 < sizeof(void*))
		saved_options.largest_required_pool_block = sizeof(void*);
	else
		// NOTE: Make sure the value is power of 2.
		saved_options.largest_required_pool_block = size_t(1)
			<< ceiling_lb(saved_options.largest_required_pool_block);
}

void*
pool_resource::pool_t::allocate()
{
	return retry_on_cond(ystdex::logical_not<>(), [&]() -> void*{
		void* p = {};

		if(!p_stashed)
		{
			// NOTE: Replenish.
			p_stashed = make_observer(&*chunks.emplace_hint(chunks.begin(),
				chunks.empty() ? 0 : chunks.begin()->first + 1,
				chunk_t(*this, upstream().allocate(size_for_capacity(
				next_capacity), block_size), next_capacity)));
			p_empty = p_stashed;
			// NOTE: See $2018-11 @ %Documentation::Workflow::Annual2018.
			next_capacity = std::min(next_capacity << 1, std::min(size_t(
				PTRDIFF_MAX >> lb_size),
				pool_rsrc_ref.get().saved_options.max_blocks_per_chunk));
		}
		else
			p = p_stashed->second.allocate_block_free();

		auto& cnk(p_stashed->second);

		if(!p)
			p = cnk.allocate_block_seq(lb_size);
		if(p)
		{
			::new(&access_meta(p)) block_meta_t{p_stashed->first, &cnk};
			if(p_stashed == p_empty)
				p_empty = {};
			return p;
		}
		if(p_stashed)
		{
			auto i(chunks.find(p_stashed->first));

			yassume(i != chunks.cend());
			++i;
			if(i != chunks.cend())
				p_stashed = make_observer(&*i);
			else
				p_stashed = {};
		}
		return {};
	});
}

void
pool_resource::pool_t::deallocate(void* p) ynothrowv
{
	auto& meta(access_meta(p));
	const auto p_cnk(meta.p_chunk);

	yassume(p_cnk);

	const bool full(p_cnk->is_full());

	p_cnk->add_free(p);
	if(full)
	{
		if(!p_stashed || p_stashed->first < meta.id)
			p_stashed = find_chunk_pr_ptr(meta.id);
	}
	else if(p_cnk->is_empty())
	{
		if(!p_empty)
			p_empty = find_chunk_pr_ptr(meta.id);
		else
			chunks.erase(std::min(p_empty->first, meta.id));
	}
}

pool_resource::~pool_resource()
{
	release_oversized();
}

void
pool_resource::release() yimpl(ynothrow)
{
	release_oversized();
	pools.clear();
	pools.shrink_to_fit();
}

void*
pool_resource::do_allocate(size_t bytes, size_t alignment)
{
	if(bytes <= saved_options.largest_required_pool_block)
	{
		auto pr(find_pool(bytes, alignment));

		if(!pool_exists(pr))
			pr.first = pools.emplace(pr.first, *this, pr.second);
		return pr.first->allocate();
	}

	auto& upstream_ref(upstream());
	const auto p(upstream_ref.allocate(bytes, alignment));
	const auto gd(make_guard([&]() ynothrow{
		upstream_ref.deallocate(p, bytes, alignment);
	}));

	oversized.emplace(p, oversized_data_t(bytes, alignment));
	return p;
}

void
pool_resource::do_deallocate(void* p, size_t bytes, size_t alignment)
	yimpl(ynothrowv)
{
	if(bytes <= saved_options.largest_required_pool_block)
	{
		const auto pr(find_pool(bytes, alignment));

		if(pool_exists(pr))
			pr.first->deallocate(p);
		else
			yassume(false);
	}
	else
	{
		const auto i(oversized.find(p));

		if(i != oversized.cend())
		{
			upstream().deallocate(i->first, i->second.first, i->second.second);
			oversized.erase(i);
		}
		else
			yassume(false);
	}
}

bool
pool_resource::do_is_equal(const memory_resource& other) const ynothrow
{
	return this == &other;
}

std::pair<pool_resource::pools_t::iterator, size_t>
pool_resource::find_pool(size_t bytes, size_t alignment) ynothrow
{
	const auto lb_size(ceiling_lb(pool_t::adjust_size(bytes, alignment)));

	return {std::lower_bound(pools.begin(), pools.end(), lb_size,
		[](const pool_t& a, size_t lb) ynothrow{
		return a.get_lb_size() < lb;
	}), lb_size};
}

bool
pool_resource::pool_exists(const std::pair<pools_t::iterator, size_t>& pr)
	ynothrow
{
	return pr.first != pools.end() && pr.first->get_lb_size() == pr.second;
}

void
pool_resource::release_oversized() ynothrow
{
	auto& upstream_ref(upstream());

	for(const auto& pr : oversized)
		upstream_ref.deallocate(pr.first, pr.second.first, pr.second.second);
	oversized.clear();
}

} // namespace pmr;

} // namespace ystdex;

