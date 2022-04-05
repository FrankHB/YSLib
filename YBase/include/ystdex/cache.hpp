/*
	© 2013-2016, 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cache.hpp
\ingroup YStandardEx
\brief 高速缓冲容器模板。
\version r748
\author FrankHB <frankhb1989@gmail.com>
\since build 521
\par 创建时间:
	2013-12-22 20:19:14 +0800
\par 修改时间:
	2022-03-22 18:11 +0800
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
#include "scope_guard.hpp" // for std::hash, optional_function, std::ref,
//	ystdex::unique_guard, ystdex::dismiss;
#include <unordered_map> // for std::unordered_map;
#include <map> // for std::map;
#include "container.hpp" // for ystdex::begin, ystdex::cbegin,
//	ystdex::search_map_by, ystdex::end, ystdex::cend;

namespace ystdex
{

//! \since build 611
//@{
/*!
\brief 使用双向链表实现的最近使用列表。
\note 保证移除项时的顺序为最远端先移除。
\warning 非虚析构。
*/
template<typename _tKey, typename _tMapped,
	class _tAlloc = std::allocator<std::pair<const _tKey, _tMapped>>>
class recent_used_list : private std::list<std::pair<const _tKey, _tMapped>>
{
public:
	/*!
	\brief 分配器类型。
	\note 支持 uses-allocator 构造。
	*/
	using allocator_type = _tAlloc;
	using value_type = std::pair<const _tKey, _tMapped>;
	using list_type = std::list<value_type, allocator_type>;
	using size_type = typename list_type::size_type;
	using const_iterator = typename list_type::const_iterator;
	using iterator = typename list_type::iterator;

	//! \since build 942
	using list_type::list_type;

	using list_type::begin;

	//! \since build 942
	YB_ATTR_nodiscard YB_PURE friend inline iterator
	cast_mutable(recent_used_list& con, const_iterator i) ynothrow
	{
		return con.erase(i, i);
	}

	using list_type::cbegin;

	using list_type::cend;

	using list_type::clear;

	template<typename... _tParams>
	iterator
	emplace(_tParams&&... args)
	{
		list_type::emplace_front(yforward(args)...);
		return begin();
	}

	using list_type::empty;

	using list_type::end;

	using list_type::front;

	iterator
	refresh(iterator i) ynothrowv
	{
		list_type::splice(begin(), *this, i);
		return i;
	}

	template<typename _tMap>
	void
	shrink(_tMap& m) ynothrowv
	{
		yassume(!empty());

		auto& val(list_type::back());

		m.erase(val.first);
		list_type::pop_back();
	}
	template<typename _tMap, typename _func>
	void
	shrink(_tMap& m, _func f)
	{
		yassume(!empty());

		auto& val(list_type::back());

		f(val);
		m.erase(val.first);
		list_type::pop_back();
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
		list_type::pop_front();
	}
};


/*!
\brief 最近刷新策略的缓存特征。
\ingroup traits
*/
//@{
template<typename _tKey, typename _tMapped, typename _fHash = std::hash<_tKey>,
	class _tAlloc = std::allocator<std::pair<const _tKey, _tMapped>>,
	class _tList = recent_used_list<_tKey, _tMapped, _tAlloc>>
struct used_list_cache_traits
{
	using map_type = std::unordered_map<_tKey, _tMapped, _fHash,
		std::equal_to<_tKey>, _tAlloc>;
	using used_list_type = _tList;
	using used_cache_type = std::unordered_map<_tKey, typename _tList::iterator,
		_fHash, typename map_type::key_equal, rebind_alloc_t<_tAlloc,
		std::pair<const _tKey, typename _tList::iterator>>>;
};

template<typename _tKey, typename _tMapped, class _tAlloc, class _tList>
struct used_list_cache_traits<_tKey, _tMapped, void, _tAlloc, _tList>
{
	using map_type = std::map<_tKey, _tMapped, std::less<_tKey>, _tAlloc>;
	using used_list_type = _tList;
	using used_cache_type = std::map<_tKey, typename _tList::iterator,
		typename map_type::key_compare, rebind_alloc_t<_tAlloc,
		std::pair<const _tKey, typename _tList::iterator>>>;
};
//@}


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
	using key_type = typename map_type::key_type;
	//! \since build 604
	using value_type = typename map_type::value_type;
	using size_type = typename map_type::size_type;
	using used_list_type = typename traits_type::used_list_type;
	using used_cache_type = typename traits_type::used_cache_type;
	using const_iterator = typename used_list_type::const_iterator;
	using iterator = typename used_list_type::iterator;
	//! \since build 611
	static_assert(are_same<size_type, typename used_list_type::size_type,
		typename used_cache_type::size_type>(), "Invalid size found.");
		
private:
	//! \since build 942
	template<typename _tParam>
	using not_key_t = nor_<is_convertible<_tParam, const key_type&>,
		is_convertible<_tParam, std::piecewise_construct_t>>;

	/*!
	\invariant <tt>std::count(used_list.begin(), used_list.end())
		== used_cache.size()</tt> 。
	*/
	//@{
	mutable used_list_type used_list;
	used_cache_type used_cache;
	//@}
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
	//! \note 强异常安全保证依赖模板参数指定的容器转移时都不抛出异常。
	//@{
	used_list_cache(used_list_cache&&) = default;

	//! \since build 878
	used_list_cache&
	operator=(used_list_cache&&) = default;
	//@}

private:
	//! \since build 595
	void
	check_max_used() ynothrowv
	{
		used_list.shrink(used_cache, max_use, std::ref(flush));
	}

public:
	size_type
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
	//@{
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
	//@}

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
	yimpl(enable_if_t)<not_key_t<head_of_t<_tParams...>>::value,
		std::pair<iterator, bool>>
	emplace(_tParams&&... args)
	{
		check_max_used();

		const auto i(used_list.emplace(yforward(args)...));
		auto gd(ystdex::unique_guard([&]() ynothrowv{
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
			[&](typename used_cache_type::const_iterator j){
			const auto i(used_list.emplace(yforward(args)...));
			auto gd(ystdex::unique_guard([&]() ynothrowv{
				used_list.undo_emplace();
			}));
			const auto r(used_cache.emplace_hint(j, k, i));

			ystdex::dismiss(gd);
			return r;
		}, used_cache, k));

		return {pr.first->second, pr.second};
	}
	
public:
	//! \since build 611
	//@{
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
	//@}

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
	//@{
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
	//@}

	//! \since build 611
	YB_ATTR_nodiscard YB_PURE size_type
	size() const ynothrow
	{
		return used_cache.size();
	}
};
//@}


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

