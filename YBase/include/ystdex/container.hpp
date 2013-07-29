/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file container.hpp
\ingroup YStandardEx
\brief 通用容器操作。
\version r486
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-09-12 01:36:20 +0800
\par 修改时间:
	2013-07-27 04:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Containers
*/


#ifndef YB_INC_ystdex_container_hpp_
#define YB_INC_ystdex_container_hpp_ 1

#include "functional.hpp"
#include "cassert.h"
#include <array> // for std::array;
#include <algorithm> // for std::copy_n;

namespace ystdex
{

/*!
\brief 容器适配器。
\note 满足容器要求（但不是 ISO C++ 要求的序列容器要求）。
\note 使用 ISO C++11 容器要求指定的成员顺序声明。
\warning 非虚析构。
\see ISO C++11 23.6[container.adaptors],
	23.2.1[container.requirements.general] 。
\since build 408
*/
template<class _tSeqCont>
class container_adaptor : protected _tSeqCont
{
protected:
	typedef _tSeqCont container_type;

private:
	typedef container_type base;

public:
	//! \brief 满足容器要求。
	//@{
	typedef typename container_type::value_type value_type;
	typedef typename container_type::reference reference;
	typedef typename container_type::const_reference const_reference;
	typedef typename container_type::iterator iterator;
	typedef typename container_type::const_iterator const_iterator;
	typedef typename container_type::difference_type difference_type;
	typedef typename container_type::size_type size_type;

	container_adaptor() = default;
	explicit
	container_adaptor(size_type n)
		: base(n)
	{}
	container_adaptor(size_type n, const value_type& value)
		: base(n, value)
	{}
	template<class _tIn>
	container_adaptor(_tIn first, _tIn last)
		: base(std::move(first), std::move(last))
	{}
	container_adaptor(const container_adaptor&) = default;
	container_adaptor(container_adaptor&&) = default;
	//@}
	container_adaptor(std::initializer_list<value_type> il)
		: base(il)
	{};

	//! \brief 满足容器要求。
	//@{
	container_adaptor&
	operator=(const container_adaptor&) = default;
	container_adaptor&
	operator=(container_adaptor&&) = default;
	//@}
	container_adaptor&
	operator=(std::initializer_list<value_type> il)
	{
		base::operator=(il);
	}

	//! \brief 满足容器要求。
	friend bool
	operator==(const container_adaptor& x, const container_adaptor& y)
	{
		return static_cast<const container_type&>(x)
			== static_cast<const container_type&>(y);
	}

	//! \brief 满足容器要求。
	//@{
	using container_type::begin;

	using container_type::end;

	using container_type::cbegin;

	using container_type::cend;

	void
	swap(container_adaptor& c) ynothrow
	{
		return base::swap(static_cast<container_type&>(c));
	}

	using base::size;

	using base::max_size;

	using base::empty;
	//@}
};

/*!
\brief 满足容器要求。
\since build 408
*/
//@{
template<class _tSeqCont>
inline bool
operator!=(const container_adaptor<_tSeqCont>& x,
	const container_adaptor<_tSeqCont>& y)
{
	return !(x == y);
}

template<class _tSeqCont>
void
swap(container_adaptor<_tSeqCont>& x,
	container_adaptor<_tSeqCont>& y) ynothrow
{
	x.swap(y);
}
//@}


/*!
\brief 序列容器适配器。
\note 满足序列要求（但不是 ISO C++ 要求的序列容器要求）。
\note 使用 ISO C++11 容器要求指定的成员顺序声明。
\warning 非虚析构。
\see ISO C++11 23.6[container.adaptors], 23.2.1[container.requirements.general],
	23.2.3[sequence.reqmts] 。
\since build 408
*/
template<class _tSeqCont>
class sequence_container_adaptor : protected container_adaptor<_tSeqCont>
{
private:
	typedef container_adaptor<_tSeqCont> base;

public:
	typedef typename base::container_type container_type;
	typedef typename container_type::value_type value_type;
	typedef typename container_type::size_type size_type;

