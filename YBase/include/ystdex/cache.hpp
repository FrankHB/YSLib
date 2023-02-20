/*
	© 2013-2016, 2018-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cache.hpp
\ingroup YStandardEx
\brief 高速缓冲容器模板。
\version r843
\author FrankHB <frankhb1989@gmail.com>
\since build 521
\par 创建时间:
	2013-12-22 20:19:14 +0800
\par 修改时间:
	2023-02-20 06:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Cache
*/


#ifndef YB_INC_ystdex_cache_hpp_
#define YB_INC_ystdex_cache_hpp_ 1

#include "allocator.hpp" // for std::pair, yassume, rebind_alloc_t, are_same,
//	nor_, is_convertible, enable_if_convertible_t, std::piecewise_construct_t,
//	std::get, std::piecewise_construct, enable_if_t, head_of_t;
#include <list> // for std::list;
#include "scope_guard.hpp" // for hash, optional_function, std::ref,
//	ystdex::make_unique_guard, ystdex::dismiss;
#include <unordered_map> // for std::unordered_map;
#include <map> // for std::map;
#include "container.hpp" // for ystdex::begin, ystdex::cbegin,
//	ystdex::search_map_by, ystdex::end, ystdex::cend;

namespace ystdex
{

/*!
\brief 使用双向链表实现的最近使用列表。
\note 保证移除项时的顺序为最远端先移除。
\warning 非虚析构。
\since build 968
*/
template<typename _tKey, typename _tMapped,
	class _tBase = std::list<std::pair<const _tKey, _tMapped>>>
class recent_used_list : private _tBase
{
public:
	//! \since build 968
	using base_type = _tBase;
	//! \since build 611
	//!@{
	/*!
	\brief 分配器类型。
	\note 支持 uses-allocator 构造。
	*/
	using allocator_type = typename base_type::allocator_type;
	using value_type = std::pair<const _tKey, _tMapped>;
	using size_type = typename base_type::size_type;
	using iterator = typename base_type::iterator;
	using const_iterator = typename base_type::const_iterator;

	//! \since build 942
	using base_type::base_type;

	using base_type::begin;

	//! \since build 942
	YB_ATTR_nodiscard YB_PURE friend inline iterator
	cast_mutable(recent_used_list& con, const_iterator i) ynothrow
	{
		return con.erase(i, i);
	}

	using base_type::cbegin;

	using base_type::cend;

	using base_type::clear;

	template<typename... _tParams>
	iterator
	emplace(_tParams&&... args)
	{
		base_type::emplace_front(yforward(args)...);
		return begin();
	}

	using base_type::empty;

	using base_type::end;

	using base_type::front;

	iterator
	refresh(iterator i) ynothrowv
	{
		base_type::splice(begin(), *this, i);
		return i;
	}

	template<typename _tMap>
	void
	shrink(_tMap& m) ynothrowv
	{
		yassume(!empty());

		auto& val(base_type::back());

		m.erase(val.first);
		base_type::pop_back();
	}
	template<typename _tMap, typename _func>
	void
	shrink(_tMap& m, _func f)
	{
		yassume(!empty());

		auto& val(base_type::back());

		f(val);
		m.erase(val.first);
		base_type::pop_back();
	}
	template<typename _tMap, typename... _tParams>
	void
	shrink(_tMap& m, size_type max_use, _tParams&&... args)
	{
		while(max_use < size_type(m.size()))
		{
			yassume(!m.empty());
			shrink(m, yforward(args)...);
		}
	}

