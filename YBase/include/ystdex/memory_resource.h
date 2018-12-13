/*
	© 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory_resource.h
\ingroup YStandardEx
\brief 存储资源。
\version r799
\author FrankHB <frankhb1989@gmail.com>
\since build 842
\par 创建时间:
	2018-10-27 19:30:12 +0800
\par 修改时间:
	2018-12-02 16:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::MemoryResource

提供 ISO C++17 标准库头 <memory_resource> 兼容的替代接口和实现。
除了部分关系操作使用 operators 实现而不保留命名空间内的声明及散列支持提供偏特化外，
其它接口同 std::pmr 中的对应接口。
注意因为一些兼容问题， std::experimental::pmr 中的接口不被可选地使用，
	即使其可用性仍然会被检查。
和原始的 std::experimental::pmr 中提供的接口及其实现有以下不同：
LWG 2724 ：纯虚函数为 private 而不是 protected 。
LWG 2843 ：成员 do_allocate 对不支持的对齐值直接抛出异常而非回退 std::max_align 。
WG21 P0337R0 ：polymorphic_allocator 的默认 operator= 定义为 = delete 。 
包括以下已有其它实现支持的 ISO C++17 后的修改：
LWG 2961 ：不需要考虑无法实现的后置条件。
LWG 2969 ：明确 polymorphic_allocator 的 construct 函数模板使用 *this 而不是
	memory_resource 进行构造。
LWG 2975 ：明确 polymorphic_allocator 的 construct 函数模板排除对第一参数为
	pair 实例的指针类型时的重载。
LWG 3000 ：存储资源类的 do_is_equal 的实现没有冗余的 dynamic_cast 。
LWG 3036 ：删除 polymorphic_allocator 的 destroy 函数模板，
	因为 allocator_traits 已提供实现。
LWG 3037 : 明确 polymorphic_allocator 支持不完整的值类型。
LWG 3038 ：在 polymorphic_allocator 的 allocate 函数处理整数溢出。
LWG 3113 ：明确 polymorphic_allocator 的 construct 函数模板转移构造的元组值。
*/


#ifndef YB_INC_ystdex_memory_resource_h_
#define YB_INC_ystdex_memory_resource_h_ 1

#include "memory.hpp" // for internal "memory.hpp", is_constructible,
//	std::allocator_arg_t, std::allocator_arg, std::uses_allocator, yforward,
//	allocator_traits, std::pair, std::piecewise_construct, size_t, yalignof,
//	yconstraint, enable_if_t, vdefer, vseq::ctor_of_t, vseq::_a,
//	std::piecewise_construct_t;
// NOTE: See "placement.hpp" for comments on inclusion conditions.
#if (YB_IMPL_MSCPP >= 1910 && _MSVC_LANG >= 201603) \
	|| (__cplusplus >= 201603L && __has_include(<memory_resource>))
#	include <memory_resource>
#	if (YB_IMPL_MSCPP >= 1910 && _MSVC_LANG >= 201603) \
	|| __cpp_lib_memory_resource >= 201603L
#		define YB_Has_memory_resource 1
#	endif
#elif __cplusplus > 201402L && __has_include(<experimental/memory_resource>)
// NOTE: For exposition only. The implementations in libstdc++ and libc++ are
//	incomplete as of ISO C++17 is published.
#	include <experimental/memory_resource>
#	if __cpp_lib_experimental_memory_resources >= 201402L
#		define YB_Has_memory_resource 2
#	endif
#endif
#if YB_Has_memory_resource != 1
#	include <tuple> // for std::tuple, std::forward_as_tuple, std::tuple_cat;
#	include "operators.hpp" // for equality_comparable;
#endif
#include "base.h" // for noncopyable, nonmovable;
#include "cstdint.hpp" // for is_power_of_2;
#include <vector> // for std::vector;
#include <unordered_map> // for std::hash, std::unordered_map;
#include <functional> // for std::equal_to;
#if YB_Has_memory_resource != 1
#	if (defined(__GLIBCXX__) && !(defined(_GLIBCXX_USE_C99_STDINT_TR1) \
	&& defined(_GLIBCXX_HAS_GTHREADS))) \
	|| (defined(_LIBCPP_VERSION) && defined(_LIBCPP_HAS_NO_THREADS))
