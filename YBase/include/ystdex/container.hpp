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
\version r1031
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-09-12 01:36:20 +0800
\par 修改时间:
	2014-12-10 00:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Container
*/


#ifndef YB_INC_ystdex_container_hpp_
#define YB_INC_ystdex_container_hpp_ 1

#include "functional.hpp"
#include "algorithm.hpp" // for ystdex::sort_unique, ystdex::make_transform,
//	ystdex::is_undereferenceable;
#include <initializer_list> // for std::initializer_list;
#include "utility.hpp" // for ystdex::arrlen;

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
	{}

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

	//! \since build 559
	container_type&
	get_container() ynothrow
	{
		return *this;
	}

	//! \since build 559
	const container_type&
	get_container() const ynothrow
	{
		return *this;
	}

	void
	swap(container_adaptor& con) ynothrow
	{
		return base::swap(static_cast<container_type&>(con));
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
	{}

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

	//! \since build 559
	using base::get_container;
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
\ingroup helper_functions
\brief 构造指定类型的容器。
\since build 532
*/
//@{
template<class _tCon, typename _tIter>
inline _tCon
make_container(_tIter first, _tIter last)
{
	return _tCon(first, last);
}
//! \note 使用 ADL \c begin 和 \c end 指定范围迭代器。
template<class _tCon, typename _tRange>
inline _tCon
make_container(_tRange&& c)
{
	using std::begin;
	using std::end;

	return _tCon(begin(c), end(c));
}
//! \note 使用 ADL \c begin 和 \c end 指定范围迭代器。
template<class _tCon, typename _tRange, typename _func>
inline _tCon
make_container(_tRange&& c, _func f)
{
	using std::begin;
	using std::end;

	return _tCon(ystdex::make_transform(begin(c), f),
		ystdex::make_transform(end(c), f));
}
template<class _tCon, typename _type>
inline _tCon
make_container(std::initializer_list<_type> il)
{
	return _tCon(il.begin(), il.end());
}
//@}


//! \since build 546
namespace details
{

template<typename _type>
auto
test_range_size(const _type& c) -> enable_if_t<
	is_convertible<decltype(c.size()), size_t>::value, true_type>;
false_type
test_range_size(...);

template<typename _tRange>
yconstfn auto
range_size(const _tRange& c, true_type) -> decltype(c.size())
{
	return c.size();
}

template<typename _tRange>
inline auto
range_size(const _tRange& c, false_type)
	-> decltype(std::distance(begin(c), end(c)))
{
	using std::begin;
	using std::end;

	return std::distance(begin(c), end(c));
}

} // namespace details;

/*!
\brief 取范围大小。
\note 需要时使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定范围迭代器。
\since build 546

对数组直接返回大小，否则：
若可调用结果可转换为 \c size_t 的成员函数 size() 则使用 size ；
否则 \c size 取序列大小则使用 \c std::distance 计算范围迭代器确定范围大小。
*/
//{
template<typename _tRange>
yconstfn auto
range_size(const _tRange& c)
	-> decltype(details::range_size(c, decltype(details::test_range_size(c))()))
{
	return details::range_size(c, decltype(details::test_range_size(c))());
}
template<typename _type, size_t _vN>
yconstfn size_t
range_size(const _type(&c)[_vN])
{
	return ystdex::arrlen(c);
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
assign(_tCon& con, _tParams&&... args)
{
	con.assign(yforward(args)...);
}
template<class _tCon, typename _type, size_t _vN>
inline void
assign(_tCon& con, const _type(&arr)[_vN])
{
	con.assign(arr, arr + _vN);
}
//@}


/*!
\brief 插入元素到容器末尾。
\since build 546
\todo 返回非 \c void 。
*/
//@{
template<class _tCon, typename _tIn>
void
concat(_tCon& con, _tIn first, _tIn last)
{
	con.insert(con.end(), std::make_move_iterator(first),
		std::make_move_iterator(last));
}
template<class _tCon, typename _tRange>
void
concat(_tCon& con, _tRange&& c)
{
	using std::begin;
	using std::end;

	con.insert(con.end(), begin(c), end(c));
}
//@}


/*!
\ingroup algorithms
\brief 逆向插入范围到容器的指定位置。
\param con 指定的容器。
\param i 指定起始插入位置的迭代器。
\param first 输入范围起始迭代器。
\param last 输入范围终止迭代器。
\pre i 是 con 的迭代器。
\since build 532

调用带有迭代器参数的 \c insert 成员函数插入迭代器指定的范围到容器的指定位置。
 con 是关联容器时，范围内元素有序可被优化。
*/
template<typename _tCon, typename _tIn>
typename _tCon::const_iterator
insert_reversed(_tCon& con, typename _tCon::const_iterator i, _tIn first,
	_tIn last)
{
	for(; first != last; ++first)
	{
		yconstraint(!is_undereferenceable(first));
		i = con.insert(i, *first);
	}
	return i;
}


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
	container_inserter(_tCon& con)
		: container(&con)
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
seq_insert(_tCon& con, _tParams&&... args)
{
	ystdex::seq_apply(container_inserter<_tCon>(con), yforward(args)...);
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
\brief 删除指定序列容器中和指定值的相等的元素。
\note 使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定范围迭代器。
\since build 289
*/
template<typename _tCon>
void
erase_all(_tCon& con, const typename _tCon::value_type& val)
{
	using std::begin;
	using std::end;

	con.erase(std::remove(begin(con), end(con), val), end(con));
}
/*!
\ingroup algorithms
\brief 删除指定容器中迭代器区间中和指定值的相等的元素。
\pre first 和 last 是 con 的有效的或表示序列终止位置的迭代器。
\since build 289
*/
template<typename _tCon, typename _tFwd, typename _tValue>
void
erase_all(_tCon& con, _tFwd first, _tFwd last, const _tValue& value)
{
	while(first != last)
		if(*first == value)
			con.erase(first++);
		else
			++first;
}

/*!
\ingroup algorithms
\brief 删除指定序列容器中满足条件的元素。
\note 使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定范围迭代器。
\since build 289
*/
template<typename _tCon, typename _fPred>
void
erase_all_if(_tCon& con, _fPred pred)
{
	using std::begin;
	using std::end;

	con.erase(std::remove_if(begin(con), end(con), pred), end(con));
}
/*!
\ingroup algorithms
\brief 删除指定容器中迭代器区间中满足条件的元素。
\pre first 和 last 是 con 的有效的或表示序列终止位置的迭代器。
\since build 289
*/
template<typename _tCon, typename _tFwd, typename _fPred>
void
erase_all_if(_tCon& con, _tFwd first, _tFwd last, _fPred pred)
{
	while(first != last)
		if(pred(*first))
			con.erase(first++);
		else
			++first;
}

/*!
\brief 删除指定容器中指定迭代器起始指定数量的元素。
\pre 指定的迭代器是指定容器的迭代器。
\pre 断言检查：删除的范围不超出容器。
\since build 531
*/
//@{
template<typename _tCon>
typename _tCon::const_iterator
erase_n(_tCon& con, typename _tCon::const_iterator i,
	typename _tCon::difference_type n)
{
	yassume(n <= std::distance(i, con.cend()));
	return con.erase(i, std::next(i, n));
}
//! \since build 532
template<typename _tCon>
typename _tCon::iterator
erase_n(_tCon& con, typename _tCon::iterator i,
	typename _tCon::difference_type n)
{
	yassume(n <= std::distance(i, con.end()));
	return con.erase(i, std::next(i, n));
}
//@}


/*!
\ingroup algorithms
\brief 排序指定序列容器，保留不重复元素。
\pre 容器的迭代器满足随机迭代器要求。
\note 使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定范围迭代器。
\since build 414
*/
template<class _tCon>
inline void
sort_unique(_tCon& con)
{
	using std::begin;
	using std::end;

	con.erase(ystdex::sort_unique(begin(con), end(con)), end(con));
}


/*!
\brief 若容器末尾存在指定值的元素则移除。
\since build 545
*/
template<class _tCon>
bool
pop_back_val(_tCon& con, const typename _tCon::value_type& val)
{
	if(!con.empty() && con.front() == val)
	{
		con.pop_front();
		return true;
	}
	return {};
}

/*!
\brief 若容器起始存在指定值的元素则移除。
\since build 545
*/
template<class _tCon>
bool
pop_front_val(_tCon& con, const typename _tCon::value_type& val)
{
	if(!con.empty() && con.back() == val)
	{
		con.pop_back();
		return true;
	}
	return {};
}


/*!
\brief 插入元素到 \c vector 末尾。
\since build 546
*/
//@{
template<class _tVector, typename _tIn>
void
vector_concat(_tVector& vec, _tIn first, _tIn last)
{
	vec.reserve(vec.size() + std::distance(first, last));
	ystdex::concat(vec, first, last);
}
template<class _tVector, typename _tRange>
void
vector_concat(_tVector& vec, _tRange&& c)
{
	vec.reserve(vec.size() + ystdex::range_size(c));
	ystdex::concat(vec, yforward(c));
}
//@}


/*!
\ingroup helper_functions
\brief 替换关联容器的值。
\note 使用 <tt>end</tt> 指定范围迭代器。
\since build 531
\todo 支持没有 \c emplace_hint 成员的关联容器。
*/
template<class _tAssocCon, typename _tKey, typename _func>
auto
replace_value(_tAssocCon& con, const _tKey& k, _func f)
	-> decltype(con.emplace_hint(con.erase(con.find(k)), f(*con.find(k))))
{
	auto i(con.find(k));

	return i != end(con) ? con.emplace_hint(con.erase(i), f(*i)) : i;
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

	return {i, (i == m.end() || m.key_comp()(k, i->first))};
}

} // namespace ystdex;

#endif