	//! \since build 942
	void
	undo_emplace() ynothrowv
	{
		yassume(!empty());
		base_type::pop_front();
	}
	//!@}
};


/*!
\ingroup traits
\brief 最近刷新策略的缓存特征。
\since build 968
*/
//!@{
template<typename _tKey, typename _tMapped, typename _fHash = hash<_tKey>,
	class _tList = recent_used_list<_tKey, _tMapped>>
struct used_list_cache_traits
{
	//! \since build 611
	//!@{
	using map_type = std::unordered_map<_tKey, _tMapped, _fHash,
		std::equal_to<_tKey>, typename _tList::allocator_type>;
	using used_list_type = _tList;
	using used_cache_type = std::unordered_map<_tKey, typename _tList::iterator,
		_fHash, typename map_type::key_equal,
		rebind_alloc_t<typename _tList::allocator_type,
		std::pair<const _tKey, typename _tList::iterator>>>;
	//!@}
};

template<typename _tKey, typename _tMapped, class _tList>
struct used_list_cache_traits<_tKey, _tMapped, void, _tList>
{
	//! \since build 611
	//!@{
	using map_type = std::map<_tKey, _tMapped, std::less<_tKey>,
		typename _tList::allocator_type>;
	using used_list_type = _tList;
	using used_cache_type = std::map<_tKey, typename _tList::iterator,
		typename map_type::key_compare,
		rebind_alloc_t<typename _tList::allocator_type,
		std::pair<const _tKey, typename _tList::iterator>>>;
	//!@}
};
//!@}


//! \since build 611
//!@{
/*!
\brief 按最近使用策略刷新的缓存。
\note 默认策略替换最近最少使用的项，保留其它项。
\warning 非虚析构。
\todo 加入异常安全的复制构造函数和复制赋值操作符。
\todo 支持其它刷新策略。
*/
template<typename _tKey, typename _tMapped, typename _fHash = void,
	typename _tTraits = used_list_cache_traits<_tKey, _tMapped, _fHash>>
class used_list_cache
{
public:
	//! \since build 611
	using traits_type = _tTraits;
	using map_type = typename traits_type::map_type;
	using used_list_type = typename traits_type::used_list_type;
	using used_cache_type = typename traits_type::used_cache_type;
	using key_type = typename map_type::key_type;
	//! \since build 604
	using value_type = typename map_type::value_type;
	using size_type = typename map_type::size_type;
	//! \since build 611
	static_assert(are_same<size_type, typename used_list_type::size_type,
		typename used_cache_type::size_type>(), "Invalid size found.");
	//! \since build 968
	using reference = typename map_type::reference;
	//! \since build 968
	using const_reference = typename map_type::const_reference;
	using iterator = typename used_list_type::iterator;
	using const_iterator = typename used_list_type::const_iterator;
		
private:
	//! \since build 942
	template<typename _tParam>
	using not_key_t = nor_<is_convertible<_tParam, const key_type&>,
		is_convertible<_tParam, std::piecewise_construct_t>>;
	//! \since build 968
	template<class _tAlloc>
	using enable_uses_alloc_t = enable_if_t<std::uses_allocator<
		map_type, _tAlloc>::value || std::uses_allocator<
		used_list_type, _tAlloc>::value, int>;

	/*!
	\invariant <tt>std::count(used_list.begin(), used_list.end())
		== used_cache.size()</tt> 。
	*/
	//!@{
	mutable used_list_type used_list;
	used_cache_type used_cache;
	//!@}
	//! \brief 保持可以再增加一个缓存项的最大容量。
	size_type max_use;

public:
	/*!
	\brief 刷新函数。
	\invariant 为空值或目标调用时不抛出异常。
	\warning 若调用抛出异常，使用刷新函数的调用行为可能未定义。
	\since build 942
	*/
	optional_function<void(value_type&)> flush{};

	explicit
	used_list_cache(size_type s = yimpl(15U))
		: used_list(), used_cache(), max_use(s)
	{}
	//! \since build 968
	template<class _tAlloc, yimpl(enable_uses_alloc_t<_tAlloc> = 0)>
	inline
	used_list_cache(size_type s, const _tAlloc& a)
		: used_list(ystdex::make_obj_using_allocator<used_list_type>(a)),
		used_cache(ystdex::make_obj_using_allocator<used_cache_type>(a)),
		max_use(s)
	{}
	//! \note 强异常安全保证依赖模板参数指定的容器转移时都不抛出异常。
	//!@{
	used_list_cache(used_list_cache&&) = default;

	//! \since build 878
	used_list_cache&
	operator=(used_list_cache&&) = default;
	//!@}

private:
	//! \since build 595
	void
	check_max_used() ynothrowv
	{
		used_list.shrink(used_cache, max_use, std::ref(flush));
	}

public:
	YB_ATTR_nodiscard YB_PURE size_type
	get_max_use() const ynothrow
	{
		return max_use;
	}

	//! \since build 595
	void
	set_max_use(size_type s) ynothrowv
	{
		max_use = s < 1 ? 1 : s;
		check_max_used();
	}

	//! \since build 611
	//!@{
	YB_ATTR_nodiscard YB_PURE iterator
	begin() ynothrow
	{
		return ystdex::begin(used_list);
	}
	YB_ATTR_nodiscard YB_PURE iterator
	begin() const ynothrow
	{
		return ystdex::begin(used_list);
	}
	//!@}

