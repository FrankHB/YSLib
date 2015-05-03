/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cache.hpp
\ingroup YStandardEx
\brief 高速缓冲容器模板。
\version r304
\author FrankHB <frankhb1989@gmail.com>
\since build 521
\par 创建时间:
	2013-12-22 20:19:14 +0800
\par 修改时间:
	2015-05-02 04:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Cache
*/


#ifndef YB_INC_ystdex_cache_hpp_
#define YB_INC_ystdex_cache_hpp_ 1

#include "deref_op.hpp" // for std::pair, is_undereferenceable;
#include <unordered_map> // for std::unordered_map;
#include <list> // for std::list;
#include <stdexcept> // for std::runtime_error;

namespace ystdex
{

/*!
\brief 按最近最多使用策略刷新的缓存。
\since build 521
\todo 加入异常安全的复制构造函数和复制赋值操作符。
\todo 支持其它刷新策略。
*/
template<typename _tKey, typename _tMapped, typename _fHash = std::hash<_tKey>,
	typename _fEqual = std::equal_to<_tKey>,
	class _tAlloc = std::allocator<std::pair<const _tKey, _tMapped>>>
class used_list_cache
	: private std::unordered_map<_tKey, _tMapped, _fHash, _fEqual, _tAlloc>
{
public:
	using map_type
		= std::unordered_map<_tKey, _tMapped, _fHash, _fEqual, _tAlloc>;
	using used_list_type
		= std::list<_tKey, typename _tAlloc::template rebind<_tKey>::other>;
	using used_cache_type = std::unordered_multimap<_tKey,
		typename used_list_type::iterator, _fHash, _fEqual,
		typename _tAlloc::template rebind<std::pair<const _tKey,
		typename used_list_type::iterator>>::other>;
	using typename map_type::const_iterator;
	using typename map_type::iterator;
	using typename map_type::key_type;
	using typename map_type::size_type;

private:
	mutable used_list_type used_list;
	used_cache_type used_cache;
	//! \brief 保持可以再增加一个缓存项的最大容量。
	size_type max_use;

public:
	explicit
	used_list_cache(size_type s = yimpl(15U))
		: map_type(),
		used_list(), used_cache(), max_use(s)
	{}
	used_list_cache(used_list_cache&&) = default;

private:
	//! \since build 595
	void
	check_max_used() ynothrowv
	{
		while(max_use < used_list.size())
		{
			yassume(!used_list.empty()),
			yassume(!used_cache.empty());

			const auto& key(used_list.front());

			map_type::erase(key),
			used_cache.erase(key);
			used_list.pop_front();
		}
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

	using map_type::begin;

	void
	clear() ynothrow
	{
		map_type::clear(),
		used_list.clear(),
		used_cache.clear();
	}

	template<typename... _tParams>
	std::pair<typename map_type::iterator, bool>
	emplace(_tParams&&... args)
	{
		check_max_used();

		const auto pr(map_type::emplace(yforward(args)...));

		if(pr.second)
		{
			yassume(!is_undereferenceable(pr.first));

			const auto& k(pr.first->first);
			const auto i(used_list.insert(used_list.end(), k));

			try
			{
				used_cache.emplace(k, i);
			}
			catch(...)
			{
				yassume(!used_list.empty());
				map_type::erase(pr.first),
				used_list.pop_back();
				throw;
			}
		}
		return pr;
	}

	using map_type::end;

	iterator
	find(const key_type& k)
	{
		const auto i(used_cache.find(k));

		if(i != used_cache.end())
		{
			used_list.splice(used_list.end(), used_list, i->second);
			return map_type::find(k);
		}
		return map_type::end();
	}
	const_iterator
	find(const key_type& k) const
	{
		const auto i(used_cache.find(k));

		if(i != used_cache.end())
		{
			used_list.splice(used_list.end(), used_list, i->second);
			return map_type::find(k);
		}
		return map_type::end();
	}

	using map_type::size;
};


/*!
\brief 以指定的关键字查找作为缓存的无序关联容器，
	若没有找到使用指定的可调用对象和参数初始化内容。
\tparam _tMap 映射类型，可以是 std::map_type 、 std::unordered_map
	或 ystdex::used_list_cache 等。
\since build 521
*/
template<class _tMap, typename _tKey, typename _func, typename... _tParams>
auto
cache_lookup(_tMap& cache, const _tKey& key, _func init, _tParams&&... args)
	-> decltype((cache.begin()->second))
{
	auto i(cache.find(key));

	if(i == cache.end())
	{
		const auto pr(cache.emplace(key, init(yforward(args)...)));

		if(YB_UNLIKELY(!pr.second))
			throw std::runtime_error("Cache insertion failed.");
		i = pr.first;
	}
	return i->second;
}

} // namespace ystdex;

#endif