// XXX: The synchonization does not work. However, this still makes
//	%synchronized_pool_resource different than %unsynchronized_pool_resource in
//	%ystdex::pmr. Preserving %ystdex::single_thread pseudo implementation
//	introduces some basic checks of sanity on mutex types.
#		include "pseudo_mutex.h" // for ystdex::single_thread::mutex,
#		define YB_Impl_mutex_ns ystdex::single_thread
//	ystdex::single_thread::lock_guard;
#	else
#		include <mutex> // for std::mutex, std::lock_guard;
#		define YB_Impl_mutex_ns std
#	endif
#	include "type_pun.hpp" // for pun_ref;
#endif

/*!
\brief \<memory_resource\> 特性测试宏。
\see WG21 P0941R2 2.2 。
\see https://docs.microsoft.com/en-us/cpp/visual-cpp-language-conformance 。
\since build 842
*/
//@{
#ifndef __cpp_lib_memory_resource
#	if (YB_IMPL_MSCPP >= 1910 && _MSVC_LANG >= 201603) || __cplusplus >= 201603L
#		define __cpp_lib_memory_resource 201603L
#	endif
#endif
//@}

//! \since build 842
namespace ystdex
{

#if YB_Has_memory_resource != 1
namespace details
{

// TODO: Wait for WG21 P0591 adopted in the working draft and migrate the
//	interface.

template<class _tAlloc>
class piecewise_args
{
	//! \since build 843
	//@{
private:
	template<typename... _tParams>
	YB_ATTR(always_inline) static inline auto
	alloc_concat(true_, _tAlloc& a, std::tuple<_tParams...>&& tp)
		-> decltype(std::tuple_cat(std::tuple<std::allocator_arg_t, _tAlloc&>(
		std::allocator_arg, a), std::move(tp)))
	{
		return std::tuple_cat(std::tuple<std::allocator_arg_t, _tAlloc&>(
			std::allocator_arg, a), std::move(tp));
	}
	template<typename... _tParams>
	YB_ATTR(always_inline) static inline auto
	alloc_concat(false_, _tAlloc& a, std::tuple<_tParams...>&& tp)
		-> decltype(std::tuple_cat(std::move(tp), std::tuple<_tAlloc&>(a)))
	{
		return std::tuple_cat(std::move(tp), std::tuple<_tAlloc&>(a));
	}

public:
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) static inline auto
	concat(true_, _tAlloc& a, std::tuple<_tParams...>&& tp)
		-> decltype(alloc_concat(is_constructible<_type, std::allocator_arg_t,
		_tAlloc&, _tParams...>(), a, std::move(tp)))
	{
		return alloc_concat(is_constructible<_type, std::allocator_arg_t,
			_tAlloc&, _tParams...>(), a, std::move(tp));
	}
	template<class, typename... _tParams>
	YB_ATTR(always_inline) static inline std::tuple<_tParams...>&&
	concat(false_, _tAlloc&, std::tuple<_tParams...>&& tp)
	{
		return std::move(tp);
	}
	//@}
};

template<class _tAllocOuter, class _tAllocInner>
class allocator_construct
{
private:
	using inner_args = piecewise_args<_tAllocInner>;
	_tAllocOuter& outer;
	_tAllocInner& inner;

public:
	allocator_construct(_tAllocOuter& o, _tAllocInner& i)
		: outer(o), inner(i)
	{}

private:
	//! \since build 843
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) inline void
	alloc_dispatch(true_, _type* p, _tParams&&... args)
	{
		allocator_traits<_tAllocOuter>::construct(outer, p,
			std::allocator_arg, inner, yforward(args)...);
	}
	//! \since build 843
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) inline void
	alloc_dispatch(false_, _type* p, _tParams&&... args)
	{
		static_assert(is_constructible<_type, _tParams..., _tAllocInner&>(),
			"Failed to meet requirement from [allocator.uses.construction]/1.");

		allocator_traits<_tAllocOuter>::construct(outer, p, yforward(args)...,
			inner);
	}

	template<typename _type, typename... _types>
	YB_ATTR(always_inline) inline auto
	compcat(std::tuple<_types...>&& val)
		-> decltype(inner_args::template concat<_type>(std::uses_allocator<
		_type, _tAllocInner>(), inner, std::move(val)))
	{
		return inner_args::template concat<_type>(std::uses_allocator<_type,
			_tAllocInner>(), inner, std::move(val));
	}

