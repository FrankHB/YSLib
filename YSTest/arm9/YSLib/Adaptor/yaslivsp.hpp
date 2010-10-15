// YSLib::Adaptor::YASLIVectorStoragePolicy by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-10-09 22:38:03 + 08:00;
// UTime = 2010-10-13 16:46 + 08:00;
// Version = 0.2275;


#ifndef INCLUDED_YASLIVSP_H_
#define INCLUDED_YASLIVSP_H_

// YASLIVectorStoragePolicy ： yasli::vector 存储策略。

#include "base.h"
#include <loki/yasli/yasli_vector.h>
/*
#include <memory>
#include <algorithm>
#include <limits>
*/

//字符串存储策略类模板：使用 yasli::vector 和 空基类优化。
template <typename _tChar, class _tAlloc = std::allocator<_tChar> >
class VectorStringStorage : protected yasli::vector<_tChar, _tAlloc>
{
	typedef yasli::vector<_tChar, _tAlloc> base;

public: // protected:
	typedef _tChar value_type;
	typedef typename base::iterator iterator;
	typedef typename base::const_iterator const_iterator;
	typedef _tAlloc allocator_type;
	typedef typename _tAlloc::size_type size_type;
	typedef typename _tAlloc::reference reference;

	VectorStringStorage(const VectorStringStorage& s)
		: base(s)
	{}
	VectorStringStorage(const _tAlloc& a)
		: base(1, value_type(), a)
	{}
	VectorStringStorage(const value_type* s, size_type len, const _tAlloc& a)
		: base(a)
	{
		base::reserve(len + 1);
		base::insert(base::end(), s, s + len);
		// Terminating zero
		base::push_back(value_type());
	}
	VectorStringStorage(size_type len, _tChar c, const _tAlloc& a)
		: base(len + 1, c, a)
	{
		// 终结符 0 。
		base::back() = value_type();
	}

	VectorStringStorage&
	operator=(const VectorStringStorage& rhs)
	{
		static_cast<base>(*this) = rhs;
		return *this;
	}

	PDefH(iterator, begin)
		ImplBodyBase(base, begin)

	PDefH(const_iterator, begin) const
		ImplBodyBase(base, begin)

	PDefH(iterator, end)
		ImplRet(base::end() - 1)

	PDefH(const_iterator, end) const
		ImplRet(base::end() - 1)

	PDefH(size_type, size) const
		ImplRet(base::size() - 1)

	PDefH(size_type, max_size) const
		ImplRet(base::max_size() - 1)

	PDefH(size_type, capacity) const
		ImplRet(base::capacity() - 1)

	void
	reserve(size_type res_arg)
	{ 
		assert(res_arg < max_size());
		base::reserve(res_arg + 1); 
	}

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

	void
	resize(size_type n, _tChar c)
	{
		base::reserve(n + 1);
		base::back() = c;
		base::resize(n + 1, c);
		base::back() = _tChar();
	}

	PDefH(void, swap, VectorStringStorage& rhs)
		ImplBodyBaseVoid(base, swap, rhs)

	PDefH(const _tChar*, c_str) const
		ImplRet(&*begin())

	PDefH(const _tChar*, data) const
		ImplRet(&*begin())

	PDefH(_tAlloc, get_allocator) const
		ImplBodyBase(base, get_allocator)
};

#endif