	//! \brief 满足序列容器要求。
	//@{
	sequence_container_adaptor() = default;
	explicit
	sequence_container_adaptor(size_type n)
		: base(n)
	{}
	sequence_container_adaptor(size_type n, const value_type& value)
		: base(n, value)
	{}
	template<class _tIn>
	sequence_container_adaptor(_tIn first, _tIn last)
		: base(std::move(first), std::move(last))
	{}
	sequence_container_adaptor(const sequence_container_adaptor&) = default;
	sequence_container_adaptor(sequence_container_adaptor&&) = default;
	sequence_container_adaptor(std::initializer_list<value_type> il)
		: base(il)
	{};

	sequence_container_adaptor&
	operator=(const sequence_container_adaptor&) = default;
	sequence_container_adaptor&
	operator=(sequence_container_adaptor&&) = default;
	sequence_container_adaptor&
	operator=(std::initializer_list<value_type> il)
	{
		base::operator=(il);
	}

	//! \brief 满足容器要求。
	friend bool
	operator==(const sequence_container_adaptor& x,
		const sequence_container_adaptor& y)
	{
		return static_cast<const container_type&>(x)
			== static_cast<const container_type&>(y);
	}

//	using container_type::emplace;

	using container_type::insert;

	using container_type::erase;

	using container_type::clear;

	using container_type::assign;
	//@}
};

/*!
\brief 满足容器要求。
\since build 408
*/
//@{
template<class _tSeqCont>
inline bool
operator!=(const sequence_container_adaptor<_tSeqCont>& x,
	const sequence_container_adaptor<_tSeqCont>& y)
{
	return !(x == y);
}

template<class _tSeqCont>
void
swap(sequence_container_adaptor<_tSeqCont>& x,
	sequence_container_adaptor<_tSeqCont>& y) ynothrow
{
	x.swap(y);
}
//@}


/*!
\ingroup algorithms
\brief 插入参数指定的元素到容器。
\since build 274
*/
//@{
template<class _tCont, typename... _tParams>
inline void
assign(_tCont& con, _tParams&&... args)
{
	con.assign(yforward(args)...);
}
template<class _tCont, typename _type, size_t _vN>
inline void
assign(_tCont& con, const _type(&arr)[_vN])
{
	con.assign(arr, arr + _vN);
}
//@}


/*!
\brief 访问关联容器。
\exception std::out_of_range 访问的值不存在。
\since build 399
*/
//@{
template<class _tCont, typename _tKey>
auto
at(_tCont& con, const _tKey& k)
	-> decltype(con.at(k))
{
	return con.at(k);
}
template<class _tCont, typename _tKey>
const _tKey&
at(const _tCont& con, const _tKey& k)
{
	const auto i(con.find(k));

	if(i != end(con))
		return *i;
	throw std::out_of_range("ystdex::at");
}
//@}


/*!
\brief 容器插入函数对象。
\note 成员命名参照 ISO C++11 24.5.2 中的类定义概要。
\since build 338
*/
template<typename _tCont>
class container_inserter
{
public:
	typedef _tCont container_type;

protected:
	_tCont* container;

public:
	container_inserter(_tCont& cont)
		: container(&cont)
	{}

