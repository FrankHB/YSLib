/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator.hpp
\ingroup YCLib
\brief C++ 标准库迭代器扩展。
\version r1256;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-27 23:01:00 +0800;
\par 修改时间:
	2011-09-15 11:49 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YStandardExtend::Iterator;
*/


#ifndef YCL_INC_YSTDEX_ITERATOR_HPP_
#define YCL_INC_YSTDEX_ITERATOR_HPP_

#include "../ystdex.h"
#include <iterator>
#include <utility>

namespace ystdex
{
	//! \brief 伪迭代器：总是返回单一值的迭代器适配器。
	template<typename _type, typename _tIterator = _type*>
	struct pseudo_iterator
	{
		typedef _tIterator iterator_type;
		typedef typename std::iterator_traits<iterator_type> traits_type;
		typedef typename traits_type::iterator_category iterator_category;
		typedef typename traits_type::value_type value_type;
		typedef typename traits_type::difference_type difference_type;
		typedef typename traits_type::reference reference;
		typedef typename traits_type::pointer pointer;

		value_type value;

		pseudo_iterator()
			: value()
		{}
		explicit
		pseudo_iterator(value_type v)
			: value(v)
		{}
		inline
		pseudo_iterator(const pseudo_iterator&) = default;
		inline
		pseudo_iterator(pseudo_iterator&&) = default;

		inline pseudo_iterator&
		operator=(const pseudo_iterator&) = default;
		inline pseudo_iterator&
		operator=(pseudo_iterator&&) = default;

		//前向迭代器需求。
		reference
		operator*() const
		{
			return value;
		}

		pointer
		operator->() const
		{
			return this;
		}

		pseudo_iterator&
		operator++()
		{
			return *this;
		}

		pseudo_iterator
		operator++(int)
		{
			return *this;
		}

		//双向迭代器需求。
		pseudo_iterator&
		operator--()
		{
			return *this;
		}

		pseudo_iterator
		operator--(int)
		{
			return *this;
		}

		//随机访问迭代器需求。
		reference
		operator[](const difference_type& _n) const
		{
			return this[_n];
		}

		pseudo_iterator&
		operator+=(const difference_type& _n)
		{
			return *this;
		}

		pseudo_iterator
		operator+(const difference_type& _n) const
		{
			return *this;
		}

		pseudo_iterator&
		operator-=(const difference_type& _n)
		{
			return *this;
		}

		pseudo_iterator
		operator-(const difference_type& _n) const
		{
			return *this;
		}
	};


	//! \brief 成对迭代器：拼接两个迭代器对得到的迭代器，以第一个为主迭代器。
	template<typename _tMaster, typename _tSlave,
		class _tTraits = std::iterator_traits<_tMaster>>
	class pair_iterator : protected std::pair<_tMaster, _tSlave>
	{
	protected:
		typedef _tTraits traits_type;

	public:
		typedef std::pair<_tMaster, _tSlave> pair_type;
		typedef _tMaster iterator_type;
		typedef typename traits_type::iterator_category iterator_category;
		typedef typename traits_type::value_type value_type;
		typedef typename traits_type::difference_type difference_type;
		typedef typename traits_type::reference reference;
		typedef typename traits_type::pointer pointer;

		pair_iterator()
			: std::pair<_tMaster, _tSlave>(_tMaster(), _tSlave())
		{}
		explicit
		pair_iterator(const _tMaster& _i)
			: std::pair<_tMaster, _tSlave>(_i, _tSlave())
		{}
		pair_iterator(const _tMaster& _i, const _tSlave& _s)
			: std::pair<_tMaster, _tSlave>(_i, _s)
		{}
		inline
		pair_iterator(const pair_iterator&) = default;
		inline
		pair_iterator(pair_iterator&& _r)
			: std::pair<_tMaster, _tSlave>(std::move(_r))
		{}

		inline pair_iterator&
		operator=(const pair_iterator&) = default;
		inline pair_iterator&
		operator=(pair_iterator&&) = default;
		// TODO: Allow iterator to const_iterator conversion;
/*		template<typename _tIter>
		pair_iterator(const pair_iterator<_Iter,
			typename __enable_if<
			(std::__are_same<_Iter, typename _Container::pointer>::__value),
			_Container>::__type>& __i)
			: _M_current(__i.base())
		{}
*/

		//前向迭代器需求。
		reference
		operator*() const
		{
			return *this->first;
		}

		pointer
		operator->() const
		{
			return this->first;
		}

		pair_iterator&
		operator++()
		{
			++this->first;
			++this->second;
			return *this;
		}

		pair_iterator
		operator++(int)
		{
			const auto it(*this);

			++*this;
			return it;
		}

		//双向迭代器需求。
		pair_iterator&
		operator--()
		{
			--this->second;
			--this->first;
			return *this;
		}

		pair_iterator
		operator--(int)
		{
			const auto it(*this);

			--*this;
			return it;
		}

		//随机访问迭代器需求。
		reference
		operator[](const difference_type& _n) const
		{
			return this->first[_n];
		}

		pair_iterator&
		operator+=(const difference_type& _n)
		{
			this->first += _n;
			this->second += _n;
			return *this;
		}

		pair_iterator
		operator+(const difference_type& _n) const
		{
			return pair_iterator(this->first + _n, this->second + _n);
		}

		pair_iterator&
		operator-=(const difference_type& _n)
		{
			this->second -= _n;
			this->first -= _n;
			return *this;
		}

		pair_iterator
		operator-(const difference_type& _n) const
		{
			return pair_iterator(this->first - _n, this->second - _n);
		}

		const pair_type&
		base() const
		{
			return *this;
		}
	};
}

#endif

