/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file variadic.hpp
\ingroup YStandardEx
\brief C++ 变长参数相关操作。
\version r773
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2013-06-06 11:38:15 +0800
\par 修改时间:
	2015-11-06 11:13 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Variadic
*/


#ifndef YB_INC_ystdex_variadic_hpp_
#define YB_INC_ystdex_variadic_hpp_ 1

#include "type_traits.hpp" // for _t, size_t, is_same, integral_constant,
//	conditional_t, cond_t;

namespace ystdex
{

/*!	\defgroup vseq_operations Variadic Sequence Operations
\ingroup meta_operations
\brief 变长参数标记的序列相关的元操作。
\since build 447

形式为模板类名声明和特化的相关操作，被操作的序列是类类型。
除此处的特化外，可有其它类类型的特化。
特化至少需保证具有表示和此处特化意义相同的 type 类型成员，并可能有其它成员。
对非类型元素，成员 value 表示结果，成员 type 表示对应的序列类型。
*/


//! \since build 589
namespace vseq
{

/*!
\ingroup vseq_operations
\since build 589
*/
//@{
#define YB_Impl_Variadic_SeqOp(_n, _tparams, _targs) \
	template<_tparams> \
	struct _n; \
	\
	template<_tparams> \
	using _n##_t = _t<_n<_targs>>;
#define YB_Impl_Variadic_SeqOpU(_n) \
	YB_Impl_Variadic_SeqOp(_n, class _tSeq, _tSeq)
#define YB_Impl_Variadic_SeqOpB(_n) YB_Impl_Variadic_SeqOp(_n, class _tSeq1 \
	YPP_Comma class _tSeq2, _tSeq1 YPP_Comma _tSeq2)
#define YB_Impl_Variadic_SeqOpI(_n) YB_Impl_Variadic_SeqOp(_n, class _tSeq \
	YPP_Comma size_t _vIdx, _tSeq YPP_Comma _vIdx)
#define YB_Impl_Variadic_SeqOpN(_n) YB_Impl_Variadic_SeqOp(_n, size_t _vN \
	YPP_Comma class _tSeq, _vN YPP_Comma _tSeq)


/*!
\brief 延迟求值。
\since build 650
*/
//@{
YB_Impl_Variadic_SeqOp(defer, template<typename...> class _gfunc YPP_Comma class
	_tSeq YPP_Comma typename _tEnabler = void,
	_gfunc YPP_Comma _tSeq YPP_Comma _tEnabler)


YB_Impl_Variadic_SeqOp(defer_i, typename _type YPP_Comma template<_type...>
	class _gfunc YPP_Comma class _tSeq YPP_Comma typename _tEnabler = void,
	_type YPP_Comma _gfunc YPP_Comma _tSeq YPP_Comma _tEnabler)
//@}


//! \brief 单位元：取空序列。
YB_Impl_Variadic_SeqOpU(clear)


//! \brief 连接：合并序列。
YB_Impl_Variadic_SeqOpB(concat)


//! \brief 取序列元素数。
YB_Impl_Variadic_SeqOpU(seq_size)


//! \brief 取序列最后一个元素。
YB_Impl_Variadic_SeqOpU(back)


//! \brief 取序列第一个元素。
YB_Impl_Variadic_SeqOpU(front)


//! \brief 取序列最后元素以外的元素的序列。
YB_Impl_Variadic_SeqOpU(pop_back)


//! \brief 取序列第一个元素以外的元素的序列。
YB_Impl_Variadic_SeqOpU(pop_front)


//! \brief 取在序列末尾插入一个元素的序列。
YB_Impl_Variadic_SeqOp(push_back, class _tSeq YPP_Comma typename _tItem,
	_tSeq YPP_Comma _tItem)


//! \brief 取在序列起始插入一个元素的序列。
YB_Impl_Variadic_SeqOp(push_front, class _tSeq YPP_Comma typename _tItem,
	_tSeq YPP_Comma _tItem)


//! \brief 投影操作。
YB_Impl_Variadic_SeqOp(project, class _tSeq YPP_Comma class _tIdxSeq,
	_tSeq YPP_Comma _tIdxSeq)


//! \brief 取指定位置的元素。
//@{
YB_Impl_Variadic_SeqOpI(at)

template<class _tSeq, size_t _vIdx>
struct at : at<pop_front_t<_tSeq>, _vIdx - 1>
{};

template<class _tSeq>
struct at<_tSeq, 0> : front<_tSeq>
{};
//@}


//! \brief 拆分序列前若干元素。
//@{
YB_Impl_Variadic_SeqOpN(split_n)

//! \note 使用二分实现减少递归实例化深度。
template<size_t _vN, class _tSeq>
struct split_n
{
private:
	using half = split_n<_vN / 2, _tSeq>;
	using last = split_n<_vN - _vN / 2, typename half::tail>;

public:
	using type = concat_t<_t<half>, _t<last>>;
	using tail = typename last::tail;
};

template<class _tSeq>
struct split_n<0, _tSeq>
{
	using type = clear_t<_tSeq>;
	using tail = _tSeq;
};

template<class _tSeq>
struct split_n<1, _tSeq>
{
	using type = push_back_t<clear_t<_tSeq>, front_t<_tSeq>>;
	using tail = pop_front_t<_tSeq>;
};
//@}


