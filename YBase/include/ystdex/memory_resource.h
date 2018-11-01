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
\version r497
\author FrankHB <frankhb1989@gmail.com>
\since build 842
\par 创建时间:
	2018-10-27 19:30:12 +0800
\par 修改时间:
	2018-11-02 03:45 +0800
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
包括以下已有其它实现支持的 ISO C++17 后的修改：
LWG 2961 ：不需要考虑无法实现的后置条件。
LWG 2969 ：明确 polymorphic_allocator 的 construct 函数模板使用 *this 而不是
	memory_resource 进行构造。
LWG 2975 ：明确 polymorphic_allocator 的 construct 函数模板排除对第一个参数为
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
struct piecewise_args
{
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) inline auto
	concat(true_, _tAlloc& a, std::tuple<_tParams...>&& tp)
		-> decltype(dispatch2(is_constructible<_type, std::allocator_arg_t,
		_tAlloc&, _tParams...>(), a, std::move(tp)))
	{
		return concat2(is_constructible<_type, std::allocator_arg_t,
			_tAlloc&, _tParams...>(), a, std::move(tp));
	}
	template<class, typename... _tParams>
	YB_ATTR(always_inline) inline std::tuple<_tParams...>&&
	concat(false_, _tAlloc&, std::tuple<_tParams...>&& tp)
	{
		return std::move(tp);
	}

	template<typename... _tParams>
	YB_ATTR(always_inline) inline auto
	concat2(true_, _tAlloc& a, std::tuple<_tParams...>&& tp)
		-> decltype(std::tuple_cat(std::tuple<std::allocator_arg_t, _tAlloc&>(
		std::allocator_arg, a), std::move(tp)))
	{
		return std::tuple_cat(std::tuple<std::allocator_arg_t, _tAlloc&>(
			std::allocator_arg, a), std::move(tp));
	}
	template<typename... _tParams>
	YB_ATTR(always_inline) inline auto
	concat2(false_, _tAlloc& a, std::tuple<_tParams...>&& tp)
		-> decltype(std::tuple_cat(std::move(tp), std::tuple<_tAlloc&>(a)))
	{
		return std::tuple_cat(std::move(tp), std::tuple<_tAlloc&>(a));
	}
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
		dispatch2(typename is_constructible<_type, std::allocator_arg_t,
			_tAllocInner&, _tParams...>::type(), p, yforward(args)...);
	}
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) inline void
	dispatch(false_, _type* p, _tParams&&... args)
	{
		static_assert(is_constructible<_type, _tParams...>::value,
			"Failed to meet requirement from [allocator.uses.construction]/1.");

		allocator_traits<_tAllocOuter>::construct(outer, p, yforward(args)...);
	}

private:
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) inline void
	dispatch2(true_, _type* p, _tParams&&... args)
	{
		allocator_traits<_tAllocOuter>::construct(outer, p,
			std::allocator_arg, inner, yforward(args)...);
	}
	template<typename _type, typename... _tParams>
	YB_ATTR(always_inline) inline void
	dispatch2(false_, _type* p, _tParams&&... args)
	{
		static_assert(is_constructible<_type, _tParams...,
			_tAllocInner&>::value,
			"Failed to meet requirement from [allocator.uses.construction]/1.");

		allocator_traits<_tAllocOuter>::construct(outer, p, yforward(args)...,
			inner);
	}

public:
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


//! \note 和 Microsoft VC++ 实现中的 _Get_size_of_n 类似。
//@{
template<size_t _vSize>
inline size_t
get_size_of_n(size_t n)
{
	size_t res(n * _vSize);

	return size_t(static_cast<size_t>(-1) / _vSize) < n ? size_t(-1) : res;
}
template<> inline
size_t
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
		return do_allocate(bytes, alignment);
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

	virtual bool
	do_is_equal(const memory_resource&) const ynothrow = 0;
};


YB_API YB_ATTR_nodiscard YB_ATTR_returns_nonnull memory_resource*
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

	polymorphic_allocator&
	operator=(const polymorphic_allocator&) = default;

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

	template<typename _type1, typename... _tParams>
	YB_NONNULL(2) yimpl(enable_if_t<!std::is_same<vseq::_a<std::pair>,
		vdefer<vseq::ctor_of_t, _type>>::value>)
	construct(_type1* p, _tParams&&... args)
	{
		fallback_alloc_t a;

		nested_dispatcher(a, *this).dispatch(std::uses_allocator<_type,
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

YB_API YB_ATTR_nodiscard memory_resource*
new_delete_resource() ynothrow;

YB_API YB_ATTR_nodiscard memory_resource*
null_memory_resource() ynothrow;

//! \see LWG 2961 。
YB_API YB_ATTR_returns_nonnull memory_resource*
set_default_resource(memory_resource*) ynothrow;


struct YB_API pool_options
{
	size_t max_blocks_per_chunk = 0;
	size_t largest_required_pool_block = 0;
};
#endif

} // inline namespace cpp2017;

} // namespace pmr;

} // namespace std;

#endif