	template<typename... _tParams>
	auto
	operator()(_tParams&&... args)
		-> decltype(container->insert(std::forward<_tParams>(args)...))
	// NOTE: Nested %decltype could cause crashing of devkitPro G++ 4.7.1.
	{
		yassume(container);

		return container->insert(yforward(args)...);
	}
};

/*!
\brief 顺序插入值至指定容器。
\since build 338
*/
template<typename _tCont, typename... _tParams>
inline void
seq_insert(_tCont& cont, _tParams&&... args)
{
	ystdex::seq_apply(container_inserter<_tCont>(cont), yforward(args)...);
}


/*!
\ingroup algorithms
\brief 删除指定序列范围中和指定值的相等的元素。
\note 使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定容器迭代器。
\since build 289
*/
template<typename _tRange>
void
erase_all(_tRange& c, const typename _tRange::value_type& val)
{
	c.erase(std::remove(begin(c), end(c), val), end(c));
}
/*!
\ingroup algorithms
\brief 删除指定容器中迭代器区间中和指定值的相等的元素。
\pre first 和 last 是 c 的有效的迭代器或 <tt>c.end()</tt> 。
\since build 289
*/
template<typename _tCont, typename _tForward, typename _tValue>
void
erase_all(_tCont& c, _tForward first, _tForward last, const _tValue& value)
{
	while(first != last)
		if(*first == value)
			c.erase(first++);
		else
			++first;
}

/*!
\ingroup algorithms
\brief 删除指定序列范围中满足条件的元素。
\note 使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定范围迭代器。
\since build 289
*/
template<typename _tRange, typename _fPred>
void
erase_all_if(_tRange& c, _fPred pred)
{
	c.erase(std::remove_if(begin(c), end(c), pred), end(c));
}
/*!
\ingroup algorithms
\brief 删除指定容器中迭代器区间中满足条件的元素。
\pre first 和 last 是 c 的有效的迭代器或 <tt>c.end()</tt> 。
\since build 289
*/
template<typename _tCont, typename _tForward, typename _fPred>
void
erase_all_if(_tCont& c, _tForward first, _tForward last, _fPred pred)
{
	while(first != last)
		if(pred(*first))
			c.erase(first++);
		else
			++first;
}


/*!
\brief 排序指定序列范围，保留不重复元素的区间。
\return 不重复元素的区间末尾。
\since build 414
*/
template<typename _tRandom>
inline _tRandom
sort_unique(_tRandom first, _tRandom last)
{
	std::sort(first, last);
	return std::unique(first, last);
}

/*!
\brief 排序指定容器，保留不重复元素。
\pre 容器的迭代器满足随机迭代器要求。
\since build 414
*/
template<class _tCont>
void
sort_unique(_tCont& c)
{
	ystdex::sort_unique(begin(c), last(c));
	c.erase(ystdex::sort_unique(begin(c), last(c)), end(c));
}


/*!
\ingroup algorithms
\brief 按指定键值搜索指定映射。
\return 一个用于表示结果的 std::pair 对象，其成员 first 为迭代器，
	second 表示是否不存在而需要插入。
\note 行为类似 std::map::operator[] 。
\since build 173
*/
template<class _tMap>
std::pair<typename _tMap::iterator, bool>
search_map(_tMap& m, const typename _tMap::key_type& k)
{
	const auto i(m.lower_bound(k));

	return std::make_pair(i, (i == m.end() || m.key_comp()(k, i->first)));
}


/*!
\brief 取指定参数初始化的 std::array 对象。
\since build 337
*/
template<typename _type, typename... _tParams>
inline std::array<_type, sizeof...(_tParams)>
make_array(_tParams&&... args)
{
	// TODO: Use one pair of braces (depending on G++).
	return {{decay_copy(yforward(args))...}};
}

/*!
\brief 取指定参数转换为 std::array 对象。
\since build 337
*/
//@{
template<typename _type, size_t _vN, typename _tSrc>
yconstfn std::array<_type, _vN>
to_array(const _tSrc& src)
{
	return std::array<_type, _vN>(src);
}
template<typename _type, size_t _vN>
yconstfn std::array<_type, _vN>
to_array(const std::array<_type, _vN>& src)
{
	return src;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
to_array(const _tSrcElement(&src)[_vN])
{
	std::array<_type, _vN> arr;

	std::copy_n(std::addressof(src[0]), _vN, std::addressof(arr[0]));
	return std::move(arr);
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
to_array(_tSrcElement(&&src)[_vN])
{
	std::array<_type, _vN> arr;

	std::copy_n(std::make_move_iterator(std::addressof(src[0])), _vN,
		std::addressof(arr[0]));
	return std::move(arr);
}
//@}

} // namespace ystdex;

#endif