public:
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) inline void
	dispatch(true_, _type* p, _tParams&&... args)
	{
		alloc_dispatch(is_constructible<_type, std::allocator_arg_t,
			_tAllocInner&, _tParams...>(), p, yforward(args)...);
	}
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) inline void
	dispatch(false_, _type* p, _tParams&&... args)
	{
		static_assert(is_constructible<_type, _tParams...>(),
			"Failed to meet requirement from [allocator.uses.construction]/1.");

		allocator_traits<_tAllocOuter>::construct(outer, p, yforward(args)...);
	}

	template<typename _type1, typename _type2, typename... _types1,
		typename... _types2>
	YB_ATTR(always_inline) inline void
	dispatch_pair(std::pair<_type1, _type2>* p, std::tuple<_types1...>&& val1,
		std::tuple<_types2...>&& val2)
	{
		allocator_traits<_tAllocOuter>::construct(outer, p,
			std::piecewise_construct, compcat<_type1>(std::move(val1)),
			compcat<_type2>(std::move(val2)));
	}
};


//! \note 和 Microsoft VC++ 实现中的 %_Get_size_of_n 类似。
//@{
template<size_t _vSize>
YB_ATTR_nodiscard YB_PURE inline size_t
get_size_of_n(size_t n)
{
	size_t res(n * _vSize);

	return size_t(static_cast<size_t>(-1) / _vSize) < n ? size_t(-1) : res;
}
template<>
YB_ATTR_nodiscard YB_PURE inline size_t
get_size_of_n<1>(size_t n)
{
	return n;
}
//@}

} // namespace details;
#endif

