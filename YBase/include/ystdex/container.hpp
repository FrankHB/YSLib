/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file container.hpp
\ingroup YStandardEx
\brief 通用容器操作。
\version r652
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-09-12 01:36:20 +0800
\par 修改时间:
	2014-05-23 09:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Container
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
template<class _tSeqCon>
class container_adaptor : protected _tSeqCon
{
protected:
	using container_type = _tSeqCon;

private:
	using base = container_type;

public:
	//! \brief 满足容器要求。
	//@{
	using value_type = typename container_type::value_type;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using difference_type = typename container_type::difference_type;
	using size_type = typename container_type::size_type;

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
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	container_adaptor(container_adaptor&& con)
		: base(static_cast<base&&>(con))
	{}
#else
	container_adaptor(container_adaptor&&) = default;
#endif
	//@}
	container_adaptor(std::initializer_list<value_type> il)
		: base(il)
	{};

	//! \brief 满足容器要求。
	//@{
	container_adaptor&
	operator=(const container_adaptor&) = default;
	container_adaptor&
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	operator=(container_adaptor&& con)
	{
		static_cast<base&>(*this) = static_cast<base&&>(con);
		return *this;
	}
#else
	operator=(container_adaptor&&) = default;
#endif
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
template<class _tSeqCon>
inline bool
operator!=(const container_adaptor<_tSeqCon>& x,
	const container_adaptor<_tSeqCon>& y)
{
	return !(x == y);
}

template<class _tSeqCon>
void
swap(container_adaptor<_tSeqCon>& x,
	container_adaptor<_tSeqCon>& y) ynothrow
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
template<class _tSeqCon>
class sequence_container_adaptor : protected container_adaptor<_tSeqCon>
{
private:
	using base = container_adaptor<_tSeqCon>;

public:
	using container_type = typename base::container_type;
	using value_type = typename container_type::value_type;
	using size_type = typename container_type::size_type;

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
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	sequence_container_adaptor(sequence_container_adaptor&& con)
		: base(static_cast<base&&>(con))
	{}
#else
	sequence_container_adaptor(sequence_container_adaptor&&) = default;
#endif
	sequence_container_adaptor(std::initializer_list<value_type> il)
		: base(il)
	{};

	sequence_container_adaptor&
	operator=(const sequence_container_adaptor&) = default;
	sequence_container_adaptor&
#if YB_IMPL_MSCPP
	//! \since build 454 as workaround for Visual C++ 2013
	operator=(sequence_container_adaptor&& con)
	{
		static_cast<base&>(*this) = static_cast<base&&>(con);
		return *this;
	}
#else
	operator=(sequence_container_adaptor&&) = default;
#endif
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
template<class _tSeqCon>
inline bool
operator!=(const sequence_container_adaptor<_tSeqCon>& x,
	const sequence_container_adaptor<_tSeqCon>& y)
{
	return !(x == y);
}

template<class _tSeqCon>
void
swap(sequence_container_adaptor<_tSeqCon>& x,
	sequence_container_adaptor<_tSeqCon>& y) ynothrow
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
template<class _tCon, typename... _tParams>
inline void
assign(_tCon& c, _tParams&&... args)
{
	c.assign(yforward(args)...);
}
template<class _tCon, typename _type, size_t _vN>
inline void
assign(_tCon& c, const _type(&arr)[_vN])
{
	c.assign(arr, arr + _vN);
}
//@}


/*!
\brief 容器插入函数对象。
\note 成员命名参照 ISO C++11 24.5.2 中的类定义概要。
\since build 338
*/
template<typename _tCon>
class container_inserter
{
public:
	using container_type = _tCon;

protected:
	_tCon* container;

public:
	container_inserter(_tCon& c)
		: container(&c)
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
template<typename _tCon, typename... _tParams>
inline void
seq_insert(_tCon& c, _tParams&&... args)
{
	ystdex::seq_apply(container_inserter<_tCon>(c), yforward(args)...);
}


//! \since build 488
namespace details
{

template<class _tCon, typename _tKey>
bool
exists(const _tCon& con, const _tKey& key,
	decltype(std::declval<_tCon>().count()) = 1U)
{
	return con.count(key) != 0;
}
template<class _tCon, typename _tKey>
bool
exists(const _tCon& con, const _tKey& key, ...)
{
	return con.find(key) != end(con);
}

} // namespace details;

/*!
\brief 判断指定的容器中存在指定的键。
\note 当容器对象右值可使用返回以整数初始化的类型的成员 <tt>count()</tt> 时使用成
	员 count() 实现；否则使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定的容器迭代
	器，使用成员 find 实现。
\since build 488
*/
template<class _tCon, typename _tKey>
inline bool
exists(const _tCon& con, const _tKey& key)
{
	return details::exists(con, key);
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
\pre first 和 last 是 c 的有效的或表示序列终止位置的迭代器。
\since build 289
*/
template<typename _tCon, typename _tFwd, typename _tValue>
void
erase_all(_tCon& c, _tFwd first, _tFwd last, const _tValue& value)
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
\pre first 和 last 是 c 的有效的或表示序列终止位置的迭代器。
\since build 289
*/
template<typename _tCon, typename _tFwd, typename _fPred>
void
erase_all_if(_tCon& c, _tFwd first, _tFwd last, _fPred pred)
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
template<class _tCon>
void
sort_unique(_tCon& c)
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
	return {{decay_copy(args)...}};
}

/*!
\brief 取指定参数转移至 std::array 对象。
\since build 495
*/
template<typename _type, typename... _tParams>
inline std::array<_type, sizeof...(_tParams)>
forward_as_array(_tParams&&... args)
{
	// TODO: Use one pair of braces (depending on G++).
	return {{yforward(args)...}};
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
	return arr;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
to_array(_tSrcElement(&&src)[_vN])
{
	std::array<_type, _vN> arr;

	std::copy_n(std::make_move_iterator(std::addressof(src[0])), _vN,
		std::addressof(arr[0]));
	return arr;
}
//@}

} // namespace ystdex;

#endif