	//! \since build 942
	YB_ATTR_nodiscard YB_PURE friend inline iterator
	cast_mutable(used_list_cache& con, const_iterator i) ynothrow
	{
		return cast_mutable(con.used_list, i);
	}

	void
	clear() ynothrow
	{
		used_list.clear(),
		used_cache.clear();
	}

	template<typename... _tParams>
	inline std::pair<iterator, bool>
	emplace(const key_type& k, _tParams&&... args)
	{
		return emplace_with_key(k, k, yforward(args)...);
	}
	//! \since build 942
	template<typename _tParam, typename... _tParams>
	inline yimpl(enable_if_convertible_t)<_tParam, const key_type&,
		std::pair<iterator, bool>>
	emplace(std::piecewise_construct_t, std::tuple<_tParam> arg,
		std::tuple<_tParams...> args)
	{
		return emplace_with_key(std::get<0>(arg), std::piecewise_construct,
			std::move(arg), std::move(args));
	}
	//! \since build 942
	template<typename... _tParams>
	yimpl(enable_if_t)<not_key_t<head_of_t<_tParams...>>{},
		std::pair<iterator, bool>>
	emplace(_tParams&&... args)
	{
		check_max_used();

		const auto i(used_list.emplace(yforward(args)...));
		auto gd(ystdex::make_unique_guard([&]() ynothrowv{
			used_list.undo_emplace();
		}));
		const auto pr(used_cache.emplace(i->first, i));

		if(pr.second)
		{
			ystdex::dismiss(gd);
			return {i, true};
		}
		return {pr.first->second, false};
	}

private:
	//! \since build 942
	template<typename... _tParams>
	std::pair<iterator, bool>
	emplace_with_key(const key_type& k, _tParams&&... args)
	{
		check_max_used();

		const auto pr(ystdex::search_map_by(
			[&](typename used_cache_type::const_iterator i){
			const auto j(used_list.emplace(yforward(args)...));
			auto gd(ystdex::make_unique_guard([&]() ynothrowv{
				used_list.undo_emplace();
			}));
			const auto r(used_cache.emplace_hint(i, k, j));

			ystdex::dismiss(gd);
			return r;
		}, used_cache, k));

		return {pr.first->second, pr.second};
	}
	
public:
	//! \since build 611
	//!@{
	YB_ATTR_nodiscard YB_PURE iterator
	end() ynothrow
	{
		return ystdex::end(used_list);
	}
	YB_ATTR_nodiscard YB_PURE iterator
	end() const ynothrow
	{
		return ystdex::end(used_list);
	}
	//!@}

	YB_ATTR_nodiscard iterator
	find(const key_type& k)
	{
		const auto i(used_cache.find(k));

		return i != used_cache.end() ? used_list.refresh(i->second) : end();
	}
	YB_ATTR_nodiscard const_iterator
	find(const key_type& k) const
	{
		const auto i(used_cache.find(k));

		return i != used_cache.end() ? used_list.refresh(i->second) : end();
	}

	//! \since build 646
	//!@{
	YB_ATTR_nodiscard YB_PURE const used_cache_type&
	get() const ynothrow
	{
		return used_cache;
	}

	YB_ATTR_nodiscard YB_PURE const used_list_type&
	list() const ynothrow
	{
		return used_list;
	}
	//!@}

	//! \since build 611
	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return used_cache.size();
	}
};
//!@}


/*!
\brief 以指定的关键字查找关联容器访问对应的元素并按需初始化其中的条目。
\tparam _tMap 映射类型，可以是 std::map 、std::unordered_map
	或 ystdex::used_list_cache 等。
\note 若没有找到使用指定的可调用对象和参数初始化内容。
\note 使用 ADL emplace_hint_in_place 。
\sa emplace_hint_in_place
\sa ystdex::try_emplace
\since build 521
*/
template<class _tMap, typename _tKey, typename _func, typename... _tParams>
auto
cache_lookup(_tMap& m, const _tKey& k, _func init, _tParams&&... args)
	-> decltype((m.begin()->second))
{
	return ystdex::search_map_by([&](typename _tMap::const_iterator i){
		// XXX: Blocked. 'yforward' may cause G++ 5.2 silent crash.
		return emplace_hint_in_place(m, i, k,
			init(std::forward<_tParams>(args)...));
	}, m, k).first->second;
}

} // namespace ystdex;

#endif