namespace pmr
{

inline namespace cpp2017
{

#if YB_Has_memory_resource == 1
using std::pmr::memory_resource;

using std::pmr::polymorphic_allocator;

using std::pmr::new_delete_resource;
using std::pmr::null_memory_resource;
using std::pmr::set_default_resource;
using std::pmr::get_default_resource;

using std::pmr::pool_options;
#else
// NOTE: Microsoft VC++ 15.8.2 implements ISO C++17 version (using WG21 N4700
//	suggested by the assertion messages), while current libstdc++ and libc++
//	only provide %std::experimental::fundamentals_v2 version, although some
//	resoultions have applied on libc++.

//! \ingroup YBase_replacement_features
//@{
//! \see LWG 2724 。
class YB_API YB_ATTR_novtable memory_resource
	: private equality_comparable<memory_resource>
{
private:
	static constexpr size_t max_align = yalignof(std::max_align_t);

public:
	// XXX: Microsoft VC++, libstdc++ and libc++ all defined the destructor
	//	inline. Keeping it out-of-line for style consistency and to avoid
	//	Clang++ warning [-Wweak-vtables] reilably.
	//! \brief 虚析构：类定义外默认实现。
	virtual
	~memory_resource();

	YB_ATTR_nodiscard friend bool
	operator==(const memory_resource& a, const memory_resource& b) ynothrow
	{
		return &a == &b || a.is_equal(b);
	}

	/*!
	\pre 断言：对齐值是 2 的整数次幂。
	\post 断言：返回值符合参数指定的对齐值要求。
	*/
	YB_ALLOCATOR void*
	allocate(size_t bytes, size_t alignment = max_align)
	{
		// NOTE: This is not specified directly by the specification, but the
		//	resolution of LWG 2843 has made the requirement on alighment value
		//	same to %do_allocation. Microsoft VC++ 15.8.2 implements it using
		//	an assertion here similarly. Libc++ experimental does not assert and
		//	does silently round up the alignment value which is always the
		//	maximul alignment in its %__resource_adaptor_imp::do_allocate.
		yconstraint(is_power_of_2(alignment));

		const auto p(do_allocate(bytes, alignment));

		yassume(is_aligned_ptr(p, alignment));
		return p;
	}

	void
	deallocate(void* p, size_t bytes, size_t alignment = max_align)
	{
		// NOTE: The alignment requirement is same to %allocate.
		yconstraint(is_power_of_2(alignment));
		return do_deallocate(p, bytes, alignment);
	}

	YB_ATTR_nodiscard bool
	is_equal(const memory_resource& other) const ynothrow
	{
		// NOTE: See LWG 2843.
		return do_is_equal(other);
	}

	// XXX: LWG 2724 is implemented by Microsoft VC++ 15.8.2. Not sure which is
	//	the initial version as it seems undocumented.
	// XXX: LWG 2724 is NOT implemented in libstdc++ and libc++ in
	//	%std::experimental currently. It is not likely to be because the issue
	//	is resolved after adoption of P0220R1.
private:
	//! \sa LWG 2843 。
	YB_ALLOCATOR virtual void*
	do_allocate(size_t, size_t) = 0;

	virtual void
	do_deallocate(void*, size_t, size_t) = 0;

	YB_ATTR_nodiscard virtual bool
	do_is_equal(const memory_resource&) const ynothrow = 0;
};


YB_ATTR_nodiscard YB_API YB_ATTR_returns_nonnull memory_resource*
get_default_resource() ynothrow;


template<class _type>
class polymorphic_allocator
{
public:
	using value_type = _type;

private:
	// NOTE: The defualt allocator would serve to construct object without
	//	allocator. The allocator is used to construct object via fallback
	//	%construct in %allocator_traits so allocation is not used. The choice of
	//	%std::allocator<char> is based on the fact that it would be almost
	//	always instantiated (e.g. by %std::string exception messages) so this is
	//	efficient in translation.
	using fallback_alloc_t = std::allocator<yimpl(char)>;
	using nested_dispatcher
		= details::allocator_construct<fallback_alloc_t, polymorphic_allocator>;

	//! \invariant \c memory_rsrc 。
	memory_resource* memory_rsrc;

public:
	polymorphic_allocator() ynothrow
		: memory_rsrc(get_default_resource())
	{}

	//! \pre 断言：参数指针非空。
	YB_NONNULL(2)
	polymorphic_allocator(memory_resource* r)
		: memory_rsrc(r)
	{
		yconstraint(r);
	}
	polymorphic_allocator(const polymorphic_allocator& other) = default;
	template<typename _tOther>
	polymorphic_allocator(const polymorphic_allocator<_tOther>& other) ynothrow
		: memory_rsrc(other.resource())
	{}

	//! \since build 843
	polymorphic_allocator&
	operator=(const polymorphic_allocator&) = delete;

	//! \see LWG 3038 。
	YB_ALLOCATOR _type*
	allocate(size_t n)
	{
		return static_cast<_type*>(memory_rsrc->allocate(
			details::get_size_of_n<sizeof(_type)>(n), yalignof(_type)));
	}

	void
	deallocate(_type* p, size_t n)
	{
		memory_rsrc->deallocate(p, details::get_size_of_n<sizeof(_type)>(n),
			yalignof(_type));
	}

	template<typename _tObj, typename... _tParams>
	YB_NONNULL(2) yimpl(enable_if_t<!std::is_same<vseq::_a<std::pair>,
		vdefer<vseq::ctor_of_t, _tObj>>::value>)
	construct(_tObj* p, _tParams&&... args)
	{
		fallback_alloc_t a;

		nested_dispatcher(a, *this).dispatch(std::uses_allocator<_tObj,
			polymorphic_allocator>(), p, yforward(args)...);
	}
	template<typename _type1, typename _type2, typename... _types1,
		typename... _types2>
	YB_NONNULL(2) void
	construct(std::pair<_type1, _type2>* p, std::piecewise_construct_t,
		std::tuple<_types1...> x, std::tuple<_types2...> y)
	{
		fallback_alloc_t a;

		nested_dispatcher(a, *this).dispatch_pair(p, std::move(x),
			std::move(y));
	}
	template<typename _type1, typename _type2>
	YB_NONNULL(2) void
	construct(std::pair<_type1, _type2>* p)
	{
		construct(p, std::piecewise_construct, std::tuple<>(),
			std::tuple<>());
	}
	template<typename _type1, typename _type2, typename _tOther1,
		typename _tOther2>
	YB_NONNULL(2) void
	construct(std::pair<_type1, _type2>* p, _tOther1&& x, _tOther2&& y)
	{
		construct(p, std::piecewise_construct,
			std::forward_as_tuple(yforward(x)),
			std::forward_as_tuple(yforward(y)));
	}
	template<typename _type1, typename _type2, typename _tOther,
		typename _tOther2>
	YB_NONNULL(2) void
	construct(std::pair<_type1, _type2>* p, const std::pair<_tOther, _tOther2>& pr)
	{
		construct(p, std::piecewise_construct,
			std::forward_as_tuple(pr.first), std::forward_as_tuple(pr.second));
	}
	template<typename _type1, typename _type2, typename _tOther1,
		typename _tOther2>
	YB_NONNULL(2) void
	construct(std::pair<_type1, _type2>* p, std::pair<_tOther1, _tOther2>&& pr)
	{
		construct(p, std::piecewise_construct,
			std::forward_as_tuple(yforward(pr.first)),
			std::forward_as_tuple(yforward(pr.second)));
	}

	YB_ATTR_nodiscard polymorphic_allocator
	select_on_container_copy_construction() const
	{
		return {};
	}

	YB_ATTR_nodiscard memory_resource*
	resource() const
	{
		return memory_rsrc;
	}
};

template<typename _type1, typename _type2>
YB_ATTR_nodiscard inline bool
operator==(const polymorphic_allocator<_type1>& a,
	const polymorphic_allocator<_type2>& b) ynothrow
{
	return *a.resource() == *b.resource();
}

template<typename _type1, typename _type2>
YB_ATTR_nodiscard inline bool
operator!=(const polymorphic_allocator<_type1>& a,
	const polymorphic_allocator<_type2>& b) ynothrow
{
	return !(a == b);
}

YB_ATTR_nodiscard YB_API memory_resource*
new_delete_resource() ynothrow;

YB_ATTR_nodiscard YB_API memory_resource*
null_memory_resource() ynothrow;

//! \see LWG 2961 。
YB_API YB_ATTR_returns_nonnull memory_resource*
set_default_resource(memory_resource*) ynothrow;


/*!
\note 实现定义：默认限制参见池的构造函数说明。
\see WG21 N4778 [mem.res.pool.options] 。
*/
struct YB_API pool_options
{
	size_t max_blocks_per_chunk = 0;
	size_t largest_required_pool_block = 0;
};
//@}
#endif

} // inline namespace cpp2017;

/*!
\brief 池资源。
\ingroup YBase_replacement_extensions
\since build 843

接口同 ISO C++17 的 std::pmr::unsynchronized_pool_resource ，
	但保证能在上游的分配器分配的区块为空时去配。
*/
class YB_API pool_resource : public memory_resource,
	private yimpl(noncopyable), private yimpl(nonmovable)
{
private:
	class pool_t;
	using pools_t = std::vector<pool_t, polymorphic_allocator<pool_t>>;
	using oversized_data_t = std::pair<size_t, size_t>;
	pool_options saved_options;
	std::unordered_map<void*, oversized_data_t, std::hash<void*>, std::equal_to<
		void*>, polymorphic_allocator<std::pair<void* const, oversized_data_t>>>
		oversized{};
	pools_t pools;

public:
	/*!
	\note 实现定义：每区块最大块数限制为 \c size_t(PTRDIFF_MAX) 。
	\note 实现定义：最大块分配大小限制为 \c size_t(PTRDIFF_MAX >> 8) + 1 。
	*/
	//@{
	pool_resource() ynothrow
		: pool_resource(pool_options(), get_default_resource())
	{}
	//! \pre 断言：指针参数非空。
	YB_NONNULL(3)
	pool_resource(const pool_options&, memory_resource*)
		ynothrow;
	//! \pre 间接断言：指针参数非空。
	explicit
	pool_resource(memory_resource* upstream)
		: pool_resource(pool_options(), upstream)
	{}
	explicit
	pool_resource(const pool_options& opts)
		: pool_resource(opts, get_default_resource())
	{}
	//@}
	~pool_resource() override;

	void
	release() yimpl(ynothrow);

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE memory_resource*
	upstream_resource() const yimpl(ynothrow)
	{
		return pools.get_allocator().resource();
	}

	YB_ATTR_nodiscard YB_PURE pool_options
	options() const yimpl(ynothrow)
	{
		return saved_options;
	}

protected:
	YB_ALLOCATOR void*
	do_allocate(size_t, size_t) override;

	void
	do_deallocate(void*, size_t, size_t) yimpl(ynothrowv) override;

	YB_ATTR_nodiscard yimpl(YB_PURE) bool
	do_is_equal(const memory_resource&) const ynothrow override;

private:
	YB_ATTR_nodiscard YB_PURE std::pair<pools_t::iterator, size_t>
	find_pool(size_t, size_t) ynothrow;

	YB_ATTR_nodiscard YB_PURE bool
	pool_exists(const std::pair<pools_t::iterator, size_t>&) ynothrow;

	void
	release_oversized() ynothrow;

	YB_ATTR_nodiscard YB_PURE memory_resource&
	upstream() ynothrowv
	{
		const auto p_upstream(upstream_resource());

		yassume(p_upstream);
		return *p_upstream;
	}
};

inline namespace cpp2017
{

// XXX: %std::pmr::synchronized_pool_resource and
//	%std::pmr::unsynchronized_pool_resource can be monotonic until %release or
//	destruction. Although they are not both in %std implementation of Microsoft
//	VC++ 15.8.2 and in %ystdex where both implement deallocation of upstream
//	blocks in %do_deallocate, this is not specified by ISO C++.

#if YB_Has_memory_resource == 1
using std::pmr::synchronized_pool_resource;
using std::pmr::unsynchronized_pool_resource;
using std::pmr::monotonic_buffer_resource;
#else
//! \ingroup YBase_replacement_features
//@{
//! \since build 845
class YB_API synchronized_pool_resource : yimpl(public pool_resource)
{
private:
	using mutex = YB_Impl_mutex_ns::mutex;
	template<typename _tMutex>
	using lock_guard = YB_Impl_mutex_ns::lock_guard<_tMutex>;

	mutable mutex mtx{};

public:
	yimpl(using) pool_resource::pool_resource;
	//! \brief 虚析构：类定义外默认实现。
	~synchronized_pool_resource() override;

	void
	release() yimpl(ynothrow)
	{
		lock_guard<mutex> gd(mtx);

		pool_resource::release();
	}

protected:
	YB_ALLOCATOR void*
	do_allocate(size_t bytes, size_t alignment) override
	{
		lock_guard<mutex> gd(mtx);

		return pool_resource::do_allocate(bytes, alignment);
	}

	void
	do_deallocate(void* p, size_t bytes, size_t alignment) yimpl(ynothrowv)
		override
	{
		lock_guard<mutex> gd(mtx);

		pool_resource::do_deallocate(p, bytes, alignment);
	}
};


yimpl(using) unsynchronized_pool_resource = pool_resource;


//! \since build 845
class YB_API monotonic_buffer_resource : public memory_resource,
	private yimpl(noncopyable), private yimpl(nonmovable)
{
private:
	struct chunks_t;
	memory_resource* upstream_rsrc;
	void* current_buffer = {};
	size_t next_buffer_size;
	size_t space_available = 0;
	ystdex::aligned_storage_t<16> chunks_data{};
	//! \invariant <tt>&pun.get() == &chunks_data</tt>
	pun_ref<chunks_t> pun;

public:
	//! \note 实现定义：未指定时，初始大小为不小于 4 * sizeof(size_t) 的定值。
	//@{
	monotonic_buffer_resource()
		: monotonic_buffer_resource(get_default_resource())
	{}
	explicit
	monotonic_buffer_resource(memory_resource*) yimpl(ynothrow);
	monotonic_buffer_resource(size_t, memory_resource*) yimpl(ynothrow);
	monotonic_buffer_resource(void*, size_t, memory_resource*) yimpl(ynothrow);
	explicit
	monotonic_buffer_resource(size_t initial_size) yimpl(ynothrow)
		: monotonic_buffer_resource(initial_size, get_default_resource())
	{}
	monotonic_buffer_resource(void* buffer, size_t buffer_size) yimpl(ynothrow)
		: monotonic_buffer_resource(buffer, buffer_size, get_default_resource())
	{}
	//@}
	~monotonic_buffer_resource() override;

	void
	release() yimpl(ynothrow);

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE memory_resource*
	upstream_resource() const yimpl(ynothrow)
	{
		return upstream_rsrc;
	}

protected:
	YB_ALLOCATOR void*
	do_allocate(size_t, size_t) override;

	//! \note 实现定义：增长因子为 2 。
	void
	do_deallocate(void*, size_t, size_t) override
	{}

	YB_ATTR_nodiscard yimpl(YB_PURE) bool
	do_is_equal(const memory_resource&) const ynothrow override;
};
//@}

#	undef YB_Impl_mutex_ns

#endif

} // inline namespace cpp2017;

} // namespace pmr;

} // namespace ystdex;

#endif

