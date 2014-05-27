/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Cache.h
\ingroup Core
\brief 高速缓冲模板。
\version r204
\author FrankHB <frankhb1989@gmail.com>
\since build 461
\par 创建时间:
	2013-12-22 20:19:14 +0800
\par 修改时间:
	2014-05-23 10:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::Cache
*/


#ifndef YSL_INC_Core_Cache_hpp_
#define YSL_INC_Core_Cache_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition
#include <ystdex/iterator.hpp> // for ystdex::is_undereferenceable;
#include YFM_YSLib_Adaptor_YContainer

namespace YSLib
{

/*!
\brief 按最近最多使用策略刷新的缓存。
\since build 461
\todo 加入异常安全的复制构造函数。
\todo 扩展为 MLU 列表。
*/
template<typename _tKey, typename _tMapped, typename _fHash = std::hash<_tKey>,
	typename _fEqual = std::equal_to<_tKey>,
	class _tAlloc = std::allocator<pair<const _tKey, _tMapped>>>
class GMRUCache
	: private unordered_map<_tKey, _tMapped, _fHash, _fEqual, _tAlloc>
{
public:
	using Map = unordered_map<_tKey, _tMapped, _fHash, _fEqual, _tAlloc>;
	using UseList = list<_tKey, _tAlloc>;
	using UseCache = unordered_multimap<_tKey, typename UseList::iterator,
		_fHash, _fEqual, _tAlloc>;
	using typename Map::const_iterator;
	using typename Map::iterator;
	using typename Map::key_type;
	using typename Map::size_type;

private:
	mutable UseList use_list;
	UseCache use_cache;
	//! \brief 保持可以再增加一个缓存项的最大容量。
	size_type max_use;

public:
	explicit
	GMRUCache(size_type s = 15U)
		: Map(), use_list(), use_cache(), max_use(s)
	{}
	DefDeMoveCtor(GMRUCache)

private:
	void
	CheckMaxUse()
	{
		while(max_use < use_list.size())
		{
			YAssert(!use_list.empty(), "Invalid state of use list found.");
			YAssert(!use_cache.empty(), "Invalid state of use cache found.");

			const auto& key(use_list.front());

			Map::erase(key),
			use_cache.erase(key);
			use_list.pop_front();
		}
	}

public:
	DefGetter(const ynothrow, size_type, MaxUse, max_use)

	void
	SetMaxUse(size_type s)
	{
		if(s < 1)
			s = 1;
		CheckMaxUse();
	}

	using Map::begin;

	void
	clear() ynoexcept
	{
		Map::clear(),
		use_list.clear(),
		use_cache.clear();
	}

	using Map::end;

	template<typename... _tParams>
	pair<typename Map::iterator, bool>
	emplace(_tParams&&... args)
	{
		CheckMaxUse();

		const auto pr(Map::emplace(yforward(args)...));

		if(pr.second)
		{
			using ystdex::is_undereferenceable;

			YAssert(!is_undereferenceable(pr.first), "Bad iterator found.");

			const auto& k(pr.first->first);
			const auto i(use_list.insert(use_list.end(), k));

			try
			{
				use_cache.emplace(k, i);
			}
			catch(...)
			{
				YAssert(!use_list.empty(), "Invalid state of use list found.");
				use_list.pop_back();
				throw;
			}
		}
		return pr;
	}

	iterator
	find(const key_type& k)
	{
		const auto i(use_cache.find(k));

		if(i != use_cache.end())
		{
			use_list.splice(use_list.end(), use_list, i->second);
			return Map::find(k);
		}
		return Map::end();
	}
	const_iterator
	find(const key_type& k) const
	{
		const auto i(use_cache.find(k));

		if(i != use_cache.end())
		{
			use_list.splice(use_list.end(), use_list, i->second);
			return Map::find(k);
		}
		return Map::end();
	}

	using Map::size;
};


/*!
\brief 以指定的关键字查找作为缓存的无序关联容器，
	若没有找到使用指定的可调用对象和参数初始化内容。
\tparam _tMap 映射类型，可以是 std::map 、 std::unordered_map 或 GMRUCache 等。
\since build 460
*/
template<class _tMap, typename _tKey, typename _fCallable, typename... _tParams>
auto
CacheLookup(_tMap& cache, const _tKey& key, _fCallable init,
	_tParams&&... args) -> decltype((cache.begin()->second))
{
	auto i(cache.find(key));

	if(i == cache.end())
	{
		const auto pr(cache.emplace(key, init(yforward(args)...)));

		if(YB_UNLIKELY(!pr.second))
			throw LoggedEvent("Cache insertion failed.", Alert);
		i = pr.first;
	}
	return i->second;
}

} // namespace YSLib;

#endif

