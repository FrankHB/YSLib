/*
	© 2010-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file container.hpp
\ingroup YStandardEx
\brief 通用容器操作。
\version r1931
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-09-12 01:36:20 +0800
\par 修改时间:
	2017-06-03 12:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Container
*/


#ifndef YB_INC_ystdex_container_hpp_
#define YB_INC_ystdex_container_hpp_ 1

#include "iterator.hpp" // for begin, end, make_transform, size, std::distance,
//	std::make_move_iterator, cbegin, cend, std::piecewise_construct_t,
//	std::piecewise_construct;
#include "functional.hpp" // for std::declval, is_detected_convertible;
#include "utility.hpp" // for std::initializer_list, ystdex::as_const;
#include "algorithm.hpp" // for is_undereferenceable, sort_unique;

namespace ystdex
{

/*!
\brief 容器适配器。
\note 满足容器要求（但不是 ISO C++ 要求的序列容器要求）。
\note 使用 ISO C++11 容器要求指定的成员顺序声明。
\warning 非虚析构。
\see ISO C++11 23.6 [container.adaptors],
	23.2.1 [container.requirements.general] 。
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
\see ISO C++11 23.6 [container.adaptors],
	23.2.1 [container.requirements.general], 23.2.3 [sequence.reqmts] 。
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
//@{
template<class _tCon, typename _tRange>
inline _tCon
make_container(_tRange&& c)
{
	return _tCon(begin(yforward(c)), end(yforward(c)));
}
template<class _tCon, typename _tRange, typename _func>
inline _tCon
make_container(_tRange&& c, _func f)
{
	return _tCon(ystdex::make_transform(begin(c), f),
		ystdex::make_transform(end(c), f));
}
//@}
template<class _tCon, typename _tElem>
inline _tCon
make_container(std::initializer_list<_tElem> il)
{
	return _tCon(il.begin(), il.end());
}
//@}


//! \since build 546
namespace details
{

//! \since build 663
template<class _type>
using range_size_t = decltype(size(std::declval<_type>()));

//! \since build 663
template<class _type>
using has_range_size = is_detected_convertible<size_t, range_size_t, _type>;

//! \since buld 730
template<typename _tRange>
yconstfn auto
range_size(const _tRange& c, true_) -> decltype(size(c))
{
	return size(c);
}
//! \since buld 730
template<typename _tRange>
inline auto
range_size(const _tRange& c, false_)
	-> decltype(std::distance(begin(c), end(c)))
{
	return std::distance(begin(c), end(c));
}

} // namespace details;

/*!
\ingroup algorithms
\pre 参数指定的迭代器范围（若存在）有效。
\note 参数 \c first 和 \c last 指定迭代器范围。
\note 对不以迭代器指定的范围，使用 ADL begin 和 end 取迭代器的值及其类型，
	但确定为 const 迭代器时使用 ADL cbegin 和 cend 代替。
*/
//@{
/*!
\brief 取范围大小。
\since build 546

对 std::initializer_list 的实例直接返回大小，否则：
若可调用结果可转换为 \c size_t 的 ADL 函数 \c size 则使用 ADL \c size ；
否则使用 std::distance 计算范围迭代器确定范围大小。
*/
//@{
template<typename _tRange>
yconstfn auto
range_size(const _tRange& c)
	-> decltype(details::range_size(c, details::has_range_size<_tRange>()))
{
	return details::range_size(c, details::has_range_size<_tRange>());
}
#if __cplusplus <= 201402L
/*!
\see CWG 1591 。
\since build 664
*/
template<typename _tElem>
yconstfn size_t
range_size(std::initializer_list<_tElem> il)
{
	return size(il);
}
#endif
//@}


/*!
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
\pre 指定元素的范围和容器不重叠。
\note 使用 ADL end 。
\since build 546
\todo 返回非 \c void 。
*/
//@{
template<class _tCon, typename _tIn>
void
concat(_tCon& con, _tIn first, _tIn last)
{
	con.insert(end(con), std::make_move_iterator(first),
		std::make_move_iterator(last));
}
template<class _tCon, typename _tRange>
void
concat(_tCon& con, _tRange&& c)
{
	con.insert(end(con), begin(yforward(c)), end(yforward(c)));
}
//@}


/*!
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
	container_inserter(_tCon& con)
		: container(&con)
	{}

	template<typename... _tParams>
	auto
	operator()(_tParams&&... args)
		-> decltype(container->insert(yforward(args)...))
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


//! \since build 663
//@{
template<class _type, typename... _tParams>
using mem_remove_t
	= decltype(std::declval<_type>().remove(std::declval<_tParams>()...));

template<typename _type, typename... _tParams>
using has_mem_remove = is_detected<mem_remove_t, _type, _tParams...>;

template<class _type, typename... _tParams>
using mem_remove_if_t
	= decltype(std::declval<_type>().remove_if(std::declval<_tParams>()...));

template<typename _type, typename... _tParams>
using has_mem_remove_if = is_detected<mem_remove_if_t, _type, _tParams...>;

template<class _type>
using key_type_t = typename _type::key_type;

template<typename _type>
using has_mem_key_type = is_detected<key_type_t, _type>;


template<typename _tSeqCon, typename _type>
inline void
erase_remove(_tSeqCon& con, decltype(begin(con)) first,
	decltype(end(con)) last, const _type& value)
{
	con.erase(std::remove(first, last, value), last);
}

template<typename _tSeqCon, typename _fPred>
inline void
erase_remove_if(_tSeqCon& con, decltype(begin(con)) first,
	decltype(end(con)) last, _fPred pred)
{
	con.erase(std::remove_if(first, last, pred), last);
}
//@}

//! \since build 730
//@{
template<typename _tSeqCon, typename _type>
inline void
erase_all_in_seq(_tSeqCon& con, _type&& value, true_)
{
	con.remove(yforward(value));
}
template<typename _tSeqCon, typename _type>
inline void
erase_all_in_seq(_tSeqCon& con, const _type& value, false_)
{
	details::erase_remove(con, begin(con), end(con), value);
}

template<typename _tSeqCon, typename _fPred>
inline void
erase_all_if_in_seq(_tSeqCon& con, _fPred pred, true_)
{
	con.remove_if(pred);
}
template<typename _tSeqCon, typename _fPred>
inline void
erase_all_if_in_seq(_tSeqCon& con, _fPred pred, false_)
{
	details::erase_remove_if(con, begin(con), end(con), pred);
}

template<typename _tCon, typename _type>
void
erase_all(_tCon& con, decltype(cbegin(con)) first, decltype(cend(con)) last,
	const _type& value, true_)
{
	while(first != last)
		if(*first == value)
			con.erase(first++);
		else
			++first;
}
template<typename _tCon, typename _type>
inline void
erase_all(_tCon& con, const _type& value, true_)
{
	details::erase_all(con, cbegin(con), cend(con), value, true_());
}
template<typename _tCon, typename _type>
inline void
erase_all(_tCon& con, decltype(begin(con)) first, decltype(end(con)) last,
	const _type& value, false_)
{
	details::erase_remove(con, first, last, value);
}
template<typename _tCon, typename _type>
inline void
erase_all(_tCon& con, const _type& value, false_)
{
	details::erase_all_in_seq(con, value, has_mem_remove<_tCon>());
}

template<typename _tCon, typename _fPred>
void
erase_all_if(_tCon& con, decltype(cbegin(con)) first, decltype(cend(con)) last,
	_fPred pred, true_)
{
	while(first != last)
		if(pred(*first))
			con.erase(first++);
		else
			++first;
}
template<typename _tCon, typename _fPred>
inline void
erase_all_if(_tCon& con, _fPred pred, true_)
{
	details::erase_all_if(con, cbegin(con), cend(con), pred, true_());
}
template<typename _tCon, typename _fPred>
inline void
erase_all_if(_tCon& con, decltype(begin(con)) first, decltype(end(con)) last,
	_fPred pred, false_)
{
	details::erase_remove_if(con, first, last, pred);
}
template<typename _tCon, typename _fPred>
inline void
erase_all_if(_tCon& con, _fPred pred, false_)
{
	details::erase_all_if_in_seq(con, pred, has_mem_remove_if<_tCon>());
}
//@}

} // namespace details;

//! \ingroup algorithms
//@{
/*!
\brief 判断指定的容器中存在指定的键。
\note 当容器对象右值可使用返回以整数初始化的类型的成员 \c count 时使用
	成员 \c count 实现；否则使用 ADL \c end 指定的容器迭代器，
	使用成员 \c find 实现。
\since build 488
*/
template<class _tCon, typename _tKey>
inline bool
exists(const _tCon& con, const _tKey& key)
{
	return details::exists(con, key);
}


/*!
\note 对不含 key_type 的容器视为序列容器。
\since build 633
*/
//@{
/*!
\brief 删除指定容器中指定区间中的或全部元素。
\note 对整个序列容器使用 ystdex::remove 移除元素范围。
\note 对序列容器中的部分序列使用 std::remove 移除元素范围。
*/
//@{
template<typename _tCon, typename _tIter>
inline void
erase_all(_tCon& con, _tIter first, _tIter last)
{
	con.erase(first, last);
}
template<typename _tCon, typename _tIter, typename _type>
inline void
erase_all(_tCon& con, _tIter first, _tIter last, const _type& value)
{
	details::erase_all(con, first, last, value,
		details::has_mem_key_type<_tCon>());
}
template<typename _tCon, typename _type>
inline void
erase_all(_tCon& con, const _type& value)
{
	details::erase_all(con, value, details::has_mem_key_type<_tCon>());
}
//@}

/*!
\brief 删除指定容器中指定区间中的或全部满足谓词的元素。
\note 对整个序列容器使用 ystdex::remove_if 移除元素范围。
\note 对序列容器中的部分序列使用 std::remove_if 移除元素范围。
*/
//@{
template<typename _tCon, typename _tIter, typename _fPred>
inline void
erase_all_if(_tCon& con, _tIter first, _tIter last, _fPred pred)
{
	details::erase_all_if(con, first, last, pred,
		details::has_mem_key_type<_tCon>());
}
template<typename _tCon, typename _fPred>
inline void
erase_all_if(_tCon& con, _fPred pred)
{
	details::erase_all_if(con, pred, details::has_mem_key_type<_tCon>());
}
//@}
//@}

//! \since build 680
//@{
/*!
\brief 删除指定关联容器中等价于指定键的起始元素。
\return 是否成功删除。
*/
template<class _tAssocCon, typename _tKey>
bool
erase_first(_tAssocCon& con, const _tKey& k)
{
	const auto i(con.find(k));

	if(i != end(con))
	{
		con.erase(i);
		return true;
	}
	return {};
}

/*!
\brief 删除指定关联容器中等价于指定键的所有元素。
\return 删除的元素数。
*/
//@{
template<class _tAssocCon>
inline typename _tAssocCon::size_type
erase_multi(_tAssocCon& con, const typename _tAssocCon::key_type& k)
{
	return con.erase(k);
}
template<class _tAssocCon, typename _tKey>
typename _tAssocCon::size_type
erase_multi(_tAssocCon& con, const _tKey& k)
{
	const auto pr(con.equal_range(k));

	if(pr.first != pr.second)
	{
		const auto n(std::distance(pr.first, pr.second));

		con.erase(pr.first, pr.second);
		return n;
	}
	return 0;
}
//@}
//@}

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
	yassume(n <= std::distance(i, cend(con)));
	return con.erase(i, std::next(i, n));
}
//! \since build 532
template<typename _tCon>
typename _tCon::iterator
erase_n(_tCon& con, typename _tCon::iterator i,
	typename _tCon::difference_type n)
{
	yassume(n <= std::distance(i, end(con)));
	return con.erase(i, std::next(i, n));
}
//@}


