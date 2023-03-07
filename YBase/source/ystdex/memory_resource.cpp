/*
	© 2018-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory_resource.cpp
\ingroup YStandardEx
\brief 存储资源。
\version r1882
\author FrankHB <frankhb1989@gmail.com>
\since build 842
\par 创建时间:
	2018-10-27 19:30:12 +0800
\par 修改时间:
	2023-02-25 07:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::MemoryResource
*/


#include "ystdex/memory_resource.h" // for __cpp_aligned_new, is_trivial,
//	std::align_val_t, std::align, __cpp_sized_deallocation, std::bad_alloc,
//	::operator new, ::operator delete, std::unique_ptr, make_observer, YAssert,
//	lref, yassume, ystdex::destruct_in, yverify, yconstraint, CHAR_BIT,
//	is_power_of_2_positive, ceiling_lb, std::swap, std::piecewise_construct,
//	std::forward_as_tuple, PTRDIFF_MAX, ystdex::aligned_store_cast;
#if YB_Has_memory_resource != 1
#	include <atomic> // for std::atomic;
#endif
#include "ystdex/pointer.hpp" // for tidy_ptr;
#include "ystdex/scope_guard.hpp" // for make_unique_guard, ystdex::dismiss;
#include "ystdex/algorithm.hpp" // for std::min, std::max,
//	ystdex::lower_bound_n;