//! \brief 删除指定位置的元素。
//@{
YB_Impl_Variadic_SeqOp(erase, class _tSeq YPP_Comma size_t _vIdx YPP_Comma \
	size_t _vEnd = _vIdx + 1, _tSeq YPP_Comma _vIdx YPP_Comma _vEnd)

template<class _tSeq, size_t _vIdx, size_t _vEnd>
struct erase
{
	static_assert(_vIdx <= _vEnd, "Invalid range found.");

public:
	using type = concat_t<split_n_t<_vIdx, _tSeq>,
		typename split_n<_vEnd, _tSeq>::tail>;
};
//@}


//! \brief 查找元素。
//@{
YB_Impl_Variadic_SeqOp(find, class _tSeq YPP_Comma typename _type,
	_tSeq YPP_Comma _type)

namespace details
{

template<size_t _vN, class _tSeq, typename _type>
struct find
{
	static yconstexpr const size_t value = is_same<front_t<_tSeq>, _type>::value
		? 0 : find<_vN - 1, pop_front_t<_tSeq>, _type>::value + 1;
};

template<class _tSeq, typename _type>
struct find<0, _tSeq, _type>
{
	static yconstexpr const size_t value = 0;
};

} // namespace details;

template<class _tSeq, typename _type>
struct find : integral_constant<size_t,
	details::find<seq_size<_tSeq>::value, _tSeq, _type>::value>
{};
//@}


//! \brief 取合并相同元素后的序列。
//@{
YB_Impl_Variadic_SeqOpU(deduplicate)

namespace details
{

template<size_t, class _tSeq>
struct deduplicate
{
private:
	using head = pop_back_t<_tSeq>;
	using tail = back_t<_tSeq>;
	using sub = deduplicate_t<head>;

public:
	using type = conditional_t<vseq::find<head, tail>::value
		== seq_size<head>::value, concat_t<sub, tail>, sub>;
};

template<class _tSeq>
struct deduplicate<0, _tSeq>
{
	using type = _tSeq;
};

template<class _tSeq>
struct deduplicate<1, _tSeq>
{
	using type = _tSeq;
};

} // namespace details;

template<class _tSeq>
struct deduplicate
{
	using type = _t<details::deduplicate<seq_size<_tSeq>::value, _tSeq>>;
};
//@}


//! \brief 重复连接序列元素。
//@{
YB_Impl_Variadic_SeqOpN(join_n)

//! \note 使用二分实现减少递归实例化深度。
template<size_t _vN, class _tSeq>
struct join_n
{
private:
	using unit = _tSeq;
	using half = join_n_t<_vN / 2, unit>;

public:
	using type = concat_t<concat_t<half, half>, join_n_t<_vN % 2, unit>>;
};

template<class _tSeq>
struct join_n<0, _tSeq>
{
	using type = clear_t<_tSeq>;
};

template<class _tSeq>
struct join_n<1, _tSeq>
{
	using type = _tSeq;
};
//@}


//! \brief 取逆序列。
//@{
YB_Impl_Variadic_SeqOpU(reverse)

template<class _tSeq>
struct reverse
{
	using type = conditional_t<seq_size<_tSeq>::value == 0, clear_t<_tSeq>,
		concat_t<reverse_t<pop_front_t<_tSeq>>, front_t<_tSeq>>>;
};
//@}


//! \brief 取合并相邻相同元素后的序列。
//@{
YB_Impl_Variadic_SeqOpU(unique)

namespace details
{

/*!
\note 使用二分实现减少递归实例化深度。
\since build 649
*/
//@{
template<class _tSeq, size_t _vN = seq_size<_tSeq>::value>
struct unique
{
private:
	using split = split_n<_vN / 2, _tSeq>;
	using half = _t<unique<typename split::type>>;
	using last = _t<unique<typename split::tail>>;
	using half_back = back_t<half>;
	using last_front = front_t<last>;

public:
	using type = concat_t<cond_t<is_same<half_back, last_front>,
		pop_back_t<half>, half>, last>;
};

template<class _tSeq>
struct unique<_tSeq, 0>
{
	using type = clear_t<_tSeq>;
};

template<class _tSeq>
struct unique<_tSeq, 1>
{
	using type = _tSeq;
};
//@}

} // namespace details;

template<class _tSeq>
struct unique
{
	using type = _t<details::unique<_tSeq>>;
};


/*!
\brief 二元操作合并应用。
\pre 二元操作符合交换律和结合律。
*/
YB_Impl_Variadic_SeqOp(fold, class _fBinary YPP_Comma typename \
	_tState YPP_Comma class _type, _fBinary YPP_Comma _tState YPP_Comma _type)


//! \brief 序列作为向量的加法。
YB_Impl_Variadic_SeqOpB(vec_add)


//! \brief 序列作为向量的减法。
YB_Impl_Variadic_SeqOpB(vec_subtract)

#undef YB_Impl_Variadic_SeqOpN
#undef YB_Impl_Variadic_SeqOpI
#undef YB_Impl_Variadic_SeqOpU
#undef YB_Impl_Variadic_SeqOp
//@}

} // namespace vseq;

} // namespace ystdex;

#endif