/*!
\brief 移除 const_iterator 的 const 限定。
\since build 680
*/
//@{
template<class _tCon>
inline typename _tCon::iterator
cast_mutable(_tCon& con, typename _tCon::const_iterator i)
{
	return con.erase(i, i);
}
template<class _tCon, typename _type>
inline std::pair<typename _tCon::iterator, _type>
cast_mutable(_tCon& con,
	const std::pair<typename _tCon::const_iterator, _type>& pr)
{
	return {ystdex::cast_mutable(con, pr.first), pr.second};
}
//@}


/*!
\brief 排序指定序列容器，保留不重复元素。
\pre 容器的迭代器满足随机迭代器要求。
\since build 414
*/
template<class _tCon>
inline void
sort_unique(_tCon& con)
{
	con.erase(ystdex::sort_unique(begin(con), end(con)), end(con));
}


/*!
\brief 保证非空容器以参数指定的元素结尾。
\since build 567
*/
//@{
template<class _tCon>
_tCon&&
trail(_tCon&& con, const typename decay_t<_tCon>::value_type& value = {})
{
	if(!con.empty() && !(con.back() == value))
		con.push_back(value);
	return static_cast<_tCon&&>(con);
}
template<class _tCon, typename... _tParams>
_tCon&&
trail(_tCon&& con, _tParams&&... args)
{
	if(!con.empty())
	{
		typename decay_t<_tCon>::value_type val(yforward(args)...);

		if(!(con.back() == val))
			con.emplace_back(std::move(val));
	}
	return static_cast<_tCon&&>(con);
}
//@}