namespace ystdex
{

namespace pmr
{

#if __cpp_aligned_new >= 201606L
#	define YB_Impl_aligned_new true
#else
//! \since build 845
//!@{
struct hdr_t
{
	// NOTE: The types shall be decayed to avoid need of call to
	//	%std::launder.
	void* p_block;
};

static_assert(is_trivial<hdr_t>(), "Invalid type found.");
using offset_n_t = size_t_<ystdex::max(sizeof(hdr_t), yalignof(hdr_t))>;
//!@}
#endif

#define YB_Impl_do_is_equal_impl(_virt, _name_pfx) \
	bool \
	_name_pfx do_is_equal(const memory_resource& other) const ynothrow _virt \
	{ \
		return this == &other; \
	}
#define YB_Impl_do_is_equal(_virt, _name_pfx) \
	YB_ATTR_nodiscard yimpl(YB_STATELESS) \
		YB_Impl_do_is_equal_impl(_virt, _name_pfx)

inline namespace cpp2017
{

#if YB_Has_memory_resource != 1
memory_resource::~memory_resource() = default;


memory_resource*
new_delete_resource() ynothrow
{
	static new_delete_resource_t r;

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

		// NOTE: This is required even it can be singleton. See ISO C++
		//	[mem.res.global]/2.
		YB_Impl_do_is_equal(override, )
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

	static YB_PURE yconstfn link_ptr
	as_link(item_ptr p) ynothrow
	{
		static_assert(is_base_of<link_type, _type>(),
			"Invalid target type found.");

		return link_ptr(make_observer(static_cast<link_type*>(p.get().get())));
	}

	static YB_PURE yconstfn item_ptr
	as_item(link_ptr p) ynothrow
	{
		static_assert(is_base_of<link_type, _type>(),
			"Invalid target type found.");

		return item_ptr(make_observer(static_cast<_type*>(p.get().get())));
	}

	YB_ATTR_nodiscard YB_PURE yconstfn bool
	empty() const ynothrow
	{
		return !head;
	}

	YB_PURE yconstfn item_ptr
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

#if YB_Has_memory_resource != 1
//! \since build 845
//!@{
struct monobuf_header final : slink
{
	size_t size;
	size_t alignment;

	monobuf_header(size_t s, size_t a)
		: size(s), alignment(a)
	{}
};

enum : size_t
{
	mono_alloc_min = 2 * sizeof(monobuf_header),
	mono_alloc_max = 0 - yalignof(monobuf_header),
	mono_scale = 2,
	mono_scale_div = 1
};

yconstfn size_t
monobuf_scale(size_t size, size_t max_size) ynothrow
{
	// NOTE: Scale size to %max_size, rounding up to a multiple of alignment of
	//	header, saturation to %mono_alloc_max.
	// XXX: This should be round up when 'mono_scale % mono_scale_div != 0'.
	return size >= max_size ? mono_alloc_max : ((size * mono_scale
		/ mono_scale_div + yalignof(monobuf_header) - 1) & mono_alloc_max);
}
yconstfn size_t
monobuf_scale(size_t size) ynothrow
{
	// NOTE: Scale size by 'mono_scale / mono_scale_div'.
	return monobuf_scale(size, (mono_alloc_max - yalignof(monobuf_header) + 1)
		/ mono_scale * mono_scale_div);
}
//!@}
#endif

} // unnamed namespace;

void*
new_delete_resource_t::do_allocate(size_t bytes, size_t alignment)
{
#if YB_Impl_aligned_new
	return alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__
		? ::operator new(bytes, std::align_val_t(alignment))
		: ::operator new(bytes);
#else
	if(alignment > 1)
	{
		// NOTE: The checks are necessary to prevent wrapping of the results of
		//	'+'. See also https://gcc.gnu.org/bugzilla/show_bug.cgi?id=19351.
		if(YB_LIKELY(bytes + alignment > bytes))
		{
			auto space(offset_n_t() + bytes + alignment);

			// NOTE: Ditto.
			if(space > offset_n_t())
			{
				// XXX: Without the checks, there may be G++ warning:
				//	[-Walloc-size-larger-than=] when this function is
				//	effectively inlined (by LTO, etc.).
				auto ptr(make_unique_default_init<byte[]>(space));
				void* p(&ptr[offset_n_t()]);

				if(std::align(alignment, bytes, p, space))
				{
					yassume(p);
					yassume(static_cast<byte*>(p) >= ptr.get());

					const auto off(size_t(static_cast<byte*>(p) - ptr.get()));

					yassume(off >= offset_n_t());
					// XXX: See
					//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70834.
					(::new(&ptr[off - offset_n_t()]) hdr_t)->p_block
						= ptr.get();
					ptr.release();
					return p;
				}
			}
		}
		// NOTE: If the wrapping checks fail or there is no suitable aligned
		//	storage available, the allocation fails. No exception matching
		//	%std::bad_array_new_length is thrown even it is caused by the
		//	wrapping failure, since it is never thrown in an allocation function
		//	in the default case specified by ISO C++.
		throw std::bad_alloc();
	}
	return ::operator new(bytes);
#endif
}

void
new_delete_resource_t::do_deallocate(void* p, size_t bytes, size_t alignment)
	ynothrow
{
#if YB_Impl_aligned_new
	if(alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
#	if __cpp_sized_deallocation >= 201309L
		::operator delete(p, bytes, std::align_val_t(alignment));
#	else
		::operator delete(p, std::align_val_t(alignment));
#	endif
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
		// TODO: Blocked. Use [[assume_aligned]]? See WG21 P0886R0.
		const auto p_hdr(static_cast<hdr_t*>(static_cast<void*>(
			static_cast<byte*>(yaligned(p, alignment)) - offset_n_t())));

		yassume(p_hdr);

		const auto p_block(p_hdr->p_block);

		yassume(p_block);

		std::unique_ptr<byte[]> ptr(static_cast<byte*>(p_block));

		p_hdr->~hdr_t();
	}
	else
		::operator delete(p);
#endif
}

YB_Impl_do_is_equal_impl(, new_delete_resource_t::)


void
adjust_pool_options(pool_options& opts)
{
	yconstexpr const auto
		max_blocks_per_chunk_limit(size_t((sizeof(size_t) * CHAR_BIT) << 16U));
	// NOTE: Larger chunks are assumed being better handled by the upstream
	//	resource by default.
	yconstexpr const auto largest_required_pool_block_limit(
		size_t((sizeof(size_t) * CHAR_BIT) << 18U));
	static_assert(is_power_of_2_positive(largest_required_pool_block_limit),
		"Invalid limit value found.");

	if(opts.max_blocks_per_chunk - 1 >= max_blocks_per_chunk_limit)
		opts.max_blocks_per_chunk = max_blocks_per_chunk_limit;
	if(opts.largest_required_pool_block - 1
		>= largest_required_pool_block_limit)
		opts.largest_required_pool_block = largest_required_pool_block_limit;
	// NOTE: The minimum chunk size should not be smaller than a machine word in
	//	typical implementations.
	else if(opts.largest_required_pool_block - 1 < sizeof(void*))
		opts.largest_required_pool_block = sizeof(void*);
	else
		// NOTE: Make sure the value is power of 2.
		opts.largest_required_pool_block = size_t(1)
			<< ceiling_lb(opts.largest_required_pool_block);
}


oversized_map::~oversized_map()
{
	release();
}

void*
oversized_map::allocate(size_t bytes, size_t alignment)
{
	auto& upstream_ref(upstream());
	const auto p(upstream_ref.allocate(bytes, alignment));
	auto gd(make_unique_guard([&]() ynothrow{
		upstream_ref.deallocate(p, bytes, alignment);
	}));

	entries.emplace(p, oversized_map::mapped_type(bytes, alignment));
	ystdex::dismiss(gd);
	return p;
}

void
oversized_map::deallocate(void* p, size_t bytes, size_t alignment)
	yimpl(ynothrowv)
{
	const auto i(entries.find(p));

	YAssert(i != entries.cend(),
		"Entry not found in oversized deallocation.");
	YAssert(bytes == i->second.first,
		"Mismatched bytes found in oversized deallocation.");
	YAssert(alignment == i->second.second,
		"Mismatched alignments found in oversized deallocation.");
	upstream().deallocate(i->first, bytes, alignment);
	entries.erase(i);
}

void
oversized_map::release() ynothrow
{
	auto& upstream_ref(upstream());

	for(const auto& pr : entries)
		upstream_ref.deallocate(pr.first, pr.second.first, pr.second.second);
	entries.clear();
}


//! \since build 863
class resource_pool::chunk_t final
{
private:
	//! \since build 845
	lref<memory_resource> mem_rsrc;
	//! \since build 843
	//!@{
	size_t block_size;
	intrusive_stack<slink> free_blocks{};
	size_t free_count;
	size_t capacity;
	byte* base;
	size_t next_available = 0;

public:
	chunk_t(resource_pool& pl, void* b, size_t c) ynothrow
		: mem_rsrc(pl.upstream()), block_size(pl.get_block_size()),
		free_count(c), capacity(c), base(static_cast<byte*>(b))
	{}
	chunk_t(chunk_t&& cnk)
		: mem_rsrc(cnk.mem_rsrc), block_size(cnk.block_size),
		free_blocks(std::move(cnk.free_blocks)), free_count(cnk.free_count),
		capacity(cnk.capacity), base(cnk.base),
		next_available(cnk.next_available)
	{
		cnk.base = {};
	}
	~chunk_t()
	{
		if(base)
		{
			for(size_t idx(next_available - 1); idx + 1 != 0; --idx)
				ystdex::destruct_in(access_meta(base + idx * block_size,
					block_size).i_chunk);
			mem_rsrc.get().deallocate(base, capacity * block_size, block_size);
		}
	}

	chunk_t&
	operator=(chunk_t&&) ynothrow;

	YB_ALLOCATOR YB_ATTR_returns_nonnull void*
	allocate_block_free()
	{
		yverify(base);
		yverify(!free_blocks.empty());
		yverify(free_count != 0);

		const auto blk(free_blocks.top().get().get());

		yassume(blk);
		--free_count, free_blocks.pop();
		return blk;
	}

	/*!
	\pre 断言：\c base 。
	\note 参数表示分配的区块大小。
	\since build 864
	*/
	YB_ALLOCATOR void*
	allocate_block_seq()
	{
		yverify(base);
		if(next_available < capacity)
		{
			const auto blk(base + next_available * block_size);

			yunseq(--free_count, ++next_available);
			return blk;
		}
		return {};
	}

	void
	add_free(void* p) ynothrow
	{
		free_blocks.push(make_observer(::new(p) slink)),
		++free_count;
	}

	//! \since build 864
	YB_ATTR_nodiscard YB_PURE bool
	has_free_block() const ynothrow
	{
		return !free_blocks.empty();
	}

	YB_ATTR_nodiscard YB_PURE bool
	is_empty() const ynothrow
	{
		return free_count == capacity;
	}

	YB_ATTR_nodiscard YB_PURE bool
	is_full() const ynothrow
	{
		return free_count == 0;
	}
	//!@}
};

resource_pool::chunk_t&
resource_pool::chunk_t::operator=(chunk_t&& cnk) ynothrow
{
	std::swap(base, cnk.base),
	yunseq(
	mem_rsrc = cnk.mem_rsrc,
	block_size = cnk.block_size,
	free_blocks = std::move(cnk.free_blocks),
	free_count = cnk.free_count,
	capacity = cnk.capacity,
	next_available = cnk.next_available
	);
	return *this;
}

resource_pool::resource_pool(memory_resource& up_rsrc, size_t mbpc,
	size_t blk_size, size_t extra, size_t n_cap) ynothrowv
	: max_blocks_per_chunk(mbpc), chunks(&up_rsrc), i_stashed(chunks.end()),
	i_backup(i_stashed), next_capacity((yconstraint(n_cap > 1), n_cap)),
	block_size((yconstraint(blk_size >= resource_pool::min_block_size),
	blk_size)), extra_data(extra)
{
	yconstraint(block_size > sizeof(block_meta_t));
}
resource_pool::resource_pool(resource_pool&& pl) ynothrow
	: max_blocks_per_chunk(pl.max_blocks_per_chunk),
	chunks(pl.chunks.get_allocator()), i_stashed(pl.i_stashed),
	i_backup(pl.i_backup), next_capacity(pl.next_capacity),
	block_size(pl.block_size), extra_data(pl.extra_data)
{
	const bool no_stashed(pl.i_stashed == pl.chunks.end()),
		no_empty(pl.i_backup == pl.chunks.end());

	std::swap(chunks, pl.chunks);
	if(no_stashed)
		i_stashed = chunks.end();
	if(no_empty)
		i_backup = chunks.end();
}
// XXX: The order destruction among chunks is unspecified.
resource_pool::~resource_pool() = yimpl(default);

resource_pool&
resource_pool::operator=(resource_pool&& pl) ynothrowv
{
	yconstraint(chunks.get_allocator() == pl.chunks.get_allocator());

	const bool no_stashed(pl.i_stashed == pl.chunks.end()),
		no_empty(pl.i_backup == pl.chunks.end());

	std::swap(chunks, pl.chunks);
	yunseq(
	i_stashed = no_stashed ? chunks.end() : pl.i_stashed,
	i_backup = no_empty ? chunks.end() : pl.i_backup,
	max_blocks_per_chunk = pl.max_blocks_per_chunk,
	next_capacity = pl.next_capacity,
	block_size = pl.block_size,
	extra_data = pl.extra_data
	);
	return *this;
}

// XXX: This makes the generated code more compact.
void*
resource_pool::allocate()
{
	while(true)
	{
		if(YB_UNLIKELY(i_stashed == chunks.end()))
		{
			// NOTE: Replenish.
			auto& up_rsrc(upstream());
			const auto del([&](void* p) ynothrow{
				up_rsrc.deallocate(p, next_capacity * block_size, block_size);
			});
			std::unique_ptr<void, decltype(del)> gd(
				up_rsrc.allocate(next_capacity * block_size, block_size), del);

			chunks.emplace_front(std::piecewise_construct,
				std::forward_as_tuple(chunks.empty() ? 0
				: chunks.begin()->first + 1),
				std::forward_as_tuple(*this, gd.get(), next_capacity));
			gd.release();
			yunseq(i_stashed = chunks.begin(), i_backup = chunks.end());
			// NOTE: See $2018-11 @ %Documentation::Workflow.
			next_capacity = std::min(next_capacity << 1, max_blocks_per_chunk);
		}
		else if(i_stashed->second.has_free_block())
			return i_stashed->second.allocate_block_free();
		if(const auto p = i_stashed->second.allocate_block_seq())
		{
			::new(&access_meta(p, block_size)) block_meta_t{i_stashed};
			return p;
		}
		yassume(i_stashed != chunks.end());
		++i_stashed;
	}
}

void
resource_pool::clear() ynothrow
{
	yunseq(i_backup = chunks.end(), i_stashed = chunks.end(),
		next_capacity = default_capacity);
	chunks.clear();
}

// NOTE: See $2022-05 @ %Documentation::Workflow.
#if defined(NDEBUG) && YB_IMPL_GNUCPP >= 90100
YB_ATTR(optimize("Os"))
#endif
void
resource_pool::deallocate(void* p) ynothrowv
{
	auto i_chunk(access_meta(p, block_size).i_chunk);

	yverify(i_chunk != chunks.end());
	yverify(i_stashed != chunks.end());

	auto& cnk(i_chunk->second);

	if(YB_UNLIKELY(cnk.is_full()))
	{
		if(YB_UNLIKELY(i_stashed->first < i_chunk->first))
			i_stashed = i_chunk;
	}
	// XXX: For some reason, x86_64-pc-linux G++ 12.1 '-O3' generated worse code
	//	around this for the copy of %tidy_ptr instance by one more redundant
	//	'mov QWORD PTR [rsp+8], rsi' instruction saving the old value of
	//	'p_link->next' (in rsi) in %intrusive_stack::push to the memory before
	//	it is overwritten by %head. That saved value is then never used. OTOH,
	//	x86_64-pc-linux Clang++ 13.0.1 always move the %free_count value from
	//	and to the memory in the call to %add_free.
	cnk.add_free(p);
	if(YB_UNLIKELY(cnk.is_empty() && i_backup != i_chunk))
	{
		if(i_backup != chunks.end() && i_backup->second.is_empty())
		{
			if(i_backup->first < i_chunk->first)
				std::swap(i_chunk, i_backup);
			if(i_stashed == i_chunk)
				i_stashed = i_backup;
			yverify(i_chunk->second.is_empty());
			chunks.erase(i_chunk);
		}
		else
			i_backup = i_chunk;
	}
}


pool_resource::pool_resource(const pool_options& opts, memory_resource* p_up)
	: saved_options(opts), oversized((yconstraint(p_up), *p_up)), pools(p_up)
{
	adjust_pool_options(saved_options);
}
pool_resource::~pool_resource() = default;

void
pool_resource::release() yimpl(ynothrow)
{
	oversized.release();
	pools.clear();
}

void*
pool_resource::do_allocate(size_t bytes, size_t alignment)
{
	if(bytes <= saved_options.largest_required_pool_block)
	{
		auto pr(find_pool(bytes, alignment));

		if(!pool_exists(pr))
			pr.first = pools.emplace(pr.first, upstream(), std::min(size_t(
				PTRDIFF_MAX >> pr.second), saved_options.max_blocks_per_chunk),
				size_t(1) << pr.second, pr.second);
		return pr.first->allocate();
	}
	return oversized.allocate(bytes, alignment);
}

void
pool_resource::do_deallocate(void* p, size_t bytes, size_t alignment)
	yimpl(ynothrowv)
{
	if(bytes <= saved_options.largest_required_pool_block)
	{
		const auto pr(find_pool(bytes, alignment));

		yverify(pool_exists(pr));
		pr.first->deallocate(p);
	}
	else
		return oversized.deallocate(p, bytes, alignment);
}

YB_Impl_do_is_equal_impl(, pool_resource::)

std::pair<pool_resource::pools_t::iterator, size_t>
pool_resource::find_pool(size_t bytes, size_t alignment) ynothrow
{
	const auto
		lb_size(ceiling_lb(resource_pool::adjust_for_block(bytes, alignment)));

	return {ystdex::lower_bound_n(pools.begin(),
		pools_t::difference_type(pools.size()), lb_size, [] YB_LAMBDA_ANNOTATE(
		(const resource_pool& a, size_t lb), ynothrow, pure){
		return a.get_extra_data() < lb;
	}), lb_size};
}

bool
pool_resource::pool_exists(const std::pair<pools_t::iterator, size_t>& pr)
	ynothrow
{
	return pr.first != pools.end() && pr.first->get_extra_data() == pr.second;
}

#if YB_Has_memory_resource != 1
inline namespace cpp2017
{

synchronized_pool_resource::~synchronized_pool_resource() = yimpl(default);


struct monotonic_buffer_resource::chunks_t
{
	intrusive_stack<monobuf_header> chunks{};
};

monotonic_buffer_resource::monotonic_buffer_resource(memory_resource* upstream)
	yimpl(ynothrow)
	: upstream_rsrc(upstream), next_buffer_size(mono_alloc_min),
	pun(&chunks_data)
{}
monotonic_buffer_resource::monotonic_buffer_resource(size_t initial_size,
	memory_resource* upstream) yimpl(ynothrow)
	: upstream_rsrc(upstream), next_buffer_size([](size_t size) ynothrow{
		// NOTE: Since 'mono_alloc_max == -yalignof(monobuf_header)',
		//	'size < mono_alloc_max' implies that
		//	'size + yalignof(monobuf_header) - 1' will always representable in
		//	the range of %size_t.
		return size < mono_alloc_min ? mono_alloc_min : (size >= mono_alloc_max
			? mono_alloc_max : ((size + yalignof(monobuf_header) - 1)
			& mono_alloc_max));
	}(initial_size)), pun(&chunks_data)
{}
monotonic_buffer_resource::monotonic_buffer_resource(void* buffer,
	size_t buffer_size, memory_resource* const upstream) yimpl(ynothrow)
	: upstream_rsrc(upstream), current_buffer(buffer),
	next_buffer_size(buffer_size ? monobuf_scale(buffer_size)
	: mono_alloc_min), space_available(buffer_size), pun(&chunks_data)
{}
monotonic_buffer_resource::~monotonic_buffer_resource()
{
	release();
}

void
monotonic_buffer_resource::release() yimpl(ynothrow)
{
	intrusive_stack<monobuf_header> tmp{};

	std::swap(tmp, pun.get().chunks);
	while(const auto p = tmp.top().get().get())
	{
		tmp.pop();
		// NOTE: The header is stored at the end of the allocated memory block.
		upstream_rsrc->deallocate(const_cast<char*>(ystdex::aligned_store_cast<
			const char*>(p + 1) - p->size), p->size, p->alignment);
	}
}

void*
monotonic_buffer_resource::do_allocate(size_t bytes, size_t alignment)
{
	// NOTE: Ensure the result is different to previously return values, to
	//	meet to ISO C++ [basic.stc.dynamic.allocation]/2 (implied by
	//	[mem.res.private]/2). This is needed only when no upstream resource
	//	handling the request, so it is typically unnecessary for other
	//	memory resources. See also LWG 3637.
	// XXX: This is done correctly in libstdc++, but not Microsoft VC++. See
	//	https://github.com/microsoft/STL/issues/2609.
	if(YB_UNLIKELY(bytes == 0))
		bytes = 1;
	if(!std::align(alignment, bytes, current_buffer, space_available))
	{
		if(bytes <= mono_alloc_max - sizeof(monobuf_header))
		{
			auto new_size(next_buffer_size);
			const auto new_size_req(bytes + sizeof(monobuf_header));

			if(new_size < new_size_req)
			{
				new_size = (new_size_req + yalignof(monobuf_header) - 1)
					& mono_alloc_max;
			}

			const auto new_align(std::max(yalignof(monobuf_header), alignment));
			auto new_buffer(upstream_rsrc->allocate(new_size, new_align));

			current_buffer = new_buffer;
			space_available = new_size - sizeof(monobuf_header);
			new_buffer = static_cast<char*>(new_buffer) + space_available;
			pun.get().chunks.push(make_observer(::new(new_buffer)
				monobuf_header(new_size, new_align)));
			next_buffer_size = monobuf_scale(new_size);
		}
		else
		{
			// XXX: ISO C++ [mem.res.monotonic.buffer.mem]/6 does not allow
			//	throwing here. Request impossible large size and alignment
			//	to ask the failure from the upstream resource. See also the
			//	issue above.
			// XXX: The size is not 'size_t(-1)' to avoid G++ warning:
			//	[-Walloc-size-larger-than=].
			// XXX: The size is not restricted at runtime, but values greater
			//	than '1 << 29' are not supported by LLVM. See
			//	https://bugs.llvm.org/show_bug.cgi?id=43638. The support in LLVM
			//	IR has been changed since LLVM 14 (see
			//	https://releases.llvm.org/14.0.0/docs/ReleaseNotes.html#changes-to-the-llvm-ir),
			//	but just keep the assumption conservative.
#if YB_IMPL_CLANGPP >= 100000
	YB_Diag_Push
	YB_Diag_Ignore(builtin-assume-aligned-alignment)
#endif
			yunused(upstream_rsrc->allocate(size_t(-1) >> 1,
				~(size_t(-1) >> 1)));
#if YB_IMPL_CLANGPP >= 100000
	YB_Diag_Pop
#endif
			yassume(false);
		}
	}

	const auto res(current_buffer);

	yunseq(
	current_buffer = static_cast<char*>(current_buffer) + bytes,
	space_available -= bytes
	);
	return res;
}

YB_Impl_do_is_equal(, monotonic_buffer_resource::)

} // inline namespace cpp2017;

#endif

#undef YB_Impl_do_is_equal
#undef YB_Impl_do_is_equal_impl

#undef YB_Impl_aligned_new

} // namespace pmr;

} // namespace ystdex;

