/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yaslivsp.hpp
\ingroup Adaptor
\brief yasli::vector 存储策略。
\version 0.2303;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-09 22:38:03 +0800;
\par 修改时间:
	2011-04-20 10:26 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YASLIVectorStoragePolicy;
*/


#ifndef INCLUDED_ADAPTOR_YASLIVSP_H_
#define INCLUDED_ADAPTOR_YASLIVSP_H_

#include "base.h"
#include <loki/yasli/yasli_vector.h>
/*
#include <memory>
#include <algorithm>
#include <limits>
*/

//! \brief 字符串存储策略类模板：使用 yasli::vector 和 空基类优化。
template <typename _tChar, class _tAlloc = std::allocator<_tChar> >
class VectorStringStorage : protected yasli::vector<_tChar, _tAlloc>
{
	typedef yasli::vector<_tChar, _tAlloc> base;

public: //!<  protected:
	typedef _tChar value_type;
	typedef typename base::iterator iterator;
	typedef typename base::const_iterator const_iterator;
	typedef _tAlloc allocator_type;
	typedef typename _tAlloc::size_type size_type;
	typedef typename _tAlloc::reference reference;

	/*!
	\brief 复制构造存储策略。
	*/
	VectorStringStorage(const VectorStringStorage& s)
		: base(s)
	{}
	/*!
	\brief 使用分配器构造存储策略。
	*/
	VectorStringStorage(const _tAlloc& a)
		: base(1, value_type(), a)
	{}
	/*!
	\brief 使用指向字符串的指针、长度和分配器构造存储策略。
	*/
	VectorStringStorage(const value_type* s, size_type len, const _tAlloc& a)
		: base(a)
	{
		base::reserve(len + 1);
		base::insert(base::end(), s, s + len);
		base::push_back(value_type()); //!< 终结符。
	}
	/*!
	\brief 使用长度、字符和分配器构造存储策略。
	\note 以 len 个字符 c 进行填充。
	*/
	VectorStringStorage(size_type len, _tChar c, const _tAlloc& a)
		: base(len + 1, c, a)
	{
		//终结符。
		base::back() = value_type();
	}

	/*!
	\brief 复制赋值存储策略。
	*/
	VectorStringStorage&
	operator=(const VectorStringStorage& rhs)
	{
		static_cast<base>(*this) = rhs;
		return *this;
	}

	PDefH1(iterator, begin)
		ImplBodyBase1(base, begin)

	PDefH1(const_iterator, begin) const
		ImplBodyBase1(base, begin)

	PDefH1(iterator, end)
		ImplRet(base::end() - 1)

	PDefH1(const_iterator, end) const
		ImplRet(base::end() - 1)

	PDefH1(size_type, size) const
		ImplRet(base::size() - 1)

	PDefH1(size_type, max_size) const
		ImplRet(base::max_size() - 1)

	PDefH1(size_type, capacity) const
		ImplRet(base::capacity() - 1)

	/*!
	\brief 保留 res_arg 个字符的空间。
	\note 实际保留 res_arg + 1 长度空间。
	*/
	void
	reserve(size_type res_arg)
	{ 
		assert(res_arg < max_size());
		base::reserve(res_arg + 1); 
	}

	/*!
	\brief 追加存储字符。
	\note 串接。
	*/
	template<class _tForIt>
	void
	append(_tForIt b, _tForIt e)
	{
		const typename std::iterator_traits<_tForIt>::difference_type
			sz(std::distance(b, e));

		assert(sz >= 0);

		if (sz == 0) return;
		base::reserve(base::size() + sz);

		const value_type& v(*b);

		struct OnBlockExit
		{
			VectorStringStorage* that;

			~OnBlockExit()
			{
				that->base::push_back(value_type());
			}
		} onBlockExit = { this };

		(void)onBlockExit;

		assert(!base::empty());
		assert(base::back() == value_type());

		base::back() = v;
		base::insert(base::end(), ++b, e);
	}

	/*!
	\brief 重新分配大小 n ，以字符 c 填充。
	*/
	void
	resize(size_type n, _tChar c)
	{
		base::reserve(n + 1);
		base::back() = c;
		base::resize(n + 1, c);
		base::back() = _tChar();
	}

	PDefH1(void, swap, VectorStringStorage& rhs)
		ImplBodyBase1(base, swap, rhs)

	PDefH1(const _tChar*, c_str) const
		ImplRet(&*begin())

	PDefH1(const _tChar*, data) const
		ImplRet(&*begin())

	PDefH1(_tAlloc, get_allocator) const
		ImplBodyBase1(base, get_allocator)
};

#endif