/*!
\brief 若容器末尾存在指定值的元素则移除。
\since build 545
*/
template<class _tCon>
bool
pop_back_val(_tCon& con, const typename _tCon::value_type& value)
{
	if(!con.empty() && con.back() == value)
	{
		con.pop_back();
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
pop_front_val(_tCon& con, const typename _tCon::value_type& value)
{
	if(!con.empty() && con.front() == value)
	{
		con.pop_front();
		return true;
	}
	return {};
}


/*!
\brief 插入元素到 \c vector 末尾。
\note 使用 ADL \c size 。
\since build 546
*/
//@{
template<class _tVector, typename _tIn>
void
vector_concat(_tVector& vec, _tIn first, _tIn last)
{
	vec.reserve(size(vec) + std::distance(first, last));
	ystdex::concat(vec, first, last);
}
template<class _tVector, typename _tRange>
void
vector_concat(_tVector& vec, _tRange&& c)
{
	vec.reserve(size(vec) + ystdex::range_size(c));
	ystdex::concat(vec, yforward(c));
}
//@}


/*!
\brief 从第一参数指定的起始缓冲区大小循环取向量结果，按需扩大缓冲区。
\pre 断言：第一参数不等于 0 。
\note 可用于支持 \c max_size 和 \c resize 的字符串类型。
\see LWG 2466 。
\since build 699
*/
template<class _tVector, typename _func>
_tVector
retry_for_vector(typename _tVector::size_type s, _func f)
{
	yconstraint(s != 0);

	_tVector res;

	for(res.resize(s); f(res, s) && s < res.max_size() yimpl(/ 2); )
		res.resize(s *= yimpl(2));
	return res;
}



/*!
\ingroup helper_functions
\brief 替换关联容器的值。
\note 使用 \c end 指定范围迭代器。
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
\ingroup metafunctions
\brief 判断关联容器、键和参数类型是否可以按 map 的方式无转移地构造。
\since build 681
*/
template<class _tAssocCon, typename _tKey, typename... _tParams>
using is_piecewise_mapped = is_constructible<typename _tAssocCon::value_type,
	std::piecewise_construct_t, std::tuple<_tKey&&>, std::tuple<_tParams&&...>>;


//! \since build 677
namespace details
{

//! \since build 708
template<class _tAssocCon>
struct assoc_con_traits
{
	//! \since build 730
	//@{
	template<typename _tKey, typename... _tParams>
	static inline typename _tAssocCon::iterator
	emplace_hint_in_place(false_, _tAssocCon& con,
		typename _tAssocCon::const_iterator hint, _tKey&&, _tParams&&... args)
	{
		return con.emplace_hint(hint, yforward(args)...);
	}
	template<typename _tKey, typename... _tParams>
	static inline typename _tAssocCon::iterator
	emplace_hint_in_place(true_, _tAssocCon& con,
		typename _tAssocCon::const_iterator hint, _tKey&& k, _tParams&&... args)
	{
		return con.emplace_hint(hint, std::piecewise_construct,
			std::forward_as_tuple(yforward(k)),
			std::forward_as_tuple(yforward(args)...));
	}

	static inline const typename _tAssocCon::key_type&
	extract_key(false_, const typename _tAssocCon::value_type& val)
	{
		return val;
	}
	static inline const typename _tAssocCon::key_type&
	extract_key(true_, const typename _tAssocCon::value_type& val)
	{
		return val.first;
	}
	//@}

	//! \since build 792
	//@{
	static inline typename _tAssocCon::value_type&
	extract_mapped(false_, typename _tAssocCon::value_type& val)
	{
		return val;
	}
	static inline const typename _tAssocCon::value_type&
	extract_mapped(false_, const typename _tAssocCon::value_type& val)
	{
		return val;
	}
	template<typename _tMap = _tAssocCon>
	static inline typename _tMap::mapped_type&
	extract_mapped(true_, typename _tAssocCon::value_type& val)
	{
		return val.second;
	}
	template<typename _tMap = _tAssocCon>
	static inline const typename _tMap::mapped_type&
	extract_mapped(true_, const typename _tAssocCon::value_type& val)
	{
		return val.second;
	}
	//@}
};

template<class _type>
using mapped_type_t = typename _type::mapped_type;

} // unnamed namespace details;

/*!
\brief 带有提示的原地插入构造。
\since build 708
*/
template<class _tAssocCon, typename _tKey, typename... _tParams>
inline auto
emplace_hint_in_place(_tAssocCon& con, typename _tAssocCon::const_iterator hint,
	_tKey&& k, _tParams&&... args) -> typename _tAssocCon::iterator
{
	return details::assoc_con_traits<_tAssocCon>::emplace_hint_in_place(
		is_piecewise_mapped<_tAssocCon, _tKey, _tParams...>(), con, hint,
		yforward(k), yforward(args)...);
}

/*!
\brief 从关联容器的值取键。
\since build 679
*/
template<class _tAssocCon, typename _type,
	yimpl(typename = enable_if_convertible_t<const _type&,
	const typename _tAssocCon::value_type&>)>
inline const typename _tAssocCon::key_type&
extract_key(const _type& val)
{
	return details::assoc_con_traits<_tAssocCon>::extract_key(
		is_detected<details::mapped_type_t, _tAssocCon>(), val);
}

/*!
\brief 从关联容器的值取映射的值。
\since build 792
*/
template<class _tAssocCon, typename _type,
	yimpl(typename = enable_if_convertible_t<_type&,
	typename _tAssocCon::value_type&>)>
inline const auto
extract_mapped(_type& val)
	-> decltype(details::assoc_con_traits<_tAssocCon>::extract_mapped(
		is_detected<details::mapped_type_t, _tAssocCon>(), val))
{
	return details::assoc_con_traits<_tAssocCon>::extract_mapped(
		is_detected<details::mapped_type_t, _tAssocCon>(), val);
}


//! \since build 792
namespace details
{

/*!
\since build 681
\note 使用 ADL extract_mapped 。
*/
template<class _tAssocCon, typename _tKey, typename _tParam>
std::pair<typename _tAssocCon::iterator, bool>
insert_or_assign(std::pair<typename _tAssocCon::iterator, bool> pr,
	_tAssocCon& con, _tKey&& k, _tParam&& arg)
{
	if(pr.second)
		pr.first
			= emplace_hint_in_place(con, pr.first, yforward(k), yforward(arg));
	else
		extract_mapped<_tAssocCon>(*pr.first) = yforward(arg);
	return pr;
}

} // namespace details;


/*!
\return 一个用于表示结果的 std::pair 值，其成员 \c first 为迭代器，
	\c second 表示是否没有找到。
\note 使用 ADL cend 和 extract_key 。
*/
//@{
/*!
\brief 按指定键搜索指定关联容器。
\since build 677
*/
//@{
template<class _tAssocCon, typename _tKey>
std::pair<typename _tAssocCon::const_iterator, bool>
search_map(const _tAssocCon& con, const _tKey& k)
{
	const auto i(con.lower_bound(k));

	return {i, i == end(con) || con.key_comp()(k, extract_key<_tAssocCon>(*i))};
}
template<class _tAssocCon, typename _tKey>
inline std::pair<typename _tAssocCon::iterator, bool>
search_map(_tAssocCon& con, const _tKey& k)
{
	return
		ystdex::cast_mutable(con, ystdex::search_map(ystdex::as_const(con), k));
}
//@}
//! \since build 680
//@{
/*!
\brief 按指定键和提示的迭代器位置搜索指定关联容器。
\pre 容器非空或提示的迭代器位置指向尾部。
\note 使用 ADL cbegin 。
*/
//@{
template<class _tAssocCon, typename _tKey>
std::pair<typename _tAssocCon::const_iterator, bool>
search_map(const _tAssocCon& con, typename _tAssocCon::const_iterator hint,
	const _tKey& k)
{
	if(!con.empty())
	{
		const auto& comp(con.key_comp());
		const bool fit_before(hint == cbegin(con)
			|| bool(comp(extract_key(*std::prev(hint)), k))),
			fit_after(hint == cend(con)
			|| bool(comp(k, extract_key(*std::next(hint)))));

		if(fit_before == fit_after)
			return {hint, fit_before && fit_after};
		return ystdex::search_map(con, k);
	}
	yconstraint(hint == cend(con));
	return {hint, true};
}
template<class _tAssocCon, typename _tKey>
inline std::pair<typename _tAssocCon::iterator, bool>
search_map(_tAssocCon& con, typename _tAssocCon::const_iterator hint,
	const _tKey& k)
{
	return ystdex::cast_mutable(con,
		ystdex::search_map(ystdex::as_const(con), hint, k));
}
//@}

/*!
\brief 按指定键值搜索指定映射并执行操作。
\pre 最后的参数构造新的值。
\return 插入成员的迭代器。
\note 行为类似 std::map::operator[] 。
\since build 734
*/
//@{
template<typename _func, class _tAssocCon, typename... _tParams>
std::pair<typename _tAssocCon::const_iterator, bool>
search_map_by(_func f, const _tAssocCon& con, _tParams&&... args)
{
	auto pr(ystdex::search_map(con, yforward(args)...));

	if(pr.second)
		pr.first = f(pr.first);
	return pr;
}
template<typename _func, class _tAssocCon, typename... _tParams>
inline std::pair<typename _tAssocCon::iterator, bool>
search_map_by(_func f, _tAssocCon& con, _tParams&&... args)
{
	return ystdex::cast_mutable(con,
		ystdex::search_map_by(f, ystdex::as_const(con), yforward(args)...));
}
//@}
//@}
//@}

/*!
\note 使用 ADL emplace_hint_in_place 。
\sa emplace_hint_in_place
\see WG21 N4279 。
*/
//@{
//! \since build 680
//@{
template<class _tAssocCon, typename _tKey, typename... _tParams>
std::pair<typename _tAssocCon::iterator, bool>
try_emplace(_tAssocCon& con, _tKey&& k, _tParams&&... args)
{
	// XXX: Blocked. 'yforward' may cause G++ 5.2 silent crash.
	return ystdex::search_map_by([&](typename _tAssocCon::const_iterator i){
		return emplace_hint_in_place(con, i, yforward(k),
			std::forward<_tParams>(args)...);
	}, con, k);
}

template<class _tAssocCon, typename _tKey, typename... _tParams>
std::pair<typename _tAssocCon::iterator, bool>
try_emplace_hint(_tAssocCon& con, typename _tAssocCon::const_iterator hint,
	_tKey&& k, _tParams&&... args)
{
	// XXX: Blocked. 'yforward' may cause G++ 5.2 silent crash.
	return ystdex::search_map_by([&](typename _tAssocCon::const_iterator i){
		return emplace_hint_in_place(con, i, yforward(k),
			std::forward<_tParams>(args)...);
	}, con, hint, k);
}
//@}

/*!
\note 使用 ADL extract_mapped 。
\since build 681
*/
//@{
template<class _tAssocCon, typename _tKey, typename _tParam>
inline std::pair<typename _tAssocCon::iterator, bool>
insert_or_assign(_tAssocCon& con, _tKey&& k, _tParam&& arg)
{
	return details::insert_or_assign(ystdex::search_map(con, k), con,
		yforward(k), yforward(arg));
}

template<class _tAssocCon, typename _tKey, typename _tParam>
inline std::pair<typename _tAssocCon::iterator, bool>
insert_or_assign_hint(_tAssocCon& con, typename _tAssocCon::const_iterator hint,
	_tKey&& k, _tParam&& arg)
{
	return details::insert_or_assign(ystdex::search_map(con, hint, k), con,
		yforward(k), yforward(arg));
}
//@}
//@}
//@}

} // namespace ystdex;

#endif

